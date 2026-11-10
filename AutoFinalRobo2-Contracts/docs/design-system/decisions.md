
## VIS-062 — Lenguaje de Infraestructura en la interfaz completa (02/11/2026)

Dirección Confirmada y autorización explícita de aplicación del manual 0.1.11 draft a toda la Web. Composición extendida Propuesta; ejecución No verificada. Ficha AFR2-20261102-15, cobertura de fuente coverage-20261102-15.json.

Se aplica desde primitivas y una capa CSS común: PageHeader con grilla/halos estáticos y caja de icono, tarjetas sólidas con profundidad, espaciado fluido, formas y estados compartidos, acentos por área y selectores existentes. Inicio conserva render y recibe título degradado. Las superficies particulares de sensores, simulación, calibración, modelos, documentación y estudio reciben las mismas familias de radios/bordes/material. Se conservan tamaños de escenas y lectura de píxeles OLED.

Las acciones primarias conservan action y los estados físicos su semántica. El acento info/success/warning del área es contexto, no dato recibido ni alarma. No se crean nuevas navegaciones ni etapas artificiales donde no hay un flujo secuencial. Se elige adaptación estática del banner en operación; no se trasladan los loops de Infraestructura ni se demora interacción.

Alternativa descartada: copiar el DOM del recorrido de Infraestructura en todas las páginas, porque alteraría organización y significado. Consecuencia: composición contextual con datos, controles, callbacks, condiciones y flujos conservados. Los tokens --visual-* viven en tokens.css y son consumidos por visual-language.css; no se modifica el inventario histórico de colores del manual. Las verificaciones sugeridas por la skill no se ejecutan por prohibición expresa del usuario: sin tests/build/validadores/navegador. Las observaciones y limitaciones de fuente siguen documentadas; no se afirma cierre visual.

## VIS-057 — Integración aditiva de Infraestructura (02/11/2026)

**Dirección Confirmada y Aprobada para incorporación documental:** el usuario pide integrar todo el desglose dentro del manual actual, reformando donde corresponda y conservando lo ya incorporado. Valores de receta Observados; generalizaciones Inferidas/Propuestas según alcance. Verificación No verificada. Ficha AFR2-20261102-14; versión 0.1.11 draft.

Se integran el atlas completo en manual.md, contratos en components.md y tokens bajo las capas normales con namespace infrastructure. El dossier de origen se conserva como evidencia, pero deja de ser una guía paralela requerida para conocer los patrones. Se añaden láminas SVG didácticas Propuestas, sin atribuirles captura/render de ejecución.

**Regla y razón:** dos regímenes de composición: operación con estado/acción/datos inmediatos y exploración con relato/mapa/etapas/magnitudes. Tipografía, espacio fluido, radios y longitudes de lectura se amplían con variantes narrativas; no se reemplazan globalmente PageHeader, SectionHeader o la escala VIS-051. Fuente: pedido actual, S01/S02/S12–S14 y extracción11.

**Alternativa no adoptada:** dejar solo enlaces al dossier o rehacer desde cero el manual. **Consecuencia:** todo el detalle queda integrado y se conserva la evolución anterior. Ámbito: documentación del sistema; sin implementar pantallas ni iniciar tests/validaciones.

## VIS-058 — Portadas por capas y color por ubicación

02/11/2026. Evidencia Observada; incorporación autorizada por VIS-057; valores y ejecución no equivalen a aprobación estética comprobada. Fuente S01/S02/S12.

Se amplía VIS-055: portada oscura fija, grilla de 32 px, halos radiales, glow independiente, texto degradado, chips translúcidos y carriles tintados. Se suma la familia pc/info, esp/warning, link/action, client/success con variantes de contenido claro/oscuro y banner fijo. Las seis densidades glass y el trabajo sólido VIS-015 continúan; la receta no tiñe todos los paneles ni redefine STOP.

**Precisión de precedencia:** la exclusión general de glow continuo de VIS-004/014/015 se limita a regiones operativas. Una portada explicativa puede usar la receta expresiva de la fuente; no hereda autorización para ocupar el fondo entero de una consola. La banda de 4 px de la ficha indica zona, no reintroduce la barra decorativa de marca de 5 px.

**Alternativa:** sustituir toda superficie por una portada dark/glass. No adoptada porque borraría los contratos instrumentales existentes. **Consecuencia:** material y acento se eligen por región/tarea, con el atlas como receta implementable y valores en sections.infrastructure.

## VIS-059 — Selección, flujo y detalle coordinados

02/11/2026. Evidencia Observada; dirección de incorporación Confirmada; ejecución No verificada. Fuente S01/S02/S05–S08 y S13.

Se desarrolla VIS-052 con la anatomía completa de dos filas, copia activa con clip-path, sombra en wrapper y comportamiento de resize/teclado. Se añaden nodos-button por zona, cables decorativos, línea de estaciones y ficha main/lateral con Entra/Sale y hechos numerados. La línea escala un degradado fijo 520 ms; done significa índice previo al seleccionado, no ejecución física ni historial de visitas.

**Límites:** conservar formularios, menús, overlays y OLED existentes; la muestra no es su sustituto. Los tamaños UiIcon fuera del tipo, hover done que puede ocultar un check y precedencia hover/active se registran como particularidades de fuente; no se adoptan como contrato deseable. Una tarjeta editorial con hover no se convierte por ello en botón.

**Alternativa:** copiar DOM/CSS local por cada página. No adoptada como norma; primero usar primitivas compartidas. **Consecuencia:** contratos nuevos en components.md, sin crear una librería ni modificar la Web en esta entrega.

## VIS-060 — Movimiento narrativo con límites de operación y accesibilidad

02/11/2026. Tiempos/mecanismos Observados en S02/S05/S06/S13; uso narrativo Inferido del patrón; incorporación autorizada por VIS-057. Ejecución No verificada.

Se amplía VIS-005/012: las entradas editoriales 600/900/1000 ms y loops de flujo/halo son una receta para Explorar, sin retener interacción. Se conserva la prohibición de retrasar operación o STOP; los tiempos de modales, tooltip, menú, foco y teclado no se reemplazan. El inventario registra disparador, propiedad, duración, curva, delay, repetición e interrupción; las fichas remontan por key y no esperan una animación exit.

**Norma preservada para futuras aplicaciones:** reduced motion sin loops ni transformaciones decorativas, magnitudes inmediatas y solo fade breve permitido; teclado inmediato. **Diferencia observada:** Infraestructura tiene reducción parcial (barras con tiempos literales, transformaciones de hover y delays restantes). Se documenta como deuda, sin rebajar el contrato accesible ni declarar que fue corregida.

**Alternativa:** trasladar loops a páginas operativas o considerar su presencia como actividad real. No adoptada. **Consecuencia:** motion.infrastructure distingue inventario de fuente y política de uso; sin corrección de implementación ni medición de rendimiento.

## VIS-061 — Magnitudes, procedencia y gobierno del manual ampliado

02/11/2026. Evidencia Observada en S01/S02/S04/S10/S11/S21; dirección de integración Confirmada; ejecución No verificada.

Se añaden barras por modelo, suma apilada, comparación raw/gzip, estadísticas, mini barras por archivo, tabla y anillo conic de almacenamiento. El anillo no tiene crecimiento temporal declarado. Conservar unidades, origen local/ESP, dato ausente explícito y progress accesible. Tamaños GLB, bytes de filesystem, tráfico y RAM no son intercambiables; los mínimos de píxeles son excepciones de visibilidad, no mediciones proporcionales exactas.

**Gobierno:** manual.md/components.md/tokens.json son la guía integrada; infraestructura/ conserva snapshot y decisiones de origen. Nuevos namespaces se suman sin sobrescribir tokens anteriores. La clasificación Propuesta/no cubierta de formularios, modales y menús se limita a esa referencia; los contratos generales existentes siguen vigentes. Validaciones históricas mantienen fecha/alcance y no certifican 0.1.11; audit.config conserva scanPaths vacío.

**Alternativa:** convertir ilustraciones/datos declarados en aprobación física, o copiar estados de error cuyo flujo no se acredita. No adoptada. **Consecuencia:** matrices documentales separan evidencia, cobertura, aprobación y verificación; sin nuevos comandos, API ni publicación.

### Correspondencia con la extracción de origen

