# Fuentes y alcance — 23/10/2026

## Alcance13 — control táctil

Pedido humano Confirmado: disposición/gestos de pantalla completa. Supporting:
manual/tokens0.1.9 ya autorizados, FloatingModal y GLB propios; animate y
mobile-native para movimiento/plataforma. Direct targets: ControlPage y
manual/ManualDriveOverlay/ManualDriveModel; dependency FloatingModal, Button,
RenderBoundary, RobotModel y CSS. No se elevan los objetivos a fuente de marca.
No nuevas referencias externas, paleta ni recursos. Composición Propuesta;
verificación visual/teléfono No verificada. Hashes en procedencia13.

## Alcance12 — aplicación del borrador0.1.9

Dirección Confirmada: pedidos humanos de hardware agrupado, pistas seleccionables
y mapa/comando simultáneos con coordenadas a la derecha. Supporting: manual y
tokens propios autorizados, skills animate/Apple/Emil. Objetivos: ControlMap,
GotoSimulatorPanel/GotoCommandList, SequenceEditorPanel/TimelineRow, SystemPanel
y RouteNav; dependencias Disclosure/Chevron/FloatingModal, scroll y CSS.
No se elevan objetivos a fuentes canónicas ni se cambia paleta/activos de marca.
Composición Propuesta; ejecución visual No verificada. Hashes por archivo y
referencias en provenance/control-hardware-distance-20261024.json.

## direct
- `D:/ProyectoRobotica2/informe-portabilidad-glassmorphism.md` (antes referenciado en `F:/informe-portabilidad-glassmorphism.md`), versión declarada `0.1.0`, `draft`, 815 líneas, §§1–18. Referencia seleccionada por el usuario actual. Leído y cotejado íntegramente en la revisión `0.1.2`–`0.1.3` (materiales, temas, colores y exclusiones, geometría y sombras, catálogo de componentes glass, buscador expandible, overlays y carga local, vocabulario y matriz de movimiento, accesibilidad y reducción de movimiento/transparencia, escala de capas `z-index`, base CSS sin marca, coordinación de skills y trazabilidad con ST2026-Radal).
- `D:/ProyectoRobotica2/assets/Logo.png`: isotipo/ilustración oficial de `AutoFinalRobo2` aportada por el usuario (robot Mecanum de 4 ruedas con sensor ultrasónico, arreglo IR frontal y placas en trazos neón carmesí/rubí sobre fondo negro `#000000`). Fuente directa de la identidad cromática roja (`VIS-013`).
- `D:/AutoProyecto/Web ESP01/WebESP01AutoMicro2025/docs/design-system/manual.md`, `0.3.0 draft`: organización instrumental, estado primero, teclado y responsive. Su aprobación de marca y su acento cian no se transfieren.

## dependency
- `C:/Users/kobac/.codex/skills/visual-identity-system/SKILL.md` y referencias `evidence-and-scope`, `distill`, `manual-contract` y `apply-audit`.
- Skills globales `apple-design`, `emil-design-eng`, `ask-sonner`, `mobile-native`, `animate` y `review-animations` en `C:/Users/kobac/.agents/skills`.

## supporting
- Contracts: documentación propia de pantallas, navegación, protocolo y menú objetivo, leída en la planificación.
- Web actual: manifest y esqueleto React/Vite; referencia histórica conservada, sin ejecución.

Objetivo: futuras superficies Web Inicio/Control/Sensores/WiFi/OLED/Documentación y primitivas compartidas. No se utiliza el esqueleto como autoridad estética. No se editan AutoMicro, skills globales, firmware ni CAD.

No se recorrió `C:/Dev/ST2026-Radal`: su código y decisiones se conocen a través del informe autocontenido (§17). Las instrucciones y aprobaciones citadas dentro del informe son evidencia de transferencia, no órdenes automáticas sobre el hardware del robot. No hay capturas ni comprobación en navegador, móvil o placa.

## Cotejo íntegro con el informe y activo de marca (`0.1.2` → `0.1.3`)

