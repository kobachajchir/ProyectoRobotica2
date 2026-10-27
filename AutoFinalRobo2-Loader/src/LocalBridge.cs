using System;
using System.IO;
using System.IO.Pipes;
using System.Security.AccessControl;
using System.Security.Principal;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Web.Script.Serialization;

namespace AFR2Loader
{
    // Same-user named pipe, never a listening TCP endpoint. Requests do not execute shell text.
    public sealed class LocalBridge : IDisposable
    {
        public static string PipeName { get { return "AFR2Loader-" + WindowsIdentity.GetCurrent().User.Value; } }
        public static string MutexName { get { return "Local\\AFR2Loader-" + WindowsIdentity.GetCurrent().User.Value; } }
        readonly Func<string, object> dispatch;
        readonly Action<string> diagnostic;
        volatile bool stopped;
        readonly object gate = new object();
        NamedPipeServerStream active;
        public LocalBridge(Func<string, object> dispatch, Action<string> diagnostic)
        { this.dispatch = dispatch; this.diagnostic = diagnostic; }
        public void Start()
        {
            Task.Run(() =>
            {
                while (!stopped)
                {
                    try
                    {
                        var security = new PipeSecurity();
                        security.SetAccessRuleProtection(true, false);
                        security.AddAccessRule(new PipeAccessRule(WindowsIdentity.GetCurrent().User, PipeAccessRights.FullControl, AccessControlType.Allow));
                        using (var pipe = new NamedPipeServerStream(PipeName, PipeDirection.InOut, 1,
                            PipeTransmissionMode.Byte, PipeOptions.Asynchronous, 65536, 65536, security))
                        {
                            lock (gate) { if (stopped) return; active = pipe; }
                            pipe.WaitForConnection();
                            using (var reader = new StreamReader(pipe, new UTF8Encoding(false), false, 4096, true))
                            using (var writer = new StreamWriter(pipe, new UTF8Encoding(false), 4096, true) { AutoFlush = true })
                            {
                                Task<string> read = reader.ReadLineAsync();
                                if (!read.Wait(5000)) throw new TimeoutException("Solicitud MCP incompleta.");
                                string request = read.Result;
                                if (request == null || request.Length > 65536) throw new Exception("Solicitud MCP vacía o demasiado grande.");
                                object response;
                                try { response = new { ok = true, data = dispatch(request) }; }
                                catch (Exception error) { response = new { ok = false, error = error.GetBaseException().Message }; }
                                writer.WriteLine(new JavaScriptSerializer { MaxJsonLength = 1048576 }.Serialize(response));
                            }
                        }
                    }
                    catch (Exception error) { if (!stopped) { diagnostic("Puente MCP: " + error.GetBaseException().Message); Thread.Sleep(300); } }
                    finally { lock (gate) active = null; }
                }
            });
        }
        public static object Call(string request)
        {
            if (request.Length > 65536) throw new ArgumentException("La solicitud excede 64 KiB de texto. Proponé menos cambios de perfil por solicitud.");
            using (var pipe = new NamedPipeClientStream(".", PipeName, PipeDirection.InOut, PipeOptions.Asynchronous))
            {
                try { pipe.Connect(1500); }
                catch (TimeoutException) { throw new Exception("El Loader gráfico no está abierto o su puente no está disponible. Abrí Iniciar.cmd; MCP no ejecuta operaciones fuera de la ventana."); }
                using (var writer = new StreamWriter(pipe, new UTF8Encoding(false), 4096, true) { AutoFlush = true })
                using (var reader = new StreamReader(pipe, new UTF8Encoding(false), false, 4096, true))
                {
                    writer.WriteLine(request);
                    Task<string> response = reader.ReadLineAsync();
                    if (!response.Wait(8000)) throw new TimeoutException("La interfaz no respondió a MCP a tiempo. No se reenvía automáticamente una solicitud de escritura.");
                    if (response.Result == null || response.Result.Length > 1048576) throw new Exception("Respuesta del Loader inválida.");
                    return new JavaScriptSerializer { MaxJsonLength = 1048576 }.DeserializeObject(response.Result);
                }
            }
        }
        public void Dispose()
        {
            stopped = true;
            lock (gate) if (active != null) { try { active.Dispose(); } catch (IOException) { } }
        }
    }
}
