using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading;
using System.Threading.Tasks;
using System.Web.Script.Serialization;

namespace AFR2Loader
{
    public sealed class MegaFlashBackup
    {
        public int Schema = 1;
        public string Id, Created, Port, Mcu, Signature, FullFlash, Application, FullSha256, ApplicationSha256;
        public string Manifest;
        public int HighFuse, ApplicationBytes, RawReadBytes;
        public bool ReadTwice;
        public const int FlashBytes = 262144;
        public static string LatestPath { get { return Path.Combine(Profile.UserRoot, "backups", "last-mega-backup.json"); } }
        public static MegaFlashBackup Latest()
        {
            if (!File.Exists(LatestPath)) return null;
            var backup = new JavaScriptSerializer().Deserialize<MegaFlashBackup>(File.ReadAllText(LatestPath));
            if (backup == null || backup.Schema != 1 || !backup.ReadTwice || backup.Mcu != "atmega2560" || backup.Signature != "0x1e9801") throw new ArgumentException("El índice del último backup es incompatible o no acredita lectura completada.");
            return backup;
        }
        public void Publish()
        {
            string json = new JavaScriptSerializer().Serialize(this);
            File.WriteAllText(Manifest, json, new UTF8Encoding(false));
            string temp = LatestPath + ".tmp"; File.WriteAllText(temp, json, new UTF8Encoding(false));
            if (File.Exists(LatestPath)) File.Replace(temp, LatestPath, LatestPath + ".bak"); else File.Move(temp, LatestPath);
        }
    }
    public sealed partial class Operations
    {
        public bool FlashedMegaBackup;
        static void MegaOnly(Profile p)
        {
            if (p.Get("Mcu") != "atmega2560" || !p.Get("Signature").Equals("0x1e9801", StringComparison.OrdinalIgnoreCase)) throw new ArgumentException("Backup de programa disponible para ATmega2560/firma0x1e9801.");
        }
        static List<string> MegaReadArgs(Profile p, string port)
        {
            return new List<string> { "-C", p.Resolve("AvrdudeConfig"), "-v", "-n", "-D", "-p", p.Get("Mcu"), "-c", p.Get("Programmer"), "-P", port, "-b", p.Get("MegaBaud") };
        }
        static void RequireMegaSignature(Profile p, CommandResult result)
        {
            Match match = Regex.Match(result.Output, @"Device signature\s*=\s*(0x[0-9a-fA-F]{6})", RegexOptions.IgnoreCase);
            if (!match.Success || !match.Groups[1].Value.Equals(p.Get("Signature"), StringComparison.OrdinalIgnoreCase)) throw new CommandFailure("No se reconoció la firma del Mega en la lectura.", p.Resolve("Avrdude"), result.ExitCode, result.Output);
        }
        static byte[] ReadFlashImage(string file)
        {
            NeedFile(file); byte[] raw = File.ReadAllBytes(file);
            if (raw.Length == 0 || raw.Length > MegaFlashBackup.FlashBytes) throw new ArgumentException("Lectura flash vacía o tamaño inválido. No se publica backup.");
            // AVRDUDE6.3 trims trailing erased bytes on export. Preserve the export
            // and normalize only its implicit FF tail; a second full read must agree.
            byte[] full = Enumerable.Repeat((byte)0xff, MegaFlashBackup.FlashBytes).ToArray();
            Array.Copy(raw, full, raw.Length); return full;
        }
        static byte ReadHighFuse(string file)
        {
            NeedFile(file); byte[] raw = File.ReadAllBytes(file);
            if (raw.Length != 1 || (raw[0] & 1) != 0) throw new ArgumentException("No se pudo identificar un BOOTRST compatible desde hfuse. Se detiene antes de escribir.");
            return raw[0];
        }
        static int MegaApplicationBytes(byte highFuse)
        { return MegaFlashBackup.FlashBytes - new[] { 8192, 4096, 2048, 1024 }[(highFuse >> 1) & 3]; }
        public async Task<MegaFlashBackup> BackupMegaFlash(Profile p, string port, CancellationToken token)
        {
            MegaOnly(p); PortValid(port); NeedFile(p.Resolve("AvrdudeConfig"));
            string id = Guid.NewGuid().ToString("N");
            string dir = Path.Combine(Profile.UserRoot, "backups", "mega-" + DateTime.Now.ToString("yyyyMMdd-HHmmss") + "-" + id.Substring(0, 8));
            Directory.CreateDirectory(dir);
            Stage("backup_reading", "Backup ATmega: leyendo flash y hfuse sin escritura; se contrastarán dos lecturas...");
            var firstArgs = MegaReadArgs(p, port); firstArgs.AddRange(new[] { "-U", "flash:r:read-1.bin:r", "-U", "hfuse:r:hfuse-1.bin:r" });
            var firstResult = await Command(p, "Avrdude", firstArgs, dir, token); RequireMegaSignature(p, firstResult);
            token.ThrowIfCancellationRequested();
            var secondArgs = MegaReadArgs(p, port); secondArgs.AddRange(new[] { "-U", "flash:r:read-2.bin:r", "-U", "hfuse:r:hfuse-2.bin:r" });
            var secondResult = await Command(p, "Avrdude", secondArgs, dir, token); RequireMegaSignature(p, secondResult);
            byte[] first = ReadFlashImage(Path.Combine(dir, "read-1.bin")), second = ReadFlashImage(Path.Combine(dir, "read-2.bin"));
            byte highFuse = ReadHighFuse(Path.Combine(dir, "hfuse-1.bin"));
            if (highFuse != ReadHighFuse(Path.Combine(dir, "hfuse-2.bin")) || !first.SequenceEqual(second)) throw new ArgumentException("Las lecturas flash/hfuse no coinciden. No se crea backup válido ni se carga el flasher.");
            int applicationBytes = MegaApplicationBytes(highFuse);
            if (first.Take(applicationBytes).All(b => b == 0xff)) throw new ArgumentException("La aplicación leída está vacía o no fue accesible. No se publica un backup de programa vacío.");
            string full = Path.Combine(dir, "flash-full.bin"), app = Path.Combine(dir, "application.bin");
            File.WriteAllBytes(full, first); File.WriteAllBytes(app, first.Take(applicationBytes).ToArray());
            var backup = new MegaFlashBackup { Id = id, Created = DateTimeOffset.Now.ToString("o"), Port = port, Mcu = "atmega2560", Signature = "0x1e9801", HighFuse = highFuse,
                ApplicationBytes = applicationBytes, RawReadBytes = (int)new FileInfo(Path.Combine(dir, "read-1.bin")).Length,
                FullFlash = full, Application = app, FullSha256 = Hash(full), ApplicationSha256 = Hash(app), Manifest = Path.Combine(dir, "backup.json"), ReadTwice = true };
            token.ThrowIfCancellationRequested(); backup.Publish();
            Steps.Add(new { megaFlashBackup = backup });
            Stage("backup_completed", "Backup ATmega leído y contrastado. Aplicación recuperable: " + applicationBytes + " bytes. No incluye EEPROM; no se escribieron fuses.");
            log("Último backup: " + backup.Manifest + " | aplicación SHA-256 " + backup.ApplicationSha256);
            return backup;
        }
        public async Task FlashLatestMegaBackup(Profile p, string port, CancellationToken token)
        {
            MegaOnly(p); PortValid(port); NeedFile(p.Resolve("AvrdudeConfig"));
            MegaFlashBackup backup = MegaFlashBackup.Latest();
            if (backup == null) throw new ArgumentException("Todavía no hay un backup leído del ATmega. No se sustituye por un HEX de la PC. Usá Flashear Firmware ATmega para elegir un firmware.");
            NeedFile(backup.Application); NeedFile(backup.FullFlash); EnsureUnchanged(backup.Application, backup.ApplicationSha256); EnsureUnchanged(backup.FullFlash, backup.FullSha256);
            if (backup.HighFuse < 0 || backup.HighFuse > 255 || (backup.HighFuse & 1) != 0 || backup.ApplicationBytes != MegaApplicationBytes((byte)backup.HighFuse) || new FileInfo(backup.Application).Length != backup.ApplicationBytes || new FileInfo(backup.FullFlash).Length != MegaFlashBackup.FlashBytes)
                throw new ArgumentException("Metadatos o tamaño del backup inválidos; no se carga.");
            if (!confirm("Flashear último backup en ATmega", "Backup leído del equipo a las " + backup.Created + "\r\nCOM de origen: " + backup.Port + " | destino: " + port + "\r\nAplicación: " + Describe(backup.Application) +
                "\r\n\r\nSe escribirá solo la región de aplicación. Se conservan bootloader/fuses/EEPROM actuales. El backup puede contener un flasher u otro programa: es lo que se leyó en esa fecha, no se identifica como producto automáticamente.", new[] {
                "RESET Mega LIBRE; D0/D1 aislados del ESP y potencia de actuadores desconectada.",
                "Revisé fecha, origen y destino. Confirmo que este es el programa que quiero volver a cargar."
            })) throw new OperationCanceledException("Carga del backup no confirmada.");
            string dir = Path.Combine(Profile.UserRoot, "logs", "backup-flash-" + Guid.NewGuid().ToString("N")); Directory.CreateDirectory(dir);
            var fuseArgs = MegaReadArgs(p, port); fuseArgs.AddRange(new[] { "-U", "hfuse:r:target-hfuse.bin:r" });
            var fuseResult = await Command(p, "Avrdude", fuseArgs, dir, token); RequireMegaSignature(p, fuseResult);
            if (ReadHighFuse(Path.Combine(dir, "target-hfuse.bin")) != backup.HighFuse) throw new ArgumentException("hfuse del destino diferente del backup. No se modifica y no se continúa con la escritura.");
            MegaServiceRecord service = MegaServiceRecord.Read(port); if (service != null) { service.Status = "backup_write_started"; service.Save(); }
            EnsureUnchanged(backup.Application, backup.ApplicationSha256);
            await UploadMegaApplication(p, port, backup.Application, token, "r");
            Stage("verifying", "Contrastando la aplicación restaurada completa, incluida su cola FF, mediante lectura posterior...");
            var readArgs = MegaReadArgs(p, port); readArgs.AddRange(new[] { "-U", "flash:r:after.bin:r" });
            var readResult = await Command(p, "Avrdude", readArgs, dir, token); RequireMegaSignature(p, readResult);
            byte[] after = ReadFlashImage(Path.Combine(dir, "after.bin")), expected = File.ReadAllBytes(backup.Application);
            if (!after.Take(backup.ApplicationBytes).SequenceEqual(expected)) throw new ArgumentException("La aplicación leída después de escribir no coincide con el backup completo. No se declara carga verificada; revisar posible recorte de páginas FF de la herramienta.");
            EnsureUnchanged(backup.Application, backup.ApplicationSha256); Verified = true; FlashedMegaBackup = true;
            if (service != null) { service.Status = "backup_verified"; service.Save(); }
            Steps.Add(new { flashedBackup = backup.Id, manifest = backup.Manifest, applicationSha256 = backup.ApplicationSha256, applicationBytes = backup.ApplicationBytes });
            CompletionMessage = "Último backup de aplicación ATmega flasheado y contrastado con lectura posterior. Revisá qué programa contenía antes de habilitar actuadores.";
            Stage("verified", CompletionMessage);
        }
    }
}
