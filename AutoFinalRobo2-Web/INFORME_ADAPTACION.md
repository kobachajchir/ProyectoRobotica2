> **AFR2-20261106-04:** SRAM AVR 5614 bytes, libres antes de pila 2578; QR finish 29 bytes. Se conservan ambos buffers OLED. Builds y paridad C/Web 2104/2104 PASS; sin carga ni prueba física. [Evidencia](../AutoFinalRobo2-Contracts/documentation/builds/2026-11-06-04/README.md).

> **AFR2-20261106-02:** regeneración y builds AVR/ESP/Web/Loader comprobados; paridad host OLED 1680/1680. **Carga bloqueada por presupuesto SRAM/pila AVR**; sin COM, flasheo ni prueba física. [Evidencia](../AutoFinalRobo2-Contracts/documentation/builds/2026-11-06-02/README.md).

> **AFR2-20261106-01 / fuente:** correcciones coordinadas EX01v13, IR digital/EEPROMv9 y Wi-Fi persistente/paginado. [Ficha y estado por modulo](../AutoFinalRobo2-Contracts/changes/2026-11-06-01-coordinated-corrections.md). Sin nuevas compilaciones, generadores, pruebas ni cargas; no acredita el firmware instalado.

> **AFR2-20261101-06 / VIS-049:** Simulador UI en /desarrollador/ui-simulator, tarjeta debajo de Protocol Studio, ayuda y pestanas con el mismo lenguaje visual. [Ficha](../AutoFinalRobo2-Contracts/changes/2026-11-01-06-ui-simulator-workspace.md). Fuente sin tests/build/navegador ni promocion.

> **AFR2-20261101-04:** encuadre cercano y centrado por geometria en simuladores y Modelos 3D; conserva orbita/zoom manuales. [Ficha](../AutoFinalRobo2-Contracts/changes/2026-11-01-04-close-model-framing.md). Fuente sin tests, builds, validadores ni navegador.

> **AFR2-20261101-03 / VIS-048:** entorno de Modelos 3D con cielo/piso canonicos de Simulaciones y fichas solidas para claro/oscuro. [Cambios y limites](../AutoFinalRobo2-Contracts/changes/2026-11-01-03-models-scene-contrast.md). Solo fuente; sin tests, build, validadores, navegador ni promocion ESP.

> **AFR2-20261101-02:** Simulador UI cotejado con el flujo AVR: retorno contextual, Si/No, Hall, rapidas, Wi-Fi y umbral largo configurable. Arbol desplegable y [documentacion del flujo](../AutoFinalRobo2-Contracts/documentation/UI_SCREEN_FLOW.md). Solo fuente; sin tests, compilaciones, generadores, navegador, promocion ni cargas.

> **Fuente AFR2-20261031-01:** Simulador UI en Desarrollador; navegacion del
> ATmega adaptada a estado local, tres controles de `EncoderControl` compartidos
> con el visor en vivo, edicion/teclado y snapshots ficticios identificados.
> [Ficha](../AutoFinalRobo2-Contracts/changes/2026-10-31-01-developer-ui-simulator.md).
> Sin tests, compilacion, validaciones, navegador ni promocion ESP.
> Firmware, identidades y recursos de dibujo conservados.

> **AFR2-20261030-19:** táctil espera modelo y vista3D listos, conectado al mismo estado del componente de progreso; luego permiso de motores igual a Mecanum, sin latch por error antiguo. [Ficha](../AutoFinalRobo2-Contracts/changes/2026-10-30-19-touch-model-ready.md). Solo fuente; sin build, tests, validadores, generación, promoción ni carga.

> **AFR2-20261030-18:** mismo permiso PWM para Mecanum/táctil, error histórico separado de bloqueo actual, modelo visual independiente y motivos de enlace/STOP/PWM visibles. [Ficha](../AutoFinalRobo2-Contracts/changes/2026-10-30-18-manual-touch-permissions.md). Solo fuente; sin generación, build, pruebas, promoción ni carga.

> **AFR2-20261030-18:** selector común de motor030044, pictogramas coherentes del encoder y dashboard con solo modo en el cuerpo. [Ficha y límites](../AutoFinalRobo2-Contracts/changes/2026-10-30-18-motor-selector-encoder-dashboard.md). Fuente AVR/Web sin compilar, probar ni cargar.

> **AFR2-20261030-16:** tamaño y progreso real de assets 3D con componente reutilizable, caché compartida y ETA de descarga. [Detalle y límites](../AutoFinalRobo2-Contracts/changes/2026-10-30-16-model-asset-progress.md). Solo fuente; sin build, pruebas, generadores ni promoción.

> **AFR2-20261030-13:** referencia longitudinal y yaw corregida; odometría/mezcla y cinemática Web coordinadas, Girar CW/CCW. [Ficha e impacto](../AutoFinalRobo2-Contracts/changes/2026-10-30-13-motion-reference.md). Solo fuente; sin generación, compilación, validadores, pruebas, promoción ni carga.

