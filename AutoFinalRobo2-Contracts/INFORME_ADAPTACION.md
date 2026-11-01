> **AFR2-20261101-02:** Simulador UI actualizado por cotejo de Afr2Ui_Apply: corto/largo contextual, Hall, Si/No, rapidas y Wi-Fi. Arbol completo y limites en [UI_SCREEN_FLOW](documentation/UI_SCREEN_FLOW.md). Solo fuente; sin tests, builds, generadores ni hardware.

> **AFR2-20261030-07:** contratos y UI vinculada actualizados; [procedencia, estado y pendientes](changes/2026-10-30-07-ui-integral.md). Solo fuente, sin ejecución de generadores, pruebas/builds/validadores ni cargas.

> **AFR2-20261030-03:** cinco IR por ADC10 (0..1023), A4..A0, perfil2 y EEPROMv6 con migración de modos anteriores en RAM. ADC compartido con bateríaA5; descarte por cambio de canal. ESP/Web/OLED ya admiten ese rango. [Contrato y límites](contracts/EEPROM_IR_ADC_6.md). Solo fuente; sin compilación, pruebas ni carga. No acredita la causa de los ceros.

> **AFR2-20261030-02:** cabecera dashboard con SSID e IP STA/AP y scroll independiente de ida/vuelta, recortado entre icono y batería fijos. Solo bandas de texto en redibujo parcial. Fuentes AVR/Web vinculadas, sin compilar ni cargar. Ficha en Contracts/changes/2026-10-30-02-dashboard-network-scroll.md.

> **AFR2-20261030-01:** dashboard010001 adaptado de AutoMicro con iconos Wi-Fi/AP/encoder, batería por bandas de voltaje y accesos corto Menú/largo Rápidas al soltar. AVR y copia raster Web vinculados, sin nuevo wire. Fuente sin compilar, probar ni cargar. Ver changes/2026-10-30-01-dashboard-automicro-icons.md.

> **AFR2-20261029-29:** desde dashboard, corto abre menu completo010002; largo al soltar abre pantallas rapidas010003. Ambos accesos conservan retorno al origen por historial. Sustituye el acceso comun de28. Fuente sin compilar/cargar.

> **AFR2-20261029-28:** dashboard abre menu de controles con clic corto; largo conservado al soltar. OLED/Web vinculados; sin compilacion, validadores, prueba ni carga.

> **AFR2-20261029-27 — fuente vigente:** ATmega/ESP0.11.0 y Web0.4.0; carrusel de controles, largo al soltar con retorno contextual, radar4/2/1/0.5m/rastro/editor compartido, IR compactos, PWM manual independiente de odometría/HC y atención prioritaria de sesión WS. EEPROMv5 / EX01v9, preservando batería26. Sin compilar, validadores, flasheos, pruebas físicas ni promoción Web. Recursos OLED copiados solamente; memoria y aceptación pendientes. [Contrato y límites](contracts/QUICK_RADAR_ACTUATORS_9.md).

> **AFR2-20261029-26:** [Batería y reinicios](contracts/BATTERY_MEASUREMENT_9.md): productor A5, puente ESP y Web con pill/icono por voltaje, confirmación de reinicios. Fuentes vinculadas, sin compilación, generación OLED, pruebas, promoción ni carga.

> **AFR2-20261029-22:** corregido en fuente el argumento PowerShell de preparación FileSystem; el perfil instalado puede ajustarse sin recompilar. Sin ejecutar scripts, compilar ni flashear. [Causa y solución](changes/2026-10-29-22-loader-powershell-filesystem.md).

> **AFR2-20261029-19 compilado y ATmega cargado:** pedido humano29/10/2026. ATmega/ESP/Web esp y LittleFS completados; promoción local Web a ESP/data/dist. Carga COM8 final13:32:50,80066bytes escritos/verificados, avrdude exit0. ESP/LittleFS sin cargar. SRAM estática7085bytes; sin prueba física/dinámica. Evidencia en AutoFinalRobo2-Contracts/documentation/builds/2026-10-29-19/README.md. Estados inferiores sin build/carga son históricos.

