// AFR2-20261029-21. Read-only bootloader comparison; bridge stays binary transparent.
using System;
using System.IO;
using System.Text.RegularExpressions;
using System.Threading;
using System.Threading.Tasks;

namespace AFR2Loader
{
    public sealed partial class Operations
    {
        public async Task VerifyMegaFlasher(Profile p, string port, CancellationToken token)
        {
            PortValid(port); MegaOnly(p); Profile.ValidateValue("BridgeBaud", p.Get("BridgeBaud"));
            foreach (string key in new[] { "Gcc", "Objcopy", "Make", "Avrdude", "AvrdudeConfig" }) NeedFile(p.Resolve(key));
            string directory = p.Resolve("AtmegaDir"), source = Path.Combine(directory, "service", "esp_uart_bridge.c");
            NeedFile(source); NeedFile(Path.Combine(directory, "Makefile"));
            if (!confirm("Verificar flasher instalado en ATmega", "Puerto " + port + " | variante " + p.Get("BridgeBaud") + " baudios.\r\nCompara los bytes de flash con el flasher esperado mediante avrdude -n -D -U flash:v. No escribe ni borra flash, EEPROM o fuses.\r\nLa apertura solicita auto-reset del Mega; no comprueba la ROM ni el cableado del ESP.", new[] {
                "Potencia de actuadores desconectada; monitor serie externo cerrado.",
                "RESET Mega libre y D0/D1 aislados del ESP. USB conectado al Mega."
            })) throw new OperationCanceledException("Verificación del flasher no confirmada.");
            Stage("wiring_confirmed", "Verificación por bootloader confirmada; no se escribirá el ATmega.");
            string sourceHash = Hash(source);
            Stage("building_mega", "Preparando binario de referencia del flasher a " + p.Get("BridgeBaud") + "...");
            await Command(p, "Make", new[] { "-B", "esp-bridge-firmware", "BRIDGE_BAUD=" + p.Get("BridgeBaud"),
                "CC=\"" + Slash(p.Resolve("Gcc")) + "\"", "OBJCOPY=\"" + Slash(p.Resolve("Objcopy")) + "\"" }, directory, token);
            string hex = BridgeHex(p); NeedFile(hex); EnsureUnchanged(source, sourceHash);
            string hexHash = Hash(hex); Artifact(hex);
            Stage("build_mega_completed", "Referencia del flasher preparada; sin escritura al dispositivo.");
            MegaServiceRecord record = MegaServiceRecord.Read(port) ?? new MegaServiceRecord { Port = port };
            record.CheckStatus = "checking"; record.CheckedAt = DateTimeOffset.Now.ToString("o"); record.Save();
            try {
                Stage("checking_flasher", "Leyendo y comparando el flasher instalado mediante el bootloader...");
                var result = await Command(p, "Avrdude", new[] { "-C", p.Resolve("AvrdudeConfig"), "-v", "-n", "-D",
                    "-p", p.Get("Mcu"), "-c", p.Get("Programmer"), "-P", port, "-b", p.Get("MegaBaud"),
                    "-U", "flash:v:" + Path.GetFileName(hex) + ":i" }, Path.GetDirectoryName(hex), token);
                RequireMegaSignature(p, result);
                if (!Regex.IsMatch(result.Output, @"[1-9]\d*\s+bytes of flash verified", RegexOptions.IgnoreCase))
                    throw new CommandFailure("No se recibió confirmación de bytes verificados; no se acredita flasher instalado.", p.Resolve("Avrdude"), result.ExitCode, result.Output);
                EnsureUnchanged(source, sourceHash); EnsureUnchanged(hex, hexHash);
                record.Status = "bridge_verified"; record.CheckStatus = "matched"; record.CheckedAt = DateTimeOffset.Now.ToString("o");
                record.Baud = p.Get("BridgeBaud"); record.BridgeHex = hex; record.BridgeSha256 = hexHash; record.SourceSha256 = sourceHash; record.Save();
                Steps.Add(new { verification = "mega-flasher-flash-compare", matched = true, port = port, baud = record.Baud, hexSha256 = hexHash, checkedAt = record.CheckedAt, writing = false });
                CompletionMessage = "Flasher instalado verificado en " + port + " a " + record.Baud + " baudios. Los bytes coinciden con la referencia. No se escribió firmware; ESP sin comprobar.";
                Stage("flasher_checked", CompletionMessage);
            } catch {
                record.CheckStatus = "unconfirmed"; record.CheckedAt = DateTimeOffset.Now.ToString("o");
                // A mismatch, timeout or cancellation cannot retain positive evidence from this attempt.
                record.Status = "bridge_check_unconfirmed"; record.Save();
                throw;
            }
        }
    }
}
