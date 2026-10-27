# AFR2-20261027-16 — Loader 0.2: MCP y refuerzo de interfaz

27/10/2026. Pedido humano: servidor MCP para conectar el cargador con Codex,
iterar la interfaz y reforzar la app en general. Se conserva su carpeta raíz.

## Comportamiento implementado en fuente

MCP stdio JSON-RPC, inicialización negociada, catálogo de siete herramientas y
manual como recurso. `Mcp.ps1` genera/ejecuta EXE consola separado del WinForms;
stdout reservado al protocolo y diagnósticos por stderr. `RegistrarMcp.ps1` y
plantilla TOML portables: no se ejecuta registro global durante esta entrega.

El servidor usa named pipe con ACL del usuario para consultar la misma GUI.
Una GUI por usuario; una operación activa o pendiente. Consultas de estado,
puertos y perfil; propuestas de build/diagnóstico/carga/cambio de perfil;
consulta incremental de trabajo y solicitud de cancelación. No shell libre.
La propuesta devuelve jobId pendiente, sin iniciar proceso ni abrir COM. Se
aprueba/rechaza en la ventana; mantiene asistentes y verificación existentes.
Las solicitudes se deduplican por requestId en esa sesión y no se reintentan
escrituras automáticamente. Los logs de ejecución siguen en LOCALAPPDATA.

Pestaña Codex / MCP con comando resuelto, propuesta y comparación de perfil;
historial de sesión; filtro de configuración; seguimiento de log opcional;
indicadores separados de arranque UART y respuesta ROM con puerto y hora.
Validación de estructura de perfiles en fuente, tamaños limitados de registros/
peticiones, limpieza de procesos ante errores y preservación de EXE anterior
si falla la compilación local del lanzador. Es comportamiento programado,
no una afirmación de validación ejecutada.

Guía [MCP.md](../../AutoFinalRobo2-Loader/MCP.md),
[manual](../../AutoFinalRobo2-Loader/MANUAL.html),
[módulo](../../AutoFinalRobo2-Loader/README.md).

## Estados por módulo y evidencia

| Módulo | Estado de esta ficha |
| --- | --- |
| Loader | Fuente 0.2.0 y documentación; sin compilar/ejecutar/validar. |
| Contracts | Registro compartido y enlaces. |
| Atmega | Sin cambios por esta ficha; no nueva compilación/carga. |
| ESP | Sin cambios por esta ficha; no nueva compilación/carga. |
| Web | Sin cambios por esta ficha; no build ni promoción. |
| CAD | Sin cambios por esta ficha. |

No se ejecutaron tests, compilaciones, validadores, interfaz, servidor MCP,
registro global, consultas COM ni flasheos. No se acredita conexión Codex por
la presencia del código/configuración. La primera conexión requiere ejecutar
el lanzador/registro y consultar loader_status en el cliente; queda pendiente
de pedido explícito según AGENTS.md. No se creó commit ni se publicó Git.
