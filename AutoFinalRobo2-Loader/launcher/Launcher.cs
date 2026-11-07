// AFR2-20261029-03. Koba Chajchir / AFR2; adaptación mediante Codex.
using System;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Windows.Forms;

[assembly: AssemblyTitle("AFR2 Loader")]
[assembly: AssemblyProduct("AFR2 Loader")]
[assembly: AssemblyCompany("Koba Chajchir / AFR2")]
[assembly: AssemblyDescription("Inicia AFR2 Loader desde su carpeta de distribución")]
[assembly: AssemblyVersion("2.0.0.0")]
[assembly: AssemblyFileVersion("2.0.0.0")]

static class Launcher
{
    static string Quote(string value)
    {
        var text = new StringBuilder("\""); int slashes = 0;
        foreach (char c in value) {
            if (c == '\\') { slashes++; continue; }
            text.Append('\\', c == '"' ? slashes * 2 + 1 : slashes); slashes = 0; text.Append(c);
        }
        return text.Append('\\', slashes * 2).Append('"').ToString();
    }
    [STAThread]
    static int Main(string[] args)
    {
        Application.EnableVisualStyles();
        try {
            string root = AppDomain.CurrentDomain.BaseDirectory;
            string script = Path.Combine(root, "Iniciar.ps1");
            if (!File.Exists(script)) throw new FileNotFoundException("Conservá AFR2 Loader.exe junto a Iniciar.ps1 y las demás carpetas de la aplicación.", script);
            string host = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.System), "WindowsPowerShell", "v1.0", "powershell.exe");
            var arguments = new[] { "-NoProfile", "-STA", "-ExecutionPolicy", "Bypass", "-File", script }.Concat(args);
            var start = new ProcessStartInfo(host, String.Join(" ", arguments.Select(Quote))) {
                WorkingDirectory = root, UseShellExecute = false, CreateNoWindow = true,
                RedirectStandardError = true
            };
            using (var process = Process.Start(start)) {
                string error = process.StandardError.ReadToEnd(); process.WaitForExit();
                if (process.ExitCode != 0) throw new Exception(String.IsNullOrWhiteSpace(error) ? "El inicio terminó con código " + process.ExitCode + "." : error);
            }
            return 0;
        } catch (Exception error) {
            MessageBox.Show(error.Message, "AFR2 Loader — No se pudo iniciar", MessageBoxButtons.OK, MessageBoxIcon.Error);
            return 1;
        }
    }
}

