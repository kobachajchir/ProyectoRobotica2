// AFR2-20261106-19. Presentation only: linker, offsets and sizing rules unchanged.
using System;
using System.Drawing;
using System.IO;
using System.Windows.Forms;
namespace AFR2Loader
{
    public sealed partial class MainForm
    {
        Label memoryCodeValue, memoryFsValue, memoryReservedValue, memoryPlacement;
        Panel memoryBar;
        long shownMemoryFlash, shownMemoryCode, shownMemoryFs, shownMemoryFsStart;
        Control CreateFilesystemCard()
        {
            var content = new TableLayoutPanel { Dock = DockStyle.Top, AutoSize = true, ColumnCount = 1, RowCount = 5, Margin = new Padding(0) };
            content.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100));
            for (int i = 0; i < 5; ++i) content.RowStyles.Add(new RowStyle(SizeType.AutoSize));
            var picker = new TableLayoutPanel { Dock = DockStyle.Top, AutoSize = true, ColumnCount = 2, Margin = new Padding(0, 0, 0, 6) };
            picker.ColumnStyles.Add(new ColumnStyle(SizeType.AutoSize)); picker.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100));
            picker.Controls.Add(new Label { Text = "Reserva FS", AutoSize = true, Anchor = AnchorStyles.Left, Margin = new Padding(0, 0, 12, 0) }, 0, 0);
            filesystemLayout.Dock = DockStyle.Fill; filesystemLayout.Margin = new Padding(0); picker.Controls.Add(filesystemLayout, 1, 0); content.Controls.Add(picker, 0, 0);
            var metrics = new TableLayoutPanel { Dock = DockStyle.Top, AutoSize = true, ColumnCount = 2, RowCount = 3, Margin = new Padding(0, 0, 0, 4) };
            metrics.ColumnStyles.Add(new ColumnStyle(SizeType.AutoSize)); metrics.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100));
            memoryCodeValue = new Label(); memoryFsValue = new Label(); memoryReservedValue = new Label();
            Label[] values = { memoryCodeValue, memoryFsValue, memoryReservedValue };
            string[] names = { "Código máx.", "FileSystem", "Reservado" };
            for (int i = 0; i < 3; ++i) {
                metrics.RowStyles.Add(new RowStyle(SizeType.AutoSize));
                metrics.Controls.Add(new Label { Text = names[i], AutoSize = true, ForeColor = LoaderAppearance.Muted, Margin = new Padding(0, 0, 14, 2) }, 0, i);
                values[i].Text = "—"; values[i].AutoSize = true; values[i].Font = new Font(Font, FontStyle.Bold); values[i].Margin = new Padding(0, 0, 0, 2);
                metrics.Controls.Add(values[i], 1, i);
            }
            content.Controls.Add(metrics, 0, 1);
            memoryBar = new Panel { Height = 12, Dock = DockStyle.Top, Margin = new Padding(0, 0, 0, 6) };
            memoryBar.Paint += (s, e) => {
                if (shownMemoryFlash <= 0) return;
                int code = (int)(memoryBar.ClientSize.Width * (double)shownMemoryCode / shownMemoryFlash);
                int fs = (int)(memoryBar.ClientSize.Width * (double)shownMemoryFs / shownMemoryFlash);
                int fsStart = (int)(memoryBar.ClientSize.Width * (double)shownMemoryFsStart / shownMemoryFlash);
                using (var brush = new SolidBrush(LoaderAppearance.Subtle)) e.Graphics.FillRectangle(brush, memoryBar.ClientRectangle);
                using (var brush = new SolidBrush(Color.FromArgb(30, 89, 153))) e.Graphics.FillRectangle(brush, 0, 0, code, memoryBar.Height);
                using (var brush = new SolidBrush(Color.FromArgb(24, 112, 69))) e.Graphics.FillRectangle(brush, fsStart, 0, fs, memoryBar.Height);
                using (var pen = new Pen(LoaderAppearance.Border)) e.Graphics.DrawRectangle(pen, 0, 0, Math.Max(0, memoryBar.Width - 1), Math.Max(0, memoryBar.Height - 1));
            };
            content.Controls.Add(memoryBar, 0, 2);
            memoryPlacement = new Label { AutoSize = true, Dock = DockStyle.Top, Margin = new Padding(0, 0, 0, 4) }; content.Controls.Add(memoryPlacement, 0, 3);
            var detail = new ActionButton { Text = "Ver detalle de memoria", Width = 200, Role = ButtonRole.Secondary, Anchor = AnchorStyles.Left };
            detail.Click += (s, e) => MessageBox.Show(this, memorySummary.Text, "Detalle de memoria", MessageBoxButtons.OK, MessageBoxIcon.Information);
            content.Controls.Add(detail, 0, 4);
            return content;
        }
        void UpdateFilesystemPresentation()
        {
            if (memoryCodeValue == null) return;
            shownMemoryFlash = shownMemoryCode = shownMemoryFs = 0;
            memoryCodeValue.Text = memoryFsValue.Text = memoryReservedValue.Text = "—";
            memoryPlacement.Text = "Confirmá la capacidad física para ver la distribución.";
            try {
                if (!CapacityConfirmed || EspFlashRules.IsAt(profile)) return;
                var layout = WebFlashLayout.Read(profile.Resolve("Linker"), profile.Number("FlashMB") * 1048576L);
                shownMemoryFlash = layout.FlashBytes; shownMemoryCode = layout.FirmwareMax; shownMemoryFs = layout.FsBytes; shownMemoryFsStart = layout.FsStart;
                memoryCodeValue.Text = EspFlashRules.Bytes(shownMemoryCode);
                memoryFsValue.Text = EspFlashRules.Bytes(shownMemoryFs);
                memoryReservedValue.Text = EspFlashRules.Bytes(shownMemoryFlash - shownMemoryCode - shownMemoryFs);
                memoryPlacement.Text = "Código · FileSystem · Reservado. Inicio FS: 0x" + layout.FsStart.ToString("X") + "\r\n" + Path.GetFileName(layout.Path) + " · El código respeta el máximo ejecutable del linker.";
            } catch (Exception error) { memoryPlacement.Text = error.Message; }
            finally { memoryBar.Invalidate(); }
        }
    }
}
