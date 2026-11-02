# Manual operativo — 0.1.11 draft

> Versión vigente 0.1.11 draft: ampliación aditiva desde Infraestructura. Las reglas y entregas anteriores se conservan con su fecha y alcance; leer la precedencia vigente y decisions.md.


<a id="vigencia-0111"></a>
## Vigencia 0.1.11 — ampliación visual desde Infraestructura

**Dirección Confirmada y autorizada por el usuario para incorporar al manual** (AFR2-20261102-14): hacer el sistema más visual con todo el desglose de Infraestructura, de forma **aditiva**, conservando lo que el manual ya incorpora. Continúa `status: draft`; la autorización documental no equivale a aprobación estética en ejecución ni encarga una reforma de la Web.

Este manual reúne ahora dos registros complementarios: las reglas instrumentales y componentes ya existentes, y un lenguaje narrativo para explicar, explorar y representar magnitudes. La paleta carmesí/rubí, el logo vigente, la operación, los formularios, las notificaciones, FloatingModal, los escenarios 3D y las reglas OLED siguen formando parte del sistema.

![Gramática de portada, selección, estados y datos](assets/infraestructura-gramatica.svg)

*Lámina didáctica Propuesta a partir de valores Observados en código. No es una captura de la Web ni una validación visual; la leyenda muestra cuatro familias cromáticas, mientras el mapa real agrupa sus nodos en tres carriles.*

### Mapa de lectura

