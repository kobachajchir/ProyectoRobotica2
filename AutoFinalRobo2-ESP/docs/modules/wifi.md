# Wi-Fi y preferencias LittleFS

[Maestro](../../../INFORME_MAESTRO.md) · [Informe del repositorio](../../INFORME_ADAPTACION.md).

## Utilidad

Conservar estados STA/AP, scan asíncrono, reintento/fallback, cache de preferencias y escrituras temporales. Sin comandos AT.

## Adaptación

Configurar identidad AutoFinalRobo2 y provisioning sin defaults privados. ESP12S flash/mode/partición pendientes; no heredar combinación board esp01_1m con linker4m3m a ciegas. Preservar configuración al actualizar assets; evitar formato automático ante fallo FS. Confirmar GPIO LED/boot del shield. Los ajustes de red no bloquean control AVR.

## Evidencia y alcance

- [reference/esp01/src/wifi_service.cpp](../../reference/esp01/src/wifi_service.cpp)
- [reference/esp01/src/wifi_scan_arbiter.c](../../reference/esp01/src/wifi_scan_arbiter.c)
- [reference/esp01/src/nvs_littlefs.cpp](../../reference/esp01/src/nvs_littlefs.cpp)
- [reference/esp01/src/boot_mode.cpp](../../reference/esp01/src/boot_mode.cpp)

Hallazgos: AFRDOC-007. [Registro](../../../AutoFinalRobo2-Contracts/documentation/review/FINDINGS.md).

Estado: lectura/copia o fuente parcial indicada; sin compilación, tests ni prueba física. La API detallada de origen está en el corpus AMDOC importado; no equivale a API AVR adaptada.


Fuente vigente AFR2-20261023-11: src/wifi_service.* y storage.* implementan STA/AP/AP_STA, scan, fallback y persistencia propia con temporal+rename y autoformat=false. Ver PROTOCOL.md secciones 8/9. El AP de aprovisionamiento inicial es abierto y no contiene secretos. Sin compilar ni probar; flash/layout siguen pendientes.
