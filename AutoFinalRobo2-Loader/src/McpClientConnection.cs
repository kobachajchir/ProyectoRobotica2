// AFR2-20261027-24. Lifetime of one stdio client; no TCP or hardware access.
using System;
using System.Threading;
using System.Web.Script.Serialization;

namespace AFR2Loader
{
    sealed class McpClientConnection : IDisposable
    {
        readonly string id = Guid.NewGuid().ToString("N");
        readonly object gate = new object();
        string name = "Cliente MCP";
        Timer heartbeat;
        bool stopped;

        public void Start(string clientName)
        {
            lock (gate)
            {
                if (heartbeat != null || stopped) return;
                if (!String.IsNullOrWhiteSpace(clientName)) name = clientName.Length <= 80 ? clientName : clientName.Substring(0, 80);
                heartbeat = new Timer(Beat, null, 0, 5000);
            }
        }
        object Send(string command, object arguments)
        {
            return LocalBridge.Call(new JavaScriptSerializer().Serialize(new { command = command, arguments = arguments, client = new { id = id, name = name } }));
        }
        public object Call(string command, object arguments)
        {
            lock (gate)
            {
                if (stopped) throw new InvalidOperationException("El cliente MCP está cerrando.");
                return Send(command, arguments);
            }
        }
        void Beat(object state)
        {
            // A delayed heartbeat is skipped rather than queued behind a tool call.
            if (!Monitor.TryEnter(gate)) return;
            try { if (!stopped) Send("mcp_client_alive", new { }); }
            catch (Exception) { /* GUI absent/closing: the next heartbeat may reconnect. */ }
            finally { Monitor.Exit(gate); }
        }
        public void Dispose()
        {
            lock (gate)
            {
                if (stopped) return;
                stopped = true;
                if (heartbeat != null)
                {
                    heartbeat.Dispose();
                    try { Send("mcp_client_closed", new { }); }
                    catch (Exception) { /* GUI may have closed first. */ }
                }
            }
        }
    }
}
