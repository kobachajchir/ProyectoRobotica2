# AFR2-20261024-04 — Modelo final servido por ESP y LittleFS de 3 MiB

El usuario confirmó que el modelo final ya existe y reemplaza al legacy;
pidió incluirlo directamente en el ESP, suponiendo flash de4MiB, y asignar
3MiB a LittleFS para crecimiento. Esta ficha sustituye el perfil ESP sin GLB
de03. Se conserva la optimización WebP/CSS/dependencias/componentes de03.

## Entrega y tamaño

| Contenido | Paquete gzip (bytes) |
| --- | ---: |
| Tres imágenes WebP con alpha | 229.970 |
| HTML con JS/CSS minificados | 437.428 |
| Modelo final de AutoFinalRobo2 | 707.669 |
| Draco WASM y su wrapper local | 75.134 |
| **Total** | **1.450.201** |

Antes de optimizar:2.897.959 bytes. Ahorro final: **1.447.758 bytes (49,96%)**,
incluyendo ahora el modelo definitivo. Los649.410 bytes de03 correspondían al
perfil intermedio sin GLB y no representan la entrega final. Hay siete activos
y un manifiesto; el modelo se descarga al visitar su ruta, sin CDN externo.

- Fuente: `AutoFinalRobo2-CAD/exports/2026-10-24/AutoFinalRobo2-ligero.glb`,
  Fusion v16 / Blender,827.232 bytes,26 nodos,20 mallas,79 materiales PBR,
  sin texturas de imagen. [Procedencia CAD](../../AutoFinalRobo2-CAD/docs/RUEDAS_Y_BLENDER_20261024.md).
- Copia exacta: `Web/public/models/AutoFinalRobo2.glb`. SHA-256 de fuente,
  copia pública y GLB descomprimido de `ESP/data/dist` coinciden:
  `d10684704200caa4da0a6a98715e52abc446b9dee9cccdde32f33ee05fc927c7`.
  No se modifican geometría, materiales, cuantización ni el maestro CAD.
- `Model3DPage` usa el modelo final, encuadra automáticamente su volumen sin
  cambiar unidades fuente, permite órbita/zoom, pausa y restablecimiento, y
  conserva carga/error/reintento. Render bajo demanda, DPR máximo1,5. Se aclaran
  las tolerancias y la pieza/PCB pendientes sin llamar al GLB malla legacy.
- El mapa de Control conserva su esquema ligero y los datos autoritativos del
  robot; no utiliza el legacy. No se cambia ningún comando, OLED ni navegación.
- El legacy se conserva fuera del paquete en `Web/reference/legacy-model/`.
  El build solo publica el GLB final y los dos archivos Draco necesarios.
  La copia a ESP retira únicamente los activos obsoletos del manifiesto anterior.

## Reparto candidato de flash

`ESP/platformio.ini`: `board_build.ldscript = eagle.flash.4m3m.ld`,
`board_build.filesystem = littlefs`. Es una configuración candidata basada en
el supuesto humano de4MiB; la línea HARDWARE del build viene de la definición
de placa y no es una detección física del chip.

El script instalado del framework establece:

| Región | Límite de compilación |
| --- | --- |
| LittleFS nominal | 3MiB |
| LittleFS disponible según linker | 3.121.152 bytes (reservas EEPROM/RF aparte) |
| Programa | 1.044.464 bytes |
| Presupuesto preventivo de activos Web | 2.867.200 bytes (2800KiB) |
| Estimación Web con overhead/reserva | 1.540.096 bytes (1504KiB) |

`tools/web-budget.mjs` comparte límites entre empaquetado y promoción, con
bloques8KiB,512 bytes auxiliares por activo y64KiB de reserva. El empaquetador
rechaza excesos y la copia exige perfil ESP y linker candidato coincidente.
La estimación no sustituye una imagen LittleFS medida. Los3MiB son para Web y
archivos: el programa tiene su límite separado de aproximadamente1MiB.

No se cambian offsets mediante una carga, ni se crea imagen FS, ni se formatea.
Antes de una carga futura se deberán identificar flash/SPI reales y preservar
la configuración existente. No se inventan capacidad física ni tolerancias.

## Compilaciones y estado real

| Módulo | Estado |
| --- | --- |
| atmega | Sin cambios; conserva compilaciones y bloqueos físicos de02 |
| esp | Firmware compilado con layout candidato3MiB; Web final promovida a data/dist |
| web | Perfiles full y esp compilados, mismo contenido con modelo final; minificación y empaquetado correctos |
| CAD | Se consume el exportado final; fuentes/maestros sin cambios |

`npm run build` y `npm run build:esp` terminaron con código0. Firmware ESP:
RAM estática44.396/81.920 bytes; programa414.281/1.044.464 bytes. Sin suite de
tests, validadores independientes, navegador, monitor serie, imagen LittleFS,
flasheo ni pruebas físicas. No afirmar funcionamiento físico por compilar.

La revisión automática agotó el plazo antes de ejecutar dos primeras solicitudes
de compilación; el reintento permitido completó ambos builds. El empaquetador
utilizó la alternativa de copia por archivos ante EPERM de Windows y escribió
el manifiesto al final. No quedaron errores de compilación.

[Logs/manifiestos/tamaños](../documentation/builds/2026-10-24-04/README.md) ·
[Contrato de activos](../contracts/WEB_ASSETS.md) ·
[Procedencia por archivo](../provenance/final-model-littlefs-20261024.json).
