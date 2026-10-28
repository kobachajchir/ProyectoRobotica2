// AFR2-20261027-22. Explicit build layouts and receipts, without editing product INI.
using System;
using System.Collections.Generic;
using System.IO;
using System.Security.Cryptography;
using System.Text;
using System.Text.RegularExpressions;
using System.Web.Script.Serialization;

namespace AFR2Loader
{
    public sealed partial class Operations
    {
        static string TextHash(string text)
        { using (var sha = SHA256.Create()) return BitConverter.ToString(sha.ComputeHash(Encoding.UTF8.GetBytes(text))).Replace("-", "").ToLowerInvariant(); }
        static string WebLayoutIdentity(Profile p)
        {
            return TextHash(p.Resolve("EspDir") + "|" + p.Get("PioEnv") + "|" + p.Get("FlashMB") + "|" + p.Get("WebUseFilesystem") + "|" + p.Get("FsOffset") + "|" + p.Get("FsBytes") + "|" + p.Get("FirmwareMax") + "|" + Hash(p.Resolve("Linker")) + "|" + Hash(Path.Combine(p.Resolve("EspDir"), "platformio.ini")));
        }
        IEnumerable<string> WebBuildArguments(Profile p, bool filesystem)
        {
            var layout = WebFlashLayout.Read(p.Resolve("Linker"), p.Number("FlashMB") * 1048576L);
            if (p.Number("FirmwareOffset") != 0 || p.Number("FsOffset") != layout.FsStart || p.Number("FsBytes") != layout.FsBytes || p.Number("FirmwareMax") <= 0 || p.Number("FirmwareMax") > layout.FirmwareMax || EspFlashRules.UseFilesystem(p) != (layout.FsBytes > 0))
                throw new ArgumentException("El perfil Web no coincide con el linker: offset de firmware 0, límite de código y rango filesystem deben corresponder.");
            var arguments = new List<string> { "run", "--project-dir", p.Resolve("EspDir"), "-e", p.Get("PioEnv") };
            if (p.Get("WebLayoutPreset") != "custom")
            {
                if (Path.GetFileName(p.Resolve("Linker")) != p.Get("WebLayoutPreset")) throw new ArgumentException("El nombre del preset Web no coincide con el linker seleccionado.");
                if (p.Number("FirmwareMax") != layout.FirmwareMax)
                    throw new ArgumentException("El perfil Web fue editado y ya no coincide con el layout elegido.");
                string ini = File.ReadAllText(Path.Combine(p.Resolve("EspDir"), "platformio.ini"));
                string pattern = @"(?ms)^\[env:" + Regex.Escape(p.Get("PioEnv")) + @"\][^\r\n]*\r?\n(.*?)(?=^\[|\z)";
                Match environment = Regex.Match(ini, pattern);
                if (!environment.Success) throw new ArgumentException("El entorno Web debe estar declarado explícitamente en platformio.ini.");
                string body = Regex.Replace(environment.Groups[1].Value, @"(?m)^\s*(board_build\.ldscript|board_build\.filesystem|board_upload\.flash_size|board_upload\.maximum_size)\s*=.*\r?\n?", "");
                string section = "[env:" + p.Get("PioEnv") + "]\r\n" + body.TrimEnd() + "\r\nboard_build.ldscript = " + p.Resolve("Linker").Replace('\\', '/') + "\r\nboard_build.filesystem = littlefs\r\nboard_upload.flash_size = " + p.Get("FlashMB") + "MB\r\nboard_upload.maximum_size = " + layout.FirmwareMax + "\r\n";
                ini = ini.Substring(0, environment.Index) + section + ini.Substring(environment.Index + environment.Length);
                string directory = Path.Combine(Profile.UserRoot, "build-configs"); Directory.CreateDirectory(directory);
                string config = Path.Combine(directory, "web-" + WebLayoutIdentity(p) + ".ini"); File.WriteAllText(config, ini, new UTF8Encoding(false));
                arguments.AddRange(new[] { "--project-conf", config }); log("Layout Web explícito: " + p.Resolve("Linker") + " | configuración temporal " + config);
            }
            if (filesystem) arguments.AddRange(new[] { "-t", "buildfs" });
            return arguments;
        }
        static string WebReceiptPath(string image)
        { return Path.Combine(Profile.UserRoot, "build-receipts", TextHash(Path.GetFullPath(image).ToUpperInvariant()) + ".json"); }
        void SaveWebReceipt(Profile p, string image, string buildLayoutIdentity)
        {
            if (p.Get("WebLayoutPreset") == "custom") return;
            if (buildLayoutIdentity != WebLayoutIdentity(p)) throw new ArgumentException("El linker o el INI cambió durante el build. El BIN no se acredita para este layout; reconstruí antes de cargar.");
            string file = WebReceiptPath(image); Directory.CreateDirectory(Path.GetDirectoryName(file));
            File.WriteAllText(file, new JavaScriptSerializer().Serialize(new { layout = buildLayoutIdentity, sha256 = Hash(image), artifact = image }), new UTF8Encoding(false));
        }
        static void RequireWebReceipt(Profile p, string image)
        {
            if (p.Get("WebLayoutPreset") == "custom") return;
            string file = WebReceiptPath(image);
            if (!File.Exists(file)) throw new ArgumentException("Compilá el BIN elegido con este layout antes de cargarlo; falta su recibo de build. Para imágenes externas, usar un perfil personalizado con el linker correspondiente.");
            var receipt = new JavaScriptSerializer().Deserialize<Dictionary<string, string>>(File.ReadAllText(file));
            if (receipt == null || !receipt.ContainsKey("layout") || !receipt.ContainsKey("sha256") || receipt["layout"] != WebLayoutIdentity(p) || receipt["sha256"] != Hash(image))
                throw new ArgumentException("El BIN o su layout cambió desde el build. Reconstruí firmware/filesystem con el layout elegido antes de cargar.");
        }
    }
}
