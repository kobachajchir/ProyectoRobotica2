# AFR2-20261025-07 — Cintas anchas, mayor salida y punta intensa

25/10/2026. Pedido humano: cintas más anchas, mucho más sobresalientes desde
el auto, punta de color fuerte y degradé que funda la cola con el fondo.

Se engrosa la banda del chevrón en la geometría SVG (cuerpo70unidades frente
a38 de06) y se amplía el viewBox a360×240. Longitud49% del menor lado local,
máximo480px; altura50%, máximo280px, frente46%/36% y320×200px de06. Sigue
naciendo bajo el chasis en capa z1, detrás del Canvas z2; input z5 intacto.

La cámara superior del control táctil encuadra al52% de ancho/alto frente66%
anterior, dejando más aire para ver las cintas. Es solo zoom de ese visor:
GLB, malla, unidades, otras cámaras/mapas y cinemática conservados. El modelo
sigue centrado y frontal hacia arriba. No se escala o modifica el CAD.

Degradé longitudinal monótono: transparente en cola, luego color de acento
hasta opacidad1 desde84% hasta la punta. Se retira el desvanecimiento frontal
de06. La máscara transversal conserva cuerpo opaco entre32..68% y suaviza
solo extremos, para que las bandas tengan más peso. Transparencia revela
el fondo real en ambos temas; no se copia un color de fondo ni nueva paleta.

Fade220ms ease-out y ciclo1200ms linear conservados; recorrido±12unidadesSVG
en lugar de20 para que la punta completa quepa en el nuevo viewBox. Fases
cola→medio→punta, pausa de frame y teclado/reduced motion estáticos intactos.
Sin filtros, dependencias, recursos externos o React por frame.

Tres fuentes adaptadas: ManualDriveOverlay.tsx, ManualDriveModel.tsx y
manual-drive.css. Sin modificar captura/foco, simulación offline04, contrato
WS/EX01v7, firmware0.9.0/build01,30comandos/59ScreenCodes o gates0. ESP solo
activos Web07 locales. Autorías previas conservadas, sin trabajo CAD.

animate y manual0.1.9 autorizado; VIS-030 dirección Confirmada, composición
Propuesta, visual No verificada. Web compilada/promovida; sin tests/validadores
independientes, navegador/teléfono, flasheos o pruebas físicas. Proporciones
son de fuente, no medidas de un render aceptado.
[Contrato](../contracts/WEB_TOUCH_SIMULATION.md),
[build07](../documentation/builds/2026-10-25-07/README.md),
[procedencia](../provenance/wide-ribbon-tips-20261025.json).
