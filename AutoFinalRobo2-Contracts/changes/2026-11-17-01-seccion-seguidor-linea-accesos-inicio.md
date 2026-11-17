# AFR2-20261117-01 - Sección Seguidor de línea, accesos rápidos de Inicio y hash por capa

Autoría: Koba Chajchir / AFR2.
Versión: 1.33.0 (subversión). Estados: atmega = solo versión; esp = solo versión; web = en fuente. Sin CMD nuevo ni cambio de contrato UART/WS.

## Pedido

1. Mantener el intervalo de sincronización por defecto en 5000 ms y revisar la detección de capas modificadas (el informe de AFR2-20261116-01 hablaba de hash FNV-1a, pero el código comparaba `JSON.stringify`).
2. Una sección donde se vea el auto como en el control táctil, con la cinta del ancho configurado y en escala por el centro, y el auto moviéndose de un lado a otro mientras se renderiza su posición.
3. En Inicio, debajo del banner, accesos **Ir**, **Controlar** y **Seguidor de línea** con el estilo de los demás; al pasar el cursor muestran de fondo una imagen de la interfaz correspondiente, con animación.

## Cambios (Web)

- `src/features/sync/cloudSyncSchema.ts`: `CLOUD_SYNC_AUTO_LAYERS` (9 capas; `simulators` fuera), `fnv1a32`, `computeLayerHashes` sobre JSON canónico (claves ordenadas, sin `undefined`). `buildCloudSyncEnvelope` recibe `ackedHashes` y devuelve `dirtyLayers` y `hashes`.
- `src/features/sync/CloudSyncController.tsx`: guarda solo los hashes del último envío (`pendingHashesRef`) y, al confirmarse, los pasa a `ackedHashesRef`. Antes se guardaba el estado completo vigente al confirmar, que podía no ser el enviado. Intervalo por defecto sin cambio: 5000 ms.
- `src/features/control/LineFollowerLiveView.tsx` (nuevo): vista cenital con cámara fija; cinta fija en x = 0 con el ancho de `afr2.sensors.lineTapeMm` (misma preferencia que Sensores) en metros reales. El auto se ubica en `position × separación entre IR` (positivo = línea a la izquierda = auto a la derecha), gira ±0,22 rad según la orientación estimada y se interpola de forma exponencial. Con línea perdida se corre hacia el último lado; con línea horizontal se dibuja el cruce. Marcas laterales del piso corren cuando el modo es `line-follow` y no está detenido. Sin auto o sin lectura válida: vista previa local (zigzag de 4 s, IR calculados por geometría), sin enviar comandos.
- `src/features/lineFollower/LineFollowerPage.tsx` (nuevo) y ruta `/seguidor-linea` en `App.tsx`; `navigation.ts` le da título y la asocia a Programas en la navegación.
- `src/features/control/LineFollowerPanel.tsx`: prop `showView`; en Programas usa la vista nueva y enlaza a la sección.
- `src/features/sensors/LineTopView.tsx`: exporta `useSensorBar`.
- `src/features/home/HomeQuickLinks.tsx` (nuevo) y `HomePage.tsx`: tres accesos bajo el banner (Ir → `/programas?tarea=goto`, Controlar → `/conducir`, Seguidor de línea → `/seguidor-linea`).
- `src/styles/line-follower.css` (nuevo, importado en `index.css`): tarjetas compactas; en `:hover`/`:focus-visible` la imagen pasa de opacidad 0 y escala 1,08 a 1 con un velo oscuro. Se respeta `prefers-reduced-motion`; en celular, una columna.
- `public/home/quick-goto.svg`, `quick-control.svg`, `quick-line.svg`: ilustraciones de cada interfaz. Para usar capturas reales, reemplazarlas con el mismo nombre (o cambiar la extensión en `HomeQuickLinks.tsx`).

## Ampliación del mismo cambio (sin nueva versión)

Pedido: que la sincronización de la barra sea una pill que abra su modal de parámetros, con el formato de los demás modales, y que en el seguidor el piso se mueva en la dirección de avance.

- `src/features/sync/CloudSyncController.tsx`: `CloudSyncPill({ onOpen })` usa el material de Red y Batería (`afr2-network-pill afr2-sync-pill`). Antes usaba `afr2-status-pill`, que no tiene estilos, y se veía como texto suelto. `CloudSyncDetails` es el contenido del modal: activar o desactivar; intervalo de 5 a 10 s con botones de opción; endpoint validado (http/https), que se guarda al salir del campo o con Enter; detalles (estado, fallas, última sincronización, tamaño, capas, revisión); «Sincronizar cambios ahora» y «Enviar todo». Nuevo `hasTarget`: sin endpoint y sin auto conectado no hay a dónde enviar. El estado es «Sin destino»: no se marca «Online» (antes sí, por error) ni se cuenta como falla, así que el aviso de 5 fallas no aparece al abrir la Web sin el auto.
- `src/components/navigation/RouteNav.tsx`: modal `sync` ("Sincronización web") administrado por la barra, como `network` y `battery`.
- `src/features/control/LineFollowerLiveView.tsx`: las marcas laterales se reemplazan por una trama de puntos en todo el piso (`CanvasTexture` con repetición cada 4 cm). La trama se desplaza en sentido contrario al avance a `v·cos(giro)`; el desvío lateral lo muestra el propio auto al correrse sobre la cinta.
- `src/styles/line-follower.css`: estilos de la pill (encendida, aviso, apagada, ícono girando mientras sincroniza) y del modal.

## Estado de validación

Revisado en el servidor de desarrollo de Vite sin auto (17/11/2026), a pedido del usuario: en escritorio y a 375 px, sin desplazamiento horizontal y sin errores en la consola. En los accesos de Inicio, la imagen aparece con fundido al pasar el cursor (tema oscuro). La vista previa zigzaguea sobre la cinta. Correcciones hechas en esa revisión: el encuadre ahora incluye el auto completo más su recorrido lateral (antes se cortaban las ruedas en celular); el auto gira alrededor de la barra IR y no del centro del chasis; los IR de la vista previa se calculan con la pose dibujada; el velo de los accesos es más claro. Sin compilar ni `tsc`. Pendiente: probar con una lectura IR real (sentido del desplazamiento en modo en vivo), en tema claro y la marcha del seguidor en el auto. Ampliación revisada en el mismo servidor: la pill abre el modal; el intervalo y el endpoint se guardan; el endpoint inválido se marca; sin auto muestra «Sin destino»; en celular la pill queda solo con el ícono; el piso del seguidor corre hacia abajo mientras el auto avanza.
