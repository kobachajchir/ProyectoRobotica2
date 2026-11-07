// AFR2-20261106-19. Koba Chajchir / AFR2; adaptación mediante Codex.
using System;
using System.Drawing;
using System.Windows.Forms;
namespace AFR2Loader
{
    public sealed partial class MainForm
    {
        TableLayoutPanel operationLayout, operationLeft, operationRight, operationContent, operationActions, summaryContent;
        Panel setupViewport;
        GroupBox prepCard; TableLayoutPanel prepCardContent; Control espPanel;
        bool fittingOperation;
        Control[] operationSummaryValues;

        void ArrangeOperationColumns(TableLayoutPanel layout)
        {
            var heading = layout.GetControlFromPosition(0, 0);
            var selectors = layout.GetControlFromPosition(0, 1);
            var oldSummary = layout.GetControlFromPosition(0, 3);
            var oldWorkspace = layout.GetControlFromPosition(0, 4) as TableLayoutPanel;
            var actions = layout.GetControlFromPosition(0, 5) as TableLayoutPanel;
            var log = oldWorkspace.GetControlFromPosition(0, 0);
            var stages = oldWorkspace.GetControlFromPosition(1, 0);
            layout.SuspendLayout(); layout.Controls.Clear(); layout.RowStyles.Clear(); layout.ColumnStyles.Clear();
            oldWorkspace.Controls.Remove(log); oldWorkspace.Controls.Remove(stages);
            layout.ColumnCount = 2; layout.RowCount = 2;
            layout.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100));
            layout.ColumnStyles.Add(new ColumnStyle(SizeType.Absolute, 420));
            layout.RowStyles.Add(new RowStyle(SizeType.Percent, 100));
            layout.RowStyles.Add(new RowStyle(SizeType.Absolute, 100));
            operationLayout = layout; operationActions = actions;

