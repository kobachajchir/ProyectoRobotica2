# Plan ATmega vigente — M01..M19

25/10/2026, AFR2-20261025-01. [Control directo EX01v7](../contracts/DIRECT_CONTROL_7.md),
firmware0.9.0 y Web0.2.0,30comandos y59identidades OLED. Los tres módulos
compilan; [evidencia](../documentation/builds/2026-10-25-01/README.md).
No adquirir/renovar/liberar, tokens de acceso o habilitación desde OLED.
Presencia automática500ms/caducidad1500ms, TTL de movimiento, STOP y watchdog
conservados. La sesión/transacción/programa/revisión solo correlaciona mensajes.

M01..M18 conservan plataforma, navegación/avisos, red/sistema, potencia/sensores,
Hall/calibración/PID/odometría/línea/GOTO, EEPROM, UI remota y documentación.
M06 pasa a control directo con STOP/caducidad/watchdog. M16 conserva revisión,
frescura, privacidad y entrada física. M17 negocia EX01v7 sin permiso/token.
M19 tiene firmware/comunicación/biblioteca compilados; pila/tiempos/ramas Hall
habilitadas/interoperabilidad/banco siguen pendientes. La matriz histórica con
IDs individuales se conserva en history/direct-control7/docs__PLAN_ATMEGA.md.

Se elimina Acquire, remote_enabled y las opciones2/3/56 (IDs reservados).
Origen de actividad no es derecho de control. PresenciaWeb1500ms y TTLs cortan
salidas. Una operación local activa responde ocupado; STOP siempre accesible.
El encoder físico puede iniciar una acción local interrumpiendo actividad Web.
No medición Hall remota; una vuelta por rueda desde OLED. Hall/memoria0.

Control manual4/Sistema7,48nodos/11avisos/59códigos. Tabla e inserción Reinicio
portadas a Web; texto de condición inválida reemplaza mensaje de autorización.
No nuevo layout gráfico, recurso, ScreenCode, pinout o CAD.

[Matriz](ALINEACION_MAESTRO.md).
