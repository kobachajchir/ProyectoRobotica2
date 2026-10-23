# AMI-20261002-06 — ESP sirve el modelo final sin caché immutable

- Fecha: 2026-10-02 (America/Argentina/Buenos_Aires)
- Origen: Firmware ESP01
- Estado: implementado y compilado localmente
- Área: recurso HTTP/modelo 3D y cabeceras de caché

## Contrato anterior y nuevo

GET `/models/auto_micro.glb` ya resolvía LittleFS `/dist/models/auto_micro.glb`, archivo final copiado en AMI-20261002-05. No era necesario renombrar la URL ni el archivo. Antes makeFileResponse(false,false) servía este recurso con `Cache-Control: public, max-age=31536000, immutable`, permitiendo que el navegador conservara el GLB anterior aunque se reemplazara data.

Ahora el handler usa makeFileResponse(false,true): Content-Type `model/gltf-binary`, sin gzip, `Cache-Control: no-cache, no-store, must-revalidate`, `Pragma: no-cache`, `Expires: 0`. El body es el archivo GLB final completo. Ruta/bytes del modelo y guardas de acceso se mantienen; no cambian payloads UNER/JSON.

## Flujo completo

1. Web src/utils/localModel.ts solicita models/auto_micro.glb desde el origen local.
2. ESP conserva webUiAllowsAsset: rechaza con 423 si no corresponde acceso a esa UI. Sin archivo responde 404.
3. Recurso disponible se sirve desde GLB_PATH con MIME GLB y política sin caché prolongada. Otros assets Draco conservan su política.
4. hello backend.modelAssets sigue verificando el mismo path LittleFS en ws_json_bridge.cpp.
5. Subir firmware actualizado y data/LittleFS con modelo final para aplicarlo al equipo. La caché immutable antigua ya guardada requiere recarga ignorando caché una vez; la nueva política solo aplica a respuestas recibidas del firmware actualizado.

## Impacto

| Repositorio | Estado | Acción |
|---|---|---|
| FirmwareF4 | no_aplica | No sirve ni solicita este GLB HTTP. |
| Firmware ESP01 | origen | Handler GLB y documentación actualizados; build exitoso. |
| Web ESP01 | compatible | localModel.ts conserva misma URL; modelo final ya copiado por AMI-20261002-05. |
| AutoMicroQtDesktop | no_aplica | Assets Qt independientes no cambian. |

## Evidencia y límites

ESP src/webserver_ws.cpp: GLB_PATH, handler GET y addNoCacheHeaders; src/ws_json_bridge.cpp: backend.modelAssets. Web src/utils/localModel.ts: URL local. Compilación PlatformIO `run -e esp01_1m` completó exit0 y generó firmware.bin: RAM64472/81920, flash493355/1044464. Sin tests, carga al dispositivo ni prueba HTTP/navegador por este cambio. Código permanece en checkout local; registro central se publica separado.
