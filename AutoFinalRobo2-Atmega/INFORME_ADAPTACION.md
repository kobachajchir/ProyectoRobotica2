> **AFR2-20261030-18:** selector común de motor030044, pictogramas coherentes del encoder y dashboard con solo modo en el cuerpo. [Ficha y límites](../AutoFinalRobo2-Contracts/changes/2026-10-30-18-motor-selector-encoder-dashboard.md). Fuente AVR/Web sin compilar, probar ni cargar.

> **AFR2-20261030-13:** referencia longitudinal y yaw corregida; odometría/mezcla y cinemática Web coordinadas, Girar CW/CCW. [Ficha e impacto](../AutoFinalRobo2-Contracts/changes/2026-10-30-13-motion-reference.md). Solo fuente; sin generación, compilación, validadores, pruebas, promoción ni carga.

> **AFR2-20261030-09:** Wi-Fi coordinado EX01v11, EEPROMv8 sin cambios; [ficha de fuente y límites](../AutoFinalRobo2-Contracts/changes/2026-10-30-09-wifi-state-flows.md). Sin generación, compilación, validadores, pruebas, navegador, promoción ni cargas; evidencia anterior corresponde a fuentes históricas.

> **AFR2-20261030-07:** fuente coordinada EX01v10/EEPROMv8; [detalle](../AutoFinalRobo2-Contracts/changes/2026-10-30-07-ui-integral.md). Sin compilar, ejecutar, validar, probar, generar, promover ni cargar. Conserva evidencia previa solo para sus fuentes históricas.

> **AFR2-20261030-03:** IR1..IR5 ADC10 (0..1023), A4..A0; perfil predeterminado2 y journal6 migrando modos previos en RAM, sin escritura automática. ADC compartido con bateríaA5. [Contrato](../AutoFinalRobo2-Contracts/contracts/EEPROM_IR_ADC_6.md). Fuente sin compilar/probar/cargar; consumidores ESP/Web/OLED ya soportan kind2. Causa física de los ceros no confirmada.

> **AFR2-20261030-02:** cabecera dashboard con SSID e IP STA/AP y scroll independiente de ida/vuelta, recortado entre icono y batería fijos. Solo bandas de texto en redibujo parcial. Fuentes AVR/Web vinculadas, sin compilar ni cargar. Ficha en Contracts/changes/2026-10-30-02-dashboard-network-scroll.md.

> **AFR2-20261030-01:** dashboard010001 adaptado de AutoMicro con iconos Wi-Fi/AP/encoder, batería por bandas de voltaje y accesos corto Menú/largo Rápidas al soltar. AVR y copia raster Web vinculados, sin nuevo wire. Fuente sin compilar, probar ni cargar. Ver ../AutoFinalRobo2-Contracts/changes/2026-10-30-01-dashboard-automicro-icons.md.

> **AFR2-20261029-29:** desde dashboard, corto abre menu completo010002; largo al soltar abre pantallas rapidas010003. Ambos accesos conservan retorno al origen por historial. Sustituye el acceso comun de28. Fuente sin compilar/cargar.

> **AFR2-20261029-28:** dashboard abre menu de controles con clic corto; largo conservado al soltar. OLED/Web vinculados; sin compilacion, validadores, prueba ni carga.

> **AFR2-20261029-27 — fuente vigente:** ATmega/ESP0.11.0 y Web0.4.0; carrusel de controles, largo al soltar con retorno contextual, radar4/2/1/0.5m/rastro/editor compartido, IR compactos, PWM manual independiente de odometría/HC y atención prioritaria de sesión WS. EEPROMv5 / EX01v9, preservando batería26. Sin compilar, validadores, flasheos, pruebas físicas ni promoción Web. Recursos OLED copiados solamente; memoria y aceptación pendientes. [Contrato y límites](../AutoFinalRobo2-Contracts/contracts/QUICK_RADAR_ACTUATORS_9.md).

> **AFR2-20261029-26:** batería A5/divisor10k/10k/104, Sistema → Batería y copia Web, puente EX01v9, pill con icono por voltaje y confirmación antes de reiniciar ESP/ATmega desde Web. [Contrato y estados](../AutoFinalRobo2-Contracts/contracts/BATTERY_MEASUREMENT_9.md). Solo fuente: sin compilar, generar catálogo OLED, probar, promover paquete ni cargar. Memoria/calibración/paridad físicas pendientes; ambas placas y Web requieren despliegue conjunto autorizado.

> **AFR2-20261029-19 compilado y ATmega cargado:** pedido humano29/10/2026. ATmega/ESP/Web esp y LittleFS completados; promoción local Web a ESP/data/dist. Carga COM8 final13:32:50,80066bytes escritos/verificados, avrdude exit0. ESP/LittleFS sin cargar. SRAM estática7085bytes; sin prueba física/dinámica. Evidencia en AutoFinalRobo2-Contracts/documentation/builds/2026-10-29-19/README.md. Estados inferiores sin build/carga son históricos.