> **AFR2-20261030-09:** Wi-Fi coordinado EX01v11, EEPROMv8 sin cambios; [ficha de fuente y límites](../AutoFinalRobo2-Contracts/changes/2026-10-30-09-wifi-state-flows.md). Sin generación, compilación, validadores, pruebas, navegador, promoción ni cargas; evidencia anterior corresponde a fuentes históricas.

> **AFR2-20261030-07:** fuente coordinada EX01v10/EEPROMv8; [detalle](../AutoFinalRobo2-Contracts/changes/2026-10-30-07-ui-integral.md). Sin compilar, ejecutar, validar, probar, generar, promover ni cargar. Conserva evidencia previa solo para sus fuentes históricas.

> **AFR2-20261030-02:** cabecera dashboard con SSID e IP STA/AP y scroll independiente de ida/vuelta, recortado entre icono y batería fijos. Solo bandas de texto en redibujo parcial. Fuentes AVR/Web vinculadas, sin compilar ni cargar. Ficha en Contracts/changes/2026-10-30-02-dashboard-network-scroll.md.

> **AFR2-20261030-01:** dashboard010001 adaptado de AutoMicro con iconos Wi-Fi/AP/encoder, batería por bandas de voltaje y accesos corto Menú/largo Rápidas al soltar. AVR y copia raster Web vinculados, sin nuevo wire. Fuente sin compilar, probar ni cargar. Ver ../AutoFinalRobo2-Contracts/changes/2026-10-30-01-dashboard-automicro-icons.md.

> **AFR2-20261029-29:** desde dashboard, corto abre menu completo010002; largo al soltar abre pantallas rapidas010003. Ambos accesos conservan retorno al origen por historial. Sustituye el acceso comun de28. Fuente sin compilar/cargar.

> **AFR2-20261029-28:** dashboard abre menu de controles con clic corto; largo conservado al soltar. OLED/Web vinculados; sin compilacion, validadores, prueba ni carga.

> **AFR2-20261029-27 — fuente vigente:** ATmega/ESP0.11.0 y Web0.4.0; carrusel de controles, largo al soltar con retorno contextual, radar4/2/1/0.5m/rastro/editor compartido, IR compactos, PWM manual independiente de odometría/HC y atención prioritaria de sesión WS. EEPROMv5 / EX01v9, preservando batería26. Sin compilar, validadores, flasheos, pruebas físicas ni promoción Web. Recursos OLED copiados solamente; memoria y aceptación pendientes. [Contrato y límites](../AutoFinalRobo2-Contracts/contracts/QUICK_RADAR_ACTUATORS_9.md).

> **AFR2-20261029-26:** batería A5/divisor10k/10k/104, Sistema → Batería y copia Web, puente EX01v9, pill con icono por voltaje y confirmación antes de reiniciar ESP/ATmega desde Web. [Contrato y estados](../AutoFinalRobo2-Contracts/contracts/BATTERY_MEASUREMENT_9.md). Solo fuente: sin compilar, generar catálogo OLED, probar, promover paquete ni cargar. Memoria/calibración/paridad físicas pendientes; ambas placas y Web requieren despliegue conjunto autorizado.

> **AFR2-20261029-19 compilado y ATmega cargado:** pedido humano29/10/2026. ATmega/ESP/Web esp y LittleFS completados; promoción local Web a ESP/data/dist. Carga COM8 final13:32:50,80066bytes escritos/verificados, avrdude exit0. ESP/LittleFS sin cargar. SRAM estática7085bytes; sin prueba física/dinámica. Evidencia en AutoFinalRobo2-Contracts/documentation/builds/2026-10-29-19/README.md. Estados inferiores sin build/carga son históricos.

> **Fuente AFR2-20261029-20:** Hall una vuelta con selección de rueda, conteo manual y clic para terminar; largo cancela con aviso. Drafts por4ruedas, estado CPR con asterisco, guardado asíncrono y diálogo genérico Si/No al salir sin guardar. OLED/Web vinculados en fuente; Web sin build/promoción, recursos generados pendientes del build autorizado. [Ficha](../AutoFinalRobo2-Contracts/changes/2026-10-29-20-hall-wizard-save-confirm.md). Compilación AVR pendiente al redactar; no flashear, placa conserva15.

> **Fuente AFR2-20261029-18:** IR1..IR5 OSOYOO digital (A4..A0), perfil predeterminado habilitado y EEPROMv4 migra solo tipos antiguos deshabilitados; calibración/seguimiento siguen pendientes. Diagnóstico servo/encoder agrega lectura RAW directa y cambios por OLED/serie, sin EEPROM. Vistas IR/Web distinguen deshabilitado/pendiente/cero válido. [Ficha](../AutoFinalRobo2-Contracts/changes/2026-10-29-18-ir-input-acquisition.md). Compilaciones pendientes al registrar la fuente; no cargar por instrucción vigente. Placa conserva15; Web sin build/promoción.

