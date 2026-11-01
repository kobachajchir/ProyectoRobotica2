using System;
using System.Globalization;
using System.Text.RegularExpressions;

namespace AFR2Loader
{
    // AFR2-20261028-13. Presentation only: never marks a write or verification successful.
    public sealed class CommandFeedback
    {
        public string Phase, LastOutput;
        public int? Percent;
        public double ElapsedSeconds, QuietSeconds;
        public bool Running;

        public static int? WritingPercent(string line)
        {
            Match match = Regex.Match(line, @"Writing at\s+0x[0-9a-f]+[^\r\n]*?([0-9]{1,3}(?:\.[0-9]+)?)\s*%", RegexOptions.IgnoreCase);
            double value;
            if (!match.Success || !Double.TryParse(match.Groups[1].Value, NumberStyles.AllowDecimalPoint, CultureInfo.InvariantCulture, out value) || value < 0 || value > 100) return null;
            return (int)value;
        }
    }
}
