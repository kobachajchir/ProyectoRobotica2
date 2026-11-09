// AFR2-20261108-09. Decodes ROBO2/profile 2 frames against wire_catalog.json and tracks ESP resets.
// Read-only interpretation of what the ATmega reports; nothing here talks to the port.
using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;

namespace AFR2Loader
{
    public sealed class DecodedFrame
    {
        public DateTime At;
        public uint AvrMs;
        public bool HasAvrMs;
        public string Origin = "", Name = "", Does = "", Correlation = "", Result = "", Fields = "", Status = "", Hex = "";
        public byte Route, Command;
        public bool HasCommand, Request, Ok;
        public int Length;
        public byte[] Body = new byte[0];
        public string Time { get { return At.ToString("HH:mm:ss.fff", CultureInfo.InvariantCulture); } }
        public string AvrTime { get { return HasAvrMs ? AvrMs.ToString(CultureInfo.InvariantCulture) : ""; } }
        public string CommandText { get { return HasCommand ? "0x" + Command.ToString("X2") : ""; } }
        public string Detail()
        {
            var text = new StringBuilder();
            text.Append(Time).Append(HasAvrMs ? " | AVR " + AvrMs + " ms" : "").Append(" | ").Append(Origin).Append("\r\n");
            if (HasCommand) text.Append(CommandText).Append(' ').Append(Name).Append(Request ? " (pedido)" : " (respuesta)").Append("\r\n");
            if (Does.Length > 0) text.Append(Does).Append("\r\n");
            text.Append("Estado: ").Append(Status).Append("\r\n");
            if (Correlation.Length > 0) text.Append(Correlation).Append("\r\n");
            if (Result.Length > 0) text.Append("Resultado: ").Append(Result).Append("\r\n");
            if (Fields.Length > 0) text.Append("Campos: ").Append(Fields.Replace(" · ", "\r\n  ")).Append("\r\n");
            if (Hex.Length > 0) text.Append("\r\nHEX (").Append(Length).Append(" B)\r\n").Append(Hex);
            return text.ToString();
        }
    }

    public static class WireDecoder
    {
        public static string Hex(byte[] data, int offset, int count)
        {
            var text = new StringBuilder(count * 3);
            for (int i = 0; i < count; i++)
            {
                if (i > 0) text.Append(i % 16 == 0 ? "\r\n" : " ");
                text.Append(data[offset + i].ToString("X2"));
            }
            return text.ToString();
        }
        static uint U32(byte[] b, int o) { return (uint)(b[o] | (b[o + 1] << 8) | (b[o + 2] << 16) | (b[o + 3] << 24)); }

        // Frame record: route, command, payload. Redacted record: route, command, original payload length.
        public static DecodedFrame FromRecord(WireCatalog catalog, TapRecord record)
        {
            var frame = new DecodedFrame { At = record.At, AvrMs = record.AvrMs, HasAvrMs = record.HasAvrMs, Origin = record.Origin };
            if (record.Corrupt) { frame.Status = "registro TAP corrupto (XOR)"; frame.Hex = Hex(record.Data, 0, record.Data.Length); frame.Length = record.Data.Length; return frame; }
            if (record.Data.Length < 2) { frame.Status = "registro TAP incompleto"; return frame; }
            byte route = record.Data[0], command = record.Data[1];
            if (record.Redacted)
            {
                int original = record.Data.Length > 2 ? record.Data[2] : 0;
                Describe(catalog, frame, route, command);
                frame.Length = original; frame.Status = "redactado en el ATmega (" + original + " B de payload, datos privados)";
                return frame;
            }
            var payload = new byte[record.Data.Length - 2];
            Buffer.BlockCopy(record.Data, 2, payload, 0, payload.Length);
            return Decode(catalog, frame, route, command, payload);
        }

        static void Describe(WireCatalog catalog, DecodedFrame frame, byte route, byte command)
        {
            frame.Route = route; frame.Command = command; frame.HasCommand = true;
            frame.Request = route == 0x31;
            WireCommand definition = catalog == null ? null : catalog.Find(command);
            frame.Name = definition == null ? "desconocido" : definition.Name;
            frame.Does = definition == null ? "" : definition.Does;
        }

