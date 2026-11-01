// AFR2-20261029-12. Koba Chajchir / AFR2; adaptación mediante Codex.
using System;
using System.Drawing;
using System.Windows.Forms;

namespace AFR2Loader
{
    public sealed partial class MainForm
    {
        readonly CheckBox skipSessionChecks = new CheckBox { Text = "Omitir confirmaciones manuales de preparación (solo esta sesión)", AutoSize = true };
        bool sessionChecksAccepted, syncingSessionChecks;
        void InitializeSessionChecks(TableLayoutPanel options)
        {
            options.RowCount = 3; options.RowStyles.Add(new RowStyle(SizeType.AutoSize));
            skipSessionChecks.Dock = DockStyle.Top; skipSessionChecks.Margin = new Padding(3, 4, 3, 4);
            options.Controls.Add(skipSessionChecks, 0, 2); idleControls.Add(skipSessionChecks);
            skipSessionChecks.CheckedChanged += (s, e) => {
                if (syncingSessionChecks) return;
                bool requested = skipSessionChecks.Checked;
                SyncSessionChecks(); // Todavía no está activado mientras se muestra la advertencia.
                if (busy || pendingJob != null || profileDialogs > 0) return;
                SetSessionChecks(requested, () => !busy && pendingJob == null);
            };
        }
        void SyncSessionChecks()
        {
            syncingSessionChecks = true;
            try { skipSessionChecks.Checked = sessionChecksAccepted; }
            finally { syncingSessionChecks = false; }
        }
        bool SetSessionChecks(bool enabled, Func<bool> stillAllowed)
        {
            if (enabled == sessionChecksAccepted) return true;
            if (enabled) {
                profileDialogs++;
                try {
                    using (var dialog = new Form { Text = "Advertencia · omitir confirmaciones de preparación", Size = new Size(780, 510), StartPosition = FormStartPosition.CenterParent, Font = Font, MinimizeBox = false, MaximizeBox = false }) {
                        var layout = new TableLayoutPanel { Dock = DockStyle.Fill, Padding = new Padding(24), ColumnCount = 1, RowCount = 2 };
                        layout.RowStyles.Add(new RowStyle(SizeType.Percent, 100)); layout.RowStyles.Add(new RowStyle(SizeType.Absolute, 60));
                        layout.Controls.Add(new TextBox { Dock = DockStyle.Fill, Multiline = true, ReadOnly = true, ScrollBars = ScrollBars.Vertical, BorderStyle = BorderStyle.None, BackColor = dialog.BackColor,
                            Text = "Activar esta opción presupone que usted conoce el procedimiento y ha verificado físicamente el cableado, la alimentación, la aislación y la desconexión de la potencia de los actuadores.\r\n\r\nDurante esta sesión, el Loader dará por aceptadas las casillas y confirmaciones manuales de preparación y carga, y continuará sin volver a solicitarlas, tanto desde la ventana como desde MCP. También se omitirá el aviso de continuar sin un backup nuevo si esa fue la opción elegida.\r\n\r\nEl software no comprueba esas condiciones físicas. Si cambia la placa, el cableado o la alimentación, desactive esta opción y revise el montaje antes de continuar.\r\n\r\nLas comprobaciones automáticas de archivos, puerto, firma, capacidad, escritura y verificación permanecen activas. La intervención física KEY/RST del ESP siempre requiere un aviso y confirmación local, incluso con esta opción activa. El aviso restaura la ventana si está minimizada.\r\n\r\nLa autorización termina al desmarcar la opción o cerrar la aplicación. Volver a activarla requiere aceptar nuevamente esta advertencia.\r\n\r\n¿Desea continuar?" }, 0, 0);
                        var buttons = new FlowLayoutPanel { Dock = DockStyle.Fill, AutoSize = true, WrapContents = true, FlowDirection = FlowDirection.RightToLeft };
                        var accept = new ActionButton { Text = "Aceptar y continuar", Width = 230, DialogResult = DialogResult.OK };
                        var reject = new ActionButton { Text = "Rechazar", Width = 150, DialogResult = DialogResult.Cancel };
                        buttons.Controls.Add(accept); buttons.Controls.Add(reject); layout.Controls.Add(buttons, 0, 1);
                        dialog.Controls.Add(layout); dialog.CancelButton = reject;
                        dialog.Shown += (s, e) => reject.Focus();
                        if (ShowStyledDialog(dialog) != DialogResult.OK) return false;
                    }
                } finally { profileDialogs--; }
            }
            if (!stillAllowed()) return false;
            sessionChecksAccepted = enabled; SyncSessionChecks();
            Log("Confirmaciones manuales de preparación " + (enabled ? "omitidas por aceptación LOCAL de la advertencia, solo esta sesión." : "restablecidas para esta sesión."));
            return true;
        }
    }
}


