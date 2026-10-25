# AFR2-20261023-13 — Entorno Python/esptool comprobado

23/10/2026. Pedido expreso del usuario de verificar Python y esptool y hacer estas validaciones. Se ejecutaron comprobaciones locales de intérpretes/imports, enumeración de puertos y CLI `esptool --help`. No hubo operaciones de dispositivo.

Resultado: `python`/`py -3` global apuntan a RoboDK Python 3.7.3, con serial pero sin esptool. PlatformIO incluye Python 3.11.7 en penv/Scripts/python.exe y pyserial 3.5. Esptool 4.5.1 está instalado en .platformio/packages/tool-esptoolpy, fuera de site-packages. Se ajustó tools/esp_probe.py para resolver esa ubicación existente y propagarla al subprocess mediante PYTHONPATH solo durante esa ejecución. No se instalaron paquetes ni modificaron variables persistentes.

Archivos adaptados: tools/esp_probe.py (resolución esptool, --check-tools, metadatos y Markdown del entorno), tools/esp-probe.ps1 (-CheckTools), hardware/ESP_DIAGNOSTICO.md y hardware/diagnostics/README.md. Procedencia: código propio AFR2-20261023-12.

Validación realizada con el lanzador real: `powershell -NoProfile -ExecutionPolicy Bypass -File .\tools\esp-probe.ps1 -CheckTools`, salida 0 y estado tools-ok-no-device-opened. [Evidencia JSON/Markdown y log CLI](../hardware/diagnostics/20261023-155540-554529-6488/report.md). Enumerados COM3/COM4/COM6/COM7, todos Standard Serial over Bluetooth link; no se seleccionó ni abrió ninguno. La enumeración no acredita placa conectada.

Estado atmega/esp/web: sin cambios ni builds. Shared: herramientas de entorno verificadas en esta PC; conexión ROM ESP, flash física, cabecera y partición pendientes de conectar la placa por su puerto de servicio. No se ejecutaron chip_id, flash_id, read_flash, borrados, formatos, cargas ni pruebas físicas.
