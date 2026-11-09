// AFR2-20261108-09. Monitor serie: Enviar | Intérprete, fuente USB del ATmega (TAP) e intérprete de tramas.
// The interpreter only reads what the capture worker already received. The source selector sends one
// explicit control line ("#AFR2 TAP n") per user choice, never on open/reopen and never from MCP.
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace AFR2Loader
{
    public sealed partial class MainForm
    {
        static readonly string[] TapSourceNames = {
            "ATmega · log de depuración",
            "RX ATmega · lo que manda el ESP",
            "RX ESP · lo que responde el ATmega",
            "Enlace completo · ambos sentidos",
            "RX ATmega crudo · bytes (resets)"
        };
        // AFR2-20261108-10: "TAP mode=n name baud=b" names the speed the ATmega uses for that mode.
        static readonly Regex TapConfirm = new Regex(@"\bTAP mode=(\d)\b(?: \S+ baud=(\d{3,7}))?");
        static readonly Regex AliveLine = new Regex(@"^\d+ [A-Z]{3,5}\d?\b");
        const int TapBaseBaud = 115200, TapFastBaud = 250000;
        static readonly Regex AvrBootLine = new Regex(@"^\d+ AVR BOOT\b");
        volatile WireCatalog wireCatalog;
        bool interpreterOn, interpreterShown, catalogTried, syncingTapSource;
        int tapActive, tapPending = -1, frameFilter = -1;
        DateTime tapPendingSince, tapAliveAt;
        int tapBaudBefore;
        long framesBad;
        TableLayoutPanel serialMonitorLayout;
        Control serialSendPanel, interpreterPanel, restartsPanel;
        ActionButton toolsToggle, interpreterToggle, restartsButton;
        int toolsView, restartsSeen;   // AFR2-20261108-13: 0 Enviar, 1 Intérprete, 2 Reinicios
        readonly ComboBox tapSource = new ComboBox { Width = 320, DropDownStyle = ComboBoxStyle.DropDownList };
        readonly Label catalogState = new Label { AutoSize = true, MaximumSize = new Size(900, 0), Margin = new Padding(0, 4, 0, 0) };
        readonly Label interpreterState = new Label { AutoSize = true, Margin = new Padding(0, 4, 16, 0) };
        readonly DataGridView framesGrid = new DataGridView(), catalogGrid = new DataGridView(), resetsGrid = new DataGridView();
        readonly TextBox frameDetail = new TextBox { Dock = DockStyle.Fill, Multiline = true, ReadOnly = true, ScrollBars = ScrollBars.Both, WordWrap = false, Font = new Font("Consolas", 9) };
        readonly CheckBox framesFollow = new CheckBox { Text = "Seguir tramas", AutoSize = true, Checked = true };
        readonly List<DecodedFrame> allFrames = new List<DecodedFrame>(), shownFrames = new List<DecodedFrame>();
        readonly Dictionary<byte, int[]> frameCounts = new Dictionary<byte, int[]>();
        readonly Dictionary<byte, DateTime> frameLast = new Dictionary<byte, DateTime>();
        readonly Dictionary<byte, int> catalogRows = new Dictionary<byte, int>();
        readonly WireScanner rawScanner = new WireScanner();
        readonly BootTracker boots = new BootTracker();
        readonly TextBox bootDetail = new TextBox { Dock = DockStyle.Fill, Multiline = true, ReadOnly = true, ScrollBars = ScrollBars.Both, WordWrap = false, Font = new Font("Consolas", 9) };
        int bootsNotified;
        const string BootDetailHint = "Elegí un arranque para ver su detalle: razón, excepción y las líneas del log antes y después. Se registra siempre que el puerto esté abierto, con el intérprete apagado también.";
        const string FrameDetailHint = "Elegí una trama para ver su detalle. Activá el intérprete para decodificar lo que llega.";

        Control CreateSerialToolsPanel(TableLayoutPanel layout)
        {
            serialMonitorLayout = layout;
            var panel = new TableLayoutPanel { Dock = DockStyle.Fill, ColumnCount = 1, RowCount = 3, Margin = new Padding(0) };
            panel.RowStyles.Add(new RowStyle(SizeType.AutoSize)); panel.RowStyles.Add(new RowStyle(SizeType.AutoSize)); panel.RowStyles.Add(new RowStyle(SizeType.Percent, 100));
            var bar = UiRow();
            toolsToggle = new ActionButton { Text = "Ver intérprete ↔", Width = 200, Role = ButtonRole.Secondary };
            toolsToggle.Click += (s, e) => ShowToolsView(toolsView == 0 ? 1 : 0);
            restartsButton = new ActionButton { Text = "Reinicios (0)", Width = 190, Role = ButtonRole.Info };
            restartsButton.Click += (s, e) => ShowToolsView(toolsView == 2 ? 0 : 2);
            interpreterToggle = new ActionButton { Text = "Intérprete: desactivado", Width = 240, Enabled = false };
            interpreterToggle.Click += (s, e) => SetInterpreter(!interpreterOn);
            var load = new ActionButton { Text = "Cargar catálogo…", Width = 190, Role = ButtonRole.Secondary };
            load.Click += (s, e) => ChooseCatalog();
            tapSource.Items.AddRange(TapSourceNames); tapSource.SelectedIndex = 0;
            tapSource.SelectedIndexChanged += async (s, e) => { if (!syncingTapSource) await RequestTapSource(tapSource.SelectedIndex); };
            bar.Controls.Add(toolsToggle); bar.Controls.Add(restartsButton); bar.Controls.Add(interpreterToggle); bar.Controls.Add(load); bar.Controls.Add(UiField("Fuente USB", tapSource));
            panel.Controls.Add(bar, 0, 0);
            var status = UiRow(); status.Controls.Add(interpreterState); status.Controls.Add(catalogState); panel.Controls.Add(status, 0, 1);
            var host = new Panel { Dock = DockStyle.Fill, Margin = new Padding(0) };
            serialSendPanel = CreateSerialSendPanel(); interpreterPanel = CreateInterpreterPanel();
            serialSendPanel.Dock = interpreterPanel.Dock = restartsPanel.Dock = DockStyle.Fill; interpreterPanel.Visible = restartsPanel.Visible = false;
            host.Controls.Add(restartsPanel); host.Controls.Add(interpreterPanel); host.Controls.Add(serialSendPanel);
            panel.Controls.Add(host, 0, 2);
            ShowInterpreter(false); UpdateInterpreterControls();
            timer.Tick += (s, e) => DrainInterpreter();
            return panel;
        }

        Control CreateInterpreterPanel()
        {
            var panel = new TableLayoutPanel { Dock = DockStyle.Fill, ColumnCount = 2, RowCount = 3, Margin = new Padding(0) };
            panel.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 68)); panel.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 32));
            panel.RowStyles.Add(new RowStyle(SizeType.AutoSize)); panel.RowStyles.Add(new RowStyle(SizeType.Percent, 58)); panel.RowStyles.Add(new RowStyle(SizeType.Percent, 42));
            var actions = UiRow(); actions.Controls.Add(framesFollow);
            var clear = new ActionButton { Text = "Limpiar tramas", Width = 165, Role = ButtonRole.Secondary }; clear.Click += (s, e) => ClearFrames();
            var copy = new ActionButton { Text = "Copiar detalle", Width = 165, Role = ButtonRole.Secondary }; copy.Click += (s, e) => CopyText(frameDetail.Text, copy);
            var save = new ActionButton { Text = "Guardar tramas", Width = 170, Role = ButtonRole.Secondary }; save.Click += (s, e) => SaveFrames();
            actions.Controls.Add(clear); actions.Controls.Add(copy); actions.Controls.Add(save);
            panel.Controls.Add(actions, 0, 0); panel.SetColumnSpan(actions, 2);
            SetupInterpreterGrid(framesGrid, new[] { "Hora PC", "ms AVR", "Origen", "CMD", "Nombre", "Qué hace", "Correlación", "Resultado", "B", "Campos", "Estado" },
                new[] { 85, 65, 135, 50, 105, 170, 170, 95, 35, 280, 160 });
            framesGrid.VirtualMode = true; framesGrid.CellValueNeeded += FrameCellValue; framesGrid.CellFormatting += FrameCellFormatting;
            framesGrid.SelectionChanged += (s, e) => ShowSelectedFrame();
            panel.Controls.Add(framesGrid, 0, 1); panel.Controls.Add(frameDetail, 1, 1);
            var lower = new TabControl { Dock = DockStyle.Fill };
            var catalogPage = new TabPage("Catálogo de comandos");
            SetupInterpreterGrid(catalogGrid, new[] { "ID", "Nombre", "Familia", "Qué hace", "Pedido B", "Respuesta B", "Vistos ped/resp", "Último" },
                new[] { 50, 125, 80, 340, 70, 85, 110, 95 });
            catalogGrid.CellClick += (s, e) => { if (e.RowIndex >= 0 && catalogGrid.Rows[e.RowIndex].Tag is byte) ToggleFrameFilter((byte)catalogGrid.Rows[e.RowIndex].Tag); };
            // AFR2-20261108-11: boots of both chips, with the log around each one.
            SetupInterpreterGrid(resetsGrid, new[] { "Nº", "Hora PC", "ms AVR", "Equipo", "Razón", "Sesión nueva (HELLO)", "Antes", "Excepción", "epc1", "Boot ID", "Fuente", "Detalle" },
                new[] { 35, 90, 70, 65, 190, 175, 200, 140, 95, 80, 150, 260 });
            resetsGrid.SelectionChanged += (s, e) => ShowSelectedBoot();
            resetsGrid.CellFormatting += (s, e) => { if (e.RowIndex >= 0 && e.RowIndex < resetsGrid.Rows.Count && resetsGrid.Rows[e.RowIndex].Tag is BootEvent && ((BootEvent)resetsGrid.Rows[e.RowIndex].Tag).Serious) e.CellStyle.ForeColor = Color.Firebrick; };
            var bootLayout = new TableLayoutPanel { Dock = DockStyle.Fill, ColumnCount = 2, RowCount = 2, Margin = new Padding(0) };
            bootLayout.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 55)); bootLayout.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 45));
            bootLayout.RowStyles.Add(new RowStyle(SizeType.Percent, 100)); bootLayout.RowStyles.Add(new RowStyle(SizeType.AutoSize));
            var bootActions = UiRow();
            var bootCopy = new ActionButton { Text = "Copiar detalle", Width = 165, Role = ButtonRole.Secondary }; bootCopy.Click += (s, e) => CopyText(bootDetail.Text, bootCopy);
            var bootClear = new ActionButton { Text = "Limpiar arranques", Width = 190, Role = ButtonRole.Secondary }; bootClear.Click += (s, e) => { boots.Clear(); bootsNotified = 0; FillResets(); bootDetail.Text = BootDetailHint; };
            bootActions.Controls.Add(bootCopy); bootActions.Controls.Add(bootClear);
            bootLayout.Controls.Add(resetsGrid, 0, 0); bootLayout.Controls.Add(bootDetail, 1, 0); bootLayout.Controls.Add(bootActions, 0, 1); bootLayout.SetColumnSpan(bootActions, 2);
            EnableTextCopyFeedback(bootDetail); bootDetail.Text = BootDetailHint;
            catalogPage.Controls.Add(catalogGrid); restartsPanel = CreateRestartsPanel(bootLayout);
            lower.TabPages.Add(catalogPage);
            panel.Controls.Add(lower, 0, 2); panel.SetColumnSpan(lower, 2);
            EnableTextCopyFeedback(frameDetail); frameDetail.Text = FrameDetailHint;
            return panel;
        }

        void SetupInterpreterGrid(DataGridView grid, string[] names, int[] widths)
        {
            grid.Dock = DockStyle.Fill; grid.ReadOnly = true; grid.RowHeadersVisible = false;
            grid.AllowUserToAddRows = grid.AllowUserToDeleteRows = grid.AllowUserToResizeRows = false;
            grid.SelectionMode = DataGridViewSelectionMode.FullRowSelect; grid.MultiSelect = false;
            grid.AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None; grid.RowTemplate.Height = 24;
            for (int i = 0; i < names.Length; i++)
            {
                int index = grid.Columns.Add("c" + i, names[i]);
                grid.Columns[index].Width = widths[i]; grid.Columns[index].SortMode = DataGridViewColumnSortMode.NotSortable;
            }
        }

        void ShowInterpreter(bool show) { ShowToolsView(show ? 1 : 0); }
        void ShowToolsView(int view)
        {
            toolsView = view; interpreterShown = view == 1;
            serialSendPanel.Visible = view == 0; interpreterPanel.Visible = view == 1; restartsPanel.Visible = view == 2;
            toolsToggle.Text = view == 0 ? "Ver intérprete ↔" : "Ver envío ↔";
            if (view == 2) { foreach (BootEvent e in boots.Events) if (e.Number > restartsSeen) restartsSeen = e.Number; }
            // Room for the extra bar; the page scrolls when the window is shorter than the minimum.
            serialMonitorLayout.RowStyles[7].Height = view == 0 ? 300 : 540;
            serialMonitorLayout.MinimumSize = new Size(0, view == 0 ? 810 : 1050);
            UpdateInterpreterControls();
        }
        // AFR2-20261108-13: restarts of both chips get their own view, opened from the monitor bar.
        Control CreateRestartsPanel(Control table)
        {
            var panel = new TableLayoutPanel { Dock = DockStyle.Fill, ColumnCount = 1, RowCount = 2, Margin = new Padding(0) };
            panel.RowStyles.Add(new RowStyle(SizeType.AutoSize)); panel.RowStyles.Add(new RowStyle(SizeType.Percent, 100));
            panel.Controls.Add(new Label { AutoSize = true, MaximumSize = new Size(1100, 0), Margin = new Padding(0, 0, 0, 6),
                Text = "Reinicios del ESP y del ATmega: razón del reset, la sesión nueva (HELLO) que lo siguió, si antes se cortó el enlace, la excepción y la línea de tiempo. Se registran siempre que el puerto esté abierto. Elegí una fila para ver el detalle con el log de antes y después." }, 0, 0);
            panel.Controls.Add(table, 0, 1);
            return panel;
        }

        void SetInterpreter(bool on)
        {
            if (on && wireCatalog == null) return;
            interpreterOn = on; rawScanner.Reset();
            if (on && !interpreterShown) ShowInterpreter(true);
            UpdateInterpreterControls();
        }

        void LoadCatalogFromProfile()
        {
            string path = "";
            try { path = profile == null ? "" : profile.Resolve("WireCatalog"); LoadCatalog(path, false); }
            catch (Exception error) { ShowCatalogProblem(error.Message); }
        }
        void LoadCatalog(string path, bool announce)
        {
            try
            {
                WireCatalog loaded = WireCatalog.Load(path);
                wireCatalog = loaded;
                catalogState.Text = "Catálogo: " + loaded.Ordered.Count + " comandos · " + (loaded.UpdatedBy.Length > 0 ? loaded.UpdatedBy + " · " : "") + loaded.FilePath;
                catalogState.ForeColor = LoaderAppearance.Muted;
                FillCatalogGrid();
                if (announce) QueueNotice(new LoaderNotice { Origin = "intérprete", Code = "configured", Message = "Catálogo cargado: " + loaded.Ordered.Count + " comandos. Para que quede fijo, configurá WireCatalog en el perfil.", Detail = loaded.FilePath });
            }
            catch (Exception error)
            {
                ShowCatalogProblem(error.Message);
                if (announce) QueueNotice(new LoaderNotice { Origin = "intérprete", Code = "cancelled", Message = "No se cargó el catálogo; se conserva el anterior si había uno.", Detail = error.Message });
            }
            UpdateInterpreterControls();
        }
        void ShowCatalogProblem(string message)
        {
            string first = (message ?? "").Split('\n')[0].Trim();
            catalogState.Text = (wireCatalog == null ? "Sin catálogo: el intérprete se habilita al cargar wire_catalog.json. " : "Se mantiene el catálogo anterior. ") + first;
            catalogState.ForeColor = LoaderAppearance.WarningInk;
        }
        void ChooseCatalog()
        {
            using (var dialog = new OpenFileDialog { Title = "Elegir wire_catalog.json", Filter = "Catálogo de tramas (*.json)|*.json|Todos los archivos|*.*", CheckFileExists = true })
            {
                try { string current = profile == null ? "" : profile.Resolve("WireCatalog"); if (current.Length > 0 && Directory.Exists(Path.GetDirectoryName(current))) dialog.InitialDirectory = Path.GetDirectoryName(current); }
                catch (Exception) { }
                if (dialog.ShowDialog(this) == DialogResult.OK) LoadCatalog(dialog.FileName, true);
            }
        }

        void FillCatalogGrid()
        {
            catalogGrid.Rows.Clear(); catalogRows.Clear();
            WireCatalog catalog = wireCatalog; if (catalog == null) return;
            foreach (WireCommand c in catalog.Ordered)
            {
                int row = catalogGrid.Rows.Add(c.IdText, c.Name, c.Family + (c.Sensitive ? " · privado" : ""), c.Does, c.Request.Describe(), c.Reply.Describe(), "", "");
                catalogGrid.Rows[row].Tag = c.Id; catalogRows[c.Id] = row;
                if (c.Retired) catalogGrid.Rows[row].DefaultCellStyle.ForeColor = LoaderAppearance.Muted;
            }
            UpdateCatalogCounts(new HashSet<byte>(frameCounts.Keys));
        }
        void UpdateCatalogCounts(IEnumerable<byte> ids)
        {
            foreach (byte id in ids)
            {
                int row; int[] count; DateTime last;
                if (!catalogRows.TryGetValue(id, out row) || row >= catalogGrid.Rows.Count) continue;
                bool seen = frameCounts.TryGetValue(id, out count);
                catalogGrid.Rows[row].Cells[6].Value = seen ? count[0] + " / " + count[1] : "";
                catalogGrid.Rows[row].Cells[7].Value = frameLast.TryGetValue(id, out last) ? last.ToString("HH:mm:ss", CultureInfo.InvariantCulture) : "";
            }
        }
        void ToggleFrameFilter(byte id)
        {
            frameFilter = frameFilter == id ? -1 : id;
            RebuildShownFrames(); UpdateInterpreterControls();
        }

        void FrameCellValue(object sender, DataGridViewCellValueEventArgs e)
        {
            if (e.RowIndex < 0 || e.RowIndex >= shownFrames.Count) return;
            DecodedFrame f = shownFrames[e.RowIndex];
            switch (e.ColumnIndex)
            {
                case 0: e.Value = f.Time; break;
                case 1: e.Value = f.AvrTime; break;
                case 2: e.Value = f.Origin; break;
                case 3: e.Value = f.CommandText; break;
                case 4: e.Value = f.Name; break;
                case 5: e.Value = f.Does; break;
                case 6: e.Value = f.Correlation; break;
                case 7: e.Value = f.Result; break;
                case 8: e.Value = f.Length; break;
                case 9: e.Value = f.Fields; break;
                default: e.Value = f.Status; break;
            }
        }
        void FrameCellFormatting(object sender, DataGridViewCellFormattingEventArgs e)
        {
            if (e.RowIndex < 0 || e.RowIndex >= shownFrames.Count) return;
            DecodedFrame f = shownFrames[e.RowIndex];
            if (f.Ok || f.Origin == "Loader TX") return;
            bool soft = f.Status.StartsWith("rechazado", StringComparison.Ordinal) || f.Status.StartsWith("redactado", StringComparison.Ordinal);
            e.CellStyle.ForeColor = soft ? LoaderAppearance.WarningInk : Color.Firebrick;
        }
        void ShowSelectedFrame()
        {
            if (framesGrid.SelectedRows.Count == 0) return;
            int index = framesGrid.SelectedRows[0].Index;
            if (index < 0 || index >= shownFrames.Count) return;
            DecodedFrame f = shownFrames[index];
            frameDetail.Text = f.Detail();
            int row;
            if (f.HasCommand && catalogRows.TryGetValue(f.Command, out row) && row < catalogGrid.Rows.Count)
            { catalogGrid.ClearSelection(); catalogGrid.Rows[row].Selected = true; }
        }

        bool AddFrame(DecodedFrame f, HashSet<byte> changed)
        {
            allFrames.Add(f);
            if (f.HasCommand)
            {
                int[] count;
                if (!frameCounts.TryGetValue(f.Command, out count)) { count = new int[2]; frameCounts[f.Command] = count; }
                count[f.Request ? 0 : 1]++; frameLast[f.Command] = f.At;
                if (changed != null) changed.Add(f.Command);
            }
            if (!f.Ok && f.Origin != "Loader TX") framesBad++;
            if (frameFilter < 0 || (f.HasCommand && f.Command == frameFilter)) shownFrames.Add(f);
            if (allFrames.Count <= 6000) return false;
            allFrames.RemoveRange(0, 1000); return true;
        }
        void RebuildShownFrames()
        {
            shownFrames.Clear();
            shownFrames.AddRange(frameFilter < 0 ? allFrames : allFrames.Where(f => f.HasCommand && f.Command == frameFilter));
            framesGrid.RowCount = shownFrames.Count; framesGrid.Invalidate(); FollowFrames();
        }
        void FollowFrames()
        {
            if (!framesFollow.Checked || shownFrames.Count == 0 || !framesGrid.Visible) return;
            try { framesGrid.FirstDisplayedScrollingRowIndex = shownFrames.Count - 1; } catch (Exception) { }
        }
        void ClearFrames()
        {
            allFrames.Clear(); shownFrames.Clear(); framesGrid.RowCount = 0; framesBad = 0;
            var seen = new HashSet<byte>(frameCounts.Keys); frameCounts.Clear(); frameLast.Clear();
            UpdateCatalogCounts(seen); rawScanner.Reset(); frameDetail.Text = FrameDetailHint;
        }
        void FillResets()
        {
            int keep = resetsGrid.SelectedRows.Count > 0 && resetsGrid.SelectedRows[0].Tag is BootEvent ? ((BootEvent)resetsGrid.SelectedRows[0].Tag).Number : -1;
            resetsGrid.Rows.Clear();
            foreach (BootEvent r in boots.Events)
            {
                int row = resetsGrid.Rows.Add(r.Number, r.Time, r.AvrMs, r.Device, r.Reason, r.Session, r.LinkBefore, r.Exception, r.Epc1, r.BootId, r.Source, r.Detail);
                resetsGrid.Rows[row].Tag = r;
            }
            resetsGrid.ClearSelection();
            foreach (DataGridViewRow row in resetsGrid.Rows) if (((BootEvent)row.Tag).Number == keep) row.Selected = true;
            if (keep < 0 && resetsGrid.Rows.Count > 0) try { resetsGrid.FirstDisplayedScrollingRowIndex = resetsGrid.Rows.Count - 1; } catch (Exception) { }
            ShowSelectedBoot();
        }
        void ShowSelectedBoot()
        {
            if (resetsGrid.SelectedRows.Count == 0 || !(resetsGrid.SelectedRows[0].Tag is BootEvent)) return;
            bootDetail.Text = ((BootEvent)resetsGrid.SelectedRows[0].Tag).Report();
        }
        // One notice per new boot, except the expected auto-reset of the Mega when the port opens.
        void NotifyBoots()
        {
            foreach (BootEvent e in boots.Events)
            {
                if (e.Number <= bootsNotified) continue;
                bootsNotified = e.Number;
                if (e.Device == "ATmega" && !e.Serious && e.Detail.IndexOf("auto-reset", StringComparison.Ordinal) >= 0) continue;
                QueueNotice(new LoaderNotice { Origin = "arranques", Code = e.Serious ? "cancelled" : "configured",
                    Message = "Arranque " + e.Device + " Nº " + e.Number + ": " + (e.Exception.Length > 0 ? "excepción " + e.Exception : e.Reason) + ". Detalle en Monitor serie > Reinicios.",
                    Detail = e.Report() });
            }
        }

        List<DecodedFrame> DecodeRecord(TapRecord record)
        {
            var output = new List<DecodedFrame>();
            WireCatalog catalog = wireCatalog;
            if (record.Type != TapRecord.RawFromEsp || record.Corrupt) { output.Add(WireDecoder.FromRecord(catalog, record)); return output; }
            if (record.Truncated) rawScanner.Reset();
            foreach (byte b in record.Data)
            {
                WireScanner.Found found = rawScanner.Push(b, record.Redacted);
                if (found == null) continue;
                var frame = new DecodedFrame { At = record.At, AvrMs = record.AvrMs, HasAvrMs = true, Origin = record.Origin };
                WireDecoder.Decode(catalog, frame, found.Route, found.Command, found.Payload);
                if (!found.ChecksumOk)
                {
                    frame.Ok = false;
                    frame.Status = found.Redacted ? "redactado en el ATmega (payload en 00)" : "checksum XOR incorrecto";
                    frame.Fields = "";
                }
                output.Add(frame);
            }
            return output;
        }

        void DrainInterpreter()
        {
            if (!catalogTried) { catalogTried = true; LoadCatalogFromProfile(); }
            SerialCapture capture = serialCapture;
            if (capture != null)
            {
                bool resetsChanged = false, rebuild = false; int added = 0;
                var changed = new HashSet<byte>();
                SerialLine line; int budget = 600;
                while (budget-- > 0 && capture.TakeLine(out line))
                {
                    if (AliveLine.IsMatch(line.Text)) tapAliveAt = DateTime.Now;
                    HandleTapLine(line);
                    if (boots.PushLine(wireCatalog, line) != null) resetsChanged = true;   /* AFR2-20261108-11: always */
                }
                TapRecord record; budget = 800;
                while (budget-- > 0 && capture.TakeRecord(out record))
                {
                    if (!record.Corrupt) tapAliveAt = DateTime.Now;
                    if (!interpreterOn) continue;
                    foreach (DecodedFrame frame in DecodeRecord(record))
                    {
                        if (AddFrame(frame, changed)) rebuild = true;
                        added++;
                        if (boots.PushFrame(wireCatalog, frame) != null) resetsChanged = true;
                    }
                }
                if (rebuild) RebuildShownFrames();
                else if (added > 0 && framesGrid.RowCount != shownFrames.Count) { framesGrid.RowCount = shownFrames.Count; FollowFrames(); }
                if (changed.Count > 0) UpdateCatalogCounts(changed);
                if (resetsChanged) { FillResets(); NotifyBoots(); }
            }
            if (tapPending >= 0 && (DateTime.Now - tapPendingSince).TotalMilliseconds > 1500)
            {
                QueueNotice(new LoaderNotice { Origin = "fuente USB", Code = "cancelled",
                    Message = "El ATmega no confirmó «" + TapSourceNames[tapPending] + "» en 1,5 s. Se mantiene «" + TapSourceNames[tapActive] + "».",
                    Detail = "Hace falta el firmware ATmega con AFR2_DEBUG_TAP=1 y abrir el monitor a 115200 (el log y los modos de un sentido usan 115200; enlace completo pasa solo a 250000). El Loader no reintenta solo." });
                tapPending = -1; SyncTapSource(tapActive);
            }
            // AFR2-20261108-10: at 250000 an ATmega reset (back to 115200) only shows up as silence or noise.
            if (capture != null && capture.State == "open" && tapPending < 0 && capture.Baud == TapFastBaud && tapActive == 3 && (DateTime.Now - tapAliveAt).TotalSeconds > 7)
            {
                tapActive = 0; SyncTapSource(0); SwitchPortBaud(capture, tapBaudBefore > 0 ? tapBaudBefore : TapBaseBaud);
                QueueNotice(new LoaderNotice { Origin = "fuente USB", Code = "cancelled", Message = "Sin datos válidos del ATmega en 7 s a 250000: probablemente se reinició. El puerto vuelve a 115200 y la fuente al log." });
            }
            UpdateInterpreterControls();
        }

        void HandleTapLine(SerialLine line)
        {
            Match confirm = TapConfirm.Match(line.Text);
            if (confirm.Success)
            {
                int mode = confirm.Groups[1].Value[0] - '0';
                bool requested = tapPending >= 0;
                tapPending = -1;
                if (mode < 0 || mode >= TapSourceNames.Length) return;
                tapActive = mode; SyncTapSource(mode); tapAliveAt = DateTime.Now;
                if (requested) ClearForSourceChange();
                int wanted;
                SerialCapture capture = serialCapture;
                if (confirm.Groups[2].Success && Int32.TryParse(confirm.Groups[2].Value, NumberStyles.None, CultureInfo.InvariantCulture, out wanted) && capture != null && capture.Baud != wanted)
                {
                    if (wanted == TapFastBaud) tapBaudBefore = capture.Baud;
                    SwitchPortBaud(capture, wanted);
                }
                return;
            }
            if (line.Text.IndexOf(" TAP error", StringComparison.Ordinal) >= 0 && tapPending >= 0)
            {
                QueueNotice(new LoaderNotice { Origin = "fuente USB", Code = "cancelled", Message = "El ATmega rechazó el cambio de fuente.", Detail = line.Text });
                tapPending = -1; SyncTapSource(tapActive); return;
            }
            if (AvrBootLine.IsMatch(line.Text))
            {
                tapPending = -1;
                if (tapActive != 0)
                {
                    tapActive = 0; SyncTapSource(0);
                    QueueNotice(new LoaderNotice { Origin = "fuente USB", Code = "cancelled", Message = "El ATmega se reinició y volvió al log. Elegí la fuente otra vez si la necesitás.", Detail = line.Text });
                }
            }
        }

        async Task RequestTapSource(int mode)
        {
            if (mode == tapActive && tapPending < 0) return;
            SerialCapture capture = serialCapture;
            if (capture == null || capture.State != "open" || busy || pendingJob != null || serialTransition)
            {
                SyncTapSource(tapActive);
                QueueNotice(new LoaderNotice { Origin = "fuente USB", Code = "configured", Message = "Abrí el puerto del monitor para elegir la fuente USB. El cambio se le pide al ATmega (firmware con AFR2_DEBUG_TAP)." });
                return;
            }
            byte[] command = Encoding.ASCII.GetBytes("#AFR2 TAP " + mode.ToString(CultureInfo.InvariantCulture) + "\r\n");
            tapPending = mode; tapPendingSince = DateTime.Now; UpdateInterpreterControls();
            try { await capture.SendAsync(command, CancellationToken.None); NoteLoaderTx(command, "Control de fuente TAP " + mode); }
            catch (Exception error) { tapPending = -1; SyncTapSource(tapActive); QueueFailure("fuente USB", error); }
        }
        // Same COM, new speed: the worker applies it between reads; the baud box follows.
        async void SwitchPortBaud(SerialCapture capture, int baud)
        {
            try
            {
                await capture.ChangeBaudAsync(baud);
                if (capture == serialCapture) { serialBaud.Text = baud.ToString(CultureInfo.InvariantCulture); UpdateSerialControls(); }
            }
            catch (OperationCanceledException) { }
            catch (Exception error) { QueueFailure("fuente USB", error); }
        }
        void SyncTapSource(int mode)
        {
            syncingTapSource = true;
            try { if (mode >= 0 && mode < tapSource.Items.Count) tapSource.SelectedIndex = mode; }
            finally { syncingTapSource = false; }
        }
        void ClearForSourceChange()
        {
            ClearSerialView(); ClearFrames();
            var header = new SerialDisplayChunk { Header = true, Text = "\r\n--- " + DateTime.Now.ToString("HH:mm:ss") + " | fuente: " + TapSourceNames[tapActive] + " ---\r\n" };
            RememberSerialChunk(header); serialText.AppendBatch(FormatSerialChunk(header));
        }
        // Called for every new capture: the ATmega restarts in log mode (also by auto-reset on open).
        void PrepareInterpreterCapture(SerialCapture capture)
        {
            capture.RecordFormatter = FormatTapSummary;
            tapActive = 0; tapPending = -1; SyncTapSource(0); rawScanner.Reset(); boots.CaptureOpenedAt = DateTime.Now;
        }
        void NoteLoaderTx(byte[] bytes, string what)
        {
            if (!interpreterOn || bytes == null) return;
            var frame = new DecodedFrame { At = DateTime.Now, Origin = "Loader TX", Name = what, Status = "enviado; no acredita recepción", Hex = WireDecoder.Hex(bytes, 0, bytes.Length), Length = bytes.Length, Ok = true };
            if (AddFrame(frame, null)) RebuildShownFrames(); else { framesGrid.RowCount = shownFrames.Count; FollowFrames(); }
        }

        void UpdateInterpreterControls()
        {
            if (interpreterToggle == null) return;
            if (restartsButton != null)
            {
                int unseen = 0; foreach (BootEvent e in boots.Events) if (e.Number > restartsSeen) unseen++;
                if (toolsView == 2) { foreach (BootEvent e in boots.Events) if (e.Number > restartsSeen) restartsSeen = e.Number; unseen = 0; }
                SetTextIfChanged(restartsButton, "Reinicios (" + boots.Events.Count + ")" + (unseen > 0 ? " · " + unseen + " nuevo" + (unseen > 1 ? "s" : "") : ""));
                SetTextIfChanged(restartsButton, toolsView == 2 ? "Ocultar reinicios" : restartsButton.Text);
            }
            bool open = serialCapture != null && serialCapture.State == "open";
            interpreterToggle.Enabled = wireCatalog != null;
            SetTextIfChanged(interpreterToggle, interpreterOn ? "Intérprete: activado" : "Intérprete: desactivado");
            tapSource.Enabled = open && tapPending < 0 && !busy && pendingJob == null && !serialTransition;
            string state;
            if (wireCatalog == null) state = "Intérprete no disponible";
            else if (!interpreterOn) state = "Intérprete apagado";
            else state = allFrames.Count + " tramas · " + framesBad + " con aviso" + (frameFilter >= 0 ? " · filtro 0x" + frameFilter.ToString("X2") + " (clic otra vez para quitar)" : "");
            if (tapPending >= 0) state += " · esperando confirmación de fuente…";
            if (serialCapture != null && (serialCapture.DroppedRecords > 0 || serialCapture.DroppedLines > 0)) state += " · descartados " + serialCapture.DroppedRecords + " registros";
            SetTextIfChanged(interpreterState, state);
        }
        static void SetTextIfChanged(Control control, string text) { if (control.Text != text) control.Text = text; }

        // Runs on the capture worker: only immutable catalog data and static helpers.
        string FormatTapSummary(TapRecord r)
        {
            WireCatalog catalog = wireCatalog;
            var text = new StringBuilder();
            text.Append(r.At.ToString("HH:mm:ss.fff", CultureInfo.InvariantCulture)).Append(" [").Append(r.AvrMs).Append(" ms] ").Append(r.Origin).Append(" | ");
            if (r.Corrupt) return text.Append("registro TAP corrupto\r\n").ToString();
            if (r.Type == TapRecord.RawFromEsp)
            {
                int n = Math.Min(24, r.Data.Length);
                text.Append(r.Data.Length).Append(" B").Append(r.Redacted ? " (redactado)" : "").Append(": ").Append(WireDecoder.Hex(r.Data, 0, n).Replace("\r\n", " ")).Append(n < r.Data.Length ? " ..." : "");
                return text.Append("\r\n").ToString();
            }
            if (r.Data.Length < 2) return text.Append("registro incompleto\r\n").ToString();
            WireCommand c = catalog == null ? null : catalog.Find(r.Data[1]);
            text.Append("0x").Append(r.Data[1].ToString("X2")).Append(' ').Append(c == null ? "?" : c.Name);
            text.Append(r.Redacted ? " | redactado" : " | " + (r.Data.Length - 2) + " B");
            if (!r.Redacted && r.Type == TapRecord.FrameToEsp && r.Data.Length >= 11 && r.Data[1] != 0x52 && r.Data[10] != 0)
                text.Append(" | ").Append((catalog == null ? null : catalog.EnumName("result", r.Data[10])) ?? "resultado " + r.Data[10]);
            return text.Append("\r\n").ToString();
        }

        void SaveFrames()
        {
            try
            {
                string dir = Path.Combine(Profile.UserRoot, "logs"); Directory.CreateDirectory(dir);
                string file = Path.Combine(dir, "frames-" + DateTime.Now.ToString("yyyyMMdd-HHmmss-fff", CultureInfo.InvariantCulture) + ".tsv");
                var text = new StringBuilder("hora_pc\tms_avr\torigen\tcmd\tnombre\tcorrelacion\tresultado\tbytes\tcampos\testado\thex\r\n");
                foreach (DecodedFrame f in allFrames)
                    text.Append(String.Join("\t", new[] { f.Time, f.AvrTime, f.Origin, f.CommandText, f.Name, f.Correlation, f.Result, f.Length.ToString(CultureInfo.InvariantCulture), f.Fields, f.Status, f.Hex.Replace("\r\n", " ") })).Append("\r\n");
                File.WriteAllText(file, text.ToString(), new UTF8Encoding(false));
                QueueNotice(new LoaderNotice { Origin = "intérprete", Code = "configured", Message = "Tramas guardadas: " + allFrames.Count + ".", Detail = file });
            }
            catch (Exception error) { QueueFailure("intérprete", error); }
        }
    }
}
