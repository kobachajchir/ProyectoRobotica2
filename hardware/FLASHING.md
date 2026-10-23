# Carga con el USB del Mega

Propuesta no ejecutada. Identificar jumpers/rutas/niveles/alimentación del shield real. Fuentes: [modo de arranque ESP8266](https://docs.espressif.com/projects/esptool/en/latest/esp8266/advanced-topics/boot-mode-selection.html), [esquema Mega Rev3](https://www.arduino.cc/en/uploads/Main/arduino-mega2560_R3-schematic.pdf).

| Modo | ATmega | ESP |
| --- | --- | --- |
| Operación | Ejecuta firmware | UART0 ESP ↔ USART1 Mega D18/D19 |
| Carga ESP | RESET Mega a GND | ESP aislado de UART1 y conectado al puente USB-serie |
| Carga Mega | Quitar reset forzado; bootloader | Aislar ESP de UART0 Mega; restaurar UART1 después |

Reset NO crea puente USART0-USART1. En Mega Rev3 original, el USB-serie conduce el nodo D0/RX0 y recibe por D1/TX0: salida del puente/nodo D0 → RX ESP con conversión 5→3,3 V; TX ESP → entrada del puente/nodo D1 con nivel aceptado por ese convertidor. La serigrafía RX/TX corresponde al Mega, no al puente. Clones/shields pueden cambiarlo: seguir señales reales.

ESP: EN y reset inactivo altos; GPIO15 bajo, GPIO2 con pull-up; GPIO0 bajo al reiniciar para programar. Restaurar GPIO0 alto y reset para ejecutar. No unir GPIO2 rígidamente a VCC. GPIO/UART ESP no reciben 5 V; usar fuente 3,3 V suficiente. El pin 3,3 V de baja capacidad del Mega no es fuente adecuada para ESP. El shield podría resolverlo, pero se debe identificar.

DTR del Mega no controla automáticamente GPIO0/EN del ESP: puede requerir reset manual. No se fijó COM ni se ejecutó upload. El parser deberá resincronizar después de mensajes ROM del ESP.

Un .hex C puede cargarse por el bootloader Mega sin framework Arduino. Conservar bootloader/límites; no tocar ISP/fuses por implicación. LittleFS y firmware son cargas separadas y un cambio de partición puede afectar preferencias. Preparar comandos concretos tras identificar flash, puerto y bootloader.
