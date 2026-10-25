# Plan ATmega vigente — M01..M19

AFR2-20261024-15. Conserva IDs/requisitos de la matriz original y el árbol
íntegro de [MENU_PLAN](../documentation/MENU_PLAN.md). Versión anterior en
[history](history/alignment15/AutoFinalRobo2-Contracts__docs__PLAN_ATMEGA.md).
AVR sin nueva fuente15: build12 incluye fuente IRQ11/distancia12, con gates0.

| ID | Requisito y fuente activa | Estado / condición pendiente |
| --- | --- | --- |
|M01|Reloj, UART1, TWI/Manager, OLED/encoder; main.c/platform|Integrado y build12; tiempos/cableado/bus sin probar|
|M02|Árbol, scroll, selección, edición; afr2_menu_catalog/afr2_screens|48nodos/59identidades compartidas, copia Web14; paridad pendiente|
|M03|Sistema, versiones/capacidades/diagnóstico; afr2_app/app_wire|INFO/BT01/versiones ESP/Web reales; no identidad inventada|
|M04|Notificaciones/cola/progreso/restauración; screens/network|11avisos de red, cola acotada/iconos; aceptación gráfica pendiente|
|M05|Wi-Fi/teclado o modal Web solicitado; app/broker_wifi|Jobs, borradores y modal conectados. Conectar físico aplica; sin prueba radio|
|M06|STOP/lease/watchdog; product/watchdog/app_wire|Autoridad de salidas/TTL/sesión integrada; no parada física medida|
|M07|Cuatro PWM/direcciones, rampas y prueba acotada; motor/product|Timer1/2 y servicio Web/ESP; signos y banco pendientes|
|M08|Línea5 digital/ADC, normalización/calibración; sensors/app|Tipo/polaridad/umbrales/calibración reales pendientes|
|M09|HC-SR04/timeout y servo local opcional; sensors|ECHO D31 muestreado Timer3/25us, servo Timer5; temporización física pendiente|
|M10|Hall y una vuelta manual; sensors/product/app|PCINT2 A8–A15 x1, separados de UI D2/D3; Hall macro0, sin conectar/calibrar|
|M11|Velocidad/PID/mezcla Mecanum; product|Fuente integrada; CPR/signos/PID/geometría sin calibrar|
|M12|Odometría/pose; product/app_wire y broker_product|Productor/consumidor afr2-pose-1 conectados; inválida hasta cumplir requisitos|
|M13|Seguimiento de línea; product y robot.mode|Fuente conectada; requiere línea/Hall/control calibrados|
|M14|GOTO preparar/iniciar/cancelar/resultado; product/app|EX01v3..v6 y Web completos; no precisión ni llegada física certificadas|
|M15|EEPROM versión/CRC, recuperación y commit incremental; preferences|save accepted, estado posterior confirma; cortes de energía no ensayados|
|M16|UI remota/framebuffer coherente; app_wire y LiveOledViewer|META/PAGE/ACTION/RELEASE conectados, revisión/edad/permisos locales; paridad pendiente|
|M17|Servicios remotos; app_wire y cabecera compartida|ROBO2/perfil2 EX01v6 negociado;33comandos en Web/ESP, sin perfil3 activo|
|M18|Recursos/autorías/procedencia y documentación|Manifests incrementales conservados; build no certifica recursos dinámicos|
|M19|Compilación/mapas/IRQ/interoperabilidad/banco|Build12 y mapas estáticos disponibles; pila/ISR/ramas Hall habilitadas y ejecución pendientes|

Extensión del alcance: ejecutor de secuencias0x40..0x45 y vista0x03002c con
etiquetas/iconos/tiempo/progreso,32registros y watchdog120s. Tiempo/distancia
por rueda requieren AFR2_SEQUENCE_MEMORY_APPROVED=0→aprobación explícita y
Hall/calibración reales; no modificar gates por reconciliar documentos.

Navegación: largo SW abre principal también desde teclado descartando borrador;
corto confirma, salida solo por Volver. Ningún menú paralelo ni botón menú.
Hall se mide localmente por rueda; acciones físicas y permisos no se eluden
desde encoder Web. Texto largo y biblioteca Web mantienen recursos/layouts AVR.

[Alineación completa](ALINEACION_MAESTRO.md), [contrato v6](../contracts/EX01_SEQUENCE_DISTANCE_6.md),
[configuración](../contracts/ATMEGA_CONFIG_ACTIONS.json), [build12](../documentation/builds/2026-10-24-12/README.md).
Faltan aceptación/cableado/calibración/memoria dinámica; no nuevos handlers
complementarios identificados en la revisión15. Sin pruebas ni flasheo nuevos.