## AFR2-20261027-25 — 27/10/2026

Consultas chip_id/flash_id por USB–TTL COM5 aportadas por el usuario: ESP8266EX,
chip `0x00c2218e`, cristal 26 MHz, flash `5e / 4016`, **4 MiB**. Registro de
salidas y límites; sin carga, verificación de contenido, cambios de firmware,
perfil o Loader, ni pruebas nuevas del agente para este registro.
[Ficha](changes/2026-10-27-25-esp-usbttl-identification.md).

## AFR2-20261027-24 — 27/10/2026

Loader0.7.0: estado del servidor MCP, comando para añadirlo e instrucción de
prueba por sesión desde Codex. Indicador conectado con prueba válida y señal
de vida del cliente; endpoint stdio/local sin IP ni puerto TCP. Autoaceptar
visible desde el inicio con filas automáticas. Fuente/documentación; sin
tests, compilación, validadores, GUI/MCP, COM ni flasheos de esta revisión.
[Ficha](changes/2026-10-27-24-loader-mcp-connection-test.md).

## AFR2-20261027-23 — 27/10/2026

Loader0.6.1: Paquete AT se oculta en Web propio y aparece solo en familia AT,
también al cargar/cambiar perfiles. Guardia de acción según familia y estado.
Fuente/documentación; sin tests, compilación, validadores, GUI/MCP, COM ni
flasheos de esta revisión. [Ficha](changes/2026-10-27-23-loader-at-package-visibility.md).

## AFR2-20261027-22 — 27/10/2026

Loader0.6.0: familia ESP Web/AT por USB, filesystem opcional y tamaños con
resumen de memoria; reglas separadas de linker Arduino y paquete AT por regiones.
Builds Web con layout/recibo, carga AT multi-BIN verificada y lanzador con selección
de familia/perfil. ATmega conserva su flujo; firmware de producto sin backend AT.
Solo fuente/documentación; sin tests, builds, validadores, GUI/MCP, COM ni flasheos
de esta revisión. [Ficha](changes/2026-10-27-22-loader-web-at-usb-layouts.md).

## AFR2-20261027-21 — 27/10/2026

Loader0.5.0: autoaceptación MCP opcional por sesión, historial Ctrl/Shift y
copia con toast, selector de perfiles y Guardar como, aviso del monitor
alineado/capitalizado y asistentes sin selección inicial. Estado accepted y
modo de aprobación registrados; asistentes físicos conservados. Solo fuente
y documentación; sin tests, compilación, validadores, GUI/MCP, COM ni flasheos
de esta revisión. [Ficha](changes/2026-10-27-21-loader-autoaccept-profiles-history.md).

## AFR2-20261027-20 — 27/10/2026

Loader 0.4.0: barra de estado global, indicadores de etapas, consolas con posición
y selección conservadas, dropdown de compilación vinculado al destino de carga,
detección ATmega/ESP junto a Destino y asistente K/RST separado con aviso de puerto
listo. Redacción de cableado central sin repetir retiro de D18/D19. Solo fuente
y documentación; sin tests, builds, validadores, ejecución, COM ni flasheos.
[Ficha](changes/2026-10-27-20-loader-workflow-scroll.md).

## AFR2-20261027-21 — 27/10/2026

Brazo v08: housing GX3270BLS ventilado, parámetros y archivos independientes;
reservas de tapa608, insertos, pestañas, abrazadera, tornillo y placa XL4016
corregidas. Revisión autorizada: 3.141 configuraciones sin penetraciones
reportables, seis ejes/doce rodamientos completos, pinza y límites nativos.
Masas/cargas recalculadas; J1/apoyos/alimentación pendientes. Sin FEA ni
aceptación física. [Ficha](changes/2026-10-27-21-arm-housing-rechecked.md).

## AFR2-20261027-20 — 27/10/2026

Brazo v07: servo GX3270BLS 270° en codo, modelo paramétrico independiente,
housing integrado con cuatro M3, acople, solapes de 27 mm y nervios locales
sin elevar la base. Masas/CG/estática actualizadas y frenadas ilustrativas
con seis componentes de carga por sección. J1 y alimentación pendientes;
sin barridos CAD, FEA, tests, compilaciones ni capacidad física acreditada.
[Ficha](changes/2026-10-27-20-arm-gx3270bls-reinforcement.md).

## AFR2-20261027-19 — 27/10/2026

