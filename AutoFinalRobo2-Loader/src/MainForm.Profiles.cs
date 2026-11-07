// AFR2-20261027-21. Local saved-profile library; schema 1 files remain portable.
using System;
using System.IO;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace AFR2Loader
{
    public sealed partial class MainForm
    {
        readonly ComboBox savedProfiles = new ComboBox { Dock = DockStyle.Fill, DropDownStyle = ComboBoxStyle.DropDownList };
        static readonly string profilesDirectory = Path.Combine(Profile.UserRoot, "profiles");
        static readonly string activeProfileFile = Path.Combine(Profile.UserRoot, "active-profile.txt");
        string savedProfileJson;
        // AFR2-20261106-06: save confirmation and pending-change marker beside the Save button.
        readonly Label saveState = new Label { AutoSize = true, Margin = new Padding(8, 12, 0, 0) };
        DateTime? savedStamp;
        void UpdateSaveState()
        {
            bool dirty = false;
            try
            {
                Profile copy = profile.Copy();
                foreach (DataGridViewRow row in settings.Rows) { var field = row.Tag as Field; if (field != null) copy.Values[field.Key] = Convert.ToString(row.Cells[1].Value).Trim(); }
                dirty = copy.ToJson() != savedProfileJson;
            }
            catch (Exception) { return; }
            saveState.Text = dirty ? "● Cambios sin guardar" : savedStamp.HasValue ? "✓ Guardado a las " + savedStamp.Value.ToString("HH:mm:ss") : "Sin cambios pendientes";
            saveState.ForeColor = dirty ? LoaderAppearance.WarningInk : LoaderAppearance.SuccessInk;
        }
        bool refreshingProfiles;
        int profileDialogs;

        sealed class SavedProfile
        {
            public string Path, Label;
            public override string ToString() { return Label; }
        }
        void InitializeSavedProfiles(TableLayoutPanel heading)
        {
            savedProfileJson = profile.ToJson();
            var row = new TableLayoutPanel { Dock = DockStyle.Fill, ColumnCount = 2, Margin = new Padding(0) };
            row.ColumnStyles.Add(new ColumnStyle(SizeType.Absolute, 130));
            row.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100));
            row.Controls.Add(new Label { Text = "Perfil guardado", Dock = DockStyle.Fill, TextAlign = System.Drawing.ContentAlignment.MiddleLeft }, 0, 0);
            row.Controls.Add(savedProfiles, 1, 0); heading.Controls.Add(row, 0, 0);
            idleControls.Add(savedProfiles);
            savedProfiles.DropDown += (s, e) => RefreshSavedProfiles();
            savedProfiles.SelectionChangeCommitted += (s, e) => SelectSavedProfile();
            RefreshSavedProfiles();
        }
        void RefreshSavedProfiles()
        {
            refreshingProfiles = true; savedProfiles.BeginUpdate();
            try
            {
                savedProfiles.Items.Clear();
                if (Directory.Exists(profilesDirectory))
                {
                    foreach (string path in Directory.GetFiles(profilesDirectory, "*.json").OrderBy(p => p, StringComparer.CurrentCultureIgnoreCase))
                    {
                        string name;
                        try { name = Profile.FromJson(File.ReadAllText(path)).Get("Name"); }
                        catch (Exception) { name = "Perfil no legible"; }
                        savedProfiles.Items.Add(new SavedProfile { Path = path, Label = name + " — " + Path.GetFileName(path) });
                    }
                }
                var selected = savedProfiles.Items.Cast<SavedProfile>().FirstOrDefault(p => SamePath(p.Path, profilePath));
                if (selected == null)
                {
                    selected = new SavedProfile { Path = profilePath, Label = profile.Get("Name") + " (sin guardar)" };
                    savedProfiles.Items.Insert(0, selected);
                }
                savedProfiles.SelectedItem = selected;
            }
            catch (Exception error) { Log("No se pudo listar la carpeta de perfiles: " + error.Message); }
            finally { savedProfiles.EndUpdate(); refreshingProfiles = false; }
        }
        static bool SamePath(string left, string right)
        { return String.Equals(left, right, StringComparison.OrdinalIgnoreCase); }
        void SelectSavedProfile()
        {
            if (refreshingProfiles || busy || pendingJob != null) return;
            var selected = savedProfiles.SelectedItem as SavedProfile;
            if (selected == null || selected.Path == null || SamePath(selected.Path, profilePath)) return;
            string path = selected.Path;
            profileDialogs++;
            try
            {
                // Parse first, so a damaged entry never replaces the visible profile.
                Profile next = Profile.FromJson(File.ReadAllText(path));
                if (!CanLeaveProfile()) { RefreshSavedProfiles(); return; }
                profile = next; profilePath = path; savedProfileJson = profile.ToJson();
                FillSettings(); RememberActiveProfile(); RefreshSavedProfiles();
                Log("Perfil seleccionado: " + profile.Get("Name") + " | " + profilePath);
            }
            catch (Exception error)
            { RefreshSavedProfiles(); MessageBox.Show(this, error.Message, "Seleccionar perfil", MessageBoxButtons.OK, MessageBoxIcon.Error); }
            finally { profileDialogs--; }
        }
        bool CanLeaveProfile()
        {
            ReadSettings();
            if (profile.ToJson() == savedProfileJson) return true;
            var choice = MessageBox.Show(this, "Hay cambios sin guardar en el perfil visible. ¿Guardarlos antes de cambiar de perfil?", "Cambiar de perfil", MessageBoxButtons.YesNoCancel, MessageBoxIcon.Question);
            if (choice == DialogResult.Cancel) return false;
            return choice == DialogResult.No || TrySaveProfile();
        }
        void SaveProfile() { TrySaveProfile(); }
        bool TrySaveProfile()
        {
            ReadSettings();
            if (String.IsNullOrEmpty(profilePath)) return SaveProfileAs();
            profile.Save(profilePath); savedProfileJson = profile.ToJson(); savedStamp = DateTime.Now; UpdateSaveState();
            RememberActiveProfile(); RefreshSavedProfiles(); Log("Perfil guardado: " + profilePath);
            return true;
        }
        bool SaveProfileAs()
        {
            profileDialogs++;
            try { return SaveProfileAsCore(); }
            finally { profileDialogs--; }
        }
        bool SaveProfileAsCore()
        {
            ReadSettings(); Directory.CreateDirectory(profilesDirectory);
            string name = profile.Get("Name");
            foreach (char invalid in Path.GetInvalidFileNameChars()) name = name.Replace(invalid, '_');
            name = name.Trim().TrimEnd('.');
            if (String.IsNullOrEmpty(name)) name = "perfil";
            using (var dialog = new Form { Text = "Guardar perfil como", Size = new System.Drawing.Size(600, 225), StartPosition = FormStartPosition.CenterParent, Font = Font, MinimizeBox = false, MaximizeBox = false })
            {
                var layout = new TableLayoutPanel { Dock = DockStyle.Fill, ColumnCount = 1, RowCount = 4, Padding = new Padding(16) };
                layout.RowStyles.Add(new RowStyle(SizeType.Absolute, 25)); layout.RowStyles.Add(new RowStyle(SizeType.Absolute, 35));
                layout.RowStyles.Add(new RowStyle(SizeType.Percent, 100)); layout.RowStyles.Add(new RowStyle(SizeType.Absolute, 38));
                layout.Controls.Add(new Label { Text = "Nombre del archivo de perfil", Dock = DockStyle.Fill }, 0, 0);
                var fileName = new TextBox { Text = name + ".json", Dock = DockStyle.Fill }; layout.Controls.Add(fileName, 0, 1);
                layout.Controls.Add(new Label { Text = "Se guarda en la biblioteca local:\r\n" + profilesDirectory, Dock = DockStyle.Fill, AutoEllipsis = true }, 0, 2);
                var buttons = new FlowLayoutPanel { Dock = DockStyle.Fill, FlowDirection = FlowDirection.RightToLeft };
                var save = new ActionButton { Text = "Guardar", Width = 110, Height = 32, DialogResult = DialogResult.OK };
                var back = new ActionButton { Text = "Cancelar", Width = 110, Height = 32, DialogResult = DialogResult.Cancel };
                buttons.Controls.Add(save); buttons.Controls.Add(back); layout.Controls.Add(buttons, 0, 3);
                dialog.Controls.Add(layout); dialog.AcceptButton = save; dialog.CancelButton = back;
                if (ShowStyledDialog(dialog) != DialogResult.OK) return false;
                name = fileName.Text.Trim();
                if (String.IsNullOrEmpty(name) || name.IndexOfAny(Path.GetInvalidFileNameChars()) >= 0 || name.EndsWith(".", StringComparison.Ordinal))
                    throw new ArgumentException("Escribí un nombre de archivo válido, sin carpetas ni puntos al final.");
                if (!name.EndsWith(".json", StringComparison.OrdinalIgnoreCase)) name += ".json";
                string path = Path.Combine(profilesDirectory, name);
                if (File.Exists(path) && MessageBox.Show(this, "Ya existe " + name + " en la biblioteca. ¿Reemplazarlo?", "Guardar perfil", MessageBoxButtons.YesNo, MessageBoxIcon.Question) != DialogResult.Yes) return false;
                profile.Save(path); profilePath = path; savedProfileJson = profile.ToJson(); savedStamp = DateTime.Now;
                RememberActiveProfile(); RefreshSavedProfiles(); Log("Perfil guardado: " + path); return true;
            }
        }
        void RememberActiveProfile()
        {
            if (String.IsNullOrEmpty(profilePath)) return;
            try { File.WriteAllText(activeProfileFile, Path.GetFileName(profilePath), new UTF8Encoding(false)); }
            catch (Exception error) { Log("No se pudo recordar el perfil seleccionado: " + error.Message); }
        }
        static string InitialProfilePath()
        {
            try
            {
                if (File.Exists(activeProfileFile))
                {
                    string name = File.ReadAllText(activeProfileFile).Trim();
                    if (name == Path.GetFileName(name) && name.EndsWith(".json", StringComparison.OrdinalIgnoreCase))
                    { string path = Path.Combine(profilesDirectory, name); if (File.Exists(path)) return path; }
                }
            }
            catch (IOException) { }
            catch (UnauthorizedAccessException) { }
            return Path.Combine(profilesDirectory, "default.json");
        }
        void ImportProfile()
        {
            profileDialogs++;
            try
            {
                using (var dialog = new OpenFileDialog { Filter = "Perfil AFR2 (*.json)|*.json" })
                {
                    if (ShowStyledDialog(dialog) != DialogResult.OK) return;
                    Profile imported = Profile.FromJson(File.ReadAllText(dialog.FileName));
                    if (!CanLeaveProfile()) return;
                    profile = imported; profilePath = null; savedProfileJson = null;
                    FillSettings(); RefreshSavedProfiles();
                    Log("Perfil importado; revisar rutas y Guardar para incorporarlo a la lista de perfiles.");
                }
            }
            finally { profileDialogs--; }
        }
        void SaveRemoteProfile(Profile next)
        {
            // MCP approval also covers saving an imported profile as a new local copy.
            string path = profilePath ?? Path.Combine(profilesDirectory, "perfil-" + Guid.NewGuid().ToString("N") + ".json");
            next.Save(path); profilePath = path; profile = next; savedProfileJson = profile.ToJson();
            FillSettings(); RememberActiveProfile(); RefreshSavedProfiles();
        }
    }
}
