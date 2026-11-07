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
    public sealed partial class Operations
    {
        readonly Runner runner;
        readonly Action<string> log, state;
        readonly Func<string, string, string[], bool> confirm;
        public bool WroteAnything { get; private set; }
        public bool Verified { get; private set; }
        public bool EspSessionStarted { get; private set; } // Operation attempted; not a measurement of KEY.
        public bool KeyRstAutomatic { get; private set; } // AFR2-20261106-11: flasher ATmega acciona KEY/RST; sin intervención física.
        bool keyReleaseAnnounced;
        public Action WritingStarted;
        public Action<string, string> Changed;
        public Func<bool, Func<CancellationToken, Task>, CancellationToken, Task> BootAssistant;
        public Func<string, string, CancellationToken, bool> EspPhysicalConfirmation;
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

        public async Task<CommandResult> Command(Profile p, string key, IEnumerable<string> args, string dir, CancellationToken token, int timeout = 0, IDictionary<string, string> environment = null)
        {
            var watch = Stopwatch.StartNew();
            int verificationNotice = 0, connectionNotice = 0;
            var commandArgs = args.ToArray();
            bool megaVerifyOnly = key == "Avrdude" && commandArgs.Any(arg => arg.StartsWith("flash:v:", StringComparison.Ordinal));
            bool espWrite = key == "Python" && (commandArgs.Contains("write_flash") || commandArgs.Contains("write-flash"));
            runner.OutputLine = line => {
                if (espWrite && line.IndexOf("Hash of data verified", StringComparison.OrdinalIgnoreCase) >= 0 && Interlocked.Exchange(ref verificationNotice, 1) == 0)
                    if (Changed != null) Changed("verifying", "Esptool verifica el hash en la misma sesión de escritura.");
                if (espWrite && !keyReleaseAnnounced && !AutoKeyRst(p) && CommandFeedback.WritingPercent(line).HasValue) {
                    keyReleaseAnnounced = true;
                    if (Changed != null) Changed("esp_key_release_ready", "Primer porcentaje de escritura ESP informado: ya podés soltar KEY. No pulsar RST ni desconectar.");
                }
                // AFR2-20261106-18: actual boot evidence from the shared COM session.
                if (key == "Python" && line.Trim() == "AFR2-AUTO: boot_mode=1")
                    if (Changed != null) Changed("bootloader_observed", "Boot mode 1 capturado en la sesión automática actual; falta respuesta ROM.");
                if (key == "Avrdude")
                {
                    Match signature = Regex.Match(line, @"Device signature\s*=\s*(0x[0-9a-fA-F]{6})", RegexOptions.IgnoreCase);
                    if (signature.Success && signature.Groups[1].Value.Equals(p.Get("Signature"), StringComparison.OrdinalIgnoreCase) && Interlocked.Exchange(ref connectionNotice, 1) == 0)
                        if (Changed != null) Changed("device_connected", "ATmega: avrdude recibió la firma esperada " + signature.Groups[1].Value + ".");
                }
                if (key == "Avrdude" && line.IndexOf("verifying flash memory against", StringComparison.OrdinalIgnoreCase) >= 0 && Interlocked.Exchange(ref verificationNotice, 1) == 0)
                    if (Changed != null) Changed(megaVerifyOnly ? "checking_flasher" : "verifying", "ATmega: avrdude inició la verificación de flash.");
            };
            CommandResult result;
            try { result = await runner.Run(p.Resolve(key), args, dir, p, token, timeout, environment); }
            finally { runner.OutputLine = null; }
            Steps.Add(new { tool = p.Resolve(key), arguments = args.ToArray(), directory = dir, exitCode = result.ExitCode, elapsedSeconds = watch.Elapsed.TotalSeconds });
            if (result.ExitCode != 0)
            {
                string hint = "Revisá la salida completa en el registro.";
                if (result.Output.IndexOf("No serial data received", StringComparison.OrdinalIgnoreCase) >= 0)
                    hint = UsesMegaBridge(p) ? "No hubo respuesta. Revisá flasher instalado, baud fijo igual a esptool, RESET Mega LIBRE, D18→E_RX y E_TX→D19 y KEY/RST del ESP. No pasar a RESET-GND dentro de este modo."
                        : "No hubo respuesta de esptool. boot mode:(1,...) confirma ESP→PC, pero no PC→ESP. Revisá TX/RX cruzados del adaptador USB–TTL, GND común y KEY/RST.";
                if (result.Output.Contains("AFR2-AUTO: boot_mode=1") && Regex.IsMatch(result.Output, "No serial data received|Failed to connect|Timed out waiting for packet", RegexOptions.IgnoreCase))
                    hint = "KEY/RST automático y boot UART ya confirmados en esta sesión, pero la ROM no respondió. Revisá la ruta PC→ESP (D18/TX1→E_RX), baud fijo y emisores paralelos. No se acredita comunicación bidireccional.";
                else if (Regex.IsMatch(result.Output, "Access is denied|PermissionError|UnauthorizedAccess|cannot open port", RegexOptions.IgnoreCase))
                    hint = "Revisá monitor serie abierto, permisos de herramientas/cachés y puerto. La aplicación no cambia permisos ni eleva privilegios automáticamente.";
                throw new CommandFailure("El comando terminó con código " + result.ExitCode + ". " + hint, p.Resolve(key), result.ExitCode, result.Output);
            }
            return result;
        }

        public async Task BuildMega(Profile p, CancellationToken token)
        {
            string preparationInputs = BuildPreparation.Begin(p, "mega");
            Stage("building_mega", "Compilando ATmega: producto, comunicación y biblioteca...");
            foreach (string key in new[] { "Gcc", "Ar", "Objcopy" }) NeedFile(p.Resolve(key));
            string dir = p.Resolve("AtmegaDir"); NeedFile(Path.Combine(dir, "Makefile"));
            await Command(p, "Make", new[] { "-B", "firmware", "communication-firmware", "library",
                "CC=\"" + Slash(p.Resolve("Gcc")) + "\"", "AR=\"" + Slash(p.Resolve("Ar")) + "\"",
                "OBJCOPY=\"" + Slash(p.Resolve("Objcopy")) + "\"" }, dir, token);
            Artifact(Path.Combine(dir, "build", "afr2.hex"));
            Artifact(Path.Combine(dir, "build", "afr2-communication.hex"));
            Artifact(Path.Combine(dir, "libafr2_ui.a"));
            BuildPreparation.Record(p, "mega", new[] { Path.Combine(dir, "build", "afr2.hex"), Path.Combine(dir, "build", "afr2-communication.hex") }, preparationInputs);
            Stage("build_mega_completed", "Compilación ATmega completada: producto, comunicación y biblioteca.");
        }
        public async Task BuildEsp(Profile p, CancellationToken token)
        {
            if (EspFlashRules.IsAt(p)) { ReviewAtPackage(p); return; }
            string preparationInputs = BuildPreparation.Begin(p, "esp");
            Stage("building_esp", "Compilando firmware ESP..."); NeedFile(Path.Combine(p.Resolve("EspDir"), "platformio.ini"));
            var arguments = WebBuildArguments(p, false);
            string buildLayoutIdentity = p.Get("WebLayoutPreset") == "custom" ? null : WebLayoutIdentity(p);
            await Command(p, "Pio", arguments, p.Resolve("EspDir"), token);
            string image = Path.Combine(p.Resolve("EspDir"), ".pio", "build", p.Get("PioEnv"), "firmware.bin");
            Artifact(image); SaveWebReceipt(p, image, buildLayoutIdentity);
            BuildPreparation.Record(p, "esp", new[] { image }, preparationInputs);
            Stage("build_esp_completed", "Compilación del firmware ESP completada.");
        }
        public async Task BuildWebFs(Profile p, CancellationToken token)
        {
            if (!EspFlashRules.UseFilesystem(p)) throw new ArgumentException("El perfil tiene filesystem desactivado. Elegí Firmware o activá un layout con filesystem.");
            if (EspFlashRules.IsAt(p)) { ReviewAtPackage(p); return; }
            string preparationInputs = BuildPreparation.Begin(p, "filesystem");
            string expectedImage = Path.GetFullPath(Path.Combine(p.Resolve("EspDir"), ".pio", "build", p.Get("PioEnv"), "littlefs.bin"));
            if (!String.Equals(Path.GetFullPath(p.Resolve("FsBin")), expectedImage, StringComparison.OrdinalIgnoreCase))
                throw new ArgumentException("Imagen FileSystem debe apuntar al littlefs.bin del entorno PlatformIO seleccionado.");
            var arguments = WebBuildArguments(p, true);
            string buildLayoutIdentity = p.Get("WebLayoutPreset") == "custom" ? null : WebLayoutIdentity(p);
            await PrepareFilesystemFiles(p, token);
            Stage("building_web_fs", "Generando imagen LittleFS desde ESP/data...");
            await Command(p, "Pio", arguments, p.Resolve("EspDir"), token);
            string image = Path.Combine(p.Resolve("EspDir"), ".pio", "build", p.Get("PioEnv"), "littlefs.bin");
            if (new FileInfo(image).Length != p.Number("FsBytes")) throw new Exception("Imagen creada, pero su tamaño no coincide con el perfil. Revisá linker, entorno y FsBytes antes de cargar.");
            Artifact(image); SaveWebReceipt(p, image, buildLayoutIdentity);
            BuildPreparation.Record(p, "filesystem", new[] { image }, preparationInputs);
            Stage("build_web_fs_completed", "Script de preparación e imagen LittleFS completados.");
        }
        void Artifact(string path)
        {
            log("Artefacto generado: " + Describe(path));
            Steps.Add(new { artifact = path, bytes = new FileInfo(path).Length, sha256 = Hash(path) });
        }

        public async Task ListenBoot(Profile p, string port, CancellationToken token, bool forVerification = false)
        {
            EspSessionStarted = true;
            if (BootAssistant == null) throw new InvalidOperationException("Falta el asistente obligatorio de intervención física ESP.");
            await BootAssistant(forVerification, t => ListenBootCore(p, port, t, forVerification), token);
            Stage("bootloader_observed", "Modo programación observado; monitor cerrado y COM liberado. Seguí manteniendo KEY hasta el aviso del Loader o el fin de la operación.");
        }
        async Task ListenBootCore(Profile p, string port, CancellationToken token, bool forVerification)
        {
            PortValid(port);
            Stage("awaiting_boot", "Abriendo escucha 74880 para KEY / RST. Esperá el aviso PUERTO LISTO.");
            int seconds = (int)p.Number("BootWaitSeconds");
            if (seconds < 5 || seconds > 600) throw new Exception("Espera KEY/RST debe estar entre 5 y 600 segundos.");
            await Task.Run(() =>
            {
                using (var serial = new SerialPort(port, 74880, Parity.None, 8, StopBits.One))
                {
                    serial.DtrEnable = false; serial.RtsEnable = false; serial.Handshake = Handshake.None;
                    serial.ReadTimeout = 200; serial.Encoding = Encoding.ASCII;
                    serial.Open();
                    token.ThrowIfCancellationRequested();
                    string ready = "PUERTO LISTO: mantené KEY, apretá RST del ESP y soltá solo RST. NO SUELTES KEY hasta " +
                        (forVerification ? "terminar la verificación. No se volverá a escribir la flash." : "terminar la operación; en una carga, incluye escritura y verificación.");
                    log(ready); if (Changed != null) Changed("boot_listener_ready", ready);
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
                    throw new TimeoutException(sawNormal ? "Se vio arranque normal, pero no modo 1. Revisá KEY/GPIO0 y el botón RST del ESP." : "Sin mensaje de modo programación. Revisá ESP TX→RX del adaptador (D1 si usás el puente Mega), alimentación, reset del ESP y puerto.");
                }
            }, token);
        }

        List<string> EspArgs(Profile p, string port, bool noStub)
        {
            PortValid(port); NeedFile(p.Resolve("Esptool"));
            Choice(p.Get("EsptoolMajor"), "4", "5");
            int baud = (int)p.Number("EspBaud"); if (baud < 9600 || baud > 921600) throw new Exception("Baudios ESP fuera de rango.");
            var a = new List<string> { "-u" };
            if (UsesMegaBridge(p))
            {
                CheckBridge(p); NeedFile(BridgeToolPath);
                Steps.Add(new { bridgeLauncher = BridgeToolPath, sha256 = Hash(BridgeToolPath), startupSeconds = p.Get("BridgeStartupSeconds") });
                a.AddRange(new[] { BridgeToolPath, "--esptool", p.Resolve("Esptool"), "--bridge-baud", p.Get("BridgeBaud"), "--startup-seconds", p.Get("BridgeStartupSeconds") });
                if (AutoKeyRst(p)) { KeyRstAutomatic = true; a.Add("--auto-key-rst"); Steps.Add(new { keyRst = "auto", pins = "D32=KEY, D33=RST", note = "transistores; sin orden previa de pines si el flasher AFR2 no responde" }); }
                a.Add("--");
            }
            else a.Add(p.Resolve("Esptool"));
            a.AddRange(new[] { "--chip", "esp8266", "--port", port, "--baud", baud.ToString(),
                "--before", Spell(p, "no_reset"), "--after", Spell(p, "no_reset") });
            if (UsesMegaBridge(p)) a.AddRange(new[] { "--connect-attempts", "30" });
            if (noStub) a.Add("--no-stub"); return a;
        }
        static string Spell(Profile p, string name) { return p.Get("EsptoolMajor") == "5" ? name.Replace('_', '-') : name; }
        public async Task<long> Probe(Profile p, string port, CancellationToken token)
        {
            EspSessionStarted = true;
            Stage("syncing", AutoKeyRst(p) ? "Consultando ROM y tamaño real de flash; sin escritura. KEY/RST automáticos..." : "Consultando ROM y tamaño real de flash; sin escritura. Mantené KEY apretado...");
            var args = EspArgs(p, port, true); args.Add(Spell(p, "flash_id"));
            Steps.Add(new { espTransport = p.Get("EspTransport"), baud = p.Get("EspBaud"), bridgeWait = UsesMegaBridge(p) ? p.Get("BridgeStartupSeconds") : null });
            var result = await Command(p, "Python", args, Path.GetDirectoryName(p.Resolve("Esptool")), token, UsesMegaBridge(p) ? 120 : 50);
            Match match = Regex.Match(result.Output, @"Detected flash size:\s*(\d+)\s*(KB|MB)\b", RegexOptions.IgnoreCase);
            if (!match.Success) throw new Exception("esptool respondió, pero no se pudo identificar la capacidad. No se permite escribir con tamaño desconocido.");
            long bytes = Int64.Parse(match.Groups[1].Value) * (match.Groups[2].Value.ToUpperInvariant() == "MB" ? 1048576L : 1024L);
            log("Comunicación bidireccional confirmada; flash detectada: " + bytes + " bytes.");
            Steps.Add(new { port = port, detectedFlashBytes = bytes, bidirectional = true });
            Stage("rom_connected", "ROM responde. Flash: " + bytes + " bytes. Esto no comprueba el firmware de aplicación.");
            return bytes;
        }

        void RequireIntegratedEspVerification(CommandResult result, int images)
        {
            int confirmed = Regex.Matches(result.Output, @"(?im)^\s*Hash of data verified[.!]?\s*$").Count;
            if (confirmed < images) throw new CommandFailure("La escritura terminó sin acreditar un hash verificado por cada imagen. No se repite la escritura ni se abre otra sesión de verificación.", "esptool", result.ExitCode, result.Output);
            Steps.Add(new { verification = "esptool_integrated_hash", images = images, confirmed = confirmed });
        }

        static string[] EspChecks(Profile p, bool fs)
        {
            var list = new List<string> {
                "Potencia de motores desconectada y monitor serie externo cerrado."
            };
            if (p.Get("EspTransport") == "usb-ttl") list.AddRange(new[] {
                "Adaptador USB–TTL; TX hacia RX ESP, RX desde TX ESP y GND común, por la ruta identificada del shield.",
                "Sin enlaces UART del Mega ni otro emisor conectados en paralelo al ESP.",
                "Shield alimentado desde el Mega; VCC del adaptador sin conectar."
            });
            list.Add("ESP alimentado por el shield y masa común. Identifiqué KEY y RST; mantendré KEY hasta el aviso del Loader o el fin de la operación.");
            if (fs) list.Add("El filesystem elegido será reemplazado, incluidas sus preferencias. Respaldé los datos que necesitaba.");
            return list.ToArray();
        }
        public bool PrepareEsp(Profile p, bool fs)
        {
            Profile.ValidateValue("EspTransport", p.Get("EspTransport"));
            if (UsesMegaBridge(p))
            {
                CheckBridge(p);
                var checks = new List<string> { "Potencia de actuadores desconectada y monitor externo cerrado.", "RESET Mega LIBRE; el Mega debe ejecutar el flasher, sin puente RESET-GND.",
                    "D18/TX1→E_RX y E_TX→D19/RX1. Sin cables D0/D1 al ESP ni jumpers paralelos.", "Alimentación por shield y masa común." };
                if (fs) checks.Add("El filesystem se reemplazará; conservé los datos que necesitaba.");
                if (AutoKeyRst(p)) checks.AddRange(new[] { "Circuito KEY/RST armado: D32→1 kΩ→base Q1 y D33→1 kΩ→base Q2, 10 kΩ de cada base a GND, colectores a las patas de señal de KEY y RST del shield, emisores a GND.", "No apretaré KEY ni RST a mano: el flasher AFR2 los acciona. Si el Mega no responde como flasher AFR2, el Loader no envía órdenes y se detiene." });
                else checks.Add("Mantendré KEY del ESP hasta el aviso del Loader o el fin de la consulta; solo RST queda suelto.");
                bool bridgeAccepted = confirm("ESP por flasher ATmega — RESET libre", "Transporte: puente software por USART1, fijo a " + p.Get("BridgeBaud") + ".\r\nPrimero cargar el flasher en ATmega; después consulta ROM." + (AutoKeyRst(p) ? "\r\nKEY/RST automáticos: no tenés que apretar nada ni soltar KEY; el flasher lo hace." : "\r\nMantener KEY hasta el aviso del Loader o el fin de la operación.") + " No se mide cableado ni se ejecuta todavía escritura ESP.", checks.ToArray());
                if (bridgeAccepted) Stage("wiring_confirmed", "ESP por puente software: cableado confirmado; RESET Mega libre.");
                return bridgeAccepted;
            }
            string connection = "Adaptador USB–TTL externo: elegí su COM, distinto del USB del Mega que alimenta el shield.\r\nTX/RX cruzados por función y GND; VCC del adaptador sin conectar. Aislar el enlace UART del Mega.";
            bool accepted = confirm("Preparar conexión ESP", connection + "\r\n\r\nCon Guiar KEY / RST activado, esperá PUERTO LISTO antes de pulsar los botones.\r\nMantené KEY, pulsá y soltá solo RST del ESP; conservá KEY hasta el aviso del Loader o el fin de la consulta. Cerrar la pantalla de botones no indica soltar KEY.\r\nTodavía no se escribe firmware.", EspChecks(p, fs));
            if (accepted) Stage("wiring_confirmed", "Preparación y cableado ESP confirmados por el usuario; no medidos por el programa.");
            return accepted;
        }

        void Layout(Profile p, long detected, bool fs)
        {
            long expected = p.Number("FlashMB") * 1048576L;
            if (expected <= 0 || expected != detected) throw new Exception("Flash detectada " + detected + " != perfil " + expected + ". Ajustá perfil/layout y recompilá. No se escribió nada.");
            long start = p.Number("FsOffset"), length = p.Number("FsBytes"), fw = p.Number("FirmwareOffset"), max = p.Number("FirmwareMax");
            if (start < 0 || length < 0 || (fs && length == 0) || fw != 0 || max <= 0 || start % 4096 != 0 || length % 4096 != 0 || start >= detected || length > detected - start)
                throw new Exception("Rangos de flash inválidos en el perfil.");
            if (fw >= start || max > start - fw) throw new Exception("El rango de firmware invade LittleFS.");
            NeedFile(p.Resolve("Linker")); string ld = File.ReadAllText(p.Resolve("Linker"));
            var actualLayout = WebFlashLayout.Read(p.Resolve("Linker"), detected);
            if (max > actualLayout.FirmwareMax || EspFlashRules.UseFilesystem(p) != (length > 0)) throw new Exception("El límite de código o la selección filesystem no coincide con el linker.");
            Match a = Regex.Match(ld, @"_FS_start\s*=\s*(0x[0-9a-fA-F]+)"), b = Regex.Match(ld, @"_FS_end\s*=\s*(0x[0-9a-fA-F]+)");
            if (!a.Success || !b.Success) throw new Exception("El linker no declara _FS_start/_FS_end en formato reconocido.");
            long ldStart = Convert.ToInt64(a.Groups[1].Value.Substring(2), 16) - 0x40200000L;
            long ldEnd = Convert.ToInt64(b.Groups[1].Value.Substring(2), 16) - 0x40200000L;
            if (start != ldStart || length != ldEnd - ldStart) throw new Exception("El perfil no coincide con el linker elegido.");
            string iniPath = Path.Combine(p.Resolve("EspDir"), "platformio.ini");
            if (p.Get("WebLayoutPreset") == "custom" && File.Exists(iniPath))
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
            RequireWebReceipt(p, bin);
        }
        void AddFlashOptions(Profile p, List<string> args)
        {
            args.AddRange(new[] { Spell(p, "--flash_mode"), p.Get("FlashMode"), Spell(p, "--flash_freq"), p.Get("FlashFreq"),
                Spell(p, "--flash_size"), p.Number("FlashMB").ToString(System.Globalization.CultureInfo.InvariantCulture) + "MB" });
        }
        public async Task FlashEsp(Profile p, string port, bool firmware, bool filesystem, bool listen, CancellationToken token)
        {
            if (filesystem && !EspFlashRules.UseFilesystem(p)) throw new ArgumentException("Filesystem desactivado en el perfil. Elegí Firmware o activá filesystem.");
            if (EspFlashRules.IsAt(p)) { await FlashAtPackage(p, port, firmware, filesystem, listen, token); return; }
            if (firmware) BuildPreparation.Require(p, "esp", p.Resolve("EspBin"));
            if (filesystem) BuildPreparation.Require(p, "filesystem", p.Resolve("FsBin"));
            await PrepareEspSession(p, port, filesystem, listen, token);
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
                new[] { UsesMegaBridge(p) ? "Revisé puerto, archivos, offsets y enlace D18/D19; mantener RESET Mega LIBRE." : p.Get("EspTransport") == "usb-ttl" ? "Revisé COM del USB–TTL, lógica 3,3 V, TX/RX cruzados y GND; enlace UART Mega aislado." : "Revisé puerto, archivos y offsets.", AutoKeyRst(p) ? "KEY/RST automáticos en cada sesión: el flasher ejecuta la entrada a programación y confirma su estado; no toco los botones." : "Mantengo KEY del ESP hasta el aviso del primer porcentaje de escritura; solo RST queda suelto.", "Si se pierde alimentación o se cancela, el contenido puede quedar incompleto." }))
                throw new OperationCanceledException("Carga no confirmada.");
            token.ThrowIfCancellationRequested();
            for (int i = 0; i < files.Count; i++) EnsureUnchanged(files[i].Item2, hashes[i]);
            var write = EspArgs(p, port, false); write.Add(Spell(p, "write_flash")); AddFlashOptions(p, write);
            foreach (var file in files) write.AddRange(new[] { file.Item1, file.Item2 });
            Stage("writing", AutoKeyRst(p) ? "Escribiendo ESP por flasher ATmega con entrada automática; RESET Mega LIBRE." : UsesMegaBridge(p) ? "Escribiendo ESP por flasher ATmega. KEY mantenido; RESET Mega LIBRE." : p.Get("EspTransport") == "usb-ttl" ? "Escribiendo ESP por USB–TTL. Mantené KEY y no desconectes." : "Escribiendo ESP. KEY mantenido.");
            MarkWriting(); // Once launched, a failed write may already have modified flash.
            var written = await Command(p, "Python", write, Path.GetDirectoryName(p.Resolve("Esptool")), token);
            for (int i = 0; i < files.Count; i++) EnsureUnchanged(files[i].Item2, hashes[i]);
            RequireIntegratedEspVerification(written, files.Count);
            Verified = true;
            Stage("verified", "ESP: carga y verificación de contenido completadas correctamente.");
            log("ESP: escritura y verificación de contenido completadas. No demuestra ejecución, Wi-Fi ni funcionamiento del robot.");
            log(EspRestoration(p));
            if (UsesMegaBridge(p)) CompletionMessage = "ESP escrito y verificado. El Mega sigue ejecutando el flasher temporal: usá Flashear Firmware ATmega o el menú Flashear último backup antes de operar.";
        }
        public async Task FlashMega(Profile p, string port, CancellationToken token)
        {
            BuildPreparation.Require(p, "mega", p.Resolve("Hex"));
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
            Stage("wiring_confirmed", "Cableado ATmega confirmado por el usuario.");
            EnsureUnchanged(hex, hash); token.ThrowIfCancellationRequested();
            MegaServiceRecord service = MegaServiceRecord.Read(port);
            if (service != null) { service.Status = "product_write_started"; service.Save(); }
            await UploadMegaApplication(p, port, hex, token);
            EnsureUnchanged(hex, hash);
            Verified = true;
            if (service != null) { service.Status = "product_verified"; service.Save(); }
            MegaFirmwareFlashed = true;
            Stage("verified", "ATmega: firma esperada, carga y bytes de flash verificados correctamente.");
            log("ATmega: firma esperada, aplicación escrita y bytes verificados. No demuestra sensores/motores calibrados.");
            log("Si Hall conectado estaba en 0: Calibracion > Geometria > Hall conectado=1; volver y Guardar EEPROM.");
        }
        public async Task ProbeMega(Profile p, string port, CancellationToken token)
        {
            PortValid(port); NeedFile(p.Resolve("AvrdudeConfig"));
            if (!Regex.IsMatch(p.Get("Signature"), @"^0x[0-9a-fA-F]{6}$")) throw new Exception("Firma AVR esperada inválida.");
            if (!confirm("Detectar ATmega sin escribir", "Puerto " + port + "\r\nSe consulta la firma mediante avrdude, sin -U y con -n (sin escritura).\r\nEl bootloader puede solicitar auto-reset; esto no es un reinicio observado por UART.", new[] {
                "Potencia de motores desconectada y monitor serie externo cerrado.",
                "RESET del ATmega libre (sin puente a GND).",
                "D0/D1 aislados del ESP; USB-B conectado al Mega. No pulsar KEY / RST del shield."
            })) throw new OperationCanceledException("Detección ATmega no confirmada.");
            Stage("wiring_confirmed", "Cableado ATmega confirmado por el usuario.");
            Stage("syncing", "Detectando ATmega: consultando firma por su bootloader, sin escribir...");
            var result = await Command(p, "Avrdude", new[] { "-C", p.Resolve("AvrdudeConfig"), "-v", "-n", "-p", p.Get("Mcu"), "-c", p.Get("Programmer"), "-P", port, "-b", p.Get("MegaBaud") }, Path.GetDirectoryName(p.Resolve("AvrdudeConfig")), token);
            Match signature = Regex.Match(result.Output, @"Device signature\s*=\s*(0x[0-9a-fA-F]{6})", RegexOptions.IgnoreCase);
            if (!signature.Success || !signature.Groups[1].Value.Equals(p.Get("Signature"), StringComparison.OrdinalIgnoreCase))
                throw new CommandFailure("No se reconoció la firma ATmega esperada. Revisá la salida de avrdude.", p.Resolve("Avrdude"), result.ExitCode, result.Output);
            Stage("device_connected", "ATmega detectado: firma " + signature.Groups[1].Value + ". No se escribió ni verificó firmware.");
        }
    }
}
