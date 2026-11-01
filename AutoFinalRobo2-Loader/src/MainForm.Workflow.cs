using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace AFR2Loader
{
    public sealed partial class MainForm
    {
        // AFR2-20261027-20. Indicators consume operation events, never console keywords.
        readonly Dictionary<string, CheckBox> workflowChecks = new Dictionary<string, CheckBox>();
        readonly Dictionary<string, string> workflowNames = new Dictionary<string, string>();
        readonly ToolTip workflowTips = new ToolTip();
        readonly ToolStripStatusLabel activity = new ToolStripStatusLabel { Spring = true, TextAlign = ContentAlignment.MiddleLeft, AutoToolTip = true };
        readonly ToolStripStatusLabel concurrentActivity = new ToolStripStatusLabel { BorderSides = ToolStripStatusLabelBorderSides.Left };
        string activeStep;
        Label bootGuideStatus;

        string TargetName(int selected)
        {
            switch (selected)
            {
                case 0: return "ATmega";
                case 1: return EspFlashRules.IsAt(profile) ? "ESP AT" : "Firmware ESP";
                case 2: return EspFlashRules.IsAt(profile) ? "FileSystem AT" : "FileSystem";
                default: return EspFlashRules.IsAt(profile) ? "AT + FileSystem" : "ESP + FileSystem";
            }
        }
        void UpdateTargetActions()
        {
            if (compileTarget == null || flashTarget == null) return;
            string name = TargetName(target.SelectedIndex);
            if (target.SelectedIndex == 0) name = megaVariant.SelectedIndex == 1 ? "ATmega comunicación" : "ATmega completo";
            compileTarget.Text = EspFlashRules.IsAt(profile) && target.SelectedIndex != 0 ? "Revisar paquete AT" : "Compilar " + name;
            compileTarget.ToolTipText = "Preparar el destino seleccionado. La flecha ofrece otras acciones de compilación.";
            flashTarget.Text = "Flashear " + name;
            flashTarget.DropDownButtonWidth = compileTarget.DropDownButtonWidth = UiSplitWidth;
            if (flashTarget.DropDownItems.Count > 1) flashTarget.DropDownItems[1].Visible = target.SelectedIndex == 0;
            detectTarget.Text = target.SelectedIndex == 0 ? "Detectar ATmega" : "Detectar ESP / ROM";
            ButtonStyle(detectTarget); UpdatePreparationControls();
            listen.Visible = target.SelectedIndex != 0 && !Operations.UsesMegaBridge(profile);
        }
        readonly TableLayoutPanel workflowList = new TableLayoutPanel { Dock = DockStyle.Top, AutoSize = true, ColumnCount = 4, Margin = new Padding(0), Padding = new Padding(0) };
        readonly Label workflowEmpty = new Label { Text = "Las etapas aparecerán al iniciar una operación.", AutoSize = true, ForeColor = Color.FromArgb(85, 102, 119), Margin = new Padding(0, 6, 0, 6) };
        readonly HashSet<string> shownWorkflow = new HashSet<string>();
        Control CreateWorkflowPanel(Control log)
        {
            var panel = new TableLayoutPanel { Dock = DockStyle.Fill, ColumnCount = 1, RowCount = 2, Margin = new Padding(0) };
            panel.RowStyles.Add(new RowStyle(SizeType.AutoSize)); panel.RowStyles.Add(new RowStyle(SizeType.Percent, 100));
            var group = new GroupBox { Text = "Etapas de la operación", Dock = DockStyle.Top, AutoSize = true, Padding = new Padding(12, 20, 12, 10), Margin = new Padding(0, 0, 0, 12), BackColor = Color.White };
            for (int i = 0; i < 4; i++) workflowList.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 25));
            string[] keys = { "wiring", "reset", "bootloader", "connected", "esp_detected", "build_mega", "build_esp", "build_web_fs", "backup", "package", "writing", "verifying", "verified", "flasher_check" };
            string[] names = { "Cableado confirmado", "Reinicio ESP", "Modo programación", "Conexión confirmada", "ESP Detectado", "Compilación ATmega", "Compilación ESP", "FileSystem", "Backup flash ATmega", "Paquete AT revisado", "Escritura", "Verificación", "Carga verificada", "Flasher comprobado sin escritura" };
            for (int i = 0; i < keys.Length; i++) {
                var check = new CheckBox { Text = names[i], AutoCheck = false, ThreeState = true, TabStop = false, Dock = DockStyle.Fill, Height = 30, Margin = new Padding(0, 2, 8, 2), Visible = false };
                workflowChecks.Add(keys[i], check); workflowNames.Add(keys[i], names[i]);
            }
            ReflowWorkflow(); group.Controls.Add(workflowList); panel.Controls.Add(group, 0, 0); panel.Controls.Add(log, 0, 1); return panel;
        }
        void ReflowWorkflow()
        {
            workflowList.SuspendLayout(); workflowList.Controls.Clear(); workflowList.RowStyles.Clear();
            var checks = workflowChecks.Where(p => shownWorkflow.Contains(p.Key)).Select(p => p.Value).ToArray();
            workflowList.RowCount = Math.Max(1, (checks.Length + 3) / 4);
            if (checks.Length == 0) { workflowList.RowStyles.Add(new RowStyle(SizeType.AutoSize)); workflowList.Controls.Add(workflowEmpty, 0, 0); workflowList.SetColumnSpan(workflowEmpty, 4); }
            else {
                for (int i = 0; i < workflowList.RowCount; i++) workflowList.RowStyles.Add(new RowStyle(SizeType.Absolute, 34));
                for (int i = 0; i < checks.Length; i++) { workflowList.Controls.Add(checks[i], i % 4, i / 4); checks[i].Visible = true; }
            }
            workflowList.ResumeLayout(true);
        }
        void InitializeActivityBar()
        {
            var strip = new StatusStrip { SizingGrip = false, ShowItemToolTips = true };
            strip.Items.Add(activity);
            strip.Items.Add(new ToolStripControlHost(followLog) { AutoSize = false, Width = 132 });
            progress.Width = 170; progress.Height = 18;
            strip.Items.Add(new ToolStripControlHost(progress) { AutoSize = false, Width = 180 });
            strip.Items.Add(concurrentActivity); Controls.Add(strip);
            timer.Tick += (s, e) => UpdateActivityBar();
            FormClosed += (s, e) => workflowTips.Dispose();
            UpdateActivityBar();
        }
        void UpdateActivityBar()
        {
            activity.Text = busy && commandActivity.Length > 0 ? commandActivity : (busy ? "En curso: " : "") + status.Text;
            activity.ToolTipText = (currentJob == null ? "" : "Trabajo: " + currentJob.Action + "\r\n") + status.Text + (busy ? "\r\n" + commandProgress.Text : "");
            string com = serialCapture != null && serialCapture.Active ? serialCapture.Port + " abierto · " + serialCapture.Baud : (serialPorts.Text.Length > 0 ? serialPorts.Text : ports.Text) + " cerrado · " + serialBaud.Text;
            bool mcpOpen = mcpPeers.Values.Any(p => (DateTime.UtcNow - p.SeenUtc).TotalSeconds <= 30);
            concurrentActivity.Text = "Monitor " + (serialCapture != null && serialCapture.Active ? "abierto" : "cerrado") + " | " + com + " | MCP " + (mcpOpen ? "abierto" : "cerrado") + " · " + (pendingJob == null ? "sin pendientes" : "pendiente");
        }
        void ResetWorkflow(string title)
        {
            activeStep = null; shownWorkflow.Clear();
            bool build = title.StartsWith("Compilar", StringComparison.Ordinal) || title.StartsWith("build_", StringComparison.Ordinal) || title.StartsWith("Revisar paquete", StringComparison.Ordinal);
            bool all = title == "build_all" || title.StartsWith("Compilar ATmega +", StringComparison.Ordinal);
            bool install = title.IndexOf("flasher", StringComparison.OrdinalIgnoreCase) >= 0;
            bool backup = title.IndexOf("backup", StringComparison.OrdinalIgnoreCase) >= 0;
            backup = backup || title == "restore_mega_product";
            bool detect = title.StartsWith("Detectar", StringComparison.Ordinal) || title.StartsWith("probe_", StringComparison.Ordinal);
            bool esp = !install && !backup && (title.IndexOf("ESP", StringComparison.OrdinalIgnoreCase) >= 0 || title.IndexOf("Web", StringComparison.OrdinalIgnoreCase) >= 0 || title.IndexOf("Filesystem", StringComparison.OrdinalIgnoreCase) >= 0 || title.IndexOf(" AT +", StringComparison.Ordinal) >= 0 || (title.StartsWith("flash_", StringComparison.Ordinal) && !title.Contains("mega")));
            foreach (var pair in workflowChecks) {
                pair.Value.CheckState = CheckState.Unchecked; pair.Value.ForeColor = SystemColors.ControlText; pair.Value.Text = workflowNames[pair.Key]; pair.Value.Visible = false;
                workflowTips.SetToolTip(pair.Value, "Pendiente de esta operación.");
            }
            if (build) {
                int selected = title == "build_mega" ? 0 : title == "build_esp" ? 1 : title == "build_web_fs" ? 2 : target.SelectedIndex;
                if (all || selected == 0) shownWorkflow.Add("build_mega");
                if (all || selected != 0) {
                    if (EspFlashRules.IsAt(profile)) shownWorkflow.Add("package");
                    else {
                        if (all || selected == 1 || selected == 3) shownWorkflow.Add("build_esp");
                        if ((all || selected >= 2) && EspFlashRules.UseFilesystem(profile)) shownWorkflow.Add("build_web_fs");
                    }
                }
            } else {
                if (!backup) shownWorkflow.Add("wiring");
                if (esp || detect) shownWorkflow.Add("connected");
                if (esp && detect) shownWorkflow.Add("esp_detected");
                if (esp && !Operations.UsesMegaBridge(profile) && listen.Checked) { shownWorkflow.Add("reset"); shownWorkflow.Add("bootloader"); }
                if (install) { shownWorkflow.Add("build_mega"); if (backupBeforeFlasher.Checked) shownWorkflow.Add("backup"); }
                if (esp && EspFlashRules.IsAt(profile) && !detect) shownWorkflow.Add("package");
                if (!detect) { shownWorkflow.Add("writing"); shownWorkflow.Add("verifying"); shownWorkflow.Add("verified"); }
            }
            ReflowWorkflow();
        }
        void ShowStep(string key, CheckState state, string message)
        {
            CheckBox check = workflowChecks[key];
            bool added = shownWorkflow.Add(key);
            check.CheckState = state;
            check.ForeColor = state == CheckState.Checked ? Color.FromArgb(20, 110, 65) : Color.FromArgb(22, 79, 99);
            check.Text = workflowNames[key] + (state == CheckState.Indeterminate ? " · en curso" : "");
            workflowTips.SetToolTip(check, message);
            if (added) ReflowWorkflow();
        }

        void UpdateWorkflowNotice(LoaderNotice notice)
        {
            if (!notice.Origin.StartsWith("cargador ", StringComparison.Ordinal)) return;
            string code = notice.Code;
            if (busy && currentJob != null && notice.Origin == "cargador " + currentJob.Port && bootGuideStatus != null &&
                (code == "boot_listener_ready" || code == "reset_observed" || code == "normal_boot_observed" || code == "unknown_boot_observed"))
            { status.Text = notice.Message; currentJob.Stage = notice.Message; UpdateActivityBar(); }
            if (code == "esp_key_release_ready") { ShowKeyReleaseNotice(); }
            else if (code == "wiring_confirmed") ShowStep("wiring", CheckState.Checked, notice.Message);
            else if (code == "checking_flasher") { activeStep = "flasher_check"; ShowStep("flasher_check", CheckState.Indeterminate, notice.Message); }
            else if (code == "flasher_checked") { ShowStep("flasher_check", CheckState.Checked, notice.Message); activeStep = null; }
            else if (code == "backup_reading") { activeStep = "backup"; ShowStep("backup", CheckState.Indeterminate, notice.Message); }
            else if (code == "backup_completed") { ShowStep("backup", CheckState.Checked, notice.Message); activeStep = null; }
            else if (code == "reviewing_package") { activeStep = "package"; ShowStep("package", CheckState.Indeterminate, notice.Message); }
            else if (code == "package_ready") { ShowStep("package", CheckState.Checked, notice.Message); activeStep = null; }
            else if (code == "reset_observed")
            {
                ShowStep("reset", CheckState.Checked, notice.Message);
                ShowStep("bootloader", CheckState.Unchecked, "Esperando modo después de este reinicio.");
                ShowStep("connected", CheckState.Unchecked, "Esperando una consulta posterior a este reinicio.");
            }
            else if (code == "bootloader_observed") ShowStep("bootloader", CheckState.Checked, notice.Message);
            else if (code == "normal_boot_observed" || code == "unknown_boot_observed")
            {
                ShowStep("bootloader", CheckState.Unchecked, notice.Message);
                ShowStep("connected", CheckState.Unchecked, "Sin consulta posterior a este arranque.");
            }
            else if (code == "checking_bootloader" || code == "verification_paused")
            {
                ShowStep("writing", CheckState.Checked, "La escritura terminó; falta completar la verificación del Loader.");
                ShowStep("verifying", CheckState.Unchecked, notice.Message);
                ShowStep("connected", code == "checking_bootloader" ? CheckState.Indeterminate : CheckState.Unchecked, notice.Message);
                activeStep = code == "checking_bootloader" ? "connected" : null;
            }
            else if (code == "syncing") { activeStep = "connected"; ShowStep(activeStep, CheckState.Indeterminate, notice.Message); }
            else if (code == "rom_connected" || code == "device_connected")
            { ShowStep("connected", CheckState.Checked, notice.Message); if (code == "rom_connected" && shownWorkflow.Contains("esp_detected")) ShowStep("esp_detected", CheckState.Checked, notice.Message); if (activeStep == "connected") activeStep = null; }
            else if (code.StartsWith("building_", StringComparison.Ordinal))
            { activeStep = "build_" + code.Substring(9); ShowStep(activeStep, CheckState.Indeterminate, notice.Message); }
            else if (code.StartsWith("build_", StringComparison.Ordinal) && code.EndsWith("_completed", StringComparison.Ordinal))
            { ShowStep(code.Substring(0, code.Length - 10), CheckState.Checked, notice.Message); activeStep = null; }
            else if (code == "writing" || code == "verifying")
            {
                if (code == "verifying") ShowStep("writing", CheckState.Checked, "La herramienta inició la verificación después de escribir.");
                activeStep = code; ShowStep(code, CheckState.Indeterminate, notice.Message);
            }
            else if (code == "verified")
            {
                ShowStep("writing", CheckState.Checked, notice.Message); ShowStep("verifying", CheckState.Checked, notice.Message);
                ShowStep("verified", CheckState.Checked, notice.Message); activeStep = null;
            }
            else if ((code == "failed" || code == "cancelled") && activeStep != null)
            {
                var check = workflowChecks[activeStep]; check.ForeColor = code == "failed" ? Color.Firebrick : Color.DarkOrange;
                check.Text = workflowNames[activeStep] + (code == "failed" ? " · falló" : " · cancelado");
                workflowTips.SetToolTip(check, notice.Message); activeStep = null;
            }
            if (bootGuideStatus != null)
            {
                if (code == "boot_listener_ready") { bootGuideStatus.Text = "PUERTO LISTO. Ahora realizá los pasos 1 a 4."; bootGuideStatus.ForeColor = Color.DarkGreen; }
                else if (code == "normal_boot_observed" || code == "unknown_boot_observed")
                { bootGuideStatus.Text = "No entró en programación. Repetí los pasos 1 a 4; la escucha sigue abierta."; bootGuideStatus.ForeColor = Color.DarkOrange; }
                else if (code == "reset_observed") bootGuideStatus.Text = "Reinicio detectado. Esperando el mensaje de modo de programación...";
            }
        }
        Task GuideEspBoot(bool forVerification, Func<CancellationToken, Task> listenBoot, CancellationToken token)
        {
            token.ThrowIfCancellationRequested();
            SetState(forVerification ? "Verificación detenida: repetir KEY / RST para recuperar el cargador ESP" : "Esperando Continuar: asistente KEY / RST del ESP");
            using (var linked = CancellationTokenSource.CreateLinkedTokenSource(token))
            using (var dialog = new Form { Text = "Intervención física necesaria · KEY y RST del ESP", Size = new Size(760, 540), MinimumSize = new Size(660, 480), StartPosition = FormStartPosition.CenterParent, Font = Font, MinimizeBox = false, MaximizeBox = false })
            {
                bool running = false, complete = false; Exception failure = null;
                var layout = new TableLayoutPanel { Dock = DockStyle.Fill, Padding = new Padding(20), ColumnCount = 1, RowCount = 4 };
                layout.RowStyles.Add(new RowStyle(SizeType.Absolute, 65)); layout.RowStyles.Add(new RowStyle(SizeType.Percent, 100));
                layout.RowStyles.Add(new RowStyle(SizeType.Absolute, 65)); layout.RowStyles.Add(new RowStyle(SizeType.Absolute, 45));
                layout.Controls.Add(new Label { Text = forVerification ? "Intervení físicamente en KEY/RST para recuperar el cargador.\r\nPulsá Continuar y esperá PUERTO LISTO. No se volverá a escribir." : "Es necesario que intervengas apretando KEY y RST del ESP.\r\nPrimero pulsá Continuar y esperá el aviso PUERTO LISTO.", Dock = DockStyle.Fill, Font = new Font(Font, FontStyle.Bold) }, 0, 0);
                var instructions = new TextBox { Dock = DockStyle.Fill, Multiline = true, ReadOnly = true, ScrollBars = ScrollBars.Vertical, BackColor = Color.White,
                    Text = "Con el puerto listo:\r\n\r\n1. Apretá y mantené KEY del shield.\r\n\r\n2. Sin soltar KEY, apretá RST del shield.\r\n\r\n3. Soltá SOLO RST y seguí manteniendo KEY.\r\n\r\n4. Mantené KEY hasta el aviso del primer porcentaje de escritura. Si solo estás detectando, esperá a que termine la consulta.\r\n\r\nEl programa busca boot mode:(1,...). Al detectarlo cierra esta pantalla y consulta la ROM; seguí manteniendo KEY durante los pasos y confirmaciones siguientes.\r\nNo pulses el reset del ATmega; si usás su puente USB, su RESET sigue unido a GND.\r\nSi aparece modo 3, repetí la secuencia mientras la escucha siga abierta." };
                EnableTextCopyFeedback(instructions); layout.Controls.Add(instructions, 0, 1);
                bootGuideStatus = new Label { Text = "Listo para abrir la escucha. Todavía no pulses KEY / RST.", Dock = DockStyle.Fill, Font = new Font(Font, FontStyle.Bold), TextAlign = ContentAlignment.MiddleLeft };
                layout.Controls.Add(bootGuideStatus, 0, 2);
                var buttons = new FlowLayoutPanel { Dock = DockStyle.Fill, FlowDirection = FlowDirection.RightToLeft };
                var next = new ActionButton { Text = "Continuar: abrir escucha", Width = 225, Height = 34 };
                var back = new ActionButton { Text = "Cancelar", Width = 120, Height = 34 };
                next.Click += async (s, e) =>
                {
                    if (running) return;
                    running = true; next.Enabled = false; dialog.AcceptButton = null;
                    bootGuideStatus.Text = "Abriendo el puerto... Esperá el aviso antes de pulsar los botones.";
                    try { await listenBoot(linked.Token); complete = true; }
                    catch (Exception error) { failure = error; }
                    finally { running = false; dialog.Close(); }
                };
                back.Click += (s, e) => dialog.Close();
                dialog.FormClosing += (s, e) => { if (running) { e.Cancel = true; linked.Cancel(); back.Enabled = false; bootGuideStatus.Text = "Cerrando escucha y liberando el puerto..."; } };
                buttons.Controls.Add(next); buttons.Controls.Add(back); layout.Controls.Add(buttons, 0, 3);
                dialog.Controls.Add(layout); dialog.AcceptButton = next; dialog.CancelButton = back;
                FocusDialogAction(dialog, next, instructions);
                back.DialogResult = DialogResult.None;
                PrepareEspInterventionWindow(dialog);
                try { ShowStyledDialog(dialog); }
                finally { bootGuideStatus = null; }
                if (failure != null) return Task.FromException(failure);
                if (!complete) throw new OperationCanceledException("Secuencia KEY / RST cancelada antes de consultar la ROM.");
                return Task.FromResult(0);
            }
        }
        void RemindOperatingWiring(Profile loadedProfile, bool installedFlasher, bool megaFirmwareWritten, bool flashedBackup)
        {
            if (flashedBackup)
            {
                MessageBox.Show(this, "Último backup de aplicación ATmega flasheado y contrastado con lectura posterior.\r\n\r\nRESET Mega libre. El contenido es el programa que tenía al hacer el backup: puede ser firmware del auto, un flasher u otro programa. No habilitar actuadores hasta revisarlo. KEY libre y RST del ESP si corresponde.", "Backup ATmega cargado", MessageBoxButtons.OK, MessageBoxIcon.Information);
                return;
            }
            if (installedFlasher)
            {
                MessageBox.Show(this, "Flasher temporal del Mega escrito y verificado.\r\n\r\nDurante la carga, D18/D19 deben permanecer desconectados. Ahora, con la alimentación desconectada, conectá D18→E_RX / E_TX→D19 para usar el flasher. Mantené RESET Mega LIBRE. Potencia de actuadores desconectada.\r\nAhora elegí Detectar ESP / ROM; el asistente indicará KEY/RST del ESP.\r\n\r\nAl terminar usá Flashear Firmware ATmega o su menú Flashear último backup, revisando qué programa contiene.", "Flasher ATmega listo; falta consultar ESP", MessageBoxButtons.OK, MessageBoxIcon.Information);
                return;
            }
            if (Operations.UsesMegaBridge(loadedProfile) && !megaFirmwareWritten)
            {
                MessageBox.Show(this, "Contenido ESP escrito y verificado.\r\nRESET Mega sigue LIBRE; no cambies a RESET-GND.\r\n\r\nEl Mega sigue ejecutando el flasher temporal: usá Flashear Firmware ATmega o el menú Flashear último backup antes de habilitar actuadores. Revisá que el backup contenga el programa del auto. Después KEY libre y RST del ESP para arrancar desde flash.", "ESP verificado; firmware Mega pendiente", MessageBoxButtons.OK, MessageBoxIcon.Information);
                return;
            }
            MessageBox.Show(this,
                "Carga y verificación completadas.\r\n\r\nAntes de usar el robot, recordá volver a las conexiones de funcionamiento:\r\n\r\n" +
                "Mega D18 / TX1  →  E_RX / RX central del shield\r\n" +
                "E_TX / TX central del shield  →  Mega D19 / RX1\r\n\r\n" +
                "Son los pines centrales del selector ESP URAT.\r\n\r\n" +
                "Ya podés soltar KEY. Desconectá la alimentación antes de mover cables. Retirá el adaptador USB–TTL o los cables de servicio D0/D1 y el puente RESET del ATmega a GND, según la conexión usada. Dejá KEY libre para el arranque normal." +
                (EspFlashRules.IsAt(loadedProfile) ? "\r\n\r\nEste perfil usa AT: el programa del microcontrolador debe implementar AT. El firmware de producto AutoFinalRobo2 usa su protocolo propio y no se convierte a AT al cargar el ESP." : ""),
                "Carga verificada · recordar cableado de funcionamiento", MessageBoxButtons.OK, MessageBoxIcon.Information);
        }
    }
}
