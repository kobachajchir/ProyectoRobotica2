# AFR2-20261030-08 — Conexión y simulación en control táctil

30/10/2026. Pedido del usuario: indicar Simulado únicamente si el auto está desconectado; pill superior derecha Conectado/punto verde o Desconectado/punto rojo, con punto izquierdo centrado y espaciado correcto.

## Comportamiento

La fuente Web considera conectado al auto cuando ambos enlaces existentes están ready: WebSocket Web–ESP y enlace ESP–Mega. Conexión no equivale a permiso para mover. Sin capacidad PWM manual o con error se muestra Control no disponible/control bloqueado, sin presentarlo como simulación. Sin enlace, los textos Simulado, velocidad/rumbo/giro simulados y simulación detenida describen la vista local. Con enlace, la rotación local con dos dedos/Q/E conserva su comportamiento sin yaw físico y se rotula Vista local; cerrar y abrir restaura el contexto de control disponible.

Pill con texto visible y role=status: punto decorativo8px a la izquierda, gap8px, padding6px/12px, cápsula y colores semánticos success/error existentes para ambos temas. Cabecera interna en dos columnas con separación8px/12px y márgenes laterales16px o safe-area mayor; el rótulo izquierdo ajusta sus líneas en ancho reducido y la pill permanece arriba a la derecha. pointer-events:none conserva los gestos de la superficie. Posición superior derecha de la zona táctil, debajo de la cabecera del modal.

Abrir conectado selecciona contexto robot aunque el permiso de movimiento esté bloqueado, sin pasar a simulación por ese bloqueo. Cambio de conexión cancela entrada sostenida y cambia destino local/robot, sin reproducir un gesto previo al reconectar. Fallo de modelo3D bloquea control físico y conserva su mensaje/reintento; no cambia conexión. STOP, TTL, confirmación de comandos, heartbeat y habilitación de motores existentes se conservan.

## Funcionamiento y estado real

El recorrido fuente es ControlPage → sendPwmCommand → robot.drive.pwm → broker ESP → servicio PWM ATmega. Requiere capacidad manual_pwm anunciada y firmware compatible; el Mega valida motor_ready/fallo/Hall/EEPROM/radar según servicio. La pantalla también conserva su requisito de modelo3D listo. TOUCH_UI de la fuente actual usa EX01v10 para presentar potencia confirmada; no se infiere compatibilidad con firmware anterior ni con lo instalado.

Web: solo fuente editada; sin compilar, abrir navegador, probar en teléfono, generar ni promover al ESP. ATmega/ESP: sin cambios de esta ficha ni carga. Lectura manual exclusivamente, sin tests, validadores, COM ni movimientos físicos. No se acredita que el control funcione ahora con las versiones cargadas. Skill mobile-native aplicada a safe-area y disposición; revisión en dispositivo pendiente por restricción del proyecto. No se modifican OLED, ScreenCodes, wire ni navegación física.

Autorías previas conservadas. Procedencia por archivo: provenance/touch-connection-20261030-08.json.
