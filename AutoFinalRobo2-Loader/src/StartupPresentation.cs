// AFR2-20261029-04. Koba Chajchir / AFR2; adaptación mediante Codex.
// Referencia: StartupWelcome.tsx/startup.css de nuestra Web; implementación nativa WinForms.
using System;
using System.Diagnostics;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.IO;
using System.Threading;
using System.Windows.Forms;

namespace AFR2Loader
{
    // Un STA separado mantiene la bienvenida fluida mientras MainForm prepara controles.
    sealed class StartupPresentation : IDisposable
    {
        readonly ManualResetEvent ready = new ManualResetEvent(false);
        readonly ManualResetEvent ended = new ManualResetEvent(false);
        volatile WelcomeForm form;
        public StartupPresentation(string appDir)
        {
            Point position = Cursor.Position;
            var thread = new Thread(() => {
                try {
                    using (var welcome = new WelcomeForm(appDir, position)) {
                        form = welcome;
                        welcome.Shown += (s, e) => ready.Set();
                        Application.Run(welcome);
                    }
                } catch (Exception error) { Debug.WriteLine("Bienvenida: " + error.Message); }
                finally { ready.Set(); ended.Set(); }
            });
            thread.IsBackground = true; thread.SetApartmentState(ApartmentState.STA); thread.Start();
            ready.WaitOne();
        }
        void Send(Action<WelcomeForm> action)
        {
            var window = form;
            if (window == null || window.IsDisposed || !window.IsHandleCreated) return;
            try { window.BeginInvoke(new Action(() => { if (!window.IsDisposed) action(window); })); }
            catch (InvalidOperationException) { }
        }
        public void Finish(Form main)
        {
            // AFR2-20261029-05: la UI nunca depende del cierre del splash para ser visible.
            RevealMain(main);
            Send(window => window.Finish());
            ThreadPool.QueueUserWorkItem(_ => {
                ended.WaitOne();
                try {
                    main.BeginInvoke(new Action(() => {
                        RevealMain(main);
                    }));
                } catch (InvalidOperationException) { }
            });
        }
        static void RevealMain(Form main)
        {
            if (main.IsDisposed || main.Disposing) return;
            main.Opacity = 1;
            if (main.WindowState == FormWindowState.Minimized) main.WindowState = FormWindowState.Normal;
            if (!main.Visible) main.Show();
            main.BringToFront(); main.Activate();
        }
        public void Dispose() { Send(window => window.Close()); }
    }

