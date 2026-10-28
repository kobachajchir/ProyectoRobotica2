using System;
using System.Collections.Generic;
using System.Linq;

namespace AFR2Loader
{
    public sealed class JobState
    {
        readonly object gate = new object();
        readonly List<string> lines = new List<string>();
        int firstLine;
        public string Id { get; private set; }
        public string Action { get; private set; }
        public string Origin { get; private set; }
        public string Port { get; private set; }
        public string Status { get; set; }
        public string Stage { get; set; }
        public string Result { get; set; }
        public string LogPath { get; set; }
        public string Created { get; private set; }
        public string Ended { get; set; }
        public bool WritingStarted { get; set; }
        public string ApprovalMode { get; set; }
        public JobState(string action, string origin, string port, string status)
        {
            Id = Guid.NewGuid().ToString("N"); Action = action; Origin = origin; Port = port;
            Status = status; Stage = status; Created = DateTimeOffset.Now.ToString("o");
        }
        public void Add(string line)
        {
            lock (gate)
            {
                lines.Add(line.Length > 16000 ? line.Substring(0, 16000) + " [línea recortada; ver archivo]" : line);
                if (lines.Count > 1800) { lines.RemoveRange(0, 300); firstLine += 300; }
            }
        }
        public string Detail()
        {
            lock (gate) return "Trabajo: " + Id + "\r\nAcción: " + Action + "\r\nOrigen: " + Origin + "\r\nPuerto: " + Port +
                "\r\nEstado: " + Status + "\r\nAprobación MCP: " + (ApprovalMode ?? "no aplica / pendiente") + "\r\nEtapa: " + Stage + "\r\nResultado: " + Result + "\r\nEscritura iniciada: " + WritingStarted +
                "\r\nArchivo: " + LogPath + "\r\n\r\nÚltimas 120 líneas disponibles:\r\n" + String.Join("\r\n", lines.Skip(Math.Max(0, lines.Count - 120)));
        }
        public object Snapshot(int cursor, int limit)
        {
            lock (gate)
            {
                int from = Math.Max(firstLine, Math.Min(Math.Max(0, cursor), firstLine + lines.Count));
                int count = Math.Min(Math.Max(1, Math.Min(limit, 120)), firstLine + lines.Count - from);
                var selected = new List<string>(); int characters = 0;
                foreach (string line in lines.Skip(from - firstLine).Take(count))
                { if (characters + line.Length > 90000) break; selected.Add(line); characters += line.Length; }
                string[] batch = selected.ToArray(); count = batch.Length;
                return new { id = Id, action = Action, origin = Origin, port = Port, status = Status,
                    stage = Stage, result = Result, logPath = LogPath, created = Created, ended = Ended,
                    writingStarted = WritingStarted, approvalMode = ApprovalMode, firstAvailableCursor = firstLine, cursorWasTruncated = cursor < firstLine,
                    nextCursor = from + count, hasMore = from + count < firstLine + lines.Count, lines = batch };
            }
        }
    }
    public static class LoaderVersion
    {
        public const string Value = "0.9.2";
    }
}
