// AFR2-20261108-09. Reads AutoFinalRobo2-Contracts/contracts/wire_catalog.json (schema afr2-wire-catalog-1).
// Immutable after Load, so the serial worker may format summaries with it.
using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Text.RegularExpressions;
using System.Web.Script.Serialization;

namespace AFR2Loader
{
    public sealed class WireField
    {
        public string Name = "", Type = "", Unit = "", Format = "", Enum = "";
        public bool Optional;
        // Fixed size in bytes; -1 for "rest" types that consume what is left.
        public int Size
        {
            get
            {
                switch (Type)
                {
                    case "u8": case "i8": return 1;
                    case "u16le": case "i16le": return 2;
                    case "u32le": case "i32le": return 4;
                }
                int colon = Type.IndexOf(':');
                if (colon < 0) return 0;
                string tail = Type.Substring(colon + 1);
                return tail == "rest" ? -1 : Int32.Parse(tail, CultureInfo.InvariantCulture);
            }
        }
    }

    public sealed class WireSpec
    {
        public bool Absent, Variable;
        public int[] Sizes = new int[0];
        public int Min = -1, Max = -1;
        public readonly List<WireField> Fields = new List<WireField>();
        public bool Accepts(int length)
        {
            if (Absent) return false;
            if (Variable) return (Min < 0 || length >= Min) && (Max < 0 || length <= Max);
            return Sizes.Contains(length);
        }
        public string Describe()
        {
            if (Absent) return "-";
            if (Variable) return Min >= 0 && Max >= 0 ? Min + ".." + Max : "variable";
            return String.Join(" o ", Sizes.Select(s => s.ToString(CultureInfo.InvariantCulture)).ToArray());
        }
    }

    public sealed class WireCommand
    {
        public byte Id;
        public string Name = "", Family = "", Ws = "", Does = "";
        public bool Sensitive, Retired, NoPrefix;
        public WireSpec Request = new WireSpec(), Reply = new WireSpec();
        public string IdText { get { return "0x" + Id.ToString("X2"); } }
    }

    public sealed class WireCatalog
    {
        public const string Schema = "afr2-wire-catalog-1";
        static readonly Regex FieldType = new Regex(@"^(u8|i8|u16le|i16le|u32le|i32le|ascii:([1-9][0-9]?|rest)|bytes:([1-9][0-9]?|rest))$");
        public string FilePath = "", UpdatedBy = "";
        public readonly List<WireCommand> Ordered = new List<WireCommand>();
        public readonly Dictionary<byte, WireCommand> Commands = new Dictionary<byte, WireCommand>();
        public readonly Dictionary<string, Dictionary<string, string>> Enums = new Dictionary<string, Dictionary<string, string>>(StringComparer.Ordinal);

        public WireCommand Find(byte id) { WireCommand c; return Commands.TryGetValue(id, out c) ? c : null; }
        public string EnumName(string table, long value)
        {
            Dictionary<string, string> names;
            string name;
            if (String.IsNullOrEmpty(table) || !Enums.TryGetValue(table, out names)) return null;
            return names.TryGetValue(value.ToString(CultureInfo.InvariantCulture), out name) ? name : null;
        }

        // Throws ArgumentException listing every problem; never returns a half-valid catalog.
        public static WireCatalog Load(string path)
        {
            if (String.IsNullOrWhiteSpace(path)) throw new ArgumentException("No hay ruta de catálogo. Configurá WireCatalog en el perfil o elegí el archivo.");
            var info = new FileInfo(path);
            if (!info.Exists) throw new ArgumentException("No existe el catálogo: " + path);
            if (info.Length > 1048576) throw new ArgumentException("El catálogo supera 1 MB; no parece wire_catalog.json.");
            object parsed;
            try { parsed = new JavaScriptSerializer { MaxJsonLength = 4194304 }.DeserializeObject(File.ReadAllText(path)); }
            catch (Exception error) { throw new ArgumentException("El catálogo no es JSON válido: " + error.Message); }
            var errors = new List<string>();
            var catalog = new WireCatalog { FilePath = System.IO.Path.GetFullPath(path) };
            var root = parsed as Dictionary<string, object>;
            if (root == null) throw new ArgumentException("El catálogo debe ser un objeto JSON.");
            if (Text(root, "schema") != Schema) errors.Add("schema debe ser " + Schema + ".");
            catalog.UpdatedBy = Text(root, "updated_by");
            var enums = Get(root, "enums") as Dictionary<string, object>;
            if (enums != null)
                foreach (var table in enums)
                {
                    var values = table.Value as Dictionary<string, object>;
                    if (values == null) { errors.Add("enums." + table.Key + " debe ser un objeto."); continue; }
                    catalog.Enums[table.Key] = values.ToDictionary(kv => kv.Key, kv => Convert.ToString(kv.Value, CultureInfo.InvariantCulture), StringComparer.Ordinal);
                }
            var commands = Get(root, "commands") as object[];
            if (commands == null || commands.Length == 0) errors.Add("commands debe ser una lista con comandos.");
            else
                for (int index = 0; index < commands.Length; index++)
                {
                    var item = commands[index] as Dictionary<string, object>;
                    string where = "commands[" + index + "]";
                    if (item == null) { errors.Add(where + " debe ser un objeto."); continue; }
                    var command = new WireCommand();
                    string id = Text(item, "id");
                    if (!Regex.IsMatch(id, "^0x[0-9a-fA-F]{2}$")) { errors.Add(where + ": id debe ser 0xHH (" + id + ")."); continue; }
                    command.Id = Convert.ToByte(id.Substring(2), 16); where += " " + id;
                    if (catalog.Commands.ContainsKey(command.Id)) { errors.Add(where + ": id repetido."); continue; }
                    command.Name = Text(item, "name"); command.Family = Text(item, "family"); command.Ws = Text(item, "ws"); command.Does = Text(item, "does");
                    if (command.Name.Length == 0) errors.Add(where + ": falta name.");
                    if (command.Does.Length == 0) errors.Add(where + ": falta does (qué hace).");
                    command.Sensitive = Flag(item, "sensitive"); command.Retired = Flag(item, "retired"); command.NoPrefix = Flag(item, "no_prefix");
                    command.Request = Spec(Get(item, "request"), where + ".request", command.Retired, catalog, errors);
                    command.Reply = Spec(Get(item, "reply"), where + ".reply", command.Retired, catalog, errors);
                    catalog.Commands[command.Id] = command; catalog.Ordered.Add(command);
                }
            if (errors.Count > 0) throw new ArgumentException("Catálogo inválido (" + errors.Count + " errores):\r\n- " + String.Join("\r\n- ", errors.Take(30).ToArray()));
            return catalog;
        }