        public static DecodedFrame Decode(WireCatalog catalog, DecodedFrame frame, byte route, byte command, byte[] payload)
        {
            Describe(catalog, frame, route, command);
            frame.Length = payload.Length; frame.Hex = Hex(payload, 0, payload.Length);
            var issues = new List<string>();
            if (route != 0x31 && route != 0x13) issues.Add("ruta 0x" + route.ToString("X2") + " desconocida");
            WireCommand definition = catalog == null ? null : catalog.Find(command);
            if (definition == null) issues.Add(catalog == null ? "sin catálogo" : "CMD fuera del catálogo");
            else if (definition.Retired) issues.Add("comando retirado");
            bool noPrefix = definition != null ? definition.NoPrefix : command == 0x52;
            int prefix = noPrefix ? 0 : frame.Request ? 8 : 9;
            if (payload.Length < prefix)
            {
                issues.Add("prefijo incompleto: se esperan " + prefix + " bytes");
                frame.Status = String.Join("; ", issues.ToArray()); return frame;
            }
            if (prefix > 0)
                frame.Correlation = "sesión 0x" + U32(payload, 0).ToString("X8") + " · transacción " + U32(payload, 4);
            bool errorReply = false;
            if (prefix == 9)
            {
                byte result = payload[8];
                string name = catalog == null ? null : catalog.EnumName("result", result);
                frame.Result = name ?? ("desconocido (" + result + ")");
                errorReply = result != 0;
            }
            var body = new byte[payload.Length - prefix];
            Buffer.BlockCopy(payload, prefix, body, 0, body.Length);
            frame.Body = body;
            if (errorReply)
            {
                if (body.Length > 0) issues.Add("respuesta de error con cuerpo");
                frame.Status = issues.Count == 0 ? "rechazado: " + frame.Result : String.Join("; ", issues.ToArray());
                return frame;
            }
            if (definition != null && !definition.Retired)
            {
                WireSpec spec = frame.Request ? definition.Request : definition.Reply;
                if (definition.NoPrefix && !frame.Request) spec = definition.Reply;
                if (!spec.Accepts(body.Length)) issues.Add("cuerpo de " + body.Length + " B; se esperan " + spec.Describe());
                else frame.Fields = Fields(catalog, spec, body);
            }
            frame.Ok = issues.Count == 0;
            frame.Status = frame.Ok ? "OK" : String.Join("; ", issues.ToArray());
            return frame;
        }

        public static string Fields(WireCatalog catalog, WireSpec spec, byte[] body)
        {
            var parts = new List<string>();
            int offset = 0;
            foreach (WireField field in spec.Fields)
            {
                int size = field.Size < 0 ? body.Length - offset : field.Size;
                if (size < 0 || offset + size > body.Length) { if (!field.Optional) parts.Add(field.Name + "=(falta)"); break; }
                parts.Add(field.Name + "=" + Value(catalog, field, body, offset, size));
                offset += size;
            }
            if (spec.Fields.Count == 0 && body.Length > 0) parts.Add("cuerpo " + body.Length + " B");
            return String.Join(" · ", parts.ToArray());
        }

        static string Value(WireCatalog catalog, WireField field, byte[] b, int o, int size)
        {
            if (field.Type.StartsWith("ascii:", StringComparison.Ordinal))
            {
                var text = new StringBuilder(size);
                for (int i = 0; i < size; i++) text.Append(b[o + i] >= 32 && b[o + i] < 127 ? (char)b[o + i] : '·');
                return "\"" + text + "\"";
            }
            if (field.Type.StartsWith("bytes:", StringComparison.Ordinal)) return Hex(b, o, size).Replace("\r\n", " ");
            long value;
            switch (field.Type)
            {
                case "u8": value = b[o]; break;
                case "i8": value = (sbyte)b[o]; break;
                case "u16le": value = (ushort)(b[o] | (b[o + 1] << 8)); break;
                case "i16le": value = (short)(b[o] | (b[o + 1] << 8)); break;
                case "u32le": value = U32(b, o); break;
                default: value = (int)U32(b, o); break;
            }
            string shown;
            if (field.Format == "hex") shown = "0x" + value.ToString("X" + (size * 2).ToString(CultureInfo.InvariantCulture), CultureInfo.InvariantCulture);
            else if (field.Format == "bin") shown = Convert.ToString(value, 2).PadLeft(size * 8, '0');
            else if (field.Type == "u32le" && value == UInt32.MaxValue) shown = "desconocido";
            else shown = value.ToString(CultureInfo.InvariantCulture);
            if (field.Unit.Length > 0 && shown != "desconocido") shown += " " + field.Unit;
            string name = catalog == null ? null : catalog.EnumName(field.Enum, value);
            return name == null ? shown : shown + " (" + name + ")";
        }
    }

