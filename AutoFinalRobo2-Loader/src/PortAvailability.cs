using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.IO.Ports;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.Win32.SafeHandles;

namespace AFR2Loader
{
    // Exclusive, momentary OS handle; no bytes, DTR/RTS or serial configuration.
    // Another program may still acquire the port after this handle is released.
    public static class PortAvailability
    {
        [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
        static extern SafeFileHandle CreateFile(string name, uint access, uint share, IntPtr security, uint creation, uint flags, IntPtr template);
        public static void Check(string port)
        {
            if (!Regex.IsMatch(port ?? "", @"^COM[1-9][0-9]{0,4}$", RegexOptions.IgnoreCase)) throw new ArgumentException("Elegí un puerto COM válido.");
            using (var handle = CreateFile(@"\\.\" + port, 0xC0000000, 0, IntPtr.Zero, 3, 0, IntPtr.Zero))
                if (handle.IsInvalid) { int code = Marshal.GetLastWin32Error(); throw new InvalidOperationException(port + " no está disponible: " + new Win32Exception(code).Message + ". Cerrá el programa que lo usa y comprobá la conexión."); }
        }
    }

    internal sealed class ComProbeResult
    {
        public string Port, Kind, Detail, Baud;
        public bool Responded, SessionSafe;
        // AFR2-20261117-09 (Loader 2.5.0): versiones informadas por el firmware de producto (línea VER del TAP).
        // null = el firmware no las informa (anterior a 1.36.0); "?" = el ATmega todavía no conoce la del ESP/Web.
        public string MegaVersion, EspVersion, WebVersion, ExtVersion;
        public bool VersionsReported { get { return MegaVersion != null; } }
    }

    internal sealed class StartupComScan
    {
        public readonly List<ComProbeResult> Matches = new List<ComProbeResult>();
        public readonly ConcurrentDictionary<string, Task> PortTasks = new ConcurrentDictionary<string, Task>(StringComparer.OrdinalIgnoreCase);
        public int ProbedCount;
    }

    internal static class StartupComDetector
    {
        static readonly byte[] FlasherQueryPacket = { 0xA5, (byte)'A', (byte)'F', (byte)'R', (byte)'2', (byte)'C', (byte)'T', (byte)'L', 0x01, 0xFE };
        static readonly byte[] FlasherResponseMagic = { 0xA5, (byte)'A', (byte)'F', (byte)'R', (byte)'2', (byte)'R', (byte)'S', (byte)'P' };
        static readonly byte[] ProductTapQueryPacket = Encoding.ASCII.GetBytes("\n#AFR2 TAP ?\n");

        public static async Task<StartupComScan> ProbeAllAsync(string[] portNames, int bridgeBaud, ConcurrentDictionary<string, Task> portTasks, CancellationToken token)
        {
            var scan = new StartupComScan();
            string[] valid = (portNames ?? new string[0])
                .Where(p => Regex.IsMatch(p ?? "", @"^COM[1-9][0-9]{0,4}$", RegexOptions.IgnoreCase))
                .Distinct(StringComparer.OrdinalIgnoreCase)
                .ToArray();
            scan.ProbedCount = valid.Length;
            if (valid.Length == 0) return scan;

            var tasks = new List<Task<ComProbeResult>>();
            foreach (string port in valid)
            {
                string targetPort = port;
                var task = Task.Run(() => ProbeSinglePort(targetPort, bridgeBaud, token), token);
                portTasks[targetPort] = task;
                tasks.Add(task);
            }

            Task allDone = Task.WhenAll(tasks);
            using (var timeout = new CancellationTokenSource(3600))
            using (var linked = CancellationTokenSource.CreateLinkedTokenSource(token, timeout.Token))
            {
                var firstMatch = WaitFirstMatchAsync(tasks, linked.Token);
                Task winner = await Task.WhenAny(allDone, firstMatch, Task.Delay(3600, linked.Token)).ConfigureAwait(false);
                if (winner == firstMatch && await firstMatch.ConfigureAwait(false) != null && !allDone.IsCompleted)
                {
                    // Allow sibling fast USB ports a brief grace window without waiting on hung virtual drivers.
                    await Task.WhenAny(allDone, Task.Delay(350, linked.Token)).ConfigureAwait(false);
                }
            }

            foreach (var task in tasks)
            {
                if (task.Status == TaskStatus.RanToCompletion && task.Result != null && task.Result.Responded)
                    scan.Matches.Add(task.Result);
            }
            return scan;
        }

        static async Task<ComProbeResult> WaitFirstMatchAsync(List<Task<ComProbeResult>> tasks, CancellationToken token)
        {
            var remaining = new HashSet<Task<ComProbeResult>>(tasks);
            while (remaining.Count > 0 && !token.IsCancellationRequested)
            {
                Task<ComProbeResult> finished = await Task.WhenAny(remaining).ConfigureAwait(false);
                remaining.Remove(finished);
                if (finished.Status == TaskStatus.RanToCompletion && finished.Result != null && finished.Result.Responded)
                    return finished.Result;
            }
            return null;
        }

        static ComProbeResult ProbeSinglePort(string portName, int bridgeBaud, CancellationToken token)
        {
            var bauds = new List<int> { bridgeBaud > 0 ? bridgeBaud : 115200 };
            if (!bauds.Contains(115200)) bauds.Add(115200);
            for (int i = 0; i < bauds.Count; i++)
            {
                if (token.IsCancellationRequested) break;
                int baud = bauds[i];
                int budgetMs = i == 0 ? 2700 : 1200;
                ComProbeResult hit = ProbePortAtBaud(portName, baud, budgetMs, token);
                if (hit != null) return hit;
            }
            return null;
        }

        static ComProbeResult ProbePortAtBaud(string portName, int baud, int budgetMs, CancellationToken token)
        {
            try
            {
                using (var serial = new SerialPort(portName, baud, Parity.None, 8, StopBits.One))
                {
                    serial.DtrEnable = false;
                    serial.RtsEnable = false;
                    serial.Handshake = Handshake.None;
                    serial.ReadTimeout = 80;
                    serial.WriteTimeout = 250;
                    serial.Encoding = Encoding.ASCII;
                    serial.Open();
                    serial.DiscardInBuffer();

                    var rxBytes = new List<byte>();
                    var rxText = new StringBuilder();
                    var watch = Stopwatch.StartNew();
                    int round = 0;
                    while (watch.ElapsedMilliseconds < budgetMs && !token.IsCancellationRequested)
                    {
                        bool tryProduct = baud == 115200 && (round % 2 == 1);
                        serial.Write(tryProduct ? ProductTapQueryPacket : FlasherQueryPacket, 0,
                            tryProduct ? ProductTapQueryPacket.Length : FlasherQueryPacket.Length);

                        long sliceEnd = watch.ElapsedMilliseconds + 130;
                        while (watch.ElapsedMilliseconds < sliceEnd && !token.IsCancellationRequested)
                        {
                            ReadAvailable(serial, rxBytes, rxText);
                            ComProbeResult hit = MatchResponse(portName, baud, rxBytes, rxText);
                            if (hit != null)
                            {
                                // AFR2-20261117-09: el firmware 1.36.0+ manda «VER …» justo después de la confirmación del TAP.
                                if (hit.Kind == "product") WaitVersions(serial, hit, rxBytes, rxText, token);
                                return hit;
                            }
                            Thread.Sleep(20);
                        }
                        round++;
                    }
                }
            }
            catch (Exception)
            {
                // Ignore busy, disconnected, virtual Bluetooth timeouts or unsupported ports during startup scan.
            }
            return null;
        }

        static readonly Regex VersionLine = new Regex(@"\bVER mega=(\S+) ext=(\d+) esp=(\S+) web=(\S+)", RegexOptions.CultureInvariant);

        static void WaitVersions(SerialPort serial, ComProbeResult hit, List<byte> rxBytes, StringBuilder rxText, CancellationToken token)
        {
            var watch = Stopwatch.StartNew();
            while (watch.ElapsedMilliseconds < 700 && !token.IsCancellationRequested)
            {
                ReadAvailable(serial, rxBytes, rxText);
                Match m = VersionLine.Match(rxText.ToString());
                if (m.Success)
                {
                    hit.MegaVersion = m.Groups[1].Value; hit.ExtVersion = m.Groups[2].Value;
                    hit.EspVersion = m.Groups[3].Value; hit.WebVersion = m.Groups[4].Value;
                    hit.Detail = "Firmware AFR2 · ATmega " + hit.MegaVersion + " · ESP " + hit.EspVersion + " · Web " + hit.WebVersion + " (115200)";
                    return;
                }
                Thread.Sleep(20);
            }
            hit.Detail = "Firmware AFR2 (115200) · sin versiones: firmware anterior a 1.36.0";
        }

        static void ReadAvailable(SerialPort serial, List<byte> rxBytes, StringBuilder rxText)
        {
            int available = serial.BytesToRead;
            if (available <= 0) return;
            byte[] chunk = new byte[Math.Min(available, 512)];
            int read = serial.Read(chunk, 0, chunk.Length);
            for (int i = 0; i < read; i++)
            {
                byte b = chunk[i];
                rxBytes.Add(b);
                if (b == '\r' || b == '\n' || (b >= 32 && b <= 126)) rxText.Append((char)b);
            }
            if (rxBytes.Count > 2048) rxBytes.RemoveRange(0, rxBytes.Count - 1024);
            if (rxText.Length > 2048) rxText.Remove(0, rxText.Length - 1024);
        }

        static ComProbeResult MatchResponse(string portName, int baud, List<byte> rxBytes, StringBuilder rxText)
        {
            for (int i = 0; i <= rxBytes.Count - 11; i++)
            {
                bool magic = true;
                for (int j = 0; j < FlasherResponseMagic.Length; j++)
                    if (rxBytes[i + j] != FlasherResponseMagic[j]) { magic = false; break; }
                if (magic && rxBytes[i + 8] == 0x01)
                {
                    byte flags = rxBytes[i + 10];
                    return new ComProbeResult
                    {
                        Port = portName,
                        Responded = true,
                        Kind = "flasher",
                        SessionSafe = (flags & 0x20) != 0,
                        Baud = baud.ToString(),
                        Detail = "Flasher AFR2 (" + baud + ")"
                    };
                }
            }
            if (baud == 115200)
            {
                string text = rxText.ToString();
                if (Regex.IsMatch(text, @"\bTAP mode=\d+\s+\S+\s+baud=\d+"))
                    return new ComProbeResult
                    {
                        Port = portName,
                        Responded = true,
                        Kind = "product",
                        Baud = "115200",
                        Detail = "Firmware AFR2 (115200)"
                    };
            }
            return null;
        }
    }
}
