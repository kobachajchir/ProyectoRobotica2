using System;
using System.Text.RegularExpressions;

namespace AFR2Loader
{
    public sealed class CommandFailure : Exception
    {
        public readonly string Tool, OutputTail;
        public readonly int ExitCode;
        public CommandFailure(string message, string tool, int exitCode, string output) : base(message)
        { Tool = tool; ExitCode = exitCode; OutputTail = output.Length > 24000 ? output.Substring(output.Length - 24000) : output; }
        public override string ToString() { return base.ToString() + "\r\nHerramienta: " + Tool + "\r\nCódigo de salida: " + ExitCode + "\r\nÚltima salida:\r\n" + OutputTail; }
    }
    public static class FailureDetails
    {
        public static string Explain(string message, string detail)
        {
            string evidence = message + "\n" + detail;
            string hint = "Revisá el comando, su salida y la etapa en que se detuvo. No se reintenta automáticamente.";
            if (evidence.Contains("AFR2-AUTO: boot_mode=1") && Regex.IsMatch(evidence, "No serial data received|Failed to connect|Timed out waiting for packet|No hubo respuesta", RegexOptions.IgnoreCase))
                hint = "Boot UART de programación confirmado en la sesión automática actual, pero falta respuesta ROM. Revisá PC→ESP: D18/TX1→E_RX, baud fijo y emisores paralelos; el boot confirma la recepción ESP→PC, no el sentido inverso.";
            else if (Regex.IsMatch(evidence, "No serial data received|Failed to connect|Timed out waiting for packet|No hubo respuesta", RegexOptions.IgnoreCase))
                hint = "Posible falta de sincronización PC↔ESP. Flasher ATmega: D18/D19, RESET LIBRE y baud fijo; USB–TTL: lógica 3,3 V, TX/RX cruzados y GND, sin otro emisor paralelo. Mantené KEY, pulsá y soltá solo RST del ESP y conservá KEY hasta el aviso del primer porcentaje de escritura o el fin de la consulta. Un log de arranque no confirma PC→ESP.";
            else if (Regex.IsMatch(evidence, "Access is denied|PermissionError|UnauthorizedAccess|acceso.*denegado|cannot open port", RegexOptions.IgnoreCase))
                hint = "Acceso denegado o recurso ocupado. Revisá el COM/ruta mencionado: cerrá monitores externos y comprobá permisos. No se eleva la aplicación automáticamente.";
            else if (Regex.IsMatch(evidence, "hfuse|BOOTRST|lecturas flash|backup.*incompatible|backup.*inválido", RegexOptions.IgnoreCase))
                hint = "No se pudo completar o contrastar el backup. Revisá la salida de avrdude y si el bootloader permite leer flash/hfuse. No se cambia a una copia HEX de PC ni se omite el respaldo automáticamente. Antes de cargar un backup revisá fecha, origen y programa; podés elegir un HEX conocido con Flashear Firmware ATmega.";
            else if (Regex.IsMatch(evidence, "verify|verification|verificaci[oó]n|MD5|checksum|hash.*match", RegexOptions.IgnoreCase))
                hint = "La comprobación de contenido o integridad no pudo completarse. Revisá archivos, salida de la herramienta, alimentación y enlace; no asumir que quedó una carga válida.";
            else if (Regex.IsMatch(evidence, "No existe|not found|FileNotFound|DirectoryNotFound", RegexOptions.IgnoreCase))
                hint = "Falta un archivo, carpeta o herramienta. Revisá el perfil y Ver rutas resueltas; elegí el binario de la compilación correspondiente.";
            else if (Regex.IsMatch(evidence, "Timeout|superó|tiempo.*agotado", RegexOptions.IgnoreCase))
                hint = "Se agotó la espera. Revisá la última salida y si el equipo seguía respondiendo. No repetir una escritura sin conocer su resultado.";
            return message + "\r\n\r\nQué revisar:\r\n" + hint + "\r\n\r\nDetalle original:\r\n" + detail;
        }
    }
}
