> **Aclaración vigente19:** el táctil sí requiere modelo3D cargado y vista lista. Se conserva el permiso de motores común y la corrección del error persistente descritos aquí. Las afirmaciones de independencia del modelo de esta revisión18 son históricas y quedan sustituidas por [TOUCH_MODEL_READY_19](TOUCH_MODEL_READY_19.md).

# AFR2-20261030-18 — permisos de Mecanum y control táctil

Solicitud: revisar por qué el táctil informa No disponible/restricciones cuando debería permitir el mismo movimiento manual que Mecanum. El usuario recuerda No disponible, sin captura o texto exacto. Se revisaron fuentes de Web, ESP y ATmega; no se ejecutaron pruebas, validadores, builds, generadores, conexión al robot ni cargas. No se atribuye el fallo observado a una condición concreta del equipo sin telemetría.

## Ruta y condiciones efectivas

Ambos mandos Web llaman ControlPage.startHoldVector → ConnectionContext.sendPwmCommand → Afr2Client.sendPwm → robot.drive.pwm → ESP AFR2_EXT_DRIVE_PWM → Afr2Product_DrivePwm. Mecanum en OLED también invoca Afr2Product_DrivePwm con porcentajes desde AFR2_A_MOVE. No usan el servicio de velocidad física robot.drive para estas maniobras.

| Capa | Condición de movimiento manual |
|---|---|
| Web, ambos mandos | WebSocket ready, enlace Mega ready, STOP sin estado sending/unconfirmed-offline, capability manual_pwm.implemented=true |
| ESP | Producto EX01 negociado con versión exacta, enlace y funciones actuales; manual_pwm anunciado desde el ATmega. No usa calibrated como permiso de PWM |
| Habilitación AVR | config.motor_ready=1. Se configura en Calibración → Motores → Habilitar control; no se activa automáticamente por abrir un modal |
| Estado AVR | Sin fault, sin medición Hall en curso, sin escritura EEPROM prefs_state=1 |
| Operaciones | Radar detenido y secuencia no ocupada; el endpoint rechaza secuencias en carga/preparadas/ejecución y edición angular de servo. El takeover táctil aceptado realiza Stop antes de activar su presentación |
| Propietario | No se toma un movimiento local todavía activo desde una orden Web; web_command conserva la guarda de origen |
| Comandos | Porcentajes -100..100, TTL200..1000ms; la Web usa400ms y renueva nominalmente cada180ms. Conserva límites y rampa |
| Continuidad | Heartbeat Web supervisado a1500ms, foco/visibilidad/soltar/STOP mantienen cancelación. Fallos del lazo y el watchdog Hall declarado disponible siguen deteniendo motores |

PWM manual NO requiere pose válida, CPR calibrados, dimensiones calibradas físicamente, lectura de HC-SR04, aprobación de memoria de secuencias ni carga del modelo 3D. manual_pwm.calibrated=false y textos informativos sobre hardware sin certificar no son condiciones de bloqueo. La calibración/odometría sí corresponde a robot.drive en mm/s, línea, GOTO o secuencias según sus requisitos, que no se eliminaron. Si Hall está declarado disponible, el watchdog de falta de pulsos puede generar fault3 aunque PWM no exija CPR; no se quitó esa supervisión.

La presentación OLED táctil es otro comando: puede rechazarse con teclado/editor, selección Hall, confirmación Sí/No, reinicio pendiente, enlace no negociado o latch tras retorno largo. Su error se muestra como OLED y no se convierte en una supuesta calibración de motor. Un retorno largo posterior a activación sigue cancelando y cerrando el táctil conforme a15. No se modifica el contrato ni se elimina ninguna guarda AVR/ESP.

## Diferencia encontrada y corrección

ControlPage ya compartía canDrive entre Mecanum y el modal, pero ManualDriveOverlay añadía !error a physicalReady. Un rechazo transitorio dejaba driveError persistente y el táctil no podía generar la siguiente solicitud que lo limpiase; el pad Mecanum sí podía reintentar. Ahora un error cancela el gesto actual y solicita el STOP existente; no funciona como latch de permiso adicional. Una vez recuperado canDrive, se exige un nuevo gesto/tecla. No se reanuda la dirección previa, y una tecla repetida o un contacto todavía en drenaje no se usa para rearmar movimiento.

Además, los callbacks de carga/fallo del modelo 3D todavía cancelaban gestos pese a que15 ya había retirado modelReady/modelFailed del permiso. Se dejan como estado de presentación, sin alterar órdenes físicas. Se conserva la cancelación por eventos reales de entrada, foco, STOP, enlace y cierre.

ControlPage prepara driveBlockedReason con el mismo predicado de habilitación que utilizan ambos controles; se pasa al modal y se muestra en el aviso general. Distingue falta de WS, falta de enlace ATmega, STOP en curso/no confirmado y motivo de disponibilidad PWM reportado por ESP. Una última orden fallida se identifica como antecedente y permite volver a deslizar cuando el permiso real se recuperó. No se anuncian fallos de 3D como bloqueo del robot.

## Archivos y estado

Web/src/features/control/ControlPage.tsx: permiso/motivo comunes y prop blockedReason. Web/src/features/control/manual/ManualDriveOverlay.tsx: elimina latch por error histórico, cancela el gesto que falla, callbacks visuales independientes y mensajes diferenciados. Web/src/features/docs/DocsPage.tsx: explicación de condiciones y reintento. Contracts: ficha, contrato, flujo, informes y procedencia18. AVR/ESP solo leídos; no se modifican EX01v12, EEPROM, motores, sensores, ScreenCodes, recursos ni framebuffer.

Estado de entrega: fuentes Web corregidas y revisión manual de flujo/efectos. El build14 previo no contiene estas correcciones. Paquete ESP/build/prueba física pendientes de pedido expreso para esta revisión.

## Comprobaciones futuras, sin ejecutar

Comparar pad/táctil con mismo snapshot de capacidades/STOP; manual_pwm calibrado=false con implemented=true; modelo lento/fallido; rechazo transitorio seguido de STOP confirmado y nuevo gesto; STOP no confirmado permanece bloqueado; pérdida/retorno del enlace sin repetir gesto; calibración Hall/EEPROM/radar/secuencia/servo rechazados por firmware; aviso OLED sin falsa atribución a calibración; teclas repetidas y contactos en drenaje; cierre/largo y captura de pantalla. No se probó físicamente ninguna maniobra en esta entrega.