Loader 0.3.0: monitor serie de lectura con baudios configurables, consola RX,
capturas de texto/bytes/eventos, observación incremental de arranque y diagnóstico
detallado de fallos. Libera el COM antes de diagnóstico/carga y expone captura por
MCP; manual y estados actualizados. Identificador separado del informe del brazo.
Solo fuente/documentación, sin tests, compilación, ejecución, COM ni flasheo.
[Ficha](changes/2026-10-27-19-loader-serial-monitor.md).

## AFR2-20261027-18 — 27/10/2026

Informe integrado del brazo PVC32: motivos de correcciones v01–v06, masas y
estática existentes de 200 g, método de frenada finita/esfuerzos combinados y plan
v07 de actuadores y refuerzos. Prioridad base, hombro, primer eslabón y unión al
acrílico; candidatos comerciales diferenciados de motor elegido. Índice y
capítulos navegables, sin nuevos cálculos, CAD, FEA o comprobaciones.
[Ficha](changes/2026-10-27-18-arm-design-iteration-report.md).

## AFR2-20261027-17 — 27/10/2026

Brazo v06: masas/CG de 264 cuerpos y carga horizontal de 200 g; copia nativa de
estudio conservando el maestro. Peso propio, fuerzas/momentos por junta, apoyos y
engranajes calculados; sensibilidad FDM y criterios de refuerzo documentados.
Codo 15,83 kgf·cm estáticos, objetivo preliminar ×2 superior al bloqueo publicado
del DS3218. Sin FEA, refuerzos nuevos, despieces ni capacidad física acreditada.
[Ficha](changes/2026-10-27-17-arm-mass-gravity-200g.md).

## AFR2-20261027-15 — 27/10/2026

Brazo v06 con contraplaca inferior única PETG, conservando montura baja y J1
retrasado. Seis ejes/doce rodamientos completos y 3.141 configuraciones CAD sin
penetraciones reportables; extremos/apertura nativos y transmisiones revisados.
Maestro/CSV/evidencia guardados. Skill de láminas planificada; despieces pendientes
de la señal del usuario. Sin fabricación, estabilidad ni capacidad de carga física
acreditadas. [Ficha](changes/2026-10-27-15-arm-single-underplate-reviewed.md).

## AFR2-20261027-14 - 27/10/2026

Loader pasa a carpeta propia AutoFinalRobo2-Loader en la raíz, sexto módulo
del único Git. Fuentes, lanzadores, perfiles y manual trasladados completos;
referencias y reglas de organización actualizadas. Sin cambio funcional,
compilación, ejecución, tests, validadores ni flasheo.
[Ficha](changes/2026-10-27-14-loader-root-module.md).

## AFR2-20261027-13 - 27/10/2026

AFR2 Loader0.1.0 para Windows: perfiles/rutas/COM configurables, builds
ATmega/ESP/Web/LittleFS, detección de boot UART y consulta ROM diferenciadas,
asistentes de cableado y confirmación, carga/verificación y logs. C# WinForms
con lanzador que genera EXE local al abrir; manual HTML y Acerca de con los
datos de Koba. Solo fuente, sin compilar/ejecutar/probar ni abrir COM/flashear.
[Ficha](changes/2026-10-27-13-windows-loader.md).

## AFR2-20261027-11 — 27/10/2026

Puntos cartesianos persistentes compartidos entre Brazo y Conjunto, con edicion,
importacion/exportacion, IK en Worker, vista previa y reproduccion de trayectorias.
Descriptor y adaptador visual independientes del GLB; perfil nominal v03 y destinos
conservados al cambiar dimensiones. Cancelacion e invalidacion por contexto.
[Ficha](changes/2026-10-27-11-arm-points-model-agnostic.md) y
[contrato/guia](contracts/WEB_ARM_POINTS.md). Solo fuente: esta entrega no ejecuta
builds/tests/validadores/navegador ni promocion ESP. No se acredita que el paquete
de la ficha10 corresponda a esta fuente final.

## AFR2-20261027-10 - 27/10/2026

ATmega producto/comunicación/biblioteca, ESP y Web perfil esp compilados. Hall
habilitado por pedido humano y cableado confirmado; preferencia EEPROM
conservada. Web promovida a ESP/data/dist (8activos,1571104bytes gzip) y
LittleFS3121152bytes generado. Corrección de retorno CollectorState para
startManualScan. Guía USB-B/COM16 y lanzador Mega preparados, sin ejecutarlos.
Sin tests, navegador, apertura serie ni flasheo; flash/jumpers por identificar.
[Ficha](changes/2026-10-27-10-builds-hall-flashing-preparation.md).

