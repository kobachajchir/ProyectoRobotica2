using System;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Windows.Forms;

namespace AFR2Loader
{
    public sealed partial class MainForm
    {
        string portCheckState = "COM sin comprobar";
        readonly ComboBox checklistMode = new ComboBox { Width = 610, DropDownStyle = ComboBoxStyle.DropDownList };
        void InitializeLogMenu(ToolStripMenuItem menu)
        {
            var logs = new ToolStripMenuItem("Carpeta de registros");
            logs.DropDownItems.Add("Ver carpeta de registros", null, (s, e) => OpenLogs());
            logs.DropDownItems.Add("Limpiar carpeta de registros...", null, (s, e) => ClearSavedLogs()); menu.DropDownItems.Add(logs);
        }
        void ClearSavedLogs()
        {
            if (busy || pendingJob != null || serialTransition || (serialCapture != null && serialCapture.Active)) { MessageBox.Show(this, "Terminá la operación y cerrá el monitor antes de limpiar registros."); return; }
            string root = Path.GetFullPath(Path.Combine(Profile.UserRoot, "logs"));
            if (!Directory.Exists(root)) { MessageBox.Show(this, "No hay registros para limpiar."); return; }
            var files = Directory.GetFiles(root, "*", SearchOption.TopDirectoryOnly).Select(p => new FileInfo(p)).ToArray();
            if (MessageBox.Show(this, "Se eliminarán " + files.Length + " archivos de registro (" + EspFlashRules.Bytes(files.Sum(f => f.Length)) + ").\r\n" + root + "\r\nNo incluye perfiles ni backups. ¿Continuar?", "Limpiar carpeta de registros", MessageBoxButtons.YesNo, MessageBoxIcon.Warning) != DialogResult.Yes) return;
            int removed = 0;
            try { foreach (var file in files) { if (!String.Equals(file.DirectoryName, root, StringComparison.OrdinalIgnoreCase)) throw new IOException("Ruta fuera de la carpeta de registros."); file.Delete(); removed++; } ClipboardToast.Display(this, removed + " registros eliminados"); }
            catch (Exception error) { MessageBox.Show(this, removed + " registros eliminados. No se pudo continuar: " + error.Message); }
        }
        void ShowPreferences(string section)
        {
            if (busy || pendingJob != null) return;
            profileDialogs++;
            try {
            using (var dialog = new Form { Text = "Preferencias", Size = new Size(820, 480), MinimumSize = new Size(720, 420), Font = Font, StartPosition = FormStartPosition.CenterParent })
            {
                var layout = new TableLayoutPanel { Dock = DockStyle.Fill, Padding = new Padding(UiInset), ColumnCount = 1, RowCount = 2 };
                layout.RowStyles.Add(new RowStyle(SizeType.Percent, 100)); layout.RowStyles.Add(new RowStyle(SizeType.Absolute, 52));
                var tabs = new TabControl { Dock = DockStyle.Fill };
                var backup = new TabPage("Backups ATmega") { Padding = new Padding(UiInset) };
                var checks = new TabPage("Asistentes") { Padding = new Padding(UiInset) };
                var logs = new TabPage("Registros") { Padding = new Padding(UiInset) };
                tabs.TabPages.AddRange(new[] { backup, checks, logs });
                var backupRow = UiRow(); backupRow.FlowDirection = FlowDirection.TopDown;
                defaultMegaBackup.Checked = preferences.BackupMegaBeforeFlasher; showNoBackupWarning.Checked = preferences.ShowNoMegaBackupWarning;
                backupRow.Controls.Add(defaultMegaBackup); backupRow.Controls.Add(showNoBackupWarning);
                backupRow.Controls.Add(new Label { AutoSize = true, MaximumSize = new Size(690, 0), Text = "El respaldo se lee del equipo antes de cargar el flasher. Los respaldos anteriores se conservan. No incluye EEPROM ni restauración de fuses." }); backup.Controls.Add(backupRow);
                var checksRow = UiRow(); checksRow.FlowDirection = FlowDirection.TopDown;
                if (checklistMode.Items.Count == 0) checklistMode.Items.AddRange(new object[] { "Requerir casillas de control antes de continuar", "Casillas marcadas por defecto; permitir revisarlas", "Sin casillas de control; conservar confirmación final" });
                checklistMode.SelectedIndex = !preferences.RequireChecklist ? 2 : preferences.ChecklistsInitiallyChecked ? 1 : 0;
                checksRow.Controls.Add(UiField("Modo de revisión", checklistMode));
                checksRow.Controls.Add(new Label { AutoSize = true, MaximumSize = new Size(690, 0), Text = "Esta preferencia se aplica a los asistentes de cableado y carga. Las consultas de COM, capacidad, hashes y verificación siguen activas. Las solicitudes MCP conservan su aprobación propia." }); checks.Controls.Add(checksRow);
                var logsRow = UiRow(); logsRow.FlowDirection = FlowDirection.TopDown;
                logsRow.Controls.Add(new Label { AutoSize = true, MaximumSize = new Size(690, 0), Text = "Registros locales: " + Path.Combine(Profile.UserRoot, "logs") });
                AddButton(logsRow, "Ver carpeta de registros", OpenLogs, 250); AddButton(logsRow, "Limpiar carpeta de registros...", ClearSavedLogs, 285); logs.Controls.Add(logsRow);
                tabs.SelectedTab = section == "Asistentes" ? checks : section == "Registros" ? logs : backup;
                var actions = new TableLayoutPanel { Dock = DockStyle.Fill, ColumnCount = 2 };
                actions.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100)); actions.ColumnStyles.Add(new ColumnStyle(SizeType.AutoSize));
                var reset = new ActionButton { Text = "Restablecer valores predeterminados", AutoSize = true, Anchor = AnchorStyles.Left };
                reset.Click += (s, e) => { defaultMegaBackup.Checked = showNoBackupWarning.Checked = true; checklistMode.SelectedIndex = 0; };
                var save = new ActionButton { Text = "Guardar preferencias", AutoSize = true, Anchor = AnchorStyles.Right };
                save.Click += (s, e) => {
                    var next = new LoaderPreferences { BackupMegaBeforeFlasher = defaultMegaBackup.Checked, ShowNoMegaBackupWarning = showNoBackupWarning.Checked,
                        RequireChecklist = checklistMode.SelectedIndex != 2, ChecklistsInitiallyChecked = checklistMode.SelectedIndex == 1 };
                    try { next.Save(); preferences = next; backupBeforeFlasher.Checked = next.BackupMegaBeforeFlasher; ClipboardToast.Display(dialog, "Preferencias guardadas"); }
                    catch (Exception error) { MessageBox.Show(dialog, error.Message, "Preferencias"); }
                };
                actions.Controls.Add(reset, 0, 0); actions.Controls.Add(save, 1, 0); layout.Controls.Add(tabs, 0, 0); layout.Controls.Add(actions, 0, 1); dialog.Controls.Add(layout); ShowStyledDialog(dialog);
                // Reused preference controls must survive disposal of their dialog.
                backupRow.Controls.Remove(defaultMegaBackup); backupRow.Controls.Remove(showNoBackupWarning); checklistMode.Parent.Controls.Remove(checklistMode);
            }
            } finally { profileDialogs--; }
        }
    }
}
