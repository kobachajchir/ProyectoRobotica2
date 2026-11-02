# Componentes OLED128-5

Especificación revisada por el usuario el01/11/2026. Iconografía vinculante,
contenido mínimo, distribución limpia y tecnológica. Las medidas son tokens de
diseño propuestos; no son evidencia de legibilidad física. Atlas en
`../assets/component-atlas.png`; parámetros en `../assets/components.json`.
Son componentes diagramados, no una biblioteca de firmware ya integrada.
Revisión5 agrega [QRScreen](qr.md), generador offline y bitmaps. Es la
excepción a cabecera/pie globales: el QR completo y su zona clara ocupan
62×62 a izquierda, textos/retorno solo a derecha, sin superposición.
Revisión4 agrega [cards de acciones rápidas](quick-actions.md): título,
contador numérico arriba y franja invertida de selección en todas las cards.
Dashboard, barras y sensores conservan sus componentes previos.
Revisión3: cabecera invertida, paneles con esquinas recortadas y lectura
principal jerarquizada; ver [dashboard](dashboard.md). Conserva gestos y
sensores de2. La carga indeterminada deja vacía la zona de porcentaje.

## 1. Composición y tokens

Un objetivo por página. Priorizar icono, dato/estado esencial y ayuda de entrada.
El detalle vive en páginas hermanas o vistas de sección, recorridas manualmente
con giro. No rotación automática de contenido mientras se lee o se actúa.
Conservar las listas desplazables para seleccionar opciones: no convertirlas
en un carrusel de pictogramas porque otra sección tenga páginas hermanas.

| Token | Valor inicial | Uso |
| --- | --- | --- |
| `screen` | 128×64,1bpp | coordenadas enteras |
| `edge` | 2px | margen exterior de contenido |
| `icon_gap` | 4px | entre icono y texto o entre dos iconos asociados |
| `row_h` | 16px | fila de menú |
| `header_h` | 14px | cabecera |
| `footer_y`, `footer_h` | 50,14px | ayuda de encoder |
| `icon_header`, `icon_footer` | caja14px | contenido centrado |
| `icon_row` | caja16px | filas |
| `stroke` | 1px | contornos y selección |
| `font_body` | 7×10, avance7 | títulos y datos principales |
| `font_mini` | 3×5, avance4 | numeración/valores compactos |
| `font_metric` | numerales5×7, escala2, avance12px | lectura principal10×14 |
| `panel_chamfer` | 2px | esquinas recortadas comunes |

El padding exterior y el espacio entre elementos son conceptos diferentes.
No colocar texto o flechas pegados al icono. Dentro de un grupo horizontal,
usar un contenedor con centro común: `y = top + floor((H-itemH)/2)` para cada
pieza. No igualar sus bordes superiores. Con alturas pares/impares hay una
cuantización inevitable de medio píxel; usar siempre el mismo redondeo.
No añadir offsets ad hoc por pantalla. Corregir padding transparente en el
recurso o registrar un ajuste óptico centralizado, si fuera necesario.

Fondo negro, contornos finos, jerarquía por espacio y escala, iconos de píxel.
Sin tramas decorativas, marcos dobles por defecto, destellos o parpadeo continuo.
El marco simple propuesto unifica estilo; la referencia AutoMicro usa también
marcos dobles, que no son un requisito del nuevo perfil.
La cabecera usa banda invertida como componente común; no cambiarla por página.
Los bloques del dashboard usan paneles de [composición visual](dashboard.md).

## 2. Icon y SemanticIconRegistry

Todo componente recibe un ID semántico, no bytes dibujados localmente.
`actuator.servo` siempre usa el servo; `system.core` siempre usa el icono de
sistema; `input.encoder` siempre usa el mando UI. El registro en JSON vincula
IDs a recursos disponibles. `system.settings` es alias documentado de
`system.core`, no un segundo dibujo. Diferenciar sensores Hall del mando UI.

Propiedades: `id`, caja, posición, estado y variante registrada. Mantener
proporción; centrar el contenido en su caja. Las marcas de gesto no reemplazan
el icono del encoder: son modificadores del mismo recurso. No dibujar un
servo distinto para calibración, estado y movimiento.

## 3. IconLabel e IconPair

`IconLabel(icon,text)` coloca ambos en una línea con4px de separación y centro
vertical común. `IconPair(left,right)` hace lo mismo entre dos iconos. El
texto dice lo que falta explicar; no repite el nombre del componente sin valor.

