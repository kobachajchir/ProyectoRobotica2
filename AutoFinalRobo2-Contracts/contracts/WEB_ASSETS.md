> **Vigente AFR2-20261024-15:** siete activos, modelo final y presupuesto2800KiB conservados. Manifest añade webVersion/packagedAt opcionales; [diagnóstico](SYSTEM_INFO_FILESYSTEM_15.md). Tamaño/identidad concretos en [build15](../documentation/builds/2026-10-24-15/README.md). Cifras inferiores históricas; sin imagen FS ni upload.

# Activos Web — AFR2-20261024-04

Actualización **AFR2-20261024-06**: un mismo GLB final se reutiliza en inspección
y las tres cámaras de Control, sin duplicar activos. Se conserva escala en
metros y base sobre Y=0; SVG solo como respaldo explícito. Paquete vigente:
**1.450.617 bytes gzip / 7 activos**, estimación1.540.096 bytes. [Evidencia](../changes/2026-10-24-06-grounded-model-control.md).

Fuente contractual de entrega Web/LittleFS. No modifica el protocolo de control
ni los contratos eléctricos. Firmware actual acepta `image/webp`, WASM y GLB.

- `Web/tools/package-web.mjs` genera `.gz` y `manifest.json` con schema
  `afr2-littlefs-manifest-1`, ruta, MIME, gzip y SHA-256 del contenido original.
- `Web/tools/copy-web-to-esp.mjs` promueve el perfil ESP a `ESP/data/dist`,
  publica el manifiesto al final y retira solo activos obsoletos administrados.
- Vite publica una lista explícita: logo1024, logo256 y favicon64 en WebP,
  `models/AutoFinalRobo2.glb`, Draco WASM y wrapper. El PNG maestro y legacy
  permanecen como referencias fuera del paquete. Sin dependencias de CDN.
- El perfil `full` y `esp` incluyen el mismo modelo final. El nombre de perfil
  identifica el destino y protege la promoción; no cambia los comandos Web.
- `npm run build`: TypeScript + Vite + empaquetado local full.
  `npm run build:esp`: TypeScript + Vite modo esp + paquete + promoción.
  `package:littlefs` usa el perfil real guardado por Vite en `.afr2-build.json`;
  ese marcador no es un activo HTTP. `copy:esp` exige un paquete ESP coherente.
- Candidato autorizado: flash4MiB / LittleFS nominal3MiB,
  `eagle.flash.4m3m.ld`, reserva efectiva FS3.121.152 bytes y programa máximo
  1.044.464 bytes según framework instalado. Capacidad física no identificada.
- `tools/web-budget.mjs` comparte presupuesto2800KiB, redondeo8KiB por activo
  más512 bytes y reserva64KiB. La copia exige el mismo linker en platformio.ini.
  Ningún comando Web genera imagen FS, flashea o formatea.

Entrega medida: **1.450.201 bytes gzip / 7 activos**, estimación con reserva
**1.540.096 bytes**. Más datos no implican mayor RAM del ESP: el firmware sirve
archivos de LittleFS; el navegador decodifica GLB/Draco. No se midió heap/radio
bajo carga en esta tarea. [Evidencia y límites](../changes/2026-10-24-04-final-model-littlefs.md).