| Origen conservado | Decisión del manual general |
| --- | --- |
| INF-VIS-001/004/005 | VIS-057, composición/tipo/espacio |
| INF-VIS-002/003 | VIS-058, zonas/material/portada |
| INF-VIS-006/007/008/010/011 | VIS-059, selector/flujo/ficha/acciones |
| INF-VIS-009 | VIS-060, movimiento y preferencias |
| INF-VIS-012/013/014 | VIS-061, datos/procedencia/gobernanza |

Particularidades INF-D01–D08 se conservan en el atlas y el dossier como deuda; esta correspondencia no las transforma en reglas aprobadas. Los componentes ausentes de Infraestructura se resuelven primero con los contratos ya presentes del manual general.

## VIS-055..VIS-056 — Modelos por banners y mapa de pantallas (02/11/2026)

Confirmada: pedidos humanos de separar simuladores y modelos 3D, entrar directo a la inspección por tres banners horizontales con renders, «Ir a simulación» abajo a la derecha, banner de Inicio y árbol de pantallas como mapa conceptual desde el dashboard. Manual 0.1.10 draft; ficha AFR2-20261102-01, [cobertura](coverage-20261102-01.json).

**VIS-055 — Banners de render.** Propuesta: los banners (Inicio y Modelos) son oscuros en ambos temas porque los renders tienen fondo azul noche; tokens `--banner-*` con tinta clara y acento rubí `#ff6b7d`. Escritorio: render a la derecha y degradado a la izquierda (2,4:1); celular: texto arriba y render abajo con fundido. Etiqueta «Render CAD» obligatoria. Mismo material e iconos que las tarjetas de Inicio; hover −2 px solo con puntero fino y pulsación `scale(.98)`. Una decisión por pantalla: portada → inspección → simulación, sin selectores apilados.

**VIS-056 — Mapa de pantallas.** Propuesta: árbol de izquierda a derecha desde el dashboard 010001, con nodos tarjeta (nombre y ScreenCode), conectores curvos, referencias repetidas punteadas y ficha lateral con tipo, gestos, vista previa OLED y camino. Abrir y cerrar ramas no anima la disposición; los nodos nuevos entran con un fundido de 120 ms.

Verificado en el navegador local sin robot (5 anchos, claro y oscuro); sin build ni hardware.

## VIS-050..VIS-054 — Plan visual 01-11, fase 1: decisiones y tokens (01/11/2026)

Confirmada: pedido humano de implementar la fase 1 del
[plan visual](../../../AutoFinalRobo2-Web/docs/plan-visual-20261101/PLAN.md),
con autorización expresa del manual en borrador y de las opciones por defecto
del plan. Modo APLICAR sobre manual0.1.10 draft. Ficha AFR2-20261101-07,
[cobertura](coverage-20261101-07.json). Ninguna decisión está verificada en
navegador: no se ejecutaron tests, builds, validadores, navegador ni `build:esp`.

### VIS-050 — Familia de iconos única

Propuesta / Aprobada por opción por defecto. Lucide (ISC) con los paths usados
copiados al repo en `components/ui/icons/registry.ts`, con crédito y licencia;
sin paquete nuevo en el bundle. Grilla 24 px, trazo 1,75 px, extremos y uniones
redondeados, color `currentColor`. Tamaños 16 (texto/insignias), 20 (botones,
barra lateral) y 24 px (tarjetas). Un concepto, un icono. Icono-botón con
`aria-label`; icono junto a texto con `aria-hidden`. STOP usa `octagon-x`, nunca
«■» (cumple el icono explícito de VIS-013). Iconos de dominio (rueda Mecanum,
servo, brazo, OLED) se dibujan propios en la misma grilla. Aplicación: fase 2.

Aplicada en AFR2-20261101-08 ([cobertura](coverage-20261101-08.json)): 58 iconos
en `registry.ts` (44 Lucide, 14 propios). Propuestas registradas: batería sin
lectura con raya en vez de contorno punteado; señal Wi-Fi con iconos de nivel en
vez de arcos atenuados; `touch` para Control táctil; `board` y `encoder` en las
fichas del modelo. Flechas y «×» dentro de texto siguen siendo texto.

### VIS-051 — Escala tipográfica y espaciado por rol

Propuesta / Aprobada por autorización del borrador. Ocho niveles con nombre:
`--type-display` 28–36 px/1,15/700/-0,03 em; `--type-title` 24–30 px/1,2/700/
-0,02 em (`<h1>` único por página); `--type-heading` 18/1,3/650/-0,01 em;
`--type-subheading` 15/1,35/600; `--type-body` 15/1,55/400 (inputs siguen en
16 px); `--type-label` 13/1,3/550/+0,01 em; `--type-caption` 12/1,4/450/+0,01 em;
`--type-data` 28/1/600/-0,02 em con cifras tabulares. Tracking por tamaño
(apple-design): negativo al crecer, levemente positivo en texto chico. Tamaños
en rem para respetar el texto del usuario.

Familia: vuelve a VIS-004 con Segoe UI Variable Text para texto y Display para
títulos, luego `system-ui`, sin descarga. Se retira Inter de `--font-sans`
porque estaba declarada y nunca se cargaba (Observada). En Windows con Segoe UI
Variable instalada el texto cambia de la fuente de sistema a esa familia; en otros
sistemas sigue `system-ui`.

Sustituye: «títulos 20–24 px» y «ayuda mínima 14 px» de VIS-004 cuando se
apliquen los niveles en fase 4. Riesgo registrado: `--type-caption` de 12 px
queda debajo del mínimo anterior de 14 px; usar solo para metadatos y unidades,
nunca para instrucciones ni avisos.

Espaciado por rol, mobile-first con un paso en 840 px: `--page-gutter` 16→32,
`--page-top` 20→32, `--section-gap` 24→40, `--card-padding` 16→24 (hoy 28 fijo),
`--stack-gap` 12→16 px. `--content-max` 1200 px limita solo texto y formularios;
escenas 3D, mapa y radar siguen a todo el ancho y `--container-max: none` de
VIS-042 (pedido humano) se conserva. Los `--workspace-*` actuales siguen en uso
hasta la fase 5.

Cabeceras aplicadas en AFR2-20261101-10 ([cobertura](coverage-20261101-10.json)):
`PageHeader` (`<h1>` único) y `SectionHeader` (`<h2>`/`<h3>`). Conflicto
registrado: el plan recupera tildes, pero AFR2-20261026-03 (pedido humano) las
quitó de la Web; se mantienen sin tildes hasta que el usuario decida.
**Resuelto en AFR2-20261101-11:** el usuario eligió recuperar las tildes; la
réplica OLED sigue sin ellas porque replica la fuente de píxeles del ATmega.

### VIS-052 — Selector segmentado con pill deslizante

Propuesta / Aprobada por opción por defecto. Un solo `SegmentedControl` (ranura
`--surface-subtle`, pill `--surface-solid` con `--glass-chrome-shadow`) para
pestañas y elecciones. Indicador por `clip-path` sobre una copia activa, 250 ms
(`--duration-segmented`) con `--ease-in-out`: es movimiento dentro de la
pantalla (animate). Sustituye `easeStandard` de §6.8 solo para este indicador.
Teclado y movimiento reducido: 0 ms. Sin transición en el primer render ni en
ajustes por `ResizeObserver`. Aplicación: fase 3.

Aplicada en AFR2-20261101-09 ([cobertura](coverage-20261101-09.json)). Propuestas
registradas: sombra y borde de la pill con `filter: drop-shadow()` (`clip-path`
recorta `box-shadow`); `activation="manual"` cuando elegir envía una orden al
robot; `reselect` para reenviar el modo; columnas iguales en celular con 2 a 4
opciones. Fuera: `.afr2-oled-side` (fase 7) y botones toggle con `aria-pressed`.

### VIS-053 — Breakpoints mobile-first y navegación en celular

Propuesta / Aprobada por opción por defecto. Cuatro cortes con `min-width`:
`sm` 600, `md` 840, `lg` 1200, `xl` 1600 px. Se publican como `--bp-*` en
`tokens.css` (referencia) y como `--breakpoint-*` en `@theme` de `index.css`, que
reemplaza los valores por defecto de Tailwind (640/768/1024/1280). Efecto ya
presente en fuente: 15 clases `sm:`/`md:`/`lg:` en 7 archivos cambian su punto
de activación (ver ficha). Las 32 consultas `max-width` de los CSS se migran en
fase 5; hasta entonces conviven ambos sistemas. Sustituye `mobileBreakpoint`
820 px (VIS-009) al migrar.

