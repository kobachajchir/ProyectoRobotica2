using System;
using System.Collections.Concurrent;
using System.IO;
using System.IO.Ports;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading;
using System.Threading.Tasks;
using System.Web.Script.Serialization;

namespace AFR2Loader
{
    public sealed class SerialViewState
    {
        public string Observed;
        public long Bytes, Skipped, SkippedBytes;
    }
    public sealed class SerialDisplayChunk
    {
        public byte[] Bytes = new byte[0];
        public string Text = "";
        public bool Header;
        public int Weight { get { return Bytes.Length + Text.Length; } }
    }
    // One worker owns the port and all capture streams. UI never closes a port during Read.
    public sealed class SerialCapture
    {
        readonly object gate = new object();
        readonly CancellationTokenSource stop = new CancellationTokenSource();
        readonly TaskCompletionSource<bool> opened = new TaskCompletionSource<bool>();
        readonly ConcurrentQueue<SerialDisplayChunk> display = new ConcurrentQueue<SerialDisplayChunk>();
        readonly Action<LoaderNotice> notify;
        readonly Encoding encoding;
        readonly JobState journal;
        Task worker;
        string state = "opening", errorText, observed = "sin observar", ended;
        string closeReason = "Cierre solicitado";
        long bytes, skippedCharacters, skippedBytes;
        int queuedDisplayUnits;
        bool released;
        public readonly string Port, Codec, Id, Prefix;
        public readonly int Baud;
        public readonly string Started = DateTimeOffset.Now.ToString("o");
        public SerialCapture(string port, int baud, string codec, Action<LoaderNotice> notify)
        {
            if (!Regex.IsMatch(port, @"^COM[1-9][0-9]{0,4}$", RegexOptions.IgnoreCase)) throw new ArgumentException("Elegí un puerto COM válido.");
            if (baud < 300 || baud > 2000000) throw new ArgumentException("Baudios de lectura: entero entre 300 y 2000000.");
            if (codec != "UTF-8" && codec != "ASCII" && codec != "Latin-1") throw new ArgumentException("Codificación no admitida.");
            Port = port; Baud = baud; Codec = codec; this.notify = notify;
            encoding = codec == "ASCII" ? Encoding.ASCII : codec == "Latin-1" ? Encoding.GetEncoding(28591) : new UTF8Encoding(false, false);
            journal = new JobState("serial_monitor", "serial", port, "opening"); Id = journal.Id;
            Prefix = Path.Combine(Profile.UserRoot, "logs", "serial-" + DateTime.Now.ToString("yyyyMMdd-HHmmss-fff") + "-" + Id.Substring(0, 6));
        }
        public bool Active { get { lock (gate) return !released; } }
        public string State { get { lock (gate) return state; } }
        public SerialViewState ViewState() { lock (gate) return new SerialViewState { Observed = observed, Bytes = bytes, Skipped = skippedCharacters, SkippedBytes = skippedBytes }; }
        public Task OpenAsync()
        {
            lock (gate) { if (worker != null) throw new InvalidOperationException("Captura ya iniciada."); worker = Task.Run((Action)ReadLoop); }
            return opened.Task;
        }
        public async Task CloseAsync(string reason)
        {
            Task running;
            lock (gate) { closeReason = reason; if (!released && state != "failed") state = "closing"; running = worker; }
            stop.Cancel();
            if (running != null) await running;
            if (Active) throw new IOException("No se pudo confirmar el cierre de " + Port + ". Revisá el error de la captura; no se permite iniciar una carga.");
        }
        public bool TakeDisplay(out SerialDisplayChunk chunk)
        {
            if (!display.TryDequeue(out chunk)) return false;
            Interlocked.Add(ref queuedDisplayUnits, -chunk.Weight); return true;
        }
        public void ClearDisplay()
        {
            int count = display.Count; SerialDisplayChunk chunk;
            while (count-- > 0 && TakeDisplay(out chunk)) { }
        }
        public object Snapshot(int cursor, int limit)
        {
            lock (gate) { journal.Status = state; journal.Stage = observed; journal.Result = errorText; journal.Ended = ended; journal.LogPath = Prefix + ".rx.txt"; }
            lock (gate) return new { id = Id, state = state, port = Port, baud = Baud, codec = Codec, framing = "8N1", dtr = false, rts = false,
                bytesReceived = bytes, started = Started, ended = ended, observed = observed, error = errorText,
                portReleased = released, skippedDisplayCharacters = skippedCharacters, skippedDisplayBytes = skippedBytes,
                textLog = Prefix + ".rx.txt", binaryLog = Prefix + ".rx.bin", eventsLog = Prefix + ".events.txt", report = Prefix + ".json",
                received = journal.Snapshot(cursor, limit) };
        }
        void Event(string code, string message, string detail, StreamWriter events)
        {
            var notice = new LoaderNotice { Origin = "serial " + Port, Code = code, Message = message, Detail = detail };
            if (events != null) events.WriteLine(notice.At + " [" + code + "] " + message + " " + detail);
            notify(notice);
        }
        void ReceiveText(string text, byte[] raw, StreamWriter transcript, StreamWriter events, BootObserver observer)
        {
            if (text.Length == 0 && raw.Length == 0) return;
            if (text.Length > 0) { transcript.Write(text); journal.Add(text); }
            var chunk = new SerialDisplayChunk { Bytes = raw, Text = text };
            int count = Interlocked.Add(ref queuedDisplayUnits, chunk.Weight);
            if (count <= 262144) display.Enqueue(chunk);
            else
            {
                Interlocked.Add(ref queuedDisplayUnits, -chunk.Weight);
                lock (gate) { skippedCharacters += text.Length; skippedBytes += raw.Length; }
            }
            foreach (BootObservation observation in observer.Push(text))
            {
                lock (gate) observed = observation.Message + " (" + DateTime.Now.ToString("HH:mm:ss") + ")";
                Event(observation.Code, observation.Message, observation.Evidence, events);
            }
        }
        void Failure(Exception error, string context, StreamWriter events = null)
        {
            lock (gate) { state = "failed"; errorText = (errorText == null ? "" : errorText + "\r\n") + context + ": " + error; }
            if (events != null)
            {
                try { events.WriteLine(DateTimeOffset.Now.ToString("o") + " [failed] " + context + ": " + error); }
                catch (Exception logError) { lock (gate) errorText += "\r\nTampoco se pudo escribir el evento de fallo: " + logError.Message; }
            }
            notify(new LoaderNotice { Origin = "serial " + Port, Code = "failed", Message = context + ": " + error.Message, Detail = error.ToString() });
        }
        void ReadLoop()
        {
            SerialPort serial = null; FileStream binary = null; StreamWriter transcript = null, events = null;
            try
            {
                Directory.CreateDirectory(Path.GetDirectoryName(Prefix));
                binary = new FileStream(Prefix + ".rx.bin", FileMode.CreateNew, FileAccess.Write, FileShare.Read);
                transcript = new StreamWriter(Prefix + ".rx.txt", false, new UTF8Encoding(false)) { AutoFlush = true };
                events = new StreamWriter(Prefix + ".events.txt", false, new UTF8Encoding(false)) { AutoFlush = true };
                Event("opening", "Abriendo " + Port + " a " + Baud + " baudios, " + Codec + ", 8N1.", "DTR/RTS inactivos; un driver puede afectar reset al abrir. Solo lectura.", events);
                serial = new SerialPort(Port, Baud, Parity.None, 8, StopBits.One) { Handshake = Handshake.None, DtrEnable = false, RtsEnable = false, ReadTimeout = 200 };
                stop.Token.ThrowIfCancellationRequested(); serial.Open();
                lock (gate) state = "open";
                Event("open", "Puerto abierto; esperando texto. No se envían comandos.", Prefix + ".rx.txt", events);
                opened.TrySetResult(true);
                Decoder decoder = encoding.GetDecoder(); var buffer = new byte[4096]; var characters = new char[encoding.GetMaxCharCount(buffer.Length)];
                var observer = new BootObserver();
                while (!stop.IsCancellationRequested)
                {
                    int count;
                    try { count = serial.Read(buffer, 0, buffer.Length); }
                    catch (TimeoutException) { continue; }
                    if (count == 0) continue;
                    lock (gate) bytes += count;
                    binary.Write(buffer, 0, count); binary.Flush();
                    int length = decoder.GetChars(buffer, 0, count, characters, 0, false);
                    var raw = new byte[count]; Buffer.BlockCopy(buffer, 0, raw, 0, count);
                    ReceiveText(new String(characters, 0, length), raw, transcript, events, observer);
                }
                int remaining = decoder.GetChars(new byte[0], 0, 0, characters, 0, true);
                ReceiveText(new String(characters, 0, remaining), new byte[0], transcript, events, observer);
            }
            catch (OperationCanceledException) { opened.TrySetCanceled(); }
            catch (Exception error) { Failure(error, "Falló la lectura/apertura o el registro serie", events); opened.TrySetException(error); }
            finally
            {
                bool portClosed = serial == null;
                if (serial != null)
                {
                    try { serial.Dispose(); portClosed = true; }
                    catch (Exception error) { Failure(error, "Falló el cierre del puerto", events); }
                }
                if (events != null)
                    try { events.WriteLine(DateTimeOffset.Now.ToString("o") + (portClosed ? " [closed] " : " [release_unconfirmed] ") + closeReason); }
                    catch (Exception error) { Failure(error, "Falló el registro del cierre"); }
                foreach (IDisposable stream in new IDisposable[] { binary, transcript, events })
                    if (stream != null) { try { stream.Dispose(); } catch (Exception error) { Failure(error, "Falló el cierre del registro serie"); } }
                lock (gate) { released = portClosed; if (state != "failed") state = "closed"; ended = DateTimeOffset.Now.ToString("o"); }
                try { File.WriteAllText(Prefix + ".json", new JavaScriptSerializer().Serialize(Snapshot(0, 1)), new UTF8Encoding(false)); }
                catch (Exception error) { Failure(error, "No se pudo guardar el resumen de captura"); }
                notify(new LoaderNotice { Origin = "serial " + Port, Code = portClosed ? "closed" : "failed",
                    Message = portClosed ? "Puerto cerrado y liberado. " + closeReason : "Cierre del puerto sin confirmar.", Detail = Prefix });
                opened.TrySetCanceled();
            }
        }
    }
}
