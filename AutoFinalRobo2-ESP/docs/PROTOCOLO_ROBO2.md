> AFR2-20261025-01: [control directo7](../../AutoFinalRobo2-Contracts/contracts/DIRECT_CONTROL_7.md), sin permiso de acceso. Firmware0.9.0 compilado,se conservan59ScreenCodes. Estados inferiores históricos.

> **Vigente AFR2-20261024-15:** [Matriz del maestro](../../AutoFinalRobo2-Contracts/docs/ALINEACION_MAESTRO.md):33comandos y59identidades OLED con fuentes complementarias. Calibración Web adquiere control al escribir; ESP/Web informan FS/paquete. Web/ESP compilados15; AVR sin nueva fuente, build12/EX01v6/firmware0.8.0/gates0. Sin brecha software definida identificada tras lectura; aceptación física, memoria dinámica, flash/SPI y planos CAD pendientes. Solo documentación en CAD, sin diseño nuevo. Estados inferiores históricos.

> **AFR2-20261024-08:** IP/AP y avisos con iconos F4, scroll seleccionado y reinicio local por Sistema → Reinicio, compilados. [EX01v5](../../AutoFinalRobo2-Contracts/contracts/EX01_NETWORK_UI_5.md) · [Estado real](../../AutoFinalRobo2-Contracts/docs/ESTADO_IMPLEMENTACION.md). En ATmega, FillRect/TextWindow, iconos y snapshot de red quedan separados del transporte/I2C. Sin prueba de placa ni flasheo; estados inferiores históricos.

# ROBO2: implementación y recepción en ESP8266

AFR2-20261023-15 · 23/10/2026. Documento para retomar el firmware ESP de este
proyecto. Leerlo junto con `README.md`, `INFORME_ADAPTACION.md` y
[contrato normativo, offsets y catálogo](../../AutoFinalRobo2-Contracts/contracts/UART_PROFILE_2.md).
El [criterio común de aceptación](../../AutoFinalRobo2-Contracts/contracts/ROBO2_VALIDACION.md)
documenta resultados esperados en ambos extremos; no es un informe de pruebas.

## Qué hay implementado y qué falta

`Contracts/contracts/afr2_wire.h` contiene UN solo parser/builder C/C++ usado
por ESP y Mega. `src/mega_link.cpp` implementa el emisor de peticiones y receptor
de respuestas ROBO2, validación de cuerpo, sesión/transacción, negociación,
heartbeat, timeout y TX parcial sin intercalar tramas. `src/broker.cpp` conserva
el sobre Web `afr2-ws-1`, traduce resultados a JSON y comunica errores RX como
MEGA_LINK_DOWN. Mega tiene endpoint y composición local OLED/TWI/encoder.

Al registrar AFR2-20261023-15 esto estaba en fuente, SIN compilar, ejecutar tests/validadores,
flashear ni probar dispositivos. No se declara interoperabilidad verificada.
Faltan productores de sensores/potencia, STOP autoritativo, lease, Hall/GOTO,
UI remota y configuración Wi-Fi desde OLED. El firmware ESP continúa siendo
dueño de Wi-Fi/credenciales/LittleFS; Mega no usa AT. Identificación flash,
modo SPI/layout e imagen de carga siguen pendientes según `docs/PLAN_ESP.md`
de Contracts. No ejecutar los scripts históricos ni cargar firmware por leer
esta guía.

## Qué debe enviar ESP y qué debe recibir

`ROBO2` son CINCO bytes ASCII: `52 4F 42 4F 32`. Luego:

`ROBO2 | N:u8 | 3A | A5 | VERSION=01 | ROUTE:u8 | CMD:u8 | PAYLOAD[N] | XOR:u8`

N cuenta SOLO payload: 0..64 en framing, 8..31 en el catálogo actual. Total
12+N, máximo 76 bytes. Ambos tokens son obligatorios y están cubiertos por XOR.
XOR acumula cada byte desde la R hasta el último payload, excluyendo el propio
checksum. Son verificaciones estructurales, NO autenticación/cifrado ni CRC.

La ruta reproduce la semántica F4: `(emisor << 4) | receptor`. Mega=1, ESP=3.
ESP envía **0x31**, Mega responde **0x13** con el MISMO CMD, sesión y transacción.
Al recibir, ESP comprueba emisor=1 Y receptor=3; Mega comprueba emisor=3 Y
receptor=1. No se aceptan broadcast, self-route ni los nodos Qt/NRF históricos.
No crear una segunda copia de constantes ni usar números de ruta dispersos:
usar AFR2_ROUTE_REQUEST/RESPONSE y afr2_route_matches del header compartido.

