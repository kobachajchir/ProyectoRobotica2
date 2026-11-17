using System;
using System.Drawing;
using System.Linq;
using System.Windows.Forms;

namespace AFR2Loader
{
    public sealed partial class MainForm
    {
        string configurationSection = "";
        TextBox configurationSearch;
        TabControl configurationTabs;
        TabPage configurationTab;
        TreeView configurationNavigation;
        Panel generalConfigOptionsPanel;
        CheckBox autoDetectComCheck;
        bool syncingAutoDetectCheck;
        string FieldSection(string key)
        {
            if (new[] { "FsScript", "FsSourceDir", "FsScriptHost", "FsScriptArguments" }.Contains(key)) return "ESP/Preparación FileSystem";
            if (new[] { "AtmegaDir", "Hex", "MegaBaud", "Mcu", "Programmer", "Signature" }.Contains(key)) return "ATmega/Firmware y conexión";
            if (new[] { "Make", "Gcc", "Ar", "Objcopy", "Avrdude", "AvrdudeConfig" }.Contains(key)) return "ATmega/Herramientas";
            if (new[] { "BridgeBaud", "BridgeStartupSeconds", "EspKeyRst" }.Contains(key)) return "ATmega/Flasher ESP";
            if (new[] { "EspTransport", "EspBaud", "BootWaitSeconds" }.Contains(key)) return "ESP/Conexión";
            if (new[] { "Python", "Esptool", "EsptoolMajor", "Pio" }.Contains(key)) return "ESP/Herramientas";
            if (new[] { "EspFirmwareMode", "EspDir", "EspBin", "PioEnv", "FirmwareOffset", "FirmwareMax" }.Contains(key)) return "ESP/Firmware propio";
            if (new[] { "AtPackage", "AtLayoutId", "AtUseFilesystem" }.Contains(key)) return "ESP/Paquete AT";
            if (new[] { "WebDir", "Node", "Npm", "WebUseFilesystem", "WebLayoutPreset", "FsBin", "FsOffset", "FsBytes", "Linker" }.Contains(key)) return "ESP/FileSystem";
            if (new[] { "FlashMB", "FlashMode", "FlashFreq" }.Contains(key)) return "ESP/Flash y memoria";
            return "General";
        }
        void InitializeConfigurationSections(TableLayoutPanel layout, TextBox search)
        {
            configurationSearch = search;
            layout.Controls.Remove(settings);
            var body = new TableLayoutPanel { Dock = DockStyle.Fill, ColumnCount = 2, RowCount = 1 };
            body.ColumnStyles.Add(new ColumnStyle(SizeType.Absolute, 230)); body.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100));
            var navigation = new TreeView { Dock = DockStyle.Fill, HideSelection = false, FullRowSelect = true, ItemHeight = 30, Margin = new Padding(0, 0, 16, 0) };
            configurationNavigation = navigation;
            var all = navigation.Nodes.Add("Todas las secciones"); all.Tag = "";
            foreach (string group in new[] { "General", "ATmega", "ESP" })
            {
                var parent = navigation.Nodes.Add(group); parent.Tag = group;
                foreach (string path in Profile.Fields.Select(f => FieldSection(f.Key)).Distinct().Where(p => p.StartsWith(group + "/")))
                { var child = parent.Nodes.Add(path.Substring(group.Length + 1)); child.Tag = path; }
            }
            navigation.ExpandAll(); navigation.AfterSelect += (s, e) => { configurationSection = Convert.ToString(e.Node.Tag); ApplyConfigurationFilter(); };
            navigation.SelectedNode = navigation.Nodes[1];
            var help = new DataGridViewLinkColumn { Name = "Help", HeaderText = "Ayuda", Text = "Ver explicación", UseColumnTextForLinkValue = true, Width = 125 };
            settings.Columns.Add(help); settings.Columns[2].HeaderText = "Acción"; settings.Columns[2].Width = 125;
            settings.CellContentClick += (s, e) => { if (e.RowIndex >= 0 && e.ColumnIndex == 3) ShowFieldHelp((Field)settings.Rows[e.RowIndex].Tag); };
            settings.CellFormatting += (s, e) => { if (e.RowIndex < 0 || e.ColumnIndex != 2) return; var field = settings.Rows[e.RowIndex].Tag as Field; if (field == null) return; e.Value = field.Kind == "text" ? "" : "Examinar…"; e.FormattingApplied = true; };
            // AFR2-20261106-06: one help entry per field ("Ver explicación"); "Examinar…" only for paths.
            StyleActionColumn(settings, 2, index => { var field = settings.Rows[index].Tag as Field; return field == null || field.Kind == "text" ? null : "Examinar…"; }, false);
            generalConfigOptionsPanel = new Panel { Dock = DockStyle.Top, AutoSize = true, AutoSizeMode = AutoSizeMode.GrowAndShrink, Margin = new Padding(0, 0, 0, 10) };
            var generalOptionsLayout = new TableLayoutPanel { Dock = DockStyle.Top, AutoSize = true, ColumnCount = 1, RowCount = 2, Margin = new Padding(0) };
            generalOptionsLayout.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100));
            autoDetectComCheck = new CheckBox
            {
                AutoSize = true,
                Text = "Autodetectar puerto COM al iniciar",
                Checked = preferences != null && preferences.AutoDetectComOnStartup,
                Margin = new Padding(0, 2, 0, 2)
            };
            var autoDetectComNote = new Label
            {
                AutoSize = true,
                Dock = DockStyle.Top,
                Text = "Envía un paquete de consulta no destructivo a cada puerto COM disponible al abrir el programa; rige desde el próximo inicio.",
                Margin = new Padding(22, 0, 0, 4),
                ForeColor = LoaderAppearance.Muted
            };
            autoDetectComCheck.CheckedChanged += (s, e) =>
            {
                if (syncingAutoDetectCheck || preferences == null) return;
                bool desired = autoDetectComCheck.Checked;
                bool previous = preferences.AutoDetectComOnStartup;
                preferences.AutoDetectComOnStartup = desired;
                try { preferences.Save(); }
                catch (Exception error)
                {
                    preferences.AutoDetectComOnStartup = previous;
                    SyncAutoDetectComSetting();
                    Log("No pude guardar la preferencia de autodetección COM: " + error.Message);
                    MessageBox.Show(this, "No pude guardar la configuración de autodetección COM:\n" + error.Message, "Configuración", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    return;
                }
                UpdateComAutoDetectHint();
                Log("Autodetectar puerto COM al iniciar: " + (desired ? "activado (aplica en el próximo inicio)." : "desactivado."));
            };
            generalOptionsLayout.Controls.Add(autoDetectComCheck, 0, 0);
            generalOptionsLayout.Controls.Add(autoDetectComNote, 0, 1);
            generalConfigOptionsPanel.Controls.Add(generalOptionsLayout);
            var contentPanel = new TableLayoutPanel { Dock = DockStyle.Fill, ColumnCount = 1, RowCount = 2, Margin = new Padding(0) };
            contentPanel.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100));
            contentPanel.RowStyles.Add(new RowStyle(SizeType.AutoSize));
            contentPanel.RowStyles.Add(new RowStyle(SizeType.Percent, 100));
            contentPanel.Controls.Add(generalConfigOptionsPanel, 0, 0);
            contentPanel.Controls.Add(settings, 0, 1);
            body.Controls.Add(navigation, 0, 0); body.Controls.Add(contentPanel, 1, 0); layout.Controls.Add(body, 0, 1);
            SyncAutoDetectComSetting();
            ApplyConfigurationFilter();
        }
        void ApplyConfigurationFilter()
        {
            if (configurationSearch == null) return;
            settings.EndEdit(); settings.CurrentCell = null;
            string query = configurationSearch.Text;
            foreach (DataGridViewRow row in settings.Rows)
            {
                var field = (Field)row.Tag; string section = FieldSection(field.Key);
                row.Visible = (configurationSection.Length == 0 || section == configurationSection || section.StartsWith(configurationSection + "/")) &&
                    (field.Key + " " + field.Label + " " + field.Hint).IndexOf(query, StringComparison.CurrentCultureIgnoreCase) >= 0;
            }
            if (generalConfigOptionsPanel != null)
            {
                bool inGeneral = configurationSection.Length == 0 || configurationSection == "General";
                bool matchesQuery = String.IsNullOrWhiteSpace(query) ||
                    "Autodetectar puerto COM al iniciar consulta paquete arranque general".IndexOf(query, StringComparison.CurrentCultureIgnoreCase) >= 0;
                generalConfigOptionsPanel.Visible = inGeneral && matchesQuery;
            }
        }
        void SyncAutoDetectComSetting()
        {
            if (autoDetectComCheck != null && preferences != null)
            {
                syncingAutoDetectCheck = true;
                try { autoDetectComCheck.Checked = preferences.AutoDetectComOnStartup; }
                finally { syncingAutoDetectCheck = false; }
            }
            UpdateComAutoDetectHint();
        }
        void OpenGeneralConfigurationTab()
        {
            if (configurationTabs != null && configurationTab != null) configurationTabs.SelectedTab = configurationTab;
            if (configurationNavigation != null && configurationNavigation.Nodes.Count > 1)
                configurationNavigation.SelectedNode = configurationNavigation.Nodes[1];
            configurationSection = "General";
            ApplyConfigurationFilter();
        }
        void ShowFieldHelp(Field field)
        {
            using (var dialog = new Form { Text = field.Label, Size = new Size(700, 350), StartPosition = FormStartPosition.CenterParent, Font = Font })
            {
                var layout = new TableLayoutPanel { Dock = DockStyle.Fill, Padding = new Padding(UiInset), RowCount = 3, ColumnCount = 1 };
                layout.RowStyles.Add(new RowStyle(SizeType.Absolute, 42)); layout.RowStyles.Add(new RowStyle(SizeType.Percent, 100)); layout.RowStyles.Add(new RowStyle(SizeType.Absolute, 42));
                layout.Controls.Add(new Label { Text = FieldSection(field.Key) + " · " + field.Label, Dock = DockStyle.Fill, Font = new Font(Font, FontStyle.Bold) }, 0, 0);
                layout.Controls.Add(new TextBox { Text = field.Hint + "\r\n\r\nValor actual: " + profile.Get(field.Key), ReadOnly = true, Multiline = true, Dock = DockStyle.Fill, BorderStyle = BorderStyle.None, BackColor = dialog.BackColor }, 0, 1);
                var link = new LinkLabel { Text = "Abrir esta sección del manual", AutoSize = true }; link.LinkClicked += (s, e) => OpenManualSection(ManualAnchor(field)); layout.Controls.Add(link, 0, 2); dialog.Controls.Add(layout); ShowStyledDialog(dialog);
            }
        }
    }
}
