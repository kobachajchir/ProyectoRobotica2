# Evidencia de diagnóstico ESP

AFR2-20261027-25: [ESP8266EX y flash 4 MiB por USB–TTL en COM5](20261027-com5-usbttl-user-report/report.md).
Consultas chip_id/flash_id ejecutadas por el usuario y aportadas en la conversación;
salidas conservadas con atribución. Sin carga ni verificación de contenido.
El estado del informe de 20261023 que sigue es histórico.

`tools/esp_probe.py` crea aquí un directorio por intento, con informe Markdown, JSON y logs. Empezar por [ESP_DIAGNOSTICO.md](../ESP_DIAGNOSTICO.md).

AFR2-20261023-13: [comprobación del entorno](20261023-155540-554529-6488/report.md) ejecutada, con Python 3.11.7, pyserial 3.5 y esptool 4.5.1/CLI disponibles. Solo se enumeraron puertos Bluetooth, sin abrirlos. No hay mediciones de conexión ESP, flash ni partición. Conservar los informes de éxitos y fallos para retomar el trabajo. Los binarios temporales no se versionan ni constituyen un backup.
