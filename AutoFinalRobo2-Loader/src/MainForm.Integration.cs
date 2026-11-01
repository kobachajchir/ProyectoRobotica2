using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text.RegularExpressions;
using System.Threading;
using System.Threading.Tasks;
using System.Web.Script.Serialization;
using System.Windows.Forms;

namespace AFR2Loader
{
    public sealed partial class MainForm
    {
        readonly CheckBox followLog = new CheckBox { Text = "Seguir registro", Checked = true, AutoSize = true, Margin = new Padding(8, 9, 0, 0) };
        readonly CheckBox remoteRequests = new CheckBox { Text = "Aceptar solicitudes de Codex", Checked = true, AutoSize = true };
        readonly CheckBox autoAcceptRequests = new CheckBox { Text = "Autoaceptar todas las solicitudes de Codex (esta sesión)", AutoSize = true };
        readonly Label evidence = new Label { Dock = DockStyle.Fill, AutoEllipsis = true };
        readonly Label remoteState = new Label { Dock = DockStyle.Fill, ForeColor = Color.FromArgb(22, 79, 99), Font = new Font("Segoe UI", 11, FontStyle.Bold) };
        readonly TextBox requestDetail = new TextBox { Dock = DockStyle.Fill, Multiline = true, ReadOnly = true, ScrollBars = ScrollBars.Vertical, BackColor = Color.White };
        readonly DataGridView history = new DataGridView();
        readonly Dictionary<string, JobState> jobs = new Dictionary<string, JobState>();
        readonly Dictionary<string, string[]> requests = new Dictionary<string, string[]>();
        readonly string sessionId = Guid.NewGuid().ToString("N");
        LocalBridge bridge;
        volatile JobState currentJob;
        JobState reservedJob, pendingJob;
        Dictionary<string, object> pendingArguments;
        Button reviewRequest, rejectRequest;
        TabPage mcpTab;
        TabControl mainTabs;
        string bootEvidence = "sin observar", romEvidence = "sin consultar", evidencePort = "";
        int historyTick;

