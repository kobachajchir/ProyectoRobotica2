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

