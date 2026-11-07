// AFR2-20261028-16. Shared WinForms spacing, action colors and navigation.
using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Windows.Forms;

namespace AFR2Loader
{
    public sealed partial class MainForm
    {
        const int UiGap = 8, UiInset = 16, UiButtonHeight = 38, UiSplitWidth = 36;
        readonly ComboBox deviceTarget = new ComboBox { Width = 130, DropDownStyle = ComboBoxStyle.DropDownList };
        readonly ComboBox espSection = new ComboBox { Width = 230, DropDownStyle = ComboBoxStyle.DropDownList };
        readonly ComboBox megaVariant = new ComboBox { Width = 270, DropDownStyle = ComboBoxStyle.DropDownList };
        Control espOptionsRow, espMethodRow, megaOptionsRow, espMemoryRow;
        bool syncingDestination;
        TabPage operationTab;
        readonly ToolStripMenuItem optionsMenu = new ToolStripMenuItem("Opciones");
        readonly Dictionary<string, Image> actionImages = new Dictionary<string, Image>();

        FlowLayoutPanel UiRow()
        { return new FlowLayoutPanel { Dock = DockStyle.Top, AutoSize = true, AutoSizeMode = AutoSizeMode.GrowAndShrink, WrapContents = true, Padding = new Padding(0), Margin = new Padding(0, 0, 0, UiGap) }; }
        Control UiField(string caption, Control control)
        {
            var row = new TableLayoutPanel { AutoSize = true, AutoSizeMode = AutoSizeMode.GrowAndShrink, ColumnCount = 2, RowCount = 1, Margin = new Padding(0, 0, 20, 0), Padding = new Padding(0) };
            row.ColumnStyles.Add(new ColumnStyle(SizeType.AutoSize)); row.ColumnStyles.Add(new ColumnStyle(SizeType.AutoSize));
            row.RowStyles.Add(new RowStyle(SizeType.Absolute, 44));
            row.Controls.Add(new Label { Text = caption, AutoSize = true, Anchor = AnchorStyles.Left, Margin = new Padding(0, 0, 10, 0) }, 0, 0);
            control.Anchor = AnchorStyles.Left; control.Margin = new Padding(0); row.Controls.Add(control, 1, 0); return row;
        }
        void SelectOperationTarget(int selected)
        {
            if (syncingDestination) return;
            if (selected != 0 && EspFlashRules.IsAt(profile)) selected = 1;
            syncingDestination = true;
            try { target.SelectedIndex = selected; deviceTarget.SelectedIndex = selected == 0 ? 0 : 1; espSection.SelectedIndex = Math.Max(0, selected - 1); }
            finally { syncingDestination = false; }
            UpdateDestinationVisibility();
        }
        void UpdateDestinationVisibility()
        {
            if (espOptionsRow == null) return;
            bool esp = target.SelectedIndex != 0;
            espOptionsRow.Visible = espMethodRow.Visible = esp;
            bool at = EspFlashRules.IsAt(profile);
            if (espSectionField != null) espSectionField.Visible = esp && !at;
            bool fsSection = esp && !at && target.SelectedIndex >= 2;
            if (capacityRow != null) { capacityRow.Visible = esp; PlaceCapacityRow(fsSection); }
            if (prepCard != null) prepCard.Visible = fsSection;
            espMemoryRow.Visible = fsSection && CapacityConfirmed;
            megaOptionsRow.Visible = !esp; if (preparationHeading != null) preparationHeading.Visible = esp && !fsSection; if (resultHeading != null) resultHeading.Text = esp ? "3 · Ejecución y resultado" : "2 · Ejecución y resultado";
            if (filesystemLayout != null) filesystemLayout.Visible = esp && !at && target.SelectedIndex >= 2 && CapacityConfirmed;
            if (compileTarget != null) UpdateTargetActions();
        }
        // La consulta de capacidad vive en la tarjeta lateral cuando hay FileSystem y en la columna izquierda en los demás casos.
        void PlaceCapacityRow(bool inCard)
        {
            var home = espPanel as TableLayoutPanel; if (capacityRow == null || prepCardContent == null || home == null) return;
            Control want = inCard ? (Control)prepCardContent : home; if (capacityRow.Parent == want) return;
            if (inCard) { capacityState.Margin = new Padding(0, 0, 0, 4); prepCardContent.Controls.Add(capacityRow, 0, 0); }
            else { capacityState.Margin = new Padding(12, 12, 0, 0); capacityState.MaximumSize = Size.Empty; home.Controls.Add(capacityRow, 0, 6); }
        }
        void ShowOperation() { if (mainTabs != null && operationTab != null) mainTabs.SelectedTab = operationTab; }

