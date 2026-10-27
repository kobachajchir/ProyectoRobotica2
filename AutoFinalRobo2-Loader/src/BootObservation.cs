using System;
using System.Collections.Generic;
using System.Text.RegularExpressions;

namespace AFR2Loader
{
    public sealed class BootObservation
    {
        public string Code, Message, Evidence;
        public int Mode;
    }
    // Incremental: accepts fragmented messages, consumes each match once, and bounds memory.
    public sealed class BootObserver
    {
        static readonly Regex Pattern = new Regex(@"rst cause:\s*(?<cause>\d+)\s*,|boot\s+mode:\s*\(\s*(?<mode>\d+)\s*,\s*(?<straps>\d+)\s*\)", RegexOptions.IgnoreCase | RegexOptions.CultureInvariant);
        string recent = "";
        long offset, consumed;
        public List<BootObservation> Push(string text)
        {
            var found = new List<BootObservation>(); recent += text;
            foreach (Match match in Pattern.Matches(recent))
            {
                long end = offset + match.Index + match.Length;
                if (end <= consumed) continue;
                consumed = end;
                if (match.Groups["cause"].Success)
                    found.Add(new BootObservation { Code = "reset_observed", Mode = -1, Evidence = match.Value,
                        Message = "Reinicio ESP observado. rst cause=" + match.Groups["cause"].Value + "; esperando modo de arranque." });
                else
                {
                    string mode = match.Groups["mode"].Value;
                    found.Add(new BootObservation { Code = mode == "1" ? "bootloader_observed" : mode == "3" ? "normal_boot_observed" : "unknown_boot_observed",
                        Mode = mode == "1" ? 1 : mode == "3" ? 3 : -1, Evidence = match.Value,
                        Message = mode == "1" ? "ESP en programación UART observado. Falta confirmar ida/vuelta con esptool."
                            : mode == "3" ? "Arranque desde flash observado. No confirma que la aplicación funcione."
                            : "Modo de arranque ESP no reconocido: " + match.Value });
                }
            }
            if (recent.Length > 16384) { int removed = recent.Length - 8192; recent = recent.Substring(removed); offset += removed; }
            return found;
        }
    }
    public sealed class LoaderNotice
    {
        public string At = DateTimeOffset.Now.ToString("o");
        public string Origin, Code, Message, Detail;
    }
}
