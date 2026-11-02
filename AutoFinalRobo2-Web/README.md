> **AFR2-20261102-04:** `#/infraestructura` en Explorar, Inicio y Más; recorridos visuales de Web y modelos 3D, pesos GLB, tabla gzip y LittleFS. Vite dev/preview lee el paquete local preparado y lo distingue del reporte del ESP. [Ficha](../AutoFinalRobo2-Contracts/changes/2026-11-02-04-infrastructure-explorer.md). Solo fuente; sin tests, compilación, validaciones, navegador ni promoción.

> **AFR2-20261102-03:** simulador con escena y controles lado a lado, zoom +/− flotante con porcentaje, cámara inicial igual a los banners de Modelos y banners re-renderizados (`tools/banner-renders`).

> **AFR2-20261102-02:** Ayuda → OLED: pantalla a la izquierda, lista a la derecha y «Ver árbol de pantallas» (`?vista=arbol`); el Simulador UI de Desarrollador redirige ahí. [Ficha](../AutoFinalRobo2-Contracts/changes/2026-11-02-02-oled-help-tree-view.md).

> **AFR2-20261102-01:** `/modelos` con tres banners → `/modelos/:modelo` (inspección, «Ir a simulación») → `/simulacion/:tipo`; banner de Inicio (`HomeHero`); mapa de pantallas en Simulador UI. Renders en `public/brand/models/` (scripts en `tools/banner-renders/`). Revisado en navegador local; sin build:esp. [Ficha](../AutoFinalRobo2-Contracts/changes/2026-11-02-01-browser-review-models-flow-tree.md).

> **AFR2-20261101-12 / VIS-053:** rutas por tarea (`src/app/navigation.ts`): Conducir, Programas, Puesta a punto, Sensores, Modelos y simulacion, Conexion y Ayuda, con redirecciones; barra inferior en celular. [Mapa](docs/ui-navigation-20261101.json) · [Ficha](../AutoFinalRobo2-Contracts/changes/2026-11-01-12-visual-plan-phase5-structure.md). Fuente sin tests/build/navegador ni promocion.

> **AFR2-20261101-11:** textos de la interfaz con tildes otra vez (sustituye AFR2-20261026-03); el texto dibujado en la replica OLED no cambia. [Ficha](../AutoFinalRobo2-Contracts/changes/2026-11-01-11-web-accents-restored.md). Fuente sin tests/build/navegador ni promocion.

> **AFR2-20261101-10 / VIS-051:** `PageHeader` (h1 unico) y `SectionHeader` h2/h3 con la escala `--type-*` (`styles/headers.css`); Modelos 3D sin vidrio exterior y chips de estado en Control. [Ficha](../AutoFinalRobo2-Contracts/changes/2026-11-01-10-visual-plan-phase4-headers.md). Fuente sin tests/build/navegador ni promocion.

> **AFR2-20261101-09 / VIS-052:** `SegmentedControl` (`src/components/ui`, `styles/segmented.css`) reemplaza los siete estilos de selector; TaskSections conserva su API. [Ficha](../AutoFinalRobo2-Contracts/changes/2026-11-01-09-visual-plan-phase3-segmented.md). Fuente sin tests/build/navegador ni promocion.

> **AFR2-20261101-08 / VIS-050:** iconos en `src/components/ui/icons/registry.ts` (Lucide ISC + propios), `UiIcon size` 16/20/24; glifos y SVG sueltos reemplazados. Plan visual con fase 7 OLED. [Ficha](../AutoFinalRobo2-Contracts/changes/2026-11-01-08-visual-plan-phase2-icons.md). Fuente sin tests/build/navegador ni promocion.

> **AFR2-20261101-07 / VIS-050..054:** plan visual fase 1: tokens `--type-*`, espaciado por rol, breakpoints 600/840/1200/1600 en `@theme`, `--attention*` y Segoe UI Variable; seis variables CSS sin definir corregidas. [Ficha](../AutoFinalRobo2-Contracts/changes/2026-11-01-07-visual-plan-phase1-tokens.md). Fuente sin tests/build/navegador ni promocion.

