# AFR2-20261102-01 — Revisión en navegador, flujo de Modelos, banners y árbol de pantallas

02/11/2026. Pedidos humanos, en orden:
1. «Sí, revisar»: revisar en el navegador la fase 5 del
   [plan visual](../../AutoFinalRobo2-Web/docs/plan-visual-20261101/PLAN.md).
2. Separar los simuladores de los modelos 3D y entrar directo a los modelos 3D,
   con tres banners horizontales con renders (auto, brazo robótico, simulación
   conjunta) del mismo estilo que Inicio. Cada uno abre la inspección 3D, con las
   referencias visibles por defecto y ocultables, el selector de partes y abajo a
   la derecha «Ir a simulación». Desde el simulador se puede cambiar entre
   simuladores. Pasar el flujo por las skills.
3. Agregar el banner de Inicio con el render del auto (pendiente del plan).
4. Que el árbol de pantallas sea un mapa conceptual que crezca desde el
   dashboard, con nodos clicables que muestren descripción y ScreenCode.

Autorización: el pedido 1 autoriza levantar el servidor de desarrollo y usar el
navegador integrado. El pedido 2 autoriza generar las imágenes (renders con
Blender). No se ejecutaron tests, `tsc`, build, `build:esp` ni validadores.

## 1. Revisión en navegador (servidor `vite` local, puerto 5174)

**Chequeo automático:** en cada ancho, desplazamiento horizontal, elementos fuera
de la pantalla y cantidad de `<h1>`, más controles de menos de 44 px a 360 px.
Cubrió 13 rutas a 360, 600, 840, 1200 y 1600 px, en tema oscuro y claro.

**Resultado final:** ninguna ruta con desplazamiento horizontal, un `<h1>` por
página y la barra lateral o inferior según el corte. Las 9 redirecciones de
rutas viejas llegan al destino y a la pestaña correctos. La hoja «Más» navega y
se cierra.

Hallazgos corregidos:

| Hallazgo | Corrección |
| --- | --- |
| La pill del selector no se deslizaba: cada cambio de valor creaba un `ResizeObserver` nuevo cuyo aviso inicial reubicaba sin animar | Un observador por montaje, ignorando su aviso inicial (`SegmentedControl`) |
| La pill arrancaba unos 240 ms tarde cuando el cambio montaba una escena 3D | La pill sigue al toque de inmediato y el cambio de contenido va en `useTransition`. Si no se acepta, al terminar vuelve el valor real |
| En tema oscuro la pill (`--surface-solid`) era más oscura que la ranura | Token `--seg-pill-bg` (`#2a4068` en oscuro) |
| El rail se desbordaba 9 px a 768 px de alto por los títulos de grupo | En el rail se ocultan los títulos (siguen como nombre accesible) y una línea separa los grupos |
| `StatusNotice` no tenía estilos: aviso sin fondo ni borde y punto invisible | Fondo, borde y punto por tono con tokens (`headers.css`) |
| Pill de red sin conexión: solo un punto | `wifi-off` sin enlace |
| A 360 px: números compactos de 22 px, deslizadores de 36 px y «Volver» OLED de 40 px | 44 px con `pointer: coarse`; «Volver» a 44 px |
| A 360 px: la cabecera de inspección se salía 13 px | `.afr2-page-header__actions` puede encogerse y saltar de renglón |

Sin conexión al robot, STOP no se muestra (comportamiento previo). Su
visibilidad y los datos reales no se verificaron con hardware.

## 2. Modelos y simulación separados

Skills aplicadas: apple-design (sencillez, orientación, familiaridad,
agrupación), visual-identity-system en APLICAR sobre el manual 0.1.10 y animate
(decisión de movimiento por frecuencia).

- **`/modelos`:** portada con tres banners (Auto robot, Brazo robótico,
  Simulación conjunta), con el mismo material, iconos y acento que las tarjetas
  de Inicio. En escritorio, render a la derecha y texto sobre degradado; en
  celular, texto arriba y render abajo con fundido. Etiqueta «Render CAD» y
  enlace secundario al cubo auxiliar y al simulador.
- **`/modelos/auto|brazo|conjunto|cubo`:** inspección directa, sin selectores
  intermedios. Referencias visibles por defecto, «Ocultar referencias», selector
  de componentes y, abajo a la derecha del visor, «Ir a simulación» (44 px). En
  celular se oculta mientras hay una ficha abierta abajo. Cabecera con «Modelos»
  (volver), «Pausar visor» y «Restablecer vista».
- **Conjunto (nuevo, `CombinedInspection`):** auto (`RobotModel`) y brazo V17
  (`ArticulatedArmAsset`) con la misma cadena que la simulación conjunta
  (`CAD_TO_THREE · mounts.mounted · restToMount`), brazo en pose de reposo. 21
  referencias: 8 del auto y 13 del brazo, agrupadas. La posición es la del
  modelo, no una medición del robot armado.
- **`/simulacion/auto|brazo|conjunta`:** solo el simulador, con selector entre
  los tres en la cabecera y «Ver modelo 3D». El colector recibe el tipo por la
  ruta (`CollectorProvider kind`) y conserva `?tipo=` como respaldo.
- **Redirecciones:** `/simulaciones?tipo=` → `/simulacion/:tipo`, `/brazo`,
  `/simulacion`, `/modelo-3d` → `/modelos`. La barra lateral marca «Modelos y
  simulación» también dentro de `/simulacion/*`.
- Se quitaron `CollectorPage` (contenedor viejo) y el modo `embedded`.

## 3. Banner de Inicio

