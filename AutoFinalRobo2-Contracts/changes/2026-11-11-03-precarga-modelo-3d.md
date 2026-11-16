# AFR2-20261111-03 · Precarga del modelo 3D al conectar

Versión: 1.26.0 (subversión: función nueva compatible).
Estado: **web** fuente editada, sin `tsc`, build ni navegador; **atmega** y **esp** solo versión. Sin CMD nuevo.

## Pedido

Las páginas que necesitan el modelo 3D tardaban mucho o no cargaban, porque el `.glb` (~0,9 MB el auto, ~0,8 MB el brazo) se pedía
recién al entrar. El usuario pidió empezar la descarga apenas la conexión esté estable, mostrar el avance en un toast y, si se corta,
reconectar y seguir o empezar de nuevo.

## Comportamiento

- `ModelPrefetcher` (montado en `App.tsx`, junto al Toaster) espera que el WebSocket con el auto lleve **4 s seguidos** en `ready`, sin
  reinicios. Recién ahí descarga, uno por uno para no saturar al ESP: el auto (`/models/AutoFinalRobo2.glb`), el brazo V17 y el cubo de 50 mm.
- Muestra un toast «Descargando modelo 3D · N %» con el modelo en curso y los KB recibidos (se actualiza cada 250 ms como mucho). Al terminar
  muestra «Modelo 3D descargado» (3,5 s).
- **Corte de conexión**: la descarga se cancela, el toast pasa a «Descarga del modelo 3D en pausa» y, con la conexión estable de nuevo, sigue
  desde el modelo que faltaba. Los ya completos no se piden otra vez. El modelo cortado empieza de cero, porque el ESP sirve los `.glb`
  comprimidos (gzip) y no se puede retomar por rango.
- **Error HTTP o de red** con la conexión arriba: hasta 5 intentos, con espera creciente (1,5 s × intento). Si no se logra, avisa y cada página
  3D lo pide por su cuenta como antes.
- Lo descargado queda en memoria. `ProgressGLTFLoader` lo usa sin volver a pedirlo; si la precarga de ese modelo está en curso, espera a que
  termine en vez de pedirlo dos veces. Si la precarga se cancela, el visor hace su descarga normal. El progreso usa el mismo estado que
  `ModelAssetProgress` y el control táctil.
- `AssetInspection` pasa de `GLTFLoader` a `ProgressGLTFLoader` para aprovechar la precarga.

## Archivos

Web: `src/components/three/modelPrefetch.ts` (nuevo), `src/components/three/ModelPrefetcher.tsx` (nuevo),
`src/components/three/modelAssetLoading.ts`, `src/features/model3d/AssetInspection.tsx`, `src/app/App.tsx`, `package.json`/`package-lock.json`.
Contracts: `contracts/afr2_build_info.h`, `docs/VERSIONADO.md`, CHANGELOG.

## Pendientes

- Sin compilar ni probar en el navegador ni con el ESP. No se midió si la descarga en paralelo con el WebSocket afecta la estabilidad del
  ESP: si aparecen reinicios durante la precarga, se puede retrasar el inicio o bajar a un solo modelo.
- La memoria del navegador conserva los `.glb` (~1,7 MB) mientras la pestaña está abierta.
