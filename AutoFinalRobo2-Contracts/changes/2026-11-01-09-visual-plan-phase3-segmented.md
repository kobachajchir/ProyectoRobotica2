# AFR2-20261101-09 — Plan visual 01-11, fase 3: selector segmentado

01/11/2026. Pedido humano: «Sí, fase 3» de
[PLAN.md](../../AutoFinalRobo2-Web/docs/plan-visual-20261101/PLAN.md). Aplica
VIS-052 (pill por `clip-path`, 250 ms con `--ease-in-out`, opción por defecto
autorizada) sobre el manual 0.1.10 draft. Solo fuente Web y documentación.
`git log` sin commits nuevos sobre `ca9a2ae`; se trabajó sobre las fases 1 y 2
sin commitear.

## Componente

`Web/src/components/ui/SegmentedControl.tsx` y `Web/src/styles/segmented.css`:

- Ranura `--surface-subtle`, radio de pill, padding 4 px, borde interior y
  sombra hundida (`--seg-track-inset`). Alto 44 px (`md`); `sm` 36 px solo con
  puntero fino.
- Pill: copia de la fila con fondo `--surface-solid` y texto `--text-strong`,
  recortada con `clip-path: inset(… round 9999px)` escrito en el elemento.
  Fondo y texto cambian juntos. La sombra y el borde fino van con `filter:
  drop-shadow()` en el contenedor (`--seg-pill-shadow`), porque `clip-path`
  recorta `box-shadow`. Solo se anima `clip-path`.
- Movimiento: `--duration-segmented` 250 ms con `--ease-in-out`. Sin transición
  en el primer render (se habilita después del primer frame), al cambiar de
  tamaño (`ResizeObserver`), con teclado (flechas, Inicio, Fin, Enter/Espacio) ni
  con movimiento reducido (`--duration-segmented` 0 ms). Interrumpible: al
  tocar otra opción a mitad de camino, la pill cambia de destino desde donde está.
- Variantes `tabs` (`tablist`/`tab`, `aria-selected`, ids y `aria-controls`) y
  `choice` (`radiogroup`/`radio`, `aria-checked`). Foco itinerante; opciones
  deshabilitadas se saltean.
- `activation="manual"`: las flechas solo mueven el foco y Enter/Espacio o un
  toque eligen. Se usa en el modo del robot, porque elegir envía una orden.
- `reselect`: volver a tocar la opción elegida llama de nuevo a `onChange`,
  para conservar el comportamiento anterior del modo del robot (reenviar) y de
  la pista de rueda (volver a seleccionarla en el editor).
- `fill`: con 2 a 4 opciones, en celular (menos de 600 px) se reparten en
  columnas iguales a todo el ancho; `always` y `never` para casos fijos. Si no
  entran, la ranura se desplaza horizontalmente con `scroll-snap` y la opción
  activa se trae a la vista sin desplazar la página.
- Pulsación: el texto pasa a `scale(.97)` en 150 ms con `--ease-out`, solo con
  puntero. Hover de color solo con puntero fino. Alto contraste: borde fuerte.
- Selectores con prefijo `.afr2-seg` para que reglas globales como
  `.afr2-collector button { min-height }` no descuadren los botones respecto
  de la copia.

## Dónde se usa

| Antes | Después |
| --- | --- |
| `TaskSections` con `.afr2-task-nav` (Control, Sensores, Wi-Fi, Simulaciones, Modelos 3D, Documentación, Protocol Studio, Simulador UI) | `TaskSections` usa `SegmentedControl variant="tabs"` por dentro; API sin cambios, pantallas sin tocar |
| `.afr2-map-views` vista del mapa de Control | `choice`, `sm` |
| Alcance del radar 0.5 / 1 / 2 / 4 m y vista Perspectiva/Superior | `choice`, `sm` (vista con iconos) |
| Cámara del simulador Perspectiva/Superior/Lateral (botones con `aria-pressed`) | `choice`, `sm`; «Centrar» sigue como botón |
| `.afr2-theme-choices` Apariencia | `choice` |
| `.afr2-segmented` modo del robot | `choice`, `activation="manual"`, `reselect`, columnas iguales |
| `.afr2-segmented` lado de curva | `choice` con iconos de giro, columnas iguales |
| `.afr2-segmented` pista de rueda destino | `choice`, `sm`, `reselect` |

CSS borrado: `.afr2-task-nav button`, `.afr2-map-views*` (control-map.css y
radar.css, incluidas sus reglas de pulsación y movimiento reducido),
`.afr2-theme-choices*`, `.afr2-segmented[…]` (control-cards.css y
control-refinements.css) y el fondo de la primera fila de acciones del
colector. Quedan fuera, como dice el plan: `.afr2-oled-side` (replica un
control físico; se revisa en la fase 7 con oled128-ui-design) y los botones
toggle con `aria-pressed` (Mover auto, Mostrar alcance, Referencias, sentido de
giro con dibujo del chasis), que no son selectores.

## Adaptación por archivo; autorías anteriores conservadas

Web/src: `components/ui/SegmentedControl.tsx` (nuevo), `styles/segmented.css`
(nuevo), `styles/index.css` (import), `styles/tokens.css` (`--seg-track-inset`,
`--seg-pill-shadow` en ambos temas), `components/ui/TaskSections.tsx`,
`components/navigation/RouteNav.tsx`, `features/control/ControlMap.tsx`,
`features/control/ControlPage.tsx`, `features/control/sequences/SequenceEditorPanel.tsx`,
`features/sensors/SensorsPage.tsx`, `features/collector/CollectorPage.tsx`,
`styles/workspace.css`, `styles/control-map.css`, `styles/radar.css`,
`styles/collector.css`, `styles/control-cards.css`, `styles/control-refinements.css`.
Contracts: decisions.md, tokens.json, index.md, coverage-20261101-09.json,
CHANGELOG.md, README.md; Web/README.md.

## Productores/consumidores/wire

Sin cambios. El modo del robot sigue llamando a `handleModeSubmit` con el
mismo valor; no cambian comandos, permisos ni mensajes.

## Estado por repo

- **atmega / esp:** sin cambios.
- **web:** fuente modificada. Sin tests, compilación, typecheck, validadores,
  navegador, `build:esp` ni promoción. Movimiento de la pill, alineación de la
  copia con los botones, sombra en ambos temas, desplazamiento horizontal y
  teclado No verificados.
- **shared/Contracts:** registro y decisiones actualizados.
- Sin commit local ni remoto.

## Pendiente

- Revisión en navegador recomendada por el plan al cerrar esta fase: cinco
  anchos, claro/oscuro, movimiento reducido y teclado. Puntos a mirar: que la
  copia coincida con los botones en cada pantalla, la pill en pantallas ocultas
  que se muestran después y el foco dentro de la ranura.
- `.afr2-oled-side` en la fase 7. Fases 4 a 7.
