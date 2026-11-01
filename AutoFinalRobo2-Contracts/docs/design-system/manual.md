# Manual operativo — 0.1.3 draft

## Emblema vigente — VIS-046 / 31-10-2026

Confirmada: el usuario designa `Neon Red Technical Robot Cutout.png` como nuevo
logo oficial de la app. Maestro exacto en `assets/Logo.png` y
`Web/reference/brand/Logo-neon-robot-source.png`. Esta decisión sustituye las
referencias gráficas anteriores del emblema en este manual; se conserva la
identidad0.1.9 existente para controles, superficies y estados.

BrandMark comparte `/brand/Logo-robot-256.webp` entre navegación, Inicio y
Desarrollador. Bienvenida usa `/brand/Logo-robot-1024.webp`; favicon64 y touch180
usan sus derivados propios. Mantener proporciones, transparencia, contención
completa, fallback AFR2 y espacio reservado. No añadir fondos ni recortar el
dibujo. WebP implica reducción de resolución/color; original sin cambios.
Fuente implementada, aspecto en ejecución No verificado; sin build ni tests.

## Propósito y composición
**Inferida / Aprobada (VIS-001, VIS-013):** consola instrumental del robot: conexión y requisitos comprensibles, acciones explícitas y lecturas con validez. Orden: título y propósito → estado de enlaces → acción primaria → datos → feedback y ayuda. Inicio organiza accesos; cada página conserva su tarea dominante. Identidad propia mediante el isotipo oficial `D:/ProyectoRobotica2/assets/Logo.png` (robot Mecanum en wireframe carmesí/rubí sobre fondo `#000000`) enmarcado en medallón `BrandMark` (`36×36 px` en cabecera, `64×64 px` en Inicio) con fallback de monograma `AFR2`, junto al wordmark `AutoFinalRobo2`. Sin logotipos de terceros ni la barra decorativa superior de 5 px de origen (§3.4).

## Material y tema
**Confirmada en dirección / Propuesta en escala / Aprobada (VIS-002):** usar vidrio del informe como lenguaje común en todos los paneles normales (§§2, 6, 15). Panel 88% blanco claro / 90% RGB `20,35,59` oscuro (`Observada` en `surface-glass`), blur `20px` y saturación `145%`; chrome `72/76%`, `16px`, `140%`; dense `94/96%`, `20px`, `140%`; field `56/62%`, `12px`, `120%`; popover `94/96%`, `20px`, `140%`; dialog `88/90%`, `24px`, `150%`. Un solo blur por región; tablas y filas comparten material. La transparencia afecta al fondo; texto e iconos conservan opacidad normal. No animar blur en paneles ni colocarlo en todas las capas anidadas.

**Confirmada en exclusión y acento rojo / Observada y Propuesta en tokens / Aprobada (VIS-003, VIS-013):** superficies glass sobre base neutra del informe (§3) combinadas con acento primario carmesí/rubí extraído de `Logo.png`. Claro: fondo `#f6f8fc`, superficie elevada `#ffffff`, superficie sutil `#eef3f8`, texto `#132033`, secundario `#4f6178`, acción primaria carmesí `#b81d33` (hover `#961426`, `onAction` `#ffffff`), foco `#b81d33` y selección `rgba(184,29,51,.10)`. Oscuro: fondo `#0b1525`, superficie elevada `#14233b`, superficie sutil `#1c2f4f`, texto `#ecf2fb`, secundario `#b6c4d9`, acción primaria rubí neón `#ff6b7d` (hover `#ff8594`, `onAction` `#0b1525`), foco `#ff6b7d` y selección `rgba(255,107,125,.14)`. Diferenciación estricta entre identidad carmesí y emergencia (`STOP`/`error`): el estado de error y el botón `STOP` usan rojo bermellón (`#c93b1b` en claro / `#ff8a65` en oscuro) sobre `surfaceSolid`, con borde reforzado e icono explícito de parada/alerta, para que jamás se confundan con un botón primario normal. Tema `light/dark/system`, clave propia `afr2-theme`, suscripción central a `matchMedia` en el provider y `color-scheme` en raíz/`.dark`; Toggle muestra la acción siguiente; cambio inmediato (§4.2). Fondo estático con uno o dos degradados radiales sutiles (§4.3). Fallback sin `backdrop-filter` y `prefers-reduced-transparency`: material sólido (`surfaceSolid`). `prefers-contrast: more` retira blur y refuerza borde/tinta (§10.2, §12).