        void InitializeIntegration(TabControl tabs, TableLayoutPanel layout, TableLayoutPanel configLayout, FlowLayoutPanel footer)
        {
            mainTabs = tabs;
            layout.RowStyles[3].Height = 172;
            var box = (TableLayoutPanel)layout.GetControlFromPosition(0, 3);
            box.RowCount = 4; box.RowStyles.Clear();
            foreach (int height in new[] { 26, 32, 42, 72 }) box.RowStyles.Add(new RowStyle(SizeType.Absolute, height));
            lastResult.AutoEllipsis = false; status.AutoEllipsis = false;
            box.Controls.Add(evidence, 0, 3);
            target.SelectedIndexChanged += (s, e) => UpdateConnectionEvidence("");
            ports.SelectedIndexChanged += (s, e) => { bootEvidence = "sin observar"; romEvidence = "sin consultar"; evidencePort = ports.Text; UpdateConnectionEvidence(""); };
            UpdateConnectionEvidence("");

            mcpTab = new TabPage("Servidor MCP") { Padding = new Padding(16) }; tabs.TabPages.Add(mcpTab);
            var mcpScroll = new Panel { Dock = DockStyle.Fill, AutoScroll = true };
            var mcpLayout = new TableLayoutPanel { Dock = DockStyle.Top, AutoSize = true, AutoSizeMode = AutoSizeMode.GrowAndShrink, ColumnCount = 1, RowCount = 7 };
            mcpLayout.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100));
            for (int row = 0; row < 7; row++) mcpLayout.RowStyles.Add(new RowStyle(row == 4 ? SizeType.Absolute : SizeType.AutoSize, row == 4 ? 220 : 0));
            mcpLayout.Controls.Add(CreateMcpRegistrationPanel(), 0, 0);
            mcpLayout.Controls.Add(CreateMcpServerPanel(), 0, 1);
            // AFR2-20261029-02: ayuda local de herramientas y acciones, sin iniciar solicitudes.
            var mcpHelp = new FlowLayoutPanel { Dock = DockStyle.Top, AutoSize = true, WrapContents = true };
            var viewMcpActions = new ActionButton { Text = "Ver acciones MCP", Width = 210 };
            viewMcpActions.Click += (s, e) => OpenManualSection("mcp-acciones");
            mcpHelp.Controls.Add(viewMcpActions); mcpLayout.Controls.Add(mcpHelp, 0, 2);
            // Separate autosized rows reserve room for both checkboxes before any click,
            // including when Windows scales fonts. Disabled never means hidden.
            var requestOptions = new TableLayoutPanel { Dock = DockStyle.Top, AutoSize = true, ColumnCount = 1, RowCount = 2, Margin = new Padding(0, 6, 0, 6) };
            requestOptions.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100));
            requestOptions.RowStyles.Add(new RowStyle(SizeType.AutoSize)); requestOptions.RowStyles.Add(new RowStyle(SizeType.AutoSize));
            remoteRequests.Dock = autoAcceptRequests.Dock = DockStyle.Top;
            remoteRequests.Margin = autoAcceptRequests.Margin = new Padding(3, 4, 3, 4);
            autoAcceptRequests.Visible = true; autoAcceptRequests.Enabled = remoteRequests.Checked;
            requestOptions.Controls.Add(remoteRequests, 0, 0); requestOptions.Controls.Add(autoAcceptRequests, 0, 1);
            InitializeSessionChecks(requestOptions);
            remoteRequests.CheckedChanged += (s, e) => {
                autoAcceptRequests.Enabled = remoteRequests.Checked;
                if (!remoteRequests.Checked) autoAcceptRequests.Checked = false;
                UpdateRequestMode();
            };
            autoAcceptRequests.CheckedChanged += (s, e) => {
                UpdateRequestMode();
                Log("Aceptación automática MCP " + (autoAcceptRequests.Checked ? "activada" : "desactivada") + " por el usuario para esta sesión.");
                if (autoAcceptRequests.Checked && pendingJob != null) ScheduleAutomaticApproval(pendingJob);
            };
            EnableTextCopyFeedback(requestDetail); requestDetail.TabStop = false;
            remoteState.AutoSize = true; remoteState.Margin = new Padding(3, 4, 3, 8);
            mcpLayout.Controls.Add(CreateRequestTable(), 0, 4); mcpLayout.Controls.Add(remoteState, 0, 3); mcpLayout.Controls.Add(requestOptions, 0, 6);
            var approvalButtons = new TableLayoutPanel { Dock = DockStyle.Top, AutoSize = true, ColumnCount = 2, Padding = new Padding(0, 8, 0, 8) };
            approvalButtons.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 50)); approvalButtons.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 50));
            reviewRequest = new ActionButton { Text = "Revisar y continuar...", Width = 190, Height = 34, Enabled = false };
            rejectRequest = new ActionButton { Text = "Rechazar solicitud", Width = 175, Height = 34, Enabled = false };
            reviewRequest.Click += (s, e) => { try { ApprovePending(false); } catch (Exception error) { FailPending(error); } };
            rejectRequest.Click += (s, e) => RejectPending(); rejectRequest.Anchor = AnchorStyles.Left; reviewRequest.Anchor = AnchorStyles.Right; approvalButtons.Controls.Add(rejectRequest, 0, 0); approvalButtons.Controls.Add(reviewRequest, 1, 0);
            mcpLayout.Controls.Add(approvalButtons, 0, 5);
            mcpScroll.Controls.Add(mcpLayout); mcpTab.Controls.Add(mcpScroll); UpdateRequestMode();

            var historyTab = new TabPage("Historial de sesión") { Padding = new Padding(UiInset) };
            history.Dock = DockStyle.Fill; history.ReadOnly = true; history.AllowUserToAddRows = false; history.AllowUserToDeleteRows = false;
            history.RowHeadersVisible = false; history.AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.Fill;
            history.SelectionMode = DataGridViewSelectionMode.FullRowSelect; history.MultiSelect = true;
            history.ClipboardCopyMode = DataGridViewClipboardCopyMode.EnableAlwaysIncludeHeaderText;
            foreach (string name in new[] { "Inicio", "Acción", "Origen", "Puerto", "Estado", "Resultado" }) history.Columns.Add(name, name);
            foreach (DataGridViewColumn column in history.Columns) column.SortMode = DataGridViewColumnSortMode.NotSortable;
            history.KeyDown += (s, e) => {
                if (e.Control && (e.KeyCode == Keys.C || e.KeyCode == Keys.Insert))
                { e.Handled = e.SuppressKeyPress = true; CopyHistorySelection(); }
            };
            var historyMenu = new ContextMenuStrip();
            historyMenu.Items.Add("Copiar seleccionados", null, (s, e) => CopyHistorySelection()); history.ContextMenuStrip = historyMenu;
            history.CellDoubleClick += (s, e) => {
                if (e.RowIndex < 0) return;
                var job = (JobState)history.Rows[e.RowIndex].Tag;
                using (var detail = new Form { Text = "Trabajo " + job.Id, Size = new Size(900, 600), StartPosition = FormStartPosition.CenterParent })
                {
                    var text = new TextBox { Dock = DockStyle.Fill, Multiline = true, ReadOnly = true, ScrollBars = ScrollBars.Both, WordWrap = false, Font = new Font("Consolas", 10), Text = job.Detail() };
                    var copyDetail = new ActionButton { Text = "Copiar detalle", Dock = DockStyle.Bottom, Height = 38 };
                    copyDetail.Click += (copySender, copyEvent) => CopyText(text.Text, copyDetail);
                    EnableTextCopyFeedback(text); detail.Controls.Add(text); detail.Controls.Add(copyDetail);
                    FocusDialogAction(detail, copyDetail, text); ShowStyledDialog(detail);
                }
            };
            var historyLayout = new TableLayoutPanel { Dock = DockStyle.Fill, ColumnCount = 1, RowCount = 2 };
            historyLayout.RowStyles.Add(new RowStyle(SizeType.Percent, 100)); historyLayout.RowStyles.Add(new RowStyle(SizeType.AutoSize));
            historyLayout.Controls.Add(history, 0, 0);
            var historyActions = UiRow();
            var copyHistory = new ActionButton { Text = "Copiar seleccionados", Width = 185, Height = 32 };
            copyHistory.Click += (s, e) => CopyHistorySelection(); historyActions.Controls.Add(copyHistory);
            foreach (string shortcut in new[] { "Doble clic: detalles", "Ctrl + clic: filas", "Shift + clic: rango", "Ctrl + C: copiar" }) historyActions.Controls.Add(new Label { Text = shortcut, AutoSize = true, ForeColor = Color.Firebrick, BackColor = Color.FromArgb(255, 237, 237), Padding = new Padding(8, 8, 8, 8), Margin = new Padding(8, 3, 0, 0) });
            historyLayout.Controls.Add(historyActions, 0, 1); historyTab.Controls.Add(historyLayout); tabs.TabPages.Add(historyTab);
            timer.Tick += (s, e) => { if (++historyTick % 10 == 0 && tabs.SelectedTab == historyTab) RefreshHistory(); };
            tabs.SelectedIndexChanged += (s, e) => { if (tabs.SelectedTab == historyTab) RefreshHistory(); };

            var heading = configLayout.GetControlFromPosition(0, 0); configLayout.Controls.Remove(heading);
            configLayout.RowStyles[0].Height = 142;
            var searchPanel = new TableLayoutPanel { Dock = DockStyle.Fill, ColumnCount = 1, RowCount = 3 };
            searchPanel.RowStyles.Add(new RowStyle(SizeType.Absolute, 38)); searchPanel.RowStyles.Add(new RowStyle(SizeType.Absolute, 60)); searchPanel.RowStyles.Add(new RowStyle(SizeType.Percent, 100));
            InitializeSavedProfiles(searchPanel); searchPanel.Controls.Add(heading, 0, 1);
            var search = new TextBox { Dock = DockStyle.Top, Text = "" };
            var searchRow = new TableLayoutPanel { Dock = DockStyle.Fill, ColumnCount = 2 }; searchRow.ColumnStyles.Add(new ColumnStyle(SizeType.Absolute, 130)); searchRow.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100));
            searchRow.Controls.Add(new Label { Text = "Filtrar opciones", Dock = DockStyle.Fill }, 0, 0); searchRow.Controls.Add(search, 1, 0);
            searchPanel.Controls.Add(searchRow, 0, 2); configLayout.Controls.Add(searchPanel, 0, 0);
            search.TextChanged += (s, e) => ApplyConfigurationFilter();
            InitializeConfigurationSections(configLayout, search);
            var configButtons = (FlowLayoutPanel)configLayout.GetControlFromPosition(0, 2);
            AddButton(configButtons, "Ver rutas resueltas", ShowResolvedPaths, 185);
            configLayout.Controls.Remove(configButtons);
            var footerLayout = new TableLayoutPanel { Dock = DockStyle.Top, AutoSize = true, ColumnCount = 2, Padding = new Padding(0, 12, 0, 0) };
            footerLayout.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100)); footerLayout.ColumnStyles.Add(new ColumnStyle(SizeType.AutoSize));
            var saveStrip = new ToolStrip { Dock = DockStyle.None, GripStyle = ToolStripGripStyle.Hidden, Margin = new Padding(0, 4, 0, 0) };
            var save = new ToolStripSplitButton("Guardar perfil") { AutoSize = false, Size = new Size(285, UiButtonHeight), DropDownButtonWidth = UiSplitWidth };
            save.ButtonClick += (s, e) => SaveProfile(); save.DropDownItems.Add("Guardar", null, (s, e) => SaveProfile()); save.DropDownItems.Add("Guardar como...", null, (s, e) => SaveProfileAs());
            saveStrip.Items.Add(save); idleControls.Add(saveStrip); configLayout.RowStyles[2].Height = 104; configButtons.WrapContents = true; configButtons.AutoSize = false; configButtons.Dock = DockStyle.Fill; footerLayout.AutoSize = false; footerLayout.Dock = DockStyle.Fill;
            footerLayout.Controls.Add(configButtons, 0, 0); footerLayout.Controls.Add(saveStrip, 1, 0); configLayout.Controls.Add(footerLayout, 0, 2);
            bridge = new LocalBridge(DispatchBridge, Log); bridge.Start();
            Log("MCP local disponible para este usuario; falta registrar/conectar el cliente. Canal: " + LocalBridge.PipeName);
        }
        void ShowResolvedPaths()
        {
            ReadSettings(); var text = new System.Text.StringBuilder("Rutas del perfil visible. Solo consulta archivos/carpetas; no ejecuta herramientas.\r\n\r\n");
            foreach (Field field in Profile.Fields.Where(f => f.Kind != "text"))
            {
                try { string resolved = profile.Resolve(field.Key); bool exists = field.Kind == "folder" ? Directory.Exists(resolved) : File.Exists(resolved); text.AppendLine(field.Label + ": " + (exists ? "EXISTE" : "NO ENCONTRADO") + "\r\n" + resolved + "\r\n"); }
                catch (Exception error) { text.AppendLine(field.Label + ": " + error.Message + "\r\n"); }
            }
            using (var dialog = new Form { Text = "Rutas del perfil", Size = new Size(900, 650), StartPosition = FormStartPosition.CenterParent })
            { dialog.Controls.Add(new TextBox { Dock = DockStyle.Fill, Multiline = true, ReadOnly = true, ScrollBars = ScrollBars.Both, WordWrap = false, Text = text.ToString() }); ShowStyledDialog(dialog); }
        }

        void UpdateConnectionEvidence(string message)
        {
            if (message.StartsWith("Escuchando", StringComparison.OrdinalIgnoreCase) || message.StartsWith("Consultando", StringComparison.OrdinalIgnoreCase))
            { evidencePort = ports.Text; romEvidence = "sin confirmar"; }
            if (message.StartsWith("Modo programación observado", StringComparison.OrdinalIgnoreCase)) bootEvidence = "UART observado a " + DateTime.Now.ToString("HH:mm:ss");
            if (message.IndexOf("ROM responde", StringComparison.OrdinalIgnoreCase) >= 0) romEvidence = "respuesta confirmada a " + DateTime.Now.ToString("HH:mm:ss");
            string wiring = target.SelectedIndex == 0
                ? "ATmega: RESET libre; D0/D1 aislados del ESP; USB-B al Mega."
                : Operations.UsesMegaBridge(profile)
                    ? "ESP por flasher software: RESET Mega LIBRE; D18/TX1→E_RX y E_TX→D19/RX1. Baud fijo " + profile.Get("BridgeBaud") + ". Al terminar usar Flashear Firmware ATmega o el menú Flashear último backup."
                    : "ESP: RESET ATmega → GND; D0 → RX central y D1 → TX central del selector ESP URAT. Al cargar te voy a avisar para que presiones KEY y RST.";
            evidence.Text = wiring + "\r\nEvidencia " + evidencePort + ": arranque " + bootEvidence + " | ROM " + romEvidence + ".\r\n" + (pendingJob == null ? "Los estados observados no prueban el cableado ni que el modo siga vigente después de otro reset." : "Solicitud MCP pendiente: revisar en Codex / MCP. No se ejecutó todavía.");
        }
        void RefreshHistory()
        {
            // Update existing rows instead of rebuilding them every second: native
            // Ctrl/Shift selection, its anchor and the scroll position survive ticks.
            if (history.ContainsFocus && Control.MouseButtons != MouseButtons.None) return;
            var top = history.FirstDisplayedScrollingRowIndex < 0 ? null : history.Rows[history.FirstDisplayedScrollingRowIndex].Tag as JobState;
            for (int i = history.Rows.Count - 1; i >= 0; i--)
                if (!jobs.ContainsKey(((JobState)history.Rows[i].Tag).Id)) history.Rows.RemoveAt(i);
            int index = 0;
            foreach (JobState job in jobs.Values.OrderByDescending(j => j.Created))
            {
                DataGridViewRow row = history.Rows.Cast<DataGridViewRow>().FirstOrDefault(r => ((JobState)r.Tag).Id == job.Id);
                if (row == null) { history.Rows.Insert(index, job.Created, job.Action, job.Origin, job.Port, job.Status, job.Result); row = history.Rows[index]; row.Tag = job; }
                object[] values = { job.Created, job.Action, job.Origin, job.Port, job.Status, job.Result };
                for (int column = 0; column < values.Length; column++)
                    if (!Object.Equals(row.Cells[column].Value, values[column])) row.Cells[column].Value = values[column];
                index++;
            }
            if (top != null)
            {
                var row = history.Rows.Cast<DataGridViewRow>().FirstOrDefault(r => ((JobState)r.Tag).Id == top.Id);
                if (row != null && history.FirstDisplayedScrollingRowIndex != row.Index) history.FirstDisplayedScrollingRowIndex = row.Index;
            }
        }
        void CopyHistorySelection()
        {
            if (history.SelectedRows.Count == 0) return;
            var text = new System.Text.StringBuilder();
            text.AppendLine(String.Join("\t", history.Columns.Cast<DataGridViewColumn>().Select(c => ClipboardCell(c.HeaderText))));
            foreach (DataGridViewRow row in history.SelectedRows.Cast<DataGridViewRow>().OrderBy(r => r.Index))
                text.AppendLine(String.Join("\t", row.Cells.Cast<DataGridViewCell>().Select(c => ClipboardCell(Convert.ToString(c.Value)))));
            CopyText(text.ToString(), history);
        }
        static string ClipboardCell(string value)
        {
            if (value.IndexOfAny(new[] { '\t', '\r', '\n', '"' }) < 0) return value;
            return "\"" + value.Replace("\"", "\"\"") + "\"";
        }
        JobState NewJob(string action, string origin, string port, string state)
        {
            while (jobs.Count >= 100)
            {
                var old = jobs.Values.OrderBy(j => j.Created).FirstOrDefault(j => j != currentJob && j != pendingJob && j != reservedJob);
                if (old == null) throw new InvalidOperationException("No se pudo reservar espacio en el historial.");
                jobs.Remove(old.Id);
            }
            var job = new JobState(action, origin, port, state); jobs[job.Id] = job; return job;
        }
        object DispatchBridge(string json)
        {
            if (IsDisposed || Disposing || !IsHandleCreated) throw new InvalidOperationException("La ventana está cerrando o todavía no está lista.");
            return Invoke(new Func<object>(() => HandleBridge(json)));
        }
        static string Arg(Dictionary<string, object> args, string key, string fallback)
        { object value; return args.TryGetValue(key, out value) ? Convert.ToString(value) : fallback; }
        object ProfileSnapshot()
        {
            ReadSettings();
            return new { values = new Dictionary<string, string>(profile.Values), espMemory = memorySummary.Text, paths = Profile.Fields.Where(f => f.Kind != "text").Select(f => {
                string resolved; try { resolved = profile.Resolve(f.Key); } catch (Exception error) { resolved = "ERROR: " + error.Message; }
                return new { key = f.Key, path = resolved, exists = f.Kind == "folder" ? Directory.Exists(resolved) : File.Exists(resolved) };
            }).ToArray() };
        }
        object HandleBridge(string json)
        {
            var serializer = new JavaScriptSerializer();
            var request = serializer.Deserialize<Dictionary<string, object>>(json);
            string command = Convert.ToString(request["command"]);
            var args = (Dictionary<string, object>)request["arguments"];
            string mcpClientId = ObserveMcpClient(request);
            switch (command)
            {
                case "mcp_client_alive": return new { sessionId = sessionId, observed = true };
                case "mcp_client_closed": ForgetMcpClient(mcpClientId); return new { observed = true };
                case "loader_connection_test": return CompleteMcpConnectionTest(args, mcpClientId);
                case "loader_status": return new { version = LoaderVersion.Value, sessionId = sessionId, busy = busy, acceptsRequests = remoteRequests.Checked,
                    espTransport = profile.Get("EspTransport"), bridgeBaud = profile.Get("BridgeBaud"),
                    backupBeforeFlasher = backupBeforeFlasher.Checked, showNoBackupWarning = preferences == null || preferences.ShowNoMegaBackupWarning,
                    mcpConnection = McpConnectionSnapshot(),
                    autoAcceptRequests = remoteRequests.Checked && autoAcceptRequests.Checked,
                    skipManualChecks = sessionChecksAccepted,
                    espFirmwareMode = profile.Get("EspFirmwareMode"), includeFilesystem = EspFlashRules.UseFilesystem(profile),
                    pendingJobId = pendingJob == null ? null : pendingJob.Id, activeJobId = currentJob == null ? null : currentJob.Id,
                    port = ports.Text, stage = status.Text, lastResult = lastResult.Text, evidencePort = evidencePort, bootEvidence = bootEvidence, romEvidence = romEvidence,
                    serial = serialCapture == null ? null : serialCapture.Snapshot(0, 1), lastFailureReport = lastFailurePath };
                case "loader_ports": return new { ports = System.IO.Ports.SerialPort.GetPortNames(), opensPort = false };
                case "loader_profile": return ProfileSnapshot();
                case "loader_serial":
                    int serialCursor = args.ContainsKey("cursor") ? Convert.ToInt32(args["cursor"]) : 0;
                    int serialLimit = args.ContainsKey("limit") ? Convert.ToInt32(args["limit"]) : 60;
                    if (serialCursor < 0 || serialLimit < 1 || serialLimit > 120) throw new ArgumentException("cursor >= 0; limit entre 1 y 120.");
                    if (serialCapture == null) return new { state = "closed", message = "No se abrió el monitor en esta sesión." };
                    if (args.ContainsKey("captureId") && Arg(args, "captureId", "") != serialCapture.Id) throw new ArgumentException("La captura cambió. Consultá loader_serial sin captureId y con cursor 0.");
                    return serialCapture.Snapshot(serialCursor, serialLimit);
                case "loader_job":
                    JobState found; if (!jobs.TryGetValue(Arg(args, "jobId", ""), out found)) throw new ArgumentException("Trabajo no disponible en esta sesión o fuera de los últimos 100.");
                    int cursor = args.ContainsKey("cursor") ? Convert.ToInt32(args["cursor"]) : 0;
                    int limit = args.ContainsKey("limit") ? Convert.ToInt32(args["limit"]) : 60;
                    if (cursor < 0 || limit < 1 || limit > 120) throw new ArgumentException("cursor >= 0; limit entre 1 y 120.");
                    return found.Snapshot(cursor, limit);
                case "loader_request_cancel":
                    string id = Arg(args, "jobId", "");
                    if (pendingJob != null && pendingJob.Id == id) { RejectPending(); return new { jobId = id, status = "rejected" }; }
                    if (currentJob == null || currentJob.Id != id) throw new ArgumentException("Ese trabajo no está pendiente ni activo.");
                    bool automaticCancel = remoteRequests.Checked && autoAcceptRequests.Checked;
                    BeginInvoke(new Action(() => { if (currentJob != null && currentJob.Id == id) Cancel(automaticCancel); }));
                    return new { jobId = id, status = automaticCancel ? "cancellation_requested" : "cancel_confirmation_requested", cancelled = false };
                case "loader_request_session_options": case "loader_request_profile": case "loader_request_operation": return QueueRequest(command, args, serializer);
                default: throw new ArgumentException("Comando local no disponible.");
            }
        }
        object QueueRequest(string command, Dictionary<string, object> args, JavaScriptSerializer serializer)
        {
            string requestId = Arg(args, "requestId", "");
            if (!Regex.IsMatch(requestId, @"^[a-zA-Z0-9_.:-]{1,128}$")) throw new ArgumentException("requestId: 1 a 128 letras, números o _.:-.");
            string identity = command + ":" + serializer.Serialize(args.OrderBy(kv => kv.Key).ToDictionary(kv => kv.Key, kv => kv.Value));
            string[] previous;
            if (requests.TryGetValue(requestId, out previous))
            {
                if (previous[0] != identity) throw new ArgumentException("requestId ya se usó con otros argumentos.");
                JobState existing;
                return new { jobId = previous[1], status = jobs.TryGetValue(previous[1], out existing) ? existing.Status : "history_expired", duplicate = true, executedByThisRequest = false };
            }
            if (!remoteRequests.Checked) throw new InvalidOperationException("Solicitudes MCP deshabilitadas en la ventana.");
            if (profileDialogs > 0) throw new InvalidOperationException("Hay un diálogo de configuración/cableado abierto. Cerralo antes de iniciar una solicitud MCP.");
            if (serialTransition) throw new InvalidOperationException("El monitor está abriendo/cerrando el puerto. Esperá a que termine antes de proponer una operación.");
            if (busy || pendingJob != null || reservedJob != null) throw new InvalidOperationException("Ya hay un trabajo activo o pendiente. Consultarlo; no se encola otra operación.");
            if (requests.Count >= 1000) throw new InvalidOperationException("Límite de solicitudes de esta sesión. Reiniciá la ventana para comenzar otra sesión.");
            string action, port = "";
            if (command == "loader_request_session_options")
            {
                if (!args.ContainsKey("skipManualChecks") || !(args["skipManualChecks"] is bool)) throw new ArgumentException("skipManualChecks debe ser booleano.");
                action = "session_options";
            }
            else if (command == "loader_request_profile")
            {
                var values = args["values"] as Dictionary<string, object>;
                if (values == null || values.Count == 0) throw new ArgumentException("values debe contener opciones a modificar.");
                foreach (var pair in values) { if (!(pair.Value is string)) throw new ArgumentException("Los valores del perfil deben ser texto."); Profile.ValidateValue(pair.Key, (string)pair.Value); }
                action = "update_profile";
            }
            else
            {
                action = Arg(args, "action", "");
                if (!new[] { "build_mega", "build_esp", "build_web_fs", "build_all", "probe_esp", "flash_mega", "flash_esp", "flash_fs", "flash_esp_fs", "install_mega_flasher", "verify_mega_flasher", "restore_mega_product", "flash_mega_backup" }.Contains(action)) throw new ArgumentException("Acción no admitida.");
                port = Arg(args, "port", ports.Text);
                if (!action.StartsWith("build_") && !Regex.IsMatch(port, @"^COM[1-9][0-9]{0,4}$", RegexOptions.IgnoreCase)) throw new ArgumentException("Elegir un puerto COM válido.");
                if (args.ContainsKey("listenBoot") && !(args["listenBoot"] is bool)) throw new ArgumentException("listenBoot debe ser booleano.");
            }
            pendingJob = NewJob(action, "mcp", port, "awaiting_approval"); pendingArguments = args;
            requests[requestId] = new[] { identity, pendingJob.Id };
            requestDetail.Text = "Trabajo: " + pendingJob.Id + "\r\nAcción: " + action + "\r\nPuerto: " + port + "\r\nFirmware ESP: " + profile.Get("EspFirmwareMode") + " | filesystem: " + EspFlashRules.UseFilesystem(profile) + "\r\nArgumentos:\r\n" + serializer.Serialize(args) + "\r\n\r\nSe usará el perfil visible al aprobar. Revisá sus rutas. La solicitud no ejecutó procesos ni abrió el puerto.";
            if (action == "install_mega_flasher") requestDetail.Text += "\r\nBackup de ATmega antes de cargar: " + (backupBeforeFlasher.Checked ? "ACTIVADO; leer/contrastar antes de escribir, detener si falla." : "DESACTIVADO; no se creará backup nuevo. Se aplica la preferencia del aviso.");
            if (action == "build_web_fs" || action == "build_all") requestDetail.Text += "\r\nPreparación FileSystem (firmware propio): " + profile.Get("FsScriptHost") + "\r\nScript: " + profile.Get("FsScript") + "\r\nArgumentos: " + profile.Get("FsScriptArguments") + "\r\nFuentes: " + profile.Get("FsSourceDir");
            if (action == "update_profile")
            {
                ReadSettings(); requestDetail.Text += "\r\n\r\nCambios de perfil (actual → propuesto):";
                foreach (var pair in (Dictionary<string, object>)args["values"]) requestDetail.Text += "\r\n" + pair.Key + ": " + profile.Get(pair.Key) + " → " + (string)pair.Value;
            }
            UpdateRequestMode();
            reviewRequest.Enabled = rejectRequest.Enabled = true;
            foreach (Control c in idleControls) c.Enabled = false; configMenu.Enabled = false;
            mainTabs.SelectedTab = mcpTab; UpdateConnectionEvidence("");
            JobState queued = pendingJob;
            bool automatic = action != "session_options" && remoteRequests.Checked && autoAcceptRequests.Checked;
            if (action == "session_options") BeginInvoke(new Action(() => { if (pendingJob == queued) { try { ApprovePending(false); } catch (Exception error) { FailPending(error); } } }));
            else if (automatic) ScheduleAutomaticApproval(queued);
            return new { jobId = queued.Id, status = queued.Status, autoAccepted = automatic, executed = false };
        }
        void UpdateRequestMode()
        {
            if (pendingJob != null)
            {
                if (!autoAcceptRequests.Checked && pendingJob.Status == "accepted")
                { pendingJob.Status = "awaiting_approval"; pendingJob.Stage = "Esperando revisión en la ventana"; }
                remoteState.Text = pendingJob.Status == "accepted" ? "Solicitud aceptada automáticamente; preparando la acción" : "Solicitud de Codex pendiente de revisión";
            }
            else remoteState.Text = !remoteRequests.Checked ? "Solicitudes de Codex deshabilitadas" : autoAcceptRequests.Checked
                ? "Aceptación automática activada para esta sesión" : "Sin solicitudes pendientes · revisión manual";
        }
        void ScheduleAutomaticApproval(JobState job)
        {
            if (job.Action == "session_options") return; // Nunca convertir la advertencia local en aprobación automática.
            job.Status = "accepted"; job.Stage = "Aceptada automáticamente; preparando la acción"; UpdateRequestMode();
            BeginInvoke(new Action(() => {
                if (pendingJob != job || busy) return;
                if (!remoteRequests.Checked || !autoAcceptRequests.Checked) { UpdateRequestMode(); return; }
                try { ApprovePending(true); } catch (Exception error) { FailPending(error); }
            }));
        }
        void ClearPending()
        {
            pendingJob = null; pendingArguments = null; reviewRequest.Enabled = rejectRequest.Enabled = false;
            UpdateRequestMode();
            foreach (Control c in idleControls) c.Enabled = !busy; configMenu.Enabled = !busy;
            UpdateConnectionEvidence("");
        }
        void RejectPending()
        {
            if (pendingJob == null) return;
            pendingJob.Status = "rejected"; pendingJob.Result = "Solicitud rechazada; no ejecutada."; pendingJob.Ended = DateTimeOffset.Now.ToString("o");
            requestDetail.Text += "\r\n\r\nRECHAZADA: no se ejecutó."; ClearPending();
        }
        void FailPending(Exception error)
        {
            if (pendingJob != null) { pendingJob.Status = "failed"; pendingJob.Result = error.Message; pendingJob.Ended = DateTimeOffset.Now.ToString("o"); }
            ClearPending(); MessageBox.Show(this, error.Message, "Solicitud MCP", MessageBoxButtons.OK, MessageBoxIcon.Error);
        }
        void ApprovePending(bool automatic)
        {
            if (pendingJob == null || busy) return;
            JobState job = pendingJob; var args = pendingArguments;
            if (job.Action == "session_options") {
                if (job.Status == "awaiting_local_confirmation") return;
                job.Status = "awaiting_local_confirmation"; job.Stage = "Esperando aceptación local de la advertencia de sesión";
                bool applied = SetSessionChecks((bool)args["skipManualChecks"], () => pendingJob == job && !busy);
                if (pendingJob != job) return;
                job.ApprovalMode = "local_session_warning";
                job.Status = applied ? "completed" : "rejected";
                job.Result = applied ? "Omitir confirmaciones manuales: " + sessionChecksAccepted : "Advertencia rechazada; no se activó la opción.";
                job.Ended = DateTimeOffset.Now.ToString("o"); job.Add(job.Result); ClearPending(); return;
            }
            if (!automatic && MessageBox.Show(this, requestDetail.Text + "\r\n\r\n¿Aprobar esta solicitud?", "Revisión de solicitud MCP", MessageBoxButtons.YesNo, MessageBoxIcon.Question) != DialogResult.Yes) return;
            // A cancellation may arrive through the bridge while the modal review is open.
            if (pendingJob != job || busy) return;
            job.ApprovalMode = automatic ? "automatic" : "manual";
            job.Add(DateTime.Now.ToString("HH:mm:ss") + (automatic ? " Solicitud autoaceptada por la opción activada en la ventana." : " Solicitud aprobada manualmente en la ventana."));
            requestDetail.AppendText(automatic ? "\r\n\r\nACEPTADA AUTOMÁTICAMENTE: iniciando la acción solicitada." : "\r\n\r\nAPROBADA EN LA VENTANA.");
            if (job.Action == "update_profile")
            {
                ReadSettings(); Profile next = profile.Copy();
                foreach (var pair in (Dictionary<string, object>)args["values"]) next.Values[pair.Key] = (string)pair.Value;
                SaveRemoteProfile(next);
                job.Status = "completed"; job.Result = automatic ? "Perfil aplicado y guardado por aceptación automática." : "Perfil aplicado y guardado por aprobación en la ventana."; job.Ended = DateTimeOffset.Now.ToString("o");
                ClearPending(); Log(job.Result); return;
            }
            bool shouldListen = !args.ContainsKey("listenBoot") || (bool)args["listenBoot"];
            if (job.Action == "build_mega" || job.Action == "flash_mega" || job.Action == "install_mega_flasher" || job.Action == "verify_mega_flasher" || job.Action == "restore_mega_product" || job.Action == "flash_mega_backup") target.SelectedIndex = 0;
            else if (job.Action == "build_web_fs" || job.Action == "flash_fs") target.SelectedIndex = 2;
            else if (job.Action == "flash_esp_fs" || job.Action == "build_all") target.SelectedIndex = 3;
            else target.SelectedIndex = 1;
            listen.Checked = shouldListen;
            if (!String.IsNullOrEmpty(job.Port)) { if (!ports.Items.Contains(job.Port)) ports.Items.Add(job.Port); ports.SelectedItem = job.Port; }
            reservedJob = job; ClearPending();
            Execute(job.Action, async (ops, p, port, token) => {
                Log("Aprobación MCP: " + (automatic ? "automática, activada en la ventana" : "manual"));
                switch (job.Action)
                {
                    case "build_mega": await ops.BuildMega(p, token); break;
                    case "build_esp": await ops.BuildEsp(p, token); break;
                    case "build_web_fs": await ops.BuildWebFs(p, token); break;
                    case "build_all": await ops.BuildAll(p, token); break;
                    case "probe_esp": await ops.PrepareEspSession(p, port, false, shouldListen, token); await ops.Probe(p, port, token); break;
                    case "flash_mega": await ops.FlashMega(p, port, token); break;
                    case "verify_mega_flasher": await ops.VerifyMegaFlasher(p, port, token); break;
                    case "install_mega_flasher": await InstallFlasherCore(ops, p, port, token); break;
                    case "restore_mega_product": await ops.RestoreMegaProduct(p, port, token); break;
                    case "flash_mega_backup": await ops.FlashLatestMegaBackup(p, port, token); break;
                    case "flash_esp": await ops.FlashEsp(p, port, true, false, shouldListen, token); break;
                    case "flash_fs": await ops.FlashEsp(p, port, false, true, shouldListen, token); break;
                    case "flash_esp_fs": await ops.FlashEsp(p, port, true, true, shouldListen, token); break;
                    default: throw new InvalidOperationException("Acción no implementada.");
                }
            }, !job.Action.StartsWith("build_", StringComparison.Ordinal));
        }
    }
}
