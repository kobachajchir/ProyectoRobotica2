> **AFR2-20261025-11:** [giro con dos dedos local](../contracts/WEB_TOUCH_SIMULATION.md), rumbo retenido/Q-E15°/0 centra; al entrar cancela target anterior y fija preview, sin yaw físico. Cámara10/chevrones09 conservados. Web compilada/promovida11; firmware0.9.0/EX01v7/build01/gates0 intactos. Gesto no probado en dispositivo; sin CAD o pruebas físicas.

# Plan Web vigente

25/10/2026, AFR2-20261025-01. [Control directo EX01v7](../contracts/DIRECT_CONTROL_7.md),
firmware0.9.0 y Web0.2.0,30comandos y59identidades OLED. Los tres módulos
compilan; [evidencia](../documentation/builds/2026-10-25-01/README.md).
No adquirir/renovar/liberar, tokens de acceso o habilitación desde OLED.
Presencia automática500ms/caducidad1500ms, TTL de movimiento, STOP y watchdog
conservados. La sesión/transacción/programa/revisión solo correlaciona mensajes.

Acciones directas desde Control, Calibración y Secuencias. Se elimina todo estado
controlLease y API de adquisición; solo se requieren conexión/capacidades/estado
del dispositivo. Presencia interactiva500ms y actividad imperativa para cancelar
antes de un render al salir o perder foco. Sin reanudación automática.

Catálogo59 y copia C/TS: retirados tres ítems, preservados ScreenCodes/Volver.
Modelo final/Draco/WebP/minificación/empaquetado conservados. Guía propia separa
Web/ESP/ATmega. Sin sensor/pose inventados, MPU/balance o permisos artificiales.
Build:esp genera recursos, compila TypeScript/Vite y promueve siete activos.
Solo compilado; navegador/móvil/placas y raster diferencial pendientes.

[Matriz](ALINEACION_MAESTRO.md).