`HomeHero`: render del plan (`home-hero-1600.webp` desde 840 px,
`home-hero-mobile-720.webp` en celular, con `<picture>`), kicker,
`<h1>` «Movimiento en todas las direcciones», descripción del plan y chips con
datos reales (Wi-Fi, ATmega, Modo, Batería en voltios o «—»), que llevan a
Conexión. Lleva la etiqueta «Render CAD» y no tiene animación propia. El triple
toque pasa a la imagen. PNG original en
`Web/reference/brand/home-hero-render-source.png`, fuera del paquete. Los
banners son oscuros en ambos temas con tokens `--banner-*`.

## 4. Renders de los banners de Modelos

Blender 4.5 (Cycles CPU, 64 muestras, denoise OIDN), desde los GLB que usa la
Web (`AutoFinalRobo2.glb`, `AFR2_Brazo_v17_Blanco.glb`). Fondo transparente con
piso que solo recibe sombra, luz de estudio y cámara 3/4. Composición en Pillow
sobre el degradado medido en el banner de Inicio. Para el conjunto, la
traslación del brazo se tomó del código que corre en la página (0, 79,0 mm,
−27,8 mm en Three.js). Salida en `Web/public/brand/models/`: seis WebP (1200 ×
500 y 640 × 400) de 6 a 17 KB; 114 KB en total junto con los dos del banner de
Inicio. Agregados a la lista explícita de `vite.config.ts`. Scripts
reproducibles en `Web/tools/banner-renders/`. Por el límite de memoria del
entorno, Blender corrió fuera del sandbox de la terminal; solo leyó los GLB y
escribió en la carpeta temporal de la sesión.

## 5. Árbol de pantallas como mapa conceptual

`ScreenTreeMap` reemplaza la lista desplegable de Simulador UI → Árbol y
catálogo (se borró `UiSimulationTree.tsx`).

- **Grafo:** el mismo del simulador (`SIMULATION_FLOW`, adaptado del ATmega).
  La raíz es el dashboard 010001; las ramas son las opciones de menú y los gestos
  (SW corto → 010002, SW largo → 010003). Una pantalla alcanzable por varios
  caminos aparece completa una vez y como referencia punteada en los demás.
  Las pantallas que se alcanzan por acciones o avisos van en un grupo aparte.
- **Disposición:** de izquierda a derecha, con conectores curvos SVG. Se puede
  desplazar dentro del mapa y abrir o cerrar ramas; tiene «Expandir todo»
  (80 entradas: 68 pantallas, 11 referencias y el grupo) y «Contraer».
- **Ficha:** nombre, ScreenCode, tipo, por dónde se llega, subpantallas, giro y
  SW corto/largo, vista previa con la réplica OLED, camino desde el dashboard,
  pantallas hijas navegables y «Abrir en el simulador». La descripción se arma
  con estos datos; `screens.json` no tiene un texto descriptivo para el usuario.
- **Movimiento:** la disposición cambia al instante; los nodos nuevos entran con
  un fundido de 120 ms (nada con teclado o movimiento reducido).
- Ayuda → OLED conserva su catálogo con vista previa; no usa todavía el mapa.

## Adaptación por archivo; autorías anteriores conservadas

Web/src nuevos: `features/models/modelCatalog.ts`,
`features/collector/SimulatorPage.tsx`, `features/model3d/CombinedInspection.tsx`,
`features/home/HomeHero.tsx`, `features/developer/ScreenTreeMap.tsx`,
`styles/models.css`, `styles/screen-tree.css`.

Web/src modificados: `app/App.tsx`, `app/navigation.ts`,
`components/navigation/RouteNav.tsx`, `components/ui/SegmentedControl.tsx`,
`features/models/ModelsPage.tsx`, `features/model3d/Model3DPage.tsx`,
`ModelInspection.tsx`, `AssetInspection.tsx`,
`features/collector/CollectorContext.tsx`, `CollectorPage.tsx`,
`features/home/HomePage.tsx`, `features/developer/UiSimulator.tsx`,
`features/connection/NetworkConnection.tsx`, `styles/tokens.css`,
`styles/index.css`, `styles/headers.css`, `styles/segmented.css`,
`styles/workspace.css`, `styles/robot-controls.css`, `styles/oled-catalogue.css`.

Borrados: `features/developer/UiSimulationTree.tsx`.

Web, fuera de `src`: `vite.config.ts`, `public/brand/` (8 WebP),
`reference/brand/home-hero-render-source.png` y `tools/banner-renders/`.
`.claude/launch.json` en la raíz (configuración del navegador integrado).

Contracts: decisions.md, index.md, coverage-20261102-01.json, CHANGELOG.md y
README.md; Web/README.md y `docs/ui-navigation-20261101.json`.

## Productores/consumidores/wire

Sin cambios de protocolo. Los chips del banner leen el mismo estado de conexión
que la barra superior.

## Estado por repo

- **atmega / esp / CAD:** sin cambios. Los GLB solo se leyeron.
- **web:** fuente y recursos públicos modificados. **Verificado en el navegador
  local** (sin robot conectado): disposición a 5 anchos, tema claro y oscuro,
  rutas y redirecciones, flujo Modelos → inspección → simulación → cambio de
  simulador, visor del conjunto, mapa de pantallas («Expandir todo», ficha,
  «Abrir en el simulador») y animación de la pill medida a mitad de transición.
  No verificado: compilación TypeScript, `build:esp` y presupuesto de LittleFS
  (+114 KB de imágenes), hardware y lectores de pantalla.
- `firmwareLibrary.ts`, que regenera el `predev`, se devolvió a su versión
  (solo cambiaban los finales de línea).
- **ID duplicado:** `2026-11-01-10-estabilidad-traccion-conjunto.md` (trabajo en
  paralelo, 21:58) usa AFR2-20261101-10, ya usado por la fase 4 del plan visual
  (21:51). No se editó esa ficha; queda para decisión humana.
- Sin commit local ni remoto.
