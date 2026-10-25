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

# AutoFinalRobo2-Contracts

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

Módulos hermanos: ../AutoFinalRobo2-Atmega, ../AutoFinalRobo2-ESP, ../AutoFinalRobo2-Web y [AutoFinalRobo2-CAD](../AutoFinalRobo2-CAD/README.md), dedicado a KiCad, la pieza nueva y el modelo completo del auto. Los cinco pertenecen al único Git de D:/ProyectoRobotica2. No hay Qt ni dependencia de ejecución hacia AutoMicro. reference/automicro conserva evidencia histórica: sus reglas y estados no son instrucciones ni resultados del nuevo proyecto.

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