Navegación en celular: barra inferior con Inicio, Conducir, Programas,
Sensores y Más; «Más» abre hoja inferior. Rail desde `md`, barra expandida desde
`lg`. Sustituirá el rail móvil de 64 px de VIS-015 en fase 5.

Aplicada en AFR2-20261101-12 ([cobertura](coverage-20261101-12.json)). La
estructura (`workspace.css`) es mobile-first; las 17 hojas de componentes usan
los 4 cortes como complemento `max-width` hasta invertirlas con verificación
visual. «Más» es un modal; la hoja inferior con arrastre es de la fase 6.

### VIS-054 — Atención por tema y colores solo en tokens

Propuesta / Aprobada por autorización del borrador. `--attention`,
`--attention-ink`, `--attention-border`, `--attention-ring` y
`--attention-subtle` en ambos temas reemplazan el ámbar fijo de la pulsación
larga del encoder Web. No es error ni STOP. Claro conserva los valores
anteriores; oscuro usa `#fbbf24` con borde `#fcd34d` (Propuesta, contraste no
medido en pantalla).

Observada y corregida: `--dur-fast`, `--dur-press`, `--on-action`, `--accent`,
`--border` y `--glass-field` se usaban sin estar definidas; pasan a
`--duration-fast`, `--duration-button-press`, `--text-on-action`, `--action`
(o `--focus` en foco), `--border-default` y `--glass-field-bg`. La paleta del
mapa (`--map-*`) se traslada a `tokens.css` sin cambiar valores, y
ControlMapScene/ThemeProvider leen tokens en vez de repetir colores.
Excepciones registradas: réplica OLED de `OledCanvas128x64` (requiere skill
oled128-ui-design), luz/sombra neutras WebGL del radar y `theme-color` inicial
de `index.html`.

## VIS-048 - Modelos 3D con entorno compartido y fichas legibles

Confirmada: pedido humano de reutilizar cielo/piso de Simulaciones y corregir legibilidad en ambos temas. Observada: fondo OLED y fichas de assets sin superficie propia. Propuesta aplicada: InspectionEnvironment, tokens compartidos sin variar colores, fichas solidas con jerarquia comun y tinta carmesi profunda para referencias sobre escena. Alcance: Auto/Brazo/Cubo; materiales, escala y modelos conservados. Borrador0.1.9 autorizado para esta adaptacion. No se ejecutaron navegador, tests, builds ni validadores; ratios reales/aspecto No verificados. [Cobertura](coverage-20261101-03.json).

# Decisiones visuales — 23/10/2026

## VIS-047 — Laboratorio de pantallas UI (31/10/2026)

Confirmada: mejorar el Simulador UI existente. Observada: identidad0.1.9
autorizada, controles compartidos y modelo local01 con datos ficticios.
Propuesta aplicada: herramienta primero; barra de sesion, dispositivo junto
al encoder y guia contextual; opciones legibles con seleccion visible y
catalogo filtrable dentro de Disclosure. Movil conserva tres controles juntos.

Sin nuevas dependencias, colores aislados ni animacion decorativa. Los gestos
se mantienen en EncoderControl; el scroll es local e inmediato. Sin alterar
pixels, identidades ni navegacion del ATmega. Direccion Confirmada, composicion
Propuesta y aspecto/interaccion No verificados por instruccion de no ejecutar
comprobaciones. [Cobertura](coverage-20261031-12.json).

## VIS-046 — Nuevo logo del robot neón (31/10/2026)

Confirmada: PNG transparente proporcionado por el usuario como logo oficial.
Sustituye el recurso gráfico de VIS-016/017 y las rutas de formato posteriores.
Observada: BrandMark es compartido por navegación, Inicio y Desarrollador;
StartupWelcome comparte la ruta grande. Se mantiene el resto de la identidad.

Aplicado: original íntegro archivado y versiones WebP1024x768/256x192,
favicon64x64 y touch180x180. La reducción mantiene proporciones; los iconos
se centran sobre lienzo transparente. Los tamaños de contenedor44/80 y el
fallback AFR2 se conservan. Bienvenida reserva la proporción4:3 correcta.
Nombres nuevos para renovar caché y selección explícita en Vite para ambos
perfiles. No generación de imagen ni reinterpretación del emblema.

Fuente y activos públicos actualizados; aspecto/contraste en temas y dispositivos
No verificados. Sin tests, builds, validadores, navegador ni promociónESP.
[Cobertura](coverage-20261031-09.json).

## VIS-045 — Escena y controles de los simuladores (31/10/2026)

Confirmada: pedido humano de aplicar Emil, Apple Design y Visual Identity a
Auto/Brazo/Conjunto, fondo celeste y suelo crema, montaje conforme al CAD y eje
del servo corregido. Supporting Observada: identidad0.1.9 autorizada, Button,
TaskSections, Disclosure, CollapsiblePanel y controles compartidos. Los objetivos
no se elevan a fuente canónica. No hay nuevas referencias externas.

Propuesta implementada: encabezado contextual, escena amplia, cámara agrupada,
lecturas debajo y controles por tarea; matemática opcional y puntos/motores
avanzados inicialmente plegados. Sustituye la apertura inicial de VIS-043,
conserva el análisis a la derecha cuando se solicita. Fondo/suelo/retícula usan
tokens locales para ambos temas; los valores exactos son Propuesta, no cambian
la paleta de marca. Botones de44px, ayudas14px, foco vuelve al botón Matemática
al cerrar el panel y la parada también cubre movimiento articular. No se agrega
movimiento decorativo ni dependencias; primitivas conservan reduced motion.

Se actualiza la montura v17 con el marco CAD/GLB y se separa la salida del SG90
en Blender conservando materiales. Fuente implementada y activo público
actualizado; aspecto, animación y montaje en navegador **No verificados**.
Sin tests/build/validadores/flasheo por instrucción humana. Los paquetes
dist/artifacts/ESP conservan la revisión05. [Cobertura](coverage-20261031-06.json).

## VIS-044 — Enseñar y recuperar destinos del brazo (27/10/2026)

Confirmada: plan humano de puntos individuales persistentes, misma biblioteca en
Brazo y Conjunto, y conservacion de posicion/orientacion al cambiar dimensiones.
Pedido explicito de Visual Consistency y Apple Design. Observada: identidad0.1.9
autorizada y primitivas de tarjeta, campos, Button y Chevron compartidas.
Propuesta aplicada: tarjeta plegable inferior, captura/lista/ficha, acciones
proximas al destino, marcadores seleccionables, postura transparente y recorrido
continuo por segmentos. Cancelar permanece junto al visor si se pliega la tarjeta.
Texto y leyenda distinguen destino, modelo, calculo y bloqueo. Sin rebotes ni
inercia en las articulaciones; feedback inmediato con primitivas existentes y
teclado/reduced motion heredados. Sin nueva dependencia o blur anidado.
[Cobertura](coverage-20261027-11.json). Fuente implementada, aspecto/interaccion
No verificados; sin tests/build/validadores/navegador por esta entrega.

## VIS-043 — Ayuda contextual y matematica protagonista (27/10/2026)

Confirmada: pedido humano de explicar cada parametro a la derecha, destacar
la matriz y usar el mismo formulario matematico en las tres simulaciones.
Reutilizar el pad simulado en Control y alinear los giros con las columnas
extremas. Observada: identidad0.1.9, superficies/tokens propios, tarjeta05,
pad simulado y numeros compactos. Propuesta aplicada: ficha con significado,
rango, interpretacion y dependencias; columna matematica mayor, corchetes
visibles, bloques de rotacion/posicion y lecturas trigonometricas centradas.
SimulationMatrix/HomogeneousMatrix, MecanumDrivePad, HoldActionButton y
RangeNumberField estandarizan presentacion; cada adaptador conserva su dominio.
No se animan las cifras ni se agregan materiales, activos o dependencias.
[Cobertura](coverage-20261027-06.json). Direccion Confirmada, composicion
Propuesta y aspecto/interaccion No verificados. Sin tests/build/navegador.