> **AFR2-20261029-20 compilado SIN cargar:** producto/comunicación/biblioteca Loader exit0 sin errores/advertencias, final29/10/2026 13:00:16. Flash80066bytes; SRAM7085bytes, margen1107bytes, main276 y Ui_Render133bytes según .su individuales; no certifica máximo dinámico de pila/ISR. [Evidencia](../AutoFinalRobo2-Contracts/documentation/builds/2026-10-29-20/README.md). Sin tests/validadores ni ensayo físico, Web fuente vinculada sin build/promoción; placa conserva15 y el pedido de no flashear sigue vigente. Estado pendiente de compilación inferior histórico.

> **Fuente AFR2-20261029-20:** Hall una vuelta con selección de rueda, conteo manual y clic para terminar; largo cancela con aviso. Drafts por4ruedas, estado CPR con asterisco, guardado asíncrono y diálogo genérico Si/No al salir sin guardar. OLED/Web vinculados en fuente; Web sin build/promoción, recursos generados pendientes del build autorizado. [Ficha](../AutoFinalRobo2-Contracts/changes/2026-10-29-20-hall-wizard-save-confirm.md). Compilación AVR pendiente al redactar; no flashear, placa conserva15.

> **AFR2-20261029-18 compilado SIN cargar:** diagnóstico BuildOnly exit0 y producto/comunicación/biblioteca Loader exit0, sin errores/advertencias. Diagnóstico flash13800/SRAM3180bytes; producto flash75600/SRAM6934bytes, margen1258bytes sin certificación dinámica de pila. Producto final12:34:40 del29/10/2026. Placa conserva15; no se abrió COM ni se ejecutó ensayo físico. Web solo fuente. Evidencia en Contracts/documentation/builds/2026-10-29-18/README.md; pendientes inferiores históricos. La prohibición de flashear sigue vigente.

> **Fuente AFR2-20261029-18:** IR1..IR5 OSOYOO digital (A4..A0), perfil predeterminado habilitado y EEPROMv4 migra solo tipos antiguos deshabilitados; calibración/seguimiento siguen pendientes. Diagnóstico servo/encoder agrega lectura RAW directa y cambios por OLED/serie, sin EEPROM. Vistas IR/Web distinguen deshabilitado/pendiente/cero válido. [Ficha](../AutoFinalRobo2-Contracts/changes/2026-10-29-18-ir-input-acquisition.md). Compilaciones pendientes al registrar la fuente; no cargar por instrucción vigente. Placa conserva15; Web sin build/promoción.

> **Diagnóstico17 compilado, sin cargar:** usuario confirma puente S1–D13 instalado además de GND/5V/S1 del servo. BuildOnly exit0, sin errores/advertencias, sin COM/avrdude. Flash13424bytes; SRAM3169bytes, margen5023bytes, sin certificación de pila. HEX afr2-i2c-diagnostic.hex SHA256 03cf3a3760f3b6bc5b9b8a7ce128a56388a5bd51b69209a2ea39e895283ca573. Causa del servo inmóvil aún sin medición/prueba física. Producto16 no cargado; placa conserva15. Las notas de compilación/puente pendientes inferiores son históricas.

> **Diagnóstico AFR2-20261029-17:** encoder controla servo D13/OC1C en pasos5grados; PWM inicialmente OFF, C=centrar, S=apagar PWM. OLED y serie muestran orden/contador/botón; I2C cada10s. S1 ModelY necesita puente adicional aD13 según manual. Script agrega -BuildOnly (sin COM/flash). [Ficha](../AutoFinalRobo2-Contracts/changes/2026-10-29-17-diagnostic-encoder-servo.md). Fuente lista, compilación de prueba pedida; no se autoriza carga. Producto16 sigue sin cargar; placa conserva15.

> **AFR2-20261029-16 compilado, NO cargado por indicación del usuario:** producto/comunicación/biblioteca exit0, sin errores/advertencias, final29/10/2026 12:15:28. Flash75518bytes; SRAM6918bytes, margen1274bytes; main259 y renderer73bytes según .su individuales, sin certificación dinámica de pila. COM8 conserva15 (cargada12:02:57). [Evidencia](../AutoFinalRobo2-Contracts/documentation/builds/2026-10-29-16/README.md). El usuario pidió continuar con otros cambios y posponer la carga; no se solicitará flasheo hasta nueva instrucción.

