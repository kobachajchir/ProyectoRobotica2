using System;
using System.Threading;
using System.IO;
using System.Windows.Forms;

namespace AFR2Loader
{
    static class Program
    {
        // AFR2-20261106-16: local build resources survive opening the cached EXE directly.
        internal static string ResolveApplicationDirectory(string requested)
        {
            if (!String.IsNullOrWhiteSpace(requested)) return Path.GetFullPath(requested);
            string executableDirectory = AppDomain.CurrentDomain.BaseDirectory;
            // A portable distribution owns its adjacent resources; ignore local build hints.
            if (File.Exists(Path.Combine(executableDirectory, "tools", "esp_bridge_esptool.py")))
                return executableDirectory;
            string executableName = Path.GetFileNameWithoutExtension(System.Reflection.Assembly.GetExecutingAssembly().Location);
            string hint = Path.Combine(executableDirectory, executableName + ".appdir");
            if (File.Exists(hint)) {
                string resourceDirectory = File.ReadAllText(hint).Trim();
                if (!Path.IsPathRooted(resourceDirectory))
                    throw new ArgumentException("La referencia local de recursos debe ser absoluta. Abrí Iniciar.ps1 desde la carpeta del Loader.");
                return Path.GetFullPath(resourceDirectory);
            }
            return executableDirectory;
        }
        [STAThread]
        static void Main(string[] args)
        {
            bool mcp = args.Length > 0 && args[0] == "--mcp";
            string appDir = ResolveApplicationDirectory(args.Length > (mcp ? 1 : 0) ? args[mcp ? 1 : 0] : null);
            Profile.SetApplicationDirectory(appDir);
            if (mcp)
            {
                try { McpServer.Run(appDir); }
                catch (Exception error) { Console.Error.WriteLine(error); Environment.ExitCode = 1; }
                return;
            }
            Application.EnableVisualStyles(); Application.SetCompatibleTextRenderingDefault(false);
            Application.ThreadException += (sender, e) => MessageBox.Show(e.Exception.Message, "AFR2 Loader", MessageBoxButtons.OK, MessageBoxIcon.Error);
            bool owned;
            using (var instance = new Mutex(true, LocalBridge.MutexName, out owned))
            {
                if (!owned) { MessageBox.Show("AFR2 Loader ya está abierto para este usuario. Usá esa ventana para compartir la sesión con MCP."); return; }
                try
                {
                    string profile = null, mode = null;
                    for (int i = 1; i < args.Length; i += 2)
                    {
                        if (i + 1 >= args.Length) throw new ArgumentException("Falta el valor de " + args[i]);
                        if (args[i] == "--profile") profile = args[i + 1];
                        else if (args[i] == "--esp-mode") mode = args[i + 1];
                        else throw new ArgumentException("Opción de inicio desconocida: " + args[i]);
                    }
                    using (var welcome = new StartupPresentation(appDir))
                    using (var main = new MainForm(appDir, profile, mode)) {
                        main.Shown += (sender, e) => welcome.Finish(main);
                        Application.Run(main);
                    }
                }
                catch (Exception error) { MessageBox.Show(error.ToString(), "AFR2 Loader - error de inicio", MessageBoxButtons.OK, MessageBoxIcon.Error); }
                finally { instance.ReleaseMutex(); }
            }
        }
    }
}
