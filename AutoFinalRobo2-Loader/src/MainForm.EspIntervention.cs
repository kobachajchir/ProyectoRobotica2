// AFR2-20261029-13. Koba Chajchir / AFR2; adaptación mediante Codex.
using System;
using System.Drawing;
using System.Threading;
using System.Windows.Forms;

namespace AFR2Loader
{
    public sealed partial class MainForm
    {
        [System.Runtime.InteropServices.DllImport("user32.dll")]
        static extern bool SetForegroundWindow(IntPtr window);
        void PrepareEspInterventionWindow(Form dialog)
        {
            ShowOperation();
            if (WindowState == FormWindowState.Minimized) WindowState = FormWindowState.Normal;
            Show(); BringToFront(); Activate(); SetForegroundWindow(Handle);
            dialog.ShowInTaskbar = false;
            dialog.Shown += (s, e) => {
                // Subir solo al presentar el aviso; no dejar ventanas siempre encima.
                dialog.TopMost = true; dialog.BringToFront(); dialog.Activate();
                SetForegroundWindow(dialog.Handle); dialog.TopMost = false;
            };
        }
        bool ConfirmEspPhysicalIntervention(string title, string text, CancellationToken token)
        {
            token.ThrowIfCancellationRequested();
            SetState("Intervención física necesaria: KEY y RST del ESP");
            using (var dialog = new Form { Text = title, Size = new Size(780, 540), MinimumSize = new Size(660, 460), StartPosition = FormStartPosition.CenterParent, Font = Font, MinimizeBox = false, MaximizeBox = false }) {
                var layout = new TableLayoutPanel { Dock = DockStyle.Fill, Padding = new Padding(24), ColumnCount = 1, RowCount = 4 };
                layout.RowStyles.Add(new RowStyle(SizeType.Absolute, 60)); layout.RowStyles.Add(new RowStyle(SizeType.Percent, 100));
                layout.RowStyles.Add(new RowStyle(SizeType.Absolute, 62)); layout.RowStyles.Add(new RowStyle(SizeType.Absolute, 52));
                layout.Controls.Add(new Label { Text = "Es necesario que intervengas en el ESP.\r\nUsá KEY y RST del shield, no RESET del ATmega.", Dock = DockStyle.Fill, Font = new Font(Font, FontStyle.Bold), ForeColor = Color.FromArgb(18, 79, 100) }, 0, 0);
                layout.Controls.Add(new TextBox { Text = text, Dock = DockStyle.Fill, ReadOnly = true, Multiline = true, ScrollBars = ScrollBars.Vertical, BackColor = Color.White }, 0, 1);
                var done = new CheckBox { Text = "Realicé la secuencia KEY/RST y sigo manteniendo KEY.", Dock = DockStyle.Fill, AutoSize = true };
                layout.Controls.Add(done, 0, 2);
                var buttons = new FlowLayoutPanel { Dock = DockStyle.Fill, FlowDirection = FlowDirection.RightToLeft };
                var next = new ActionButton { Text = "Confirmar y continuar", Width = 240, Enabled = false, DialogResult = DialogResult.OK };
                var cancelPhysical = new ActionButton { Text = "Cancelar", Width = 140, DialogResult = DialogResult.Cancel };
                done.CheckedChanged += (s, e) => next.Enabled = done.Checked;
                buttons.Controls.Add(next); buttons.Controls.Add(cancelPhysical); layout.Controls.Add(buttons, 0, 3);
                dialog.Controls.Add(layout); dialog.CancelButton = cancelPhysical;
                PrepareEspInterventionWindow(dialog);
                bool accepted = ShowStyledDialog(dialog) == DialogResult.OK && done.Checked;
                token.ThrowIfCancellationRequested();
                Log(accepted ? "Intervención física KEY/RST confirmada localmente; falta respuesta ROM." : "Intervención física ESP cancelada; no se continúa.");
                return accepted;
            }
        }
    }
}