    // C# port of afr2_wire_push (Contracts/afr2_wire.h) to find frames inside raw TAP chunks.
    public sealed class WireScanner
    {
        public sealed class Found { public byte Route, Command; public byte[] Payload; public bool ChecksumOk, Redacted; }
        int state, position, length;
        byte route, command, checksum;
        bool drop, redacted;
        readonly byte[] payload = new byte[64];

        public void Reset() { state = 0; position = 0; checksum = 0; drop = false; redacted = false; }
        void Resync(byte b) { Reset(); if (b == (byte)'R') { state = 1; checksum = b; } }

        public Found Push(byte b, bool chunkRedacted)
        {
            switch (state)
            {
                case 0: if (b == (byte)'R') { checksum = b; state = 1; redacted = chunkRedacted; } return null;
                case 1: if (b != (byte)'O') { Resync(b); return null; } break;
                case 2: if (b != (byte)'B') { Resync(b); return null; } break;
                case 3: if (b != (byte)'O') { Resync(b); return null; } break;
                case 4: if (b != (byte)'2') { Resync(b); return null; } break;
                case 5: length = b; position = 0; drop = b > 64; break;
                case 6: if (b != 0x3A) { Resync(b); return null; } break;
                case 7: if (b != 0xA5) { Resync(b); return null; } break;
                case 8: if (b != 1) { Resync(b); return null; } break;
                case 9: route = b; if (b != 0x31 && b != 0x13) drop = true; break;
                case 10: command = b; checksum ^= b; state = length > 0 ? 11 : 12; return null;
                case 11:
                    if (!drop) payload[position] = b;
                    position++; checksum ^= b; redacted |= chunkRedacted;
                    if (position == length) state = 12;
                    return null;
                case 12:
                {
                    bool ok = b == checksum, wasDropped = drop, wasRedacted = redacted;
                    byte[] copy = new byte[wasDropped ? 0 : length];
                    if (!wasDropped) Buffer.BlockCopy(payload, 0, copy, 0, length);
                    byte frameRoute = route, frameCommand = command;
                    if (ok) Reset(); else Resync(b);
                    if (wasDropped && ok) return null;
                    return new Found { Route = frameRoute, Command = frameCommand, Payload = copy, ChecksumOk = ok, Redacted = wasRedacted };
                }
                default: Resync(b); return null;
            }
            checksum ^= b; state++; return null;
        }
    }

    // AFR2-20261108-11. One row per detected boot of the ESP or the ATmega, with the log around it.
    public sealed class BootEvent
    {
        public int Number;
        public DateTime At;
        public string Device = "", AvrMs = "", Source = "", BootId = "", Reason = "", Exception = "", Epc1 = "", Detail = "";
        // AFR2-20261108-13: what followed (HELLO / new session) and what preceded (link loss), step by step.
        public string Session = "", LinkBefore = "";
        public readonly List<string> Timeline = new List<string>();
        public bool Serious;   // exception/watchdog/brown-out: worth a notice
        public readonly List<string> Before = new List<string>(), After = new List<string>();
        public DateTime AfterUntil;
        public string Time { get { return At.ToString("HH:mm:ss.fff", CultureInfo.InvariantCulture); } }
        public string Report()
        {
            var text = new StringBuilder();
            text.Append("Arranque ").Append(Device).Append(" Nº ").Append(Number).Append(" | ").Append(At.ToString("yyyy-MM-dd HH:mm:ss.fff", CultureInfo.InvariantCulture));
            if (AvrMs.Length > 0) text.Append(" | AVR ").Append(AvrMs).Append(" ms");
            text.Append("\r\nRazón: ").Append(Reason.Length > 0 ? Reason : "sin dato");
            if (BootId.Length > 0) text.Append("\r\nBoot ID: ").Append(BootId);
            if (Exception.Length > 0) text.Append("\r\nExcepción: ").Append(Exception);
            if (Epc1.Length > 0) text.Append("\r\nepc1: ").Append(Epc1).Append("  (traducir con xtensa-lx106-elf-addr2line y el ELF cargado)");
            if (Detail.Length > 0) text.Append("\r\nDetalle: ").Append(Detail.Replace(" · ", "\r\n  "));
            text.Append("\r\nSesión nueva (HELLO): ").Append(Session.Length > 0 ? Session : "no vista");
            if (LinkBefore.Length > 0) text.Append("\r\nAntes: ").Append(LinkBefore);
            if (Timeline.Count > 0) text.Append("\r\n\r\n--- Línea de tiempo ---\r\n").Append(String.Join("\r\n", Timeline.ToArray()));
            text.Append("\r\nFuente: ").Append(Source);
            text.Append("\r\n\r\n--- Log antes del arranque (").Append(Before.Count).Append(" líneas) ---\r\n").Append(String.Join("\r\n", Before.ToArray()));
            text.Append("\r\n\r\n--- Log después (").Append(After.Count).Append(" líneas, hasta 3 s) ---\r\n").Append(String.Join("\r\n", After.ToArray()));
            return text.ToString();
        }
    }

