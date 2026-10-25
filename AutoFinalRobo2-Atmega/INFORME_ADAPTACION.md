> **Vigente AFR2-20261025-01:** [control Web directo](../AutoFinalRobo2-Contracts/contracts/DIRECT_CONTROL_7.md), sin permiso previo ni habilitación desde OLED. Firmware0.9.0/EX01v7 y Web0.2.0,30comandos/59ScreenCodes, compilados juntos. STOP/caducidad/watchdog y requisitos físicos conservados; sin flash/tests/placas. Estados inferiores históricos.

> **Vigente AFR2-20261024-15:** [Matriz del maestro](../AutoFinalRobo2-Contracts/docs/ALINEACION_MAESTRO.md):33comandos y59identidades OLED con fuentes complementarias. Calibración Web adquiere control al escribir; ESP/Web informan FS/paquete. Web/ESP compilados15; AVR sin nueva fuente, build12/EX01v6/firmware0.8.0/gates0. Sin brecha software definida identificada tras lectura; aceptación física, memoria dinámica, flash/SPI y planos CAD pendientes. Solo documentación en CAD, sin diseño nuevo. Estados inferiores históricos.

> **Vigente AFR2-20261024-12:** mapa/comando actual juntos y coordenadas laterales; Hardware/Reinicio anidados, editor de pistas reversible y distancia por rueda EX01v6. Firmware0.8.0 y Web compilados; activos promovidos localmente. Build AVR incluye fuente IRQ11 con gates0; sin navegador, Hall habilitado, hardware, flasheo ni CAD. [Ficha12](../AutoFinalRobo2-Contracts/changes/2026-10-24-12-control-hardware-distance.md). Los estados inferiores son históricos.

> **Fuente vigente AFR2-20261024-11:** encoder CLK/DT en D2/D3 por INT4/INT5, SW D42; cola y acciones en main. Hall PCINT2 A8–A15 con preferencia de captura por software, macro0. **Esta adaptación no está compilada ni probada**; build08 corresponde a la fuente anterior. ESP/Web conservan su estado10. [Ficha11](../AutoFinalRobo2-Contracts/changes/2026-10-24-11-encoder-hall-interrupts.md). Los estados previos inferiores son históricos.

> **Vigente AFR2-20261024-08:** IP/AP en OLED,11tipos de aviso de red,14iconos F4 en flash y scroll de opción seleccionada. Sistema → Reinicio → Reiniciar ATMEGA/Reiniciar ESP/Volver. ATmega/ESP/Web compilados con EX01v5; sin placas, tests ni flasheos. Clave AP solo en OLED local; STA oculta. [Estado por módulo](../AutoFinalRobo2-Contracts/docs/ESTADO_IMPLEMENTACION.md) · [Contrato v5](../AutoFinalRobo2-Contracts/contracts/EX01_NETWORK_UI_5.md). Estados inferiores históricos.

> **AFR2-20261024-07:** renderer puro de secuencia, snapshot y navegación Volver/SW, etiquetas empaquetadas, INFO/REBOOT/BOOT y espera de UART completados en fuente y compilados. Fuentes originales/autorías conservadas; [adaptaciones por archivo07](../AutoFinalRobo2-Contracts/provenance/control-system-sequence-20261024.json). [Estado por módulo](../AutoFinalRobo2-Contracts/docs/ESTADO_IMPLEMENTACION.md). No acredita OLED, memoria máxima, movimiento ni reinicios físicos; gates de memoria/Hall0 conservados. Estados inferiores históricos.

> Estado vigente **AFR2-20261024-02**, 24/10/2026: integración ROBO2/perfil2 + EX01v3 compilada en Atmega/ESP/Web; GOTO completo, configuración por catálogo compartido y jobs Wi-Fi OLED conectados. Paquete Web actualizado en ESP/data/dist. Sin pruebas de navegador/hardware ni flasheo; secuencias físicas mantienen bloqueo de memoria. Ver [matriz vigente](../AutoFinalRobo2-Contracts/contracts/EX01_ALIGNMENT_3.md) y [evidencia](../AutoFinalRobo2-Contracts/changes/2026-10-24-02-project-alignment.md). Los estados previos que siguen son históricos.

