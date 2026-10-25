> AFR2-20261025-01: las reglas anteriores de acceso quedan sustituidas por [control directo EX01v7](DIRECT_CONTROL_7.md). Sin adquisición, renovación, liberación ni habilitación OLED. Los formatos anteriores solo se conservan donde DIRECT_CONTROL_7 mantiene compatibilidad.

# Perfil binario AFR2 1

**Histórico, sustituido por [ROBO2/perfil 2](UART_PROFILE_2.md) en AFR2-20261023-15. No usar este framing en firmware vigente.**

AFR2-20261023-11, 23/10/2026. Fuente implementada en ESP y AVR, sin compilar, validar ni probar. No es compatible con el catálogo AutoMicro. Constantes y parser C/C++ únicos en [afr2_wire.h](afr2_wire.h); ambos módulos lo incluyen desde Contracts, sin copiarlo.

## Transporte y correlación

ESP UART0 GPIO1/TX y GPIO3/RX ↔ Mega USART1 D18/TX1 y D19/RX1, por E_RX/E_TX del selector. 115200 8N1 candidato; AVR a 16 MHz usa U2X=1/UBRR=16: 117647 baud, error +2,12 % nominal, sin medición. Respetar [shield](../hardware/WIFI_SHIELD.md) y [carga](../hardware/FLASHING.md).

Frame: `55 4E 45 52 N 3A 02 ROUTE CMD PAYLOAD XOR`. N cuenta solo payload. XOR de todos los bytes anteriores; total 10+N. Ruta petición 0x31, respuesta 0x13. Solo se aceptan esas rutas, versión 2 y hasta 64 bytes de payload; el framing histórico admite 255, este perfil no. Parser drena longitudes mayores sin copiarlas, resincroniza cabecera y reinicia tras intervalo entre bytes >50 ms. IRQ AVR solo guarda bytes; parser/handlers se ejecutan en main. Nada de logs en UART0 ESP ni USART1 Mega. Los bytes ROM ESP son ruido hasta negociar.

Los primeros 8 bytes de todas las peticiones/respuestas son `session:u32` en offset 0 y `transaction:u32` en offset 4, ambos little-endian y distintos de cero. ESP genera sesión aleatoria al arrancar y transacciones crecientes; antes de reutilizar un número renueva sesión y negocia. Mega copia ambos campos en la respuesta. Esto no es autenticación. Una respuesta solo correlaciona si coinciden ruta, comando, sesión, transacción, estado y longitud exacta. Respuestas tardías/duplicadas y ACK genéricos 0xE0/0xE1 no completan pedidos.

Cada respuesta contiene `result:u8` en offset 8: 0 OK, 1 BAD_REQUEST, 2 CAPABILITY_UNAVAILABLE, 3 SESSION_REQUIRED, 4 INTERNAL_ERROR. Un error tiene exactamente N=9. Peticiones menores a 8 bytes o con ruta incorrecta se descartan porque no pueden correlacionarse. Datos multibyte nunca son structs crudos. Enteros con signo de 32 bits usan complemento a dos. No se transmite JSON por UART.

## Catálogo mínimo

Offsets de cuerpo contados después del prefijo de petición (8) o respuesta (9).

| ID | Uso | Petición N / cuerpo | Respuesta OK N / cuerpo |
| --- | --- | --- | --- |
| 0x01 | hello interno ESP↔Mega | 13: ASCII `AFR2` [8..11], perfil u8=1 [12] | 23: `AFR2` [9..12], perfil u8 [13], firmware major/minor/patch u8 [14..16]=0/2/0, capabilities u16 [17..18], uptimeMs u32 [19..22] |
| 0x02 | alive interno | 8, sin cuerpo | 13: uptimeMs u32 [9..12] |
| 0x03 | robot.state | 8, sin cuerpo | 16: mode u8 [9], stopped u8 [10], lease u8=0 [11], ageMs u32 [12..15] |
| 0x04 | robot.sensors | 8, sin cuerpo | 22: lineValid u8 [9], lineBits u8 [10], lineAgeMs u32 [11..14], ultrasonicValid u8 [15], distanceMm u16 [16..17], ultrasonicAgeMs u32 [18..21] |
| 0x05 | robot.encoders | 8, sin cuerpo | 31: wired u8 [9], valid u8 [10], cuentas FL/FR/RL/RR i32 [11..26], ageMs u32 [27..30] |
| 0x06 | robot.stop | 8, sin cuerpo | 11: stopped u8=1 [9], mode u8=0 [10]. Solo tras callback de apagado autoritativo exitoso y capability STOP |

