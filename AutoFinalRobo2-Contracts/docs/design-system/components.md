# Componentes y cobertura

<a id="infraestructura-vigente"></a>
## Vigencia 0.1.11 — catálogo ampliado, no sustituido

Dirección Confirmada por el usuario (AFR2-20261102-14). Se suman portada por capas, mapa de zonas, conectores, navegación por estaciones, ficha de transformación, gráficos de magnitudes, anillo y glosario a los botones, campos, overlays, notificaciones, navegación, escenarios y OLED existentes. Los contratos completos están [integrados más abajo](#infraestructura-integrada), no remitidos únicamente al dossier externo.

| Familia existente | Ampliación / precisión vigente | Contrato que se conserva |
| --- | --- | --- |
| Tarjeta/panel | Receta narrativa solid de radio 24 px, subgrupos 18 px y acento zonal; banda semántica de 4 px solo en ficha. | Trabajo sólido; glass en chrome/overlays; sin blur por celda. |
| Tabs/segmentado | Copia activa recortada, 250 ms ease-in-out, ranura sutil y pill propia por tema. | Semántica choice/tablist, roving focus y teclado inmediato de VIS-052. |
| Botón/enlace | Ejemplos de secundario/primario, press .97 y flecha de enlace +3 px; sin inventar hover cromático ausente. | Disabled, loading, foco, nombre y acción real. |
| Tabla/badge | Tabla sólida con mini barras y badge zonal con procedencia. | Scope/caption, unidades, scroll y estados reales. |
| Loader/estado | Reveal editorial separado de loading; anillo FS es dato, no loader. | Spinner solo en trabajo real, aria-busy y recuperación. |
| Formularios/modales/menús/toast/OLED | La referencia no los redefine. | Mantener sus contratos y gestos ya documentados. |

Las matrices antiguas conservan su versión. Las fichas de Infraestructura distinguen **cobertura en la referencia** de cobertura general: un formulario no observado allí sigue existiendo en este catálogo. Particularidades de hover, tamaño de iconos y reduced motion quedan como deuda, no como especificación deseable. Esta revisión es documental, No verificada en ejecución; sin tests ni validaciones.


VIS-047: laboratorio UI con dispositivo/encoder, guia contextual, sesion y
catalogo filtrable. Reutiliza controles y tokens0.1.9; scroll inmediato local,
campos etiquetados y acciones de44px. Los pixels OLED y gestos se conservan.
Composicion Propuesta; aspecto/interaccion No verificados. [Cobertura](coverage-20261031-12.json).

VIS-046: BrandMark y bienvenida usan el nuevo robot neón oficial. El original
se conserva íntegro y los derivados mantienen proporción/transparencia.
Favicon y touch-icon comparten el emblema. Sustituye solo las referencias al
recurso gráfico de las filas históricas siguientes. Fuente aplicada; aspecto
en ejecución No verificado. [Cobertura](coverage-20261031-09.json).

VIS-045: simuladores con escenario celeste/crema, encabezado/estado/cámara,
datos y controles inferiores; matemática opcional con retorno de foco. Puntos
guardados y controles por rueda se abren bajo demanda. Se conservan primitivas
compartidas, reglas de reduced motion y acciones de44px. Dirección Confirmada,
composición/colores Propuesta y aspecto **No verificado**; sin tests/build ni
validadores. [Cobertura31-06](coverage-20261031-06.json).

Contratos cerrados a nivel documental (`0.1.3`) para ambos temas. Cada control conserva geometría en `default/hover/focus/active/disabled/loading/error/success`. Ninguno fue renderizado todavía en la nueva interfaz.

| Componente | Anatomía y variantes | Teclado, movimiento y límites | Evidencia | Cobertura | Verificación | Aprobación | Decisión |
|---|---|---|---|---|---|---|---|
| `BrandMark` (Logo + wordmark) | medallón `#000000` (`36×36 px` header / `64×64 px` Inicio) con `Logo.png` (`object-fit: contain`), borde `border` y reflejo `highlight`, más texto `AutoFinalRobo2`; fallback monograma `AFR2` en carmesí si la imagen no está disponible | Enlace a Inicio (`#/`) con `Enter`, foco visible `2px` carmesí/rubí y dimensiones fijas (`CLS = 0`) | Confirmada / Propuesta | Cubierta en especificación | No verificada | Aprobada | VIS-013 |
| Botón/enlace | label, icono opcional, foco; variantes `primary` (carmesí `#b81d33` / rubí `#ff6b7d`), `secondary` (`glassField`/`glassPanel`), `danger (STOP)` (bermellón `#c93b1b` / `#ff8a65` sobre `surfaceSolid` con borde e icono explícito) | `Enter` en enlaces; `Enter/Space` en botones; presión `scale(.97)` `150ms` `easeOut` (`0ms` en teclado/reduced motion); `STOP` siempre visible en contexto activo y nunca confundible con `primary` | Observada / Propuesta | Cubierta en especificación | No verificada | Aprobada | VIS-004/005/008/011/012/013 |
| Campo/select | label persistente, control `glassField`, ayuda y error; mínimo `44px`, texto `≥16px` | `<select>` nativo sin atribuir animación glass al popup del SO (§6.2); listbox `glassPopover` solo si se extiende con primitiva accesible | Observada / Propuesta | Cubierta en especificación | No verificada | Aprobada | VIS-004/005/008/012 |
| Tarjeta/panel | `glassPanel` o `glassDense`, título, contexto, contenido y acciones; sin barra superior de 5 px (§3.4) | Tarjeta estática no se eleva; tarjeta accionable hasta `2px` con `hover:hover` y `pointer:fine` (§6.1) | Confirmada / Propuesta | Cubierta en especificación | No verificada | Aprobada | VIS-002/004/012 |
| Tabs/segmentado | Vigente VIS-052/059: ranura `surface-subtle`, copia activa recortada con texto/icono, pill clara o `#2a4068` oscura; variante choice o tabs | `clip-path` 250ms `easeInOut`; label press .97/150ms con puntero; primer render/resize/teclado/reduced motion inmediatos. La receta histórica de indicador carmesí/easeStandard se conserva en decisions.md como antecedente, no como estado actual | Observada / Propuesta | Cubierta en especificación | No verificada | Aprobada | VIS-005/011/012/013 |
| Tabla/acordeón/badge | contenedor `glassDense`, cabeceras, valores con unidad/edad y grupos colapsables | Sin blur por celda (§2.2); acordeón `200ms` (`grid-template-rows: 0fr -> 1fr`, chevron `180deg`), contenido cerrado `inert` (§6.9) | Observada / Propuesta | Cubierta en especificación | No verificada | Aprobada | VIS-002/005/008/012 |
| Modal/drawer/menú | modal `glassDialog` + scrim; drawer móvil `glassDense` + scrim; menú `glassPopover` | Modal `250ms` `easeEnter` (`translateY(12px) scale(.96)`), `max-height: min(90svh,900px)`; drawer `240ms` `easeDrawer` (`translateX(-100%)`), ancho `min(75vw,360px)`; menú `200/150ms` `easeOut` (`translateY(-4px) scale(.97)`); desmontaje diferido, Escape y retorno de foco (§§6.3, 6.5, 6.6) | Observada / Propuesta | Cubierta en especificación | No verificada | Aprobada | VIS-005/008/011/012 |
| Tooltip | `glassDense`/`glassPopover`, radio `12–14px`, ayuda breve asociada | `150ms` (`scale(.97) -> 1`), delay hover `280ms`, foco inmediato, Escape cierra, offset `10px`, margen `8px`; sin acciones esenciales (§6.4) | Observada / Propuesta | Cubierta en especificación | No verificada | Aprobada | VIS-005/008/012 |
| Alerta/toast | `glassPopover`, mensaje, estado, botón `Cerrar` y acción opcional | Sonner único en raíz (`top-right`/`top-center`, `zToast: 100`); cierre explícito `toast.dismiss(id)` en custom; live regions sin duplicados | Confirmada / Propuesta | Cubierta en especificación | No verificada | Aprobada | VIS-007/010/012 |
| Loader/overlays/vacío | 3 alcances: spinner de botón, overlay regional `glass` (`blur 12px`) y estado vacío/error | Spinner `800ms` lineal (estático en reduced motion); controles cubiertos `disabled`/`inert` y región `aria-busy` (§8.1, §8.2); sin simular avance ni bloquear `STOP` | Observada / Propuesta | Cubierta en especificación | No verificada | Aprobada | VIS-005/008/012 |
| OLED | canvas `128x64` en región propia y encoder virtual único | Giro + único SW (corto confirma, largo `800ms` menú excluyente), `Volver` por ítem, acción remota confirmada | Confirmada | Cubierta en especificación | No verificada | Aprobada | VIS-006 |

## Recetas por página y patrón
- **Inicio:** `BrandMark` con `Logo.png` (`64×64 px`), estado de enlaces, accesos y modelo de prueba; sin login ni bienvenida acrílica de pantalla completa que demore la operación (§9.4).
- **Control:** requisitos/dueño, acción/`STOP`, parámetros y resultado. El botón `STOP` se distingue de las acciones primarias carmesí por usar bermellón (`error`), fondo `surfaceSolid`, borde fuerte e icono de parada explícito, además de quedar libre de toasts gracias a la posición superior (`VIS-010`, `VIS-013`).
- **Sensores/tabla:** validez, unidades y edad antes de ayudas en contenedor `glassDense`; datos desconocidos (`—`) sin ceros inventados y filas sin blur individual.
- **WiFi/formulario:** estado STA/AP, tarea elegida, campos `glassField`, aplicar (`primary` carmesí/rubí) y resultado. Carga de escaneo/opciones mediante overlay local sobre la región del formulario (§8.1). Password efímera, sin autocompletar desde fuente histórica.
- **OLED/detalle:** snapshot identificado, visor reconstruido y un SW; catálogo cuatro IDs propios, árbol ampliado pendiente.
- **Documentación:** navegación por temas, títulos y lectura de ancho acotado; no requiere placa. Si en el futuro se incorpora buscador expandible (§7), usar `<dialog>` o panel `glassPopover` con apertura `240ms` / cierre `200ms` `easeInOut` y teclado instantáneo.
- **Modal y Drawer:** tarea única, ayuda próxima al campo, acciones al final; scroll propio, foco contenido y conservación del montaje durante la salida (`250ms` modal, `240ms` drawer).
- **Vacío/error:** motivo concreto y acción de recuperar; preservar contexto sin presentar éxito.

## Contrato común preparado para implementación

Variantes `Button`: `primary` (fondo `action` `#b81d33`/`#ff6b7d`, texto `onAction` `#ffffff`/`#0b1525`), `secondary` (`glassField`/`glassPanel` con texto `text`), `danger` (parada `STOP` y acciones destructivas: texto e icono `error` `#c93b1b`/`#ff8a65` y borde `error` sobre `surfaceSolid`, nunca igual al botón `primary`); tamaño regular `≥44px`. `Disabled` conserva opacidad `1`, contraste, nombre y explicación próxima. `Loading` reserva espacio para evitar saltos. Enlaces mantienen semántica `<a>`; no simular enlaces con `<div>`. Campos con `<label>`, `aria-describedby` y `aria-invalid`; `borderStrong` delimita el control y `focus` aplica anillo carmesí/rubí de `2px` con separador `2px` `surfaceSolid`.

Primitivas previstas: `ThemeProvider` (preferencia `afr2-theme`, suscripción central a `matchMedia` y clase `.dark`), `BrandMark` (copia/empaquetado de `D:/ProyectoRobotica2/assets/Logo.png` en `public/` o `src/assets/` con fallback `AFR2`), `GlassSurface` (`chrome/panel/dense/field/popover/dialog`), `Button`, `Field`, `StatusNotice`, `SectionHeader`, `RouteNav`, `NotificationShell` y `EncoderControl`. Cada una consume `tokens.json`. `GlassSurface` no aplica blur nuevo si ya está dentro de un material; en interior comparte fondo o usa `surfaceSubtle` (`#eef3f8` / `#1c2f4f`) y bordes. Escala `z-index` unificada (`0, 20, 40, 50, 60, 80, 90, 100, 110`, §11). Escena 3D y canvas OLED encapsulados en su propia región, sin texto glass superpuesto sobre WebGL/canvas.

## Lista de estados por validar en la interfaz futura

Ambos temas (`light`/`dark`) y escritorio/móvil: reposo, hover fino, foco de teclado carmesí/rubí, puntero pulsado (`150ms`), teclado instantáneo (`0ms`), `prefers-reduced-motion` (`120ms` fade), `prefers-reduced-transparency` / `prefers-contrast: more` (`surfaceSolid`), deshabilitado con motivo, carga de botón y de región, vacío, dato `unknown`, `stale`, diferenciación visual `primary` vs `STOP`/`error`, aceptación pendiente y resultado final.

## Componentes de organización 0.1.4

| Componente | Responsabilidad | Evidencia | Cobertura | Verificación | Aprobación |
|---|---|---|---|---|---|
| TaskSections | Pestañas de tarea, un panel montado, teclado y referencias ARIA; borradores en padre | Propuesta VIS-014 | Fuente implementada | No verificada en ejecución | Dirección autorizada; detalle estético pendiente |
| Disclosure | Details/summary nativo cerrado al iniciar para información secundaria | Propuesta VIS-014 | Fuente implementada | No verificada en ejecución | Dirección autorizada; detalle estético pendiente |
| AppShell | Sidebar, navegación móvil, cabecera compacta y STOP sticky | Confirmada en dirección, Propuesta en composición | Fuente implementada | No verificada en ejecución | Dirección autorizada; detalle estético pendiente |

| Before | After | Why |
|---|---|---|
| Control mostraba cuatro operaciones simultáneas | Movimiento, prueba, GOTO y calibración por pestaña | Reduce decisiones simultáneas y separa flujos |
| Cabecera con tres estados, temas, conexión y navegación | Ubicación, Conexión desplegable y STOP | Jerarquía estable y menor carga visual |
| Inicio mezclaba control, telemetría y capacidades | Seis accesos y diagnóstico bajo demanda | Prioriza elegir sección |
| Documentación abierta completa | Temas cerrados, solo uno abierto | Lectura gradual |
| Fondo con halos y paneles con sombras fuertes | Fondo neutro y paneles sin sombra | Profundidad tranquila sin cambiar marca |

## Componentes flotantes 0.1.5 — VIS-015

| Componente | Anatomía y estados | Teclado/foco y responsive | Evidencia | Cobertura | Verificación | Aprobación |
|---|---|---|---|---|---|---|
| AppShell/RouteNav | Rail, marca, colapso, OLED superior, rutas, Ajustes; encabezado con modo/estado/STOP | Nombres ARIA aun colapsado; preferencias opcionales; rail móvil persistente y expansión superpuesta | Confirmada en dirección / Observada en ST / Propuesta dimensiones | Fuente adaptada | No verificada | Dirección autorizada; estética pendiente |
| FloatingModal | closed/opening/open/closing, material, cabecera sticky con STOP y cerrar, cuerpo único | dialog.showModal; Escape/teclado inmediatos, foco nativo/retorno, salida diferida e inert, reversión; ancho min(760px,viewport), scroll propio | Observada en buscador ST / Propuesta panel central | Fuente adaptada | No verificada | Dirección autorizada; estética pendiente |
| Tarjeta de sección/SectionArtwork | Texto izquierdo, dibujo simbólico derecho, nombre, entrada y flecha | Link Enter / botón OLED Enter-Space; uno por fila al estrechar; sin hover de ratón en tacto | Confirmada composición / Propuesta ilustraciones | Fuente adaptada | No verificada | Dirección autorizada; estética pendiente |
| LiveOledViewer | Disponibilidad, captura o vacío, edad, error/reintento, encoder si autorizado | Polling sin solapamiento; pausa en background; gestos cancelables; sin SW si falta capacidad | Confirmada semántica / Propuesta consumidor | Fuente adaptada; productor pendiente | No verificada | Dirección autorizada; integración pendiente |

Las recetas anteriores de modal 250ms y navegación móvil horizontal son históricas, sustituidas en estas superficies. Nuevos modales usan FloatingModal. Tokens y perfil: ../WEB_FLOATING_UI.md, ../../contracts/WEB_OLED_CONSUMER.md. No se heredaron roles/servicios de comercio ni se agregaron librerías.

## Componentes 0.1.6 — VIS-016

| Componente | Cambio | Teclado y límites | Evidencia | Cobertura | Verificación | Aprobación |
|---|---|---|---|---|---|---|
| BrandMark | Imagen WebP transparente libre, header44/hero80 desde256px, fallback AFR2; bienvenida1024px y favicon64px (AFR2-20261024-03) | Nombre y dimensiones reservadas, sin marco | Confirmada dirección / Propuesta derivado y tamaños | Fuente adaptada y compilada | Imagen/alpha inspeccionados; Web No verificada | Dirección autorizada, estética pendiente |
| StartupWelcome | Entrada/pausa/salida/velo ST adaptados, portal/dialog propio | Escape/Omitir, STOP, foco nativo/retorno, fallo imagen/fallback4s, reduced fade120 | Observada ST / Confirmada solicitud | Fuente adaptada | No verificada en navegador | Dirección autorizada, estética pendiente |
| Afr2Toaster | Un Toaster bottom-right en todos los tamaños, offsets inferiores con safe areas | Cierre explícito, sin listener de viewport/medición de cabecera; margen móvil respeta rail | Confirmada ubicación | Fuente adaptada | No verificada en navegador | Dirección autorizada |

La receta de medallón negro y posición superior de toast queda sustituida; los modales operativos siguen usando FloatingModal.

## Componentes0.1.7 — VIS-017

| Componente | Antes | Después | Evidencia/cobertura | Verificación |
|---|---|---|---|---|
| StartupWelcome | Velo simplificado, controles con aspecto de tarjeta | Velo/halo/sheen Radal, logo libre decodificado, controles discretos | Observada en ST; fuente adaptada | No verificada en Web |
| Toast | Título truncate, ancho móvil asimétrico y recorte | Texto multilínea, item100%, contenedor acotado y un toast legible por vez | Observada en código; fuente adaptada | No verificada en Web |
| ControlMap | No había mapa en Control | Superior/Perspectiva/Seguir auto, zoom/centrar, trazo/edad | Propuesta autorizada; consumidor en fuente | Productor pose pendiente; Web No verificada |

Teclado: botones nativos, nombres y aria-pressed en cámaras, sin enviar movimiento. Cambiar cámara cancela hold. Escala/centro son del visor; datos recibidos quedan en unidades mm/mrad. Vista superior se adapta al tamaño real del contenedor para conservar legibilidad móvil. Los errores 3D permiten elegir Superior. Títulos toast completos y cierre accesible aparte; en tacto cierre44px. Estado crítico del robot permanece inline, no depende del toast.


## Componentes 0.1.8 — VIS-018

| Componente | Antes | Después | Evidencia/cobertura | Verificación |
|---|---|---|---|---|
| `DismissibleNotice` | Aviso STOP fijo en cabecera y modal sin botón de cierre | Componente compartido con X accesible (`44×44 px`), `noticeId` y cierre sincronizado que preserva estado real | Confirmada dirección; fuente adaptada | No verificada en Web |
| `EncoderControl` / `LiveOledViewer` | Controles ocultos sin captura fresca y sin botón manual de actualizar | Siempre visibles (Giro izq., SW, Giro der. y Actualizar), motivo junto al control, estado largo (`800 ms`, `--warning`) mantenido hasta soltar | Observada en `ScreenStreamWorkspace`; fuente adaptada | Productor remoto pendiente; Web No verificada |
| `ChassisSpinGraphic` | Botones textuales de giro en Control | Vista superior de chasis Mecanum con flecha circular antihoraria/horaria en Control y bloques `pivot` | Confirmada dirección; fuente adaptada | No verificada en Web |
| `SequenceEditorPanel` / `TimelineRow` / `MotorBlock` | Control sin tarea de secuencias ni simulador Mecanum | Tarea Secuencias con Movimientos y 4 pistas FL/FR/RL/RR, copia avanzada, simulador 10 ms y cliente físico ROBO2 perfil 3 | Observada en `TimelineRow`/`MotorBlock`; fuente adaptada | Productor físico pendiente; Web No verificada |
## Vigencia0.1.9 — VIS-019

- Chevron compartido para Disclosure/select: giro180° solo SVG; no transformar
  spans/títulos. Teclado y reduced motion inmediatos.
- Control: paneles24px/20px móvil, ancho completo, propiedades debajo de pista
  seleccionada. Una tarea visible; botones sin cambio de tamaño por estado.
- Sistema: placas en Disclosure, datos consultados o Sin datos, reinicios con
  estado de aceptación separado de boot observado. Reusa en Ajustes/Desarrollador.
- Desarrollador: BrandMark propio, perfil y enlaces con áreas≥44px, AppShell
  conservado y ninguna ruta de barra seleccionada. Triple toque es descubrimiento,
  no autenticación.
- STOP: una acción global contextual, sin duplicados de control/cancelar en Control;
  réplica condicional en diálogo por accesibilidad del documento inert.

Propuesta implementada/compilada; cotejo visual pendiente por navegador denegado.



## VIS-035 — Sensores y red (fuente, sin comprobación visual)

| Componente | Comportamiento | Evidencia / límite |
|---|---|---|
| Radar/servo | Composición GOTO, escala métrica, datos laterales, muestras por ángulo, control directo y barrido autónomo | Dirección Confirmada; layout Propuesto, sin navegador |
| IR/Ruedas | Tareas separadas, originales5canales con unidad correcta, sin ceros para desconocido | Fuente integrada; adquisición real pendiente |
| NetworkPill/Red | Verde STA, amarillo AP, icono según RSSI, modal propio y Configuración renombrada | Usa tokens/primitivas; señal AP no se inventa |
| RobotModel/Needle | Interpolación críticamente amortiguada, giro HC/soporte, cuerpo SG90 fijo; Modelo3D estático | Pivote visual inferido; PWM off no centra la animación |
| OLED hermanas | C/TS enteros, mismos recursos, códigos propios; giro alterna/corto retorna | Biblioteca copiada; paridad física no comprobada |

## VIS-036 — Inspección de componentes del modelo

| Componente | Anatomía y comportamiento | Evidencia / límite |
|---|---|---|
| ModelPoint | Punto exacto, línea y selector numerado44px; proyección inmediata con cámara, separación de vecinos, oculta fuera del encuadre | Pedido Confirmado; referencias Inferidas del GLB, sin comprobar visualmente |
| ModelInfo | Ficha no modal en un rincón fijo; icono, modelo, explicación, datos, acción; scroll propio, X/Escape/clic exterior | Composición Propuesta con material popover común; drag/zoom conserva ficha |
| Selector de piezas | Desplegable con los once componentes y foco alternativo si el punto sale del encuadre | Teclado, nombres accesibles y referencias DOM; ejecución No verificada |
| Accesos a tareas | Distancia/servo, línea, rueda preseleccionada, Control, Wi-Fi y OLED | Solo navegación; no inicia movimientos o reinicios |

Manual0.1.9 autorizado; superficie/acento/iconos existentes. Ambos temas por
tokens, tamaño mínimo44px, reduced motion/transparency y estado sin telemetría
inventada. No cambia semántica del encoder ni crea pantallas de firmware.
Alcance fuente; sin build/browser/tests/validaciones. Cobertura15 y contrato
WEB_MODEL_INSPECTION.md documentan referencias y límites.


## VIS-037 — Radar y parámetros (fuente16)

| Componente | Antes | Fuente actual | Verificación |
|---|---|---|---|
| RadarScene | Cámara cenital y líneas planas | Plataforma/anillos tubulares, proyección oblicua, contorno/eco con volumen, sector previo | No verificada; sin render/build |
| ScanAngleRange | Desde/Hasta independientes | Barra doble0–180°, paso/separación5°, campos numéricos y operación táctil/teclado | No verificada; sin navegador |
| ScanRangeField | Entrada numérica sola | Valor/unidad arriba a la derecha, barra y límites; Enter/blur/Escape | No verificada; sin navegador |
| Configuracion | Etiquetas dispares y título mal codificado | Mismo nombre sin acento en lateral, modal, tooltip y nombre accesible | Fuente; sin comprobación visual |

El campo numérico se normaliza antes de modificar el borrador y la barra. El
barrido físico sigue exigiendo Aplicar; no se producen órdenes al arrastrar los
parámetros. Carácter rubí propio, chrome neutro y límites del firmware vigentes.


## VIS-038 — Radar como instrumento (fuente26-02)

| Before | After | Why |
|---|---|---|
| Escena en pedestal sólido | Campo continuo y sector rubí con retícula desvanecida | Integración con el mapa canónico |
| Encuadre4m y controles en una columna larga | Inicio1m, vistas Perspectiva/Superior y parámetros debajo | Protagonismo del auto y espacio del mapa |
| Datos al mismo nivel | Distancia principal, ángulo aplicado y diagnósticos en Disclosure | Jerarquía de lectura |

`radar.css` concentra esta composición; `sensors.css` conserva los controles
compartidos/IR/ruedas/red. ScanRangeField/ScanAngleRange y Button/Disclosure
siguen siendo las primitivas. GPU bajo demanda y shader sin reloj. Estado real:
fuente integrada; no renderizado/compilado/comparado. No verificado.


<a id="infraestructura-integrada"></a>
## Contratos y recetas integrados desde Infraestructura — VIS-057–061

Lectura estática de S01–S23 incorporados a sources.md. INF-VIS identifica decisiones de origen; su correspondencia general está en decisions.md. Las rutas abreviadas de tokens de estas fichas pertenecen a los namespaces `infrastructure`. El carácter Propuesta de formularios/menús/overlays en esta muestra no rebaja ni sustituye sus contratos generales ya incorporados.

### 1. Matriz de cobertura

`Cubierta` significa descrita desde la muestra, no probada ni aprobada. Las superficies ausentes de esta referencia se identifican como extensiones Propuestas respecto de ella; sus contratos generales anteriores siguen vigentes.

| Elemento | Tema / estados | Evidencia | Cobertura | Verificación | Aprobación | Decisión | Fuente |
| --- | --- | --- | --- | --- | --- | --- | --- |
| Portada por capas | Fija oscura, ambos temas; montaje | Observada | Cubierta | No verificada | Pendiente | INF-VIS-003 | S01/S02/S12 |
| Chips de portada | Estáticos, wrap | Observada | Cubierta | No verificada | Pendiente | INF-VIS-003 | S01/S02 |
| Mapa/carriles | 3 composiciones por ancho | Observada | Cubierta | No verificada | Pendiente | INF-VIS-008 | S01/S02 |
| Nodo accionable | Reposo, hover, active, focus | Observada | Cubierta | No verificada | Pendiente | INF-VIS-008 | S01/S02 |
| Wire | Vertical/horizontal, loop/reduce | Observada | Cubierta | No verificada | Pendiente | INF-VIS-009 | S01/S02 |
| Selector segmentado | Puntero, teclado, resize, reduce | Observada | Cubierta | No verificada | Pendiente | INF-VIS-006 | S05/S06/S12 |
| Opciones disabled del selector | Admitidas por primitiva, no usadas aquí | Observada | Parcial | No verificada | Pendiente | INF-VIS-006 | S05/S06 |
| Estaciones/línea | next/current/done, foco, hover | Observada | Cubierta | No verificada | Pendiente | INF-VIS-007 | S01/S02 |
| Ficha de etapa | Cambio, inicio, final | Observada | Cubierta | No verificada | Pendiente | INF-VIS-010 | S01/S02/S03 |
| Button | Primario/secundario, disabled/loading/active | Observada | Cubierta | No verificada | Pendiente | INF-VIS-011 | S07/S13 |
| Enlaces | Modelos/Conexión, foco y hover | Observada | Cubierta | No verificada | Pendiente | INF-VIS-011 | S01/S02/S13 |
| Insignia de zona | pc/esp/link/client | Observada | Cubierta | No verificada | Pendiente | INF-VIS-002 | S01/S02/S12 |
| Barras de peso | 3 individuales + suma de 3 tramos | Observada | Cubierta | No verificada | Pendiente | INF-VIS-012 | S01/S02/S10/S11 |
| Compresión | Raw/gzip, menos/más/sin comparación | Observada | Cubierta | No verificada | Pendiente | INF-VIS-012 | S01/S02/S04 |
| Anillo FS | Con dato/sin dato, consulta | Observada | Cubierta | No verificada | Pendiente | INF-VIS-013 | S01/S02/S21 |
| Estadísticas | 1/2/4 columnas, dato ausente | Observada | Cubierta | No verificada | Pendiente | INF-VIS-012 | S01/S02 |
| Tabla | Con reporte, hover, foco/scroll | Observada | Cubierta | No verificada | Pendiente | INF-VIS-012 | S01/S02 |
| Estado informativo sin reporte | Error y falta de compatibilidad | Observada | Cubierta | No verificada | Pendiente | INF-VIS-013 | S01/S04 |
| Mensaje de fallo de placa | Rama declarada, propagación limitada | Observada | Parcial | No verificada | Pendiente | INF-VIS-014 | S01/S21 |
| Glosario | Entrada/hover, texto abierto | Observada | Cubierta | No verificada | Pendiente | INF-VIS-005 | S01/S02 |
| Atributos title | Bytes exactos en cifras | Observada | Parcial | No verificada | Pendiente | INF-VIS-012 | S01 |
| Tooltip propio | Sin implementación directa | Propuesta | No cubierta | No verificada | Pendiente | INF-VIS-014 | Sin muestra |
| Campos/formularios editables | Sin implementación directa | Propuesta | No cubierta | No verificada | Pendiente | INF-VIS-014 | Sin muestra |
| Dropdown/select/menú | El selector observado es segmentado | Propuesta | No cubierta | No verificada | Pendiente | INF-VIS-014 | Sin muestra |
| Pestañas tablist | Primitiva las admite; aquí usa radiogroup | Observada | Parcial | No verificada | Pendiente | INF-VIS-006 | S05 |
| Modal/diálogo local | Sin implementación directa | Propuesta | No cubierta | No verificada | Pendiente | INF-VIS-014 | Sin muestra |
| Toast propio | No disparado por esta página | Propuesta | No cubierta | No verificada | Pendiente | INF-VIS-014 | Sin muestra |
| Login/acceso | Sin implementación directa | Propuesta | No cubierta | No verificada | Pendiente | INF-VIS-014 | Sin muestra |

### 2. Contratos de los componentes presentes

#### 2.1 Hero narrativo

**Propósito:** dar contexto, mostrar un recorrido general y abrir sus etapas. Anatomía obligatoria de la muestra: glow decorativo → copy → mapa → takeaway. El copy contiene eyebrow, único h1, lede y lista de chips. Color fijo oscuro; fuente S01/S02/S12; tokens `sections.banner`, `components.hero`.

Se admite texto de dos líneas, contexto hasta 64ch y chips con wrap. El mapa aprovecha todo el ancho del hero. La ilustración no carga modelos 3D ni necesita raster. La grilla y los halos quedan detrás del contenido; el recorte y el stacking pertenecen a la caja exterior. No hay semántica interactiva en el glow ni en el takeaway.

**Regla extraída:** separar material de portada y material de lectura. **No deducir:** que cada tarjeta de contenido deba tener fondo oscuro, grilla o blur. Las capas de esta portada son una composición específica.

#### 2.2 Carril y FlowNode

**Propósito:** agrupar el trabajo por equipo y ofrecer accesos a etapas. Carril con `data-zone`, etiqueta absoluta y body; FlowNode con botón, icono, strong y detalle. El color sale de la zona de banner. El nodo requiere `onPick`, no recibe prop de estado seleccionado.

Tiene hover/active/focus, sin disabled ni loading propios. Los tres carriles responden al ancho según manual §16. Se conserva un nombre accesible descriptivo por botón. El nodo se puede enfocar y activar de forma nativa; la selección se expresa en el recorrido de destino.

**Límite:** no extraer de él una tarjeta genérica seleccionable con check, ni una orden de hardware. Hover/active tienen la particularidad de cascada documentada. Tokens `components.flowNode` y zonas.

#### 2.3 Wire

**Propósito:** visualizar transmisión entre nodos/celdas, sin representar tráfico real. Anatomía: span aria-hidden, dos `i`, `em` opcional. Vertical en base; horizontal según ubicación y breakpoint. `delay` se aplica a paquetes. `label` puede mostrar .bin/gzip. Dentro de la transformación de etapa usa zone; en portada usa colores del banner.

Decoración sin foco ni nombre accesible propio. Con reduced motion queda línea estática sin paquetes. No debe convertirse su velocidad en una medida de ancho de banda o actividad del equipo. Tokens `components.wire`, `motion.wire`.

#### 2.4 Selector de recorrido

**Propósito:** elegir entre dos explicaciones locales. Anatomía de doble fila y recorte descrita en manual §8. Variante efectiva `choice`, dos radios, roving focus, auto activation, size md y fill never. Texto/icono/fondo activos coinciden por la copia recortada.

Estados: reposo, hover, selected, active de label, focus; disabled disponible en primitiva pero ausente de estas opciones. Teclado cicla entre habilitados; Home/End, Enter/espacio mediante botón; sin animación de selección por teclado. Resize recoloca instantáneo. La selección reinicia paso a cero.

**Combinación correcta observada:** selector a la derecha de un h2 dentro de heading flex con wrap. **No derivar:** dropdown, pestañas de navegación de ruta o selector que envía comandos al robot. Tokens `components.segmented` y `motion.segmented`.

#### 2.5 Metro

**Propósito:** elegir una de ocho etapas y mostrar ubicación relativa. Anatomía: nav → ol con dos pseudo-líneas → li zonal y botón → dot y label. Las etapas anteriores se llenan y muestran check; la actual crece y recibe halo; las posteriores son neutras.

Los labels se ocultan en celular; números/check permanecen. Scroll del rail automático si hay desborde; foco y selección se mantienen. Flechas se limitan a extremos, Home/End disponibles. El actual lleva aria-current step. Color no basta para identificar estado: número/check, escala y ubicación lo acompañan.

**Límite funcional:** done significa índice anterior, no paso visitado ni tarea cumplida. La barra no es loader. Tokens `components.metro`, `motion.metro`. Particularidad hover de done en manual §9.

#### 2.6 Ficha de etapa

**Propósito:** exponer una transformación y detalles de una etapa. Anatomía: main con zona/contador, título/icono, descripción, Entra/Sale; side con h4, hechos y navegación. Se renderiza un único artículo con aria-live polite; key variable remonta todo el contenido.

Variantes por zona y por posición primera/intermedia/última. Entra/Sale vertical hasta 600 px, luego tres columnas. El lateral pasa de debajo a derecha a 840 px. Foco de la estación o botón que inició la acción no se mueve explícitamente al h3.

La salida tiene el acento interior izquierdo; los hechos tienen numeración CSS y entrada en cascada. La línea de fuente técnica no se presenta. No deducir un modal ni una ficha expandible. Tokens `components.stage`, `motion.stage`.

#### 2.7 Button y enlace de acción

**Propósito:** navegación de pasos y consultas de datos. Variante primaria para avanzar disponible; secundaria para retroceder y refrescar. Size sm, altura mínima 44 px. Iconos de navegación/refresh; spinner reemplaza al icono principal cuando loading. Disabled evita activar; los límites deshabilitan Anterior/Siguiente.

Foco nativo reforzado por global, feedback de escala active; no hay cambio cromático específico de hover/active declarado para Button. El enlace a Modelos usa flecha que se desplaza y conserva navegación por Link. La consulta a placa solo se ofrece según ready/origen; no es visible siempre.

**No generalizar:** los tokens de hover/active existentes como estados efectivamente consumidos por estos botones. Tokens semánticos de acción/superficie y `motion.buttonPress`.

#### 2.8 Badge de zona/procedencia

**Propósito:** identificar equipo u origen de un dato. Span inline-flex, icono 14 px y etiqueta, padding 6×12 px, gap 7 px, radio pill. Fondo/tinta/borde de la misma familia. La ficha lo hereda; el origen del paquete lo declara pc/esp en el propio elemento.

No es botón ni filtro. El texto mantiene la procedencia explícita; local preparado no se presenta como placa instalada. Token `components.zoneTag` y familias semánticas.

#### 2.9 Barras de peso y suma

**Propósito:** comparar tamaño de GLB. Anatomía lista → nombre/valor/track → fill; suma con tres fills y posiciones acumuladas. Se conserva etiqueta textual y title de bytes exactos; tracks aria-hidden. Variantes por modelo y suma. Aparición única ligada a Reveal; no hay click.

Debe conservarse la base común de comparación para estas filas. Los mínimos de píxeles son excepciones de legibilidad y no una proporción exacta. No usar estos valores como memoria de ejecución. Tokens `components.modelBars`, `motion.modelBars`.

#### 2.10 Compresión y estadísticas

**Propósito:** comparar archivos antes/después y resumir magnitudes. Compresión: label, barra raw, barra gzip, delta grande, caption. Estadística: icono/label arriba, cifra, detalle. Números tabulares, unidades es-AR y dato ausente explícito.

Se muestran solo si hay manifiesto. Las dos áreas del paquete pasan a columnas desde 840 px; stats a dos desde 600 y cuatro desde 1200. No existe recuento numérico animado. Tokens `components.compression`, `components.stat`.

#### 2.11 Storage y anillo

**Propósito:** mostrar capacidad/uso reportados y permitir consulta. Anatomía ring a izquierda + bloque de texto/acción; en móvil se apila. El espacio ring se mantiene sin dato, cambiando porcentaje por board. Progress invisible para lectores acompaña al gráfico decorativo cuando existe dato.

Estados sin dato, con dato, conectado, consulta busy y rama local failed. El porcentaje no se anima en la implementación. Procedencia HTTP tiene prioridad según la condición de fuente; no hay polling propio de la página para este anillo. La rama de error tiene la limitación de S21.

No añadir thresholds de estado como si existieran. Tokens `components.storage`, `components.ring`.

#### 2.12 Tabla de archivos

**Propósito:** comparar cada recurso. Columnas: ruta, antes, guardado y cambio. Caption identifica local/ESP; encabezados scope; números nowrap/tabulares; rutas code con wrap; pares de mini barras por fila. Wrapper enfocable y scrollable para anchos pequeños.

Hover solo resalta fondo; no cambia selección ni abre detalle. Sin estados de sorting, filtros o paginación en muestra. Datos ausentes ocultan la tabla, no crean filas ficticias. Tokens `components.fileTable`.

#### 2.13 Estado de reporte ausente

**Propósito:** explicar por qué faltan datos sin interrumpir el recorrido. Anatomía flex con caja de icono, strong y párrafos; borde dashed, superficie subtle y tinta muted. role=status en wrapper. El botón de refrescar sigue en cabecera.

El componente es un div local, no una alerta compartida genérica ni modal. La acción de recuperación reside fuera del bloque. No deducir que errores físicos deban usar esta misma jerarquía informativa.

#### 2.14 Glosario

**Propósito:** lectura simultánea de cuatro conceptos. Icono + h3 + párrafo completo. Fondo solid, halo recortado abajo a derecha, hover con elevación y entrada escalonada. Una/dos/cuatro columnas. Sin teclado de interacción ni selected/expanded; es article estático.

La geometría se puede describir como tarjeta editorial. Copiarla como tarjeta de acción requeriría una decisión Propuesta para añadir semántica de enlace/botón y foco correspondiente. Tokens `components.concept`.

### 3. Recetas de composición derivadas

| Arquetipo | Objetivo y orden | Densidad / responsive | Procedencia |
| --- | --- | --- | --- |
| Portada explicativa | Etiqueta → promesa h1 → contexto → chips → mapa de accesos → takeaway | Copy con ancho de lectura; mapa ocupa espacio libre; carriles se reorganizan por ancho. | Observada, INF-VIS-001/003/008 |
| Exploración por etapas | Kicker/h2 + selector → línea → ficha → acciones anterior/siguiente dentro del lateral | Una etapa visible; columnas del detalle solo cuando hay espacio suficiente. | Observada, INF-VIS-007/010 |
| Comparación de magnitudes | Kicker/h2 + acceso secundario → filas numéricas con barras → suma diferenciada → explicación de unidades/límite | Nombre y valor arriba; track abajo; no exigir una tabla para tres modelos. | Observada, INF-VIS-012 |
| Reporte de archivos | Cabecera/refresh → loading/error/origen → comparación + almacenamiento → stats → tabla → procedencia | Lectura general antes del detalle; tabla conserva scroll; storage visible sin reporte. | Observada, INF-VIS-012/013 |
| Glosario | Kicker/h2 → tarjetas con icono/título/texto | Texto abierto, 1/2/4 columnas, sin pasos ocultos. | Observada, INF-VIS-005 |
| Carga/ausencia de datos | Estado textual + feedback del botón → explicación y posibilidad de consulta/refresh | No reemplaza el contenido educativo por una pantalla vacía. | Observada, INF-VIS-013 |
| Listado no técnico | Adaptar jerarquía a contenido dominante; selección, filtros y acciones por especificar | No existe un listado genérico de registros en la muestra. | Propuesta, no diseñada, INF-VIS-014 |
| Formulario/acceso | Necesita fuente o especificación propia de campos, errores y confirmación | No heredar un layout de inputs desde chips o nodos. | Propuesta, no diseñada, INF-VIS-014 |
| Modal/menú/tooltip | Necesita contrato explícito de apertura, foco, cierre y material | No extrapolar glow, drop-shadow o línea de progreso a un overlay. | Propuesta, no diseñada, INF-VIS-014 |

### 4. Mapa de materiales y límites

| Región | Acento | Superficie | Rasgo dominante | No extender automáticamente |
| --- | --- | --- | --- | --- |
| Hero | Cuatro banner-zone fijos | Azul noche, capas radiales/grilla | Expresividad técnica | A todas las tarjetas de trabajo. |
| Recorrido | Zona de la etapa | Solid + lateral subtle | Selección y contexto | A estados de ejecución del robot. |
| Pesos | Por modelo | Solid/track subtle | Proporción y presencia mínima | A memoria real o tráfico. |
| Paquete | Action + procedencia pc/esp | Solid/subtle | Datos y origen | A aprobación de carga/instalación. |
| Glosario | Action | Solid + halo local | Lectura abierta | A una tarjeta accionable sin semántica. |

No se recomienda ningún componente externo nuevo: esta extracción describe las primitivas existentes y sus límites.
