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
        Button cancel, detectTarget;
        ToolStripSplitButton flashTarget;
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
            Profile.SetApplicationDirectory(this.appDir);
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
            AutoScaleMode = AutoScaleMode.Dpi; Font = new Font("Segoe UI", 10F); BackColor = Color.FromArgb(245, 247, 250);
            var menu = new MenuStrip();
            configMenu.DropDownItems.Add("Importar perfil...", null, (s, e) => ImportProfile());
            configMenu.DropDownItems.Add("Exportar perfil...", null, (s, e) => ExportProfile());
            configMenu.DropDownItems.Add("Guardar perfil actual", null, (s, e) => SaveProfile());
            configMenu.DropDownItems.Add("Guardar perfil como...", null, (s, e) => SaveProfileAs());
            var help = optionsMenu;
            help.DropDownItems.Add("Manual detallado", null, (s, e) => OpenManual());
            InitializeLogMenu(help);
            help.DropDownItems.Add("Acerca de...", null, (s, e) => About());
            menu.Items.Add(configMenu); menu.Items.Add(help); MainMenuStrip = menu;

            var tabs = new TabControl { Dock = DockStyle.Fill, Padding = new Point(12, 8) };
            var operations = new TabPage("Operación") { Padding = new Padding(16) };
            var configuration = new TabPage("Configuración") { Padding = new Padding(UiInset) };
            tabs.TabPages.Add(operations); InitializeMegaBridge(tabs); tabs.TabPages.Add(configuration);
            var layout = new TableLayoutPanel { Dock = DockStyle.Fill, ColumnCount = 1, RowCount = 6 };
            layout.RowStyles.Add(new RowStyle(SizeType.Absolute, 48));
            layout.RowStyles.Add(new RowStyle(SizeType.Absolute, 48));
            layout.RowStyles.Add(new RowStyle(SizeType.Absolute, 82));
            layout.RowStyles.Add(new RowStyle(SizeType.Absolute, 112));
            layout.RowStyles.Add(new RowStyle(SizeType.Percent, 100));
            layout.RowStyles.Add(new RowStyle(SizeType.Absolute, 42));
            operations.Controls.Add(layout);
            layout.Controls.Add(new Label { AutoSize = true, Anchor = AnchorStyles.Left, Text = "Preparar y cargar", Font = new Font(Font.FontFamily, 17, FontStyle.Bold), ForeColor = Color.FromArgb(23, 39, 51), Margin = new Padding(0, 0, 0, UiGap) }, 0, 0);
            operationTab = operations;
            target.Items.AddRange(new object[] { "ATmega", "Firmware ESP", "FileSystem", "Firmware ESP + FileSystem" });
            target.SelectedIndex = 0;
            layout.RowStyles[1].SizeType = SizeType.AutoSize;
            layout.RowStyles[2].Height = 0;
            layout.Controls.Add(CreateEspOptions(CreateOperationSelectors()), 0, 1);
            target.SelectedIndexChanged += (s, e) => { if (!syncingDestination) SelectOperationTarget(target.SelectedIndex); };
            var stateBox = new TableLayoutPanel { Dock = DockStyle.Fill, ColumnCount = 1, RowCount = 3 };
            layout.RowStyles[3].Height = 160;
            stateBox.RowStyles.Add(new RowStyle(SizeType.Percent, 35));
            stateBox.RowStyles.Add(new RowStyle(SizeType.Percent, 30));
            stateBox.RowStyles.Add(new RowStyle(SizeType.Percent, 35));
            status.Text = "Sin operación. Ningún puerto se abre al iniciar."; status.Dock = DockStyle.Fill;
            status.Font = new Font(Font, FontStyle.Bold); status.ForeColor = Color.FromArgb(22, 79, 99);
            lastResult.Text = "El mensaje boot mode:(1,...) confirma arranque UART; esptool confirma comunicación de ida y vuelta.";
            lastResult.Dock = DockStyle.Fill; lastResult.AutoEllipsis = true;
            stateBox.Controls.Add(status); stateBox.Controls.Add(lastResult); stateBox.Controls.Add(commandProgress); layout.Controls.Add(stateBox, 0, 3);
            console.Dock = DockStyle.Fill; console.ReadOnly = true; console.WordWrap = false;
            console.BackColor = Color.FromArgb(19, 29, 39); console.ForeColor = Color.FromArgb(222, 232, 241);
            console.Font = new Font("Consolas", 10); console.DetectUrls = false;
            console.BindFollow(followLog); layout.Controls.Add(CreateWorkflowPanel(console), 0, 4);
            var footer = new FlowLayoutPanel { Dock = DockStyle.Fill, WrapContents = false, AutoSize = true };
            var copy = new ActionButton { Text = "Copiar registro", Width = 150, Role = ButtonRole.Secondary };
            copy.Click += (s, e) => CopyText(console.Text, copy); footer.Controls.Add(copy);
            var clear = new ActionButton { Text = "Limpiar ventana", Width = 150, Role = ButtonRole.Secondary };
            clear.Click += (s, e) => ClearOperationView(); footer.Controls.Add(clear);
            EnableTextCopyFeedback(console);
            var compileMenu = new ToolStrip { GripStyle = ToolStripGripStyle.Hidden, Dock = DockStyle.None, AutoSize = true, Padding = new Padding(0), Margin = new Padding(0, 4, 8, 4), RenderMode = ToolStripRenderMode.System };
            compileTarget = new ToolStripSplitButton("Compilar") { AutoSize = false, Size = new Size(285, UiButtonHeight), DropDownButtonWidth = UiSplitWidth };
            compileTarget.ButtonClick += (s, e) => Build(false);
            compileTarget.DropDownItems.Add("Compilar destino seleccionado", null, (s, e) => Build(false));
            compileTarget.DropDownItems.Add("Compilar todo", null, (s, e) => Build(true));
            compileMenu.Items.Add(compileTarget); idleControls.Add(compileMenu); footer.Controls.Add(compileMenu);
            cancel = new ActionButton { Text = "Cancelar operación", Width = 180, Enabled = false, Role = ButtonRole.Danger };
            cancel.Click += (s, e) => Cancel(); footer.Controls.Add(cancel);
            var flashMenu = CreateFirmwareFlashMenu(true);
            // AFR2-20261106-06: the next missing requirement is written beside the actions, not hidden in a tooltip.
            nextStep.Dock = DockStyle.Fill; nextStep.AutoSize = true; nextStep.Margin = new Padding(0, 4, 0, 0);
            var footerLayout = new TableLayoutPanel { Dock = DockStyle.Fill, AutoSize = true, ColumnCount = 2, RowCount = 2, Padding = new Padding(0, UiGap, 0, 0) };
            footerLayout.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100)); footerLayout.ColumnStyles.Add(new ColumnStyle(SizeType.AutoSize));
            footerLayout.RowStyles.Add(new RowStyle(SizeType.AutoSize)); footerLayout.RowStyles.Add(new RowStyle(SizeType.AutoSize));
            footer.Dock = DockStyle.Fill; footer.WrapContents = true; footer.AutoSize = true;
            footerLayout.Controls.Add(footer, 0, 0); footerLayout.Controls.Add(flashMenu, 1, 0);
            footerLayout.Controls.Add(nextStep, 0, 1); footerLayout.SetColumnSpan(nextStep, 2);
            footerLayout.SizeChanged += (s, e) => nextStep.MaximumSize = new Size(Math.Max(200, footerLayout.ClientSize.Width), 0);
            layout.Controls.Add(footerLayout, 0, 5);
            UpdateDestinationVisibility();
            var configLayout = new TableLayoutPanel { Dock = DockStyle.Fill, ColumnCount = 1, RowCount = 3 };
            configLayout.RowStyles.Add(new RowStyle(SizeType.Absolute, 58)); configLayout.RowStyles.Add(new RowStyle(SizeType.Percent, 100)); configLayout.RowStyles.Add(new RowStyle(SizeType.Absolute, 42));
            configLayout.Controls.Add(new Label { Text = "Configurá las rutas de tu equipo. {App}, {Repo}, {AtmegaDir}, {EspDir}, {WebDir}, {Env} y variables de entorno se resuelven al ejecutar.\r\nLas rutas relativas parten de la carpeta del Loader. No se instalan herramientas al abrir; compilaciones externas pueden usar la red.", Dock = DockStyle.Fill }, 0, 0);
            settings.Dock = DockStyle.Fill; settings.AllowUserToAddRows = false; settings.AllowUserToDeleteRows = false;
            settings.RowHeadersVisible = false; settings.AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.AllCells;
            settings.BackgroundColor = Color.White; settings.AutoGenerateColumns = false; settings.SelectionMode = DataGridViewSelectionMode.CellSelect;
            settings.Columns.Add(new DataGridViewTextBoxColumn { Name = "Field", HeaderText = "Opción", ReadOnly = true, Width = 220 });
            settings.Columns.Add(new DataGridViewTextBoxColumn { Name = "Value", HeaderText = "Valor / ruta", AutoSizeMode = DataGridViewAutoSizeColumnMode.Fill });
            settings.Columns.Add(new DataGridViewButtonColumn { Name = "Browse", HeaderText = "", Width = 45, Text = "...", UseColumnTextForButtonValue = true });
            settings.CellContentClick += (s, e) => { if (e.ColumnIndex == 2 && e.RowIndex >= 0) Browse(e.RowIndex); };
            idleControls.Add(settings); configLayout.Controls.Add(settings, 0, 1);
            var configButtons = UiRow();

            AddButton(configButtons, "Detectar rutas vacías", () => { ReadSettings(); profile.Discover(); FillSettings(); Log("Detección de rutas terminada; revisar valores antes de guardar."); }, 175);
            AddButton(configButtons, "Importar...", ImportProfile, 110); AddButton(configButtons, "Exportar...", ExportProfile, 110);
            configLayout.Controls.Add(configButtons, 0, 2); configuration.Controls.Add(configLayout);
            // AFR2-20261106-06: the activity bar owns a row of its own below the tabs, so it never overlaps tab content.
            var root = new TableLayoutPanel { Dock = DockStyle.Fill, ColumnCount = 1, RowCount = 2, Margin = new Padding(0), Padding = new Padding(0) };
            root.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100));
            root.RowStyles.Add(new RowStyle(SizeType.Percent, 100)); root.RowStyles.Add(new RowStyle(SizeType.AutoSize));
            tabs.Margin = new Padding(0); root.Controls.Add(tabs, 0, 0);
            Controls.Add(root); Controls.Add(menu); InitializeActivityBar(root);
            timer.Interval = 100; timer.Tick += (s, e) => { DrainLog(); DrainCommandFeedback(); }; timer.Start();
            FillSettings(); RefreshPorts();
            FormClosing += WindowClosing;
            InitializeIntegration(tabs, layout, configLayout, footer);
            InitializeSerial(tabs);
            InitializePreferences(tabs);
            InitializeConverter(tabs);
            bridgeTab.Text = "Flasher";
            string[] tabOrder = { "Operación", "Flasher", "Monitor serie", "Historial de sesión", "Eventos y errores", "Conversor HEX / ASCII", "Servidor MCP", "Configuración" };
            var orderedPages = tabs.TabPages.Cast<TabPage>().OrderBy(page => { int index = Array.IndexOf(tabOrder, page.Text); return index < 0 ? 8 : index; }).ToArray();
            tabs.TabPages.Clear(); tabs.TabPages.AddRange(orderedPages);
            StyleControls(this);
            InitializeAppearance();
            ArrangeOperationWorkspace(layout);
            InitializeBrand();
            timer.Tick += (sender, args) => { if (++preparationTick % 30 == 0 && !busy) UpdatePreparationControls(); };
            UpdatePreparationControls();
            Log("AFR2 Loader " + LoaderVersion.Value + " | Hecho por Koba Chajchir");
            Log("Perfiles y registros: " + Profile.UserRoot);
        }

        Button AddButton(Control parent, string text, Action action, int width, ButtonRole role = ButtonRole.Auto)
        {
            var b = new ActionButton { Text = text, Width = width, Height = 33, Margin = new Padding(0, 4, 8, 4), Role = role };
            b.Click += (s, e) => { try { action(); } catch (Exception error) { MessageBox.Show(this, error.Message, "AFR2 Loader", MessageBoxButtons.OK, MessageBoxIcon.Error); } };
            parent.Controls.Add(b); idleControls.Add(b); ButtonStyle(b); return b;
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
        void ClearOperationView()
        {
            // Discard only the display backlog. Log() already records it in the job and on disk.
            int queued = pending.Count; string ignored;
            while (queued-- > 0 && pending.TryDequeue(out ignored)) { }
            console.Clear();
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
            SyncBridgeOptions(); ApplyConfigurationFilter();
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
                { if (Directory.Exists(profile.Resolve(field.Key))) dialog.SelectedPath = profile.Resolve(field.Key); if (ShowStyledDialog(dialog) == DialogResult.OK) settings.Rows[index].Cells[1].Value = dialog.SelectedPath; }
            }
            else if (field.Kind == "file")
                using (var dialog = new OpenFileDialog { Title = field.Label + " - " + field.Hint, CheckFileExists = true })
                { if (ShowStyledDialog(dialog) == DialogResult.OK) settings.Rows[index].Cells[1].Value = dialog.FileName; }
            else ShowFieldHelp(field);
        }
        void ExportProfile()
        {
            profileDialogs++;
            try
            {
                ReadSettings();
                using (var dialog = new SaveFileDialog { Filter = "Perfil AFR2 (*.json)|*.json", FileName = "afr2-perfil.json" })
                    if (ShowStyledDialog(dialog) == DialogResult.OK) { profile.Save(dialog.FileName); Log("Perfil exportado sin credenciales: " + dialog.FileName); }
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
            if (sessionChecksAccepted) { Log("Confirmación manual omitida por autorización de sesión: " + title + "\r\n" + text + "\r\n" + String.Join("\r\n", checks)); return true; }
            SetState("Esperando confirmación: " + title);
            using (var dialog = new Form { Text = title, Size = new Size(850, 690), MinimumSize = new Size(700, 540), StartPosition = FormStartPosition.CenterParent, Font = Font, MinimizeBox = false, MaximizeBox = false })
            {
                dialog.Height = Math.Min(dialog.Height, Screen.FromControl(this).WorkingArea.Height - 40);
                var grid = new TableLayoutPanel { Dock = DockStyle.Fill, Padding = new Padding(UiInset), ColumnCount = 1, RowCount = 5 };
                grid.RowStyles.Add(new RowStyle(SizeType.Percent, 45)); grid.RowStyles.Add(new RowStyle(SizeType.Percent, 55));
                grid.RowStyles.Add(new RowStyle(SizeType.Absolute, 46)); grid.RowStyles.Add(new RowStyle(SizeType.Absolute, 48)); grid.RowStyles.Add(new RowStyle(SizeType.Absolute, 48));
                var detail = new TextBox { Text = text, Multiline = true, ReadOnly = true, ScrollBars = ScrollBars.Vertical, Dock = DockStyle.Fill, BackColor = Color.White };
                detail.TabStop = false; EnableTextCopyFeedback(detail); grid.Controls.Add(detail, 0, 0);
                bool require = preferences == null || preferences.RequireChecklist;
                var checklist = new TableLayoutPanel { Dock = DockStyle.Fill, ColumnCount = 1, AutoScroll = true };
                var boxes = new System.Collections.Generic.List<CheckBox>();
                if (require) foreach (string check in checks) {
                    var box = new CheckBox { Text = check, AutoSize = true, MaximumSize = new Size(735, 0), Dock = DockStyle.Top, Checked = preferences != null && preferences.ChecklistsInitiallyChecked, Margin = new Padding(0, 8, 0, 8) };
                    checklist.RowStyles.Add(new RowStyle(SizeType.AutoSize)); checklist.Controls.Add(box, 0, boxes.Count); boxes.Add(box);
                }
                else checklist.Controls.Add(new Label { Text = "Casillas desactivadas en Opciones → Preferencias → Asistentes.\r\nRevisá la información antes de confirmar.", Dock = DockStyle.Top, AutoSize = true });
                grid.Controls.Add(checklist, 0, 1);
                var review = UiRow(); var all = new ActionButton { Text = "Seleccionar todos / aceptar todo", Width = 310, Visible = require && boxes.Count > 0 };
                all.Click += (s, e) => { foreach (var box in boxes) box.Checked = true; }; review.Controls.Add(all);
                var manual = new LinkLabel { Text = "Ver explicación en el manual", AutoSize = true, Margin = new Padding(8, 12, 0, 0) }; manual.LinkClicked += (s, e) => OpenManualSection(ManualForAction(title)); review.Controls.Add(manual); grid.Controls.Add(review, 0, 2);
                var portRow = UiRow(); var portState = new Label { Text = portCheckState, AutoSize = true, Margin = new Padding(0, 10, 12, 0) }; portRow.Controls.Add(portState);
                var checkPort = new ActionButton { Text = "Comprobar COM", Width = 185 };
                checkPort.Click += (s, e) => { try { PortAvailability.Check(ports.Text); portCheckState = ports.Text + " disponible a " + DateTime.Now.ToString("HH:mm:ss") + " (comprobación puntual)"; } catch (Exception error) { portCheckState = error.Message; } portState.Text = portCheckState; }; portRow.Controls.Add(checkPort); grid.Controls.Add(portRow, 0, 3);
                var buttons = new FlowLayoutPanel { Dock = DockStyle.Fill, FlowDirection = FlowDirection.RightToLeft };
                var next = new ActionButton { Text = "Confirmar y continuar", Width = 230, Enabled = boxes.All(b => b.Checked), DialogResult = DialogResult.OK };
                var back = new ActionButton { Text = "Cancelar", Width = 140, DialogResult = DialogResult.Cancel };
                foreach (var box in boxes) box.CheckedChanged += (s, e) => next.Enabled = boxes.All(b => b.Checked);
                buttons.Controls.Add(next); buttons.Controls.Add(back); grid.Controls.Add(buttons, 0, 4);
                dialog.Controls.Add(grid); dialog.AcceptButton = next; dialog.CancelButton = back; FocusDialogAction(dialog, boxes.Count > 0 ? (Control)boxes[0] : next, detail);
                return ShowStyledDialog(dialog) == DialogResult.OK;
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
            if (busy || pendingJob != null || profileDialogs > 0) return;
            if (serialTransition) throw new InvalidOperationException("Esperá que el monitor termine de abrir/cerrar el puerto antes de iniciar otra operación.");
            ReadSettings(); Profile snapshot = profile.Copy(); string port = ports.Text; DrainNotices();
            idleControls.RemoveAll(c => c.IsDisposed);
            currentJob = reservedJob ?? NewJob(title, "window", port, "running"); reservedJob = null;
            currentJob.Status = "running"; currentJob.Stage = title;
            ShowOperation(); followLog.Checked = true; DrainLog(); console.GoToEnd();
            ResetWorkflow(title);
            ResetCommandFeedback();
            busy = true; foreach (Control c in idleControls) c.Enabled = false; configMenu.Enabled = false;
            cancel.Enabled = true; progress.Style = ProgressBarStyle.Marquee; lastResult.ForeColor = Color.FromArgb(71, 85, 105);
            cancellation = new CancellationTokenSource(); runner = new Runner(Log);
            runner.Feedback = QueueCommandFeedback;
            lastResult.Text = "Operación en curso. Esperá el resultado de escritura y verificación.";
            var ops = new Operations(runner, Log, SetState, Confirm);
            ops.BridgeToolPath = Path.Combine(appDir, "tools", "esp_bridge_esptool.py");
            ops.BootAssistant = GuideEspBoot;
            ops.EspPhysicalConfirmation = ConfirmEspPhysicalIntervention;
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
                if (usesSerial) { SetState("Comprobando disponibilidad de " + port + "..."); PortAvailability.Check(port); portCheckState = port + " disponible a " + DateTime.Now.ToString("HH:mm:ss"); Log(portCheckState + ". Comprobación puntual; la herramienta abrirá el puerto a continuación."); }
                SetState(title);
                await body(ops, snapshot, port, cancellation.Token);
                resultState = "completed";
                status.Text = "Completado: " + title;
                lastResult.Text = !String.IsNullOrEmpty(ops.CompletionMessage) ? ops.CompletionMessage : ops.Verified ? "Carga completada y contenido verificado correctamente. Consultá el registro y restaurá el cableado de operación." : "Operación completada sin escritura/verificación de firmware. Consultá el detalle del registro.";
                lastResult.ForeColor = Color.FromArgb(20, 110, 65); Log(status.Text);
                if (ops.Verified)
                    Log(ops.FlashedMegaBackup ? "Backup de aplicación ATmega flasheado y contrastado. Revisar qué programa contiene antes de operar; no se supone que sea el firmware del auto."
                        : ops.InstalledMegaFlasher || (Operations.UsesMegaBridge(snapshot) && !ops.MegaFirmwareFlashed)
                        ? "Servicio temporal: RESET Mega LIBRE, D18→E_RX / E_TX→D19. El Mega aún ejecuta el flasher; restaurar el firmware del auto antes de operar."
                        : "Recordatorio de funcionamiento: Mega D18/TX1 → E_RX (RX central); E_TX (TX central) → Mega D19/RX1. Con alimentación desconectada, retirar el adaptador USB–TTL, si se usó.");
                if (!ops.Verified) QueueNotice(new LoaderNotice { Origin = "cargador " + port, Code = "completed", Message = lastResult.Text });
            }
            catch (OperationCanceledException error)
            {
                resultState = "cancelled"; errorText = error.Message; status.Text = "Operación cancelada";
                lastResult.Text = ops.Verified ? "El contenido se verificó antes de la cancelación; el flujo de servicio no terminó." : ops.WroteAnything ? "Se inició una escritura: puede haber contenido incompleto. No se declara verificación correcta." : "Cancelado sin iniciar escritura de firmware.";
                lastResult.ForeColor = Color.DarkOrange; Log(status.Text + ": " + error.Message);
                QueueNotice(new LoaderNotice { Origin = "cargador " + port, Code = "cancelled", Message = lastResult.Text, Detail = error.ToString() });
            }
            catch (Exception error)
            {
                errorText = error.ToString(); status.Text = "No se completó: " + title;
                lastResult.Text = (ops.Verified ? "Contenido verificado; falló un paso posterior del flujo. " : ops.WroteAnything ? "Escritura iniciada, resultado NO verificado. " : "") + error.Message;
                lastResult.ForeColor = Color.Firebrick; Log("ERROR: " + error);
                QueueNotice(new LoaderNotice { Origin = "cargador " + port, Code = "failed", Message = lastResult.Text, Detail = "Etapa: " + currentJob.Stage + "\r\nRegistro: " + currentLog + ".txt\r\n" + error });
            }
            finally
            {
                if (ops.EspSessionStarted && ops.KeyRstAutomatic)
                {
                    Log("Operación ESP finalizada en modo KEY/RST automático. El registro del puente indica qué acciones y limpieza fueron confirmadas; finalizar no acredita por sí solo un reinicio ni respuesta ROM.");
                }
                else if (ops.EspSessionStarted)
                {
                    lastResult.Text += " Ya podés soltar KEY.";
                    Log("Operación ESP finalizada: ya podés soltar KEY. El Loader no reinicia automáticamente el ESP.");
                }
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
                lock (commandFeedbackGate) pendingCommandFeedback = null;
                commandProgress.Text = resultState == "completed" ? "Proceso finalizado. " + (ops.Verified ? "Contenido verificado." : "Consultá el resultado de la operación.")
                    : "Proceso detenido. " + (ops.WroteAnything && !ops.Verified ? "Escritura iniciada; contenido sin verificar." : "Consultá el resultado y el registro.");
                commandProgress.ForeColor = lastResult.ForeColor;
                workflowTips.SetToolTip(commandProgress, commandProgress.Text);
                DrainNotices(); UpdateSerialControls(); SyncBridgeOptions();
            }
            if (resultState == "completed" && ops.Verified) RemindOperatingWiring(snapshot, ops.InstalledMegaFlasher, ops.MegaFirmwareFlashed, ops.FlashedMegaBackup);
        }
        void Build(bool all)
        {
            ReadSettings();
            if ((all || target.SelectedIndex != 0) && !CapacityConfirmed) {
                MessageBox.Show(this, "Primero pulsá Consultar tamaño de flash o Detectar ESP. Después volvé a preparar el paquete. No se inició ninguna operación.", "Detección pendiente");
                return;
            }
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
            ShowOperation();
            bool shouldListen = listen.Checked;
            Execute("Detectar ESP y consultar ROM", async (ops, p, port, token) =>
            { confirmedFlashCapacity = null;
                await ops.PrepareEspSession(p, port, false, shouldListen, token);
                long bytes = await ops.Probe(p, port, token);
                ApplyDetectedFlashCapacity(bytes);
            }, true);
        }
        void DetectSelected()
        {
            if (target.SelectedIndex != 0) { DetectEsp(); return; }
            Execute("Detectar ATmega / firma", (ops, p, port, token) => ops.ProbeMega(p, port, token), true);
        }
        void Flash()
        {
            if (busy || pendingJob != null || profileDialogs > 0 || syncingEspOptions || syncingDestination || serialTransition) return;
            ReadSettings();
            if (!SelectedSectionPrepared()) { UpdatePreparationControls(); MessageBox.Show(this, "Primero compilá esta sección con el perfil actual (AT: revisar paquete).", "Preparación pendiente"); return; }
            int selected = target.SelectedIndex; bool shouldListen = listen.Checked;
            bool communicationOnly = selected == 0 && megaVariant.SelectedIndex == 1;
            Execute("Flashear y verificar " + target.Text, async (ops, p, port, token) =>
            {
                if (selected == 0) { if (communicationOnly) p.Values["Hex"] = Path.Combine(p.Resolve("AtmegaDir"), "build", "afr2-communication.hex"); await ops.FlashMega(p, port, token); }
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
            OpenManualSection("inicio");
        }
        void OpenLogs() { string dir = Path.Combine(Profile.UserRoot, "logs"); Directory.CreateDirectory(dir); Process.Start("explorer.exe", Runner.Quote(dir)); }
        void About()
        {
            ShowAboutLoader();
        }
    }
}