## AFR2-20261027-09 — 27/10/2026

Brazo sobre AGV v04: dos anclajes M3 izquierdos con contrabarra completan seis
laterales más el central. Alza paramétrica de 32 mm y giro nativo solidario de
salida SG90, soporte y HC-SR04. Envolvente circular completa libre y 363 casos
CAD sin penetraciones reportables; montajes internos y presentación nativa
revisados. Tapa J1 con rebaje independiente corregido. F3D/126 parámetros y
vistas guardados; carga, estabilidad y fabricación física pendientes.
[Ficha](changes/2026-10-27-09-arm-bilateral-mount-pan-clearance.md).

## AFR2-20261027-07 - 27/10/2026

Pinout completo del auto en Markdown y PDF A4 de dos páginas, con referencias y
autoría Hecho por Koba Chajchir. Hall en dos headers consecutivos: A8-A11=A,
A12-A15=B, ambos FR/FL/RR/RL; fuente AVR y máscara0x0F alineadas, sin cambiar
contadores lógicos ni wire. Panel OLED/encoder a 5 V según montaje comunicado;
CAD previo +3V3 no editado. Hall deshabilitado; sin tests/build/flasheo,
validadores ni validación visual/física. [Ficha](changes/2026-10-27-07-complete-pinout-grouped-hall.md).

## AFR2-20261027-06 — 27/10/2026

Calibracion con 29 fichas contextuales a la derecha; matematica mayor y centrada,
con un mismo dibujo y formulario para Auto/Brazo/Conjunto. Pad de movimiento,
boton mantenido y campos numero/barra compartidos con Control; giros alineados
bajo columnas extremas. Adaptadores fisico/simulado separados. Solo fuente,
sin tests/build/validaciones/navegador ni promocion ESP.
[Ficha](changes/2026-10-27-06-calibration-live-math-shared-controls.md).

## AFR2-20261027-03 — 27/10/2026

Brazo sobre AGV v03 guardado: guías continuas, doce rodamientos, ejes y sujeciones,
montura sobre agujeros del auto, contraplaca y electrónica vertical.114 parámetros,
despieces y2.904 posturas CAD muestreadas sin penetraciones reportables. Carga,
estabilidad y fabricación pendientes. Firmware/Web sin cambios de esta ficha.
[Alcance y evidencia](changes/2026-10-27-03-arm-agv-mechanical-continuity.md).

## AFR2-20261027-05 — 27/10/2026

Aplicacion a todo el ancho util; simulaciones en una tarjeta con matematica
adosada/plegable y controles inferiores. Numeros compactos, motores y modos en
pills. Control agrupa manual/secuencias en Movimiento y pruebas/calibracion en
una seccion. GOTO y funciones conservados. Fuente sin tests/build/validaciones.
[Ficha](changes/2026-10-27-05-wide-workspaces-control-cards.md).

## AFR2-20261027-04 — 27/10/2026

Simulaciones con pills Auto/Brazo/Conjunto y Control aparte; articulaciones
arrastrables, gripper independiente, sliders vinculados y matrices laterales
en vivo. Modelos3D con selector Auto/Brazo y herramientas conservadas; archivo
final y referencias del brazo pendientes. Sin assets/dependencias nuevos ni
compilacion, pruebas o validaciones. [Ficha](changes/2026-10-27-04-simulations-models-interface.md).

## AFR2-20261027-02 — 27/10/2026

Control original restituido, incluido su flujo tactil y componentes3D compartidos.
Solo se agregan Brazo robotico y Simulacion conjunta, con controles a la derecha,
seleccion de articulaciones/gripper, posiciones/spawn de cubos y recogida segun
alcance. Adaptadores nuevos aislados, estados individuales y llegada con tolerancia.
Fuente sin tests/build/validaciones/navegador. Atmega/ESP/CAD sin cambios de esta ficha.
[Ficha](changes/2026-10-27-02-preserve-control-arm-combined.md).

## AFR2-20261027-01 — 27/10/2026

Paneles Robot / Brazo robotico con escena3D compartida, auto seleccionable y
movil, ruedas adelante/atras, seis ejes/pinza, referencias reversibles, cubos
por semilla, radar ideal y mision local. Alcance vertical calculado y propuesta
SVG de montaje. CAD nativo/informe cinematico/pruebas fisicas pendientes.
Solo fuente y documentacion, sin tests/build/validaciones/navegador/promocion ESP.
[Ficha](changes/2026-10-27-01-collector-3d-panels.md).

