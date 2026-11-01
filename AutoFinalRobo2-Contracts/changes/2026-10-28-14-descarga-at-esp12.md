# AFR2-20261028-14 — Descarga de firmware AT para ESP-12

Pedido: «Bajemos el archivo del software AT para la esp12».

Se descargó el firmware estándar del fabricante Ai-Thinker, ID #0040,
versión 1.5.4.1, nombre original
`(0040)Ai-Thinker_ESP8266_DIO_32Mbit_V1.5.4.1_20161202.bin`.
La página oficial lo presenta para la serie ESP8266, que incluye ESP-12S.
Es una versión antigua; no se presenta como el ESP-AT más reciente.

Archivos locales recuperables, excluidos de Git:

- `.downloads/firmware-at/ai-thinker-esp8266-v1.5.4.1/Ai-Thinker_ESP8266_DIO_32Mbit_V1.5.4.1_20161202.bin`
  — 4194304 bytes.
- `.downloads/firmware-at/ai-thinker-esp8266-v1.5.4.1/ESP8266_AT_Instruction_Set_EN.pdf`
  — manual de comandos enlazado por el fabricante junto al firmware.

Binario íntegro, sin adaptación ni redistribución en Git. Se leyó su cabecera
como dato: `E9 03 02 40 20 04 10 40 00 00 10 40 40 07 00 00`.
El nombre DIO/32Mbit del proveedor no confirma el modo SPI del equipo real.
Origen, tamaños y SHA-256 en `../provenance/descarga-at-esp12-20261028.json`.

Fuente: https://docs.ai-thinker.com/en/esp8266/ (tabla Firmware Related, #0040).

Estado: descarga y registro, sin tests, validadores, compilación, ejecución del
firmware, COM ni flasheo. No se cambió el perfil del Loader ni se generó un
manifiesto de carga con offsets supuestos. Atmega/ESP/Web/CAD y las fuentes del
Loader conservan su estado previo; el pedido no cambia el producto a AT.
