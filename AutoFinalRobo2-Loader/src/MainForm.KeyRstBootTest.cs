// AFR2-20261106-17. Dedicated diagnostic view sharing the operation/COM owner.
using System;
using System.Drawing;
using System.Text.RegularExpressions;
using System.Windows.Forms;
namespace AFR2Loader
{
    public sealed partial class MainForm
    {
        Form keyRstTestWindow;
        void ShowKeyRstTestWindow()
        {
            if (keyRstTestWindow != null && !keyRstTestWindow.IsDisposed) { keyRstTestWindow.Activate(); return; }
            var dialog = new Form { Text = "Prueba KEY/RST y boot del ESP", Size = new Size(860, 650), MinimumSize = new Size(740, 550), Font = Font, StartPosition = FormStartPosition.CenterParent };
            keyRstTestWindow = dialog;
            var layout = new TableLayoutPanel { Dock = DockStyle.Fill, Padding = new Padding(16), ColumnCount = 1, RowCount = 5 };
            layout.RowStyles.Add(new RowStyle(SizeType.AutoSize)); layout.RowStyles.Add(new RowStyle(SizeType.AutoSize));
            layout.RowStyles.Add(new RowStyle(SizeType.AutoSize)); layout.RowStyles.Add(new RowStyle(SizeType.Percent, 100)); layout.RowStyles.Add(new RowStyle(SizeType.AutoSize));
            layout.Controls.Add(new Label { Text = "Acciona KEY/RST y lee el mensaje boot del ESP. Sin cargar firmware.\r\nRequiere flasher con captura 74880; D32=KEY violeta, D33=RST marrón.\r\nBoot mode 1 es evidencia de arranque UART, no de comunicación ROM ida/vuelta.", AutoSize = true, MaximumSize = new Size(780, 0), Margin = new Padding(0, 0, 0, 16) }, 0, 0);
            var row = UiRow();
            var selectedPort = new ComboBox { Width = 140, DropDownStyle = ComboBoxStyle.DropDownList };
            foreach (string name in System.IO.Ports.SerialPort.GetPortNames()) selectedPort.Items.Add(name);
            if (selectedPort.Items.Contains(ports.Text)) selectedPort.SelectedItem = ports.Text;
            row.Controls.Add(UiField("Puerto Mega", selectedPort));
            row.Controls.Add(new Label { Text = "Flasher USB: " + profile.Get("BridgeBaud") + " · Boot ESP: 74880", AutoSize = true, Anchor = AnchorStyles.Left, Margin = new Padding(8, 12, 0, 0) });
            layout.Controls.Add(row, 0, 1);
            var result = new Label { Text = "Sin ejecutar. Secuencia: pendiente · Boot: sin observar.", AutoSize = true, MaximumSize = new Size(780, 0), Margin = new Padding(0, 8, 0, 12) };
            layout.Controls.Add(result, 0, 2);
            var output = new TextBox { Dock = DockStyle.Fill, Multiline = true, ReadOnly = true, ScrollBars = ScrollBars.Both, WordWrap = false, Font = new Font("Consolas", 10), TabStop = false };
            EnableTextCopyFeedback(output); layout.Controls.Add(output, 0, 3);
            var buttons = UiRow();
            var start = new ActionButton { Text = "Probar KEY + RST y leer boot", Width = 320 };
            var stop = new ActionButton { Text = "Cancelar prueba", Width = 180, Enabled = false };
            var close = new ActionButton { Text = "Cerrar", Width = 130 };
            buttons.Controls.Add(start); buttons.Controls.Add(stop); buttons.Controls.Add(close); layout.Controls.Add(buttons, 0, 4);
            bool running = false; JobState job = null;
            var refresh = new Timer { Interval = 200 };
            refresh.Tick += (s, e) => {
                start.Enabled = !running && !busy && pendingJob == null && !serialTransition;
                if (job == null) return;
                string detail = job.Detail();
                if (output.Text != detail) { output.Text = detail; output.Select(output.TextLength, 0); output.ScrollToCaret(); }
                bool sequence = detail.Contains("AFR2-BOOTTEST: sequence=confirmed");
                var mode = Regex.Match(detail, @"AFR2-BOOTTEST: mode=(\d+|none)");
                result.Text = "Secuencia: " + (sequence ? "confirmada por flasher" : running ? "en curso" : "sin confirmar") + " · Boot: " +
                    (mode.Success ? mode.Groups[1].Value == "1" ? "programación UART (1) observado" : mode.Groups[1].Value == "none" ? "sin mensaje legible" : "modo " + mode.Groups[1].Value + " (no programación)" : "sin observar") +
                    (running ? "" : "\r\n" + job.Result);
            };
            start.Click += async (s, e) => {
                if (busy || pendingJob != null || running || profileDialogs > 0 || serialTransition) return;
                if (selectedPort.SelectedItem == null) { result.Text = "Elegí el puerto del Mega."; return; }
                if (!ports.Items.Contains(selectedPort.Text)) ports.Items.Add(selectedPort.Text);
                ports.SelectedItem = selectedPort.Text;
                running = true; job = null; start.Enabled = selectedPort.Enabled = close.Enabled = false; stop.Enabled = true;
                output.Clear(); result.Text = "Preparando prueba...";
                try {
                    await ExecuteCore("Prueba KEY/RST y boot ESP", async (ops, p, port, token) => { job = currentJob; await ops.TestKeyRstBoot(p, port, token); }, true);
                } catch (Exception error) { result.Text = error.Message; }
                finally { running = false; selectedPort.Enabled = close.Enabled = true; stop.Enabled = false; }
            };
            stop.Click += (s, e) => { if (running) Cancel(); };
            close.Click += (s, e) => dialog.Close();
            dialog.FormClosing += (s, e) => { if (running) { e.Cancel = true; result.Text = "Cancelá la prueba o esperá a que termine antes de cerrar."; } };
            dialog.FormClosed += (s, e) => { refresh.Stop(); refresh.Dispose(); keyRstTestWindow = null; dialog.Dispose(); };
            dialog.Controls.Add(layout); StyleControls(dialog); ApplyAppearance(dialog);
            dialog.Show(this); refresh.Start();
        }
    }
}
