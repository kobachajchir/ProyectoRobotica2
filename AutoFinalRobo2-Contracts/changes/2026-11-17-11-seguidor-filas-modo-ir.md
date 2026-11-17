# AFR2-20261117-11 - Seguidor de línea en filas plegables, modo «Ir» y overlays con desenfoque

Autoría: Koba Chajchir / AFR2.
Versión: 1.38.0 (subversión).

Estados:
- web = fuente editada, sin compilar ni probar;
- atmega = solo versión (sin cambio de código);
- esp = solo versión (sin cambio de código).

Sin cambio de contrato UART: «Ir» y «Línea» reutilizan `robot.mode` (`idle|manual|line-follow`).

## Pedido

1. «Seguidor de línea» y «Ajuste y calibración de línea» deben ir uno debajo del otro, con chevrones que los muestren u oculten.
2. Con el seguidor activo aparece «Iniciar movimiento». Al iniciar, aparece Detener arriba y el botón de abajo pasa a ser Detener.
3. El ancho de la cinta va en «Ajuste y calibración de línea».
4. La pill «Auto detenido · lectura en vivo» va centrada, con un overlay con fade detrás.
5. Nuevo modo «Ir»: se elige desde el banner de GOTO («Cambiar a Ir») y desde la pill de modo; el seguidor pasa a modo Línea.
6. Overlays de los modales con desenfoque.
7. Donde decía GOTO, ahora dice «Ir».

## Cambios (Web)

- `LineFollowerPanel.tsx`: dos tarjetas plegables (`CollapsibleCard`) en columna. Flujo: «Iniciar seguidor de línea» (activa/prepara Línea) → «Iniciar movimiento» (+ «Desactivar seguidor») → en marcha: «Detener» arriba y «Detener movimiento» abajo.
- `lineTape.tsx` (nuevo): ancho de cinta compartido (`afr2.sensors.lineTapeMm`) entre el campo de Ajuste y calibración y la vista en vivo, que se actualiza al instante.
- `LineFollowerLiveView.tsx` y `line-follower.css`: pill centrada con degradado oscuro que se desvanece hacia abajo; sin campo de cinta en la vista.
- `armedMode.ts` (nuevo) y `useRobotMode.ts`: el firmware solo entra en GOTO o en seguimiento al iniciar el movimiento, así que «Ir» y «Línea» envían Manual (habilita motores, sin consigna) y la Web recuerda el modo elegido. Se suelta al perder el enlace, ante Fallo/Secuencia/Prueba de rueda o si el auto hace la otra tarea. Nunca inicia movimiento.
- `RobotModePill.tsx`: cuarta opción «Ir»; «Línea» ahora prepara el seguidor en vez de iniciarlo.
- `GotoSimulatorPanel.tsx`: banner «Para usar Ir, el auto debe estar en modo Ir» con «Cambiar a Ir».
- `workspace.css`: el `::backdrop` del modal también desenfoca (el scrim ya lo hacía).
- GOTO → «Ir» en pestañas, pill, simulador y pruebas mínimas. Los identificadores y el comando `robot.goto.start` no cambian.

## Pendiente

- Compilar y probar en el auto: Manual como preparación de Ir/Línea, y el desenfoque en el navegador del usuario.
- Si se quiere un modo Ir/Línea persistente en el firmware (y en el OLED), hace falta un cambio de contrato (`robot.mode`) y de ATmega/ESP.
- La versión 1.37.0 estaba en curso con AFR2-20261117-10 (sincronización); esta ficha sube sobre ella.
