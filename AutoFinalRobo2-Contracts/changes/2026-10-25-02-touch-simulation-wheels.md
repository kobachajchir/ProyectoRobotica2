# AFR2-20261025-02 — Táctil sin conexión y ruedas simuladas

Pedido humano25/10/2026: usar Control táctil sin robot, dirección inferior,
vista superior y flechas con identidad compartida; consultar giro de ruedas.
La inspección del GLB muestra las cuatro ruedas y motores separados; se
implementa su giro visual en las tres simulaciones sin cambiar el activo.

| Antes | Fuente adaptada |
| --- | --- |
| Sin conexión bloqueaba el gesto | Sesión local con velocidad propia, sin callbacks físicos |
| Cámara oblicua | Superior ortográfica, frente arriba y modelo centrado |
| SVG propio para cada flecha | UiIcon arrow y superficies/tokens comunes |
| Ruedas estáticas | Pivotes por motor, giro Mecanum ideal por cursor o gesto |
| Reconexión podía volver a habilitar input | Local queda local hasta cerrar/reabrir; pérdida real cancela |

No nuevas dependencias, recursos, ScreenCodes o comandos. Radio visual40mm,
rodillos rígidos y sin realimentación física. No se gira la dirección de la
rueda como un auto convencional. reduced motion evita el giro secundario.
Skills animate/apple-design/mobile-native/visual-identity-system aplicadas
sobre manual0.1.9 autorizado; VIS-026 registra composición no verificada.
Autorías y créditos previos conservados; adaptación propia por archivo en
[procedencia](../provenance/touch-simulation-wheels-20261025.json).

Web compilada/empaquetada/promovida localmente. ESP solo activos nuevos;
ATmega y firmware ESP conservan build01. Sin tests/validadores independientes,
navegador, teléfono, flasheo, pruebas físicas, CAD, commit ni publicación.
[Contrato de comportamiento](../contracts/WEB_TOUCH_SIMULATION.md),
[evidencia de compilación](../documentation/builds/2026-10-25-02/README.md).