> **Fuente AFR2-20261029-16:** OLED sin esperas artificiales entre etapas, radar con grilla punteada, cono visual nominal15grados por PWM aplicado y discos rellenos por eco válido. Copia Web vinculada; sin build Web/promoción ESP. [Ficha](../AutoFinalRobo2-Contracts/changes/2026-10-29-16-oled-latency-radar-cone.md). Build/carga AVR COM8 autorizados, pendientes al registrar esta nota. Se conserva corrección del watchdog y migración Hall de15, ya cargada y verificada enCOM8 a12:02:57.

> **Fuente AFR2-20261029-15:** corregido bucle Bresenham del radar que bloqueaba main y podía disparar watchdog al girar desde HC-SR04; productos trigonométricos AVR en32bits y contraparte Web vinculada. Hall conectado por montaje confirmado, migración local EEPROMv3 preserva calibraciones y habilitación de control. [Ficha](../AutoFinalRobo2-Contracts/changes/2026-10-29-15-radar-watchdog-hall-wiring.md). Build/carga AVR COM8 autorizados, pendientes al registrar esta nota; sin prueba física del cambio.

> **AFR2-20261028-10:** Protocol Studio recupera el flujo de AutoMicro: parámetros guiados, estructura por campos, cuatro salidas, mapa seleccionable, stream continuo y ayuda. Identidad AFR2 0.1.9 conservada. [Continuidad](../AutoFinalRobo2-Contracts/docs/PROTOCOL_STUDIO_AUTOMICRO.md). Fuente sin tests, build, validadores, navegador ni promoción ESP.

> **AFR2-20261028-09:** Protocol Studio en **Desarrollador**, con Traductor de capturas HEX/texto y Constructor ROBO2. Copia HEX/RealTerm/C/Python/decimal, lectura de arranque ESP y tramas con XOR. [Uso](docs/modules/protocol-studio.md). Fuente implementada; sin tests, compilación, validadores, navegador ni promoción ESP en esta revisión.

> **Compilación14 posterior al pedido de fuente:** ATmega/ESP/Web compilados sin errores ni advertencias finales; carpeta build Windows y siete advertencias de indentación corregidas. Sin tests, navegador, flash ni promoción a ESP/data/dist. Ver Contracts/documentation/builds/2026-10-25-14/README.md. La falta de compilación mencionada más abajo describe la entrega previa.

> **Fuente vigente AFR2-20261025-14:** [sensores, servo y radar autónomo EX01v8](../AutoFinalRobo2-Contracts/contracts/SENSORS_RADAR_8.md). Firmware nominal0.10.0/Web0.3.0,41comandos/64identidades OLED. Fuentes vinculadas y recursos Web copiados; sin compilar, ejecutar tests/validadores, renderizar ni flashear. Paquete ESP anterior; los estados inferiores son históricos.

> **Vigente AFR2-20261025-11:** giro local con dos dedos/Q-E, orientación persistente y rumbo visible; levantar uno cancela hasta soltar ambos. No envía yaw físico. Cámara10/chevrones09 conservados. [Contrato](../AutoFinalRobo2-Contracts/contracts/WEB_TOUCH_SIMULATION.md), [build11](../AutoFinalRobo2-Contracts/documentation/builds/2026-10-25-11/README.md). Web compilada/promovida; firmware0.9.0/EX01v7/build01/gates0 sin cambios. Gesto/teléfono No verificados; estados inferiores históricos.

> **Vigente AFR2-20261025-01:** [control Web directo](../AutoFinalRobo2-Contracts/contracts/DIRECT_CONTROL_7.md), sin permiso previo ni habilitación desde OLED. Firmware0.9.0/EX01v7 y Web0.2.0,30comandos/59ScreenCodes, compilados juntos. STOP/caducidad/watchdog y requisitos físicos conservados; sin flash/tests/placas. Estados inferiores históricos.

> **Vigente AFR2-20261024-15:** [Matriz del maestro](../AutoFinalRobo2-Contracts/docs/ALINEACION_MAESTRO.md):33comandos y59identidades OLED con fuentes complementarias. Calibración Web adquiere control al escribir; ESP/Web informan FS/paquete. Web/ESP compilados15; AVR sin nueva fuente, build12/EX01v6/firmware0.8.0/gates0. Sin brecha software definida identificada tras lectura; aceptación física, memoria dinámica, flash/SPI y planos CAD pendientes. Solo documentación en CAD, sin diseño nuevo. Estados inferiores históricos.

> **Vigente AFR2-20261024-12:** mapa/comando actual juntos y coordenadas laterales; Hardware/Reinicio anidados, editor de pistas reversible y distancia por rueda EX01v6. Firmware0.8.0 y Web compilados; activos promovidos localmente. Build AVR incluye fuente IRQ11 con gates0; sin navegador, Hall habilitado, hardware, flasheo ni CAD. [Ficha12](../AutoFinalRobo2-Contracts/changes/2026-10-24-12-control-hardware-distance.md). Los estados inferiores son históricos.

