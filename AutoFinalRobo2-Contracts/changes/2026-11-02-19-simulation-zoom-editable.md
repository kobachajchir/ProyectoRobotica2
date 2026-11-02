# AFR2-20261102-19 — Información y porcentaje editable del visor

02/11/2026. Pedido humano sobre el componente compartido de simulación.

- Unidad y cuadrícula pasan a un div informativo con el mismo material, borde y forma del zoom, dentro de una fila flotante adaptable. Se quitan las leyendas duplicadas de CollectorPage y ControlMap.
- Clic en porcentaje abre un input con selección del contenido. No modifica la cámara mientras se escribe. Enter o blur confirman números finitos entre 0 y 100 inclusive; vacío, texto o valores fuera del rango restauran el porcentaje guardado al iniciar la edición. Escape cancela. Se acepta coma decimal. Una guarda evita confirmar dos veces por Enter seguido de blur.
- Más/menos ajustan diez puntos dentro de 0–100. El componente usa onChange absoluto compartido por Auto/Brazo/Conjunto y secuencias/GOTO.
- Los adaptadores mantienen la cámara con zoom positivo: 0 corresponde al encuadre mínimo seguro del visor y 100 al encuadre de referencia. Collector usa razón .25–1 respecto al encuadre inicial; ControlMap usa su mínimo de ruta o mapa hasta 1. La rueda del visor Collector queda acotada al mismo intervalo. No se pasa zoom=0 a Three.js.

Archivos: SimulationViewport.tsx, simulation-viewport.css, CollectorScene.tsx, CollectorPage.tsx y ControlMap.tsx. atmega/esp sin cambios; web fuente modificada; Contracts registro. Sin comandos nuevos ni modificación de motores de simulación.

Se implementa la validación de entrada solicitada como comportamiento del producto. No se ejecutan tests, compilaciones, validadores, navegador, promoción ESP ni flasheos. Comportamiento y presentación en ejecución No verificados. Sin commit ni publicación solicitados.
