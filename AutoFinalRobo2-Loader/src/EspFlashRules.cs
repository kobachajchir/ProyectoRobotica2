// AFR2-20261027-22. Independent Web/linker and AT/vendor-package rules.
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text.RegularExpressions;
using System.Web.Script.Serialization;

namespace AFR2Loader
{
    public sealed class WebFlashLayout
    {
        public string Path;
        public long FlashBytes, FirmwareMax, FsStart, FsBytes;
        public static WebFlashLayout Read(string path, long flashBytes)
        {
            if (!new long[] { 1048576, 2097152, 4194304, 8388608, 16777216 }.Contains(flashBytes))
                throw new ArgumentException("Flash Web admitida: 1, 2, 4, 8 o 16 MiB; elegí un linker correspondiente.");
            string text = File.ReadAllText(path);
            Match start = Regex.Match(text, @"_FS_start\s*=\s*(0x[0-9a-fA-F]+)"), end = Regex.Match(text, @"_FS_end\s*=\s*(0x[0-9a-fA-F]+)");
            Match code = Regex.Match(text, @"irom0_0_seg\s*:\s*org\s*=\s*0x[0-9a-fA-F]+\s*,\s*len\s*=\s*(0x[0-9a-fA-F]+)");
            if (!start.Success || !end.Success || !code.Success) throw new ArgumentException("Linker sin rangos ESP8266 reconocidos.");
            long first = EspFlashRules.Number(start.Groups[1].Value) - 0x40200000L, last = EspFlashRules.Number(end.Groups[1].Value) - 0x40200000L;
            long maximum = EspFlashRules.Number(code.Groups[1].Value);
            if (first < 0 || last < first || last > flashBytes || first % 4096 != 0 || last % 4096 != 0 || maximum <= 0 || maximum > first)
                throw new ArgumentException("El linker no cabe en la flash esperada o sus regiones se superponen.");
            return new WebFlashLayout { Path = path, FlashBytes = flashBytes, FirmwareMax = maximum, FsStart = first, FsBytes = last - first };
        }
        public override string ToString() { return FsBytes == 0 ? "Sin filesystem" : EspFlashRules.Bytes(FsBytes) + " · " + System.IO.Path.GetFileName(Path); }
        public string Summary()
        {
            return "Flash esperada: " + EspFlashRules.Bytes(FlashBytes) + " | Código: máximo " + EspFlashRules.Bytes(FirmwareMax) +
                " | FileSystem: " + EspFlashRules.Bytes(FsBytes) + "\r\nFS desde 0x" + FsStart.ToString("X") + " | Reservado/no asignable a código: " + EspFlashRules.Bytes(FlashBytes - FirmwareMax - FsBytes) + ". El código usa el máximo del linker; en ESP8266 no todo el sobrante de FileSystem es ejecutable. El tamaño físico se consulta antes de cargar.";
        }
    }
    public sealed class AtFlashRegion
    {
        public string Name { get; set; }
        public string Role { get; set; }
        public string Offset { get; set; }
        public long Capacity { get; set; }
        public string File { get; set; }
        public string Sha256 { get; set; }
        public long Start { get { return EspFlashRules.Number(Offset); } }
    }
    public sealed class AtFlashLayout
    {
        public string Id { get; set; }
        public string Label { get; set; }
        public string Filesystem { get; set; }
        public AtFlashRegion[] Regions { get; set; }
        public long CodeBytes { get { return Regions.Where(r => r.Role == "firmware").Sum(r => r.Capacity); } }
        public long FsBytes { get { return Regions.Where(r => r.Role == "filesystem").Sum(r => r.Capacity); } }
        public override string ToString() { return Label + " · FS " + EspFlashRules.Bytes(FsBytes); }
    }
    public sealed class AtFirmwarePackage
    {
        public string Schema { get; set; }
        public string Name { get; set; }
        public string Chip { get; set; }
        public long FlashBytes { get; set; }
        public string FlashMode { get; set; }
        public string FlashFreq { get; set; }
        public AtFlashLayout[] Layouts { get; set; }
        public static AtFirmwarePackage Read(string path)
        {
            var package = String.Equals(System.IO.Path.GetExtension(path), ".bin", StringComparison.OrdinalIgnoreCase)
                ? ReadKnownFullImage(path)
                : new JavaScriptSerializer().Deserialize<AtFirmwarePackage>(System.IO.File.ReadAllText(path));
            if (package == null || package.Schema != "afr2-esp-at-package-1" || package.Chip != "esp8266" || String.IsNullOrWhiteSpace(package.Name) || package.Layouts == null || package.Layouts.Length == 0)
                throw new ArgumentException("Se necesita un paquete afr2-esp-at-package-1 para esp8266, con layouts explícitos.");
            if (!new long[] { 1048576, 2097152, 4194304, 8388608, 16777216 }.Contains(package.FlashBytes)) throw new ArgumentException("Capacidad AT admitida: 1, 2, 4, 8 o 16 MiB.");
            if (!new[] { "dio", "dout", "qio", "qout" }.Contains(package.FlashMode) || !new[] { "20m", "26m", "40m", "80m" }.Contains(package.FlashFreq))
                throw new ArgumentException("El paquete AT debe declarar modo/frecuencia SPI concretos del proveedor.");
            var ids = new HashSet<string>(StringComparer.Ordinal);
            foreach (AtFlashLayout layout in package.Layouts)
            {
                if (layout == null || String.IsNullOrWhiteSpace(layout.Id) || String.IsNullOrWhiteSpace(layout.Label) || !ids.Add(layout.Id) || layout.Regions == null || layout.Regions.Length == 0 || layout.Regions.Any(r => r == null)) throw new ArgumentException("Layouts AT vacíos, sin nombre, repetidos o con regiones nulas.");
                if (!new[] { "none", "littlefs", "spiffs", "vendor" }.Contains(layout.Filesystem)) throw new ArgumentException("Filesystem AT: none, littlefs, spiffs o vendor.");
                long end = 0; var names = new HashSet<string>(StringComparer.Ordinal);
                foreach (AtFlashRegion region in layout.Regions.OrderBy(r => r.Start))
                {
                    if (!new[] { "firmware", "filesystem", "system" }.Contains(region.Role) || String.IsNullOrWhiteSpace(region.Name) || !names.Add(region.Name)) throw new ArgumentException("Regiones AT sin rol/nombre válido o repetidas.");
                    if (region.Start < end || region.Start < 0 || region.Start % 4096 != 0 || region.Capacity <= 0 || region.Capacity % 4096 != 0 || region.Capacity > package.FlashBytes - region.Start)
                        throw new ArgumentException("Regiones AT solapadas, fuera de flash o sin alineación de sectores: " + region.Name);
                    if (String.IsNullOrWhiteSpace(region.File) || System.IO.Path.IsPathRooted(region.File) || region.File.IndexOfAny(new[] { ':', '\r', '\n', '\0', '"' }) >= 0 || region.File.Split('/', '\\').Any(p => p == "..")) throw new ArgumentException("Cada BIN AT debe tener una ruta relativa dentro del paquete.");
                    if (!String.IsNullOrEmpty(region.Sha256) && !Regex.IsMatch(region.Sha256, @"^[0-9a-fA-F]{64}$")) throw new ArgumentException("SHA-256 AT inválido.");
                    end = region.Start + region.Capacity;
                }
                if (!layout.Regions.Any(r => r.Role == "firmware")) throw new ArgumentException("Falta la región firmware del paquete AT.");
                if ((layout.FsBytes == 0) != (layout.Filesystem == "none")) throw new ArgumentException("El tipo y las regiones filesystem AT no coinciden.");
            }
            return package;
        }
        // AFR2-20261028-15. Exact vendor image downloaded in AFR2-20261028-14.
        // Geometry comes from its full-flash size/header; placement at zero is
        // the full-image interpretation, not a rule for arbitrary AT binaries.
        static AtFirmwarePackage ReadKnownFullImage(string path)
        {
            const string knownHash = "ac30e8b6aee2cfbd58fd1e39d72486641f56fefab345038cb94c847167992103";
            if (new FileInfo(path).Length != 4194304 || !String.Equals(Operations.Hash(path), knownHash, StringComparison.OrdinalIgnoreCase))
                throw new ArgumentException("BIN AT sin mapa reconocido. La selección directa admite la imagen completa Ai-Thinker v1.5.4.1 descargada (#0040). Para otro BIN, elegí un manifiesto JSON con el mapa de su proveedor; no se deducen offsets por el nombre.");
            return new AtFirmwarePackage {
                Schema = "afr2-esp-at-package-1", Name = "Ai-Thinker AT v1.5.4.1 (#0040), imagen completa 4 MiB",
                Chip = "esp8266", FlashBytes = 4194304, FlashMode = "dio", FlashFreq = "40m",
                Layouts = new[] { new AtFlashLayout {
                    Id = "ai-thinker-0040-full-4m", Label = "Imagen completa AT · 4 MiB · desde 0x0", Filesystem = "none",
                    Regions = new[] { new AtFlashRegion { Name = "Imagen completa Ai-Thinker", Role = "firmware", Offset = "0x0",
                        Capacity = 4194304, File = System.IO.Path.GetFileName(path), Sha256 = knownHash } }
                } }
            };
        }
        public AtFlashLayout Selected(Profile profile)
        {
            string id = profile.Get("AtLayoutId");
            var layout = String.IsNullOrEmpty(id) && Layouts.Length == 1 ? Layouts[0] : Layouts.FirstOrDefault(l => l.Id == id);
            if (layout == null) throw new ArgumentException("El layout AT elegido no existe en este paquete.");
            return layout;
        }
        public static string RegionPath(string manifest, AtFlashRegion region)
        {
            string root = System.IO.Path.GetFullPath(System.IO.Path.GetDirectoryName(manifest)).TrimEnd('\\', '/') + System.IO.Path.DirectorySeparatorChar;
            string path = System.IO.Path.GetFullPath(System.IO.Path.Combine(root, region.File));
            if (!path.StartsWith(root, StringComparison.OrdinalIgnoreCase)) throw new ArgumentException("El BIN sale de la carpeta del paquete AT.");
            return path;
        }
    }
    public static class EspFlashRules
    {
        public static bool IsAt(Profile profile) { return profile.Get("EspFirmwareMode") == "at"; }
        public static bool UseFilesystem(Profile profile) { return profile.Get(IsAt(profile) ? "AtUseFilesystem" : "WebUseFilesystem") == "true"; }
        public static long Number(string text) { return text != null && text.StartsWith("0x", StringComparison.OrdinalIgnoreCase) ? Convert.ToInt64(text.Substring(2), 16) : Int64.Parse(text); }
        public static string Bytes(long bytes) { return bytes.ToString("N0") + " B (" + (bytes / 1048576.0).ToString("0.###") + " MiB)"; }
    }
}
