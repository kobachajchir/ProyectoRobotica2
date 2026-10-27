using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace AFR2Loader
{
    public sealed class CommandResult
    {
        public int ExitCode;
        public string Output;
    }
    public sealed class Runner
    {
        readonly Action<string> log;
        readonly object gate = new object();
        Process active;
        public Action<string> OutputLine;
        public Runner(Action<string> writer) { log = writer; }
        public static string Quote(string value)
        {
            if (value.IndexOf('\0') >= 0 || value.IndexOf('\r') >= 0 || value.IndexOf('\n') >= 0) throw new Exception("Argumento con caracteres de control.");
            var b = new StringBuilder("\""); int slashes = 0;
            foreach (char c in value)
            {
                if (c == '\\') { slashes++; continue; }
                if (c == '"') b.Append('\\', slashes * 2 + 1); else b.Append('\\', slashes);
                slashes = 0; b.Append(c);
            }
            b.Append('\\', slashes * 2).Append('"'); return b.ToString();
        }
        public void StopTree()
        {
            lock (gate)
            {
                if (active == null) return;
                try
                {
                    if (!active.HasExited)
                    {
                        var info = new ProcessStartInfo("taskkill.exe", "/PID " + active.Id + " /T /F");
                        info.UseShellExecute = false; info.CreateNoWindow = true;
                        using (Process killer = Process.Start(info)) killer.WaitForExit(5000);
                    }
                } catch (InvalidOperationException) { } catch (System.ComponentModel.Win32Exception) { }
            }
        }
        public async Task<CommandResult> Run(string tool, IEnumerable<string> arguments, string workingDir,
                                            Profile profile, CancellationToken token, int timeout = 0)
        {
            if (!File.Exists(tool)) throw new Exception("No existe la herramienta: " + tool + ". Revisá Configuración.");
            if (!Directory.Exists(workingDir)) throw new Exception("No existe la carpeta de trabajo: " + workingDir);
            int limit = timeout > 0 ? timeout : (int)profile.Number("TimeoutSeconds");
            if (limit < 10 || limit > 86400) throw new Exception("El tiempo máximo debe estar entre 10 y 86400 segundos.");
            string[] args = arguments.ToArray();
            string argumentLine = String.Join(" ", args.Select(Quote));
            var info = new ProcessStartInfo(tool, argumentLine);
            if (tool.EndsWith(".cmd", StringComparison.OrdinalIgnoreCase) || tool.EndsWith(".bat", StringComparison.OrdinalIgnoreCase))
            {
                // cmd.exe expands percent expressions even inside quotes; reject them in command paths/arguments.
                if (tool.IndexOfAny(new[] { '%', '!', '"', '\r', '\n' }) >= 0 || args.Any(a => a.IndexOfAny(new[] { '%', '!', '"' }) >= 0))
                    throw new Exception("La ruta/argumentos de CMD no admite %, ! ni comillas. Elegí otra ubicación.");
                info.FileName = Environment.GetEnvironmentVariable("COMSPEC") ?? "cmd.exe";
                info.Arguments = "/d /v:off /s /c \"" + Quote(tool) + " " + argumentLine + "\"";
            }
            info.WorkingDirectory = workingDir; info.UseShellExecute = false;
            info.CreateNoWindow = true; info.RedirectStandardOutput = true; info.RedirectStandardError = true;
            var pathParts = new List<string>();
            foreach (string key in new[] { "Node", "Gcc", "Make", "Python", "Pio" })
                if (File.Exists(profile.Resolve(key))) pathParts.Add(Path.GetDirectoryName(profile.Resolve(key)));
            pathParts.Add(Environment.GetEnvironmentVariable("PATH") ?? "");
            info.EnvironmentVariables["PATH"] = String.Join(";", pathParts);
            log("> " + Quote(tool) + " " + argumentLine);
            log("Carpeta: " + workingDir);
            return await Task.Run(() =>
            {
                token.ThrowIfCancellationRequested();
                var output = new StringBuilder(); var outputGate = new object();
                using (var process = new Process())
                {
                    process.StartInfo = info;
                    DataReceivedEventHandler receive = (s, e) =>
                    {
                        if (e.Data == null) return;
                        lock (outputGate)
                        {
                            output.AppendLine(e.Data);
                            if (output.Length > 4194304) output.Remove(0, output.Length - 3145728);
                        }
                        log(e.Data);
                        Action<string> observer = OutputLine; if (observer != null) observer(e.Data);
                    };
                    process.OutputDataReceived += receive; process.ErrorDataReceived += receive;
                    try
                    {
                        lock (gate) { process.Start(); active = process; }
                        process.BeginOutputReadLine(); process.BeginErrorReadLine();
                        var clock = Stopwatch.StartNew();
                        while (!process.WaitForExit(100))
                        {
                            if (token.IsCancellationRequested) { StopTree(); token.ThrowIfCancellationRequested(); }
                            if (clock.Elapsed.TotalSeconds > limit) { StopTree(); throw new TimeoutException("El comando superó " + limit + " segundos."); }
                        }
                        process.WaitForExit(); // Drains asynchronous output handlers.
                        token.ThrowIfCancellationRequested();
                        log("Código de salida: " + process.ExitCode);
                        lock (outputGate) return new CommandResult { ExitCode = process.ExitCode, Output = output.ToString() };
                    }
                    catch { StopTree(); throw; }
                    finally { lock (gate) active = null; }
                }
            }, token);
        }
    }
}