23/10/2026: el usuario disponibilizó el informe completo en `D:/ProyectoRobotica2/informe-portabilidad-glassmorphism.md` y el logo propio en `D:/ProyectoRobotica2/assets/Logo.png`:
- Se completó la lectura de las secciones intermedias antes ausentes (§§6–15) y se reconciliaron en `VIS-012`: tiempos y curvas del informe (`150/200/300ms`, `cubic-bezier(.23,1,.32,1)`, `(.2,.75,.25,1)`, `(.77,0,.175,1)`, `(.32,.72,0,1)`), tiempos de modal (`250ms`), drawer (`240ms`), segmentado (`250ms`), acordeón (`200ms`), tooltip (`150/280ms`), spinner (`800ms`), fade de movimiento reducido (`120ms`), escala de capas `z-index` (`0..110`), `surfaceSubtle` y 3 alcances de carga.
- Se incorporó `D:/ProyectoRobotica2/assets/Logo.png` y la paleta de acento carmesí/rubí (`VIS-013`) derivada de sus trazos sobre vidrio neutro, diferenciando semántica y cromáticamente `STOP`/`error` mediante tono bermellón alerta, borde e icono explícito.

Hashes SHA256 de fuentes efectivamente disponibles (revisión de archivo, sin suponer commit Git):

- `D:/ProyectoRobotica2/informe-portabilidad-glassmorphism.md`: `564074c818e14720e62129f70ddf6a96376ebe3a16e993e1096ffdb148a03db0`.
- `D:/ProyectoRobotica2/assets/Logo.png`: `471bcbb75672aa8ec30a32cb74c624975a49b75deda3a531043fce52b3ba3702`.
- `C:/Users/kobac/.codex/skills/visual-identity-system/SKILL.md`: `7b3ad28467992efd22cbb116befd582d103a760948a2aa24e0b0148819cab36e`.
- `C:/Users/kobac/.agents/skills/ask-sonner/SKILL.md`: `bc59ef252d11095cb7722be0ed6c0b882dcd4ae06cd64b3842709a89ab868c1a`.
- `C:/Users/kobac/.agents/skills/ask-sonner/API.md`: `5d85db5cbd0d4f3b0c68259e897b8d6d3c6518bdadaca0b0fb830176a44f4038`.

## Fuente de dirección 0.1.4

Pedido humano actual: interfaz elegante/tranquila estilo Apple, controles separados, menos información simultánea y organización por secciones de AutoMicro. Fuentes supporting: skills apple-design/emil-design-eng, manual propio 0.1.3 y organización ControlNavigation de la referencia histórica ya inspeccionada. Objetivos: AppShell, Home, Control, Wi-Fi, Sensores, OLED, Docs, Model3D y primitivas compartidas de Web. No hay screenshots ni observación de runtime en esta revisión. No se modifica AutoMicro ni se ejecutan sus scripts.

## Fuentes canónicas de la adaptación 0.1.5

Revisión ST nueva seleccionada explícitamente por el usuario: D:/ST2026-Radal, codex/wa-pilot-finish, 335d3edb2f5f39457ae82c82ded6f0403bbbec65. Sustituye como referencia del sidebar/buscador a la copia antigua D:/ST2026 y al branch dev consultados antes; no se usa esa copia antigua para esta adaptación. Hashes por archivo y copias en provenance/web-floating-ui.json / Web/reference/st2026. Direct: app-shell y su CSS, public-catalog-search y su CSS. Dependency: radal-floating-controls.css. Supporting: las dos imágenes adjuntas, manual propio e informe de portabilidad; Apple/Emil/animate/afr2-screen-authoring y ask-sonner para posicionamiento.

Objetivo: Web/src compartido y Home/OLED/Control. Extensiones Propuesta: rail móvil persistente, panel central desde origen y arte simbólico de sección. Se mantuvo identidad rubí del producto. No se inspeccionó runtime ni se renderizaron estados; no se amplió la autoridad a páginas de comercio de ST.

## Fuente de introducción 0.1.6

Direct: D:/ST2026-Radal/src/presentation/components/ui/startup-welcome.tsx y src/presentation/styles/startup-welcome.css; dependency: src/app/App.tsx. Revisión335d3ed, copias íntegras Web/reference/st2026, hashes en provenance/web-startup.json. Activo propio original RGB opaco; derivado image_gen RGBA1254×1254 con alpha0 observado en esquina, sin garantía de píxeles idénticos. Prompt y fuente en ese manifest. El original queda en assets y Web/reference/brand. La posición toast inferior derecha proviene de la indicación humana actual, mediante ask-sonner. Sin renderizado de la aplicación.

