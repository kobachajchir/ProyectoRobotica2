using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.IO.Ports;
using System.Linq;
using System.Security.Cryptography;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading;
using System.Threading.Tasks;

namespace AFR2Loader
{
    public sealed class Operations
    {
        readonly Runner runner;
        readonly Action<string> log, state;
        readonly Func<string, string, string[], bool> confirm;
        public bool WroteAnything { get; private set; }
        public bool Verified { get; private set; }
        public Action WritingStarted;
        public Action<string, string> Changed;
        void Stage(string code, string message) { state(message); if (Changed != null) Changed(code, message); }
        void MarkWriting() { WroteAnything = true; if (WritingStarted != null) WritingStarted(); }
        public List<object> Steps = new List<object>();
        public Operations(Runner runner, Action<string> log, Action<string> state,
                          Func<string, string, string[], bool> confirm)
        { this.runner = runner; this.log = log; this.state = state; this.confirm = confirm; }

        static void NeedFile(string path) { if (!File.Exists(path)) throw new Exception("No existe el archivo: " + path); }
        static string Slash(string path) { return path.Replace('\\', '/'); }
        public static string Hash(string path)
        { using (var file = File.OpenRead(path)) using (var sha = SHA256.Create()) return BitConverter.ToString(sha.ComputeHash(file)).Replace("-", "").ToLowerInvariant(); }
        public static string Describe(string path)
        { NeedFile(path); return path + "\r\n" + new FileInfo(path).Length + " bytes | SHA-256 " + Hash(path); }
        static void PortValid(string port)
        { if (!Regex.IsMatch(port, @"^COM\d+$", RegexOptions.IgnoreCase)) throw new Exception("Elegí un puerto COM válido."); }
        static void Choice(string value, params string[] accepted)
        { if (!accepted.Contains(value)) throw new Exception("Valor no admitido: " + value); }
        static void EnsureUnchanged(string path, string hash)
        { if (Hash(path) != hash) throw new Exception("El archivo cambió durante la operación. No se continúa: " + path); }

        public async Task<CommandResult> Command(Profile p, string key, IEnumerable<string> args, string dir, CancellationToken token, int timeout = 0)
        {
            var watch = Stopwatch.StartNew();
            int verificationNotice = 0;
            runner.OutputLine = line => {
                if (key == "Avrdude" && line.IndexOf("verifying flash memory against", StringComparison.OrdinalIgnoreCase) >= 0 && Interlocked.Exchange(ref verificationNotice, 1) == 0)
                    if (Changed != null) Changed("verifying", "ATmega: avrdude inició la verificación de flash.");
            };
            CommandResult result;
            try { result = await runner.Run(p.Resolve(key), args, dir, p, token, timeout); }
            finally { runner.OutputLine = null; }
            Steps.Add(new { tool = p.Resolve(key), arguments = args.ToArray(), directory = dir, exitCode = result.ExitCode, elapsedSeconds = watch.Elapsed.TotalSeconds });
            if (result.ExitCode != 0)
            {
                string hint = "Revisá la salida completa en el registro.";
                if (result.Output.IndexOf("No serial data received", StringComparison.OrdinalIgnoreCase) >= 0)
                    hint = "No hubo respuesta de esptool. boot mode:(1,...) confirma el modo y ESP→PC, pero no PC→ESP. Revisá D0→E_RX, KEY/RST y RESET del Mega a GND.";
                else if (Regex.IsMatch(result.Output, "Access is denied|PermissionError|UnauthorizedAccess|cannot open port", RegexOptions.IgnoreCase))
                    hint = "Revisá monitor serie abierto, permisos de herramientas/cachés y puerto. La aplicación no cambia permisos ni eleva privilegios automáticamente.";
                throw new CommandFailure("El comando terminó con código " + result.ExitCode + ". " + hint, p.Resolve(key), result.ExitCode, result.Output);
            }
            return result;
        }