Todas afectan el objetivo Web. Alternativa de partida: lenguaje instrumental cian de AutoMicro con paneles más sólidos; la especificación adopta el informe de transferencia para material/temas/movimiento y el activo propio `D:/ProyectoRobotica2/assets/Logo.png` para la identidad carmesí/rubí sobre vidrio neutro. Cerrar el plan visual documental habilita su implementación cuando se solicite, sin confirmar experiencia en ejecución.

| ID | Categoría y regla | Evidencia | Aprobación | Fuente/razón | Consecuencia |
|---|---|---|---|---|---|
| VIS-001 | Estado y operación primero | Inferida | Aprobada | Manual AutoMicro, adaptado a robot actual | Orden compartido por páginas |
| VIS-002 | Vidrio en paneles y densidad por tarea | Confirmada en dirección; Propuesta en escala | Aprobada | Informe §§2, 6, 15 y pedido humano | Evitar blur anidado; fallback sólido accesible |
| VIS-003 | Base de temas neutral en claro/oscuro, sin marcas de terceros | Confirmada en exclusión; Observada en fondos/textos | Aprobada; acento precisado por VIS-013 | Informe §§3, 4, 15 y pedido humano | Sustituye cian heredado; base neutra para el vidrio |
| VIS-004 | Radio 14/24/32, ritmo 4–48, texto 16 | Observada en radios/controles; Propuesta en escala | Aprobada | Informe §5, Mobile Native | Tokens comunes; lectura y tacto |
| VIS-005 | Movimiento causado, curvas del informe y teclado inmediato | Observada en tokens/matriz; Propuesta en política única | Aprobada | Informe §§6, 9, 10, 12 y Apple/Emil | Teclado inmediato, reduced motion de 120ms sin conflicto |
| VIS-006 | Sin auth/Studios, SW único | Confirmada | Aprobada | Pedidos humanos en este chat | Precede reglas genéricas de gestos |
| VIS-007 | Sonner con Toaster único y resultado real | Confirmada para librería; Propuesta para receta | Aprobada | Pedido humano, Informe §8.3 y ask-sonner | Sin spam, sin secretos, feedback local conservado |
| VIS-008 | Móvil, teclado y estados extremos | Propuesta | Aprobada | Informe §10, Mobile Native y manual AutoMicro | Verificación física pendiente |

## Validacion documental 0.1.1

Fecha 23/10/2026. Alcance: manual/tokens, no Web. Evidencia: cálculo matemático de los pares listados en `validation.json`, sin render. Historial `0.1.0` en `AFR2-20261023-06`; revisión `0.1.1` en `AFR2-20261023-07`.

| ID | Regla y alternativa | Evidencia | Aprobación | Razón y consecuencia |
|---|---|---|---|---|
| VIS-009 | Éxito claro `#087a46` y `borderStrong` `.55`/`.50`; alternativa `#0f8c53` (4,18:1) y borde `.28`/`.20` descartados para texto/controles | Propuesta | Aprobada | Superar 4,5:1 en texto y 3:1 en bordes de control sobre las superficies declaradas |
| VIS-010 | Sonner custom con cierre explícito (`toast.dismiss(id)`) y posición superior (`top-right`/`top-center`) | Propuesta | Aprobada | Evitar que headless pierda cierre y que el toast inferior derecho de origen tape `STOP` |
| VIS-011 | Semántica de links/tabs/menu y teclado separada de OLED | Propuesta | Aprobada | Eliminar ambigüedad Enter/Space y navegación; sin cambiar wire |

## Cotejo íntegro del informe, identidad carmesí/rubí y logo propio (`0.1.2` → `0.1.3`)

Fecha 23/10/2026. Con `D:/ProyectoRobotica2/informe-portabilidad-glassmorphism.md` completo (815 líneas, §§1–18) y `D:/ProyectoRobotica2/assets/Logo.png` aportado por el usuario, se cierra el plan visual en versión `0.1.3` (`AFR2-20261023-08`).

| ID | Regla y alternativa | Evidencia | Aprobación | Razón y consecuencia |
|---|---|---|---|---|
| VIS-012 | Reconciliación íntegra con §§6–15 del informe: curvas (`cubic-bezier(.23,1,.32,1)`, `(.2,.75,.25,1)`, `(.77,0,.175,1)`, `(.32,.72,0,1)`), tiempos (`150/200/300ms`, modal `250ms`, drawer `240ms`, segmentado `250ms`, acordeón `200ms`, tooltip `150/280ms`, spinner `800ms`), fade `120ms` en reduced motion, capas `z-index` `0..110`, `surfaceSubtle`, 3 alcances de carga y exclusión de barra de 5px y entradas editoriales largas | Observada / Propuesta | Aprobada | Fidelidad completa al informe de transferencia sin heredar el reset `.01ms !important` contradictorio ni demorar controles del robot |
| VIS-013 | Identidad gráfica propia desde `D:/ProyectoRobotica2/assets/Logo.png` y acento carmesí/rubí sobre vidrio neutro: `action` `#b81d33` (hover `#961426`, `onAction` `#ffffff`, `focus` `#b81d33`, `selection` `rgba(184,29,51,.10)`) en claro; `action` `#ff6b7d` (hover `#ff8594`, `onAction` `#0b1525`, `focus` `#ff6b7d`, `selection` `rgba(255,107,125,.14)`) en oscuro. Diferenciación de `STOP`/`error` con rojo bermellón (`#c93b1b` claro / `#ff8a65` oscuro), contenedor `surfaceSolid`, borde e icono explícito. Insignia `BrandMark` (`36×36 px` header / `64×64 px` Inicio) con `Logo.png` sobre medallón `#000000` y fallback monograma `AFR2` | Confirmada en dirección y logo; Propuesta en valores exactos | Aprobada | Incorpora el logo Mecanum rojo y dota de identidad propia a acciones/foco/marca sin teñir el vidrio neutro ni confundirse con `STOP` |

El plan visual queda **cerrado y autorizado como especificación `0.1.3`**. El estado global del artefacto `tokens.json` / `index.md` permanece `draft` hasta que la interfaz sea implementada y verificada visualmente en ejecución (`verification: "No verificada"`).

## Reorganizacion por tareas 0.1.4

VIS-014 — 23/10/2026. Fuente: pedido humano explícito de reformar toda la interfaz Web con Apple Design, menos densidad y secciones como AutoMicro. Dirección Confirmada y autorizada para implementación; valores concretos Propuesta, aprobación estética final Pendiente, verificación en ejecución No verificada.

Regla: navegación lateral en escritorio y horizontal en móvil; portada de seis destinos; una tarea por panel, detalles cerrados al inicio y separación de 32px. Control separa Movimiento/Prueba/GOTO/Calibración; Wi-Fi Estado/Buscar/Configurar; Sensores Entorno/Ruedas. Conexión/apariencia se revelan a pedido. Documentación abre solo un tema; OLED mantiene visor/SW como área principal y catálogo/datos/árbol en detalles. 3D prioriza escena y pliega procedencia. Borradores viven en la página, fuera del panel desmontado.

Material: conservar vidrio/temas/acento/logo, suprimir halos de fondo y sombras de panel/dense. Tipografía de títulos usa familia de cuerpo y peso 600. Valores de workspace en tokens.json y CSS compartido. Alternativa descartada: mostrar formularios/pad/GOTO y diagnóstico juntos en grids densos. Consecuencia: más navegación explícita, menor contenido simultáneo. No se agregan comandos ni autenticación.

Seguridad de navegación: cambiar tarea detiene hold manual antes de cambiar panel; salir de Control elimina su intervalo y solicita STOP si había hold. STOP permanece en la cabecera sticky en todo viewport. Toasts se desplazan bajo ella (112px escritorio/96px móvil) para no cubrir su acción. Estas son adaptaciones de la UI, no pruebas físicas de parada.

## VIS-015 — Sidebar y modales de ST, 0.1.5

Fecha 23/10/2026. Dirección Confirmada y autorizada por el pedido humano actual; patrón de sidebar y buscador Observada en ST2026-Radal 335d3ed; dimensiones/ilustraciones Propuesta con detalle estético Pendiente. Verificación en ejecución No verificada. Objetivo exclusivo AutoFinalRobo2-Web; fuente ST de lectura.