## Tipo, ritmo y geometría
**Observada y Propuesta / Aprobada (VIS-004):** `Segoe UI Variable, Segoe UI, system-ui, sans-serif`, sin descarga externa; cuerpo `16px/1.5`, ayuda mínima `14px`, títulos `20–24px` peso `700` tracking `-.02em`; cifras tabulares (§5.3). Espacios `4/8/12/16/24/32/48px`. Controles `≥44px`, inputs `≥16px` para evitar zoom móvil (§5.4). Radios control `14px` (`0.875rem`), card `24px` (`1.5rem`), panel `32px` (`2rem`), cápsula `999px` (§5.1). Panel padding `clamp(16px,3vw,32px)`. Mantener dimensiones en hover, foco, carga, error y deshabilitado. Sombras neutrales (`shadowControl`, `shadowCard`, `shadowPanel`, con `shadowPopover` como alias expreso de `shadowPanel`, §5.2) y reflejo superior discreto (`inset 0 1px 0 var(--glass-highlight)`); sin glow ambiental continuo.

## Movimiento y acceso
**Observada y Propuesta / Aprobada (VIS-005, VIS-012):** feedback de pulsación inmediato; acción normal al soltar y gesto cancelable. Hover solo con `@media (hover: hover) and (pointer: fine)` (§10.3). Tokens base del informe (§9.1, §12): `motionFast` `150ms`, `motionStandard` `200ms`, `motionSlow` `300ms`; curvas `easeStandard` `cubic-bezier(.2,.75,.25,1)`, `easeOut` / `easeEnter` `cubic-bezier(.23,1,.32,1)`, `easeInOut` `cubic-bezier(.77,0,.175,1)` y `easeDrawer` `cubic-bezier(.32,.72,0,1)`. Pulsación de botón `scale(.97)` en `150ms` con `easeOut` (§6.10, §9.2); rutas `opacity` y `translateY(4px)` en `180ms`, sin espera entre salida y entrada. Teclado (`data-input="keyboard"`) y cambios frecuentes inmediatos (`0ms`). Popover/dropdown de puntero entra en `200ms` y sale en `150ms` desde `opacity:0; translateY(-4px) scale(.97)` con `easeOut`, origen en el ancla y conservación del montaje durante la salida (§6.3, §12). Modal glass entra y sale en `250ms` con `easeEnter` desde `translateY(12px) scale(.96)`, scrim `250ms` y desmontaje diferido `250ms` (§6.5). Drawer móvil entra y sale en `240ms` con `easeDrawer` desde `translateX(-100%)`, scrim `240ms` con blur `8px` (§6.6). Segmentado desplaza indicador en `250ms` con `easeStandard` (§6.8). Acordeón anima `grid-template-rows: 0fr -> 1fr`, opacidad y chevron `180deg` en `200ms` como única excepción funcional de layout documentada (§6.9). Tooltip `150ms` con delay de hover `280ms` y foco inmediato (§6.4). Spinner `800ms` lineal solo durante trabajo real (§8.2). Política única de `prefers-reduced-motion` (§10.1, §12): eliminar traslación, escala, rebote y giros continuos, conservando únicamente un fade de opacidad de `120ms` (`0ms` en teclado), sin aplicar un reset global `.01ms !important` que contradiga los fades locales. Se excluyen en páginas operativas las entradas editoriales de `500–1000ms` (§9.3) y la bienvenida acrílica de `1300ms` (§9.4) para no demorar el acceso a datos o a `STOP`. Resortes solo si existe un gesto arrastrable real (§9.5).

**Confirmada / Aprobada (VIS-006):** acceso directo sin autenticación, Protocol Studio ni OLED Studio. OLED: giro, un SW con corto confirma/largo `800ms` abre menú; largo no genera corto al liberar, Volver solo por opción. Esta regla semántica precede al feedback genérico de botones. Perder foco, conexión o visibilidad cancela el gesto. Identidad/selección recibidas del productor, sin éxito optimista. Encoders de interfaz y rueda son distintos.