> **AFR2-20261101-06 / VIS-049:** Simulador UI en /desarrollador/ui-simulator, tarjeta debajo de Protocol Studio, ayuda y pestanas con el mismo lenguaje visual. [Ficha](../AutoFinalRobo2-Contracts/changes/2026-11-01-06-ui-simulator-workspace.md). Fuente sin tests/build/navegador ni promocion.

> **AFR2-20261101-04:** encuadre cercano y centrado por geometria en simuladores y Modelos 3D; conserva orbita/zoom manuales. [Ficha](../AutoFinalRobo2-Contracts/changes/2026-11-01-04-close-model-framing.md). Fuente sin tests, builds, validadores ni navegador.

> **AFR2-20261101-03 / VIS-048:** entorno de Modelos 3D con cielo/piso canonicos de Simulaciones y fichas solidas para claro/oscuro. [Cambios y limites](../AutoFinalRobo2-Contracts/changes/2026-11-01-03-models-scene-contrast.md). Solo fuente; sin tests, build, validadores, navegador ni promocion ESP.

> **AFR2-20261101-02:** Simulador UI cotejado con el flujo AVR: retorno contextual, Si/No, Hall, rapidas, Wi-Fi y umbral largo configurable. Arbol desplegable y [documentacion del flujo](../AutoFinalRobo2-Contracts/documentation/UI_SCREEN_FLOW.md). Solo fuente; sin tests, compilaciones, generadores, navegador, promocion ni cargas.

> **AFR2-20261030-16:** tamaño y progreso real de assets 3D con componente reutilizable, caché compartida y ETA de descarga. [Detalle y límites](../AutoFinalRobo2-Contracts/changes/2026-10-30-16-model-asset-progress.md). Solo fuente; sin build, pruebas, generadores ni promoción.

> **AFR2-20261030-02:** cabecera dashboard con SSID e IP STA/AP y scroll independiente de ida/vuelta, recortado entre icono y batería fijos. Solo bandas de texto en redibujo parcial. Fuentes AVR/Web vinculadas, sin compilar ni cargar. Ficha en Contracts/changes/2026-10-30-02-dashboard-network-scroll.md.

> **AFR2-20261030-01:** dashboard010001 adaptado de AutoMicro con iconos Wi-Fi/AP/encoder, batería por bandas de voltaje y accesos corto Menú/largo Rápidas al soltar. AVR y copia raster Web vinculados, sin nuevo wire. Fuente sin compilar, probar ni cargar. Ver ../AutoFinalRobo2-Contracts/changes/2026-10-30-01-dashboard-automicro-icons.md.

> **AFR2-20261029-27 — fuente vigente:** ATmega/ESP0.11.0 y Web0.4.0; carrusel de controles, largo al soltar con retorno contextual, radar4/2/1/0.5m/rastro/editor compartido, IR compactos, PWM manual independiente de odometría/HC y atención prioritaria de sesión WS. EEPROMv5 / EX01v9, preservando batería26. Sin compilar, validadores, flasheos, pruebas físicas ni promoción Web. Recursos OLED copiados solamente; memoria y aceptación pendientes. [Contrato y límites](../AutoFinalRobo2-Contracts/contracts/QUICK_RADAR_ACTUATORS_9.md).

> **AFR2-20261029-26:** batería A5/divisor10k/10k/104, Sistema → Batería y copia Web, puente EX01v9, pill con icono por voltaje y confirmación antes de reiniciar ESP/ATmega desde Web. [Contrato y estados](../AutoFinalRobo2-Contracts/contracts/BATTERY_MEASUREMENT_9.md). Solo fuente: sin compilar, generar catálogo OLED, probar, promover paquete ni cargar. Memoria/calibración/paridad físicas pendientes; ambas placas y Web requieren despliegue conjunto autorizado.

> **AFR2-20261029-19 compilado y ATmega cargado:** pedido humano29/10/2026. ATmega/ESP/Web esp y LittleFS completados; promoción local Web a ESP/data/dist. Carga COM8 final13:32:50,80066bytes escritos/verificados, avrdude exit0. ESP/LittleFS sin cargar. SRAM estática7085bytes; sin prueba física/dinámica. Evidencia en AutoFinalRobo2-Contracts/documentation/builds/2026-10-29-19/README.md. Estados inferiores sin build/carga son históricos.

