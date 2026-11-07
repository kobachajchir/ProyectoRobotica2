// AFR2-20261028-17. Koba Chajchir / AFR2, adaptación mediante Codex.
using System;
using System.Drawing;
using System.Windows.Forms;

namespace AFR2Loader
{
    // AFR2-20261106-06. Explicit visual role; Auto keeps the legacy label-based guess.
    public enum ButtonRole { Auto, Primary, Secondary, Info, Danger }

    // Explicit painting keeps the icon gap and contrast independent of Windows themes.
    public sealed class ActionButton : Button
    {
        bool hovering, pressed;
        public ButtonRole Role { get; set; }
        public ActionButton() { SetStyle(ControlStyles.UserPaint | ControlStyles.OptimizedDoubleBuffer | ControlStyles.ResizeRedraw, true); }
        protected override void OnMouseEnter(EventArgs e) { hovering = true; Invalidate(); base.OnMouseEnter(e); }
        protected override void OnMouseLeave(EventArgs e) { hovering = pressed = false; Invalidate(); base.OnMouseLeave(e); }
        protected override void OnMouseDown(MouseEventArgs e) { pressed = true; Invalidate(); base.OnMouseDown(e); }
        protected override void OnMouseUp(MouseEventArgs e) { pressed = false; Invalidate(); base.OnMouseUp(e); }
        protected override void OnEnabledChanged(EventArgs e) { Invalidate(); base.OnEnabledChanged(e); }
        public override Size GetPreferredSize(Size proposedSize)
        { return new Size(TextRenderer.MeasureText(Text, Font).Width + (Image == null ? 32 : 60), Math.Max(38, Font.Height + 20)); }
        protected override void OnPaint(PaintEventArgs e)
        {
            Color background = !Enabled ? LoaderAppearance.Subtle : pressed ? ControlPaint.Dark(BackColor, .12F) : hovering ? ControlPaint.Light(BackColor, .10F) : BackColor;
            Color foreground = Enabled ? ForeColor : LoaderAppearance.Muted;
            using (var fill = new SolidBrush(background)) e.Graphics.FillRectangle(fill, ClientRectangle);
            using (var border = new Pen(Enabled ? FlatAppearance.BorderColor : Color.FromArgb(198, 207, 216))) e.Graphics.DrawRectangle(border, 0, 0, Width - 1, Height - 1);
            int iconWidth = Image == null ? 0 : 28;
            int textWidth = TextRenderer.MeasureText(Text, Font).Width;
            int left = Math.Max(12, (Width - textWidth - iconWidth) / 2);
            if (Image != null) {
                var icon = new Rectangle(left, (Height - 18) / 2, 18, 18);
                if (Enabled) e.Graphics.DrawImage(Image, icon);
                else using (var attributes = new System.Drawing.Imaging.ImageAttributes()) {
                    attributes.SetColorMatrix(new System.Drawing.Imaging.ColorMatrix(new float[][] {
                        new float[] {0,0,0,0,0}, new float[] {0,0,0,0,0}, new float[] {0,0,0,0,0}, new float[] {0,0,0,1,0}, new float[] {.36F,.41F,.46F,0,1}
                    }));
                    e.Graphics.DrawImage(Image, icon, 0, 0, Image.Width, Image.Height, GraphicsUnit.Pixel, attributes);
                }
            }
            TextRenderer.DrawText(e.Graphics, Text, Font, new Rectangle(left + iconWidth, 0, Math.Max(0, Width - left - iconWidth - 10), Height), foreground,
                TextFormatFlags.Left | TextFormatFlags.VerticalCenter | TextFormatFlags.SingleLine | TextFormatFlags.EndEllipsis);
            if (Focused && ShowFocusCues) ControlPaint.DrawFocusRectangle(e.Graphics, new Rectangle(4, 4, Width - 8, Height - 8), foreground, background);
        }
    }