> **AFR2-20261024-10:** Control/GOTO agrega destinos con clic/toque y lista de pasos relativos, reproducción ordenada y seguimiento del comando activo. Mapa completo y coordenadas en fila; recorrido debajo con botones al lado. Fade/scroll con tokens y reduced motion, sin dependencias nuevas. [Módulo GOTO](docs/modules/goto-simulation.md), [ficha10](../AutoFinalRobo2-Contracts/changes/2026-10-24-10-goto-map-queue.md). Web compilada/empaquetada/promovida; visual/gestos sin comprobar. La cola es local, sin envío a placas. Estados inferiores históricos.

> **AFR2-20261024-09:** OLED usa las59 identidades compartidas (48nodos/11avisos), copia actual de layouts/primitivas AVR y recursos generados del C. Inicio/build enlazan catálogo/glifos/XBM; cambios C requieren port revisado. El visor conectado exige framebuffer1024bytes, sin fallback antiguo. Vista local de menús identificada. [Módulo OLED](docs/modules/oled-ui.md) y [ficha09](../AutoFinalRobo2-Contracts/changes/2026-10-24-09-linked-oled-library.md). Web compilada/promovida, sin comparación diferencial, navegador ni placas. Estados anteriores históricos.

> **AFR2-20261024-08:** guía Web actualizada para IP/AP, avisos OLED y reinicios locales EX01v5. El visor existente consume los nuevos IDs como framebuffer; la clave AP se mantiene solo local. TypeScript/Vite/paquete/promoción completados:1452714bytes gzip,7activos. Sin navegador ni placas. [Estado actual](../AutoFinalRobo2-Contracts/docs/ESTADO_IMPLEMENTACION.md). Estados inferiores históricos.

> **AFR2-20261024-07:** interfaz Control/editor, suelo preciso, Chevron compartido, Sistema/Desarrollador, conexión automática y Docs Web/ESP/ATmega compilados. Adaptaciones por archivo y fuentes de solo lectura en [procedencia07](../AutoFinalRobo2-Contracts/provenance/control-system-sequence-20261024.json). [Estado actual](../AutoFinalRobo2-Contracts/docs/ESTADO_IMPLEMENTACION.md). No se verificó el render: acceso de navegador denegado. No se ejecutaron tests ni placas. Estados inferiores históricos.

> Actualizacion **AFR2-20261024-06**: suelo en la base del modelo y GLB final compartido por inspeccion y Control (Superior ortografica, Perspectiva y Seguir auto). Escala1unidad=1m; cuadricula0,5m, sin agrandar la malla. Web compilada/promovida: **1.450.617 bytes gzip**,7 activos. Sin render visual, pruebas fisicas ni flasheo. [Detalle](../AutoFinalRobo2-Contracts/changes/2026-10-24-06-grounded-model-control.md).

> Correccion **AFR2-20261024-05**: Reintentar carga 3D elimina el error cacheado por useLoader; publicDir absoluto y copia selectiva de build conservada. Web compilada y promovida: **1.450.338 bytes gzip**. Diagnostico HTTP local: GLB/Draco/WebP200 con contenido correcto; sin renderizado visual ni flasheo. Modelo final y layout candidato3MiB de04 conservados. [Detalle](../AutoFinalRobo2-Contracts/changes/2026-10-24-05-model-loading-retry.md).

> Vigente **AFR2-20261024-04**: **modelo final incluido en ESP**, paquete **1.450.201 bytes gzip** (49,96% menos que los 2,90 MB iniciales). WebP, JS/CSS minificados y dependencias/componentes optimizados. Layout candidato: flash 4 MiB / LittleFS nominal 3 MiB, solicitado por el usuario; flash fisica sin identificar. Web full/esp y firmware ESP compilados; activos promovidos a data/dist. Sin navegador, imagen FS ni flasheo. Atmega y comandos conservan el estado de02. [Evidencia](../AutoFinalRobo2-Contracts/changes/2026-10-24-04-final-model-littlefs.md). Los estados previos inferiores son historicos.

> Estado vigente **AFR2-20261024-02**, 24/10/2026: integración ROBO2/perfil2 + EX01v3 compilada en Atmega/ESP/Web; GOTO completo, configuración por catálogo compartido y jobs Wi-Fi OLED conectados. Paquete Web actualizado en ESP/data/dist. Sin pruebas de navegador/hardware ni flasheo; secuencias físicas mantienen bloqueo de memoria. Ver [matriz vigente](../AutoFinalRobo2-Contracts/contracts/EX01_ALIGNMENT_3.md) y [evidencia](../AutoFinalRobo2-Contracts/changes/2026-10-24-02-project-alignment.md). Los estados previos que siguen son históricos.

# Adaptación Web (`AFR2-20261023-09`)