    // Builds boot rows from the ATmega log (AVR BOOT, ESP BOOT, ESP> crash dump, ESP ROM) and BOOT frames.
    public sealed class BootTracker
    {
        static readonly Regex Stamp = new Regex(@"^(\d{1,10}) (.*)$");
        static readonly Regex AvrBoot = new Regex(@"^AVR BOOT id=(\d{1,10}) cause=0x([0-9a-fA-F]{2})(.*?) fw=(\S+) ext=(\d+)");
        static readonly Regex EspBoot = new Regex(@"^ESP BOOT id=([0-9a-fA-F]{1,8}) rst=(\d{1,3}) ?(\S*)(.*)$");
        static readonly Regex ExceptionLine = new Regex(@"Exception \((\d{1,3})\)");
        static readonly Regex Epc1 = new Regex(@"epc1=(0x[0-9a-fA-F]{8})");
        static readonly Regex Vaddr = new Regex(@"(?:exc)?vaddr=(0x[0-9a-fA-F]{8})");
        static readonly Regex Alloc = new Regex(@"last failed alloc call: ([0-9a-fA-F]{8})\((\d+)\)");
        static readonly Regex Hello = new Regex(@"^LINK hello, new session ([0-9a-fA-F]{1,8})");
        static readonly Regex Lost = new Regex(@"^LINK lost: no ESP request for (\d+) ms");
        string lostText = ""; DateTime lostAt = DateTime.MinValue;
        const int BeforeLines = 30, AfterLines = 20;
        public readonly List<BootEvent> Events = new List<BootEvent>();
        readonly Queue<string> recent = new Queue<string>();
        readonly List<BootEvent> collecting = new List<BootEvent>();
        public DateTime CaptureOpenedAt = DateTime.MinValue;
        BootEvent pendingEsp;
        DateTime pendingSince;
        int count;

        // Returns the event created or updated by this line (null when the table did not change).
        public BootEvent PushLine(WireCatalog catalog, SerialLine line)
        {
            BootEvent changed = Expire(line.At);
            for (int i = collecting.Count - 1; i >= 0; i--)
            {
                BootEvent e = collecting[i];
                if (line.At > e.AfterUntil || e.After.Count >= AfterLines) { collecting.RemoveAt(i); continue; }
                e.After.Add(Stamped(line));
            }
            string text = line.Text, ms = "";
            Match stamp = Stamp.Match(text);
            if (stamp.Success) { ms = stamp.Groups[1].Value; text = stamp.Groups[2].Value; }
            BootEvent result = Classify(catalog, line, text, ms);
            if (result != null) Step(result, Stamped(line));
            if (result == null) result = changed;
            recent.Enqueue(Stamped(line)); while (recent.Count > BeforeLines) recent.Dequeue();
            return result;
        }
        static string Stamped(SerialLine line) { return line.At.ToString("HH:mm:ss.fff", CultureInfo.InvariantCulture) + "  " + line.Text; }

