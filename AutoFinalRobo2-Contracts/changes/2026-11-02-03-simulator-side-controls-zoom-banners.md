# AFR2-20261102-03 — Simulador: controles a la derecha, zoom flotante, cámara de los banners y banners re-renderizados

02/11/2026. Pedido humano:
- En los simuladores (auto, brazo y conjunto) los controles quedaban debajo de una
  escena a todo el ancho. Ponerlos a la derecha de la escena, cómodos y sin tener
  que desplazar la página.
- Botones flotantes de vidrio «+ / −» dentro de la escena, con el porcentaje de
  zoom en el color del tema.
- Al entrar a una simulación, la misma disposición de cámara que los banners de
  Modelos (antes entraban «al revés»).
- Volver a renderizar los banners con más calidad, mejor luz y sin sobreexposición,
  con las mismas posiciones de cámara. Blender quedó abierto con el bridge
  «Blender Agent MCP» en 127.0.0.1:9876.

## Cambios

- **Disposición** (`CollectorPage.tsx`, `styles/collector.css`): la tarjeta del
  simulador es un contenedor (`container: sim-card`). Desde 760 px de ancho de
  tarjeta, la escena queda a la izquierda y el panel de controles a la derecha
  (`clamp(320px, 34cqi, 420px)`). El panel usa `contain: size`: toma la altura de
  la columna de la escena y, si su contenido fuese más alto, desplaza por dentro
  (sin mover la página). La escena sube a `clamp(440px, 66svh, 780px)`. Debajo de
  760 px se apila (escena, lecturas y luego controles). Matemática, al abrirse,
  ocupa una fila completa debajo. Dentro del panel, las grillas de dos columnas
  pasan a una y los selectores J1–J6 a una fila de seis.
- **Barra de la escena** (segundo pedido: «quedó muy fea la disposición de botones»,
  porque las herramientas bajaban a una segunda fila): una sola fila con Cámara +
  «Centrar» a la izquierda y las herramientas a la derecha, todas con ícono
  (`crosshair`, `move`, `cube`, `radar`, Lucide). Si la barra mide menos de 640 px,
  las herramientas quedan solo con ícono; por debajo de 470 px, también «Centrar».
  El nombre sigue disponible para lectores de pantalla y como `title`. «Mostrar /
  Ocultar alcance» pasa a ser un interruptor «Alcance» (`aria-pressed`). En el
  panel lateral, las pestañas ocupan todo el ancho en partes iguales y
  «Posiciones» se llama «Cubos» para que entren las cuatro.
- **Zoom** (`CollectorScene.tsx`): grupo flotante horizontal (− · % · +) abajo a la
  derecha de la escena, fuera de la franja superior donde aparece la barra de carga
  del modelo,
  con vidrio `--glass-chrome-*` (una sola región con `backdrop-filter`), botones
  «Acercar» y «Alejar» de 40 px (44 px en táctil) y, entre ellos, el porcentaje
  sobre `--action` / `--text-on-action`. El porcentaje es relativo al encuadre
  inicial (100 %), también sigue la rueda del mouse, va de 25 % a 800 % y tocarlo
  vuelve a 100 %. Pasos ×1,25 / ×0,8 con una transición breve (sin transición con
  `prefers-reduced-motion`). Íconos `plus` y `minus` (Lucide) agregados al registro.
- **Cámara inicial** (`components/three/frameModel.ts`): `BANNER_VIEW_DIRECTION`
  = (−0,62; 0,42; −1) en ejes Three, que es la dirección (−0,62; 1; 0,42) de la
  cámara de Blender de los banners (Blender (x, y, z) ↔ Three (x, z, −y)). La usa
  la vista Perspectiva del simulador (y «Centrar»). Superior y Lateral no cambian.
  Las páginas de inspección de modelos 3D conservan su encuadre anterior.
- **Banners** (`tools/banner-renders/render_banner.py`): mismas cámaras que
  AFR2-20261102-01. Los blancos de los GLB (albedo 1,0) se limitan a 0,8 solo en
  el render; luz principal más grande y baja (62 en vez de 110), relleno 18,
  contraluz 55 y una luz cenital tenue; AgX «Medium High Contrast» con exposición
  −0,3; 256 muestras con muestreo adaptativo, render al 150 % y reducción Lanczos
  en `compose_banner.py`. Se reemplazaron `public/brand/models/model-{auto,brazo,conjunto}-{1200,640}.webp`.
- **Bridge de Blender:** se usó para consultar la versión, el ejecutable y los
  dispositivos de cómputo. No se cargaron modelos en la escena abierta (el
  `.blend` del brazo v17, con cambios sin commit): los renders corrieron en un
  proceso de Blender aparte (`-b --factory-startup`). Al consultar dispositivos se
  cambió el tipo de cómputo de Cycles en las preferencias; como el equipo solo
  ofrece CPU, quedó en «Ninguno».

## Adaptación por archivo

Web/src: `features/collector/CollectorPage.tsx`, `features/collector/CollectorScene.tsx`,
`components/three/frameModel.ts`, `components/ui/icons/registry.ts`,
`styles/collector.css`. Web/tools: `gen-icon-registry.py`,
`banner-renders/render_banner.py`, `banner-renders/compose_banner.py`.
Web/public: `brand/models/*.webp`. Contracts: CHANGELOG.md y README.md; Web/README.md.

## Estado por repo

- **atmega / esp:** sin cambios.
- **web:** fuente modificada. **Verificado en el navegador local** (sin robot):
  - a 1440 y 1600 px, escena a la izquierda y controles a la derecha en auto, brazo
    y conjunto; el panel no supera la altura de la escena; barra en una fila con nombres;
  - a 1000 px (rail), lado a lado (escena 491 px, controles 320 px), barra solo con
    íconos y las cuatro pestañas del conjunto visibles;
  - a 390 px, apilado y sin desplazamiento horizontal;
  - zoom: dos «+» → 156 %, «−» → 125 %, tocar el porcentaje → 100 %;
  - orientación inicial del auto igual a la del banner; el conjunto y el brazo con la
    misma dirección de cámara (el brazo difiere solo en la postura articular inicial
    de la simulación, que no se cambió);
  - página Modelos con los tres banners nuevos.

  No verificado: compilación TypeScript, `build:esp` ni hardware. Durante la prueba,
  otra edición en paralelo agregó `/infraestructura` en `App.tsx`; no es parte de esta ficha.
- Sin commit local ni remoto.