AFR2-20261025-12: herramienta propia [proxy LAN](docs/modules/lan-proxy.md),
`tools/lan-proxy.mjs`, basada solo en node:net/node:os. package.json incorpora
`proxy:lan`; documentación de uso/puertos añadida. Inicio local observado en
0.0.0.0:5180, destino localhost:5173; sin modificar Vite, fuentes de la app,
lockfile, dependencias, build11 o activos ESP. Sin tests, nueva compilación,
navegador ni acceso comprobado desde otro dispositivo.



[Informe maestro](../INFORME_MAESTRO.md) · [Contracts](../AutoFinalRobo2-Contracts/README.md) · [Plan Web](../AutoFinalRobo2-Contracts/docs/PLAN_WEB.md).



## Estado del módulo



`src/` contiene ahora la aplicación TypeScript/React 19 adaptada a `AutoFinalRobo2` con el sistema visual Glassmorphism `v0.1.3` (paleta carmesí/rubí `#b81d33` / `#ff6b7d`, emergencia bermellón `#c93b1b` / `#ff8a65` y emblema `/brand/Logo.png`), notificaciones `Sonner`, cliente WebSocket `afr2-ws-1`, réplica OLED `128×64` bit a bit y control de encoder único.



`reference/web` conserva las fuentes históricas de solo lectura para trazabilidad (`Contracts/provenance/files.json`) y no participa en el árbol activo.



## Subsistemas implementados



| Subsistema | Archivos activos en `src/` y `tools/` | Adaptación realizada |

| --- | --- | --- |

| Sistema visual, tema y notificaciones | `src/styles/tokens.css`, `src/styles/index.css`, `src/app/providers/ThemeProvider.tsx`, `src/components/notifications/notify.tsx`, `src/components/ui/*` | 107 tokens `0.1.3`, 6 densidades de vidrio sin blur anidado, `BrandMark` con `/brand/Logo.png`, `<Afr2Toaster />` único con `toast.dismiss(id)` e ID estable `afr2-connection`. |

| Cliente WebSocket y sesión | `src/protocol/wsEnvelope.ts`, `src/protocol/afr2Client.ts`, `src/features/connection/ConnectionContext.tsx` | Sobre `afr2-ws-1`, separación de `wsState`, `megaLinkState` y `controlLease`, handshake `system.hello` (`5000 ms`), latido `system.alive` (`2000 ms`/`6000 ms`), `robot.stop` prioritario y `single-latest-slot` para `robot.drive`. |

| Control Mecanum, pruebas y sensores | `src/features/control/ControlPage.tsx`, `src/features/sensors/SensorsPage.tsx` | Tracción Mecanum de 4 ruedas (`FL`/`FR`/`RL`/`RR`), corte automático al perder foco, `GOTO` bloqueado mientras no exista odometría cableada/calibrada, telemetría de 5 canales `IR1..IR5` y `HC-SR04` sin simular valores ni mostrar IMU/balance. |

| Réplica OLED 128×64 y encoder único | `src/features/oled/font7x10.ts`, `src/features/oled/oledRasterizer.ts`, `src/features/oled/OledCanvas128x64.tsx`, `src/features/oled/EncoderControl.tsx`, `src/features/oled/OledPage.tsx` | Port exacto de `font7x10.c`, `afr2_oled.c` y `afr2_screens.c` (`0x010001`, `0x010002`, `0x020001`, `0x020002`). Giro `±1`, `SW` corto `< 800 ms` confirma, `SW` largo `>= 800 ms` abre Menú principal y suprime corto al soltar; retroceso solo por ítem `Volver`. |

| Red Wi-Fi, Modelo 3D y Documentación | `src/features/wifi/WifiPage.tsx`, `src/features/model3d/Model3DPage.tsx`, `src/features/docs/DocsPage.tsx` | Escaneo asíncrono `wifi.scan`, credenciales efímeras `wifi.configure`, visor GLB+Draco local (`/draco/`, `/models/auto_micro.glb` advertido como malla de prueba) y documentación offline. |

| Empaquetado LittleFS | `tools/package-web.mjs`, `tools/copy-web-to-esp.mjs` | Staging atómico en `artifacts/littlefs/dist`, generación de `manifest.json` (`afr2-littlefs-manifest-1`) y copia controlada a `../AutoFinalRobo2-ESP/data/dist`. |



## Reorganización actual — AFR2-20261023-10



Fuente de interfaz revisada con Apple Design/Emil y manual 0.1.4: Inicio de accesos, navegación lateral y móvil, Control por tarea, Wi-Fi por paso, Sensores por grupo y documentación/detalles plegados. Espaciado mayor, fondo sin halos, menos sombras y cabecera compacta. STOP sticky visible; cambio/salida de tarea cancela hold manual. Sin cambios de protocolo ni recursos binarios.



