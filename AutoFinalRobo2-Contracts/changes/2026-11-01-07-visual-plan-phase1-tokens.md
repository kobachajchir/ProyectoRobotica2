# AFR2-20261101-07 — Plan visual 01-11, fase 1: decisiones y tokens

01/11/2026. Pedido humano: implementar la fase 1 («Base: decisiones y tokens»)
de [PLAN.md](../../AutoFinalRobo2-Web/docs/plan-visual-20261101/PLAN.md), con
autorización expresa para aplicar el manual visual en borrador y usar las
opciones por defecto del plan (Lucide, barra inferior en celular, `--ease-in-out`
para la pill, cuatro grupos y textos propuestos). Decisiones VIS-050..VIS-054,
manual 0.1.10 draft. Solo fuente Web y documentación de Contracts.

## Integración con el repositorio

Revisado antes de editar: `git log` sin commits posteriores a `ca9a2ae`.
`git status` mostraba cambios ajenos sin commitear (skill oled128-ui-design,
fichas OLED128 del día, `CHANGELOG.md`, `README.md`, SKILL de afr2-screen-authoring
y un `.blend` del brazo). No se tocaron esos archivos salvo agregar entradas
arriba en `CHANGELOG.md` y `README.md`, conservando lo existente. Los IDs
AFR2-20261101-01..06 ya estaban usados (dos veces cada uno por trabajo en
paralelo); esta ficha toma el 07.

## Antes/después

| Antes (Observada en fuente) | Después |
| --- | --- |
| `--dur-fast`, `--dur-press`, `--on-action` sin definir en `workspace.css`: el navegador descarta esas transiciones y el texto sobre acción hereda color | `--duration-fast`, `--duration-button-press`, `--text-on-action` |
| `--accent` sin definir (`oled-catalogue.css`, `modelAssetProgress.css` con respaldo fijo `#c02646`) | `--focus` en el foco, `--action` en la barra de progreso |
| `--border` sin definir en clases Tailwind de `StatusNotice`, `EncoderControl`, `LiveOledViewer` | `--border-default` |
| `--glass-field` sin definir (usaba el respaldo) en la pill de red de `sensors.css` | `--glass-field-bg` |
| Ámbar fijo `#f59e0b/#fbbf24` en la pulsación larga, igual en ambos temas | `--attention*` por tema (VIS-054); glifo del encoder con `--attention-ink` |
| Paleta `--map-*` definida en `control-map.css` y repetida a mano en `ControlMapScene` | Paleta en `tokens.css` con los mismos valores; ControlMapScene la lee con `getComputedStyle` (patrón de RadarScene) y suma `--map-waypoint-muted` |
| `ThemeProvider` repetía los colores del lienzo para `theme-color` | Lee `--canvas` del tema activo |
| `--welcome-sheen` fijo en `startup.css` | `--startup-sheen` en `tokens.css` |
| `--font-sans` declaraba Inter, que no se cargaba | Segoe UI Variable Text / Display → `system-ui` (VIS-004/VIS-051) |
| Sin escala tipográfica ni espaciado por rol | `--type-*` con `--type-*-tracking`, `--page-gutter`, `--page-top`, `--section-gap`, `--card-padding`, `--stack-gap`, `--content-max` |
| Breakpoints por defecto de Tailwind (640/768/1024/1280) | `@theme` con 600/840/1200/1600 y referencia `--bp-*` |

Búsqueda por texto en `Web/src` al cerrar (no es un validador): ninguna
`var(--…)` queda sin definición. Colores fijos fuera de `tokens.css` solo en las
excepciones registradas: `OledCanvas128x64.tsx` (réplica del panel físico; se
revisa con oled128-ui-design antes de tocarla), `RadarScene.tsx` (luz blanca y
máscara alfa WebGL, no colores de tema) e `index.html` (`theme-color` inicial
antes de que cargue el CSS).

## Efectos visibles esperados (no comprobados)

