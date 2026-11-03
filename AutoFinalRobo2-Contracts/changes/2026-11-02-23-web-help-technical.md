# AFR2-20261102-23 — Ayuda técnica por módulo

02/11/2026. Pedido humano: corregir la ayuda y la documentación para que sean más técnicas, con detalles tomados de los repos de código. Se permite separar en secciones dentro de cada módulo, con el orden ATmega, ESP, Web y OLED.

## Cambios

- **Orden y sección inicial:** las pestañas pasan a ATmega · ESP · Web · OLED. Sin `?seccion=`, se abre ATmega. Los enlaces `?seccion=web|esp|atmega|oled` y la redirección `/oled` siguen funcionando.
- **Estructura de cada módulo:** una introducción y grupos con título. Cada tema es un desplegable con tres partes:
  - el texto;
  - una tabla de datos técnicos (dato → valor), cuando aplica;
  - la línea «Fuente», con el archivo o contrato de origen.
- **ATmega**, en cuatro grupos:
  - **Arquitectura y compilación:** carpetas `src/`, bucle cooperativo, objetivos de make, opciones de avr-gcc y último build registrado (AFR2-20261029-20). Temporización: Timer0 a 1 ms, watchdog de 500 ms y de 15 ms para reiniciar, e ISR.
  - **Hardware y pines:** motores con IN1/IN2/PWM y PWM de Timer1/Timer2 a ≈977 Hz; Hall en A8–A15 con PCINT2; HC-SR04 con Timer3 a 25 µs y tope de 32 ms; servo en D13 con Timer5 (1000–2000 µs cada 20 ms); ADC de IR y batería; panel OLED/encoder y conectores J1/J2.
  - **Comunicación con el ESP:** trama ROBO2, rutas, sesión y transacción, códigos de resultado, supervisión de 1500 ms y USART1 a 115200 (UBRR 16, U2X).
  - **Movimiento, secuencias y memoria:** guardas y STOP, ejecutor (32 instrucciones, 120 s) y journal EEPROM (2 × 128 B, CRC16-CCITT, commit 0xA5, v8).
- **ESP**, en cuatro grupos:
  - **Plataforma:** PlatformIO, bibliotecas fijadas, mapa 4m3m y módulos de `src/`.
  - **Servidor y WebSocket:** rutas HTTP y paquete gzip. Protocolo afr2-ws-1: una sesión, 4 pedidos más STOP prioritario, frames ≤ 1024 B, anidamiento ≤ 5 y cierres 1008/1002/1009.
  - **Puente con el ATmega:** transacción única con plazo de 800 ms, enlace caído a 1500 ms y captura OLED en páginas de 40 B. Shield y carga.
  - **Red, sistema y batería:** modos Wi-Fi, `system.info` y reinicios, batería.
- **Web**, en tres grupos:
  - **Arquitectura:** stack y versiones, estructura de `src/`. Cliente WebSocket: hello de 5 s, alive cada 500 ms, sesión de 30 s, reconexión de 500 ms a 15 s, plazo de 2,5 s y lecturas periódicas.
  - **Operación:** supervisión, manual y táctil, secuencias y GOTO con sus límites, calibración.
  - **Distribución:** pipeline de `build:esp`, gzip nivel 9 y manifiesto con SHA-256.
- **OLED**, en tres grupos (el catálogo queda abajo):
  - **Funcionamiento:** driver SSD1306 por TWI a 100 kHz, framebuffer de 8 páginas con tramos de hasta 16 columnas, encoder (INT4/INT5, 4 transiciones, antirrebote de 20 ms, largo ≥ 800 ms al soltar) y reglas de navegación.
  - **Pantalla en vivo y simulación:** `robot.screen.status`, `robot.screen.get`, `robot.ui.action`, modos Periódica / Al cambiar (AFR2-20261102-22) y copia Web del renderer.
  - **Catálogo de pantallas.**
- **Contenido anterior:** se conservó, reubicado y condensado. El tema «Árbol de pantallas y controles OLED», que estaba en Web, pasó a OLED.

Cada dato se tomó de la fuente citada:
- ATmega: `board_pinmap.h`, `afr2_board.c`, `afr2_sensors.c`, `afr2_twi.c`, `afr2_uart1.c`, `afr2_watchdog.c`, `afr2_preferences.c`, `afr2_link.c`, `afr2_oled.c`, `afr2_encoder_ui.c`, `Makefile`;
- ESP: `platformio.ini`, `broker.cpp`/`.h`, `mega_link.cpp`, `main.cpp`;
- Web: `package.json`, `afr2Client.ts`, `ConnectionContext.tsx`, `package-web.mjs`;
- Contracts: `UART_PROFILE_2.md` y `PINOUT.md`.

## Adaptación por archivo

Web/src:
- `features/docs/helpContent.ts`: nuevo; contenido tipado por módulo, grupo y tema.
- `features/docs/DocsPage.tsx`: orden, sección inicial y renderizado.
- `styles/workspace.css`: bloque de grupos, tabla de datos y fuente.

Contracts: CHANGELOG.md y README.md.

## Estado por repo

- **atmega / esp:** sin cambios de código. Solo se leyeron sus fuentes.
- **web:** fuente modificada.
  - **Verificado en el navegador local:** las cuatro pestañas a 1440 px, un tema abierto con su tabla y su fuente, la pestaña OLED con el catálogo debajo y ESP a 390 px sin desplazamiento horizontal. Consola sin errores propios; solo los del WebSocket sin placa.
  - **No verificado:** compilación TypeScript y `build:esp`. Los valores describen la fuente actual y no prueban el comportamiento físico.
- Sin commit.