El WebSocket no transmite estos bytes: usa JSON `afr2-ws-1`; el ESP es el puente.
Los comandos 0x01 hello y 0x02 alive son internos del enlace UART; system.hello
y system.alive de la Web son servicios propios del ESP y no dependen del Mega.

## Contrato de datos esperado en ESP

Todos los enteros multibyte son LE. No transmitir `sizeof(struct)` ni enums
nativos. Payload de petición: `session:u32` [0..3], `transaction:u32` [4..7],
ambos no cero. Respuesta: copia ese prefijo, `result:u8` [8], cuerpo desde [9].
Los offsets siguientes son relativos al PAYLOAD, no a la trama completa.

| CMD | ESP transmite | ESP espera si result=0 |
| --- | --- | --- |
| 01 hello | N=13; `AFR2` [8..11], perfil=2 [12] | N=23; `AFR2` [9..12], perfil=2 [13], firmware major/minor/patch [14..16], caps u16 [17..18], uptime u32 [19..22] |
| 02 alive | N=8, solo prefijo | N=13; uptime u32 [9..12] |
| 03 state | N=8 | N=16; mode [9], stopped [10], lease=0 [11], ageMs u32 [12..15] |
| 04 sensors | N=8 | N=22; lineValid [9], lineBits [10], lineAge u32 [11..14], ultrasonicValid [15], distanceMm u16 [16..17], usAge u32 [18..21] |
| 05 encoders | N=8 | N=31; wired [9], valid [10], FL/FR/RL/RR i32 [11..26], ageMs u32 [27..30] |
| 06 stop | N=8, ranura prioritaria | N=11; stopped=1 [9], mode=0 [10], SOLO con capability y callback de apagado real |

Los errores SIEMPRE tienen N=9 y el mismo prefijo/CMD: 1 BAD_REQUEST,
2 CAPABILITY_UNAVAILABLE, 3 SESSION_REQUIRED, 4 INTERNAL_ERROR. Resultado >4 o
un error con cuerpo se descartan. No aceptar ACK E0/E1 como confirmación.

Mode admite 0 idle, 1 manual, 2 line-follow, 3 wheel-test, 4 goto, 5 fault,
255 unknown. Stopped admite 0,1,255 (255→null). Lease únicamente 0. Flags de
validez/wired: exactamente 0/1. Línea bit0..4 IR1..5, bits altos cero.
Capabilities: bit0 UART, bit1 STATE, bit2 LINE, bit3 ULTRASONIC, bit4 ENCODERS,
bit5 STOP; resto cero. Hello requiere bit UART y perfil correcto. No conceder
control/lease por saludar. Las composiciones Mega actuales anuncian solo UART.

Un flag válido necesita edad conocida y capability correspondiente; encoders
válidos también necesitan wired=1. Unknown age=FFFFFFFF. El broker convierte
padding inválido en null y aplica frescura máxima 2000 ms. Cero milímetros o
cuentas cero nunca sirven como reemplazo de una muestra ausente. STOP no se
confirma por recepción/transmisión, hello ni por mantener GPIO inactivos.

## Validación en los dos sentidos

1. Compartida: sincronizar los cinco bytes, N, dos tokens, versión, ruta
   permitida, exactamente N bytes y XOR. No entregar paquetes parciales.
2. Destinatario: aplicar emisor/receptor locales, incluso si el parser conoce
   ambas rutas. Un eco de la propia trama no es una respuesta.
3. Semántica: tamaño exacto según CMD/dirección, prefijo no cero, perfil/producto
   en hello, y resultado/cuerpo válido en respuestas.
4. Correlación: ESP exige transacción activa y coincidencia de CMD, sesión Y
   transacción; descarta tardíos, duplicados y respuestas espontáneas. Mega
   exige sesión negociada y transacción creciente para ejecutar una petición.
   Un mismo número repetido/menor en esa sesión se rechaza sin ejecutar.
5. Solo después se actualiza estado/heartbeat y se entrega resultado al broker.

La validación de FRAMING es literalmente la misma fuente en ambos lados; las
reglas de rol cambian según se reciba una petición o una respuesta. No eliminar
controles en un extremo suponiendo que el emisor ya los hizo. El catálogo actual
solo permite iniciar pedidos en ESP y responder en Mega: no hay eventos UART
ni pedidos espontáneos Mega→ESP. Incorporarlos exige nueva ficha/contrato,
handlers y correlación; la ruta por sí sola no identifica el tipo de mensaje.

## Ruido, fallos, tiempo y buffers

Parser: al fallar cabecera/token/versión/checksum busca nueva R, conserva una R
coincidente y descarta el resto del paquete. N>64 o ruta ajena: drena la trama
declarada sin copiar payload ni interpretar una cabecera dentro. Reiniciar tras
gap entre bytes **>50 ms**, con aritmética u32 modular. Contadores locales
u16 saturan (frames/header/length/token/version/route/checksum/gap). No imprimir
diagnóstico por UART de protocolo. Los mensajes ROM del ESP son ruido previo.