> **Fuente AFR2-20261029-16:** OLED sin esperas artificiales entre etapas, radar con grilla punteada, cono visual nominal15grados por PWM aplicado y discos rellenos por eco válido. Copia Web vinculada; sin build Web/promoción ESP. [Ficha](../AutoFinalRobo2-Contracts/changes/2026-10-29-16-oled-latency-radar-cone.md). Build/carga AVR COM8 autorizados, pendientes al registrar esta nota. Se conserva corrección del watchdog y migración Hall de15, ya cargada y verificada enCOM8 a12:02:57.

> **Fuente AFR2-20261029-15:** corregido bucle Bresenham del radar que bloqueaba main y podía disparar watchdog al girar desde HC-SR04; productos trigonométricos AVR en32bits y contraparte Web vinculada. Hall conectado por montaje confirmado, migración local EEPROMv3 preserva calibraciones y habilitación de control. [Ficha](../AutoFinalRobo2-Contracts/changes/2026-10-29-15-radar-watchdog-hall-wiring.md). Build/carga AVR COM8 autorizados, pendientes al registrar esta nota; sin prueba física del cambio.

> **AFR2-20261029-14 compilado y cargado:** 29/10/2026 11:37:12, COM8, Loader1.1.2,74516bytes escritos y verificados, exit0. Producto/comunicación/biblioteca compilados sin errores/advertencias. SRAM estática6889bytes, margen1303bytes; main259bytes según .su individual, sin certificación de máximo dinámico. [Evidencia](../AutoFinalRobo2-Contracts/documentation/builds/2026-10-29-14/README.md). La entrega sin compilar indicada debajo es histórica; sin ensayo físico posterior de UI.

> **Fuente AFR2-20261029-14:** recuperación OLED sin apagar ante error aislado, conserva páginas confirmadas y acota la postergación por trabajo pendiente. Reconfiguración sin0xAE tras fallos persistentes. Sin nueva compilación, tests, validadores, carga ni prueba física; COM8 conserva11. SRAM/pila y causa del negro permanente pendientes. [Investigación y cambios](../AutoFinalRobo2-Contracts/changes/2026-10-29-14-oled-fault-recovery.md).

> **Compilación14 posterior al pedido de fuente:** ATmega/ESP/Web compilados sin errores ni advertencias finales; carpeta build Windows y siete advertencias de indentación corregidas. Sin tests, navegador, flash ni promoción a ESP/data/dist. Ver Contracts/documentation/builds/2026-10-25-14/README.md. La falta de compilación mencionada más abajo describe la entrega previa.

> **Fuente vigente AFR2-20261025-14:** [sensores, servo y radar autónomo EX01v8](../AutoFinalRobo2-Contracts/contracts/SENSORS_RADAR_8.md). Firmware nominal0.10.0/Web0.3.0,41comandos/64identidades OLED. Fuentes vinculadas y recursos Web copiados; sin compilar, ejecutar tests/validadores, renderizar ni flashear. Paquete ESP anterior; los estados inferiores son históricos.

> **Vigente AFR2-20261025-01:** [control Web directo](../AutoFinalRobo2-Contracts/contracts/DIRECT_CONTROL_7.md), sin permiso previo ni habilitación desde OLED. Firmware0.9.0/EX01v7 y Web0.2.0,30comandos/59ScreenCodes, compilados juntos. STOP/caducidad/watchdog y requisitos físicos conservados; sin flash/tests/placas. Estados inferiores históricos.

> **Vigente AFR2-20261024-15:** [Matriz del maestro](../AutoFinalRobo2-Contracts/docs/ALINEACION_MAESTRO.md):33comandos y59identidades OLED con fuentes complementarias. Calibración Web adquiere control al escribir; ESP/Web informan FS/paquete. Web/ESP compilados15; AVR sin nueva fuente, build12/EX01v6/firmware0.8.0/gates0. Sin brecha software definida identificada tras lectura; aceptación física, memoria dinámica, flash/SPI y planos CAD pendientes. Solo documentación en CAD, sin diseño nuevo. Estados inferiores históricos.

> **Vigente AFR2-20261024-12:** mapa/comando actual juntos y coordenadas laterales; Hardware/Reinicio anidados, editor de pistas reversible y distancia por rueda EX01v6. Firmware0.8.0 y Web compilados; activos promovidos localmente. Build AVR incluye fuente IRQ11 con gates0; sin navegador, Hall habilitado, hardware, flasheo ni CAD. [Ficha12](../AutoFinalRobo2-Contracts/changes/2026-10-24-12-control-hardware-distance.md). Los estados inferiores son históricos.

> **Fuente vigente AFR2-20261024-11:** encoder CLK/DT en D2/D3 por INT4/INT5, SW D42; cola y acciones en main. Hall PCINT2 A8–A15 con preferencia de captura por software, macro0. **Esta adaptación no está compilada ni probada**; build08 corresponde a la fuente anterior. ESP/Web conservan su estado10. [Ficha11](../AutoFinalRobo2-Contracts/changes/2026-10-24-11-encoder-hall-interrupts.md). Los estados previos inferiores son históricos.

