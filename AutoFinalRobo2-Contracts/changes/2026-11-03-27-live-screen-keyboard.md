# AFR2-20261103-27 — Teclado en Pantalla en vivo

03/11/2026. Pedido humano:
- mientras Pantalla en vivo está abierta, todas las teclas pasan por un captor propio;
- ← y → son los giros del encoder; Espacio es el pulsador SW (mantenido, largo al soltar);
- Escape cierra la pantalla; Enter no hace nada por ahora;
- nada del visor se selecciona con el teclado;
- mostrar junto a cada acción la tecla que la ejecuta.

Misma técnica de captura que el control táctil (ManualDriveOverlay).

## Cambios en Web/src
- `features/oled/EncoderControl.tsx`: prop `keyboardCapture`. Escucha `keydown`/`keyup` en la fase de captura de `window` solo mientras su `dialog` está abierto y sin `inert`. Detiene y anula todas las teclas, salvo Escape, F1–F12 y combinaciones con Ctrl/Alt/Meta. Giros sin repetición automática; Espacio inicia y suelta el gesto del SW con el mismo umbral largo. Pistas `←`, `Espacio` y `→` junto a cada botón.
- `features/oled/LiveOledViewer.tsx`: activa `keyboardCapture` mientras el visor está abierto; el foco que llegue por clic se suelta (`onFocusCapture`) y se evita el foco por `mousedown`.
- `styles/workspace.css`: sin selección de texto ni contorno de foco en el visor; estilo de la pista de tecla.
- Los otros usos de EncoderControl (simulación UI y biblioteca local) no cambian.

## Estado por repo
- **atmega / esp / loader:** sin cambios.
- **web:** fuente modificada. Verificado en el navegador local con eventos de teclado sintéticos: Tab, Enter, Espacio, ← y la tecla Espacio al soltar se anulan; Escape, F5 y Ctrl+R pasan; las tres pistas se ven. No verificado: giro y pulsación reales con el robot (los controles están deshabilitados sin enlace), Escape real con el teclado, compilación TypeScript.
- Sin commit.

## Ajuste posterior (mismo ID): todo al soltar
- Los giros ← y → se emiten al soltar la tecla, igual que el SW; solo si antes se vio su pulsación, y sin repetición automática. Espacio sigue sosteniendo el gesto: corto o largo se decide y se emite al soltar.
- Se quitan los `console.log` de prueba de AFR2-20261103-28 (ya cumplieron su fin).
