# AMI-20261005-02 — Modelo V2 y copia recurrente Web → data ESP

- Fecha: 2026-10-05 (America/Argentina/Buenos_Aires)
- Repositorio de origen: Web ESP01
- Referencia de código: cambios locales en `public/models/auto_micro.glb`, `src/utils/localModel.ts`, `tools/copy-web-to-esp.mjs` y `docs/copy-web-to-esp.md`
- Estado del origen: implementado; modelo copiado al data local ESP
- Área: distribución de assets HTTP / flujo de preparación de LittleFS

## Contrato anterior y nuevo

AMI-20261002-05 copiaba automáticamente sólo `dist/index.html.gz` a `data/dist/index.html.gz`. El GLB se había copiado una sola vez y no se reemplazaba en builds posteriores. El asset anterior pesaba 861.084 bytes.

Ahora `copy:esp`, invocado por el postbuild existente, copia dos archivos:

| Fuente Web | Destino dentro del proyecto ESP |
| --- | --- |
| `dist/index.html.gz` | `data/dist/index.html.gz` |
| `public/models/auto_micro.glb` | `data/dist/models/auto_micro.glb` |

La fuente del GLB es siempre el asset vigente de public, incluso al ejecutar `copy:esp` sobre un gzip ya generado. Debe existir, no estar vacío y pesar menos de **900.000 bytes**. No se copia el HTML original, se elimina el antiguo `data/dist/index.html` si existe y se conservan Draco y los demás archivos del filesystem.

El modelo vigente es AutoMicro V2 embedded: **883.260 bytes**, **266.697 triángulos**, **681 piezas distintas** agrupadas en 12 objetos y 8 materiales; incluye 16 cuerpos de acrílico con transmisión física. Formato GLB 2 con `KHR_draco_mesh_compression`; posiciones de 14 bits, normales de 9 bits, nivel de compresión 10. La exportación se mantiene en metros con Y arriba. Los archivos CAD y `.blend` quedan fuera de data.

GET `/models/auto_micro.glb` conserva el destino LittleFS `/dist/models/auto_micro.glb`, MIME `model/gltf-binary` y la política sin caché prolongada de AMI-20261002-06. No cambian comandos, eventos, screen codes, payloads UNER/WebSocket, autenticación, capacidades ni hello.

## Flujo completo

1. `npm run build` genera el bundle; el postbuild ya existente comprime el index y llama `npm run copy:esp`. También se puede copiar el gzip existente y el modelo vigente mediante `copy:esp`, sin compilar.
2. El script resuelve el proyecto ESP por su ubicación, o mediante el override existente `--esp-dir`. Exige `platformio.ini`, gzip no vacío y GLB dentro del límite antes de publicar archivos. Los destinos dist y models deben resolver a sus carpetas propias, sin enlaces a otra ubicación.
3. Prepara archivos temporales para ambos recursos; publica primero el modelo y luego el gzip mediante rename. Cada reemplazo es individual, no una transacción conjunta de dos archivos. Si falla el segundo rename, el modelo puede quedar actualizado mientras permanece el gzip anterior; la URL del modelo sigue siendo compatible.
4. Errores de archivo, tamaño o copia terminan con exit code distinto de cero y un mensaje explícito. No existe ACK/evento de placa para esta operación local. No se añade un timeout de transporte.
5. La generación y carga de la imagen LittleFS siguen siendo pasos separados de PlatformIO. El script no flashea ni reinicia la ESP. En el dispositivo, el handler HTTP conserva las guardas existentes, 423 si la UI no está permitida y 404 si falta el archivo.
6. Todas las vistas Web que consumen `LOCAL_AUTO_MICRO_MODEL_URL` reciben el mismo asset; sus cargadores Draco existentes siguen siendo necesarios.

## Impacto en repositorios

| Repositorio | Estado | Acción o motivo concreto |
| --- | --- | --- |
| FirmwareF4 | no_aplica | No sirve ni consume este asset. |
| Firmware ESP01 | adaptado | V2 copiada a `data/dist/models/auto_micro.glb`; el handler existente sirve esa misma ruta y MIME. No se modificó su código. |
| Web ESP01 | origen | Asset V2 reemplazado y copia recurrente del GLB añadida al script. |
| AutoMicroQtDesktop | no_aplica | Sus assets independientes no participan en este flujo. |

## Compatibilidad y orden de integración

Primero actualizar public y el script Web. El próximo build autorizado copiará el gzip nuevo y el GLB vigente a data ESP. Para llevar el modelo al equipo hay que generar/cargar LittleFS. La carga local del GLB a data ya se efectuó en este turno, sin cambiar el gzip existente ni subir archivos a una placa.

El firmware con la política HTTP de AMI-20261002-06 es compatible sin cambios de código porque ruta, formato y decodificación permanecen iguales. Esta ficha reemplaza la regla de copia única del modelo de AMI-20261002-05; no altera el resto de ese flujo.

## Evidencia y límites

Se inspeccionaron el cargador común Web, sus consumidores, el script de copia y el handler `GLB_PATH` en `src/webserver_ws.cpp` de ESP. Se ejecutó Copy-Item del GLB V2 a public Web y a data ESP. El tamaño y los conteos del modelo proceden del informe de exportación Blender V2 del mismo trabajo.

No se ejecutaron tests, compilaciones, validaciones, el script modificado, generación/carga de LittleFS ni prueba física o de navegador. La ejecución integrada del script en el próximo build y la carga al dispositivo siguen pendientes. El registro central se publica por separado del código y de los assets de los otros repositorios.
