// AFR2-20261106-17. Explicit diagnostic, no firmware writing.
using System;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
namespace AFR2Loader
{
    public sealed partial class Operations
    {
        public async Task TestKeyRstBoot(Profile p, string port, CancellationToken token)
        {
            PortValid(port); NeedFile(BridgeToolPath);
            if (!UsesMegaBridge(p)) throw new ArgumentException("La prueba requiere el flasher ATmega, RESET Mega libre y D18/D19.");
            if (!confirm("Probar KEY/RST y capturar boot", "Puerto " + port + " | USB flasher " + p.Get("BridgeBaud") + " baudios.\r\nAcciona D32=KEY (violeta) y D33=RST (marrón). Reinicia el ESP y captura su boot a 74880 mediante UART1.\r\nEl flasher debe admitir captura boot. No escribe firmware. Al terminar suelta KEY/RST, desactiva automático y no reinicia otra vez el ESP.",
                new[] { "Potencia de actuadores desconectada, RESET Mega libre y monitor externo cerrado.", "Circuito conectado: D32 a KEY, D33 a RST y GND común; D18/D19 conectados al ESP." }))
                throw new OperationCanceledException("Prueba KEY/RST cancelada.");
            Stage("wiring_confirmed", "Prueba KEY/RST confirmada; sin escritura.");
            var args = new[] { "-u", BridgeToolPath, "--test-keyrst-boot", "--port", port, "--bridge-baud", p.Get("BridgeBaud"), "--startup-seconds", p.Get("BridgeStartupSeconds") };
            var result = await Command(p, "Python", args, Path.GetDirectoryName(BridgeToolPath), token, 20);
            if (!System.Text.RegularExpressions.Regex.IsMatch(result.Output, @"(?m)^AFR2-BOOTTEST: mode=1\r?$") || !result.Output.Contains("AFR2-BOOTTEST: sequence=confirmed"))
                throw new Exception("No se acreditó boot UART de programación.");
            Steps.Add(new { diagnostic = "keyrst-boot", mode = 1, port = port, writing = false, romRoundtrip = false });
            CompletionMessage = "Secuencia KEY/RST confirmada y boot de programación observado. Pines sueltos; sin escritura ni consulta ROM ida/vuelta.";
            Stage("bootloader_observed", CompletionMessage);
        }
    }
}
