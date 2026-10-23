# AMI-20261005-04 — dos WebSockets y Qt USB simultaneos

- Fecha: 2026-10-05 (America/Buenos_Aires).
- Repositorios de origen: Firmware ESP01 y FirmwareF4.
- Referencia de codigo: cambios locales sin commit de los cuatro consumidores.
- Estado del origen: implementado en fuentes; sin nuevas compilaciones ni carga.
- Area: sesiones WebSocket, autenticacion PIN, rutas y suscripciones UNER.

## Contrato anterior y nuevo

ESP limitaba el tracking y bridge a una conexion; HTTP/assets y `/ws` reservaban
una unica IP y un unico cliente. F4 conservaba un solo destino global por stream
MPU/IR, por lo que SET de USB podia reemplazar a UART y STOP detenia a ambos.
Un boot ESP revocaba todas las sesiones remotas, incluida Qt USB.

El limite solicitado es **dos sesiones WebSocket como maximo**, definido una
sola vez en `Firmware ESP01/FirmESP01MicroArduino2025/src/ws_limits.h` como
`WS_MAX_SESSIONS=2u`. Tracking, bridge, session manager y stream broker utilizan
ese limite. Una tercera conexion recibe error `busy` y se cierra; no se expulsa
a las existentes. HTTP/assets ya no reservan una IP ni otorgan autoridad PIN.
Qt USB no consume una plaza WebSocket.

JSON API v1 y `/ws` conservan formato y ruta. `hello` sigue siendo evento
automatico; anuncia `data.maxClients=2`, `features.independentSessions=true` y
el campo opcional `data.resumeToken:string` de 32 caracteres hexadecimales.
El token se genera con cuatro palabras aleatorias del SDK y solo identifica
una conexion. Web/Qt lo conservan en memoria, asociado al endpoint, y envian
query `resumeToken` al reconectar. No se guarda ni transmite el PIN para
recuperar sesiones. Se requiere token coincidente, slot suspendido y gracia
vigente; se asigna generation nueva. Una IP compartida no transfiere autoridad.
El helper historico `wsm_resume_by_ip` queda disponible, pero no se usa en el
recorrido de conexion del servidor.

Autenticacion conserva `AUTH_VALIDATE_PIN 0x51` (6/10 bytes; respuesta de 9) y
`AUTH_REMOTE_RESULT 0x5D` (request de 1 byte: logout `0` o query `1`; respuesta
de 6: `[status:u8, authenticated:u8, ttlMs:u32 LE]`). F4 mantiene TTL y bloqueo
por nodo. La consulta renueva solo una sesion F4 aun vigente. ESP requiere que
esa conexion ya tenga su login STM vigente para confirmar/renovar el grant;
no concede login por encontrar un grant antiguo en un nodo reutilizado. En
ese caso devuelve `authenticated=false, ttlMs=0`. Web/Qt consultan su propia
sesion cada 20 s mientras esta autenticada. Logout afecta solo a su nodo;
cambio de PIN y reset F4 conservan sus revocaciones globales. El limite de
intentos ESP sigue en cinco por conexion/60 s; el agregado por IP permite diez
para que ambas conexiones compartan IP sin compartir su cuota individual.

SET/STOP conserva IDs y payloads:

| Operacion | Request | Respuesta final |
| --- | --- | --- |
| SET_MPU_STREAM `0x61`, SET_IR_STREAM `0x6B` | `enabled:u8` o `enabled:u8, periodMs:u16 LE`; se conserva la aceptacion F4 historica de 2 bytes | `[status:u8, active:u8, periodMs:u16 LE]` (4 bytes) |
| STOP_MPU_STREAM `0x62`, STOP_IR_STREAM `0x6C` | vacio | `[status:u8]` (1 byte) |

F4 conserva diez slots acotados por stream, identificados por transporte y
nodo. SET actualiza solo ese endpoint; SET `enabled=0` y STOP eliminan solo
ese endpoint. STOP propio se permite despues de logout/TTL para limpieza;
no concede datos ni capacidad para detener otro endpoint. Falta de capacidad
produce NACK `busy`. MPU `0x90` mantiene 42 bytes; IR `0x91` mantiene 56.
Periodos y seguimiento de secuencia son propios por endpoint; una muestra MPU
se construye una vez y se distribuye a los endpoints cuyo periodo vencio.
Cada envio exitoso renueva solo permisos aun vigentes del destino. Logout,
expiracion o perdida de transporte cancelan exclusivamente sus suscripciones.

GET_TELEMETRY_SNAPSHOT `0x64` conserva 92 bytes: `mpuStreamActive` pasa a indicar
si existe cualquier suscriptor; `mpuStreamPeriodMs` indica el menor periodo
activo, o el defecto si no hay suscriptores. GET_IR_SNAPSHOT `0x6A` conserva 56
bytes, usa periodo propio si existe o defecto, y secuencia de snapshots separada.

