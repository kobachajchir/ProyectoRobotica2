> AFR2-20261025-01: las reglas anteriores de acceso quedan sustituidas por [control directo EX01v7](DIRECT_CONTROL_7.md). Sin adquisición, renovación, liberación ni habilitación OLED. Los formatos anteriores solo se conservan donde DIRECT_CONTROL_7 mantiene compatibilidad.

# ROBO2: perfil UART 2

Verificación posterior AFR2-20261023-17: fuente ESP revisada contra este contrato y compilada con éxito, sin warnings/errors. [Evidencia](../documentation/builds/2026-10-23-17-robo2-esp/README.md). No se compiló AVR ni se ejecutaron casos de protocolo/hardware en esa ficha; el estado de fuente sin compilar mencionado abajo corresponde al registro inicial de este perfil.

AFR2-20261023-15 · 23/10/2026. Contrato normativo vigente para Mega↔ESP. Sustituye
framing UNER/perfil1; no hay fallback ni compatibilidad binaria. Fuente escrita
en ambos extremos, sin compilar, ejecutar tests/validadores ni probar hardware.
[Criterios comunes de validación](ROBO2_VALIDACION.md) ·
[Guía Mega](../../AutoFinalRobo2-Atmega/docs/COMUNICACION_ESP.md) ·
[Guía ESP para retomar](../../AutoFinalRobo2-ESP/docs/PROTOCOLO_ROBO2.md).

## Trama y ruta

| Offset trama | Tamaño | Campo | Validación idéntica en ambos parsers |
| --- | --- | --- | --- |
| 0..4 | 5 | Cabecera ASCII ROBO2: 52 4F 42 4F 32 | Cinco bytes exactos, mayúsculas |
| 5 | 1 | N:u8 | Solo bytes de payload; máximo64 |
| 6 | 1 | Token A=3A (`:`) | Exacto |
| 7 | 1 | Token B=A5 | Exacto, byte binario |
| 8 | 1 | Versión de framing=01 | Exacta, independiente del perfil |
| 9 | 1 | ROUTE:u8 | Emisor nibble alto, receptor nibble bajo |
| 10 | 1 | CMD:u8 | Catálogo/dirección/longitud en endpoint |
| 11..10+N | N | Payload | Exactamente N bytes, sin interpretar cabeceras interiores |
| 11+N | 1 | XOR:u8 | XOR de TODOS los bytes anteriores, cabecera y tokens incluidos |

Total12+N, máximo76. N=0 válido solo a nivel framing; ningún comando actual
admite N<8. Núcleo único C/C++ en [afr2_wire.h](afr2_wire.h); no copiar ni
mantener parsers separados. Dos tokens fijos, versión y XOR verifican estructura
como UNER; no son autenticación ni CRC y no detectan toda corrupción posible.
El builder comprueba punteros, N, ruta y capacidad de salida >=12+N antes de
escribir; falla con longitud0 sin publicar una trama parcial.

`route = ((source & 0x0F) << 4) | (destination & 0x0F)`, mismo significado que
F4. IDs estables: Mega1, ESP3. Petición ESP→Mega=31; respuesta Mega→ESP=13.
Parser admite solo esas dos rutas. Cada endpoint exige además su emisor remoto
y destinatario local mediante afr2_route_matches. No broadcast, Qt, NRF ni
self-route. No confundir recepción UART física con destino lógico válido.
La ruta no codifica petición/respuesta: este catálogo solo inicia pedidos desde
ESP, Mega responde; no hay eventos UART ni solicitudes espontáneas Mega→ESP.

Transporte candidato UART0 ESP GPIO1/TX-GPIO3/RX ↔ Mega USART1 D18/TX1-D19/RX1
por E_RX/E_TX, 1152008N1. Mega a16 MHz U2X1/UBRR16, +2,12 % nominal, no medido.
No logs ni AT. [Ruta eléctrica](../hardware/WIFI_SHIELD.md),
[carga separada](../hardware/FLASHING.md). No inferir niveles desde el framing.

