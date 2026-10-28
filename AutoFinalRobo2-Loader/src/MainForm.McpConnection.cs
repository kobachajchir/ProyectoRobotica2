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

        Control CreateMcpServerPanel()
        {
            var panel = new TableLayoutPanel { Dock = DockStyle.Top, AutoSize = true, ColumnCount = 1, RowCount = 6, Margin = new Padding(0, 0, 0, 8) };
            panel.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100));
            for (int i = 0; i < panel.RowCount; i++) panel.RowStyles.Add(new RowStyle(SizeType.AutoSize));
            panel.Controls.Add(new Label { Text = "Estado del servidor MCP", AutoSize = true, Dock = DockStyle.Top, Font = new Font(Font, FontStyle.Bold) }, 0, 0);
            panel.Controls.Add(mcpServerState, 0, 1);
            panel.Controls.Add(new Label { Text = "Transporte: stdio · Canal local de Windows: " + LocalBridge.PipeName, AutoSize = true, Dock = DockStyle.Top, Margin = new Padding(3, 4, 3, 4) }, 0, 2);
            var actions = new FlowLayoutPanel { Dock = DockStyle.Top, AutoSize = true };
            var test = new Button { Text = "Ejecutar prueba", Width = 160, Height = 32 };
            test.Click += (s, e) => StartMcpConnectionTest(); actions.Controls.Add(test); panel.Controls.Add(actions, 0, 3);
            mcpTestPanel = new TableLayoutPanel { Dock = DockStyle.Top, AutoSize = true, ColumnCount = 1, RowCount = 3, Visible = false, Margin = new Padding(0, 4, 0, 8) };
            mcpTestPanel.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100));
            for (int i = 0; i < 3; i++) mcpTestPanel.RowStyles.Add(new RowStyle(SizeType.AutoSize));
            mcpTestPanel.Controls.Add(new Label { Text = "Instrucción para ejecutar en el chat de Codex", AutoSize = true, Dock = DockStyle.Top }, 0, 0);
            mcpTestPanel.Controls.Add(mcpTestCommand, 0, 1);
            var copy = new Button { Text = "Copiar instrucción de prueba", Width = 240, Height = 32 };
            copy.Click += (s, e) => CopyText(mcpTestCommand.Text, copy); mcpTestPanel.Controls.Add(copy, 0, 2);
            EnableTextCopyFeedback(mcpTestCommand); panel.Controls.Add(mcpTestPanel, 0, 4); panel.Controls.Add(mcpTestResult, 0, 5);
            timer.Tick += (s, e) => { if (++mcpConnectionTick % 10 == 0) UpdateMcpServerState(); };
            UpdateMcpServerState(); return panel;
        }

        Control CreateMcpRegistrationPanel()
        {
            var panel = new TableLayoutPanel { Dock = DockStyle.Top, AutoSize = true, ColumnCount = 1, RowCount = 4, Margin = new Padding(0, 0, 0, 8) };
            panel.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100));
            for (int i = 0; i < 4; i++) panel.RowStyles.Add(new RowStyle(SizeType.AutoSize));
            panel.Controls.Add(new Label { Text = "Comando para añadir servidor MCP", AutoSize = true, Dock = DockStyle.Top, Font = new Font(Font, FontStyle.Bold) }, 0, 0);
            string command = "codex mcp add afr2-loader -- powershell.exe -NoProfile -NonInteractive -ExecutionPolicy Bypass -File \"" + Path.Combine(appDir, "Mcp.ps1") + "\"";
            var text = new TextBox { Dock = DockStyle.Top, Text = command, ReadOnly = true, Multiline = true, BackColor = Color.White, Height = 52, ScrollBars = ScrollBars.Vertical, TabStop = false };
            panel.Controls.Add(text, 0, 1); EnableTextCopyFeedback(text);
            var copy = new Button { Text = "Copiar comando para añadir servidor MCP", Width = 335, Height = 32 };
            copy.Click += (s, e) => CopyText(command, copy); panel.Controls.Add(copy, 0, 2);
            panel.Controls.Add(new Label { Text = "Ejecutá este comando en PowerShell y reconectá el servidor en Codex. Luego usá Ejecutar prueba y pegá la instrucción en el chat.", AutoSize = true, Dock = DockStyle.Top }, 0, 3);
            return panel;
        }

        void StartMcpConnectionTest()
        {
            pendingMcpTestId = Guid.NewGuid().ToString("N");
            mcpTestDeadlineUtc = DateTime.UtcNow.AddMinutes(5); mcpTestExpired = false;
            string arguments = new JavaScriptSerializer().Serialize(new { sessionId = sessionId, testId = pendingMcpTestId });
            mcpTestCommand.Text = "En el servidor MCP afr2-loader, ejecutá loader_connection_test con estos argumentos: " + arguments + ". Informá el resultado que devuelva. Esta prueba solo comprueba la conexión con la ventana; no abras puertos ni solicites operaciones. Si la herramienta no aparece, reconectá el servidor MCP en Codex.";
            mcpTestCommand.Select(0, 0); mcpTestPanel.Visible = true;
            UpdateMcpServerState();
            Log("Prueba de conexión MCP preparada; esperando la instrucción desde Codex.");
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
            if (pendingMcpTestId != null)
            {
                mcpServerState.Text = "Esperando prueba desde Codex"; mcpServerState.ForeColor = Color.FromArgb(140, 83, 0);
                mcpTestResult.Text = "Prueba pendiente. Pegá la instrucción en Codex antes de " + mcpTestDeadlineUtc.ToLocalTime().ToString("HH:mm:ss") + ".";
            }
            else if (connected)
            {
                mcpServerState.Text = "Servidor MCP conectado"; mcpServerState.ForeColor = Color.FromArgb(24, 107, 66);
                mcpTestResult.Text = (mcpTestExpired ? "Nueva prueba vencida sin respuesta. " : "") + "Última prueba correcta: " + mcpVerifiedUtc.ToLocalTime().ToString("dd/MM/yyyy HH:mm:ss") + " · " + mcpPeers[verifiedMcpClientId].Name;
            }
            else if (verifiedMcpClientId != null)
            {
                mcpServerState.Text = "Servidor MCP desconectado"; mcpServerState.ForeColor = Color.FromArgb(164, 40, 40);
                mcpTestResult.Text = "El cliente cerró o dejó de responder. Última prueba correcta: " + mcpVerifiedUtc.ToLocalTime().ToString("HH:mm:ss") + ". Reconectá el servidor y ejecutá otra prueba.";
            }
            else
            {
                bool detected = mcpPeers.Values.Any(p => (DateTime.UtcNow - p.SeenUtc).TotalSeconds <= 30);
                mcpServerState.Text = detected ? "Servidor MCP activado · conexión sin comprobar" : "Servidor MCP sin conexión comprobada";
                mcpServerState.ForeColor = Color.FromArgb(87, 99, 112);
                mcpTestResult.Text = mcpTestExpired ? "No se recibió una respuesta válida a tiempo. Ejecutá otra prueba; comprobá que Codex haya reconectado este servidor." : "Ejecutá una prueba desde Codex para comprobar la conexión con esta ventana.";
            }
        }
    }
}
