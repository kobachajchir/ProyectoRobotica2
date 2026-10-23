# Cliente WebSocket y estado

[Maestro](../../../INFORME_MAESTRO.md) · [Informe del repositorio](../../INFORME_ADAPTACION.md).

## Utilidad

Reutilizar promesas por requestId, respuesta/error/evento, cancelación al desconectar y estados de conexión. Contextos capturan APIs históricas y simulación.

## Adaptación

Crear cliente de producto/capabilities nuevo; no mandar stm.* antes del port ESP/Mega. Separar aceptación y final. Unknown/stale no se rellena con datos de demo. Adaptar dueño de mando/lease, reconexión y permisos. No inferir compatibilidad del catálogo TypeScript.

## Evidencia y alcance

- [reference/web/src/protocol/espClient.ts](../../reference/web/src/protocol/espClient.ts)
- [reference/web/src/protocol/wsApi.ts](../../reference/web/src/protocol/wsApi.ts)
- [reference/web/src/contexts/WebSocketContext.tsx](../../reference/web/src/contexts/WebSocketContext.tsx)

Hallazgos: AFRDOC-008. [Registro](../../../AutoFinalRobo2-Contracts/documentation/review/FINDINGS.md).

Estado: lectura/copia o fuente parcial indicada; sin compilación, tests ni prueba física. La API detallada de origen está en el corpus AMDOC importado; no equivale a API AVR adaptada.
