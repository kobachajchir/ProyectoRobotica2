# Plan maestro vigente — AFR2-20261024-15

24/10/2026. [Matriz de fuente, comunicación y pendientes](ALINEACION_MAESTRO.md).
Reemplaza el diagnóstico antiguo de esqueleto/no integración; copia anterior en
[history](history/alignment15/AutoFinalRobo2-Contracts__docs__ROADMAP.md).
No equivale a aceptación física ni elimina pendientes dependientes de datos.

| Etapa | Implementación | Evidencia / condición de cierre pendiente |
| --- | --- | --- |
|0. Hardware y CAD | Mapas/recursos/contratos y modelo base Fusionv16/Blender/GLB final | Cableado, niveles, flash/SPI, dimensiones reales. PCB/pieza nueva esperan planos |
|1. AVR y UI | Reloj, USART1 IRQ, TWI asíncrono/Manager, OLED y encoder IRQ; navegación completa | Build12. Recuperación/temporización/gestos y paridad visual sin probar |
|2. Comunicación |33comandos, ROBO2/perfil2+EX01v6, broker WS, STOP/lease y snapshots conectados | Complementos Web/ESP/AVR escritos. Pruebas de correlación/fallos/timeout pendientes |
|3. Sensores/potencia | Línea5, HC-SR04, Hall, cuatro PWM, rampas/watchdog, pruebas acotadas | Tipo/polaridad, temporalidad, cableado y signos reales |
|4. Movimiento | PID/Mecanum/odometría/línea/GOTO y secuencias por tiempo/distancia | Gates Hall/memoria0, CPR/geometría y memoria dinámica pendientes |
|5. Web/pantallas | Modelo final, simuladores, editor, movimiento táctil, sistema/red, OLED59 y guía | Web15 compilada/empaquetada; interacción móvil/visual y raster diferencial pendientes |
|6. Puesta en marcha | Builds registrados; diagnóstico ESP y contratos de aceptación preparados | No ejecutada: pruebas automatizadas/banco/robot, imagen FS y flasheo requieren pedido |

Revisión15 completa los dos huecos software encontrados: diagnóstico FS/paquete
en Web y adquisición de control al aplicar configuración. Los planes de módulo
indican condiciones, no listas obsoletas de handlers que ya existen:
[ATmega](PLAN_ATMEGA.md), [ESP](PLAN_ESP.md), [Web](PLAN_WEB.md).

Reglas firmes: C/avr-libc sin Arduino/DMA en Mega; ESP Arduino propio/LittleFS
sin AT; React/Vite en Web. Sin MPU/balance. I2C Manager, OLED único cliente.
Giro selecciona, SW corto confirma, largo800ms principal, Volver explícito.
Operación UART Mega D18/D19; carga temporal USB D0/D1 según FLASHING.
J1 cinco señales y J2 alimentación común1x2,2,54mm; CAD nuevo no se inicia por
cerrar software. IRQ UI D2/D3 y Hall A8–A15 según contrato11, no mapa UNO.

No declarar este plan entero terminado hasta completar las condiciones de
aceptación y recibir los insumos CAD. No habilitar memoria/Hall, fabricar datos,
medir precisión o publicar capacidad física por tener código/compilación.