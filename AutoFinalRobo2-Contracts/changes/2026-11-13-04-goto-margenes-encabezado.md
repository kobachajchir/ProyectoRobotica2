# AFR2-20261113-04 · GOTO: margen y padding del encabezado como en Secuencias

Versión: 1.31.3 (subversión mínima: ajuste visual).
Estado: **web** fuente editada (revisado en el navegador de desarrollo); **atmega** y **esp** solo versión. Sin compilar ni cargar.

## Pedido

En el Simulador GOTO el selector de vista estaba pegado al título. Ordenarlo con el mismo margen y padding que la
Simulación de la secuencia.

## Causa

El GOTO usa el mapa en modo compacto (`data-compact`), y `control-refinements.css` lo sobrescribía con más especificidad:

| Elemento | Compacto (antes) | Común |
|---|---|---|
| Encabezado | 16 px 20 px 8 px | 24 px |
| Título | 20 px | 18 px |
| Selector de vista | 0 arriba, 20 px a los lados, 12 px abajo | 14 px 20 px |

Además, las herramientas de dibujo no tenían margen arriba.

## Cambios

`control-refinements.css`:

- El compacto conserva solo el `gap`; el encabezado, el título y el selector usan las reglas comunes.
- Herramientas de dibujo con `padding: 16px 20px` (16 px en celular).

## Verificación

En el navegador se midieron los dos simuladores:

- Encabezado 24 px, título 18 px, selector 14 px 20 px y 14 px entre el borde del encabezado y las pills, iguales en ambos.
- Herramientas del GOTO 16 px 20 px.
- A 375 px no hay desplazamiento horizontal.
