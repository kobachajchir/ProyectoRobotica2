// AFR2-20261027-24. MCP connection evidence and setup UI, by Koba Chajchir.
using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Web.Script.Serialization;
using System.Windows.Forms;

namespace AFR2Loader
{
    public sealed partial class MainForm
    {
        sealed class McpPeer
        {
            public string Name;
            public DateTime SeenUtc;
        }
        readonly Dictionary<string, McpPeer> mcpPeers = new Dictionary<string, McpPeer>();
        readonly Label mcpServerState = new Label { AutoSize = true, Dock = DockStyle.Top, Font = new Font("Segoe UI", 11, FontStyle.Bold) };
        readonly Label mcpTestResult = new Label { AutoSize = true, Dock = DockStyle.Top, UseMnemonic = false };
        readonly TextBox mcpTestCommand = new TextBox { Dock = DockStyle.Top, Multiline = true, ReadOnly = true, ScrollBars = ScrollBars.Vertical, BackColor = Color.White, Height = 88, TabStop = false };
        TableLayoutPanel mcpTestPanel;
        string pendingMcpTestId, completedMcpTestId, verifiedMcpClientId;
        DateTime mcpTestDeadlineUtc, mcpVerifiedUtc;
        bool mcpTestExpired;
        int mcpConnectionTick;

