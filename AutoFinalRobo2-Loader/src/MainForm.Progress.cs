using System;
using System.Drawing;
using System.Windows.Forms;

namespace AFR2Loader
{
    public sealed partial class MainForm
    {
        readonly object commandFeedbackGate = new object();
        CommandFeedback pendingCommandFeedback;
        string commandActivity = "";
        readonly Label commandProgress = new Label { Dock = DockStyle.Fill, AutoEllipsis = true,
            Text = "Avance de carga: sin operación.", ForeColor = Color.FromArgb(71, 85, 105) };

        void QueueCommandFeedback(CommandFeedback feedback)
        { lock (commandFeedbackGate) pendingCommandFeedback = feedback; }

        void DrainCommandFeedback()
        {
            CommandFeedback feedback;
            lock (commandFeedbackGate) { feedback = pendingCommandFeedback; pendingCommandFeedback = null; }
            if (feedback == null || !busy) return;
            string elapsed = TimeSpan.FromSeconds(feedback.ElapsedSeconds).ToString(@"hh\:mm\:ss");
            string reported = feedback.Percent.HasValue ? feedback.Percent.Value + "% informado por esptool (imagen actual)" : "sin porcentaje informado";
            commandProgress.Text = feedback.Phase + " · " + elapsed + " · " + reported + "\r\n" +
                (feedback.Running ? "Sin salida nueva hace " + (int)feedback.QuietSeconds + " s. " : "Proceso finalizado; continúa el flujo. ") + feedback.LastOutput;
            commandProgress.ForeColor = feedback.Running && feedback.QuietSeconds >= 15 ? Color.FromArgb(145, 80, 0) : Color.FromArgb(71, 85, 105);
            commandActivity = feedback.Running ? feedback.Phase + " · " + elapsed + (feedback.Percent.HasValue ? " · " + feedback.Percent.Value + "%" : "") +
                " · sin salida " + (int)feedback.QuietSeconds + " s" : "";
            workflowTips.SetToolTip(commandProgress, commandProgress.Text);
            UpdateActivityBar();
            progress.Style = feedback.Running && feedback.Percent.HasValue ? ProgressBarStyle.Blocks : ProgressBarStyle.Marquee;
            progress.Value = feedback.Percent ?? 0;
            // A reported 100% belongs to a single image; the existing operation
            // events and exit checks remain responsible for final verification.
        }

        void ResetCommandFeedback()
        {
            lock (commandFeedbackGate) pendingCommandFeedback = null;
            commandActivity = "";
            commandProgress.Text = "Esperando herramienta. El avance aparecerá al iniciar el proceso.";
            commandProgress.ForeColor = Color.FromArgb(71, 85, 105);
            progress.Value = 0;
        }
    }
}
