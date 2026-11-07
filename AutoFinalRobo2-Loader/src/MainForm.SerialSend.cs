using System;
using System.Drawing;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace AFR2Loader
{
    public sealed partial class MainForm
    {
        readonly TextBox serialSendInput = new TextBox { Multiline = true, ScrollBars = ScrollBars.Vertical, Dock = DockStyle.Fill, MaxLength = 262144 };
        readonly ComboBox serialSendFormat = new ComboBox { Width = 145, DropDownStyle = ComboBoxStyle.DropDownList };
        readonly CheckBox sendCr = new CheckBox { Text = "CR (0D)", AutoSize = true }, sendLf = new CheckBox { Text = "LF (0A)", AutoSize = true };
        readonly NumericUpDown sendRepeats = new NumericUpDown { Minimum = 1, Maximum = 1000, Value = 1, Width = 75 };
        readonly NumericUpDown sendInterval = new NumericUpDown { Minimum = 0, Maximum = 60000, Value = 100, Width = 85 };
        readonly Label serialSendState = new Label { AutoSize = true, Text = "Envío manual · abrir puerto para enviar", MaximumSize = new Size(880, 0) };
        Button serialSendButton, serialStopSend;
        CancellationTokenSource serialTxCancellation;
        bool serialSending;
        Control CreateSerialSendPanel()
        {
            var panel = new TableLayoutPanel { Dock = DockStyle.Fill, ColumnCount = 1, RowCount = 3, Padding = new Padding(12), BackColor = Color.White };
            panel.RowStyles.Add(new RowStyle(SizeType.Absolute, 54)); panel.RowStyles.Add(new RowStyle(SizeType.Percent, 100)); panel.RowStyles.Add(new RowStyle(SizeType.Absolute, 38));
            var options = UiRow(); serialSendFormat.Items.AddRange(new object[] { "ASCII literal", "HEX", "Números (0..255)" }); serialSendFormat.SelectedIndex = 0;
            options.Controls.Add(UiField("Enviar como", serialSendFormat)); options.Controls.Add(sendCr); options.Controls.Add(sendLf);
            // AFR2-20261106-06: repeat and pause are advanced; folding them keeps their values and the typed text.
            var repeatField = UiField("Repetir", sendRepeats); var pauseField = UiField("Pausa (ms)", sendInterval);
            repeatField.Visible = pauseField.Visible = false;
            var advanced = new ActionButton { Text = "Avanzado: repetir ▸", Width = 230, Role = ButtonRole.Secondary };
            Action refreshAdvanced = () => advanced.Text = (repeatField.Visible ? "Avanzado: repetir ▾" : "Avanzado: repetir ▸") + (!repeatField.Visible && (sendRepeats.Value != 1 || sendInterval.Value != 100) ? " (" + sendRepeats.Value + "×)" : "");
            advanced.Click += (s, e) => { repeatField.Visible = pauseField.Visible = !repeatField.Visible; refreshAdvanced(); };
            sendRepeats.ValueChanged += (s, e) => refreshAdvanced(); sendInterval.ValueChanged += (s, e) => refreshAdvanced();
            options.Controls.Add(advanced); options.Controls.Add(repeatField); options.Controls.Add(pauseField);
            panel.RowStyles[0] = new RowStyle(SizeType.AutoSize); panel.Controls.Add(options, 0, 0);
            var data = new TableLayoutPanel { Dock = DockStyle.Fill, ColumnCount = 2, RowCount = 1 };
            data.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100)); data.ColumnStyles.Add(new ColumnStyle(SizeType.AutoSize));
            serialSendInput.Font = new Font("Consolas", 10); EnableTextCopyFeedback(serialSendInput); data.Controls.Add(serialSendInput, 0, 0);
            var actions = UiRow(); actions.FlowDirection = FlowDirection.TopDown; actions.WrapContents = false; actions.Margin = new Padding(12, 0, 0, 0);
            serialSendButton = new ActionButton { Text = "Enviar", Width = 170 }; serialSendButton.Click += async (s, e) => await SendSerialData();
            serialStopSend = new ActionButton { Text = "Detener envío", Width = 170, Enabled = false }; serialStopSend.Click += (s, e) => { if (serialTxCancellation != null) serialTxCancellation.Cancel(); };
            actions.Controls.Add(serialSendButton); actions.Controls.Add(serialStopSend); data.Controls.Add(actions, 1, 0); panel.Controls.Add(data, 0, 1); panel.Controls.Add(serialSendState, 0, 2);
            serialSendInput.KeyDown += async (s, e) => { if (e.Control && e.KeyCode == Keys.Enter) { e.Handled = e.SuppressKeyPress = true; await SendSerialData(); } };
            return panel;
        }
        byte[] PrepareSerialPayload()
        {
            byte[] bytes;
            if (serialSendInput.Text.Length == 0) bytes = new byte[0];
            else if (serialSendFormat.SelectedIndex == 1) bytes = HexAsciiConverter.ParseHex(serialSendInput.Text).Bytes;
            else if (serialSendFormat.SelectedIndex == 2)
            {
                string[] tokens = Regex.Split(serialSendInput.Text.Trim(), @"[\s,;]+");
                if (tokens.Length > 65536) throw new ArgumentException("Hasta 65536 números por envío.");
                bytes = tokens.Select(value => { byte number; if (!Byte.TryParse(value, NumberStyles.None, CultureInfo.InvariantCulture, out number)) throw new ArgumentException("Números: ingresá enteros decimales de 0 a 255 separados por espacios."); return number; }).ToArray();
            }
            else bytes = serialSendInput.Text.Length == 0 ? new byte[0] : HexAsciiConverter.EncodeAscii(serialSendInput.Text).Bytes;
            var payload = bytes.Concat(sendCr.Checked ? new byte[] { 13 } : new byte[0]).Concat(sendLf.Checked ? new byte[] { 10 } : new byte[0]).ToArray();
            if (payload.Length == 0 || payload.Length > 65536) throw new ArgumentException("El envío debe tener entre 1 y 65536 bytes, incluidos CR/LF.");
            return payload;
        }
        async Task SendSerialData()
        {
            if (serialSending || busy || pendingJob != null || serialTransition) return;
            int completed = 0;
            try
            {
                if (serialCapture == null || serialCapture.State != "open") throw new InvalidOperationException("Abrí el puerto del monitor antes de enviar.");
                if (serialSendInput.Text.Length > 262144) throw new ArgumentException("Entrada demasiado larga.");
                byte[] payload = PrepareSerialPayload(); int repeat = (int)sendRepeats.Value, interval = (int)sendInterval.Value; SerialCapture capture = serialCapture;
                serialSending = true; serialTxCancellation = new CancellationTokenSource(); UpdateSerialSendControls();
                for (int index = 0; index < repeat; index++)
                {
                    serialTxCancellation.Token.ThrowIfCancellationRequested(); await capture.SendAsync(payload, serialTxCancellation.Token); completed++;
                    serialSendState.Text = "TX " + completed + "/" + repeat + " · " + payload.Length + " bytes por envío · " + capture.Port;
                    if (index + 1 < repeat) await Task.Delay(interval, serialTxCancellation.Token);
                }
            }
            catch (OperationCanceledException) { serialSendState.Text = "Envío detenido · " + completed + " repeticiones completadas. Los bytes ya transmitidos no se pueden retirar."; }
            catch (Exception error) { serialSendState.Text = "Error de envío: " + error.Message; QueueFailure("envío serie", error); }
            finally { serialSending = false; if (serialTxCancellation != null) { serialTxCancellation.Dispose(); serialTxCancellation = null; } UpdateSerialSendControls(); }
        }
        void UpdateSerialSendControls()
        {
            if (serialSendButton == null) return;
            serialSendButton.Enabled = !busy && pendingJob == null && !serialTransition && !serialSending && serialCapture != null && serialCapture.State == "open";
            serialStopSend.Enabled = serialSending; serialSendInput.ReadOnly = serialSending;
            serialSendFormat.Enabled = sendCr.Enabled = sendLf.Enabled = sendRepeats.Enabled = sendInterval.Enabled = !serialSending;
        }
    }
}
