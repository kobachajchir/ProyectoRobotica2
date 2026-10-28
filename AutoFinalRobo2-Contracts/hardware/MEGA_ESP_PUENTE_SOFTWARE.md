> **Actualización AFR2-20261028-05:** Loader0.9 reemplaza la recuperación por copia HEX por backup opcional leído del equipo y menú de firmware/último backup. [Plan vigente](PLAN_PRUEBA_FLASHER_MEGA.md) · [Ficha05](../changes/2026-10-28-05-loader-mega-backups.md). Implementado solo en fuente, sin compilar, abrir COM o cargar. Las notas03 inferiores describen el mecanismo anterior.

# ESP por el USB-B del Mega: puente por software en D18/D19

> **Integración AFR2-20261028-03:** Loader0.8 incorpora instalación, copia del
> HEX y restauración, baud fijo y transporte software. [Plan A/B](PLAN_PRUEBA_FLASHER_MEGA.md).
> Solo fuente, sin compilar/ejecutar ni COM. Los pendientes de02 inferiores
> describen el estado previo a03.

AFR2-20261028-02, 28/10/2026. Investigación y fuente de servicio preparada;
**sin compilación, consultas serie, escritura ni validación física**.

## Conclusión y evidencia

La alternativa es técnicamente viable: PC ↔ puente USB de la placa ↔ USART0 del
ATmega2560 ↔ reenvío de bytes ↔ USART1 ↔ ESP8266. Arduino publica el concepto
[SerialPassthrough](https://docs.arduino.cc/built-in-examples/communication/SerialPassthrough/).
La versión propuesta usa C/avr-libc y ambas UART hardware. No sustituye el USB
por código: reutiliza el conversor USB-serie que ya tiene el Mega y agrega el
reenvío desde su microcontrolador principal.

Ya existe [identificación del ESP por USB–TTL aportada por el usuario](diagnostics/20261027-com5-usbttl-user-report/report.md):
ESP8266EX/4 MiB a 9600, sin nueva escritura. El usuario aclaró que había empleado
los mismos pines del shield con ambos adaptadores; no se atribuye el éxito a un
cambio de cabecera. El puente por software cambia el emisor/receptor que habla
con ESP, pero no demuestra que la ruta de niveles esté correcta ni garantiza
resolver el fallo anterior por COM8/Mega.

## La imagen aportada y las resistencias

La foto representa UNO + ESP-01, con ATmega328 retirado. Es puente físico USB,
no passthrough por software ni el mapa del Mega/ESP-12S. Sus resistencias forman
un divisor para **la señal lógica** del emisor de 5 V hacia RX del ESP:

```text
TX de 5 V ── R1 = 1 kΩ ──┬── RX directo del ESP (GPIO3)
                         │
                       R2 = 2 kΩ
                         │
                        GND
```

Ejemplo calculado: V_RX = 5 × 2 / (1 + 2) = 3,33 V, sin carga. Valores de 1 %,
masa común y cables cortos son una base de diseño; no una medición del shield.
Las bandas del dibujo parecen corresponder a 1 kΩ/2,2 kΩ, pero no se certifican
desde esa imagen. Esa pareja daría 3,4375 V nominales a 5 V de entrada; no es
exactamente 3,3 V. No agregar el divisor sobre una ruta que ya traduzca niveles.

No usar resistencias como fuente de alimentación del ESP ni unir EN/CH_PD a la
señal UART. EN debe permanecer en su nivel de habilitación según el circuito.
El dibujo tampoco demuestra una alimentación suficiente desde el pin 3V3 del
Arduino. Conservar la regulación del shield identificada; ESP debe tener una
fuente de 3,3 V adecuada, sin unir salidas de fuentes entre sí.

[OSOYOO declara traducción de niveles integrada](https://osoyoo.com/2020/07/31/introduction-of-osoyoo-uart-wifi-shield/),
pero el recorrido real selector/módulo sigue pendiente. Cabecera directa ESP
UART y centrales E_RX/E_TX no se tratan como equivalentes eléctricos. Si se
usa RX directo, adaptar 5→3,3 V una sola vez y aislar otros emisores. La vuelta
ESP TX→Mega RX1 requiere comprobar umbrales/márgenes o usar adaptación apropiada;
la tensión nominal de 3,3 V no certifica todos los márgenes bajo carga.

Referencias eléctricas: [ESP8266EX](https://documentation.espressif.com/0a-esp8266ex_datasheet_en.html),
[diseño de alimentación ESP8266](https://documentation.espressif.com/esp8266_hardware_design_guidelines_en.html),
[ATmega2560](https://www.microchip.com/en-us/product/ATmega2560).

## Dos modos incompatibles de conexión

| Elemento | Puente físico anterior | Puente por software propuesto |
| --- | --- | --- |
| RESET del Mega | A GND | **Libre**, Mega ejecutando la imagen de servicio |
| Enlace ESP | D0/RX0→E_RX, E_TX→D1/TX0 | **D18/TX1→E_RX, E_TX→D19/RX1** por ruta de niveles identificada |
| Cables de servicio D0/D1 al ESP | Necesarios para esa ruta | Retirados; D0/D1 quedan para USB↔Mega |
| Jumpers paralelos D0/D1 o D4/D5 | Aislados salvo conexión definida | Retirados; Bluetooth aislado de la UART |
| KEY/RST del shield | Manuales | Manuales, solo afectan al ESP |
| Firmware del Mega | Detenido | Sustituido temporalmente por el puente |

Con el puente por software se conserva el cableado operativo de D18/D19. No
mantener RESET a masa: con el Mega detenido no existe reenvío USART0↔USART1.
Los sensores en A0–A4 no constituyen ese enlace y no deben recablearse como UART.

## Velocidad y timeouts

**115200 bit/s es el candidato de carga; 9600 es una alternativa diagnóstica.**
[esptool admite otras velocidades](https://docs.espressif.com/projects/esptool/en/latest/esp8266/esptool/basic-options.html).
No hay evidencia de un plazo global fijo del ESP que corte toda carga por usar
9600. Sí existen esperas por sincronización, escritura del puerto, bloques y
otras operaciones, además del límite del proceso lanzado por el Loader.
El código 4.5.1 define, entre otros, 10 s de espera de escritura serie por
defecto. Un bloque grande a una velocidad baja puede presionar ese límite:
que exista soporte de baud bajo no acredita que cualquier combinación de
stub, bloque, driver y timeout complete una imagen grande sin ajustes.

En el código esptool 4.5.1 consultado, el baud inicial es min(115200, solicitado);
si se pide más, intenta cambiar PC/ESP después de sincronizar. Un puente fijo
no sigue ese cambio: no subir a 230400/460800 con esta imagen. A 16 MHz, 115200
en el Mega da 117647,06 (+2,12 %); 57600 con U2X da 57142,86 (−0,79 %) y 38400
da 38461,54 (+0,16 %). Son alternativas calculadas si el margen a 115200 falla,
sin atribuir al baud la causa del fallo antes de consultar la ROM.

Un passthrough **no** recibe de forma automática los cambios de baud solicitados
al conversor USB. Deben coincidir baud compilado del puente y baud elegido en
esptool/lector. El mensaje ROM 74880 no atravesará legiblemente una imagen fija
a 115200. Para la primera consulta, entrar por KEY/RST manualmente y consultar
ROM directamente; no exigir que la escucha a 74880 del Loader valide ese modo.

Si se transmitieran 3121152 bytes, 8N1 y sin compresión, solo el transporte
necesitaría unos 54 min 11 s a 9600 o 4 min 31 s a 115200. Son cálculos, no
duraciones de carga: compresión, SLIP, borrado, verificación y USB modifican el
tiempo. El timeout genérico del Loader de 900 s podría cortar una transferencia
lenta larga; aumentarlo no arregla errores eléctricos, pérdidas ni baud desigual.
No cambiar esperas globales a ciegas: identificar qué etapa agotó su espera.

Fuentes: [configuración de tiempos esptool](https://docs.espressif.com/projects/esptool/en/latest/esp8266/esptool/configuration-file.html),
[CLI 4.5.1](https://github.com/espressif/esptool/blob/v4.5.1/esptool/__init__.py),
[loader 4.5.1](https://github.com/espressif/esptool/blob/v4.5.1/esptool/loader.py).
Se leyó también el código instalado, sin ejecutar esptool.

## Secuencia prevista, todavía sin ejecutar

1. Desconectar potencia de actuadores y cerrar monitores. Conservar el HEX
   completo del producto y la configuración necesaria para restaurarlo.
2. Compilar la imagen de servicio elegida y cargarla al Mega por su bootloader,
   con RESET libre. Esto reemplaza la aplicación del auto; no sus fuses/EEPROM.
3. Mantener D18→E_RX y E_TX→D19, sin conexiones paralelas. Tras abrir el COM,
   contemplar que DTR/driver puede reiniciar el Mega y pasar por su bootloader.
   No interpretar la ausencia de banner como fallo: el puente es binario y mudo.
4. KEY mantenido, pulsar/soltar RST **del ESP**, soltar KEY. GPIO0 debe estar
   bajo en el reset; no confundirlo con RESET del Mega.
5. Consultar chip/flash a 115200, sin reset automático y inicialmente sin stub.
   Ejemplo para esptool 4: `--chip esp8266 --port COMx --baud 115200 --before
   no_reset --after no_reset --no-stub --connect-attempts 100 flash_id`.
   En 5 se usan nombres con guiones. Consultar no escribe flash.
6. Solo tras una respuesta estable, preparar escritura del firmware/filesystem
   con sus offsets/layout y verificar contenido. Una consulta no prueba que el
   puente soporte aún bloques largos/stub. No ejecutar erase_flash por defecto.
7. Restaurar el HEX completo del Mega y verificarlo; después KEY libre y reset
   ESP para arrancar desde flash, con cableado de operación. No hace falta
   mantener instalada la imagen de servicio cuando el auto vuelve a funcionar.

`no_reset` evita las maniobras explícitas de esptool, no garantiza que el driver
no afecte RESET del Mega al abrir. Esperar su arranque y mantener la secuencia
manual del ESP. Referencia: [opciones de reset](https://docs.espressif.com/projects/esptool/en/latest/esp8266/esptool/advanced-options.html).

## Entrega y continuación

[Fuente de servicio](../../AutoFinalRobo2-Atmega/service/esp_uart_bridge.c) y
[build separado](../../AutoFinalRobo2-Atmega/service/README.md):
`esp-bridge-firmware BRIDGE_BAUD=115200`, HEX con baud en el nombre, sin cambiar
`afr2.hex`. No se generó ni cargó ningún binario. El firmware normal y su UART
de producto no se modifican para activar este servicio al arrancar.

En la entrega02 el Loader conservaba solo el asistente físico RESET→GND,
incompatible con esta alternativa. La entrega03 implementa transporte software,
instalación/verificación del servicio, baud fijo, consulta ROM sin esperar74880,
espera tras abrir COM y restauración visible. Sigue pendiente compilar/ejecutar
y acreditar ese comportamiento con hardware.

Como segunda etapa se puede integrar un modo de servicio persistente en el
firmware del auto, con entrada local/autorizada y salidas desactivadas. Eso
evitaría cargar/restaurar un programa cada vez, pero requiere diseño de entrada,
salida y recuperación: no usar un patrón ambiguo que pueda dispararse por tráfico
binario normal ni anunciarlo como función existente.
