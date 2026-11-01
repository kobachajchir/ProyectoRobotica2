// AFR2-20261027-22. USB ROM flashing of vendor AT packages, isolated from Web builds.
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
        public async Task BuildAll(Profile p, CancellationToken token)
        {
            await BuildMega(p, token); await BuildEsp(p, token);
            if (!EspFlashRules.IsAt(p) && EspFlashRules.UseFilesystem(p)) await BuildWebFs(p, token);
        }
        void ReviewAtPackage(Profile p)
        {
            string preparationInputs = BuildPreparation.Begin(p, "at");
            string manifest = p.Resolve("AtPackage"); NeedFile(manifest);
            var package = AtFirmwarePackage.Read(manifest); var layout = package.Selected(p);
            if (package.FlashBytes != p.Number("FlashMB") * 1048576L) throw new ArgumentException("Flash esperada del perfil distinta de la declarada en el paquete AT.");
            Stage("reviewing_package", "Revisando paquete AT y BIN del proveedor; no se compila Arduino/Web...");
            foreach (var region in layout.Regions)
            {
                string file = AtFirmwarePackage.RegionPath(manifest, region); CheckAtRegion(region, file);
                log("BIN precompilado del paquete AT: " + Describe(file));
                Steps.Add(new { suppliedArtifact = file, bytes = new FileInfo(file).Length, sha256 = Hash(file) });
            }
            BuildPreparation.Record(p, "at", new[] { manifest }.Concat(layout.Regions.Select(r => AtFirmwarePackage.RegionPath(manifest, r))), preparationInputs);
            Stage("package_ready", "Paquete AT revisado: regiones, capacidades, archivos y hashes. Sin conexión física ni escritura.");
        }
        static void CheckAtRegion(AtFlashRegion region, string file)
        {
            NeedFile(file); long bytes = new FileInfo(file).Length;
            if (bytes <= 0 || bytes > region.Capacity || (region.Role == "filesystem" && bytes != region.Capacity))
                throw new ArgumentException("BIN AT fuera de su región o imagen filesystem con tamaño distinto: " + region.Name);
            if (!String.IsNullOrEmpty(region.Sha256) && !String.Equals(Hash(file), region.Sha256, StringComparison.OrdinalIgnoreCase)) throw new ArgumentException("SHA-256 distinto al paquete AT: " + region.Name);
        }
        async Task FlashAtPackage(Profile p, string port, bool firmware, bool filesystem, bool listen, CancellationToken token)
        {
            BuildPreparation.Require(p, "at", p.Resolve("AtPackage"));
            string manifest = p.Resolve("AtPackage"); NeedFile(manifest); string manifestHash = Hash(manifest);
            var package = AtFirmwarePackage.Read(manifest); var layout = package.Selected(p);
            if (p.Number("FlashMB") * 1048576L != package.FlashBytes) throw new ArgumentException("La capacidad esperada del perfil no coincide con el paquete AT.");
            var regions = layout.Regions.Where(r => r.Role == "filesystem" ? filesystem : firmware).OrderBy(r => r.Start).ToArray();
            if (regions.Length == 0 || (filesystem && !regions.Any(r => r.Role == "filesystem"))) throw new ArgumentException("El layout AT no contiene las regiones solicitadas.");
            var files = regions.Select(r => AtFirmwarePackage.RegionPath(manifest, r)).ToArray();
            for (int i = 0; i < regions.Length; i++) CheckAtRegion(regions[i], files[i]);
            string[] hashes = files.Select(Hash).ToArray();
            Steps.Add(new { firmwareFamily = "at", package = manifest, packageSha256 = manifestHash, layout = layout.Id, regions = regions, imageSha256 = hashes });
            await PrepareEspSession(p, port, filesystem, listen, token);
            long detected = await Probe(p, port, token);
            if (detected != package.FlashBytes) throw new ArgumentException("Flash física distinta de la requerida por el paquete AT. No se escribe.");
            string details = "Firmware AT por USB / ROM. No se envían comandos AT para instalar el firmware.\r\n" + package.Name + " | layout " + layout.Id + "\r\nFlash " + detected + " B | SPI " + package.FlashMode + " / " + package.FlashFreq + "\r\n\r\n";
            if (String.Equals(Path.GetExtension(manifest), ".bin", StringComparison.OrdinalIgnoreCase))
                details += "Imagen completa reconocida por SHA-256: 4 MiB desde 0x0. Reemplaza toda la flash, incluidos firmware Web, LittleFS y datos previos. Su mapa no incluye un filesystem separable.\r\n\r\n";
            for (int i = 0; i < regions.Length; i++) details += regions[i].Name + " | " + regions[i].Role + " | offset " + regions[i].Offset + " | reserva " + regions[i].Capacity + " B\r\n" + Describe(files[i]) + "\r\n\r\n";
            if (!confirm("Cargar paquete AT y verificar", details, new[] {
                "El paquete y sus offsets corresponden al ESP8266 y a la revisión que quiero instalar.",
                firmware ? "Revisé todas las imágenes de firmware/sistema; cambiar a AT reemplaza el firmware Web de las regiones elegidas." : "El firmware AT ya instalado usa exactamente este layout y filesystem.",
                "Revisé las regiones de datos seleccionadas y conservé sus preferencias antes de reemplazarlas.",
                "Mantengo KEY hasta el aviso del primer porcentaje de escritura; solo RST queda suelto."
            })) throw new OperationCanceledException("Carga AT no confirmada.");
            EnsureUnchanged(manifest, manifestHash); token.ThrowIfCancellationRequested();
            for (int i = 0; i < files.Length; i++) EnsureUnchanged(files[i], hashes[i]);
            Profile transfer = p.Copy(); transfer.Values["FlashMode"] = package.FlashMode; transfer.Values["FlashFreq"] = package.FlashFreq;
            var write = EspArgs(transfer, port, false); write.Add(Spell(transfer, "write_flash")); AddFlashOptions(transfer, write);
            for (int i = 0; i < regions.Length; i++) write.AddRange(new[] { regions[i].Offset, files[i] });
            Stage("writing", "Escribiendo paquete AT según su mapa de regiones. Mantené KEY apretado..."); MarkWriting();
            var written = await Command(transfer, "Python", write, Path.GetDirectoryName(transfer.Resolve("Esptool")), token);
            EnsureUnchanged(manifest, manifestHash);
            for (int i = 0; i < files.Length; i++) EnsureUnchanged(files[i], hashes[i]);
            RequireIntegratedEspVerification(written, files.Length);
            Verified = true; Stage("verified", "Paquete AT: contenido de las regiones seleccionadas escrito y verificado por USB.");
            log(EspRestoration(p));
            if (UsesMegaBridge(p)) CompletionMessage = "Paquete AT escrito y verificado. El Mega sigue ejecutando el flasher temporal; restaurar su producto no lo convierte en compatible con AT.";
            log("La verificación de bytes no acredita respuestas AT ni compatibilidad del firmware del Mega con AT.");
        }
    }
}
