# AFR2-20261030-13 — referencia adelante/atrás y giro

Fuente ATmega y Web implementada; revisión manual por lectura. ESP conserva el reenvío existente. Sin generación, compilación, tests, validadores, navegador, promoción, carga ni prueba física. No certifica el sentido físico tras instalar estas fuentes.

## Pedido y alcance

El usuario informa avance/retroceso invertidos y confirma también giro invertido; pide Girar CW y Girar CCW. Se conservó el desplazamiento lateral porque no se informó invertido. La referencia lógica sigue siendo X positivo adelante, Y positivo izquierda y yaw positivo antihorario visto desde arriba. CW es horario y CCW antihorario. Los signos de transporte de robot.drive, robot.drive.pwm y GOTO conservan esa semántica; no se vuelven a negar en WebSocket, ESP o botones.

El ajuste es fijo en la transformación chasis↔ruedas. Contracts/afr2_motion_reference.h es consumido por afr2_product.c; motion_reference.json es la contraparte de datos consumida por Web/control/motionReference.ts. Ambos fijan signo longitudinal -1, signo yaw -1 y lateral conservado. La base de comparación son las fuentes anteriores y la observación del usuario, no una inferencia del CAD ni una calibración eléctrica demostrada.

## Ecuaciones

Para el orden FL,FR,RL,RR se define f=-vx, r=-wz*(Lx+Ly), con wz en rad/s. La mezcla queda:

- FL = f - vy - r.
- FR = f + vy + r.
- RL = f + vy - r.
- RR = f - vy + r.

PWM usa la misma estructura con porcentajes: f=-forwardPct, r=-turnPct y vy=strafePct. Validación, normalización de pico, límite PWM, rampa, PID, permisos, STOP y tiempos conservan sus reglas. Se invierten entradas locales después de validar rangos.

Odometría aplica la transformación inversa: dx=-(dFL+dFR+dRL+dRR)/4; dy=(-dFL+dFR+dRL-dRR)/4; dYaw=-(-dFL+dFR-dRL+dRR)/(4*(Lx+Ly)). Se conserva la integración con rumbo medio. Las velocidades individuales p.speed y sus consignas PID permanecen en la misma base de rueda; invertirlas además produciría una segunda corrección y rompería la realimentación.

## Movimientos alcanzados

| Operación | Efecto respecto de las fuentes previas |
|---|---|
| Adelante / Atrás | Consignas de las cuatro ruedas intercambiadas en signo |
| Girar CCW / Girar CW | Giro intercambiado en signo; CCW sigue siendo consigna angular lógica positiva |
| Diagonales | Se invierte su componente longitudinal; se conserva el lado solicitado |
| Curvas / avance más giro | Se corrigen ambas componentes; no basta cambiar nombres de botones |
| Seguimiento de línea | Afr2Product_Drive aplica la corrección a avance y giro; conservados orden/polaridad/calibración IR y requisitos |
| GOTO relativo | Mezcla y odometría usan la misma referencia; X hacia delante y ángulo CCW mantienen su significado; no se niegan objetivos en el consumidor |
| Secuencias desde movimientos | resolveMovementBlockEndpoints recalcula ruedas para desplazamiento, rampa, pivot y curva; alcanza segmentos temporizados y pistas generadas para distancia |
| Simulación Web | Directa e inversa comparten signos; la trayectoria lógica sigue hacia el destino solicitado, con las nuevas órdenes de rueda |
| GOTO Web y vista manual | Adaptada también fórmula analítica de recorrido de ruedas y offsets de giro que no pasaban por la función inversa común |
| Simulador de recolección | Consume las mismas funciones directa/inversa y MecanumDrivePad; hereda referencia/etiquetas sin intervención en su lógica de misiones |
| Lateral puro | Conservado |
| Prueba o pista de rueda individual | Conserva índice, signo y magnitud de cada rueda; no se intercambia un motor ni se invierte una máscara EEPROM |

Las secuencias a nivel movimientos se normalizan desde sus bloques y vuelven a calcularse con la referencia actual. Los programas editados a nivel ruedas y segmentos ya calculados conservan literalmente sus valores; no se les atribuye una intención de chasis que el formato no contiene ni se migran automáticamente. Su proyección de pose usa la nueva cinemática. Se conservan etiquetas personalizadas; solo nombres automáticos antiguos exactos Girar izquierda/derecha en pivots se normalizan al importar según pivotDirection. Se actualizaron nombres por defecto y demo.

## OLED y Web

Mecanum030009 conserva identidad, filas, acciones y argumentos8=CCW/9=CW. Solo cambia texto a Girar CCW / Girar CW; no cambia la navegación del encoder. firmwareRenderer.ts porta las etiquetas por acción/argumento, incluso mientras la biblioteca generada conserve cadenas antiguas. MecanumDrivePad y selector de pivots muestran los mismos términos y flechas; wz positivo sigue CCW. El visor conectado sigue mostrando el framebuffer recibido. Sin nuevos iconos, glifos, pantallas ni buffers; costo de código/flash/pila no medido. El hash revisado del catálogo se actualiza tras portar las etiquetas; generador sin ejecutar.

No se modifican motor_signs, hall_signs, CPR, EEPROMv8, dimensiones nominales, nombres/índices FL/FR/RL/RR, sensor frontal, servo, rumbo visual de dos dedos ni la semántica de STOP. Los trims eléctricos/Hall existentes son independientes de la referencia del chasis. No se agrega wire: EX01v11 y los formatos actuales permanecen. Para compartir expectativas de secuencias desde movimientos deben desplegarse juntos estas fuentes AVR y Web; el paquete/firmware cargado anterior no cambia por editar fuentes.

## Pendientes autorizables

Sin ejecutar en esta entrega: compilación coordinada AVR/Web y regeneración de catálogo; revisar flash/SRAM/pila; comprobar con salidas y montaje adecuados adelante/atrás, CW/CCW vistos desde arriba, laterales y cuatro diagonales; comprobar combinaciones avance/giro, saturación y STOP; signo de pose X/yaw respecto de Hall; GOTO con traslación/rumbo; seguimiento con orden real IR; secuencias de movimientos temporizadas/por distancia y pistas individuales conservadas; fidelidad de etiquetas OLED/Web. Estas comprobaciones no se sustituyen por inspección matemática de las fórmulas.

## Actualización AFR2-20261107-41: lateral invertido

El usuario informó izquierda/derecha cruzadas al conducir. Se agrega `AFR2_LATERAL_WHEEL_SIGN` = -1 (y `lateralWheelSign` = -1 en el JSON). Con s = -vy (PWM: s = -strafePct) la mezcla queda FL = f - s - r, FR = f + s + r, RL = f + s - r, RR = f - s + r, y la odometría usa dy = -(-dFL + dFR + dRL - dRR)/4. Avance y yaw conservan -1. Los ejes lógicos no cambian (Y positivo = izquierda). La frase «lateral conservado» de las secciones anteriores queda reemplazada por esta. Ficha: [2026-11-07-41](../changes/2026-11-07-41-lateral-wheel-sign.md). Sin compilar ni prueba física.