## Notificaciones
**Confirmada para librería / Propuesta para receta / Aprobada (VIS-007, VIS-010):** Sonner con un Toaster en raíz, fuera de elementos transformados, tema resuelto por el provider y offset que respete safe areas. Envolver llamadas en servicio `notify`: toast normal/info para información, loading con id estable y actualización al resultado, success solo ante confirmación final y error saneado. Capturar excepciones y rechazos también junto al control; un toast no reemplaza el estado persistente. Sin spam por cada alive/reintento: un id de conexión actualizable (`afr2-connection`), recuperación solo después de pérdida observada. Errores físicos o `STOP` sin confirmación permanecen visibles en la página. Credenciales y payloads sensibles nunca en toast, consola ni storage. Cancelar loading al desconectar. El estado de aceptación de una operación no se anuncia como ejecución completada. Shell `glassPopover` propio mediante `toast.custom` manteniendo posicionamiento, cierre y swipe de Sonner (§8.3); no envolver el toast en otra animación externa que compita con la librería.

## Responsive, contenido, capas y accesibilidad
**Propuesta / Aprobada (VIS-008, VIS-012):** `100dvh`, `viewport-fit=cover`, `safe-area-inset`, zoom permitido; contenido seleccionable y botones sin selección. Bajo `820px` reflujo a columna, encabezados con wrap, navegación accesible y escena con alto acotado. Escala de capas `z-index` (§11): contenido `0`, navegación fija `20`, popover contextual `40`, scrim de drawer `50`, drawer `60`, scrim modal `80`, modal `90`, toast/tooltip contextual `100`, actividad global `110`. Respetar contextos de apilamiento creados por `backdrop-filter`, `transform` o `isolation`, y el `top layer` nativo cuando se use `<dialog>`. Labels persistentes, foco visible, icono acompañado de texto y live region sin repetir telemetría. Contenido cerrado o cubierto por un overlay bloqueante queda fuera del recorrido de teclado (`inert` o desmontado tras salida, §6.9, §8.1, §10.3). Distinción de carga por alcance (§8.1): acción puntual → spinner en el botón; carga de dependencias de una región/formulario → overlay local `absolute inset-0` con `glass` (`blur 12px`), radio heredado y región `aria-busy`; proceso global → solo cuando impida realmente toda la aplicación. Voz española directa: acción, requisito y resultado. `Unknown` muestra `—`, `stale` muestra dato con antigüedad; no animar cifras para decorar. Sin data fake en producto. Errores 3D muestran reintento sin bloquear el resto de la aplicación.

## Precisiones de validación y cierre documental (`0.1.1` → `0.1.3`)

**VIS-009 / Propuesta / Aprobada:** `success` claro pasa de `#0f8c53` (`Observada` en §3.3, contraste `4,18:1`) a `#087a46` (`≥ 4,5:1` en todas las superficies declaradas). `borderStrong` pasa de `.28`/`.20` (§3.2) a `rgba(19,32,51,.55)` en claro y `rgba(255,255,255,.50)` en oscuro para superar `3:1` frente a todos los materiales. Opacidad visual de deshabilitado `1`: usar texto secundario, atributo `disabled` y motivo, sin atenuar todo el contenedor. No permitir imágenes, WebGL ni colores ajenos directamente bajo texto glass: encapsular el visor 3D y OLED en su propia región, y usar `surfaceSolid` o `glassDense` si cambia el fondo. Borde fuerte identifica campos y controles; borde sutil solo delimita panel decorativo. El anillo de foco es de `2px`, separado `2px` con `surfaceSolid`, para conservar definición.

**VIS-010 / Propuesta / Aprobada:** Sonner `2.0.8` de la base, un Toaster. Posición `top-right` en escritorio y `top-center` en móvil para no tapar `STOP` en el borde inferior (sustituyendo la ubicación inferior derecha observada en §8.3 del origen comercial), `visibleToasts` `3`, duración `4000ms` y offsets `24/16px` más safe area. En resize solo cambia posición, nunca se monta otro Toaster. La shell headless implementa por sí misma botón `Cerrar` con nombre accesible y `toast.dismiss(id)`. Mensajes rutinarios `role="status"`; fallo urgente `role="alert"`; evitar anuncio duplicado en mensaje inline y toast. `STOP` siempre disponible para enviar, pero no se anuncia detenido sin estado confirmado.

**VIS-011 / Propuesta / Aprobada:** Navegación Web usa enlaces de hash y selección de ruta, sin roles `menu` innecesarios. Tabs locales sin cambio de URL usan tabs ARIA y flechas/Home/End. Menú de acciones usa primitiva accesible con teclado; no agrega navegación atrás al OLED. Menú principal OLED y `Volver` son semántica del firmware, no el menú contextual Web. Los enlaces se activan con `Enter`; `Space` activa botones y conserva desplazamiento nativo sobre enlaces. Foco y atajos nunca interceptan escritura en campos.

