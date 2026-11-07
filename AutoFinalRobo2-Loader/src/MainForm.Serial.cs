using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
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
        readonly ComboBox serialFormat = new ComboBox();
        readonly Queue<SerialDisplayChunk> serialView = new Queue<SerialDisplayChunk>();
        int serialViewWeight, serialHexColumn;
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

        Button serialConvert;
        string copiedSerialText;
        int copiedSerialFormat;
        void RememberCopiedSerial(string text) { copiedSerialText = text; copiedSerialFormat = serialFormat.SelectedIndex; if (serialConvert != null) serialConvert.Visible = !String.IsNullOrEmpty(text); }
        void ShowSerialInConverter(TabControl tabs)
        {
            string selected = serialText.SelectionLength > 0 ? serialText.SelectedText : copiedSerialText;
            if (String.IsNullOrEmpty(selected) || converterTab == null) return;
            int format = serialText.SelectionLength > 0 ? serialFormat.SelectedIndex : copiedSerialFormat;
            converterDirection.SelectedIndex = format == 1 ? 0 : 1;
            converterInput.Text = selected; tabs.SelectedTab = converterTab; converterInput.Focus();
        }
        LoaderNotice detailedNotice;
        void InitializeSerial(TabControl tabs)
        {
            var page = new TabPage("Monitor serie") { Padding = new Padding(UiInset), AutoScroll = true };
            var layout = new TableLayoutPanel { Dock = DockStyle.Fill, ColumnCount = 1, RowCount = 8, MinimumSize = new Size(0, 720) };
            foreach (int height in new[] { 48, 48, 42, 90, 44 }) layout.RowStyles.Add(new RowStyle(SizeType.Absolute, height));
            layout.RowStyles.Add(new RowStyle(SizeType.Percent, 100)); layout.RowStyles.Add(new RowStyle(SizeType.Absolute, 88)); layout.RowStyles.Add(new RowStyle(SizeType.Absolute, 210));
            var controls = UiRow();
            serialPorts.Width = 115; serialPorts.DropDownStyle = ComboBoxStyle.DropDownList; controls.Controls.Add(UiField("COM", serialPorts));
            var refresh = new ActionButton { Text = "Actualizar", Width = 140 }; refresh.Click += (s, e) => { try { RefreshSerialPorts(); } catch (Exception error) { QueueFailure("monitor", error); } }; controls.Controls.Add(refresh);
            serialOpen = new ActionButton { Text = "Abrir puerto", Width = 155 }; serialOpen.Click += async (s, e) => await OpenSerialMonitor(); controls.Controls.Add(serialOpen);
            serialClose = new ActionButton { Text = "Cerrar puerto", Width = 155 }; serialClose.Click += async (s, e) => { try { await StopSerialMonitor("Cierre manual"); } catch (Exception error) { QueueFailure("monitor", error); } }; controls.Controls.Add(serialClose);
            var readMega = new ActionButton { Text = "Leer ATmega", Width = 175 }; readMega.Click += (s, e) => OpenManualSection("leer-atmega"); controls.Controls.Add(readMega);
            var readEsp = new ActionButton { Text = "Leer ESP", Width = 175 }; readEsp.Click += (s, e) => OpenManualSection("leer-esp"); controls.Controls.Add(readEsp);
            layout.Controls.Add(controls, 0, 0);
            var codecs = UiRow(); serialBaud.Width = 110; serialBaud.Items.AddRange(new object[] { "9600", "19200", "38400", "57600", "74880", "115200", "230400", "460800", "921600" }); serialBaud.Text = "74880";
            codecs.Controls.Add(UiField("Baudios", serialBaud)); serialApply = new ActionButton { Text = "Aplicar baudios", Width = 175 }; serialApply.Click += async (s, e) => await ApplySerialBaud(); codecs.Controls.Add(serialApply);
            serialCodec.Width = 105; serialCodec.DropDownStyle = ComboBoxStyle.DropDownList; serialCodec.Items.AddRange(new object[] { "UTF-8", "ASCII", "Latin-1" }); serialCodec.SelectedIndex = 0; codecs.Controls.Add(UiField("Codificación", serialCodec)); layout.Controls.Add(codecs, 0, 1);
            var statePanel = new TableLayoutPanel { Dock = DockStyle.Fill, ColumnCount = 1, RowCount = 2 };
            statePanel.RowStyles.Add(new RowStyle(SizeType.Percent, 50)); statePanel.RowStyles.Add(new RowStyle(SizeType.Percent, 50));
            serialState.Font = new Font(Font, FontStyle.Bold); statePanel.Controls.Add(serialState, 0, 0); statePanel.Controls.Add(serialObservation, 0, 1); layout.Controls.Add(statePanel, 0, 2);
            transferCard.ColumnStyles.Add(new ColumnStyle(SizeType.Absolute, 48)); transferCard.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100));
            transferCard.RowStyles.Add(new RowStyle(SizeType.Absolute, 34)); transferCard.RowStyles.Add(new RowStyle(SizeType.Percent, 100));
            transferIcon.Image = infoIcon; transferIcon.Anchor = AnchorStyles.None; transferIcon.Size = new Size(30, 30);
            transferTitle.Text = "Estado de la sesión"; transferTitle.Font = new Font(Font.FontFamily, 13, FontStyle.Bold); transferSummary.Text = "Sin carga en curso.";
            transferCard.BackColor = transferSummary.BackColor = Color.FromArgb(232, 241, 246); transferTitle.ForeColor = transferSummary.ForeColor = Color.FromArgb(22, 79, 99);
            transferCard.Controls.Add(transferIcon, 0, 0); transferCard.SetRowSpan(transferIcon, 2); transferCard.Controls.Add(transferTitle, 1, 0); transferCard.Controls.Add(transferSummary, 1, 1); EnableTextCopyFeedback(transferSummary); layout.Controls.Add(transferCard, 0, 3);
            // AFR2-20261106-06: reception first. With no transfer the card shrinks to its title line.
            Action fitTransfer = () => {
                bool idle = transferSummary.Text == "Sin carga en curso." || transferSummary.Text.Length == 0;
                transferSummary.Visible = !idle; layout.RowStyles[3].Height = (int)((idle ? 56 : 90) * DisplayScale());
            };
            transferSummary.TextChanged += (s, e) => fitTransfer(); fitTransfer();
            var viewBar = UiRow(); viewBar.FlowDirection = FlowDirection.RightToLeft;
            serialFormat.Width = 195; serialFormat.DropDownStyle = ComboBoxStyle.DropDownList; serialFormat.Items.AddRange(new object[] { "ASCII", "HEX con espacios", "Texto decodificado" }); serialFormat.SelectedIndex = 0;
            viewBar.Controls.Add(UiField("Vista", serialFormat)); layout.Controls.Add(viewBar, 0, 4);
            serialText.Dock = DockStyle.Fill; serialText.ReadOnly = true; serialText.WordWrap = false; serialText.DetectUrls = false;
            serialText.BackColor = Color.FromArgb(19, 29, 39); serialText.ForeColor = Color.FromArgb(222, 232, 241); serialText.Font = new Font("Consolas", 10);
            serialText.BindFollow(serialFollow); EnableTextCopyFeedback(serialText); layout.Controls.Add(serialText, 0, 5);
            serialText.TextCopied += RememberCopiedSerial;
            var footer = new TableLayoutPanel { Dock = DockStyle.Fill, ColumnCount = 2, Padding = new Padding(0, 8, 0, 0) }; footer.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100)); footer.ColumnStyles.Add(new ColumnStyle(SizeType.AutoSize));
            var buttons = UiRow(); buttons.Controls.Add(serialFollow); buttons.Controls.Add(serialPause);
            var clear = new ActionButton { Text = "Limpiar vista", Width = 135 }; clear.Click += (s, e) => ClearSerialView(); buttons.Controls.Add(clear);
            var copy = new ActionButton { Text = "Copiar texto", Width = 140 }; copy.Click += (s, e) => { string text = serialText.SelectionLength > 0 ? serialText.SelectedText : serialText.Text; CopyText(text, serialText); }; buttons.Controls.Add(copy);
            var files = new ActionButton { Text = "Abrir registros", Width = 155 }; files.Click += (s, e) => OpenLogs(); buttons.Controls.Add(files);
            var failure = new ActionButton { Text = "Ver último error", Width = 175 }; failure.Click += (s, e) => ShowLastFailure(); buttons.Controls.Add(failure);
            serialConvert = new ActionButton { Text = "Mostrar en conversor HEX", Width = 260, Visible = false, Anchor = AnchorStyles.Right };
            serialConvert.Click += (s, e) => ShowSerialInConverter(tabs);
            footer.Controls.Add(buttons, 0, 0); footer.Controls.Add(serialConvert, 1, 0); layout.Controls.Add(footer, 0, 6); layout.Controls.Add(CreateSerialSendPanel(), 0, 7);
            Action resizeTools = () => {
                int width = Math.Max(1, buttons.ClientSize.Width), used = 0, lines = 1;
                foreach (Control tool in buttons.Controls) { int item = tool.Width + tool.Margin.Horizontal; if (used > 0 && used + item > width) { lines++; used = 0; } used += item; }
                layout.RowStyles[6].Height = lines * 44 + 20;
            };
            buttons.SizeChanged += (s, e) => resizeTools(); serialConvert.VisibleChanged += (s, e) => resizeTools();
            serialText.SelectionChanged += (s, e) => { if (serialConvert != null) serialConvert.Visible = serialText.SelectionLength > 0 || !String.IsNullOrEmpty(copiedSerialText); };
            page.Controls.Add(layout); tabs.TabPages.Add(page);
            InitializeEventsPage(tabs);
            timer.Tick += (s, e) => { DrainNotices(); DrainSerial(); UpdateSerialControls(); UpdateSerialSendControls(); };
            ports.SelectedIndexChanged += (s, e) => { if (serialCapture == null || !serialCapture.Active) { if (serialPorts.Items.Contains(ports.Text)) serialPorts.SelectedItem = ports.Text; } };
            RefreshSerialPorts(); UpdateSerialControls();
            serialFormat.SelectedIndexChanged += (s, e) => { RenderSerialView(); serialConvert.Visible = !String.IsNullOrEmpty(copiedSerialText); UpdateSerialControls(); };
            FormClosed += (s, e) => { infoIcon.Dispose(); warningIcon.Dispose(); errorIcon.Dispose(); };
        }
        void InitializeEventsPage(TabControl tabs)
        {
            serialEventsPage = new TabPage("Eventos y errores") { Padding = new Padding(UiInset) };
            var layout = new TableLayoutPanel { Dock = DockStyle.Fill, ColumnCount = 1, RowCount = 3 };
            layout.RowStyles.Add(new RowStyle(SizeType.Percent, 55)); layout.RowStyles.Add(new RowStyle(SizeType.Percent, 45)); layout.RowStyles.Add(new RowStyle(SizeType.Absolute, 52));
            notices.Dock = DockStyle.Fill; notices.ReadOnly = true; notices.AllowUserToAddRows = notices.AllowUserToDeleteRows = false; notices.RowHeadersVisible = false;
            notices.SelectionMode = DataGridViewSelectionMode.FullRowSelect; notices.MultiSelect = false; notices.AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.Fill;
            foreach (string name in new[] { "Hora", "Origen", "Estado", "Mensaje" }) notices.Columns.Add(name, name);
            notices.Columns[3].FillWeight = 300;
            notices.Columns.Add(new DataGridViewButtonColumn { Name = "Actions", HeaderText = "Acciones", Text = "Ver detalles", UseColumnTextForButtonValue = true, FillWeight = 90 });
            notices.RowTemplate.Height = 38;
            StyleActionColumn(notices, 4, index => "Ver detalles", true);
            foreach (DataGridViewColumn column in notices.Columns) column.SortMode = DataGridViewColumnSortMode.NotSortable;
            notices.CellContentClick += (s, e) => { if (e.RowIndex >= 0 && e.ColumnIndex == 4) ShowNoticeDetails(notices.Rows[e.RowIndex]); };
            layout.Controls.Add(notices, 0, 0); layout.Controls.Add(noticeDetail, 0, 1); EnableTextCopyFeedback(noticeDetail);
            // AFR2-20261106-06: explain the empty state; copying needs a selected detail.
            noticeDetail.Text = "Todavía no hay detalle seleccionado. Los eventos aparecen arriba al compilar, cargar o recibir datos del puerto. Elegí una fila y pulsá «Ver detalles» para leerlo completo aquí.";
            var actions = UiRow(); var copy = new ActionButton { Text = "Copiar detalle", Width = 170, Role = ButtonRole.Secondary, Enabled = false }; noticeCopy = copy; copy.Click += (s, e) => CopyText(noticeDetail.Text, copy); actions.Controls.Add(copy);
            layout.Controls.Add(actions, 0, 2); serialEventsPage.Controls.Add(layout); tabs.TabPages.Add(serialEventsPage);
        }
        ActionButton noticeCopy;
        void ShowNoticeDetails(DataGridViewRow selected)
        {
            var notice = selected.Tag as LoaderNotice; if (notice == null) return; detailedNotice = notice;
            if (noticeCopy != null) noticeCopy.Enabled = true;
            // Theme-aware row colors (a fixed white broke dark mode; high contrast keeps system colors).
            bool contrast = SystemInformation.HighContrast;
            foreach (DataGridViewRow row in notices.Rows) {
                row.DefaultCellStyle.BackColor = contrast ? SystemColors.Window : LoaderAppearance.Surface;
                row.DefaultCellStyle.SelectionBackColor = LoaderAppearance.Selection;
                row.DefaultCellStyle.SelectionForeColor = LoaderAppearance.SelectionText;
            }
            noticeDetail.Text = notice.At + " | " + notice.Origin + " | " + notice.Code + "\r\n" + notice.Message + "\r\n\r\n" + notice.Detail;
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
                serialPause.Checked = false; DrainSerial(); shownSkipped = 0;
                serialCapture = new SerialCapture(port, baud, codec, QueueNotice);
                var header = new SerialDisplayChunk { Header = true, Text = "\r\n--- " + DateTime.Now.ToString("HH:mm:ss") + " | " + port + " | " + baud + " | " + codec + " ---\r\n" };
                RememberSerialChunk(header); serialText.AppendBatch(FormatSerialChunk(header));
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
            if (serialTxCancellation != null) serialTxCancellation.Cancel();
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
            serialFormat.Enabled = !serialTransition;
            string stateName = serialCapture == null ? "cerrado" : serialCapture.State == "open" ? "ABIERTO" : serialCapture.State == "opening" ? "ABRIENDO" : serialCapture.State == "closing" ? "CERRANDO" : serialCapture.State == "failed" ? "FALLÓ" : "CERRADO";
            serialState.Text = serialCapture == null ? "Puerto cerrado. Sin captura." : serialCapture.Port + " | " + serialCapture.Baud + " baudios | " + serialCapture.Codec + " | vista " + serialFormat.Text + " | " + stateName + (serialPause.Checked ? " | VISTA EN PAUSA; archivo sigue grabando mientras esté abierto" : "");
            if (serialCapture == null) serialObservation.Text = "Sin observaciones de arranque. No se abre el puerto al iniciar.";
            serialState.ForeColor = serialCapture != null && serialCapture.State == "failed" ? Color.Firebrick : Color.FromArgb(22, 79, 99);
        }
        void DrainSerial()
        {
            if (serialCapture == null) return;
            if (!serialPause.Checked)
            {
                var batch = new StringBuilder(); SerialDisplayChunk chunk; int count = 0;
                while (count++ < 60 && batch.Length < 60000 && serialCapture.TakeDisplay(out chunk))
                { RememberSerialChunk(chunk); batch.Append(FormatSerialChunk(chunk)); }
                if (batch.Length > 0)
                {
                    serialText.AppendBatch(batch.ToString().Replace("\0", "␀"));
                }
            }
            var data = serialCapture.ViewState();
            serialObservation.Text = data.Observed + " | RX " + data.Bytes + " bytes";
            if (data.SkippedBytes != shownSkipped)
            { shownSkipped = data.SkippedBytes; QueueNotice(new LoaderNotice { Origin = "monitor", Code = "view_truncated", Message = "Vista atrasada: " + shownSkipped + " bytes omitidos en pantalla. Consultá el registro de captura completo." }); }
        }
        void RememberSerialChunk(SerialDisplayChunk chunk)
        {
            serialView.Enqueue(chunk); serialViewWeight += chunk.Weight;
            while (serialViewWeight > 65536 && serialView.Count > 0) serialViewWeight -= serialView.Dequeue().Weight;
        }
        string FormatSerialChunk(SerialDisplayChunk chunk)
        {
            if (chunk.Header) { serialHexColumn = 0; return chunk.Text; }
            if (serialFormat.SelectedIndex == 0) return Encoding.ASCII.GetString(chunk.Bytes).Replace("\0", "␀");
            if (serialFormat.SelectedIndex != 1) return chunk.Text.Replace("\0", "␀");
            var text = new StringBuilder(chunk.Bytes.Length * 3);
            foreach (byte value in chunk.Bytes)
            {
                if (serialHexColumn == 16) { text.Append("\r\n"); serialHexColumn = 0; }
                else if (serialHexColumn > 0) text.Append(' ');
                text.Append(value.ToString("X2")); serialHexColumn++;
            }
            return text.ToString();
        }
        void RenderSerialView()
        {
            serialHexColumn = 0;
            var text = new StringBuilder();
            foreach (SerialDisplayChunk chunk in serialView) text.Append(FormatSerialChunk(chunk));
            serialText.Clear(); serialText.AppendBatch(text.ToString());
        }
        void ClearSerialView()
        {
            if (serialCapture != null) serialCapture.ClearDisplay();
            serialView.Clear(); serialViewWeight = serialHexColumn = 0; serialText.Clear();
        }
        void QueueNotice(LoaderNotice notice) { if (noticeQueue.Count < 1000) noticeQueue.Enqueue(notice); }
        void QueueFailure(string origin, Exception error) { QueueNotice(new LoaderNotice { Origin = origin, Code = "failed", Message = error.Message, Detail = error.ToString() }); }
        void DrainNotices()
        {
            LoaderNotice notice; int budget = 60;
            while (budget-- > 0 && noticeQueue.TryDequeue(out notice))
            {
                UpdateWorkflowNotice(notice);
                int row = notices.Rows.Add(ReadableTime(notice.At), notice.Origin, notice.Code, notice.Message); notices.Rows[row].Tag = notice;
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
                var copy = new ActionButton { Text = "Copiar último error", Dock = DockStyle.Bottom, Height = 38 };
                copy.Click += (s, e) => CopyText(detail.Text, copy); EnableTextCopyFeedback(detail);
                dialog.Controls.Add(detail); dialog.Controls.Add(copy); FocusDialogAction(dialog, copy, detail); ShowStyledDialog(dialog);
            }
        }
    }
}