[Adaptaciones por archivo y hashes resultantes](docs/ui-refactor-20261023.json). Los resultados de build descritos para AFR2-20261023-09 son históricos: esta edición no se compiló ni se renderizó, y dist/paquete ESP no se actualizaron. No se ejecutaron pruebas ni validadores. Ver ficha en Contracts/changes/2026-10-23-10-calm-web-workspaces.md.


## Integración ESP — AFR2-20261023-11

El broker ESP ahora está implementado en fuente (HTTP/WS/LittleFS/Wi-Fi/UART, sin compilar ni probar). Dos ajustes de integración propios: `src/protocol/afr2Client.ts` actualiza capacidades por heartbeat y evento system.status; `src/features/connection/ConnectionContext.tsx` confirma STOP solo con status=completed y stopped=true. La falta de control de potencia AVR produce error explícito, sin parada física ficticia. Se conserva la reorganización visual AFR2-20261023-10.

Estos cambios fuente requieren un nuevo build, empaquetado y promoción autorizados; dist y ESP/data/dist siguen representando AFR2-20261023-09. No se ejecutaron tests, compilaciones, validadores ni flasheos en esta ficha. [Contrato y estado completo](../AutoFinalRobo2-Contracts/changes/2026-10-23-11-esp-server-and-mega-link.md).


## Compilación actual — AFR2-20261023-15

TypeScript y Vite completados, con postbuild LittleFS exitoso: seis archivos, 2.925.630 bytes gzip totales. Se verificaron los seis gzip por descompresión, tamaños y SHA-256 del contenido contra manifest.json. El primer intento falló por EPERM al renombrar dist.staging en Windows; tools/package-web.mjs incorpora fallback de publicación por archivos con manifiesto al final, y verifica destinos antes de limpiezas recursivas. El fallback no garantiza intercambio atómico de carpetas y fue la ruta ejecutada en el build final.

Los artefactos actuales están en dist y artifacts/littlefs/dist; incluyen las revisiones de fuente presentes al compilar. ESP/data/dist conserva su paquete anterior: no se ejecutó copy:esp. [Evidencia](../AutoFinalRobo2-Contracts/documentation/builds/2026-10-23-15-esp-web/README.md). Sin renderizado/prueba de navegador ni hardware en esta ficha. El nuevo protocolo está pendiente de documentación del lado ATmega.

## Navegación flotante — AFR2-20261023-14

Manual 0.1.5: barra lateral persistente/colapsable adaptada de ST2026-Radal (335d3ed), iconos y etiquetas, OLED superior global y Ajustes en FloatingModal compartido. Home tiene tarjetas sólidas con ilustraciones propias a la derecha. Modo de cabecera consultado por robot.state, sin inventar un modo activo. Sonner usa el borde inferior medido de cabecera para dejar STOP libre; consultas periódicas no generan toasts.

La ruta /oled y el modal comparten el consumidor remoto, sin snapshot simulado ni navegación optimista. El firmware actual aún rechaza robot.screen.get/robot.ui.action; se muestra disponibilidad real. Abrir un modal cancela hold manual; STOP cancela también el emisor local. No se modificaron GLB/Draco, dependencias ni firmware.

[Detalle](../AutoFinalRobo2-Contracts/docs/WEB_FLOATING_UI.md) · [Perfil consumidor](../AutoFinalRobo2-Contracts/contracts/WEB_OLED_CONSUMER.md) · [Adaptaciones/hashes](docs/ui-floating-20261023.json). Fuentes implementadas sin compilar ni probar; no se ejecutó servidor/renderizado ni se actualizaron dist, LittleFS o ESP/data/dist. Los resultados anteriores corresponden a sus revisiones históricas.

## Inicio y logo — AFR2-20261023-16

StartupWelcome adapta la introducción de ST Radal al logo propio: entrada de 300ms, pausa de 600ms, salida de logo de 200ms y velo de 300ms, final alrededor de 1300ms tras cargar imagen. Se monta una vez por carga de la Web, fuera del cambio de rutas. Escape/Omitir cierran inmediatamente, STOP permanece disponible, el teclado queda contenido por dialog nativo y la página se libera si la imagen falla o en 4s como máximo. Movimiento reducido usa solo fade de 120ms.

BrandMark usa public/brand/Logo-transparent.png sin medallón, borde, sombra ni fondo en barra e Inicio. Derivado PNG RGBA generado con image_gen; el original está preservado en reference/brand/Logo-original.png y assets/Logo.png. El favicon usa el mismo derivado; el PNG opaco se retira de public para evitar doble empaquetado. No se garantiza coincidencia de píxeles entre el derivado generativo y el original.

Manual 0.1.6/VIS-016 y procedencia en Contracts/provenance/web-startup.json. Fuente guardada sin compilar, renderizar ni probar esta revisión. Los builds anteriores no cubren estos cambios. Sin modificaciones de firmware ni contrato de comunicación.

Sonner queda en bottom-right en escritorio y móvil por indicación del usuario; offsets inferiores/derechos con safe area y margen móvil que deja libre el rail. Se retira el cálculo de offset superior de la cabecera.

