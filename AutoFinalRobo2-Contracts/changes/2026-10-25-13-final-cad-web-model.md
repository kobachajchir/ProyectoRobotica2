# AFR2-20261025-13 — modelo final CAD compartido por Web y ESP

25/10/2026. Pedido: conservar ajustes/materiales manuales, actualizar PCB en Fusion, exportar a Blender, entregar GLB menor de 900.000 bytes, compilar/promover Web y hacer commit/push a origin.

La Web ahora incluye PCB OLED/encoder y soporte finales: **871.756 bytes**, Draco, 27 objetos, cuatro ruedas y un panel. Maestros Fusion/Blender, KiCad y bibliotecas conservados. [Fuentes, comprobaciones y recuperación](../../AutoFinalRobo2-CAD/docs/MODELO_FINAL_WEB_20261025.md).

Estados: **Atmega:** fuente/firmware sin cambios. **ESP:** firmware sin cambios; siete activos Web promovidos localmente. **Web:** modelo sustituido, compilación ESP y paquete completados; hashes OLED independientes de CRLF, sin cambio del renderer. **CAD:** KiCad exportado, PCB importada en Fusion, maestros guardados, render y reimportación Draco completados.

SHA-256 del GLB y del descomprimido de ESP: `c1bb1d87b40a4224a56e8e6473692929b36b0497799dd0971d6561b4939a4475`. Paquete 1.505.042 bytes gzip, estimación FS 1.597.440. Conserva ruta, escala en metros, Draco local y prefijos del rig; sin cambio de wire, comandos, versiones de firmware ni permisos de movimiento.

Comprobación CAD solicitada: cero penetraciones PCB–fondo; solapes internos STEP registrados aparte. Sin ERC/DRC, tests, compilación de firmware, montaje físico, imagen LittleFS ni flasheo. Chrome abrió la Web compilada pero informó WebGL no disponible: render Web pendiente en ese entorno. Publicación Git y funcionamiento físico son estados separados.
