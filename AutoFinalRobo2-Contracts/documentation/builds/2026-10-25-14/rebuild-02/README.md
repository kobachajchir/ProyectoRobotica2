# Recompilación 02 — sensores y radar

25/10/2026. Nuevo pedido humano: compilar y corregir errores. ATmega, ESP y Web para ESP finalizaron con **exit0, cero errores y cero advertencias**. No hicieron falta correcciones adicionales de código.

| Módulo | Compilación |
|---|---|
| ATmega | `make -B firmware communication-firmware library`, reconstrucción completa |
| ESP | `platformio run -e esp12s_candidate` |
| Web ESP | Sincronizador OLED, `tsc -b --force`, `vite build --mode esp`, `package-web --profile esp` |

Firmware0.10.0/EX01v8, Web0.3.0. Paquete ESP local: 7 activos, 1515587 bytes gzip; estimación de filesystem 1605632 bytes, presupuesto 2867200 bytes.

[ATmega](atmega-build.txt) · [ESP](esp-build.txt) · [Web ESP](web-esp-build.txt) · [Manifest](manifest-esp.json) · [Resumen](summary.json).

No se ejecutaron tests, validadores independientes, navegador, imagen de filesystem, serie ni flasheos. No se ejecutó copy:esp: ESP/data/dist conserva el paquete anterior. El perfil full y sus evidencias anteriores se conservan en la carpeta padre; dist/artifacts locales ahora contienen el perfil esp.