> **Vigente AFR2-20261024-08:** IP/AP en OLED,11tipos de aviso de red,14iconos F4 en flash y scroll de opción seleccionada. Sistema → Reinicio → Reiniciar ATMEGA/Reiniciar ESP/Volver. ATmega/ESP/Web compilados con EX01v5; sin placas, tests ni flasheos. Clave AP solo en OLED local; STA oculta. [Estado por módulo](../AutoFinalRobo2-Contracts/docs/ESTADO_IMPLEMENTACION.md) · [Contrato v5](../AutoFinalRobo2-Contracts/contracts/EX01_NETWORK_UI_5.md). Estados inferiores históricos.

> **AFR2-20261024-07:** renderer puro de secuencia, snapshot y navegación Volver/SW, etiquetas empaquetadas, INFO/REBOOT/BOOT y espera de UART completados en fuente y compilados. Fuentes originales/autorías conservadas; [adaptaciones por archivo07](../AutoFinalRobo2-Contracts/provenance/control-system-sequence-20261024.json). [Estado por módulo](../AutoFinalRobo2-Contracts/docs/ESTADO_IMPLEMENTACION.md). No acredita OLED, memoria máxima, movimiento ni reinicios físicos; gates de memoria/Hall0 conservados. Estados inferiores históricos.

> Estado vigente **AFR2-20261024-02**, 24/10/2026: integración ROBO2/perfil2 + EX01v3 compilada en Atmega/ESP/Web; GOTO completo, configuración por catálogo compartido y jobs Wi-Fi OLED conectados. Paquete Web actualizado en ESP/data/dist. Sin pruebas de navegador/hardware ni flasheo; secuencias físicas mantienen bloqueo de memoria. Ver [matriz vigente](../AutoFinalRobo2-Contracts/contracts/EX01_ALIGNMENT_3.md) y [evidencia](../AutoFinalRobo2-Contracts/changes/2026-10-24-02-project-alignment.md). Los estados previos que siguen son históricos.

> **AFR2-20261028-19:** diagnóstico I2C por USART0 preparado en fuente, Probe
> por el mismo Manager/TWI y HEX separado previsto. Sin compilación, pruebas,
> COM ni carga. [Procedencia](../AutoFinalRobo2-Contracts/changes/2026-10-28-19-oled-i2c-uart0-diagnostic.md).

> **AFR2-20261028-20:** servicio de diagnóstico19 ampliado con encoder y SW,
> reutilizando captura/decoder del producto. Sin compilación, pruebas o carga.
> [Procedencia](../AutoFinalRobo2-Contracts/changes/2026-10-28-20-encoder-uart0-diagnostic.md).

# Adaptación atmega

> **AFR2-20261029-11:** comparación exacta contra bytes ACKed y transporte
> cooperativo en bloques16; baja prioridad al final del main y pausa explícita.
> Fuente sin compilar/probar/cargar; SRAM/pila pendientes por nueva referencia
>1024bytes. [Procedencia](../AutoFinalRobo2-Contracts/changes/2026-10-29-11-oled-delta-cooperative.md).

> **AFR2-20261029-10:** configuración UI confirmada por usuario en servicio09
> trasladada al producto en fuente; defaults/migración parcial EEPROMv2,
> pulls1, espera500ms y refresco1s. Sin build/tests/carga. [Procedencia y límites](../AutoFinalRobo2-Contracts/changes/2026-10-29-10-product-oled-encoder-verified-setup.md).

> **AFR2-20261029-07:** diagnóstico activo OLED/encoder comparado con sketch
> Arduino funcional reportado; conserva C/avr-libc + Manager. Fuente sin
> compilar/cargar/probar. [Procedencia](../AutoFinalRobo2-Contracts/changes/2026-10-29-07-diagnostic-active-oled.md).

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





## AFR2-20261029-19 — OLED táctil y visor bajo demanda

Fuente adaptada con Web/ATmega/ESP: overlay scrim+blur en modales, Volver por ítem del menú, ScreenCode0x030035 con aviso/bitmap/chevrones y suscripción explícita a capturas/metadatos. Contrato en AutoFinalRobo2-Contracts/contracts/TOUCH_OLED_19.md; procedencia por archivo en provenance/touch-oled-20261029-19.json de Contracts. El rumbo es solicitado, no medido, y no agrega giro físico. Sin tests, compilación, validadores, navegador, ejecución del generador OLED, promoción ESP ni flasheo de esta entrega. Memoria/paridad/latencia físicas pendientes.
