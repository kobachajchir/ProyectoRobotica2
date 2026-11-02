# Componentes, estados y recetas

02/11/2026 · versión 0.1.0 · INF-VIS-001–014. Los IDs S remiten a [sources.md](sources.md). Anatomías, dimensiones y movimientos completos están en [manual.md](manual.md); este archivo concentra los contratos reutilizables.

## 1. Matriz de cobertura

`Cubierta` significa descrita desde la muestra, no probada ni aprobada. Las superficies ausentes se identifican como extensiones Propuestas; no se han diseñado variantes nuevas.

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

## 2. Contratos de los componentes presentes

### 2.1 Hero narrativo

**Propósito:** dar contexto, mostrar un recorrido general y abrir sus etapas. Anatomía obligatoria de la muestra: glow decorativo → copy → mapa → takeaway. El copy contiene eyebrow, único h1, lede y lista de chips. Color fijo oscuro; fuente S01/S02/S12; tokens `sections.banner`, `components.hero`.

Se admite texto de dos líneas, contexto hasta 64ch y chips con wrap. El mapa aprovecha todo el ancho del hero. La ilustración no carga modelos 3D ni necesita raster. La grilla y los halos quedan detrás del contenido; el recorte y el stacking pertenecen a la caja exterior. No hay semántica interactiva en el glow ni en el takeaway.

**Regla extraída:** separar material de portada y material de lectura. **No deducir:** que cada tarjeta de contenido deba tener fondo oscuro, grilla o blur. Las capas de esta portada son una composición específica.

### 2.2 Carril y FlowNode

**Propósito:** agrupar el trabajo por equipo y ofrecer accesos a etapas. Carril con `data-zone`, etiqueta absoluta y body; FlowNode con botón, icono, strong y detalle. El color sale de la zona de banner. El nodo requiere `onPick`, no recibe prop de estado seleccionado.

Tiene hover/active/focus, sin disabled ni loading propios. Los tres carriles responden al ancho según manual §16. Se conserva un nombre accesible descriptivo por botón. El nodo se puede enfocar y activar de forma nativa; la selección se expresa en el recorrido de destino.

**Límite:** no extraer de él una tarjeta genérica seleccionable con check, ni una orden de hardware. Hover/active tienen la particularidad de cascada documentada. Tokens `components.flowNode` y zonas.

### 2.3 Wire

**Propósito:** visualizar transmisión entre nodos/celdas, sin representar tráfico real. Anatomía: span aria-hidden, dos `i`, `em` opcional. Vertical en base; horizontal según ubicación y breakpoint. `delay` se aplica a paquetes. `label` puede mostrar .bin/gzip. Dentro de la transformación de etapa usa zone; en portada usa colores del banner.

Decoración sin foco ni nombre accesible propio. Con reduced motion queda línea estática sin paquetes. No debe convertirse su velocidad en una medida de ancho de banda o actividad del equipo. Tokens `components.wire`, `motion.wire`.

### 2.4 Selector de recorrido

**Propósito:** elegir entre dos explicaciones locales. Anatomía de doble fila y recorte descrita en manual §8. Variante efectiva `choice`, dos radios, roving focus, auto activation, size md y fill never. Texto/icono/fondo activos coinciden por la copia recortada.

Estados: reposo, hover, selected, active de label, focus; disabled disponible en primitiva pero ausente de estas opciones. Teclado cicla entre habilitados; Home/End, Enter/espacio mediante botón; sin animación de selección por teclado. Resize recoloca instantáneo. La selección reinicia paso a cero.

**Combinación correcta observada:** selector a la derecha de un h2 dentro de heading flex con wrap. **No derivar:** dropdown, pestañas de navegación de ruta o selector que envía comandos al robot. Tokens `components.segmented` y `motion.segmented`.

### 2.5 Metro

**Propósito:** elegir una de ocho etapas y mostrar ubicación relativa. Anatomía: nav → ol con dos pseudo-líneas → li zonal y botón → dot y label. Las etapas anteriores se llenan y muestran check; la actual crece y recibe halo; las posteriores son neutras.

Los labels se ocultan en celular; números/check permanecen. Scroll del rail automático si hay desborde; foco y selección se mantienen. Flechas se limitan a extremos, Home/End disponibles. El actual lleva aria-current step. Color no basta para identificar estado: número/check, escala y ubicación lo acompañan.

**Límite funcional:** done significa índice anterior, no paso visitado ni tarea cumplida. La barra no es loader. Tokens `components.metro`, `motion.metro`. Particularidad hover de done en manual §9.

### 2.6 Ficha de etapa

**Propósito:** exponer una transformación y detalles de una etapa. Anatomía: main con zona/contador, título/icono, descripción, Entra/Sale; side con h4, hechos y navegación. Se renderiza un único artículo con aria-live polite; key variable remonta todo el contenido.

Variantes por zona y por posición primera/intermedia/última. Entra/Sale vertical hasta 600 px, luego tres columnas. El lateral pasa de debajo a derecha a 840 px. Foco de la estación o botón que inició la acción no se mueve explícitamente al h3.

La salida tiene el acento interior izquierdo; los hechos tienen numeración CSS y entrada en cascada. La línea de fuente técnica no se presenta. No deducir un modal ni una ficha expandible. Tokens `components.stage`, `motion.stage`.

### 2.7 Button y enlace de acción

