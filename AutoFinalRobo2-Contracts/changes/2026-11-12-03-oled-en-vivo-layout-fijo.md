# AFR2-20261112-03 · Pantalla en vivo: layout fijo y «Al cambiar» solo por pantalla

Versión: 1.28.2 (subversión mínima: ajuste, sin función nueva).
Estado: **web** fuente editada (`tsc -b` sin errores; revisado en el navegador de desarrollo sin auto conectado);
**atmega** y **esp** solo versión. Sin CMD nuevo. Sin compilar ni cargar.

## Pedido

- Al conectar y desconectar, el layout de la OLED en vivo se mueve; con la conexión establecida los elementos no deben moverse.
- En «Al cambiar», pedir y renderizar solo cuando cambia la pantalla (screen code), no cuando se mueve la selección o el scroll.
- El lienzo de 128×64 debe quedar siempre visible y no colapsarse.

## Causa

- Avisos (`StatusNotice`) que aparecían y desaparecían según enlace, error o caducidad, empujando la pantalla.
- La pill de captura y el pie solo existían con captura; el botón cambiaba de texto («Sincronizando…»).
- La columna lateral de selección solo existía en menús: al pasar a una vista el lienzo tomaba todo el ancho y al volver se achicaba.
- La grilla `minmax(0,1fr) auto` permitía que el lienzo se encogiera casi a cero; el canvas no mantenía su proporción.
- La clave de «Al cambiar» incluía selección y fila visible, así que cada movimiento pedía otra captura.

## Cambios (Web)

- `LiveOledViewer.tsx`: un único aviso con prioridad (sin enlace → sin captura remota → error → caducada) en un renglón de
  altura fija (dos líneas, el texto completo en el `title`); pill de captura y pie siempre presentes; botón con texto fijo.
  Clave «Al cambiar» = `screenCode:navigationRevision`. Antes de una entrada remota en «Al cambiar», si la selección o la
  ventana informadas por el auto difieren de la captura, se toma una nueva y se pide repetir la entrada.
- `OledWorkbench.tsx`: opción `reserveSide` que mantiene la columna lateral aunque la pantalla no tenga filas.
- `OledCanvas128x64.tsx`: el canvas ocupa el ancho disponible con relación 2:1, hasta su tamaño nativo.
- `workspace.css`: lienzo con mínimo 128×64 px (256×128 cuando entra); fila pantalla + columna de 7,5 rem que en angosto
  baja debajo; aviso, pill y pie con tamaño estable.

## Verificación

- `tsc -b` sin errores. En el navegador de desarrollo, sin auto: a 375 px el lienzo mide 269×134 y la columna baja; a 480 px,
  359×180; sin desplazamiento horizontal. No se probó con capturas reales ni con conexión y desconexión del auto.
