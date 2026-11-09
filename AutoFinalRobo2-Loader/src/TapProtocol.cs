// AFR2-20261108-09. TAP1 records written by the ATmega debug port (afr2_debug with AFR2_DEBUG_TAP).
// Record: 0x1E 'T' type flags ms:u32LE n data[n] xor   (xor of every byte from 'T' to the last data byte).
// The text log only uses printable ASCII plus CR/LF, so 0x1E never appears inside it.
using System;
using System.Collections.Generic;
using System.IO;

namespace AFR2Loader
{
    public sealed class TapRecord
    {
        public const byte Sync = 0x1E, Marker = (byte)'T';
        public const byte FrameFromEsp = 1, FrameToEsp = 2, RawFromEsp = 3, LoaderTx = 200;
        public const byte FlagRedacted = 1, FlagTruncated = 2;
        public const int MaxData = 96;
        public byte Type, Flags;
        public uint AvrMs;
        public bool HasAvrMs = true, Corrupt;
        public byte[] Data = new byte[0];
        public DateTime At = DateTime.Now;
        public bool Redacted { get { return (Flags & FlagRedacted) != 0; } }
        public bool Truncated { get { return (Flags & FlagTruncated) != 0; } }
        public string Origin
        {
            get
            {
                switch (Type)
                {
                    case FrameFromEsp: return "ESP -> ATmega";
                    case FrameToEsp: return "ATmega -> ESP";
                    case RawFromEsp: return "ESP -> ATmega (crudo)";
                    case LoaderTx: return "Loader TX";
                    default: return "TAP tipo " + Type;
                }
            }
        }
    }

    public sealed class SerialLine
    {
        public DateTime At = DateTime.Now;
        public string Text = "";
    }

    // Splits the received byte stream into text runs and TAP1 records, in arrival order.
    // State survives across reads, so a record split between two reads is still recovered.
    public sealed class TapDemux
    {
        readonly byte[] record = new byte[10 + 255];
        int length, expected;
        public long Records, Corrupt;

        // Each returned item is either a byte[] (text bytes) or a TapRecord.
        public List<object> Push(byte[] raw, int count)
        {
            var output = new List<object>();
            var text = new MemoryStream();
            for (int i = 0; i < count; i++)
            {
                byte b = raw[i];
                if (length == 0)
                {
                    if (b == TapRecord.Sync) { record[0] = b; length = 1; }
                    else text.WriteByte(b);
                    continue;
                }
                if (length == 1 && b != TapRecord.Marker)
                {
                    // Not a record: the sync byte goes back to the text.
                    text.WriteByte(TapRecord.Sync); length = 0;
                    if (b == TapRecord.Sync) { record[0] = b; length = 1; } else text.WriteByte(b);
                    continue;
                }
                record[length++] = b;
                if (length == 9)
                {
                    if (b > TapRecord.MaxData) { Corrupt++; length = 0; continue; }
                    expected = 10 + b;
                }
                if (length >= 9 && length == expected)
                {
                    if (text.Length > 0) { output.Add(text.ToArray()); text.SetLength(0); }
                    output.Add(Build());
                    length = 0; expected = 0;
                }
            }
            if (text.Length > 0) output.Add(text.ToArray());
            return output;
        }
        TapRecord Build()
        {
            byte sum = 0;
            for (int i = 1; i < length - 1; i++) sum ^= record[i];
            int n = record[8];
            var r = new TapRecord { Type = record[2], Flags = record[3], AvrMs = BitConverter.ToUInt32(record, 4), Data = new byte[n], Corrupt = sum != record[length - 1] };
            Buffer.BlockCopy(record, 9, r.Data, 0, n);
            Records++; if (r.Corrupt) Corrupt++;
            return r;
        }
    }
}
