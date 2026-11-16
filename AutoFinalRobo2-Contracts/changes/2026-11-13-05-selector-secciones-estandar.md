# AFR2-20261113-05 · Selector de secciones de página estandarizado

Versión: 1.31.4 (subversión mínima: ajuste visual).
Estado: **web** fuente editada (`tsc -b` sin errores; medido en el navegador de desarrollo); **atmega** y **esp** solo
versión. Sin compilar ni cargar.

## Pedido

El selector de pills de las secciones tenía padding y márgenes distintos en cada página. Revisar todas y estandarizar.

## Relevamiento (escritorio, antes)

| Página | Encabezado → selector | Selector → contenido |
|---|---|---|
| Programas, Puesta a punto | 20 px | 36 px (relleno superior del panel) |
| Sensores, Ayuda, Protocol Studio | 32 px | 16 px |
| Conexión | tarjeta de modo Wi-Fi en medio | 16 px |
| Simulador | dentro del encabezado | — |

En todas, las pills eran iguales: tamaño md, 52 px de alto, relleno 0 16 px, 13 px de texto, ancho completo en celular.
Inicio, Conducir, Modelos, Armado y Desarrollador no tienen selector. En Infraestructura y Loader el selector va dentro
de una sección, no debajo del encabezado; no se tocó.

## Cambios

- `workspace.css`
  - Encabezado → selector: la separación de sección de la página (`--visual-section-gap`: 32 px en escritorio, 28 px en
    celular), también en las páginas de control.
  - Selector → contenido: `--afr2-task-nav-after`, 24 px (20 px en celular).
  - Se quitó el relleno superior extra del panel de control.
- `SimulatorPage.tsx`: el selector Auto/Brazo/Conjunto sale del encabezado y usa la misma estructura (`afr2-workspace` +
  `afr2-task-nav`).

## Verificación

Medido en el navegador en Programas, Puesta a punto, Sensores, Conexión, Ayuda, Protocol Studio y Simulador (brazo y
conjunto):

- Escritorio: 32 px desde el encabezado (Conexión: después de la tarjeta Wi-Fi) y 24 px hasta el contenido, alineado
  con el encabezado.
- 375 px: 28 px y 20 px, ancho completo y sin desplazamiento horizontal.
