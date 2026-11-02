# Plan de skill: láminas técnicas de ensambles y piezas

> **02/11/2026 — Ejecucion autorizada por el usuario.** Este texto queda como antecedente de planificacion. Ya existe [skill implementada](../../AutoFinalRobo2-Contracts/.agents/skills/fusion-technical-sheets/SKILL.md) y [primera tanda de13 laminas](../mecanica/brazo-antropomorfo/revision_v17/laminas_20261102/README.md). Composicion final confirmada por referencia: planos izquierda,3D derecha, notas inferiores en bloque propio a la izquierda del rotulo. No significa que todos los planos del brazo esten completos.

Pedido del usuario del 27/10/2026. **Solo planificación:** no generar todavía despieces, láminas de fabricación ni dibujos del brazo. El usuario indicará cuándo empezar y podrá aportar más modelos. Referencia visual recibida: [lámina aportada](../reference/brazo-prototipo/referencia_lamina_tecnica_usuario_20261027.png).

## Resultado esperado

Skill propuesta: `fusion-technical-sheets`. Preparar láminas técnicas desde un maestro CAD identificado, con **vistas y cotas a la izquierda** e **isométrica 3D o despiece a la derecha**, más notas y rótulo inferior. Trabajar con un modo de ensamble y otro de pieza; conservar los vínculos entre identificadores de piezas, globos del despiece y lista de materiales.

La referencia define la composición visual. Sus nombres, autorías, dimensiones, masa, normas, tolerancias, material, notas e institución no se copian al proyecto como si fueran datos propios. Se tomarán del CAD, de fuentes verificadas o de valores aportados para cada entrega; los faltantes quedan explícitos.

## Modo ensamble

Una lámina por conjunto que se arma por separado: montura/base, giro J1 y torre, eslabón hombro–codo, antebrazo/codo, giro de muñeca J4, inclinación J5, giro de herramienta J6 y pinza. Afinar estas divisiones con la lista real de fabricación antes de numerar; una pieza común no cambia de código por aparecer en varias láminas.

- Izquierda: cuadros de las piezas **fabricadas** del conjunto, cada cuadro con código, cantidad, material y vistas ortogonales acotadas pertinentes. Los comerciales se identifican por norma/modelo y cotas de interfaz cuando sean necesarias.
- Derecha: despiece isométrico del conjunto, piezas separadas según sus ejes de desmontaje, globos correlativos y líneas de recorrido discretas; orientación consistente con las vistas.
- Parte inferior: lista de materiales, fijaciones, orden de montaje cuando influye en el acceso, notas y rótulo de revisión.
- Si las piezas no caben con cotas legibles, usar varias hojas vinculadas. No reducir todo para forzarlo en una sola plancha. Una hoja general puede mostrar todos los cuadros y remitir a los planos individuales.

## Modo pieza

Una lámina por pieza única fabricada, con cantidad total y conjuntos donde aparece. Izquierda: alzado, planta y perfil según geometría, más cortes/detalles solo donde explican alojamientos, ejes o pasos ocultos. Derecha: isométrica sombreada del sólido real. Rótulo con código/revisión, título, material/proceso, unidades, escala y autoría confirmada. La isométrica llevará su propia escala o «sin escala».

Acotar desde datums funcionales: distancia entre ejes, patrón y diámetro de agujeros, asientos de rodamientos, secciones D, largos de corte de PVC, solapes, espesores y accesos a fijaciones. Evitar cadenas redundantes. No inferir diámetros o longitudes midiendo una imagen. No confundir agujeros con número de caras cilíndricas del BRep: identificar agujeros físicos únicos y anotar «cantidad × diámetro» cuando corresponda.

Los ajustes de impresión de PVC/rodamientos se muestran como cotas de diseño con su estado de calibración. No declarar tolerancias ISO, ajuste H7, rugosidad, masa real o conformidad normativa sin datos. Si se muestra una masa calculada, indicar material/densidad utilizados y «masa CAD estimada».

## Formato y edición

Predeterminado A3 apaisado, márgenes y rótulo común; proporción aproximada 52 % para planos y 48 % para 3D, ajustable según el conjunto. Fondo blanco, líneas negras, jerarquía de grosores y acentos discretos para encabezados. Geometría 2D vectorial, texto y cotas legibles impresos a tamaño real. Estilo de proyección elegido y símbolo correspondiente coherentes en todas las hojas; no copiar el símbolo de la referencia sin adoptar esa convención.

Preferir dibujos asociativos nativos de Fusion para cotas y vistas. Si la API no permite alguna operación de Drawing, usar la interfaz de Fusion o un flujo CAD vectorial que conserve origen y medidas; documentar la limitación. Una captura raster puede servir para la isométrica, no sustituye las vistas acotadas de fabricación. No generar ilustraciones mediante IA para representar una pieza técnica real.

Entregables al autorizar la ejecución: fuente editable del dibujo/maquetación, PDF vectorial por hoja y PDF índice del conjunto; opcionales SVG/DXF cuando el usuario los necesite. Cada salida registra maestro, revisión, unidades, convención de proyección, código de pieza, fecha y procedencia de cotas. Los despieces se crean en documento derivado o presentación independiente: nunca se desplazan las piezas del maestro armado para obtener la imagen.

## Recursos previstos de la skill

```text
fusion-technical-sheets/
  SKILL.md                       Selección de modo, entradas y límites
  references/assembly-sheets.md   Cuadros, despiece, globos y BOM
  references/part-sheets.md       Datums, vistas, cortes y cotas funcionales
  assets/title-block.svg         Rótulo editable sin autoría inventada
```

Agregar scripts de extracción/maquetación solo cuando exista un flujo real probado; no crear ejecutables de ejemplo ni prometer operaciones API no disponibles. La skill se instalará cuando se concrete su implementación; este documento no es una skill instalada.

## Secuencia de uso futura

1. Identificar maestro y revisión, inventariar piezas fabricadas/comerciales y elegir ensamble o pieza.
2. Revisar las referencias gráficas aportadas y resolver los datos del rótulo que falten.
3. Extraer geometría y dimensiones nativas, definir códigos y hojas; si se pidió solo plan, detenerse aquí.
4. Generar el despiece derivado y las vistas ortográficas con cotas; conservar referencias al CAD.
5. Componer las hojas y cotejar BOM/globos/códigos, ausencia de cotas cortadas, escala y legibilidad. Hacer comprobaciones solo dentro de la autorización de esa tarea.
6. Entregar PDF y fuentes con las limitaciones pendientes, sin declarar aptitud para fabricación a partir de una imagen.

Cuando el usuario diga que comencemos, la primera entrega será una hoja de ensamble representativa y una hoja de pieza para ajustar el formato con sus modelos; después se extenderá al resto conservando códigos y rótulo.
