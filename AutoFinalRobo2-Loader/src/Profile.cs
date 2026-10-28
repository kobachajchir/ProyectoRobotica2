using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Web.Script.Serialization;

namespace AFR2Loader
{
    public sealed class Field
    {
        public string Key, Label, Hint, Kind;
        public Field(string key, string label, string hint, string kind)
        { Key = key; Label = label; Hint = hint; Kind = kind; }
    }

    public sealed class Profile
    {
        public int Schema { get; set; }
        public Dictionary<string, string> Values { get; set; }
        public static readonly string UserRoot = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData), "AutoFinalRobo2Loader");
        public static readonly Field[] Fields = {
            new Field("Name", "Nombre del perfil", "Ejemplo: Robot del grupo / notebook de Juan", "text"),
            new Field("EspFirmwareMode", "Firmware ESP", "web: firmware Arduino propio; at: paquete AT. Ambos se cargan por USB/ROM.", "text"),
            new Field("WebUseFilesystem", "Incluir LittleFS Web", "true o false; el firmware y su linker deben corresponder a la elección", "text"),
            new Field("WebLayoutPreset", "Layout Web", "custom conserva el perfil; otro valor es el nombre de un linker instalado elegido arriba", "text"),
            new Field("AtPackage", "Paquete AT", "Manifiesto afr2-esp-at-package-1 con archivos, regiones y layouts del proveedor", "file"),
            new Field("AtLayoutId", "Layout del paquete AT", "ID declarado dentro del paquete; no usa offsets Arduino/LittleFS", "text"),
            new Field("AtUseFilesystem", "Incluir filesystem AT", "true o false; solo regiones filesystem declaradas en el paquete", "text"),
            new Field("Repo", "Raíz del repositorio", "Carpeta ProyectoRobotica2. No se necesita Git para cargar archivos externos.", "folder"),
            new Field("AtmegaDir", "Proyecto ATmega", "Carpeta con Makefile", "folder"),
            new Field("EspDir", "Proyecto ESP", "Carpeta con platformio.ini", "folder"),
            new Field("WebDir", "Proyecto Web", "Carpeta con package.json", "folder"),
            new Field("Make", "GNU Make", "make.exe; no nmake.exe", "file"),
            new Field("Gcc", "Compilador AVR", "avr-gcc.exe", "file"),
            new Field("Ar", "Bibliotecario AVR", "avr-ar.exe", "file"),
            new Field("Objcopy", "Conversor HEX", "avr-objcopy.exe", "file"),
            new Field("Avrdude", "Cargador AVR", "avrdude.exe", "file"),
            new Field("AvrdudeConfig", "Configuración avrdude", "avrdude.conf de la misma instalación", "file"),
            new Field("Pio", "PlatformIO", "platformio.exe o pio.exe", "file"),
            new Field("Node", "Node.js", "node.exe; se añade su carpeta al PATH de los procesos", "file"),
            new Field("Npm", "npm", "npm.cmd", "file"),
            new Field("Python", "Python de esptool", "python.exe con pyserial y dependencias de esptool", "file"),
            new Field("Esptool", "Entrada de esptool", "esptool.py; no seleccionar el ejecutable de otra versión", "file"),
            new Field("EsptoolMajor", "Versión mayor esptool", "4 o 5; adapta guiones/barras bajas de su CLI", "text"),
            new Field("PioEnv", "Entorno PlatformIO", "Nombre entre env:... de platformio.ini", "text"),
            new Field("Hex", "HEX ATmega", "Firmware de producto afr2.hex; no el HEX de comunicación sola", "file"),
            new Field("EspBin", "BIN firmware ESP", "firmware.bin del entorno elegido", "file"),
            new Field("FsBin", "Imagen LittleFS", "littlefs.bin del mismo entorno/layout", "file"),
            new Field("Linker", "Linker ESP", "eagle.flash.4m3m.ld o el linker exacto de tu build; valida rangos", "file"),
            new Field("FirmwareOffset", "Offset firmware ESP", "Offset flash, no dirección de memoria CPU; habitual 0x0", "text"),
            new Field("FsOffset", "Offset LittleFS", "Debe coincidir con _FS_start del linker menos 0x40200000", "text"),
            new Field("FsBytes", "Tamaño LittleFS (bytes)", "Debe coincidir exactamente con el rango del linker y littlefs.bin", "text"),
            new Field("FirmwareMax", "Límite BIN firmware (bytes)", "Límite de programa del layout; perfil actual 1044464", "text"),
            new Field("FlashMB", "Flash esperada (MiB)", "Se compara contra flash_id real antes de cada escritura", "text"),
            new Field("FlashMode", "Modo SPI", "keep, dio, dout, qio o qout. keep conserva la cabecera del BIN", "text"),
            new Field("FlashFreq", "Frecuencia SPI", "keep, 20m, 26m, 40m u 80m", "text"),
            new Field("MegaBaud", "Baudios carga Mega", "Habitual 115200 en bootloader Mega2560", "text"),
            new Field("EspBaud", "Baudios esptool", "Habitual 115200; 9600 puede ayudar al diagnóstico", "text"),
            new Field("Mcu", "Microcontrolador AVR", "atmega2560", "text"),
            new Field("Programmer", "Protocolo bootloader AVR", "wiring para Mega2560", "text"),
            new Field("Signature", "Firma AVR esperada", "0x1e9801; se exige en la salida antes de declarar éxito", "text"),
            new Field("TimeoutSeconds", "Tiempo máximo por comando", "Incluye build y verificación; mínimo 60, recomendado 900", "text"),
            new Field("BootWaitSeconds", "Espera KEY/RST", "Segundos de escucha a 74880; recomendado 90", "text")
        };

        public Profile() { Schema = 1; Values = new Dictionary<string, string>(); }
        public string Get(string key) { string v; return Values.TryGetValue(key, out v) ? (v ?? "") : ""; }
        public static void ValidateValue(string key, string value)
        {
            if (!Fields.Any(f => f.Key == key)) throw new ArgumentException("Opción de perfil desconocida: " + key);
            if (value == null || value.Length > 4096 || value.Any(Char.IsControl)) throw new ArgumentException("Valor inválido para " + key + ": máximo 4096 caracteres, sin caracteres de control.");
            if (key == "PioEnv" && !System.Text.RegularExpressions.Regex.IsMatch(value, @"^[a-zA-Z0-9_-]{1,100}$")) throw new ArgumentException("El entorno PlatformIO debe contener letras, números, _ o -.");
            if (key == "EspFirmwareMode" && value != "web" && value != "at") throw new ArgumentException("Firmware ESP: web o at.");
            if ((key == "WebUseFilesystem" || key == "AtUseFilesystem") && value != "true" && value != "false") throw new ArgumentException(key + ": true o false.");
        }
        public string Resolve(string key)
        {
            string v = Environment.ExpandEnvironmentVariables(Get(key));
            if (v.Contains("{Repo}") && String.IsNullOrWhiteSpace(Get("Repo"))) throw new Exception("Configurá la raíz del repositorio antes de resolver " + key + ".");
            v = v.Replace("{Repo}", Environment.ExpandEnvironmentVariables(Get("Repo")));
            v = v.Replace("{Env}", Get("PioEnv"));
            return v;
        }
        public long Number(string key)
        {
            string v = Get(key).Trim();
            return v.StartsWith("0x", StringComparison.OrdinalIgnoreCase) ? Convert.ToInt64(v.Substring(2), 16) : Convert.ToInt64(v);
        }
        public Profile Copy() { return FromJson(ToJson()); }
        public string ToJson() { return new JavaScriptSerializer().Serialize(this); }
        public static Profile FromJson(string json)
        {
            Profile p = new JavaScriptSerializer().Deserialize<Profile>(json);
            if (p == null || p.Schema != 1 || p.Values == null) throw new Exception("Perfil incompatible. Se necesita schema 1.");
            foreach (var kv in p.Values) ValidateValue(kv.Key, kv.Value);
            Profile defaults = Defaults("");
            foreach (var kv in defaults.Values) if (!p.Values.ContainsKey(kv.Key)) p.Values[kv.Key] = kv.Value;
            return p;
        }
        public void Save(string path)
        {
            foreach (var kv in Values) ValidateValue(kv.Key, kv.Value);
            Directory.CreateDirectory(Path.GetDirectoryName(Path.GetFullPath(path)));
            string temporary = path + ".tmp";
            File.WriteAllText(temporary, ToJson(), new System.Text.UTF8Encoding(false));
            if (File.Exists(path)) File.Replace(temporary, path, path + ".bak"); else File.Move(temporary, path);
        }
        public static Profile Defaults(string appDir)
        {
            Profile p = new Profile();
            string repo = "";
            if (!String.IsNullOrEmpty(appDir))
                for (DirectoryInfo d = new DirectoryInfo(appDir); d != null; d = d.Parent)
                    if (Directory.Exists(Path.Combine(d.FullName, "AutoFinalRobo2-Atmega"))) { repo = d.FullName; break; }
            var v = p.Values;
            foreach (Field f in Fields) v[f.Key] = "";
            v["Name"] = "Mi AutoFinalRobo2"; v["Repo"] = repo;
            v["EspFirmwareMode"] = "web"; v["WebUseFilesystem"] = "true"; v["WebLayoutPreset"] = "custom";
            v["AtPackage"] = ""; v["AtLayoutId"] = ""; v["AtUseFilesystem"] = "false";
            v["AtmegaDir"] = "{Repo}\\AutoFinalRobo2-Atmega";
            v["EspDir"] = "{Repo}\\AutoFinalRobo2-ESP"; v["WebDir"] = "{Repo}\\AutoFinalRobo2-Web";
            v["PioEnv"] = "esp12s_candidate"; v["EsptoolMajor"] = "4";
            v["Hex"] = "{Repo}\\AutoFinalRobo2-Atmega\\build\\afr2.hex";
            v["EspBin"] = "{Repo}\\AutoFinalRobo2-ESP\\.pio\\build\\{Env}\\firmware.bin";
            v["FsBin"] = "{Repo}\\AutoFinalRobo2-ESP\\.pio\\build\\{Env}\\littlefs.bin";
            v["FirmwareOffset"] = "0x0"; v["FsOffset"] = "0x100000"; v["FsBytes"] = "3121152";
            v["FirmwareMax"] = "1044464"; v["FlashMB"] = "4";
            v["FlashMode"] = "keep"; v["FlashFreq"] = "keep";
            v["MegaBaud"] = "115200"; v["EspBaud"] = "115200";
            v["Mcu"] = "atmega2560"; v["Programmer"] = "wiring"; v["Signature"] = "0x1e9801";
            v["TimeoutSeconds"] = "900"; v["BootWaitSeconds"] = "90";
            p.Discover(); return p;
        }
        static string FirstFile(IEnumerable<string> paths) { return paths.FirstOrDefault(File.Exists) ?? ""; }
        static IEnumerable<string> Executables(string name)
        {
            foreach (string dir in (Environment.GetEnvironmentVariable("PATH") ?? "").Split(';'))
            { string file; try { file = Path.Combine(dir.Trim('"'), name); } catch { continue; } if (File.Exists(file)) yield return file; }
        }
        static string LatestTool(string parent, string child)
        {
            if (!Directory.Exists(parent)) return "";
            return FirstFile(Directory.GetDirectories(parent).OrderByDescending(s => s).Select(s => Path.Combine(s, child)));
        }
        void Fill(string key, string value) { if (String.IsNullOrWhiteSpace(Get(key)) && !String.IsNullOrWhiteSpace(value)) Values[key] = value; }
        public void Discover()
        {
            string home = Environment.GetFolderPath(Environment.SpecialFolder.UserProfile);
            string pio = Path.Combine(home, ".platformio");
            string arduino = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData), "Arduino15", "packages", "arduino", "tools");
            Fill("Pio", FirstFile(Executables("platformio.exe").Concat(new[] { Path.Combine(pio, "penv", "Scripts", "platformio.exe") })));
            Fill("Python", FirstFile(new[] { Path.Combine(pio, "penv", "Scripts", "python.exe") }.Concat(Executables("python.exe"))));
            Fill("Esptool", PathFile(Path.Combine(pio, "packages", "tool-esptoolpy", "esptool.py")));
            Fill("Linker", PathFile(Path.Combine(pio, "packages", "framework-arduinoespressif8266", "tools", "sdk", "ld", "eagle.flash.4m3m.ld")));
            Fill("Node", FirstFile(Executables("node.exe")));
            Fill("Npm", FirstFile(Executables("npm.cmd").Concat(new[] { String.IsNullOrEmpty(Get("Node")) ? "" : Path.Combine(Path.GetDirectoryName(Get("Node")), "npm.cmd") })));
            Fill("Gcc", LatestTool(Path.Combine(arduino, "avr-gcc"), "bin\\avr-gcc.exe"));
            Fill("Ar", LatestTool(Path.Combine(arduino, "avr-gcc"), "bin\\avr-ar.exe"));
            Fill("Objcopy", LatestTool(Path.Combine(arduino, "avr-gcc"), "bin\\avr-objcopy.exe"));
            Fill("Avrdude", LatestTool(Path.Combine(arduino, "avrdude"), "bin\\avrdude.exe"));
            Fill("AvrdudeConfig", LatestTool(Path.Combine(arduino, "avrdude"), "etc\\avrdude.conf"));
            Fill("Make", FirstFile(Executables("make.exe").Concat(new[] { Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ProgramFilesX86), "Atmel", "Studio", "7.0", "shellUtils", "make.exe") })));
        }
        static string PathFile(string path) { return File.Exists(path) ? path : ""; }
    }
}
