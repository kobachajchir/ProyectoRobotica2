// AFR2-20261028-12. Offline HEX/ASCII workspace. It does not create jobs,
// consult a command catalogue, open COM, transmit or persist pasted content.
using System;
using System.Drawing;
using System.Windows.Forms;

namespace AFR2Loader
{
    public sealed partial class MainForm
    {
        TabPage converterTab;
        readonly TextBox converterInput = new TextBox(), converterOutput = new TextBox();
        readonly ComboBox converterDirection = new ComboBox(), converterView = new ComboBox();
        readonly Label converterInputLabel = new Label(), converterOutputLabel = new Label(), converterState = new Label(), converterHint = new Label();
        Button converterCopy, converterCopyHex;
        HexAsciiData converterData;

        void InitializeConverter(TabControl tabs)
        {
            converterTab = new TabPage("Conversor HEX / ASCII") { Padding = new Padding(16) };
            var layout = new TableLayoutPanel { Dock = DockStyle.Fill, ColumnCount = 1, RowCount = 6 };
            layout.RowStyles.Add(new RowStyle(SizeType.Absolute, 45));
            layout.RowStyles.Add(new RowStyle(SizeType.Absolute, 56));
            layout.RowStyles.Add(new RowStyle(SizeType.Absolute, 50));
            layout.RowStyles.Add(new RowStyle(SizeType.Percent, 100));
            layout.RowStyles.Add(new RowStyle(SizeType.Absolute, 80));
            layout.RowStyles.Add(new RowStyle(SizeType.Absolute, 92));
            layout.Controls.Add(new Label { Text = "Conversor universal de bytes y texto", Font = new Font(Font.FontFamily, 16, FontStyle.Bold), Dock = DockStyle.Fill }, 0, 0);
            layout.Controls.Add(new Label { Text = "Pegá HEX o texto ASCII y convertí sin conectarte a un dispositivo. No identifica comandos ni interpreta protocolos.\r\nFunciona independientemente del monitor, del perfil y de las operaciones de carga.", Dock = DockStyle.Fill }, 0, 1);
            var options = new FlowLayoutPanel { Dock = DockStyle.Fill, WrapContents = true };
            options.Controls.Add(new Label { Text = "Conversión", AutoSize = true, Margin = new Padding(0, 8, 8, 0) });
            converterDirection.Width = 195; converterDirection.DropDownStyle = ComboBoxStyle.DropDownList;
            converterDirection.Items.AddRange(new object[] { "HEX → ASCII", "ASCII literal → HEX" }); converterDirection.SelectedIndex = 0; options.Controls.Add(converterDirection);
            options.Controls.Add(new Label { Text = "Vista ASCII", AutoSize = true, Margin = new Padding(12, 8, 8, 0) });
            converterView.Width = 270; converterView.DropDownStyle = ComboBoxStyle.DropDownList;
            converterView.Items.AddRange(new object[] { "Legible · CR/LF/tab reales", "Escapada · \\r \\n \\t \\xHH" }); converterView.SelectedIndex = 0; options.Controls.Add(converterView);
            layout.Controls.Add(options, 0, 2);
            var editors = new TableLayoutPanel { Dock = DockStyle.Fill, ColumnCount = 2, RowCount = 2 };
            editors.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 50)); editors.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 50));
            editors.RowStyles.Add(new RowStyle(SizeType.Absolute, 28)); editors.RowStyles.Add(new RowStyle(SizeType.Percent, 100));
            converterInputLabel.Dock = converterOutputLabel.Dock = DockStyle.Fill;
            editors.Controls.Add(converterInputLabel, 0, 0); editors.Controls.Add(converterOutputLabel, 1, 0);
            foreach (TextBox box in new[] { converterInput, converterOutput })
            {
                box.Dock = DockStyle.Fill; box.Multiline = true; box.AcceptsReturn = true; box.AcceptsTab = true;
                box.ScrollBars = ScrollBars.Both; box.WordWrap = false; box.MaxLength = Int32.MaxValue;
                box.Font = new Font("Consolas", 10); EnableTextCopyFeedback(box);
            }
            converterOutput.ReadOnly = true; converterOutput.BackColor = Color.FromArgb(19, 29, 39); converterOutput.ForeColor = Color.FromArgb(222, 232, 241);
            editors.Controls.Add(converterInput, 0, 1); editors.Controls.Add(converterOutput, 1, 1); layout.Controls.Add(editors, 0, 3);
            var actions = new FlowLayoutPanel { Dock = DockStyle.Fill, WrapContents = true };
            // Kept out of idleControls: pure conversion remains available during a job.
            ConverterButton(actions, "Convertir", ConvertUniversalBytes, 125);
            converterCopy = ConverterButton(actions, "Copiar resultado", () => CopyText(converterOutput.Text, converterCopy), 150);
            converterCopyHex = ConverterButton(actions, "Copiar HEX normalizado", () => { if (converterData != null) CopyText(HexAsciiConverter.FormatHex(converterData.Bytes), converterCopyHex); }, 195);
            ConverterButton(actions, "Limpiar", () => { converterInput.Clear(); ClearConverterResult(); converterInput.Focus(); }, 100);
            ConverterButton(actions, "Pegar del portapapeles", () => { try { if (Clipboard.ContainsText()) { converterInput.Text = Clipboard.GetText(); converterInput.Focus(); } } catch (System.Runtime.InteropServices.ExternalException) { converterState.Text = "No se pudo leer el portapapeles. Usá Ctrl+V en Entrada."; converterState.ForeColor = Color.Firebrick; } }, 195);
            layout.Controls.Add(actions, 0, 4);
            var footer = new TableLayoutPanel { Dock = DockStyle.Fill, RowCount = 2, ColumnCount = 1 };
            footer.RowStyles.Add(new RowStyle(SizeType.Percent, 50)); footer.RowStyles.Add(new RowStyle(SizeType.Percent, 50));
            converterState.Dock = converterHint.Dock = DockStyle.Fill;
            converterState.AccessibleName = "Estado de conversión";
            footer.Controls.Add(converterState, 0, 0); footer.Controls.Add(converterHint, 0, 1); layout.Controls.Add(footer, 0, 5);
            converterDirection.SelectedIndexChanged += (s, e) => { ClearConverterResult(); UpdateConverterLabels(); };
            converterView.SelectedIndexChanged += (s, e) => RenderConverterResult();
            converterInput.TextChanged += (s, e) => ClearConverterResult();
            converterInput.KeyDown += (s, e) => { if (e.Control && e.KeyCode == Keys.Enter) { e.Handled = e.SuppressKeyPress = true; ConvertUniversalBytes(); } };
            converterTab.Controls.Add(layout); tabs.TabPages.Add(converterTab); UpdateConverterLabels(); ClearConverterResult();
        }
        Button ConverterButton(Control parent, string text, Action action, int width)
        {
            var button = new Button { Text = text, Width = width, Height = 33, Margin = new Padding(0, 4, 8, 4) };
            button.Click += (s, e) => action(); parent.Controls.Add(button); return button;
        }
        void UpdateConverterLabels()
        {
            bool hex = converterDirection.SelectedIndex == 0;
            converterInputLabel.Text = hex ? "Entrada HEX · HH / 0xHH / pares continuos" : "Entrada ASCII literal · caracteres 0..127";
            converterOutputLabel.Text = hex ? "Resultado ASCII" : "Resultado HEX · 16 bytes por fila";
            converterInput.AccessibleName = converterInputLabel.Text; converterOutput.AccessibleName = converterOutputLabel.Text;
            converterView.Enabled = hex;
            converterHint.Text = hex ? "Los controles distintos de CR/LF/tab y los bytes 80..FF se muestran como \\xHH. Cabeceras --- ... --- del monitor se omiten." : "ASCII literal: \\r, \\n y \\xHH escritos son caracteres literales, no escapes a interpretar. No se reemplazan caracteres fuera de ASCII.";
        }
        void ClearConverterResult()
        {
            converterData = null; converterOutput.Clear();
            if (converterCopy != null) converterCopy.Enabled = converterCopyHex.Enabled = false;
            converterState.Text = "Sin conversión. Hasta 64 KiB de bytes / 262144 caracteres. Ctrl+Enter convierte.";
            converterState.ForeColor = Color.FromArgb(71, 85, 105);
        }
        void ConvertUniversalBytes()
        {
            try
            {
                converterData = converterDirection.SelectedIndex == 0 ? HexAsciiConverter.ParseHex(converterInput.Text) : HexAsciiConverter.EncodeAscii(converterInput.Text);
                RenderConverterResult(); int extended = 0;
                foreach (byte value in converterData.Bytes) if (value > 127) extended++;
                converterState.Text = "Convertidos " + converterData.Bytes.Length + " bytes." + (converterData.HeaderLines > 0 ? " Cabeceras de monitor omitidas: " + converterData.HeaderLines + "." : "") + (extended > 0 ? " Fuera de ASCII: " + extended + "; conservados como \\xHH." : "") + " Sin análisis de comandos.";
                converterState.ForeColor = Color.FromArgb(20, 110, 65);
            }
            catch (ArgumentException error)
            {
                ClearConverterResult(); converterState.Text = error.Message; converterState.ForeColor = Color.Firebrick;
            }
        }
        void RenderConverterResult()
        {
            if (converterData == null) return;
            converterOutput.Text = converterDirection.SelectedIndex == 0 ? HexAsciiConverter.FormatAscii(converterData.Bytes, converterView.SelectedIndex == 1) : HexAsciiConverter.FormatHex(converterData.Bytes);
            converterOutput.Select(0, 0); converterCopy.Enabled = converterCopyHex.Enabled = true;
        }
    }
}
