# AFR2-20261113-01 · GOTO: orientación y nodos de giro; estado de la línea IR

Versión: 1.31.0 (subversión: funciones nuevas, compatibles).
Estado: **web** fuente editada (`tsc -b` sin errores; revisado en el navegador de desarrollo sin auto); **atmega** y **esp**
solo versión. Sin CMD nuevo. Sin compilar ni cargar.

## Pedido

- En los programas con movimientos en distintas direcciones, elegir entre mantener la orientación frontal (traslación
  lateral, como ahora) o girar para encarar de frente el camino definido.
- GOTO: herramienta de giro. Clic sobre el camino agrega un nodo distinto (círculo con un rombo). Clic en el nodo abre una
  ventana flotante sobre él con la orientación del auto (vista superior con el modelo 3D), un campo de ángulo y un
  deslizador; el auto gira en vivo al cambiarlos.
- Sensores → Línea IR: si detecta la línea, si es una línea horizontal, la orientación de la línea, si está sobre la línea
  o derivando a izquierda o derecha.

## Firmware: sin cambios

El ATmega ya acepta en `robot.route.leg` un `theta` incremental: acumula el rumbo objetivo en el marco de partida, gira
mientras traslada hacia un objetivo fijo en el mundo, y un tramo x=y=0 es un giro en el lugar (AFR2-20261111-08). Por
eso todo se resuelve en la Web.

## Cambios (Web)

- `gotoSimulation.ts`
  - `GotoOrientation` (`fixed` | `path`) y `GotoTurn` (`at` = fracción del largo del comando, `headingMrad` = rumbo absoluto).
  - Cada tramo del plan tiene cuadros de pose (`frames`: tiempo, x, y, rumbo, recorrido de ruedas) y sus nodos de giro.
  - «Seguir camino» gira en el lugar si la dirección cambia más de 20°; si no, gira mientras avanza.
  - Un nodo de giro detiene, gira y mantiene ese rumbo hasta el fin del comando.
  - El tiempo de giro usa un radio equivalente de 200 mm. El simulador interpola la pose y las ruedas por cuadro.
  - Funciones nuevas: `nearestOnGotoSegment` y `headingOnGotoSegment`.
- `useGotoRouteRunner.ts` → `gotoRouteLegs`
  - Arma los tramos desde los cuadros: una traslación lleva el cambio de rumbo que ocurre mientras avanza.
  - Un giro en el lugar es un tramo x=y=0. Los giros de más de 180° se parten en partes iguales.
  - El presupuesto de 32 tramos (lápiz, aviso de envío) ya cuenta los giros.
- `GotoSimulatorPanel.tsx`
  - Selector «Orientación»; herramienta «Giro»; tocar un nodo con cualquier herramienta abre su ventana; el Borrador lo quita.
  - Arrastrar el deslizador guarda un solo paso de deshacer.
  - Al convertir un tramo de forma se conservan sus giros.
- `GotoTurnPopover.tsx` (nuevo): ventana con vista superior del modelo 3D (`SimulationRobot`), brújula con el rumbo y la
  dirección del camino, campo numérico, deslizador, quitar y cerrar (Escape).
- `ControlMap.tsx` / `ControlMapScene.tsx` / `TopMap.tsx` / `controlMapTypes.ts`
  - Marcador de giro (círculo con rombo).
  - Ventana flotante anclada al punto: se reubica al mover, hacer zoom o cambiar de tamaño, y va arriba o abajo según el espacio.
- `sensors/lineAnalysis.ts` (nuevo) y `SensorsPage.tsx`: análisis de la máscara digital de 5 IR.
  - IR1 a la izquierda, como el seguidor del ATmega.
  - Datos: detección, línea horizontal (5 sensores o 4 seguidos), posición (centrado ±0,5 sensor; la línea a la
    izquierda = el auto deriva a la derecha) y marcas separadas.
  - La orientación se estima por el desplazamiento de la línea en los últimos 1,2 s.
  - Esquema SVG de sensores y línea.

## Verificación

`tsc -b` sin errores. En el navegador de desarrollo, sin auto:

- Tramos de Adelante 1 m + Izquierda 1 m con un giro a −90° + curva + Atrás 0,5 m:
  - «Frontal»: 15 tramos, theta 0 salvo el giro.
  - «Seguir camino»: 19 tramos con giros en el lugar en las esquinas, 180° partido en 1571 + 1570 y la curva girando mientras avanza.
- El nodo aparece sobre el camino; la ventana muestra el auto 3D rotado y el campo y el deslizador se actualizan juntos.
- Línea IR: casos 00100, 00001, 10000, 11111, 10001 y 00000 dan el texto esperado; la pestaña sin auto muestra «—».

No probado en el auto (giros reales, presupuesto de tramos con muchos giros) ni con lecturas IR reales. La orientación de
la línea es una estimación y necesita que el auto avance.
