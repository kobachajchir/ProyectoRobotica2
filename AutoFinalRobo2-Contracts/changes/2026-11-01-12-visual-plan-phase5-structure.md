# AFR2-20261101-12 — Plan visual 01-11, fase 5: estructura responsive y navegación

01/11/2026. Pedido humano: «Sí, fase 5» de
[PLAN.md](../../AutoFinalRobo2-Web/docs/plan-visual-20261101/PLAN.md). Aplica
VIS-053 (breakpoints mobile-first y barra inferior en celular, opciones por
defecto autorizadas) y la estructura de 4 grupos con los nombres propuestos.
Solo fuente Web y documentación. Sin commits nuevos sobre `ca9a2ae`; integrado
sobre las fases 1 a 4 y AFR2-20261101-11 sin commitear.

## Navegación y rutas

- **Destinos por tarea** en `Web/src/app/navigation.ts` (fuente única):
  Inicio; Operar: Conducir, Programas; Preparar: Puesta a punto, Sensores;
  Explorar: Modelos y simulación; Sistema: Conexión, Ayuda. Configuración sigue
  como modal; Desarrollador se abre desde Configuración → Acerca de (y el triple
  toque del logo de Inicio).
- **Rutas nuevas:** `/conducir`, `/programas`, `/puesta-a-punto`, `/modelos`,
  `/conexion`, `/ayuda`. **Redirecciones** desde `/control` (según `?tarea=`),
  `/simulaciones`, `/brazo`, `/simulacion`, `/modelo-3d`, `/wifi`, `/oled` y
  `/documentacion`, conservando los parámetros. Enlaces internos actualizados.
  Mapa vigente: [ui-navigation-20261101.json](../../AutoFinalRobo2-Web/docs/ui-navigation-20261101.json).
- **Celular (base):** sin barra lateral; barra inferior con Inicio, Conducir,
  Programas, Sensores y «Más». «Más» abre un modal con OLED en vivo, Puesta a
  punto, Modelos y simulación, Conexión, Ayuda y Configuración.
- **md 840:** rail lateral con grupos. **lg 1200:** barra expandida por defecto
  (la preferencia guardada del usuario se respeta).
- **Barra superior en una fila** en celular: el contexto (título y modo) se
  oculta bajo 600 px; las pills de red y batería muestran icono y valor (texto
  completo en `aria-label`/`title`); STOP conserva 48 px.
- **Barra de estado operativo** (Modo, GOTO, Salidas, Fallo) bajo la barra
  superior solo en Conducir y Programas (`OperationalStatusBar`); sale de la
  cabecera de Control.

## Páginas

| Antes | Ahora |
| --- | --- |
| Control con tres pestañas y tarjetas plegables | `ControlPage` con `area`: **Conducir** (conducción manual y Control táctil), **Programas** (Secuencias / GOTO) y **Puesta a punto** (Ruedas / Calibración / Preferencias). Una sola instancia sirve a las tres rutas: los borradores de GOTO y de la prueba de rueda sobreviven al cambiar de ruta; al cambiar de área se detiene el movimiento manual |
| Configuración → Sistema → Preferencias | Puesta a punto → Preferencias |
| Simulaciones y Modelos 3D separadas | **Modelos y simulación** (`ModelsPage`) con selector Simular / Inspeccionar en la cabecera; las dos páginas se reutilizan con `embedded` (sin repetir el `<h1>`). El proveedor del colector vive solo dentro de Simular |
| Wi-Fi | **Conexión**, con pestaña **Diagnóstico** (detalles de la conexión y WebSocket, antes en Configuración) |
| Documentación + página Pantalla OLED + «Supervisión del control Web» al pie de Control | **Ayuda** con pestañas Web / ESP / ATmega / OLED (catálogo de pantallas) y el capítulo de supervisión; el visor en vivo queda como modal global |
| Configuración → Batería (copia) | Quitada; queda la pill y su modal |
| Inicio con 7 tarjetas (incluida OLED) | Tarjetas de los 7 destinos nuevos desde `navigation.ts` |

`features/oled/OledPage.tsx` se borró (su contenido pasó a Ayuda y al modal).

