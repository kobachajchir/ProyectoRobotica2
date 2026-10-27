# AFR2-20261027-05 — Ancho util, tarjeta de simulacion y Control agrupado

27/10/2026. Pedido humano: aprovechar el ancho disponible de toda la aplicacion,
unificar el espacio de simulacion, compactar numeros vinculados a sliders y
reorganizar las tareas de Control sin perder sus funciones.

## Resultado en fuente

- Se elimina el limite global de1120 px y el centrado con margenes automaticos.
  Relleno lateral clamp(12px,1.25vw,24px), manteniendo el espacio de la barra
  lateral y las reglas moviles. Modelos3D conserva su interfaz interna.
- Cada simulacion usa una sola tarjeta: visor, leyenda, lecturas, matematica y
  controles. La matematica queda adosada al visor con divisor lateral; su
  chevron pliega el cuerpo y reduce la columna a52 px, ampliando el visor.
- Los controles y sus pills Recolectar/Posiciones/Brazo/Auto pasan debajo del
  renderizador. Se elimina el sidebar de altura fija con scroll interno. Brazo
  y gripper se distribuyen en dos zonas, y movimiento/motores tambien. Los
  listados de cubos fluyen en grilla, sin un segundo scroll vertical interno.
- Se corrige la causa de los campos altos: la regla global de48 px excluye los
  numeros compactos asociados a sliders. En simulaciones y sensores esos campos
  usan22 px de altura y la tipografia del titulo, conservando el valor/barra
  vinculados y los limites/confirmacion existentes de cada componente.
- Las pills de motores diferencian los botones Atras/Adelante con fondo y borde
  dentro de la pill mayor; nombre del motor al centro.
- Control agrupa Conduccion manual y Secuencias bajo Movimiento, como dos tarjetas
  plegables; Prueba de rueda y Calibracion quedan bajo Pruebas y calibracion.
  GOTO se conserva. Los campos, handlers, control tactil y flujos se reutilizan.
  Al cerrar/cambiar una tarjeta se corta la consigna manual y se cierra su overlay;
  el desmontaje ejecuta la limpieza propia de cada editor igual que las tabs previas.
- Reposo/Manual/Linea se muestran como pills separadas, con la seleccion vigente
  de la interfaz. No se cambia la semantica de confirmacion de robot.mode.

## Plegables y movimiento

CollapsiblePanel es una divulgacion controlada con boton nativo, aria-expanded,
aria-controls, id estable y Chevron compartido. El texto no gira. Solo el
chevron indica estado: transition transform180 ms, ease-out existente. Las
activaciones por teclado y prefers-reduced-motion cambian inmediatamente.
No se anima el ancho de columnas, lecturas numericas ni la matriz.
Proposito: indicacion de estado en una accion ocasional; sin nueva dependencia.

## Estados y limites

Web: fuente implementada, sin tests, compilacion, validadores ni navegador.
Contracts: tokens/decision/cobertura actualizados. Atmega/ESP/CAD: sin cambios de
esta ficha; data/dist sin promover. Sin nuevos comandos, wire, capacidades,
pantallas OLED ni movimientos fisicos ejecutados. Sin commit/push.

El pedido actual autoriza reorganizar visualmente Control y sustituye la
restriccion anterior de conservar su disposicion exacta. Sus servicios y
guardas de movimiento no se reescriben. Los estados desconocidos no se convierten
en confirmacion fisica. El modelo final del brazo y la foto de la burbuja siguen
pendientes, sin cambios de Modelos3D de esta ficha.

Procedencia: implementacion propia, reutilizando TaskSections, Chevron, controles
y servicios existentes. VIS-042 con identidad0.1.9 autorizada y skill animate
para el giro; aspecto No verificado. Cambios concurrentes ajenos preservados.

Pendiente por pedido expreso: observacion de escritorio/movil, plegado rapido y
teclado/reduced motion, numeros de slider, espacio recuperado al plegar matriz,
control tactil/secuencias y acceso a pruebas/calibracion/GOTO. No se ejecutaron.