            operationLeft = new TableLayoutPanel { Dock = DockStyle.Fill, ColumnCount = 1, RowCount = 2, Margin = new Padding(0, 0, 16, 0) };
            operationLeft.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100));
            operationLeft.RowStyles.Add(new RowStyle(SizeType.Absolute, 300));
            operationLeft.RowStyles.Add(new RowStyle(SizeType.Percent, 100));
            setupViewport = new Panel { Dock = DockStyle.Fill, AutoScroll = true, Margin = new Padding(0, 0, 0, 12) };
            operationContent = new TableLayoutPanel { Dock = DockStyle.Top, AutoSize = true, AutoSizeMode = AutoSizeMode.GrowAndShrink, ColumnCount = 1, RowCount = 2, Margin = new Padding(0) };
            operationContent.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100));
            operationContent.RowStyles.Add(new RowStyle(SizeType.AutoSize)); operationContent.RowStyles.Add(new RowStyle(SizeType.AutoSize));
            operationContent.Controls.Add(heading, 0, 0); operationContent.Controls.Add(selectors, 0, 1);
            setupViewport.Controls.Add(operationContent);
            var logRegion = new TableLayoutPanel { Dock = DockStyle.Fill, ColumnCount = 1, RowCount = 2, Margin = new Padding(0) };
            logRegion.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100));
            logRegion.RowStyles.Add(new RowStyle(SizeType.AutoSize)); logRegion.RowStyles.Add(new RowStyle(SizeType.Percent, 100));
            logRegion.Controls.Add(SectionLabel("Registro de operación"), 0, 0); logRegion.Controls.Add(log, 0, 1);
            log.Margin = new Padding(0); log.Dock = DockStyle.Fill;
            operationLeft.Controls.Add(setupViewport, 0, 0); operationLeft.Controls.Add(logRegion, 0, 1);

            operationRight = new TableLayoutPanel { Dock = DockStyle.Fill, ColumnCount = 1, RowCount = 3, Margin = new Padding(0) };
            operationRight.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100));
            operationRight.RowStyles.Add(new RowStyle(SizeType.AutoSize));
            operationRight.RowStyles.Add(new RowStyle(SizeType.Percent, 100));
            operationRight.RowStyles.Add(new RowStyle(SizeType.Absolute, 140));
            // (filas: preparación auto, resultado 100 %, etapas fijas)
            var card = new GroupBox { Text = "Ejecución y resultado", Dock = DockStyle.Fill, Padding = new Padding(16, 24, 16, 12), Margin = new Padding(0, 0, 0, 10) };
            var summaryViewport = new Panel { Dock = DockStyle.Fill, AutoScroll = true };
            summaryContent = new TableLayoutPanel { Dock = DockStyle.Top, AutoSize = true, AutoSizeMode = AutoSizeMode.GrowAndShrink, ColumnCount = 1, Margin = new Padding(0) };
            summaryContent.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100));
            operationSummaryValues = new Control[] { status, lastResult, commandProgress, evidence };
            string[] captions = { "Estado actual", "Resultado", "Actividad de la herramienta", "Conexión y evidencias" };
            for (int i = 0; i < operationSummaryValues.Length; ++i) {
                var value = operationSummaryValues[i]; oldSummary.Controls.Remove(value);
                var caption = new Label { Text = captions[i], AutoSize = true, Font = new Font(Font, FontStyle.Bold), ForeColor = LoaderAppearance.Muted, Margin = new Padding(0, i == 0 ? 0 : 16, 0, 6) };
                value.AutoSize = true; value.Dock = DockStyle.Top; value.MinimumSize = Size.Empty; value.Margin = new Padding(0);
                var label = value as Label; if (label != null) label.AutoEllipsis = false;
                summaryContent.RowStyles.Add(new RowStyle(SizeType.AutoSize)); summaryContent.RowStyles.Add(new RowStyle(SizeType.AutoSize));
                summaryContent.Controls.Add(caption, 0, 2 * i); summaryContent.Controls.Add(value, 0, 2 * i + 1);
            }
            summaryViewport.Controls.Add(summaryContent); card.Controls.Add(summaryViewport);
            // Existing stage controls remain compact, with no stage scrollbar.
            stages.Margin = new Padding(0); stages.Dock = DockStyle.Fill;
            // Preparación + FileSystem: solo existe en pantalla cuando la sección elegida incluye FileSystem.
            espPanel = selectors; var capacityHost = capacityRow; var memoryHost = espMemoryRow;
            prepCard = new GroupBox { Text = "Preparación · FileSystem", Dock = DockStyle.Fill, AutoSize = true, AutoSizeMode = AutoSizeMode.GrowAndShrink, Padding = new Padding(12, 20, 12, 6), Margin = new Padding(0, 0, 0, 10), Visible = false };
            prepCardContent = new TableLayoutPanel { Dock = DockStyle.Top, AutoSize = true, AutoSizeMode = AutoSizeMode.GrowAndShrink, ColumnCount = 1, RowCount = 2, Margin = new Padding(0) };
            prepCardContent.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100));
            prepCardContent.RowStyles.Add(new RowStyle(SizeType.AutoSize)); prepCardContent.RowStyles.Add(new RowStyle(SizeType.AutoSize));
            if (memoryHost != null) { ((Control)selectors).Controls.Remove(memoryHost); memoryHost.Margin = new Padding(0); prepCardContent.Controls.Add(memoryHost, 0, 1); }
            prepCard.Controls.Add(prepCardContent);
            prepCard.SizeChanged += (s, e) => {
                int w = Math.Max(120, prepCardContent.ClientSize.Width - 4);
                capacityState.MaximumSize = new Size(w, 0); if (memoryPlacement != null) memoryPlacement.MaximumSize = new Size(w, 0);
            };
            operationRight.Controls.Add(prepCard, 0, 0); operationRight.Controls.Add(card, 0, 1); operationRight.Controls.Add(stages, 0, 2);
            layout.Controls.Add(operationLeft, 0, 0); layout.Controls.Add(operationRight, 1, 0);
            actions.AutoSize = false; actions.Dock = DockStyle.Fill;
            layout.Controls.Add(actions, 0, 1); layout.SetColumnSpan(actions, 2);
            resultHeading = new Label { Visible = false }; // legacy destination label; card owns the visible heading
            layout.SizeChanged += (s, e) => FitOperationRows();
            operationContent.SizeChanged += (s, e) => FitOperationRows();
            summaryViewport.SizeChanged += (s, e) => {
                int width = Math.Max(160, summaryViewport.ClientSize.Width - SystemInformation.VerticalScrollBarWidth - 4);
                foreach (var value in operationSummaryValues) value.MaximumSize = new Size(width, 0);
            };
            oldSummary.Dispose(); oldWorkspace.Dispose();
            layout.ResumeLayout(true); FitOperationRows();
        }

        void FitOperationColumns()
        {
            if (operationLayout == null || operationLayout.IsDisposed || fittingOperation || operationLayout.ClientSize.Height <= 0) return;
            fittingOperation = true;
            try {
                float scale = DisplayScale(); int width = operationLayout.ClientSize.Width;
                int side = (int)(Math.Min(500, Math.Max(340, width / scale * .28)) * scale);
                if (Math.Abs(operationLayout.ColumnStyles[1].Width - side) > 1) operationLayout.ColumnStyles[1].Width = side;
                int actions = MeasureOperationActions(width, scale);
                operationLayout.RowStyles[1].Height = actions;
                int body = Math.Max(160, operationLayout.ClientSize.Height - actions);
                int setupPreferred = operationContent.GetPreferredSize(new Size(Math.Max(240, operationLeft.ClientSize.Width), 0)).Height + 12;
                int setupLimit = Math.Max((int)(130 * scale), Math.Min((int)(body * .55), body - (int)(220 * scale)));
                operationLeft.RowStyles[0].Height = Math.Min(setupPreferred, setupLimit);
                int stages = Math.Max((int)(105 * scale), WorkflowRequiredHeight());
                operationRight.RowStyles[2].Height = stages;
            } finally { fittingOperation = false; }
        }
    }
}
