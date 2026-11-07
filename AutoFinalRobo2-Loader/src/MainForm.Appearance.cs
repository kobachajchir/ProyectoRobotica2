// AFR2-20261106-03. Koba Chajchir / AFR2; adaptación mediante Codex.
// Palette follows Contracts/docs/design-system and Web/src/styles/tokens.css.
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Windows.Forms;
using Microsoft.Win32;

namespace AFR2Loader
{
    internal static class LoaderAppearance
    {
        internal static bool Dark;
        internal static Color Canvas { get { return ColorTranslator.FromHtml(Dark ? "#0b1220" : "#f3f6fb"); } }
        internal static Color Surface { get { return ColorTranslator.FromHtml(Dark ? "#14233b" : "#ffffff"); } }
        internal static Color Subtle { get { return ColorTranslator.FromHtml(Dark ? "#182946" : "#eaf0f8"); } }
        internal static Color Text { get { return ColorTranslator.FromHtml(Dark ? "#e2e8f0" : "#1e293b"); } }
        internal static Color Muted { get { return ColorTranslator.FromHtml(Dark ? "#94a3b8" : "#475569"); } }
        internal static Color Accent { get { return ColorTranslator.FromHtml(Dark ? "#ff6b7d" : "#b81d33"); } }
        internal static Color Border { get { return ColorTranslator.FromHtml(Dark ? "#405572" : "#b7c4d5"); } }
        // AFR2-20261106-06. Colors by function: action (compile/flash), information, danger, warning, success.
        // High contrast uses system colors instead (see MainForm.ButtonStyle).
        internal static readonly Color ActionFill = Color.FromArgb(24, 112, 69), InfoFill = Color.FromArgb(30, 89, 153), DangerFill = Color.FromArgb(166, 40, 48);
        internal static readonly Color WarningInk = Color.FromArgb(145, 80, 0), SuccessInk = Color.FromArgb(20, 110, 65);
        internal static Color Selection { get { return Dark ? Border : SystemColors.Highlight; } }
        internal static Color SelectionText { get { return Dark ? Text : SystemColors.HighlightText; } }
    }

    public sealed partial class MainForm
    {
        readonly Dictionary<Control, Color> appearanceInk = new Dictionary<Control, Color>();
        readonly Dictionary<Control, Color> appearancePaper = new Dictionary<Control, Color>();
        readonly HashSet<TabControl> appearanceTabs = new HashSet<TabControl>();
        readonly List<ToolStripMenuItem> appearanceChoices = new List<ToolStripMenuItem>();
        bool applyingAppearance;

        void InitializeAppearance()
        {
            var menu = new ToolStripMenuItem("Apariencia");
            string[] keys = { "system", "light", "dark" }, labels = { "Seguir al sistema", "Claro", "Oscuro" };
            for (int i = 0; i < keys.Length; i++) {
                string key = keys[i];
                var item = new ToolStripMenuItem(labels[i]) { Tag = key };
                item.Click += (s, e) => {
                    string previous = preferences.Theme; preferences.Theme = key;
                    try { preferences.Save(); SetAppearance(); }
                    catch (Exception error) { preferences.Theme = previous; MessageBox.Show(this, error.Message, "Apariencia"); }
                };
                appearanceChoices.Add(item); menu.DropDownItems.Add(item);
            }
            optionsMenu.DropDownItems.Add(menu);
            UserPreferenceChangedEventHandler systemChanged = (s, e) => {
                if (preferences.Theme == "system" && IsHandleCreated && !IsDisposed)
                    BeginInvoke(new Action(SetAppearance));
            };
            SystemEvents.UserPreferenceChanged += systemChanged;
            Disposed += (s, e) => SystemEvents.UserPreferenceChanged -= systemChanged;
            SetAppearance();
        }

        void SetAppearance()
        {
            bool systemDark = false;
            try { using (var key = Registry.CurrentUser.OpenSubKey(@"Software\Microsoft\Windows\CurrentVersion\Themes\Personalize"))
                systemDark = key != null && Convert.ToInt32(key.GetValue("AppsUseLightTheme", 1)) == 0; } catch { }
            LoaderAppearance.Dark = !SystemInformation.HighContrast && (preferences.Theme == "dark" || preferences.Theme == "system" && systemDark);
            foreach (var item in appearanceChoices) item.Checked = (string)item.Tag == preferences.Theme;
            ApplyAppearance(this);
        }

        Color AppearanceForeground(Color original)
        {
            if (!LoaderAppearance.Dark) return original.GetBrightness() > .72F ? LoaderAppearance.Text : original;
            if (original.GetBrightness() > .72F) return original;
            if (Math.Abs(original.R - original.G) < 24 && Math.Abs(original.G - original.B) < 24) return LoaderAppearance.Text;
            if (original.R > original.G * 1.25) return ColorTranslator.FromHtml("#ff8a65");
            if (original.G > original.R * 1.25 && original.G > original.B) return ColorTranslator.FromHtml("#4ade80");
            if (original.B > original.R * 1.2) return ColorTranslator.FromHtml("#38bdf8");
            return LoaderAppearance.Text;
        }

        void ApplyAppearance(Control root)
        {
            if (applyingAppearance || root.IsDisposed) return;
            applyingAppearance = true;
            try { RegisterAppearanceTree(root); ApplyAppearanceCore(root); } finally { applyingAppearance = false; }
            root.Invalidate(true);
        }