> **Loader 1.1.2 / AFR2-20261029-13:** intervención KEY/RST ESP obligatoria y visible al frente aun con app minimizada/omisión de confirmaciones. Sin cambio en carga ATmega. GUI/MCP compilados y distribución generada; sin ejecución ni hardware. [Ficha](changes/2026-10-29-13-loader-esp-physical-required.md).

> **Loader 1.1.1 / AFR2-20261029-12:** autorización local por sesión para omitir confirmaciones manuales desde GUI/MCP, con advertencia obligatoria y comprobaciones automáticas conservadas. GUI/MCP compilados y distribución generada; sin ejecución ni hardware. [Ficha](changes/2026-10-29-12-loader-session-checks.md).

> **Loader 1.1.0 / AFR2-20261029-01:** preparación de FileSystem mediante script configurable; nuestra Web también usa ese lanzador. Incluye ejemplo sin Web y guía de integración. Fuente/documentación sin compilar ni ejecutar. [Ficha](changes/2026-10-29-01-loader-filesystem-scripts.md).

> **Loader AFR2-20261028-23 / 0.9.5:** preparación previa por sección, FileSystem/capacidad confirmada, verificación ESP integrada sin reconexión adicional, aviso KEY al primer porcentaje, prompts de Servidor MCP y manual/cableado/UNER renovados. Interfaz compilada y revisada visualmente; sin COM, flash ni compilación de firmware por el agente. [Ficha y límites](changes/2026-10-28-23-loader-flujo-manual-mcp.md).

> **Loader AFR2-20261028-17 / 0.9.5:** KEY corregido, botones y desplegables con contraste y separación propios, etapas sin scroll por operación, flasher visible y distribución revisada con capturas. Loader compilado y GUI revisada por pedido expreso; sin COM, carga ni firmware compilado. [Detalle y límites](changes/2026-10-28-17-loader-095-revision-visual.md).

> **Loader AFR2-20261028-16 / 0.9.5:** flujo ATmega/ESP, Configuración por placa, flasher separado, Preferencias en Opciones, tablas MCP/Eventos, colores/iconos y espaciado compartidos. Monitor con envío ASCII/HEX/números, CR/LF y repeticiones cancelables por el mismo dueño del COM, con registros TX separados. [Ficha y límites](changes/2026-10-28-16-loader-095-interfaz-envio-serie.md). Solo fuente, revisión manual: sin tests, validadores, compilación, GUI, COM ni flasheos.

> **Loader AFR2-20261028-15 / 0.9.4:** selección directa del BIN completo Ai-Thinker AT v1.5.4.1 descargado: reconocido por tamaño y SHA-256, mapa completo 4 MiB desde 0x0, DIO/40 MHz del archivo. Otros BIN requieren manifiesto del proveedor. Conserva consulta física, confirmaciones y verificación. Solo fuente, sin compilar, ejecutar ni flashear. [Ficha](changes/2026-10-28-15-loader-at-bin-directo.md).

> **Loader AFR2-20261028-13 / 0.9.3:** [Feedback de carga](changes/2026-10-28-13-loader-feedback-carga.md): tiempo del proceso, último mensaje, segundos sin salida y porcentaje de escritura informado por esptool. Detección por flasher reportada por el usuario; escritura y nueva interfaz pendientes de comprobar. Solo fuente; sin compilar, ejecutar ni abrir COM.

> **Loader AFR2-20261028-12 / 0.9.2:** [Conversor universal HEX / ASCII](changes/2026-10-28-12-loader-conversor-universal.md) para texto pegado, con vista legible/escapada, ASCII literal a HEX y copia normalizada. Sin asociación a comandos, COM ni jobs. Solo fuente; sin compilar/ejecutar/probar. Web/atmega/esp/CAD sin cambios de esta ficha.

