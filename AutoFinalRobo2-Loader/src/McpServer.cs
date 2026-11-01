using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Web.Script.Serialization;

namespace AFR2Loader
{
    // MCP stdio / JSON-RPC. Each tools/call is a short request; jobs are polled separately.
    public static class McpServer
    {
        static readonly JavaScriptSerializer Json = new JavaScriptSerializer { MaxJsonLength = 1048576, RecursionLimit = 32 };
        static readonly string[] Actions = { "build_mega", "build_esp", "build_web_fs", "build_all", "probe_esp", "flash_mega", "flash_esp", "flash_fs", "flash_esp_fs", "install_mega_flasher", "verify_mega_flasher", "restore_mega_product", "flash_mega_backup" };
        static object Schema(object properties, params string[] required)
        { return new { type = "object", properties = properties, required = required, additionalProperties = false }; }
        static object Tool(string name, string description, object schema, bool readOnly)
        { return new { name = name, description = description, inputSchema = schema, annotations = new { readOnlyHint = readOnly, destructiveHint = !readOnly, openWorldHint = false } }; }
        public static object[] Tools()
        {
            return new[] {
                Tool("loader_status", "Estado del Loader gráfico, conexión MCP comprobada, trabajo activo y autoAcceptRequests de esta sesión. No abre puertos.", Schema(new {}), true),
                Tool("loader_connection_test", "Comprueba la conexión con esta ventana usando sessionId y testId de la instrucción que muestra Ejecutar prueba. Devuelve connected y actualiza el indicador del Loader. Solo conexión: no abre COM ni ejecuta operaciones. Si el ID venció o cambió la ventana, generar otra prueba.",
                    Schema(new { sessionId = new { type = "string" }, testId = new { type = "string" } }, "sessionId", "testId"), true),
                Tool("loader_ports", "Enumera puertos presentes sin abrirlos.", Schema(new {}), true),
                Tool("loader_profile", "Perfil visible, familia ESP Web/AT, memoria y rutas resueltas. No ejecuta herramientas.", Schema(new {}), true),
                Tool("loader_serial", "Estado y texto incremental de la última captura serie de la ventana. No abre ni cierra el puerto. Usar received.nextCursor y captureId=id para continuar en la misma captura.",
                    Schema(new { captureId = new { type = "string" }, cursor = new { type = "integer", minimum = 0 }, limit = new { type = "integer", minimum = 1, maximum = 120 } }), true),
                Tool("loader_request_profile", "Solicita cambios de perfil. La ventana los revisa o los aplica y guarda automáticamente si el usuario activó Autoaceptar. Consultar el jobId devuelto.",
                    Schema(new { values = new { type = "object", additionalProperties = new { type = "string" } }, requestId = new { type = "string", description = "ID único para deduplicar reintentos" } }, "values", "requestId"), false),
                Tool("loader_request_session_options", "Solicita omitir o restablecer confirmaciones manuales durante esta sesión mediante skipManualChecks. Activar exige SIEMPRE aceptar localmente la advertencia en la GUI, incluso con Autoaceptar; el servidor no puede aceptarla. Devuelve jobId: consultar loader_job y loader_status.skipManualChecks. No persiste al reiniciar ni omite comprobaciones automáticas. La intervención KEY/RST del ESP SIEMPRE requiere confirmación local y aparece al frente. Desactivar no requiere advertencia.",
                    Schema(new { skipManualChecks = new { type = "boolean" }, requestId = new { type = "string" } }, "skipManualChecks", "requestId"), false),
                Tool("loader_request_operation", "Solicita una operación con el perfil visible y devuelve jobId. ESP Web usa Arduino/LittleFS; ESP AT revisa/carga el paquete del proveedor por USB. build_all omite Web en AT y filesystem desactivado. Consultar loader_profile. Autoaceptar permite inicio automático; awaiting_approval espera revisión. accepted no significa ejecutada ni verificada. Mantiene asistentes físicos. No repetir si existe el job.",
                    Schema(new { action = new { type = "string", @enum = Actions }, port = new { type = "string", description = "COM para diagnóstico/carga; no necesario para builds" }, listenBoot = new { type = "boolean", description = "Escuchar arranque ESP a 74880, default true; false NO omite intervención física local KEY/RST. Puente software usa baud fijo." }, requestId = new { type = "string", description = "ID único para deduplicar reintentos" } }, "action", "requestId"), false),
                Tool("loader_job", "Estado y lote incremental del registro. Usar nextCursor para continuar; no espera ni ejecuta procesos.",
                    Schema(new { jobId = new { type = "string" }, cursor = new { type = "integer", minimum = 0 }, limit = new { type = "integer", minimum = 1, maximum = 120 } }, "jobId"), true),
                Tool("loader_request_cancel", "Solicita cancelar un trabajo pendiente o activo. Autoaceptar permite pedir la interrupción sin revisión adicional; en modo manual la ventana confirma. Consultar loader_job para el resultado; no provoca reintento ni borrado.",
                    Schema(new { jobId = new { type = "string" } }, "jobId"), false)
            };
        }
        static Dictionary<string, object> Object(object value)
        { var result = value as Dictionary<string, object>; if (result == null) throw new ArgumentException("Se esperaba un objeto JSON."); return result; }
        static string StringValue(Dictionary<string, object> value, string key)
        { object v; if (!value.TryGetValue(key, out v) || !(v is string)) throw new ArgumentException("Falta texto: " + key); return (string)v; }
        static void Validate(string tool, Dictionary<string, object> args)
        {
            string[] keys;
            switch (tool)
            {
                case "loader_status": case "loader_ports": case "loader_profile": keys = new string[0]; break;
                case "loader_connection_test":
                    keys = new[] { "sessionId", "testId" };
                    foreach (string key in keys)
                    {
                        Guid parsed;
                        if (!Guid.TryParseExact(StringValue(args, key), "N", out parsed)) throw new ArgumentException(key + " debe ser el ID de la instrucción del Loader.");
                    }
                    break;
                case "loader_request_profile":
                    keys = new[] { "values", "requestId" }; StringValue(args, "requestId");
                    Object(args["values"]); break;
                case "loader_request_session_options":
                    keys = new[] { "skipManualChecks", "requestId" }; StringValue(args, "requestId");
                    if (!args.ContainsKey("skipManualChecks") || !(args["skipManualChecks"] is bool)) throw new ArgumentException("skipManualChecks debe ser booleano.");
                    break;
                case "loader_serial":
                    keys = new[] { "captureId", "cursor", "limit" };
                    if (args.ContainsKey("captureId") && !(args["captureId"] is string)) throw new ArgumentException("captureId debe ser texto.");
                    foreach (string key in new[] { "cursor", "limit" }) if (args.ContainsKey(key) && !(args[key] is int)) throw new ArgumentException(key + " debe ser entero.");
                    break;
                case "loader_request_operation":
                    keys = new[] { "action", "port", "listenBoot", "requestId" }; StringValue(args, "requestId");
                    if (!Actions.Contains(StringValue(args, "action"))) throw new ArgumentException("Acción no admitida.");
                    if (args.ContainsKey("port") && !(args["port"] is string)) throw new ArgumentException("port debe ser texto.");
                    if (args.ContainsKey("listenBoot") && !(args["listenBoot"] is bool)) throw new ArgumentException("listenBoot debe ser booleano.");
                    break;
                case "loader_job":
                    keys = new[] { "jobId", "cursor", "limit" }; StringValue(args, "jobId");
                    foreach (string key in new[] { "cursor", "limit" }) if (args.ContainsKey(key) && !(args[key] is int)) throw new ArgumentException(key + " debe ser entero.");
                    break;
                case "loader_request_cancel": keys = new[] { "jobId" }; StringValue(args, "jobId"); break;
                default: throw new ArgumentException("Herramienta desconocida.");
            }
            if (args.Keys.Any(k => !keys.Contains(k))) throw new ArgumentException("Argumentos no admitidos.");
        }
        static object ToolCall(Dictionary<string, object> parameters, McpClientConnection client)
        {
            try
            {
                string name = StringValue(parameters, "name");
                var args = parameters.ContainsKey("arguments") ? Object(parameters["arguments"]) : new Dictionary<string, object>();
                Validate(name, args);
                var bridge = Object(client.Call(name, args));
                bool ok = bridge.ContainsKey("ok") && (bool)bridge["ok"];
                object data = bridge.ContainsKey("data") ? bridge["data"] : bridge["error"];
                return new { content = new[] { new { type = "text", text = Json.Serialize(data) } }, isError = !ok };
            }
            catch (Exception error) { return new { content = new[] { new { type = "text", text = error.GetBaseException().Message } }, isError = true }; }
        }
        public static void Run(string appDir)
        {
            bool initialized = false;
            using (var client = new McpClientConnection())
            using (var input = new StreamReader(Console.OpenStandardInput(), new UTF8Encoding(false), false, 4096))
            using (var output = new StreamWriter(Console.OpenStandardOutput(), new UTF8Encoding(false), 4096) { AutoFlush = true })
            {
                string line;
                while ((line = input.ReadLine()) != null)
                {
                    object id = null; bool respond = true;
                    try
                    {
                        if (line.Length > 1048576) throw new ArgumentException("Mensaje demasiado grande.");
                        var request = Object(Json.DeserializeObject(line));
                        respond = request.TryGetValue("id", out id);
                        if (!respond) continue;
                        if (!request.ContainsKey("jsonrpc") || Convert.ToString(request["jsonrpc"]) != "2.0") throw new ArgumentException("Se requiere JSON-RPC 2.0.");
                        string method = StringValue(request, "method");
                        var parameters = request.ContainsKey("params") ? Object(request["params"]) : new Dictionary<string, object>();
                        object result;
                        if (method == "initialize")
                        {
                            string requested = StringValue(parameters, "protocolVersion");
                            string protocol = new[] { "2024-11-05", "2025-03-26", "2025-06-18" }.Contains(requested) ? requested : "2025-06-18";
                            initialized = true;
                            object clientInfo;
                            var info = parameters.TryGetValue("clientInfo", out clientInfo) ? clientInfo as Dictionary<string, object> : null;
                            object clientName;
                            client.Start(info != null && info.TryGetValue("name", out clientName) ? Convert.ToString(clientName) : "Cliente MCP");
                            result = new { protocolVersion = protocol, serverInfo = new { name = "afr2-loader", version = LoaderVersion.Value },
                                capabilities = new { tools = new { listChanged = false }, resources = new { } },
                                instructions = "Use loader_status antes de operar y consulte autoAcceptRequests. Las solicitudes devuelven jobId: awaiting_approval espera revisión manual; accepted indica aceptación automática habilitada por el usuario en la ventana, no ejecución ni verificación. Consultar loader_job con cursor; no reenviar solicitudes de escritura. Los asistentes físicos siguen activos. boot mode 1 no demuestra ida/vuelta. No hay herramienta de comandos arbitrarios." };
                        }
                        else if (method == "ping") result = new { };
                        else
                        {
                            if (!initialized) throw new InvalidOperationException("Inicializar MCP primero.");
                            switch (method)
                            {
                                case "tools/list": result = new { tools = Tools() }; break;
                                case "tools/call": result = ToolCall(parameters, client); break;
                                case "resources/list": result = new { resources = new[] { new { uri = "loader://manual", name = "Manual AFR2 Loader", description = "Manual local, incluyendo cableado y estados de verificación", mimeType = "text/html" } } }; break;
                                case "resources/read":
                                    string uri = StringValue(parameters, "uri");
                                    if (uri != "loader://manual") throw new ArgumentException("Recurso desconocido.");
                                    result = new { contents = new[] { new { uri = uri, mimeType = "text/html", text = File.ReadAllText(Path.Combine(appDir, "MANUAL.html")) } } }; break;
                                default:
                                    output.WriteLine(Json.Serialize(new { jsonrpc = "2.0", id = id, error = new { code = -32601, message = "Método no disponible" } })); continue;
                            }
                        }
                        output.WriteLine(Json.Serialize(new { jsonrpc = "2.0", id = id, result = result }));
                    }
                    catch (Exception error)
                    {
                        if (respond) output.WriteLine(Json.Serialize(new { jsonrpc = "2.0", id = id, error = new { code = id == null ? -32700 : -32602, message = error.GetBaseException().Message } }));
                        Console.Error.WriteLine("AFR2 MCP: " + error.GetBaseException().Message);
                    }
                }
            }
        }
    }
}