    public sealed class ActionStripRenderer : ToolStripProfessionalRenderer
    {
        static bool HC { get { return SystemInformation.HighContrast; } }
        protected override void OnRenderToolStripBackground(ToolStripRenderEventArgs e)
        { using (var brush = new SolidBrush(HC ? SystemColors.Window : LoaderAppearance.Surface)) e.Graphics.FillRectangle(brush, e.AffectedBounds); }
        protected override void OnRenderImageMargin(ToolStripRenderEventArgs e)
        { using (var brush = new SolidBrush(HC ? SystemColors.Window : LoaderAppearance.Surface)) e.Graphics.FillRectangle(brush, e.AffectedBounds); }
        protected override void OnRenderMenuItemBackground(ToolStripItemRenderEventArgs e)
        { using (var brush = new SolidBrush(HC ? (e.Item.Selected ? SystemColors.Highlight : SystemColors.Window) : e.Item.Selected ? LoaderAppearance.Subtle : LoaderAppearance.Surface)) e.Graphics.FillRectangle(brush, new Rectangle(Point.Empty, e.Item.Size)); }
        static Color Ink(ToolStripItem item) { if (HC) return item.Enabled ? SystemColors.ControlText : SystemColors.GrayText; return item.Enabled ? Color.White : Color.FromArgb(92, 105, 117); }
        protected override void OnRenderSplitButtonBackground(ToolStripItemRenderEventArgs e)
        {
            var item = (ToolStripSplitButton)e.Item;
            Color color = HC ? SystemColors.ButtonFace : !item.Enabled ? Color.FromArgb(227, 232, 237) : item.Pressed || item.ButtonPressed ? Color.FromArgb(15, 84, 51) : item.Selected ? Color.FromArgb(28, 125, 78) : Color.FromArgb(22, 105, 65);
            using (var fill = new SolidBrush(color)) e.Graphics.FillRectangle(fill, new Rectangle(Point.Empty, item.Size));
            using (var pen = new Pen(item.Enabled ? Color.FromArgb(16, 85, 51) : Color.FromArgb(198, 207, 216))) e.Graphics.DrawRectangle(pen, 0, 0, item.Width - 1, item.Height - 1);
            using (var pen = new Pen(item.Enabled ? Color.FromArgb(107, 168, 135) : Color.FromArgb(198, 207, 216))) e.Graphics.DrawLine(pen, item.DropDownButtonBounds.Left, 8, item.DropDownButtonBounds.Left, item.Height - 8);
            int x = item.DropDownButtonBounds.Left + item.DropDownButtonBounds.Width / 2, y = item.Height / 2;
            using (var brush = new SolidBrush(Ink(item))) e.Graphics.FillPolygon(brush, new[] { new Point(x - 4, y - 2), new Point(x + 4, y - 2), new Point(x, y + 3) });
        }
        protected override void OnRenderItemText(ToolStripItemTextRenderEventArgs e)
        {
            e.TextColor = e.Item is ToolStripSplitButton ? Ink(e.Item) : HC ? SystemColors.WindowText : LoaderAppearance.Text;
            if (e.Item is ToolStripSplitButton) e.TextRectangle = new Rectangle(48, 0, Math.Max(0, e.Item.Width - 48 - 40), e.Item.Height);
            base.OnRenderItemText(e);
        }
        protected override void OnRenderItemImage(ToolStripItemImageRenderEventArgs e)
        {
            if (e.Item is ToolStripSplitButton && e.Image != null) { e.Graphics.DrawImageUnscaled(e.Image, 16, (e.Item.Height - e.Image.Height) / 2); return; }
            base.OnRenderItemImage(e);
        }
        protected override void OnRenderArrow(ToolStripArrowRenderEventArgs e)
        { if (e.Item is ToolStripSplitButton) e.ArrowColor = Ink(e.Item); base.OnRenderArrow(e); }
        protected override void OnRenderToolStripBorder(ToolStripRenderEventArgs e)
        { if (e.ToolStrip is ToolStripDropDown) base.OnRenderToolStripBorder(e); }
    }
}