ESP agrega la demanda MPU/IR Wi-Fi y envia datos solo a conexiones autenticadas
y suscritas. Si cambia su owner F4, confirma STOP del endpoint anterior y despues
SET del nuevo para no dejar suscripciones residuales. El STOP Wi-Fi no afecta
USB. Un boot ESP cancela UART y revoca solo nodos Web `0x05..0x0C`; Qt USB sigue
vigente. Los servicios de streams no se pausan por una transaccion de control
ESP pendiente. No se agrega una exclusividad de manejo: ambas interfaces pueden
enviar comandos, con guardas existentes y aplicacion en orden recibido.

## Flujo completo

1. Web y Qt por Wi-Fi abren conexiones independientes a ESP `/ws`. Cada una
   recibe hello, nodo y generation propios y completa login PIN con F4 por UART.
   Qt USB usa UNER directo, nodo PC_QT `0x02`, y su propio login.
2. ESP serializa operaciones F4 en su transaction manager. La respuesta se
   correlaciona con cliente, generation, nodo, comando y requestId; los eventos
   de autenticacion se envian solo al cliente afectado. Consultar una sesion
   ajena no autoriza la conexion nueva.
3. subscribe/unsubscribe conserva requestAccepted cuando requiere trabajo F4,
   waiter por cliente/generation/stream y respuesta final tras confirmacion de
   SET/STOP. Si no requiere trabajo, responde inmediatamente. El plazo del
   waiter sigue en 5 s; se preservan timeout, cola ocupada, NACK y error UART.
4. F4 publica MPU/IR por cada endpoint suscrito; ESP reparte su stream agregado
   entre los clientes autenticados. Las respuestas siguen privadas y los eventos
   de estado comunes conservan el filtrado de autenticacion existente.
5. Logout/expiry/disconnect remueven solo demanda propia. Transferencia de owner
   Wi-Fi usa STOP y SET secuenciales. Reinicio ESP deja USB operativo; reset F4 o
   cambio de PIN exige recuperar las autorizaciones que corresponda.

## Impacto en repositorios

| Repositorio | Estado | Accion concreta |
| --- | --- | --- |
| FirmwareF4 | origen | `Core/Src/uner_app.c`: slots por endpoint, stop/cleanup propios, autorizacion de streams USB/UART, TTL propio y boot ESP acotado a Web. `Docs/concurrent-interfaces-20261005.md`. |
| Firmware ESP01 | adaptado | Limite comun dos; reserva exclusiva eliminada; reconexion por token; consulta de sesion propia; broker con STOP/SET al transferir owner y cuotas por conexion/IP. Contrato WS actualizado. |
| Web ESP01 | adaptado | `wsApi.ts`, `WebSocketContext.tsx` y `UserContext.tsx`: token en memoria por provider/URL y consulta de sesion propia cada 20 s. Documento de simultaneidad. |
| AutoMicroQtDesktop | adaptado | `WebSocketSemanticChannel.*` y `ProtocolControllers.*`: token por endpoint, consulta de sesion propia, invalidacion de auth por evento y logout local inmediato. Documento de simultaneidad. |

## Compatibilidad y orden de integracion

No cambian IDs, largos ni endian de SET/STOP, auth o muestras. Cambian alcance
de STOP, metadata agregada de telemetria, semantica de refresh/query y campos
aditivos de hello. Consumidores viejos pueden conectar y hacer login; sin
token tienen que autenticarse otra vez tras perder el socket. No se promete
simultaneidad con firmware F4 viejo, cuyo stream seguia siendo unico.

Desplegar F4 y ESP actualizados junto con la Web servida y Qt. Requiere compilar
y cargar ambos firmwares, reconstruir/cargar el bundle Web y recompilar Qt.
Esos pasos quedan pendientes por instruccion del usuario.

## Evidencia y limites

Cambios de codigo y documentacion aplicados en los cuatro checkouts, preservando
el trabajo previo de F4/Web/Qt y el modelo de data ESP. Sin tests, compilaciones,
validaciones, ejecucion visual, carga en placa ni prueba fisica. No se registran
numeros de RAM, ancho de banda ni estabilidad de dos clientes como comprobados.

Los cuatro repositorios consumidores quedan con cambios locales sin commit ni
push de esta tarea. Solo el registro central se commitea y publica; no implica
que los ejecutables o firmwares actualmente instalados ya tengan el cambio.

## Actualizaciones posteriores

- 2026-10-05: implementacion en fuentes y registro inicial; builds/carga/prueba
  de ambos casos (dos Wi-Fi y USB mas Wi-Fi) pendientes.
