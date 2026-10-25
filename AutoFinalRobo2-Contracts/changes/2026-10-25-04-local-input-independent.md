# AFR2-20261025-04 — Simulación local sin bloqueo del visor

25/10/2026. El usuario informa que sigue sin poder simular sin conexión y
precisa que el texto inferior es «Modelo 3D no disponible».

Causa localizada en fuente: ManualDriveModel pasaba un componente Unavailable
con efecto onFailure a Canvas.fallback. La implementación instalada de R3F
(dist/react-three-fiber.esm.js) coloca fallback en los hijos del canvas DOM,
por lo que su efecto se ejecuta incluso con WebGL disponible. Ese efecto
marcaba modelFailed=true y la condición enabled anulaba todos los gestos.
La revisión03 de foco/captura no corregía este bloqueo de disponibilidad.

Canvas.fallback queda como texto pasivo. Unavailable con efecto queda solo
en RenderBoundary, donde se monta ante una excepción real. La entrada local
se habilita al abrir, sin consultar conexión/capacidades/estado del visor;
mantiene dirección/flechas aunque el modelo tarde en cargar o falle. Si carga,
recibe las velocidades simuladas existentes. No nuevo activo ni dependencia.
La dirección inferior informa Simulando hacia… o Simulación detenida; estado
del modelo conserva su región/recuperación propia. Movimiento anuncia que
Control táctil permite simulación sin robot, en vez de bloquear toda la UI.

Órdenes reales conservan enlace/capacidad, modelo listo, límites y error
vigente; pérdida de disponibilidad cancela y exige un gesto nuevo. Local
sigue local al reconectar y no invoca callbacks físicos ni envía cero.
STOP, captura, keyup/blur/Tab/cierre y reducción de movimiento03 conservados.

Web compilada/promovida04. Firmware ESP/ATmega0.9.0/EX01v7/build01,30comandos,
59ScreenCodes y gates0 sin cambios. Sin navegador, tests/validadores separados,
teléfono, flasheo, pruebas físicas, CAD, commit o publicación. Causa deducida
del ciclo de montaje en la fuente instalada y el síntoma humano, no de una
reproducción en navegador. Compilación no acredita aceptación visual.

[Contrato](../contracts/WEB_TOUCH_SIMULATION.md),
[build04](../documentation/builds/2026-10-25-04/README.md),
[adaptaciones](../provenance/local-input-independent-20261025.json).
