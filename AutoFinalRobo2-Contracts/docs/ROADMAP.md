# Plan maestro vigente

25/10/2026, AFR2-20261025-01. [Control directo EX01v7](../contracts/DIRECT_CONTROL_7.md),
firmware0.9.0 y Web0.2.0,30comandos y59identidades OLED. Los tres módulos
compilan; [evidencia](../documentation/builds/2026-10-25-01/README.md).
No adquirir/renovar/liberar, tokens de acceso o habilitación desde OLED.
Presencia automática500ms/caducidad1500ms, TTL de movimiento, STOP y watchdog
conservados. La sesión/transacción/programa/revisión solo correlaciona mensajes.

| Etapa | Fuente existente | Cierre pendiente |
| --- | --- | --- |
|0. Hardware/CAD|Mapas, recursos, Fusionv16/Blender/GLB final|Cableado/niveles, flash/SPI y planos PCB/pieza nueva|
|1. AVR/UI|Reloj/UART1/TWI/Manager/OLED/encoder IRQ, navegación y biblioteca Web|Tiempos/recuperación/paridad/gestos físicos|
|2. Comunicación|30comandos directos y supervisión, productores/adaptadores en tres módulos|Pruebas de correlación/fallos, sin permisos de acceso|
|3. Sensores/potencia|Línea5, HC-SR04, Hall, PWM/rampas/watchdog|Tipo/polaridad, signos, temporización, montaje/calibración|
|4. Movimiento|Mecanum/PID/odometría/línea/GOTO/secuencias|Hall/CPR/geometría/memoria aprobada, gates0|
|5. Web|Control táctil, editor/simuladores, OLED59, diagnóstico/red/documentación|Interacción visual/móvil y aceptación conectada|
|6. Puesta en marcha|Compilaciones y mapas registrados|Pruebas físicas, imágenes/flash solo por pedido|

El retiro del permiso no habilita hardware sin requisitos. Se conservan SW largo
para principal, corto confirma y atrás por Volver. [Matriz](ALINEACION_MAESTRO.md).
Sin nuevo CAD; J1 cinco señales/J2 alimentación común,2,54mm y fijación pendientes.
