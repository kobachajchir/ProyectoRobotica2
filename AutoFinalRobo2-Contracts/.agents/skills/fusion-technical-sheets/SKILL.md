---
name: fusion-technical-sheets
description: Crear laminas tecnicas uniformes de piezas y ensambles CAD con vistas ortogonales acotadas, render 3D, BOM, rotulo y despieces en linea. Usar para documentacion mecanica desde geometria real, no ilustraciones inventadas.
---

# Láminas técnicas desde CAD

Usar la geometría nativa identificada por documento/ID/revisión. Comprobar primero si ya existe una captura documental y qué cambios tuvo el maestro. La autorización para documentar no implica modificar el diseño ni ejecutar análisis mecánicos.

## Composición

Predeterminado A3 horizontal, milímetros, fondo blanco y líneas negras; acento discreto en títulos. **Planos ortogonales a la izquierda, modelo 3D a la derecha.** Respetar cualquier distribución que el usuario cambie explícitamente.

Reservar una franja inferior independiente antes de colocar geometría:

- Bloque de notas a la izquierda, con su propio marco/contenedor, ajuste de líneas y límites de altura.
- Rótulo a la derecha, sin texto de notas encima ni dentro. Código, título, revisión, fecha, material/proceso, unidades, escala por vista, hoja/total y estado documental.
- Separación mínima4mm entre notas y rótulo. Texto normal mínimo2,5mm; cotas mínimo2,5mm. No reducir indefinidamente para encajar: dividir hojas.

### Visibilidad del modelo3D y del despiece

Ajustar la escala visual del render para que la geometría ocupe bien el espacio disponible y sus elementos se distingan. Recortar márgenes transparentes o vacíos antes de encajar la imagen; aumentar resolución no compensa mostrar una pieza diminuta. La escala de presentación del3D es independiente de la escala métrica de las vistas acotadas.

No imponer un reparto rígido50/50. Un despiece puede ocupar más de media hoja y tomar espacio libre de otras zonas, siempre que no se superponga a planos, cotas, textos, notas o rótulo. Reservar cajas de contenido disjuntas; mover laBOM a una columna más estrecha o a otra hoja si hace falta ampliar la explosión.

Priorizar perspectiva isométrica u otro ángulo oblicuo que muestre profundidad y relaciones de montaje. No presentar por defecto las piezas como una hilera plana de objetos uno junto al otro. Mantener orientaciones y relaciones espaciales del ensamble y separar componentes por ejes de montaje conocidos; identificar como documental cualquier desplazamiento de visualización no validado como trayectoria de desmontaje.

Los tornillos, insertos y rodamientos deben seguir siendo identificables. Si una pieza pequeña queda demasiado chica junto a la carcasa, generar un detalle ampliado o una hoja/tramo adicional, indicando que tiene otra escala visual; no agrandar solo esa pieza dentro del mismo ensamble sin avisar. Los globos deben quedar fuera de la geometría, con líderes claros.

El rótulo admite autor/dibujante confirmado por el usuario. Para las láminas deAutoFinalRobo2, incluir **Koba Chajchir** como responsable de documentación; esto no implica firma de aprobación o fabricación. En otros proyectos usar el nombre aportado, sin reutilizar automáticamente esta autoría.

La lámina de referencia define composición, no autoría, normas, ajustes, masa, datos o institución. No copiar esos campos de un tercero.

## Modos

**Conjunto:** vista armada 3D y plano general; BOM vinculada a códigos. Separar BOM larga en hojas. Identificar comerciales por modelo/norma conocida; agrupar una ocurrencia comercial formada por varios sólidos como una sola pieza.

**Pieza:** una hoja por definición fabricada única, con cantidad y conjuntos donde aparece. Alzado/planta/perfil con proyección declarada, isométrica sombreada y cotas funcionales desde datums. Si solo hay cotas envolventes, declarar “preliminar, acotado funcional pendiente”; no presentarla como plano completo para fabricar.

**Explosión:** derivado independiente del maestro, preferentemente isométrico, grande y legible. Para desmontaje real, desplazamientos sobre ejes de montaje y líneas de recorrido. Un catálogo alineado solo se utiliza cuando el usuario lo pide; si reubica piezas laterales, rotular “despiece lineal de inventario, no secuencia de desmontaje”. Dividir los conjuntos densos en tramos/detalles; no ocultar tornillería ni confundir cantidad con sólidos internos. Globos y BOM deben compartir identificadores.

## Obtención geométrica

Preferir Drawing asociativo nativo si las herramientas disponibles lo permiten. Si no, conservar una fuente vectorial reproducible derivada de aristas BRep y documentar que no es un Drawing nativo asociativo. Para isométricas usar render del sólido real, nunca generación de imagen por IA.

Las cotas salen de geometría nativa o parámetros, no de píxeles. Si se usa una malla para visibilidad, declarar tolerancia y limitación. Las aristasBRep no incluyen todas las siluetas: generar también los contornos aparentes de cilindros y superficies curvas para no dejar vistas abiertas. Diferenciar contornos visibles, ocultos y ejes. Diámetros de superficies cilíndricas no equivalen a cantidad de agujeros: no contar caras como perforaciones. Mostrar escala exacta de las vistas medidas; la isométrica puede estar “sin escala”.

No asignar tolerancias ISO, ajusteH7, propiedades FDM, infill o aprobación de fabricación sin evidencia. “Masa CAD” debe incluir método/densidad y no se rotula masa real.

## Flujo y entrega

1. Leer referencia y restricciones locales. Inventariar fabricados/comerciales y establecer códigos persistentes.
2. Capturar geometría y procedencia; excluir referencias ajenas al alcance aunque estén ocultas. No mover el maestro para explotar.
3. Producir láminas y fuentes editables. El compositor opcional `scripts/componer.py` recibe un libroJSON con vistas vectoriales y rutas de render; usarlo solo cuando ya exista esa extracción, no como generador CAD.
4. Entregar PDF, índice/BOM y fuentes, indicando qué hojas están completas y qué cotas/interfaces faltan. Las revisiones gráficas o técnicas se ejecutan según la autorización del usuario; no iniciar tests, FEA o comprobaciones físicas por aplicar esta skill.

La guía de campos del libro está en `references/libro.md`. Esta skill no exige una aprobación intermedia para producir la primera tanda autorizada.