        void RegisterAppearanceTree(Control control)
        {
            if (!appearanceInk.ContainsKey(control)) {
                appearanceInk[control] = control.ForeColor;
                appearancePaper[control] = control.BackColor;
                control.Disposed += (s, e) => { appearanceInk.Remove(control); appearancePaper.Remove(control); var tab = control as TabControl; if (tab != null) appearanceTabs.Remove(tab); };
                control.ForeColorChanged += (s, e) => {
                    if (applyingAppearance || control.IsDisposed) return;
                    appearanceInk[control] = control.ForeColor;
                    ApplyAppearance(control);
                };
                control.ControlAdded += (s, e) => { if (!applyingAppearance) ApplyAppearance(e.Control); };
            }
            foreach (Control child in control.Controls) RegisterAppearanceTree(child);
        }

        void ApplyAppearanceCore(Control control)
        {
            if (SystemInformation.HighContrast) {
                control.BackColor = SystemColors.Window; control.ForeColor = SystemColors.WindowText;
            } else {
                control.BackColor = LoaderAppearance.Canvas;
                control.ForeColor = AppearanceForeground(appearanceInk[control]);
                Color paper = appearancePaper[control];
                if (paper.A == 255 && paper.R > paper.G * 1.3 && paper.R > paper.B * 1.2 && paper.GetBrightness() < .65F) {
                    control.BackColor = paper; control.ForeColor = Color.White;
                }
                var button = control as Button;
                if (button != null) {
                    ButtonStyle(button);
                    if (button.BackColor == Color.White) {
                        button.BackColor = LoaderAppearance.Surface; button.ForeColor = LoaderAppearance.Text;
                        button.FlatAppearance.BorderColor = LoaderAppearance.Border;
                        string text = button.Text.ToLowerInvariant();
                        button.Image = ActionImage(text.Contains("copiar") ? "copy" : text.Contains("guardar") ? "save" : "section", button.ForeColor);
                    }
                }
                var input = control as TextBoxBase;
                if (input != null) {
                    input.BackColor = LoaderAppearance.Surface; input.ForeColor = LoaderAppearance.Text;
                }
                if (control is ComboBox || control is ListBox || control is TreeView || control is ListView || control is NumericUpDown) {
                    control.BackColor = LoaderAppearance.Surface; control.ForeColor = LoaderAppearance.Text;
                }
                var link = control as LinkLabel;
                if (link != null) { link.LinkColor = LoaderAppearance.Accent; link.ActiveLinkColor = LoaderAppearance.Text; link.VisitedLinkColor = LoaderAppearance.Accent; }
                var grid = control as DataGridView;
                if (grid != null) {
                    grid.EnableHeadersVisualStyles = false; grid.BackgroundColor = LoaderAppearance.Surface; grid.GridColor = LoaderAppearance.Border;
                    grid.DefaultCellStyle.BackColor = LoaderAppearance.Surface; grid.DefaultCellStyle.ForeColor = LoaderAppearance.Text;
                    grid.DefaultCellStyle.SelectionBackColor = LoaderAppearance.Subtle; grid.DefaultCellStyle.SelectionForeColor = LoaderAppearance.Text;
                    grid.AlternatingRowsDefaultCellStyle.BackColor = LoaderAppearance.Canvas;
                    grid.ColumnHeadersDefaultCellStyle.BackColor = LoaderAppearance.Subtle; grid.ColumnHeadersDefaultCellStyle.ForeColor = LoaderAppearance.Text;
                    grid.ColumnHeadersDefaultCellStyle.SelectionBackColor = LoaderAppearance.Subtle; grid.ColumnHeadersDefaultCellStyle.SelectionForeColor = LoaderAppearance.Text;
                    grid.RowHeadersDefaultCellStyle = grid.ColumnHeadersDefaultCellStyle;
                    foreach (DataGridViewColumn column in grid.Columns) {
                        var links = column as DataGridViewLinkColumn;
                        if (links != null) { links.LinkColor = LoaderAppearance.Accent; links.ActiveLinkColor = LoaderAppearance.Text; links.VisitedLinkColor = LoaderAppearance.Accent; }
                    }
                }
                var strip = control as ToolStrip;
                if (strip != null) { strip.Renderer = new ActionStripRenderer(); foreach (ToolStripItem item in strip.Items) ApplyAppearanceMenu(item); }
                var tabs = control as TabControl;
                if (tabs != null && appearanceTabs.Add(tabs)) {
                    tabs.DrawMode = TabDrawMode.OwnerDrawFixed;
                    tabs.DrawItem += (s, e) => {
                        bool selected = e.Index == tabs.SelectedIndex;
                        using (var brush = new SolidBrush(selected ? LoaderAppearance.Surface : LoaderAppearance.Canvas)) e.Graphics.FillRectangle(brush, e.Bounds);
                        if (selected) using (var pen = new Pen(LoaderAppearance.Accent, 3)) e.Graphics.DrawLine(pen, e.Bounds.Left + 5, e.Bounds.Bottom - 2, e.Bounds.Right - 5, e.Bounds.Bottom - 2);
                        TextRenderer.DrawText(e.Graphics, tabs.TabPages[e.Index].Text, tabs.Font, e.Bounds, LoaderAppearance.Text, TextFormatFlags.HorizontalCenter | TextFormatFlags.VerticalCenter);
                    };
                }
            }
            foreach (Control child in control.Controls) ApplyAppearanceCore(child);
        }

        void ApplyAppearanceMenu(ToolStripItem item)
        {
            item.BackColor = LoaderAppearance.Surface; item.ForeColor = LoaderAppearance.Text;
            if (!(item is ToolStripSplitButton) && !(item is ToolStripControlHost)) item.Image = ActionImage("section", LoaderAppearance.Text);
            var dropdown = item as ToolStripDropDownItem;
            if (dropdown != null) {
                dropdown.DropDown.BackColor = LoaderAppearance.Surface;
                dropdown.DropDown.Renderer = new ActionStripRenderer();
                foreach (ToolStripItem child in dropdown.DropDownItems) ApplyAppearanceMenu(child);
            }
        }
    }
}
