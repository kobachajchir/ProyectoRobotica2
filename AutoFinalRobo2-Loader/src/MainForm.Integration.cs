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
        readonly CheckBox remoteRequests = new CheckBox { Text = "Aceptar solicitudes de Codex para revisión en esta ventana", Checked = true, AutoSize = true };
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
            mainTabs = tabs; footer.Controls.Add(followLog);
            layout.RowStyles[3].Height = 165;
            var box = (TableLayoutPanel)layout.GetControlFromPosition(0, 3);
            box.RowCount = 3; box.RowStyles.Add(new RowStyle(SizeType.Absolute, 25));
            box.RowStyles.Add(new RowStyle(SizeType.Absolute, 40)); box.RowStyles.Add(new RowStyle(SizeType.Percent, 100));
            box.Controls.Add(evidence, 0, 2);
            target.SelectedIndexChanged += (s, e) => UpdateConnectionEvidence("");
            ports.SelectedIndexChanged += (s, e) => { bootEvidence = "sin observar"; romEvidence = "sin consultar"; evidencePort = ports.Text; UpdateConnectionEvidence(""); };
            UpdateConnectionEvidence("");

            mcpTab = new TabPage("Codex / MCP") { Padding = new Padding(16) }; tabs.TabPages.Add(mcpTab);
            var mcpLayout = new TableLayoutPanel { Dock = DockStyle.Fill, ColumnCount = 1, RowCount = 7 };
            foreach (int height in new[] { 58, 68, 40, 48 }) mcpLayout.RowStyles.Add(new RowStyle(SizeType.Absolute, height));
            mcpLayout.RowStyles.Add(new RowStyle(SizeType.Percent, 100)); mcpLayout.RowStyles.Add(new RowStyle(SizeType.Absolute, 45)); mcpLayout.RowStyles.Add(new RowStyle(SizeType.Absolute, 65));
            mcpLayout.Controls.Add(new Label { Dock = DockStyle.Fill, Text = "Una ventana, una operación, el mismo registro.\r\nCodex consulta la sesión y propone operaciones. Revisá la solicitud aquí; los asistentes de cableado siguen activos.", Font = new Font(Font, FontStyle.Bold) }, 0, 0);
            string command = "codex mcp add afr2-loader -- powershell.exe -NoProfile -NonInteractive -ExecutionPolicy Bypass -File \"" + Path.Combine(appDir, "Mcp.ps1") + "\"";
            var commandBox = new TextBox { Dock = DockStyle.Fill, Text = command, ReadOnly = true, Multiline = true, BackColor = Color.White };
            mcpLayout.Controls.Add(commandBox, 0, 1);
            var commandButtons = new FlowLayoutPanel { Dock = DockStyle.Fill };
            var copyCommand = new Button { Text = "Copiar comando MCP", Width = 185, Height = 32 };
            copyCommand.Click += (s, e) => Clipboard.SetText(command); commandButtons.Controls.Add(copyCommand);
            remoteRequests.Margin = new Padding(10, 8, 0, 0); commandButtons.Controls.Add(remoteRequests);
            mcpLayout.Controls.Add(commandButtons, 0, 2); mcpLayout.Controls.Add(remoteState, 0, 3); mcpLayout.Controls.Add(requestDetail, 0, 4);
            var approvalButtons = new FlowLayoutPanel { Dock = DockStyle.Fill };
            reviewRequest = new Button { Text = "Revisar y continuar...", Width = 190, Height = 34, Enabled = false };
            rejectRequest = new Button { Text = "Rechazar solicitud", Width = 175, Height = 34, Enabled = false };
            reviewRequest.Click += (s, e) => { try { ApprovePending(); } catch (Exception error) { FailPending(error); } };
            rejectRequest.Click += (s, e) => RejectPending(); approvalButtons.Controls.Add(reviewRequest); approvalButtons.Controls.Add(rejectRequest);
            mcpLayout.Controls.Add(approvalButtons, 0, 5);
            mcpLayout.Controls.Add(new Label { Dock = DockStyle.Fill, Text = "El comando registra el servidor cuando lo ejecutás. Mantené esta ventana abierta y reiniciá o reconectá el cliente MCP después de registrarlo.\r\nNo se abre ningún COM al iniciar. Desmarcar solicitudes bloquea acciones nuevas; las consultas siguen disponibles. El historial corresponde a esta sesión." }, 0, 6);
            mcpTab.Controls.Add(mcpLayout); remoteState.Text = "Sin solicitudes pendientes";

            var historyTab = new TabPage("Historial de sesión") { Padding = new Padding(12) };
            history.Dock = DockStyle.Fill; history.ReadOnly = true; history.AllowUserToAddRows = false; history.AllowUserToDeleteRows = false;
            history.RowHeadersVisible = false; history.AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.Fill;
            foreach (string name in new[] { "Inicio", "Acción", "Origen", "Puerto", "Estado", "Resultado" }) history.Columns.Add(name, name);
            history.CellDoubleClick += (s, e) => {
                if (e.RowIndex < 0) return;
                var job = (JobState)history.Rows[e.RowIndex].Tag;
                using (var detail = new Form { Text = "Trabajo " + job.Id, Size = new Size(900, 600), StartPosition = FormStartPosition.CenterParent })
                { detail.Controls.Add(new TextBox { Dock = DockStyle.Fill, Multiline = true, ReadOnly = true, ScrollBars = ScrollBars.Both, WordWrap = false, Font = new Font("Consolas", 10), Text = job.Detail() }); detail.ShowDialog(this); }
            };
            historyTab.Controls.Add(history); tabs.TabPages.Add(historyTab);
            timer.Tick += (s, e) => { if (++historyTick % 10 == 0 && tabs.SelectedTab == historyTab) RefreshHistory(); };
            tabs.SelectedIndexChanged += (s, e) => { if (tabs.SelectedTab == historyTab) RefreshHistory(); };

            var heading = configLayout.GetControlFromPosition(0, 0); configLayout.Controls.Remove(heading);
            configLayout.RowStyles[0].Height = 100;
            var searchPanel = new TableLayoutPanel { Dock = DockStyle.Fill, ColumnCount = 1, RowCount = 2 };
            searchPanel.RowStyles.Add(new RowStyle(SizeType.Absolute, 60)); searchPanel.RowStyles.Add(new RowStyle(SizeType.Percent, 100));
            searchPanel.Controls.Add(heading, 0, 0);
            var search = new TextBox { Dock = DockStyle.Top, Text = "" };
            var searchRow = new TableLayoutPanel { Dock = DockStyle.Fill, ColumnCount = 2 }; searchRow.ColumnStyles.Add(new ColumnStyle(SizeType.Absolute, 130)); searchRow.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100));
            searchRow.Controls.Add(new Label { Text = "Filtrar opciones", Dock = DockStyle.Fill }, 0, 0); searchRow.Controls.Add(search, 1, 0);
            searchPanel.Controls.Add(searchRow, 0, 1); configLayout.Controls.Add(searchPanel, 0, 0);
            search.TextChanged += (s, e) => {
                settings.EndEdit(); settings.CurrentCell = null;
                foreach (DataGridViewRow row in settings.Rows)
                { var field = (Field)row.Tag; row.Visible = (field.Key + " " + field.Label + " " + field.Hint).IndexOf(search.Text, StringComparison.CurrentCultureIgnoreCase) >= 0; }
            };
            var configButtons = (FlowLayoutPanel)configLayout.GetControlFromPosition(0, 2);
            AddButton(configButtons, "Ver rutas resueltas", ShowResolvedPaths, 165);
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
            { dialog.Controls.Add(new TextBox { Dock = DockStyle.Fill, Multiline = true, ReadOnly = true, ScrollBars = ScrollBars.Both, WordWrap = false, Text = text.ToString() }); dialog.ShowDialog(this); }
        }

        void UpdateConnectionEvidence(string message)
        {
            if (message.StartsWith("Escuchando", StringComparison.OrdinalIgnoreCase) || message.StartsWith("Consultando", StringComparison.OrdinalIgnoreCase))
            { evidencePort = ports.Text; romEvidence = "sin confirmar"; }
            if (message.StartsWith("Modo programación observado", StringComparison.OrdinalIgnoreCase)) bootEvidence = "UART observado a " + DateTime.Now.ToString("HH:mm:ss");
            if (message.IndexOf("ROM responde", StringComparison.OrdinalIgnoreCase) >= 0) romEvidence = "respuesta confirmada a " + DateTime.Now.ToString("HH:mm:ss");
            string wiring = target.SelectedIndex == 0
                ? "ATmega: RESET libre; D0/D1 aislados del ESP; USB-B al Mega."
                : "ESP: RESET Mega a GND; D0/RX0 → E_RX central; E_TX central → D1/TX0; retirar D18/D19. KEY + RST del shield.";
            evidence.Text = wiring + "\r\nEvidencia " + evidencePort + ": arranque " + bootEvidence + " | ROM " + romEvidence + ".\r\n" + (pendingJob == null ? "Los estados observados no prueban el cableado ni que el modo siga vigente después de otro reset." : "Solicitud MCP pendiente: revisar en Codex / MCP. No se ejecutó todavía.");
        }
        void RefreshHistory()
        {
            history.Rows.Clear();
            foreach (JobState job in jobs.Values.OrderByDescending(j => j.Created))
            { int row = history.Rows.Add(job.Created, job.Action, job.Origin, job.Port, job.Status, job.Result); history.Rows[row].Tag = job; }
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
            return new { values = new Dictionary<string, string>(profile.Values), paths = Profile.Fields.Where(f => f.Kind != "text").Select(f => {
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
            switch (command)
            {
                case "loader_status": return new { version = LoaderVersion.Value, sessionId = sessionId, busy = busy, acceptsRequests = remoteRequests.Checked,
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
                    BeginInvoke(new Action(Cancel)); return new { jobId = id, status = "cancel_confirmation_requested", cancelled = false };
                case "loader_request_profile": case "loader_request_operation": return QueueRequest(command, args, serializer);
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
            if (serialTransition) throw new InvalidOperationException("El monitor está abriendo/cerrando el puerto. Esperá a que termine antes de proponer una operación.");
            if (busy || pendingJob != null || reservedJob != null) throw new InvalidOperationException("Ya hay un trabajo activo o pendiente. Consultarlo; no se encola otra operación.");
            if (requests.Count >= 1000) throw new InvalidOperationException("Límite de solicitudes de esta sesión. Reiniciá la ventana para comenzar otra sesión.");
            string action, port = "";
            if (command == "loader_request_profile")
            {
                var values = args["values"] as Dictionary<string, object>;
                if (values == null || values.Count == 0) throw new ArgumentException("values debe contener opciones a modificar.");
                foreach (var pair in values) { if (!(pair.Value is string)) throw new ArgumentException("Los valores del perfil deben ser texto."); Profile.ValidateValue(pair.Key, (string)pair.Value); }
                action = "update_profile";
            }
            else
            {
                action = Arg(args, "action", "");
                if (!new[] { "build_mega", "build_esp", "build_web_fs", "build_all", "probe_esp", "flash_mega", "flash_esp", "flash_fs", "flash_esp_fs" }.Contains(action)) throw new ArgumentException("Acción no admitida.");
                port = Arg(args, "port", ports.Text);
                if (!action.StartsWith("build_") && !Regex.IsMatch(port, @"^COM[1-9][0-9]{0,4}$", RegexOptions.IgnoreCase)) throw new ArgumentException("Elegir un puerto COM válido.");
                if (args.ContainsKey("listenBoot") && !(args["listenBoot"] is bool)) throw new ArgumentException("listenBoot debe ser booleano.");
            }
            pendingJob = NewJob(action, "mcp", port, "awaiting_approval"); pendingArguments = args;
            requests[requestId] = new[] { identity, pendingJob.Id };
            requestDetail.Text = "Trabajo: " + pendingJob.Id + "\r\nAcción: " + action + "\r\nPuerto: " + port + "\r\nArgumentos:\r\n" + serializer.Serialize(args) + "\r\n\r\nSe usará el perfil visible al aprobar. Revisá sus rutas. La solicitud no ejecutó procesos ni abrió el puerto.";
            if (action == "update_profile")
            {
                ReadSettings(); requestDetail.Text += "\r\n\r\nCambios de perfil (actual → propuesto):";
                foreach (var pair in (Dictionary<string, object>)args["values"]) requestDetail.Text += "\r\n" + pair.Key + ": " + profile.Get(pair.Key) + " → " + (string)pair.Value;
            }
            remoteState.Text = "Solicitud de Codex pendiente de revisión";
            reviewRequest.Enabled = rejectRequest.Enabled = true;
            foreach (Control c in idleControls) c.Enabled = false; configMenu.Enabled = false;
            mainTabs.SelectedTab = mcpTab; UpdateConnectionEvidence("");
            return new { jobId = pendingJob.Id, status = "awaiting_approval", executed = false };
        }
        void ClearPending()
        {
            pendingJob = null; pendingArguments = null; reviewRequest.Enabled = rejectRequest.Enabled = false;
            remoteState.Text = "Sin solicitudes pendientes";
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
        void ApprovePending()
        {
            if (pendingJob == null || busy) return;
            JobState job = pendingJob; var args = pendingArguments;
            if (MessageBox.Show(this, requestDetail.Text + "\r\n\r\n¿Aprobar esta solicitud?", "Revisión de solicitud MCP", MessageBoxButtons.YesNo, MessageBoxIcon.Question) != DialogResult.Yes) return;
            if (job.Action == "update_profile")
            {
                ReadSettings(); Profile next = profile.Copy();
                foreach (var pair in (Dictionary<string, object>)args["values"]) next.Values[pair.Key] = (string)pair.Value;
                next.Save(profilePath); profile = next; FillSettings();
                job.Status = "completed"; job.Result = "Perfil aplicado y guardado por aprobación en la ventana."; job.Ended = DateTimeOffset.Now.ToString("o");
                ClearPending(); Log(job.Result); return;
            }
            bool shouldListen = !args.ContainsKey("listenBoot") || (bool)args["listenBoot"];
            if (job.Action == "build_mega" || job.Action == "flash_mega") target.SelectedIndex = 0;
            else if (job.Action == "build_web_fs" || job.Action == "flash_fs") target.SelectedIndex = 2;
            else if (job.Action == "flash_esp_fs" || job.Action == "build_all") target.SelectedIndex = 3;
            else target.SelectedIndex = 1;
            listen.Checked = shouldListen;
            if (!String.IsNullOrEmpty(job.Port)) { if (!ports.Items.Contains(job.Port)) ports.Items.Add(job.Port); ports.SelectedItem = job.Port; }
            reservedJob = job; ClearPending();
            Execute(job.Action, async (ops, p, port, token) => {
                switch (job.Action)
                {
                    case "build_mega": await ops.BuildMega(p, token); break;
                    case "build_esp": await ops.BuildEsp(p, token); break;
                    case "build_web_fs": await ops.BuildWebFs(p, token); break;
                    case "build_all": await ops.BuildMega(p, token); await ops.BuildEsp(p, token); await ops.BuildWebFs(p, token); break;
                    case "probe_esp": if (!ops.PrepareEsp(false)) throw new OperationCanceledException(); if (shouldListen) await ops.ListenBoot(p, port, token); await ops.Probe(p, port, token); break;
                    case "flash_mega": await ops.FlashMega(p, port, token); break;
                    case "flash_esp": await ops.FlashEsp(p, port, true, false, shouldListen, token); break;
                    case "flash_fs": await ops.FlashEsp(p, port, false, true, shouldListen, token); break;
                    case "flash_esp_fs": await ops.FlashEsp(p, port, true, true, shouldListen, token); break;
                    default: throw new InvalidOperationException("Acción no implementada.");
                }
            }, !job.Action.StartsWith("build_", StringComparison.Ordinal));
        }
    }
}
