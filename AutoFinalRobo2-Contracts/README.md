> **Loader AFR2-20261027-19 / 0.3.0:** [monitor serie y eventos](../AutoFinalRobo2-Loader/MANUAL.html#monitor), COM abrir/cerrar, baudios, texto RX, detección de reinicio/modo ESP y logs de fallos; liberación antes de cargar y consulta MCP de captura. [Ficha](changes/2026-10-27-19-loader-serial-monitor.md). Solo fuente/documentación; sin compilar/ejecutar/probar ni abrir COM/flashear.

> **Loader AFR2-20261027-16 / 0.2.0:** [MCP para Codex](../AutoFinalRobo2-Loader/MCP.md), solicitudes revisables en GUI, historial, filtro y evidencias UART/ROM separadas. [Ficha](changes/2026-10-27-16-loader-mcp-ui.md). Solo fuente/documentación: sin compilar, ejecutar, registrar ni comprobar conexión; sin COM ni flasheos.

> **Loader AFR2-20261027-14:** cargador trasladado a [AutoFinalRobo2-Loader](../AutoFinalRobo2-Loader/README.md), sexto módulo en la raíz del único Git. [Manual](../AutoFinalRobo2-Loader/MANUAL.html), [registro](changes/2026-10-27-14-loader-root-module.md). Contracts conserva documentación compartida; fuentes/lanzadores fuera de tools. Sin compilar/ejecutar/probar esta entrega.

> **Herramienta Windows AFR2-20261027-13:** [AFR2 Loader](../AutoFinalRobo2-Loader/README.md), [manual detallado](../AutoFinalRobo2-Loader/MANUAL.html). Interfaz con perfiles, rutas, builds AVR/ESP/Web/LittleFS, escucha de arranque ESP, asistentes de cableado y escritura/verificación. Entrega en fuente y lanzador; no compilada, ejecutada ni probada en esta ficha. [Registro](changes/2026-10-27-13-windows-loader.md).

> **Build/carga AFR2-20261027-10:** [compilaciones completas y LittleFS](documentation/builds/2026-10-27-10/README.md), [guía USB-B/COM16](hardware/CARGA_USB_COM16.md). Hall habilitado en fuente por pedido humano; preferencia Hall conectado/EEPROM conservada. Web esp compilada/promovida,8activos/1571104bytes gzip; littlefs.bin3121152bytes generado. Sin apertura serie ni carga; jumpers y flash física pendientes. Estados inferiores históricos.

> **Pinout AFR2-20261027-07:** [pinout completo del auto](hardware/PINOUT.md) y [PDF A4 para compartir](hardware/output/pdf/PINOUT_DEL_AUTO.pdf), firmado por Koba Chajchir. Hall A8-A11=A y A12-A15=B, ambos DEL DER / DEL IZQ / TRAS DER / TRAS IZQ; máscara0x0F en fuente ATmega. Panel OLED/encoder a 5 V según montaje comunicado. Hall deshabilitado, sin compilar/flashear/probar esta revisión; CAD +3V3 no actualizado. [Ficha](changes/2026-10-27-07-complete-pinout-grouped-hall.md).

> **Radar Web AFR2-20261026-02 / VIS-038:** escena continua, encuadre1m, vistas y jerarquía de lecturas; parámetros debajo del mapa. [Ficha](changes/2026-10-26-02-radar-composition.md). Fuente integrada; **sin compilación ni verificación visual ni promoción ESP**. Firmware/OLED/CAD y Modelo3D sin intervención. Sustituye la presentación16.

> **Interfaz AFR2-20261025-16:** radar Web con líneas volumétricas, sector editable, selector doble y parámetros barra/número; Configuracion sin acento. [Ficha](changes/2026-10-25-16-radar-volume-scan-controls.md). **Solo fuente, sin build ni navegador ni promoción ESP**. La compilación14 no cubre esta interfaz; ATmega/ESP/OLED sin cambios en esta revisión. Estados inferiores históricos.

> **Fuente Web AFR2-20261025-15:** [componentes interactivos del modelo](contracts/WEB_MODEL_INSPECTION.md), once selectores con ficha fija y accesos a controlar. Sin tests/build/validaciones/navegador ni promoción ESP; ATmega/ESP/CAD sin cambios. [Ficha](changes/2026-10-25-15-model-component-hotspots.md). El build14 inferior corresponde a la fuente previa.

> **Compilación AFR2-20261025-14 autorizada y completada:** firmware ATmega (producto/comunicación/biblioteca), ESP y Web full compilan sin errores ni advertencias finales. Firmware0.10.0/EX01v8, Web0.3.0. [Evidencia](documentation/builds/2026-10-25-14/README.md). Sin tests, navegador ni flasheos; paquete Web local full generado, **ESP/data/dist conserva el paquete anterior**. La SRAM estática no certifica pila/ISR ni hardware. Estados previos inferiores históricos.

> **Vigente AFR2-20261025-14:** [radar autónomo, servo/IR, pantallas hermanas y red](contracts/SENSORS_RADAR_8.md). Firmware nominal0.10.0/EX01v8 y Web0.3.0: fuente integrada y recursos OLED copiados, **sin compilar, probar, renderizar ni flashear**. Paquete ESP anterior; estados inferiores históricos. Modelo3D conserva pose; otras escenas siguen PWM aplicado.

> **Vigente AFR2-20261025-11:** giro local con dos dedos/Q-E, orientación persistente y rumbo visible; levantar uno cancela hasta soltar ambos. No envía yaw físico. Cámara10/chevrones09 conservados. [Contrato](contracts/WEB_TOUCH_SIMULATION.md), [build11](documentation/builds/2026-10-25-11/README.md). Web compilada/promovida; firmware0.9.0/EX01v7/build01/gates0 sin cambios. Gesto/teléfono No verificados; estados inferiores históricos.

> **Vigente AFR2-20261025-01:** [control Web directo](contracts/DIRECT_CONTROL_7.md), sin permiso previo ni habilitación desde OLED. Firmware0.9.0/EX01v7 y Web0.2.0,30comandos/59ScreenCodes, compilados juntos. STOP/caducidad/watchdog y requisitos físicos conservados; sin flash/tests/placas. Estados inferiores históricos.

> **Vigente AFR2-20261024-15:** [Matriz del maestro](docs/ALINEACION_MAESTRO.md):33comandos y59identidades OLED con fuentes complementarias. Calibración Web adquiere control al escribir; ESP/Web informan FS/paquete. Web/ESP compilados15; AVR sin nueva fuente, build12/EX01v6/firmware0.8.0/gates0. Sin brecha software definida identificada tras lectura; aceptación física, memoria dinámica, flash/SPI y planos CAD pendientes. Solo documentación en CAD, sin diseño nuevo. Estados inferiores históricos.

> **Vigente AFR2-20261024-12:** mapa/comando actual juntos y coordenadas laterales; Hardware/Reinicio anidados, editor de pistas reversible y distancia por rueda EX01v6. Firmware0.8.0 y Web compilados; activos promovidos localmente. Build AVR incluye fuente IRQ11 con gates0; sin navegador, Hall habilitado, hardware, flasheo ni CAD. [Ficha12](changes/2026-10-24-12-control-hardware-distance.md). Los estados inferiores son históricos.

> **Fuente vigente AFR2-20261024-11:** encoder CLK/DT en D2/D3 por INT4/INT5, SW D42; cola y acciones en main. Hall PCINT2 A8–A15 con preferencia de captura por software, macro0. **Esta adaptación no está compilada ni probada**; build08 corresponde a la fuente anterior. ESP/Web conservan su estado10. [Ficha11](changes/2026-10-24-11-encoder-hall-interrupts.md). Los estados previos inferiores son históricos.

> **Vigente AFR2-20261024-10:** GOTO local por puntos del mapa y comandos ordenados, acento/seguimiento activo animados y controles laterales con mapa ancho completo. [Ficha10](changes/2026-10-24-10-goto-map-queue.md), [mapa](contracts/WEB_CONTROL_MAP.md) y [estado](docs/ESTADO_IMPLEMENTACION.md). Web compilada/promovida; firmware/ScreenCodes/gates sin cambios. Sin navegador, tests ni placas. Estados anteriores históricos.

> **Vigente AFR2-20261024-09:** catálogo Web59, copia de biblioteca AVR actual y skill que exige entregar ambas partes vinculadas. Visor conectado con framebuffer obligatorio; preview local identificado. [Ficha09](changes/2026-10-24-09-linked-oled-library.md), [estado](docs/ESTADO_IMPLEMENTACION.md) y [build09](documentation/builds/2026-10-24-09/README.md). Firmware conserva build08. Sin comparación diferencial, pruebas físicas ni flasheo. Estados anteriores históricos.

> **Vigente AFR2-20261024-08:** [Red OLED/reinicio EX01v5](contracts/EX01_NETWORK_UI_5.md), [estado Web/ESP/ATmega](docs/ESTADO_IMPLEMENTACION.md), [ficha](changes/2026-10-24-08-oled-network-restart.md) y [build08](documentation/builds/2026-10-24-08/README.md). IP/AP, avisos, iconos, scroll y menú Reinicio compilados. Sin prueba física, flasheos ni aprobación nueva de memoria/Hall. Estados anteriores históricos.

> **Vigente AFR2-20261024-07:** [estado organizado en Web, ESP y ATmega](docs/ESTADO_IMPLEMENTACION.md), [EX01v4](contracts/EX01_SYSTEM_SEQUENCE_4.md), [ficha](changes/2026-10-24-07-control-system-sequence.md) y [builds](documentation/builds/2026-10-24-07/README.md). Los tres módulos compilan; activos Web promovidos a ESP. Control/OLED/sistema/reinicios/boot/desarrollador implementados en fuente. Secuencias físicas siguen bloqueadas por memoria/Hall/calibración. Navegador denegado: visual no comprobado; sin pruebas físicas ni flasheo. Estados inferiores históricos.

> Actualizacion **AFR2-20261024-06**: suelo en la base del modelo y GLB final compartido por inspeccion y Control (Superior ortografica, Perspectiva y Seguir auto). Escala1unidad=1m; cuadricula0,5m, sin agrandar la malla. Web compilada/promovida: **1.450.617 bytes gzip**,7 activos. Sin render visual, pruebas fisicas ni flasheo. [Detalle](changes/2026-10-24-06-grounded-model-control.md).

> Correccion **AFR2-20261024-05**: Reintentar carga 3D elimina el error cacheado por useLoader; publicDir absoluto y copia selectiva de build conservada. Web compilada y promovida: **1.450.338 bytes gzip**. Diagnostico HTTP local: GLB/Draco/WebP200 con contenido correcto; sin renderizado visual ni flasheo. Modelo final y layout candidato3MiB de04 conservados. [Detalle](changes/2026-10-24-05-model-loading-retry.md).

> Vigente **AFR2-20261024-04**: **modelo final incluido en ESP**, paquete **1.450.201 bytes gzip** (49,96% menos que los 2,90 MB iniciales). WebP, JS/CSS minificados y dependencias/componentes optimizados. Layout candidato: flash 4 MiB / LittleFS nominal 3 MiB, solicitado por el usuario; flash fisica sin identificar. Web full/esp y firmware ESP compilados; activos promovidos a data/dist. Sin navegador, imagen FS ni flasheo. Atmega y comandos conservan el estado de02. [Evidencia](changes/2026-10-24-04-final-model-littlefs.md). Los estados previos inferiores son historicos.

> Estado vigente **AFR2-20261024-02**, 24/10/2026: integración ROBO2/perfil2 + EX01v3 compilada en Atmega/ESP/Web; GOTO completo, configuración por catálogo compartido y jobs Wi-Fi OLED conectados. Paquete Web actualizado en ESP/data/dist. Sin pruebas de navegador/hardware ni flasheo; secuencias físicas mantienen bloqueo de memoria. Ver [matriz vigente](contracts/EX01_ALIGNMENT_3.md) y [evidencia](changes/2026-10-24-02-project-alignment.md). Los estados previos que siguen son históricos.

> **AFR2-20261027-05:** [Ancho util y tarjetas integradas](changes/2026-10-27-05-wide-workspaces-control-cards.md): matriz adosada/plegable, controles inferiores y numeros compactos; Movimiento y Pruebas/calibracion agrupados en Control. Funciones y GOTO conservados. Sin tests/build/validaciones/navegador ni cambios de atmega/ESP de esta ficha.

> **AFR2-20261027-06:** calibracion con ayuda derecha para los 29 parametros; matriz mayor y centrada, mismo formulario en las tres simulaciones; pad, boton mantenido y numero/barra reutilizados con Control. [Ficha](changes/2026-10-27-06-calibration-live-math-shared-controls.md). Solo fuente, sin tests/build/validaciones/navegador ni promocion ESP.

# AutoFinalRobo2-Contracts

> **Brazo AFR2-20261027-18:** [informe integrado de esfuerzos e iteración](../AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/analisis_cargas_200g/INFORME_ITERACION_DISENO.md), con historial de correcciones, datos estáticos existentes, frenada/esfuerzos combinados y plan v07. [Ficha](changes/2026-10-27-18-arm-design-iteration-report.md). Solo documentación; sin nuevo CAD, cálculo dinámico, FEA, tests, compilaciones ni flasheos.

> **AFR2-20261027-11:** [puntos cartesianos, modelos y trayectorias Web](contracts/WEB_ARM_POINTS.md).
> Biblioteca persistente, Worker numerico, visualizacion y ejecucion del mismo plan;
> perfil v03 y adaptador para GLB articulados. [Ficha](changes/2026-10-27-11-arm-points-model-agnostic.md).
> Solo fuente, sin comprobaciones ni promocion ejecutadas por esta entrega.

**CAD/Web AFR2-20261025-13:** [modelo final de 871.756 bytes](changes/2026-10-25-13-final-cad-web-model.md), con PCB y soporte manuales; Fusion/Blender guardados y mismo GLB en Web y paquete ESP compilado/promovido. Firmware sin cambios ni flasheo; comprobación CAD/Draco y render Blender realizados.

Hito AFR2-20261024-01: [Fusion v16 con ruedas y Blender](changes/2026-10-24-01-wheels-blender.md), GLB 827.232 bytes, Draco y materiales PBR. Atmega/ESP/Web sin cambios. Hitos anteriores históricos.

Hito CAD AFR2-20261023-07: [ensamble parcial Fusion guardado/exportado](../AutoFinalRobo2-CAD/docs/ARMADO_FUSION_PASO_A_PASO.md), versión 15, con modelos aportados por el usuario y Model Y esencial. Ruedas 80 mm y acoples pendientes; sin construir ruedas ni cambiar código/contratos eléctricos. [Registro](changes/2026-10-23-07-fusion-assembly.md).

Actualización ATmega **AFR2-20261023-19**,24/10/2026: fuente de producto integrada,
sin compilar/probar:44 menús/vistas, Sistema/teclado/avisos, motores/sensores,
Hall/PID/odometría/línea/GOTO condicionados, EEPROM y EX01 sobre ROBO2/perfil2.
[Matriz](docs/PLAN_ATMEGA.md) · [Implementación y límites](docs/ATMEGA_IMPLEMENTACION_19.md) ·
[Complemento ESP/Web](contracts/ROBO2_PRODUCT_EXTENSION.md).
Hall sin conectar; ninguna capacidad física se acredita por esta entrega.
ESP20 y Web21 tienen sus propias evidencias de build; EX01 no está integrado allí.
Perfil3/secuencias de21 sigue diseñado y separado de los IDs EX01.
El contenido anterior se conserva como historia y queda sustituido en estado ATmega.


Estado histórico **AFR2-20261023-15**: protocolo **ROBO2/perfil 2**, validación
compartida Mega/ESP y composición AVR USART1/Timer0/TWI/Manager/OLED/encoder en
fuente. Cuatro vistas locales conservadas; sin compilación ni pruebas de esta
entrega. Sustituye menciones previas a UNER/perfil 1 y backend TWI pendiente.
[Ficha](changes/2026-10-23-15-robo2-atmega-ui.md) ·
[Contrato](contracts/UART_PROFILE_2.md) ·
[Validación de ambos extremos](contracts/ROBO2_VALIDACION.md) ·
[Documento para ESP](../AutoFinalRobo2-ESP/docs/PROTOCOLO_ROBO2.md).
Sensores/STOP autoritativo/motores/lease/Hall/GOTO/UI remota siguen pendientes.

Central independiente. Empezar por [maestro](docs/INFORME_MAESTRO.md), [decisiones](docs/DECISIONES.md) y [plan](docs/ROADMAP.md).

- [Pinout/PCB](hardware/PINOUT.md), [recursos](hardware/RECURSOS_Y_TIEMPOS.md), [carga](hardware/FLASHING.md).
- [Script de diagnóstico ESP](hardware/ESP_DIAGNOSTICO.md): conexión primero, ID/tamaño de flash y cabecera después; guarda informes para retomar. Preparado, todavía sin ejecutar.
- [Protocolo](contracts/PROTOCOL.md), [capabilities](contracts/capabilities.json), [catálogo propuesto](contracts/commands.json).
- [Pantallas](documentation/SCREEN_SYSTEM.md), [navegación](documentation/NAVIGATION.md), [skill local](.agents/skills/afr2-screen-authoring/SKILL.md).
- [Hallazgos](documentation/review/FINDINGS.md), [registro](CHANGELOG.md), [plantilla](templates/change.md).
- [Procedencia](provenance/README.md), [manifest](provenance/files.json), [orígenes](provenance/origins.json).

Módulos hermanos: ../AutoFinalRobo2-Atmega, ../AutoFinalRobo2-ESP, ../AutoFinalRobo2-Web y [AutoFinalRobo2-CAD](../AutoFinalRobo2-CAD/README.md), dedicado a KiCad, la pieza nueva y el modelo completo del auto. Junto con [Loader](../AutoFinalRobo2-Loader/README.md), los seis pertenecen al único Git de D:/ProyectoRobotica2. No hay Qt ni dependencia de ejecución hacia AutoMicro. reference/automicro conserva evidencia histórica: sus reglas y estados no son instrucciones ni resultados del nuevo proyecto.

Estado vigente [AFR2-20261023-15](changes/2026-10-23-15-esp-web-build-fixes.md): ESP compilada con perfil candidato, sin errores ni warnings finales; Web compilada y empaquetada localmente (6 assets, 2.925.630 bytes gzip, integridad cotejada). ESP/data/dist conserva el paquete anterior. ATmega se trabaja en paralelo y no se tocó/compiló en esta ficha; nuevo protocolo pendiente de la documentación que entregará el usuario. Sin verificación física, identificación de flash/layout ni flasheo. [Plan ESP](docs/PLAN_ESP.md) · [Perfil provisional UART 1](contracts/UART_PROFILE_1.md) · [Implementación inicial](changes/2026-10-23-11-esp-server-and-mega-link.md).

## Continuación del proyecto

- [Menús y submenús objetivo](documentation/MENU_PLAN.md), implementados en fuente ATmega19, sin compilar/probar.
- [PCB OLED/encoder y soporte futuro](hardware/PCB_OLED_ENCODER.md): cabeceras SDA/SCL del shield, señales 1x5 y VCC/GND 1x2, ambos paso 2,54 mm.

La raíz de trabajo y de Git es D:/ProyectoRobotica2; Contracts sigue siendo la central documental. [Organización, exclusiones y conservación del historial](docs/ESTRUCTURA_Y_GIT.md). El [plan de Fusion](../AutoFinalRobo2-CAD/docs/PLAN_MODELO_3D_FUSION.md) define el futuro ensamble del auto base sin cables y con CAD/inferencias documentadas; la pieza nueva, soporte y PCB siguen pendientes.

## Shield Wi-Fi documentado

[Informe OSOYOO UART V1.3](hardware/WIFI_SHIELD.md), [carga ESP/Mega](hardware/FLASHING.md), [evidencia](hardware/wifi-shield/INFORME_CARPETA.md) y [prompt completo de continuidad](docs/PROMPT_CONTINUIDAD.md). Mapa UNO no sustituye pinout Mega; selector y UART directo se distinguen. Firmware propio Arduino/LittleFS sigue sin AT. [Registro AFR2-20261023-03](changes/2026-10-23-03-wifi-shield-evidence.md).

## Aplicación Web e identidad visual

[Plan Web ejecutado](docs/PLAN_WEB.md), [política de skills](docs/INTERFACE_SKILLS.md) y [manual visual 0.1.3](docs/design-system/index.md). Plan visual cerrado documentalmente (`694` comprobaciones, `107` tokens, `0` errores) e implementado en `AutoFinalRobo2-Web` con emblema `/brand/Logo.png`, paleta carmesí/rubí + emergencia bermellón, notificaciones `Sonner`, cliente `afr2-ws-1`, réplica OLED `128×64` bit a bit y empaquetador LittleFS (`AFR2-20261023-09`).
- [AFR2-20261023-06](changes/2026-10-23-06-web-visual-system.md) · [AFR2-20261023-07](changes/2026-10-23-07-visual-manual-validation.md) · [AFR2-20261023-08](changes/2026-10-23-08-visual-plan-closure.md) · [AFR2-20261023-09](changes/2026-10-23-09-web-app-implementation.md).

## Interfaz flotante actual — AFR2-20261023-14

[Manual 0.1.5](docs/design-system/index.md), [adaptación ST](docs/WEB_FLOATING_UI.md) y [consumidor OLED](contracts/WEB_OLED_CONSUMER.md). Web en fuente: sidebar persistente/colapsable, tarjetas ilustradas, modo autoritativo y OLED/Ajustes con modal compartido. Firmware sin cambio y productor OLED pendiente. Sin compilar, probar o renderizar esta revisión; dist/paquete ESP anteriores no se actualizaron.

## Inicio y marca — AFR2-20261023-16

[Manual0.1.6](docs/design-system/index.md) / [ficha](changes/2026-10-23-16-startup-and-unframed-logo.md): introducción ST con logo propio sin contenedor y notificaciones inferiores derechas. Derivado transparente con original preservado; fuente Web sin nuevo build/render. Sin cambios de comunicación/firmware.

## Mapa y refinamiento de interfaz — AFR2-20261023-18

[Ficha](changes/2026-10-23-18-control-map-and-ui-polish.md), [perfil pose](contracts/WEB_CONTROL_MAP.md), [manual0.1.7](docs/design-system/index.md). Velo Radal más fiel, toast con ancho y texto completos, mapa con cámaras. Fuente Web sin nuevo build/render; ESP/AVR no reportan pose por esta ficha. Sin datos, el mapa muestra solo un esquema identificado.


## Controles OLED, avisos cerrables y secuencias — AFR2-20261023-21

Web adaptada en fuente (`VIS-018`, manual `0.1.8`): componente compartido `DismissibleNotice` con `noticeId` para avisos en cabecera y modales; controles OLED siempre visibles (`Giro izquierdo`, `SW`, `Giro derecho` y `Actualizar pantalla`) con motivo de bloqueo y estado visual largo (`>= 800 ms`) mantenido hasta soltar; controles gráficos de giro sobre el eje (`ChassisSpinGraphic`); tarea `Secuencias` en Control con documento `afr2-sequence-1`, dos niveles (`Movimientos` y `Cuatro ruedas FL/FR/RL/RR`), copia avanzada conservando el original, simulador cinemático Mecanum determinista de `10 ms` aislado de telemetría y cliente físico para `control.renew` y `robot.sequence.*`. Especificación binaria coordinada en [UART_PROFILE_3.md](contracts/UART_PROFILE_3.md) (estado diseñado para ESP/ATmega). [Ficha completa](changes/2026-10-23-21-web-oled-notices-sequences.md).


## Integración vigente — AFR2-20261024-01

Revisadas fichas19/20/21. La Web21 ya contiene los cambios visuales/editor/simulador;
Wi-Fi20 se conserva. La integración de firmware pendiente se adapta en fuente:
ROBO2/perfil2 + EX01 v2, estado/pose, control supervisado, framebuffer OLED y
secuencias0x40..0x45 sin colisión con comandos de producto19.
Sin nuevo build, validador, tests, flasheo o promoción LittleFS. Secuencias físicas
permanecen bloqueadas por defecto hasta aprobar SRAM/pila y calibración real.
Contrato en AutoFinalRobo2-Contracts/contracts/EX01_WEB_INTEGRATION.md y ficha
AutoFinalRobo2-Contracts/changes/2026-10-24-01-firmware-web-integration.md.



