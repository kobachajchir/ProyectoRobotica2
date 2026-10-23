# AMI-20261022-01 — Arranque ESP con aviso diferido y recuperacion I2C/UART F4

- Fecha: 2026-10-22 (America/Buenos_Aires).
- Repositorio de origen: FirmwareF4.
- Referencia de codigo: fuentes locales modificadas sobre a51319bf4135589c5b25cc42141abdb174f615cb; ese HEAD no representa la implementacion completa de esta ficha.
- Estado del origen: implementado en fuentes; sin compilacion, tests, validaciones ejecutadas ni flasheo.
- Area: flujo REBOOT_ESP / notificacion OLED / transporte UNER / recuperacion interna.

## Contrato anterior y nuevo

La F4 envia el reinicio ESP automatico al completar su inicializacion, antes de detectar AT/WEB. Esto ya estaba implementado. El timeout UNER de su ACK es de 50 ms y ya existia una gracia de recuperacion de 10000 ms para la perdida de alive. Antes, el NACK/timeout del comando automatico podia encolar Fallo conexion / a ESP inmediatamente, evitando esa gracia.

Ahora la solicitud automatica lleva una marca interna en F4, sin bytes nuevos. Su NACK/timeout durante la gracia libera la transaccion y registra el fallo para la evaluacion final. Al finalizar la gracia, si el enlace AT o WEB esta conectado, no muestra ese fallo temprano; si sigue desconectado y hubo fallo o perdida de enlace, solicita el aviso de 2000 ms segun el mute y disponibilidad de la cola existentes. Si la gracia ya termino cuando se procesa el error, conserva el camino inmediato. Los reinicios manuales/proxy no llevan la marca y conservan sus respuestas y errores.

Se conservan REBOOT_ESP 0x16, request boot_mode:u8 (0 normal, 1 AP), origen MCU/nodo 1, destino ESP/nodo 3 y UART1 UNER v2. Se conserva ACK/NACK con ID original 0x16 y status; ACK no acredita recuperacion posterior del enlace. Se conserva el timeout de ACK de 50 ms, sin nueva respuesta final ni reenvio automatico por falta de ACK. El reintento de transmision ocupada sigue cada 50 ms hasta aceptar el envio una vez por arranque STM.

El aviso conserva screenCode 0x060302 y el renderer Fallo conexion / a ESP. Los reportes 0x95 SCREEN_CHANGED conservan screenCode:u32 LE, source:u8=3, remainingMs:u16 LE, totalMs:u16 LE (nueve bytes para este aviso; totalMs=2000). No agrega clientNumber ni estado remoto de recuperacion I2C. Los controles remotos y permisos conservan sus formatos.

Las correcciones internas adicionales no cambian el wire: callbacks I2C diferidos al bucle principal, recuperacion hardware antes de limpiar buffers y arbitrar otro propietario, comprobacion de fallos de inicio DMA OLED/MPU, redibujo tras error y buffer TX UART propio de UNER_OVERHEAD+255 bytes. El menu se inicializa despues del bridge y antes de instalar el aviso inicial, para conservar su handler de cierre por encoder.

## Flujo completo

1. Tras inicializar los subsistemas STM, F4 solicita REBOOT_ESP normal sin esperar el backend. BUSY conserva el reintento de envio de 50 ms. Aceptar DMA inicia la gracia de 10 s y la espera de ACK existente.
2. ACK del ESP libera la transaccion. Un NACK o timeout del reinicio automatico durante esa gracia tambien libera la transaccion, pero su aviso queda diferido. No se genera una respuesta de exito sintetica.
3. Los probes AT/WEB y la revocacion de sesiones por perdida de enlace continuan. Al finalizar la gracia, la tarea de alive evalua el enlace: recuperado descarta el fallo temprano; desconectado con fallo observado solicita la notificacion.
4. La cola OLED muestra el mismo aviso y progreso durante 2000 ms cuando su politica lo permite; al vencer o cerrar con encoder restaura el contexto anterior mediante los callbacks existentes. No se agrega una accion remota ni payload de recuperacion.
5. Un reinicio manual o proxy sigue el flujo anterior de ACK/NACK/timeout y permisos. No se modifica autorizacion ni concesion de control.
6. I2C solo publica eventos desde IRQ. La tarea principal despacha callbacks y recuperacion; el lease interno de 120 ms se renueva con cada finalizacion. Un bus ocupado sin propietario y con solicitudes pendientes tambien pasa por recuperacion acotada. Se reutilizan pinout, valores I2C y MSP de CubeMX; NVIC no cambia.
7. El transporte UART conserva una copia de la trama hasta completar DMA. Un segundo envio que devuelve BUSY no modifica esa copia.

## Impacto en repositorios

| Repositorio | Estado | Accion o motivo concreto |
| --- | --- | --- |
| FirmwareF4 | origen | Codigo de arranque, manager/puerto I2C, adaptador HAL, drivers OLED/MPU y transporte UART modificados; documentacion local actualizada. |
| Firmware ESP01 | compatible | send_reboot_ack_and_flush en src/uner_app_handlers.cpp conserva ACK 0x16; src/ws_bridge/reports.inc admite screenCode y progreso u16 existentes. No se exige un mensaje nuevo ni se cambia el tiempo de ACK del productor. |
| Web ESP01 | compatible | src/screens/screenRenderRegistry.ts mantiene el renderer de SCREEN_CODE_DIAG_ESP_CONN_FAILED y progreso a partir de remainingMs/totalMs. Solo cambia cuando F4 publica ese aviso automatico. |
| AutoMicroQtDesktop | compatible | src/protocol/CommandCatalog.cpp conserva REBOOT_ESP con timeout 50 ms; src/services/F4ApiCodec.cpp decodifica screen.changed con el mismo screenCode y progreso. El proxy manual no usa la marca automatica. |

## Compatibilidad y orden de integracion

La actualizacion funcional se instala en F4; no se requieren cambios de formato en ESP, Web ni Qt. Esos repositorios solo se leyeron. Compatible describe la inspeccion de los formatos y caminos indicados, no una prueba de interoperabilidad ni despliegue sincronizado. Los pendientes de otras fichas permanecen.

## Evidencia y limites

Fuentes F4: Core/Src/app_tasks.c (APP_Init, i2cManager_MainTask, APP_DeferEspStartupRebootFailure, APP_ESPAlive_Task y APP_ESPRebootAfterStmStartup_Task); Core/Src/uner_app.c (marca de origen y caminos de NACK/timeout); Core/Src/i2c_manager.c; Core/Inc/app_i2c_ports.h; Core/Src/ssd1306.c; Core/Src/mpu9250.c; Core/Src/uner_transport_uart1_dma.c y sus headers.

Documentacion local: Docs/ESP_STARTUP_FREEZE_INVESTIGATION_20261022.md, Docs/f4_esp_web_protocol.md y CHANGELOG.md. El informe distingue el aviso temprano de la causa aun no confirmada del congelamiento.

No se ejecutaron compilaciones, tests, validaciones, flasheo, depuracion ni prueba fisica. La transferencia diferida cambia el momento de despacho de los callbacks I2C; latencia y funcionamiento en placa pendientes. Un cortocircuito o dispositivo que retiene SDA/SCL no queda reparado por reiniciar el periferico. Los nuevos artefactos binarios no se generaron; las fuentes F4 permanecen sin commit/push en esta tarea.

## Actualizaciones posteriores

- 2026-10-22: implementacion y documentacion del cambio preparadas por pedido expreso del usuario; publicacion de esta ficha e indice en el remoto privado de Contracts como paso de cierre.