# AFR2-20261025-06 — Chevrones grandes desde debajo del chasis

25/10/2026. Pedido humano: ampliar chevrones, degradé/fade y aparición desde
debajo del auto, con un efecto más estético. Se interpreta como estela que
nace bajo el chasis y emerge hacia la dirección activa; no como dirección
fija hacia la parte inferior de la pantalla.

El origen de las ocho direcciones ahora es el centro del mismo rectángulo
del modelo (inset superior48px), con rotación sobre su extremo izquierdo.
Se retiran las posiciones periféricas x/y del tipo ManualDirection. La estela
queda en z1 y el Canvas transparente en z2: el auto oculta la raíz de los
trazos. Indicador de modo/hint en z4 y captura DOM en z5, sin bloquear gestos.
La sombra queda en z0. No se modifica GLB, escala, cámara o cinemática.

Tres chevrones anchos/altos en viewBox320×200. Su longitud se adapta al46%
del menor lado del contenedor, máximo320px; altura36%, máximo200px. Container
query units en decorativo absoluto, con fallback de viewport. Más cuerpo que
los trazos80..120px/72px de05. El origen central reemplaza el marco periférico.

Degradé longitudinal transparente→acento→transparente y máscara transversal
de opacidad para extremos suaves, sin blur/filtros. Blanco solo en la máscara
alfa/luminancia, nunca como color de producto. IDs únicos por useId/dirección.
Entrada/salida220ms ease-out; flujo1200ms linear de±20unidadesSVG, fases
cola→medio→punta. Se pausa al soltar y desvanece desde el frame actual.
Teclado inmediato y reduced motion estáticos. Solo transform/opacity animan.

Dos fuentes propias adaptadas: ManualDriveOverlay.tsx y manual-drive.css.
No nuevas dependencias/activos ni cambios de captura, foco, teclado,
simulación offline04, físico, protocolo o firmware0.9.0/EX01v7/build01.
30comandos/59ScreenCodes y gates0 conservados. ESP solo activos Web06 locales.
animate/identidad0.1.9 autorizada, VIS-029: dirección Confirmada, composición
Propuesta, visual No verificada. Autorías anteriores conservadas.

Web compilada/promovida; sin navegador/teléfono, tests/validadores separados,
flasheos, pruebas físicas o CAD. Compilar no acredita aceptación estética.
[Contrato](../contracts/WEB_TOUCH_SIMULATION.md),
[build06](../documentation/builds/2026-10-25-06/README.md),
[procedencia](../provenance/underbody-gradient-trail-20261025.json).