        static WireSpec Spec(object value, string where, bool retired, WireCatalog catalog, List<string> errors)
        {
            var spec = new WireSpec();
            var map = value as Dictionary<string, object>;
            if (map == null) { errors.Add(where + " debe ser un objeto."); spec.Absent = true; return spec; }
            object bytes = Get(map, "bytes");
            if (bytes == null) { spec.Absent = true; if (!retired) errors.Add(where + ": bytes null solo en comandos retirados."); }
            else if (bytes is string)
            {
                if ((string)bytes != "variable") errors.Add(where + ": bytes texto solo admite \"variable\".");
                spec.Variable = true; spec.Min = Number(Get(map, "min"), -1); spec.Max = Number(Get(map, "max"), -1);
            }
            else if (bytes is object[]) spec.Sizes = ((object[])bytes).Select(b => Number(b, -1)).ToArray();
            else spec.Sizes = new[] { Number(bytes, -1) };
            if (spec.Sizes.Any(s => s < 0 || s > 64) || spec.Max > 64) errors.Add(where + ": cada largo debe estar entre 0 y 64.");
            var fields = Get(map, "fields") as object[];
            if (fields != null)
            {
                foreach (object entry in fields)
                {
                    var f = entry as Dictionary<string, object>;
                    if (f == null) { errors.Add(where + ": cada campo debe ser un objeto."); continue; }
                    var field = new WireField { Name = Text(f, "name"), Type = Text(f, "type"), Unit = Text(f, "unit"), Format = Text(f, "format"), Enum = Text(f, "enum"), Optional = Flag(f, "optional") };
                    if (field.Name.Length == 0 || !FieldType.IsMatch(field.Type)) { errors.Add(where + ": campo inválido " + field.Name + ":" + field.Type + "."); continue; }
                    if (field.Enum.Length > 0 && !catalog.Enums.ContainsKey(field.Enum)) errors.Add(where + ": enum desconocido " + field.Enum + ".");
                    spec.Fields.Add(field);
                }
                bool rest = spec.Fields.Any(f => f.Size < 0);
                if (!spec.Variable && !rest && spec.Fields.Count > 0)
                {
                    int required = spec.Fields.Where(f => !f.Optional).Sum(f => f.Size), all = spec.Fields.Sum(f => f.Size);
                    if (!spec.Sizes.Contains(required) || !spec.Sizes.Contains(all))
                        errors.Add(where + ": los campos suman " + (required == all ? all.ToString(CultureInfo.InvariantCulture) : required + "/" + all) + " bytes y bytes dice " + spec.Describe() + ".");
                }
            }
            return spec;
        }
        static object Get(Dictionary<string, object> map, string key) { object v; return map.TryGetValue(key, out v) ? v : null; }
        static string Text(Dictionary<string, object> map, string key) { object v = Get(map, key); return v == null ? "" : Convert.ToString(v, CultureInfo.InvariantCulture); }
        static bool Flag(Dictionary<string, object> map, string key) { object v = Get(map, key); return v is bool && (bool)v; }
        static int Number(object value, int fallback)
        {
            if (value == null) return fallback;
            try { return Convert.ToInt32(value, CultureInfo.InvariantCulture); } catch (Exception) { return -1; }
        }
    }
}