## CSS

- `workspace.css`: los bloques `max-width` 1100/819/380 px de la estructura se
  reemplazaron por reglas mobile-first (`min-width` 600/840/1200). Excepción
  documentada: bienvenida y botones laterales del encoder siguen con
  `max-width: 839.98px` porque pisan valores de otras hojas que no se pueden
  restablecer desde md sin repetirlos.
- **Breakpoints:** las 30 consultas restantes de 17 hojas pasan de 16 valores a
  los 4 cortes, como complemento exacto (`max-width` 599.98 / 839.98 /
  1199.98 px): 579–640 → sm, 700–820 → md, 900–1279 → lg (lado conservador).
  **Desvío del plan:** no se invirtieron a `min-width` regla por regla en esas
  hojas; hacerlo exige conocer cada valor base y no hay revisión en navegador.
  Corrimientos visibles: paneles que cambiaban a una columna en 900–1100 px
  ahora lo hacen bajo 1200 px; el radar usa su disposición ancha desde 1200 px
  (antes 1280).
- **Tamaños de texto** (pendiente de la fase 4): 140 `font-size` migrados a
  `--type-caption-size` (12), `--type-label-size` (13), `--type-body-size` (15;
  los de 14 px crecen 1 px) y `--type-heading-size` (18). Quedan 82 fuera de la
  escala (10–11 px de datos densos, inputs de 16 px y títulos propios de cada
  componente) y las clases de texto de Tailwind en TSX.

## Adaptación por archivo; autorías anteriores conservadas

Web/src: `app/navigation.ts` (nuevo), `app/App.tsx`,
`components/navigation/RouteNav.tsx`, `components/ui/icons/registry.ts`
(`programs`, `setup`, `help`, `more` de Lucide), `features/control/ControlPage.tsx`,
`features/control/OperationalStatusBar.tsx` (nuevo),
`features/models/ModelsPage.tsx` (nuevo), `features/collector/CollectorPage.tsx`,
`features/model3d/Model3DPage.tsx`, `features/model3d/modelComponents.ts`,
`features/model3d/AssetInspection.tsx`, `features/wifi/WifiPage.tsx`,
`features/connection/SystemPanel.tsx` (`ConnectionDiagnostics`),
`features/connection/NetworkConnection.tsx`, `features/docs/DocsPage.tsx`,
`features/home/HomePage.tsx`, `features/oled/OledPage.tsx` (borrado),
`styles/workspace.css`, `styles/tokens.css` y 17 hojas con breakpoints o
tamaños. Web/tools/gen-icon-registry.py. Web/docs/ui-navigation-20261101.json
(nuevo). Contracts: decisions.md, tokens.json, index.md,
coverage-20261101-12.json, CHANGELOG.md, README.md; Web/README.md.

## Productores/consumidores/wire

Sin cambios de protocolo, comandos ni permisos. La lógica de conducción,
secuencias, GOTO, prueba de rueda, calibración y preferencias es la misma; solo
cambia en qué ruta se muestra.

## Estado por repo

- **atmega / esp:** sin cambios.
- **web:** fuente modificada. Sin tests, compilación, typecheck, validadores,
  navegador, `build:esp` ni promoción. Comprobaciones por texto: imports y
  módulos relativos existentes, sin imports sin usar, sin finales de línea
  mezclados, sin enlaces internos a rutas viejas fuera de las redirecciones.
  **No verificado** el criterio del plan (sin desplazamiento horizontal a
  360/600/840/1200/1600 px, STOP visible, objetivos de 44 px).
- **shared/Contracts:** registro y decisiones actualizados.
- Sin commit local ni remoto.

## Pendiente

- **Revisión en navegador recomendada por el plan al cerrar esta fase** (cinco
  anchos, claro/oscuro, movimiento reducido): es la fase con más riesgo de
  regresiones de disposición.
- Invertir a `min-width` las reglas de las 17 hojas de componentes cuando haya
  verificación visual; migrar los tamaños fuera de escala.
- Fase 6 (banner, Conducir con sensores en vivo, Programas con simular y
  ejecutar separados, hojas inferiores, container queries) y fase 7 (OLED).