> **Integración AFR2-20261028-11 / Loader0.9.1:** se incorporó origin/main2a72b9a (Loader0.9.0, servicio AVR y cambios CAD) conservando monitor HEX, USB–TTL, verificación y Protocol Studio locales. [Resoluciones y resguardo](changes/2026-10-28-11-integracion-local-remoto.md). Protocol Studio reasignado a09/10 para evitar IDs remotos repetidos. Solo fuente y Git; sin compilar, probar, validar, COM, flash ni promoción ESP.

> **Web AFR2-20261028-10:** [Esencia de AutoMicro en Protocol Studio](changes/2026-10-28-10-protocol-studio-essence.md): formularios, estructura visual, salidas simultáneas, mapa de captura y ayuda, con identidad AFR2. [Cotejo en fuente](docs/PROTOCOL_STUDIO_AUTOMICRO.md). Sin compilación, tests, validadores ni navegador; atmega/esp y paquete ESP sin cambios.

> **Web AFR2-20261028-09:** [Protocol Studio desde Desarrollador](changes/2026-10-28-09-web-protocol-studio.md), traductor de capturas HEX/texto y constructor ROBO2 offline. [Contrato y límites](contracts/WEB_PROTOCOL_STUDIO.md). Fuente sin compilar, probar, validar ni observar en navegador; ATmega/ESP y paquete ESP sin cambios de esta ficha.

> **Loader AFR2-20261027-30 / 0.7.4:** [selector ASCII / HEX con espacios](changes/2026-10-27-30-loader-serial-hex-ascii.md) en Monitor serie, con bytes originales y cambio de vista sin reabrir puerto. Registros, parser y journal MCP independientes del formato visual. Fuente sin compilar/ejecutar esta revisión; atmega/esp/web/CAD sin cambios por esta ficha.

> **Loader AFR2-20261027-29 / 0.7.3:** [consulta del cargador antes de verificar](changes/2026-10-27-29-loader-verification-bootloader-gate.md), con pausa y recuperación KEY/RST si no se confirma respuesta. No repite escritura; cancelar conserva resultado sin verificación completada. Fuente sin compilar/ejecutar esta revisión; atmega/esp/web/CAD sin cambios por esta ficha.

> **Loader AFR2-20261027-28 / 0.7.2:** [Limpiar ventana de Operación](changes/2026-10-27-28-loader-clear-operation-view.md) conserva logs, estados e historial y permite continuar viendo salida nueva durante trabajos. Fuente sin compilar ni ejecutar esta revisión; atmega/esp/web/CAD sin cambios por esta ficha.

> **Loader AFR2-20261027-27 / 0.7.1:** [escritura equivalente al comando manual y KEY sostenido](changes/2026-10-27-27-loader-esp-command-key.md). Fuente adaptada con Python `-u`, instrucciones USB–TTL/puente Mega y recordatorio de soltar KEY solo al terminar la operación. Sin compilar/ejecutar/probar esta revisión. Evidencia separada: el usuario aportó una carga manual ESP por COM5/115200 con hash verificado; sin prueba de arranque, Wi-Fi o LittleFS. atmega/web/CAD sin cambios de esta ficha.

> **Loader AFR2-20261027-24 / 0.7.0:** [alta, prueba y estado del servidor MCP](changes/2026-10-27-24-loader-mcp-connection-test.md), instrucción para Codex por sesión/prueba y casilla Autoaceptar visible desde el inicio. Fuente/documentación; sin compilar, ejecutar GUI/MCP, tests, validadores, COM ni flasheos de esta revisión. atmega/esp/web sin cambios por esta ficha.

> **Loader AFR2-20261027-23 / 0.6.1:** [visibilidad de Paquete AT según familia](changes/2026-10-27-23-loader-at-package-visibility.md). Corrección en fuente y documentación, sin compilar, ejecutar GUI/MCP, tests, validadores, COM ni flasheos de esta revisión. atmega/esp/web sin cambios por esta ficha.