`Header(sectionIcon,title,pageIndex,pageCount)` utiliza los mismos componentes.
Si el icono basta para nombrar sección, un título breve contextual puede
reemplazar una repetición como `Servo` en la misma cabecera. El contador `1/3`
queda reservado a derecha. No invadirlo con texto desplazable.

## 4. EncoderHint y NavigationFooter

Reglas revisadas expresamente por el usuario:

| Entrada | Componente visual | Resultado |
| --- | --- | --- |
| Pulsación corta | `[encoder]  OK` | confirmar/entrar/aplicar/cerrar aviso |
| Pulsación larga | `[encoder con L interna]  [flecha atrás]` | volver al contexto padre |
| Giro en páginas | contador fijo de página + feedback de cambio | cambiar de página hermana |
| Giro en lista/editor | marco de foco o valor en edición | cambiar opción/borrador |

No imprimir `Volver` o `Atras` en el pie nuevo. La flecha es un bitmap, no un
carácter Unicode que la fuente no contiene. **L interna en el encoder** indica
pulsación larga, por corrección del usuario: queda retirado el subrayado
externo usado en el primer diagrama. Recurso `input.encoder.long`, separado
del icono básico y centralizado en `assets/encoder-gestures.json`. Significa
el umbral común de800ms; definirlo una vez en introducción/ayuda.
No representar corto y largo con exactamente el mismo par de símbolos.
Al mantener se puede indicar avance de reconocimiento con feedback discreto;
no ejecutar el retorno repetidas veces. Al soltar un largo, jamás emitir OK.

Pie14px: grupo OK a izquierda, retorno a derecha. Todos sus elementos comparten
el centro de ese pie; los dos iconos de retorno están a la misma altura. Si
el contexto no tiene acción corta, omitir OK en vez de fingir una acción.
En la raíz omitir flecha, corto abre principal; no inventar un nivel anterior.
Cada página debe tener un contexto padre accesible, sin callejones.

En editor largo descarta borrador y vuelve al contexto de origen; no guarda.
En teclado descarta credenciales temporales y vuelve al padre. En proceso,
volver es navegación: cancelación/parada se modela en el servicio por separado.
Firmware/simulador actuales todavía usan largo=principal y algunas vistas
corto=volver. No afirmar que esa migración ya ocurrió ni mezclar ambos perfiles.

## 5. MenuRow y ScrollMenu

Fila16px, marco fino en foco, icono y etiqueta centrados verticalmente.
Ventana de texto recortada; solo etiqueta enfocada hace marquesina. No cambiar
foco por relleno invertido en otros submenús. Mantener indicador de bloqueo
y explicación al confirmar una opción no disponible.

Variante `with-footer`: cabecera0..13, filas16..31 y32..47; pie50..63.
Variante `full`: las tres filas heredadas16..31,32..47,48..63, sin pie.
Elegir una variante para los menús de un proyecto y mantenerla; por defecto
el perfil nuevo prioriza `with-footer` para mostrar gestos. Ambas comparten
exactamente MenuRow. La página no gana una fila sin perder otra zona.
El retorno está en el gesto largo, sin una fila de texto `Volver` duplicada.

## 6. PageDeck y Metric

Grupo de páginas con ID estable, padre común, contador fijo y orden estable.
Giro en modo lectura cambia página; corto entra en la tarea indicada. Si hay
valores editables, entrar primero al modo edición: el giro no puede cambiar
página y valor simultáneamente. Cada página muestra lo esencial para una tarea.
Un dashboard puede mostrar una o dos métricas claras; tres son un máximo de
la plantilla de texto, no una cuota a llenar. No paginar cinco u ocho sensores
solo porque la fuente principal sea grande: usar SensorBarGroup compacto.

## 7. ProgressBar: carga y progreso con la misma forma

Un componente, un marco, una altura y un relleno en todas las pantallas.
Dimensión inicial124×8, borde1px, margen interior2px; relleno interior120×4.
No otra barra redondeada o segmentada según qué servicio se esté ejecutando.
El color es monocromo, no un degradado. El atlas muestra la misma barra en dos
estados para hacer visible esta identidad.

- `determinate`: fracción conocida0..1; relleno proporcional dentro del marco.
  Mostrar porcentaje solo si el servicio puede calcularlo.