## Prefijo, respuesta y correlación

Offsets siguientes relativos al payload. Sesión u32 LE [0..3], transacción
u32 LE [4..7], ambas no cero y presentes en cada petición/respuesta. ESP elige
sesión no cero al arrancar, transacciones crecientes y nueva sesión antes de
reutilizar contador. Mega replica prefijo y CMD, invierte emisor/receptor.
No se serializan structs/punteros/enums nativos ni JSON por UART.

Mega solo ejecuta pedidos tras hello válido y sesión coincidente. En la sesión
actual rechaza transacción <= última aceptada, también para hello; no ejecuta
duplicados ni renueva supervisión con ellos. Conserva ese último número al
caducar/revocar sesión. Un nuevo hello válido puede establecer otra sesión y
revoca la anterior, invocando hook STOP si existe. No es protección contra
replay malicioso de sesiones históricas ni mecanismo de autenticación.

ESP solo acepta respuesta con transacción activa y coincidencia de ruta, CMD,
sesión, transacción, longitud exacta, resultado y cuerpo. Duplicadas, tardías,
eco propio, espontáneas y ACK genéricos E0/E1 se descartan. Wire válido todavía
no equivale a operación completada. Ambas validaciones se aplican al recibir;
no se confía en que el emisor ya haya validado.
El receptor descarta una respuesta si el plazo de800 ms ya venció antes de
despacharla, aunque encuentre sus bytes en la cola RX de esa iteración.

Resultado [8] en cada respuesta: 0 OK,1 BAD_REQUEST,2 CAPABILITY_UNAVAILABLE,
3 SESSION_REQUIRED,4 INTERNAL_ERROR. Error tiene exactamente N=9; ningún cuerpo.
Ruta incorrecta/prefijo incompleto: descarte sin error correlacionado. Prefijo
cero o longitud incorrecta: BAD_REQUEST; sin sesión negociada: SESSION_REQUIRED;
comando desconocido con N=8 y sesión vigente: CAPABILITY_UNAVAILABLE. No hay ACK
de encolado UART separado. Respuesta OK significa resultado semántico descrito
por cada comando; STOP exige confirmación del productor autoritativo.

## Catálogo y cuerpos exactos

Los payloads base conservan campos del perfil1, con perfil hello=2 y firmware
actual0.3.0. Esta conservación no implica compatibilidad de frames.

| CMD | Productor/consumidor | Petición ESP→Mega | Respuesta OK Mega→ESP |
| --- | --- | --- | --- |
| 01 hello | enlace interno | N=13; `AFR2` ASCII [8..11], perfil u8=2 [12] | N=23; `AFR2` [9..12], perfil=2 [13], fw major/minor/patch u8 [14..16], capabilities u16 [17..18], uptimeMs u32 [19..22] |
| 02 alive | enlace interno | N=8, sin cuerpo | N=13; uptimeMs u32 [9..12] |
| 03 state | aplicación Mega → robot.state Web | N=8 | N=16; mode u8 [9], stopped u8 [10], lease u8=0 [11], ageMs u32 [12..15] |
| 04 sensors | productores Mega → robot.sensors | N=8 | N=22; lineValid u8 [9], lineBits u8 [10], lineAgeMs u32 [11..14], ultrasonicValid u8 [15], distanceMm u16 [16..17], ultrasonicAgeMs u32 [18..21] |
| 05 encoders | Hall Mega → robot.encoders | N=8 | N=31; wired u8 [9], valid u8 [10], counts FL/FR/RL/RR i32 [11..26], ageMs u32 [27..30] |
| 06 stop | autoridad Mega → robot.stop | N=8, sin lease, prioridad | N=11; stopped u8=1 [9], mode u8=0 [10], solo tras callback STOP exitoso y capability |

