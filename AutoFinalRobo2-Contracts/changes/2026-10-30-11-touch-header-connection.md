# AFR2-20261030-11 — Conexión táctil junto a STOP

30/10/2026, corrección de ubicación solicitada por el usuario. Sustituye la posición de pill descrita en08: conexión en el extremo derecho de la cabecera del modal táctil, STOP inmediatamente a su izquierda cuando está disponible. Cierre conserva posición izquierda. En ancho reducido el título pasa a una segunda fila para conservar espacio entre cierre, STOP y conexión. Pill con punto centrado, colores, texto y estado de enlace de08 conservados; modo/rumbo vuelven a centrarse sobre la vista.

Adaptaciones por archivo (autorías anteriores conservadas):

- AutoFinalRobo2-Web/src/components/ui/FloatingModal.tsx: slot opcional headerStatus después de STOP; atributo para estilo de cabecera con estado. Otros consumidores no reciben contenido nuevo.
- AutoFinalRobo2-Web/src/features/control/manual/ManualDriveOverlay.tsx: mueve pill al slot de cabecera y elimina instancia en la zona de gestos.
- AutoFinalRobo2-Web/src/styles/manual-drive.css: columna derecha ajustada al contenido, orden STOP/pill, separación12px, título en fila inferior a640px, modo local centrado.
- AutoFinalRobo2-Contracts/changes/2026-10-30-11-touch-header-connection.md: pedido, alcance, procedencia y estado real.

Web solo fuente, revisada por lectura. Sin tests, validadores, compilación, navegador, teléfono, promoción ni carga. AVR/ESP sin cambios; OLED/ScreenCodes/wire y comportamiento de movimiento/STOP sin cambios. La disposición visual en dispositivo queda sin comprobar.