- `indeterminate`: mismo marco, bloque interno móvil; no porcentaje inventado.
  El dibujo estático del atlas muestra una fase, no prueba de animación.
  Omitir la cifra y dejar ese espacio libre: no mostrar `Sin porcentaje`,
  `Porcentaje desconocido` ni guiones a modo de porcentaje. Un texto de tarea
  como `Conectando` puede permanecer si aporta información útil.
- `paused`/`error`: conservar geometría y agregar estado identificable fuera
  del relleno. No confundir cancelado, fallido y100%.

Barra de ejemplo en y29; detalle/porcentaje bajo ella, pie fuera de su área.
Progreso de secuencia usa esta misma barra y un contador de etapa aparte.
La barra segmentada del renderer actual es antecedente, no el nuevo estándar.
No cambiar dimensiones por pantalla; si una variante compacta llegara a ser
necesaria, definirla en el registro y usarla en todos los contextos equivalentes.

## 8. SensorBar y SensorBarGroup: hasta ocho simultáneos

Referencia de AutoMicro disponible en
`AutoFinalRobo2-Atmega/reference/f4/Core/Src/oled_utils.c`:
`OledUtils_RenderIRGraphScene`, `OledUtils_DrawIRGroupFrame` y
`OledUtils_UpdateIRBars`. Agrupa línea/objetos en rectángulos49px y77px, con
barras verticales de8px y7px, respectivamente. La Web de referencia también
tiene una distribución de ocho barras sobre128px. Es lectura de fuente,
no afirmación de que esa interfaz histórica muestre ya valores abajo.

Nueva celda de familia, siempre en este orden vertical:

```text
   1       <- numero del sensor/entrada
  |#|      <- barra vertical
  |#|
  |#|
  512      <- valor actual
```

Número, barra y valor centrados sobre el mismo eje horizontal de su celda.
Las barras comparten base, altura, ancho, escala y separación dentro del grupo.
Un rectángulo exterior organiza el conjunto; no usar ocho tarjetas o grandes
iconos repetidos. El icono de sensores y unidad/escala van una vez en cabecera.

Geometría inicial con cabecera y pie completos:

- Grupo en(0,14),128×36. Área interior horizontal: x2..125, ancho124.
- Números mini3×5 en y16. Barras desde y24, alto16. Valores mini3×5 en y43.
- Para N canales, celda `i` entre `2+floor(i*124/N)` y
  `2+floor((i+1)*124/N)`, centrada; hasta8 canales en una página.
- Ancho común de barra: `min(12,floor(124/N)-6)`:12px con cinco,9px con ocho.
  Ajustar geometría solo desde tokens/perfil, no cada sensor individual.
- Cuatro dígitos mini ocupan15px, por lo que un valor0..4095 cabe en una celda
  de15/16px con ocho canales. No hace falta paginar el grupo por defecto.

La fuente mini es un recurso acotado para numeración, no para todo el texto.
Los glifos numéricos de3×5 derivan de `OLED_MiniGlyphRow` de la misma referencia.
No suponer que ya están integrados al firmware activo7×10. El atlas usa esta
variante como diagrama de adaptación. Las alturas del gráfico pueden crecer
en una variante sin pie, conservando orden y proporciones de la familia.

Si un valor necesita signo, más de cuatro dígitos o decimales, declarar el
formato y escala comunes: permitir una página de cifras ampliadas manteniendo
el gráfico completo. No recortar el número ni cambiar unidades silenciosamente.
`--` representa ausencia; cero válido conserva su etiqueta0 y barra vacía.
Compartir escala por grupo para que alturas sean comparables. No normalizar
cada sensor a su máximo observado. Orden físico estable, sin ordenar por valor.

## 9. Notice y Dialog

Mismo icono semántico para el mismo estado, título breve, detalle necesario y
pie común. No saturar un aviso de red con todo el diagnóstico. Un fallo no
desaparece por el mismo timeout que un evento informativo si sigue requiriendo
atención. Short=OK cierra/reconoce cuando proceda; largo retorna sin disparar
una acción nueva. No anunciar aceptación del hardware por una solicitud enviada.

## 10. Uso y alcance

Diseñar con estos componentes antes de agregar plantillas. Diagramar estados
normal, sin datos y bloqueado cuando corresponda. La identidad se mantiene entre
firmware y réplica al implementarlos; esta revisión solo actualiza skill,
especificación y atlas. Las muestras del atlas son datos ficticios.
