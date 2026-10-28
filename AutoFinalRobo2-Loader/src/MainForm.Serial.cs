using System;
using System.Collections.Concurrent;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace AFR2Loader
{
    public sealed partial class MainForm
    {
        readonly ComboBox serialPorts = new ComboBox(), serialBaud = new ComboBox(), serialCodec = new ComboBox();
        readonly LogTextBox serialText = new LogTextBox();
        readonly CheckBox serialFollow = new CheckBox { Text = "Seguir texto", AutoSize = true, Checked = true };
        readonly CheckBox serialPause = new CheckBox { Text = "Pausar vista", AutoSize = true };
        readonly Label serialState = new Label { Dock = DockStyle.Fill }, serialObservation = new Label { Dock = DockStyle.Fill };
        readonly Label transferTitle = new Label { Dock = DockStyle.Fill, Font = new Font("Segoe UI", 10, FontStyle.Bold), TextAlign = ContentAlignment.MiddleLeft, Margin = new Padding(0) };
        readonly TextBox transferSummary = new TextBox { Dock = DockStyle.Fill, Multiline = true, ReadOnly = true, BorderStyle = BorderStyle.None, ScrollBars = ScrollBars.Vertical, TabStop = false, Margin = new Padding(0, 3, 0, 0) };
        readonly TableLayoutPanel transferCard = new TableLayoutPanel { Dock = DockStyle.Fill, ColumnCount = 2, RowCount = 2, Padding = new Padding(12), Margin = new Padding(0, 4, 0, 8) };
        readonly DataGridView notices = new DataGridView();
        readonly TextBox noticeDetail = new TextBox { Dock = DockStyle.Fill, Multiline = true, ReadOnly = true, ScrollBars = ScrollBars.Vertical, BackColor = Color.White };
        readonly ConcurrentQueue<LoaderNotice> noticeQueue = new ConcurrentQueue<LoaderNotice>();
        readonly PictureBox transferIcon = new PictureBox { SizeMode = PictureBoxSizeMode.Zoom, Size = new Size(24, 24), Anchor = AnchorStyles.Top | AnchorStyles.Left, Margin = new Padding(0, 0, 10, 0), TabStop = false };
        readonly Image infoIcon = SystemIcons.Information.ToBitmap(), warningIcon = SystemIcons.Warning.ToBitmap(), errorIcon = SystemIcons.Error.ToBitmap();
        SerialCapture serialCapture;
        TabPage serialEventsPage;
        Button serialOpen, serialClose, serialApply;
        bool serialTransition, closingAfterSerial;
        string lastFailureText, lastFailurePath;
        long shownSkipped;

        void InitializeSerial(TabControl tabs)
        {
            var page = new TabPage("Monitor serie") { Padding = new Padding(12) };
            var layout = new TableLayoutPanel { Dock = DockStyle.Fill, ColumnCount = 1, RowCount = 6 };
            foreach (int height in new[] { 84, 58, 124 }) layout.RowStyles.Add(new RowStyle(SizeType.Absolute, height));
            layout.RowStyles.Add(new RowStyle(SizeType.Percent, 100)); layout.RowStyles.Add(new RowStyle(SizeType.Absolute, 72)); layout.RowStyles.Add(new RowStyle(SizeType.Absolute, 55));
            var controls = new FlowLayoutPanel { Dock = DockStyle.Fill, WrapContents = true };
            controls.Controls.Add(new Label { Text = "COM", AutoSize = true, Margin = new Padding(0, 9, 5, 0) });
            serialPorts.Width = 105; serialPorts.DropDownStyle = ComboBoxStyle.DropDownList; controls.Controls.Add(serialPorts);
            var refresh = new Button { Text = "Actualizar", Width = 95, Height = 29 }; refresh.Click += (s, e) => { try { RefreshSerialPorts(); } catch (Exception error) { QueueFailure("monitor", error); } }; controls.Controls.Add(refresh);
            controls.Controls.Add(new Label { Text = "Baudios", AutoSize = true, Margin = new Padding(10, 9, 5, 0) });
            serialBaud.Width = 110; serialBaud.Items.AddRange(new object[] { "9600", "19200", "38400", "57600", "74880", "115200", "230400", "460800", "921600" }); serialBaud.Text = "74880"; controls.Controls.Add(serialBaud);
            controls.Controls.Add(new Label { Text = "Codificación", AutoSize = true, Margin = new Padding(10, 9, 5, 0) });
            serialCodec.Width = 95; serialCodec.DropDownStyle = ComboBoxStyle.DropDownList; serialCodec.Items.AddRange(new object[] { "UTF-8", "ASCII", "Latin-1" }); serialCodec.SelectedIndex = 0; controls.Controls.Add(serialCodec);
            controls.SetFlowBreak(serialCodec, true);
            serialOpen = new Button { Text = "Abrir puerto", Width = 125, Height = 30 }; serialOpen.Click += async (s, e) => await OpenSerialMonitor(); controls.Controls.Add(serialOpen);
            serialClose = new Button { Text = "Cerrar puerto", Width = 125, Height = 30 }; serialClose.Click += async (s, e) => { try { await StopSerialMonitor("Cierre manual"); } catch (Exception error) { QueueFailure("monitor", error); } }; controls.Controls.Add(serialClose);
            serialApply = new Button { Text = "Aplicar baudios", Width = 145, Height = 30 }; serialApply.Click += async (s, e) => await ApplySerialBaud(); controls.Controls.Add(serialApply);
            layout.Controls.Add(controls, 0, 0);
            var statePanel = new TableLayoutPanel { Dock = DockStyle.Fill, ColumnCount = 1, RowCount = 2 };
            serialState.Font = new Font(Font, FontStyle.Bold); statePanel.Controls.Add(serialState, 0, 0); statePanel.Controls.Add(serialObservation, 0, 1); layout.Controls.Add(statePanel, 0, 1);
            transferCard.ColumnStyles.Add(new ColumnStyle(SizeType.Absolute, 36)); transferCard.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100));
            transferCard.RowStyles.Add(new RowStyle(SizeType.Absolute, 26)); transferCard.RowStyles.Add(new RowStyle(SizeType.Percent, 100));
            transferIcon.Image = infoIcon; transferTitle.Text = "Estado de la sesión";
            transferSummary.Text = "Sin carga en curso. El texto recibido no demuestra una carga verificada.";
            transferCard.BackColor = transferSummary.BackColor = Color.FromArgb(232, 241, 246);
            transferTitle.ForeColor = transferSummary.ForeColor = Color.FromArgb(22, 79, 99);
            transferCard.Controls.Add(transferIcon, 0, 0); transferCard.Controls.Add(transferTitle, 1, 0); transferCard.Controls.Add(transferSummary, 1, 1);
            EnableTextCopyFeedback(transferSummary); layout.Controls.Add(transferCard, 0, 2);
            serialText.Dock = DockStyle.Fill; serialText.ReadOnly = true; serialText.WordWrap = false; serialText.DetectUrls = false;
            serialText.BackColor = Color.FromArgb(19, 29, 39); serialText.ForeColor = Color.FromArgb(222, 232, 241); serialText.Font = new Font("Consolas", 10);
            serialText.BindFollow(serialFollow); EnableTextCopyFeedback(serialText);
            layout.Controls.Add(serialText, 0, 3);
            var buttons = new FlowLayoutPanel { Dock = DockStyle.Fill };
            buttons.Controls.Add(serialFollow); buttons.Controls.Add(serialPause);
            var clear = new Button { Text = "Limpiar vista", Width = 120, Height = 30 }; clear.Click += (s, e) => serialText.Clear(); buttons.Controls.Add(clear);
            var copy = new Button { Text = "Copiar texto", Width = 120, Height = 30 }; copy.Click += (s, e) => CopyText(serialText.Text, copy); buttons.Controls.Add(copy);
            var files = new Button { Text = "Abrir registros", Width = 125, Height = 30 }; files.Click += (s, e) => OpenLogs(); buttons.Controls.Add(files);
            var failure = new Button { Text = "Ver último error", Width = 140, Height = 30 }; failure.Click += (s, e) => ShowLastFailure(); buttons.Controls.Add(failure);
            layout.Controls.Add(buttons, 0, 4);
            layout.Controls.Add(new Label { Dock = DockStyle.Fill, Text = "Lectura 8N1, DTR/RTS inactivos. ESP arranque: 74880; aplicación: elegir los baudios de su firmware. Abrir el COM puede afectar reset según el adaptador.\r\nAntes de diagnosticar/cargar se cierra este monitor. No hay reapertura automática después de flashear ni cambio automático de baudios." }, 0, 5);
            page.Controls.Add(layout); tabs.TabPages.Add(page);

            var eventsPage = new TabPage("Eventos y errores") { Padding = new Padding(12) };
            serialEventsPage = eventsPage;
            var eventsLayout = new TableLayoutPanel { Dock = DockStyle.Fill, ColumnCount = 1, RowCount = 3 };
            eventsLayout.RowStyles.Add(new RowStyle(SizeType.Percent, 60)); eventsLayout.RowStyles.Add(new RowStyle(SizeType.Percent, 40)); eventsLayout.RowStyles.Add(new RowStyle(SizeType.Absolute, 42));
            notices.Dock = DockStyle.Fill; notices.ReadOnly = true; notices.AllowUserToAddRows = false; notices.AllowUserToDeleteRows = false; notices.RowHeadersVisible = false;
            notices.SelectionMode = DataGridViewSelectionMode.FullRowSelect; notices.MultiSelect = false; notices.AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.Fill;
            foreach (string name in new[] { "Hora", "Origen", "Estado", "Mensaje" }) notices.Columns.Add(name, name);
            notices.Columns[3].FillWeight = 300;
            notices.SelectionChanged += (s, e) => { if (notices.SelectedRows.Count > 0) { var n = notices.SelectedRows[0].Tag as LoaderNotice; if (n != null) noticeDetail.Text = n.At + " | " + n.Origin + " | " + n.Code + "\r\n" + n.Message + "\r\n\r\n" + n.Detail; } };
            eventsLayout.Controls.Add(notices, 0, 0); eventsLayout.Controls.Add(noticeDetail, 0, 1);
            var eventButtons = new FlowLayoutPanel { Dock = DockStyle.Fill };
            var lastError = new Button { Text = "Ver último error", Width = 150, Height = 32 }; lastError.Click += (s, e) => ShowLastFailure(); eventButtons.Controls.Add(lastError);
            var copyError = new Button { Text = "Copiar último error", Width = 170, Height = 32 }; copyError.Click += (s, e) => CopyText(lastFailureText, copyError); eventButtons.Controls.Add(copyError);
            EnableTextCopyFeedback(noticeDetail); noticeDetail.TabStop = false;
            eventsLayout.Controls.Add(eventButtons, 0, 2); eventsPage.Controls.Add(eventsLayout); tabs.TabPages.Add(eventsPage);
            timer.Tick += (s, e) => { DrainNotices(); DrainSerial(); UpdateSerialControls(); };
            ports.SelectedIndexChanged += (s, e) => { if (serialCapture == null || !serialCapture.Active) { if (serialPorts.Items.Contains(ports.Text)) serialPorts.SelectedItem = ports.Text; } };
            RefreshSerialPorts(); UpdateSerialControls();
            FormClosed += (s, e) => { infoIcon.Dispose(); warningIcon.Dispose(); errorIcon.Dispose(); };
        }
        void RefreshSerialPorts()
        {
            if (serialCapture != null && serialCapture.Active) return;
            string selected = serialPorts.Text; serialPorts.Items.Clear();
            serialPorts.Items.AddRange(System.IO.Ports.SerialPort.GetPortNames().OrderBy(p => p.Length).ThenBy(p => p).Cast<object>().ToArray());
            if (serialPorts.Items.Contains(selected)) serialPorts.SelectedItem = selected;
            else if (serialPorts.Items.Contains(ports.Text)) serialPorts.SelectedItem = ports.Text;
            else if (serialPorts.Items.Count > 0) serialPorts.SelectedIndex = 0;
        }
        int ReadSerialBaud()
        {
            int baud;
            if (!Int32.TryParse(serialBaud.Text, out baud) || baud < 300 || baud > 2000000) throw new ArgumentException("Ingresá baudios entre 300 y 2000000; para arranque del ESP usá 74880.");
            return baud;
        }
        async Task OpenSerialMonitor()
        {
            if (busy || pendingJob != null || serialTransition || (serialCapture != null && serialCapture.Active)) return;
            SerialCapture previous = serialCapture;
            try { await StartSerialCapture(serialPorts.Text, ReadSerialBaud(), serialCodec.Text); }
            catch (Exception error) { if (serialCapture == previous || serialCapture == null || serialCapture.State != "failed") QueueFailure("monitor", error); }
        }
        async Task StartSerialCapture(string port, int baud, string codec)
        {
            serialTransition = true; UpdateSerialControls();
            try
            {
                DrainSerial(); serialPause.Checked = false; shownSkipped = 0;
                serialCapture = new SerialCapture(port, baud, codec, QueueNotice);
                serialText.AppendBatch("\r\n--- " + DateTime.Now.ToString("HH:mm:ss") + " | " + port + " | " + baud + " | " + codec + " ---\r\n");
                await serialCapture.OpenAsync();
            }
            finally { serialTransition = false; UpdateSerialControls(); }
        }
        async Task ApplySerialBaud()
        {
            if (busy || pendingJob != null || serialTransition) return;
            try
            {
                int baud = ReadSerialBaud();
                if (serialCapture == null || !serialCapture.Active) { QueueNotice(new LoaderNotice { Origin = "monitor", Code = "configured", Message = "Baudios seleccionados: " + baud + ". Pulsá Abrir puerto para leer." }); return; }
                string port = serialCapture.Port, codec = serialCapture.Codec;
                await StopSerialMonitor("Cambio de baudios a " + baud);
                if (!busy && pendingJob == null) await StartSerialCapture(port, baud, codec);
            }
            catch (Exception error) { QueueFailure("monitor", error); }
        }
        async Task StopSerialMonitor(string reason)
        {
            if (serialCapture == null) return;
            if (serialTransition) throw new InvalidOperationException("El monitor está abriendo/cerrando; esperá que termine antes de continuar.");
            serialTransition = true; UpdateSerialControls();
            try { await serialCapture.CloseAsync(reason); }
            finally { serialTransition = false; DrainSerial(); UpdateSerialControls(); }
        }
        void UpdateSerialControls()
        {
            bool active = serialCapture != null && serialCapture.Active;
            serialOpen.Enabled = !busy && pendingJob == null && !serialTransition && !active;
            serialClose.Enabled = active && !serialTransition;
            serialApply.Enabled = !busy && pendingJob == null && !serialTransition;
            serialPorts.Enabled = !busy && pendingJob == null && !serialTransition && !active;
            serialBaud.Enabled = !busy && pendingJob == null && !serialTransition;
            serialCodec.Enabled = !busy && pendingJob == null && !serialTransition && !active;
            string stateName = serialCapture == null ? "cerrado" : serialCapture.State == "open" ? "ABIERTO" : serialCapture.State == "opening" ? "ABRIENDO" : serialCapture.State == "closing" ? "CERRANDO" : serialCapture.State == "failed" ? "FALLÓ" : "CERRADO";
            serialState.Text = serialCapture == null ? "Puerto cerrado. Sin captura." : serialCapture.Port + " | " + serialCapture.Baud + " baudios | " + serialCapture.Codec + " | " + stateName + (serialPause.Checked ? " | VISTA EN PAUSA; archivo sigue grabando mientras esté abierto" : "");
            if (serialCapture == null) serialObservation.Text = "Sin observaciones de arranque. No se abre el puerto al iniciar.";
            serialState.ForeColor = serialCapture != null && serialCapture.State == "failed" ? Color.Firebrick : Color.FromArgb(22, 79, 99);
        }
        void DrainSerial()
        {
            if (serialCapture == null) return;
            if (!serialPause.Checked)
            {
                var batch = new StringBuilder(); string chunk; int count = 0;
                while (count++ < 60 && batch.Length < 60000 && serialCapture.TakeDisplay(out chunk)) batch.Append(chunk);
                if (batch.Length > 0)
                {
                    serialText.AppendBatch(batch.ToString().Replace("\0", "␀"));
                }
            }
            var data = serialCapture.ViewState();
            serialObservation.Text = data.Observed + " | RX " + data.Bytes + " bytes";
            if (data.Skipped != shownSkipped)
            { shownSkipped = data.Skipped; QueueNotice(new LoaderNotice { Origin = "monitor", Code = "view_truncated", Message = "Vista atrasada: " + shownSkipped + " caracteres omitidos en pantalla. Consultá el registro de captura completo." }); }
        }
        void QueueNotice(LoaderNotice notice) { if (noticeQueue.Count < 1000) noticeQueue.Enqueue(notice); }
        void QueueFailure(string origin, Exception error) { QueueNotice(new LoaderNotice { Origin = origin, Code = "failed", Message = error.Message, Detail = error.ToString() }); }
        void DrainNotices()
        {
            LoaderNotice notice; int budget = 60;
            while (budget-- > 0 && noticeQueue.TryDequeue(out notice))
            {
                UpdateWorkflowNotice(notice);
                int row = notices.Rows.Add(notice.At, notice.Origin, notice.Code, notice.Message); notices.Rows[row].Tag = notice;
                if (notice.Code == "failed")
                {
                    notices.Rows[row].DefaultCellStyle.ForeColor = Color.Firebrick; RecordFailure(notice);
                    notices.ClearSelection(); notices.Rows[row].Selected = true;
                    serialEventsPage.Text = "Eventos y errores (!)"; mainTabs.SelectedTab = serialEventsPage;
                }
                else if (notice.Code == "verified") notices.Rows[row].DefaultCellStyle.ForeColor = Color.DarkGreen;
                if (notices.Rows.Count > 250) notices.Rows.RemoveAt(0);
                if (notice.Code == "verifying" && currentJob != null && notice.Origin == "cargador " + currentJob.Port)
                { currentJob.Stage = notice.Message; status.Text = notice.Message; }
                if ((notice.Origin.StartsWith("serial ", StringComparison.Ordinal) || notice.Origin.StartsWith("cargador ", StringComparison.Ordinal)) && notice.Code.EndsWith("_observed", StringComparison.Ordinal))
                {
                    evidencePort = notice.Origin.Substring(notice.Origin.StartsWith("serial ", StringComparison.Ordinal) ? 7 : 9);
                    bootEvidence = notice.Message + " a " + notice.At; romEvidence = "sin confirmar después de este arranque"; UpdateConnectionEvidence("");
                }
                if (notice.Code == "rom_connected") { romEvidence = notice.Message + " a " + notice.At; UpdateConnectionEvidence(""); }
                if (notice.Code == "closed" && serialCapture != null && serialCapture.State == "failed") continue;
                ShowSerialNotice(notice);
            }
        }
        static string UppercaseInitial(string value)
        {
            if (String.IsNullOrEmpty(value)) return "";
            int first = 0; while (first < value.Length && !Char.IsLetter(value[first])) first++;
            return first == value.Length ? value : value.Substring(0, first) + Char.ToUpper(value[first], System.Globalization.CultureInfo.CurrentCulture) + value.Substring(first + 1);
        }
        void ShowSerialNotice(LoaderNotice notice)
        {
            bool failed = notice.Code == "failed", warning = notice.Code == "cancelled" || notice.Code == "view_truncated", verified = notice.Code == "verified";
            string origin = notice.Origin == "monitor" ? "Monitor serie" : UppercaseInitial(notice.Origin);
            transferTitle.Text = (failed ? "Error" : warning ? "Aviso" : verified ? "Carga verificada" : "Estado") + " · " + origin;
            // Only presentation is capitalized. The original message stays in events/logs.
            transferSummary.Text = UppercaseInitial(notice.Message); transferSummary.Select(0, 0);
            transferIcon.Image = failed ? errorIcon : warning ? warningIcon : infoIcon;
            transferTitle.ForeColor = transferSummary.ForeColor = failed ? Color.Firebrick : verified ? Color.DarkGreen : Color.FromArgb(22, 79, 99);
            transferCard.BackColor = transferSummary.BackColor = failed ? Color.FromArgb(255, 239, 239) : warning ? Color.FromArgb(255, 247, 228) : verified ? Color.FromArgb(235, 247, 238) : Color.FromArgb(232, 241, 246);
        }
        void RecordFailure(LoaderNotice notice)
        {
            lastFailureText = "AFR2 Loader " + LoaderVersion.Value + "\r\n" + notice.At + " | " + notice.Origin + "\r\n" +
                FailureDetails.Explain(notice.Message, notice.Detail ?? "") + (String.IsNullOrEmpty(currentLog) ? "" : "\r\n\r\nÚltimo registro de operación: " + currentLog + ".txt\r\n") +
                (serialCapture == null ? "" : "Captura serie: " + serialCapture.Prefix + ".rx.txt / .rx.bin / .events.txt");
            lastFailurePath = null;
            try
            {
                string dir = Path.Combine(Profile.UserRoot, "logs"); Directory.CreateDirectory(dir);
                string file = Path.Combine(dir, "failure-" + DateTime.Now.ToString("yyyyMMdd-HHmmss-fff") + "-" + Guid.NewGuid().ToString("N").Substring(0, 6) + ".txt");
                File.WriteAllText(file, lastFailureText, new UTF8Encoding(false)); lastFailurePath = file;
            }
            catch (Exception error) { lastFailureText += "\r\nNo se pudo guardar el informe de error: " + error.Message; }
        }
        void ShowLastFailure()
        {
            if (String.IsNullOrEmpty(lastFailureText)) { MessageBox.Show(this, "Todavía no se registraron errores en esta sesión.", "Último error"); return; }
            using (var dialog = new Form { Text = "Último error | " + (lastFailurePath ?? "sin archivo en disco"), Size = new Size(950, 650), StartPosition = FormStartPosition.CenterParent })
            {
                var detail = new TextBox { Text = lastFailureText, Dock = DockStyle.Fill, ReadOnly = true, Multiline = true, ScrollBars = ScrollBars.Both, WordWrap = false };
                var copy = new Button { Text = "Copiar último error", Dock = DockStyle.Bottom, Height = 38 };
                copy.Click += (s, e) => CopyText(detail.Text, copy); EnableTextCopyFeedback(detail);
                dialog.Controls.Add(detail); dialog.Controls.Add(copy); FocusDialogAction(dialog, copy, detail); dialog.ShowDialog(this);
            }
        }
    }
}