# Adaptación atmega

Estado de producto **AFR2-20261023-19**, continuación24/10/2026: fuente integrada,
sin compilar ni ejecutar. [Guía vigente](docs/PRODUCTO_ATMEGA.md),
[matriz del maestro](../AutoFinalRobo2-Contracts/docs/PLAN_ATMEGA.md) y
[contrato EX01](../AutoFinalRobo2-Contracts/contracts/ROBO2_PRODUCT_EXTENSION.md).
44 nodos/UI, Sistema, Wi-Fi/teclado/avisos, potencia/sensores, Hall/PID/odometría/
línea/GOTO condicionados, EEPROM y watchdog. Hall sin conectar, macro0 y CPR0.
STOP apaga salidas; no mide rueda detenida. Fuente escrita no acredita SRAM,
compilación ni funcionamiento físico. El texto previo queda como historia.


Estado vigente AFR2-20261023-15: ROBO2/perfil 2 y composición AVR UI/TWI escritos, sin compilar/probar. Sustituye menciones anteriores a UNER/perfil 1 y backend TWI pendiente. Ver actualización al final.


[Informe maestro](../INFORME_MAESTRO.md) · [Contracts](../AutoFinalRobo2-Contracts/README.md).

Se copió todo Core propio F4 como referencia autocontenida de librerías y composición; HAL/CMSIS/USB middleware y build STM32 no se migran. MPU, balance, AT y botón menú permanecen solo para localizar acoplamientos históricos. La base src/ contiene una adaptación parcial nueva, no el firmware del vehículo.

Código nuevo: services/afr2_i2c_manager.c, display/afr2_oled.c y font7x10.c, input/afr2_encoder_ui.c y ui/afr2_screens.c. API pública en include/. El Manager está reducido a un OLED, con start/poll/abort y callbacks en main. Wrappers usan snapshots. Fuente 7x10 con PROGMEM. Encoder largo/corto excluyentes y Volver por ítem. AFR2-20261023-11 agrega endpoint ESP, USART1 por IRQ y composición opcional de comunicaciones con reloj de 1 ms. AFR2-20261023-15 agrega backend TWI y composición local UART/OLED/encoder; faltan composición completa de producto, sensores/motores/odometría/GOTO.

Makefile conserva ayuda por defecto y library; firmware integra UART/TWI/OLED/encoder y communication-firmware mantiene opción UART sola, ambas con potencia inactiva. No se ejecutó make ni se generó ELF/HEX/mapa, y no hay upload. [Composición](docs/COMPOSICION.md).

## Guías junto al código

| Subsistema | Para qué sirve |
| --- | --- |
| [Comandos, parser y transporte](docs/modules/protocol.md) | Reutilizar parser de bytes, framing, cola, tablas de handlers y separación de transporte. UNER_CoreConfig permite aportar slots/pool; no hace falta HAL para el núcleo. uner_app.c es composición del robot anterior, no biblioteca neutra. |
| [I2C Manager, SSD1306 y recursos](docs/modules/i2c-oled.md) | Preservar propiedad del bus, callback diferido a main, deadline/recovery, framebuffer por páginas y separación dibujo/transporte. El define SSD1306_USE_DMA existe, pero su rama 0 usa mem_write en bucle; no convierte a TWI asíncrono. AM_I2C_Hardware original conserva métodos DMA. |
| [Wrappers, menús y pantallas](docs/modules/screens.md) | Wrappers preparan datos/contexto; renderer dibuja; navegación decide pantalla/selección; acciones llaman servicios. Reutilizar conceptos y recursos, no System/globales completos. La referencia incluye menús/OSK/avisos y toda la biblioteca propia para port posterior. |
| [Encoder de interfaz y pulsador único](docs/modules/encoder-ui.md) | encoder.c F4 procesa un encoder rotativo y su pulsador. Con la ampliación solicitada vuelve a ser una referencia útil para UI; no mide las cuatro ruedas. user_button queda como antecedente del botón eliminado. |
| [Cinco sensores de línea](docs/modules/line-ir.md) | Reutilizar normalización, estado/calibración, filtrado y separación sensor/control. El origen modela ocho TCRT5000 (línea/objetos), adquisición ADC DMA y emisores por grupo. El módulo OSOYOO de cinco puntos no prueba esas mismas capacidades. |
| [Motores, PID, Mecanum y GOTO](docs/modules/motion.md) | Reutilizar arbitraje de fuentes, estados de misión/cancelación, límites, PID y estructura de perfiles. El origen es diferencial/balance; AutoMotion ready exige MPU/calibración/balance y objetos IR. El estimador no sustituye odometría de cuatro Hall. |
| [Wi-Fi, preferencias y composición AVR](docs/modules/wifi-composition.md) | Bridge UNER y estados de configuración muestran pedidos/respuestas y separación de dueño Wi-Fi. main/app_tasks/globals son mapa de integración, no código que se retargetea cambiando includes. |
| [Nuevo driver HC-SR04](docs/modules/hcsr04.md) | No se localizó un driver reusable directo en la fuente F4. El pinout recibido asigna TRIG D30 y ECHO D31. |

