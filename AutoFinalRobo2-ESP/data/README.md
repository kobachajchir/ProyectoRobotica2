> **Vigente AFR2-20261025-11:** giro local con dos dedos/Q-E, sin yaw físico;7archivos/1483767bytes gzip, FS estimado1572864/2867200. Firmware0.9.0/EX01v7/build01 intacto. [Build11](../../AutoFinalRobo2-Contracts/documentation/builds/2026-10-25-11/README.md). Solo copia local; sin imagen FS/flasheo. Estados inferiores históricos.

> **Vigente AFR2-20261025-01:** Web0.2.0/control directo7, siete activos promovidos, 1479980 bytes gzip. Estimación FS 1564672/2867200. ESP/ATmega0.9.0 deben actualizarse juntos; sin imagen FS/upload. [Evidencia](../../AutoFinalRobo2-Contracts/documentation/builds/2026-10-25-01/README.md). Estados inferiores históricos.

> **Vigente AFR2-20261024-15:** siete activos promovidos, 1480773 bytes gzip, 1564672 bytes FS estimados de 2867200. Manifest con webVersion/packagedAt opcionales, compatible con Storage15. Fuente ESP compilada; sin imagen FS ni flasheo. [Evidencia15](../../AutoFinalRobo2-Contracts/documentation/builds/2026-10-24-15/README.md). Cifras inferiores históricas.

> **Vigente AFR2-20261024-14:** Desarrollador sin hardware/reinicios, regreso con flecha y perfil espaciado; catálogo OLED59 en árbol con selección/scroll laterales adaptados de AutoMicro. SW confirma; visor conserva framebuffer y captura nueva entre giros. Web compilada y siete activos promovidos (1479434 bytes gzip). Firmware ESP/AVR sin cambios, evidencia12/EX01v6/gates0. Visual/paridad/placas pendientes; sin tests, navegador ni flash. [Ficha14](../../AutoFinalRobo2-Contracts/changes/2026-10-24-14-developer-oled-tree.md). Estados inferiores históricos.

> Actualizacion **AFR2-20261024-06**: suelo en la base del modelo y GLB final compartido por inspeccion y Control (Superior ortografica, Perspectiva y Seguir auto). Escala1unidad=1m; cuadricula0,5m, sin agrandar la malla. Web compilada/promovida: **1.450.617 bytes gzip**,7 activos. Sin render visual, pruebas fisicas ni flasheo. [Detalle](../../AutoFinalRobo2-Contracts/changes/2026-10-24-06-grounded-model-control.md).

> Correccion **AFR2-20261024-05**: Reintentar carga 3D elimina el error cacheado por useLoader; publicDir absoluto y copia selectiva de build conservada. Web compilada y promovida: **1.450.338 bytes gzip**. Diagnostico HTTP local: GLB/Draco/WebP200 con contenido correcto; sin renderizado visual ni flasheo. Modelo final y layout candidato3MiB de04 conservados. [Detalle](../../AutoFinalRobo2-Contracts/changes/2026-10-24-05-model-loading-retry.md).

> Vigente **AFR2-20261024-04**: **modelo final incluido en ESP**, paquete **1.450.201 bytes gzip** (49,96% menos que los 2,90 MB iniciales). WebP, JS/CSS minificados y dependencias/componentes optimizados. Layout candidato: flash 4 MiB / LittleFS nominal 3 MiB, solicitado por el usuario; flash fisica sin identificar. Web full/esp y firmware ESP compilados; activos promovidos a data/dist. Sin navegador, imagen FS ni flasheo. Atmega y comandos conservan el estado de02. [Evidencia](../../AutoFinalRobo2-Contracts/changes/2026-10-24-04-final-model-littlefs.md). Los estados previos inferiores son historicos.

> AFR2-20261024-02: Web actual compilada y promovida aqui: 6 assets, 2.897.959 bytes gzip. Sin imagen FS ni flasheo; identificar flash/SPI y layout antes de construir imagenes. Los estados previos inferiores son historicos.

# LittleFS

Destino de empaquetado Web: `data/dist`, servido desde `/dist` según manifest.json (`afr2-littlefs-manifest-1`). Paquete existente AFR2-20261023-09: seis archivos, 2.925.405 bytes gzip sin overhead LittleFS.

Servidor y almacenamiento activos en fuente AFR2-20261023-11. `Web/tools/package-web.mjs` genera y `Web/tools/copy-web-to-esp.mjs` promueve el bundle; no se ejecutaron en esta ficha. Los dos ajustes Web posteriores todavía necesitan recompilar/empaquetar/promover.

No agregar credenciales aquí. El firmware crea `/config/wifi.json` solo en el dispositivo; HTTP no lo expone. Montaje con autoformat=false. Una imagen FS completa puede reemplazar configuración existente: preservarla antes de una futura carga autorizada.

Identificar flash/SPI y definir layout común de firmware/FS antes de construir imágenes o flashear. El perfil candidato no acredita capacidad suficiente. No hay upload automático ni API HTTP de escritura.
