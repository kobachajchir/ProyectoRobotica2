# Compilación y paquete — AFR2-20261024-10

24/10/2026. `npm run build:esp`, exit0: recursos OLED vinculados de09,
TypeScript, Vite, JS/CSS minificados, gzip y promoción a ESP/data/dist.
El log final incluye la aclaración de disposición: mapa completo, coordenadas
en fila, lista abajo y controles al lado. Sin errores de compilación.

| Medida | Resultado |
| --- | --- |
| Activos | 7 |
| Gzip total | 1467583bytes |
| Diferencia frente a09 | +6620bytes (0,45%) |
| LittleFS estimado | 1556480bytes |
| Presupuesto del paquete | 2867200bytes |
| Margen estimado | 1310720bytes |

No se agregan dependencias ni activos binarios. GLB final827232bytes sin cambio;
conserva escala1unidad=1m. Layout candidato4MiB/3MiB LittleFS; capacidad real no
identificada. Firmware AVR/ESP sin cambios, compilación08 conservada. Catálogo
OLED59 y gates de memoria/Hall/calibración se mantienen.

[Log final](web-build.txt), [manifest](manifest.json), [resumen/hashes](summary.json)
y [ficha10](../../../changes/2026-10-24-10-goto-map-queue.md).
El empaquetador usa fallback por EPERM a archivos con manifest final y termina
correctamente. Hashes Web/ESP idénticos en summary. No es publicación remota.

Compilación bajo la autorización humana previa de compilar/corregir la
integración. Solo fuente/artefactos: sin suite de tests, validador independiente,
navegador (denegación previa respetada), evaluación visual/motion/gestos,
pruebas físicas, radio/UART, imagen FS, reinicios ni flasheo. El funcionamiento
observado del picking, scroll y temas queda pendiente de navegador autorizado.