## AFR2-20261026-06 — 26/10/2026

Brazo v02: piñón/cremalleras de pinza vinculados, agarre50 mm y entrada60,011 mm,
J6 directo, alojamientos y abrazadera corregidos, separaciones ampliadas.
Revisión CAD solicitada:2902 poses muestreadas sin penetraciones reportables,
18 accionamientos nativos de ejes y9 de pinza. No prueba espacio continuo ni carga física.
Firmware/ESP/Web/PCB sin cambios. [Ficha y evidencia](changes/2026-10-26-06-arm-gripper-motion.md).

## AFR2-20261026-05 — 26/10/2026

Prototipo del brazo PVC32 guardado en Fusion y F3D local: 88 parámetros,
componentes nativos, seis articulaciones, pinza, motores/housings rojos,
rodamientos y Nano/PCA9685/XL4016 en la base. Pieza50 mm/100 g como requisito.
Sin cálculo mecánico, interferencias ni pruebas; firmware/ESP/Web/PCB sin cambios.
[Ficha y límites](changes/2026-10-26-05-arm-pvc32-prototype.md).

## AFR2-20261026-03 - 26/10/2026

Textos de toda la interfaz Web sin acentos agudos, con enie conservada;
correccion de cadenas mal codificadas y etiquetas importadas normalizadas
solo para presentacion. Fuente sin tests/build/validaciones ni promocion ESP.
ATmega/ESP/CAD y recursos de framebuffer sin cambios.
[Ficha](changes/2026-10-26-03-web-unaccented-text.md).

## AFR2-20261026-02 — 26/10/2026

Radar Web recompuesto: suelo continuo y retícula radial, anillos/sector rubí,
encuadre adaptado y vistas Perspectiva/Superior. Inicio1m sin alterar escala física.
Distancia destacada, diagnósticos desplegables y parámetros debajo del mapa.
VIS-038; fuente sin tests/build/validadores/navegador/promoción ESP.
[Ficha](changes/2026-10-26-02-radar-composition.md). Firmware/OLED/CAD/Modelo3D sin cambios de esta revisión.

## AFR2-20261026-01 — 26/10/2026

Modelo3D con botón Mostrar/Ocultar referencias a la derecha del selector;
ocultar cierra la ficha y retira puntos/líneas. Ocho referencias, incluida una
única rueda FL representativa. Fuente Web sin tests/build/validaciones ni
promoción ESP; ATmega/ESP/CAD sin cambios.
[Ficha](changes/2026-10-26-01-model-reference-visibility.md).

## AFR2-20261025-16 — 25/10/2026

Radar Web con plataforma/anillos 3D, brillo rubí y sector de escaneo en vista
previa. Selector doble0–180° y barras/campos numéricos vinculados con límites,
Enter/blur y Escape; inversión explicada y etiqueta Configuracion sin acento.
Fuente Web/VIS-037; sin tests/builds/validadores/navegador/promoción ESP.
ATmega/ESP/OLED/CAD sin cambio en esta revisión. [Ficha](changes/2026-10-25-16-radar-volume-scan-controls.md).

## AFR2-20261025-15 — 25/10/2026

Modelo3D con once selectores de componentes, ficha local fija, explicación y
acceso a la tarea/rueda correspondiente. X/Escape/clic fuera cierran; orbitar
conserva la ficha. Identidad0.1.9/VIS-036, solo fuente Web y documentación,
sin tests/builds/validaciones ni promoción ESP. ATmega/ESP/CAD sin cambios.
[Ficha](changes/2026-10-25-15-model-component-hotspots.md).

### AFR2-20261025-14 — Compilación posterior autorizada

ATmega firmware/communication/library, ESP y Web full compilados. Corregida creación de build en Windows y siete advertencias de indentación. Sin errores/advertencias finales; recursos OLED regenerados. Sin tests, navegador ni flash; sin promoción al paquete ESP. [Evidencia](documentation/builds/2026-10-25-14/README.md).

## AFR2-20261025-14 — 25/10/2026

Sensores separados, HC-SR04/servo/radar autónomo en ATmega, EX01v8 y41comandos públicos con ESP/Web, vistas OLED hermanas y copia Web64identidades, radar Three.js y animación común salvo Modelo3D. Pill STA/AP y modal Red; Configuración renombrada. Fuente sin builds/tests/validadores/browser/flasheo; recursos OLED copiados en modo copy-only. [Ficha](changes/2026-10-25-14-sensors-autonomous-radar.md).

# Registro AutoFinalRobo2

