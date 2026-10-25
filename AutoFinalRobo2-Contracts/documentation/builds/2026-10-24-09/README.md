# Compilación y paquete — AFR2-20261024-09

24/10/2026. `npm run build:esp`, exit0: copia de recursos AVR, TypeScript,
Vite, minificación, gzip y promoción local a `AutoFinalRobo2-ESP/data/dist`.
48nodos AVR,14XBM,950bytes Font7x10 y recursos de secuencia extraídos del C.
Catálogo compartido Web:59identidades (48nodos +11avisos). Sin dependencias nuevas.

| Medida | Resultado |
| --- | --- |
| Activos | 7 |
| Gzip total | 1460963bytes |
| Aumento respecto a build08 | 8249bytes (0,57%) |
| LittleFS estimado | 1548288bytes |
| Presupuesto del paquete | 2867200bytes |
| Margen estimado | 1318912bytes |

Tamaños nominales de archivos/estimación FS; flash física no identificada.
Layout candidato4MiB/3MiB LittleFS conservado. Modelo final827232bytes sin cambios.

- [Log final](web-build.txt), [manifest](manifest.json), [tamaños y hashes](summary.json).
- [Firmware AVR/ESP conservado de08](../2026-10-24-08/README.md).
- [Ficha09](../../../changes/2026-10-24-09-linked-oled-library.md).

Sin errores TypeScript/Vite. El empaquetador informó fallback por EPERM a copia
por archivos con manifest final, y terminó con éxito. Los hashes del paquete Web
y ESP se conservan en summary. El primer intento antes de completar esta ficha
detectó el fallback de metadata sin renderer/actions; corregido y recompilado.
El log final corresponde a la fuente actual.

El build comprueba códigos compartidos y hashes de algoritmos revisados. Eso no
es una comparación diferencial de frames. La biblioteca local requiere el mismo
snapshot preparado/reloj/estado/buffer previo que AVR; el visor conectado conserva
los1024bytes originales. No se acredita paridad OLED física ni F4 por compilar.

Autorización humana previa de compilar/corregir la integración, aplicada a estos
cambios. Solo compilación e inspección de fuente/artefactos: sin suite de tests,
validadores independientes (incluido quick_validate de la skill), comparación
diferencial, preview de navegador, UART/radio, hardware, reinicios, imagen FS,
flasheo, commit ni publicación. Sin nuevo intento del navegador denegado.