        // AFR2-20261106-19. Setup/log left, result/stages right.
        void ArrangeOperationWorkspace(TableLayoutPanel layout) { ArrangeOperationColumns(layout); }
        Label resultHeading;
        float DisplayScale() { using (var g = Graphics.FromHwnd(IntPtr.Zero)) return g.DpiY / 96F; }
        int MeasureOperationActions(int width, float scale)
        {
            var box = operationActions;
            if (box == null) return (int)(100 * scale);
            var buttons = box.GetControlFromPosition(0, 0) as FlowLayoutPanel; var flash = box.GetControlFromPosition(1, 0);
            int available = Math.Max(200, width - (flash == null ? 0 : flash.Width + flash.Margin.Horizontal));
            int used = 0, lines = 1;
            if (buttons != null) foreach (Control c in buttons.Controls)
            {
                if (!c.Visible) continue;
                int item = c.Width + c.Margin.Horizontal;
                if (used > 0 && used + item > available) { lines++; used = 0; }
                used += item;
            }
            int row = Math.Max(lines * (int)((UiButtonHeight + 6) * scale) + (buttons == null ? 0 : buttons.Margin.Vertical), flash == null ? 0 : flash.Height + flash.Margin.Vertical + (int)(8 * scale)) + (int)(4 * scale);
            int note = nextStep.GetPreferredSize(new Size(width, 0)).Height + (int)(4 * scale);
            return row + note + box.Padding.Vertical;
        }
        void FitOperationRows() { FitOperationColumns(); }

