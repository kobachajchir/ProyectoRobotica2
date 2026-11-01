using System;
using System.Drawing;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace AFR2Loader
{
    public sealed partial class MainForm
    {
        readonly ComboBox bridgeTransport = new ComboBox { Width = 440, DropDownStyle = ComboBoxStyle.DropDownList };
        readonly ComboBox operationTransport = new ComboBox { Width = 410, DropDownStyle = ComboBoxStyle.DropDownList };
        readonly ComboBox bridgeSpeed = new ComboBox { Width = 105, DropDownStyle = ComboBoxStyle.DropDownList };
        readonly Label bridgeServiceState = new Label { Dock = DockStyle.Fill, AutoEllipsis = true };
        TabPage bridgeTab;
        bool syncingBridge;
        static readonly object[] TransportLabels = { "Puente serial del Mega — D0/D1, RESET a GND", "Flasher en ATmega — D18/D19, RESET libre", "Adaptador USB–TTL externo" };
        Control CreateEspMethodSelector()
        {
            var row = UiRow(); row.WrapContents = true;
            operationTransport.Items.AddRange(TransportLabels); row.Controls.Add(UiField("Método de flasheo ESP", operationTransport)); idleControls.Add(operationTransport);
            operationTransport.SelectionChangeCommitted += (s, e) => ChooseEspTransport(operationTransport.SelectedIndex);
            AddButton(row, "Configurar flasher...", () => { if (bridgeTab != null) mainTabs.SelectedTab = bridgeTab; }, 180);
            return row;
        }
        async void ChooseEspTransport(int selected)
        {
            if (syncingBridge || busy || pendingJob != null || serialTransition || profileDialogs > 0) { SyncBridgeOptions(); return; }
            profileDialogs++;
            try
            {
                ReadSettings(); string next = selected == 1 ? "mega_bridge" : selected == 2 ? "usb-ttl" : "mega_reset";
                await StopSerialMonitor("Cambio manual de método ESP; liberar COM antes de recablear");
                string detail = selected == 1
                    ? "MÉTODO: flasher ejecutándose en ATmega.\r\n\r\nQUITAR RESET Mega→GND: RESET queda LIBRE.\r\nD18/TX1→E_RX central (RX ESP).\r\nE_TX central (TX ESP)→D19/RX1.\r\nRetirar cables D0/D1 al ESP y jumpers paralelos.\r\n\r\nEl flasher temporal debe estar cargado al baud elegido. Si falta, usar Cargar flasher en ATmega. Luego KEY/RST del ESP y consulta ROM; no escuchar 74880 por este puente fijo."
                    : selected == 2 ? "MÉTODO: adaptador USB–TTL externo.\r\n\r\nElegí el COM del adaptador; el USB del Mega solo alimenta el shield. VCC del adaptador sin conectar.\r\nTX del adaptador→RX ESP, TX ESP→RX del adaptador y GND común, por la ruta identificada.\r\nAislá D18/D19, D0/D1 y cualquier otro emisor paralelo al ESP. No unir RESET Mega a GND por este método.\r\nEl asistente KEY/RST puede escuchar el arranque a 74880."
                    : "MÉTODO: usar el conversor USB-serie del Mega con el ATmega detenido.\r\n\r\nUnir RESET del Mega a GND (NO RST del ESP).\r\nRetirar el enlace D18/D19.\r\nD0/RX0, salida del conversor USB→RX ESP por ruta adaptada.\r\nTX ESP→D1/TX0, entrada del conversor USB.\r\nSin jumpers/emisores paralelos.\r\n\r\nEste sentido corresponde al conversor USB, no al TX/RX del micro detenido. El asistente de KEY/RST y escucha 74880 se aplica a este método.";
                var checks = new[] { "Desconecté alimentación para recablear y mantengo potencia de actuadores desconectada.",
                    selected == 1 ? "RESET Mega LIBRE; D18→E_RX y E_TX→D19; D0/D1 al ESP retirados." : selected == 2 ? "USB–TTL externo, VCC sin conectar, TX/RX cruzados y GND; enlace serie Mega aislado." : "RESET Mega a GND; D18/D19 retirados; D0→RX ESP y TX ESP→D1 por ruta adaptada.",
                    "Masa común y alimentación lógica identificadas.",
                    "Retiré jumpers/emisores paralelos. Entiendo que elegir el método no mueve cables ni flashea por sí solo." };
                if (Confirm("Confirmar método de flasheo ESP", detail, checks))
                { SetProfileValue("EspTransport", next); Log("Método ESP seleccionado tras checklist: " + next + ". Cambiarlo no inició una carga ni abrió el puerto."); }
            }
            catch (Exception error) { MessageBox.Show(this, error.Message, "Método ESP", MessageBoxButtons.OK, MessageBoxIcon.Error); }
            finally { profileDialogs--; SyncBridgeOptions(); }
        }
        void InitializeMegaBridge(TabControl tabs)
        {
            bridgeTab = new TabPage("Flasher ATmega") { Padding = new Padding(UiInset) };
            var layout = new TableLayoutPanel { Dock = DockStyle.Fill, ColumnCount = 1, RowCount = 6 };
            layout.RowStyles.Add(new RowStyle(SizeType.Absolute, 54)); layout.RowStyles.Add(new RowStyle(SizeType.AutoSize));
            layout.RowStyles.Add(new RowStyle(SizeType.AutoSize)); layout.RowStyles.Add(new RowStyle(SizeType.Percent, 100));
            layout.RowStyles.Add(new RowStyle(SizeType.Absolute, 118)); layout.RowStyles.Add(new RowStyle(SizeType.Absolute, 52));
            var heading = UiRow(); heading.Controls.Add(new Label { Text = "Flasher temporal del Mega", AutoSize = true, Font = new Font(Font.FontFamily, 17, FontStyle.Bold), Margin = new Padding(0, 5, 24, 0) });
            AddButton(heading, "Información del flasher", () => OpenManualSection("mega-flasher"), 240); layout.Controls.Add(heading, 0, 0);
            var settingsPanel = new TableLayoutPanel { Dock = DockStyle.Top, AutoSize = true, Padding = new Padding(UiInset), BackColor = Color.White, ColumnCount = 1, RowCount = 3 };
            for (int i = 0; i < 3; i++) settingsPanel.RowStyles.Add(new RowStyle(SizeType.AutoSize));
            bridgeTransport.Items.AddRange(TransportLabels); idleControls.Add(bridgeTransport);
            settingsPanel.Controls.Add(UiField("Método de programación", bridgeTransport), 0, 0);
            bridgeSpeed.Items.AddRange(new object[] { "115200", "57600", "38400", "9600" }); idleControls.Add(bridgeSpeed);
            var baudRow = UiRow(); baudRow.Controls.Add(UiField("Baudios del flasher", bridgeSpeed));
            baudRow.Controls.Add(new Label { Text = "NOTA IMPORTANTE: cambiar la velocidad requiere volver a cargar el flasher.", ForeColor = Color.Firebrick, Font = new Font(Font, FontStyle.Bold), AutoSize = true, MaximumSize = new Size(610, 0), Margin = new Padding(8, 9, 0, 0) });
            settingsPanel.Controls.Add(baudRow, 0, 1); backupBeforeFlasher.Margin = new Padding(0, 16, 0, 8); settingsPanel.Controls.Add(backupBeforeFlasher, 0, 2); idleControls.Add(backupBeforeFlasher);
            layout.Controls.Add(settingsPanel, 0, 1);
            var actions = UiRow();
            AddButton(actions, "Verificar flasher", VerifyFlasher, 200);
            AddButton(actions, "Detectar ESP sin escribir", () => { SelectOperationTarget(1); ShowOperation(); DetectEsp(); }, 255);
            layout.Controls.Add(actions, 0, 2);
            layout.Controls.Add(new Label { Text = "1. Elegí método y baudios.\r\n2. Cargá el flasher temporal o verificá el que ya está instalado.\r\n3. Detectá el ESP y continuá desde Operación.\r\n4. Al terminar, restaurá el backup o el firmware del auto desde Operación.", Dock = DockStyle.Top, AutoSize = true, Padding = new Padding(0, 16, 0, 0), ForeColor = Color.FromArgb(71, 85, 105) }, 0, 3);
            layout.Controls.Add(bridgeServiceState, 0, 4);
            var footer = UiRow(); footer.FlowDirection = FlowDirection.RightToLeft; footer.Dock = DockStyle.Fill;
            AddButton(footer, "Cargar flasher en ATmega...", InstallFlasher, 285); AddButton(footer, "Flashear último backup", () => { ShowOperation(); FlashMegaBackup(); }, 245); layout.Controls.Add(footer, 0, 5);
            bridgeTab.Controls.Add(layout); tabs.TabPages.Add(bridgeTab);
            bridgeTransport.SelectionChangeCommitted += (s, e) => ChooseEspTransport(bridgeTransport.SelectedIndex);
            bridgeSpeed.SelectionChangeCommitted += (s, e) => { if (syncingBridge) return; ReadSettings(); SetProfileValue("BridgeBaud", Convert.ToString(bridgeSpeed.SelectedItem)); SetProfileValue("EspBaud", Convert.ToString(bridgeSpeed.SelectedItem)); SyncBridgeOptions(); };
            ports.SelectedIndexChanged += (s, e) => UpdateBridgeServiceState();
            settings.CellEndEdit += (s, e) => { if (!syncingBridge) { ReadSettings(); SyncBridgeOptions(); } };
            tabs.SelectedIndexChanged += (s, e) => { if (tabs.SelectedTab == bridgeTab) { ReadSettings(); SyncBridgeOptions(); } }; SyncBridgeOptions();
        }
        void SyncBridgeOptions()
        {
            if (operationTransport.Items.Count == 0) return;
            syncingBridge = true;
            try
            {
                operationTransport.SelectedIndex = Operations.UsesMegaBridge(profile) ? 1 : profile.Get("EspTransport") == "usb-ttl" ? 2 : 0;
                if (bridgeTransport.Items.Count > 0) bridgeTransport.SelectedIndex = operationTransport.SelectedIndex;
                bridgeSpeed.SelectedItem = profile.Get("BridgeBaud");
                UpdateTargetActions(); UpdateConnectionEvidence(""); UpdateBridgeServiceState();
            }
            finally { syncingBridge = false; }
        }
        void UpdateBridgeServiceState()
        {
            if (bridgeTab == null) return;
            string summary = "COM seleccionado: " + ports.Text + " | Conexión: " + profile.Get("EspTransport") + " | flasher " + profile.Get("BridgeBaud") + " | esptool " + profile.Get("EspBaud") + "\r\n";
            try
            {
                var record = String.IsNullOrEmpty(ports.Text) ? null : MegaServiceRecord.Read(ports.Text);
                summary += record == null ? "Sin registro de carga del flasher para este COM en esta PC." : "Último registro local: " + record.Status + " a " + record.Baud + " | " + record.At;
                if (record != null && !String.IsNullOrEmpty(record.CheckStatus)) summary += "\r\nÚltima comprobación en dispositivo: " + record.CheckStatus + " | " + record.CheckedAt;
                var backup = MegaFlashBackup.Latest();
                summary += backup == null ? "\r\nSin backup leído del equipo disponible." : "\r\nÚltimo backup: " + backup.Created + " | origen " + backup.Port + " | " + backup.Application;
                summary += "\r\nEl registro local no detecta si cambiaste de placa o firmware usando otra herramienta.";
            }
            catch (Exception error) { summary += "No se pudo leer el registro de servicio: " + error.Message; }
            bridgeServiceState.Text = summary;
        }
        void VerifyFlasher()
        {
            ShowOperation();
            Execute("Verificar flasher instalado en ATmega", (ops, p, port, token) => ops.VerifyMegaFlasher(p, port, token), true);
        }
        void InstallFlasher()
        {
            ShowOperation();
            Execute("Cargar flasher temporal en ATmega", InstallFlasherCore, true);
        }
        async Task InstallFlasherCore(Operations ops, Profile p, string port, CancellationToken token)
        {
            bool makeBackup = backupBeforeFlasher.Checked;
            if (!makeBackup && !ConfirmNoMegaBackup()) throw new OperationCanceledException("Se canceló la carga sin backup.");
            Log("Backup antes de cargar flasher: " + makeBackup);
            await ops.InstallMegaFlasher(p, port, token, makeBackup);
            SetProfileValue("EspTransport", "mega_bridge"); SetProfileValue("EspBaud", p.Get("BridgeBaud"));
            SyncBridgeOptions();
            Log("Conexión visible cambiada a flasher por software y baudios esptool alineados. Guardá el perfil si querés conservarlo. Falta consultar la ROM ESP.");
        }
    }
}
