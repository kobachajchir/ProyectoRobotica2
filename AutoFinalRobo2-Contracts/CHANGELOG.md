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
