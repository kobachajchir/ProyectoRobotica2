> **Loader vigente AFR2-20261028-05 / 0.9.0:** [plan A/B actualizado](PLAN_PRUEBA_FLASHER_MEGA.md), backup leído opcional, aviso y Preferencias; Flashear Firmware ATmega con menú Flashear último backup. [Ficha05](../changes/2026-10-28-05-loader-mega-backups.md). Solo fuente/documentación; no se compiló, abrió COM ni flasheó. Sustituye la copia HEX de03.

> **AFR2-20261027-27:** el usuario completó una carga manual de firmware ESP
> por USB–TTL COM5/115200 con esptool4.5.1, 437936 bytes y hash verificado.
> Mantuvo KEY durante la carga. [Comando, salida y límites](../changes/2026-10-27-27-loader-esp-command-key.md).
> En esta unidad, mantener KEY hasta terminar consulta/carga/verificación,
> soltando solo RST tras pulsarlo. Loader0.7.1 adaptado en fuente, sin compilar
> ni ejecutar esta revisión; sin arranque, Wi-Fi o carga LittleFS acreditados.

> **Preparación AFR2-20261027-10:** [guía específica de carga USB-B/COM16](CARGA_USB_COM16.md). ATmega/ESP/Web compilados, Web esp promovida e imagen LittleFS generada. Hall habilitado en build por pedido humano; configuración EEPROM conservada. No se abrió puerto ni se cargó a placas; jumpers y flash real aún sin identificar. Los estados de no imagen FS inferiores son históricos.

# Carga con el USB del Mega y shield OSOYOO UART

> **Integración AFR2-20261028-03:** [plan A/B](PLAN_PRUEBA_FLASHER_MEGA.md), instalar/restaurar flasher en Loader0.8, baud fijo115200 candidato. RESET Mega **libre**, D18/D19. Fuente sin compilar/cargar/probar. El puente físico RESET-GND inferior es otro transporte; no hay cambio automático de montaje ni restauración al cerrar.

> **Diagnóstico AFR2-20261027-25:** consultas externas por USB–TTL en COM5
> aportadas por el usuario identifican ESP8266EX y flash **4 MiB**. [Informe](diagnostics/20261027-com5-usbttl-user-report/report.md).
> La ruta por Mega/selector COM8 no sincronizó en los intentos documentados;
> la causa exacta sigue sin aislar. Esta consulta no valida el layout 4m3m,
> el modo/frecuencia SPI ni una carga de imágenes. Sin flasheo de esta ficha.

Actualización de configuración **AFR2-20261024-04**: el usuario pidió incluir el
modelo final en ESP y reservar3MiB para LittleFS, suponiendo flash4MiB.
`ESP/platformio.ini` selecciona `eagle.flash.4m3m.ld`; firmware compilado con
éxito y Web de1.450.201 bytes gzip promovida. Es un layout candidato explícito,
no una identificación física. Sin imagen FS ni carga. Confirmar flash/SPI y
preservar configuración antes de un futuro flasheo; el programa tiene su límite
separado de1.044.464 bytes. [Evidencia](../changes/2026-10-24-04-final-model-littlefs.md).

[Herramienta de diagnóstico previa](ESP_DIAGNOSTICO.md), AFR2-20261023-12: `tools/esp-probe.ps1` comprueba conexión y consulta flash/cabecera sin escribir firmware; guarda evidencia en `hardware/diagnostics/`. Se lanza cuando esté cableada la placa, siguiendo las rutas y niveles de este documento. El script no determina por sí solo offsets de LittleFS ni autoriza un flasheo.

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

OSOYOO identifica KEY y RST como controles de programación/reset. La hipótesis de conexión es KEY→GPIO0 y RST→reset ESP; no hay medición de continuidad de la unidad. El usuario aportó una carga manual completa con esta secuencia: mantener KEY, pulsar y soltar solo RST y conservar KEY apretado durante toda la carga. El procedimiento vigente de esta unidad mantiene KEY hasta terminar la operación, incluida la verificación; cerrar el asistente o recibir modo1 no indica soltarlo. Para ejecutar después de terminar, soltar KEY y resetear el ESP. La evidencia manual no es ejecución del agente ni prueba del Loader corregido.

Espressif requiere GPIO0 bajo al reset para descarga y alto para arranque de flash; GPIO15 bajo, GPIO2 alto mediante pull-up, EN alto. No unir GPIO2 rígidamente a VCC. DTR/RTS del puente Mega no se presumen conectados a GPIO0/EN del shield: KEY/RST no acreditan auto-reset compatible con esptool.

## Preparación posterior

Identificar puerto, puente USB, flash real y modo SPI antes de fijar comandos. La ficha nominal del ESP-12S indica 4 MiB, no un layout ya elegido. No usar offsets ni binarios AT antiguos como plantilla de partición. Firmware Arduino y LittleFS tienen imágenes/layout coordinados; cambiar ese reparto puede afectar datos. No autorizar erase/format automático por un error de montaje.

No se descargaron ejecutables Flash Download Tool, no se cargó firmware AT, no se ejecutó esptool, ni se abrió un puerto serie. Un .hex C/avr-libc puede cargarse por el bootloader Mega conservado sin framework Arduino; ISP/fuses no forman parte de esta tarea. Tras programar ESP, el parser de aplicación deberá resincronizar frente a bytes de arranque y negociar versión/capabilities antes de habilitar órdenes.

Seguimiento: [AFRDOC-007](../documentation/review/FINDINGS.md#afrdoc-007), [012](../documentation/review/FINDINGS.md#afrdoc-012), [013](../documentation/review/FINDINGS.md#afrdoc-013).