Sesión ESP no cero al arranque, transacciones crecientes, cambiar sesión antes
de reutilizar un contador. Hello inicial/reintento tras 1000 ms; alive cada
500 ms sin otra transacción; timeout de pedido 800 ms. Mega revoca negociación
tras >1500 ms sin petición válida. Un RX overflow/FE/DOR/paridad revoca sesión
en Mega y solicita STOP si hay hook. Error RX en ESP revoca ready/capabilities,
descarta backlog incierto y termina petición de usuario con MEGA_LINK_DOWN.
No se presenta una pérdida de enlace como parada física confirmada.

Una transacción normal y una ranura STOP reservada. STOP puede reemplazar una
lectura (SUPERSEDED), pero termina primero los bytes de una trama ya iniciada.
Jamás intercalar dos tramas. No reintentar acciones automáticamente. TX parcial
retiene ambos buffers hasta copiarlos. RX ESP 256 B; AVR RX/TX 128 B cada ring,
127 útiles. AVR acepta trama entera o rechaza sin consumir respuesta pendiente.
ISR AVR solo mueve bytes; parser y handlers corren en main. Sin DMA.

UART0 ESP GPIO1 TX/GPIO3 RX ↔ USART1 Mega D18 TX/D19 RX por selector E_RX/E_TX.
115200 8N1 candidato; Mega nominal +2,12 %, no medido. No mezclar logs ni puertos
de carga con esta ruta. [Cableado y niveles](../../AutoFinalRobo2-Contracts/hardware/WIFI_SHIELD.md).

## Cómo continuar desde firmware ESP

Leer primero este documento y `UART_PROFILE_2.md`. Inspeccionar la fuente
vigente y los pendientes, sin rehacer lo ya escrito. Mantener el header único y
actualizar AMBOS extremos juntos: UNER/perfil 1 no negocia con ROBO2. Web no
necesita un cambio de framing porque su sobre sigue `afr2-ws-1`; su paquete
previo aún requiere reconstrucción por otros cambios fuente documentados.
No asumir UI remota, STOP o sensores integrados por haber conectado Mega.
Cuando el usuario lo pida expresamente: compilar ambos, estudiar mapas/memoria,
ejecutar los casos comunes, y recién luego probar enlace/banco autorizados.

Procedencia: ideas de `Atmega/reference/f4/Core/Src/uner_core.c`,
`uner_handle.c`, `uner_transport_uart1_dma.c` y `Core/Inc/uner_v2.h`.
Se conservan autores/hash en Contracts/provenance; DMA se reemplaza por IRQ AVR.
No se modifican AutoMicro, sus registros ni reference/.

## Retoma y compilación ESP — AFR2-20261023-17

Se leyó esta guía junto con UART_PROFILE_2.md y ROBO2_VALIDACION.md y se revisó su correspondencia con afr2_wire.h, mega_link.* y broker.cpp vigentes. El trabajo paralelo ya había incorporado el protocolo a estas fuentes; no se duplicó ni modificó el parser/enlace durante esta revisión.

Build ESP `platformio run -e esp12s_candidate`: SUCCESS, exit 0, sin warnings/errors en el log. RAM estática 40.096 bytes; programa 386.189 bytes según el perfil candidato. [Evidencia](../../AutoFinalRobo2-Contracts/documentation/builds/2026-10-23-17-robo2-esp/README.md). No se ejecutaron los casos comunes de validación ni se compiló AVR, abrió puerto o flasheó. Compilar no acredita interoperabilidad, margen dinámico ni flash/layout real. Las notas iniciales de fuente sin compilar conservan el estado histórico de la ficha 15; este apartado registra el estado posterior de ESP.


## Integración vigente — AFR2-20261024-01

Revisadas fichas19/20/21. La Web21 ya contiene los cambios visuales/editor/simulador;
Wi-Fi20 se conserva. La integración de firmware pendiente se adapta en fuente:
ROBO2/perfil2 + EX01 v2, estado/pose, control supervisado, framebuffer OLED y
secuencias0x40..0x45 sin colisión con comandos de producto19.
Sin nuevo build, validador, tests, flasheo o promoción LittleFS. Secuencias físicas
permanecen bloqueadas por defecto hasta aprobar SRAM/pila y calibración real.
Contrato en AutoFinalRobo2-Contracts/contracts/EX01_WEB_INTEGRATION.md y ficha
AutoFinalRobo2-Contracts/changes/2026-10-24-01-firmware-web-integration.md.

