# Recuperación de fallos y diagnóstico temporal · AFR2-20261110-14

Versión: 1.22.0 (subversión). Fuente ATmega/ESP/Web coordinada. No se ejecutaron tests, compilaciones, validaciones, generadores, navegador, COM ni flasheos. La captura del usuario muestra F1, pero no identifica la demora causante. No se da por corregida ni confirmada una causa física.

## Qué significa F1

Afr2Product_Task observa el tiempo transcurrido desde su última actualización de control. Si supera 100 ms y hay salidas activas (pwm distinto de cero y stopped=false), registra F1 y detiene motores. Es una fecha límite del ATmega, no el tiempo de respuesta del Wi-Fi, ESP o navegador. La marcha nominal del control tiene un mínimo de 10 ms. El fault queda enclavado hasta una limpieza explícita o un reinicio. **AFR2-20261110-15 (1.22.1):** el corte de salidas sigue siendo inmediato en cada demora, pero solo se enclava F1 con 3 demoras dentro de 2 s; una aislada permite que el próximo comando vuelva a mover con rampa desde cero. `late_count` cuenta todos los cortes.

La hipótesis inicial del reloj antiguo tras limpiar fue descartada como causa demostrada: DrivePwm actual modifica targets y deja la aplicación real del PWM para Product_Task; además existe la guarda de PWM cero de AFR2-20261110-09. No se cambia esa guarda ni se reinicia la base temporal de la odometría para ocultar una demora. El umbral de 100 ms permanece intacto.

## Corrección de recuperación

El panel Web anterior solo tenía STOP, información y reinicio; el texto remitía al encoder. Ahora tiene **Limpiar fallo**, colocado antes de la explicación larga para ser accesible en móvil. Requiere estado reciente con salidas detenidas y diagnóstico AVR del mismo código; no simula el borrado del contador.

Nuevo CMD compatible **0x56 FAULT** en EX01 vigente, sin reutilizar ID ni cambiar framing/EEPROM. Se registró en header, wire_catalog, commands.json y Protocol Studio/codec.

- Request de 2 bytes: operation u8 (0 leer, 1 limpiar), expected_fault u8 (0 si leer; código no nulo si limpiar).
- Reply de 18 bytes: fault u8, stopped u8, late_count u16LE, control_gap_ms u16LE, y total/app/uart_debug/input/render/io como seis u16LE en ms.
- WebSocket: robot.fault.get, sin payload; robot.fault.clear con confirm=true y expectedFault=1..255. El ESP valida y traduce; el ATmega comprueba código esperado, stopped, PWM cero y ausencia de captura Hall/secuencia activa. No requiere habilitar motores para reparar un fallo.
- Se usa el mismo Afr2Product_ClearFault del encoder. La respuesta correcta confirma fault=0 y stopped=true, y la OLED recibe el aviso existente Fallo limpiado. No reinicia, no borra configuración y no reanuda marcha.
- Web conserva el estado físico del lector robot.state compartido; la pill se actualiza con la siguiente respuesta real. La biblioteca OLED local sigue siendo una simulación identificada; sus guardas de Hall/secuencia se alinean en fuente con la acción local.

## Reducción de demora y evidencia para la próxima aparición

Main vuelve a atender App_Task, si el control está vencido al menos 10 ms y hay marcha, después de UART/encoder y antes de sumar un render. Usa la misma envoltura App para no perder notificaciones de preferencias. Evita acumular el trabajo de entrada y todo el dibujo sin atender control. No introduce control desde ISR ni servicios dentro de renderers. Una tarea individual aún puede demorar más de 100 ms; no se afirma un límite medido.

Al disparar F1 se conservan: separación real de control (saturada a 65535 ms), contador saturado de disparos desde arranque y contexto de tiempos por etapas. El contexto es el ciclo anterior completo, o el ciclo actual parcial si el disparo ocurre en la atención adicional antes de render. No debe confundirse una fase asociada con la prueba de causalidad ni exigirse que su total coincida con el intervalo entre controles.

La Web permite leer esos valores y el log USB emite FAULT F1 gap/loop/render y FAULT stages app/uart/input/io. Se conservan al limpiar para que el usuario pueda compartirlos. No son lecturas simuladas. Tras cargar firmware nuevo habrá datos de etapa solo si vuelve a dispararse F1; un fallo ocurrido antes de cargar no trae retrospectivamente esos tiempos.

Memoria nominal añadida a Afr2Product: dos estructuras de seis u16 y dos u16 = 28 bytes, más una estructura local de 12 bytes en main antes de optimizaciones. No hay nuevo framebuffer. Tamaños enlazados, pila/ISR y presupuesto real pendientes de compilación autorizada.

Fuentes: Atmega/include/afr2_product.h; src/main.c y src/services/{afr2_product,afr2_app,afr2_app_wire}.c; ESP/src/broker_product.cpp; Web/src/protocol/{robotFaults.ts,wsEnvelope.ts}, features/connection/RobotAlerts.tsx, features/developer/uiSimulation.ts y features/protocolStudio/{catalog,codec}.ts. La causa exacta de la captura sigue sin evidencia temporal del equipo; no se abrió ningún puerto para obtenerla.
