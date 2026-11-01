> **Fuente AFR2-20261029-18:** IR1..IR5 OSOYOO digital (A4..A0), perfil predeterminado habilitado y EEPROMv4 migra solo tipos antiguos deshabilitados; calibración/seguimiento siguen pendientes. Diagnóstico servo/encoder agrega lectura RAW directa y cambios por OLED/serie, sin EEPROM. Vistas IR/Web distinguen deshabilitado/pendiente/cero válido. [Ficha](../changes/2026-10-29-18-ir-input-acquisition.md). Compilaciones pendientes al registrar la fuente; no cargar por instrucción vigente. Placa conserva15; Web sin build/promoción.

> **Diagnóstico17 compilado, sin cargar:** usuario confirma puente S1–D13 instalado además de GND/5V/S1 del servo. BuildOnly exit0, sin errores/advertencias, sin COM/avrdude. Flash13424bytes; SRAM3169bytes, margen5023bytes, sin certificación de pila. HEX afr2-i2c-diagnostic.hex SHA256 03cf3a3760f3b6bc5b9b8a7ce128a56388a5bd51b69209a2ea39e895283ca573. Causa del servo inmóvil aún sin medición/prueba física. Producto16 no cargado; placa conserva15. Las notas de compilación/puente pendientes inferiores son históricas.

> **Diagnóstico AFR2-20261029-17:** encoder controla servo D13/OC1C en pasos5grados; PWM inicialmente OFF, C=centrar, S=apagar PWM. OLED y serie muestran orden/contador/botón; I2C cada10s. S1 ModelY necesita puente adicional aD13 según manual. Script agrega -BuildOnly (sin COM/flash). [Ficha](../changes/2026-10-29-17-diagnostic-encoder-servo.md). Fuente lista, compilación de prueba pedida; no se autoriza carga. Producto16 sigue sin cargar; placa conserva15.

# Diagnóstico I2C por USART0 — AFR2-20261028-19

> **Fuente AFR2-20261029-11:** driver compartido ahora diferencial exacto y
> cooperativo, tramos de hasta16columnas y sin retransmisión completa periódica.
> También se quitó el marcado periódico en la fuente del diagnóstico. No hay
> nuevo HEX generado; las confirmaciones físicas de09 no validan este driver11.
> Memoria/pila pendientes. [Ficha](../changes/2026-10-29-11-oled-delta-cooperative.md).

> **AFR2-20261029-10:** el usuario confirma que diagnóstico09 funciona, con
> orientaciones correctas de OLED/encoder, y solicita trasladarlo al producto.
> Configuración aplicada en fuente, sin nuevo build/carga del agente.
> [Alcance de integración](../changes/2026-10-29-10-product-oled-encoder-verified-setup.md).

> **Vigente AFR2-20261029-09, solo prueba:** sentido del encoder invertido
> respecto de08, tanto para contador como para etiquetas A(+)/B(-) serie.
> OLED y barrido ahora se intercalan por transferencia: el barrido completo no
> suspende el envío de PRESIONADO/SUELTO. Serie mediante cola511bytes útiles,
> sin esperar por cada carácter; tx_dropped saturado indica salida perdida si
> se llena. Se reenvía el framebuffer actual cada1s sin mensajes periódicos,
> para refrescar una imagen desactualizada sin necesitar otro evento físico.
> Sigue antirrebote20ms y timeout/recovery del Manager. Banner esperado09,
> mismo script. Solo fuente, sin compilar/cargar/probar; no se afirma que
> todo cuelgue físico esté resuelto. [Ficha](../changes/2026-10-29-09-diagnostic-direction-responsive.md).

> **Vigente AFR2-20261029-08, solo servicio de prueba:** reemplaza las barras
> por CONTADOR, valor firmado y PRESIONADO/SUELTO debajo. Giro A suma1 y B
> resta1 por paso decodificado. Arranca en0, no guarda EEPROM y satura en los
> límites int32. Botón con antirrebote20ms; presionar/corto/largo no resetea el
> contador ni ejecuta acciones. Conserva orientación180 y el script de carga;
> banner esperado AFR2 OLED+ENC DIAG AFR2-20261029-08. Redibuja solo al cambiar
> valor/estado. Durante el barrido I2C puede pausar el envío al OLED, pero la
> captura del encoder continúa; esta pausa se elimina en09. Solo fuente,
> sin compilar/cargar/probar. Las referencias a barras más abajo describen07.
> Producto/Web sin cambios por08, según alcance explícito del usuario.

