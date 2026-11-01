// AFR2-20261028-23. Session preparation and explicit capacity confirmation.
using System;
using System.Drawing;
using System.IO;
using System.Windows.Forms;

namespace AFR2Loader
{
    public sealed partial class MainForm
    {
        Control espSectionField;
        string confirmedFlashCapacity;
        Control capacityRow;
        readonly Label capacityState = new Label { AutoSize = true, Margin = new Padding(12, 12, 0, 0) };
        bool CapacityConfirmed { get { return confirmedFlashCapacity == profile.Get("FlashMB"); } }
        int preparationTick;
        bool ConfirmFlashCapacity()
        {
            if (CapacityConfirmed) return true;
            DetectEsp();
            return false; // La consulta es asíncrona: continuar cuando termine correctamente.
        }
        bool SelectedSectionPrepared()
        {
            if (profile == null) return false;
            if (target.SelectedIndex == 0) {
                string image = megaVariant.SelectedIndex == 1 ? Path.Combine(profile.Resolve("AtmegaDir"), "build", "afr2-communication.hex") : profile.Resolve("Hex");
                return BuildPreparation.Ready(profile, "mega", image);
            }
            if (!CapacityConfirmed) return false;
            if (EspFlashRules.IsAt(profile)) return BuildPreparation.Ready(profile, "at", profile.Resolve("AtPackage"));
            return (target.SelectedIndex == 2 || BuildPreparation.Ready(profile, "esp", profile.Resolve("EspBin"))) &&
                (target.SelectedIndex == 1 || BuildPreparation.Ready(profile, "filesystem", profile.Resolve("FsBin")));
        }
        void UpdatePreparationControls()
        {
            if (flashTarget == null) return;
            bool ready = false; try { ready = SelectedSectionPrepared(); } catch (Exception) { }
            flashTarget.Enabled = !busy && pendingJob == null && profileDialogs == 0 && !syncingEspOptions && ready;
            flashTarget.ToolTipText = ready ? "Cargar las imágenes preparadas para esta sección." : "Primero confirmá el tamaño ESP y compilá esta sección (AT: revisar paquete). Cambiar fuentes, perfil o imágenes requiere prepararla otra vez.";
        }
        void ShowKeyReleaseNotice()
        {
            var dialog = new Form { Text = "Ya podés soltar KEY", Size = new Size(570, 245), Font = Font, StartPosition = FormStartPosition.CenterParent, MinimizeBox = false, MaximizeBox = false };
            var text = new Label { Text = "La herramienta informó el primer porcentaje de escritura ESP.\r\n\r\nYa podés soltar KEY. No pulses RST ni desconectes la alimentación: la escritura y su verificación siguen en curso.", Dock = DockStyle.Fill, Padding = new Padding(20) };
            var close = new ActionButton { Text = "Entendido", Dock = DockStyle.Bottom, Height = 44 };
            close.Click += (s, e) => dialog.Close(); dialog.Controls.Add(text); dialog.Controls.Add(close);
            dialog.FormClosed += (s, e) => dialog.Dispose(); StyleControls(dialog); dialog.Show(this);
        }
    }
}