> **Fuente AFR2-20261029-20:** Hall una vuelta con selección de rueda, conteo manual y clic para terminar; largo cancela con aviso. Drafts por4ruedas, estado CPR con asterisco, guardado asíncrono y diálogo genérico Si/No al salir sin guardar. OLED/Web vinculados en fuente; Web sin build/promoción, recursos generados pendientes del build autorizado. [Ficha](../AutoFinalRobo2-Contracts/changes/2026-10-29-20-hall-wizard-save-confirm.md). Compilación AVR pendiente al redactar; no flashear, placa conserva15.

> **AFR2-20261028-10:** Protocol Studio recupera el flujo de AutoMicro: parámetros guiados, estructura por campos, cuatro salidas, mapa seleccionable, stream continuo y ayuda. Identidad AFR2 0.1.9 conservada. [Continuidad](../AutoFinalRobo2-Contracts/docs/PROTOCOL_STUDIO_AUTOMICRO.md). Fuente sin tests, build, validadores, navegador ni promoción ESP.

> **AFR2-20261028-09:** Protocol Studio en **Desarrollador**, con Traductor de capturas HEX/texto y Constructor ROBO2. Copia HEX/RealTerm/C/Python/decimal, lectura de arranque ESP y tramas con XOR. [Uso](docs/modules/protocol-studio.md). Fuente implementada; sin tests, compilación, validadores, navegador ni promoción ESP en esta revisión.

> **Build AFR2-20261027-10:** perfil esp compilado,8activos/1571104bytes gzip y copy:esp completado. [Evidencia](../AutoFinalRobo2-Contracts/documentation/builds/2026-10-27-10/README.md). Retorno tipado de startManualScan corregido para TypeScript; sin navegador, tests ni carga física.

> **AFR2-20261027-06:** calibracion con ayuda contextual para 29 parametros; matematica mayor y centrada con el mismo formulario en Auto/Brazo/Conjunto. Pad de movimiento, boton mantenido y campos numero/barra reutilizados con Control. [Ficha](../AutoFinalRobo2-Contracts/changes/2026-10-27-06-calibration-live-math-shared-controls.md). Solo fuente, sin tests/build/validaciones/navegador ni promocion ESP.

> **Presentación vigente AFR2-20261026-02:** radar recompuesto con campo continuo, escala inicial1m, vistas Perspectiva/Superior y parámetros debajo del mapa. [Detalle](../AutoFinalRobo2-Contracts/changes/2026-10-26-02-radar-composition.md). Solo fuente; **sin build, tests, validadores ni navegador**. Paquete ESP anterior; semántica de controles16 y Modelo3D01 conservados.

> **Fuente vigente AFR2-20261025-16:** radar con volumen/neón rubí, selector doble angular, controles barra/número y Configuracion sin acento. [Detalle](../AutoFinalRobo2-Contracts/changes/2026-10-25-16-radar-volume-scan-controls.md). **Sin compilar, validar en navegador ni promover activos al ESP**. Los artefactos14 no incluyen esta interfaz. ATmega/ESP/OLED/CAD sin cambios de esta revisión.

> **Compilación AFR2-20261025-14 autorizada y completada:** firmware ATmega (producto/comunicación/biblioteca), ESP y Web full compilan sin errores ni advertencias finales. Firmware0.10.0/EX01v8, Web0.3.0. [Evidencia](../AutoFinalRobo2-Contracts/documentation/builds/2026-10-25-14/README.md). Sin tests, navegador ni flasheos; paquete Web local full generado, **ESP/data/dist conserva el paquete anterior**. La SRAM estática no certifica pila/ISR ni hardware. Estados previos inferiores históricos.

> **Sensores y red AFR2-20261025-14:** [radar autónomo, servo/IR, pantallas hermanas y red](../AutoFinalRobo2-Contracts/contracts/SENSORS_RADAR_8.md). Firmware nominal0.10.0/EX01v8 y Web0.3.0: fuente integrada y recursos OLED copiados, **sin compilar, probar, renderizar ni flashear**. Paquete ESP anterior; estados inferiores históricos. Modelo3D conserva pose; otras escenas siguen PWM aplicado.

