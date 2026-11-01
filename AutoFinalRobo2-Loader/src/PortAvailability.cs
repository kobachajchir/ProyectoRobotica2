using System;
using System.ComponentModel;
using System.Runtime.InteropServices;
using System.Text.RegularExpressions;
using Microsoft.Win32.SafeHandles;

namespace AFR2Loader
{
    // Exclusive, momentary OS handle; no bytes, DTR/RTS or serial configuration.
    // Another program may still acquire the port after this handle is released.
    public static class PortAvailability
    {
        [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
        static extern SafeFileHandle CreateFile(string name, uint access, uint share, IntPtr security, uint creation, uint flags, IntPtr template);
        public static void Check(string port)
        {
            if (!Regex.IsMatch(port ?? "", @"^COM[1-9][0-9]{0,4}$", RegexOptions.IgnoreCase)) throw new ArgumentException("Elegí un puerto COM válido.");
            using (var handle = CreateFile(@"\\.\" + port, 0xC0000000, 0, IntPtr.Zero, 3, 0, IntPtr.Zero))
                if (handle.IsInvalid) { int code = Marshal.GetLastWin32Error(); throw new InvalidOperationException(port + " no está disponible: " + new Win32Exception(code).Message + ". Cerrá el programa que lo usa y comprobá la conexión."); }
        }
    }
}