Independiente del AMI AutoMicro.

| Fecha | ID | Cambio | Atmega | ESP | Web | Detalle |
| --- | --- | --- | --- | --- | --- | --- |
|2026-10-25|AFR2-20261025-13|Modelo final con PCB/soporte manuales y GLB de 871.756 B|sin cambio|firmware sin cambio; activos promovidos|compilada/empaquetada; modelo final|[CAD guardado, render/Draco y hashes](changes/2026-10-25-13-final-cad-web-model.md)|
|2026-10-25|AFR2-20261025-12|PCB20×35 B.Cu, OLED en socket y soporte M3×15 en Fusion|sin cambio|sin cambio|sin cambio|[Diseño CAD, sin ERC/DRC ni montaje físico](changes/2026-10-25-12-oled-encoder-pcb-support.md)|
|2026-10-25|AFR2-20261025-12|Proxy LAN de desarrollo HTTP/WebSocket|sin cambios, build01|sin cambios, activos11|herramienta local iniciada; acceso remoto pendiente|[Ficha](changes/2026-10-25-12-lan-development-proxy.md)|
|2026-10-25|AFR2-20261025-11|Giro local con dos dedos, rumbo/Q-E y ruedas|sin cambio, build01/gates0|firmware01, activos11|compilada/promovida, tacto pendiente|[Ficha](changes/2026-10-25-11-two-finger-local-rotation.md)|
|2026-10-25|AFR2-20261025-10|Modelo manual mayor por cámara60%; chevrones intactos|sin cambio, build01/gates0|firmware01, activos10|compilada/promovida, visual pendiente|[Ficha](changes/2026-10-25-10-larger-manual-model.md)|
|2026-10-25|AFR2-20261025-09|Restaurar diseño07, suavizar solo color|sin cambio, build01/gates0|firmware01, activos09|compilada/promovida, visual pendiente|[Ficha](changes/2026-10-25-09-restore-previous-trail.md)|
|2026-10-25|AFR2-20261025-08|Color suave y difusión por brazo; punta aguda|sin cambio, build01/gates0|firmware01, activos08|compilada/promovida, visual pendiente|[Ficha](changes/2026-10-25-08-soft-pointed-ribbons.md)|
|2026-10-25|AFR2-20261025-07|Cintas anchas/salientes, punta intensa y cola transparente|sin cambio, build01/gates0|firmware01, activos07|compilada/promovida, visual pendiente|[Ficha](changes/2026-10-25-07-wide-ribbon-tips.md)|
|2026-10-25|AFR2-20261025-06|Chevrones grandes bajo el chasis, degradé/fade suave|sin cambio, build01/gates0|firmware01, activos06|compilada/promovida, visual pendiente|[Ficha](changes/2026-10-25-06-underbody-gradient-trail.md)|
|2026-10-25|AFR2-20261025-05|Estela direccional de degradé/fade en táctil|sin cambio, build01/gates0|firmware01, activos05|compilada/promovida, visual pendiente|[Ficha](changes/2026-10-25-05-directional-gradient-trail.md)|
|2026-10-25|AFR2-20261025-04|Falso fallo Canvas corregido; simulación sin red/carga3D|sin cambio, build01/gates0|firmware01, activos04|compilada/promovida, visual pendiente|[Ficha](changes/2026-10-25-04-local-input-independent.md)|
|2026-10-25|AFR2-20261025-03|Captura táctil/foco/teclado y flecha activa animada|sin cambio, build01/gates0|firmware01, activos03|compilada/promovida, interacción pendiente|[Ficha](changes/2026-10-25-03-touch-input-feedback.md)|
|2026-10-25|AFR2-20261025-02|Táctil offline, superior, flechas y ruedas simuladas|sin cambio, build01/gates0|firmware01, activos02|compilada/promovida, visual pendiente|[Ficha](changes/2026-10-25-02-touch-simulation-wheels.md)|
|2026-10-25|AFR2-20261025-01|Control directo7, sin permiso ni habilitación OLED|0.9.0 compilado, gates0|0.9.0 compilado, edad de presencia|0.2.0 compilada/promovida,30comandos|[Ficha](changes/2026-10-25-01-direct-web-control.md)|
| 2026-10-24 | AFR2-20261024-15 | Maestro reconciliado, Calibración auto-lease y diagnóstico FS/paquete | sin nueva fuente, build12/gates0 | compilado15, salud/identidad y activos nuevos | compilada15/promovida; aceptación pendiente | [Ficha](changes/2026-10-24-15-master-alignment.md) |
| 2026-10-24 | AFR2-20261024-14 | Desarrollador simplificado y árbol59 con controles laterales OLED | sin fuente nueva; build12/gates0 | firmware12, activos Web14 promovidos | compilada/empaquetada; sin navegador/placas | [Ficha](changes/2026-10-24-14-developer-oled-tree.md) |
| 2026-10-24 | AFR2-20261024-13 | Control táctil fullscreen con modelo final,8direcciones y velocidad compartida | sin fuente nueva; build12/gates0 | firmware12 conservado, activos Web13 promovidos | fuente compilada/paquete; gestos en teléfono pendientes | [Ficha](changes/2026-10-24-13-fullscreen-touch-drive.md) |
| 2026-10-24 | AFR2-20261024-12 | Hardware agrupado, pistas reversibles/terminal/distancia y GOTO compacto con coordenadas laterales | EX01v6 compilado con gates0; incluye IRQ11, sin placa | EX01v6 candidato compilado, activos Web12 promovidos | compilada/empaquetada; sin navegador | [Ficha](changes/2026-10-24-12-control-hardware-distance.md) |
| 2026-10-24 | AFR2-20261024-11 | Encoder D2/D3 por IRQ, cola/main y preferencia de captura Hall A8–A15 | fuente adaptada sin compilar; Hall/memoria0 | sin cambios, firmware08/activos10 | sin cambios, entrega10 | [Ficha](changes/2026-10-24-11-encoder-hall-interrupts.md) |
| 2026-10-24 | AFR2-20261024-10 | Simulador GOTO por puntos/lista, activo animado; mapa completo y controles laterales | sin cambios, evidencia08 | firmware08; activos Web10 promovidos | compilada/empaquetada; gestos/visual pendientes | [Ficha](changes/2026-10-24-10-goto-map-queue.md) |
| 2026-10-24 | AFR2-20261024-09 | Catálogo59 y copia biblioteca AVR/Web; skill exige ambas partes por pantalla | fuente sin cambios, build08 | firmware08, paquete Web09 promovido | copia actual, fuente/XBM/catálogo enlazados, compilada; paridad física pendiente | [Ficha](changes/2026-10-24-09-linked-oled-library.md) |
| 2026-10-24 | AFR2-20261024-08 | IP/AP/avisos OLED, iconos F4, scroll y Sistema/Reinicio; EX01v5 | producto/comm/library compilados; sin placa, gates0 | firmware candidato compilado; datos/avisos/ACK/reset local | guía compilada/empaquetada/promovida, visor framebuffer conservado | [Ficha](changes/2026-10-24-08-oled-network-restart.md) |
| 2026-10-24 | AFR2-20261024-07 | Control uniforme, suelo preciso, secuencias OLED y EX01v4, info/boot/reinicio, conexión automática y desarrollador | firmware/comm/library compilados; memoria/Hall gated | firmware candidato compilado; activos Web promovidos | compilada/empaquetada; navegador denegado | [Ficha](changes/2026-10-24-07-control-system-sequence.md) |
| 2026-10-24 | AFR2-20261024-06 | Suelo en base del GLB, modelo final en Control y metros proporcionales | sin cambios | Web actualizada, firmware sin cambios | compilada/promovida; sin render visual | [Ficha](changes/2026-10-24-06-grounded-model-control.md) |
| 2026-10-24 | AFR2-20261024-05 | Reintento GLB limpia cargas rechazadas y publicDir estable; diagnóstico HTTP correcto | sin cambios | activos Web actualizados | compilada/promovida; sin render visual | [Ficha](changes/2026-10-24-05-model-loading-retry.md) |
| 2026-10-24 | AFR2-20261024-04 | Modelo CAD final dentro del ESP y LittleFS candidato3MiB; paquete1.450.201bytes (-49,96%) | sin cambios | compilado con layout3MiB y activos promovidos | ambos perfiles compilados, modelo final incluido | [Ficha](changes/2026-10-24-04-final-model-littlefs.md) |
| 2026-10-24 | AFR2-20261024-03 | WebP, CSS acotado, dependencias y perfil ESP de649.410bytes (-77,59%) | sin cambios, evidencia02 | activos Web promovidos, sin recompilar firmware | ambos perfiles compilados, sin navegador | [Ficha](changes/2026-10-24-03-web-payload.md) |
| 2026-10-24 | AFR2-20261024-02 | Alineación EX01v3, GOTO/calibración/Wi-Fi OLED, comandos/estados y builds | firmware/comm/library compilados; secuencias bloqueadas | compilada, Web actualizada en data/dist | compilada/empaquetada/promovida; sin navegador | [Ficha](changes/2026-10-24-02-project-alignment.md) |
| 2026-10-24 | AFR2-20261024-01 | Integra firmware EX01, OLED ampliada y secuencias sin colisión con producto19 | fuente adaptada, memoria gated | puente EX01 en fuente, Wi-Fi20 conservado | consumidor adaptado sobre21 | [Ficha](changes/2026-10-24-01-firmware-web-integration.md) |
| 2026-10-24 | AFR2-20261024-01 | Fusion v16 con ruedas y Blender; GLB 827.232 bytes | fuente parcial; sin cambio | esqueleto; sin cambio | esqueleto; sin cambio | [Ficha](changes/2026-10-24-01-wheels-blender.md) |
| 2026-10-24 | AFR2-20261023-19 | Producto ATmega,44 menús/vistas, sensores/control/EEPROM y EX01 | fuente integrada sin compilar/probar; Hall sin conectar | complemento EX01 documentado, no implementado | complemento EX01 documentado, no implementado | [Ficha](changes/2026-10-23-19-atmega-product.md) |
| 2026-10-23 | AFR2-20261023-21 | Controles OLED, avisos cerrables, giro gráfico y secuencias Mecanum | perfil ROBO2 3 diseñado; fuente sin cambio | perfil ROBO2 3 diseñado; fuente sin cambio | fuente adaptada y compilada (tsc + Vite + LittleFS OK) | [Ficha](changes/2026-10-23-21-web-oled-notices-sequences.md) |
| 2026-10-23 | AFR2-20261023-20 | Wi-Fi ESP/Web: scan terminal, estado real, persistencia elegible | sin cambio | Wi-Fi ampliado y compilado | flujo Wi-Fi compilado/empaquetado local | [Ficha](changes/2026-10-23-20-wifi-esp-web.md) |
| 2026-10-23 | AFR2-20261023-18 | Velo Radal fiel, toast legibles y mapa por vistas | sin cambio; pose pendiente | sin cambio; pose pendiente | fuente adaptada sin build/render | [Ficha](changes/2026-10-23-18-control-map-and-ui-polish.md) |
| 2026-10-23 | AFR2-20261023-16 | Intro ST, logo transparente y toast inferiores derechos | sin cambio | sin cambio | fuente adaptada; sin nuevo build/render | [Ficha](changes/2026-10-23-16-startup-and-unframed-logo.md) |
| 2026-10-23 | AFR2-20261023-17 | Retoma guía ROBO2, revisión y build ESP | sin cambios en esta ficha | ROBO2 compilado; sin warnings/errors | sin cambios | [Ficha](changes/2026-10-23-17-robo2-esp-review-build.md) |
| 2026-10-23 | AFR2-20261023-15 | ROBO2/perfil2 y documentación bidireccional; OLED/encoder/TWI integrados | fuente integrada sin compilar/probar | receptor/validación y guía escritos sin compilar | sin cambio en esta ficha | [Ficha](changes/2026-10-23-15-robo2-atmega-ui.md) |
| 2026-10-23 | AFR2-20261023-15 | Build ESP/Web, limpieza tipada C++ y corrección EPERM del paquete | sin cambio en esta ficha | compilada con perfil candidato; 0 warnings final | tsc/Vite/paquete OK, 6 assets verificados | [Ficha](changes/2026-10-23-15-esp-web-build-fixes.md) |
| 2026-10-23 | AFR2-20261023-14 | Sidebar ST flotante, modales comunes y OLED global | sin cambio; reporte pendiente | sin cambio; rechazo OLED/UI explícito | fuente adaptada; sin compilar/renderizar/probar | [Ficha](changes/2026-10-23-14-floating-web-shell.md) |
| 2026-10-23 | AFR2-20261023-13 | Python/esptool/pyserial y CLI verificados; resolución del paquete PlatformIO | sin cambio | hardware aún sin consultar | sin cambio | [Ficha](changes/2026-10-23-13-python-esptool-check.md) |
| 2026-10-23 | AFR2-20261023-12 | Script futuro de conexión/flash ESP e informes persistentes | sin cambio | sin cambio; diagnóstico pendiente | sin cambio | [Ficha](changes/2026-10-23-12-esp-diagnostic-script.md) |
| 2026-10-23 | AFR2-20261023-11 | Servidor HTTP/WS ESP, LittleFS/Wi-Fi y perfil UART AFR2 1 | endpoint/USART1 y composición de comunicaciones escritos, sin compilar | servidor/broker escritos, sin compilar | ajustes fuente de capacidades/STOP; nuevo paquete pendiente | [Ficha](changes/2026-10-23-11-esp-server-and-mega-link.md) |
| 2026-10-23 | AFR2-20261023-10 | Web sobria, secciones por tarea y revelado progresivo | sin cambio | sin cambio | fuente editada; sin nuevo build | [Ficha](changes/2026-10-23-10-calm-web-workspaces.md) |
| 2026-10-23 | AFR2-20261023-09 | Implementación Web TS/React 19, protocolo afr2-ws-1, réplica OLED 128x64, encoder único y Sonner | fuente parcial; sintaxis C validada | esqueleto; contrato y dist listos | implementada y compilada (tsc + vite singlefile + LittleFS) | [Ficha](changes/2026-10-23-09-web-app-implementation.md) |
| 2026-10-23 | AFR2-20261023-08 | Cierre documental del plan visual 0.1.3: cotejo íntegro del informe, Logo.png y acento carmesí/rubí | sin cambio | sin cambio | lista para implementar | [Ficha](changes/2026-10-23-08-visual-plan-closure.md) |
| 2026-10-23 | AFR2-20261023-07 | Manual visual 0.1.1: validación documental y preparación, sin implementar | sin cambio | sin cambio | sin cambio | [Ficha](changes/2026-10-23-07-visual-manual-validation.md) |
| 2026-10-23 | AFR2-20261023-07 | Ensamble Fusion parcial guardado/exportado; ruedas 80 mm pendientes | fuente parcial; sin cambio | esqueleto; sin cambio | esqueleto; sin cambio | [Ficha](changes/2026-10-23-07-fusion-assembly.md) |
| 2026-10-23 | AFR2-20261023-06 | Plan Web, skills y manual visual 0.1.0 draft | sin cambio | sin cambio | implementación pendiente | [Ficha](changes/2026-10-23-06-web-visual-system.md) |
| 2026-10-23 | AFR2-20261023-06 | Primer lote de candidatos CAD para aprobación; chasis manual | fuente parcial; sin cambio | esqueleto; sin cambio | esqueleto; sin cambio | [Ficha](changes/2026-10-23-06-cad-candidates.md) |
| 2026-10-23 | AFR2-20261023-05 | Plan Fusion, reserva UI y origin privado para continuar en otra PC | fuente parcial; código sin cambio | esqueleto; código sin cambio | esqueleto; código sin cambio | [Ficha](changes/2026-10-23-05-fusion-plan-and-origin.md) |
| 2026-10-23 | AFR2-20261023-04 | Módulo CAD/KiCad y Git global con historial conservado | fuente parcial; código sin cambio | esqueleto; código sin cambio | esqueleto; código sin cambio | [Ficha](changes/2026-10-23-04-cad-and-monorepo.md) |
| 2026-10-23 | AFR2-20261023-03 | Shield UART V1.3: evidencia, niveles, cruce USB, I2C Mega y prompt | pendiente; código sin cambio | pendiente; código sin cambio | pendiente; código sin cambio | [Ficha](changes/2026-10-23-03-wifi-shield-evidence.md) |
| 2026-10-23 | AFR2-20261023-02 | Cabeceras I2C, conectores PCB y árbol de menús objetivo | pendiente; fuente sin cambio | pendiente; sin cambio | pendiente; sin cambio | [Ficha](changes/2026-10-23-02-navigation-and-interface-board.md) |
| 2026-10-23 | AFR2-20261023-01 | Base, hardware/UI y contrato candidato | OLED/UI fuente parcial; resto pendiente | port pendiente | port pendiente | [Ficha](changes/2026-10-23-01-bootstrap.md) |
## Integración del trabajo local y CAD de origin — 2026-10-24

Se conservan las fichas de firmware/Web locales y las fichas CAD recibidas de origin. Hay IDs históricos repetidos entre trabajos independientes (AFR2-20261023-06, AFR2-20261023-07 y AFR2-20261024-01), además de la duplicación local previa de AFR2-20261023-15: para distinguirlos usar el enlace completo de cada ficha. Esta sincronización no renumera los registros ni sustituye sus evidencias. Los estados de cada fila corresponden al alcance de esa ficha; los hitos CAD no revierten el avance local de firmware/ESP/Web. No se ejecutaron compilaciones, tests, validadores ni pruebas físicas para integrar los cambios.