> **Vigente AFR2-20261025-11:** giro local con dos dedos/Q-E, orientación persistente y rumbo visible; levantar uno cancela hasta soltar ambos. No envía yaw físico. Cámara10/chevrones09 conservados. [Contrato](../AutoFinalRobo2-Contracts/contracts/WEB_TOUCH_SIMULATION.md), [build11](../AutoFinalRobo2-Contracts/documentation/builds/2026-10-25-11/README.md). Web compilada/promovida; firmware0.9.0/EX01v7/build01/gates0 sin cambios. Gesto/teléfono No verificados; estados inferiores históricos.

> **Vigente AFR2-20261025-01:** [control Web directo](../AutoFinalRobo2-Contracts/contracts/DIRECT_CONTROL_7.md), sin permiso previo ni habilitación desde OLED. Firmware0.9.0/EX01v7 y Web0.2.0,30comandos/59ScreenCodes, compilados juntos. STOP/caducidad/watchdog y requisitos físicos conservados; sin flash/tests/placas. Estados inferiores históricos.

> **Vigente AFR2-20261024-15:** [Matriz del maestro](../AutoFinalRobo2-Contracts/docs/ALINEACION_MAESTRO.md):33comandos y59identidades OLED con fuentes complementarias. Calibración Web adquiere control al escribir; ESP/Web informan FS/paquete. Web/ESP compilados15; AVR sin nueva fuente, build12/EX01v6/firmware0.8.0/gates0. Sin brecha software definida identificada tras lectura; aceptación física, memoria dinámica, flash/SPI y planos CAD pendientes. Solo documentación en CAD, sin diseño nuevo. Estados inferiores históricos.

> **Vigente AFR2-20261024-12:** mapa/comando actual juntos y coordenadas laterales; Hardware/Reinicio anidados, editor de pistas reversible y distancia por rueda EX01v6. Firmware0.8.0 y Web compilados; activos promovidos localmente. Build AVR incluye fuente IRQ11 con gates0; sin navegador, Hall habilitado, hardware, flasheo ni CAD. [Ficha12](../AutoFinalRobo2-Contracts/changes/2026-10-24-12-control-hardware-distance.md). Los estados inferiores son históricos.

> **AFR2-20261024-10:** Control/GOTO agrega destinos con clic/toque y lista de pasos relativos, reproducción ordenada y seguimiento del comando activo. Mapa completo y coordenadas en fila; recorrido debajo con botones al lado. Fade/scroll con tokens y reduced motion, sin dependencias nuevas. [Módulo GOTO](docs/modules/goto-simulation.md), [ficha10](../AutoFinalRobo2-Contracts/changes/2026-10-24-10-goto-map-queue.md). Web compilada/empaquetada/promovida; visual/gestos sin comprobar. La cola es local, sin envío a placas. Estados inferiores históricos.

> **AFR2-20261024-09:** OLED usa las59 identidades compartidas (48nodos/11avisos), copia actual de layouts/primitivas AVR y recursos generados del C. Inicio/build enlazan catálogo/glifos/XBM; cambios C requieren port revisado. El visor conectado exige framebuffer1024bytes, sin fallback antiguo. Vista local de menús identificada. [Módulo OLED](docs/modules/oled-ui.md) y [ficha09](../AutoFinalRobo2-Contracts/changes/2026-10-24-09-linked-oled-library.md). Web compilada/promovida, sin comparación diferencial, navegador ni placas. Estados anteriores históricos.

> **AFR2-20261024-08:** guía Web actualizada para IP/AP, avisos OLED y reinicios locales EX01v5. El visor existente consume los nuevos IDs como framebuffer; la clave AP se mantiene solo local. TypeScript/Vite/paquete/promoción completados:1452714bytes gzip,7activos. Sin navegador ni placas. [Estado actual](../AutoFinalRobo2-Contracts/docs/ESTADO_IMPLEMENTACION.md). Estados inferiores históricos.

> **Vigente AFR2-20261024-07:** Control uniforme, Secuencias a todo el ancho con propiedades debajo, mapa solo simulado y apoyo GLB por vértices precisos. Conexión automática al mismo origen; Sistema (info/boot/reinicio) y Desarrollador por triple toque. Web compilada/empaquetada y promovida; revisión visual bloqueada por permiso de navegador. Secuencias físicas siguen gated. [Estado por módulo](../AutoFinalRobo2-Contracts/docs/ESTADO_IMPLEMENTACION.md) · [Contrato EX01v4](../AutoFinalRobo2-Contracts/contracts/EX01_SYSTEM_SEQUENCE_4.md). Los estados previos inferiores son históricos.

