> **AFR2-20261029-18 compilado SIN cargar:** diagnóstico BuildOnly exit0 y producto/comunicación/biblioteca Loader exit0, sin errores/advertencias. Diagnóstico flash13800/SRAM3180bytes; producto flash75600/SRAM6934bytes, margen1258bytes sin certificación dinámica de pila. Producto final12:34:40 del29/10/2026. Placa conserva15; no se abrió COM ni se ejecutó ensayo físico. Web solo fuente. Evidencia en Contracts/documentation/builds/2026-10-29-18/README.md; pendientes inferiores históricos. La prohibición de flashear sigue vigente.

> **Fuente AFR2-20261029-18:** IR1..IR5 OSOYOO digital (A4..A0), perfil predeterminado habilitado y EEPROMv4 migra solo tipos antiguos deshabilitados; calibración/seguimiento siguen pendientes. Diagnóstico servo/encoder agrega lectura RAW directa y cambios por OLED/serie, sin EEPROM. Vistas IR/Web distinguen deshabilitado/pendiente/cero válido. [Ficha](../../AutoFinalRobo2-Contracts/changes/2026-10-29-18-ir-input-acquisition.md). Compilaciones pendientes al registrar la fuente; no cargar por instrucción vigente. Placa conserva15; Web sin build/promoción.

> **Diagnóstico17 compilado, sin cargar:** usuario confirma puente S1–D13 instalado además de GND/5V/S1 del servo. BuildOnly exit0, sin errores/advertencias, sin COM/avrdude. Flash13424bytes; SRAM3169bytes, margen5023bytes, sin certificación de pila. HEX afr2-i2c-diagnostic.hex SHA256 03cf3a3760f3b6bc5b9b8a7ce128a56388a5bd51b69209a2ea39e895283ca573. Causa del servo inmóvil aún sin medición/prueba física. Producto16 no cargado; placa conserva15. Las notas de compilación/puente pendientes inferiores son históricas.

> **Diagnóstico AFR2-20261029-17:** encoder controla servo D13/OC1C en pasos5grados; PWM inicialmente OFF, C=centrar, S=apagar PWM. OLED y serie muestran orden/contador/botón; I2C cada10s. S1 ModelY necesita puente adicional aD13 según manual. Script agrega -BuildOnly (sin COM/flash). [Ficha](../../AutoFinalRobo2-Contracts/changes/2026-10-29-17-diagnostic-encoder-servo.md). Fuente lista, compilación de prueba pedida; no se autoriza carga. Producto16 sigue sin cargar; placa conserva15.

> **Integración vigente AFR2-20261028-05:** Loader0.9 agrega backup opcional leído del Mega antes de cargar este servicio y botón Flashear Firmware ATmega con menú Flashear último backup. [Plan A/B](../../AutoFinalRobo2-Contracts/hardware/PLAN_PRUEBA_FLASHER_MEGA.md). Fuente AVR conservada, sin compilar/cargar/probar esta revisión. La descripción de copia HEX de03 inferior es histórica.

# Diagnóstico OLED por USART0

**Vigente AFR2-20261029-09, solo prueba:** giro invertido respecto de08;
envío OLED intercalado con barrido para atender pulsación/liberación, cola TX
sin esperas y reenvío del framebuffer cada1s sin spam serie. Mismo script,
banner09. Fuente sin compilar/cargar/probar. [Ficha](../../AutoFinalRobo2-Contracts/changes/2026-10-29-09-diagnostic-direction-responsive.md).

**Vigente AFR2-20261029-08, solo prueba:** muestra contador que aumenta con
sentido A y disminuye con B, y PRESIONADO/SUELTO debajo. Inicial0, sin EEPROM,
sin acciones de botón, giro180 conservado. Mismo script; banner08. Solo fuente,
sin compilar/cargar/probar. No modifica menús/pantallas del auto ni Web.

**Vigente AFR2-20261029-07:** ahora inicializa OLED0x3C y envía barras móviles
a180grados, conserva encoder y barridos cada10s. Pull-ups internos como Wire
solo en este servicio. Fuente sin compilar/cargar/probar. Se usa el mismo script
y nombre HEX de diagnóstico. [Detalle](../../AutoFinalRobo2-Contracts/changes/2026-10-29-07-diagnostic-active-oled.md).

