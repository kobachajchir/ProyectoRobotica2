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
            converterTab = new TabPage("Conversor HEX / ASCII") { Padding = new Padding(UiInset) };
            var layout = new TableLayoutPanel { Dock = DockStyle.Fill, ColumnCount = 1, RowCount = 5 };
            foreach (int height in new[] { 48, 56 }) layout.RowStyles.Add(new RowStyle(SizeType.Absolute, height));
            layout.RowStyles.Add(new RowStyle(SizeType.Percent, 100)); layout.RowStyles.Add(new RowStyle(SizeType.Absolute, 62)); layout.RowStyles.Add(new RowStyle(SizeType.Absolute, 60));
            layout.Controls.Add(new Label { Text = "Conversor universal de bytes y texto", Font = new Font(Font.FontFamily, 16, FontStyle.Bold), Dock = DockStyle.Fill }, 0, 0);
            var options = new TableLayoutPanel { Dock = DockStyle.Fill, ColumnCount = 2 };
            options.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100)); options.ColumnStyles.Add(new ColumnStyle(SizeType.AutoSize));
            converterDirection.Width = 195; converterDirection.DropDownStyle = ComboBoxStyle.DropDownList; converterDirection.Items.AddRange(new object[] { "HEX → ASCII", "ASCII literal → HEX" }); converterDirection.SelectedIndex = 0;
            converterView.Width = 270; converterView.DropDownStyle = ComboBoxStyle.DropDownList; converterView.Items.AddRange(new object[] { "Legible · CR/LF/tab reales", "Escapada · \\r \\n \\t \\xHH" }); converterView.SelectedIndex = 0;
            options.Controls.Add(UiField("Conversión", converterDirection), 0, 0); options.Controls.Add(UiField("Vista", converterView), 1, 0); layout.Controls.Add(options, 0, 1);
            var editors = new TableLayoutPanel { Dock = DockStyle.Fill, ColumnCount = 2, RowCount = 3 };
            editors.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 50)); editors.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 50));
            editors.RowStyles.Add(new RowStyle(SizeType.Absolute, 32)); editors.RowStyles.Add(new RowStyle(SizeType.Percent, 100)); editors.RowStyles.Add(new RowStyle(SizeType.Absolute, 104));
            converterInputLabel.Dock = converterOutputLabel.Dock = DockStyle.Fill; editors.Controls.Add(converterInputLabel, 0, 0); editors.Controls.Add(converterOutputLabel, 1, 0);
            foreach (TextBox box in new[] { converterInput, converterOutput }) { box.Dock = DockStyle.Fill; box.Multiline = true; box.AcceptsReturn = box.AcceptsTab = true; box.ScrollBars = ScrollBars.Both; box.WordWrap = false; box.MaxLength = Int32.MaxValue; box.Font = new Font("Consolas", 10); EnableTextCopyFeedback(box); }
            converterOutput.ReadOnly = true; converterOutput.BackColor = Color.FromArgb(19, 29, 39); converterOutput.ForeColor = Color.FromArgb(222, 232, 241);
            editors.Controls.Add(converterInput, 0, 1); editors.Controls.Add(converterOutput, 1, 1);
            var inputActions = UiRow(); inputActions.Margin = new Padding(0, 8, 8, 0);
            ConverterButton(inputActions, "Pegar", () => { try { if (Clipboard.ContainsText()) { converterInput.Text = Clipboard.GetText(); converterInput.Focus(); } } catch (System.Runtime.InteropServices.ExternalException) { converterState.Text = "No se pudo leer el portapapeles. Usá Ctrl+V."; } }, 115);
            ConverterButton(inputActions, "Limpiar", () => { converterInput.Clear(); ClearConverterResult(); converterInput.Focus(); }, 115);
            converterCopyHex = ConverterButton(inputActions, "Copiar HEX normalizado", () => { if (converterData != null) CopyText(HexAsciiConverter.FormatHex(converterData.Bytes), converterCopyHex); }, 240);
            inputActions.Controls.SetChildIndex(converterCopyHex, 1);
            var outputActions = UiRow(); outputActions.Margin = new Padding(8, 8, 0, 0); converterCopy = ConverterButton(outputActions, "Copiar resultado", () => CopyText(converterOutput.Text, converterCopy), 200);
            editors.Controls.Add(inputActions, 0, 2); editors.Controls.Add(outputActions, 1, 2); layout.Controls.Add(editors, 0, 2);
            converterHint.Dock = DockStyle.Fill; converterHint.Padding = new Padding(12); converterHint.BackColor = Color.FromArgb(232, 241, 246); layout.Controls.Add(converterHint, 0, 3);
            var footer = new TableLayoutPanel { Dock = DockStyle.Fill, ColumnCount = 2 }; footer.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100)); footer.ColumnStyles.Add(new ColumnStyle(SizeType.AutoSize));
            converterState.Dock = DockStyle.Fill; converterState.TextAlign = ContentAlignment.MiddleLeft; converterState.AccessibleName = "Estado de conversión"; footer.Controls.Add(converterState, 0, 0);
            var actions = UiRow(); actions.FlowDirection = FlowDirection.RightToLeft; actions.Dock = DockStyle.None; actions.Anchor = AnchorStyles.Right; actions.Margin = new Padding(0); ConverterButton(actions, "Convertir", ConvertUniversalBytes, 200); footer.Controls.Add(actions, 1, 0); layout.Controls.Add(footer, 0, 4);
            converterDirection.SelectedIndexChanged += (s, e) => { ClearConverterResult(); UpdateConverterLabels(); }; converterView.SelectedIndexChanged += (s, e) => RenderConverterResult();
            converterInput.TextChanged += (s, e) => ClearConverterResult(); converterInput.KeyDown += (s, e) => { if (e.Control && e.KeyCode == Keys.Enter) { e.Handled = e.SuppressKeyPress = true; ConvertUniversalBytes(); } };
            converterTab.Controls.Add(layout); tabs.TabPages.Add(converterTab); UpdateConverterLabels(); ClearConverterResult();
        }
        Button ConverterButton(Control parent, string text, Action action, int width)
        {
            var button = new ActionButton { Text = text, Width = width, Height = 33, Margin = new Padding(0, 4, 8, 4) };
            button.Click += (s, e) => action(); parent.Controls.Add(button); ButtonStyle(button); return button;
        }
        void UpdateConverterLabels()
        {
            bool hex = converterDirection.SelectedIndex == 0;
            converterInputLabel.Text = hex ? "Entrada HEX · HH / 0xHH / pares continuos" : "Entrada ASCII literal · caracteres 0..127";
            converterOutputLabel.Text = hex ? "Resultado ASCII" : "Resultado HEX · 16 bytes por fila";
            converterInput.AccessibleName = converterInputLabel.Text; converterOutput.AccessibleName = converterOutputLabel.Text;
            converterView.Enabled = hex;
            converterHint.Text = hex ? "Nota: los controles distintos de CR/LF/tab y los bytes 80..FF se muestran como \\xHH. Cabeceras --- ... --- del monitor se omiten." : "Nota: ASCII literal: \\r, \\n y \\xHH escritos son caracteres literales, no escapes a interpretar. No se reemplazan caracteres fuera de ASCII.";
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