        public async Task BuildMega(Profile p, CancellationToken token)
        {
            Stage("building", "Compilando ATmega: producto, comunicación y biblioteca...");
            foreach (string key in new[] { "Gcc", "Ar", "Objcopy" }) NeedFile(p.Resolve(key));
            string dir = p.Resolve("AtmegaDir"); NeedFile(Path.Combine(dir, "Makefile"));
            await Command(p, "Make", new[] { "-B", "firmware", "communication-firmware", "library",
                "CC=\"" + Slash(p.Resolve("Gcc")) + "\"", "AR=\"" + Slash(p.Resolve("Ar")) + "\"",
                "OBJCOPY=\"" + Slash(p.Resolve("Objcopy")) + "\"" }, dir, token);
            Artifact(Path.Combine(dir, "build", "afr2.hex"));
            Artifact(Path.Combine(dir, "build", "afr2-communication.hex"));
            Artifact(Path.Combine(dir, "libafr2_ui.a"));
        }
        public async Task BuildEsp(Profile p, CancellationToken token)
        {
            Stage("building", "Compilando firmware ESP..."); NeedFile(Path.Combine(p.Resolve("EspDir"), "platformio.ini"));
            await Command(p, "Pio", new[] { "run", "-e", p.Get("PioEnv") }, p.Resolve("EspDir"), token);
            Artifact(Path.Combine(p.Resolve("EspDir"), ".pio", "build", p.Get("PioEnv"), "firmware.bin"));
        }
        public async Task BuildWebFs(Profile p, CancellationToken token)
        {
            Stage("building", "Web: build:esp, gzip y copia a ESP/data/dist...");
            NeedFile(p.Resolve("Node")); NeedFile(Path.Combine(p.Resolve("WebDir"), "package.json"));
            await Command(p, "Npm", new[] { "run", "build:esp" }, p.Resolve("WebDir"), token);
            string manifest = Path.Combine(p.Resolve("EspDir"), "data", "dist", "manifest.json");
            NeedFile(manifest);
            var m = new System.Web.Script.Serialization.JavaScriptSerializer().Deserialize<Dictionary<string, object>>(File.ReadAllText(manifest));
            if (!m.ContainsKey("buildProfile") || Convert.ToString(m["buildProfile"]) != "esp") throw new Exception("El manifiesto promovido no pertenece al perfil esp.");
            Stage("building", "Generando imagen LittleFS desde ESP/data...");
            await Command(p, "Pio", new[] { "run", "-e", p.Get("PioEnv"), "-t", "buildfs" }, p.Resolve("EspDir"), token);
            string image = Path.Combine(p.Resolve("EspDir"), ".pio", "build", p.Get("PioEnv"), "littlefs.bin");
            if (new FileInfo(image).Length != p.Number("FsBytes")) throw new Exception("Imagen creada, pero su tamaño no coincide con el perfil. Revisá linker, entorno y FsBytes antes de cargar.");
            Artifact(image); Artifact(manifest);
        }
        void Artifact(string path)
        {
            log("Artefacto generado: " + Describe(path));
            Steps.Add(new { artifact = path, bytes = new FileInfo(path).Length, sha256 = Hash(path) });
        }

        public async Task ListenBoot(Profile p, string port, CancellationToken token)
        {
            PortValid(port);
            Stage("awaiting_boot", "Escuchando 74880: mantené KEY, pulsá/soltá RST ESP, soltá KEY. Mega RESET→GND.");
            int seconds = (int)p.Number("BootWaitSeconds");
            if (seconds < 5 || seconds > 600) throw new Exception("Espera KEY/RST debe estar entre 5 y 600 segundos.");
            await Task.Run(() =>
            {
                using (var serial = new SerialPort(port, 74880, Parity.None, 8, StopBits.One))
                {
                    serial.DtrEnable = false; serial.RtsEnable = false; serial.Handshake = Handshake.None;
                    serial.ReadTimeout = 200; serial.Encoding = Encoding.ASCII;
                    serial.Open();
                    log("Monitor abierto. Pulsá los botones AHORA; el programa no los controla eléctricamente.");
                    var clock = Stopwatch.StartNew(); var observer = new BootObserver(); bool sawNormal = false;
                    while (clock.Elapsed.TotalSeconds < seconds)
                    {
                        token.ThrowIfCancellationRequested();
                        string chunk = serial.ReadExisting();
                        if (chunk.Length > 0)
                        {
                            log("[UART] " + chunk.Replace("\0", ""));
                            bool programming = false;
                            foreach (BootObservation observation in observer.Push(chunk))
                            {
                                log(observation.Message);
                                if (Changed != null) Changed(observation.Code, observation.Message);
                                if (observation.Mode == 3) { sawNormal = true; log("Repetí KEY mantenido + pulsar/soltar RST del ESP."); }
                                if (observation.Code == "reset_observed") programming = false;
                                else programming = observation.Mode == 1;
                            }
                            if (programming) return;
                        }
                        Thread.Sleep(50);
                    }
                    throw new TimeoutException(sawNormal ? "Se vio arranque normal, pero no modo 1. Revisá KEY/GPIO0 y el botón RST del ESP." : "Sin mensaje de modo programación. Revisá ESP TX→D1, alimentación, reset del ESP y puerto.");
                }
            }, token);
            Stage("bootloader_observed", "Modo programación observado; monitor cerrado y COM liberado.");
        }

