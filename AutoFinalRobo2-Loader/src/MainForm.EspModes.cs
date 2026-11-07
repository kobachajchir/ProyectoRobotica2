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

        Control CreateOperationSelectors()
        {
            var selectors = UiRow(); selectors.WrapContents = true;
            var portGroup = UiRow(); portGroup.WrapContents = false; portGroup.Margin = new Padding(0, 0, 24, 0);
            ports.Width = 115; ports.DropDownStyle = ComboBoxStyle.DropDownList;
            portGroup.Controls.Add(UiField("Puerto", ports)); idleControls.Add(ports);
            AddButton(portGroup, "Actualizar puertos", RefreshPorts, 165).Margin = new Padding(16, 4, 0, 4);
            var targetGroup = UiRow(); targetGroup.WrapContents = false; targetGroup.Margin = new Padding(0);
            deviceTarget.Items.AddRange(new object[] { "ATmega", "ESP" }); deviceTarget.SelectedIndex = 0;
            idleControls.Add(deviceTarget); targetGroup.Controls.Add(UiField("Destino", deviceTarget));
            detectTarget = AddButton(targetGroup, "Detectar ATmega", DetectSelected, 180); detectTarget.Margin = new Padding(16, 4, 0, 4);
            deviceTarget.SelectionChangeCommitted += (s, e) => SelectOperationTarget(deviceTarget.SelectedIndex == 0 ? 0 : espSection.SelectedIndex + 1);
            selectors.Controls.Add(portGroup); selectors.Controls.Add(targetGroup); return selectors;
        }
        Label preparationHeading;
        Label SectionLabel(string text)
        { return new Label { Text = text, AutoSize = true, Font = new Font("Segoe UI", 10.5F, FontStyle.Bold), ForeColor = Color.FromArgb(22, 79, 99), Margin = new Padding(0, UiGap, 0, UiGap) }; }
        Control CreateEspOptions(Control selectors)
        {
            // AFR2-20261106-06: destination and connection first, then preparation; execution follows below.
            var panel = new TableLayoutPanel { Dock = DockStyle.Top, AutoSize = true, ColumnCount = 1, RowCount = 8, Margin = new Padding(0) };
            for (int i = 0; i < 8; i++) panel.RowStyles.Add(new RowStyle(SizeType.AutoSize));
            panel.Controls.Add(SectionLabel("1 · Destino y conexión"), 0, 0);
            panel.Controls.Add(selectors, 0, 1);
            var mega = UiRow();
            megaVariant.Items.AddRange(new object[] { "Firmware completo del auto", "Solo comunicación" }); megaVariant.SelectedIndex = 0;
            mega.Controls.Add(UiField("Variante ATmega", megaVariant)); idleControls.Add(megaVariant); megaOptionsRow = mega;
            megaVariant.SelectionChangeCommitted += (s, e) => UpdateTargetActions(); panel.Controls.Add(mega, 0, 2);
            var options = UiRow();
            espMode.Width = 195; espMode.Items.AddRange(new object[] { "Firmware propio", "Paquete AT" });
            options.Controls.Add(UiField("Variante de firmware ESP", espMode)); idleControls.Add(espMode);
            espSection.Items.AddRange(new object[] { "Firmware ESP", "FileSystem", "Firmware ESP + FileSystem" }); espSection.SelectedIndex = 0;
            espSectionField = UiField("Sección del paquete", espSection); options.Controls.Add(espSectionField); idleControls.Add(espSection);
            atPackageButton = AddButton(options, "Paquete AT / BIN...", SelectAtPackage, 185); atPackageButton.Visible = false;
            espOptionsRow = options; panel.Controls.Add(options, 0, 3);
            preparationHeading = SectionLabel("2 · Preparación"); panel.Controls.Add(preparationHeading, 0, 5);
            var capacity = UiRow(); AddButton(capacity, "Consultar tamaño de flash…", () => ConfirmFlashCapacity(), 280); capacity.Controls.Add(capacityState); capacityRow = capacity; panel.Controls.Add(capacity, 0, 6);
            espMemoryRow = CreateFilesystemCard(); idleControls.Add(filesystemLayout);
            panel.Controls.Add(espMemoryRow, 0, 7);
            espMethodRow = CreateEspMethodSelector();
            listen.Text = "Escuchar arranque ESP (74880)"; listen.AutoSize = true; listen.Checked = true;
            workflowTips.SetToolTip(listen, "Activar escucha del arranque en USB–TTL/puente USB. Desmarcar no omite la intervención física KEY/RST obligatoria. El flasher Mega usa su baud fijo.");
            ((FlowLayoutPanel)espMethodRow).Controls.Add(listen); ((FlowLayoutPanel)espMethodRow).WrapContents = true; idleControls.Add(listen);
            panel.Controls.Add(espMethodRow, 0, 4);
            espMode.SelectionChangeCommitted += (s, e) => {
                try { ReadSettings(); SetProfileValue("EspFirmwareMode", espMode.SelectedIndex == 1 ? "at" : "web");
                    if (espMode.SelectedIndex == 1) { SetProfileValue("AtUseFilesystem", "false"); SelectOperationTarget(1); } else ApplySectionFilesystem();
                } catch (Exception error) { MessageBox.Show(this, error.Message, "Variante ESP"); }
                SyncEspOptions(); UpdateDestinationVisibility();
            };
            espSection.SelectionChangeCommitted += (s, e) => {
                int previous = target.SelectedIndex;
                try { SelectOperationTarget(espSection.SelectedIndex + 1); ApplySectionFilesystem(); SyncEspOptions(); }
                catch (Exception error) { SelectOperationTarget(previous); SyncEspOptions(); MessageBox.Show(this, error.Message, "Sección del paquete"); }
            };
            filesystemLayout.SelectionChangeCommitted += (s, e) => {
                if (syncingEspOptions) return;
                var web = filesystemLayout.SelectedItem as WebFlashLayout; var at = filesystemLayout.SelectedItem as AtFlashLayout; ReadSettings();
                if (!CapacityConfirmed) return;
                if (web != null) ApplyWebLayout(web);
                else if (at != null) { SetProfileValue("AtLayoutId", at.Id); SetProfileValue("AtUseFilesystem", target.SelectedIndex >= 2 ? "true" : "false"); }
                else if (!EspFlashRules.IsAt(profile)) SetProfileValue("WebLayoutPreset", "custom");
                SyncEspOptions(); UpdateDestinationVisibility();
            };
            settings.CellEndEdit += (s, e) => { if (!syncingEspOptions) { ReadSettings(); SyncEspOptions(); } };
            UpdateDestinationVisibility(); return panel;
        }
        void ApplySectionFilesystem()
        {
            ReadSettings(); bool filesystem = target.SelectedIndex >= 2;
            // AFR2-20261029-22: selection is configuration only. Detection has its own button.
            if (filesystem && !CapacityConfirmed) {
                SetProfileValue(EspFlashRules.IsAt(profile) ? "AtUseFilesystem" : "WebUseFilesystem", "true");
                return;
            }
            if (EspFlashRules.IsAt(profile))
            {
                if (filesystem && AtFirmwarePackage.Read(profile.Resolve("AtPackage")).Selected(profile).FsBytes == 0)
                    throw new ArgumentException("Este paquete AT es monolítico y no tiene Web/filesystem separable. Elegí Firmware ESP.");
                SetProfileValue("AtUseFilesystem", filesystem ? "true" : "false"); return;
            }
            if (filesystem)
            {
                string directory = Path.GetDirectoryName(profile.Resolve("Linker"));
                var layouts = Directory.GetFiles(directory, "eagle.flash." + profile.Get("FlashMB") + "m*.ld")
                    .Select(p => WebFlashLayout.Read(p, profile.Number("FlashMB") * 1048576L)).Where(l => l.FsBytes > 0).ToArray();
                var chosen = layouts.Where(l => l.FsBytes == profile.Number("FsBytes")).OrderByDescending(l => l.FirmwareMax).FirstOrDefault() ?? layouts.OrderByDescending(l => l.FsBytes).FirstOrDefault();
                if (chosen == null) throw new ArgumentException("No hay layouts de filesystem instalados para la capacidad del perfil.");
                ApplyWebLayout(chosen);
            }
            // Firmware-only keeps its compiled linker and memory map. Selecting a
            // section controls which images are written, not the firmware's ABI.
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
                capacityState.Text = CapacityConfirmed ? "Capacidad confirmada: " + profile.Get("FlashMB") + " MiB (detectada por esptool)" : "Consultá el ESP para detectar su capacidad real.";
                if (!CapacityConfirmed) {
                    string packageName = at && !String.IsNullOrWhiteSpace(profile.Get("AtPackage")) ? "Paquete seleccionado: " + Path.GetFileName(profile.Resolve("AtPackage")) + ". " : "";
                    memorySummary.Text = packageName + "Usá Consultar tamaño de flash para detectar el ESP; luego prepará o revisá el paquete. Flashear se solicita por separado.";
                    return;
                }
                if (at)
                {
                    if (String.IsNullOrWhiteSpace(profile.Get("AtPackage")))
                    { memorySummary.Text = "AT por USB: elegí el BIN completo Ai-Thinker v1.5.4.1 reconocido o un manifiesto JSON del paquete. No usa el layout Web/LittleFS."; return; }
                    var package = AtFirmwarePackage.Read(profile.Resolve("AtPackage"));
                    foreach (var layout in package.Layouts) filesystemLayout.Items.Add(layout);
                    var selected = package.Selected(profile);
                    filesystemLayout.SelectedItem = selected;
                    memorySummary.Text = package.Name + " | Flash requerida: " + EspFlashRules.Bytes(package.FlashBytes) + " | Regiones de firmware: " + EspFlashRules.Bytes(selected.CodeBytes) + " | FS " + selected.Filesystem + ": " + EspFlashRules.Bytes(selected.FsBytes) +
                        "\r\nSistema/sin asignar: " + EspFlashRules.Bytes(package.FlashBytes - selected.CodeBytes - selected.FsBytes) + (includeFilesystem.Checked ? ". Se incluirán las imágenes FS del paquete." : ". La reserva FS del paquete existe, pero sus imágenes no se cargarán.") + " La flash física se consulta antes de escribir.";
                    if (package.FlashBytes != profile.Number("FlashMB") * 1048576L) throw new ArgumentException("La flash esperada del perfil no coincide con el paquete AT. Ajustá Flash esperada en Configuración.");
                    if (includeFilesystem.Checked && selected.FsBytes == 0) throw new ArgumentException("Este layout AT no tiene filesystem separable; elegí Firmware ESP u otra variante.");
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
                    memorySummary.Text = selected.Summary() + (includeFilesystem.Checked ? "" : " Sin FileSystem no se cargan sus archivos.");
                    if (includeFilesystem.Checked != (selected.FsBytes > 0)) throw new ArgumentException("El filesystem configurado no coincide con el linker. Elegí un tamaño compatible.");
                }
            }
            catch (Exception error) { memorySummary.Text = error.Message; memorySummary.ForeColor = Color.Firebrick; }
            finally { filesystemLayout.EndUpdate(); syncingEspOptions = false; UpdateEspTargetLabels(); UpdateFilesystemPresentation(); FitOperationRows(); }
        }
        void SelectAtPackage()
        {
            if (!EspFlashRules.IsAt(profile) || busy || pendingJob != null) return;
            profileDialogs++;
            try
            {
                using (var dialog = new OpenFileDialog { Title = "Firmware AT: BIN completo reconocido o manifiesto JSON", Filter = "Firmware AT (*.bin;*.json)|*.bin;*.json|BIN completo (*.bin)|*.bin|Paquete AFR2 AT (*.json)|*.json", CheckFileExists = true })
                {
                    if (ShowStyledDialog(dialog) != DialogResult.OK) return;
                    var package = AtFirmwarePackage.Read(dialog.FileName); ReadSettings();
                    BuildPreparation.Invalidate("at");
                    SetProfileValue("AtPackage", dialog.FileName); SetProfileValue("AtLayoutId", package.Layouts[0].Id);
                    if (package.Layouts[0].FsBytes == 0) SetProfileValue("AtUseFilesystem", "false");
                    SetProfileValue("EspFirmwareMode", "at"); SelectOperationTarget(1); SyncEspOptions();
                    status.Text = "Paquete seleccionado · preparación pendiente";
                    lastResult.Text = "Revisá la configuración y pulsá Revisar paquete AT. La carga comienza únicamente al pulsar Flashear.";
                    Log("Paquete AT seleccionado: " + dialog.FileName + ". No se inició detección, preparación ni flasheo.");
                    ActiveControl = atPackageButton;
                }
            }
            catch (Exception error) { MessageBox.Show(this, error.Message, "Firmware AT", MessageBoxButtons.OK, MessageBoxIcon.Error); }
            finally { profileDialogs--; UpdatePreparationControls(); }
        }
        void UpdateEspTargetLabels()
        {
            if (compileTarget == null || flashTarget == null) return;
            UpdateDestinationVisibility(); UpdateTargetActions();
        }
    }
}