## Mapa e interfaz — AFR2-20261023-18

El inicio recupera el material, halo/sheen y logo libre de Radal, con controles sin aspecto tarjeta y espera del decode. Sonner se presenta en portal body, bottom-right, con ancho explícito de viewport/item y texto multilínea; una notificación visible por vez evita recorte por pila. Se conservan cierre, estados y safe areas.

Control incorpora Superior SVG y Perspectiva/Seguir auto Three.js lazy, zoom/centrar, coordenadas y trazo acotado. Las órdenes se identifican aparte; no mueven la posición del dibujo. El chasis es esquemático de referencia, no CAD validado. Sin pose, esquema y coordenadas desconocidas; el firmware actual no produce el perfil pose todavía.

[Perfil y límites](../AutoFinalRobo2-Contracts/contracts/WEB_CONTROL_MAP.md), [ficha](../AutoFinalRobo2-Contracts/changes/2026-10-23-18-control-map-and-ui-polish.md), [manifest](docs/ui-control-map-20261023.json). Lectura robot.state compartida entre modo/mapa, timeout2500 y pausa1000ms tras terminar; no duplica queries ni genera toast por tick. Fuente sin compilar/renderizar/probar; builds/paquetes anteriores no incluyen esta revisión.

## Wi-Fi independiente del Mega — AFR2-20261023-20

Completado el flujo local ESP/Web: scan espera resultado terminal, estado real de radio/fallback/aplicación, selección Web de guardado temporal o permanente y validación UTF-8. Fuente compilada en ESP y Web, sin warnings/errors ESP ni errores TypeScript/Vite/postbuild. Sin hardware ni renderizado. UART/ROBO2, broker y módulo ATmega sin ediciones en esta ficha. [Contrato, procedencia y pendientes](../AutoFinalRobo2-Contracts/changes/2026-10-23-20-wifi-esp-web.md). Paquete Web local actualizado (6 archivos, 2.855.479 bytes gzip), ESP/data/dist sin promover.


## Controles OLED, avisos cerrables y secuencias de acciones — AFR2-20261023-21

Se adaptaron desde `reference/web` (`ScreenStreamWorkspace.tsx`, `TimelineRow.tsx`, `MotorBlock.tsx`, `StopDiscontinuityGlyph.tsx` y `ControlSection.tsx`) los controles OLED y el editor de líneas de tiempo hacia la arquitectura Mecanum de 4 ruedas de `AutoFinalRobo2` (`VIS-018` / manual `0.1.8`):

- `src/components/ui/DismissibleNotice.tsx`, `src/features/connection/ConnectionContext.tsx`, `src/components/navigation/RouteNav.tsx`, `src/components/ui/FloatingModal.tsx`: cierre compartido por `noticeId` para avisos fijos en cabecera y modales sin alterar el estado real de `STOP`, enlace o lease.
- `src/features/oled/EncoderControl.tsx` y `src/features/oled/LiveOledViewer.tsx`: controles `Giro izquierdo`, `SW` y `Giro derecho` más botón `Actualizar pantalla` siempre visibles incluso deshabilitados, motivo junto al control, umbral de `800 ms` y retención del estado visual largo hasta soltar.
- `src/features/control/ChassisSpinGraphic.tsx` y `src/features/control/ControlPage.tsx`: botones gráficos de giro sobre el eje («Girar izquierda» y «Girar derecha»), renovación de lease cada `500 ms` (`control.renew`) y tarea `Secuencias`.
- `src/features/control/sequences/*` y `src/features/control/ControlMap.tsx`: esquema `afr2-sequence-1`, niveles `Movimientos` y `Cuatro ruedas (FL/FR/RL/RR)`, copia avanzada conservando el original, simulador cinemático Mecanum determinista de `10 ms` con fuente separada de telemetría y cliente físico para `robot.sequence.begin/append/commit/start/cancel/status`.
- Sin ejecución de `npm install`, builds, tests ni validadores en esta entrega. Detalle y hashes/autores en [ficha AFR2-20261023-21](../AutoFinalRobo2-Contracts/changes/2026-10-23-21-web-oled-notices-sequences.md) y [procedencia](../AutoFinalRobo2-Contracts/provenance/web-oled-notices-sequences.json).


## Integración vigente — AFR2-20261024-01

Revisadas fichas19/20/21. La Web21 ya contiene los cambios visuales/editor/simulador;
Wi-Fi20 se conserva. La integración de firmware pendiente se adapta en fuente:
ROBO2/perfil2 + EX01 v2, estado/pose, control supervisado, framebuffer OLED y
secuencias0x40..0x45 sin colisión con comandos de producto19.
Sin nuevo build, validador, tests, flasheo o promoción LittleFS. Secuencias físicas
permanecen bloqueadas por defecto hasta aprobar SRAM/pila y calibración real.
Contrato en AutoFinalRobo2-Contracts/contracts/EX01_WEB_INTEGRATION.md y ficha
AutoFinalRobo2-Contracts/changes/2026-10-24-01-firmware-web-integration.md.



