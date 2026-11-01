// AFR2-20261028-23. Supplied AFR2 and UNER assets, no generated replacements.
using System;
using System.Drawing;
using System.IO;
using System.Windows.Forms;

namespace AFR2Loader
{
    public sealed partial class MainForm
    {
        Image LoadBrandImage(string name)
        {
            string file = Path.Combine(appDir, "assets", "brand", name);
            if (!File.Exists(file)) return null;
            using (var image = Image.FromFile(file)) return new Bitmap(image);
        }
        void InitializeBrand()
        {
            using (var image = LoadBrandImage("afr2-loader-logo-blue.png")) {
                if (image == null) return;
                using (var small = new Bitmap(32, 32)) using (var graphics = Graphics.FromImage(small)) {
                    graphics.Clear(Color.Transparent); graphics.DrawImage(image, new Rectangle(0, 0, 32, 32));
                    IntPtr handle = small.GetHicon();
                    try { using (var native = System.Drawing.Icon.FromHandle(handle)) Icon = (Icon)native.Clone(); }
                    finally { DestroyIcon(handle); }
                }
            }
        }
        [System.Runtime.InteropServices.DllImport("user32.dll")]
        static extern bool DestroyIcon(IntPtr handle);
        void ShowAboutLoader()
        {
            // AFR2-20261028-24: adaptación visual; recursos y autoría originales conservados.
            using (var dialog = new Form { Text = "Acerca de AFR2 Loader", Size = new Size(790, 650), MinimumSize = new Size(790, 650), Font = new Font("Segoe UI", 10F), BackColor = Color.FromArgb(244, 247, 250), ForeColor = Color.FromArgb(35, 51, 65), StartPosition = FormStartPosition.CenterParent, Icon = Icon, MinimizeBox = false, MaximizeBox = false }) {
                var panel = new TableLayoutPanel { Dock = DockStyle.Fill, Padding = new Padding(24), ColumnCount = 1, RowCount = 5 };
                foreach (int height in new[] { 120, 100, 100 }) panel.RowStyles.Add(new RowStyle(SizeType.Absolute, height));
                panel.RowStyles.Add(new RowStyle(SizeType.Percent, 100)); panel.RowStyles.Add(new RowStyle(SizeType.Absolute, 48));
                var logo = new PictureBox { Dock = DockStyle.Left, Width = 100, SizeMode = PictureBoxSizeMode.Zoom, Image = LoadBrandImage("afr2-loader-logo-blue.png") };
                var small = new PictureBox { Dock = DockStyle.Right, Width = 110, SizeMode = PictureBoxSizeMode.Zoom, Image = LoadBrandImage("logoChicoUner.png") };
                var marks = new Panel { Dock = DockStyle.Fill, BackColor = Color.FromArgb(18, 79, 100), Padding = new Padding(12), Margin = new Padding(0, 0, 0, 16) };
                var title = new Label { Dock = DockStyle.Fill, Text = "AFR2 Loader", Font = new Font("Segoe UI", 25F, FontStyle.Bold), ForeColor = Color.White, TextAlign = ContentAlignment.MiddleLeft, Padding = new Padding(16, 0, 0, 0) };
                marks.Controls.Add(title); marks.Controls.Add(logo); marks.Controls.Add(small);
                var uner = new PictureBox { Dock = DockStyle.Fill, SizeMode = PictureBoxSizeMode.Zoom, Image = LoadBrandImage("LogoUner.png"), BackColor = Color.White };
                panel.Controls.Add(marks, 0, 0); panel.Controls.Add(uner, 0, 1);
                panel.Controls.Add(new Label { Dock = DockStyle.Fill, Text = "AFR2 Loader " + LoaderVersion.Value + "\r\nSimplifica la programación del auto Mecanum de Mecatrónica. Compila, prepara y carga ATmega, ESP y FileSystem. También permite flashear el firmware AT original del ESP8266.", Padding = new Padding(0, 12, 0, 0) }, 0, 2);
                var credits = new TableLayoutPanel { Dock = DockStyle.Fill, BackColor = Color.White, Padding = new Padding(16), Margin = new Padding(0, 8, 0, 12), ColumnCount = 1, RowCount = 3 };
                credits.RowStyles.Add(new RowStyle(SizeType.Absolute, 32)); credits.RowStyles.Add(new RowStyle(SizeType.Absolute, 32)); credits.RowStyles.Add(new RowStyle(SizeType.Percent, 100));
                credits.Controls.Add(new Label { Dock = DockStyle.Fill, Text = "Hecho por Koba Chajchir", Font = new Font("Segoe UI", 12F, FontStyle.Bold) }, 0, 0);
                var links = new FlowLayoutPanel { Dock = DockStyle.Fill, WrapContents = false };
                foreach (var entry in new[] { new[] { "GitHub", "https://github.com/kobachajchir" }, new[] { "LinkedIn", "https://www.linkedin.com/in/kchajchir" } }) {
                    string url = entry[1];
                    var link = new LinkLabel { Text = entry[0], AutoSize = true, LinkColor = Color.FromArgb(18, 79, 100), Margin = new Padding(0, 0, 24, 0), AccessibleDescription = url };
                    link.LinkClicked += (sender, args) => { try { System.Diagnostics.Process.Start(new System.Diagnostics.ProcessStartInfo(url) { UseShellExecute = true }); } catch (Exception ex) { MessageBox.Show(dialog, ex.Message, "No se pudo abrir el enlace"); } };
                    links.Controls.Add(link);
                }
                credits.Controls.Add(links, 0, 1);
                credits.Controls.Add(new Label { Dock = DockStyle.Fill, ForeColor = Color.FromArgb(80, 99, 113), Text = "Facultad de Ciencias de la Alimentación · UNER\r\nLas conexiones y los botones físicos requieren intervención del usuario." }, 0, 2);
                panel.Controls.Add(credits, 0, 3);
                var manual = new ActionButton { Text = "Abrir manual", Width = 200, Anchor = AnchorStyles.Right }; manual.Click += (s, e) => OpenManualSection("autor"); panel.Controls.Add(manual, 0, 4);
                dialog.Controls.Add(panel); ShowStyledDialog(dialog);
                if (logo.Image != null) logo.Image.Dispose(); if (uner.Image != null) uner.Image.Dispose(); if (small.Image != null) small.Image.Dispose();
            }
        }
    }
}
