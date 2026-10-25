# AFR2-20261025-03 — Captura táctil, teclado y flecha activa

25/10/2026. El usuario informa que arrastrar el robot no muestra movimiento
y que las flechas navegan la interfaz. Pide flechas animadas al elegir dirección.

La fuente muestra que FloatingModal enfocaba Cerrar, mientras ManualDriveOverlay
solo escuchaba teclas en el stage. Además Canvas crea un wrapper con pointerEvents
auto explícito, que no queda anulado por pointerEvents:none heredado. Son
hallazgos de fuente; no se afirma haber reproducido el fallo en navegador.

Se incorpora foco inicial optativo en el modal y se usa para conducción.
Captura de flechas en el modal abierto antes de scroll/navegación, diagonales
y liberación por keyup; campos de velocidad conservan sus teclas nativas.
Tab/foco/blur/STOP/cierre cancelan. Fuera de ese modal no quedan listeners.

Una superficie transparente encima del Canvas posee los pointer events,
con captura DOM y liberación global. Canvas es explícitamente pasivo. Zona
muerta12/8px, buttons solo requerido a mouse, segundo puntero global cancela.
pointerout nulo se filtra por borde real de ventana, no por transición interna.

Flecha activa56px, UiIcon compartido/carmesí-rubí, entrada140ms y oscilación
±3px/800ms mientras se mantiene el arrastre. Modelo responde8px por eje y
las ruedas siguen la consigna simulada. Teclado inmediato; reduced motion
sin oscilación/desplazamiento secundario. No nuevas librerías ni activos.
animate/mobile-native/visual-identity-system sobre manual0.1.9 autorizado,
VIS-027; composición Propuesta, verificación visual No verificada.

Web compilada y promovida localmente; ESP solo data/dist. Sin cambio de
firmware0.9.0/EX01v7/build01,30comandos/59ScreenCodes ni gates0. Sin navegador,
tests/validadores independientes, teléfono, flasheo, pruebas físicas o CAD.
El modo local de02 sigue fijo al abrir; reconexión no envía movimientos.

[Comportamiento vigente](../contracts/WEB_TOUCH_SIMULATION.md),
[build03](../documentation/builds/2026-10-25-03/README.md),
[procedencia por archivo](../provenance/touch-input-feedback-20261025.json).