    sealed class WelcomeForm : Form
    {
        readonly System.Windows.Forms.Timer timer = new System.Windows.Forms.Timer { Interval = 16 };
        readonly Stopwatch clock = Stopwatch.StartNew();
        readonly Font titleFont = new Font("Segoe UI", 24, FontStyle.Bold);
        readonly Font detailFont = new Font("Segoe UI", 10);
        readonly Font versionFont = new Font("Segoe UI", 12, FontStyle.Bold);
        Image logo;
        readonly bool reducedMotion = ReduceMotion();
        [System.Runtime.InteropServices.DllImport("user32.dll")]
        static extern bool SystemParametersInfo(uint action, uint parameter, ref int value, uint flags);
        static bool ReduceMotion()
        {
            int animations = 0;
            return SystemInformation.HighContrast || !SystemParametersInfo(0x1042, 0, ref animations, 0) || animations == 0;
        }
        bool leaving;
        double exitAt, exitOpacity;
        public WelcomeForm(string appDir, Point position)
        {
            Text = "AFR2 Loader — Iniciando"; FormBorderStyle = FormBorderStyle.None;
            ShowInTaskbar = false; StartPosition = FormStartPosition.Manual;
            AutoScaleMode = AutoScaleMode.Dpi; ClientSize = new Size(500, 370);
            var area = Screen.FromPoint(position).WorkingArea;
            Location = new Point(area.Left + (area.Width - Width) / 2, area.Top + (area.Height - Height) / 2);
            BackColor = Color.FromArgb(19, 39, 52); ForeColor = Color.White;
            DoubleBuffered = true; KeyPreview = true; Opacity = 0;
            AccessibleName = "Iniciando AFR2 Loader";
            string file = Path.Combine(appDir, "assets", "brand", "afr2-loader-logo-blue.png");
            if (File.Exists(file)) { using (var original = Image.FromFile(file)) logo = new Bitmap(original); }
            KeyDown += (s, e) => { if (e.KeyCode == Keys.Escape) Close(); };
            timer.Tick += (s, e) => {
                double elapsed = clock.Elapsed.TotalMilliseconds;
                if (leaving) {
                    double progress = Math.Min(1, (elapsed - exitAt) / (reducedMotion ? 120 : 200));
                    Opacity = exitOpacity * (1 - EaseOut(progress));
                    if (progress >= 1) { Close(); return; }
                } else Opacity = EaseOut(Math.Min(1, elapsed / 250));
                Invalidate();
            };
            Shown += (s, e) => { clock.Restart(); timer.Start(); };
        }
        public void Finish()
        {
            if (leaving) return;
            leaving = true; exitAt = clock.Elapsed.TotalMilliseconds; exitOpacity = Opacity;
        }
        // Misma curva ease-out que la referencia de motion: cubic-bezier(.23,1,.32,1).
        static double EaseOut(double x)
        {
            double lo = 0, hi = 1, t = x;
            for (int i = 0; i < 16; i++) {
                t = (lo + hi) / 2; double u = 1 - t;
                double bx = 3 * u * u * t * .23 + 3 * u * t * t * .32 + t * t * t;
                if (bx < x) lo = t; else hi = t;
            }
            return 1 - Math.Pow(1 - t, 3);
        }
        protected override void OnPaint(PaintEventArgs e)
        {
            base.OnPaint(e);
            var g = e.Graphics; g.SmoothingMode = SmoothingMode.AntiAlias;
            g.InterpolationMode = InterpolationMode.HighQualityBicubic;
            float unit = ClientSize.Width / 500F;
            g.ScaleTransform(unit, unit);
            // Halo estático y logo con escala breve; sin redibujar la UI principal.
            using (var path = new GraphicsPath()) {
                path.AddEllipse(100, 0, 300, 280);
                using (var halo = new PathGradientBrush(path)) {
                    halo.CenterColor = Color.FromArgb(70, 22, 122, 150);
                    halo.SurroundColors = new[] { Color.FromArgb(0, 22, 122, 150) };
                    g.FillEllipse(halo, 100, 0, 300, 280);
                }
            }
            double progress = EaseOut(Math.Min(1, clock.Elapsed.TotalMilliseconds / 250));
            float size = (float)(132 * (reducedMotion ? 1 : .95 + .05 * progress));
            if (logo != null) g.DrawImage(logo, (500 - size) / 2, 62 + (132 - size) / 2, size, size);
            using (var center = new StringFormat { Alignment = StringAlignment.Center })
            using (var white = new SolidBrush(Color.White))
            using (var muted = new SolidBrush(Color.FromArgb(191, 216, 228))) {
                g.DrawString("AFR2 Loader", titleFont, white, new RectangleF(12, 214, 476, 48), center);
                g.DrawString("Versión " + LoaderVersion.Value, versionFont, white, new RectangleF(12, 262, 476, 28), center);
                g.DrawString("Preparando la interfaz…", detailFont, muted, new RectangleF(12, 296, 476, 28), center);
                g.DrawString("Esc para omitir", detailFont, muted, new RectangleF(12, 334, 476, 28), center);
            }
        }
        protected override void Dispose(bool disposing)
        {
            if (disposing) { timer.Dispose(); if (logo != null) logo.Dispose(); titleFont.Dispose(); detailFont.Dispose(); versionFont.Dispose(); }
            base.Dispose(disposing);
        }
    }
}
