# AFR2-20261101-10 — Plan visual 01-11, fase 4: títulos y contenido

01/11/2026. Pedido humano: «Sí, fase 4» de
[PLAN.md](../../AutoFinalRobo2-Web/docs/plan-visual-20261101/PLAN.md). Aplica
VIS-051 (escala `--type-*`) sobre el manual 0.1.10 draft. Solo fuente Web y
documentación. Sin commits nuevos sobre `ca9a2ae`; integrado sobre las fases
1 a 3 sin commitear.

## Cambios

- **PageHeader** (`Web/src/components/ui/PageHeader.tsx`, nuevo): único `<h1>`
  de la página con `--type-title`, acciones a la derecha, descripción en
  `--type-body`/`--text-muted` (máximo 60ch), insignia opcional y fila 3
  opcional. Espacios: 8 px título-descripción y 20 px hasta la fila 3; hasta el
  contenido manda el gap de cada vista (32 px en pilas y Modelos 3D, 24 px en
  Control y Simulaciones, sin cambios).
- **SectionHeader** queda para encabezados internos: `<h2>` con
  `--type-heading` o `<h3>` con `--type-subheading` (`level={3}`). Se quitaron
  las clases Tailwind de tamaño y los estilos en línea. Insignia común
  `HeaderBadge` con tonos por `data-tone`.
- **Páginas con `<h1>` único**, igual al nombre de la barra lateral: Control,
  Simulaciones, Sensores, Wi-Fi, Pantalla OLED, Modelos 3D, Documentacion,
  Desarrollador, Protocol Studio y Simulador UI. Inicio conserva su `<h1>` de
  bienvenida, ahora con `--type-display` (el banner es de la fase 6). La
  bienvenida animada pasa su título de `<h1>` a `<h2>` para no sumar un segundo
  `<h1>` mientras está abierta.
- **Niveles sin saltos:** en Wi-Fi, «Estado de conexion», «Datos de red» y
  «Cambiar modo» pasan de `<h2>` a `<h3>` bajo el `<h2>` Conectado/Desconectado
  (clase `.afr2-subheading`). En Sensores, «Linea IR» y «Ruedas» quedan como
  `<h2>` de su pestaña. La ficha de componente de Auto/Brazo/Cubo pasa de `<h3>`
  a `<h2>` (estaba directamente bajo el `<h1>`, como ya hacía ModelInspection).
- **Modelos 3D** deja de envolver la página en `GlassSurface`: la cabecera queda
  sobre el lienzo como en las demás páginas y solo el visor lleva superficie.
- **Control:** el párrafo «Modo AVR: … · GOTO: … · Salidas apagadas: … · Fallo: …»
  pasa a una fila de chips (Modo, GOTO, Salidas, Fallo) con cifras tabulares.
  Salidas desconocidas muestran «—». Un código de fallo distinto de 0 marca el
  chip en tono de error; el valor sigue siendo el que informa el robot.
- Estilos en `Web/src/styles/headers.css` (nuevo). Se borraron de workspace.css
  las reglas viejas de `.afr2-section-header` y `.afr2-page-title`.

## Decisiones tomadas y desvíos del plan

- **Tildes:** el plan propone recuperarlas, pero AFR2-20261026-03 registra un
  pedido humano de quitarlas de toda la Web porque se mostraban mal. No se
  reintrodujeron. Los textos de título y descripción se conservaron; los
  nuevos se escribieron sin tildes. Queda como decisión del usuario.
- **Descripciones:** la tabla del plan está escrita para las páginas de la fase 5
  (Conducir, Programas, Puesta a punto…) y en voseo con tildes. Se conservan las
  descripciones actuales hasta que existan esas páginas y se resuelva lo de las tildes.
- **Criterio «todos los tamaños de texto salen de `--type-*`»:** cumplido para
  títulos y cabeceras (`<h1>`, SectionHeader, bienvenida, insignias, chips).
  Quedan 269 declaraciones de tamaño en CSS y 121 clases de Tailwind de texto
  en el resto de la interfaz; se migran en la fase 5, que reescribe esos mismos
  24 CSS mobile-first.
- La barra superior no tiene encabezado propio; mostrar el título solo al
  desplazarse es de la fase 6 (tabla de movimiento).

## Adaptación por archivo; autorías anteriores conservadas

Web/src: `components/ui/PageHeader.tsx` (nuevo), `components/ui/SectionHeader.tsx`,
`components/ui/StartupWelcome.tsx`, `styles/headers.css` (nuevo), `styles/index.css`,
`styles/workspace.css`, `styles/model-inspection.css`,
`features/control/ControlPage.tsx`, `features/collector/CollectorPage.tsx`,
`features/sensors/SensorsPage.tsx`, `features/wifi/WifiPage.tsx`,
`features/oled/OledPage.tsx`, `features/model3d/Model3DPage.tsx`,
`features/model3d/AssetInspection.tsx`, `features/docs/DocsPage.tsx`,
`features/developer/DeveloperPage.tsx`, `features/developer/UiSimulatorPage.tsx`,
`features/protocolStudio/ProtocolStudioPage.tsx`. Contracts: decisions.md,
index.md, coverage-20261101-10.json, CHANGELOG.md, README.md; Web/README.md.

## Productores/consumidores/wire

Sin cambios. La fila de chips lee el mismo `robotState` que el párrafo anterior.

## Estado por repo

- **atmega / esp:** sin cambios.
- **web:** fuente modificada. Sin tests, compilación, typecheck, validadores,
  navegador, `build:esp` ni promoción. Comprobaciones por texto: un `<h1>` por
  página (`PageHeader` o bienvenida) y ningún import sin usar en `src`.
  Tamaños, cortes de línea y espaciado No verificados.
- **shared/Contracts:** registro actualizado.
- Sin commit local ni remoto.

## Pendiente

- Decidir si la Web recupera las tildes (revierte AFR2-20261026-03) o sigue sin
  ellas; de eso dependen los textos del plan.
- Migrar el resto de tamaños de texto a `--type-*` en la fase 5.
- Fases 5 a 7.
