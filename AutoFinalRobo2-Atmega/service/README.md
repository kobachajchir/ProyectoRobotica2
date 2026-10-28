> **Integración vigente AFR2-20261028-05:** Loader0.9 agrega backup opcional leído del Mega antes de cargar este servicio y botón Flashear Firmware ATmega con menú Flashear último backup. [Plan A/B](../../AutoFinalRobo2-Contracts/hardware/PLAN_PRUEBA_FLASHER_MEGA.md). Fuente AVR conservada, sin compilar/cargar/probar esta revisión. La descripción de copia HEX de03 inferior es histórica.

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