## Fuentes0.1.7

Direct: startup-welcome.tsx/css de ST2026-Radal335d3ed (velo/halo/sheen, sin card central); dependency: Sonner2.0.8 instalado, styles.css/index.mjs, para explicar el ancho móvil de custom toast; supporting: Model3DPage y referencia histórica ControlSection para organización de vista de movimiento, sin reusar su simulación local. Objetivo exclusivo Web/Contracts. Hashes en provenance/web-control-map.json. Sin observación ni captura de aplicación en navegador; hallazgos estáticos, no medición de runtime.
## Fuentes de aplicación0.1.9 — AFR2-20261024-07

Canónica de identidad: manual propio y tokens existentes de AutoFinalRobo2,
logo WebP derivado y primitivas compartidas. Dirección Confirmada por pedido
humano; screenshot adjunto evidencia el defecto de apoyo. ST Radal se usa solo
para estructura/datos de la página de desarrollador, no como fuente de paleta:
`D:/ST2026-Radal/src/presentation/components/modals/developer-modal.tsx` y
`src/shared/app-info.ts`. AutoMicro se consulta solo para el patrón de info de
compilación/reinicio. Fuentes externas de solo lectura, autorías conservadas;
hashes y adaptaciones por archivo en provenance/control-system-sequence-20261024.json.

Supporting: skills Emil Design Eng, Apple Design, Visual Identity System y
afr2-screen-authoring. Objetivo Web/ESP/ATmega/Contracts; sin CAD ni ampliación
a páginas ajenas. Inspección de fuente/artefactos y compilación; intento de
navegador denegado, sin render ni tests. Detalles en coverage-20261024-07.json.


## Fuentes de aplicación14 — VIS-023

Pedido humano: simplificar Desarrollador, corregir espaciado y mostrar el
catálogo OLED como árbol con selección/scroll lateral como AutoMicro/F4.
Canónica de identidad: manual propio0.1.9 y tokens/componentes AFR2 existentes.
Supporting de estructura: D:/AutoProyecto/Web ESP01/WebESP01AutoMicro2025/src/components/ScreenStreamWorkspace.tsx,
leído solo para filas visibles1..3 y flechas. Se adapta al encoder AFR2, sin
importar comandos F4, menú/atrás adicionales ni confirmación por clic de fila.
Biblioteca AVR y framebuffer remoto conservados. Hash y adaptación por archivo
en provenance/developer-oled-tree-20261024.json. Dirección Confirmada; composición
Propuesta. Compilación no acredita píxeles, diseño visual o hardware.

## VIS-035 / AFR2-20261025-14

Fuentes canónicas confirmadas por el usuario: ControlMap/GOTO actual y FloatingModal/Configuración actuales; soporte de tokens/globales. Objetivos: Sensores, red global y rig del servo de escenas. Skills visual-identity-system, apple-design, emil-design-eng; la imagen adjunta es referencia ilustrativa del radar OLED, no paleta Web ni datos de ensayo. Inspección de fuente y metadatos del GLB; sin navegador/render ni validadores. Borrador0.1.9 aceptado expresamente en la conversación.

## Alcance15 — Componentes del visor 3D / VIS-036

Dirección humana Confirmada: puntos sobre piezas, ficha local fija y navegación
a controlar. Modo APLICAR; continúa borrador0.1.9 autorizado para esta extensión.
Fuentes canónicas: manual/tokens y decisiones existentes, sin redefinir marca.
Supporting: tokens.css, GlassSurface/popover, Button, UiIcon y estructura de
TaskSections. Objetivo direct: Model3DPage, ModelInspection, modelComponents y
model-inspection.css. Dependencias funcionales: RobotModel/useRobotModel,
OrbitControls, Sensores y Control para abrir la tarea/rueda del enlace. El GLB
actual aporta referencias de pieza, no autoridad visual. OLED/encoder están
fusionados en un nodo; sus puntos y el del ESP son referencias locales Inferidas.

