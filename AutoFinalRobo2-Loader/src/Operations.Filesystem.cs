// AFR2-20261029-01. Koba Chajchir / AFR2; adaptación mediante Codex.
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;

namespace AFR2Loader
{
    public sealed partial class Operations
    {
        async Task PrepareFilesystemFiles(Profile p, CancellationToken token)
        {
            string script = Path.GetFullPath(p.Resolve("FsScript"));
            string source = Path.GetFullPath(p.Resolve("FsSourceDir"));
            string data = Path.GetFullPath(Path.Combine(p.Resolve("EspDir"), "data"));
            NeedFile(script); NeedFile(p.Resolve("FsScriptHost"));
            if (!Directory.Exists(source)) throw new ArgumentException("No existe la carpeta de fuentes del FileSystem.");
            // Las entradas no pueden contener la salida que el script va a modificar.
            if (RelatedDirectories(source, data)) throw new ArgumentException("Las fuentes del script y ESP/data deben estar en carpetas separadas, sin contenerse entre sí.");
            Directory.CreateDirectory(data);
            var args = Profile.ParseScriptArguments(p.Get("FsScriptArguments")).Select(a => a.Replace("{Script}", script)).ToArray();
            var environment = new Dictionary<string, string> {
                { "AFR2_FS_SOURCE_DIR", source }, { "AFR2_FS_DATA_DIR", data },
                { "AFR2_ESP_DIR", Path.GetFullPath(p.Resolve("EspDir")) },
                { "AFR2_FS_BYTES", p.Number("FsBytes").ToString(System.Globalization.CultureInfo.InvariantCulture) },
                { "AFR2_WEB_DIR", OptionalScriptPath(p, "WebDir") }, { "AFR2_NPM", OptionalScriptPath(p, "Npm") }
            };
            Stage("building_web_fs", "Ejecutando script de preparación FileSystem: " + script);
            Steps.Add(new { filesystemScript = script, sha256 = Hash(script), sourceDirectory = source, dataDirectory = data });
            await Command(p, "FsScriptHost", args, source, token, 0, environment);
            if (!Directory.EnumerateFiles(data, "*", SearchOption.AllDirectories).Any())
                throw new InvalidOperationException("El script terminó, pero ESP/data está vacío. No se prepara una imagen vacía.");
        }
        static string OptionalScriptPath(Profile p, string key)
        {
            if (p.Get(key).Contains("{Repo}") && String.IsNullOrWhiteSpace(p.Get("Repo"))) return "";
            return p.Resolve(key);
        }
        static bool RelatedDirectories(string a, string b)
        {
            a = a.TrimEnd(Path.DirectorySeparatorChar, Path.AltDirectorySeparatorChar) + Path.DirectorySeparatorChar;
            b = b.TrimEnd(Path.DirectorySeparatorChar, Path.AltDirectorySeparatorChar) + Path.DirectorySeparatorChar;
            return a.StartsWith(b, StringComparison.OrdinalIgnoreCase) || b.StartsWith(a, StringComparison.OrdinalIgnoreCase);
        }
    }
}
