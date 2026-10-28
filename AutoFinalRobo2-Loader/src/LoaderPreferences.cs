using System;
using System.IO;
using System.Text;
using System.Web.Script.Serialization;

namespace AFR2Loader
{
    public sealed class LoaderPreferences
    {
        public int Schema = 1;
        public bool BackupMegaBeforeFlasher = true;
        public bool ShowNoMegaBackupWarning = true;
        public static string FilePath { get { return Path.Combine(Profile.UserRoot, "preferences.json"); } }
        public static LoaderPreferences Read()
        {
            if (!File.Exists(FilePath)) return new LoaderPreferences();
            var value = new JavaScriptSerializer().Deserialize<LoaderPreferences>(File.ReadAllText(FilePath));
            if (value == null || value.Schema != 1) throw new ArgumentException("Preferencias incompatibles; se necesita schema1.");
            return value;
        }
        public void Save()
        {
            Directory.CreateDirectory(Profile.UserRoot);
            string temp = FilePath + ".tmp";
            File.WriteAllText(temp, new JavaScriptSerializer().Serialize(this), new UTF8Encoding(false));
            if (File.Exists(FilePath)) File.Replace(temp, FilePath, FilePath + ".bak"); else File.Move(temp, FilePath);
        }
    }
}
