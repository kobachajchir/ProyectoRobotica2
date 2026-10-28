using System;
using System.Drawing;
using System.Runtime.InteropServices;
using System.Windows.Forms;

namespace AFR2Loader
{
    // AFR2-20261027-20. Shared by the operation log and the serial monitor.
    public sealed class LogTextBox : RichTextBox
    {
        const int GetScrollPosition = 0x04DD, SetScrollPosition = 0x04DE, SetRedraw = 0x000B;
        [DllImport("user32.dll", EntryPoint = "SendMessageW")]
        static extern IntPtr SendPoint(IntPtr handle, int message, IntPtr parameter, ref Point point);
        [DllImport("user32.dll", EntryPoint = "SendMessageW")]
        static extern IntPtr SendValue(IntPtr handle, int message, IntPtr parameter, IntPtr value);
        CheckBox follow;
        bool updating;

        public void BindFollow(CheckBox option)
        {
            follow = option;
            follow.CheckedChanged += (s, e) => { if (follow.Checked) GoToEnd(); };
        }
        public void GoToEnd()
        {
            updating = true;
            try { Select(TextLength, 0); ScrollToCaret(); }
            finally { updating = false; }
        }
        void PauseFollow() { if (!updating && follow != null) follow.Checked = false; }
        protected override void OnMouseDown(MouseEventArgs e) { PauseFollow(); base.OnMouseDown(e); }
        protected override void WndProc(ref Message message)
        {
            // Wheel, scrollbars and keyboard navigation belong to the reader.
            if (message.Msg == 0x020A || message.Msg == 0x020E || message.Msg == 0x0115 || message.Msg == 0x0114) PauseFollow();
            if (message.Msg == 0x0100)
            {
                Keys key = (Keys)message.WParam.ToInt32();
                if (key == Keys.Up || key == Keys.Down || key == Keys.Left || key == Keys.Right ||
                    key == Keys.PageUp || key == Keys.PageDown || key == Keys.Home || key == Keys.End ||
                    (key == Keys.A && (ModifierKeys & Keys.Control) != 0)) PauseFollow();
            }
            base.WndProc(ref message);
        }
        public void AppendBatch(string text)
        {
            if (String.IsNullOrEmpty(text)) return;
            Point position = new Point();
            SendPoint(Handle, GetScrollPosition, IntPtr.Zero, ref position);
            int start = SelectionStart, end = start + SelectionLength;
            bool shouldFollow = follow != null && follow.Checked;
            updating = true; SendValue(Handle, SetRedraw, IntPtr.Zero, IntPtr.Zero);
            try
            {
                AppendText(text);
                int removed = TextLength > 220000 ? TextLength - 170000 : 0;
                if (removed > 0)
                {
                    // Account for the actual RichEdit line height when old text expires.
                    int height = GetPositionFromCharIndex(removed).Y - GetPositionFromCharIndex(0).Y;
                    Select(0, removed); SelectedText = "";
                    position.Y = Math.Max(0, position.Y - height);
                }
                if (shouldFollow) { Select(TextLength, 0); ScrollToCaret(); }
                else
                {
                    start = Math.Min(TextLength, Math.Max(0, start - removed));
                    end = Math.Min(TextLength, Math.Max(start, end - removed));
                    Select(start, end - start);
                    SendPoint(Handle, SetScrollPosition, IntPtr.Zero, ref position);
                }
            }
            finally
            {
                SendValue(Handle, SetRedraw, new IntPtr(1), IntPtr.Zero);
                updating = false; Invalidate();
            }
        }
    }
}
