# AFR2-20261102-10 — Infraestructura: cubo en el peso 3D, LittleFS desde la placa y paso de Blender

02/11/2026. Pedido humano sobre la página de AFR2-20261102-04/07:
- Incluir el cubo auxiliar en el contador de «Cuánto pesa el 3D».
- La ocupación de LittleFS se puede traer con un comando desde la placa.
- Sacar «Fuente en el proyecto» de todas las fichas del recorrido.
- Explicar que en Blender también se «minifica» el modelo: se sacan mallas internas
  y se optimiza.

## Cambios

- **Cuánto pesa el 3D:** la fila resumen pasa de «Conjunto: auto + brazo» a
  «Simulación conjunta: auto + brazo + cubo», con barra apilada de tres tramos
  (1,59 MiB con el catálogo actual). La simulación conjunta usa el GLB del cubo,
  reutilizado para cada pieza.
- **Ocupación de LittleFS:** el ESP ya devuelve `filesystem.usedBytes` y
  `filesystem.totalBytes` (`Storage::health`) en `system.info` y en la respuesta de
  `system.hello`, y la Web ya pide `system.info` al conectarse (`ConnectionContext`).
  Infraestructura usa primero `/api/health`, cuando la Web se sirve desde el ESP, y
  si no, la respuesta de la placa por WebSocket. El bloque «Flash dedicada a
  LittleFS» se muestra siempre:
  - con datos: anillo, bytes, origen y hora de la respuesta;
  - con la placa conectada: botón «Consultar a la placa» / «Volver a consultar»;
  - sin conexión: indicación y enlace a Conexión.

  No hay comando nuevo ni cambios de firmware.
- **Recorrido:** se quitó la línea «Fuente en el proyecto» de todas las etapas.
  El campo `source` sigue en `infrastructureFlow.ts` como procedencia, pero no se
  muestra.
- **Blender:** la etapa pasa a llamarse «Preparar y aligerar en Blender» y explica
  que, así como Vite minimiza el código, en Blender se «minimiza» el modelo. Detalles
  respaldados por `blender_final_20261025.py` (auto) y `web_blanco/blender_preparar.py`
  (brazo):
  - quedan afuera las mallas ocultas, las referencias y los cables (la exportación de
    Fusion omite lo no visible);
  - se sueldan vértices duplicados, se disuelven caras coplanares y Decimate reduce
    los polígonos por pieza;
  - se unifican materiales y se agrupan piezas o se comparten como instancias.

  La eliminación de mallas internas se incluyó según lo indicado por el usuario; el
  script del auto no tiene un paso explícito que borre geometría interior.

- **Portada en tema claro** (segundo pedido: el fondo degradado quedaba fijo en
  oscuro). Tokens propios `--infra-hero-*` en `tokens.css`:
  - `.dark` conserva los valores de AFR2-20261102-07: noche azul, halos celeste y
    rubí, tinta clara;
  - `:root` (claro) mantiene la misma composición (grilla, halos, brillo, carriles
    por zona, nodos de vidrio y paquetes) sobre papel frío `#f3f6fb`, con tinta
    `#0f172a`, texto secundario `#3f4b5e` y zonas profundas (`#0369a1`, `#a16207`,
    `#b81d33`, `#15803d`) para títulos, etiquetas y degradado del `h1`, con contraste
    AA. Nodos, chips y aviso pasan a blanco translúcido con sombras suaves.

  Se retiraron `--banner-zone-*`, que solo usaba esta portada; los `--banner-*` de
  Inicio y Modelos no cambian.

## Adaptación por archivo

Web/src: `features/infrastructure/InfrastructurePage.tsx`,
`features/infrastructure/infrastructureFlow.ts`, `styles/infrastructure.css` y
`styles/tokens.css`.
Contracts: CHANGELOG.md y README.md.

## Estado por repo

- **atmega / esp:** sin cambios (se usa `system.info` existente).
- **web:** fuente modificada. **Verificado en el navegador local** sin robot: barra
  del conjunto con tres tramos, «Fuente en el proyecto» ausente, bloque de LittleFS
  en estado «sin conexión» con enlace a Conexión. Portada a 1440 px en claro (fondo
  `rgb(243, 246, 251)`) y oscuro (`rgb(11, 20, 36)`, igual que antes), y a 390 px en
  claro sin desplazamiento horizontal.

  No verificado: lectura real de `system.info` con la placa, compilación TypeScript,
  `build:esp` ni hardware.
- Sin commit.
