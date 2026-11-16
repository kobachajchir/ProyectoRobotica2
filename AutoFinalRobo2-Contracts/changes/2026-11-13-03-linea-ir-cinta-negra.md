# AFR2-20261113-03 · Línea IR: cinta negra bajo el modelo y ancho configurable

Versión: 1.31.2 (subversión mínima: ajuste visual).
Estado: **web** fuente editada (`tsc -b` sin errores; revisado en el navegador de desarrollo sin auto); **atmega** y **esp**
solo versión. Sin CMD nuevo. Sin compilar ni cargar.

## Pedido

Abajo del modelo 3D, mostrar la cinta negra cuando se detecta la línea. El ancho se puede configurar; normalmente 18 mm.
Dejar el lugar para configurarlo.

## Cambios (Web)

- `sensors/LineTopView.tsx`
  - La línea es una cinta negra en el piso, debajo del modelo: queda tapada por el chasis y asoma delante y detrás.
  - Se dibuja sobre una pista clara de 1,2 m para que contraste.
  - Se quitó la cinta transparente sobre el chasis; los cinco IR siguen encima.
  - Prop `tapeMm`: 18 mm por defecto, de 5 a 60 mm.
- `SensorsPage.tsx`
  - Campo «Ancho de la cinta (mm)» debajo del modelo, con aviso fuera de rango.
  - Se guarda en este navegador (`afr2.sensors.lineTapeMm`). Solo cambia el dibujo; no afecta la lectura ni el firmware.
- `sensors.css`: columna del modelo con el campo debajo.

## Verificación

`tsc -b` sin errores. En el navegador, sin auto, con una lectura de prueba temporal (IR2 e IR3), la cinta negra de 18 mm
aparece bajo el auto, delante y detrás; con 40 mm se ensancha y el valor queda guardado. El código de prueba se quitó y el
valor guardado se borró. No probado con lecturas reales.