        BootEvent Classify(WireCatalog catalog, SerialLine line, string text, string ms)
        {
            Match lost = Lost.Match(text);
            if (lost.Success) { lostText = "enlace perdido: el ESP no pidió nada en " + lost.Groups[1].Value + " ms (" + line.At.ToString("HH:mm:ss.fff", CultureInfo.InvariantCulture) + ")"; lostAt = line.At; return null; }
            Match hello = Hello.Match(text);
            if (hello.Success) return SessionStarted(line, ms, hello.Groups[1].Value.ToLowerInvariant().PadLeft(8, '0'), "log LINK hello");
            Match avr = AvrBoot.Match(text);
            if (avr.Success)
            {
                int cause = Convert.ToInt32(avr.Groups[2].Value, 16);
                var e = Create(line.At, ms, "ATmega", "log AVR BOOT");
                e.BootId = avr.Groups[1].Value; e.Reason = "0x" + avr.Groups[2].Value + " " + avr.Groups[3].Value.Trim();
                e.Detail = "fw " + avr.Groups[4].Value + " EX" + avr.Groups[5].Value;
                e.Serious = (cause & 0x0C) != 0;   // brown-out or watchdog
                if ((cause & 0x02) != 0 && (line.At - CaptureOpenedAt).TotalSeconds < 4)
                    e.Detail += " · reset-pin a menos de 4 s de abrir el puerto: probablemente el auto-reset del USB, no una falla";
                if ((cause & 0x08) != 0) e.Detail += " · watchdog: el loop se trabó más de 500 ms";
                if ((cause & 0x04) != 0) e.Detail += " · brown-out: caída de la alimentación";
                return e;
            }
            Match boot = EspBoot.Match(text);
            if (boot.Success)
            {
                string reason = boot.Groups[3].Value;
                int code = Int32.Parse(boot.Groups[2].Value, CultureInfo.InvariantCulture);
                if (catalog != null) reason = catalog.EnumName("esp_reset_reason", code) ?? reason;
                return MergeEsp(line.At, ms, "log ESP BOOT", boot.Groups[1].Value.ToLowerInvariant().PadLeft(8, '0'), "rst=" + code + " " + reason, boot.Groups[4].Value.Trim(), code >= 1 && code <= 4);
            }
            if (text.StartsWith("ESP ROM", StringComparison.Ordinal))
            {
                BootEvent p = PendingEsp(line.At, ms, "ROM a 74880 (ruido)");
                p.Detail = Append(p.Detail, text.Substring(3).Trim()); return p;
            }
            if (!text.StartsWith("ESP>", StringComparison.Ordinal) && !text.StartsWith("ESP+", StringComparison.Ordinal)) return null;
            Match exception = ExceptionLine.Match(text), epc = Epc1.Match(text), vaddr = Vaddr.Match(text), alloc = Alloc.Match(text);
            BootEvent target = null;
            if (exception.Success)
            {
                int cause = Int32.Parse(exception.Groups[1].Value, CultureInfo.InvariantCulture);
                string name = catalog == null ? null : catalog.EnumName("xtensa_exccause", cause);
                target = PendingEsp(line.At, ms, "volcado ESP"); target.Exception = cause + (name == null ? "" : " " + name); target.Serious = true;
            }
            if (epc.Success) { target = PendingEsp(line.At, ms, "volcado ESP"); target.Epc1 = epc.Groups[1].Value; }
            if (vaddr.Success) { target = PendingEsp(line.At, ms, "volcado ESP"); target.Detail = Append(target.Detail, "excvaddr=" + vaddr.Groups[1].Value); }
            if (alloc.Success) { target = PendingEsp(line.At, ms, "volcado ESP"); target.Detail = Append(target.Detail, "alloc fallido en 0x" + alloc.Groups[1].Value + " (" + alloc.Groups[2].Value + " B): sin memoria"); target.Serious = true; }
            if (text.IndexOf("wdt reset", StringComparison.OrdinalIgnoreCase) >= 0)
            { target = PendingEsp(line.At, ms, "volcado ESP"); target.Detail = Append(target.Detail, "watchdog"); target.Serious = true; }
            return target;
        }

        // BOOT frame from the ESP (route 0x31, CMD 0x52, BT01 body).
        public BootEvent PushFrame(WireCatalog catalog, DecodedFrame frame)
        {
            if (frame.HasCommand && frame.Route == 0x31 && (frame.Command == 0x01 || frame.Command == 0x10))
            {
                BootEvent recent = Events.LastOrDefault(e => e.Device == "ESP" && (frame.At - e.At).TotalSeconds < 10);
                if (recent == null) return null;
                Step(recent, frame.Time + "  trama " + (frame.Command == 0x01 ? "HELLO (abre sesión ROBO2)" : "EXT_HELLO (negocia EX01)") + " " + frame.Correlation);
                return recent;
            }
            if (!frame.HasCommand || frame.Command != 0x52 || frame.Route != 0x31 || frame.Body.Length < 14) return null;
            byte[] b = frame.Body;
            uint id = (uint)(b[5] | (b[6] << 8) | (b[7] << 16) | (b[8] << 24));
            string reason = catalog == null ? null : catalog.EnumName("esp_reset_reason", b[9]);
            BootEvent booted = MergeEsp(frame.At, frame.AvrTime, "trama BOOT", id.ToString("x8"), "rst=" + b[9] + (reason == null ? "" : " " + reason), "fw " + b[10] + "." + b[11] + "." + b[12] + " EX" + b[13], b[9] >= 1 && b[9] <= 4);
            Step(booted, frame.Time + "  trama BOOT rst=" + b[9] + (reason == null ? "" : " " + reason));
            return booted;
        }

