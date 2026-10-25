# AFR2-20261023-11 — Servidor ESP8266, LittleFS y enlace ATmega2560

Fecha: 23/10/2026. Origen: pedido directo del usuario de revisar primero Web/plan/ESP01 e implementar ESP e interacción con Mega. Se leyó la referencia conservada de AutoMicro/ESP01, sin escribir en AutoMicro ni ejecutar sus scripts. Estado: fuente implementada, sin compilación ni ejecución en hardware.

## Antes y después

Antes: Web React/Vite ya implementada, paquete AFR2-20261023-09 en ESP/data/dist, ESP con Serial/yield, sin servicios de red; AVR con biblioteca OLED/UI parcial y sin UART activa.

Después: composición Arduino ESP8266 con HTTP :80, WebSocket `/ws`, montaje LittleFS sin formato implícito, servido del paquete por manifiesto, diagnóstico `/api/health`, preferencias propias y servicio Wi-Fi STA/AP/AP_STA con escaneo asíncrono/fallback. Broker afr2-ws-1 con sesiones/colas acotadas, negociación UART y traducción del catálogo mínimo. AVR incorpora parser/endpoint, USART1 por IRQ y composición opcional de comunicaciones con Timer0 de 1 ms y salidas de potencia inactivas. No se implementa movimiento como efecto de conectarse a la Web.

## Productores y consumidores

- **Web↔ESP:** se conserva `afr2-ws-1`. Se implementan system.hello/alive, wifi.status/scan/configure y errores explícitos para servicios pendientes. Evento `system.status` informa enlace/capacidades cada 1 s. La fuente Web consume esas actualizaciones y exige stopped=true/status=completed para confirmar STOP.
- **ESP↔Mega:** [perfil AFR2 1](../contracts/UART_PROFILE_1.md), constantes/parser compartidos en [afr2_wire.h](../contracts/afr2_wire.h). IDs 0x01 hello, 0x02 alive, 0x03 state, 0x04 sensors, 0x05 encoders, 0x06 STOP. Prefijo sesión/transacción u32 LE; resultado y longitudes exactas. Sin compatibilidad con stm.*, genéricos E0/E1 ni catálogo F4.
- **Productores pendientes:** sensores, potencia, control por rueda, lease, Hall/odometría, GOTO y UI remota. El endpoint permite conectar callbacks de snapshot/STOP y declara solo servicios efectivamente integrados. La composición actual declara UART; sensores unknown y STOP CAPABILITY_UNAVAILABLE. No se presume vehículo parado físicamente por un ACK de transporte.
- **HTTP/LittleFS:** paquete `afr2-littlefs-manifest-1`, ruta host data/dist, ruta flash /dist. Hasta 16 archivos, manifest <=8192 bytes. Esquema, rutas exactas, MIME y tamaños revisados por el firmware; SHA de contenido no recalculado. Solo representaciones gzip y GET. `/config` no se sirve. No hay API de edición/upload/formato.

## Secuencia y límites

Cola ESP común de 4 pedidos, STOP separado, 2 sesiones WS de hasta 1024 bytes y 4 mensajes de salida por cliente. Se acepta segmentación TCP de un frame de texto final; continuaciones WS/binary no soportadas. Origin restringido al Host servido, sin bypass de desarrollo. Un consumidor lento se cierra. WS vence a 6000 ms sin petición válida. Las promesas Web se resuelven por requestId; los eventos no las completan.

UART 115200 8N1 candidato, AVR U2X/UBRR=16 a 16 MHz, +2,12 % nominal sin medición. N<=64. TX no intercala frames y conserva buffers ante escrituras parciales. Sesión+transacción+comando evitan correlación de respuestas tardías. Timeout UART 800 ms, alive 500 ms, renegociación tras 1000 ms, supervisión AVR 1500 ms, intervalo parser 50 ms. STOP sustituye la lectura correlacionada y espera como máximo terminar la trama ya empezada; jamás informa completed sin respuesta autoritativa positiva.

Wi-Fi: sin preferencias válidas inicia AP abierto `AutoFinalRobo2-<chipId>`. Configurar AP usa ssid/password del AP; STA/AP_STA los usa para la estación. AP_STA/fallback tienen identidad de aprovisionamiento propia. La configuración se guarda por defecto solo en la flash privada del ESP (`persist=false` permite RAM), con temporal+rename, y se aplica 350 ms después de accepted. Guardar/aplicar no acredita conexión STA: consultar wifi.status. Scan entrega hasta 16 redes y estado completed/failed; un timeout no se confunde con cancelación del SDK. STA añade fallback a los 15 s y reintenta cada 30 s. Sin contraseñas en fuentes/logs/respuestas.

## Estado por módulo

| Módulo | Estado real de esta ficha | Pendiente |
| --- | --- | --- |
| atmega | Endpoint C y backend UART1 escritos; composición opcional de comunicaciones; OLED/UI anteriores conservados | Compilar/mapa SRAM/pila, validar protocolo/timeout, integrar aplicación y backend TWI; banco autorizado |
| esp | Servidor, storage, Wi-Fi y broker escritos; versiones de dependencias fijadas; adaptación del parche ESP01 preservada | Compilar, verificar APIs/memoria, identificar flash/SPI/layout real, crear imagen y cargar cuando se autorice |
| web | Dos ajustes de integración en fuente: capacidades dinámicas y resultado STOP | Recompilar, empaquetar/promover; data/dist sigue siendo el paquete anterior |
| shared | Wire perfil 1, catálogo/capabilities/plan, procedencia por archivo y documentación actualizados | Verificar interoperabilidad cuando se pidan pruebas |

## Alcance de la entrega y evidencia

Se hicieron lecturas, edición de fuente/documentación, lectura de estado/diff Git e inventario SHA-256 de seis archivos de referencia para procedencia. No son tests ni validación de interoperabilidad. **No se ejecutaron tests, compilaciones, validadores, scripts de dependencias, servidores de desarrollo, imágenes LittleFS, puertos serie, flasheos ni pruebas físicas.** No se modificaron referencias, documentos originales ni repositorios AutoMicro. No se crearon Git anidados ni se publicaron commits.

El paquete existente declara 2.925.405 bytes gzip de contenido: no incluye overhead LittleFS ni preferencias. El perfil esp12e no detecta la flash real; su layout por defecto puede no alojarlo. No se fija linker ni offset AT/ESP01. Una futura carga de imagen FS completa puede borrar preferencias y requiere preservar configuración y coordinar el layout con el firmware.

[Plan ESP y continuación](../docs/PLAN_ESP.md) · [Procedencia incremental por archivo](../provenance/esp-firmware.json) · [Protocolo](../contracts/PROTOCOL.md).