        Image ActionImage(string kind, Color color)
        {
            string key = kind + color.ToArgb(); Image existing;
            if (actionImages.TryGetValue(key, out existing)) return existing;
            var bitmap = new Bitmap(18, 18);
            using (var g = Graphics.FromImage(bitmap)) using (var pen = new Pen(color, 1.6F)) using (var brush = new SolidBrush(color))
            {
                g.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.AntiAlias;
                if (kind == "stop") g.FillRectangle(brush, 4, 4, 10, 10);
                else if (kind == "run") g.FillPolygon(brush, new[] { new Point(5, 3), new Point(15, 9), new Point(5, 15) });
                else if (kind == "copy") { g.DrawRectangle(pen, 3, 3, 9, 10); g.DrawRectangle(pen, 6, 6, 9, 10); }
                else if (kind == "save") { g.DrawRectangle(pen, 3, 3, 12, 12); g.DrawRectangle(pen, 6, 3, 6, 4); g.DrawRectangle(pen, 6, 10, 6, 5); }
                else if (kind == "refresh") { g.DrawArc(pen, 3, 3, 12, 12, 40, 280); g.FillPolygon(brush, new[] { new Point(13, 2), new Point(16, 7), new Point(10, 6) }); }
                else if (kind == "info") { g.DrawEllipse(pen, 2, 2, 14, 14); g.DrawLine(pen, 9, 8, 9, 13); g.FillEllipse(brush, 8, 4, 2, 2); }
                else { g.DrawRectangle(pen, 2, 4, 14, 11); g.DrawLine(pen, 2, 7, 16, 7); }
            }
            actionImages[key] = bitmap; return bitmap;
        }
        void ButtonStyle(ButtonBase button)
        {
            if (button is CheckBox || button is RadioButton)
            {
                if (button.Parent is FlowLayoutPanel) { int width = button.GetPreferredSize(Size.Empty).Width; button.AutoSize = false; button.Size = new Size(width, UiButtonHeight); }
                button.Margin = new Padding(0, 3, UiGap, 3); return;
            }
            string text = button.Text.ToLowerInvariant();
            var actionButton = button as ActionButton;
            ButtonRole role = actionButton == null ? ButtonRole.Auto : actionButton.Role;
            bool stop = role == ButtonRole.Danger, run = role == ButtonRole.Primary, info = role == ButtonRole.Info;
            if (role == ButtonRole.Auto)
            {
                stop = new[] { "cancel", "cerrar", "rechazar", "detener", "borrar registros", "limpiar carpeta" }.Any(text.Contains);
                run = new[] { "flashear", "cargar", "abrir puerto", "convertir", "continuar", "aceptar", "compilar", "enviar", "ejecutar" }.Any(text.Contains);
                info = new[] { "actualizar", "detectar", "inform", "ayuda", "prueba", "comprobar", "ver detalles" }.Any(text.Contains);
            }
            Color background = stop ? LoaderAppearance.DangerFill : run ? LoaderAppearance.ActionFill : info ? LoaderAppearance.InfoFill : Color.White;
            button.BackColor = background; button.ForeColor = background == Color.White ? Color.FromArgb(35, 51, 65) : Color.White;
            if (SystemInformation.HighContrast)
            {
                // Windows high contrast: keep the role through the border, use system colors for fill and text.
                button.BackColor = SystemColors.ButtonFace; button.ForeColor = SystemColors.ControlText;
                button.UseVisualStyleBackColor = false; button.FlatStyle = FlatStyle.Flat; button.FlatAppearance.BorderColor = SystemColors.WindowText;
                button.Height = UiButtonHeight; button.MinimumSize = new Size(0, UiButtonHeight); button.Margin = new Padding(0, 3, UiGap, 3);
                button.Image = null; button.Padding = new Padding(12, 0, 12, 0);
                button.Width = Math.Max(button.Width, TextRenderer.MeasureText(button.Text, button.Font).Width + 40);
                return;
            }
            button.UseVisualStyleBackColor = false; button.FlatStyle = FlatStyle.Flat; button.FlatAppearance.BorderColor = background == Color.White ? Color.FromArgb(177, 190, 201) : background;
            button.Height = UiButtonHeight; button.MinimumSize = new Size(0, UiButtonHeight); button.Margin = new Padding(0, 3, UiGap, 3);
            string kind = stop ? "stop" : text.Contains("copiar") ? "copy" : text.Contains("guardar") ? "save" : text.Contains("actualizar") ? "refresh" : info ? "info" : run ? "run" : "section";
            button.Image = ActionImage(kind, button.ForeColor); button.ImageAlign = ContentAlignment.MiddleLeft; button.TextImageRelation = TextImageRelation.ImageBeforeText;
            button.Padding = new Padding(12, 0, 12, 0);
            button.Width = Math.Max(button.Width, TextRenderer.MeasureText(button.Text, button.Font).Width + 60);
        }
        void StyleControls(Control root)
        {
            foreach (Control control in root.Controls)
            {
                var button = control as ButtonBase; if (button != null) ButtonStyle(button);
                var strip = control as ToolStrip;
                if (control is TabPage) control.BackColor = Color.FromArgb(245, 247, 250);
                var grid = control as DataGridView;
                if (grid != null && grid != notices) {
                    grid.BackgroundColor = Color.White; grid.BorderStyle = BorderStyle.FixedSingle;
                    grid.EnableHeadersVisualStyles = false;
                    grid.ColumnHeadersDefaultCellStyle.BackColor = Color.FromArgb(232, 239, 245);
                    grid.ColumnHeadersDefaultCellStyle.ForeColor = Color.FromArgb(35, 51, 65);
                    grid.ColumnHeadersHeight = 34; grid.RowTemplate.Height = 34;
                    grid.GridColor = Color.FromArgb(215, 224, 232);
                    grid.AlternatingRowsDefaultCellStyle.BackColor = Color.FromArgb(248, 250, 252);
                }
                if (strip != null) {
                    if (!(strip is MenuStrip) && !(strip is StatusStrip)) { strip.Padding = new Padding(0); strip.Margin = new Padding(0, 3, 8, 3); }
                    strip.Renderer = new ActionStripRenderer();
                    foreach (ToolStripItem item in strip.Items) StyleMenuItem(item);
                }
                StyleControls(control);
            }
        }
        void StyleMenuItem(ToolStripItem item)
        {
            if (item is ToolStripControlHost) return;
            bool stop = item.Text.IndexOf("rechazar", StringComparison.OrdinalIgnoreCase) >= 0;
            item.Image = ActionImage(stop ? "stop" : "section", Color.FromArgb(35, 51, 65));
            var split = item as ToolStripSplitButton;
            if (split != null) { split.DropDownButtonWidth = UiSplitWidth; split.Margin = new Padding(0); split.Size = new Size(285, UiButtonHeight); split.Padding = new Padding(16, 0, 12, 0); split.ImageAlign = ContentAlignment.MiddleLeft; split.TextAlign = ContentAlignment.MiddleLeft; split.ImageScaling = ToolStripItemImageScaling.None; split.BackColor = Color.FromArgb(24, 112, 69); split.ForeColor = Color.White; var spaced = new Bitmap(28, 18);
                using (var g = Graphics.FromImage(spaced)) g.DrawImage(ActionImage(item.Text.StartsWith("Guardar") ? "save" : "run", Color.White), 0, 0);
                split.Image = spaced; }
            var dropdown = item as ToolStripDropDownItem;
            if (dropdown != null) foreach (ToolStripItem child in dropdown.DropDownItems) StyleMenuItem(child);
        }
        DialogResult ShowStyledDialog(Form dialog) { StyleControls(dialog); ApplyAppearance(dialog); return dialog.ShowDialog(this); }
        DialogResult ShowStyledDialog(CommonDialog dialog) { return dialog.ShowDialog(this); }
        void StyleActionColumn(DataGridView grid, int column, Func<int, string> caption, bool information)
        {
            grid.CellPainting += (s, e) => {
                if (e.RowIndex < 0 || e.ColumnIndex != column) return;
                if (caption(e.RowIndex) == null) { e.PaintBackground(e.ClipBounds, true); e.Handled = true; return; }
                e.PaintBackground(e.ClipBounds, true);
                Rectangle bounds = e.CellBounds; bounds.Inflate(information ? -1 : -4, information ? -1 : -4);
                Color background = information ? Color.FromArgb(30, 89, 153) : LoaderAppearance.Surface;
                Color foreground = information ? Color.White : LoaderAppearance.Text;
                if (SystemInformation.HighContrast) { background = SystemColors.ButtonFace; foreground = SystemColors.ControlText; }
                using (var brush = new SolidBrush(background)) e.Graphics.FillRectangle(brush, bounds);
                using (var pen = new Pen(Color.FromArgb(177, 190, 201))) e.Graphics.DrawRectangle(pen, bounds);
                e.Graphics.DrawImage(ActionImage(information ? "info" : "section", foreground), bounds.Left + 6, bounds.Top + Math.Max(0, (bounds.Height - 18) / 2));
                var textBounds = new Rectangle(bounds.Left + 28, bounds.Top, Math.Max(0, bounds.Width - 30), bounds.Height);
                TextRenderer.DrawText(e.Graphics, caption(e.RowIndex), grid.Font, textBounds, foreground, TextFormatFlags.Left | TextFormatFlags.VerticalCenter | TextFormatFlags.EndEllipsis);
                e.Handled = true;
            };
        }
        // Technical ISO stamps stay in the detail/log; lists show a readable local time.
        static string ReadableTime(string iso)
        { DateTimeOffset parsed; return DateTimeOffset.TryParse(iso, out parsed) ? parsed.ToLocalTime().ToString("dd/MM HH:mm:ss") : iso; }
        void OpenManualSection(string anchor)
        {
            string file = Path.Combine(appDir, "MANUAL.html");
            if (!File.Exists(file)) { MessageBox.Show(this, "No se encontró MANUAL.html."); return; }
            var dialog = new Form { Text = "Ayuda del Loader", Size = new Size(940, 700), StartPosition = FormStartPosition.CenterParent, Font = Font };
            var browser = new WebBrowser { Dock = DockStyle.Fill, ScriptErrorsSuppressed = true, Url = new Uri(new Uri(file).AbsoluteUri + "#" + anchor) };
            browser.DocumentCompleted += (s, e) => { if (browser.Document != null) { var section = browser.Document.GetElementById(anchor); if (section != null) section.ScrollIntoView(true); } };
            dialog.Controls.Add(browser); dialog.FormClosed += (s, e) => dialog.Dispose(); dialog.Show(this);
        }
        string ManualAnchor(Field field) { return "campo-" + field.Key; }
        string ManualForAction(string title)
        {
            if (title.IndexOf("flasher", StringComparison.OrdinalIgnoreCase) >= 0) return "mega-flasher";
            if (title.IndexOf("método", StringComparison.OrdinalIgnoreCase) >= 0) return "metodo";
            if (title.IndexOf("backup", StringComparison.OrdinalIgnoreCase) >= 0) return "preferencias";
            if (title.IndexOf("ATmega", StringComparison.OrdinalIgnoreCase) >= 0) return "mega";
            if (title.IndexOf("FileSystem", StringComparison.OrdinalIgnoreCase) >= 0) return "fs";
            return "esp";
        }

    }
}
