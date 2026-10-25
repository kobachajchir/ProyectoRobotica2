> AFR2-20261025-01: las reglas anteriores de acceso quedan sustituidas por [control directo EX01v7](DIRECT_CONTROL_7.md). Sin adquisición, renovación, liberación ni habilitación OLED. Los formatos anteriores solo se conservan donde DIRECT_CONTROL_7 mantiene compatibilidad.

# EX01 v4 — Sistema y presentación de secuencias

AFR2-20261024-07. Extiende [EX01 v3](EX01_ALIGNMENT_3.md) sobre ROBO2/perfil2;
no cambia framing, XOR, rutas, sesión, correlación ni límites64bytes. Los campos
no citados conservan sus offsets. Ambos extremos deben actualizarse juntos.

## Secuencias

`SEQ_APPEND 0x41`: cuerpo30bytes. Bytes0..28 conservan propietario/token,
índice, duración y velocidades iniciales/finales. Byte29: ID semántico.

| ID | Tipo Web | Nombre OLED constante | Icono |
| --- | --- | --- | --- |
| 0 | constant | Desplazamiento | Flecha |
| 1 | ramp | Rampa | Rampa |
| 2 | pivot | Giro sobre eje | Giro |
| 3 | curve | Curva | Curva |
| 4 | wait | Espera | Reloj |
| 5 | finish | Finalizacion | Marca final |
| 6 | wheels | Cuatro ruedas | Ruedas |

ASCII OLED por font7x10; la Web conserva acentos. No se copia un nombre libre
del usuario a SRAM AVR. Dos IDs por byte:16bytes para32segmentos, conservando
`sizeof(Afr2Segment)==20`. La finalización0ms no consume un segmento temporizado;
la barra cuenta los segmentos ejecutables. Cuatro pistas avanzadas se normalizan
en segmentos comunes y usan tipo wheels cuando no existe tipo de movimiento.

ESP solo envía secuencias con EX01>=4; no manda30bytes a un AVRv3. El runtime
sigue condicionado por memoria aprobada, Hall, geometría, límites y ausencia de
fallos. No se activó `AFR2_SEQUENCE_MEMORY_APPROVED` ni `AFR2_HALL_CONNECTED`.

Pantalla `0x03002c`, nodo añadido al final del catálogo para no renumerar los
anteriores: activación una vez por ejecución nueva en estado running. Barra en
Y0..6, icono16×16 en Y10..25, movimientoY28, tiempoY42 y VolverY54. Completados
rellenos; pendientes con contorno; bloque activo con relleno parcial temporal.
Cancelada/fallida no se presentan como completadas. El snapshot lo prepara main;
renderer puro, framebuffer/I2C Manager existentes. LargoSW conserva menú y no
reabre la vista automáticamente; Volver es la única vuelta al contexto previo.

## Identidad de compilación — 0x50

Request body1byte: página0..4. Reply body: página[0], bootIdLE[1..4],
causaReset[5], longitud[6], textoASCII[7..], hasta48bytes. La respuesta total
con los9bytes de correlación/resultado nunca excede64bytes.

Páginas: versión, fecha/hora del compilador, compilador, arquitectura/reloj,
revisión. La revisión predeterminada workspace no afirma un commit limpio.
ESP verifica página, longitud e identidad coherente entre las cinco respuestas.
Tras completar publica `system.info`; sin datos vigentes devuelve atmega=null.

`system.info {}` también informa ESP: versión, compilación, compilador, Core,
SDK, causa/identidad de arranque, uptime, reloj, tamaño de programa, heap y flash
detectada al ejecutar la consulta en placa. La fuente compilada no acredita
ninguna medición de una placa real. No se publican claves Wi-Fi.

## Reinicio — 0x51

Web: `system.reboot {target:"esp"|"atmega", confirm:true}`. Requiere una
petición explícita; no se dispara al abrir Ajustes o la página Desarrollador.

- ATmega: cuerpo UART1byte0xa5. Rechaza salidas activas, EEPROM escribiendo y
  un reinicio ya pendiente. Detiene y conserva apagadas las salidas, rechaza
  nuevos mandos, responde accepted y espera al menos250ms y TX USART1 completo
  antes de activar watchdog15ms. No modifica EEPROM/fuses/bootloader.
- ESP: exige estado detenido reciente si hay Mega conectado, sin dueño ni
  parada pendientes. Responde accepted y difiere300ms `ESP.restart()`; no borra
  LittleFS ni configuración. No acepta reinicios anteriores a una revocación.

El ACK solo confirma programación del reinicio. Puede perderse durante una
desconexión; no es garantía de recepción ni evidencia del arranque posterior.
STOP sigue prioritario y no se reanuda ninguna ejecución al reconectar.

## Aviso de arranque — 0x52

Evento UART sin correlación, reconocido antes del handler normal:

| Bytes | Contenido |
| --- | --- |
| 0..3 | BT01 |
| 4 | Nodo emisor del encabezado (Mega o ESP) |
| 5..8 | bootIdLE no nulo |
| 9 | Causa de reset del dispositivo |
| 10..12 | Versión mayor/menor/parche |
| 13 | Versión EX01 |

Ambos dispositivos emiten el evento una vez al arrancar, en su ruta normal
opuesta. No se mezclan logs con UART. Recibir arranque del par invalida control y
negociación; no ejecuta un comando de movimiento ni una reanudación.

ESP bootId usa identidad aleatoria de sesión inicial. AVR conserva contador
en noinit entre resets cuando la cookie es válida y reinicia en POR; no es un
identificador global persistente ni una credencial. CausaMCUSR puede verse
afectada por el bootloader; no se inventa una causa cuando falta evidencia.

WebSocket: `system.boot {device,bootId,resetCauseBits,observation}`.
`uart-boot-event` identifica el aviso recibido; `session-snapshot` e
`identity-snapshot` son identidades retenidas/consultadas. Un snapshot al conectar
no se afirma como reinicio nuevo. Web muestra identidad observada y anuncia
cambios de identidad/avisos vivos; consulta de nuevo la compilación.

## Estado

Atmega/ESP/Web compilados; Web promovida a ESP/data/dist. No se ejecutaron
reinicios, secuencias físicas, flasheos ni tests de interoperabilidad. Revisión
visual bloqueada por denegación del acceso del navegador a localhost.
[Ficha/evidencia](../changes/2026-10-24-07-control-system-sequence.md).
