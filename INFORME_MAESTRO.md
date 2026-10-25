> **Vigente AFR2-20261025-01:** [control directo7](AutoFinalRobo2-Contracts/contracts/DIRECT_CONTROL_7.md), sin permiso ni opciones de adquisición. Los tres módulos compilan con firmware0.9.0/Web0.2.0. Estado anterior inferior histórico.

# AutoFinalRobo2 — informe maestro vigente

24/10/2026, AFR2-20261024-15. Un único Git en D:/ProyectoRobotica2 con cinco
módulos. Proyecto independiente; D:/AutoProyecto es referencia de solo lectura.

La [matriz del plan maestro](AutoFinalRobo2-Contracts/docs/ALINEACION_MAESTRO.md)
concentra implementación, productores/consumidores y pendientes reales. Se
revisaron los33comandos Web/ESP/AVR y las59identidades OLED. Se completaron
diagnóstico de LittleFS/paquete en Web y adquisición de control en Calibración.
Código presente y compilación no acreditan funcionamiento físico.

| Módulo | Alcance vigente |
| --- | --- |
| [Atmega](AutoFinalRobo2-Atmega/README.md) | C/avr-libc sin Arduino/DMA; servicios de robot y autoridad de salidas. Fuente12/build12, firmware0.8.0; Hall/memoria0 |
| [ESP](AutoFinalRobo2-ESP/README.md) | Arduino ESP8266 propio, sin AT; Wi-Fi/HTTP/WS/LittleFS y broker; diagnóstico ampliado15 |
| [Web](AutoFinalRobo2-Web/README.md) | React/Vite; control, simuladores, sensores/red, OLED, desarrollador y guía; calibración alineada15 |
| [Contracts](AutoFinalRobo2-Contracts/README.md) | Contratos, hardware, evidencias, planes y cambios compartidos |
| [CAD](AutoFinalRobo2-CAD/README.md) | Auto base Fusionv16/Blender y GLB final; PCB/pieza nueva diferidas |

Navegador → JSON afr2-ws-1 /ws del mismo origen → ESP → UART ROBO2/perfil2,
EX01v6 → USART1 Mega D18/D19. ESP UART directo y selector E_RX/E_TX son
conexiones distintas. La programación temporal por puente USB usa D0/D1 según
[FLASHING](AutoFinalRobo2-Contracts/hardware/FLASHING.md), sin trasladar mapa UNO.

OLED único dispositivo I2C por Manager. Encoder UI CLK D2/DT D3 por IRQ, SW D42
según cambio11; largo800ms abre principal, corto confirma, atrás solo Volver.
Hall de ruedas separado, A8–A15, pendiente de montaje/calibración.

No quedan brechas de implementación identificadas en el alcance software
definido tras esta revisión por fuente. **El proyecto global no está cerrado:**
Hall/CPR/signos/geometría, pila/tiempos, flash/SPI y puesta en marcha requieren
datos/pruebas; no se habilitan macros por compilar. PCB/soporte/cubierta esperan
planos y componentes. J1 cinco señales y J2 VCC/GND común,2,54mm, conservados.

[Roadmap](AutoFinalRobo2-Contracts/docs/ROADMAP.md),
[cambio15](AutoFinalRobo2-Contracts/changes/2026-10-24-15-master-alignment.md),
[evidencia15](AutoFinalRobo2-Contracts/documentation/builds/2026-10-24-15/README.md).
Pruebas, validadores independientes, navegador, flasheo y CAD no se ejecutan
por esta revisión. [Informe anterior conservado](AutoFinalRobo2-Contracts/docs/history/alignment15/INFORME_MAESTRO.md).