using System;
using System.Drawing;
using System.Windows.Forms;

namespace AFR2Loader
{
    public sealed partial class MainForm
    {
        LoaderPreferences preferences;
        TabPage preferencesTab;
        readonly CheckBox backupBeforeFlasher = new CheckBox { Text = "Backup de ATmega antes de cargar", AutoSize = true, Checked = true };
        readonly CheckBox defaultMegaBackup = new CheckBox { Text = "Marcar Backup de ATmega antes de cargar por defecto", AutoSize = true };
        readonly CheckBox showNoBackupWarning = new CheckBox { Text = "Mostrar aviso al cargar flasher sin crear backup", AutoSize = true };
        void InitializePreferences(TabControl tabs)
        {
            try { preferences = LoaderPreferences.Read(); }
            catch (Exception error) { preferences = new LoaderPreferences(); MessageBox.Show(this, "No se pudieron leer las preferencias; se usan los valores predeterminados hasta Guardar.\r\n" + error.Message, "Preferencias"); }
            backupBeforeFlasher.Checked = preferences.BackupMegaBeforeFlasher;
            var page = new TabPage("Preferencias") { Padding = new Padding(20) };
            preferencesTab = page;
            var layout = new FlowLayoutPanel { Dock = DockStyle.Fill, FlowDirection = FlowDirection.TopDown, WrapContents = false, AutoScroll = true };
            layout.Controls.Add(new Label { Text = "Backup y avisos de ATmega", Font = new Font(Font.FontFamily, 16, FontStyle.Bold), Width = 650, Height = 45 });
            defaultMegaBackup.Checked = preferences.BackupMegaBeforeFlasher; showNoBackupWarning.Checked = preferences.ShowNoMegaBackupWarning;
            layout.Controls.Add(defaultMegaBackup); layout.Controls.Add(showNoBackupWarning); idleControls.Add(defaultMegaBackup); idleControls.Add(showNoBackupWarning);
            layout.Controls.Add(new Label { Text = "La casilla de Flasher ATmega se puede cambiar para esa carga. Aquí elegís su valor predeterminado.\r\nNo mostrar de nuevo desactiva solo el aviso por omitir backup; no elimina los asistentes de cableado ni la confirmación de escritura.\r\n\r\nEl backup lee flash del equipo; EEPROM y fuses no se restauran. Flashear último backup usa solo un respaldo leído y completado, nunca una antigua copia de HEX de PC.", Width = 760, Height = 165 });
            AddButton(layout, "Guardar preferencias", () => {
                var next = new LoaderPreferences { BackupMegaBeforeFlasher = defaultMegaBackup.Checked, ShowNoMegaBackupWarning = showNoBackupWarning.Checked };
                next.Save(); preferences = next; backupBeforeFlasher.Checked = next.BackupMegaBeforeFlasher;
                Log("Preferencias guardadas. Backup predeterminado=" + next.BackupMegaBeforeFlasher + "; aviso sin backup=" + next.ShowNoMegaBackupWarning);
            }, 220);
            AddButton(layout, "Restablecer valores predeterminados", () => { defaultMegaBackup.Checked = true; showNoBackupWarning.Checked = true; }, 320);
            layout.Controls.Add(new Label { Text = "Preferencias locales: " + LoaderPreferences.FilePath + "\r\nLos cambios se conservan para todos los perfiles de este usuario al Guardar. El botón Restablecer requiere Guardar.", Width = 760, Height = 80 });
            page.Controls.Add(layout); tabs.TabPages.Add(page);
        }
        bool ConfirmNoMegaBackup()
        {
            if (!preferences.ShowNoMegaBackupWarning) { Log("Carga de flasher sin backup nuevo; aviso suprimido por preferencia del usuario."); return true; }
            using (var dialog = new Form { Text = "No se va a crear backup del ATmega", Size = new Size(720, 320), StartPosition = FormStartPosition.CenterParent, Font = Font, MinimizeBox = false, MaximizeBox = false })
            {
                var layout = new TableLayoutPanel { Dock = DockStyle.Fill, Padding = new Padding(16), ColumnCount = 1, RowCount = 3 };
                layout.RowStyles.Add(new RowStyle(SizeType.Percent, 100)); layout.RowStyles.Add(new RowStyle(SizeType.Absolute, 40)); layout.RowStyles.Add(new RowStyle(SizeType.Absolute, 42));
                layout.Controls.Add(new Label { Dock = DockStyle.Fill, Text = "Backup de ATmega antes de cargar está desmarcado.\r\n\r\nSe reemplazará el programa del Mega SIN crear un respaldo nuevo de lo que tiene instalado. Los backups anteriores, si existen, se conservan y pueden corresponder a otro programa.\r\nPodrás cargar un firmware conocido desde Configuración o el último backup válido disponible." }, 0, 0);
                var noMore = new CheckBox { Text = "No mostrar de nuevo", AutoSize = true }; layout.Controls.Add(noMore, 0, 1);
                var buttons = new FlowLayoutPanel { Dock = DockStyle.Fill, FlowDirection = FlowDirection.RightToLeft };
                var proceed = new Button { Text = "Continuar sin backup", Width = 180, Height = 32, DialogResult = DialogResult.OK };
                var cancelSkip = new Button { Text = "Cancelar", Width = 115, Height = 32, DialogResult = DialogResult.Cancel };
                buttons.Controls.Add(proceed); buttons.Controls.Add(cancelSkip); layout.Controls.Add(buttons, 0, 2);
                dialog.Controls.Add(layout); dialog.AcceptButton = proceed; dialog.CancelButton = cancelSkip;
                if (dialog.ShowDialog(this) != DialogResult.OK) return false;
                if (noMore.Checked)
                {
                    var next = new LoaderPreferences { BackupMegaBeforeFlasher = preferences.BackupMegaBeforeFlasher, ShowNoMegaBackupWarning = false };
                    next.Save(); preferences = next; showNoBackupWarning.Checked = false;
                    Log("Aviso sin backup desactivado; se puede reactivar en Preferencias.");
                }
                return true;
            }
        }
        ToolStrip CreateFirmwareFlashMenu(bool useSelectedTarget)
        {
            var menu = new ToolStrip { GripStyle = ToolStripGripStyle.Hidden, Dock = DockStyle.None, AutoSize = true, Margin = new Padding(0, 4, 8, 4), Padding = new Padding(0), RenderMode = ToolStripRenderMode.System };
            var button = new ToolStripSplitButton("Flashear Firmware ATmega") { AutoSize = false, Size = new Size(285, 32) };
            button.ButtonClick += (s, e) => { if (useSelectedTarget) Flash(); else FlashMegaFirmware(); };
            button.DropDownItems.Add("Flashear Firmware ATmega", null, (s, e) => FlashMegaFirmware());
            button.DropDownItems.Add("Flashear último backup", null, (s, e) => FlashMegaBackup());
            menu.Items.Add(button); idleControls.Add(menu);
            if (useSelectedTarget) flashTarget = button;
            return menu;
        }
        void FlashMegaFirmware()
        {
            if (busy || pendingJob != null || profileDialogs > 0) return;
            target.SelectedIndex = 0; Flash();
        }
        void FlashMegaBackup()
        {
            if (busy || pendingJob != null || profileDialogs > 0) return;
            target.SelectedIndex = 0;
            Execute("Flashear último backup de ATmega", async (ops, p, port, token) => { await ops.FlashLatestMegaBackup(p, port, token); UpdateBridgeServiceState(); }, true);
        }
    }
}