**VIS-012 / Observada y Propuesta / Aprobada:** Cotejo íntegro con `D:/ProyectoRobotica2/informe-portabilidad-glassmorphism.md` (§§1–18). Se incorporan las curvas, tiempos, matriz de movimiento, `surfaceSubtle`, escala `z-index` y reglas de overlays de las secciones §§6–15 antes no disponibles.

**VIS-013 / Confirmada y Propuesta / Aprobada:** Adopción de `D:/ProyectoRobotica2/assets/Logo.png` y acento carmesí/rubí (`#b81d33`/`#961426` claro, `#ff6b7d`/`#ff8594` oscuro) sobre vidrio neutro, diferenciando `STOP`/`error` con rojo bermellón (`#c93b1b` claro / `#ff8a65` oscuro), contenedor `surfaceSolid`, borde e icono explícito, y componente `BrandMark` con medallón `#000000` y fallback de monograma `AFR2`.

## Gobernanza
Plan visual cerrado documentalmente en versión `0.1.3`; el estado técnico del manual permanece `draft` y la verificación `No verificada` hasta renderizar y comprobar la interfaz en navegador. No ejecutar scanners, tests, builds o ensayos físicos sin pedido expreso del usuario.

## Composición serena y revelado progresivo — VIS-014, 0.1.4

La organización nueva se guía por elegir tarea → operar → abrir detalles si se necesitan. No repetir control, sensores y capacidades en Inicio: Inicio ofrece destinos; cada sección contiene su operación. Cabecera: ubicación, acceso a conexión y STOP. Preferencias/estados técnicos se abren mediante Conexión. No insertar nombres de comandos en acciones habituales: conservarlos en documentación o detalles.

TaskSections implementa tablist/tabs/tabpanel con flechas/Home/End, ids relacionados y tabIndex; solo el panel activo monta controles. Las variables de formulario viven en la página para preservar borradores al cambiar tarea sin guardarlos ni enviar comandos. Disclosure usa details/summary nativo con foco y teclado; no sustituye la navegación del encoder OLED.

Espacio: contenedor 1120px, sidebar 228px, relleno clamp(24px,4vw,56px), 32px entre secciones y 28px dentro del panel. Formularios con 24px entre grupos. Móvil apila tarjetas y deja navegación horizontal con scroll local; STOP sigue accesible arriba. Fondo neutro sin halos y sombras de panel desactivadas; color propio concentrado en selección/acción, sin recolorear el logo. Conservar preferencias de transparencia/contraste/movimiento.

Comparación visual móvil/escritorio pendiente; edición de fuente no demuestra ausencia de solapamientos en ejecución. No se ejecutan tests, builds ni validadores por implicación.

## Navegación flotante y profundidad — VIS-015, 0.1.5

El pedido actual y las imágenes aportadas precisan la composición: superficies sólidas claras/oscuras, acento rubí y glass en capas flotantes. Esta revisión reemplaza la navegación móvil horizontal y el modal genérico anteriores. La barra de ST se adapta a rail siempre visible, ancho 248px expandido / 76px colapsado / 64px rail móvil (valores Propuesta). Los iconos tienen nombre accesible en ambas variantes, selección por NavLink y preferencia afr2.sidebar-collapsed. En móvil expandir superpone la barra; seleccionar sección o pulsar fuera la colapsa y mantiene el rail.

Tarjetas accionables: texto izquierdo e ilustración SVG simbólica a la derecha (decorativa, sin superponerse al texto), borde suave, sombra baja y acento carmesí/rubí. No hay animación continua. Planos y cantidades del robot no se deducen de esas ilustraciones.

FloatingModal es la primitiva obligatoria de nuevos modales: adapta el material del buscador ST, no su catálogo comercial. Diálogo nativo con overlay transparente blur24/saturate1.2, material con traducción/clip desde origen medido, apertura240/cierre200 easeInOut, backdrop180 y contenido100 con demora80. Geometría de layout no se anima. Escape/teclado instantáneos; reduced motion fade120 y transparencia/contraste reducidos con superficie sólida y scrim. Cabecera con STOP/cerrar sticky, contenido inerte durante salida y retorno al trigger existente. Sonner único mantiene feedback efímero fuera de la cabecera, según su altura real; dentro del top layer los resultados importantes se muestran inline.

El visor global y /oled comparten el consumidor documentado. Sin capacidad/productor, vacío con motivo; no se dibujan datos simulados ni se modifica selección localmente tras enviar acciones. Modo de cabecera requiere robot.state completed y edad válida. Fuente adaptada; observación móvil/escritorio/claro/oscuro y prueba física pendientes.

