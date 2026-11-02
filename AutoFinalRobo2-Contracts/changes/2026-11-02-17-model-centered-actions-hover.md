# AFR2-20261102-17 — Accesos centrados y hover de acento

Pedido humano: centrar los botones debajo de las tarjetas de Modelos, sumar
flechas a ambos y animar el fondo de hover hacia el acento conservando legibilidad.

`Web/src/features/models/ModelsPage.tsx`: grupo centrado, flecha derecha en
ambos botones y estilos parametrizados para la primitiva Button existente.
`Web/src/styles/models.css`: reglas locales al grupo, fondo `--action`, texto
`--text-on-action` y borde `--action-border` en hover de puntero fino y foco
visible. Transición breve `--duration-fast` con retorno reversible; cambios
inmediatos con teclado o movimiento reducido. Conserva los iconos iniciales,
destinos y el comportamiento compartido de pulsación.

Se usa la skill animate para feedback de hover mediante CSS, sin dependencias
ni animación continua. La transición cromática responde al pedido explícito.
atmega/esp sin cambios; web solo fuente. Sin tests, compilaciones, validaciones,
navegador, promoción ni flasheos. Contraste en ejecución no comprobado;
se reutiliza el par semántico de acento/texto del tema vigente.
