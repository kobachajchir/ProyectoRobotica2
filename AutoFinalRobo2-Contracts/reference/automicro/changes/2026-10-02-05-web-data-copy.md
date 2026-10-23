# AMI-20261002-05 — Copia automática del bundle gzip Web a LittleFS ESP

- Fecha: 2026-10-02 (America/Argentina/Buenos_Aires)
- Origen: Web ESP01
- Estado: implementado localmente y copia inicial ejecutada
- Área: distribución de assets HTTP / filesystem LittleFS

## Contrato anterior y nuevo

Antes, npm run build generaba dist/index.html y postbuild generaba su gzip; la actualización de data del firmware requería copia manual.

Ahora postbuild ejecuta `gzip dist/index.html -e=gz -f && npm run copy:esp`. El script tools/copy-web-to-esp.mjs lleva **solo** dist/index.html.gz a Firmware ESP01/FirmESP01MicroArduino2025/data/dist/index.html.gz, reemplaza la versión anterior y elimina data/dist/index.html si existe. El HTML original permanece solamente en el dist local de Web. No se purga data: se conservan modelos, Draco y archivos no relacionados.

`copy:esp` permite copiar un gzip existente; `build:esp` es alias de build, sin duplicar la copia. Override opcional `--esp-dir` apunta a raíz ESP con platformio.ini. Las rutas por defecto derivan de la ubicación del script, sin rutas personales fijas.

Por pedido explícito y única vez se reemplazó data/dist/models/auto_micro.glb desde public/models/auto_micro.glb de Web, modelo utilizado por src/utils/localModel.ts. Fuente 861084 bytes; antiguo destino 463764 bytes. No se añade copia del modelo al postbuild.

## Flujo

1. Usuario ejecuta npm run build desde Web.
2. TypeScript/Vite terminan; gzip genera index.html.gz.
3. Copia mediante temporal y rename reemplaza index comprimido; elimina HTML original del destino. Errores terminan el proceso con exit code no-cero.
4. ESP ya sirve /dist/index.html.gz mediante webserver_ws.cpp; la ruta /models/auto_micro.glb conserva el archivo LittleFS /dist/models/auto_micro.glb. No cambian protocolo, payloads UNER/WS ni autenticación.
5. Generación/carga de imagen LittleFS con PlatformIO continúa siendo un paso separado. Este script no flashea ni reinicia dispositivos.

## Impacto

| Repositorio | Estado | Acción |
|---|---|---|
| FirmwareF4 | no_aplica | No consume estos assets ni cambia wire. |
| Firmware ESP01 | adaptado | data/dist actualizado: gzip y modelo final; servidor conserva rutas HTTP ya implementadas. |
| Web ESP01 | origen | Script Node, comandos npm y documentación implementados. |
| AutoMicroQtDesktop | no_aplica | Sus assets independientes no cambian por este pedido. |

## Evidencia y límites

Web src/utils/localModel.ts usa models/auto_micro.glb. ESP src/webserver_ws.cpp define INDEX_GZ=/dist/index.html.gz y GLB_PATH=/dist/models/auto_micro.glb. Se ejecutó directamente el script de copia con exit0 y Copy-Item del modelo solicitado. Gzip copiado1750650 bytes. No se recompiló, no se ejecutaron tests/validaciones y no se generó/cargó una nueva imagen LittleFS. La ejecución integrada del próximo npm run build queda a cargo del usuario; su postbuild ya está configurado.

## Actualizaciones

- 2026-10-02: inicialmente planteada copia de dist completo; reemplazada antes de cierre por instrucción expresa de copiar exclusivamente index comprimido, postbuild automático y copia única del modelo final.