        readonly Label mcpConnectorState = new Label { AutoSize = true, Dock = DockStyle.Top, Font = new Font("Segoe UI", 12, FontStyle.Bold) };
        Control CreateMcpServerPanel()
        {
            var panel = new TableLayoutPanel { Dock = DockStyle.Top, AutoSize = true, ColumnCount = 1, RowCount = 6, Padding = new Padding(UiInset), Margin = new Padding(0, 0, 0, 16), BackColor = Color.White };
            panel.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100)); for (int i = 0; i < 6; i++) panel.RowStyles.Add(new RowStyle(SizeType.AutoSize));
            panel.Controls.Add(SectionLabel("2 · Comprobar conexión"), 0, 0);
            panel.Controls.Add(mcpConnectorState, 0, 1); panel.Controls.Add(mcpServerState, 0, 2);
            panel.Controls.Add(new Label { Text = "Transporte: stdio · Canal local de Windows: " + LocalBridge.PipeName, AutoSize = true, MaximumSize = new Size(990, 0), Margin = new Padding(0, 8, 0, 8) }, 0, 3);
            var actions = UiRow(); var test = new ActionButton { Text = "Ejecutar prueba", Width = 185 }; test.Click += (s, e) => StartMcpConnectionTest(); actions.Controls.Add(test);
            mcpTestResult.Dock = DockStyle.None; mcpTestResult.MaximumSize = new Size(700, 0); mcpTestResult.Margin = new Padding(16, 9, 0, 0); actions.Controls.Add(mcpTestResult); panel.Controls.Add(actions, 0, 4);
            mcpTestPanel = new TableLayoutPanel { Dock = DockStyle.Top, AutoSize = true, ColumnCount = 1, RowCount = 3, Visible = false, Margin = new Padding(0, 8, 0, 0) };
            mcpTestPanel.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100)); for (int i = 0; i < 3; i++) mcpTestPanel.RowStyles.Add(new RowStyle(SizeType.AutoSize));
            mcpTestPanel.Controls.Add(new Label { Text = "Instrucción de prueba para Codex", AutoSize = true }, 0, 0); mcpTestPanel.Controls.Add(mcpTestCommand, 0, 1);
            var copy = new ActionButton { Text = "Copiar instrucción de prueba", Width = 280 }; copy.Click += (s, e) => CopyText(mcpTestCommand.Text, copy); mcpTestPanel.Controls.Add(copy, 0, 2);
            mcpTestCommand.ForeColor = Color.Firebrick; mcpTestCommand.BackColor = Color.FromArgb(255, 240, 241);
            EnableTextCopyFeedback(mcpTestCommand); panel.Controls.Add(mcpTestPanel, 0, 5);
            timer.Tick += (s, e) => { if (++mcpConnectionTick % 10 == 0) UpdateMcpServerState(); }; UpdateMcpServerState(); return panel;
        }
        Control CreateMcpRegistrationPanel()
        {
            // AFR2-20261106-06: MCP page order = configure client, check connection, requests.
            var root = new TableLayoutPanel { Dock = DockStyle.Top, AutoSize = true, ColumnCount = 1, RowCount = 3, Margin = new Padding(0, 0, 0, 16) };
            root.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100)); root.RowStyles.Add(new RowStyle(SizeType.AutoSize)); root.RowStyles.Add(new RowStyle(SizeType.AutoSize)); root.RowStyles.Add(new RowStyle(SizeType.AutoSize));
            root.Controls.Add(SectionLabel("1 · Configurar cliente"), 0, 0);
            var toggle = new ActionButton { Text = "Mostrar prompt agregar MCP", Width = 340 };
            var panel = new TableLayoutPanel { Dock = DockStyle.Top, AutoSize = true, ColumnCount = 1, RowCount = 4, Visible = false, Padding = new Padding(UiInset), BackColor = Color.White };
            panel.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100)); for (int i = 0; i < 4; i++) panel.RowStyles.Add(new RowStyle(SizeType.AutoSize));
            panel.Controls.Add(new Label { Text = "Prompt para añadir el servidor MCP en cualquier IA", AutoSize = true, Font = new Font(Font, FontStyle.Bold) }, 0, 0);
            string configuration = new JavaScriptSerializer().Serialize(new { mcpServers = new Dictionary<string, object> { { "afr2-loader", new { command = "powershell.exe", args = new[] { "-NoProfile", "-NonInteractive", "-ExecutionPolicy", "Bypass", "-File", Path.Combine(appDir, "Mcp.ps1") }, env = new Dictionary<string, string> { { "AFR2_LOADER_DATA_DIR", Profile.UserRoot } } } } } });
            string command = "Configurá en tu cliente de IA este servidor MCP local de Windows. Nombre: afr2-loader. Transporte: stdio (sin URL HTTP ni credenciales). Usá el siguiente comando y argumentos, adaptando solo el formato de configuración a tu cliente:\r\n\r\n" + configuration + "\r\n\r\nLa ventana AFR2 Loader debe estar abierta en la misma PC y usuario. El servidor comparte esa ventana mediante un canal local. Registrarlo no demuestra conexión: reconectá el cliente y pedime el prompt generado por Ejecutar prueba. No abras COM, no compiles firmware, no flashees ni actives autoaceptación para configurar o comprobar la conexión. Si tu cliente no admite servidores stdio locales, explicá esa limitación y no inventes una URL.";
            var text = new TextBox { Dock = DockStyle.Top, Text = command, ForeColor = Color.Firebrick, ReadOnly = true, Multiline = true, BackColor = Color.FromArgb(255, 240, 241), Height = 210, ScrollBars = ScrollBars.Vertical, TabStop = false };
            panel.Controls.Add(text, 0, 1); EnableTextCopyFeedback(text);
            var copy = new ActionButton { Text = "Copiar prompt", Width = 185 }; copy.Click += (s, e) => CopyText(command, copy); panel.Controls.Add(copy, 0, 2);
            panel.Controls.Add(new Label { Text = "Pegá este prompt en la IA que quieras configurar. Después reconectá el servidor y usá Ejecutar prueba para generar un prompt de conexión de esta sesión.", AutoSize = true, MaximumSize = new Size(950, 0) }, 0, 3);
            toggle.Click += (s, e) => { panel.Visible = !panel.Visible; toggle.Text = panel.Visible ? "Ocultar prompt agregar MCP" : "Mostrar prompt agregar MCP"; };
            toggle.Role = ButtonRole.Secondary;
            root.Controls.Add(toggle, 0, 1); root.Controls.Add(panel, 0, 2); return root;
        }
        void StartMcpConnectionTest()
        {
            pendingMcpTestId = Guid.NewGuid().ToString("N");
            mcpTestDeadlineUtc = DateTime.UtcNow.AddMinutes(5); mcpTestExpired = false;
            string arguments = new JavaScriptSerializer().Serialize(new { sessionId = sessionId, testId = pendingMcpTestId });
            mcpTestCommand.Text = "En el servidor MCP afr2-loader, ejecutá loader_connection_test con estos argumentos: " + arguments + ". Informá el resultado que devuelva. Esta prueba solo comprueba la conexión con la ventana; no abras puertos ni solicites operaciones. Si la herramienta no aparece, reconectá el servidor MCP en Codex.";
            mcpTestCommand.Select(0, 0); mcpTestPanel.Visible = true; ShowMcpTestPrompt();
            UpdateMcpServerState();
            Log("Prueba de conexión MCP preparada; esperando la instrucción desde Codex.");
        }
        void ShowMcpTestPrompt()
        {
            using (var dialog = new Form { Text = "Prompt para probar la conexión en Codex", Size = new Size(800, 430), Font = Font, StartPosition = FormStartPosition.CenterParent }) {
                var layout = new TableLayoutPanel { Dock = DockStyle.Fill, Padding = new Padding(16), ColumnCount = 1, RowCount = 3 };
                layout.RowStyles.Add(new RowStyle(SizeType.Absolute, 46)); layout.RowStyles.Add(new RowStyle(SizeType.Percent, 100)); layout.RowStyles.Add(new RowStyle(SizeType.Absolute, 48));
                layout.Controls.Add(new Label { Text = "Copiá este prompt en Codex. Usa la sesión y el identificador de prueba actuales; vence en cinco minutos.", Dock = DockStyle.Fill }, 0, 0);
                var text = new TextBox { Text = mcpTestCommand.Text, ForeColor = Color.Firebrick, BackColor = Color.FromArgb(255, 240, 241), ReadOnly = true, Multiline = true, ScrollBars = ScrollBars.Vertical, Dock = DockStyle.Fill };
                EnableTextCopyFeedback(text); layout.Controls.Add(text, 0, 1);
                var copy = new ActionButton { Text = "Copiar prompt de prueba", Width = 270, Anchor = AnchorStyles.Right }; copy.Click += (s, e) => CopyText(text.Text, copy); layout.Controls.Add(copy, 0, 2);
                dialog.Controls.Add(layout); ShowStyledDialog(dialog);
            }
        }

        string ObserveMcpClient(Dictionary<string, object> request)
        {
            object value; if (!request.TryGetValue("client", out value)) return null;
            var client = value as Dictionary<string, object>;
            Guid parsed; string id = client == null ? "" : Arg(client, "id", "");
            if (!Guid.TryParseExact(id, "N", out parsed)) throw new ArgumentException("Identidad de cliente MCP inválida.");
            string name = Arg(client, "name", "Cliente MCP");
            if (name.Length > 80) name = name.Substring(0, 80);
            name = name.Replace('\r', ' ').Replace('\n', ' ');
            if (!mcpPeers.ContainsKey(id) && mcpPeers.Count >= 8)
                mcpPeers.Remove(mcpPeers.OrderBy(p => p.Value.SeenUtc).First().Key);
            mcpPeers[id] = new McpPeer { Name = name, SeenUtc = DateTime.UtcNow };
            UpdateMcpServerState(); return id;
        }

        void ForgetMcpClient(string id)
        {
            if (id != null) mcpPeers.Remove(id);
            UpdateMcpServerState();
        }

        bool VerifiedMcpClientAlive()
        {
            McpPeer peer;
            return verifiedMcpClientId != null && mcpPeers.TryGetValue(verifiedMcpClientId, out peer)
                && (DateTime.UtcNow - peer.SeenUtc).TotalSeconds <= 30;
        }

        object CompleteMcpConnectionTest(Dictionary<string, object> args, string clientId)
        {
            if (clientId == null) throw new ArgumentException("Reconectá el servidor MCP actualizado para ejecutar la prueba.");
            if (Arg(args, "sessionId", "") != sessionId) throw new ArgumentException("La prueba corresponde a otra ventana. Pulsá Ejecutar prueba y copiá la instrucción actual.");
            string id = Arg(args, "testId", "");
            bool repeated = pendingMcpTestId == null && !mcpTestExpired && id == completedMcpTestId && clientId == verifiedMcpClientId;
            if (!repeated)
            {
                if (pendingMcpTestId == null || id != pendingMcpTestId) throw new ArgumentException("La prueba no es la actual. Pulsá Ejecutar prueba y copiá la nueva instrucción.");
                if (DateTime.UtcNow > mcpTestDeadlineUtc) { UpdateMcpServerState(); throw new ArgumentException("La prueba venció. Pulsá Ejecutar prueba para generar otra."); }
                completedMcpTestId = pendingMcpTestId; pendingMcpTestId = null;
                verifiedMcpClientId = clientId; mcpVerifiedUtc = DateTime.UtcNow; mcpTestExpired = false;
                Log("Prueba de conexión MCP correcta. Servidor conectado a " + mcpPeers[clientId].Name + ".");
            }
            UpdateMcpServerState();
            return new { ok = true, status = "connected", sessionId = sessionId, testId = completedMcpTestId, version = LoaderVersion.Value,
                transport = "stdio", localEndpoint = LocalBridge.PipeName, networkEndpoint = (string)null,
                verifiedAt = mcpVerifiedUtc.ToString("o"), opensPort = false, executesOperation = false, duplicate = repeated };
        }

        object McpConnectionSnapshot()
        {
            UpdateMcpServerState();
            return new { connected = VerifiedMcpClientAlive(), transport = "stdio", localEndpoint = LocalBridge.PipeName, networkEndpoint = (string)null,
                testPending = pendingMcpTestId != null, testExpired = mcpTestExpired,
                verifiedAt = verifiedMcpClientId == null ? null : mcpVerifiedUtc.ToString("o"), display = mcpServerState.Text };
        }

        void UpdateMcpServerState()
        {
            if (pendingMcpTestId != null && DateTime.UtcNow > mcpTestDeadlineUtc) { pendingMcpTestId = null; mcpTestExpired = true; }
            bool connected = VerifiedMcpClientAlive();
            mcpConnectorState.Text = bridge != null ? "Conector MCP activado" : "Conector MCP iniciando";
            mcpConnectorState.ForeColor = bridge != null ? Color.FromArgb(24, 107, 66) : Color.FromArgb(87, 99, 112);
            if (pendingMcpTestId != null)
            {
                mcpServerState.Text = "Esperando prueba desde Codex"; mcpServerState.ForeColor = Color.FromArgb(140, 83, 0);
                mcpTestResult.Text = "Prueba pendiente. Pegá la instrucción en Codex antes de " + mcpTestDeadlineUtc.ToLocalTime().ToString("HH:mm:ss") + ".";
            }
            else if (connected)
            {
                mcpServerState.Text = "Conexión comprobada"; mcpServerState.ForeColor = Color.FromArgb(24, 107, 66);
                mcpTestResult.Text = (mcpTestExpired ? "Nueva prueba vencida sin respuesta. " : "") + "Última prueba correcta: " + mcpVerifiedUtc.ToLocalTime().ToString("dd/MM/yyyy HH:mm:ss") + " · " + mcpPeers[verifiedMcpClientId].Name;
            }
            else if (verifiedMcpClientId != null)
            {
                mcpServerState.Text = "Conexión cerrada / cliente sin respuesta"; mcpServerState.ForeColor = Color.FromArgb(164, 40, 40);
                mcpTestResult.Text = "El cliente cerró o dejó de responder. Última prueba correcta: " + mcpVerifiedUtc.ToLocalTime().ToString("HH:mm:ss") + ". Reconectá el servidor y ejecutá otra prueba.";
            }
            else
            {
                bool detected = mcpPeers.Values.Any(p => (DateTime.UtcNow - p.SeenUtc).TotalSeconds <= 30);
                mcpServerState.Text = detected ? "Cliente detectado · conexión sin comprobar" : "Conexión sin comprobar";
                mcpServerState.ForeColor = Color.FromArgb(87, 99, 112);
                mcpTestResult.Text = mcpTestExpired ? "No se recibió una respuesta válida a tiempo. Ejecutá otra prueba; comprobá que Codex haya reconectado este servidor." : "Ejecutá una prueba desde Codex para comprobar la conexión con esta ventana.";
            }
        }
    }
}
