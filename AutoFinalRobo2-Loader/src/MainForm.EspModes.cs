// AFR2-20261027-22. Firmware family, filesystem choice and exact memory preview.
using System;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Windows.Forms;

namespace AFR2Loader
{
    public sealed partial class MainForm
    {
        readonly ComboBox espMode = new ComboBox { Width = 225, DropDownStyle = ComboBoxStyle.DropDownList };
        readonly ComboBox filesystemLayout = new ComboBox { Width = 285, DropDownStyle = ComboBoxStyle.DropDownList };
        readonly CheckBox includeFilesystem = new CheckBox { Text = "Incluir filesystem", AutoSize = true };
        readonly Label memorySummary = new Label { Dock = DockStyle.Fill, TextAlign = ContentAlignment.MiddleLeft };
        Button atPackageButton;
        bool syncingEspOptions;

        Control CreateEspOptions(Control selectors)
        {
            var panel = new TableLayoutPanel { Dock = DockStyle.Fill, ColumnCount = 1, RowCount = 3, Margin = new Padding(0) };
            panel.RowStyles.Add(new RowStyle(SizeType.Absolute, 44)); panel.RowStyles.Add(new RowStyle(SizeType.Absolute, 42)); panel.RowStyles.Add(new RowStyle(SizeType.Percent, 100));
            panel.Controls.Add(selectors, 0, 0);
            var options = new FlowLayoutPanel { Dock = DockStyle.Fill, WrapContents = false };
            options.Controls.Add(new Label { Text = "ESP", AutoSize = true, Margin = new Padding(0, 8, 6, 0) });
            espMode.Items.AddRange(new object[] { "Web propio / Arduino", "Firmware AT / paquete" }); options.Controls.Add(espMode); idleControls.Add(espMode);
            includeFilesystem.Margin = new Padding(10, 6, 10, 0); options.Controls.Add(includeFilesystem); idleControls.Add(includeFilesystem);
            options.Controls.Add(filesystemLayout); idleControls.Add(filesystemLayout);
            atPackageButton = AddButton(options, "Paquete AT...", SelectAtPackage, 125);
            atPackageButton.Visible = false;
            panel.Controls.Add(options, 0, 1); panel.Controls.Add(memorySummary, 0, 2);
            espMode.SelectionChangeCommitted += (s, e) => { string chosen = espMode.SelectedIndex == 1 ? "at" : "web"; ReadSettings(); SetProfileValue("EspFirmwareMode", chosen); SyncEspOptions(); };
            includeFilesystem.CheckedChanged += (s, e) => {
                if (syncingEspOptions) return;
                bool requested = includeFilesystem.Checked;
                try
                {
                    ReadSettings();
                    if (!EspFlashRules.IsAt(profile))
                    {
                        string directory = Path.GetDirectoryName(profile.Resolve("Linker"));
                        var candidates = Directory.GetFiles(directory, "eagle.flash." + profile.Get("FlashMB") + "m*.ld")
                            .Select(p => WebFlashLayout.Read(p, profile.Number("FlashMB") * 1048576L)).Where(l => (l.FsBytes > 0) == requested).ToArray();
                        var chosen = candidates.FirstOrDefault(l => l.FsBytes == profile.Number("FsBytes")) ?? candidates.OrderByDescending(l => l.FsBytes).FirstOrDefault();
                        if (chosen == null) throw new ArgumentException("No hay linker instalado para esa capacidad/selección de filesystem.");
                        ApplyWebLayout(chosen);
                    }
                    else
                    {
                        if (requested)
                        {
                            var package = AtFirmwarePackage.Read(profile.Resolve("AtPackage"));
                            if (package.Selected(profile).FsBytes == 0) throw new ArgumentException("Este layout AT no declara filesystem. Elegí otra variante del paquete antes de incluirlo.");
                        }
                        SetProfileValue("AtUseFilesystem", requested ? "true" : "false");
                    }
                    if (!requested && target.SelectedIndex >= 2) target.SelectedIndex = 1;
                    SyncEspOptions();
                }
                catch (Exception error) { SyncEspOptions(); memorySummary.Text = error.Message; memorySummary.ForeColor = Color.Firebrick; }
            };
            filesystemLayout.SelectionChangeCommitted += (s, e) => {
                if (syncingEspOptions) return;
                var web = filesystemLayout.SelectedItem as WebFlashLayout; var at = filesystemLayout.SelectedItem as AtFlashLayout; ReadSettings();
                if (web != null) ApplyWebLayout(web);
                else if (at != null) { SetProfileValue("AtLayoutId", at.Id); if (at.FsBytes == 0) SetProfileValue("AtUseFilesystem", "false"); }
                else if (!EspFlashRules.IsAt(profile)) SetProfileValue("WebLayoutPreset", "custom");
                SyncEspOptions();
            };
            settings.CellEndEdit += (s, e) => { if (!syncingEspOptions) { ReadSettings(); SyncEspOptions(); } };
            return panel;
        }
        void SetProfileValue(string key, string value)
        {
            profile.Values[key] = value;
            foreach (DataGridViewRow row in settings.Rows)
                if (((Field)row.Tag).Key == key) { row.Cells[1].Value = value; break; }
        }
        void ApplyWebLayout(WebFlashLayout layout)
        {
            SetProfileValue("WebLayoutPreset", Path.GetFileName(layout.Path)); SetProfileValue("Linker", layout.Path);
            SetProfileValue("FsOffset", "0x" + layout.FsStart.ToString("X")); SetProfileValue("FsBytes", layout.FsBytes.ToString());
            SetProfileValue("FirmwareOffset", "0x0"); SetProfileValue("FirmwareMax", layout.FirmwareMax.ToString());
            SetProfileValue("WebUseFilesystem", layout.FsBytes > 0 ? "true" : "false");
        }
        void SyncEspOptions()
        {
            if (espMode.Items.Count == 0) return;
            syncingEspOptions = true; filesystemLayout.BeginUpdate();
            try
            {
                bool at = EspFlashRules.IsAt(profile);
                atPackageButton.Visible = at;
                Profile.ValidateValue("EspFirmwareMode", profile.Get("EspFirmwareMode"));
                Profile.ValidateValue("WebUseFilesystem", profile.Get("WebUseFilesystem"));
                Profile.ValidateValue("AtUseFilesystem", profile.Get("AtUseFilesystem"));
                espMode.SelectedIndex = at ? 1 : 0; includeFilesystem.Checked = EspFlashRules.UseFilesystem(profile);
                filesystemLayout.Items.Clear(); memorySummary.ForeColor = Color.FromArgb(22, 79, 99);
                if (at)
                {
                    if (String.IsNullOrWhiteSpace(profile.Get("AtPackage")))
                    { memorySummary.Text = "AT por USB: elegí un paquete con archivos, offsets, capacidad y layouts declarados por su proveedor. No usa el layout Arduino/LittleFS."; return; }
                    var package = AtFirmwarePackage.Read(profile.Resolve("AtPackage"));
                    foreach (var layout in package.Layouts) filesystemLayout.Items.Add(layout);
                    var selected = package.Selected(profile);
                    filesystemLayout.SelectedItem = selected;
                    memorySummary.Text = package.Name + " | Flash requerida: " + EspFlashRules.Bytes(package.FlashBytes) + " | Regiones de firmware: " + EspFlashRules.Bytes(selected.CodeBytes) + " | FS " + selected.Filesystem + ": " + EspFlashRules.Bytes(selected.FsBytes) +
                        "\r\nSistema/sin asignar: " + EspFlashRules.Bytes(package.FlashBytes - selected.CodeBytes - selected.FsBytes) + (includeFilesystem.Checked ? ". Se incluirán las imágenes FS del paquete." : ". La reserva FS del paquete existe, pero sus imágenes no se cargarán.") + " La flash física se consulta antes de escribir.";
                    if (package.FlashBytes != profile.Number("FlashMB") * 1048576L) throw new ArgumentException("La flash esperada del perfil no coincide con el paquete AT. Ajustá Flash esperada en Configuración.");
                    if (includeFilesystem.Checked && selected.FsBytes == 0) throw new ArgumentException("Este layout AT no tiene filesystem; desmarcá la casilla o elegí otra variante.");
                }
                else
                {
                    long flash = profile.Number("FlashMB") * 1048576L; string path = profile.Resolve("Linker");
                    var selected = WebFlashLayout.Read(path, flash);
                    filesystemLayout.Items.Add("Personalizado (valores del perfil)");
                    foreach (string file in Directory.GetFiles(Path.GetDirectoryName(path), "eagle.flash." + profile.Get("FlashMB") + "m*.ld").OrderBy(p => p))
                    {
                        var candidate = WebFlashLayout.Read(file, flash);
                        if ((candidate.FsBytes > 0) == includeFilesystem.Checked) filesystemLayout.Items.Add(candidate);
                    }
                    filesystemLayout.SelectedItem = profile.Get("WebLayoutPreset") == "custom" ? filesystemLayout.Items[0] : filesystemLayout.Items.Cast<object>().OfType<WebFlashLayout>().FirstOrDefault(l => Path.GetFileName(l.Path) == profile.Get("WebLayoutPreset"));
                    if (filesystemLayout.SelectedIndex < 0) filesystemLayout.SelectedIndex = 0;
                    memorySummary.Text = selected.Summary() + (includeFilesystem.Checked ? "" : " Sin FS no se cargan los activos Web.");
                    if (includeFilesystem.Checked != (selected.FsBytes > 0)) throw new ArgumentException("La casilla filesystem no coincide con el linker. Elegí un layout compatible.");
                }
            }
            catch (Exception error) { memorySummary.Text = error.Message; memorySummary.ForeColor = Color.Firebrick; }
            finally { filesystemLayout.EndUpdate(); syncingEspOptions = false; UpdateEspTargetLabels(); }
        }
        void SelectAtPackage()
        {
            if (!EspFlashRules.IsAt(profile) || busy || pendingJob != null) return;
            profileDialogs++;
            try
            {
                using (var dialog = new OpenFileDialog { Title = "Paquete AT: mapa de carga y variantes de filesystem", Filter = "Paquete AFR2 AT (*.json)|*.json" })
                {
                    if (dialog.ShowDialog(this) != DialogResult.OK) return;
                    var package = AtFirmwarePackage.Read(dialog.FileName); ReadSettings();
                    SetProfileValue("AtPackage", dialog.FileName); SetProfileValue("AtLayoutId", package.Layouts[0].Id);
                    if (package.Layouts[0].FsBytes == 0) SetProfileValue("AtUseFilesystem", "false");
                    SetProfileValue("EspFirmwareMode", "at"); SyncEspOptions();
                }
            }
            finally { profileDialogs--; }
        }
        void UpdateEspTargetLabels()
        {
            if (compileTarget == null || flashTarget == null) return;
            if (!EspFlashRules.UseFilesystem(profile) && target.SelectedIndex >= 2 && !busy && pendingJob == null) target.SelectedIndex = 1;
            bool at = EspFlashRules.IsAt(profile);
            string[] labels = { "Firmware ATmega (producto completo)", at ? "Firmware ESP AT" : "Firmware ESP Web / Arduino", at ? "Filesystem del paquete AT" : "Web / LittleFS en ESP", at ? "Paquete AT + filesystem" : "Firmware ESP + Web / LittleFS" };
            int selected = target.SelectedIndex;
            for (int i = 0; i < labels.Length; i++)
            {
                if (Convert.ToString(target.Items[i]) != labels[i]) target.Items[i] = labels[i];
                if (i < compileTarget.DropDownItems.Count) compileTarget.DropDownItems[i].Text = labels[i];
            }
            if (target.SelectedIndex != selected) target.SelectedIndex = selected;
            UpdateTargetActions();
        }
    }
}