        List<string> EspArgs(Profile p, string port, bool noStub)
        {
            PortValid(port); NeedFile(p.Resolve("Esptool"));
            Choice(p.Get("EsptoolMajor"), "4", "5");
            int baud = (int)p.Number("EspBaud"); if (baud < 9600 || baud > 921600) throw new Exception("Baudios ESP fuera de rango.");
            var a = new List<string> { p.Resolve("Esptool"), "--chip", "esp8266", "--port", port, "--baud", baud.ToString(),
                "--before", Spell(p, "no_reset"), "--after", Spell(p, "no_reset") };
            if (noStub) a.Add("--no-stub"); return a;
        }
        static string Spell(Profile p, string name) { return p.Get("EsptoolMajor") == "5" ? name.Replace('_', '-') : name; }
        public async Task<long> Probe(Profile p, string port, CancellationToken token)
        {
            Stage("syncing", "Consultando ROM y tamaño real de flash; sin escritura...");
            var args = EspArgs(p, port, true); args.Add(Spell(p, "flash_id"));
            var result = await Command(p, "Python", args, Path.GetDirectoryName(p.Resolve("Esptool")), token, 50);
            Match match = Regex.Match(result.Output, @"Detected flash size:\s*(\d+)\s*(KB|MB)\b", RegexOptions.IgnoreCase);
            if (!match.Success) throw new Exception("esptool respondió, pero no se pudo identificar la capacidad. No se permite escribir con tamaño desconocido.");
            long bytes = Int64.Parse(match.Groups[1].Value) * (match.Groups[2].Value.ToUpperInvariant() == "MB" ? 1048576L : 1024L);
            log("Comunicación bidireccional confirmada; flash detectada: " + bytes + " bytes.");
            Steps.Add(new { port = port, detectedFlashBytes = bytes, bidirectional = true });
            Stage("rom_connected", "ROM responde. Flash: " + bytes + " bytes. Esto no comprueba el firmware de aplicación.");
            return bytes;
        }

        static string[] EspChecks(bool fs)
        {
            var list = new List<string> {
                "Potencia de motores desconectada y monitor serie externo cerrado.",
                "RESET del ATmega unido a GND; no es el RST del ESP.",
                "D0/RX0 del Mega → E_RX CENTRAL del selector; E_TX CENTRAL → D1/TX0.",
                "Cables D18/D19 retirados; sin jumpers paralelos ni conexión directa ESP UART a 5 V.",
                "ESP alimentado por el shield; masa común. KEY y RST identificados."
            };
            if (fs) list.Add("LittleFS será reemplazado, incluidas preferencias Wi-Fi. Respaldé los datos que necesitaba.");
            return list.ToArray();
        }
        public bool PrepareEsp(bool fs)
        { return confirm("Preparar conexión ESP", "Mismo USB-B, modo de servicio distinto al de operación. Al continuar NO se escribe todavía.", EspChecks(fs)); }

