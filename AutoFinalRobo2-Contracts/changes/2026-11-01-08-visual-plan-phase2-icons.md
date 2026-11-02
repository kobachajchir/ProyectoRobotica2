# AFR2-20261101-08 — Plan visual 01-11, fase 2: iconos

01/11/2026. Pedido humano: después de la fase 1, sumar la revisión de pantallas
OLED al plan como última etapa y continuar con la fase 2 (iconos) de
[PLAN.md](../../AutoFinalRobo2-Web/docs/plan-visual-20261101/PLAN.md). Aplica
VIS-050 (Lucide, opción por defecto autorizada) sobre el manual 0.1.10 draft.
Solo fuente Web y documentación.

## Plan actualizado: etapa 7

PLAN.md suma la fase 7 «Pantallas OLED: réplica Web y auto con la misma
identidad»: tokens de `OledCanvas128x64` sin cambiar píxeles, acciones rápidas
010003 en tarjetas OLED128-4 (agrupación propuesta a confirmar), dashboard
OLED128-3 y radar Web/OLED con el alcance del ATmega. Queda pendiente el dato
del alcance nuevo: en `ca9a2ae` el ATmega usa 4 / 2 / 1 / 0,5 m
(`afr2_sensor_screens.c` `range_mm`, `afr2_menu_catalog.c`), igual que la Web, y
no se encontró otra versión en ramas, worktrees ni stash.

## Antes/después

- **Registro único:** `Web/src/components/ui/icons/registry.ts` con 58 iconos.
  44 trazos de Lucide copiados de `lucide-static@0.460.0` (unpkg) con la licencia
  ISC en el encabezado, convertidos a un `d` por elemento. 14 propios en la
  misma grilla: `arm`, `servo`, `wheel`, `antenna`, `screen`, `encoder`,
  `battery-0..4` y `battery-unknown`. Unos 9,9 KB sin comprimir; sin paquete
  de iconos en el bundle. Generador reproducible en
  `Web/tools/gen-icon-registry.py` (lee SVG locales de Lucide; no se ejecuta en
  el build).
- **UiIcon:** misma API `name`; los nombres históricos (`home`, `control`,
  `cube`, `close`, `arrow`…) se conservan y apuntan al dibujo nuevo. Nueva prop
  `size` 16/20/24 (por defecto 20, antes 22 fijo), trazo 1,75 px, `currentColor`,
  `aria-hidden` por defecto o `label` para `role="img"`.
- **Glifos Unicode reemplazados** (los que funcionaban como icono):
  STOP «■» → `stop` (octágono con cruz) en RouteNav, FloatingModal y
  StartupWelcome; pad Mecanum ↖↑↗←→↙↓↘ ↶↷ y «■» → flechas, `rotate-ccw/cw` y
  `square` («Detener consigna» no es el STOP de emergencia); cerrar toast «✕»;
  GOTO ✓ ↑ ↓ ×; editor de secuencias ▶ ⏸ ⏹, ↶ ↷ de curva y deshacer/rehacer,
  insignias ↶ ↷ ⏸ de bloques; Desarrollador ↗ (externo en GitHub/LinkedIn,
  flecha en tarjetas internas); Wi-Fi ↻; «Ver análisis →» de Protocol Studio.
- **SVG sueltos al registro:** batería por bandas de voltaje (sin lectura:
  batería con raya, antes contorno punteado; ya no se dibujan las barras vacías
  tenues), señal Wi-Fi por nivel (`wifi`, `wifi-high`, `wifi-low`, `wifi-zero`;
  antes arcos apagados al 22 %), iconos de tono de los toasts, refrescar de
  Sistema y del encoder, Control táctil (`touch`; antes un icono de pantalla
  completa), chevrons del menú lateral OLED, `Chevron` común, candado de red y
  papelera de pista.
- **Fichas del modelo 3D:** ATmega usa `board` (antes `panel`) y encoder usa
  `encoder` (antes `settings`).
- Se mantienen como texto las flechas y «×» dentro de frases y medidas
  («Sistema → Preferencias», «128 × 64», «Alt+←»). Se mantienen como dibujo los
  gráficos que no son iconos (estela del táctil, mapa, giro del chasis, guías del
  modelo 3D, `EncoderGlyph` del bitmap OLED).

## Adaptación por archivo; autorías anteriores conservadas

Web/src: `components/ui/icons/registry.ts` (nuevo), `components/ui/UiIcon.tsx`,
`components/ui/Chevron.tsx`, `components/ui/FloatingModal.tsx`,
`components/ui/StartupWelcome.tsx`, `components/navigation/RouteNav.tsx`,
`components/notifications/notify.tsx`, `components/robot/MecanumDrivePad.tsx`
(`DriveIntent.symbol` pasa a `icon`), `features/connection/BatteryStatus.tsx`,
`NetworkConnection.tsx`, `SystemPanel.tsx`, `features/control/ControlPage.tsx`,
`goto/GotoCommandList.tsx`, `sequences/MotorBlock.tsx`,
`sequences/SequenceEditorPanel.tsx`, `features/developer/DeveloperPage.tsx`,
`features/model3d/modelComponents.ts`, `features/oled/EncoderControl.tsx`,
`features/oled/MenuSideControls.tsx`, `features/protocolStudio/CaptureInspector.tsx`,
`features/wifi/WifiPage.tsx`, `styles/startup.css` (separación icono/texto de STOP).
Web/tools/gen-icon-registry.py (nuevo). Web/docs/plan-visual-20261101/PLAN.md
(fase 7). Contracts: decisions.md, tokens.json, index.md,
coverage-20261101-08.json, CHANGELOG.md, README.md; Web/README.md.

Los archivos OLED tocados (`EncoderControl`, `MenuSideControls`) solo cambian
iconos de la interfaz Web alrededor del visor: no cambian píxeles OLED,
ScreenCodes, navegación ni archivos firmados por `firmwareRenderer.sources.json`.

## Productores/consumidores/wire

Sin cambios. No se tocan protocolos, comandos, UART, EEPROM ni firmware.

## Estado por repo

- **atmega:** sin cambios.
- **esp:** sin cambios; `ESP/data/dist` conserva el paquete anterior.
- **web:** fuente modificada. Sin tests, compilación, typecheck, validadores,
  navegador, `build:esp` ni promoción. Aspecto, alineación de iconos en texto,
  contraste y tamaños No verificados.
- **shared/Contracts:** registro y decisiones actualizados.
- Comprobaciones hechas: búsqueda por texto de nombres de icono usados contra
  el registro (todos existen) y de glifos restantes (solo quedan en texto).
  Se bajaron 47 SVG y la licencia de Lucide desde unpkg a una carpeta temporal.
- Sin commit local ni remoto.

## Pendiente

- Comprobar en navegador alineación y tamaño de iconos dentro de texto
  (`align-[-3px]`), barra lateral, pad y toasts, en ambos temas.
- `build:esp` para confirmar el impacto en LittleFS, solo si se pide.
- Fases 3 a 7. Para la 7: confirmar el alcance nuevo del radar y la agrupación
  de las acciones rápidas en tarjetas.
