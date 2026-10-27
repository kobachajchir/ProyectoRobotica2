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
            if (Regex.IsMatch(evidence, "No serial data received|Failed to connect|Timed out waiting for packet|No hubo respuesta", RegexOptions.IgnoreCase))
                hint = "Posible falta de sincronización PC↔ESP. Revisá modo UART, D0→E_RX, E_TX→D1, RESET del Mega a GND y alimentación. Un boot mode 1 recibido no confirma PC→ESP.";
            else if (Regex.IsMatch(evidence, "Access is denied|PermissionError|UnauthorizedAccess|acceso.*denegado|cannot open port", RegexOptions.IgnoreCase))
                hint = "Acceso denegado o recurso ocupado. Revisá el COM/ruta mencionado: cerrá monitores externos y comprobá permisos. No se eleva la aplicación automáticamente.";
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
