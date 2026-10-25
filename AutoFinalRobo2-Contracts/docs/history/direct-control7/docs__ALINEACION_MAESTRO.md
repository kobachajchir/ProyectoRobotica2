# Alineación del plan maestro — AFR2-20261024-15

24/10/2026. Revisión por lectura de fuentes activas de los cinco módulos del
Git único de D:/ProyectoRobotica2. Este es el estado vigente; los estados de
entregas anteriores no deben usarse como lista actual de código faltante.

El software definido en el plan tiene productores y consumidores en fuente.
Esta revisión completa dos huecos: diagnóstico LittleFS/paquete en la Web y
adquisición de control al escribir desde Calibración. **No cierra la aceptación
del proyecto:** faltan datos y comprobaciones físicas, memoria dinámica y CAD
diferido. Fuente, compilación y funcionamiento comprobado son estados distintos.

[Ficha15](../changes/2026-10-24-15-master-alignment.md),
[compilaciones y límites](../documentation/builds/2026-10-24-15/README.md),
[procedencia](../provenance/master-alignment-20261024.json).

## Contrato compartido y recorrido de los comandos

Los 33 comandos de commands.json corresponden al catálogo tipado Web y al
broker ESP. Los servicios de robot terminan en sus handlers AVR; los de red,
archivos y radio se resuelven en ESP. No todos requieren una pantalla o botón
propio: renew, carga de secuencias y paginado de framebuffer son internos.

| Servicio Web | ESP | ATmega / resultado |
| --- | --- | --- |
| system.hello, system.alive | broker.cpp, estado/capacidades de sesión | Hello/alive ROBO2 independientes; no conceden control |
| system.info | broker_system.cpp y Storage::health | INFO 0x50 paginado; identidad AVR o null. LittleFS local no requiere Mega |
| system.reboot | reinicio ESP diferido o puente REBOOT | 0x51, salidas apagadas y EEPROM libre; accepted no es boot confirmado |
| control.acquire, control.renew, control.release | token, dueño y TTL | LEASE 0x12; liberación detiene salidas |
| robot.stop | prioridad y revocación de operaciones anteriores | STOP 0x06; confirmación de salidas apagadas, no velocidad física medida |
| robot.state | STATE y POSE opcional | 0x11/0x18; modo, dueño, fallo, EEPROM y pose solo si válida |
| robot.drive, robot.mode, robot.test.wheel | validación y límites publicados | 0x13/0x15/0x14, autoridad y requisitos del controlador |
| robot.sensors, robot.encoders | validez y edades, sin completar ceros | 0x04/0x05; línea5, HC-SR04 y cuatro Hall condicionados |
| robot.goto.start, robot.goto.cancel | objetivo completo y cancelación | 0x16/0x17, odometría/lease/calibración; accepted no es llegada |
| robot.config.get, robot.calibrate | CONFIG y catálogo de IDs compartido | 0x1A, consulta o escritura RAM con salidas apagadas |
| robot.preferences, robot.line.calibrate | servicios explícitos | 0x1B/0x1C; guardado asíncrono y confirmación/invalidez de línea |
| robot.screen.get, robot.ui.action | captura coherente, revisión y permisos | 0x20..0x23, framebuffer1024bytes y encoder; prioridad local |
| robot.sequence.begin, append, commit, start, cancel, status | programa/ejecución, traducción EX01v6 | 0x40..0x45, 32registros/120s, ejecución bloqueada por requisitos |
| wifi.status, wifi.scan, wifi.configure | WifiService local, STA/AP/AP_STA | No necesitan Mega; eventos/status hacia OLED por canal de red |
| wifi.draft.submit, wifi.draft.cancel | borrador RAM acotado y jobs | 0x30/0x31; Conectar físico desde OLED aplica el borrador solicitado por el auto |

Fuentes enlazadas: [catálogo](../contracts/commands.json),
[tipos Web](../../AutoFinalRobo2-Web/src/protocol/wsEnvelope.ts),
[broker](../../AutoFinalRobo2-ESP/src/broker.cpp),
[adaptador](../../AutoFinalRobo2-ESP/src/broker_product.cpp),
[handler AVR](../../AutoFinalRobo2-Atmega/src/services/afr2_app_wire.c),
[cabecera compartida](../contracts/afr2_product_wire.h).

Framing activo: ROBO2/perfil2, extensión EX01v6 y sobre afr2-ws-1. Perfil1/UNER,
el diseño histórico perfil3 y EX01v1..v5 son antecedentes o compatibilidad
explícita, no alternativas para mezclar IDs. Firmware nominal0.8.0. La versión
Web procede de package.json; hash del índice y fecha de empaquetado distinguen
el paquete concreto. No se deriva versión desde el modelo CAD.

## Cobertura del plan por módulo

