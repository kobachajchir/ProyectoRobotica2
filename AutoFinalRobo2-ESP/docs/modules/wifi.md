# Wi-Fi y preferencias LittleFS

## Configuración local — AFR2-20261027-26

Se preparó `data/config/wifi.json` con modo `STA` y las credenciales indicadas
por el usuario. Es un archivo privado, excluido de Git por la regla específica
de `.gitignore` raíz. No copiar su contenido a fuentes, informes, ejemplos ni
al paquete público `data/dist`.

El firmware existente lee `/config/wifi.json` al iniciar mediante
`Storage::loadWifi()` y aplica SSID/contraseña mediante `WifiService::begin()`.
No hizo falta cambiar el C++ ni el esquema `afr2-wifi-1`. Si la configuración
no está disponible o es inválida, se mantiene el AP de aprovisionamiento;
si STA no conecta, se conserva el fallback y el reintento existentes.

Para aplicar este archivo hay que regenerar la imagen LittleFS desde `data/`
y cargarla con el layout correspondiente al firmware. Cargar únicamente
`firmware.bin` no incorpora ni actualiza estas preferencias. Una carga completa
de filesystem reemplaza su contenido, incluidas preferencias previas.

En esta revisión solo se preparó el JSON local y se leyó la fuente: sin
compilaciones, generación de imagen FS, tests, validadores, COM ni flasheos.
La imagen LittleFS existente es anterior a esta configuración.

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
