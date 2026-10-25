# Componentes y cobertura

Contratos cerrados a nivel documental (`0.1.3`) para ambos temas. Cada control conserva geometría en `default/hover/focus/active/disabled/loading/error/success`. Ninguno fue renderizado todavía en la nueva interfaz.

| Componente | Anatomía y variantes | Teclado, movimiento y límites | Evidencia | Cobertura | Verificación | Aprobación | Decisión |
|---|---|---|---|---|---|---|---|
| `BrandMark` (Logo + wordmark) | medallón `#000000` (`36×36 px` header / `64×64 px` Inicio) con `Logo.png` (`object-fit: contain`), borde `border` y reflejo `highlight`, más texto `AutoFinalRobo2`; fallback monograma `AFR2` en carmesí si la imagen no está disponible | Enlace a Inicio (`#/`) con `Enter`, foco visible `2px` carmesí/rubí y dimensiones fijas (`CLS = 0`) | Confirmada / Propuesta | Cubierta en especificación | No verificada | Aprobada | VIS-013 |
| Botón/enlace | label, icono opcional, foco; variantes `primary` (carmesí `#b81d33` / rubí `#ff6b7d`), `secondary` (`glassField`/`glassPanel`), `danger (STOP)` (bermellón `#c93b1b` / `#ff8a65` sobre `surfaceSolid` con borde e icono explícito) | `Enter` en enlaces; `Enter/Space` en botones; presión `scale(.97)` `150ms` `easeOut` (`0ms` en teclado/reduced motion); `STOP` siempre visible en contexto activo y nunca confundible con `primary` | Observada / Propuesta | Cubierta en especificación | No verificada | Aprobada | VIS-004/005/008/011/012/013 |
| Campo/select | label persistente, control `glassField`, ayuda y error; mínimo `44px`, texto `≥16px` | `<select>` nativo sin atribuir animación glass al popup del SO (§6.2); listbox `glassPopover` solo si se extiende con primitiva accesible | Observada / Propuesta | Cubierta en especificación | No verificada | Aprobada | VIS-004/005/008/012 |
| Tarjeta/panel | `glassPanel` o `glassDense`, título, contexto, contenido y acciones; sin barra superior de 5 px (§3.4) | Tarjeta estática no se eleva; tarjeta accionable hasta `2px` con `hover:hover` y `pointer:fine` (§6.1) | Confirmada / Propuesta | Cubierta en especificación | No verificada | Aprobada | VIS-002/004/012 |
| Tabs/segmentado | pista `glassField`, indicador con borde/realce carmesí sutil (`selection`) y panel asociado | Indicador `250ms` `easeStandard`, opción `160ms`, presión `140ms`; roving focus con flechas/Home/End y `data-input="keyboard"` instantáneo (§6.8) | Observada / Propuesta | Cubierta en especificación | No verificada | Aprobada | VIS-005/011/012/013 |
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

