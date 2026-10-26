> **Compilación AFR2-20261025-14 autorizada y completada:** firmware ATmega (producto/comunicación/biblioteca), ESP y Web full compilan sin errores ni advertencias finales. Firmware0.10.0/EX01v8, Web0.3.0. [Evidencia](../documentation/builds/2026-10-25-14/README.md). Sin tests, navegador ni flasheos; paquete Web local full generado, **ESP/data/dist conserva el paquete anterior**. La SRAM estática no certifica pila/ISR ni hardware. Estados previos inferiores históricos.

> **Vigente AFR2-20261025-14:** [sensores/radar/servo y pantallas hermanas](../contracts/SENSORS_RADAR_8.md). Fuente ATmega/ESP/Web integrada, EX01v8 y recursos OLED copiados; sin builds, tests, validadores, navegador ni flasheo. Paquete ESP anterior; montaje y memoria pendientes. [Ficha14](../changes/2026-10-25-14-sensors-autonomous-radar.md).

# Maestro de contratos

25/10/2026, AFR2-20261025-01. [Control directo EX01v7](../contracts/DIRECT_CONTROL_7.md),
firmware0.9.0 y Web0.2.0,30comandos y59identidades OLED. Los tres módulos
compilan; [evidencia](../documentation/builds/2026-10-25-01/README.md).
No adquirir/renovar/liberar, tokens de acceso o habilitación desde OLED.
Presencia automática500ms/caducidad1500ms, TTL de movimiento, STOP y watchdog
conservados. La sesión/transacción/programa/revisión solo correlaciona mensajes.

[Matriz actual](ALINEACION_MAESTRO.md), [roadmap](ROADMAP.md), planes Web/ESP/ATmega.
Compilado, aceptación física pendiente. Antecedentes en history/, sin usarlos como requisitos actuales.

## AFR2-20261025-15 — Modelo 3D con componentes interactivos

Fuente Web nueva: once puntos con fichas locales de explicación y acceso a la
tarea/rueda correspondiente. La cámara mueve los puntos y la ficha queda fija;
arrastrar/zoom la conserva, X/Escape/clic exterior la cierra. Identidad0.1.9,
VIS-036. Referencias visuales inferidas del GLB, sin modificar CAD.

Estado de esta entrega: atmega sin cambios; esp sin cambios y sin promoción a
data/dist; web implementada en fuente, sin compilar ni empaquetar. El build14
previo no incluye esta interfaz. Sin tests, compilaciones, validaciones,
navegador, render ni flasheo. Documentación y límites en
[contrato del visor](../contracts/WEB_MODEL_INSPECTION.md) y
[ficha15](../changes/2026-10-25-15-model-component-hotspots.md).
