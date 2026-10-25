> AFR2-20261025-01: [control directo7](../../../AutoFinalRobo2-Contracts/contracts/DIRECT_CONTROL_7.md). Se eliminan todas las operaciones y estados de permiso y la habilitación OLED. Las acciones se envían directamente; solo permanecen conexión/capacidades, límites, estado ocupado, parada/caducidad y calibración. Fuente compilada; runtime pendiente. Estados inferiores históricos.

> AFR2-20261024-15:33comandos conservados; system.info añade filesystem/paquete opcional, presentado en Hardware. Productores afr2-pose-1 ya implementados, válidos solo bajo calibración real. [Contrato](../../../AutoFinalRobo2-Contracts/contracts/SYSTEM_INFO_FILESYSTEM_15.md).

# Cliente WebSocket y estado

Vigente12: EX01v6 añade carga por rueda para distancia; tiempos puros conservan
el formato anterior. Web usa distanceBlocks negociado antes de enviar variantes.
Los gates de memoria/Hall/calibración siguen bloqueando hardware. GOTO local
es independiente de esta API. Compilado sin intercambio físico comprobado.
[Contrato](../../../AutoFinalRobo2-Contracts/contracts/EX01_SEQUENCE_DISTANCE_6.md).

[Maestro](../../../INFORME_MAESTRO.md) · [Informe del repositorio](../../INFORME_ADAPTACION.md).

## Utilidad

Reutilizar promesas por requestId, respuesta/error/evento, cancelación al desconectar y estados de conexión. Contextos capturan APIs históricas y simulación.

## Adaptación

Crear cliente de producto/capabilities nuevo; no mandar stm.* antes del port ESP/Mega. Separar aceptación y final. Unknown/stale no se rellena con datos de demo. Adaptar responsable del control/lease, reconexión y permisos. No inferir compatibilidad del catálogo TypeScript.

## Evidencia y alcance

- [reference/web/src/protocol/espClient.ts](../../reference/web/src/protocol/espClient.ts)
- [reference/web/src/protocol/wsApi.ts](../../reference/web/src/protocol/wsApi.ts)
- [reference/web/src/contexts/WebSocketContext.tsx](../../reference/web/src/contexts/WebSocketContext.tsx)

Hallazgos: AFRDOC-008. [Registro](../../../AutoFinalRobo2-Contracts/documentation/review/FINDINGS.md).

Estado: lectura/copia o fuente parcial indicada; sin compilación, tests ni prueba física. La API detallada de origen está en el corpus AMDOC importado; no equivale a API AVR adaptada.
