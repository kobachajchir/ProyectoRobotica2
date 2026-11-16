# AFR2-20261113-02 · Línea IR con el modelo 3D en vista superior

Versión: 1.31.1 (subversión mínima: ajuste visual).
Estado: **web** fuente editada (`tsc -b` sin errores; revisado en el navegador de desarrollo sin auto); **atmega** y **esp**
solo versión. Sin CMD nuevo. Sin compilar ni cargar.

## Pedido

En el estado de la línea IR (AFR2-20261113-01), usar el modelo 3D en vista superior en lugar del esquema dibujado.

## Cambios (Web)

- `sensors/LineTopView.tsx` (nuevo)
  - `SimulationViewport` con cámara superior (frente hacia arriba, como el mapa del GOTO) y `SimulationRobot`.
  - La barra de sensores sale del nodo `AFR2_SensorLinea_5IR` del CAD, con el mismo desplazamiento del modelo. Los IR se
    reparten en 5 partes iguales de su ancho (unos 16 mm). IR1 queda a la izquierda.
  - El sensor está bajo el chasis: los IR se dibujan encima, sin prueba de profundidad, sobre una placa de fondo. Encendido = punto lleno.
  - La línea es una cinta de 19 mm, en el piso y en transparencia sobre el chasis: desplazada por la posición, inclinada
    unos 18° según la orientación estimada y atravesada si es horizontal.
  - Rótulos «IR1 · izq.» y «der. · IR5». Spinner hasta que baja el modelo.
- `SensorsPage.tsx`: `LineStatus` usa `LineTopView`; se quita el SVG.
- `sensors.css`: estilos de la vista; se quita el del esquema.

## Verificación

`tsc -b` sin errores. En el navegador, sin auto: modelo con los cinco IR apagados en el frente. Con una lectura de prueba
temporal (00011 con la línea corriéndose a la izquierda), IR1 e IR2 se encienden y la línea queda a la izquierda e
inclinada; ese código de prueba ya se quitó. No probado con lecturas reales; la separación entre IR es una inferencia del
ancho del CAD, no una medida.
