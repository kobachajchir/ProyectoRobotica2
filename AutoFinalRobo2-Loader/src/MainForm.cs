using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Web.Script.Serialization;
using System.Windows.Forms;

namespace AFR2Loader
{
    public sealed partial class MainForm : Form
    {
        readonly string appDir;
        string profilePath = InitialProfilePath();
        Profile profile;
        readonly ComboBox ports = new ComboBox(), target = new ComboBox();
        readonly Label status = new Label(), lastResult = new Label();
        readonly LogTextBox console = new LogTextBox();
        readonly CheckBox listen = new CheckBox();
        readonly ProgressBar progress = new ProgressBar();
        readonly DataGridView settings = new DataGridView();
        readonly ConcurrentQueue<string> pending = new ConcurrentQueue<string>();
        readonly List<Control> idleControls = new List<Control>();
        readonly object logGate = new object();
        readonly System.Windows.Forms.Timer timer = new System.Windows.Forms.Timer();
        Button cancel, detectTarget, flashTarget;
        ToolStripSplitButton compileTarget;
        CancellationTokenSource cancellation;
        Runner runner;
        StreamWriter logFile;
        string currentLog;
        bool busy;
        readonly ToolStripMenuItem configMenu = new ToolStripMenuItem("Perfiles");

        public MainForm(string appDir, string requestedProfile = null, string requestedEspMode = null)
        {
            this.appDir = Path.GetFullPath(appDir);
            Directory.CreateDirectory(Profile.UserRoot);
            if (!String.IsNullOrEmpty(requestedProfile))
            {
                if (requestedProfile != Path.GetFileName(requestedProfile) || requestedProfile.IndexOfAny(Path.GetInvalidFileNameChars()) >= 0 || !requestedProfile.EndsWith(".json", StringComparison.OrdinalIgnoreCase)) throw new ArgumentException("Seleccioná un nombre de perfil JSON de la biblioteca local.");
                profilePath = Path.Combine(profilesDirectory, requestedProfile);
                if (!File.Exists(profilePath)) throw new FileNotFoundException("No existe el perfil guardado solicitado.", profilePath);
            }
            try { profile = File.Exists(profilePath) ? Profile.FromJson(File.ReadAllText(profilePath)) : Profile.Defaults(this.appDir); }
            catch (Exception error)
            {
                if (!String.IsNullOrEmpty(requestedProfile)) throw new ArgumentException("No se pudo leer el perfil solicitado; no se inicia con otro perfil.", error);
                MessageBox.Show("No se pudo leer el perfil. Se carga uno nuevo sin sobrescribir el anterior hasta Guardar.\r\n" + error.Message, "Perfil", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                profile = Profile.Defaults(this.appDir);
            }
            if (!String.IsNullOrEmpty(requestedEspMode)) { Profile.ValidateValue("EspFirmwareMode", requestedEspMode); profile.Values["EspFirmwareMode"] = requestedEspMode; }
            Text = "AFR2 Loader " + LoaderVersion.Value + " | Compilación y carga del robot"; StartPosition = FormStartPosition.CenterScreen;
            Rectangle screen = Screen.FromPoint(Cursor.Position).WorkingArea;
            MinimumSize = new Size(Math.Min(1000, screen.Width), Math.Min(650, screen.Height));
            Size = new Size(Math.Min(1220, screen.Width), Math.Min(880, screen.Height));
            AutoScaleMode = AutoScaleMode.Dpi; Font = new Font("Segoe UI", 9.5F); BackColor = Color.FromArgb(245, 247, 250);
            var menu = new MenuStrip();
            configMenu.DropDownItems.Add("Importar perfil...", null, (s, e) => ImportProfile());
            configMenu.DropDownItems.Add("Exportar perfil...", null, (s, e) => ExportProfile());
            configMenu.DropDownItems.Add("Guardar perfil actual", null, (s, e) => SaveProfile());
            configMenu.DropDownItems.Add("Guardar perfil como...", null, (s, e) => SaveProfileAs());
            var help = new ToolStripMenuItem("Opciones / ayuda");
            help.DropDownItems.Add("Manual detallado", null, (s, e) => OpenManual());
            help.DropDownItems.Add("Carpeta de registros", null, (s, e) => OpenLogs());
            help.DropDownItems.Add("Acerca de...", null, (s, e) => About());
            menu.Items.Add(configMenu); menu.Items.Add(help); MainMenuStrip = menu;

            var tabs = new TabControl { Dock = DockStyle.Fill, Padding = new Point(20, 8) };
            var operations = new TabPage("Operación") { Padding = new Padding(16) };
            var configuration = new TabPage("Configuración") { Padding = new Padding(12) };
            tabs.TabPages.Add(operations); tabs.TabPages.Add(configuration);
            var layout = new TableLayoutPanel { Dock = DockStyle.Fill, ColumnCount = 1, RowCount = 6 };
            layout.RowStyles.Add(new RowStyle(SizeType.Absolute, 48));
            layout.RowStyles.Add(new RowStyle(SizeType.Absolute, 48));
            layout.RowStyles.Add(new RowStyle(SizeType.Absolute, 82));
            layout.RowStyles.Add(new RowStyle(SizeType.Absolute, 112));
            layout.RowStyles.Add(new RowStyle(SizeType.Percent, 100));
            layout.RowStyles.Add(new RowStyle(SizeType.Absolute, 42));
            operations.Controls.Add(layout);
            layout.Controls.Add(new Label { Dock = DockStyle.Fill, Text = "Prepará los archivos. Confirmá el cableado. Cargá y verificá.", Font = new Font(Font.FontFamily, 17, FontStyle.Bold), ForeColor = Color.FromArgb(23, 39, 51), TextAlign = ContentAlignment.MiddleLeft }, 0, 0);
            var selectors = new FlowLayoutPanel { Dock = DockStyle.Fill, WrapContents = false };
            selectors.Controls.Add(new Label { Text = "Puerto", AutoSize = true, Margin = new Padding(0, 9, 8, 0) });
            ports.Width = 120; ports.DropDownStyle = ComboBoxStyle.DropDownList; ports.Margin = new Padding(0, 4, 8, 0);
            selectors.Controls.Add(ports); idleControls.Add(ports);
            AddButton(selectors, "Actualizar puertos", RefreshPorts, 145);
            selectors.Controls.Add(new Label { Text = "Destino", AutoSize = true, Margin = new Padding(20, 9, 8, 0) });
            target.Width = 280; target.DropDownStyle = ComboBoxStyle.DropDownList; target.Margin = new Padding(0, 4, 8, 0);
            target.Items.AddRange(new object[] { "Firmware ATmega (producto completo)", "Firmware ESP", "Web / LittleFS en ESP", "Firmware ESP + Web / LittleFS" });
            target.SelectedIndex = 0; selectors.Controls.Add(target); idleControls.Add(target);
            detectTarget = AddButton(selectors, "Detectar ATmega", DetectSelected, 165);
            layout.RowStyles[1].Height = 170; layout.Controls.Add(CreateEspOptions(selectors), 0, 1);
            var actions = new FlowLayoutPanel { Dock = DockStyle.Fill, WrapContents = true };
            var compileMenu = new ToolStrip { GripStyle = ToolStripGripStyle.Hidden, Dock = DockStyle.None, AutoSize = true, Margin = new Padding(0, 4, 8, 4), Padding = new Padding(0), RenderMode = ToolStripRenderMode.System };
            compileTarget = new ToolStripSplitButton("Compilar ATmega") { AutoSize = false, Size = new Size(245, 32), ToolTipText = "Compilar el destino seleccionado. La flecha permite elegir otro firmware y compilarlo." };
            compileTarget.ButtonClick += (s, e) => Build(false);
            for (int index = 0; index < target.Items.Count; index++)
            {
                int selected = index;
                compileTarget.DropDownItems.Add(Convert.ToString(target.Items[index]), null, (s, e) => { target.SelectedIndex = selected; Build(false); });
            }
            compileMenu.Items.Add(compileTarget); actions.Controls.Add(compileMenu); idleControls.Add(compileMenu);
            AddButton(actions, "Compilar todo", () => Build(true), 140);
            flashTarget = AddButton(actions, "Flashear ATmega...", Flash, 285);
            flashTarget.BackColor = Color.FromArgb(22, 79, 99); flashTarget.ForeColor = Color.White;
            flashTarget.FlatStyle = FlatStyle.Flat;
            listen.Text = "Guiar K / RST y detectar modo de programación antes de consultar o cargar ESP"; listen.Checked = true;
            listen.AutoSize = true; listen.Margin = new Padding(0, 8, 0, 0); idleControls.Add(listen); actions.SetFlowBreak(flashTarget, true); actions.Controls.Add(listen);
            target.SelectedIndexChanged += (s, e) => UpdateTargetActions(); UpdateTargetActions();
            layout.Controls.Add(actions, 0, 2);
            var stateBox = new TableLayoutPanel { Dock = DockStyle.Fill, ColumnCount = 1, RowCount = 2 };
            status.Text = "Sin operación. Ningún puerto se abre al iniciar."; status.Dock = DockStyle.Fill;
            status.Font = new Font(Font, FontStyle.Bold); status.ForeColor = Color.FromArgb(22, 79, 99);
            lastResult.Text = "El mensaje boot mode:(1,...) confirma arranque UART; esptool confirma comunicación de ida y vuelta.";
            lastResult.Dock = DockStyle.Fill; lastResult.AutoEllipsis = true;
            stateBox.Controls.Add(status); stateBox.Controls.Add(lastResult); layout.Controls.Add(stateBox, 0, 3);
            console.Dock = DockStyle.Fill; console.ReadOnly = true; console.WordWrap = false;
            console.BackColor = Color.FromArgb(19, 29, 39); console.ForeColor = Color.FromArgb(222, 232, 241);
            console.Font = new Font("Consolas", 9); console.DetectUrls = false;
            console.BindFollow(followLog); layout.Controls.Add(CreateWorkflowPanel(console), 0, 4);
            var footer = new FlowLayoutPanel { Dock = DockStyle.Fill };
            progress.Width = 150; progress.Height = 20; progress.Margin = new Padding(0, 9, 15, 0);
            footer.Controls.Add(progress);
            cancel = new Button { Text = "Cancelar operación", Width = 160, Height = 30, Enabled = false };
            cancel.Click += (s, e) => Cancel(); footer.Controls.Add(cancel);
            var copy = new Button { Text = "Copiar registro", Width = 140, Height = 30 };
            copy.Click += (s, e) => CopyText(console.Text, copy); footer.Controls.Add(copy);
            EnableTextCopyFeedback(console);
            var manual = new Button { Text = "Manual", Width = 100, Height = 30 }; manual.Click += (s, e) => OpenManual(); footer.Controls.Add(manual);
            layout.Controls.Add(footer, 0, 5);

            var configLayout = new TableLayoutPanel { Dock = DockStyle.Fill, ColumnCount = 1, RowCount = 3 };
            configLayout.RowStyles.Add(new RowStyle(SizeType.Absolute, 58)); configLayout.RowStyles.Add(new RowStyle(SizeType.Percent, 100)); configLayout.RowStyles.Add(new RowStyle(SizeType.Absolute, 42));
            configLayout.Controls.Add(new Label { Text = "Configurá rutas para esta computadora. {Repo}, {Env} y %USERPROFILE% se resuelven al ejecutar.\r\nNo se instalan herramientas ni se descargan dependencias al abrir la aplicación. Un build puede usar la red mediante npm/PlatformIO.", Dock = DockStyle.Fill }, 0, 0);
            settings.Dock = DockStyle.Fill; settings.AllowUserToAddRows = false; settings.AllowUserToDeleteRows = false;
            settings.RowHeadersVisible = false; settings.AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.AllCells;
            settings.BackgroundColor = Color.White; settings.AutoGenerateColumns = false; settings.SelectionMode = DataGridViewSelectionMode.CellSelect;
            settings.Columns.Add(new DataGridViewTextBoxColumn { Name = "Field", HeaderText = "Opción", ReadOnly = true, Width = 220 });
            settings.Columns.Add(new DataGridViewTextBoxColumn { Name = "Value", HeaderText = "Valor / ruta", AutoSizeMode = DataGridViewAutoSizeColumnMode.Fill });
            settings.Columns.Add(new DataGridViewButtonColumn { Name = "Browse", HeaderText = "", Width = 45, Text = "...", UseColumnTextForButtonValue = true });
            settings.CellContentClick += (s, e) => { if (e.ColumnIndex == 2 && e.RowIndex >= 0) Browse(e.RowIndex); };
            idleControls.Add(settings); configLayout.Controls.Add(settings, 0, 1);
            var configButtons = new FlowLayoutPanel { Dock = DockStyle.Fill };
            AddButton(configButtons, "Guardar", SaveProfile, 105);
            AddButton(configButtons, "Guardar como...", () => SaveProfileAs(), 130);
            AddButton(configButtons, "Detectar rutas vacías", () => { ReadSettings(); profile.Discover(); FillSettings(); Log("Detección de rutas terminada; revisar valores antes de guardar."); }, 175);
            AddButton(configButtons, "Importar...", ImportProfile, 110); AddButton(configButtons, "Exportar...", ExportProfile, 110);
            configLayout.Controls.Add(configButtons, 0, 2); configuration.Controls.Add(configLayout);
            Controls.Add(tabs); Controls.Add(menu); InitializeActivityBar();
            timer.Interval = 100; timer.Tick += (s, e) => DrainLog(); timer.Start();
            FillSettings(); RefreshPorts();
            FormClosing += WindowClosing;
            InitializeIntegration(tabs, layout, configLayout, footer);
            InitializeSerial(tabs);
            Log("AFR2 Loader " + LoaderVersion.Value + " | Hecho por Koba Chajchir");
            Log("Perfiles y registros: " + Profile.UserRoot);
        }

        Button AddButton(Control parent, string text, Action action, int width)
        {
            var b = new Button { Text = text, Width = width, Height = 33, Margin = new Padding(0, 4, 8, 4) };
            b.Click += (s, e) => { try { action(); } catch (Exception error) { MessageBox.Show(this, error.Message, "AFR2 Loader", MessageBoxButtons.OK, MessageBoxIcon.Error); } };
            parent.Controls.Add(b); idleControls.Add(b); return b;
        }
        void Log(string line)
        {
            string record = DateTime.Now.ToString("HH:mm:ss") + " " + line;
            JobState job = currentJob;
            if (job != null) job.Add(record);
            if (pending.Count < 5000) pending.Enqueue(record);
            lock (logGate) if (logFile != null)
            {
                try { logFile.WriteLine(record); logFile.Flush(); }
                catch (IOException error) { pending.Enqueue("No se pudo escribir el registro en disco: " + error.Message); try { logFile.Dispose(); } catch (IOException) { } logFile = null; }
                catch (UnauthorizedAccessException error) { pending.Enqueue("Sin permiso para guardar el registro: " + error.Message); logFile = null; }
            }
        }
        void DrainLog()
        {
            string line; int budget = 180; var batch = new StringBuilder();
            while (budget-- > 0 && pending.TryDequeue(out line)) batch.AppendLine(line);
            if (batch.Length > 0) console.AppendBatch(batch.ToString());
        }
        void CloseLog()
        {
            lock (logGate)
            {
                try { if (logFile != null) logFile.Dispose(); }
                catch (Exception error) { pending.Enqueue("No se pudo cerrar el registro: " + error.Message); }
                finally { logFile = null; }
            }
        }
        void SetState(string value)
        {
            status.Text = value;
            if (currentJob != null) currentJob.Stage = value;
            UpdateConnectionEvidence(value); UpdateActivityBar(); Log(value);
        }
        void FillSettings()
        {
            settings.Rows.Clear();
            foreach (Field field in Profile.Fields)
            { int i = settings.Rows.Add(field.Label, profile.Get(field.Key)); settings.Rows[i].Tag = field; settings.Rows[i].Cells[1].ToolTipText = field.Hint; }
            SyncEspOptions();
        }
        void ReadSettings()
        {
            settings.EndEdit();
            foreach (DataGridViewRow row in settings.Rows) profile.Values[((Field)row.Tag).Key] = Convert.ToString(row.Cells[1].Value).Trim();
        }
        void Browse(int index)
        {
            ReadSettings(); Field field = (Field)settings.Rows[index].Tag;
            if (field.Kind == "folder")
            {
                using (var dialog = new FolderBrowserDialog { Description = field.Hint })
                { if (Directory.Exists(profile.Resolve(field.Key))) dialog.SelectedPath = profile.Resolve(field.Key); if (dialog.ShowDialog(this) == DialogResult.OK) settings.Rows[index].Cells[1].Value = dialog.SelectedPath; }
            }
            else if (field.Kind == "file")
                using (var dialog = new OpenFileDialog { Title = field.Label + " - " + field.Hint, CheckFileExists = true })
                { if (dialog.ShowDialog(this) == DialogResult.OK) settings.Rows[index].Cells[1].Value = dialog.FileName; }
            else MessageBox.Show(this, field.Hint, field.Label);
        }
        void ExportProfile()
        {
            profileDialogs++;
            try
            {
                ReadSettings();
                using (var dialog = new SaveFileDialog { Filter = "Perfil AFR2 (*.json)|*.json", FileName = "afr2-perfil.json" })
                    if (dialog.ShowDialog(this) == DialogResult.OK) { profile.Save(dialog.FileName); Log("Perfil exportado sin credenciales: " + dialog.FileName); }
            }
            finally { profileDialogs--; }
        }
        void RefreshPorts()
        {
            string selected = ports.Text; ports.Items.Clear();
            ports.Items.AddRange(SerialPort.GetPortNames().OrderBy(p => p.Length).ThenBy(p => p).Cast<object>().ToArray());
            if (ports.Items.Contains(selected)) ports.SelectedItem = selected;
            else if (ports.Items.Count > 0) ports.SelectedIndex = 0;
        }
        bool Confirm(string title, string text, string[] checks)
        {
            SetState("Esperando confirmación: " + title);
            using (var dialog = new Form { Text = title, Size = new Size(800, 660), MinimumSize = new Size(650, 560), StartPosition = FormStartPosition.CenterParent, Font = Font, MinimizeBox = false, MaximizeBox = false })
            {
                var grid = new TableLayoutPanel { Dock = DockStyle.Fill, Padding = new Padding(16), ColumnCount = 1, RowCount = 3 };
                grid.RowStyles.Add(new RowStyle(SizeType.Percent, 100)); grid.RowStyles.Add(new RowStyle(SizeType.Absolute, checks.Length * 39 + 8)); grid.RowStyles.Add(new RowStyle(SizeType.Absolute, 42));
                var detail = new TextBox { Text = text, Multiline = true, ReadOnly = true, ScrollBars = ScrollBars.Vertical, Dock = DockStyle.Fill, BackColor = Color.White };
                detail.TabStop = false; EnableTextCopyFeedback(detail); grid.Controls.Add(detail, 0, 0);
                var checklist = new FlowLayoutPanel { Dock = DockStyle.Fill, FlowDirection = FlowDirection.TopDown, WrapContents = false, AutoScroll = true };
                var boxes = new List<CheckBox>();
                foreach (string check in checks) { var box = new CheckBox { Text = check, Width = 715, Height = 35 }; boxes.Add(box); checklist.Controls.Add(box); }
                grid.Controls.Add(checklist, 0, 1);
                var buttons = new FlowLayoutPanel { Dock = DockStyle.Fill, FlowDirection = FlowDirection.RightToLeft };
                var next = new Button { Text = "Confirmar y continuar", Width = 185, Height = 32, Enabled = checks.Length == 0, DialogResult = DialogResult.OK };
                var back = new Button { Text = "Cancelar", Width = 120, Height = 32, DialogResult = DialogResult.Cancel };
                foreach (var box in boxes) box.CheckedChanged += (s, e) => next.Enabled = boxes.All(b => b.Checked);
                buttons.Controls.Add(next); buttons.Controls.Add(back); grid.Controls.Add(buttons, 0, 2);
                dialog.Controls.Add(grid); dialog.AcceptButton = next; dialog.CancelButton = back;
                FocusDialogAction(dialog, boxes.Count > 0 ? (Control)boxes[0] : next, detail);
                return dialog.ShowDialog(this) == DialogResult.OK;
            }
        }
        async void Execute(string title, Func<Operations, Profile, string, CancellationToken, Task> body, bool usesSerial = false)
        {
            try { await ExecuteCore(title, body, usesSerial); }
            catch (Exception error)
            {
                if (cancellation != null) { cancellation.Dispose(); cancellation = null; }
                CloseLog();
                busy = false; runner = null; foreach (Control c in idleControls) c.Enabled = true;
                configMenu.Enabled = true; cancel.Enabled = false; progress.Style = ProgressBarStyle.Blocks;
                status.Text = "No se pudo iniciar la operación"; lastResult.Text = error.Message;
                lastResult.ForeColor = Color.Firebrick; pending.Enqueue(error.ToString());
                JobState failed = currentJob ?? reservedJob;
                if (failed != null) { failed.Status = "failed"; failed.Result = error.Message; failed.Ended = DateTimeOffset.Now.ToString("o"); }
                currentJob = null; reservedJob = null;
                QueueFailure("Inicio de operación", error); UpdateSerialControls();
            }
        }
        async Task ExecuteCore(string title, Func<Operations, Profile, string, CancellationToken, Task> body, bool usesSerial)
        {
            if (busy || pendingJob != null) return;
            if (serialTransition) throw new InvalidOperationException("Esperá que el monitor termine de abrir/cerrar el puerto antes de iniciar otra operación.");
            ReadSettings(); Profile snapshot = profile.Copy(); string port = ports.Text; DrainNotices();
            currentJob = reservedJob ?? NewJob(title, "window", port, "running"); reservedJob = null;
            currentJob.Status = "running"; currentJob.Stage = title;
            ResetWorkflow(title);
            busy = true; foreach (Control c in idleControls) c.Enabled = false; configMenu.Enabled = false;
            cancel.Enabled = true; progress.Style = ProgressBarStyle.Marquee; lastResult.ForeColor = Color.FromArgb(71, 85, 105);
            cancellation = new CancellationTokenSource(); runner = new Runner(Log);
            var ops = new Operations(runner, Log, SetState, Confirm);
            ops.BootAssistant = GuideEspBoot;
            ops.WritingStarted = () => { if (currentJob != null) currentJob.WritingStarted = true; };
            ops.Changed = (code, message) => QueueNotice(new LoaderNotice { Origin = "cargador " + port, Code = code, Message = message });
            UpdateSerialControls();
            string directory = Path.Combine(Profile.UserRoot, "logs"); Directory.CreateDirectory(directory);
            currentLog = Path.Combine(directory, DateTime.Now.ToString("yyyyMMdd-HHmmss-fff") + "-" + Guid.NewGuid().ToString("N").Substring(0, 6));
            currentJob.LogPath = currentLog + ".txt";
            string resultState = "failed", errorText = null;
            DateTime started = DateTime.Now;
            try
            {
                lock (logGate) logFile = new StreamWriter(currentLog + ".txt", false, new UTF8Encoding(false));
                Log(title + " | perfil: " + snapshot.Get("Name") + " | puerto: " + port);
                if (usesSerial && serialCapture != null)
                {
                    SetState("Liberando monitor serie antes de usar la herramienta de carga/diagnóstico...");
                    await StopSerialMonitor("Puerto entregado al cargador");
                    cancellation.Token.ThrowIfCancellationRequested();
                }
                SetState(title);
                await body(ops, snapshot, port, cancellation.Token);
                resultState = "completed";
                status.Text = "Completado: " + title;
                lastResult.Text = ops.Verified ? "Carga completada y contenido verificado correctamente. Consultá el registro y restaurá el cableado de operación." : "Operación completada sin escritura/verificación de firmware. Consultá el detalle del registro.";
                lastResult.ForeColor = Color.FromArgb(20, 110, 65); Log(status.Text);
                if (ops.Verified) Log("Recordatorio de funcionamiento: Mega D18/TX1 → E_RX (RX central); E_TX (TX central) → Mega D19/RX1. Retirar servicio D0/D1 y RESET Mega→GND con alimentación desconectada.");
                if (!ops.Verified) QueueNotice(new LoaderNotice { Origin = "cargador " + port, Code = "completed", Message = lastResult.Text });
            }
            catch (OperationCanceledException error)
            {
                resultState = "cancelled"; errorText = error.Message; status.Text = "Operación cancelada";
                lastResult.Text = ops.WroteAnything ? "Se inició una escritura: puede haber contenido incompleto. No se declara verificación correcta." : "Cancelado sin iniciar escritura de firmware.";
                lastResult.ForeColor = Color.DarkOrange; Log(status.Text + ": " + error.Message);
                QueueNotice(new LoaderNotice { Origin = "cargador " + port, Code = "cancelled", Message = lastResult.Text, Detail = error.ToString() });
            }
            catch (Exception error)
            {
                errorText = error.ToString(); status.Text = "No se completó: " + title;
                lastResult.Text = (ops.WroteAnything ? "Escritura iniciada, resultado NO verificado. " : "") + error.Message;
                lastResult.ForeColor = Color.Firebrick; Log("ERROR: " + error);
                QueueNotice(new LoaderNotice { Origin = "cargador " + port, Code = "failed", Message = lastResult.Text, Detail = "Etapa: " + currentJob.Stage + "\r\nRegistro: " + currentLog + ".txt\r\n" + error });
            }
            finally
            {
                try
                {
                    string report = new JavaScriptSerializer().Serialize(new { schema = 1, application = "AFR2 Loader " + LoaderVersion.Value, jobId = currentJob.Id, approvalMode = currentJob.ApprovalMode, operation = title, started = started.ToString("o"), ended = DateTime.Now.ToString("o"), status = resultState, error = errorText,
                        writingStarted = ops.WroteAnything, contentVerified = ops.Verified, port = port, profile = snapshot, steps = ops.Steps });
                    File.WriteAllText(currentLog + ".json", report, new UTF8Encoding(false));
                    Log("Registro: " + currentLog + ".txt | Resumen: .json");
                }
                catch (Exception error) { pending.Enqueue("No se pudo guardar el resumen: " + error.Message); }
                CloseLog();
                cancellation.Dispose(); cancellation = null; busy = false; runner = null;
                currentJob.Status = resultState; currentJob.Result = lastResult.Text;
                currentJob.Ended = DateTimeOffset.Now.ToString("o"); currentJob.WritingStarted = ops.WroteAnything;
                currentJob = null;
                foreach (Control c in idleControls) c.Enabled = true; configMenu.Enabled = true;
                cancel.Enabled = false; progress.Style = ProgressBarStyle.Blocks; progress.Value = 0; DrainLog();
                DrainNotices(); UpdateSerialControls();
            }
            if (resultState == "completed" && ops.Verified) RemindOperatingWiring(snapshot);
        }
        void Build(bool all)
        {
            int selected = target.SelectedIndex;
            Execute(all ? "Compilar ATmega + preparar ESP según perfil" : EspFlashRules.IsAt(profile) && selected != 0 ? "Revisar paquete AT" : "Compilar " + TargetName(selected), async (ops, p, port, token) =>
            {
                if (all) await ops.BuildAll(p, token);
                else if (selected == 0) await ops.BuildMega(p, token);
                else if (EspFlashRules.IsAt(p))
                {
                    if (selected >= 2) await ops.BuildWebFs(p, token);
                    else await ops.BuildEsp(p, token);
                }
                else
                {
                    if (selected == 1 || selected == 3) await ops.BuildEsp(p, token);
                    if (selected == 2 || selected == 3) await ops.BuildWebFs(p, token);
                }
                Log("Preparación finalizada según perfil: " + p.Get("EspFirmwareMode") + ". Sin apertura COM ni flasheo; el registro distingue compilación de revisión de paquete precompilado.");
            });
        }
        void DetectEsp()
        {
            bool shouldListen = listen.Checked;
            Execute("Detectar ESP y consultar ROM", async (ops, p, port, token) =>
            { if (!ops.PrepareEsp(false)) throw new OperationCanceledException(); if (shouldListen) await ops.ListenBoot(p, port, token); await ops.Probe(p, port, token); }, true);
        }
        void DetectSelected()
        {
            if (target.SelectedIndex != 0) { DetectEsp(); return; }
            Execute("Detectar ATmega / firma", (ops, p, port, token) => ops.ProbeMega(p, port, token), true);
        }
        void Flash()
        {
            int selected = target.SelectedIndex; bool shouldListen = listen.Checked;
            Execute("Flashear y verificar " + target.Text, async (ops, p, port, token) =>
            {
                if (selected == 0) await ops.FlashMega(p, port, token);
                else await ops.FlashEsp(p, port, selected == 1 || selected == 3, selected == 2 || selected == 3, shouldListen, token);
            }, true);
        }
        void Cancel()
        { Cancel(false); }
        void Cancel(bool automaticallyAccepted)
        {
            if (!busy || cancellation == null) return;
            JobState job = currentJob; CancellationTokenSource requested = cancellation;
            if (!automaticallyAccepted && MessageBox.Show(this, "Cancelar durante una escritura puede dejar firmware o filesystem incompleto. ¿Detener la operación y sus procesos hijos?", "Cancelar", MessageBoxButtons.YesNo, MessageBoxIcon.Warning) != DialogResult.Yes) return;
            if (!busy || currentJob != job || cancellation != requested) return;
            cancellation.Cancel(); if (runner != null) runner.StopTree(); Log(automaticallyAccepted ? "Cancelación MCP aceptada automáticamente por la opción de la sesión." : "Cancelación solicitada.");
        }
        async void WindowClosing(object sender, FormClosingEventArgs e)
        {
            if (busy) { e.Cancel = true; MessageBox.Show(this, "Hay una operación en curso. Esperá a que termine o usá Cancelar; el programa mantendrá el puerto hasta cerrarlo correctamente."); }
            else if (serialTransition) { e.Cancel = true; MessageBox.Show(this, "Esperá que el monitor termine de abrir/cerrar el puerto."); }
            else if (!closingAfterSerial && serialCapture != null)
            {
                e.Cancel = true;
                try { await StopSerialMonitor("Cierre de ventana"); closingAfterSerial = true; BeginInvoke(new Action(Close)); }
                catch (Exception error) { QueueFailure("Cierre de ventana", error); }
            }
            else { RejectPending(); if (bridge != null) bridge.Dispose(); timer.Stop(); timer.Dispose(); }
        }
        void OpenManual()
        {
            string file = Path.Combine(appDir, "MANUAL.html");
            if (!File.Exists(file)) { MessageBox.Show(this, "No se encontró MANUAL.html. Distribuí la carpeta completa."); return; }
            Process.Start(new ProcessStartInfo(file) { UseShellExecute = true });
        }
        void OpenLogs() { string dir = Path.Combine(Profile.UserRoot, "logs"); Directory.CreateDirectory(dir); Process.Start("explorer.exe", Runner.Quote(dir)); }
        void About()
        {
            MessageBox.Show(this, "AFR2 Loader " + LoaderVersion.Value + "\r\nCompilación, carga e integración MCP de AutoFinalRobo2\r\n\r\nHecho por Koba Chajchir\r\nGitHub: https://github.com/kobachajchir\r\nLinkedIn: https://www.linkedin.com/in/kchajchir\r\nTeléfono: +54 388 488 9973\r\n\r\nDatos de la sección Desarrollador de AutoFinalRobo2.\r\nInterfaz WinForms / .NET Framework; herramientas externas configurables.\r\nMCP comparte la sesión; revisión manual o aceptación automática activada en la ventana.\r\nNo controla físicamente los botones KEY/RST ni verifica el cableado.", "Acerca de AFR2 Loader", MessageBoxButtons.OK, MessageBoxIcon.Information);
        }
    }
}
