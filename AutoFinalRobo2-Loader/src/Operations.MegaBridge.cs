using System;
using System.IO;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Web.Script.Serialization;

namespace AFR2Loader
{
    public sealed class MegaServiceRecord
    {
        public string Port, Baud, Status, At, ProductSource, ProductSnapshot, ProductSha256, BridgeHex, BridgeSha256, SourceSha256;
        public string BackupManifest;
        public string CheckStatus, CheckedAt;
        public bool BackupRequested;
        public static string RecordPath(string port)
        {
            if (!System.Text.RegularExpressions.Regex.IsMatch(port, @"^COM[1-9][0-9]{0,4}$", System.Text.RegularExpressions.RegexOptions.IgnoreCase)) throw new ArgumentException("COM inválido.");
            return Path.Combine(Profile.UserRoot, "service", "mega-" + port.ToUpperInvariant() + ".json");
        }
        public static MegaServiceRecord Read(string port)
        {
            string path = RecordPath(port);
            return File.Exists(path) ? new JavaScriptSerializer().Deserialize<MegaServiceRecord>(File.ReadAllText(path)) : null;
        }
        public void Save()
        {
            string path = RecordPath(Port); Directory.CreateDirectory(Path.GetDirectoryName(path));
            string temp = path + ".tmp";
            File.WriteAllText(temp, new JavaScriptSerializer().Serialize(this), new UTF8Encoding(false));
            if (File.Exists(path)) File.Replace(temp, path, path + ".bak"); else File.Move(temp, path);
        }
    }
    public sealed partial class Operations
    {
        public string BridgeToolPath;
        public string CompletionMessage;
        public bool InstalledMegaFlasher, MegaFirmwareFlashed;
        public static bool UsesMegaBridge(Profile p) { return p.Get("EspTransport") == "mega_bridge"; }
        // AFR2-20261106-11: opt-in; solo con el flasher ATmega y el circuito de transistores armado.
        public static bool AutoKeyRst(Profile p) { return UsesMegaBridge(p) && p.Get("EspKeyRst") == "auto"; }
        static string BridgeHex(Profile p) { return Path.Combine(p.Resolve("AtmegaDir"), "build", "afr2-esp-bridge-" + p.Get("BridgeBaud") + ".hex"); }
        static void CheckBridge(Profile p)
        {
            Profile.ValidateValue("EspTransport", p.Get("EspTransport"));
            Profile.ValidateValue("BridgeBaud", p.Get("BridgeBaud"));
            Profile.ValidateValue("BridgeStartupSeconds", p.Get("BridgeStartupSeconds"));
            if (p.Get("EspBaud") != p.Get("BridgeBaud")) throw new ArgumentException("Puente fijo: Baudios esptool debe coincidir con Baudios flasher ATmega. No alcanza cambiar Windows; cargar la variante correspondiente.");
        }
        public async Task InstallMegaFlasher(Profile p, string port, CancellationToken token, bool backupBeforeLoad)
        {
            PortValid(port); Profile.ValidateValue("BridgeBaud", p.Get("BridgeBaud"));
            if (p.Get("Mcu") != "atmega2560" || p.Get("Signature").ToLowerInvariant() != "0x1e9801") throw new ArgumentException("La imagen de servicio requiere ATmega2560/firma 0x1e9801.");
            foreach (string key in new[] { "Gcc", "Objcopy", "Make", "Avrdude", "AvrdudeConfig" }) NeedFile(p.Resolve(key));
            string directory = p.Resolve("AtmegaDir"), source = Path.Combine(directory, "service", "esp_uart_bridge.c");
            NeedFile(source); NeedFile(Path.Combine(directory, "Makefile"));
            MegaServiceRecord previous = MegaServiceRecord.Read(port);
            if (!confirm("Cargar flasher temporal en ATmega", "Puerto " + port + " | puente USART0↔USART1 a " + p.Get("BridgeBaud") + "\r\nSe compilará la fuente de servicio y se reemplazará la aplicación del Mega.\r\n\r\n" +
                (backupBeforeLoad ? "BACKUP ACTIVADO: antes de escribir se leerá la flash del Mega dos veces. Si falla, se detiene. EEPROM no incluida; no se programan fuses." : "BACKUP DESACTIVADO: no se leerá ni creará un nuevo respaldo de la aplicación actual.") +
                (previous == null ? "" : "\r\nRegistro anterior: " + previous.Status + ". El backup leerá lo que hay AHORA en el Mega: puede ser otro flasher. Los respaldos anteriores se conservan."), new[] {
                "Potencia de motores/servos desconectada; USB y alimentación lógica identificados.",
                "Quité RESET Mega→GND. RESET del Mega queda LIBRE durante todo este flujo.",
                "D18 y D19 DESCONECTADOS del ESP durante la carga del flasher. D0/D1 al ESP y jumpers paralelos retirados.",
                "Alimentación lógica y masa común identificadas.",
                "Revisé la opción de backup. Mientras use el flasher, el auto no ejecutará su aplicación."
            })) throw new OperationCanceledException("Instalación del flasher no confirmada.");
            Stage("wiring_confirmed", "Puente software: cableado confirmado; RESET Mega libre.");
            string sourceHash = Hash(source);
            Stage("building_mega", "Compilando flasher temporal para ATmega a " + p.Get("BridgeBaud") + "...");
            await Command(p, "Make", new[] { "-B", "esp-bridge-firmware", "BRIDGE_BAUD=" + p.Get("BridgeBaud"),
                "CC=\"" + Slash(p.Resolve("Gcc")) + "\"", "OBJCOPY=\"" + Slash(p.Resolve("Objcopy")) + "\"" }, directory, token);
            string hex = BridgeHex(p); NeedFile(hex); EnsureUnchanged(source, sourceHash);
            Artifact(hex); Stage("build_mega_completed", "Flasher ATmega compilado. Todavía no se reemplazó la aplicación.");
            // AFR2-20261030-15: compare actual flash before deciding whether to back up.
            bool installedFlasher = false;
            Stage("checking_flasher", "Comprobando software ATmega antes del backup...");
            try {
                var check = await Command(p, "Avrdude", new[] { "-C", p.Resolve("AvrdudeConfig"), "-v", "-n", "-D",
                    "-p", p.Get("Mcu"), "-c", p.Get("Programmer"), "-P", port, "-b", p.Get("MegaBaud"),
                    "-U", "flash:v:" + Path.GetFileName(hex) + ":i" }, Path.GetDirectoryName(hex), token);
                RequireMegaSignature(p, check);
                installedFlasher = System.Text.RegularExpressions.Regex.IsMatch(check.Output, @"[1-9]\d*\s+bytes of flash verified", System.Text.RegularExpressions.RegexOptions.IgnoreCase);
                if (!installedFlasher) throw new ArgumentException("Comprobación sin resultado concluyente; se detiene antes del backup y escritura.");
            } catch (CommandFailure ex) {
                // Only a real byte mismatch identifies a different program. Transport failures abort.
                if (ex.OutputTail.IndexOf("0x1e9801", StringComparison.OrdinalIgnoreCase) < 0 ||
                    !System.Text.RegularExpressions.Regex.IsMatch(ex.OutputTail, "verification error.*(mismatch|content mismatch)", System.Text.RegularExpressions.RegexOptions.IgnoreCase)) throw;
            }
            if (installedFlasher) Stage("backup_skipped", "El ATmega contiene el flasher verificado: se omite backup y se conservan los respaldos anteriores.");
            MegaFlashBackup backup = backupBeforeLoad && !installedFlasher ? await BackupMegaFlash(p, port, token) : null;
            string backupDescription = backup == null ? "Sin backup nuevo. Los anteriores se conservan, si existían." : "Backup leído del Mega: " + backup.Manifest;
            var record = new MegaServiceRecord { Port = port, Baud = p.Get("BridgeBaud"), Status = "prepared", At = DateTimeOffset.Now.ToString("o"),
                BackupRequested = backupBeforeLoad, BackupManifest = backup == null ? null : backup.Manifest, BridgeHex = hex, BridgeSha256 = Hash(hex), SourceSha256 = sourceHash };
            record.Save(); Steps.Add(new { service = "mega_bridge", baud = record.Baud, sourceSha256 = sourceHash, backupRequested = backupBeforeLoad, backup = record.BackupManifest, hex = hex, hexSha256 = record.BridgeSha256 });
            log(backupDescription);
            if (!confirm("Escribir flasher en ATmega", Describe(hex) + "\r\n\r\n" + backupDescription + "\r\nSe grabará/verificará solo la aplicación, sin fuses ni EEPROM.",
                new[] { "Confirmo reemplazar temporalmente el programa del auto. D18/D19 DESCONECTADOS, RESET Mega libre y potencia de actuadores desconectada." })) throw new OperationCanceledException("Escritura de servicio no confirmada.");
            token.ThrowIfCancellationRequested(); EnsureUnchanged(hex, record.BridgeSha256);
            record.Status = "bridge_write_started"; record.Save();
            await UploadMegaApplication(p, port, hex, token);
            Verified = true; record.Status = "bridge_verified"; record.Save(); InstalledMegaFlasher = true;
            CompletionMessage = "Flasher temporal ATmega escrito y verificado a " + record.Baud + ". Falta detectar el ESP; al terminar usá Flashear Firmware ATmega o Flashear último backup.";
            Stage("verified", CompletionMessage);
        }
        public async Task RestoreMegaProduct(Profile p, string port, CancellationToken token)
        {
            await FlashLatestMegaBackup(p, port, token);
        }
        async Task UploadMegaApplication(Profile p, string port, string hex, CancellationToken token, string format = "i")
        {
            string hash = Hash(hex); token.ThrowIfCancellationRequested();
            Stage("writing", "Cargando aplicación ATmega por bootloader. RESET libre; auto-reset solicitado por avrdude."); MarkWriting();
            var result = await Command(p, "Avrdude", new[] { "-C", p.Resolve("AvrdudeConfig"), "-v", "-p", p.Get("Mcu"), "-c", p.Get("Programmer"),
                "-P", port, "-b", p.Get("MegaBaud"), "-D", "-U", "flash:w:" + Path.GetFileName(hex) + ":" + format }, Path.GetDirectoryName(hex), token);
            if (result.Output.IndexOf(p.Get("Signature"), StringComparison.OrdinalIgnoreCase) < 0 || !System.Text.RegularExpressions.Regex.IsMatch(result.Output, @"\d+\s+bytes of flash verified", System.Text.RegularExpressions.RegexOptions.IgnoreCase))
                throw new CommandFailure("avrdude terminó, pero no se reconoció firma/verificación. No se acredita carga correcta.", p.Resolve("Avrdude"), result.ExitCode, result.Output);
            EnsureUnchanged(hex, hash);
        }
        public async Task PrepareEspSession(Profile p, string port, bool filesystem, bool listen, CancellationToken token)
        {
            if (!PrepareEsp(p, filesystem)) throw new OperationCanceledException("Cableado ESP no confirmado.");
            if (!UsesMegaBridge(p)) {
                if (listen) await ListenBoot(p, port, token);
                else RequirePhysicalEsp("Intervención física necesaria · KEY y RST del ESP", "1. Apretá y mantené KEY del shield.\r\n2. Sin soltar KEY, apretá RST del ESP.\r\n3. Soltá SOLO RST y seguí manteniendo KEY.\r\n4. Confirmá abajo para consultar la ROM y continuar.\r\n\r\nMantené KEY hasta el aviso del Loader o el fin de la operación. No pulses RESET del ATmega. La escucha 74880 está desactivada: la respuesta ROM comprobará la comunicación.", token);
                return;
            }
            CheckBridge(p);
            MegaServiceRecord record = MegaServiceRecord.Read(port);
            if (record != null && (record.Status != "bridge_verified" || record.Baud != p.Get("BridgeBaud"))) throw new ArgumentException("El registro de este COM no acredita flasher verificado a ese baud. Usá Cargar flasher en ATmega antes de consultar ESP. Estado: " + record.Status);
            GuideMegaBridgeBoot(p, false, token);
        }
        void GuideMegaBridgeBoot(Profile p, bool forVerification, CancellationToken token)
        {
            token.ThrowIfCancellationRequested(); CheckBridge(p); EspSessionStarted = true;
            if (AutoKeyRst(p)) {
                KeyRstAutomatic = true;
                Stage("boot_entry_requested", "KEY/RST automáticos: el flasher AFR2 acciona D32/D33 al abrir el puerto de esptool. No se afirma modo observado por UART hasta la respuesta ROM.");
                return;
            }
            RequirePhysicalEsp("Intervención física necesaria · ESP por flasher ATmega", "Mega ejecutando flasher a " + p.Get("BridgeBaud") + ". RESET del Mega LIBRE.\r\n\r\n1. Apretá y mantené KEY del ESP.\r\n2. Pulsá y soltá SOLO RST del ESP.\r\n3. Seguí manteniendo KEY y confirmá abajo.\r\n\r\nConservá KEY hasta el aviso del Loader o el fin de la operación. No pulses RESET del ATmega.\r\nNo se escucha 74880: el puente tiene baud fijo. La consulta comprobará comunicación bidireccional.\r\nEl wrapper esperará al Mega DESPUÉS de abrir el mismo COM para esptool." + (forVerification ? "\r\nNo se repetirá la escritura, solo consulta y verificación." : ""), token);
            Stage("boot_entry_requested", "Secuencia KEY/RST confirmada manualmente; falta respuesta ROM. No se afirma modo observado por UART.");
            token.ThrowIfCancellationRequested();
        }
        void RequirePhysicalEsp(string title, string text, CancellationToken token)
        {
            token.ThrowIfCancellationRequested();
            if (EspPhysicalConfirmation == null) throw new InvalidOperationException("Falta el asistente obligatorio de intervención física ESP.");
            Stage("esp_physical_required", "Esperando intervención física local: KEY y RST del ESP.");
            if (!EspPhysicalConfirmation(title, text, token)) throw new OperationCanceledException("Intervención física ESP no confirmada.");
            token.ThrowIfCancellationRequested();
        }
        string EspRestoration(Profile p)
        {
            if (AutoKeyRst(p)) return "KEY/RST automáticos en cada sesión física: consultá el registro para confirmar boot, liberación de pines y reinicio final. RESET Mega sigue libre. Al terminar usá Flashear Firmware ATmega o el menú Flashear último backup; no habilitar motores con el flasher temporal instalado.";
            return UsesMegaBridge(p) ? "RESET Mega sigue libre. Al terminar usá Flashear Firmware ATmega o el menú Flashear último backup; no habilitar motores con el flasher temporal instalado. KEY libre y resetear ESP para arrancar desde flash."
                : p.Get("EspTransport") == "usb-ttl" ? "Para operar: al terminar soltar KEY, apagar y retirar el adaptador USB–TTL; restaurar D18→E_RX y E_TX→D19 con RESET Mega libre. KEY libre; resetear ESP."
                : "KEY libre; resetear ESP.";
        }
    }
}