- Las transiciones de los botones OLED, de giro y de cierre de avisos ahora sí
  se declaran; antes no había feedback de pulsación.
- En Windows con Segoe UI Variable la familia de texto cambia; en otros sistemas
  sigue la fuente del sistema.
- 15 clases Tailwind en 7 archivos cambian su punto de activación:
  `sm:` 640→600 px (RouteNav, SectionHeader, ControlPage ×3, Model3DPage,
  EncoderControl, WifiPage ×3), `md:` 768→840 px (SequenceEditorPanel),
  `lg:` 1024→1200 px (ControlPage ×3, SequenceEditorPanel).
- La pulsación larga en tema oscuro usa `#fbbf24` en vez de `#f59e0b`.
- Los tokens `--type-*` y de espaciado se definen pero todavía no se aplican a
  páginas (fase 4) ni al workspace (fase 5).

## Adaptación por archivo; autorías anteriores conservadas

- Web/src/styles/tokens.css: cabecera, familias, `--startup-sheen`, bloques
  VIS-051 (tipo y espaciado), VIS-053 (`--bp-*`), VIS-054 (`--attention*`) y
  paleta `--map-*` trasladada.
- Web/src/styles/index.css: `@theme` con `--breakpoint-sm/md/lg/xl`.
- Web/src/styles/workspace.css: variables de duración y tinta corregidas;
  ámbar reemplazado por `--attention*`.
- Web/src/styles/control-map.css: se quita la definición local de `--map-*`.
- Web/src/styles/startup.css, sensors.css, oled-catalogue.css,
  Web/src/components/three/modelAssetProgress.css: variables corregidas.
- Web/src/features/control/ControlMapScene.tsx: paleta leída de tokens.
- Web/src/app/providers/ThemeProvider.tsx: `theme-color` desde `--canvas`.
- Web/src/features/oled/EncoderControl.tsx, LiveOledViewer.tsx,
  Web/src/components/ui/StatusNotice.tsx: `--border-default`,
  `--text-on-action` y `--attention-ink`. No cambian píxeles OLED,
  identidades de pantalla ni la navegación corto/largo; no son archivos
  firmados por `firmwareRenderer.sources.json`.
- Contracts/docs/design-system/decisions.md: VIS-050..VIS-054.
- Contracts/docs/design-system/manual.md: encabezado 0.1.10 y resumen del plan.
- Contracts/docs/design-system/tokens.json: `manualVersion` 0.1.10 y bloque
  `visualPlan20261101`.
- Contracts/docs/design-system/index.md y coverage-20261101-07.json.
- Contracts/CHANGELOG.md, Contracts/README.md, Web/README.md: registro.

## Productores/consumidores/wire

Sin cambios. No se tocan protocolos, comandos, UART, EEPROM, pinout ni skills OLED.

## Estado por repo

- **atmega:** sin cambios.
- **esp:** sin cambios; `ESP/data/dist` conserva el paquete anterior.
- **web:** fuente modificada. Sin tests, compilación, validadores, navegador,
  `build:esp` ni promoción. Aspecto, contraste y comportamiento No verificados.
- **shared/Contracts:** documentación de diseño y registro actualizados. El
  único chequeo ejecutado fue leer `tokens.json` como JSON tras editarlo.
- Sin commit local ni remoto en esta entrega.

## Pendiente

- Fases 2 a 6 del plan (iconos, selector segmentado, títulos, estructura
  responsive y navegación, pantallas y cierre).
- Revisar `OledCanvas128x64.tsx` con oled128-ui-design para mover sus colores a
  tokens sin cambiar píxeles.
- Comprobación en navegador a 360/600/840/1200/1600 px, claro/oscuro y
  movimiento reducido, y `build:esp` para el presupuesto de LittleFS, solo si se piden.
- `validate-manual.py` sigue esperando la versión 0.1.3 (desactualizado desde
  antes de esta ficha); no se ejecutó ni se modificó.