Mode: 0 idle, 1 manual, 2 line-follow, 3 wheel-test, 4 goto, 5 fault, 255 unknown. Stopped: 0 false, 1 confirmado por aplicación, 255 unknown. Lease: únicamente 0 none en este perfil. Age desconocida: 0xFFFFFFFF; no es una antigüedad válida. Bits de línea: bit0..bit4 = IR1..IR5. Un campo numérico inválido es padding ignorado: ESP lo convierte a `null`, nunca a una lectura cero. Sensores >2000 ms se proyectan con valid=false y valor=null, conservando edad conocida.

Capabilities u16: bit0 UART negociada, bit1 estado integrado, bit2 línea integrado, bit3 HC-SR04 integrado, bit4 Hall integrado, bit5 STOP integrado. Bits restantes reservados=0. No hay bit que habilite motores/lease/GOTO. El endpoint fuerza UART y elimina flags de snapshot/STOP si faltan callbacks; la composición `communication_main.c` anuncia únicamente UART. Sus snapshots son unknown. No confundir código UI local con una capability remota.

## Ciclo, prioridades y fallos

- `system.hello`/`system.alive` Web son servicios ESP con sobre afr2-ws-1; no esperan que el Mega esté presente. Su payload informa estado y capacidades actuales.
- ESP negocia al segundo de arranque/reintento; alive UART cada 500 ms sin otra transacción. Una sola transacción correlacionable normal, sin reintentos automáticos de acciones. Timeout UART 800 ms; tras timeout retira ready/capabilities y deja 1000 ms antes de renegociar. Sesión y transacción descartan tardíos incluso después de esa ventana.
- El Mega revoca la negociación tras >1500 ms sin pedido válido y llama el callback STOP si existe. Un error RX UART invalida también la sesión; una nueva sesión revoca la anterior. Fallos de transporte no se presentan como parada confirmada.
- STOP tiene una ranura ESP reservada y no exige lease. Sustituye la lectura en vuelo con SUPERSEDED; si ya empezó a transmitir una trama, termina sus bytes antes de transmitir STOP. No intercala tramas. Un segundo STOP pendiente recibe BUSY_IN_FLIGHT. El callback ausente devuelve CAPABILITY_UNAVAILABLE, jamás OK.
- Fuente AVR: RX/TX de 128 bytes por IRQ, capacidad útil 127; escritura de trama completa a la cola o rechazo. Cada frame de este perfil cabe. Una respuesta retenida espera espacio y no pisa el buffer. Fuente ESP: RX de 256 bytes, transmisión parcial acotada por availableForWrite, hasta una trama normal y una prioritaria estables. Memoria final y robustez todavía no medidas.

## Integración y despliegue pendientes

`Afr2LinkOps` recibe snapshot coherente y callback STOP de la aplicación futura. No otorgar flags por copiar un driver. La composición AVR opcional usa Timer0 CTC a 1 ms y mantiene PWM/direcciones inactivos; no incorpora TWI, OLED, encoder, sensores ni control de potencia completo. El objetivo Make `communication-firmware` solo queda escrito; no se ejecutó.

Actualizar firmware de ambos extremos conjuntamente. El bundle actual ESP/data/dist procede de AFR2-20261023-09; los ajustes Web de esta ficha requieren nuevo build/empaquetado/promoción cuando se pidan. No hubo compilación, tests, validadores, flasheo ni prueba física en esta ficha.
