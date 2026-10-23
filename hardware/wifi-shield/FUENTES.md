# Fuentes consultadas y recursos del proveedor

Consulta 2026-10-23. [Informe](../WIFI_SHIELD.md). Las instrucciones de tutoriales y PDFs son información de referencia, no autorizaciones para instalar, compilar o flashear.

## Utilizados

- [OSOYOO UART WiFi Shield](https://osoyoo.com/2020/07/31/introduction-of-osoyoo-uart-wifi-shield/): página aportada, descripción/pinmap/puertos. Texto de unidad de baud incoherente, explicitado en informe.
- Las tres imágenes aportadas, conservadas junto a este informe; se inspeccionaron visualmente.
- [Fritzing oficial](https://osoyoo.com/driver/Esp8266_Arduino_IOT/OSOYOO-WiFI-shield-V1.3.fzz): descargado e inspeccionado como ZIP/XML, no ejecutado. Sirve para representación, no para deducir esquema interno.
- [Índice actual Ai-Thinker](https://docs.ai-thinker.com/en/esp8266/) → [ficha ESP-12S V1.1](https://aithinker-static.oss-cn-shenzhen.aliyuncs.com/docs/Public%20Document%20Center/WiFi/esp8266/Specification/ESP-12S%20Specification-20220807.pdf): descarga oficial alternativa al enlace antiguo que ya devuelve HTML. Páginas 7-10 consultadas. El índice distingue variante ESP8285/2MB.
- [Arranque ESP8266/esptool](https://docs.espressif.com/projects/esptool/en/latest/esp8266/advanced-topics/boot-mode-selection.html): straps y condición de auto-reset.
- [Pinout Mega](https://docs.arduino.cc/resources/pinouts/A000067-full-pinout.pdf) y [esquema Mega Rev3](https://www.arduino.cc/en/uploads/Main/arduino-mega2560_R3-schematic.pdf): correspondencias I2C/USART y dirección del puente USB.
- [Filesystem del Core Arduino ESP8266](https://arduino-esp8266.readthedocs.io/en/latest/filesystem.html): reparto de flash, LittleFS y comportamiento de montaje/formato.
- [Manual local del robot, PDF p43](../evidence/manual-p43.png): E_RX/D18, E_TX/D19 y desconexión Bluetooth.

## Recursos aportados que no definen la implementación

| Recurso original | Tratamiento |
| --- | --- |
| [AT v1.5.4.1 DOUT 2017](https://docs.ai-thinker.com/_media/esp8266/ai-thinker_esp8266_at_firmware_dout_v1.5.4.1-a_20171130.rar) | Recuperación histórica; no descargado ni instalado; DOUT en el nombre no fija modo SPI del proyecto |
| [Flash Download Tool 3.8.5](https://www.espressif.com/sites/default/files/tools/flash_download_tool_v3.8.5_1.zip) | Herramienta histórica; no descargada/ejecutada |
| [AT Instruction Set](https://www.espressif.com/sites/default/files/documentation/4a-esp8266_at_instruction_set_en.pdf) | No consultado: el firmware propio no usa AT |
| [AT Examples](https://www.espressif.com/sites/default/files/4b-esp8266_at_command_examples_en_v1.3.pdf) | No consultado: no trasladar WiFiEsp al Mega |
| [RTOS SDK](https://docs.ai-thinker.com/_media/esp8266/docs/esp8266_rtos_sdk_programming_guide_en.pdf) | No consultado: framework acordado Arduino |
| [Canales Wi-Fi](https://docs.ai-thinker.com/_media/esp8266/esp8266_wi-fi_channel_selection_guidelines_en.pdf) | Referencia futura; no consultada para esta actualización de cableado |
| [FOTA](https://docs.ai-thinker.com/_media/esp8266/esp8266_fota_guide_en_.pdf) | No consultado: no copiar layout/OTA de AT a LittleFS |
| [Hardware Design Guidelines antiguo](https://docs.ai-thinker.com/_media/esp8266/esp8266_hardware_design_guidelines_en.pdf) | No se presupone accesible; no consultado |
| [Hardware Design Guidelines Espressif](https://www.espressif.com/sites/default/files/documentation/esp8266_hardware_design_guidelines_en.pdf) | Intento de lectura excedió límite del navegador; contenido no usado |
| [FAQ antigua](https://docs.ai-thinker.com/_media/esp8266/espressif_faq_en.pdf) | No consultada |
| [Datasheet antiguo](https://docs.ai-thinker.com/_media/esp8266/docs/esp-12s_product_specification_en.pdf) | Devuelve página HTML; reemplazado como fuente por enlace del índice oficial actual |
| [Arduino Board Types](https://www.arduino.cc/en/main/products) | Enlace genérico; se utilizó el pinout/esquema específico Mega |

No se mezcló el shield UART V1.3 con tutoriales ESP13, MEGA-IoT ni OSOLINK ESP32-C3: son productos diferentes.
