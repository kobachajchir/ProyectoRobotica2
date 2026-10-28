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
        static readonly object[] TransportLabels = { "Puente serial del Mega — D0/D1, RESET a GND", "Flasher en ATmega — D18/D19, RESET libre", "Adaptador USB–TTL externo — lógica 3,3 V" };
        Control CreateEspMethodSelector()
        {
            var row = new FlowLayoutPanel { Dock = DockStyle.Fill, WrapContents = false };
            row.Controls.Add(new Label { Text = "Método ESP", AutoSize = true, Margin = new Padding(0, 8, 8, 0) });
            operationTransport.Items.AddRange(TransportLabels); row.Controls.Add(operationTransport); idleControls.Add(operationTransport);
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
                    : selected == 2 ? "MÉTODO: adaptador USB–TTL externo con lógica 3,3 V.\r\n\r\nElegí el COM del adaptador; el USB del Mega solo alimenta el shield. VCC del adaptador sin conectar.\r\nTX del adaptador→RX ESP, TX ESP→RX del adaptador y GND común, por la ruta identificada.\r\nAislá D18/D19, D0/D1 y cualquier otro emisor paralelo al ESP. No unir RESET Mega a GND por este método.\r\nEl asistente KEY/RST puede escuchar el arranque a 74880."
                    : "MÉTODO: usar el conversor USB-serie del Mega con el ATmega detenido.\r\n\r\nUnir RESET del Mega a GND (NO RST del ESP).\r\nRetirar el enlace D18/D19.\r\nD0/RX0, salida del conversor USB→RX ESP por ruta adaptada.\r\nTX ESP→D1/TX0, entrada del conversor USB.\r\nSin jumpers/emisores paralelos.\r\n\r\nEste sentido corresponde al conversor USB, no al TX/RX del micro detenido. El asistente de KEY/RST y escucha 74880 se aplica a este método.";
                var checks = new[] { "Desconecté alimentación para recablear y mantengo potencia de actuadores desconectada.",
                    selected == 1 ? "RESET Mega LIBRE; D18→E_RX y E_TX→D19; D0/D1 al ESP retirados." : selected == 2 ? "USB–TTL de lógica 3,3 V, VCC sin conectar, TX/RX cruzados y GND; enlace serie Mega aislado." : "RESET Mega a GND; D18/D19 retirados; D0→RX ESP y TX ESP→D1 por ruta adaptada.",
                    "Ruta de niveles identificada: no TX de 5 V al RX directo ESP. Masa común y alimentación lógica identificadas.",
                    "Retiré jumpers/emisores paralelos. Entiendo que elegir el método no mueve cables ni flashea por sí solo." };
                if (Confirm("Confirmar método de flasheo ESP", detail, checks))
                { SetProfileValue("EspTransport", next); Log("Método ESP seleccionado tras checklist: " + next + ". Cambiarlo no inició una carga ni abrió el puerto."); }
            }
            catch (Exception error) { MessageBox.Show(this, error.Message, "Método ESP", MessageBoxButtons.OK, MessageBoxIcon.Error); }
            finally { profileDialogs--; SyncBridgeOptions(); }
        }
        void InitializeMegaBridge(TabControl tabs)
        {
            bridgeTab = new TabPage("Flasher ATmega") { Padding = new Padding(16) };
            var layout = new TableLayoutPanel { Dock = DockStyle.Fill, ColumnCount = 1, RowCount = 5 };
            foreach (int height in new[] { 52, 134, 83 }) layout.RowStyles.Add(new RowStyle(SizeType.Absolute, height));
            layout.RowStyles.Add(new RowStyle(SizeType.Percent, 100)); layout.RowStyles.Add(new RowStyle(SizeType.Absolute, 100));
            layout.Controls.Add(new Label { Text = "Programar el ESP usando el Mega del auto", Dock = DockStyle.Fill, Font = new Font(Font.FontFamily, 17, FontStyle.Bold) }, 0, 0);
            var options = new FlowLayoutPanel { Dock = DockStyle.Fill, WrapContents = true };
            options.Controls.Add(new Label { Text = "Conexión ESP", AutoSize = true, Margin = new Padding(0, 8, 8, 0) });
            bridgeTransport.Items.AddRange(TransportLabels);
            options.Controls.Add(bridgeTransport); idleControls.Add(bridgeTransport); options.SetFlowBreak(bridgeTransport, true);
            options.Controls.Add(new Label { Text = "Baudios del flasher", AutoSize = true, Margin = new Padding(0, 8, 8, 0) });
            bridgeSpeed.Items.AddRange(new object[] { "115200", "57600", "38400", "9600" }); options.Controls.Add(bridgeSpeed); idleControls.Add(bridgeSpeed);
            var speedHint = new Label { Text = "Cambiar esta velocidad requiere volver a cargar el flasher. El COM se elige en Operación.", Width = 570, Height = 36, Margin = new Padding(12, 5, 0, 0) };
            options.Controls.Add(speedHint); options.SetFlowBreak(speedHint, true);
            options.Controls.Add(backupBeforeFlasher); idleControls.Add(backupBeforeFlasher);
            layout.Controls.Add(options, 0, 1);
            var actions = new FlowLayoutPanel { Dock = DockStyle.Fill, WrapContents = true };
            AddButton(actions, "Cargar flasher en ATmega...", InstallFlasher, 260);
            actions.Controls.Add(CreateFirmwareFlashMenu(false));
            AddButton(actions, "Detectar ESP sin escribir", () => { target.SelectedIndex = 1; DetectEsp(); }, 215);
            AddButton(actions, "Elegir puerto / imágenes", () => tabs.SelectedIndex = 0, 210);
            AddButton(actions, "Manual y plan de prueba", OpenManual, 210);
            AddButton(actions, "Preferencias...", () => { if (preferencesTab != null) mainTabs.SelectedTab = preferencesTab; }, 145);
            layout.Controls.Add(actions, 0, 2);
            var guide = new TextBox { Dock = DockStyle.Fill, Multiline = true, ReadOnly = true, ScrollBars = ScrollBars.Vertical, BackColor = Color.White,
                Text = "PRUEBA A — PUENTE POR SOFTWARE\r\n\r\n1. Elegí el COM del Mega. Configurá el HEX del auto para cargarlo al terminar o usá un backup que contenga ese programa. Desconectá potencia de actuadores.\r\n2. Quitá RESET Mega→GND. Conservá D18/TX1→E_RX y E_TX→D19/RX1 por la ruta de niveles del shield. Retirá cables D0/D1 al ESP y jumpers paralelos. TX y RX se cruzan por función; no por la posición física del header.\r\n3. Elegí Backup de ATmega antes de cargar. Marcado: lee flash/hfuse dos veces y conserva un respaldo contrastado antes de escribir; si falla, se detiene. Desmarcado: avisa que no creará backup nuevo. No mostrar de nuevo y el valor predeterminado se cambian en Preferencias. Pulsá Cargar flasher en ATmega: compila solo el servicio y pide confirmar su escritura/verificación.\r\n4. Tras éxito se selecciona la conexión por software y el mismo baud para esptool. Pulsá Detectar ESP sin escribir. El asistente indica KEY/K + RST del ESP; no pide RESET-GND ni escucha a 74880.\r\n5. Si ROM/flash responden, elegí los BIN y cargá ESP primero; después filesystem si corresponde. Cada carga consulta y verifica. No continuar después de una consulta fallida.\r\n6. Al terminar o abandonar: Flashear Firmware ATmega carga el HEX configurado; la flecha del mismo botón ofrece Flashear último backup. El respaldo es lo que estaba en el Mega cuando se leyó: puede ser otro flasher. No incluye EEPROM ni programa fuses/bootloader. Revisá qué contiene antes de operar. KEY libre y resetear ESP para ejecutar flash. El auto no está operativo mientras el Mega ejecuta el flasher.\r\n\r\nPRUEBA B — ALTERNATIVA FÍSICA, SOLO SI A FALLA\r\n\r\nGuardar logs y detenerse. Desconectar alimentación antes de recablear; identificar adaptación de nivel. En una conexión directa, el divisor es solo para TX de 5 V→RX ESP (ejemplo 1k/2k). No duplicar el traductor del shield ni usar resistencias como fuente. Elegir explícitamente Mega en RESET, luego RESET→GND y ruta D0→RX ESP / TX ESP→D1 adaptada. Nunca mantener ese reset en el modo por software.\r\n\r\nLas marcas de verificación corresponden a la acción realizada, no prueban que el cableado haya sido medido. No hay cambio automático a la prueba B ni restauración automática al cerrar." };
            layout.Controls.Add(guide, 0, 3); layout.Controls.Add(bridgeServiceState, 0, 4); bridgeTab.Controls.Add(layout); tabs.TabPages.Add(bridgeTab);
            bridgeTransport.SelectionChangeCommitted += (s, e) => {
                if (syncingBridge || busy || pendingJob != null) return;
                ChooseEspTransport(bridgeTransport.SelectedIndex);
            };
            bridgeSpeed.SelectionChangeCommitted += (s, e) => {
                if (syncingBridge || busy || pendingJob != null) return;
                ReadSettings(); SetProfileValue("BridgeBaud", bridgeSpeed.Text); SyncBridgeOptions();
            };
            ports.SelectedIndexChanged += (s, e) => UpdateBridgeServiceState();
            settings.CellEndEdit += (s, e) => { if (!syncingBridge) { ReadSettings(); SyncBridgeOptions(); } };
            tabs.SelectedIndexChanged += (s, e) => { if (tabs.SelectedTab == bridgeTab) { ReadSettings(); SyncBridgeOptions(); } };
            SyncBridgeOptions();
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
                var backup = MegaFlashBackup.Latest();
                summary += backup == null ? "\r\nSin backup leído del equipo disponible." : "\r\nÚltimo backup: " + backup.Created + " | origen " + backup.Port + " | " + backup.Application;
                summary += "\r\nEl registro local no detecta si cambiaste de placa o firmware usando otra herramienta.";
            }
            catch (Exception error) { summary += "No se pudo leer el registro de servicio: " + error.Message; }
            bridgeServiceState.Text = summary;
        }
        void InstallFlasher()
        {
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
