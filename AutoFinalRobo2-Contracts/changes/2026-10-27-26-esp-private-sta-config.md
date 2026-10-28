# AFR2-20261027-26 — Configuración STA privada para el ESP

27/10/2026. El usuario pidió configurar SSID y contraseña y aclaró que
corresponden a una red existente a la que debe conectarse el ESP, modo `STA`.
Valores guardados solo en datos locales privados; no se transcriben aquí.
Adaptación asistida por Codex sobre el proyecto de Koba Chajchir; autorías
anteriores conservadas.

## Antes y después

La lectura de `src/wifi_service.cpp` y `src/storage.cpp` confirma que el
firmware ya admite preferencias `afr2-wifi-1` en `/config/wifi.json` de
LittleFS. Antes no había `data/config/` en el árbol local. Se preparó
`AutoFinalRobo2-ESP/data/config/wifi.json` con `mode: STA` y los datos pedidos,
y se excluyó explícitamente esa carpeta privada del Git global.

No se cambió el C++, el modo AP de aprovisionamiento ni el fallback de STA.
No se agregaron credenciales a fuentes versionadas, al paquete Web ni a los
informes. El esquema existente y ROBO2/EX01 permanecen sin modificaciones.

## Aplicación y límites

La configuración preparada requiere regenerar y cargar LittleFS con el layout
correspondiente al firmware. La imagen FS previa no contiene estos datos.
Cargar solo `firmware.bin` no actualiza `/config/wifi.json`. Reemplazar el
filesystem completo también reemplaza sus otras preferencias.

El corte de carga y la ausencia de salida serie comunicados por el usuario
en esta conversación siguen sin causa aislada. Preparar estas preferencias
no acredita recuperación, programación ni conexión Wi-Fi de la placa.

## Estado por módulo

| Módulo | Estado de esta ficha |
| --- | --- |
| esp | JSON STA local preparado. Fuente C++ y BIN existentes sin cambios; imagen LittleFS no regenerada ni cargada; conexión no comprobada. |
| atmega | Sin cambios, compilación ni carga. |
| web | Sin cambios, build ni promoción de activos. |
| Loader / CAD | Sin cambios ni ejecución. |
| Contracts / Git global | Registro y exclusión específica de preferencias privadas. |

No se ejecutaron tests, compilaciones, validadores, generación de imagen FS,
apertura de COM, flasheos ni pruebas físicas en esta revisión.

## Adaptaciones por archivo

| Archivo | Adaptación |
| --- | --- |
| `.gitignore` raíz | Excluye `AutoFinalRobo2-ESP/data/config/`, incluidos secretos y temporales de preferencias. |
| `AutoFinalRobo2-ESP/data/config/wifi.json` | Dato privado local creado con el esquema existente y modo STA. Excluido de Git; valores omitidos del registro. |
| `AutoFinalRobo2-ESP/docs/modules/wifi.md` | Documenta la configuración privada y la necesidad de regenerar/cargar LittleFS. |
| `AutoFinalRobo2-Contracts/changes/2026-10-27-26-esp-private-sta-config.md` | Ficha, procedencia y estados reales por módulo. |
| `AutoFinalRobo2-Contracts/CHANGELOG.md` | Entrada incremental sin incluir credenciales. |
