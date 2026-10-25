# Plan ESP vigente

25/10/2026, AFR2-20261025-01. [Control directo EX01v7](../contracts/DIRECT_CONTROL_7.md),
firmware0.9.0 y Web0.2.0,30comandos y59identidades OLED. Los tres módulos
compilan; [evidencia](../documentation/builds/2026-10-25-01/README.md).
No adquirir/renovar/liberar, tokens de acceso o habilitación desde OLED.
Presencia automática500ms/caducidad1500ms, TTL de movimiento, STOP y watchdog
conservados. La sesión/transacción/programa/revisión solo correlaciona mensajes.

Broker sin concesión/renovación/liberación o tokens de acceso. Hello anuncia
controlMode=direct. EX01v7 exacto en enlace Mega, reserved0 en escrituras. Una
sesión WS; límites/colas/correlación/generación de STOP se conservan. WEB_PULSE
retransmite edad de presencia, sin activar motores ni conceder autorización.

Wi-Fi/FS/HTTP/boot/reinicio/avisos OLED conservados; salud y versión/hash declarado
de paquete disponibles sin Mega, sin SHA recalculado en placa. Radio/persistencia/
fallos/clientes siguen sin aceptación física. LittleFS nominal3MiB es candidato;
flash/SPI reales sin identificar. Operación Mega D18/D19 y carga temporal USB
D0/D1 según FLASHING; no mapa UNO ni puerto directo ESP confundidos con selector.

[Matriz](ALINEACION_MAESTRO.md).