Enteros con signo: i32 complemento a dos. Uptime y edades en ms; distancia mm;
cuentas Hall sin convertir a velocidad/posición. Desconocido age=FFFFFFFF.
Mode:0idle/1manual/2line-follow/3wheel-test/4goto/5fault/255unknown.
Stopped:0false/1confirmado/255unknown. Lease exclusivamente0none. Booleanos de
validez y wired exactamente0/1. Línea: bit0..4=IR1..5, bits5..7=0. No inferir
calibración, distancias, cableado Hall ni actividad de motor de datos ausentes.

Capabilities u16: bit0UART,1STATE,2LINE,3ULTRASONIC,4ENCODERS,5STOP; resto0.
Hello requiere bit UART y perfil/producto correctos; firmware version es dato,
no prueba de hardware. Mega filtra flags si faltan callbacks y fuerza UART.
Valid=true necesita capability y edad conocida; encoders además wired=true.
El emisor normaliza flags y rangos de estado; el receptor vuelve a comprobar.
ESP ignora respuestas con cuerpo fuera de rango en vez de completar el pedido.
Sin capability STATE, mode/stopped/age de respuesta deben ser unknown.

Valor inválido es padding: ESP lo proyecta a null. Edad desconocida no es cero;
lectura válida pero >2000 ms se muestra caducada/valid=false y conserva edad
conocida. No anunciar potencia/lease/GOTO por tener bit UART. Composiciones
actuales anuncian SOLO UART; STOP unavailable y sensores/state desconocidos.
Las cuatro pantallas OLED son locales: no agregan comandos ni bits remotos.

## Fallos, prioridades y tiempos

- Gap entre bytes >50 ms: reinicio de framing con resta modular u32. Cabecera,
  token, versión o checksum incorrectos resincronizan buscando R, reutilizando
  el byte fallido si es R. Tramas N>64 o ruta no permitida se drenan sin copiar
  su payload ni buscar una cabecera dentro. Contadores u16 saturan por motivo.
- Error RX/overflow: descartar flujo incierto, invalidar negociación y aplicar
  política de supervisión. Mega llama hook STOP si existe; respuesta retenida
  aún no publicada se descarta. ESP termina pendiente con MEGA_LINK_DOWN. Una
  trama TX ya empezada puede terminar; no habilita aceptación de una tardía.
- Hello inicial/reintento tras1000 ms; alive cada500 ms sin otra transacción.
  Timeout ESP800 ms, retira ready/caps y renegocia tras1000 ms. Mega revoca tras
  >1500 ms sin petición válida. Comando desconocido, duplicado o framing malo
  no renuevan supervisión. Timeout no confirma resultado físico.
- Una transacción normal y un STOP reservado en ESP. STOP reemplaza lectura
  pendiente con SUPERSEDED, nunca intercala bytes con frame previo. Segundo
  STOP pendiente recibe BUSY_IN_FLIGHT. No reintentar acciones automáticamente.
- TX ESP conserva buffers parciales; AVR acepta frame completo o devuelve busy.
  Rings AVR RX/TX128 B (127 útiles), ESP RX256 B, frame76 B. Parser/handlers main;
  ISR AVR solo publica bytes/estado. Presupuesto RX64bytes por iteración AVR y
  192 ESP. Sin DMA/HAL. No es medición de CPU/latencia ni margen de memoria.

## Despliegue y estado real

Actualizar header y ambos firmwares juntos. Web conserva afr2-ws-1 sin cambios
en este ID; paquete ESP/data/dist anterior y cambios Web posteriores conservan
sus propios pendientes de reconstrucción. No se generaron firmware, imagen FS,
paquete Web, mapas ni pruebas. Falta identificar flash/SPI/layout ESP, comprobar
UART/hardware, medir SRAM/pila/IRQ/render y ejecutar casos solo bajo pedido.

Mega integra TWI/Manager/OLED/encoder local sin motores/lease/GOTO/STOP real ni
sensores. ESP adapta transporte/recepción ROBO2 en fuente y conserva broker.
UI remota, nuevos comandos y solicitudes/eventos Mega→ESP necesitan ficha con
productor/consumidores/ACK/timeout antes de activarlos.