> **Loader AFR2-20261027-22 / 0.6.0:** [familias Web/AT por USB y memoria](changes/2026-10-27-22-loader-web-at-usb-layouts.md), [reglas y esquema](contracts/LOADER_FLASH_MODES.md). Cargador/lanzador adaptados en fuente; sin compilar, ejecutar GUI/MCP, tests, validadores, COM ni flasheos de esta revisión. atmega/esp/web sin cambios por esta ficha; el firmware de producto no se convierte a AT.

> **Loader AFR2-20261027-21 / 0.5.0:** [autoaceptación MCP, historial Ctrl/Shift y perfiles locales](changes/2026-10-27-21-loader-autoaccept-profiles-history.md), con toast de copia y presentación de errores/foco inicial corregidos. Solo fuente/documentación; sin compilar, ejecutar GUI/MCP, tests, validadores, COM ni flasheos de esta revisión. atmega/esp/web sin cambios de esta ficha.

> **Vigente AFR2-20261025-01:** [control Web directo](contracts/DIRECT_CONTROL_7.md), sin permiso previo ni habilitación desde OLED. Firmware0.9.0/EX01v7 y Web0.2.0,30comandos/59ScreenCodes, compilados juntos. STOP/caducidad/watchdog y requisitos físicos conservados; sin flash/tests/placas. Estados inferiores históricos.

> **Vigente AFR2-20261024-15:** [Matriz del maestro](docs/ALINEACION_MAESTRO.md):33comandos y59identidades OLED con fuentes complementarias. Calibración Web adquiere control al escribir; ESP/Web informan FS/paquete. Web/ESP compilados15; AVR sin nueva fuente, build12/EX01v6/firmware0.8.0/gates0. Sin brecha software definida identificada tras lectura; aceptación física, memoria dinámica, flash/SPI y planos CAD pendientes. Solo documentación en CAD, sin diseño nuevo. Estados inferiores históricos.

> **Vigente AFR2-20261024-12:** mapa/comando actual juntos y coordenadas laterales; Hardware/Reinicio anidados, editor de pistas reversible y distancia por rueda EX01v6. Firmware0.8.0 y Web compilados; activos promovidos localmente. Build AVR incluye fuente IRQ11 con gates0; sin navegador, Hall habilitado, hardware, flasheo ni CAD. [Ficha12](changes/2026-10-24-12-control-hardware-distance.md). Los estados inferiores son históricos.

> **Fuente vigente AFR2-20261024-11:** encoder CLK/DT en D2/D3 por INT4/INT5, SW D42; cola y acciones en main. Hall PCINT2 A8–A15 con preferencia de captura por software, macro0. **Esta adaptación no está compilada ni probada**; build08 corresponde a la fuente anterior. ESP/Web conservan su estado10. [Ficha11](changes/2026-10-24-11-encoder-hall-interrupts.md). Los estados previos inferiores son históricos.

> **Vigente AFR2-20261024-10:** GOTO local por puntos del mapa y comandos ordenados, acento/seguimiento activo animados y controles laterales con mapa ancho completo. [Ficha10](changes/2026-10-24-10-goto-map-queue.md), [mapa](contracts/WEB_CONTROL_MAP.md) y [estado](docs/ESTADO_IMPLEMENTACION.md). Web compilada/promovida; firmware/ScreenCodes/gates sin cambios. Sin navegador, tests ni placas. Estados anteriores históricos.

> **Vigente AFR2-20261024-09:** catálogo Web59, copia de biblioteca AVR actual y skill que exige entregar ambas partes vinculadas. Visor conectado con framebuffer obligatorio; preview local identificado. [Ficha09](changes/2026-10-24-09-linked-oled-library.md), [estado](docs/ESTADO_IMPLEMENTACION.md) y [build09](documentation/builds/2026-10-24-09/README.md). Firmware conserva build08. Sin comparación diferencial, pruebas físicas ni flasheo. Estados anteriores históricos.