Cada carpeta contiene INFORME_CARPETA.md con enlaces. reference/ no participa en la configuración activa. Los archivos originales se identifican por hash en Contracts/provenance/files.json. No se modifica ni publica en AutoMicro.

## Comunicación con ESP — AFR2-20261023-11

[Guía de integración](docs/COMUNICACION_ESP.md) y [perfil UART compartido](../AutoFinalRobo2-Contracts/contracts/UART_PROFILE_1.md). Fuente nueva sin compilar ni probar. El endpoint informa unknown/invalid antes de integrar productores y no confirma STOP sin callback autoritativo; la composición opcional anuncia solo UART. No habilita motores ni lease al conectar una Web. El renderer OLED/UI se conserva; su composición física se integra en AFR2-20261023-15. [Registro y procedencia](../AutoFinalRobo2-Contracts/changes/2026-10-23-11-esp-server-and-mega-link.md).


## Integración ROBO2 y UI — AFR2-20261023-15

Estado vigente: `src/main.c` integra USART1, Timer0 1 ms, backend TWI por IRQ,
I2C Manager, OLED y encoder físico. C/avr-libc sin Arduino ni DMA. Cuatro vistas
locales existentes, sin alterar renderers. Sensores inválidos mientras no haya
productores; STOP autoritativo, motores, lease, Hall y GOTO pendientes.

Framing **ROBO2**, dos tokens, versión 1/perfil 2, ruta con emisor alto y receptor
bajo. Comparte parser y constantes con ESP. No compatible con UNER/perfil 1.
[Composición](docs/COMPOSICION.md), [enlace Mega](docs/COMUNICACION_ESP.md) y
[contrato vigente](../AutoFinalRobo2-Contracts/contracts/UART_PROFILE_2.md).
`make firmware` queda preparado, sin ejecutar; no se produjo ELF/HEX/mapa.
OLED 0x3C/SSD1306, tensión, pull-ups, pasos/sentido del encoder y tiempos son
candidatos sin comprobar en hardware. No se ejecutaron tests, compilaciones,
validadores, flasheos ni pruebas físicas. No se habilita movimiento.


## Integración vigente — AFR2-20261024-01

Revisadas fichas19/20/21. La Web21 ya contiene los cambios visuales/editor/simulador;
Wi-Fi20 se conserva. La integración de firmware pendiente se adapta en fuente:
ROBO2/perfil2 + EX01 v2, estado/pose, control supervisado, framebuffer OLED y
secuencias0x40..0x45 sin colisión con comandos de producto19.
Sin nuevo build, validador, tests, flasheo o promoción LittleFS. Secuencias físicas
permanecen bloqueadas por defecto hasta aprobar SRAM/pila y calibración real.
Contrato en AutoFinalRobo2-Contracts/contracts/EX01_WEB_INTEGRATION.md y ficha
AutoFinalRobo2-Contracts/changes/2026-10-24-01-firmware-web-integration.md.