Reglas: rail lateral fijo/flotante y persistente, iconos al colapsar/nombres al expandir, OLED arriba como modal global y selección de ruta. Superficies operativas sólidas, acento propio y profundidad discreta; glass reservado a chrome, overlays y flotantes. Tarjetas con texto a izquierda e ilustración simbólica derecha. FloatingModal común adapta PublicCatalogSearch: diálogo nativo, material translate/clip, 240/200ms easeInOut, backdrop neutro 24px/180ms, contenido 100ms/80ms; teclado inmediato y reduced fade 120ms. La curva y tiempos observados se conservan; origen X/Y y panel centrado son extensión Propuesta.

Sustituye VIS-014 solo en navegación móvil horizontal, ancho fijo 228px y sombras de tarjetas accionables; sustituye VIS-012 solo en receta modal 250ms/scale .96. La geometría de sidebar cambia inmediatamente (la fuente ST anima width/padding); se evita animar layout. Modales conservan contenido hasta acabar salida, foco nativo, Escape y retorno, fondo inert y cierre con puntero reversible. STOP dentro de cada modal y cabecera sticky; Sonner queda debajo de la cabecera medida con ResizeObserver. Polling OLED no genera toasts ni simula resultados.

Consecuencia: identidad propia, sin colores comerciales ST, login, permisos ni dependencias nuevas. Estado real/ausencia se conserva; ui.action no predice navegación. La implementación no valida funcionamiento de firmware, fluidez o contraste en pantalla. Fuentes y hashes en provenance/web-floating-ui.json.

## VIS-016 — Introducción ST, marca libre y toast inferiores, 0.1.6

Fecha 23/10/2026. Dirección Confirmada y autorizada por el usuario. Secuencia Observada en ST2026-Radal 335d3ed (`startup-welcome.tsx/css`, montaje `src/app/App.tsx`). Logo propio Confirmado; derivado/tamaños44/80px Propuesta, revisión estética Pendiente. Verificación Web No verificada.

Reglas: BrandMark compartido sin medallón, borde, fondo ni sombra añadida. El PNG original es RGB con fondo negro opaco, por lo que se genera un derivado RGBA sin fondo mediante image_gen. Se preserva el original íntegro en assets/Logo.png y Web/reference/brand/Logo-original.png; no se promete identidad exacta de píxeles de un derivado generativo. El favicon usa el mismo derivado; el PNG opaco deja public para evitar empaquetar dos versiones.

StartupWelcome se monta una vez por carga completa, fuera de rutas: entrada300ms scale(.97→1), pausa600ms, salida de logo200ms y velo300ms con desfase de media salida (total1300ms tras imagen lista). Propósito: reconocimiento de marca en entrada poco frecuente. CSS opacity/transform con easeEnter existente; sin dependencias nuevas. Escape/Omitir inmediatos, dialog nativo, foco contenido/retornado y STOP accesible. Error de imagen libera; fallback4s; reduced motion fade120 sin pausa/escala. No representa progreso ni espera hardware.

Sonner único queda explícitamente en bottom-right para escritorio y móvil. Offset24px en escritorio/16px móvil con safe areas; margen izquierdo móvil respeta el rail. Se elimina ResizeObserver y --afr2-toast-top, que solo servían a su posición anterior. STOP permanece arriba y los resultados críticos dentro de modales siguen inline. No se duplican Toaster ni animaciones externas.

Sustituye VIS-013 solo en forma/material de marca; VIS-012 solo en exclusión del inicio solicitado; VIS-010/014/015 solo en ubicación/offset de notificaciones. No cambia API, firmware ni tiempos de los modales operativos. Fuente adaptada sin compilar/renderizar/probar.

## VIS-017 — Velo fiel a Radal, toast legibles y mapa

23/10/2026. Confirmada: pedido humano de quitar la sensación de card del inicio, arreglar recortes e incorporar mapa por vistas. Observada: Radal usa velo completo con halo/sheen y logo libre; Sonner2.0.8 limita el ancho móvil con un cálculo simétrico y el título anterior usaba truncate. Propuesta: composición y chasis esquemático del mapa; estética final Pendiente. Verificación de ejecución No verificada.

Reglas: restaurar mezcla28%/halo/sheen, logo min(76vw,32rem), superficie nativa sin borde/radio/sombra de panel. Logo decodificado antes de iniciar secuencia; Escape/Omitir y STOP como controles discretos sin tarjeta. Conservar300/600/200/300ms, reduced fade120 y fallback4s.

Toast: portal en document.body, contenedor con ancho explícito min(400px,viewport-márgenes), item width100% y offsets coherentes. Títulos/descripciones pueden envolver palabras largas; sin truncate. Una notificación visible por vez, expandida, cierre aparte del texto; contenido excepcionalmente largo con scroll propio. Esto sustituye la pila de3 y margen izquierdo móvil de rail que Sonner descontaba dos veces. Sigue bottom-right; sin animador externo.

Mapa: superficie sólida/grilla, botones de cámara y zoom accesibles. Superior SVG disponible sin WebGL; Perspectiva/Seguir auto Three.js lazy/frameloop demand con fallback. Sin animación de pose por decoración ni integración de consignas. Datos válidos recibidos únicamente, huecos de trazo segmentados, edad y unknown explícitos. Chasis de referencia no implica CAD validado. Fuente/valores en tokens y contrato de mapa; firmware sin cambios.


## VIS-018 — Avisos cerrables, controles OLED siempre visibles, giro gráfico y secuencias, 0.1.8

23/10/2026. Confirmada: pedido humano de implementar en Web avisos cerrables en cabecera y modal, controles OLED siempre visibles con estado largo retenido hasta soltar, controles gráficos de giro sobre el eje y editor/simulador de secuencias Mecanum con cliente físico. Observada: disposición e iconografía de `ScreenStreamWorkspace` y bloques/líneas de tiempo de `TimelineRow`/`MotorBlock` en la base de referencia `AutoMicro-Web`. Propuesta: adaptación a superficies sólidas `--surface-solid`, acento carmesí/rubí `--action` y cinemática Mecanum de cuatro ruedas sin dependencias de PIN, balance ni tracción diferencial. Verificación en ejecución No verificada.
## VIS-019 — Control coherente, Sistema y Desarrollador,0.1.9

24/10/2026. Confirmada: dirección del pedido humano y autorización de aplicar el
borrador propio. Observada: títulos transformados por selector CSS amplio,
geometría aproximada de cajas, propiedades laterales y datos de desarrollador
en ST Radal. Propuesta: ritmo24px/20px, detalles bajo la pista y nueva composición
de Sistema/Desarrollador; estética final Pendiente. Compilación Web completada;
verificación visual No verificada: el navegador denegó localhost, sin elusión.

Sustituye VIS-018 en propiedades lateral/modal y selector Simulación/Robot.
Sustituye persistencia incondicional de STOP de VIS-014/015 por disponibilidad
durante operación o parada incierta, manteniendo la misma acción accesible en
diálogo cuando el documento queda inert. Conserva paleta/tokens de identidad,
logo WebP, superficies sólidas y navegación OLED. Chevron SVG común gira180°;
texto estable,180ms easeOut, teclado y reduced motion sin transición.

No se incorpora Radal como autoridad de color/material, no se instala librería
nueva ni se modifica CAD. Fuentes/hashes en provenance/control-system-sequence-20261024.json.
Cobertura: Control/editor/disclosures/selects/settings/Inicio/Desarrollador/Docs.
El cálculo preciso del suelo conserva metros y el GLB. No certifica cotas físicas.


## VIS-020 — GOTO local y recorrido ordenado

24/10/2026, AFR2-20261024-10. Pedido humano y aclaración de disposición autorizan
mapa ancho completo y X/Y/rumbo/tiempo en fila, lista inferior y controles a la
derecha (móvil apila). Acento solo al comando actual, texto de estado adicional;
scroll acompaña automáticamente y se libera al tomarlo manualmente. Estado
local de simulación separado del envío físico y sin feedback inventado.

Skill animate: State indication/Spatial consistency durante avance automático,
fade con capas opacity140ms ease y FLIP transform220ms easeInOut, tokens vigentes.
Interrupción conserva posición; reduced motion/teclado sin transform y fade120ms
suave con reduced motion. Sin animar alto/margen/color ni añadir Motion. Reusa
paleta carmesí/rubí, superficies/primitivas,44px y ritmo24/20px. Layout/dirección
aprobados por pedido; ajuste visual/tacto No verificados por denegación previa
del navegador. No actualiza identidad de marca ni afirma validación estética.

## VIS-021 — Hardware agrupado y simulador compacto (12)

