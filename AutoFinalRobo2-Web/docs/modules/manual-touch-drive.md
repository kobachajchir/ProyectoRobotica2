> **Vigente AFR2-20261025-11:** [giro local con dos dedos](../../../AutoFinalRobo2-Contracts/contracts/WEB_TOUCH_SIMULATION.md), Q/E15°,0 centra. Orientación al soltar/rumbo y fase de ruedas conservados, un dedo desplaza en pantalla. Entrar cancela target previo/fija preview; no yaw físico. Soltar ambos para otro gesto. Compilado/promovido; prueba táctil pendiente.

> **Vigente AFR2-20261025-02:** [táctil local y giro simulado](../../../AutoFinalRobo2-Contracts/contracts/WEB_TOUCH_SIMULATION.md). Sesión local sin comandos, vista superior/flechas compartidas y ruedas por cursor en GOTO/secuencias. No se usa pose medida. Web compilada; visual/móvil pendientes. Descripciones previas conservadas como historia.

> AFR2-20261025-01: [control directo7](../../../AutoFinalRobo2-Contracts/contracts/DIRECT_CONTROL_7.md). Se eliminan todas las operaciones y estados de permiso y la habilitación OLED. Las acciones se envían directamente; solo permanecen conexión/capacidades, límites, estado ocupado, parada/caducidad y calibración. Fuente compilada; runtime pendiente. Estados inferiores históricos.

# Control táctil — AFR2-20261024-13

Movimiento → Control táctil, en la misma fila que Conducción manual, abre un
dialog de pantalla completa. Modelo final3D grande, centrado y elevado sobre
sombra; no se anima una pose física. X izquierda/Escape cierra. STOP contextual
permanece accesible dentro de la capa modal.

Apoyar/clic no mueve. Arrastrar desde ese punto arma una de8direcciones al
superar22px; mantener sostiene la consigna. Volver a14px libera. Soltar,
captura perdida, cancelación, salir del área, segundo dedo en el área, perder
foco/visibilidad, resize, STOP o indisponibilidad cancela y exige otro gesto.
Flechas del teclado combinables, liberadas en keyup/blur. No gestos de giro:
los controles de rotación existentes siguen en Movimiento.

La barra inferior comparte velocidad con parámetros. En diagonal reparte el
límite entre los dos ejes; la consigna máxima de rueda no supera ese límite.
Se puede ajustar mientras se mantiene la dirección, reemplazando el target.
No manda una petición por pixel: heartbeat180ms, TTL400ms y cola single-latest
existentes. Abrir no adquiere control. El modelo debe estar disponible; recuperación
WebGL/modelo cancela input y nunca reanuda una orden por sí sola.

FloatingModal incorpora size=fullscreen y callbacks previos a cerrar/STOP.
Mantiene dialog nativo, foco, restauración al botón, material240/200ms,
reduced motion/teclado. Canvas demand, DPR≤1,5; cámara adapta encuadre sin
escalar GLB. Interfaz usa safe-area/dvh, captura de puntero y touch-action
local; conserva el zoom de página. No nuevas dependencias/activos.

Fuentes: `manual/ManualDriveOverlay.tsx`, `manual/ManualDriveModel.tsx`,
`ControlPage.tsx`, FloatingModal y `styles/manual-drive.css`.
Mismo contrato WS/EX01v6 y autoridad AVR, sin cambios de firmware. Fuente
compilada/empaquetada; gestos, foco/animaciones, safe areas y conducción no
comprobados en teléfono/placas. Sin navegador, tests, validadores o flasheo.
