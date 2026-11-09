# Cierre de fuentes — AFR2-20261109-03

Producto1.9.1, EX01v13 y EEPROMv11 sin cambios de formato. Complementa WIFI_AP_POLICY_20261109 y sustituye el bypass temporal de HALL_MANUAL_STREAM_20261106.

DRIVE_PWM0x1e / robot.drive.pwm conserva reserved:u32LE=0, forward/strafe/turn:i16LE porcentajes[-100,100], ttl:u16LE[200,1000]ms (12B), reply0B. Servicio AVR devuelve DENIED si fallo/captura Hall, UNAVAILABLE si motores no habilitados y BUSY si escritura EEPROM/radar/secuencia; dispatcher conserva el mapeo de esos estados a resultado wire UNAVAILABLE. Ninguna consigna habilita motores, borra fallos o cancela tareas. PWM sin odometría/HC obligatorios. Web no desplaza movimiento local ni edición local protegida. feature MANUAL_PWM anuncia la disponibilidad actual del servicio; el AVR vuelve a comprobarla al ejecutar.

ESP mantiene slot de última consigna con200ms de vigencia, sesión/generación, ALIVE y STOP; también exige observación real de estado AVR menor2s y MANUAL_PWM. accepted queued=true/confirmed=false no confirma ejecución. Las consignas no esperan ACK AVR para encolarse. Los fallos y los cambios de permiso siguen visibles en estado/capacidades existentes.

Wi-Fi/OLED sin cambios nuevos de wire: selector OFF acotado, política desconocida -- y búsqueda solo con bitSTA. Réplica Web y simulador alineados. Recursos OLED/modelos regenerados por herramientas propias (sin bypass del guard).

Compilación y despliegue pendientes a cargo del usuario. Fuentes preparadas no acreditan compilación, memoria suficiente, paridad de píxeles ni hardware.