Dirección Confirmada por los pedidos humanos de esta tarea; composición/valores
Propuesta, basada en tokens/primitivas Observados del borrador0.1.9 ya autorizado.
Verificación visual No verificada. Alcance Web de Control/GOTO y ajustes.

Información de hardware contiene cuatro bloques verticales (conexión, ESP,
ATmega, arranques); fecha/hora y Actualizar en una fila interior. Reinicio es
el otro desplegable principal. Apariencia usa botones, sin selector de control.
En GOTO, mapa y fila actual comparten columna; X/Y/rumbo/tiempo se apilan en
barra derecha230px, junto al reproductor. En ancho<620px se apila esa barra.
Mapa220..300px según alto; lista cerrada160px, fila72px con medias vecinas,
sin scrollbar. Recorrido local y detalles secundarios se pliegan para reservar
espacio al mapa y al movimiento. No se afirma que todo quepa en toda pantalla.

Pista seleccionada usa acento/borde de tokens actuales; botones FL/FR/RL/RR
separados, papelera44px junto a Editable con margen8px por lado. Eliminación
usa FloatingModal existente, animación interrumpible/teclado/reduced motion.
Carrusel conserva FLIP220ms y fade140ms; abrir gira solo el chevron180ms,
sin transform con reduced motion. No dependencias, colores ni marca nuevos.
Compilación completada; revisión visual/interactiva sigue pendiente, sin
reintentar la superficie de navegador que previamente denegó localhost.

## VIS-022 — Control táctil inmersivo (13)

Dirección Confirmada por el pedido humano; composición Propuesta y tokens
Observados del borrador0.1.9 autorizado. Objetivos: Movimiento, nueva superficie
de gesto y variante fullscreen del dialog compartido. Auto final centrado grande,
con encuadre de cámara; sombra y elevación fija, sin flotación continua ni pose
física inventada. Flechas visibles solo durante input, acento al sector elegido.
X izquierda, título centrado, dirección/velocidad debajo. Botón junto al título
Conducción manual, alineado verticalmente, encima de parámetros/modo.

Entrada ocasional: Spatial consistency; revelado del material240ms/salida200ms
existentes. Opacity de flechas140ms easeOut. Gesto funcional frecuente: respuesta
inmediata, sin spring/bob/inercia que continúe una consigna al soltar. Teclado
inmediato/reduced motion sin desplazamiento. Safe areas/dvh y touch-action:none
solo en control; global zoom preservado. Rotar/resize cancela. STOP contextual
dentro del dialog nativo por accesibilidad del resto inert. No nueva paleta,
tipografía, dependencia o activo raster. Cobertura fuente, visual No verificada.

## VIS-023 — Perfil y biblioteca OLED en árbol (14)

Dirección Confirmada por pedido humano; composición Propuesta con tokens propios
Observados del borrador0.1.9 autorizado. Perfil: nombre/copy sin márgenes heredados,
ritmo12/24px, logo/copy centrados y enlaces44px; Volver al inicio con flecha antes.
Hardware/Reinicio solo en Ajustes. Árbol por padres, avisos en grupo contextual,
selección accent y contenido local separado del framebuffer conectado. Se reusa
la estructura1..3/flechas SVG de AutoMicro ScreenStreamWorkspace, adaptada a
giros AFR2 con captura, sin portar sus comandos ni controles USER/Menú/Atrás.
Chevrón de rama cambia orientación; reduced motion/teclado sin transición.
Layout apila árbol/biblioteca en ancho reducido y conserva botones44px laterales.
No nuevos colores/recursos/dependencias; visual/interoperabilidad No verificadas.

## VIS-024 — Diagnóstico y configuración conectados (15)

Pedido humano de alineación del maestro. Dirección Confirmada; composición
Propuesta sobre borrador0.1.9 autorizado. Se reutiliza Hardware/Disclosure/dl,
sin nueva paleta ni controles de control. LittleFS/paquete muestra valores reales
o desconocidos; hash declarado identificado. Calibración conserva sus botones,
solicita lease al escribir e informa consulta necesaria. No aceptación visual,
interacción móvil, tests o datos simulados. Cobertura15 separa fuente de runtime.
## VIS-025 — Control directo sin autorización previa (25/01)

Dirección Confirmada por pedido humano explícito. VIS-024 no obliga a adquirir
control: esa conducta se retira en Web/ESP/AVR. Se eliminan opciones y estados
de acceso; acciones directas con supervisión automática. OLED/C/TS conserva
59identidades, Volver y recursos; control4/sistema7. Sin nueva paleta, layout
gráfico o dependencia. Manual0.1.9 conservado, composición Propuesta, visual
No verificada; compilación y aceptación se separan.

## VIS-026 — Táctil local, superior y ruedas (25/02)

Dirección Confirmada por pedido humano; composición Propuesta sobre manual0.1.9
autorizado. Aplicación en ManualDriveOverlay/Model, RobotModel y mapas locales.
Se reutilizan UiIcon arrow, superficies sólidas y tokens de acento/borde/texto;
se reserva una franja para modo y se conserva ritmo8/12/16/24 y controles44px.
Frente arriba, texto inferior inmediato y acento por dirección; aparición por
opacity sin animar layout. Ruedas son simulación funcional, sin inercia al soltar
ni pose física inventada. reduced motion elimina giro/fade secundario; teclado
conserva respuesta inmediata. Sin nueva marca/paleta/dependencia. Fuente compilada,
visual/teléfono No verificados; no sustituir esa aceptación por lectura o build.

## VIS-027 — Captura y feedback de conducción (25/03)

Pedido humano Confirmado; dirección sobre manual0.1.9 autorizado, composición
Propuesta y visual No verificada. Objetivo Overlay/Model/FloatingModal/CSS;
fuentes canónicas siguen UiIcon/tokens/manual actuales. Foco inicial optativo
y superficie táctil pasiva para Canvas resuelven conflictos de interacción
en fuente. Solo dirección activa56px, radio pill y acento existentes; opacity/
scale en140ms y oscilación±3px/800ms para indicar hold. Pequeña respuesta CSS
del modelo8px, sin trayectoria física. Teclado inmediato y reduced motion
sin oscilación/desplazamiento. Sin nuevos colores/activos/dependencias; sin
afirmar prueba visual o aceptación móvil. Mantener soltado/STOP inmediato.

## VIS-028 — Estela direccional de degradé (25/05)

Dirección Confirmada por el pedido humano. Manual0.1.9 autorizado; composición
Propuesta, visual No verificada. Fuentes: tokens de acento del tema y reglas
de movimiento; objetivo Overlay/CSS de conducción, sin cambiar marca ni datos.
Tres trazos sin superficie/borde, degradé transparente→acento, entrada/salida
opacity/scale140ms ease-out y ciclo800ms linear de±6px con fases cola→punta.
Pausar conserva frame al salir; teclado/reduced motion estáticos. El ciclo
indica hold continuo, no un éxito físico ni una medición. Paleta/dependencias
y captura intactas; CSS/SVG sin filtros y solo tres trazos activos. Revisión
visual en ambos temas/anchos pendiente, no cubierta por la compilación.

## VIS-029 — Chevrones bajo el chasis (25/06)

Dirección Confirmada por pedido humano; bajo el auto se interpreta como capa
trasera al modelo y raíz oculta por el chasis, rotada hacia cada dirección.
Manual0.1.9 autorizado, composición Propuesta, visual No verificada. Mayor
cuerpo, origen central y dimensiones relativas al menor lado del viewport
local. Degradé longitudinal de acento y máscara de opacidad transversal;
sin colores visibles nuevos, filtros, raster ni dependencias. Fade220ms
ease-out/ciclo1200ms linear con pausa de frame, teclado y reduced motion
estáticos. Datos/captura/modelo y órdenes físicas conservados. Ver ambos
temas/anchos y oclusión del Canvas queda pendiente, no acreditado por build.

## VIS-030 — Cintas anchas y punta intensa (25/07)

Dirección Confirmada por pedido humano. Composición Propuesta sobre manual0.1.9
autorizado, visual No verificada. Fuentes: tokens de acento/movimiento y
profundidad actuales; objetivo SVG/CSS/cámara manual. Banda más gruesa y mayor
extensión, encuadre52% para dar aire. Punta de acento pleno, cola transparente
que muestra el fondo de cada tema. Máscara de extremos con cuerpo opaco;
sin nueva paleta/filtros/recursos. Fade220ms/flujo1200ms, teclado y reducción
de movimiento estáticos. Solo cámara local, sin alterar escala CAD o mapas.
Revisión visual de puntas/oclusiones/ambos temas pendiente; build no la acredita.