        BootEvent Create(DateTime at, string ms, string device, string source)
        {
            var e = new BootEvent { Number = ++count, At = at, AvrMs = ms, Device = device, Source = source, AfterUntil = at.AddSeconds(3) };
            e.Before.AddRange(recent);
            if (device == "ESP" && lostText.Length > 0 && (at - lostAt).TotalSeconds < 10) e.LinkBefore = lostText;
            Events.Add(e); collecting.Add(e);
            while (Events.Count > 200) Events.RemoveAt(0);
            return e;
        }
        BootEvent Expire(DateTime now)
        {
            if (pendingEsp == null || (now - pendingSince).TotalSeconds < 15) return null;
            BootEvent e = pendingEsp; e.Detail = Append(e.Detail, "sin línea ESP BOOT en 15 s"); pendingEsp = null; return e;
        }
        BootEvent PendingEsp(DateTime at, string ms, string source)
        {
            if (pendingEsp != null) { if (pendingEsp.Source.IndexOf(source, StringComparison.Ordinal) < 0) pendingEsp.Source = Append(pendingEsp.Source, source); return pendingEsp; }
            pendingEsp = Create(at, ms, "ESP", source); pendingEsp.Reason = "reinicio en curso"; pendingSince = at;
            return pendingEsp;
        }
        BootEvent MergeEsp(DateTime at, string ms, string source, string bootId, string reason, string detail, bool serious)
        {
            BootEvent target = Events.LastOrDefault(e => e.Device == "ESP" && e.BootId == bootId) ?? pendingEsp ?? Create(at, ms, "ESP", source);
            if (target.Source.IndexOf(source, StringComparison.Ordinal) < 0) target.Source = Append(target.Source, source);
            target.BootId = bootId; target.Reason = reason; target.Serious |= serious;
            if (detail.Length > 0 && target.Detail.IndexOf(detail, StringComparison.Ordinal) < 0) target.Detail = Append(target.Detail, detail);
            if (target.AvrMs.Length == 0) target.AvrMs = ms;
            if (target == pendingEsp) pendingEsp = null;
            return target;
        }
        static void Step(BootEvent e, string step) { if (e.Timeline.Count < 40 && (e.Timeline.Count == 0 || e.Timeline[e.Timeline.Count - 1] != step)) e.Timeline.Add(step); }
        // HELLO after a boot: how long the new session took. HELLO with no boot: the link restarted alone.
        BootEvent SessionStarted(SerialLine line, string ms, string session, string source)
        {
            BootEvent e = Events.LastOrDefault(x => x.Device == "ESP" && x.BootId == session && x.Session.Length == 0)
                ?? Events.LastOrDefault(x => x.Device == "ESP" && x.Session.Length == 0 && (line.At - x.At).TotalSeconds < 10)
                ?? Events.LastOrDefault(x => x.Device == "ATmega" && x.Session.Length == 0 && (line.At - x.At).TotalSeconds < 10);
            long now, then;
            if (e != null)
            {
                string delay = Int64.TryParse(ms, out now) && Int64.TryParse(e.AvrMs, out then) && now >= then ? "+" + (now - then) + " ms" : "+" + (long)(line.At - e.At).TotalMilliseconds + " ms (PC)";
                e.Session = delay + " · sesión " + session;
                return e;
            }
            e = Create(line.At, ms, "Enlace", source);
            e.BootId = session; e.Reason = "sesión nueva sin reinicio visto del ESP"; e.Session = "nueva · sesión " + session;
            if (lostText.Length > 0 && (line.At - lostAt).TotalSeconds < 10) e.LinkBefore = lostText;
            return e;
        }
        static string Append(string current, string more) { return String.IsNullOrEmpty(current) ? more : current + " · " + more; }
        public void Clear() { Events.Clear(); collecting.Clear(); pendingEsp = null; count = 0; lostText = ""; }
    }
}
