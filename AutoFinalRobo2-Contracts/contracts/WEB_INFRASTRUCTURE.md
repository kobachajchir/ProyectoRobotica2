# Infraestructura Web — AFR2-20261102-04

02/11/2026. Ruta `#/infraestructura`, grupo Explorar. Extensión HTTP de consulta;
no agrega comandos WebSocket, UART, acciones de control ni cambios de EEPROM.

## Consulta del paquete

`GET /api/web-manifest` devuelve `application/json; charset=utf-8`,
`Cache-Control: no-store`, `X-Content-Type-Options: nosniff` y el manifiesto
`afr2-littlefs-manifest-1` completo. Los campos usados son `project`, `webVersion`,
`packagedAt`, `buildProfile`, `totalRawBytes`, `totalGzipBytes`, `files[].route`,
`files[].rawBytes`, `files[].gzipBytes`; opcionales `estimatedFilesystemBytes`,
`filesystemBudgetBytes`, `modelAssetsUpdatedAt`, `modelAssetsOnlyUpdate`.

El encabezado `X-AFR2-Package-Source` identifica la procedencia:

| Valor | Productor | Significado |
| --- | --- | --- |
| `esp-littlefs` | ESP | Manifiesto `/dist/manifest.json` del filesystem montado en el servidor que entrega la Web. Solo se responde si `storage.webReady()`. |
| `local-prepared` | Vite dev/preview | Lee `AutoFinalRobo2-ESP/data/dist/manifest.json` en la computadora. No acredita carga en una placa ni que el paquete coincida con las fuentes actuales. |

ESP responde 503 si el paquete no está disponible. Vite responde 503 si el archivo
local no se puede leer y 405 a métodos distintos de GET. La ruta es fija: no admite
selección de rutas de disco ni expone `/config`, credenciales u otros archivos.
El manifiesto contiene metadatos de recursos públicos ya servidos por HTTP.
El ESP transmite el JSON desde LittleFS; no duplica el manifiesto en un
`JsonDocument` grande ni descomprime los recursos para esta consulta.

La Web solo acepta los dos orígenes declarados. Consulta con `cache: no-store`,
timeout de 8 s y cancelación al salir o actualizar. No hace sondeo periódico.
Si el manifiesto proviene del ESP, consulta también el `/api/health` existente
para `filesystem.{mounted,webReady,fault,usedBytes,totalBytes}`. No requiere
WebSocket ni conexión con el ATmega. Si health falla conserva el manifiesto y
deja la ocupación sin dato. `Actualizar datos` repite estas consultas de lectura.

En un firmware anterior la nueva consulta puede no existir. El recorrido y los
pesos del catálogo de la Web siguen disponibles, con aviso de reporte ausente.
Vite solo lee el paquete que ya existe; no compila, comprime, genera imagen ni
promueve archivos al abrir esta página.

## Significado de los tamaños

- Catálogo 3D: `Web/src/components/three/modelAssetSizes.json`, actualizado por
  el flujo de preparación existente. Son bytes de GLB antes de gzip; el conjunto
  suma auto y brazo porque no tiene un GLB independiente. Excluye el cubo.
- Tabla del paquete: tamaños **declarados** por el manifiesto del servidor.
  Se muestran procedencia, versión, fecha y actualización separada de modelos,
  cuando está declarada. No identifica automáticamente el build de las fuentes
  locales ni comprueba el contenido por SHA-256.
- gzip: compresión externa por recurso, distinta de Draco. Puede aumentar un
  archivo pequeño o ya comprimido; el texto muestra reducción o aumento.
- LittleFS ocupado/capacidad: reporte de la placa consultada. Incluye estructura
  y otros archivos, no solo la suma del paquete Web.
- Estimación FS: metadato del empaquetador con redondeo por bloques y reserva;
  no se presenta como ocupación física ni tamaño de `littlefs.bin`.
- Imagen `.bin`: artefacto distinto del firmware, incluye estructura y espacio
  del filesystem. No se inventa su tamaño a partir de los `.gz`.
- KiB = 1.024 bytes; MiB = 1.048.576 bytes. El peso de GLB/gzip no mide RAM,
  memoria gráfica ni tráfico HTTP total.

## Flujo explicado

Fuentes React/TypeScript/CSS → Vite/Terser/esbuild/singlefile → HTML y recursos
→ gzip/manifiesto → carpeta ESP/data → imagen LittleFS → carga en flash →
servidor HTTP ESP → descompresión gzip del navegador → React/Three.js.

Maestros Fusion → exportación de mallas/instancias → escena Blender y copia
visual aligerada → GLB con Draco → mismo empaquetado/almacenamiento/HTTP →
gzip del navegador → GLTFLoader/DRACOLoader WebAssembly → WebGL.

El ESP **sirve comprimido**, el navegador descomprime gzip y decodifica Draco.
HTTP de recursos y WebSocket de comandos/telemetría son flujos diferentes.
El ATmega conserva su tarea de control, sin ejecutar React ni renderizar CAD.

## Estado de entrega

atmega: sin cambios. esp: fuente HTTP modificada, sin compilar/cargar.
web: fuente implementada, sin tests, compilación, validadores ni navegador.
Sin generación/promoción de paquete, imagen LittleFS, puertos ni flasheos.