> Actualizacion **AFR2-20261024-06**: suelo en la base del modelo y GLB final compartido por inspeccion y Control (Superior ortografica, Perspectiva y Seguir auto). Escala1unidad=1m; cuadricula0,5m, sin agrandar la malla. Web compilada/promovida: **1.450.617 bytes gzip**,7 activos. Sin render visual, pruebas fisicas ni flasheo. [Detalle](../AutoFinalRobo2-Contracts/changes/2026-10-24-06-grounded-model-control.md).

> Correccion **AFR2-20261024-05**: Reintentar carga 3D elimina el error cacheado por useLoader; publicDir absoluto y copia selectiva de build conservada. Web compilada y promovida: **1.450.338 bytes gzip**. Diagnostico HTTP local: GLB/Draco/WebP200 con contenido correcto; sin renderizado visual ni flasheo. Modelo final y layout candidato3MiB de04 conservados. [Detalle](../AutoFinalRobo2-Contracts/changes/2026-10-24-05-model-loading-retry.md).

> Vigente **AFR2-20261024-04**: **modelo final incluido en ESP**, paquete **1.450.201 bytes gzip** (49,96% menos que los 2,90 MB iniciales). WebP, JS/CSS minificados y dependencias/componentes optimizados. Layout candidato: flash 4 MiB / LittleFS nominal 3 MiB, solicitado por el usuario; flash fisica sin identificar. Web full/esp y firmware ESP compilados; activos promovidos a data/dist. Sin navegador, imagen FS ni flasheo. Atmega y comandos conservan el estado de02. [Evidencia](../AutoFinalRobo2-Contracts/changes/2026-10-24-04-final-model-littlefs.md). Los estados previos inferiores son historicos.

> Estado vigente **AFR2-20261024-02**, 24/10/2026: integración ROBO2/perfil2 + EX01v3 compilada en Atmega/ESP/Web; GOTO completo, configuración por catálogo compartido y jobs Wi-Fi OLED conectados. Paquete Web actualizado en ESP/data/dist. Sin pruebas de navegador/hardware ni flasheo; secuencias físicas mantienen bloqueo de memoria. Ver [matriz vigente](../AutoFinalRobo2-Contracts/contracts/EX01_ALIGNMENT_3.md) y [evidencia](../AutoFinalRobo2-Contracts/changes/2026-10-24-02-project-alignment.md). Los estados previos que siguen son históricos.

> **AFR2-20261027-05:** ancho util ampliado en toda la aplicacion. Simulaciones en una tarjeta con matematica adosada/plegable y controles abajo; numeros compactos y motores distinguidos. Control agrupa manual/secuencias en Movimiento y pruebas/calibracion en una seccion, conservando funciones y GOTO. [Estado](../AutoFinalRobo2-Contracts/changes/2026-10-27-05-wide-workspaces-control-cards.md). Fuente sin tests/build/validaciones/navegador ni promocion ESP. Modelos3D sin reforma interna de esta ficha.

# AutoFinalRobo2-Web

> **Fuente AFR2-20261031-01:** Desarrollador agrega **Simulador UI** para navegar
> la OLED con los tres controles compartidos de la vista en vivo. Edicion,
> teclado, guia y escenarios con datos ficticios, sin comandos al equipo.
> [Uso y alcance](docs/modules/ui-simulator.md). Sin tests, compilacion,
> validaciones, navegador ni promocion ESP de esta entrega.

> **Fuente AFR2-20261027-11:** puntos guardados y trayectorias en Brazo/Conjunto,
> descriptor de robot independiente del GLB e IK cartesiana en Worker. Los destinos
> conservan posicion/orientacion al cambiar dimensiones. [Contrato y guia para
> incorporar modelos](../AutoFinalRobo2-Contracts/contracts/WEB_ARM_POINTS.md).
> Esta entrega no ejecuta tests/build/validadores/navegador ni promocion ESP;
> el paquete de otra ficha no acredita esta fuente final. Perfil inicial v03,
> sin sustituir el CAD v04 concurrente ni ejecutar el pendiente de envolvente.