## VIS-031 — Difusión por brazo y punta aguda (25/08)

Dirección Confirmada por el nuevo pedido humano sustituye la punta de color
pleno07. Composición Propuesta sobre manual0.1.9 autorizado, visual No verificada.
Mismo acento con opacidad máxima0,58 y fondo real por transparencia. Cada brazo
usa gradiente normal simétrico y longitudinal alineado, reflejados en la unión;
se retira la difusión global. V más aguda/proporción conservada, sin estirar
el dibujo. Capa bajo chasis, cámara52% y límites de control intactos. SVG/CSS
propios sin filtros/dependencias ni colores nuevos. Fade220ms/flujo1200ms,
teclado y reduced motion estáticos. Contraste/oclusión/unión/ambos temas y
anchos siguen pendientes de revisión visual; compilación no los acredita.

## VIS-032 — Preferencia por diseño anterior (25/09)

El usuario revoca la composición08 y prefiere07 conservando solo color suave.
Dirección Confirmada, composición Propuesta sobre manual0.1.9 autorizado,
visual No verificada. Se restaura íntegro el gráfico/posición/animación07;
opacidadSVG0,58 como único ajuste visual. Se retira componente08 propio tras
comprobar que no tenía cambios posteriores. Hashes de fuente respaldan la
restauración, no una aceptación visual. Cámara/captura/modo local intactos.

## VIS-033 — Modelo manual mayor (25/10)

Pedido humano Confirmado. Solo encuadre52%→60% en ambos ejes (≈15,4% lineal),
centrado/vista superior iguales; chevrones09 intactos. Composición Propuesta
con manual0.1.9 autorizado, visual No verificada. Sin nueva paleta, recursos,
animación o controles. Malla/unidades y cámaras de otras vistas intactas.

## VIS-034 — Manipulación local con dos dedos (25/10/2026)

Pedido humano Confirmado para simular giro sobre su propio eje. Composición
Propuesta sobre manual0.1.9 autorizado, visual/gesto No verificados. Seguimiento
directo del ángulo de dedos, sin inercia; vista/cámara no orbitan. Q/E15° y0
alternativa. Modo local y rumbo en chapa existente, sentido en estado inferior;
número sin anuncios por frame. Chevrones09/cámara60%10 conservados. Un dedo
sigue pantalla después de girar, ruedas convertidas a marco del cuerpo.
Captura dual, cancelar y drenar al perder un contacto; no yaw físico. Máscara/
paleta/animación de chevrones sin cambio; teclado/reduced motion conservados.
Apple-design/mobile-native/animate aplicadas como manipulación funcional,
no inercia de liberación por ser control explícito. Teléfono/lectores/temas
y transiciones de contacto por comprobar, no acreditados por build.



## VIS-035 — Sensores, radar y red (25/10/2026)

Confirmada por pedido humano: usar borrador0.1.9, disposición GOTO con mediciones/ángulo al costado, tres tareas separadas, modal propio de Red y nombre Configuración. Radar semicircular Three.js con escala física y PWM aplicado; Modelo3D se conserva estático. IR original/digital/ADC no se etiqueta como distancia.

Propuesta: anillos4, escalas0.5/1/2/4m, iconos propios coherentes y umbrales RSSI -55/-67/-75dBm. Amarillo AP y verde STA consumen warning/success existentes con texto/icono, no solo color. Origen de la sesión por IP del socket, sin atribuir RSSI STA al cliente AP. Superficies de trabajo sólidas, chrome/modal neutro; acento carmesí/rubí propio.

Movimiento: presión inmediata, modal común con foco/escape/retorno; interpolación críticamente amortiguada desde ángulo visual/velocidad actual. En reduced motion se actualiza el dato sin transición; nunca se congela el ángulo real ni se integran muestras visuales. Geometrías del grid memoizadas y recursos dispuestos al desmontar. Las nuevas lecturas no producen toasts por muestra.

Cinco reglas protectoras: (1) fuente autoritativa y edades propias, (2) misma composición GOTO/primitivas, (3) paleta y semántica existentes, (4) navegación OLED y copia Web vinculadas, (5) motion accesible sin datos fabricados. Sin validación visual, browser ni ensayos; cobertura de fuente no acredita contraste, paridad o montaje.

Revisión de barrido independiente: cada inicio cambia scanRevision para descartar mapas anteriores aunque no se haya observado la parada. La vista fija puede superponer la última muestra válida con ángulo/PWM conocido sobre el histórico; los puntos fuera de escala se cuentan. Ningún punto se genera desde la interpolación visual.

## VIS-036 — Puntos y fichas del modelo 3D (25/10/2026)

Dirección Confirmada por pedido humano: puntos fijados a piezas, ficha explicativa
local que conserva su posición al girar el auto y acceso a la sección relacionada.
Continúa aplicación del borrador0.1.9 autorizado, sin nueva fuente de marca.
Propuesta: numeración01–11, selectores44px separados con líneas, ficha superior
derecha/escritorio e inferior/móvil, desplegable alternativo, X/Escape y cierre
exterior que distingue clic de arrastre. Referencias de geometría Inferidas;
ESP y OLED/encoder requieren coordenadas locales del exportado conjunto actual.

Usa popover neutro compartido y tokens de acento, superficie, radio, tipografía
y sombra, con iconos propios. Seguimiento geométrico inmediato sin interpolar
selectores; solo feedback corto en botones, sin transición en reduced motion.
La ficha no es modal: permite seguir orbitando y hacer zoom. Scroll propio,
foco en cierre y retorno al selector/desplegable. La navegación abre tareas y
rueda elegida, sin disparar acciones físicas. OLED conserva SW único y Volver.

Cinco reglas protectoras: material/token común; una ficha por vez; puntos unidos
al CAD y panel independiente; teclado/tacto/movimiento reducido; explicaciones
con datos del proyecto y acciones físicas solo en sus controles. Composición
Propuesta e interacción/visual No verificadas; sin navegador, tests, build ni
validadores. Cobertura: coverage-20261025-15.json. Fuente nueva separada del
build14 anterior. Revisión visual de referencias, temas y gestos queda pendiente.


## VIS-037 — Radar volumétrico y controles vinculados (25/10/2026)

Confirmada: pedido humano de un radar tecnológico/neón con líneas 3D, rango
doble0–180°, barra y campo numérico para velocidad/estabilización/intervalo,
confirmación Enter/blur y límites coherentes. Configuracion sin acento por pedido
expreso. Se conserva el manual0.1.9 autorizado y la excepción Modelo3D.

Observada: disposición mapa/lecturas laterales de Control/GOTO, primitivas de
modal/Disclosure, paleta neutra con acento carmesí/rubí y límites del contrato8.
Propuesta: proyección oblicua, plataforma con espesor, anillos tubulares/halo
moderado, contorno elevado y hitos 3D. La altura del hito no representa altura
medida. No se agrega otra marca cromática ni un barrido ficticio.

Manipulación1:1, sin easing del rango/sector. Microfeedback de borde/foco140ms;
aguja conserva interpolación amortiguada del dato aplicado y reduced motion.
Canvas bajo demanda; geometrías/texturas liberadas. La vista previa se distingue
de la configuración aceptada, bloqueando iniciar hasta aplicar los cambios.
No toasts por muestra ni nuevos comandos/pantallas OLED.

Cinco reglas protectoras: escala y procedencia real, composición GOTO, tokens
existentes, estado de aplicación explícito y movimiento accesible funcional.
No verificada: presentación en temas/tamaños, táctil/teclado/lectores, rendimiento
WebGL y contraste final; sin ejecución de herramientas de comprobación.


## VIS-042 — Ancho util y tarjetas integradas (27/10/2026)

Confirmada: pedido humano de quitar margenes amplios, unificar simulaciones en
una tarjeta, adosar/plegar matematica y mover controles debajo del renderizador.
Numeros de slider compactos; botones de motor distinguibles; Control agrupado
en Movimiento y Pruebas/calibracion, manteniendo GOTO y funciones existentes.
Observada: primitivas y Chevron0.1.9, con transition transform180ms ease-out;
teclado/reduced motion inmediatos. Propuesta: relleno12…24 px, matriz320 px o
rail52 px, divisores internos y controles distribuidos en dos columnas.
Se retira el limite1120 px (contentMax=none) por direccion humana confirmada.
[Cobertura](coverage-20261027-05.json). Fuente sin tests/build/validaciones ni
observacion visual; aspecto y respuesta No verificados.