> **Vigente AFR2-20261024-08:** [Red OLED/reinicio EX01v5](contracts/EX01_NETWORK_UI_5.md), [estado Web/ESP/ATmega](docs/ESTADO_IMPLEMENTACION.md), [ficha](changes/2026-10-24-08-oled-network-restart.md) y [build08](documentation/builds/2026-10-24-08/README.md). IP/AP, avisos, iconos, scroll y menú Reinicio compilados. Sin prueba física, flasheos ni aprobación nueva de memoria/Hall. Estados anteriores históricos.

> **Vigente AFR2-20261024-07:** EX01v4, etiquetas de secuencias OLED, info/boot/reinicio y cliente Web implementados y compilados en tres módulos. [Estado por módulo](docs/ESTADO_IMPLEMENTACION.md), [adaptaciones por archivo](provenance/control-system-sequence-20261024.json) y [evidencia](documentation/builds/2026-10-24-07/README.md). Paquete1452387bytes gzip promovido; render bloqueado por permiso de navegador, sin hardware probado ni flasheo. Gates memoria/Hall0 conservados. Estados inferiores históricos.

> Vigente **AFR2-20261024-04**: **modelo final incluido en ESP**, paquete **1.450.201 bytes gzip** (49,96% menos que los 2,90 MB iniciales). WebP, JS/CSS minificados y dependencias/componentes optimizados. Layout candidato: flash 4 MiB / LittleFS nominal 3 MiB, solicitado por el usuario; flash fisica sin identificar. Web full/esp y firmware ESP compilados; activos promovidos a data/dist. Sin navegador, imagen FS ni flasheo. Atmega y comandos conservan el estado de02. [Evidencia](changes/2026-10-24-04-final-model-littlefs.md). Los estados previos inferiores son historicos.

> Estado vigente **AFR2-20261024-02**, 24/10/2026: integración ROBO2/perfil2 + EX01v3 compilada en Atmega/ESP/Web; GOTO completo, configuración por catálogo compartido y jobs Wi-Fi OLED conectados. Paquete Web actualizado en ESP/data/dist. Sin pruebas de navegador/hardware ni flasheo; secuencias físicas mantienen bloqueo de memoria. Ver [matriz vigente](contracts/EX01_ALIGNMENT_3.md) y [evidencia](changes/2026-10-24-02-project-alignment.md). Los estados previos que siguen son históricos.

# Función de Contracts

AFR2-20261031-01: [Simulador UI en Desarrollador](changes/2026-10-31-01-developer-ui-simulator.md).
Estado local Web y respuestas ficticias, reutilizando el encoder del visor en
vivo y dibujos/identidades del ATmega. Flujo y procedencia registrados;
atmega/esp sin cambios. Sin tests, compilaciones, validaciones ni promocion ESP.

AFR2-20261027-25: [identificación ESP8266EX y flash 4 MiB por USB–TTL](hardware/diagnostics/20261027-com5-usbttl-user-report/report.md).
Ejecución aportada por el usuario, esptool 4.5.1/COM5/9600, IDs `5e / 4016`.
Sin cambios de atmega/esp/web/Loader/CAD, nueva carga ni verificación de contenido.
Registro documental, sin pruebas nuevas del agente; layout y modo/frecuencia SPI pendientes.

AFR2-20261027-20: Loader 0.4.0 incorpora estado inferior, etapas tipadas,
scroll que respeta la lectura, selección compartida de compilación/carga,
detección ATmega/ESP y asistente separado K/RST. [Ficha](changes/2026-10-27-20-loader-workflow-scroll.md)
y [adaptaciones por archivo](provenance/loader-workflow-20261027.json).
Solo fuente/documentación; sin tests, compilaciones, validadores, ejecución,
COM ni flasheos. atmega/esp/web/CAD sin cambios de esta entrega.