**Propósito:** navegación de pasos y consultas de datos. Variante primaria para avanzar disponible; secundaria para retroceder y refrescar. Size sm, altura mínima 44 px. Iconos de navegación/refresh; spinner reemplaza al icono principal cuando loading. Disabled evita activar; los límites deshabilitan Anterior/Siguiente.

Foco nativo reforzado por global, feedback de escala active; no hay cambio cromático específico de hover/active declarado para Button. El enlace a Modelos usa flecha que se desplaza y conserva navegación por Link. La consulta a placa solo se ofrece según ready/origen; no es visible siempre.

**No generalizar:** los tokens de hover/active existentes como estados efectivamente consumidos por estos botones. Tokens semánticos de acción/superficie y `motion.buttonPress`.

### 2.8 Badge de zona/procedencia

**Propósito:** identificar equipo u origen de un dato. Span inline-flex, icono 14 px y etiqueta, padding 6×12 px, gap 7 px, radio pill. Fondo/tinta/borde de la misma familia. La ficha lo hereda; el origen del paquete lo declara pc/esp en el propio elemento.

No es botón ni filtro. El texto mantiene la procedencia explícita; local preparado no se presenta como placa instalada. Token `components.zoneTag` y familias semánticas.

### 2.9 Barras de peso y suma

**Propósito:** comparar tamaño de GLB. Anatomía lista → nombre/valor/track → fill; suma con tres fills y posiciones acumuladas. Se conserva etiqueta textual y title de bytes exactos; tracks aria-hidden. Variantes por modelo y suma. Aparición única ligada a Reveal; no hay click.

Debe conservarse la base común de comparación para estas filas. Los mínimos de píxeles son excepciones de legibilidad y no una proporción exacta. No usar estos valores como memoria de ejecución. Tokens `components.modelBars`, `motion.modelBars`.

### 2.10 Compresión y estadísticas

**Propósito:** comparar archivos antes/después y resumir magnitudes. Compresión: label, barra raw, barra gzip, delta grande, caption. Estadística: icono/label arriba, cifra, detalle. Números tabulares, unidades es-AR y dato ausente explícito.

Se muestran solo si hay manifiesto. Las dos áreas del paquete pasan a columnas desde 840 px; stats a dos desde 600 y cuatro desde 1200. No existe recuento numérico animado. Tokens `components.compression`, `components.stat`.

### 2.11 Storage y anillo

**Propósito:** mostrar capacidad/uso reportados y permitir consulta. Anatomía ring a izquierda + bloque de texto/acción; en móvil se apila. El espacio ring se mantiene sin dato, cambiando porcentaje por board. Progress invisible para lectores acompaña al gráfico decorativo cuando existe dato.

Estados sin dato, con dato, conectado, consulta busy y rama local failed. El porcentaje no se anima en la implementación. Procedencia HTTP tiene prioridad según la condición de fuente; no hay polling propio de la página para este anillo. La rama de error tiene la limitación de S21.

No añadir thresholds de estado como si existieran. Tokens `components.storage`, `components.ring`.

### 2.12 Tabla de archivos

**Propósito:** comparar cada recurso. Columnas: ruta, antes, guardado y cambio. Caption identifica local/ESP; encabezados scope; números nowrap/tabulares; rutas code con wrap; pares de mini barras por fila. Wrapper enfocable y scrollable para anchos pequeños.

Hover solo resalta fondo; no cambia selección ni abre detalle. Sin estados de sorting, filtros o paginación en muestra. Datos ausentes ocultan la tabla, no crean filas ficticias. Tokens `components.fileTable`.

### 2.13 Estado de reporte ausente

**Propósito:** explicar por qué faltan datos sin interrumpir el recorrido. Anatomía flex con caja de icono, strong y párrafos; borde dashed, superficie subtle y tinta muted. role=status en wrapper. El botón de refrescar sigue en cabecera.

El componente es un div local, no una alerta compartida genérica ni modal. La acción de recuperación reside fuera del bloque. No deducir que errores físicos deban usar esta misma jerarquía informativa.

### 2.14 Glosario

**Propósito:** lectura simultánea de cuatro conceptos. Icono + h3 + párrafo completo. Fondo solid, halo recortado abajo a derecha, hover con elevación y entrada escalonada. Una/dos/cuatro columnas. Sin teclado de interacción ni selected/expanded; es article estático.

La geometría se puede describir como tarjeta editorial. Copiarla como tarjeta de acción requeriría una decisión Propuesta para añadir semántica de enlace/botón y foco correspondiente. Tokens `components.concept`.

## 3. Recetas de composición derivadas

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

## 4. Mapa de materiales y límites

| Región | Acento | Superficie | Rasgo dominante | No extender automáticamente |
| --- | --- | --- | --- | --- |
| Hero | Cuatro banner-zone fijos | Azul noche, capas radiales/grilla | Expresividad técnica | A todas las tarjetas de trabajo. |
| Recorrido | Zona de la etapa | Solid + lateral subtle | Selección y contexto | A estados de ejecución del robot. |
| Pesos | Por modelo | Solid/track subtle | Proporción y presencia mínima | A memoria real o tráfico. |
| Paquete | Action + procedencia pc/esp | Solid/subtle | Datos y origen | A aprobación de carga/instalación. |
| Glosario | Action | Solid + halo local | Lectura abierta | A una tarjeta accionable sin semántica. |

No se recomienda ningún componente externo nuevo: esta extracción describe las primitivas existentes y sus límites.