## Inicio y marca sin contenedor — VIS-016, 0.1.6

Barra e Inicio usan /brand/Logo-transparent.png sin tarjeta, medallón, borde, fondo, sombra externa ni recorte de esquinas. El símbolo conserva superficies oscuras internas y trazo neón; solo el exterior es transparente. Tamaños44/80px Propuesta con fallback AFR2 y dimensiones reservadas. El original se archiva fuera del paquete activo.

Actualización de formato AFR2-20261024-03: cabecera/Inicio usan `/brand/Logo-256.webp`, bienvenida `/brand/Logo-1024.webp` y favicon `/brand/favicon.webp`(64px). Se conserva transparencia y tamaños CSS44/80; no cambia la dirección visual. El maestro RGBA queda en `Web/reference/brand/Logo-transparent-source.png`, fuera del bundle. Conversión con pérdida de color y reducción de resolución, sin afirmar identidad de píxeles ni validación visual de navegador. [Evidencia de conversión y tamaño](../../changes/2026-10-24-03-web-payload.md).

StartupWelcome adapta la secuencia ST solicitada: entrada300/pausa600/salida200/velo300, total1300ms desde imagen lista. Es excepción autorizada a la anterior exclusión de introducciones, solo al montar la Web; no se repite por navegación. Material acrílico con tinta propia y blur24; omitir/Escape y STOP disponibles; foco nativo y fallback4s. Movimiento reducido: fade120 sin pausa/zoom. Transparencia reducida/contraste alto: superficie sólida. No es un indicador de progreso o conexión.

Notificaciones: bottom-right explícito en ambos tamaños por indicación actual; separación24px escritorio/16px móvil, safe areas y margen lateral para el rail. Se conserva Sonner único, cierre explícito y feedback real. La receta de offsets superiores es histórica. No se ejecutaron compilaciones ni revisión visual de aplicación.

## Refinamiento de inicio, feedback y mapa — VIS-017

La introducción recupera los materiales de Radal: velo completo acrílico, halo central y sheen diagonal estáticos, logo libre. No hay panel con radio/sombra o contenedor visual del logo; tampoco controles secundarios con aspecto tarjeta. Se preservan omisión/STOP/foco y reducción de movimiento/transparencia. Las imágenes se decodifican antes de iniciar su secuencia.

Sonner controla una sola notificación visible, inferior derecha y expandida. Su contenedor e item tienen anchos compatibles en todos los tamaños; no mezclar insets asimétricos con la fórmula móvil de la librería. Título/descripción multilínea, overflow-wrap anywhere, cierre separado y texto largo con scroll propio; no aplicar truncate/line-clamp a mensajes. El portal evita recorte por transform/overflow de páginas.

Control ofrece un mapa único por encima de la tarea: grilla sobria, cámaras por botones, telemetría compacta debajo y ayudas secundarias al pie. SVG superior y escena Three.js bajo demanda comparten mm/mrad y trazo observado. Se cambia de cámara sin animación ornamental. Si falta pose, mostrar esquema identificado y «—», sin avance simulado. Con dato stale no se inventa movimiento. Contrato: ../../contracts/WEB_CONTROL_MAP.md. La pose por odometría no es una localización absoluta garantizada.
## Aplicación vigente0.1.9 — VIS-019

Dirección Confirmada por pedido humano; extensión de composición Propuesta.
Reglas históricas se leen con las sustituciones de decisions.md. Control usa
ritmo de panel24px escritorio/20px móvil y espacios8/12/16/24; editor de una
columna a todo el ancho. Propiedades junto a la selección, debajo de su pista,
sin modal ni columna reservada. Mapa exclusivamente simulado, metros nativos.

Disclosure y select conservan semántica nativa. Solo Chevron SVG cambia de
orientación; texto no se transforma. Transición180ms easeOut, inmediata con
teclado/reduced motion. Sistema/Desarrollador usan Border/Surface/Action del
tema, foco visible, etiquetas humanas y datos ausentes explícitos. No usan la
paleta comercial de Radal. STOP global disponible durante trabajo o parada
incierta y accesible dentro de diálogo inert; no duplica controles de Control.

Código compilado, visual No verificada por denegación de navegador. Revisar
claro/oscuro, viewport móvil/escritorio, teclado y filas de texto largo cuando
se autorice acceso visual. No se ejecutó el scanner/validador de la skill.

