# AFR2-20261112-04 · Burbuja de acción del brazo

Versión: 1.29.0 (subversión: función visual nueva, compatible).
Estado: **web** fuente editada (`tsc -b` sin errores; revisado en el navegador de desarrollo); **atmega** y **esp** solo
versión. Sin CMD nuevo. Sin compilar ni cargar.

## Pedido

Un botón para ver la burbuja de acción del brazo, con centro en su base y tipo malla, que se pueda activar o no.

## Cambios (Web)

- `armKinematics.ts`: `armReach()` (cota máxima: suma de los eslabones + TCP del modelo activo) y
  `armBubble(environment)` (centro en el montaje del brazo, independiente o sobre el auto, en el marco Three).
- `collectorSimulation.ts`: `bubble` en el estado del simulador, apagado por defecto; se conserva al regenerar cargas.
- `CollectorScene.tsx`: `ReachBubble`, esfera de 32×16 divisiones dibujada como meridianos y paralelos (`EdgesGeometry`,
  sin diagonales), velo interior al 5 %, cortada en el piso (ángulo polar `acos(−h/r)`) con un anillo en el suelo. Va dentro
  del grupo del auto, así en el Conjunto se mueve y gira con él. Cambia con el modelo del brazo activo.
- `CollectorPage.tsx`: botón «Burbuja» (icono brazo, `aria-pressed`) junto a «Alcance», en Brazo robótico y Conjunto.

## Alcance de la burbuja

Es una cota geométrica (todas las articulaciones estiradas), no el volumen que el brazo alcanza de verdad ni una prueba de
cinemática inversa. La nube de puntos «Alcance» sigue mostrando las tomas verticales válidas.

## Verificación

`tsc -b` sin errores. En el simulador Brazo robótico la burbuja aparece y desaparece con el botón, centrada en la base y
cortada en el piso. No se revisó el Conjunto con el auto en movimiento.
