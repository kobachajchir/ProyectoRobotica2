# AFR2-20261105-01 — Comprobaciones junto a la terminal del Loader

05/11/2026. Pedido humano: mover las comprobaciones superiores al lateral del log, ordenadas verticalmente, y aumentar la altura del registro.

## Adaptaciones por archivo

- `AutoFinalRobo2-Loader/src/MainForm.Workflow.cs`: terminal a la izquierda y etapas a la derecha en una columna de 300 unidades de layout, con separación, desplazamiento independiente y filas de altura automática. Las etiquetas largas pueden ocupar varias líneas. Conserva autores, etapas, orden, estados, tooltips y consumo de eventos de operación; las casillas siguen siendo indicadores, sin activación manual.
- `AutoFinalRobo2-Loader/src/MainForm.Design.cs`: el área de terminal/etapas pasa del 42 % al 55 % del espacio disponible entre la cabecera y las acciones. La configuración superior conserva su desplazamiento propio; barra de acciones conservada.

## Estado real

Loader: cambios en fuente, revisados mediante lectura del diff. Sin compilación, ejecución de la interfaz modificada, pruebas ni validadores. La ventana ya abierta sigue usando el ejecutable anterior; requiere recompilar y reabrir para mostrar esta disposición.

ATmega / ESP / Web: sin modificaciones por esta entrega. Sin puertos abiertos, flasheos ni pruebas físicas. No cambia contratos de comunicación ni procedimientos de comprobación.
