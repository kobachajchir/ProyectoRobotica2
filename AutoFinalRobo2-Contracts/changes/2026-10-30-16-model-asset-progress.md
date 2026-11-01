# AFR2-20261030-16 — Progreso reutilizable de modelos 3D

Pedido: conocer tamaño antes/durante la descarga, cantidad recibida y avance; reutilizar en futuros modelos.

Web: fuente implementada, sin compilación, tests, validadores, navegador, ejecución de generadores, promoción ESP ni cargas. ATmega/ESP/Loader sin cambios por esta ficha. Sin acreditación visual, descarga real ni desempeño en hardware.

El GLB actual public/models/AutoFinalRobo2.glb tiene 871756 bytes (851,3 KiB). Catálogo inicial modelAssetSizes.json preparado con esa longitud leída del archivo. tools/catalog-model-assets.mjs recorre modelos y recursos y actualiza tamaños al iniciar dev/start o construir full/esp. Script escrito, no ejecutado en esta entrega. El catálogo se incluye en JS; no requiere HEAD ni endpoint adicional del ESP. Los metadatos deben regenerarse junto a la Web cuando cambien los assets.

ProgressGLTFLoader conserva caché useLoader compartida, DRACO y recursos del modelo. Recibe eventos de bytes del FileLoader; no duplica descargas ni reemplaza el LoadingManager global. Estado por URL y suscripción compartida fuera de Suspense. Distingue idle/downloading/preparing/ready/error, bytes, total y tiempo restante estimado tras observar al menos750ms de transferencia. Reintento limpia estado y caché juntos. La etapa de preparación no tiene ETA inventada.

ModelAssetProgress recibe url, label y hideWhenReady; usa progress nativo y modo indeterminado si no conoce el tamaño. Integrado en Modelos3D, ControlMap, táctil, radar y recolector. Modelo de brazo pendiente no muestra el tamaño del auto ni inicia su descarga por este componente. Se pueden incorporar otras URLs con el mismo loader y el componente correspondiente; el catálogo acepta varios assets. No representa todavía una suma de archivos externos de un glTF multifichero: cada URL informa su propio tamaño. Modelo actual es GLB autocontenido; DRACO/decodificación forman parte de preparación.

Los bytes cuentan contenido del archivo recibido/descomprimido por Fetch/Three, no tráfico TCP ni cantidad gzip por cable. Esto evita usar Content-Length comprimido como total de bytes descomprimidos. No se usa un porcentaje artificial ni tamaño de geometría en RAM/GPU. La carga desde caché puede completar de inmediato. Al finalizar, si hubo eventos se conserva como tamaño la cantidad real observada.

## Reutilización

```tsx
<ModelAssetProgress url="/models/OtroModelo.glb" label="Otro modelo" />
```

Mantener el componente fuera del Canvas y del subárbol que suspende. Cargar esa URL con ProgressGLTFLoader mediante useLoader; configurar DRACO cuando corresponda. No introducir otro fetch para medir progreso.

Procedencia por archivo: ../provenance/model-asset-progress-20261030-16.json. Dependencias conservadas; fuentes previas mantienen autoría. Validación de comportamiento y empaquetado quedan pendientes de pedido expreso.
