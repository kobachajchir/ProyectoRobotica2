// AFR2-20261028-12. Universal in-memory conversion, independent of COM,
// boot observations, firmware, framing and command catalogues.
using System;
using System.Collections.Generic;
using System.Globalization;
using System.Text;
using System.Text.RegularExpressions;

namespace AFR2Loader
{
    public sealed class HexAsciiData
    {
        public byte[] Bytes;
        public int HeaderLines;
    }
    public static class HexAsciiConverter
    {
        public const int MaxInputCharacters = 262144;
        public const int MaxBytes = 65536;

        public static HexAsciiData ParseHex(string input)
        {
            CheckInput(input);
            string source = input.Trim();
            if (source.Length >= 2 && ((source[0] == '"' && source[source.Length - 1] == '"') || (source[0] == '\'' && source[source.Length - 1] == '\''))) source = source.Substring(1, source.Length - 2).Trim();
            var bytes = new List<byte>(); int headers = 0;
            string[] lines = Regex.Split(source, @"\r\n|\r|\n");
            for (int index = 0; index < lines.Length; index++)
            {
                string line = lines[index].Trim();
                if (line.Length == 0 || Regex.IsMatch(line, @"^```(?:hex|text)?$", RegexOptions.IgnoreCase)) continue;
                if (Regex.IsMatch(line, @"^---\s+.+\s+---$")) { headers++; continue; }
                foreach (string token in Regex.Split(line, @"[\s,;]+"))
                {
                    if (token.Length == 0) continue;
                    if (Regex.IsMatch(token, @"^0x[0-9a-f]{1,2}$", RegexOptions.IgnoreCase)) bytes.Add(Byte.Parse(token.Substring(2), NumberStyles.HexNumber, CultureInfo.InvariantCulture));
                    else if (Regex.IsMatch(token, @"^(?:[0-9a-f]{2})+$", RegexOptions.IgnoreCase))
                    {
                        if (token.Length / 2 > MaxBytes - bytes.Count) throw new ArgumentException("La captura supera 64 KiB de bytes. Dividila en bloques.");
                        for (int at = 0; at < token.Length; at += 2) bytes.Add(Byte.Parse(token.Substring(at, 2), NumberStyles.HexNumber, CultureInfo.InvariantCulture));
                    }
                    else throw new ArgumentException("HEX inválido en línea " + (index + 1) + ". Usá pares HH o 0xHH; no se extraen bytes de palabras ni columnas de un log.");
                    if (bytes.Count > MaxBytes) throw new ArgumentException("La captura supera 64 KiB de bytes. Dividila en bloques.");
                }
            }
            if (bytes.Count == 0) throw new ArgumentException("Pegá al menos un byte HEX. Las cabeceras del monitor no aportan bytes.");
            return new HexAsciiData { Bytes = bytes.ToArray(), HeaderLines = headers };
        }
        public static HexAsciiData EncodeAscii(string input)
        {
            CheckInput(input);
            if (input.Length == 0) throw new ArgumentException("Escribí o pegá el texto ASCII a convertir.");
            if (input.Length > MaxBytes) throw new ArgumentException("El texto supera 64 KiB. Dividilo en bloques.");
            var bytes = new byte[input.Length];
            for (int index = 0; index < input.Length; index++)
            {
                if (input[index] > 127) throw new ArgumentException("El carácter en la posición " + (index + 1) + " no pertenece a ASCII (0..127). No se lo reemplaza por ? ni se lo convierte a otra codificación.");
                bytes[index] = (byte)input[index];
            }
            return new HexAsciiData { Bytes = bytes };
        }
        public static string FormatAscii(byte[] bytes, bool escaped)
        {
            var text = new StringBuilder();
            foreach (byte value in bytes)
            {
                if (value >= 32 && value <= 126)
                { if (escaped && value == 92) text.Append("\\\\"); else text.Append((char)value); }
                else if (value == 13) text.Append(escaped ? "\\r" : "\r");
                else if (value == 10) text.Append(escaped ? "\\n" : "\n");
                else if (value == 9) text.Append(escaped ? "\\t" : "\t");
                else text.Append("\\x" + value.ToString("X2", CultureInfo.InvariantCulture));
            }
            return text.ToString();
        }
        public static string FormatHex(byte[] bytes)
        {
            var text = new StringBuilder(bytes.Length * 3);
            for (int index = 0; index < bytes.Length; index++)
            {
                if (index > 0) text.Append(index % 16 == 0 ? "\r\n" : " ");
                text.Append(bytes[index].ToString("X2", CultureInfo.InvariantCulture));
            }
            return text.ToString();
        }
        static void CheckInput(string input)
        {
            if (input == null) throw new ArgumentNullException("input");
            if (input.Length > MaxInputCharacters) throw new ArgumentException("La entrada supera 262144 caracteres. Dividila en bloques; no se convierte una captura truncada.");
        }
    }
}
