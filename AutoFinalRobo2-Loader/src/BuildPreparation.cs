// AFR2-20261028-23. Koba Chajchir / AFR2; adaptación mediante Codex.
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

namespace AFR2Loader
{
    // Session receipts: a successful build/review prepares only its own images.
    public static class BuildPreparation
    {
        sealed class Receipt { public string Inputs, Stamp; public Dictionary<string, string> Images, ImageStamps; }
        static readonly Dictionary<string, Receipt> receipts = new Dictionary<string, Receipt>();
        static string Full(string path) { return Path.GetFullPath(path); }
        static IEnumerable<string> SourceFiles(string root, bool includeDist = false, bool dataInputs = false)
        {
            if (!Directory.Exists(root)) yield break;
            foreach (var file in Directory.GetFiles(root).OrderBy(p => p, StringComparer.OrdinalIgnoreCase)) {
                if (!dataInputs && new[] { ".a", ".o", ".elf", ".map", ".hex", ".d" }.Contains(Path.GetExtension(file), StringComparer.OrdinalIgnoreCase)) continue;
                yield return file;
            }
            foreach (var dir in Directory.GetDirectories(root).OrderBy(p => p, StringComparer.OrdinalIgnoreCase)) {
                string name = Path.GetFileName(dir);
                if (new[] { ".git", ".pio", "node_modules", "build", "__pycache__", ".cache" }.Contains(name, StringComparer.OrdinalIgnoreCase) || (!includeDist && name == "dist") || (File.GetAttributes(dir) & FileAttributes.ReparsePoint) != 0) continue;
                foreach (var file in SourceFiles(dir, includeDist, dataInputs)) yield return file;
            }
        }
        static string Fingerprint(string path, bool hash) { var file = new FileInfo(path); return hash ? Operations.Hash(path) : file.Length + ":" + file.LastWriteTimeUtc.Ticks; }
        static string Inputs(Profile p, string section, bool generatedData = true, bool hash = true)
        {
            var text = new StringBuilder();
            foreach (var pair in p.Values.OrderBy(v => v.Key, StringComparer.Ordinal))
                if (section != "mega" || pair.Key != "Hex") text.Append(pair.Key).Append('=').Append(pair.Value).Append('\n');
            string root = p.Resolve(section == "mega" ? "AtmegaDir" : section == "filesystem" ? "FsSourceDir" : "EspDir");
            if (section == "filesystem") {
                text.Append(Fingerprint(p.Resolve("FsScript"), hash));
                text.Append(Fingerprint(p.Resolve("FsScriptHost"), hash));
            }
            if (section != "at") foreach (string file in SourceFiles(root, false, section == "filesystem")) {
                // Bundled data is not an input to firmware compilation.
                if (section == "esp" && file.StartsWith(Path.Combine(root, "data") + Path.DirectorySeparatorChar, StringComparison.OrdinalIgnoreCase)) continue;
                text.Append(file).Append(':').Append(Fingerprint(file, hash)).Append('\n');
            }
            if (section == "esp" || section == "filesystem") {
                text.Append(Fingerprint(p.Resolve("Linker"), hash));
                text.Append(Fingerprint(Path.Combine(p.Resolve("EspDir"), "platformio.ini"), hash));
            }
            if (section == "at") {
                text.Append(Fingerprint(p.Resolve("AtPackage"), hash));
                foreach (var region in AtFirmwarePackage.Read(p.Resolve("AtPackage")).Selected(p).Regions) text.Append(Fingerprint(AtFirmwarePackage.RegionPath(p.Resolve("AtPackage"), region), hash));
            }
            if (section == "filesystem" && generatedData) foreach (string file in Directory.GetFiles(Path.Combine(p.Resolve("EspDir"), "data"), "*", SearchOption.AllDirectories).OrderBy(f => f, StringComparer.OrdinalIgnoreCase)) text.Append(file).Append(':').Append(Fingerprint(file, hash)).Append('\n');
            return text.ToString();
        }
        public static string Begin(Profile p, string section) { Invalidate(section); return Inputs(p, section, false); }
        public static void Record(Profile p, string section, IEnumerable<string> images, string before)
        {
            if (before != Inputs(p, section, false)) throw new InvalidOperationException("Las fuentes o el perfil cambiaron durante la preparación. Repetí la compilación antes de flashear.");
            var paths = images.Select(Full).Distinct(StringComparer.OrdinalIgnoreCase).ToArray();
            var receipt = new Receipt { Inputs = Inputs(p, section), Stamp = Inputs(p, section, true, false), Images = paths.ToDictionary(f => f, Operations.Hash, StringComparer.OrdinalIgnoreCase), ImageStamps = paths.ToDictionary(f => f, f => Fingerprint(f, false), StringComparer.OrdinalIgnoreCase) };
            lock (receipts) receipts[section] = receipt;
        }
        public static void Invalidate(string section) { lock (receipts) receipts.Remove(section); }
        public static bool Ready(Profile p, string section, params string[] images)
        {
            Receipt receipt; lock (receipts) if (!receipts.TryGetValue(section, out receipt)) return false;
            try {
                if (receipt.Stamp != Inputs(p, section, true, false)) return false;
                return images.Length > 0 && images.All(path => { string stamp; return receipt.ImageStamps.TryGetValue(Full(path), out stamp) && stamp == Fingerprint(path, false); });
            } catch (Exception) { return false; }
        }
        public static void Require(Profile p, string section, params string[] images)
        {
            Receipt receipt; lock (receipts) receipts.TryGetValue(section, out receipt);
            if (!Ready(p, section, images) || receipt == null || receipt.Inputs != Inputs(p, section) || !images.All(path => receipt.Images[Full(path)] == Operations.Hash(path)))
                throw new InvalidOperationException("Primero compilá esta sección con el perfil y las fuentes actuales. Para AT, revisá el paquete precompilado. La preparación se exige en esta sesión y se invalida si cambian fuentes, perfil o imágenes.");
        }
    }
}