**AFR2-20261028-21:** script de compilación y carga
`../tools/flash-i2c-encoder-diagnostic.ps1 -Port COM16`, con puerto obligatorio
y rutas de herramientas configurables. Solo creado en fuente; no ejecutado.
Comando completo y requisitos en la [guía](../../AutoFinalRobo2-Contracts/hardware/OLED_I2C_DIAGNOSTICO.md#objetivo-preparado-no-ejecutado).

**Ampliación AFR2-20261028-20:** el mismo servicio agrega encoder CLK D2 / DT D3
y SW D42: sentidos A(+)/B(-), pulsación/liberación, corto/largo y errores de
captura por USART0. Solo fuente; sin compilar/cargar/probar. Conserva objetivo
y HEX previstos abajo. [Detalle](../../AutoFinalRobo2-Contracts/changes/2026-10-28-20-encoder-uart0-diagnostic.md).

**AFR2-20261028-19:** servicio separado `i2c_uart0_diagnostic.c`, búsqueda I2C
D20/SDA y D21/SCL, salida D1/TX0 y entrada D0/RX0 a **115200 8N1**.
Objetivo `make i2c-diagnostic-firmware`, salida `build/afr2-i2c-diagnostic.hex`.
Solo fuente, **sin compilar/cargar/probar**. [Guía](../../AutoFinalRobo2-Contracts/hardware/OLED_I2C_DIAGNOSTICO.md).

# Puente de servicio USB ↔ ESP por USART1

AFR2-20261028-02. Fuente candidata **sin compilar, cargar ni probar**.
No es el firmware del auto ni `afr2-communication.hex`. Implementa el equivalente
a un serial passthrough con AVR C/avr-libc, respetando la arquitectura del módulo.
No necesita Arduino Core, SoftwareSerial, librerías de motor ni el protocolo ROBO2.

`esp_uart_bridge.c` reenvía bytes entre USART0 (puente USB integrado) y USART1
(D18/TX1, D19/RX1). Dos colas de 511 bytes útiles, servicio por polling sin
interrupciones, sin mensajes en el canal ni cambios de líneas/bytes de protocolo.
Los errores de recepción se descartan y cuentan para depurador; un desborde de
cola detiene las UART hasta reset. No implica integridad garantizada: esptool
debe identificar/escribir/verificar y fallar ante datos perdidos.

Se deshabilitan watchdog heredado, temporizadores y servicios del producto.
PWM/direcciones de motores documentados se solicitan a cero. **Desconectar
potencia de actuadores antes de cargar:** el bootloader y transiciones no están
controlados por esta fuente. D13 pertenece al servo de radar, no se usa de LED.
No hay odometría, OLED, botones del menú, aplicación del robot ni EEPROM en
esta imagen. No toca GPIO0/RST del ESP: KEY/RST siguen siendo manuales.

## Generar cuando se solicite

Con GNU Make y AVR GCC configurados, desde la carpeta Atmega:

```powershell
make esp-bridge-firmware BRIDGE_BAUD=115200
```

No se ejecutó este comando. Produce `build/afr2-esp-bridge-115200.hex`, ELF y
mapa propios. No sobrescribe `build/afr2.hex`; cargar cualquiera de ellos sí
reemplaza la aplicación que ejecuta el Mega. Restaurar después el HEX completo
del producto con avrdude y verificación, sin cambiar fuses/bootloader/EEPROM.
El script `tools/upload-mega.ps1` continúa cargando el producto; no selecciona
este puente. No cambiarle el nombre al HEX para confundirlo con `afr2.hex`.

| BRIDGE_BAUD | U2X | UBRR | Baud calculado a 16 MHz | Error nominal |
| --- | --- | --- | --- | --- |
| 115200 (candidato inicial) | 1 | 16 | 117647,06 | +2,12 % |
| 57600 | 1 | 34 | 57142,86 | −0,79 % |
| 38400 | 1 | 51 | 38461,54 | +0,16 % |
| 9600 (diagnóstico) | 0 | 103 | 9615,38 | +0,16 % |

Son cálculos, no mediciones ni certificación de enlace. Recompilar/cargar otra
variante para cambiar el baud fijo de **ambas UART**. Un cambio en Windows no
reconfigura este programa. No usar `--baud 230400`/460800 con este puente fijo:
esptool sincroniza como máximo a 115200 y puede cambiar después PC/ESP, pero
el Mega seguiría a su velocidad compilada.

## Montaje y secuencia

Procedimiento, división de tensión, límites y plan de aceptación en
[MEGA_ESP_PUENTE_SOFTWARE.md](../../AutoFinalRobo2-Contracts/hardware/MEGA_ESP_PUENTE_SOFTWARE.md).
RESET del Mega **libre**, D18→E_RX central y E_TX central→D19 a través de la
ruta de niveles identificada. No puentes D0/D1 al ESP ni jumpers paralelos.

Desde AFR2-20261028-03, Loader0.8 incorpora **Cargar flasher en ATmega** y
**Restaurar firmware del auto**, y conexión `mega_bridge` con RESET libre.
`mega_reset` conserva el puente físico y no sirve para esta imagen. Integración
en fuente, sin compilar/ejecutar. [Plan A/B](../../AutoFinalRobo2-Contracts/hardware/PLAN_PRUEBA_FLASHER_MEGA.md).
