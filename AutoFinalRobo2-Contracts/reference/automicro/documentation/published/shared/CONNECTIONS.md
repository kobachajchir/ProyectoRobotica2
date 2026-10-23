# Conexiones e impacto entre documentaciones

Este índice conecta módulo, mensaje, pantalla, archivo y hallazgo mediante IDs estables. Sus relaciones de fuente son candidatas; no acreditan ejecución ni compatibilidad.

## Recorridos compartidos

| Productor | Consumidor | Relación y datos |
| --- | --- | --- |
| [`web:module/src/protocol/wsApi.ts`](../web/libraries/websocket-client.md) | [`esp:module/src/ws_json_bridge.h`](../esp/libraries/json-bridge.md) | wire-json-request: requestId/command/args API1; respuesta/error correlacionado completa la Promise, queued solo acepta cola. |
| [`esp:module/src/ws_json_bridge.h`](../esp/libraries/json-bridge.md) | [`esp:module/src/uart_transaction_manager.h`](../esp/libraries/uart-transactions.md) | in-process-request: Copia argumentos binarios a UtmRequest, conserva client/generation/node y política de finalización. |
| [`esp:module/src/uart_transaction_manager.h`](../esp/libraries/uart-transactions.md) | [`f4:module/Core/Inc/uner_app.h`](../f4/libraries/uner-application.md) | wire-uner-uart: Sin requestId en wire; ruta/command y ACK/NACK enlazan con la transacción ESP. |
| [`f4:module/Core/Inc/menusystem.h`](../f4/libraries/menus.md) | [`esp:module/src/ws_json_bridge.h`](../esp/libraries/json-bridge.md) | wire-screen-report: 0x52/0x95/0x96 código/source y extensiones; faltan ventana, buffer OSK y snapshot general. |
| [`esp:module/src/ws_json_bridge.h`](../esp/libraries/json-bridge.md) | [`web:module/src/contexts/ScreenContext.tsx`](../web/libraries/screen-state.md) | wire-screen-json: Decodifica enteros LE y publica screenCode/source/selección; conservar presencia de campos (ESP-002). |
| [`web:module/src/contexts/ScreenContext.tsx`](../web/libraries/screen-state.md) | [`web:module/src/screens/screenRenderRegistry.ts`](../web/libraries/screen-resolver.md) | render-model: Normalización de identidad/datos alimenta builder candidato; no garantiza estado completo ni píxeles. |
| [`web:module/src/screens/screenRenderRegistry.ts`](../web/libraries/screen-resolver.md) | [`web:module/src/components/OledCommandPreview.tsx`](../web/libraries/oled-renderer.md) | draw-commands: OledCommand[] termina en SVG actual; objetivo de port completo F4 pendiente. |
| [`web:module/src/components/ScreenStreamWorkspace.tsx`](../web/libraries/screen-controls.md) | [`f4:module/Core/Inc/ui_event_router.h`](../f4/libraries/ui-router.md) | remote-logical-input: Comandos 0x53..0x58 atraviesan ESP/UNER, comparan screenCode y pasan por prioridades F4. |
| [`f4:module/Core/Inc/ssd1306.h`](../f4/libraries/ssd1306.md) | [`web:module/src/components/OledCommandPreview.tsx`](../web/libraries/oled-renderer.md) | port-target: Fuente canónica de raster/recursos F4; adaptación Web actual no acredita copia completa. |
| [`web:module/src/features/oledEditor/useOledCanvasTransfer.ts`](../web/libraries/oled-transfer.md) | [`esp:module/src/oled_canvas_transfer.h`](../esp/libraries/oled-canvas.md) | wire-oled-upload: Begin/chunk/status/commit transaccional; sentido Web→F4, distinto del visor F4→Web. |
| [`esp:module/src/oled_canvas_transfer.h`](../esp/libraries/oled-canvas.md) | [`f4:module/Core/Inc/oled_canvas.h`](../f4/libraries/oled-canvas.md) | wire-oled-frame: 1024 bytes para 128×64 monocromo con contrato de páginas/CRC definido por protocolo, no imagen SVG. |
| [`f4:module/Core/Inc/ssd1306.h`](../f4/libraries/ssd1306.md) | [`f4:module/Core/Inc/i2c_manager.h`](../f4/libraries/i2c-manager.md) | runtime-bus-port: OLED solicita servicio de bus; MPU es otro cliente, sin dependencia OLED→MPU necesaria. |
| [`esp:module/src/stream_broker.h`](../esp/libraries/streams.md) | [`esp:module/src/ws_session_manager.h`](../esp/libraries/sessions.md) | lifetime-session: Demanda de streams ligada a client/generation, limpieza de sesión y estado upstream compartido. |
| [`web:module/src/features/docs/docsContent.ts`](../web/libraries/documentation-ui.md) | [`shared:documentation/amdoc-1`](../../METHOD.md) | documentation-consumer: Contenido Markdown/JSON e IDs por destino F4/ESP/Web; integración UI/selector diferida. |
| [`web:module/src/contexts/UserContext.tsx`](../web/libraries/authentication.md) | [`shared:message/auth-pin/v1`](contracts/auth-pin-v1.md) | uses-shared-message-contract: Productor JSON de PIN/scopes y consumidor de concesión/TTL. |
| [`esp:module/src/ws_json_bridge.h`](../esp/libraries/json-bridge.md) | [`shared:message/auth-pin/v1`](contracts/auth-pin-v1.md) | uses-shared-message-contract: Serializa dígitos, correlaciona respuesta y aplica autoridad por generación. |
| [`f4:module/Core/Inc/permissions.h`](../f4/libraries/permissions.md) | [`shared:message/auth-pin/v1`](contracts/auth-pin-v1.md) | uses-shared-message-contract: Dueño del permiso y validación; edición física no es snapshot JSON. |
| [`web:module/src/features/oledEditor/useOledCanvasTransfer.ts`](../web/libraries/oled-transfer.md) | [`shared:message/oled-canvas/v1`](contracts/oled-canvas-v1.md) | uses-shared-message-contract: Upload JSON, chunks512 y validación del resultado rendered. |
| [`esp:module/src/oled_canvas_transfer.h`](../esp/libraries/oled-canvas.md) | [`shared:message/oled-canvas/v1`](contracts/oled-canvas-v1.md) | uses-shared-message-contract: Ensamblado1024, CRC32 y fragmentos UART112. |
| [`f4:module/Core/Inc/oled_canvas.h`](../f4/libraries/oled-canvas.md) | [`shared:message/oled-canvas/v1`](contracts/oled-canvas-v1.md) | uses-shared-message-contract: Recepción/commit de imagen en pantalla de servicio, modo TEST. |

## Uso para una corrección local

1. Buscar archivo o ID con `documentation_impact.py`; la consulta solo lee `connections.json`.
2. Abrir las fichas/secciones indicadas y resolver el cambio de código en su repositorio dueño.
3. Editar overlays/contratos afectados, conservar la historia del hallazgo y regenerar solo las unidades indicadas.
4. Publicar el registro con `render_findings.py` y actualizar este índice con `build_connections.py`. No hace falta releer todos los repositorios.

```text
python documentation/scripts/documentation_impact.py --finding AMDOC-WEB-004
python documentation/scripts/documentation_impact.py --file web:src/protocol/screenMenuState.ts
```

## Archivos publicados

- [Modelo de conexiones y búsqueda inversa](connections.json).
- [Contrato binario y JSON](wire-contracts.md).
- [Modelo, acciones y límites de pantallas](SCREEN_SYSTEM.md).
- [Fichas por código](SCREEN_CODE_REGISTER.md).
- [Registro de hallazgos](../../review/FINDINGS.md).

Cada registro incluye fuente relativa, ID y selector; las líneas son ayudas de lectura. La ausencia en el índice se conserva como desconocida. Los consumidores Qt siguen pendientes de una revisión interna equivalente.