## Paquete compacto intermedio — AFR2-20261024-03 (histórico)

El PNG maestro queda fuera de los paquetes. Se usan WebP1024/256/64 con alpha;
fuente exacta preservada en reference/brand/Logo-transparent-source.png. Regenerar
opcionalmente con `python tools/optimize-images.py` requiere Pillow con WebP
(versión usada12.3.0); npm no necesita Python para compilar.

`npm ci` instala lo fijado por package-lock. `npm run build:esp` produce cuatro
activos,649.410 bytes gzip, y los copia a ESP/data/dist. El límite de software es
900KiB; la estimación con bloques/reserva es656KiB, sin afirmar capacidad física.
`npm run build` incluye el GLB de prueba y Draco WASM para PC; `copy:esp` rechaza
ese perfil. Los dos builds sobrescriben dist y el paquete local; preview sirve
el último. No producen imagen FS ni flashean.

JS usa Terser (tres pasadas), CSS esbuild y Tailwind solo fuentes activas. Se
retiraron framer-motion y Drei, reemplazando sus auxiliares usados por Three.
Se comparten campos/validación Wi-Fi, chasis esquemático, órbita y recuperación
de render. Control, secuencias, sensores, Wi-Fi, OLED y navegación conservan sus
contratos. La vista3D ESP es esquemática; no incorpora la malla antigua.
Los tamaños/afirmaciones anteriores son evidencia histórica de cada ficha.


## Modelo final y entrega actual — AFR2-20261024-04

El usuario pidió incluir el GLB final en el ESP y reservar 3 MiB para LittleFS,
con flash de 4 MiB supuesta. Ambos perfiles usan ahora el mismo modelo final:
`public/models/AutoFinalRobo2.glb`, copia exacta de CAD, 827.232 bytes. Se mantiene
el mapa ligero de Control, que no usa el legacy. El legacy se archivó fuera de
public, y la selección explícita de activos excluye maestros y decodificadores
innecesarios. Geometría/materiales del GLB final sin modificaciones.

Comandos vigentes: `npm ci`; `npm run build` compila/empaqueta full local;
`npm run build:esp` compila/empaqueta/promueve a ESP/data/dist. Los dos incluyen
modelo y Draco locales, sin CDN. `package:littlefs` usa el perfil real de dist;
`copy:esp` exige el presupuesto compartido y el linker candidato coincidente.
`preview` sirve el último dist. No generan imagen FS ni flashean.

Paquete: 1.450.201 bytes gzip; estimación con reserva: 1.540.096 bytes.
Presupuesto preventivo: 2800 KiB bajo LittleFS candidato nominal de 3 MiB.
Firmware ESP recompilado con eagle.flash.4m3m.ld. La flash física, SPI, navegador
y hardware siguen sin comprobar. Fuentes y autorías preservadas; detalles y
hashes en Contracts/changes/2026-10-24-04-final-model-littlefs.md y
Web/docs/model-asset-20261024.json. Los párrafos de hitos anteriores son historia.




## AFR2-20261029-19 — OLED táctil y visor bajo demanda

Fuente adaptada con Web/ATmega/ESP: overlay scrim+blur en modales, Volver por ítem del menú, ScreenCode0x030035 con aviso/bitmap/chevrones y suscripción explícita a capturas/metadatos. Contrato en AutoFinalRobo2-Contracts/contracts/TOUCH_OLED_19.md; procedencia por archivo en provenance/touch-oled-20261029-19.json de Contracts. El rumbo es solicitado, no medido, y no agrega giro físico. Sin tests, compilación, validadores, navegador, ejecución del generador OLED, promoción ESP ni flasheo de esta entrega. Memoria/paridad/latencia físicas pendientes.


## AFR2-20261106-12 — Hall y PWM manual

Adaptación de fuente coordinada; véase Contracts/contracts/HALL_MANUAL_STREAM_20261106.md y Contracts/provenance/hall-motion-20261106-12.json. EEPROM AVR v10; Hall por motor y raster Web vinculados; ESP PWM reciente sin esperar ACK AVR por consigna. Sin tests, compilaciones, validadores, generadores, promoción ni carga. Fluidez, SRAM/pila y funcionamiento físico pendientes. Bypass manual conservado; restitución de permisos pendiente.


## AFR2-20261106-13 — Movimiento sin requisito HC-SR04

Se retiran los bloqueos de inicio y parada por lectura HC-SR04 inválida o distancia frontal<150mm en los modos físicos vigentes. Se conservan adquisición, telemetría, radar y simulación de aproximación. El simulador de UI refleja los requisitos del firmware. Sin tests, compilaciones, validadores, generadores ni cargas. Véase Contracts/changes/2026-11-06-13-motion-without-hcsr-gate.md.