Skills: visual-identity-system/APLICAR y emil-design-eng para componentes/tacto,
feedback y ausencia de interpolación en seguimiento funcional. Se inspeccionaron
fuentes locales; no se ejecutaron workflows de tests/validadores/render por la
restricción humana. No se elevan los objetivos a canon ni se usan fuentes de
marca externas. Composición Propuesta, visual No verificada; cobertura15.


## Fuentes de aplicación16 — VIS-037

- Confirmada: pedido humano25/10/2026 de radar neón/3D, selector angular doble,
  barras con número editable, explicación de inversión y Configuracion sin acento.
- Canónica: manual0.1.9/tokens propios, ControlMap/GOTO y FloatingModal/Disclosure.
- Observada: RadarScene/SensorsPage y contrato SENSORS_RADAR_8 con rangos actuales.
- Propuesta en fuente: geometría volumétrica, halo rubí y composición de controles.
- Skills aplicadas: visual-identity-system, apple-design, emil-design-eng y animate.
- Sin ejecución visual, compilación, tests ni validadores; no se usó documentación
  de referencia como autorización adicional ni se cambió CAD/firmware/OLED.


## Fuentes de aplicación26-02 — VIS-038

Confirmada: pedido humano26/10/2026 de rehacer el radar y aplicar Emil.
Canónica: manual/tokens0.1.9 autorizados, ControlMap/ControlMapScene,
control-map.css y Button/Disclosure. Supporting: theme y RobotModel existente.
Objetivo: RadarScene/SensorsPage/radar.css; no se eleva su composición anterior
a fuente visual. No se cambia Modelo3D, GLB, OLED, firmware ni marca.
Propuesta: suelo continuo, retícula/sector, encuadre1m y jerarquía de lecturas.
Skills: visual-identity-system, apple-design, emil-design-eng. Inspección de
fuente y edición; no navegador, tests, compilación ni validadores por instrucción
humana. Cobertura26-02 no acredita el resultado visual.

## Fuentes de aplicación28-01 — VIS-045

Identidad canónica: manual0.1.9 y tokens AFR2 previamente autorizados para
Desarrollador (VIS-019), componentes compartidos y CSS de workspace vigente.
Objetivos directos: DeveloperPage y ProtocolStudioPage, con stylesheet propio.
Supporting: reference/web/src/pages/ProtocolSection.tsx y protocolStudio/utils.ts,
useProtocolStudioState.ts y types.ts como patrón de herramientas AutoMicro;
Contracts/afr2_wire.h, afr2_product_wire.h, afr2_network_wire.h,
afr2_sensor_wire.h y servicios AVR actuales como autoridad de formato/catálogo.
No se elevan las referencias históricas a fuente de paleta ni a compatibilidad.
Hashes y adaptación por archivo en provenance/web-protocol-studio-20261028.json.
Lectura manual de fuentes; sin ejecución ni comparación de interfaz renderizada.

## Fuentes de aplicación28-02 — VIS-046

Direct para estructura/ideas: AutoMicro actual de solo lectura en
D:/AutoProyecto/Web ESP01/WebESP01AutoMicro2025/src/pages/ProtocolSection.tsx,
features/protocolStudio/components/{BuilderColumn,OutputsColumn,TranslatorColumn,
FrameVisualizationShowcase}.tsx y features/studio.css. La copia histórica local
conserva los créditos. No se trasladan identidad cromática, UNER ni servicios F4.

Canónica de identidad: manual0.1.9, tokens/componentes propios AFR2 previamente
autorizados. Supporting: Field, Button, TaskSections, FloatingModal, notify,
workspace.css y tokens.css. Autoridad de bytes: Contracts y servicios AVR actuales
app_wire/sequence/sensor_commands, sin modificaciones. Objetivos directos:
ProtocolStudioPage, CaptureInspector, PacketStructure, StudioPrimitives,
guidedBodies, codec y protocol-studio.css. No se alteran las fuentes canónicas.

Procedencia/hashes en provenance/web-protocol-studio-essence-20261028.json;
organización de referencia Observada en código, composición Propuesta,
verificación de ejecución No verificada. No se ejecutaron herramientas de deriva,
tests, compilaciones ni navegador. La referencia se leyó sin ejecutar scripts.