Actualización ATmega19,24/10: fuente de producto y documentación completadas,
sin build/tests/validadores/hardware. Leer [matriz](docs/PLAN_ATMEGA.md) y [detalle vigente](docs/ATMEGA_IMPLEMENTACION_19.md).
Hall sin conectar y calibración/aceptación pendientes. EX01 requiere complemento
ESP/Web; perfil3/secuencias de21 es diseño separado. Las menciones previas a
menús/sensores/control pendientes de fuente son históricas.

Estado vigente **AFR2-20261023-15**: protocolo **ROBO2/perfil 2**, validación
compartida Mega/ESP y composición AVR USART1/Timer0/TWI/Manager/OLED/encoder en
fuente. Cuatro vistas locales conservadas; sin compilación ni pruebas de esta
entrega. Sustituye menciones previas a UNER/perfil 1 y backend TWI pendiente.
[Ficha](changes/2026-10-23-15-robo2-atmega-ui.md) ·
[Contrato](contracts/UART_PROFILE_2.md) ·
[Validación de ambos extremos](contracts/ROBO2_VALIDACION.md) ·
[Documento para ESP](../AutoFinalRobo2-ESP/docs/PROTOCOLO_ROBO2.md).
Sensores/STOP autoritativo/motores/lease/Hall/GOTO/UI remota siguen pendientes.

[Maestro](docs/INFORME_MAESTRO.md). Mantiene especificación, evidencia y coordinación. reference/automicro conserva AMDOC/AMI originales, sin transferir sus estados al nuevo hardware. Nuevas fichas AFR2-YYYYMMDD-NN y hallazgos AFRDOC-NNN.

Claves propias atmega/esp/web/shared; f4/esp/web/qt/shared se preservan dentro de las referencias. Documentar productor/consumidor, ID/versión, wire, longitud/tipos/endian/unidades, permisos, secuencia/correlación, ACK/resultado/eventos/timeout y despliegue. Distinguir diseñado, fuente parcial, adaptado, compilado y probado. No cambiar remotos por implicación.

AFR2-20261023-11 implementa en fuente el servidor ESP/LittleFS/Wi-Fi, broker WS/UART y endpoint USART1 AVR. [Ficha](changes/2026-10-23-11-esp-server-and-mega-link.md), [plan ESP](docs/PLAN_ESP.md), [perfil UART](contracts/UART_PROFILE_1.md) y [adaptaciones por archivo](provenance/esp-firmware.json). Sin builds, tests, validadores ni hardware en esta ficha; producto AVR/motores/lease/GOTO y nuevo empaquetado Web pendientes. El ID 10 corresponde a la reforma Web concurrente y se conserva.

AFR2-20261023-04 incorpora [AutoFinalRobo2-CAD](../AutoFinalRobo2-CAD/README.md) como módulo del Git único de D:/ProyectoRobotica2. Contracts conserva decisiones y cambios compartidos; CAD conserva fuentes mecánicas y eléctricas. [Organización e historial](docs/ESTRUCTURA_Y_GIT.md). No cambian wire ni implementaciones de atmega/esp/web.

AFR2-20261023-14 documenta la adaptación visual de ST2026-Radal a Web: [ficha](changes/2026-10-23-14-floating-web-shell.md), [procedencia](provenance/web-floating-ui.json) y [perfil consumidor](contracts/WEB_OLED_CONSUMER.md). Fuente Web sin compilar/renderizar ni probar; firmware y wire sin cambio.

AFR2-20261023-16: introducción de ST adaptada, BrandMark sin medallón y Sonner bottom-right. [Ficha](changes/2026-10-23-16-startup-and-unframed-logo.md), [procedencia/prompt](provenance/web-startup.json). Fuente sin compilar/renderizar/probar; firmware/API sin cambios.

AFR2-20261023-18 incorpora mapa de Control, refinamiento del velo Radal y shell Sonner legible. [Ficha](changes/2026-10-23-18-control-map-and-ui-polish.md) / [procedencia](provenance/web-control-map.json). Sin firmware nuevo ni pruebas/build/render de esta entrega; pose es diseño consumidor pendiente de productores.


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