## Acceso desde otro dispositivo de la LAN

Con Vite abierto en localhost:5173, ejecutar `npm run proxy:lan` en otra
terminal de este módulo. El proxy imprime las direcciones para abrir desde
el teléfono, con puerto5180. Incluye HTTP, recursos3D y WebSockets de Vite;
no emula el robot. [Uso y puertos configurables](docs/modules/lan-proxy.md).
Herramienta AFR2-20261025-12, independiente del bundle y de LittleFS.



Aplicación Web oficial de `AutoFinalRobo2` (`AFR2-20261023-09`), implementada según el [Plan Web aprobado](../AutoFinalRobo2-Contracts/docs/PLAN_WEB.md) y el [Sistema de Identidad Visual v0.1.3](../AutoFinalRobo2-Contracts/docs/design-system/index.md).



## Alcance implementado en `src/`



- **Stack TypeScript + React 19 + Vite + Tailwind v4**:

  - `src/styles/tokens.css` y `src/styles/index.css`: 107 tokens canónicos (`VIS-001`..`VIS-013`), 6 densidades de vidrio con un único `backdrop-filter` por región, acento carmesí/rubí (`#b81d33` / `#ff6b7d`) y superficie de emergencia bermellón (`#c93b1b` / `#ff8a65`) sobre `--surface-solid`.

  - `src/app/providers/ThemeProvider.tsx`: persistencia `afr2-theme` (`light` | `dark` | `system`), sincronización dinámica de `<meta name="theme-color">` y detección de modalidad teclado (`0 ms` en transiciones).

  - `src/components/notifications/notify.tsx`: integración de `Sonner` (`sonner@^2.0.8`) con un único `<Afr2Toaster />` raíz, posición `top-right` en escritorio y `top-center` en móvil (`< 820px`, sin cubrir el botón `STOP` inferior), botón explícito de cierre (`toast.dismiss(id)`) e ID estable `afr2-connection` para eventos de sesión.

- **Cliente de protocolo WebSocket (`afr2-ws-1`)**:

  - `src/protocol/wsEnvelope.ts` y `src/protocol/afr2Client.ts`: separación de los 3 estados (`wsState`, `megaLinkState`, `controlLease`), handshake `system.hello` (`5000 ms`), heartbeat `system.alive` (`2000 ms` / `6000 ms`), reconexión exponencial con jitter, política `single-latest-slot` para `robot.drive` y prioridad máxima para `robot.stop`.

- **Siete vistas con `HashRouter`**:

  1. `#/` (`HomePage`): estado de los 3 enlaces, emblema oficial WebP `/brand/Logo-256.webp` (`BrandMark`) y matriz de capacidades reales (`capabilities.json`).

  2. `#/control` (`ControlPage`): permiso temporal de control Web (`control.acquire` / `control.release`), pad Mecanum 4WD con corte seguro por pérdida de foco, prueba individual de rueda (`FL`/`FR`/`RL`/`RR`), misión `GOTO` condicionada por odometría y calibración sin IMU.

  3. `#/sensores` (`SensorsPage`): 5 canales `IR1..IR5`, ultrasonido `HC-SR04` y contadores Hall de 4 ruedas con frescura/antigüedad explícitas (`—` cuando no hay lectura válida, sin ceros inventados).

  4. `#/wifi` (`WifiPage`): estado `STA`/`AP`, escaneo asíncrono `wifi.scan` con indicador local y formulario efímero `wifi.configure` sin persistir secretos.

  5. `#/oled` (`OledPage`): rasterizador `<canvas>` bit a bit de `128×64` píxeles portado de `afr2_oled.c`, `font7x10.c` y `afr2_screens.c` con catálogo compartido59 (48nodos/11avisos), copia actual AVR y framebuffer remoto obligatorio, control de encoder único (`SW` corto `< 800 ms` confirma, `SW` largo `>= 800 ms` abre Menú principal y suprime corto al soltar, atrás solo por ítem `Volver`) y árbol objetivo (`MENU_PLAN.md`).

  6. `#/modelo-3d` (`Model3DPage`): visor del modelo final `/models/AutoFinalRobo2.glb` con Draco WASM local y recuperación de errores, incluido también en el paquete ESP. Fuente exacta del exportado CAD Fusion v16/Blender.

  7. `#/documentacion` (`DocsPage`): consulta offline de arquitectura, cableado `D18/D19`, navegación OLED y sistema visual.