| Área | Fuente implementada | Estado y condición restante |
| --- | --- | --- |
| Plataforma AVR | reloj, USART1 IRQ, TWI/Manager, OLED y encoder por IRQ | Compilación12; temporización, recuperación y cableado pendientes |
| Navegación y pantallas | árbol completo MENU_PLAN,48nodos/11avisos, iconos/scroll, editores/teclado | Copia de biblioteca Web, catálogo59 y framebuffer; paridad visual/diferencial pendiente |
| Seguridad y potencia | STOP, lease, watchdog, cuatro PWM/direcciones, rampas e inversión | Fuente con salidas condicionadas; no hay parada física certificada |
| Sensores | línea5, HC-SR04 y captura Hall separada del encoder UI | Tipo/polaridad, niveles, temporalidad, cableado y calibración reales pendientes |
| Movimiento | PID por rueda, Mecanum, odometría, línea y GOTO relativo | Hall/geometría/CPR válidos; no se inventa pose a partir de consignas |
| Secuencias | editor v2, simulador y ejecutor de tiempo/distancia por rueda | Macros de memoria/Hall0; rama física habilitada sin compilar/probar |
| Configuración | IDs/rangos compartidos, RAM, EEPROM de dos registros, Hall local por vuelta | Web15 adquiere control al escribir y confirma salidas; aceptación de persistencia pendiente |
| Red y servidor | radio, preferencias, HTTP/LittleFS/WS y broker acotado | ESP15 añade diagnóstico/identidad de paquete; radio, FS y clientes no ensayados en placa |
| Sistema | versiones, boot IDs/reportes, reinicio ESP/Mega y avisos OLED | Sin reinicios físicos ejecutados ni garantía de recepción de todos los avisos |
| Web | rutas, control, simuladores, sensores/Wi-Fi/OLED, desarrollador, documentación | Compilación15 y paquete local; interacción visual/móvil no comprobada |
| Modelo base | Fusionv16/Blender, GLB final827232bytes y Draco local | Entrega CAD existente; ajuste geométrico/físico pendiente |
| PCB/pieza nueva | requisitos, conectores y estructura conservados | Diseño expresamente diferido a planos/componentes/indicaciones del usuario |

Hall se calibra desde OLED por una vuelta de rueda; la Web no simula esa
medición ni convierte CONFIG en invocación arbitraria. Servo opcional y
acciones locales mantienen su ámbito. Las diferencias deliberadas de permisos
entre OLED y Web no son handlers complementarios faltantes.

## Brechas corregidas en esta revisión

- Calibración dependía de un botón de control ya retirado. Ahora cada escritura
  solicita el lease mediante el flujo existente, consulta salidas apagadas y
  conserva las barreras de STOP, conexión, foco y cambio de tarea. Consulta de
  parámetro sin lease; lectura anterior inválida al cambiar de parámetro.
- PLAN_ESP pedía diagnóstico de FS/paquete pero solo existía /api/health.
  system.info y la sección Hardware muestran montaje, ocupación, fallo,
  versión, fecha, perfil, cantidad/tamaño y SHA del índice declarado. Paquetes
  antiguos siguen aceptándose y sus metadatos ausentes permanecen desconocidos.
- /api/health publicaba controlLease=none fijo, aunque el broker pudiera tener
  dueño. Se retira ese valor ficticio; la autoridad de control sigue reportándose
  mediante las respuestas y eventos WS del broker.
- Planes y maestros mezclaban estados históricos de esqueleto, pendientes de
  integración y versiones antiguas. Se sustituyen por estados vigentes y enlaces
  de evidencia; las copias anteriores se conservan en history/alignment15/.

No se cambian los layouts/recursos OLED, ScreenCodes, framing, permisos del
ATmega, pinout o gates por esta revisión. Sin dependencia o activo nuevo.

## Lo que impide declarar el proyecto completo

| Pendiente real | Dato o trabajo necesario | No se resuelve afirmando código completo |
| --- | --- | --- |
| Hall y calibración | ocho señales/niveles reales, CPR/signos por rueda, diámetro/geometría y ajuste de control | Mantener AFR2_HALL_CONNECTED=0 hasta resolver montaje y aceptación |
| Memoria/tiempos AVR | pila/ISR/carga real y revisión del peor caso con Hall/secuencias habilitados | AFR2_SEQUENCE_MEMORY_APPROVED=0; SRAM estática no aprueba margen dinámico |
| Puesta en marcha | OLED/encoder/TWI, sensores/PWM, STOP/TTL/watchdog, UART, Wi-Fi, reinicios y persistencia | Sin pruebas físicas ni flasheo implícitos |
| Flash y FS reales | identificar chip/SPI/layout y verificar espacio con el hardware instalado |4MiB/FS3MiB es candidato; promover archivos no carga LittleFS |
| Aceptación de software | pruebas de protocolo/correlación, fallos y comparación de raster; revisión visual/móvil | No ejecutadas por este pedido de revisión/implementación; builds separados |
| PCB/soporte/cubierta | planos, componentes/pull-ups/tensión/huellas, cotas/fijaciones y decisiones del usuario | J1 señales1x5 y J2 alimentación1x2,2,54mm; dos tornillos/tuercas, sin iniciar CAD |

Las compilaciones autorizadas se documentan por separado. No se ejecutan tests,
validadores independientes, navegador, serie, flasheo o pruebas físicas para
convertir artificialmente estas filas en completadas. Los hallazgos AFRDOC
conservan aceptación pendiente, aun cuando su brecha de fuente esté resuelta.
