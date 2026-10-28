using System;
using System.Collections.Generic;
using System.Drawing;
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
                case 1: return EspFlashRules.IsAt(profile) ? "ESP AT" : "ESP Web";
                case 2: return EspFlashRules.IsAt(profile) ? "Filesystem AT" : "Web / LittleFS";
                default: return EspFlashRules.IsAt(profile) ? "AT + filesystem" : "ESP + Web / LittleFS";
            }
        }
        void UpdateTargetActions()
        {
            string name = TargetName(target.SelectedIndex);
            compileTarget.Text = EspFlashRules.IsAt(profile) && target.SelectedIndex != 0 ? "Revisar paquete AT" : "Compilar " + name;
            compileTarget.ToolTipText = EspFlashRules.IsAt(profile) && target.SelectedIndex != 0
                ? "Revisar los BIN precompilados y el mapa del paquete AT; no compila Arduino ni firmware del proveedor."
                : "Compilar el destino seleccionado con el perfil y layout visibles.";
            flashTarget.Text = "Flashear " + name + "...";
            detectTarget.Text = target.SelectedIndex == 0 ? "Detectar ATmega" : "Detectar ESP / ROM";
            listen.Visible = target.SelectedIndex != 0;
            for (int i = 0; i < compileTarget.DropDownItems.Count; i++)
            {
                var item = compileTarget.DropDownItems[i] as ToolStripMenuItem;
                if (item != null) item.Checked = i == target.SelectedIndex;
            }
        }
        Control CreateWorkflowPanel(Control log)
        {
            var panel = new TableLayoutPanel { Dock = DockStyle.Fill, ColumnCount = 2, RowCount = 1, Margin = new Padding(0) };
            panel.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100));
            panel.ColumnStyles.Add(new ColumnStyle(SizeType.Absolute, 260));
            panel.Controls.Add(log, 0, 0);
            var group = new GroupBox { Text = "Etapas de la operación", Dock = DockStyle.Fill, Padding = new Padding(10), Margin = new Padding(10, 0, 0, 0) };
            var list = new FlowLayoutPanel { Dock = DockStyle.Fill, AutoScroll = true, FlowDirection = FlowDirection.TopDown, WrapContents = false };
            list.Controls.Add(new Label { Text = "Vacío: pendiente · marca: completado\r\nCasilla rellena: en curso", Width = 218, Height = 42 });
            string[] keys = { "wiring", "reset", "bootloader", "connected", "build_mega", "build_esp", "build_web_fs", "package", "writing", "verifying", "verified" };
            string[] names = { "Cableado confirmado", "Reinicio ESP detectado", "Modo programación detectado", "Conexión confirmada", "Compilación ATmega", "Compilación ESP", "Compilación Web / LittleFS", "Paquete AT revisado", "Escritura", "Verificación", "Carga verificada" };
            for (int i = 0; i < keys.Length; i++)
            {
                var check = new CheckBox { Text = names[i], AutoCheck = false, ThreeState = true, TabStop = false, Width = 218, Height = 38, Margin = new Padding(0, 2, 0, 2) };
                workflowChecks.Add(keys[i], check); workflowNames.Add(keys[i], names[i]); list.Controls.Add(check);
            }
            group.Controls.Add(list); panel.Controls.Add(group, 1, 0);
            return panel;
        }
        void InitializeActivityBar()
        {
            var strip = new StatusStrip { SizingGrip = false, ShowItemToolTips = true };
            strip.Items.Add(activity); strip.Items.Add(concurrentActivity); Controls.Add(strip);
            timer.Tick += (s, e) => UpdateActivityBar();
            FormClosed += (s, e) => workflowTips.Dispose();
            UpdateActivityBar();
        }
        void UpdateActivityBar()
        {
            activity.Text = (busy ? "En curso: " : "") + status.Text;
            activity.ToolTipText = (currentJob == null ? "" : "Trabajo: " + currentJob.Action + "\r\n") + status.Text;
            string monitor = serialCapture == null ? "cerrado" : serialCapture.Active ? serialCapture.Port + " / " + serialCapture.Baud : "cerrado";
            concurrentActivity.Text = "Monitor: " + monitor + (serialTransition ? " (cambiando)" : "") + " | MCP: " + (pendingJob == null ? "sin pendientes" : "esperando revisión");
        }
        void ResetWorkflow(string title)
        {
            activeStep = null;
            bool build = title.StartsWith("Compilar", StringComparison.Ordinal) || title.StartsWith("build_", StringComparison.Ordinal) || title.StartsWith("Revisar paquete", StringComparison.Ordinal);
            bool all = title == "build_all" || title.StartsWith("Compilar ATmega +", StringComparison.Ordinal);
            foreach (var pair in workflowChecks)
            {
                pair.Value.CheckState = CheckState.Unchecked; pair.Value.ForeColor = SystemColors.ControlText;
                pair.Value.Text = workflowNames[pair.Key]; workflowTips.SetToolTip(pair.Value, "Sin completar en esta operación.");
                bool buildStep = pair.Key.StartsWith("build_", StringComparison.Ordinal) || pair.Key == "package";
                if (!build) pair.Value.Visible = !buildStep && (target.SelectedIndex != 0 || (pair.Key != "reset" && pair.Key != "bootloader"));
                else if (EspFlashRules.IsAt(profile)) pair.Value.Visible = (pair.Key == "package" && (all || target.SelectedIndex != 0)) || (pair.Key == "build_mega" && (all || target.SelectedIndex == 0));
                else pair.Value.Visible = buildStep && pair.Key != "package" && (all || pair.Key == (target.SelectedIndex == 0 ? "build_mega" : target.SelectedIndex == 1 ? "build_esp" : "build_web_fs") || (target.SelectedIndex == 3 && pair.Key == "build_esp")) && (pair.Key != "build_web_fs" || EspFlashRules.UseFilesystem(profile));
            }
        }
        void ShowStep(string key, CheckState state, string message)
        {
            CheckBox check = workflowChecks[key]; check.Visible = true; check.CheckState = state;
            check.ForeColor = state == CheckState.Checked ? Color.FromArgb(20, 110, 65) : Color.FromArgb(22, 79, 99);
            check.Text = workflowNames[key] + (state == CheckState.Indeterminate ? " · en curso" : "");
            workflowTips.SetToolTip(check, message);
        }
        void UpdateWorkflowNotice(LoaderNotice notice)
        {
            if (!notice.Origin.StartsWith("cargador ", StringComparison.Ordinal)) return;
            string code = notice.Code;
            if (busy && currentJob != null && notice.Origin == "cargador " + currentJob.Port && bootGuideStatus != null &&
                (code == "boot_listener_ready" || code == "reset_observed" || code == "normal_boot_observed" || code == "unknown_boot_observed"))
            { status.Text = notice.Message; currentJob.Stage = notice.Message; UpdateActivityBar(); }
            if (code == "wiring_confirmed") ShowStep("wiring", CheckState.Checked, notice.Message);
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
            else if (code == "syncing") { activeStep = "connected"; ShowStep(activeStep, CheckState.Indeterminate, notice.Message); }
            else if (code == "rom_connected" || code == "device_connected")
            { ShowStep("connected", CheckState.Checked, notice.Message); if (activeStep == "connected") activeStep = null; }
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
        Task GuideEspBoot(Func<CancellationToken, Task> listenBoot, CancellationToken token)
        {
            token.ThrowIfCancellationRequested();
            SetState("Esperando Continuar: asistente K / RST del ESP");
            using (var linked = CancellationTokenSource.CreateLinkedTokenSource(token))
            using (var dialog = new Form { Text = "Entrar en programación ESP · K y RST", Size = new Size(760, 540), MinimumSize = new Size(660, 480), StartPosition = FormStartPosition.CenterParent, Font = Font, MinimizeBox = false, MaximizeBox = false })
            {
                bool running = false, complete = false; Exception failure = null;
                var layout = new TableLayoutPanel { Dock = DockStyle.Fill, Padding = new Padding(20), ColumnCount = 1, RowCount = 4 };
                layout.RowStyles.Add(new RowStyle(SizeType.Absolute, 65)); layout.RowStyles.Add(new RowStyle(SizeType.Percent, 100));
                layout.RowStyles.Add(new RowStyle(SizeType.Absolute, 65)); layout.RowStyles.Add(new RowStyle(SizeType.Absolute, 45));
                layout.Controls.Add(new Label { Text = "Al momento de flashear te voy a avisar para que presiones K y RST.\r\nPrimero pulsá Continuar y esperá el aviso PUERTO LISTO.", Dock = DockStyle.Fill, Font = new Font(Font, FontStyle.Bold) }, 0, 0);
                var instructions = new TextBox { Dock = DockStyle.Fill, Multiline = true, ReadOnly = true, ScrollBars = ScrollBars.Vertical, BackColor = Color.White,
                    Text = "Con el puerto listo:\r\n\r\n1. Apretá y mantené K (KEY) del shield.\r\n\r\n2. Sin soltar K, apretá RST del shield.\r\n\r\n3. Soltá RST y seguí manteniendo K.\r\n\r\n4. Soltá K. El programa busca boot mode:(1,...).\r\n\r\nNo pulses el reset del ATmega. Su RESET sigue unido a GND.\r\nSi aparece modo 3, repetí la secuencia mientras la escucha siga abierta.\r\nAl detectar modo 1 se cerrará esta pantalla y se consultará la ROM." };
                EnableTextCopyFeedback(instructions); layout.Controls.Add(instructions, 0, 1);
                bootGuideStatus = new Label { Text = "Listo para abrir la escucha. Todavía no pulses K / RST.", Dock = DockStyle.Fill, Font = new Font(Font, FontStyle.Bold), TextAlign = ContentAlignment.MiddleLeft };
                layout.Controls.Add(bootGuideStatus, 0, 2);
                var buttons = new FlowLayoutPanel { Dock = DockStyle.Fill, FlowDirection = FlowDirection.RightToLeft };
                var next = new Button { Text = "Continuar: abrir escucha", Width = 225, Height = 34 };
                var back = new Button { Text = "Cancelar", Width = 120, Height = 34 };
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
                try { dialog.ShowDialog(this); }
                finally { bootGuideStatus = null; }
                if (failure != null) return Task.FromException(failure);
                if (!complete) throw new OperationCanceledException("Secuencia K / RST cancelada antes de consultar la ROM.");
                return Task.FromResult(0);
            }
        }
        void RemindOperatingWiring(Profile loadedProfile)
        {
            MessageBox.Show(this,
                "Carga y verificación completadas.\r\n\r\nAntes de usar el robot, recordá volver a las conexiones de funcionamiento:\r\n\r\n" +
                "Mega D18 / TX1  →  E_RX / RX central del shield\r\n" +
                "E_TX / TX central del shield  →  Mega D19 / RX1\r\n\r\n" +
                "Son los pines centrales del selector ESP URAT.\r\n\r\n" +
                "Desconectá la alimentación antes de mover cables. Retirá los cables de servicio D0/D1 y quitá el puente RESET del ATmega a GND. Dejá K (KEY) libre para el arranque normal." +
                (EspFlashRules.IsAt(loadedProfile) ? "\r\n\r\nEste perfil usa AT: el programa del microcontrolador debe implementar AT. El firmware de producto AutoFinalRobo2 usa su protocolo propio y no se convierte a AT al cargar el ESP." : ""),
                "Carga verificada · recordar cableado de funcionamiento", MessageBoxButtons.OK, MessageBoxIcon.Information);
        }
    }
}
