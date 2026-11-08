# AFR2-20261107-42 - Control táctil: paneles cerrados, joystick y animaciones

Pedido del usuario: mejorar la interfaz y las animaciones de movimiento del control táctil, y que los paneles colapsables aparezcan cerrados por defecto.

## Cambios (solo Web)

- `features/control/manual/ManualDriveOverlay.tsx`
  - «Información» (subtítulo «Gestos y estado») y «Parámetros» empiezan cerrados cada vez que se abre el control; ya no se recuerda el estado en `localStorage` (`afr2.touch.panels` queda sin uso).
  - Los dos paneles van juntos en `.afr2-manual-panels` (dos columnas desde 720 px); avisos de potencia, OLED y reintento del modelo quedan arriba de los paneles.
  - Joystick visual (`.afr2-manual-puck`): aro en el punto de contacto y perilla que sigue al dedo, limitada a 56 px. Se mueve con variables CSS, sin re-renderizar el overlay en cada `pointermove`.
  - Estela (`.afr2-manual-wake`): tres anillos que se expanden bajo el auto mientras se mueve o gira (punteados en giro).
  - El modelo se inclina hacia la dirección pedida (14 px, diagonales normalizadas) con curva elástica; la sombra se desplaza al revés.
  - `--drive-intensity` (potencia / máximo) acelera la estela y las cintas de dirección.
  - Texto de dirección con punto de estado y entrada animada al cambiar.
- `features/control/manual/useManualDrivePointers.ts`: nueva opción `onDragVisual` (tipo `DragVisual`) que informa el origen y desplazamiento del arrastre de un dedo; se oculta al soltar, al empezar un giro de dos dedos y al cancelar.
- `styles/manual-drive.css`: estilos de lo anterior, encabezados de panel compactos (44 px mínimo), apertura animada del cuerpo, cartel de giro bajado a 56 px para no tapar el rumbo, y el bloque landscape de poca altura reordenado con los paneles en la columna derecha. `prefers-reduced-motion` desactiva las animaciones nuevas.

Sin cambios de protocolo ni de las órdenes enviadas: la dirección, el TTL y los giros siguen igual.

## Estado

web=fuente editada, sin compilar, sin navegador ni prueba en teléfono; atmega/esp=sin cambios.