- **Empaquetado LittleFS y promoción a ESP**:

  - `tools/package-web.mjs`: genera `artifacts/littlefs/dist` con `.gz` y `manifest.json` (`afr2-littlefs-manifest-1`).

  - `tools/copy-web-to-esp.mjs`: promueve de forma segura a `../AutoFinalRobo2-ESP/data/dist`.



## Referencias y estado de verificación



- [Informe de adaptación](INFORME_ADAPTACION.md) · [Informe maestro](../INFORME_MAESTRO.md) · [Contracts](../AutoFinalRobo2-Contracts/README.md).

- `reference/web` se mantiene como archivo histórico de solo lectura y no interviene en la configuración activa.

- Estado: código implementado, validado con TypeScript (`tsc -b`), compilado con `viteSingleFile` (`dist/index.html` `404,08 kB` gzip) y empaquetado para LittleFS (`artifacts/littlefs/dist`, `6` archivos, `2.925.405` bytes gzip totales incluyendo GLB, Draco y `Logo.png`) con `0` errores (`AFR2-20261023-09`).



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


## Controles OLED, avisos cerrables y secuencias — AFR2-20261023-21

- **Avisos con X (`DismissibleNotice`)**: aviso cerrable compartido entre cabecera (`RouteNav`) y modales (`FloatingModal`) con `noticeId` sincronizado en `ConnectionContext`; cerrarlo evita su reaparición al navegar o abrir un modal y conserva el estado real de `STOP`, enlace y lease.
- **Controles OLED siempre visibles (`EncoderControl` + `LiveOledViewer`)**: adapta la disposición, iconografía (`TurnArrowIcon`, `MonoBitmapEncoderIcon`) y botón manual `Actualizar pantalla` de `ScreenStreamWorkspace.tsx`. Muestra siempre **Giro izquierdo, SW y Giro derecho** con motivo explícito cuando están deshabilitados. `SW` único con umbral de `800 ms`: corto confirma al soltar; largo envía menú una vez y mantiene su estado visual de atención (`#f59e0b`) hasta soltar aunque la solicitud esté en curso.
- **Giro sobre el eje (`ChassisSpinGraphic`)**: dos controles con vista superior del chasis Mecanum y flecha circular antihoraria/horaria («Girar izquierda» y «Girar derecha»), reutilizados como símbolo en los bloques `pivot`.
- **Editor de secuencias y simulador Mecanum (`Secuencias` en Control)**: documento versionado `afr2-sequence-1`, dos niveles (`Movimientos` y `Cuatro ruedas FL/FR/RL/RR` sincronizadas), conversión a copia avanzada conservando el original, reordenamiento con `@dnd-kit` (`6.3.1`/`10.0.0`/`3.2.2`), teclado y botones, propiedades en panel lateral (escritorio) y `FloatingModal` (móvil), simulador determinista en pasos de `10 ms` separado de la telemetría en `ControlMap`, y cliente de ejecución física (`control.renew` cada `500 ms` + `robot.sequence.begin/append/commit/start/cancel/status` cada `250 ms`).
- **Estado**: fuente adaptada sin ejecutar `npm install`, compilación ni pruebas ([Ficha AFR2-20261023-21](../AutoFinalRobo2-Contracts/changes/2026-10-23-21-web-oled-notices-sequences.md) · [Procedencia](docs/ui-oled-sequences-20261023.json)).


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

## AFR2-20261102-05 — Ayuda OLED

Ayuda → OLED abre Simular flujo con datos ficticios y mantiene Biblioteca sin datos como alternativa. Reutiliza el reductor UI y el dibujo AVR portado; navegar conserva contexto y ajustes locales. Nuevos QR030045/030046 están en Wi-Fi. En el auto usan SSID/clave/IP reales; los ejemplos Web son ficticios. Detalles en Contracts/contracts/OLED_RUNTIME_QR_20261102.md. Sin tests/build/validadores/navegador/promoción ESP de esta entrega.
