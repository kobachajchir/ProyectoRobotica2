# AMI-20261004-01 — AP fallback configurable y reintento Wi-Fi

- Fecha: 2026-10-04 (America/Buenos_Aires)
- Repositorios de origen: FirmwareF4 y Firmware ESP01
- Referencia de código: cambios locales pendientes
- Estado del origen: implementado en fuentes locales; sin compilación ni flasheo. Contrato registrado antes de editar firmware.
- Área: preferencia persistente / UNER / navegación Wi-Fi

## Contrato anterior y nuevo

Antes: la FSM ESP AP+STA iniciaba el AP automáticamente al vencer el intento STA o perder su enlace. F4 no disponía de una preferencia para evitarlo.

Nuevo: `wifiApFallbackEnabled` en F4, espejo `apFallbackEnabled` en ESP. Activada por defecto y al migrar registros existentes para preservar el comportamiento previo. Desactivada: un fallo STA no crea un AP y F4 permanece/regresa al menú Wi-Fi con una acción de reintento. El AP iniciado explícitamente sigue permitido.

- Nuevo comando **0x1B SET_AP_FALLBACK**, antes reservado: request F4->ESP de 1 byte `enabled:u8` (solo 0/1). Respuesta ESP->F4 del mismo ID: `status:u8, enabled:u8`, 2 bytes; status 0 aplicado/persistido, 1 payload inválido, 3 fallo de almacenamiento. ACK exitoso después de persistir; no sustituye esta respuesta. Longitud inválida recibe NACK de argumentos. El ajuste solo se confirma después de guardar; F4 conserva su preferencia y reintenta la sincronización si falta la respuesta o falla.
- F4: máscara de preferencia bit 24 (`0x01000000`), bool. Se usa un bit libre (bit 6, inverso: fallback desactivado) en flags de los registros Flash; no crece el registro V11 de 188 bytes. Snapshot legacy 0x40 de 37 bytes conservado. Descriptor bool nuevo agregado al final, sin renumerar índices previos. El patch 0x63 admite el nuevo campo.
- Reintento: reutiliza **0x48 CONNECT_WIFI**, request de 0 bytes y respuestas existentes. Se usan las credenciales guardadas en ESP. Fallo/timeout se informa con status no exitoso del mismo ID; éxito conserva finalizador `FE, ipv4[4]` y NETWORK_IP. No se cambian 0x4E auto-reconnect ni los comandos de inicio manual AP.
- Nuevas identidades de acciones de menú: `0x05041D` AP fallback y `0x020211` Reintentar Wi-Fi. Los códigos anteriores permanecen.
- ESP firmware **v2.4**, JSON schema 4 conservado: `apFallbackEnabled` es un campo opcional aditivo; su ausencia se migra con valor 1. Un firmware previo puede leer el archivo ignorando el campo, pero no aplicar el ajuste nuevo. No cambia la versión de API WebSocket.

## Flujo completo

1. Usuario abre Configuración -> Preferencias -> Sistema -> AP fallback. El icono usa el círculo activo/inactivo existente. Click guarda la preferencia en F4; fallo de Flash conserva el valor anterior.
2. La tarea F4 sincroniza 0x1B por UART UNER con el nodo ESP, usando la exclusión de comandos vigente. Reintenta sin espera bloqueante hasta recibir status 0 y el valor actual.
3. ESP valida el bool, guarda la configuración mediante las operaciones de almacenamiento inyectadas y responde. Al reiniciar usa el valor persistido, sin depender de que F4 responda durante el arranque.
4. El intento STA mantiene el timeout vigente de 10 segundos. Con fallback activo puede iniciar el AP configurado; con fallback inactivo termina el intento sin crear AP. El fallo usa 0x48 y F4 conserva/regresa al menú Wi-Fi.
5. Click en Reintentar Wi-Fi encola el comando 0x48, después de sincronizar la política y cuando el transporte esté disponible. Sincronización reintenta cada 2 s; tras confirmar, se reconfirma a los 30 s sin reescritura ESP si el valor es idéntico. Éxito/fallo conserva los finalizadores y notificaciones vigentes. No es auto-reconnect periódico.
6. 0x1B usa los caminos proxy y su autorización remota vigente; se registra como ESP admin. Los fallos de sincronización no se presentan como una conexión Wi-Fi fallida ni provocan notificaciones repetidas. No se incluyen credenciales en esta ficha.