> **Vigente AFR2-20261029-07:** prueba activa SSD1306 en0x3C, barras móviles
> a180grados y encoder. Pull-ups internos débiles activados solo en este
> servicio para comparar con Wire/Adafruit, que el usuario reporta funcional.
> Código C/avr-libc + Manager, sin Arduino. Fuente sin compilar/cargar/probar.
> [Cambios y límites](../changes/2026-10-29-07-diagnostic-active-oled.md).

> **Conservado de AFR2-20261028-22:** salida solo ante cambios. El barrido I2C se
> repite cada10s entre inicios; desaparece el reporte del encoder en reposo.
> Primera exploración establece la referencia. Solo fuente, sin compilar/cargar.

> Ampliación **AFR2-20261028-20**: el mismo servicio también depura encoder
> y pulsador. Fuente preparada, sin compilar/cargar/probar. Conserva el objetivo
> y nombre de HEX previstos; el banner vigente identifica AFR2-20261029-07.

Fuente preparada por pedido de Koba Chajchir. **Sin compilar, cargar ni probar.**
Es una imagen de servicio separada; al cargarla sustituye temporalmente el
programa del auto. Inicializa el OLED0x3C y transmite un patrón de barras que
se desplaza cada segundo. Además busca direcciones. ACK de inicialización no
prueba imagen visible: confirmar físicamente el patrón. No identifica el chip.

## Cableado y monitor

| Señal | Mega 2560 |
| --- | --- |
| SDA del OLED | D20 / PD1 |
| SCL del OLED | D21 / PD0 |
| RX0, entrada de órdenes | D0 / PE0 |
| TX0, salida de diagnóstico | D1 / PE1 |
| Encoder CLK | D2 / PE4 / INT4 |
| Encoder DT | D3 / PE5 / INT5 |
| Pulsador SW | D42 / PL7, activo en bajo |
| Monitor serie | 115200 baudios, 8 bits, sin paridad, 1 stop |

