# Enlace Mega ↔ ESP: ROBO2

AFR2-20261023-15. Fuente C/avr-libc, sin compilar/probar. Sustituye el framing
UNER/perfil 1 de AFR2-20261023-11; no implementa compatibilidad automática.

Contrato normativo: [UART_PROFILE_2.md](../../AutoFinalRobo2-Contracts/contracts/UART_PROFILE_2.md).
Casos y criterios compartidos: [ROBO2_VALIDACION.md](../../AutoFinalRobo2-Contracts/contracts/ROBO2_VALIDACION.md).
Documento del otro extremo: [ESP/docs/PROTOCOLO_ROBO2.md](../../AutoFinalRobo2-ESP/docs/PROTOCOLO_ROBO2.md).

## Qué recibe Mega y cómo lo valida

`ROBO2 | N | 3A | A5 | 01 | ROUTE | CMD | payload[N] | XOR`, total 12+N,
N<=64. Parser/builder únicos en Contracts/contracts/afr2_wire.h, incluidos
por ambos firmwares. Ambos verifican los cinco bytes, N, ambos tokens, versión,
XOR y ruta. Emisor en nibble alto y destinatario en bajo, como F4. Mega recibe
**0x31** (ESP=3→Mega=1) y envía **0x13**; comprueba ambos nibbles al despachar.
La UART es USART1 D18/D19, 115200 8N1 candidato (error nominal +2,12 %).

Primeros ocho bytes: sesión u32 LE y transacción u32 LE, no cero. Hello 0x01:
N=13, `AFR2` y perfil=2. Solo saluda tras producto/perfil/longitud correctos.
En una sesión dada no acepta transacciones repetidas ni anteriores; no se
re-ejecutan acciones duplicadas. Nuevo hello válido puede cambiar de sesión,
revocando autoridad previa y solicitando STOP si hay hook. El resto del catálogo
0x02..0x06 requiere N=8 y sesión negociada. Desconocido con N=8/sesión válida:
CAPABILITY_UNAVAILABLE; tamaño incorrecto: BAD_REQUEST; sin sesión: SESSION_REQUIRED.
Ruta ajena o prefijo incompleto: descartar sin respuesta correlacionada.

Respuesta conserva CMD, sesión y transacción. Result [8], datos desde [9].
N OK: hello23/alive13/state16/sensors22/encoders31/STOP11; error N=9. Tipos,
offsets, rangos y unidades están en el contrato enlazado y la guía ESP. No hay
ACK genérico, JSON, telemetría espontánea ni pedidos Mega→ESP en este perfil.

Gap >50 ms reinicia framing. RX fault revoca negociación, descarta respuesta
todavía no encolada y solicita STOP si existe hook; la cola TX ya publicada
puede terminar, pero ESP solo correlaciona su transacción activa. Timeout de
sesión >1500 ms sin petición válida; ruido, rutas erróneas, comandos desconocidos
y duplicados no renuevan heartbeat. No afirmar parada física por revocar sesión.

## Productores y composición

Afr2LinkOps presta ctx, snapshot coherente y hook de parada autoritativa. Caps
se filtran por callbacks disponibles; UART siempre presente. Sin productores,
mode/stopped/age son unknown, sensores inválidos y STOP responde unavailable.
No anunciar motores, lease, GOTO, UI remota ni Hall por copiar un driver.

`src/main.c`: UART + Timer0 + TWI/Manager/OLED + encoder y cuatro vistas locales.
`src/communication_main.c`: opción de comunicaciones sola. Comparten board.c;
no enlazar los dos main. Ambas composiciones anuncian únicamente UART.
ISR UART solo mueve bytes. Rings RX/TX 128 B, 127 útiles; trama máxima 76 B.
TX copia sin enmascarar IRQ durante todo el frame y publica head al final.
No consumir nuevos bytes con reply_length pendiente; limpiar solo tras Write
exitoso. Un fault impide nuevas lecturas hasta que main vacía la entrada incierta.

No se ejecutaron build/tests/validadores/puertos/flasheos. Pendientes: memoria
real, interoperabilidad, productores, STOP, movimiento y banco autorizado.
