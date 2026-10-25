> **AFR2-20261025-11:** [giro con dos dedos local](../contracts/WEB_TOUCH_SIMULATION.md), rumbo retenido/Q-E15°/0 centra; al entrar cancela target anterior y fija preview, sin yaw físico. Cámara10/chevrones09 conservados. Web compilada/promovida11; firmware0.9.0/EX01v7/build01/gates0 intactos. Gesto no probado en dispositivo; sin CAD o pruebas físicas.

# Alineación vigente del maestro

25/10/2026, AFR2-20261025-01. [Control directo EX01v7](../contracts/DIRECT_CONTROL_7.md),
firmware0.9.0 y Web0.2.0,30comandos y59identidades OLED. Los tres módulos
compilan; [evidencia](../documentation/builds/2026-10-25-01/README.md).
No adquirir/renovar/liberar, tokens de acceso o habilitación desde OLED.
Presencia automática500ms/caducidad1500ms, TTL de movimiento, STOP y watchdog
conservados. La sesión/transacción/programa/revisión solo correlaciona mensajes.

| Área | Productores y consumidores | Condición conservada |
| --- | --- | --- |
| Conexión/sistema | Web Afr2Client/ConnectionContext, broker/system, Link/AppWire | Mismo origen,hello/controlMode=direct, EX01v7 exacto; no activar al conectar |
| Movimiento/modos/prueba | ControlPage/manual, broker_product, product/app_wire | Acción directa, límites/calibración/estado ocupado; STOP prioritario |
| Configuración/EEPROM | CalibrationPanel, CONFIG/PREFS/LINE_CAL | Consulta antes de editar, salidas apagadas, RAM/EEPROM separadas |
| GOTO/línea/pose | Web, broker y producto AVR | Hall/CPR/geometría válidos; no pose deducida desde consignas |
| Secuencias | Editor/simulador, broker, sequence |32registros/120s, programa/ejecución correlacionados; Hall/memoria0 |
| Sensores | SensorsPage, broker, sensors | Línea5/HC-SR04/Hall con validez y edad; unknown no es cero |
| Red/FS/boot | WifiPage/SystemPanel, ESP, avisos AVR | STA/AP/AP_STA, salud/paquete, boot/reinicio y privacidad; ACK no es resultado físico |
| OLED | Catálogo C/Contracts, copia de biblioteca TS y framebuffer |48nodos/11avisos; Control4ítems/Sistema7, retiradas acciones2/3/56 |
| Modelo/CAD | GLB final827232bytes/Fusionv16/Blender existentes | Nueva PCB/pieza/soporte diferidos a planos; sin nuevo CAD |

## Cambios de esta entrega

Se elimina el permiso interno Web confirmado por el usuario. No quedan métodos,
estados ni handlers de concesión. La Web envía directamente las30operaciones;
encoder y Web mantienen su navegación y condiciones de operación. Los códigos
de acción retirados quedan reservados; no se reasignan. C/TS ajustan índices de
menú y cuentan las mismas opciones. Los62documentos/fuentes con terminología
anterior se adaptan; fuentes originales y evidencias históricas se conservan.

Presencia interactiva no es permiso: no habilita ni arranca salidas y no reanuda
ejecuciones. Se retransmite edad, sin rejuvenecer observaciones viejas. Cambiar
de sesión, STOP, desenfoque/ocultamiento/salir de Control cancelan lo pendiente.
Firmware/paquete deben actualizarse juntos; no se flashea en esta tarea.

## Pendientes reales del plan

Hall/cableado/CPR/signos/geometría/PID y aceptación física; pila/ISR/tiempos con
ramas físicas habilitadas; identificar flash/SPI/layout y revisar espacio real;
pruebas de protocolo/fallos, raster diferencial y aceptación visual/móvil;
planos/componentes para PCB/soporte/cubierta. Gates Hall/memoria0 conservados.
Sin tests/validadores independientes, navegador, imagen FS, serie o flasheos.
Fuente y compilación no acreditan operación del robot ni precisión medida.

[Ficha](../changes/2026-10-25-01-direct-web-control.md),
[procedencia](../provenance/direct-web-control-20261025.json).
Planes anteriores en history/alignment15 y history/direct-control7, como historia.
