# AFR2-20261026-01 — Mostrar u ocultar referencias del modelo

Pedido humano: botón a la derecha del selector que alterne Mostrar referencias /
Ocultar referencias; una sola rueda señalada y cierre de explicaciones al ocultar.

Fuente Web adaptada: botón compartido `Button`, estado local visible inicialmente,
ocultación de puntos/líneas, cierre y limpieza de la ficha, selector desactivado
mientras están ocultos. Mostrar otra vez no reabre la explicación anterior ni
restablece la cámara. La proyección de puntos se desmonta mientras no se necesita.
Los ocho selectores incluyen una única rueda FL, cuya ficha describe el diseño
común de las cuatro ruedas. Las otras ruedas siguen en sus controles habituales.

Archivos: Web `ModelInspection.tsx`, `modelComponents.ts` y
`styles/model-inspection.css`; contrato `WEB_MODEL_INSPECTION.md` actualizado.
Identidad existente VIS-036/manual0.1.9: botón/tokens compartidos y composición
adaptativa; dirección Confirmada, distribución Propuesta, aspecto No verificado.

Estado: **atmega sin cambios; esp sin cambios, sin promoción `data/dist`;
web implementada en fuente, sin compilar ni empaquetar**. CAD/protocolo/OLED sin
cambios. No se ejecutaron tests, compilaciones, validaciones, navegador ni
flasheos por instrucción humana. Se preservan los cambios concurrentes.
