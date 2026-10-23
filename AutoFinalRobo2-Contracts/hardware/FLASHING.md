# Carga con el USB del Mega y shield OSOYOO UART

Diseño de conexión, no procedimiento ejecutado. Leer [evidencia del shield](WIFI_SHIELD.md) antes de aplicar. Fuentes: [arranque ESP8266](https://docs.espressif.com/projects/esptool/en/latest/esp8266/advanced-topics/boot-mode-selection.html), [esquema Mega Rev3](https://www.arduino.cc/en/uploads/Main/arduino-mega2560_R3-schematic.pdf) y [mapa del selector](wifi-shield/wifi-pin-map.png). El puente de la placa instalada y la ruta de niveles deben identificarse antes de convertir esto en instrucciones físicas finales.

## Tres modos distintos

| Modo | ATmega | Ruta serie |
| --- | --- | --- |
| Operación | Ejecuta firmware propio | TX1/D18 → E_RX; E_TX → RX1/D19. Selectores D4/D5 y D0/D1 aislados; Bluetooth desconectado |
| Carga ESP por USB Mega | RESET Mega a GND; no el reset del puente USB | Salida puente en nodo D0/RX0 → RX ESP; TX ESP → entrada puente en nodo D1/TX0. UART1 y otros emisores aislados |
| Carga ATmega | Quitar RESET forzado; usar bootloader existente | USB ↔ USART0 Mega D0/D1; ESP aislado de esos nodos. Restaurar después la ruta operativa por USART1 |

## Por qué no basta mover los jumpers a RX/TX

RESET del Mega no crea un puente USART0-USART1. En el esquema Mega Rev3, el USB-serie transmite al nodo D0/RX0 y escucha al nodo D1/TX0. Los rótulos corresponden al Mega; desde el puente el sentido es inverso.

El selector normal a hardware UART une E_RX con D1/TX y E_TX con D0/RX. Esa disposición sirve para Mega ejecutando código y hablando con ESP, pero no es la conexión necesaria al mantener Mega en reset y usar el puente USB: enfrentaría salidas en D0 y entradas en D1. Retirar esos jumpers y definir el cruce D0 → RX ESP / TX ESP → D1 a través de una ruta con niveles adecuados. No conectar en paralelo el selector y el puerto directo. No asignar posiciones físicas finales sin identificar la revisión y el trazado.

El shield declara regulación y adaptación integradas; aprovecharlas solo en la ruta que realmente las contiene. La cabecera directa `ESP UART` (GND, ESP TX, ESP RX, 3V3) se trata como 3,3 V. Si se usa directamente desde un puente con salida de 5 V, necesita adaptación apropiada hacia ESP RX; comprobar también el umbral del receptor del puente para ESP TX. Masa común, una alimentación definida y capacidad suficiente. No alimentar el ESP desde el pin 3,3 V de baja capacidad del Mega.

## Arranque manual ESP

OSOYOO identifica KEY y RST como controles de programación/reset. La hipótesis de conexión es KEY→GPIO0 y RST→reset ESP; falta comprobarla en la unidad. Con esa ruta confirmada: mantener KEY, pulsar y soltar RST, luego soltar KEY para entrar al cargador serie. Para ejecutar, KEY suelto y otro reset. Esto describe la secuencia prevista; no se ejecutó.

Espressif requiere GPIO0 bajo al reset para descarga y alto para arranque de flash; GPIO15 bajo, GPIO2 alto mediante pull-up, EN alto. No unir GPIO2 rígidamente a VCC. DTR/RTS del puente Mega no se presumen conectados a GPIO0/EN del shield: KEY/RST no acreditan auto-reset compatible con esptool.

## Preparación posterior

Identificar puerto, puente USB, flash real y modo SPI antes de fijar comandos. La ficha nominal del ESP-12S indica 4 MiB, no un layout ya elegido. No usar offsets ni binarios AT antiguos como plantilla de partición. Firmware Arduino y LittleFS tienen imágenes/layout coordinados; cambiar ese reparto puede afectar datos. No autorizar erase/format automático por un error de montaje.

No se descargaron ejecutables Flash Download Tool, no se cargó firmware AT, no se ejecutó esptool, ni se abrió un puerto serie. Un .hex C/avr-libc puede cargarse por el bootloader Mega conservado sin framework Arduino; ISP/fuses no forman parte de esta tarea. Tras programar ESP, el parser de aplicación deberá resincronizar frente a bytes de arranque y negociar versión/capabilities antes de habilitar órdenes.

Seguimiento: [AFRDOC-007](../documentation/review/FINDINGS.md#afrdoc-007), [012](../documentation/review/FINDINGS.md#afrdoc-012), [013](../documentation/review/FINDINGS.md#afrdoc-013).