        void Layout(Profile p, long detected, bool fs)
        {
            long expected = p.Number("FlashMB") * 1048576L;
            if (expected <= 0 || expected != detected) throw new Exception("Flash detectada " + detected + " != perfil " + expected + ". Ajustá perfil/layout y recompilá. No se escribió nada.");
            long start = p.Number("FsOffset"), length = p.Number("FsBytes"), fw = p.Number("FirmwareOffset"), max = p.Number("FirmwareMax");
            if (start < 0 || length <= 0 || fw < 0 || max <= 0 || start % 4096 != 0 || length % 4096 != 0 || start >= detected || length > detected - start)
                throw new Exception("Rangos de flash inválidos en el perfil.");
            if (fw >= start || max > start - fw) throw new Exception("El rango de firmware invade LittleFS.");
            NeedFile(p.Resolve("Linker")); string ld = File.ReadAllText(p.Resolve("Linker"));
            Match a = Regex.Match(ld, @"_FS_start\s*=\s*(0x[0-9a-fA-F]+)"), b = Regex.Match(ld, @"_FS_end\s*=\s*(0x[0-9a-fA-F]+)");
            if (!a.Success || !b.Success) throw new Exception("El linker no declara _FS_start/_FS_end en formato reconocido.");
            long ldStart = Convert.ToInt64(a.Groups[1].Value.Substring(2), 16) - 0x40200000L;
            long ldEnd = Convert.ToInt64(b.Groups[1].Value.Substring(2), 16) - 0x40200000L;
            if (start != ldStart || length != ldEnd - ldStart) throw new Exception("El perfil no coincide con el linker elegido.");
            string iniPath = Path.Combine(p.Resolve("EspDir"), "platformio.ini");
            if (File.Exists(iniPath))
            {
                string ini = File.ReadAllText(iniPath);
                Match environment = Regex.Match(ini, @"(?ms)^\[env:" + Regex.Escape(p.Get("PioEnv")) + @"\]\s*\r?\n(.*?)(?=^\[|\z)");
                if (!environment.Success) throw new Exception("No se encontró el entorno configurado en platformio.ini.");
                Match linker = Regex.Match(environment.Groups[1].Value, @"(?m)^\s*board_build\.ldscript\s*=\s*([^;\r\n]+)");
                if (!linker.Success || Path.GetFileName(linker.Groups[1].Value.Trim()) != Path.GetFileName(p.Resolve("Linker")))
                    throw new Exception("El linker del entorno PlatformIO no coincide con el seleccionado. No se escribe.");
            }
            Choice(p.Get("FlashMode"), "keep", "dio", "dout", "qio", "qout");
            Choice(p.Get("FlashFreq"), "keep", "20m", "26m", "40m", "80m");
            string bin = p.Resolve(fs ? "FsBin" : "EspBin"); NeedFile(bin);
            long bytes = new FileInfo(bin).Length;
            if (fs ? bytes != length : bytes <= 0 || bytes > max) throw new Exception("El tamaño del BIN no coincide con el rango permitido del perfil.");
            if (!fs && File.ReadAllBytes(bin)[0] != 0xe9) throw new Exception("Se esperaba firmware ESP8266 combinado de Arduino con cabecera 0xE9.");
        }
        void AddFlashOptions(Profile p, List<string> args)
        {
            args.AddRange(new[] { Spell(p, "--flash_mode"), p.Get("FlashMode"), Spell(p, "--flash_freq"), p.Get("FlashFreq"),
                Spell(p, "--flash_size"), p.Get("FlashMB") + "MB" });
        }
        public async Task FlashEsp(Profile p, string port, bool firmware, bool filesystem, bool listen, CancellationToken token)
        {
            if (!PrepareEsp(filesystem)) throw new OperationCanceledException("Cableado no confirmado.");
            if (listen) await ListenBoot(p, port, token);
            long size = await Probe(p, port, token);
            if (firmware) Layout(p, size, false);
            if (filesystem) Layout(p, size, true);
            var files = new List<Tuple<string, string>>();
            if (firmware) files.Add(Tuple.Create(p.Get("FirmwareOffset"), p.Resolve("EspBin")));
            if (filesystem) files.Add(Tuple.Create(p.Get("FsOffset"), p.Resolve("FsBin")));
            string details = "Puerto " + port + " | flash real " + size + " bytes\r\nSPI " + p.Get("FlashMode") + " / " + p.Get("FlashFreq") + "\r\n\r\n";
            foreach (var file in files) details += "Offset " + file.Item1 + "\r\n" + Describe(file.Item2) + "\r\n\r\n";
            var hashes = files.Select(f => Hash(f.Item2)).ToArray();
            if (!confirm("Continuar para flashear ESP", details + "La ROM respondió. Esto autoriza ESCRIBIR y después VERIFICAR los archivos indicados.",
                new[] { "Revisé puerto, archivos, offsets y cableado; mantener RESET del Mega a GND.", "Si se pierde alimentación o se cancela, el contenido puede quedar incompleto." }))
                throw new OperationCanceledException("Carga no confirmada.");
            token.ThrowIfCancellationRequested();
            for (int i = 0; i < files.Count; i++) EnsureUnchanged(files[i].Item2, hashes[i]);
            var write = EspArgs(p, port, false); write.Add(Spell(p, "write_flash")); AddFlashOptions(p, write);
            foreach (var file in files) write.AddRange(new[] { file.Item1, file.Item2 });
            Stage("writing", "Escribiendo ESP. No desconectar ni quitar RESET-GND del Mega.");
            MarkWriting(); // Once launched, a failed write may already have modified flash.
            await Command(p, "Python", write, Path.GetDirectoryName(p.Resolve("Esptool")), token);
            Stage("verifying", "Escritura terminada; verificando automáticamente contra los archivos...");
            for (int i = 0; i < files.Count; i++) EnsureUnchanged(files[i].Item2, hashes[i]);
            var verify = EspArgs(p, port, false); verify.Add(Spell(p, "verify_flash")); AddFlashOptions(p, verify);
            foreach (var file in files) verify.AddRange(new[] { file.Item1, file.Item2 });
            await Command(p, "Python", verify, Path.GetDirectoryName(p.Resolve("Esptool")), token);
            Verified = true;
            Stage("verified", "ESP: carga y verificación de contenido completadas correctamente.");
            log("ESP: escritura y verificación de contenido completadas. No demuestra ejecución, Wi-Fi ni funcionamiento del robot.");
            log("Para operar: apagar, quitar RESET Mega→GND, retirar D0/D1, restaurar D18→E_RX y E_TX→D19. KEY libre; resetear ESP.");
        }
        public async Task FlashMega(Profile p, string port, CancellationToken token)
        {
            PortValid(port); string hex = p.Resolve("Hex"); NeedFile(hex); NeedFile(p.Resolve("AvrdudeConfig"));
            if (!Regex.IsMatch(p.Get("Signature"), @"^0x[0-9a-fA-F]{6}$")) throw new Exception("Firma AVR esperada inválida; ejemplo 0x1e9801.");
            string hash = Hash(hex);
            if (!confirm("Preparar y cargar ATmega", "Puerto " + port + "\r\n" + Describe(hex) + "\r\n\r\nSe grabará aplicación y se verificará mediante avrdude. No cambia fuses ni EEPROM.", new[] {
                "Potencia de motores desconectada y monitor serie cerrado.",
                "QUITÉ el puente RESET del Mega a GND. RESET queda libre para auto-reset.",
                "ESP aislado de D0/D1; retiré los cables de servicio y todos los jumpers que los unan.",
                "D18→E_RX y E_TX→D19 pueden permanecer para operación. No pulsar KEY/RST del shield.",
                "Seleccioné el HEX de producto completo; no el de comunicación sola."
            })) throw new OperationCanceledException("Cableado no confirmado.");
            EnsureUnchanged(hex, hash); token.ThrowIfCancellationRequested();
            Stage("writing", "Cargando ATmega por su bootloader; RESET debe estar libre. Auto-reset de bootloader solicitado por avrdude; no es un reinicio observado por UART."); MarkWriting();
            var result = await Command(p, "Avrdude", new[] { "-C", p.Resolve("AvrdudeConfig"), "-v", "-p", p.Get("Mcu"), "-c", p.Get("Programmer"),
                "-P", port, "-b", p.Get("MegaBaud"), "-D", "-U", "flash:w:" + Path.GetFileName(hex) + ":i" }, Path.GetDirectoryName(hex), token);
            if (result.Output.IndexOf(p.Get("Signature"), StringComparison.OrdinalIgnoreCase) < 0 || !Regex.IsMatch(result.Output, @"\d+\s+bytes of flash verified", RegexOptions.IgnoreCase))
                throw new CommandFailure("avrdude terminó sin error, pero no se reconoció firma/verificación. Revisá el log; no se declara éxito verificado.", p.Resolve("Avrdude"), result.ExitCode, result.Output);
            EnsureUnchanged(hex, hash);
            Verified = true;
            Stage("verified", "ATmega: firma esperada, carga y bytes de flash verificados correctamente.");
            log("ATmega: firma esperada, aplicación escrita y bytes verificados. No demuestra sensores/motores calibrados.");
            log("Si Hall conectado estaba en 0: Calibracion > Geometria > Hall conectado=1; volver y Guardar EEPROM.");
        }
    }
}
