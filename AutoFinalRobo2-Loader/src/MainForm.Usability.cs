// AFR2-20261027-21. UI feedback and read-only dialog focus, by Koba Chajchir.
using System;
using System.Drawing;
using System.Linq;
using System.Runtime.InteropServices;
using System.Windows.Forms;

namespace AFR2Loader
{
    public sealed partial class MainForm
    {
        void CopyText(string text, Control source)
        {
            if (String.IsNullOrEmpty(text)) return;
            CopyData(new DataObject(DataFormats.UnicodeText, text), source);
        }
        void CopyData(IDataObject data, Control source)
        {
            if (data == null) return;
            Form owner = source == null ? this : source.FindForm() ?? this;
            try { Clipboard.SetDataObject(data, true, 3, 100); ClipboardToast.Display(owner, "Copiado al portapapeles");
                if (source == serialText || source == serialConvert) { string copied = data.GetData(DataFormats.UnicodeText) as string; if (!String.IsNullOrEmpty(copied)) RememberCopiedSerial(copied); } }
            catch (ExternalException) { ClipboardToast.Display(owner, "No se pudo copiar. Volvé a intentarlo."); }
        }
        void EnableTextCopyFeedback(TextBoxBase text)
        {
            text.KeyDown += (s, e) => {
                if (e.Control && (e.KeyCode == Keys.C || e.KeyCode == Keys.Insert) && text.SelectionLength > 0)
                { e.Handled = e.SuppressKeyPress = true; CopyText(text.SelectedText, text); }
            };
        }
        static void FocusDialogAction(Form dialog, Control action, params TextBoxBase[] texts)
        {
            foreach (TextBoxBase text in texts) text.TabStop = false;
            dialog.Shown += (s, e) => {
                foreach (TextBoxBase text in texts) text.Select(0, 0);
                dialog.ActiveControl = action; action.Focus();
            };
        }
    }

    // A child panel stays inside its owner and never steals focus from a selection.
    sealed class ClipboardToast : Panel
    {
        readonly Form owner;
        readonly Label message = new Label { AutoSize = true, ForeColor = Color.White, BackColor = Color.Transparent };
        readonly Timer lifetime = new Timer { Interval = 2400 };
        ClipboardToast(Form owner)
        {
            this.owner = owner;
            BackColor = Color.FromArgb(23, 39, 51); Padding = new Padding(18, 12, 18, 12);
            Font = owner.Font; TabStop = false; Visible = false;
            Controls.Add(message); owner.Controls.Add(this);
            lifetime.Tick += (s, e) => { lifetime.Stop(); Visible = false; };
            owner.Resize += Reposition;
        }
        public static void Display(Form owner, string text)
        {
            var toast = owner.Controls.OfType<ClipboardToast>().FirstOrDefault() ?? new ClipboardToast(owner);
            toast.message.Text = text;
            toast.Size = new Size(Math.Min(owner.ClientSize.Width - 24, toast.message.PreferredWidth + toast.Padding.Horizontal), toast.message.PreferredHeight + toast.Padding.Vertical);
            toast.message.Location = new Point(toast.Padding.Left, toast.Padding.Top);
            toast.Reposition(null, EventArgs.Empty); toast.Visible = true; toast.BringToFront();
            toast.lifetime.Stop(); toast.lifetime.Start();
        }
        void Reposition(object sender, EventArgs e)
        {
            var bar = owner.Controls.OfType<StatusStrip>().FirstOrDefault();
            int bottom = bar == null ? owner.ClientSize.Height : bar.Top;
            Location = new Point(Math.Max(12, (owner.ClientSize.Width - Width) / 2), Math.Max(12, bottom - Height - 12));
        }
        protected override void Dispose(bool disposing)
        {
            if (disposing) { lifetime.Dispose(); owner.Resize -= Reposition; }
            base.Dispose(disposing);
        }
    }
}
