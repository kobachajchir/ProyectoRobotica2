# Libro de láminas

`componer.py libro.json salida.pdf` requiere reportlab yPillow. El JSON usa `project`, `date`, `source`, `author` confirmado y `sheets`.

Cada hoja: `code`, `title`, `kind` (`part`, `assembly`, `exploded`, `general`), `render` (ruta absoluta), `material`, `notes` (lista), y opcionalmente `views` (vistas con `title`, `bounds`=[xmin,ymin,xmax,ymax], `lines` con `a`, `b`, `hidden`), `bom` (filas con `id`, `label`, `qty`), `diameters` y `mass_note`.

Para una ficha general visual, `render_cards` reemplaza las vistas ortogonales de la izquierda por hasta seis renders identificados mediante `code`, `title` y `render`. `cards_title` titula el panel; `main_label` titula el render principal. Dividir hojas si no caben todos los conjuntos. La ficha no sustituye los planos individuales ni se presenta como acotado de fabricacion.

Las vistas emplean mm de modelo y proyección explícita en el libro. El compositor ajusta escala común a1:1,1:2,1:4,1:5,1:10,2:1,4:1 según espacio. Las cotas envolventes se consideran preliminares: el adaptador CAD debe aportar cotas funcionales adicionales antes de declarar fabricación.

El bloque de notas y el rótulo están reservados y no reciben geometría. El3D recorta márgenes alfa antes de encajar; las explosiones usan272mm de ancho y laBOM una columna de103mm, sin invadir planos. Los desplazamientos y ángulos se definen en el adaptadorCAD, no en el compositor. Cuando hay más filas deBOM o texto que el espacio admite, producir hojas adicionales en el adaptador; el compositor no debe ocultar información silenciosamente.
