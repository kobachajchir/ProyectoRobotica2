# AFR2-20261110-05 — Recolección: escaneo por paradas y toma por distancia frontal

Versión: **1.15.0**, conservada como corrección de la misión AFR2-20261110-03 aún sin cargar (regla 3).

Aclaración del usuario sobre el ciclo de «Iniciar recolección» en el simulador: se mueve y escanea, se mueve y escanea; luego se posiciona lo más de frente posible, agarra la caja midiendo hacia adelante a cuánto está y la lleva al drop.

## Cambios en `collectorSimulation.ts`

- **Escaneo por parada:** cada parada hace un barrido de 180° delante y, si no detecta, un único giro de 180° y barrido detrás (antes: tres giros de 90° con cuatro barridos). Sin detección, avanza al siguiente punto de patrulla y vuelve a escanear.
- **Centrado frontal:** al detenerse para tomar, si la caja queda desplazada más de 4 mm del eje frontal, el auto corrige con desplazamiento lateral Mecanum (máx. 30 mm/s) y vuelve a medir cada 100 ms. Timeout de 6 s con falla explicada.
- **Toma por distancia medida:** el punto de agarre ya no usa el centro exacto que entrega el detector virtual. Se calcula con el eco frontal: origen del sensor (103 mm) + distancia medida a la cara + media caja (25 mm), recto al frente del auto. El texto de estado informa la distancia en mm. La identidad del objeto se sigue usando solo para confirmar que es la misma caja.

Transporte, entrega en el drop, contador y búsqueda de la siguiente caja sin cambios.

## Corrección: «El modelo activo no tiene una toma frontal válida para esta carga»

Reportado por el usuario al iniciar la recolección. Causa (diagnóstico numérico sobre el descriptor `pvc25-v17-blanco.json`): con la muñeca hacia abajo, el V17 montado solo alcanza el centro de una caja en el suelo (TCP a 25 mm) hasta unos 165 mm delante del origen del auto, y ahí la palma (118 mm) cae dentro de la envolvente del chasis para cualquier giro de muñeca. Además, el giro de muñeca se limitaba a −45°…45° y el auto llegaba en diagonal a la caja. Ninguna distancia de la lista fija resultaba válida.

- `armKinematics.ts`: `GRASP_ABOVE_CENTER` = 20 mm, así que la caja se toma por su parte superior (TCP a 45 mm). `pickupSolutions` prueba la muñeca alineada con la caja y ±90°, y devuelve una única muñeca libre de choques para aproximación, toma y elevación. Con el V17 la ventana válida es TCP a 170–177,5 mm, con mordazas cruzadas al auto.
- `collectorSimulation.ts`:
  - La distancia de acople es el centro de la ventana válida.
  - El auto encara la cara de la caja más cercana a su rumbo, con orientación múltiplo de 90°. Si esa cara está ocupada, prueba las otras.
  - La llegada al drop también se cuadra.
  - Toma, elevación y descarga usan las posturas de `pickupSolutions`.
  - La caja sujeta se dibuja 20 mm por debajo del TCP. El agarre manual y la confirmación de entrega usan ese centro.
  - Al medir de nuevo, si la caja queda fuera de la ventana, el auto avanza o retrocede midiendo hasta la distancia de toma (timeout de 6 s).
  - Los giros en el lugar comprueban el barrido de la huella. Si un sentido choca, gira por el otro; si los dos chocan, se desplaza atrás, de costado o adelante.
  - La planificación de rutas usa un margen de 10 mm alrededor de la base.
  - Si aparece otra caja más cerca delante durante la aproximación, pasa a tomar esa en lugar de detenerse.

Diagnóstico sin interfaz: un script del scratchpad, fuera del repositorio, empaquetó la misión con esbuild y la ejecutó sobre 13 semillas (6 cajas cada una). Las 13 entregaron las 6 cajas. No es una prueba del proyecto ni una compilación de producto, y no se revisaron tipos ni el navegador.

## Ciclo continuo hasta vaciar el mapa

Pedido del usuario: el ciclo se repite solo, sin detenerse, hasta que no queden cajas. Recorre el mapa escaneando, elige una opción, intenta acercarse, mide de nuevo, se posiciona, agarra, lleva al drop, sale del drop y vuelve a buscar.

- **Patrulla cíclica:** los 16 puntos se recorren en orden y vuelven a empezar al terminar; antes la misión se detenía al acabar la lista. Los puntos sin ruta libre (con margen de 10 mm) se saltean.
- **Acercamiento en dos etapas:** si la caja elegida está a más de 550 mm, el auto se acerca hasta quedar a 400 mm (fase `scout`, «Acercándose a la caja»), se detiene y vuelve a barrer. Después elige de nuevo la caja más cercana y recién entonces se encuadra frente a una cara y se acopla.
- **Salida del drop:** después de cada entrega, el auto se aleja hasta 350 mm por fuera del círculo, del lado del centro del mapa (fase `leave-drop`), y barre de nuevo antes de seguir la patrulla.
- **Sin detenerse por una caja difícil:** si una caja no tiene cara libre o ruta, se pierde al medir, cambia la medición o no se logra centrar o ajustar la distancia, queda apartada y la búsqueda sigue. Las apartadas vuelven a intentarse después de cada entrega. Si la patrulla da dos vueltas sin nada alcanzable, primero reintenta las apartadas y solo después pide intervención.
- La misión termina únicamente cuando no quedan cajas en el suelo. Siguen deteniéndola la falta de ruta al drop, un choque durante el movimiento y la falta de toma frontal válida en el modelo.
- La acción sobre una caja seleccionada (modo de una sola entrega) conserva el comportamiento de detenerse con explicación.

Diagnóstico sin interfaz (script del scratchpad, fuera del repositorio): 16 escenas, 13 con 6 cajas, 2 con 10 y 1 con 12. Todas terminaron en «Todas las cargas se entregaron en el drop». Secuencia observada en la semilla 42: alinear › aproximar › medir de nuevo › tomar › elevar › transportar › bajar › soltar › recoger › salir del drop › barrer › acercarse › barrer › aproximar… No es una prueba del proyecto ni una compilación, y no se revisaron tipos ni el navegador.

Estado: Web=fuente; ATmega/ESP/CAD/Loader=sin cambios. Sin tests del proyecto, compilaciones, validaciones, navegador ni flasheo.
