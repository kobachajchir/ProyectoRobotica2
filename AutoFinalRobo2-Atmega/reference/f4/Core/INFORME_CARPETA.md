# Carpeta reference/f4/Core

[Maestro](../../../../INFORME_MAESTRO.md) · [Informe del repositorio](../../../INFORME_ADAPTACION.md).

Fuentes históricas de referencia: no forman parte del producto adaptado.

## Utilidad y reformas específicas

### [Encoder de interfaz y pulsador único](../../../docs/modules/encoder-ui.md)

encoder.c F4 procesa un mando rotativo y su pulsador. Con la ampliación solicitada vuelve a ser una referencia útil para UI; no mide las cuatro ruedas. user_button queda como antecedente del botón eliminado.

Reformas: CLK D40/PG1 y DT D41/PG0 se leen juntos; SW D42/PL7 activo bajo. Inicializar desde niveles reales, evitar gesto de arranque pulsado, rechazar transición cuadratura inválida y ajustar signo/transiciones por detent. Base: 1 ms poll, 20 ms debounce, 800 ms largo exclusivo; ajustar sin asumir hardware probado. El corto sale al soltar solo si no se emitió largo. No usar polling de UI para Hall motriz.

### [I2C Manager, SSD1306 y recursos](../../../docs/modules/i2c-oled.md)

Preservar propiedad del bus, callback diferido a main, deadline/recovery, framebuffer por páginas y separación dibujo/transporte. El define SSD1306_USE_DMA existe, pero su rama 0 usa mem_write en bucle; no convierte a TWI asíncrono. AM_I2C_Hardware original conserva métodos DMA.

Reformas: La base nueva reduce Manager a un cliente OLED y cambia la API (no es ABI drop-in). start/poll/abort no bloquean; dirección es 7 bits. Se incluye inicialización SSD1306 128x64, framebuffer 1024, snapshot TX de 129 y dirty pages. Falta backend TWI AVR y confirmación de controlador/voltaje/dirección OLED. Portar fuentes/iconos grandes a PROGMEM y revisar punteros/far. Mantener buffers hasta fin/abort; render y callbacks solo en main. No reinicializar un handle en vuelo. INVERT nuevo usa XOR, diferencia intencional del F4 histórico.

### [Cinco sensores de línea](../../../docs/modules/line-ir.md)

Reutilizar normalización, estado/calibración, filtrado y separación sensor/control. El origen modela ocho TCRT5000 (línea/objetos), adquisición ADC DMA y emisores por grupo. El módulo OSOYOO de cinco puntos no prueba esas mismas capacidades.

Reformas: Confirmar salida digital/analógica y polaridad. A4..A0 son canales de línea, no dirección I2C. Si digital: bitmask de cinco con validez/edad y calibración de polaridad; no inventar raw ADC. Si analógico: ADC10 bits sin DMA, secuencia iniciar/esperar/publicar y umbrales recalibrados. Retirar tres sensores de objetos y control de emisores si no existe cableado. Adaptar seguimiento a cuatro ruedas/robot estable sin balance.

### [Motores, PID, Mecanum y GOTO](../../../docs/modules/motion.md)

Reutilizar arbitraje de fuentes, estados de misión/cancelación, límites, PID y estructura de perfiles. El origen es diferencial/balance; AutoMotion ready exige MPU/calibración/balance y objetos IR. El estimador no sustituye odometría de cuatro Hall.

Reformas: Crear driver cuatro ruedas FL/FR/RL/RR y mezclar vx/vy/wz con geometría/signos calibrados. Usar PID de velocidad por rueda, no balance. Captura Hall por diseñar: variante/PPR/niveles/cuentas no verificados. Sin IMU, orientación solo estimada por ruedas y susceptible al deslizamiento. GOTO requiere encoder/odometría válida; cancelar/STOP/obstáculo con timeout. No falsificar ready de MPU para saltar guardas. Evitar floats costosos en ISR; unidades y saturación explícitas. MPU/balance se conservan solo como referencias de acoplamientos, fuera del producto.

### [Comandos, parser y transporte](../../../docs/modules/protocol.md)

Reutilizar parser de bytes, framing, cola, tablas de handlers y separación de transporte. UNER_CoreConfig permite aportar slots/pool; no hace falta HAL para el núcleo. uner_app.c es composición del robot anterior, no biblioteca neutra.

Reformas: Reemplazar UART DMA por USART1 con ISR RX/UDRE y rings estáticos. ISR solo mueve bytes/contadores; PushByte/dispatch en main con presupuesto. Tratar overflow y resincronización. Reducir router/transportes/snapshots a lo necesario, conservar lifetime de payloads hasta ReleasePacket. Serializar LE con tipos de tamaño fijo y evitar shifts promocionados a int AVR de 16 bits. No copiar sizeof(struct). Elegir IDs/longitudes en Contracts antes de activar handlers. Retirar STM32 USB CDC, NRF, bootloader y comandos MPU/balance.

### [Wrappers, menús y pantallas](../../../docs/modules/screens.md)

Wrappers preparan datos/contexto; renderer dibuja; navegación decide pantalla/selección; acciones llaman servicios. Reutilizar conceptos y recursos, no System/globales completos. La referencia incluye menús/OSK/avisos y toda la biblioteca propia para port posterior.

Reformas: Nueva UI usa snapshot explícito sin consultas de hardware en renderer. Mantener identidad/rol/estado/acciones por separado. Largo SW abre menú; corto selecciona; atrás solo por ítem Volver. Retirar llamadas al botón menú y no reinterpretar largo como atrás. Main procesa acciones y respeta valid/unknown. Llevar menús/textos a flash; decidir contexto de edición/confirmación sin ejecutar motor al navegar. Skill en Contracts documenta cada pantalla; paridad Web pendiente.

### [Wi-Fi, preferencias y composición AVR](../../../docs/modules/wifi-composition.md)

Bridge UNER y estados de configuración muestran pedidos/respuestas y separación de dueño Wi-Fi. main/app_tasks/globals son mapa de integración, no código que se retargetea cambiando includes.

Reformas: Elegir solo backend UNER; retirar AT y modem sequences. ESP posee radio/credenciales. AVR conserva preferencias de control en EEPROM con versión/CRC, actualización acotada y desgaste; no copiar sectores flash STM32. Rehacer reloj/ISR/watchdog/arranque; no incluir HAL/Cube/USB/boot-mailbox. Revisar RAM de System/handles y evitar logs en UART ESP. Build solo cuando se solicite.

## Subcarpetas

- [Inc](Inc/INFORME_CARPETA.md)
- [Src](Src/INFORME_CARPETA.md)

Estado: material preparado sin build, tests, validadores ni prueba física. Los hashes de fuentes copiadas están en [manifest](../../../../AutoFinalRobo2-Contracts/provenance/files.json).