## VIS-041 — Simulaciones y Modelos3D (27/10/2026)

Confirmada: pedido humano de unificar tres simulaciones, mantener Control aparte,
modelos elegibles mediante pills, gripper independiente y controles mas ordenados.
Observada: TaskSections, campos de radar con titulo/numero superiores y barra
inferior, tokens y superficies0.1.9 autorizados. Propuesta: controles compactos
derechos, matriz persistente debajo, motores en pills con nombre central.
Articulaciones seleccionables y arrastrables con ambos botones del mouse.
El selector del visor conserva herramientas; brazo final/referencias y burbuja
definitiva esperan los insumos del usuario. [Cobertura](coverage-20261027-04.json).
Fuente implementada, aspecto/interaccion No verificados, sin tests/build/navegador.

## VIS-040 — Control intacto, brazo y conjunto separados (27/10/2026)

Confirmada: correccion humana que exige conservar Control/control tactil y
agregar solamente Brazo robotico y Simulacion conjunta. Controles a la derecha,
articulaciones seleccionables y gripper, posiciones de cubos y toma segun alcance.
Observada: tokens0.1.9 autorizados, Button, TaskSections y disposiciones laterales
existentes. Propuesta: escena izquierda, controles derechos con eje seleccionado,
selector de tareas del conjunto, proyeccion del espacio de toma y cubos verdes
alcanzables/ocre fuera de alcance; en movil se apilan los controles debajo.
Sustituye rutas y disposicion VIS-039. Aspecto No verificado, sin ejecucion de
tests/build/validadores/navegador. [Cobertura](coverage-20261027-02.json).

## VIS-039 — Robot recolector y panel del brazo (27/10/2026)

Confirmada: pedido humano de simulador3D, referencias reversibles, auto
seleccionable/movil, motores adelante/atras y panel del brazo separado del robot.
Observada: identidad0.1.9 previamente autorizada, tokens, Button/Disclosure,
SectionHeader, navegacion y escenas propias. Propuesta: escena ancha, vistas
Superior/Lateral/Perspectiva, datos inferiores y controles por panel, con estado
compartido en memoria. Rig del brazo y deposito nominales; no CAD final.
[Cobertura](coverage-20261027-01.json). Direccion Confirmada, composicion
Propuesta y aspecto No verificado. Sin tests/build/validaciones/navegador.

## VIS-038 — Composición del radar (26/10/2026)

Confirmada: el usuario rechaza el resultado visual anterior y pide rehacer el
radar con Emil y skills visuales, coherente con el producto. Se conserva la
identidad0.1.9 autorizada. No se usa el radar rechazado como referencia canónica.
Observada: tokens de Control/GOTO, Button/Disclosure y jerarquías del manual.
Propuesta: campo continuo con retícula que desvanece, anillos/sector rubí y
encuadre de límites proyectados; distancia dominante y detalles plegables,
parámetros debajo del mapa. Inicio1m y dos vistas; no se agranda el modelo.

El shader de suelo no usa tiempo. Marcas en una sola geometría, sin bloom ni
paquetes adicionales. Cámara/alcance y rangos responden inmediatamente;
servo/aguja conservan interpolación funcional y reduced motion. Sin animar
lecturas numéricas, sin ecos de muestra y sin atribuir al halo un haz calibrado.

Cinco reglas: (1) tokens propios Control/GOTO, (2) jerarquía dato/control/detalle,
(3) escala y procedencia reales, (4) controles accesibles y datos desconocidos
explícitos, (5) movimiento ligado al dato. VIS-037 se conserva para los controles,
pero se sustituye su plataforma/composición. Diseño Propuesto; temas/móvil,
contraste, teclado y rendimiento No verificados. Sin tests/build/validadores.

## VIS-045 — Protocol Studio desde Desarrollador (28/10/2026)

Confirmada: solicitud humana de copiar y traducir capturas HEX como AutoMicro.
Observada: constructor/traductor histórico, manual 0.1.9 previamente autorizado,
TaskSections, Field, Button, SectionHeader, notify y tokens AFR2 existentes.
Propuesta: acceso por tarjeta y ruta propia, dos tareas, conversión en columnas,
scanner plegable paginado y constructor con salida adosada. Bajo 700 px las
conversiones se apilan; el constructor se apila bajo 1050 px. No se redefine
paleta, fuentes, marca ni movimiento. El código fuente conserva los tokens
vigentes. No se ejecutaron herramientas de búsqueda de deriva.

Fuentes de identidad: manual y primitivas propias; AutoMicro solo aporta flujo
instrumental. Cinco reglas: tokens en ambos temas, superficie sólida de trabajo,
una tarea visible, foco/etiquetas/estados explícitos y datos con procedencia real.
Cobertura parcial de fuente en coverage-20261028-09.json. Aspecto, contraste,
teclado, copia y móvil No verificados; sin tests/build/validadores/navegador.

## VIS-046 — Esencia de AutoMicro y consistencia de Protocol Studio (28/10/2026)

Confirmada: pedido humano de conservar funcionalidad, ideas y esencia de
Protocol Studio, con libertad para alinearlo a la interfaz actual. Continúa
APLICAR sobre borrador 0.1.9 previamente autorizado para Desarrollador/primitivas.
Observada en fuente AutoMicro: constructor con parámetros y override manual,
campos desplegables, cuatro salidas, traductor, mapa seleccionable y stream.

Propuesta: columna de edición 290–360 px y área flexible de estructura/salidas,
mapa con selección explícita y análisis único. Se conserva la pestaña inicial
Constructor de la referencia. Superficies y componentes compartidos AFR2;
acentos de campos toman action/info/border, sin importar paleta cian/violeta.
Las etiquetas/offsets/separaciones comunican la función además del color.
Bajo 1050 px el constructor se apila; bajo 700 px, salidas y conversiones.
No hay nueva animación; los detalles nativos y tabs mantienen teclado/foco.

Cinco reglas protectoras: tokens de ambos temas; superficies sólidas de trabajo;
una tarea visible; estado y etiquetas accesibles; bytes y procedencia reales.
Matriz funcional en docs/PROTOCOL_STUDIO_AUTOMICRO.md desde Contracts;
cobertura coverage-20261028-10.json. Fuente revisada manualmente; interacción,
contraste, temas y móvil No verificados. Sin tests/build/validadores/navegador.


## AFR2-20261110-02 — Canvas de navegación con cuadrícula

Dirección Confirmada por pedido explícito: usar el piso final de Armado como referencia del fondo general de navegación. Fuente directa: Web/src/features/assembly/AssemblyCollectionEnvironment.tsx. Objetivo: html/body de styles/index.css; soporte: tokens.css, canvas y text-muted de los temas existentes. Aplicación autorizada de este ajuste visual; no sustituye las reglas de superficies del manual.

Adaptación Propuesta implementada: cuatro gradientes CSS, pasos --canvas-grid-step=48px y --canvas-grid-major-step=240px, tintas --canvas-grid-line/--canvas-grid-major-line derivadas de text-muted al 12/20 %. Apariencia estática, sin eventos ni medios adicionales; las superficies operativas conservan su material. Cobertura de fuente global; ambos temas y tamaños sin revisión visual. El piso 3D permanece propio de las escenas. Ficha: changes/2026-11-10-02-navigation-grid-background.md desde Contracts.


## AFR2-20261110-04 — Corrección de la superficie canónica

Confirmada por aclaración del usuario: el fondo solicitado era para los simuladores 3D. La decisión de -02 sobre canvas de navegación queda Sustituida; se retira su implementación y los tokens canvas-grid. Objetivo corregido: components/three/SimulationViewport.tsx y styles/simulation-viewport.css, con tokens sim-scene. Referencia: piso y fondo de la presentación Armado. Líneas y plano con las mismas opacidades, cuadrícula física de 0,05 m y fondo de gradientes; las páginas generales recuperan canvas-gradient. Fuente implementada; ejecución visual No verificada. Ficha changes/2026-11-10-04-simulator-grid-background-correction.md.