## Impacto en repositorios

| Repositorio | Estado | Acción o motivo concreto |
| --- | --- | --- |
| FirmwareF4 | origen | Preferencia, persistencia, círculo, callback de pantalla, tarea de sincronización y reintento implementados localmente. |
| Firmware ESP01 | adaptado | Espejo persistente, callbacks, 0x1B, finalizador de fallo y política FSM implementados; metadata v2.4. |
| Web ESP01 | pendiente | No solicitado modificar este repo. Nuevas acciones/catálogo y representación de preferencia pendientes; comandos previos se conservan. |
| AutoMicroQtDesktop | pendiente | No solicitado modificar este repo. Registro/catálogo de 0x1B y nuevas identidades pendientes; payloads previos se conservan. |

## Compatibilidad y orden de integración

Actualizar ESP a v2.4 y F4 juntos. ESP anterior desconoce 0x1B: no puede aplicar el ajuste nuevo; F4 conserva pendiente la sincronización y no debe darla por confirmada. Sin la F4 nueva, el ESP conserva su política persistida (por defecto activa). No se declaran consumidores remotos adaptados sin modificar sus fuentes.

## Evidencia y límites

Contrato definido antes de editar fuentes. No se ejecutaron compilaciones, tests ni pruebas físicas para esta entrega. Las compilaciones anteriores pertenecen a las versiones previas y no validan este cambio.

## Actualizaciones posteriores

- 2026-10-04: implementación local finalizada en F4 y ESP. Los changelogs de ambos repositorios documentan el cambio. Metadata ESP v2.4, metadata F4 conservada; fuentes de firmware sin commit/push.
- Entradas de menú añadidas después de Volver, al final de las tablas: se conservan todos los índices previos de 0x53. No se añadieron renderers independientes. Se amplía el conteo de ítems con las acciones nuevas.
- Se corrigió en F4 la interpretación de una respuesta status 0 con datos de Wi-Fi: es aceptación del intento, no conexión lograda. Los finalizadores previos de 5 bytes y FE+IPv4 permanecen.
- Preferencia F4 adicional no se agrega al snapshot legacy de 37 bytes ni a su máscara; descriptor bool añadido al final permite el patch nuevo. Consulta remota tipada del valor y representación Web/Qt pendientes.
- El candidato de credenciales Wi-Fi del ESP conserva el fallback vigente al hacer commit; no se persisten credenciales fallidas. Rollback existente preservado.
- Un AP ya activo no se derriba al deshabilitar fallback. El cambio condiciona nuevas creaciones automáticas; inicio AP explícito/boot AP independiente.

Evidencia de implementación: [F4](../../FirmwareF4/Docs/WIFI_AP_FALLBACK_20261004.md), [ESP](../../Firmware%20ESP01/FirmESP01MicroArduino2025/docs/WIFI_AP_FALLBACK_2_4.md). Los artefactos de compilación anteriores siguen preservados y no contienen estas fuentes nuevas.


2026-10-05: adaptación Qt en fuentes y controles AP fallback Web registrados en AMI-20261005-01. Captura local IR/Balance, archivos interoperables y formulario PID; sin protocolo nuevo de ciclos F4 ni ensayo físico. Distancias Qt usan interpolación nativa, sin equivalencia acreditada del solucionador Web completo. Sin build, tests, validaciones ni hardware. La historia de pendientes de sincronización física permanece.