El usuario confirma que el pinout del shield une SDA/SCL con D20/D21.
Se conserva la conexión actual: no hace falta duplicarla. En Mega Rev3 la
cabecera adicional SDA/SCL expone esas mismas señales, según el
[pinout oficial](https://docs.arduino.cc/resources/pinouts/A000067-full-pinout.pdf).
No se midió continuidad de la unidad instalada.

El USB-B del Mega permite observar USART0 mediante el puente USB-serie
integrado: no hace falta otro cable serie. Mantener ESP/otros emisores aislados
de D0/D1. Con adaptador externo, D1/TX0 va a RX del adaptador con masa/niveles
adecuados; no conectar dos salidas TX. D0 recibe la orden opcional R.

En esta imagen se activan pull-ups débiles hacia VCC del AVR, como hace Wire
en el sketch funcional reportado. No es adaptación de niveles ni cambia la
configuración por defecto del producto. Los niveles reportados son lógicos,
no voltajes. La alimentación
del panel a 5 V es un montaje comunicado; esta fuente no certifica tolerancias.
Como para el puente de servicio, desconectar potencia de actuadores antes de
cargar. El código solicita PWM/direcciones inactivos y deja D13 del servo como
entrada sin pull-up; eso no prueba el estado físico de potencia.

## Objetivo preparado, no ejecutado

Desde **AFR2-20261028-21** se puede compilar y flashear con el script separado:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File "D:\ProyectoRobotica2\AutoFinalRobo2-Atmega\tools\flash-i2c-encoder-diagnostic.ps1" -Port COM16
```

Cambiar COM16 por el puerto real. Puede ejecutarse desde cualquier carpeta:
resuelve el módulo desde su ubicación. Recompila con `-B`, se detiene si falla
la compilación o falta el HEX y solo después carga/verifica con avrdude.
Requiere monitor cerrado, RESET libre, ESP aislado de D0/D1 y potencia de
actuadores desconectada. Reemplaza temporalmente el programa del auto. No lo
ejecutó el agente: está preparado en fuente, sin compilación ni carga acreditada.
Herramientas configurables con `-MakePath`, `-AvrBin` y `-AvrdudeRoot`; los
valores por defecto son las rutas instaladas identificadas en esta PC.
Restaura carpeta y PATH al finalizar/fallar; no cambia políticas persistentes.

Desde `D:/ProyectoRobotica2/AutoFinalRobo2-Atmega` con AVR GCC/GNU Make:

```powershell
make i2c-diagnostic-firmware
```

Salida prevista: `build/afr2-i2c-diagnostic.hex`, ELF y mapa propios. No se
generaron en esta entrega. No sobrescribe `afr2.hex`, no ejecuta upload y no
forma parte del firmware, biblioteca, comunicación o puente ESP. El Loader no
selecciona este servicio automáticamente. Compilar/cargar sigue pendiente de
pedido expreso. Restaurar luego el producto; no confundirlo con backup de puente.

## Comportamiento y lectura

Banner `AFR2 OLED+ENC DIAG AFR2-20261029-07`; OLED espera500ms para inicializar,
primera exploración después de1segundo y cuando el Manager quede libre; barrido
de direcciones de 7 bits `0x08..0x77` (sin reservadas), a 100 kHz por el único
I2C Manager/backend TWI existente. Cada consulta envía START, SLA/W y STOP,
sin bytes de datos en los probes. El OLED sí recibe comandos y framebuffer
fuera del barrido, por el mismo Manager. Se pausa su tarea durante el barrido
y se reanuda después sin descartar su fase pendiente. Repite cada10segundos
entre inicios, sujeto a que termine la transferencia en curso. Si excepcionalmente
un barrido dura más de10s, termina antes de iniciar el siguiente, sin solaparlos.
R/r adelanta el siguiente barrido; si se recibe durante uno, espera a que termine.

En el primer barrido se imprimen direcciones con ACK y errores, omitiendo la
lista normal de direcciones vacías (NACK), más un resumen inicial. Después se
compara por dirección resultado, estado TWI y niveles/recovery: solo se imprimen
cambios. Una dirección que deja de responder se marca `LOST`; una recuperación
también se informa. No se repiten encabezados o resúmenes de barridos iguales.
Un cambio en `tx_dropped` también habilita el resumen. Silencio no significa
que el escáner se haya detenido. Repetir con R no fuerza salida si nada cambió.

- `ADDR7`: dirección de 7 bits; `WRITE`: byte de dirección con R/W=0.
- `ACK FOUND`: ACK de dirección y STOP completado. No identifica controlador
  ni demuestra encendido/compatibilidad OLED.
- `NACK`: dirección sin ACK y STOP completado; por sí solo no inicia recovery
  en un probe. Las escrituras de producto conservan ERROR/recuperación previos.
- `TIMEOUT`: transferencia aceptada sin completar en 40 ms o admisión ocupada
  100 ms. `BUS_ERROR`: otro fallo de inicio/transferencia.
- `SDA(D20)`/`SCL(D21)`: niveles al reportar, posiblemente durante recovery
  tras un error; no son captura de osciloscopio ni medida de tensión.
- `TWSR`: último estado de ISR retenido. FF=sin estado desde el último intento
  que reinició el campo; 18=ACK dirección; 20=NACK dirección; 08=START;
  38=arbitraje perdido; 00=error de bus. Puede conservar un estado anterior
  durante recovery; interpretarlo junto al resultado y recovery.
- `recovery`: 0 ninguna, 1 activa, 2 fallida.
- Resumen `END found/nack/faults` por barrido. `tx_dropped`: caracteres
  descartados desde arranque por timeout TX (10 ms); si es distinto de cero,
  la salida está incompleta. Watchdog activo y admisión/transferencias acotadas.

Ejemplo **ilustrativo, no resultado medido**:

```text
ADDR7=0x3C WRITE=0x78 ACK FOUND SDA(D20)=1 SCL(D21)=1 TWSR=0x18 recovery=0
```

0x78 de escritura equivale a 0x3C, dirección actual del producto. 0x7A equivale
a 0x3D; 0x7C a 0x3E. La corrección del usuario («creo que es 0x78») sigue sin
confirmarse físicamente; no se cambia la dirección del producto por este dato.

Si hay solo NACK y ningún ACK, revisar alimentación, masa, continuidad,
SDA/SCL y pull-ups. Si hay faults/timeouts, no se establece ausencia de
dispositivos: resolver el bus primero. Con ACK en 0x3C, investigar después
controlador/secuencia de encendido. No hay consulta física ejecutada aquí.

Semántica TWI: [avr-libc, util/twi.h](https://avrdudes.github.io/avr-libc/avr-libc-user-manual-2.2.0/group__util__twi.html).

## Encoder y pulsador — revisión20

La misma captura por IRQ y cola del producto atiende D2/D3; main consume las
transiciones y llama al decoder existente. No hay impresión ni acciones en ISR.
Esta imagen usa 4 transiciones por paso y sentido sin invertir; no lee EEPROM
ni carga preferencias del auto. CLK/DT/SW usan los pull-ups internos por defecto
de la capa board, a diferencia de SDA/SCL. Pulsador cierra a masa.

Cada línea `ENC` informa:

- `A(+)` y `B(-)`: pasos en sentidos opuestos desde el último reporte. Se
  cuentan por separado para conservar un giro de ida y vuelta. No se asume
  cuál es horario/antihorario hasta observar el montaje.
- `CLK(D2)`, `DT(D3)` y `SW(D42)`: últimos niveles muestreados. SW=0 es
  presionado; `PRESIONADO`/`SUELTO` es el estado con antirrebote de 20 ms.
- `press`/`release`: cambios de estado estable en el intervalo.
- `CORTO`: clic emitido al soltar; `LARGO`: pulsación de al menos 800 ms.
  Son excluyentes para una pulsación; soltar después de largo no emite corto.
  Si arranca con SW apretado debe soltarlo antes de emitir corto/largo.
- `edges`: transiciones capturadas; `invalid`: cambios de ambos bits a la vez,
  que reinician el paso parcial; `overflow`: cola desbordada, descarte y
  resincronización. Con overflow no se garantiza la cantidad real de pasos.

Se agrega actividad hasta 10 reportes por segundo, sin impresión en reposo;
los contadores se reinician después de imprimir y saturan en65535.
Una línea puede mostrar A y B a la vez si hubo ida/vuelta en el intervalo.
El muestreo de SW apunta a 1 ms; la salida serie en main introduce intervalos
sin muestreo. Pulsos muy breves pueden perderse; no es analizador lógico.
El barrido I2C continúa en el mismo bucle. La depuración no abre menús, confirma
acciones, guarda preferencias ni mueve motores.

Ejemplo ilustrativo, **no medición**: `ENC A(+)=1 B(-)=0 ... CORTO=0 LARGO=0`
al completar un paso en A; al girar en sentido contrario aumenta B. `edges`
sin pasos completos permite ver actividad con un encoder de otra resolución,
pero no acredita por sí sola que esté bien cableado o sin rebote.

## Prueba activa OLED — revisión20261029-07

`OLED 0x3C INIT ACK` significa que la secuencia de comandos obtuvo ACK; no
certifica que haya luz. Se envían barras verticales que cambian cada1s, con
orientación180grados. Durante el barrido se congela el patrón brevemente.
`OLED 0x3C TRANSFER ERROR` informa un fallo; errores idénticos consecutivos se
silencian y se reintenta después de1s. Un cambio de estado/error vuelve a imprimirse.

Para reproducir la comparación con Wire se compila el diagnóstico con
`AFR2_TWI_INTERNAL_PULLUPS=1`, a100kHz y espera500ms. Mantiene modo de páginas
porque nuestro driver transmite por páginas; no se copia modo horizontal sin
cambiar el direccionamiento de datos. Añade desactivar scroll antes de encender.
No usa Arduino Core/Adafruit dentro del firmware, ni ejecuta dos dueños del bus.
Para una comparación posterior sin pull-ups internos, generar explícitamente
con `make -B i2c-diagnostic-firmware DIAG_PULLUPS=0`; el script habitual usa1.
No se ejecutaron builds ni pruebas de estas variantes por el agente.