| Consultar | Contenido vigente incorporado |
| --- | --- |
| [Material y tema](#material-y-tema) | Superficies sólidas, portada oscura, capas, grilla, halos y colores por zona. |
| [Tipo, ritmo y geometría](#tipo-ritmo-y-geometría) | Escala editorial, padding fluido, gaps, radios, separadores y composición. |
| [Movimiento y acceso](#movimiento-y-acceso) | Dos regímenes: operación inmediata y exploración narrativa, con preferencias accesibles. |
| [Atlas completo de Infraestructura](#atlas-infraestructura) | Desglose íntegro de DOM, valores, interacciones, fórmulas y excepciones del código. |
| [Componentes vigentes](components.md#infraestructura-integrada) | Contratos y cobertura integrados en el catálogo general. |
| [Tokens](tokens.json) | Inventario integrado en las capas normales, bajo namespaces `infrastructure`. |
| [Decisiones](decisions.md#vis-057--integración-aditiva-de-infraestructura-02112026) | VIS-057–061, relación con reglas anteriores y límites de adopción. |

### Cómo conviven las reglas

| Contexto | Prioridad y receta | Expresividad permitida |
| --- | --- | --- |
| Operar / control / parámetros | Título, estado válido, acción, datos, feedback y ayuda; acceso inmediato. | Feedback breve, foco y selección; sin loops decorativos ni entradas que retrasen controles o STOP. |
| Explorar / documentación / introducción | Portada, mapa general, selección, detalle, magnitudes y glosario, según la tarea. | Grilla, halos, degradados, conectores y entradas narrativas dentro de sus regiones. |
| Datos / diagnóstico | Valor, unidad, validez y procedencia antes del adorno. | Barras y anillos que representen magnitudes reales o declaradas, sin fingir ejecución. |
| Overlays, formularios y OLED | Se conservan los contratos específicos ya incorporados. | La nueva portada no cambia sus gestos, foco, confirmaciones ni materiales por implicación. |

Las reglas específicas vigentes prevalecen sobre sus antecedentes históricos: VIS-015 para trabajo sólido/chrome glass, VIS-016 y emblema vigente para marca libre, VIS-051–054 para escala/selector/breakpoints y VIS-057–061 para esta ampliación. Los apartados previos se conservan como evolución; la prohibición histórica de todo halo continuo se acota ahora a superficies operativas. La banda zonal de 4 px de una ficha narrativa es un indicador de contexto, distinto de una barra decorativa de marca repetida en todas las tarjetas.

### Cinco reglas protectoras de esta ampliación

1. Mantener la identidad y los contratos existentes; sumar lenguaje visual según contexto, sin sustituirlos por una única portada.
2. Separar la paleta fija de banner y los colores por tema; el color zonal comunica ubicación, no estado físico.
3. Construir jerarquía con títulos, grupos, espacio y superficies; iconos y degradados acompañan al contenido.
4. Usar primitivas compartidas, semántica nativa y feedback de selección inmediato; operación y STOP no esperan una animación editorial.
5. Mostrar significado, unidades y procedencia; las particularidades defectuosas observadas no se convierten en normas.

**Evidencia:** Observada en el código de Infraestructura para valores y mecanismos, Inferida para su gramática general y Propuesta cuando se extiende a una pantalla sin muestra. **Verificación de esta revisión:** No verificada en ejecución. No se ejecutaron tests, builds, validadores, escáneres, navegador ni consultas al robot. Las láminas SVG son documentación nueva sin render de comprobación.

## Plan visual 01-11 — VIS-050..VIS-054, 0.1.10

Aplicación autorizada del borrador para la fase 1 del
[plan visual](../../../AutoFinalRobo2-Web/docs/plan-visual-20261101/PLAN.md)
(AFR2-20261101-07). Detalle y sustituciones en [decisions.md](decisions.md);
valores en `tokens.json` → `visualPlan20261101`.

- **Iconos (VIS-050, fase 2):** Lucide copiado al repo, grilla 24 px, trazo
  1,75 px, `currentColor`, tamaños 16/20/24 px. STOP con `octagon-x`; ningún
  glifo Unicode como icono.
- **Tipo (VIS-051):** familia Segoe UI Variable Text/Display → `system-ui`, sin
  descargas. Niveles `--type-display`, `--type-title` (un `<h1>` por página),
  `--type-heading`, `--type-subheading`, `--type-body`, `--type-label`,
  `--type-caption` y `--type-data`, cada uno con su `--type-*-tracking`. Se usan
  como `font: var(--type-title); letter-spacing: var(--type-title-tracking)`.
- **Espaciado (VIS-051):** `--page-gutter`, `--page-top`, `--section-gap`,
  `--card-padding`, `--stack-gap` (celular → desde 840 px) y `--content-max`
  1200 px solo para texto y formularios.
- **Selector segmentado (VIS-052, fase 3):** pill por `clip-path`, 250 ms con
  `--ease-in-out`; teclado y movimiento reducido instantáneos.
- **Breakpoints (VIS-053):** `min-width` 600/840/1200/1600 px (`--bp-*` y
  `@theme`). Celular con barra inferior y «Más» (fase 5).
- **Atención y colores (VIS-054):** `--attention*` en ambos temas para la
  pulsación larga. Todo color fijo vive en `tokens.css`, salvo las excepciones
  registradas en VIS-054.

Estado: fuente modificada; aspecto, contraste y comportamiento No verificados.

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

### Ampliación vigente — materiales narrativos y color zonal (VIS-058)

Se incorpora una familia de portada oscura **igual en ambos temas**: base `#0b1424`, tinta `#f8fafc`, secundaria `rgba(226,232,240,.82)`, grilla tenue de 32 px y halos radiales. El contenido sigue sobre `surface-solid`/`surface-subtle`; el vidrio se localiza en chrome/overlays y chips de portada. No aplicar blur al texto ni repetirlo dentro de cada celda.

| Zona | Contenido claro | Contenido oscuro | Banner fijo |
| --- | --- | --- | --- |
| Computadora / info | `#0369a1` | `#38bdf8` | `#7dd3fc` |
| ESP / warning | `#b45309` | `#fbbf24` | `#fcd34d` |
| Wi-Fi / action | `#b81d33` | `#ff6b7d` | `#ff8a9a` |
| Cliente / success | `#15803d` | `#4ade80` | `#86efac` |

La familia zonal aporta tinta/fondo suave/borde por herencia. No introduce cuatro marcas ni cambia la diferenciación de STOP bermellón. Mantener acciones y emergencias con la semántica existente; en un recorrido, ámbar/verde pueden identificar lugar sin implicar alarma o éxito del robot.

![Fondos CSS y elementos superpuestos de la portada](assets/infraestructura-capas.svg)

*Esquema Propuesto, no captura: cinco fondos del hero y un glow independiente; sus miniaturas intensifican los halos para explicarlos.*

El fondo combina grilla horizontal/vertical, radial celeste superior derecho, radial rubí inferior izquierdo y base noche. Un div absoluto separado respira con blur de 20 px. Los chips usan backdrop-blur de 8 px; los carriles, tinte al 6% y borde dashed al 45% de su zona de banner. Son valores **Observados de la receta**, no sustituciones generales de las seis densidades glass. Desglose y CSS completos en el atlas, apartados 4–5.

**Confirmada en dirección / Propuesta en escala / Aprobada (VIS-002):** usar vidrio del informe como lenguaje común en todos los paneles normales (§§2, 6, 15). Panel 88% blanco claro / 90% RGB `20,35,59` oscuro (`Observada` en `surface-glass`), blur `20px` y saturación `145%`; chrome `72/76%`, `16px`, `140%`; dense `94/96%`, `20px`, `140%`; field `56/62%`, `12px`, `120%`; popover `94/96%`, `20px`, `140%`; dialog `88/90%`, `24px`, `150%`. Un solo blur por región; tablas y filas comparten material. La transparencia afecta al fondo; texto e iconos conservan opacidad normal. No animar blur en paneles ni colocarlo en todas las capas anidadas.

**Confirmada en exclusión y acento rojo / Observada y Propuesta en tokens / Aprobada (VIS-003, VIS-013):** superficies glass sobre base neutra del informe (§3) combinadas con acento primario carmesí/rubí extraído de `Logo.png`. Claro: fondo `#f6f8fc`, superficie elevada `#ffffff`, superficie sutil `#eef3f8`, texto `#132033`, secundario `#4f6178`, acción primaria carmesí `#b81d33` (hover `#961426`, `onAction` `#ffffff`), foco `#b81d33` y selección `rgba(184,29,51,.10)`. Oscuro: fondo `#0b1525`, superficie elevada `#14233b`, superficie sutil `#1c2f4f`, texto `#ecf2fb`, secundario `#b6c4d9`, acción primaria rubí neón `#ff6b7d` (hover `#ff8594`, `onAction` `#0b1525`), foco `#ff6b7d` y selección `rgba(255,107,125,.14)`. Diferenciación estricta entre identidad carmesí y emergencia (`STOP`/`error`): el estado de error y el botón `STOP` usan rojo bermellón (`#c93b1b` en claro / `#ff8a65` en oscuro) sobre `surfaceSolid`, con borde reforzado e icono explícito de parada/alerta, para que jamás se confundan con un botón primario normal. Tema `light/dark/system`, clave propia `afr2-theme`, suscripción central a `matchMedia` en el provider y `color-scheme` en raíz/`.dark`; Toggle muestra la acción siguiente; cambio inmediato (§4.2). Fondo estático con uno o dos degradados radiales sutiles (§4.3). Fallback sin `backdrop-filter` y `prefers-reduced-transparency`: material sólido (`surfaceSolid`). `prefers-contrast: more` retira blur y refuerza borde/tinta (§10.2, §12).

## Tipo, ritmo y geometría

### Ampliación vigente — jerarquía editorial y ritmo (VIS-057)

El nivel de portada admite `750 clamp(2rem,5.4vw,3.6rem)/1.04` en display y tracking `-.035em`, con segunda línea degradada. Los títulos de sección narrativa y de ficha usan `--type-title`; las páginas operativas conservan PageHeader/SectionHeader y su jerarquía ya definida. No agrandar todos los h2 por esta excepción contextual.

| Rol narrativo | Valor observado | Uso |
| --- | --- | --- |
| Espacio entre secciones | `clamp(28px,4vw,48px)` | Relato de varios bloques. |
| Portada | Padding `clamp(24px,4.5vw,56px)`, gap `clamp(24px,3.5vw,40px)` | Copy, mapa y cierre en una columna. |
| Tarjeta de datos / main de ficha | Padding `clamp(20px,3vw,36px)` | Información dominante. |
| Lateral de ficha | Padding `clamp(20px,3vw,32px)` | Hechos y navegación. |
| Cabecera de bloque | Flex wrap, gap 16 px, margen inferior 24 px | Kicker/título y acción/selector. |
| Radios de receta | 24 px exterior, 18 px grupo, 14 px varias celdas de icono | Jerarquía de cajas; no reemplaza todos los radios generales. |
| Longitud de lectura | Lede 64ch, descripción 68ch | Evitar líneas extensas sin estrechar mapas y tablas. |

Usar `section/article/nav/listas/tabla` para información y `button/Link` para acciones; los divs agrupan y distribuyen. Mantener min-width:0 y minmax(0,1fr) donde los hijos deban encogerse. Separar grupos con espacio, cambios de superficie y bordes de 1 px; reservar dashed para agrupación o suma y la banda zonal de 4 px para la ficha de etapa. El atlas contiene árbol DOM e inventario dimensional completos, apartados 3 y 7.

**Observada y Propuesta / Aprobada (VIS-004):** `Segoe UI Variable, Segoe UI, system-ui, sans-serif`, sin descarga externa; cuerpo `16px/1.5`, ayuda mínima `14px`, títulos `20–24px` peso `700` tracking `-.02em`; cifras tabulares (§5.3). Espacios `4/8/12/16/24/32/48px`. Controles `≥44px`, inputs `≥16px` para evitar zoom móvil (§5.4). Radios control `14px` (`0.875rem`), card `24px` (`1.5rem`), panel `32px` (`2rem`), cápsula `999px` (§5.1). Panel padding `clamp(16px,3vw,32px)`. Mantener dimensiones en hover, foco, carga, error y deshabilitado. Sombras neutrales (`shadowControl`, `shadowCard`, `shadowPanel`, con `shadowPopover` como alias expreso de `shadowPanel`, §5.2) y reflejo superior discreto (`inset 0 1px 0 var(--glass-highlight)`); sin glow ambiental continuo en superficies operativas; las portadas explicativas disponen de la receta contextual VIS-058/060.

## Movimiento y acceso

### Ampliación vigente — movimiento de exploración (VIS-059/060)

La exclusión de entradas editoriales largas sigue protegiendo las páginas operativas. Se añade una receta narrativa, observada en Infraestructura, que puede usar loops de flujo/halo y entradas de magnitudes **sin retener el acceso a controles**. Sus duraciones no pasan a ser las duraciones de formularios, menús, modales o STOP.

| Elemento | Mecanismo y tiempo de referencia | Significado |
| --- | --- | --- |
| Segmentado compartido | Copia activa de icono/texto/fondo recortada con clip-path, 250 ms in-out; teclado/resize instantáneos. | Elegir contenido local. |
| Estaciones | Línea scaleX 520 ms y círculos 260 ms; check anterior, halo actual, número siguiente. | Posición seleccionada, no tarea ejecutada. |
| Ficha | Entrada 340 ms; icono 460 ms; hechos 380 ms y delay 80+i×60 ms. | Orientar el cambio de contexto, sin animación de salida que bloquee. |
| Reveal / magnitudes | Sección 600 ms una vez; pesos 900 ms; compresión 1000 ms. | Presentar comparación, no fingir descarga. |
| Flujo de portada | Dash .7 s, paquetes 2.4 s, icono 3.2 s, halo 9 s por tramo. | Ilustración conceptual; no telemetría en vivo. |
| Anillo LittleFS | Conic-gradient según porcentaje; sin crecimiento temporal declarado. | Ocupación reportada. |

**Norma para futuras aplicaciones:** teclado inmediato; con reduced motion eliminar loops y transformaciones decorativas, presentar progreso/magnitudes de forma inmediata y conservar solo el fade breve permitido por el contrato general. **Estado observado de Infraestructura:** cobertura parcial; quedan tiempos literales y transformaciones descritos en el atlas. Se registra esta diferencia como deuda, no como relajación de accesibilidad. No se corrige la Web en esta entrega.

No copiar el hover sobre una estación done si iguala tinta y fondo, ni la precedencia hover/active de nodos. El hover de una tarjeta editorial no debe convertirse en promesa de clic; una acción nueva necesita botón/enlace y foco. Matriz completa de disparadores, curvas, interrupción y preferencias en atlas 8–17 y catálogo de componentes integrado.

**Observada y Propuesta / Aprobada (VIS-005, VIS-012):** feedback de pulsación inmediato; acción normal al soltar y gesto cancelable. Hover solo con `@media (hover: hover) and (pointer: fine)` (§10.3). Tokens base del informe (§9.1, §12): `motionFast` `150ms`, `motionStandard` `200ms`, `motionSlow` `300ms`; curvas `easeStandard` `cubic-bezier(.2,.75,.25,1)`, `easeOut` / `easeEnter` `cubic-bezier(.23,1,.32,1)`, `easeInOut` `cubic-bezier(.77,0,.175,1)` y `easeDrawer` `cubic-bezier(.32,.72,0,1)`. Pulsación de botón `scale(.97)` en `150ms` con `easeOut` (§6.10, §9.2); rutas `opacity` y `translateY(4px)` en `180ms`, sin espera entre salida y entrada. Teclado (`data-input="keyboard"`) y cambios frecuentes inmediatos (`0ms`). Popover/dropdown de puntero entra en `200ms` y sale en `150ms` desde `opacity:0; translateY(-4px) scale(.97)` con `easeOut`, origen en el ancla y conservación del montaje durante la salida (§6.3, §12). Modal glass entra y sale en `250ms` con `easeEnter` desde `translateY(12px) scale(.96)`, scrim `250ms` y desmontaje diferido `250ms` (§6.5). Drawer móvil entra y sale en `240ms` con `easeDrawer` desde `translateX(-100%)`, scrim `240ms` con blur `8px` (§6.6). Segmentado desplaza indicador en `250ms` con `easeStandard` (§6.8). Acordeón anima `grid-template-rows: 0fr -> 1fr`, opacidad y chevron `180deg` en `200ms` como única excepción funcional de layout documentada (§6.9). Tooltip `150ms` con delay de hover `280ms` y foco inmediato (§6.4). Spinner `800ms` lineal solo durante trabajo real (§8.2). Política única de `prefers-reduced-motion` (§10.1, §12): eliminar traslación, escala, rebote y giros continuos, conservando únicamente un fade de opacidad de `120ms` (`0ms` en teclado), sin aplicar un reset global `.01ms !important` que contradiga los fades locales. Se excluyen en páginas operativas las entradas editoriales de `500–1000ms` (§9.3) y la bienvenida acrílica de `1300ms` (§9.4) para no demorar el acceso a datos o a `STOP`. Resortes solo si existe un gesto arrastrable real (§9.5).

**Confirmada / Aprobada (VIS-006):** acceso directo sin autenticación, Protocol Studio ni OLED Studio. OLED: giro, un SW con corto confirma/largo `800ms` abre menú; largo no genera corto al liberar, Volver solo por opción. Esta regla semántica precede al feedback genérico de botones. Perder foco, conexión o visibilidad cancela el gesto. Identidad/selección recibidas del productor, sin éxito optimista. Encoders de interfaz y rueda son distintos.

## Notificaciones
**Confirmada para librería / Propuesta para receta / Aprobada (VIS-007, VIS-010):** Sonner con un Toaster en raíz, fuera de elementos transformados, tema resuelto por el provider y offset que respete safe areas. Envolver llamadas en servicio `notify`: toast normal/info para información, loading con id estable y actualización al resultado, success solo ante confirmación final y error saneado. Capturar excepciones y rechazos también junto al control; un toast no reemplaza el estado persistente. Sin spam por cada alive/reintento: un id de conexión actualizable (`afr2-connection`), recuperación solo después de pérdida observada. Errores físicos o `STOP` sin confirmación permanecen visibles en la página. Credenciales y payloads sensibles nunca en toast, consola ni storage. Cancelar loading al desconectar. El estado de aceptación de una operación no se anuncia como ejecución completada. Shell `glassPopover` propio mediante `toast.custom` manteniendo posicionamiento, cierre y swipe de Sonner (§8.3); no envolver el toast en otra animación externa que compita con la librería.

## Responsive, contenido, capas y accesibilidad

### Ampliación vigente — reflujo y datos visuales (VIS-057/061)

Se utilizan los cortes ya existentes 600/840/1200 px. En la receta de Infraestructura los carriles se apilan hasta 1200; desde 600 los nodos interiores se ordenan horizontalmente; desde 840 la ficha usa main/lateral y el paquete dos columnas; stats/glosario pasan 1→2→4 columnas. El hero sigue una columna. Son decisiones de esta receta, no una obligación de cuatro tarjetas por fila en cualquier pantalla.

Las cifras mantienen unidad, números tabulares y procedencia. GLB, gzip, bytes del filesystem y memoria de ejecución son magnitudes distintas. No convertir el estado visual de estaciones en verificación física. El anillo decorativo se acompaña de progress accesible; la tabla conserva caption/scope y scroll local; las opciones tienen roving focus. Conservar contratos ya definidos para unknown/stale, error inline y acciones de recuperación. Detalle de dimensiones, fórmulas y estados en atlas 12–18.

**Propuesta / Aprobada (VIS-008, VIS-012):** `100dvh`, `viewport-fit=cover`, `safe-area-inset`, zoom permitido; contenido seleccionable y botones sin selección. Bajo `820px` reflujo a columna, encabezados con wrap, navegación accesible y escena con alto acotado. Escala de capas `z-index` (§11): contenido `0`, navegación fija `20`, popover contextual `40`, scrim de drawer `50`, drawer `60`, scrim modal `80`, modal `90`, toast/tooltip contextual `100`, actividad global `110`. Respetar contextos de apilamiento creados por `backdrop-filter`, `transform` o `isolation`, y el `top layer` nativo cuando se use `<dialog>`. Labels persistentes, foco visible, icono acompañado de texto y live region sin repetir telemetría. Contenido cerrado o cubierto por un overlay bloqueante queda fuera del recorrido de teclado (`inert` o desmontado tras salida, §6.9, §8.1, §10.3). Distinción de carga por alcance (§8.1): acción puntual → spinner en el botón; carga de dependencias de una región/formulario → overlay local `absolute inset-0` con `glass` (`blur 12px`), radio heredado y región `aria-busy`; proceso global → solo cuando impida realmente toda la aplicación. Voz española directa: acción, requisito y resultado. `Unknown` muestra `—`, `stale` muestra dato con antigüedad; no animar cifras para decorar. Sin data fake en producto. Errores 3D muestran reintento sin bloquear el resto de la aplicación.

## Precisiones de validación y cierre documental (`0.1.1` → `0.1.3`)

**VIS-009 / Propuesta / Aprobada:** `success` claro pasa de `#0f8c53` (`Observada` en §3.3, contraste `4,18:1`) a `#087a46` (`≥ 4,5:1` en todas las superficies declaradas). `borderStrong` pasa de `.28`/`.20` (§3.2) a `rgba(19,32,51,.55)` en claro y `rgba(255,255,255,.50)` en oscuro para superar `3:1` frente a todos los materiales. Opacidad visual de deshabilitado `1`: usar texto secundario, atributo `disabled` y motivo, sin atenuar todo el contenedor. No permitir imágenes, WebGL ni colores ajenos directamente bajo texto glass: encapsular el visor 3D y OLED en su propia región, y usar `surfaceSolid` o `glassDense` si cambia el fondo. Borde fuerte identifica campos y controles; borde sutil solo delimita panel decorativo. El anillo de foco es de `2px`, separado `2px` con `surfaceSolid`, para conservar definición.

**VIS-010 / Propuesta / Aprobada:** Sonner `2.0.8` de la base, un Toaster. Posición `top-right` en escritorio y `top-center` en móvil para no tapar `STOP` en el borde inferior (sustituyendo la ubicación inferior derecha observada en §8.3 del origen comercial), `visibleToasts` `3`, duración `4000ms` y offsets `24/16px` más safe area. En resize solo cambia posición, nunca se monta otro Toaster. La shell headless implementa por sí misma botón `Cerrar` con nombre accesible y `toast.dismiss(id)`. Mensajes rutinarios `role="status"`; fallo urgente `role="alert"`; evitar anuncio duplicado en mensaje inline y toast. `STOP` siempre disponible para enviar, pero no se anuncia detenido sin estado confirmado.

**VIS-011 / Propuesta / Aprobada:** Navegación Web usa enlaces de hash y selección de ruta, sin roles `menu` innecesarios. Tabs locales sin cambio de URL usan tabs ARIA y flechas/Home/End. Menú de acciones usa primitiva accesible con teclado; no agrega navegación atrás al OLED. Menú principal OLED y `Volver` son semántica del firmware, no el menú contextual Web. Los enlaces se activan con `Enter`; `Space` activa botones y conserva desplazamiento nativo sobre enlaces. Foco y atajos nunca interceptan escritura en campos.

**VIS-012 / Observada y Propuesta / Aprobada:** Cotejo íntegro con `D:/ProyectoRobotica2/informe-portabilidad-glassmorphism.md` (§§1–18). Se incorporan las curvas, tiempos, matriz de movimiento, `surfaceSubtle`, escala `z-index` y reglas de overlays de las secciones §§6–15 antes no disponibles.

**VIS-013 / Confirmada y Propuesta / Aprobada:** Adopción de `D:/ProyectoRobotica2/assets/Logo.png` y acento carmesí/rubí (`#b81d33`/`#961426` claro, `#ff6b7d`/`#ff8594` oscuro) sobre vidrio neutro, diferenciando `STOP`/`error` con rojo bermellón (`#c93b1b` claro / `#ff8a65` oscuro), contenedor `surfaceSolid`, borde e icono explícito, y componente `BrandMark` con medallón `#000000` y fallback de monograma `AFR2`.

## Gobernanza

### Ampliación vigente — precedencia y conservación (VIS-057–061)

La versión documental actual es 0.1.11 draft. Se conservan reglas, componentes e historia anteriores, añadiendo recetas por contexto y el atlas íntegro dentro de este manual. El dossier `infraestructura/` pasa a ser evidencia de origen; su antiguo límite de no integración queda superado por el pedido actual exclusivamente para documentación.

`tokens.json` mantiene las capas existentes e incorpora `primitives.infrastructure`, `semantic.light.infrastructure`, `semantic.dark.infrastructure`, `sections.infrastructure`, `components.infrastructure` y `motion.infrastructure`. Las rutas abreviadas del atlas se interpretan dentro de esos namespaces. Los valores añadidos no se importan ni sustituyen automáticamente variables CSS; unificar valores generales o crear primitivas nuevas requiere un trabajo de implementación posterior.

Las mediciones y validaciones históricas conservan su fecha y alcance; no cubren 0.1.11. La ampliación está autorizada en dirección documental, sus valores son Observados y su ejecución No verificada. Las particularidades de fuente y componentes no vistos en Infraestructura no desplazan los contratos ya existentes de formularios, modales, menús, notificaciones ni OLED.

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



<a id="atlas-infraestructura"></a>
## Atlas integrado — Infraestructura en detalle

Se incorpora a continuación **el desglose completo** de la extracción AFR2-20261102-11, no solo un enlace al dossier. Valores y comportamientos describen la fuente; las reglas de adopción vigentes son VIS-057–061. Los IDs INF-VIS se conservan como procedencia y se relacionan en decisions.md. S01–S23 están incorporados a sources.md. Las referencias de tokens de componente/sección/movimiento se leen bajo el namespace `infrastructure` del tokens.json general.

### 1. Cómo interpretar este documento

La sección elegida por el usuario es la fuente canónica de este atlas, ahora incorporado al manual general. Se describe **lo implementado en los archivos de trabajo**, no un rediseño. Salvo indicación expresa, los valores y comportamientos llevan evidencia **Observada**, aprobación **Pendiente** y ejecución **No verificada**. `S01`–`S23` remiten a [sources.md](sources.md). Las conclusiones de intención se etiquetan **Inferida** y las extensiones futuras **Propuesta**.

Las equivalencias rem→px asumen el tamaño raíz declarado de 16 px; los valores CSS originales se conservan. No son medidas tomadas del navegador.

### 2. Esencia y gramática visual

**Inferida, INF-VIS-001.** La personalidad es técnica, explicativa y expresiva: una portada de laboratorio digital introduce el recorrido y después las tarjetas permiten leerlo con calma. La sensación de profundidad viene de fondos superpuestos, bordes translúcidos, halos y pequeñas variaciones de elevación. La jerarquía se construye con tipografía, espacio y grupos reconocibles.

Hay tres niveles de protagonismo:

1. **Relato principal:** h1 grande, segunda línea con degradado, mapa del viaje y frase de cierre del banner.
2. **Exploración:** selector de recorrido, estaciones y una ficha de etapa visible.
3. **Evidencia y consulta:** pesos, paquete, almacenamiento, tabla y glosario.

La portada admite movimiento continuo; la información inferior usa sobre todo entradas y respuesta a acciones. El contenedor exterior de las tarjetas es sólido; la translucidez y el blur aparecen de forma localizada en chips y chrome global. No toda superficie está hecha con vidrio.

**Observada.** El orden real es portada → recorrido → pesos 3D → paquete/almacenamiento → glosario. La interfaz no intercala paneles de otras páginas. **Inferida:** conservar ese orden permite pasar de explicación general a paso concreto y luego a magnitudes.

### 3. Árbol HTML y función de los divs

Fuente: S01:148–323. Esquema de la composición real; se omiten textos repetidos.

```text
div.afr2-route-view.afr2-page-stack.afr2-infra
├─ section.afr2-infra-hero [aria-labelledby=infra-title]
│  ├─ div.afr2-infra-hero__glow [decoración]
│  ├─ div.afr2-infra-hero__copy
│  │  ├─ p.eyebrow → span «Explorar» + texto «Infraestructura»
│  │  ├─ h1 → primera línea + br + span con texto degradado
│  │  ├─ p.lede
│  │  └─ ul.chips → li × 3, cada uno con UiIcon
│  ├─ div.afr2-infra-flow [role=group]
│  │  ├─ div.lane [data-zone=pc]
│  │  │  ├─ span.lane__label → icono + nombre de zona
│  │  │  └─ div.lane__body
│  │  │     ├─ div.flow__origins → button React/Vite + button Fusion/Blender
│  │  │     ├─ span.wire → i + i
│  │  │     └─ button Paquete gzip
│  │  ├─ span.wire → i + i + em «.bin»
│  │  ├─ div.lane [data-zone=esp] → etiqueta + nodos LittleFS/HTTP + wire
│  │  ├─ span.wire → i + i + em «gzip»
│  │  └─ div.lane [data-zone=client] → etiqueta + nodo navegador
│  └─ p.takeaway → UiIcon + span → strong + explicación
├─ section.reveal.journey
│  ├─ span.anchor [ancla de scroll]
│  ├─ div.heading → div(kicker + h2) + SegmentedControl
│  ├─ nav.metro → ol → li × 8 → button → dot + label(strong + small)
│  └─ article.stage [key=journey-selected, data-zone]
│     ├─ div.stage__main
│     │  ├─ div.stage__top → span.zone-tag + span.counter
│     │  ├─ div.stage__title → span.icon + h3
│     │  ├─ p.description
│     │  └─ div.transform → div(Entra) + span.wire + div(Sale)
│     └─ div.stage__side
│        ├─ h4
│        ├─ ol.facts → li con número generado por CSS
│        └─ div.stage__nav → Button Anterior + Button Siguiente
├─ section.reveal.card.weights
│  ├─ div.heading → kicker/h2 + Link
│  ├─ ul.bars → auto + brazo + cubo + suma apilada
│  └─ p.help
├─ section.reveal.card.package [aria-busy]
│  ├─ div.heading → kicker/h2 + Button Actualizar
│  ├─ p de carga / div.empty de error [condicionales]
│  ├─ div.package-origin [si hay manifiesto]
│  ├─ div.package-grid
│  │  ├─ div.compress [si hay manifiesto]
│  │  └─ div.storage [siempre] → div.ring + div de texto/acción
│  ├─ div.stats → div.stat × 4 [si hay manifiesto]
│  ├─ div.file-table → table [si hay manifiesto]
│  └─ párrafos de ayuda/procedencia
└─ section.reveal.concepts
   ├─ div.heading → kicker + h2
   └─ div.glossary → article.concept × 4 → icono + h3 + p
```

**Función de cada envoltorio:** los divs de layout agrupan celdas y distribuyen contenido; los elementos semánticos (`section`, `article`, `nav`, listas, tabla) identifican la información y la navegación. Los `span` alojan iconos, etiquetas y decoración en línea. Los nodos accionables son `button`, no divs con un listener.

`Reveal` emite un `section` por defecto; también admite `div`, pero aquí las cuatro secciones posteriores al hero usan `section`. El propio `Reveal` guarda un ref y añade `data-visible`; no agrega una caja interior. `Wire` emite un `span` con dos `i` decorativos y un `em` opcional; los paquetes luminosos no son iconos SVG ni imágenes.

**Consecuencia de composición:** muchos elementos llevan `min-width:0` y columnas `minmax(0,1fr)` para permitir encogimiento dentro de grids/flex. Los contenedores recortan decoración con `overflow:hidden`; las tablas y estaciones, en cambio, habilitan scroll horizontal.

### 4. Paleta por tema y por función

#### 4.1 Superficies y texto del contenido

Fuente: S12:18–22, 67–85 y 193–247. Los bordes son transparencias compuestas sobre el fondo, no colores sólidos equivalentes.

| Rol CSS | Claro | Oscuro | Uso en Infraestructura |
| --- | --- | --- | --- |
| `--canvas` | `#f3f6fb` | `#0b1220` | Fondo de la aplicación. |
| `--surface-solid` | `#ffffff` | `#14233b` | Tarjetas, ficha, centros de estaciones y celdas Entra/Sale. |
| `--surface-subtle` | `#eaf0f8` | `#182946` | Panel lateral, ranura del selector, tracks y grupos de datos. |
| `--text-strong` | `#0f172a` | `#f8fafc` | Títulos y cifras dominantes. |
| `--text` | `#1e293b` | `#e2e8f0` | Etiquetas y texto de jerarquía intermedia. |
| `--text-muted` | `#475569` | `#94a3b8` | Descripciones y ayuda. |
| `--text-subtle` | `#64748b` | `#94a3b8` | Contadores, leyendas y metadatos. |
| `--border-subtle` | `rgba(15,23,42,.09)` | `rgba(226,232,240,.09)` | Separadores y subceldas. |
| `--border-default` | `rgba(15,23,42,.16)` | `rgba(226,232,240,.16)` | Tarjetas, tabla y línea base. |
| `--border-strong` | `rgba(15,23,42,.28)` | `rgba(226,232,240,.28)` | Estaciones futuras y vacío. |
| `--text-on-action` | `#ffffff` | `#190408` | Texto del botón primario. |
| `--focus` | `#b81d33` | `#ff6b7d` | Foco general. |

En oscuro, `text-muted` y `text-subtle` tienen el mismo HEX: la diferencia de jerarquía depende también del tamaño, peso y posición. No inventar una diferencia de color inexistente.

#### 4.2 El color acompaña la ubicación

Fuente: S02:15–18; S01:20–26; S12 familias semánticas.

| `data-zone` | Significado | Alias | Claro | Oscuro | Color fijo en banner |
| --- | --- | --- | --- | --- | --- |
| `pc` | Computadora | `info` | `#0369a1` | `#38bdf8` | `#7dd3fc` |
| `esp` | ESP8266 | `warning` | `#b45309` | `#fbbf24` | `#fcd34d` |
| `link` | Transferencia por Wi-Fi | `action` | `#b81d33` | `#ff6b7d` | `#ff8a9a` |
| `client` | Celular/computadora cliente | `success` | `#15803d` | `#4ade80` | `#86efac` |

Cada zona asigna `--zone`, `--zone-subtle`, `--zone-border` y `--zone-hero`. Los descendientes heredan la familia y no necesitan recibir un color individual como prop.

| Familia | Fondo suave claro / borde claro | Fondo suave oscuro / borde oscuro |
| --- | --- | --- |
| info | `rgba(3,105,161,.11)` / `.28` | `rgba(56,189,248,.14)` / `.34` |
| warning | `rgba(180,83,9,.13)` / `.32` | `rgba(251,191,36,.15)` / `.36` |
| action | `rgba(184,29,51,.11)` / `.30` | `rgba(255,107,125,.14)` / `.36` |
| success | `rgba(21,128,61,.12)` / `.30` | `rgba(74,222,128,.14)` / `.34` |

La columna de borde conserva el mismo RGB de su familia con el alfa indicado. El uso ámbar en una etapa del ESP expresa ubicación: no indica por sí solo un problema. Las estaciones completadas tampoco representan tareas ejecutadas en hardware; describen su posición respecto de la etapa elegida.

#### 4.3 Paleta fija de portada

Fuente: S12:471–493; igual en claro y oscuro.

| Token | Valor |
| --- | --- |
| `--banner-bg` | `#0b1424` |
| `--banner-ink` | `#f8fafc` |
| `--banner-muted` | `rgba(226,232,240,.82)` |
| `--banner-accent` | `#ff6b7d` |
| `--banner-accent-subtle` | `rgba(255,107,125,.22)` |
| `--banner-accent-border` | `rgba(255,107,125,.55)` |
| `--banner-border` | `rgba(148,163,184,.18)` |
| `--banner-chip-bg` | `rgba(11,20,36,.62)` |
| `--banner-chip-border` | `rgba(226,232,240,.18)` |

`banner-scrim` y `banner-scrim-mid` existen globalmente, pero esta portada no los consume: no hay un overlay scrim de esos tokens en su DOM. El efecto de overlay aquí nace de los fondos, el glow, la translucidez de celdas y las sombras.

`color-mix(in srgb, color N%, transparent)` se usa para graduar intensidad. Mezclar con transparente produce un tinte translúcido, no una versión aclarada con blanco. Se conservan las expresiones sRGB del código; no se reemplazan por aproximaciones OKLCH ni HEX que oculten su composición.

### 5. Banner inicial: reconstrucción capa por capa

Fuente: S02:21–43, S01:149–193. **Observada, INF-VIS-003.** No hay una imagen de fondo ni un canvas. El componente construye el efecto con múltiples fondos CSS y cajas HTML.

#### 5.1 Caja exterior

`position:relative`, `isolation:isolate`, `overflow:hidden`, `display:grid`. Radio de 24 px, borde de 1 px `banner-border`, padding `clamp(24px,4.5vw,56px)` y separación interna `clamp(24px,3.5vw,40px)`. La sombra exterior es `0 24px 60px rgba(2,6,23,.28)`.

`relative` contiene los absolutos. `isolate` crea un contexto de apilamiento local para el glow con z-index negativo. `overflow:hidden` recorta halos y formas por el radio de la portada. No implica un desenfoque del texto.

#### 5.2 Fondos del hero, de delante hacia atrás

```css
background:
  linear-gradient(rgba(148,163,184,.06) 1px, transparent 1px)
    0 0 / 32px 32px,
  linear-gradient(90deg, rgba(148,163,184,.06) 1px, transparent 1px)
    0 0 / 32px 32px,
  radial-gradient(120% 90% at 100% 0%,
    rgba(56,189,248,.16), transparent 55%),
  radial-gradient(90% 80% at 0% 100%,
    rgba(255,107,125,.18), transparent 60%),
  var(--banner-bg);
```

| Capa | Qué dibuja | Por qué se percibe así |
| --- | --- | --- |
| 1 | Líneas horizontales de 1 px cada 32 px | Grilla muy tenue, alfa .06. |
| 2 | Líneas verticales de 1 px cada 32 px | Completa la cuadrícula técnica sin un asset. |
| 3 | Halo celeste en esquina superior derecha | Elipse 120% × 90%; se disuelve al 55%. |
| 4 | Halo rubí en esquina inferior izquierda | Elipse 90% × 80%; se disuelve al 60%. |
| 5 | Base azul noche | Mantiene el ambiente oscuro en ambos temas. |

Las primeras capas del `background` se pintan por encima de las siguientes. Ningún degradado de esta lista tiene una animación propia de posiciones o colores.

#### 5.3 Glow independiente

Un único `div.afr2-infra-hero__glow`, `aria-hidden`, absoluto y `z-index:-1`, añade otro halo rubí. Usa `inset:auto -10% -40% 30%`, `height:70%`, radio 50%, `radial-gradient(closest-side,rgba(255,107,125,.22),transparent)` y `filter:blur(20px)`.

Su animación dura **9 s por tramo**, `ease-in-out`, infinita y alterna: `translate3d(0,0,0) scale(1), opacity:.8` → `translate3d(-8%,-6%,0) scale(1.12), opacity:1`. La ida y vuelta completa ocupa 18 s. Lo que respira es la caja del halo, no el gradiente tipográfico ni la grilla.

#### 5.4 Copy, título y chips

La caja de texto es grid con gap 16 px y máximo 760 px. El eyebrow reúne una pill «Explorar» con padding 4×10 px y el texto «Infraestructura», separados 10 px. La pill lleva borde/fondo rubí translúcido; el resto usa tinta secundaria de banner.

El h1 tiene una primera línea clara y segunda línea dentro de un `span`. Esa segunda línea lleva `linear-gradient(92deg,pc,link 55%,esp)`, `background-clip:text`, prefijo WebKit y `color:transparent`. Es una máscara de texto sobre un fondo degradado. El `<br>` fuerza el salto; `text-wrap:balance` además equilibra las líneas disponibles.

Los chips son una lista flex con wrap, gap 8 px y margen superior 4 px. Cada chip: padding 8×14 px, radio pill, borde translúcido, fondo `banner-chip-bg`, `backdrop-filter:blur(8px)`. El icono mide 16 px, el espacio icono/texto 8 px y el color del icono es `banner-accent`. Aquí el blur actúa sobre lo que queda detrás del chip; el glow usa `filter`, que desenfoca el propio elemento.

#### 5.5 Tres carriles y sus overlays

Cada carril es una caja grid relativa con gap 12 px, padding **30px 12px 12px**, radio 18 px, borde dashed de 1 px al **45%** del color de su zona de banner y fondo al **6%**. El padding superior reserva lugar a la etiqueta absoluta: top 9 px, left 14 px, icono 14 px, gap 6 px, texto de 11 px/650, uppercase y tracking .06em.

Los nodos añaden otra capa: fondo `rgba(15,23,42,.55)`, borde de su zona al 28%, brillo superior inset y sombra exterior. Su icono está dentro de una celda de 44×44 px, radio 14 px, tinte al 14%, contorno luminoso y halo. La caja de icono pulsa; el trazado SVG no gira ni cambia de forma.

El banner tiene **tres carriles** (pc, esp, client). La zona link aparece como color de transmisión y como etapa HTTP del recorrido inferior; no existe un cuarto carril Wi-Fi separado.

#### 5.6 Frase de cierre

`p.takeaway` es flex: gap 12 px, padding 16×18 px, radio 18 px, borde del banner y fondo blanco al 4%. Icono de pantalla de 18 px en verde cliente, alineado arriba con margen 2 px. El texto usa `banner-muted`; la frase inicial en `strong` pasa a `banner-ink`. Refuerza el mensaje sin añadir un botón.

### 6. Tipografía y jerarquía exacta

Fuente: S12:118–120, 384–405; S02 reglas específicas. Se usan familias de sistema. La presencia del nombre de una fuente no demuestra que esté instalada en todos los dispositivos.

| Familia | Stack principal | Función |
| --- | --- | --- |
| `--font-display` | Segoe UI Variable Display → Segoe UI Variable → Segoe UI → system-ui | Titulares y cifras destacadas. |
| `--font-sans` | Segoe UI Variable Text → Segoe UI Variable → Segoe UI → system-ui | Descripciones, etiquetas y controles. |
| `--font-mono` | JetBrains Mono → ui-monospace → SFMono-Regular → Menlo → Monaco → Consolas | Contador de etapa y etiquetas de cables. |

Los stacks display/sans continúan con `-apple-system`, BlinkMacSystemFont, Roboto y sans-serif. No se observó `@font-face` necesario para esta sección.

| Elemento | Tamaño | Peso / interlineado | Tracking / tratamiento |
| --- | --- | --- | --- |
| H1 portada | `clamp(2rem,5.4vw,3.6rem)` ≈ 32–57.6 px | 750 / 1.04 | -.035em, balance, margen 0. |
| Segunda línea h1 | Heredado | Heredado | Texto con degradado recortado. |
| Lede | `clamp(1rem,1.4vw,1.125rem)` ≈ 16–18 px | 400 / 1.6 | Máximo 64ch, margen 0. |
| H2 de secciones | `clamp(1.5rem,2.6vw,1.875rem)` ≈ 24–30 px | 700 / 1.2 | -.02em, margen 0. |
| H3 ficha de etapa | Mismo `--type-title` que h2 | 700 / 1.2 | -.02em; misma importancia visual dentro de su contexto. |
| Kicker inferior | `.75rem` = 12 px | Hereda peso, normalmente 400 / 1.5 | Muted; margen 0 0 6 px; no uppercase impuesto. |
| Texto descriptivo de etapa | `1.0625rem` = 17 px | 400 / 1.65 | Máximo 68ch. |
| H4 panel lateral | `.8125rem` = 13 px | 650 / 1.3 | .01em. |
| Label estándar | `.8125rem` = 13 px | 550 / 1.3 | Token label; tracking separado donde se declara. |
| Caption estándar | `.75rem` = 12 px | 450 / 1.4 | Token caption. |
| Título de nodo | 13 px | 650 / 1.25 | Centrado. |
| Etiqueta de zona carril | 11 px | 650 / 1 | Uppercase, .06em. |
| Insignia de zona en ficha | 12 px | 650 / 1.2 | Icono 14 px. |
| Contador `01 / 08` | 13 px | 650 / 1, mono | Tabular, no se encoge. |
| Nombre estación | 12 px | 600 / 1.3 | Máximo 132 px de ancho. |
| Subtítulo estación | 11 px | 450 / 1.4 | Subtle. |
| Entra / Sale | 11 px | 650 / 1 | Uppercase, .06em. |
| Valor Entra / Sale | 15 px | 650 / 1.35 | `overflow-wrap:anywhere`. |
| Bytes al final de barra 3D | `clamp(1.125rem,2vw,1.5rem)` ≈ 18–24 px | 700 / 1, display | -.02em, números tabulares. |
| Porcentaje de reducción | `clamp(1.5rem,3vw,2rem)` ≈ 24–32 px | 750 / 1, display | -.03em, action. |
| Valor de estadística | `clamp(1.25rem,2.4vw,1.75rem)` ≈ 20–28 px | 750 / 1.1, display | -.02em, tabular. |
| Centro del anillo | 1.25rem = 20 px | 750 / 1, display | Tabular. |
| H3 glosario | 1.125rem = 18 px | 650 / 1.3, display | Margen 6 px 0 0. |
| Párrafo glosario | Hereda 16 px del body | Hereda peso / 1.6 | Muted, margen 0. |
| Ayuda `.afr2-help` | 15 px | Hereda peso / 1.7 | Base margen 8px 0 16px, con overrides locales. |

Los párrafos de glosario y el takeaway no reciben automáticamente `--type-body`: pueden usar 16 px heredados. El cuerpo de etapa sí declara font completo y después cambia tamaño/line-height. Esta diferencia existe en fuente y no debe uniformarse al describirla.

### 7. Layout, márgenes, padding y separadores

#### 7.1 Caja de página y ritmo

S14:33–34 y 177; S12:411–426; S02:6–9. El main ocupa el ancho disponible sin max-width global. La regla posterior de workspace sustituye el padding antiguo: **20px 16px 40px** en base y **32px 32px 40px** desde 840 px. En escritorio el contenido queda a la derecha de la sidebar con margen `sidebar-width + 32px`; debajo de 840 px no hay ese margen y se reserva espacio para la navegación inferior.

La raíz de Infraestructura es flex column y sustituye el gap genérico por `clamp(28px,4vw,48px)`. Por tanto, el ritmo propio de esta página no es siempre `--section-gap` ni siempre 32 px. Los gaps que usan vw responden al viewport, no al ancho libre que deja la sidebar.

La cabecera de sección es flex, `align-items:flex-end`, `justify-content:space-between`, wrap y gap 16 px. Su margen inferior es 24 px; dentro del recorrido se pone a 0 porque el grid ya separa los bloques 24 px; en glosario se usa 20 px.

#### 7.2 Inventario dimensional

| Caja / selector | Padding | Gap / margen | Radio / detalle |
| --- | --- | --- | --- |
| Hero | clamp(24,4.5vw,56) px | clamp(24,3.5vw,40) px | 24 px, borde 1 px. |
| Hero copy | 0 | 16 px; max 760 px | Texto a la izquierda. |
| Chips | 8×14 px por li | 8 px entre chips; mt 4 px lista | Pill. |
| Takeaway | 16×18 px | Gap 12 px | 18 px. |
| Carril | 30px 12px 12px | Gap 12 px | 18 px, dashed. |
| Orígenes del mapa | 0 | Gap 8 px | Dos columnas en base, una desde 600. |
| Nodo mapa | 16×12 px | Gap 6 px | 18 px, min-height 112 px. |
| Cable vertical | 0 | Margen 4px 0 | 2×34 px. |
| Cable horizontal entre nodos/carriles | 0 | Margen 0 4px | 44×2 px. |
| Recorrido | 0 | Gap 24 px | Grid, ancla top -96 px. |
| Metro | 6px 2px 4px | Grid gap 4 px | Scroll x, scrollbar oculta. |
| Botón estación | 0 2px 6px | Gap 10 px | Fondo transparente. |
| Ficha exterior | 0 | Gap 0 | 24 px, overflow hidden. |
| Ficha principal | clamp(20,3vw,36) px | Gap 20 px | Halo de zona al fondo. |
| Ficha lateral | clamp(20,3vw,32) px | Gap 16 px | Surface subtle. |
| Top de ficha | 0 | Gap 12 px | Space-between. |
| Fila título ficha | 0 | Gap 16 px | Icono 60×60 px. |
| Insignia zona | 6×12 px | Gap 7 px | Pill. |
| Grupo Entra/Sale | 16 px | Gap 0 | 18 px. |
| Celdas Entra/Sale | 14×16 px | Gap 6 px | 12 px. |
| Lista de hechos | 0 | Gap 10 px | Sin bullets nativos. |
| Hecho individual | 14px 14px 14px 48px | Delay por índice | 14 px. |
| Botonera ficha | Top 4 px | Gap 8 px | Space-between. |
| Tarjeta pesos/paquete | clamp(20,3vw,36) px | Cabecera mb 24 px | 24 px. |
| Lista pesos | 0 | Gap 18 px | Sin bullets. |
| Fila peso | 0 | Gap 8px 16px | Nombre/valor arriba, track abajo. |
| Fila suma | Top 18 px | Borde superior dashed | Track apilado. |
| Origen paquete | 0 | Gap 8 px; mb 20 px | Párrafos margen 0. |
| Grid compresión/FS | 0 | Gap 16 px; mb 16 px | 1 o 2 columnas. |
| Panel compresión | 20 px | Gap 10 px | 18 px. |
| Almacenamiento | 20 px | Gap 20 px | 18 px; texto flex-basis 220 px. |
| Estadísticas | 18 px por tarjeta | Gap 12 px entre tarjetas | 18 px. |
| Tabla | Celdas/caption 14×16 px | Wrapper mt 20 px | 18 px. |
| Mini barras de archivo | 0 | Gap 3 px; mb 5 px | Ancho 100 px, alto de barra 4 px. |
| Vacío/error | 20 px | Gap 16 px | 18 px, dashed. |
| Glosario | 24 px por tarjeta | Grid gap 14 px; interno 10 px | 24 px. |

#### 7.3 Separadores

No se usan elementos `<hr>` para la estructura descrita. La separación se consigue con:

- Espacio entre bloques y cambios de superficie.
- Bordes exteriores de 1 px y radios de tarjeta.
- Borde superior del panel lateral en móvil; pasa a borde izquierdo desde 840 px.
- Banda absoluta de 4 px arriba de la ficha: `linear-gradient(90deg,zone,transparent 85%)`.
- Borde dashed de 1 px y padding 18 px sobre la suma de pesos.
- Bordes inferiores sutiles entre filas de tabla, retirados en la última fila.
- Grilla del hero y cables discontinuos como separadores narrativos, no como divisiones de datos.

La ficha principal añade un fondo `radial-gradient(120% 80% at 0% 0%,zone-subtle,transparent 60%)`. El panel lateral tiene `surface-subtle`; las celdas numeradas vuelven a `surface-solid`. Este cambio de densidad distingue explicación principal y detalles.

### 8. Selector con iconos: mecanismo completo

Fuente: S05 completo y S06. **Observada, INF-VIS-006.** Infraestructura usa `SegmentedControl` con dos opciones: `programs / Viaje de la Web` y `cube / Viaje del modelo 3D`. Pasa `fill="never"`; conserva `variant="choice"`, `size="md"`, `activation="auto"` y `reselect=false` por defecto.

#### 8.1 Geometría y material

El root es una ranura flex relativa con `isolation:isolate`, ancho fit-content, max-width 100%, padding 4 px, radio pill y fondo `surface-subtle`. Los items tienen mínimo 44 px de alto, padding horizontal 16 px, texto sin wrap y radio pill. Con padding externo la altura nominal mínima del conjunto es 52 px. Iconos de 16 px, gap 6 px con el texto.

La ranura usa sombra inset para parecer hundida:

| Tema | Ranura | Pill |
| --- | --- | --- |
| Claro | `inset 0 0 0 1px border-subtle, inset 0 1px 2px rgba(15,23,42,.06)` | Fondo blanco `surface-solid`; drop-shadow 0 0 .5 px al .32 y 0 1px 2px al .10. |
| Oscuro | Mismo borde, segundo inset `rgba(2,6,23,.40)` | Fondo propio `#2a4068`; drop-shadow claro .34 en contorno y oscuro .45 debajo. |

La pill oscura es más clara que la tarjeta y la ranura. No basta con usar `surface-solid`, que en este tema es más oscuro que `surface-subtle`.

#### 8.2 Dos filas superpuestas

```text
div.afr2-seg
├─ div.afr2-seg__row [radiogroup]
│  ├─ button.afr2-seg__item [radio] → icono + texto
│  └─ button.afr2-seg__item [radio] → icono + texto
└─ div.afr2-seg__glow [aria-hidden, pointer-events:none]
   └─ div.afr2-seg__row.afr2-seg__active [clip-path]
      ├─ span.afr2-seg__ghost → mismo icono + texto
      └─ span.afr2-seg__ghost → mismo icono + texto
```

La fila inferior lleva los botones reales en muted. La superior copia toda la fila con fondo de pill y tinta strong; se muestra solo la franja de la opción seleccionada. No se traslada una caja suelta vacía por debajo de los textos: se anima la ventana visible de una copia del contenido.

Al medir `getBoundingClientRect` de botón y copia, calcula left/right y escribe directamente:

```css
clip-path: inset(0 <right>px 0 <left>px round 9999px);
transition: clip-path 250ms cubic-bezier(.77,0,.175,1);
```

El fondo, el texto y el SVG pasan juntos por el recorte. La sombra se aplica con `filter:drop-shadow()` al wrapper externo para seguir la silueta recortada; un box-shadow colocado dentro del recorte quedaría cortado.

#### 8.3 Activación y estados

| Estado / acción | Implementación |
| --- | --- |
| Reposo no elegido | Botón transparente, texto muted. |
| Hover con puntero fino | Texto pasa a `text`; transición de color 140 ms con `ease` estándar CSS. |
| Elegido | La copia visible muestra pill y texto/icono strong. No usa rubí como fondo activo. |
| Pulsación | Label del botón real escala .97 durante 150 ms ease-out, solo modalidad puntero. La copia ghost no recibe esa transformación: no afirmar sin render que el texto activo se achica igual. |
| Primer montaje | Recorte colocado sin transición; se habilita después de un requestAnimationFrame. |
| Resize | ResizeObserver recoloca sin animar; omite su primera notificación. |
| Teclado | Flechas eligen con auto-activation; Home/End llevan a extremos; foco roving. Recorte instantáneo. Las flechas ciclan entre opciones habilitadas. |
| Repetir opción | No cambia ni vuelve a ejecutar onChange con `reselect=false`. |
| Disabled | Admitido por la primitiva, texto disabled y cursor not-allowed; ninguna de estas dos opciones lo declara. |

`pending + useTransition` deja que la pill represente la elección inmediatamente mientras React actualiza el contenido. Una vez finalizada la transición domina de nuevo el `value` aceptado. Cambiar de recorrido llama `chooseJourney`, que reinicia la etapa a 0.

El selector permite scroll horizontal y trae la opción activa a la vista sin mover la página. `fill="never"` significa que esta instancia conserva ancho por contenido también en móvil; no aplica la distribución automática de dos opciones a todo el ancho.

### 9. Línea de estaciones y barra que va creciendo

Fuente: S01:127–146 y 200–230; S02:90–112. **Observada, INF-VIS-007.** Ambos recorridos tienen ocho etapas en la fuente actual. La barra indica la estación seleccionada, no porcentaje de descarga, tiempo ni ejecución real.

#### 9.1 Construcción de la línea

Un `nav` contiene `ol` grid con `repeat(--stations,minmax(44px,1fr))`, gap 4 px y min-width max-content en base. La línea son dos pseudoelementos absolutos del `ol`:

- `::before`: base neutra en border-default.
- `::after`: frente con `linear-gradient(90deg,info,warning,action,success)`, glow `0 0 12px action-subtle`, origen a izquierda y `scaleX(--progress)`.

Altura 4 px, top 21 px, radios pill; cada extremo se retira `100% / stations / 2` para aproximar los centros de primera y última estación. El gap de la grilla no forma parte explícita de esa fórmula: no presentarlo como alineación geométrica medida.

```text
progress = selected / (steps.length - 1)
8 estaciones → 0, 1/7, 2/7, 3/7, 4/7, 5/7, 6/7, 1
```

El cambio de escala dura **520 ms ease-out**. Al retroceder se contrae; al cambiar de recorrido vuelve a 0.

**Detalle decisivo del color:** no hay keyframes que cambien el color de la barra con el tiempo ni umbrales de progreso. Se escala todo el frente, incluido su degradado fijo. A medida que se estira se redistribuyen espacialmente azul→ámbar→rubí→verde; por separado, las estaciones y la ficha adoptan el color de su zona.

#### 9.2 Estados de cada estación

El li tiene z-index 1, por encima de las líneas. El botón es columna, gap 10 px y fondo transparente. El círculo mide 46×46 px en base, borde 2 px, número de dos cifras, 13 px/700 y números tabulares.

| Estado | Círculo | Texto / símbolo |
| --- | --- | --- |
| `next` | Fondo solid, borde strong, texto muted | Número `01`–`08`. |
| `done` | Fondo y borde `zone`, texto `surface-solid` | Check SVG de 14 px sustituye al número. |
| `current` | Fondo solid, borde/texto `zone`, escala 1.14 | Halo de 6 px y sombra 0 8px 20px zone-subtle; nombre en strong. |
| Hover no actual | Borde/texto zone y escala 1.06 | Disponible con hover/pointer fino. |
| Foco | Outline 2 px focus con offset 4 px en el círculo | El botón retira su outline propio. |

Los cambios de transformación y sombra duran 260 ms ease-out; fondo, borde y color 260 ms ease-standard. El CSS de hover también alcanza estaciones `done`; su color de check puede pasar a zone sobre fondo zone mientras se mantiene ese hover. Es una consecuencia de selectores, no una regla de contraste recomendada.

`done` se calcula como `index < selected`; no se conserva historial de visitas. Tocar directamente la estación 8 marca las anteriores como done aunque no se hayan abierto.

#### 9.3 Teclado y scroll

Solo la estación actual tiene tabIndex 0. Flecha derecha/abajo avanza; izquierda/arriba retrocede; Home/End llevan a extremos. A diferencia del selector segmentado, aquí las flechas **se limitan a extremos** y no ciclan. Se actualizan selección y foco.

En móvil, un effect centra horizontalmente la estación dentro del rail si desborda, con smooth o auto según reduced motion. No usa scrollIntoView para esta acción; no mueve deliberadamente la página vertical. Los nodos de portada sí desplazan la página al ancla del recorrido (top -96 px).

### 10. Clic en nodos, botones y enlaces

Fuente: S01:78–82 y 127–145; S02:52–66; S07; S13.

#### 10.1 Nodo del mapa

`FlowNode` es un botón con aria-label que incorpora título, descripción e invitación a ver la etapa. Reposo: min-height 112 px, fondo oscuro translúcido, borde zonal al 28%, brillo inset y sombra `0 10px 24px rgba(2,6,23,.25)`.

Hover fino: sube 3 px, borde al 70%, sombra inferior `0 16px 32px rgba(2,6,23,.35)` y halo exterior de 4 px al 12%. Transform/sombra cambian en 220 ms ease-out; borde en 140 ms ease-standard. Foco: outline 2 px de la zona de banner y offset 3 px.

El selector `:active` asigna lift +1 px. Sin embargo, la regla hover está después y tiene la misma especificidad: en un mouse que mantiene hover al pulsar, el lift -3 px puede prevalecer. En táctil sin hover sí puede aplicarse +1 px. Se registra la cascada sin corregirla.

Al hacer clic, se cambia recorrido/índice y se hace scroll hasta la ficha. Mapeo real:

| Nodo | Destino |
| --- | --- |
| React + Vite | Web, etapa 1 |
| Fusion → Blender | Modelo, etapa 1 |
| Paquete gzip | Web, etapa 3 |
| LittleFS + flash | Web, etapa 5 |
| Servidor HTTP | Web, etapa 6 |
| Tu navegador | Web, etapa 7 |

No hay selección persistente pintada sobre el nodo de portada ni una animación de check en ese nodo; el estado seleccionado vive en el recorrido.

#### 10.2 Botones comunes

Anterior, Actualizar y Consultar usan variante secundaria; Siguiente es primaria salvo en el último paso, donde pasa a secundaria y disabled. Todos aquí usan `size=sm`: min-height 44 px, padding horizontal 1rem, vertical .5rem, gap .375rem y texto Tailwind `text-sm` (.875rem en su escala predeterminada). `rounded-lg` es una utilidad Tailwind; no es una referencia literal al token local `--radius-lg` de 18 px.

Primario: fondo action, texto on-action y borde action-border. Secundario: fondo subtle, texto strong y borde default. Las variantes se escriben como estilos inline.

La clase compartida `.afr2-pressable` escala a .97 durante 150 ms ease-out al pulsar si no está disabled; declara transiciones de color/fondo/borde/sombra a 140 ms. **En las fuentes leídas no se asigna un hover o active cromático específico a este Button.** Que existan tokens action-hover y action-active no implica que se usen en esta instancia. En teclado se eliminan duraciones de transición globalmente; la regla de transformación active sigue existiendo para Button mientras no haya reduced motion.

`loading` bloquea el botón y sustituye el icono por spinner: span de 16×16 px, borde de 2 px currentColor, tramo superior transparente, animación de giro con duración 800 ms. No hay opacidad reducida automática declarada para todos los botones disabled, ni `aria-busy` local en Button; la sección de paquete sí recibe aria-busy.

#### 10.3 Enlaces

«Explorar modelos» es inline-flex con gap 8 px, min-height 44 px, texto action de 13 px/650 y sin subrayado. Al hacer hover fino solo la flecha se desplaza 3 px a la derecha en 140 ms ease-out. Lleva a `/modelos`.

«Ir a Conexión» en el bloque de almacenamiento lleva a `/conexion`; el enlace local usa action y peso 650. No hay un menú desplegable asociado.

### 11. Ficha de etapa y transición de contenido

Fuente: S01:211–230; S02:115–142. La ficha es un `article` con `key=journey-selected`, `aria-live=polite` y `aria-labelledby`. La key provoca desmontaje/montaje al cambiar de paso o recorrido y reinicia sus animaciones.

La entrada del article dura **340 ms**, opacity 0→1 y translateY 10 px→0, ease-out, fill both. No hay una animación de salida del artículo anterior ni morph entre títulos: el anterior se retira y el nuevo entra. El icono grande entra en **460 ms**, de escala .7/rotación -8°/opacity 0 a estado normal.

La columna principal ordena ubicación/contador → icono/título → descripción → Entra/Sale. La salida recibe borde zonal y una línea interior izquierda de 3 px mediante box-shadow inset; la entrada conserva borde neutro. El cable entre ambas usa el color de la zona para línea y paquete.

El lateral muestra h4 y una lista numerada. Los números se generan con CSS counters en un pseudoelemento: 24×24 px, radio 8 px, left 14 px/top 13 px. Cada hecho entra desde translateY 8 px y opacity 0, durante **380 ms**, con delay `80ms + index*60ms`. La cantidad de hechos varía por etapa; no se fija una altura constante ni se anima el cambio de altura total.

«Anterior» se bloquea en el primer paso y «Siguiente» en el último. Cambiar con esos botones no llama al scroll vertical del banner. El campo `source` existe en los datos, pero la línea «Fuente en el proyecto» ya no se renderiza; la extracción respeta la fuente actual frente a la descripción histórica 07.

### 12. Todas las barras, sus colores y su movimiento

#### 12.1 Pesos de los modelos

Fuente: S01:114–117 y 235–251; S02:148–163. Cada fila usa grid areas `name value / track track`: nombre a izquierda, cifra a derecha y barra abajo. La pista mide 14 px, radio pill, fondo subtle y contorno inset de 1 px; recorta los fills.

```text
weightMax = max(1, suma conocida, auto, brazo, cubo)
w = bytesModelo / weightMax
ancho del fill = w × 100%
```

Catálogo leído, no medido: auto 880.232 B, brazo 780.972 B, cubo 1.784 B; suma 1.662.988 B. El cubo tiene presencia visual mínima aunque su proporción real sea pequeña.

| Modelo | Degradado del fill |
| --- | --- |
| Auto | `linear-gradient(90deg,info,color-mix(in srgb,info 60%,success))` |
| Brazo | `linear-gradient(90deg,action,color-mix(in srgb,action 55%,warning))` |
| Cubo | `linear-gradient(90deg,warning,color-mix(in srgb,warning 60%,success))` |

Los fills tienen ancho objetivo calculado y arrancan con **scaleX(0)**; al entrar la sección visible pasan a 1 con **900 ms ease-out**, origen izquierda y delay `index*110ms`: 0/110/220/330 ms. Es el transform lo que crece, no una transición del ancho.

La fila suma contiene tres fills absolutos: auto empieza en 0; brazo en auto/weightMax; cubo en (auto+brazo)/weightMax. Los tres comparten el índice 3 de la fila, así que arrancan juntos con delay 330 ms: no es una secuencia uno tras otro. Se escalan desde su propia posición. Los fills individuales tienen min-width 8 px; el cubo de la suma tiene min-width 4 px. Esa mínima visibilidad puede apartarse de la proporción exacta y recortarse en el borde derecho. La unión auto/brazo tiene radios específicos; el brazo conserva extremo derecho redondeado aun seguido del cubo.

El título numérico expone los bytes exactos con atributo title. Estas barras comparan archivos GLB antes de gzip; no son RAM, VRAM ni progreso de carga.

#### 12.2 Antes y después de gzip

Fuente: S01:268–274; S02:168–177. Dos pistas de 34 px de alto, radio 10 px y etiquetas superpuestas. La primera recibe w=1 y fondo `color-mix(text-muted 22%,transparent)`. La segunda recibe `min(1,totalGzipBytes/totalRawBytes)` y degradado action→mezcla 60% action/warning.

Los fills escalan 0→1 en **1000 ms ease-out**; gzip espera **260 ms**. El texto `em` es relativo, queda por encima del fill, usa 12 px/600 y padding horizontal 12 px. En gzip hay text-shadow de 6 px con surface-solid. El porcentaje inferior se expresa como «X% menos» o «X% más».

El ancho gzip se limita a 100% aunque el archivo crezca, pero la etiqueta puede decir «más». Un raw=0 produce «Sin comparación». Al actualizar, el hook borra el manifiesto y luego remonta el bloque; como Reveal ya está visible, no se puede asegurar que se repita una transición desde cero sin observar el montaje real en navegador.

#### 12.3 Mini barras por archivo

Fuente: S01:307; S02:218–220. Ancho del grupo 100 px, dos barras de 4 px con gap 3 px. Arriba text-muted, abajo action. Ambas se normalizan por `max(1,rawBytes,gzipBytes)`: una fila puede mostrar la barra gzip más larga. **No tienen transición ni keyframes propios.** Sus anchos aparecen con los datos.

### 13. Anillo de almacenamiento LittleFS

Fuente: S01:119–126 y 276–290; S02:180–193. Es una caja de **108×108 px** y un pseudoelemento con conic-gradient, no un SVG animado.

```css
--deg: calc(var(--p) * 3.6deg);
background: conic-gradient(
  var(--warning) 0deg,
  var(--action) var(--deg),
  var(--border-default) var(--deg)
);
mask: radial-gradient(farthest-side,
  transparent calc(100% - 12px),
  #000 calc(100% - 11px));
```

La máscara vacía el centro y deja un aro de aproximadamente 12 px con 1 px de transición. El arco ocupado interpola de warning a action; el resto es borde neutro. La ocupación calculada es `min(100,used/capacity*100)` cuando hay ambos valores y capacity>0. No hay transición de `--p`, interpolación temporal del gradiente, contador ascendente ni umbrales verde/amarillo/rojo declarados.

Con dato se muestra porcentaje central con un decimal máximo y números tabulares. Sin dato el mismo espacio muestra icono board de 28 px y p=0: no se imprime «0%» como si se hubiera medido vacío. Un `<progress class=sr-only>` conserva used/max para tecnología asistiva; el anillo visible lleva aria-hidden.

Se usa primero filesystem HTTP cuando cumple la condición de S01 y, si no, el del contexto de placa. La fecha/hora y el origen se explican en texto. Una placa lista permite Consultar/Volver a consultar si el origen no es HTTP; sin conexión se ofrece un enlace. El bloque de almacenamiento permanece aunque no exista manifiesto.

### 14. Tarjetas de estadísticas, tabla, vacío y glosario

#### Estadísticas

Grid de 1/2/4 columnas según ancho, gap 12 px. Cada stat usa áreas `icon label / value value / detail detail`, padding 18 px, fondo subtle, borde sutil y radio 18 px. Icono de 18 px en caja rubí de 32×32 px/radio 10 px. La cifra ocupa toda la segunda fila. Los cuatro roles son antes de gzip, gzip, ocupación LittleFS y estimación FS.

#### Tabla

Wrapper con overflow-x:auto, radio 18 px, borde default, foco explícito y tabIndex 0. Tabla full-width, border-collapse, texto alineado a izquierda y font label. Caption y thead tienen fondo subtle; celdas 14×16 px y separador inferior. El nombre de archivo va dentro de code; la cabecera de fila permite corte de palabras entre 180 y 380 px, los valores quedan nowrap y tabulares. Hover fino cambia el fondo de una fila a subtle durante 140 ms ease-standard. No hay selección, ordenación, paginación ni animación de filas implementadas aquí.

#### Vacío/error

El error de reporte se presenta como bloque informativo, borde dashed strong y fondo subtle. Icono book de 24 px dentro de caja 48×48 px/radio 14 px con action-subtle. El título explica que el recorrido sigue disponible; párrafos margen 8px 0 0. No se colorea todo en rojo: el fallo del reporte no invalida el contenido educativo.

#### Glosario

Cuatro `article`, texto íntegro visible. Grid gap 14 px; cada tarjeta padding 24 px, gap 10 px, radio 24 px, solid y borde default. Un `::after` circular de ancho 70%, aspect-ratio 1, right -30%/bottom -60%, añade un radial action-subtle→transparente; tiene pointer-events:none. No aplica blur ni backdrop-filter.

Icono de 22 px en caja 48×48 px/radio 14 px y borde action-border. Hover fino: translateY(-3px), borde action y sombra `0 16px 36px rgba(15,23,42,.1)`; transform/sombra 240 ms ease-out, borde 140 ms ease-standard. Aunque responde al hover, **el artículo no es botón ni enlace**. No hay clic ni desplegable; no interpretar elevación como una acción confirmada.

### 15. Registro completo de movimiento

Fuente: S02 keyframes/transitions; S05/S06; S13. Las curvas nombradas son `out=cubic-bezier(.23,1,.32,1)`, `standard=cubic-bezier(.2,.75,.25,1)` e `in-out=cubic-bezier(.77,0,.175,1)`. Son curvas CSS, no springs de una biblioteca.

| Patrón | Disparador y propiedades | Tiempo / delay | Curva / repetición / salida |
| --- | --- | --- | --- |
| Entrada de ruta | Montaje; opacity 0→1, y 4→0 px | 180 ms | out, both; sin exit propio. |
| Glow de portada | Montaje; translate/scale/opacity | 9 s por tramo | in-out, alternate infinite. |
| Pulso de iconos del mapa | Montaje; box-shadow 14→28→14 px y alfa | 3.2 s | in-out, infinite. |
| Dash de cables | Montaje; background-position 10 px por ciclo | .7 s | linear, infinite. |
| Paquetes del cable | Transform + opacity, aparición 15–85% | 2.4 s; segundo +1.2 s | in-out, infinite. |
| Offset entre cables del mapa | --wire-delay | 0/.5/1/1.5 s | Solo demora los paquetes, no el dash. |
| Hover nodo | y/borde/sombra | 220/140/220 ms | out/standard/out. |
| Flecha de enlace | Hover; x 0→3 px | 140 ms | out; revierte al salir. |
| Pill selector | Elección con puntero; clip-path | 250 ms | in-out; nueva elección actualiza destino. |
| Press de label selector | Active en puntero; scale .97 | 150 ms | out; revierte al soltar. |
| Metro línea | Cambio índice; scaleX progress | 520 ms | out; también contrae. |
| Metro estaciones | Transform/color/fondo/borde/sombra | 260 ms | out para geométricas, standard para cromáticas. |
| Ficha | Key nueva; opacity + y 10→0 px | 340 ms | out, both; anterior se desmonta. |
| Icono ficha | Key nueva; opacity, scale .7→1, rot -8→0° | 460 ms | out, both. |
| Hechos | Key nueva; opacity + y 8→0 px | 380 ms; 80+i×60 ms | out, both. |
| Reveal sección | Primera intersección; opacity + y 18→0 px | 600 ms | out; no se vuelve a ocultar. |
| Pesos | Reveal visible; scaleX 0→1 | 900 ms; i×110 ms | out. |
| Compresión | Reveal visible; scaleX 0→1 | 1000 ms; gzip +260 ms | out. |
| Glosario entrada | Reveal visible; opacity + y 8→0 px | 480 ms; i×90 ms | out, both. |
| Glosario hover | y/borde/sombra | 240/140/240 ms | out/standard/out. |
| Hover fila tabla | Fondo | 140 ms | standard. |
| Button press | Active; scale .97 | 150 ms | out. |
| Spinner de Button | loading | 800 ms | Clase animate-spin, giro continuo. |
| Scroll de nodo / rail | pick / cambio selección con desborde | Duración del navegador | smooth; auto con reduced motion. |

El paquete vertical viaja de y=-2 a y=28 px; en horizontal usa `translateX(calc(100cqi - 8px))`. Los cables horizontales declaran `container-type:inline-size`, de modo que cqi sigue el contenedor. Los paquetes tienen 8×8 px, radio circular, halo de 3 px al 25% y glow 12 px; opacity 0 al principio/final y 1 entre 15–85%.

El pulso cambia contorno/halo: extremos con contorno al 30% y halo 14 px al 18%; centro con contorno al 50% y halo 28 px al 40%. No hay rotación continua de iconos.

`useReveal` observa con `rootMargin:'0px 0px -10% 0px'`, marca visible en la primera intersección y desconecta. No tiene threshold específico ni reactivación al volver a subir. Sin IntersectionObserver marca visible. La entrada de sección y sus hijos pueden solaparse; no hay un timeline central que espere a que termine cada una.

Las transiciones CSS redirigen el objetivo ante nuevos cambios; las animaciones de ficha se reinician por remount. No hay cola de animaciones, espera de salida, persistencia de dirección ni cancelación programada de loops cuando el elemento queda fuera del viewport. No se midió rendimiento; animar sombras/background-position/clip-path no equivale a garantizar trabajo exclusivo en compositor.

### 16. Responsive: cuatro composiciones

Fuente: S02:239–281 y S14. Los puntos de corte usan ancho de viewport; no son container queries de la tarjeta.

| Ancho | Portada / mapa | Recorrido | Datos y glosario |
| --- | --- | --- | --- |
| <600 px | Carriles apilados; nodos de cada carril en columna. Los dos orígenes PC siguen en dos columnas. Cables verticales. | Sin labels de estaciones; círculos 36 px/12 px, columnas mínimas 40 px y rail con padding lateral 6 px. Entra/Sale vertical. Ficha apilada. | Una columna. Storage pasa a column/start. |
| 600–839.98 px | Carriles apilados, cuerpos internos en fila. Orígenes PC pasan a una columna de dos nodos, altura mínima 96 px; cables interiores horizontales. | Labels visibles, círculos 46 px; Entra/Sale en 1fr / 56–96px / 1fr. Ficha apilada. | Stats y glosario 2 columnas; paquete una columna. |
| 840–1199.98 px | Mismo mapa de carriles apilados. | Ficha en `minmax(0,1.25fr) minmax(300px,1fr)`; separador lateral izquierdo. | Paquete 1.4fr / 1fr; solo un hijo lo vuelve a 1 columna mediante :has. Stats/glosario siguen 2. |
| ≥1200 px | Carriles en fila con proporciones flex PC 2.3 / ESP 2 / cliente 1. Cables externos horizontales de 44 px. Hero sigue una columna. | Misma ficha de dos columnas. | Stats y glosario 4 columnas. |

No hay un layout hero de dos columnas texto/ilustración en la fuente actual: incluso a partir de 1200 px declara una sola columna. La descripción histórica «todo vertical» en móvil admite la excepción concreta de los dos orígenes PC lado a lado. Estas son razones para tomar el código actual como evidencia principal.

La navegación general usa barra inferior bajo 840 px y sidebar desde ese ancho. La sidebar inicial está plegada entre md/lg y expandida desde lg si no existe preferencia guardada. Esta estructura soporta la sección; no se eleva a regla nueva derivada de Infraestructura.

### 17. Accesibilidad, preferencias y límites de la implementación

#### Patrones observados

Sections con aria-labelledby; un h1 propio; niveles h2/h3/h4; botones nativos; radiogroup con aria-checked en selector; estación actual con aria-current=step y aria-controls; ficha aria-live polite; errores/carga con role=status; paquete aria-busy; tabla con caption y scope; wrapper desplazable enfocable; iconos decorativos aria-hidden; progreso accesible separado del anillo.

Foco general 2 px y offset 2 px, con overrides de mapa/metro/tabla. Estaciones y selector usan tabIndex roving. `touch-action:manipulation` y tap-highlight transparente están en global. El tamaño objetivo de Button y selector es al menos 44 px; en móvil los círculos del metro miden 36 px y el botón no declara min-height 44, así que no se certifica un área táctil mínima por el solo token global.

#### Movimiento reducido, comportamiento real

Se anulan loops de cables, paquetes, pulso de iconos y glow; se esconden los paquetes. Ficha, icono, hechos y conceptos cambian su animation-name a un fade. Reveal pierde el translateY inicial; el frente del metro pasa a 0 ms; el selector usa duration-segmented=0; los scrolls usan auto. Button elimina su active transform.

**Cobertura parcial:** las barras conservan scaleX y sus 900/1000 ms literales; el fade conserva las duraciones/delays originales; el hover/active de nodo y el hover de conceptos todavía asignan transform, aunque se retire transform de transition-property, por lo que pueden cambiar instantáneamente. El icono de nodo sigue manteniendo el box-shadow estático. El spinner no tiene una anulación específica observada en las fuentes recorridas. No afirmar que «todo queda estático».

#### Teclado y contraste

ThemeProvider coloca `data-input=keyboard` al usar Tab/flechas/Enter/espacio y `pointer` al pulsar. Globalmente, las **transiciones** pasan a 0 ms con !important; esto no desactiva automáticamente todos los **keyframes** locales. La entrada de ruta sí se desactiva explícitamente en workspace al usar teclado.

Contraste aumentado refuerza bordes generales y ranura del selector. Los fallbacks de transparencia global cambian familias glass; los chips de este banner conservan su blur 8 px y sus fondos específicos. No se leyeron reglas propias de forced-colors para esta superficie ni se midieron ratios. Números/check, etiquetas y texto apoyan el color; aún existen casos de contraste que requieren una revisión futura autorizada.

### 18. Estados de datos y tono visible

Fuente: S04; S01:254–313; S21:440–447. La carga del reporte empieza automáticamente al montar, solicita `/api/web-manifest` sin caché y espera hasta 8 s antes de abortar. En origen ESP intenta health bajo el mismo controller/timeout. Al actualizar, vacía reporte y error, vuelve a loading y después reconstruye la presentación.

El diseño usa textos locales y concretos: «Seguí», «Descubrí», «Conectá», «Podés volver a intentarlo». Etiquetas de acción con verbo, explicación cerca de los datos y procedencia visible. Tamaños en es-AR; B/KiB/MiB, máximo dos decimales para unidades mayores y title con bytes exactos. Porcentajes con un decimal máximo. No hay contador interpolado de cifras.

| Estado | Presentación |
| --- | --- |
| Loading | Botón Actualizar bloqueado con spinner y texto «Consultando el manifiesto del servidor…». |
| Error del reporte | Panel dashed de información; recorrido y almacenamiento siguen disponibles. |
| Paquete local | Badge azul «Paquete local preparado · no acredita una carga» y advertencia de versión. |
| Paquete ESP | Badge ámbar «Manifiesto leído del ESP». |
| Sin ocupación / desconectado | Icono en anillo, explicación y enlace a Conexión. |
| Placa ready sin origen HTTP | Botón Consultar/Volver a consultar, spinner mientras espera. |
| Con ocupación | Porcentaje, used/capacity, origen y hora si corresponde. |
| Datos ausentes | «Sin dato», no número cero inventado. |

**Particularidad:** el componente tiene una rama `boardQuery='failed'` que muestra «La placa no respondió». Pero `refreshSystemInfo` captura internamente la excepción y no la propaga, de modo que un fallo de ese comando no necesariamente alcanza el catch de `askBoard`. Esta rama se documenta como declarada, sin garantizar que aparezca en el flujo actual.

### 19. Qué conservar, qué no generalizar

#### Cinco reglas protectoras

1. **Observada:** usar paleta fija de banner sobre base oscura y paleta semántica por tema en contenido; conservar su separación.
2. **Observada:** transportar el color de zona por herencia y repetirlo en insignia, estación, icono y ficha sin convertirlo en alarma.
3. **Observada:** estructurar con jerarquía de títulos, gaps y superficies; reservar los bordes para grupos y separadores específicos.
4. **Observada:** mantener el selector como copia activa recortada, con texto/icono/fondo sincronizados y teclado inmediato.
5. **Inferida:** relacionar cada movimiento con lectura, selección o recorrido, y no copiar sus excepciones técnicas como reglas deseables.

#### Inconsistencias de mayor impacto en una reutilización

| Hallazgo estático | Implicación |
| --- | --- |
| `UiIconSize=16\|20\|24`, pero esta página pasa 14/18/22/28 en varios puntos | Los tamaños dibujados están solicitados por JSX, pero exceden el tipo declarado. No se ejecutó TypeScript. |
| data-zone global sin prefijo de Infraestructura | La familia zonal puede alcanzar otras superficies con ese atributo al estar el CSS importado globalmente. |
| Hover posterior al active del nodo | La respuesta de pulsación puede diferir mouse/táctil. |
| Hover de estación done cambia texto a zone sobre fondo zone | Puede ocultar el check al mantener hover. |
| Movimiento reducido parcial | No copiar la frase general «todo el movimiento se detiene» como descripción fiel. |
| Artículos del glosario elevan con hover sin acción | Elevación no significa siempre clic en esta muestra. |
| Mínimos de fills y suma apilada | La presencia del cubo se favorece sobre la proporcionalidad exacta. |
| Error de consulta absorbido | El estado failed local no acredita feedback efectivo de fallo. |
| Comentarios/manual global con versiones distintas | Registrar qué archivo se consume; este dossier no resuelve el versionado general. |

Estas observaciones no autorizan cambios. El código actual y las fichas históricas se distinguen; no se inventaron validaciones ni se generó una línea de base automática.

#### Extensiones no observadas

Formularios, campos editables, dropdowns, tooltips personalizados, menús, modales, autenticación y notificaciones toast propias de esta sección no forman parte del patrón directo. En el manual general conservan los contratos ya existentes. Solo las variantes nuevas que pretendan deducirse de Infraestructura son **Propuesta**: no se derivan de las propiedades del banner ni sustituyen campos, overlays o menús documentados.

El siguiente paso de menor riesgo es consultar [components.md](components.md) y [tokens.json](tokens.json) al especificar un nuevo destino, usando el manual general integrado como guía y el dossier original como evidencia. La integración documental no modifica automáticamente la Web.
