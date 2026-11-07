> **AFR2-20261107-52:** Hall tolerante a la holgura (unos 370 pulsos por vuelta ya se aceptan), nueva prueba de encoder, ícono de largo centrado, árbol de pantallas con zoom, ajustes de márgenes y captura del Loader en la Web. ATmega sin compilar; Web sin build. [Ficha](changes/2026-11-07-52-hall-tolerance-ui-polish.md).

> **AFR2-20261107-51:** OLED: la falla era un conector flojo, ya corregido. Firmware del auto recargado con el Loader en COM16 y verificado (SHA-256 16826727…7ca0a4). Red STA por defecto «Koba» guardada localmente, con el ESP sin compilar. [Ficha](changes/2026-11-07-51-mega-restore-loader-wifi.md).

> **AFR2-20261107-50:** prueba de servicio OLED/I2C compilada para descartar hardware o cableado (`build/afr2-i2c-diagnostic.hex`, flash 5,6 %, SRAM 39,0 %). Cargada en COM16 y verificada: el Mega queda con la prueba hasta volver a cargar `afr2.hex`. [Ficha](changes/2026-11-07-50-oled-i2c-diagnostic-build.md).

> **AFR2-20261107-49:** firmware ATmega compilado con los cambios del 07/11 (OLED, notificaciones, IR, lateral): sin errores, flash 50,4 %, SRAM 77,3 %. Sin cargar. [Ficha](changes/2026-11-07-49-atmega-build.md).

> **AFR2-20261107-48:** el clic sobre una notificación de red ya no deja la OLED trabada en una cadena de avisos. ATmega solo fuente, sin compilar. [Ficha](changes/2026-11-07-48-network-notice-click.md).

> **AFR2-20261107-47:** pantalla inicializada al final, con espera de 1,2 s y una configuración completa de confirmación. ATmega solo fuente, sin compilar. [Ficha](changes/2026-11-07-47-oled-init-last.md).

> **AFR2-20261107-46:** OLED con refresco periódico liviano y menos tráfico I2C; causas eléctricas a medir. ATmega solo fuente, sin compilar. [Ficha](changes/2026-11-07-46-oled-refresh-scrub.md).

> **AFR2-20261107-45:** Loader 2.0.0 portable: distribución sin rutas ni datos de esta PC (escaneo de texto y EXE antes de comprimir) y ZIP con separadores `/`; compilado y empaquetado, sin abrir en otra PC. [Ficha](changes/2026-11-07-45-loader-portable.md).

> **AFR2-20261107-44:** [informe de pinout actualizado](hardware/INFORME_PINOUT_ROBOT_20261107.html) con sensor de batería (A5) y AutoKEY/AutoRESET (D32/D33). Solo documentación. [Ficha](changes/2026-11-07-44-pinout-report-update.md).

> **AFR2-20261107-43:** sensores de línea OSOYOO digitales válidos sin calibración obligatoria; la calibración pide solo la muestra sobre blanco. ATmega solo fuente, sin compilar ni prueba física. [Ficha](changes/2026-11-07-43-ir-osoyoo-digital.md).

> **AFR2-20261107-42:** control táctil con paneles cerrados al abrir, joystick y animaciones de movimiento; Web solo fuente, sin compilar. [Ficha](changes/2026-11-07-42-touch-drive-ui.md).

> **AFR2-20261107-41:** izquierda/derecha corregidas al conducir (signo lateral -1 en ATmega y Web); solo fuente, sin compilar ni prueba física. [Ficha](changes/2026-11-07-41-lateral-wheel-sign.md).

> **AFR2-20261106-15:** [informe completo de pinout](hardware/INFORME_PINOUT_ROBOT_20261106.md) y [PDF](hardware/output/pdf/INFORME_PINOUT_ROBOT_20261106.pdf). Agregados propios marcados, incluidos D32 AutoKEY y D33 AutoRESET. Solo documentación; ATmega/ESP/Web sin cambios ni nuevas pruebas.

> **AFR2-20261106-14:** incorporado el PDF de encoders BioLog y [cotejado con el código](hardware/COTEJO_ENCODERS_BIOLOG_20261106.md). Pines coincidentes; x4/CPR1500/signos del PDF no se trasladan a AFR2 x1/calibración individual. ATmega/ESP/Web sin cambios; solo documentación, sin tests, compilaciones, validadores ni pruebas físicas.

> **AFR2-20261106-13:** retirado HC-SR04 como requisito y causa de parada de movimiento en todos los modos físicos vigentes. Sensores/radar y simulación conservados; aproximación física al brazo pendiente. ATmega y simulador UI Web solo fuente, ESP sin cambios; sin tests, compilaciones, validadores ni cargas.

> **AFR2-20261106-12:** Hall por motor con Confirmar/Descartar y EEPROMv10; CW exterior positivo. PWM manual con slot reciente ESP sin esperar ACK AVR por consigna; ALIVE/STOP conservados. Solo fuente, sin compilación, tests, validadores, generadores, carga ni prueba física. Restituir permisos del bypass manual queda pendiente. Ver [contrato](contracts/HALL_MANUAL_STREAM_20261106.md).

> **AFR2-20261106-04:** RAM AVR optimizada: 2022 bytes estáticos recuperados, margen2578; QR finish29. Ambos buffers OLED conservados. Builds y paridad2104/2104 PASS; sin carga ni aceptación física. [Ficha](changes/2026-11-06-04-avr-ram-qr.md).

> **AFR2-20261106-03 / Loader 1.2.0:** nueva distribución compartible compilada (GUI/MCP), rutas {App}/módulos y datos de cada usuario configurables; pestaña ATmega redundante retirada, soporte conservado en Flasher/Operación. ZIP y auditoría: [ficha](changes/2026-11-06-03-loader-portable-120.md). Sin COM, firmware compilado ni flasheos por esta entrega.

> **AFR2-20261106-02:** regenerado y compilado; paridad 1680/1680 C/TS. **Despliegue bloqueado por SRAM/pila AVR**. Sin cargas ni pruebas físicas. [Comprobaciones](documentation/builds/2026-11-06-02/README.md).

> **AFR2-20261106-01:** EX01v13 / EEPROMv9: IR digital, Wi-Fi persistente y paginado, presion WS y pantallas vinculadas. [Estado y limites](changes/2026-11-06-01-coordinated-corrections.md). Solo fuente; sin generar, compilar, probar ni cargar.

> **Revisión del auto, 06/11/2026:** síntomas comunicados, cotejo estático ATmega/ESP/Web y plan priorizado de IR, HC-SR04, PWM, Wi-Fi/AP/WS y pantallas. [Informe y mapa de archivos](docs/REVISION_AUTO_Y_PLAN_20261106.md). Diagnóstico y plan; sin nuevas correcciones de firmware, pruebas ni cargas.
>
> **AFR2-20261105-01:** comprobaciones del Loader en columna a la derecha de la terminal, con log más alto. Solo fuente, sin compilar ni ejecutar la interfaz modificada. [Ficha](changes/2026-11-05-01-loader-checks-sidebar.md).
>
> **AFR2-20261105-33:** OLED autorreparable ante ruido I2C (refresco de configuración y RAM); sin compilar. [Ficha](changes/2026-11-05-33-oled-self-healing.md).
>
> **AFR2-20261104-32:** el ESP admite 24 activos Web y manifiesto de 12 KiB; sin compilar ni medir RAM. [Ficha](changes/2026-11-04-32-esp-24-assets.md).
>
> **AFR2-20261103-31:** la Web compila; peso medido y bloqueo de 17 archivos frente al límite de 16 del ESP. [Ficha](changes/2026-11-03-31-web-build-weight.md).
> **AFR2-20261103-30:** banco OLED único para Pantalla en vivo y Ayuda; modal de Configuración ordenado. [Ficha](changes/2026-11-03-30-oled-workbench.md).
> **AFR2-20261103-29:** banner de Inicio sin recortes en anchos chicos. [Ficha](changes/2026-11-03-29-home-hero-narrow.md).
> **AFR2-20261103-28:** Estado de red con «Sin datos» en todo el panel. [Ficha](changes/2026-11-03-28-wifi-no-data.md).
> **AFR2-20261103-27:** teclado en Pantalla en vivo (← → Espacio, Escape). [Ficha](changes/2026-11-03-27-live-screen-keyboard.md).
> **AFR2-20261103-26:** Ayuda reescrita, código resaltado y pinout por módulo. [Ficha](changes/2026-11-03-26-help-rewrite-pinout.md).
> **AFR2-20261103-25:** Loader más directo, banner de IA y sin carteles de fuente. [Ficha](changes/2026-11-03-25-loader-direct-no-source.md).
> **AFR2-20261102-24:** tarjetas técnicas por módulo en Ayuda y página del Loader en Explorar. [Ficha](changes/2026-11-02-24-help-descriptors-loader-page.md).
> **AFR2-20261102-23:** Ayuda técnica por módulo en orden ATmega, ESP, Web y OLED, con datos tomados del código. [Ficha](changes/2026-11-02-23-web-help-technical.md).
> **AFR2-20261102-22:** manual revisado en todas las rutas y pantalla en vivo con actualización periódica o al cambiar. [Ficha](changes/2026-11-02-22-web-manual-review-live-screen.md).
> **AFR2-20261102-21:** radar de la Web rediseñado como instrumento, revisado en las cuatro escalas. [Ficha](changes/2026-11-02-21-radar-instrument-redesign.md).

> **AFR2-20261102-14:** dashboard con tarjeta de estados por inactividad y familia OLED alineada AVR/Web. [Contrato](contracts/OLED_DASHBOARD_STATUS_20261102.md). Fuentes y recursos copiados, sin pruebas/build/validaciones/navegador/flasheo.

> **AFR2-20261102-14:** [Manual visual 0.1.11](docs/design-system/index.md) ampliado de forma aditiva con toda la identidad de Infraestructura: reglas por contexto, atlas completo, componentes, tokens y dos láminas explicativas. Contenido anterior conservado. Solo documentación, sin tests, compilación, validaciones ni navegador. [Ficha](changes/2026-11-02-14-visual-manual-infrastructure-integration.md).

> **AFR2-20261102-11:** dashboard original con cabecera invertida, OLED blanca y filas de submenú con flecha grande + Volver. [Ficha](changes/2026-11-02-11-oled-original-dashboard-white-back.md). Solo fuente AVR/Web, sin pruebas/build/validaciones/flasheo.

> **AFR2-20261102-11:** [Identidad visual extraída de Infraestructura](docs/design-system/infraestructura/index.md): DOM, paletas, capas del banner, layout, tipografía, espaciado, selectores, estados y movimiento. Solo documentación desde fuente, sin tests, compilación, validaciones ni navegador. [Ficha](changes/2026-11-02-11-infrastructure-visual-identity-extraction.md).

> **AFR2-20261102-10:** Infraestructura con cubo en el peso 3D, LittleFS desde la placa (system.info) y paso de Blender ampliado. [Ficha](changes/2026-11-02-10-infrastructure-cube-board-fs-blender.md).

> **AFR2-20261102-07:** reforma visual de Infraestructura (mapa de flujo animado, línea de metro, barras y glosario en tarjetas). [Ficha](changes/2026-11-02-07-infrastructure-visual-redesign.md).

> **AFR2-20261102-05:** componentes OLED128-5 activos en fuentes AVR/Web; Ayuda → OLED con simulación local; QR de conexión AP y acceso Web generados dentro del auto. [Ficha](changes/2026-11-02-05-oled-runtime-components-qr.md). Sin compilar/probar/validar/flashear.

> **AFR2-20261102-04:** Explorar → Infraestructura, flujo Web/CAD hasta flash/HTTP/navegador, tamaños GLB/gzip y reporte del paquete local o del ESP. [Ficha](changes/2026-11-02-04-infrastructure-explorer.md) · [Contrato HTTP](contracts/WEB_INFRASTRUCTURE.md). atmega sin cambios; esp/web solo fuente, sin tests, compilación, validaciones, navegador, promoción ni flasheos.

> **AFR2-20261102-03:** simuladores con controles a la derecha, zoom flotante y cámara de los banners; banners re-renderizados. [Ficha](changes/2026-11-02-03-simulator-side-controls-zoom-banners.md).

> **AFR2-20261102-02:** Ayuda → OLED reúne vista previa, lista y árbol de pantallas; el Simulador UI de Desarrollador se retiró. [Ficha](changes/2026-11-02-02-oled-help-tree-view.md).

> **AFR2-20261102-01 / VIS-055/056:** Web revisada en navegador; Modelos por banners con inspección directa y simulador aparte; banner de Inicio; mapa de pantallas desde el dashboard. [Ficha](changes/2026-11-02-01-browser-review-models-flow-tree.md). Sin build:esp ni hardware.

> **AFR2-20261101-12 / VIS-053:** plan visual fase 5: Web organizada por tareas con barra inferior en celular y rutas nuevas con redirecciones. [Ficha](changes/2026-11-01-12-visual-plan-phase5-structure.md). Fuente sin tests/build/navegador.

> **AFR2-20261101-11:** la Web recupera las tildes (sustituye AFR2-20261026-03); la replica OLED sigue sin ellas por su fuente de pixeles. [Ficha](changes/2026-11-01-11-web-accents-restored.md). Fuente sin tests/build/navegador.

> **AFR2-20261101-10 / VIS-051:** plan visual fase 4: un h1 por pagina con PageHeader, encabezados internos sin saltos y chips de estado en Control. [Ficha](changes/2026-11-01-10-visual-plan-phase4-headers.md). Fuente sin tests/build/navegador.

> **AFR2-20261101-09 / VIS-052:** plan visual fase 3: un solo selector segmentado con pill deslizante en pestanas y elecciones. [Ficha](changes/2026-11-01-09-visual-plan-phase3-segmented.md). Fuente sin tests/build/navegador.

> **AFR2-20261101-08 / VIS-050:** plan visual fase 2: una sola familia de iconos (Lucide + propios), STOP con octagono y sin glifos como icono; fase 7 OLED agregada al plan. [Ficha](changes/2026-11-01-08-visual-plan-phase2-icons.md). Fuente sin tests/build/navegador.

> **AFR2-20261101-07 / VIS-050..054:** plan visual fase 1: decisiones y tokens de tipo, espaciado, breakpoints y atencion; variables CSS sin definir corregidas. [Ficha](changes/2026-11-01-07-visual-plan-phase1-tokens.md). Fuente sin tests/build/navegador.

> **AFR2-20261101-06 / VIS-049:** Simulador UI en seccion propia, con tarjeta debajo de Protocol Studio y el mismo shell visual, ayuda y pestanas. [Ficha](changes/2026-11-01-06-ui-simulator-workspace.md). Fuente sin tests/build/navegador.

> **AFR2-20261101-05:** reparados simbolos mal codificados de Control tactil, calibracion y encoder; archivos UTF-8. [Ficha](changes/2026-11-01-05-web-text-encoding.md). Solo texto, sin tests/build/navegador.

> **AFR2-20261101-04:** camaras de Simulaciones y Modelos 3D encuadran el modelo cercano y centrado segun sus limites reales, FOV y proporcion del viewport. [Detalle](changes/2026-11-01-04-close-model-framing.md). Fuente sin tests, builds ni navegador.

> **AFR2-20261101-03 / VIS-048:** Modelos 3D usa el cielo/piso de Simulaciones en ambos temas; fichas de auto/brazo/cubo solidas y tipografia/controles con tokens semanticos. [Detalle y limites](changes/2026-11-01-03-models-scene-contrast.md). Solo fuente, sin tests, build, validadores ni navegador.

> **AFR2-20261101-02:** Simulador UI actualizado por cotejo de Afr2Ui_Apply: corto/largo contextual, Hall, Si/No, rapidas y Wi-Fi. Arbol completo y limites en [UI_SCREEN_FLOW](documentation/UI_SCREEN_FLOW.md). Solo fuente; sin tests, builds, generadores ni hardware.

> **AFR2-20261101-01:** corregidos cuatro hashes OLED CRLF a LF para predev Web; guard conservado. [Causa y limites](changes/2026-11-01-01-web-predev-lf-hashes.md). Solo fuente, sin ejecutar npm ni compilar.

> **AFR2-20261029-03, entrega 3D:** [V7 con codo remoto v17](changes/2026-10-29-03-brazo-v7-codo-remoto-v17.md). J1/J4/J5 integrados con motores, housings y fijaciones. Ensambles nativos y limites registrados; correa20/50, tensor, PVC25/20 y pinzaV7 conservados. Nuevos calculos, comprobaciones globales y aligerado diferidos por el usuario. atmega/esp/web/Loader sin cambios.

> **AFR2-20261029-02:** [banco mecanico compacto v15](changes/2026-10-29-02-banco-brazo-compacto-v15.md). CAD nativo separado, eslabones de aluminio y pinza corregida; masas y revision geometrica local. Reductor J2 no aceptado para PLA e integracion global pendiente. atmega/esp/web/Loader sin cambios.

> **AFR2-20261029-01:** [brazo mas compacto y liviano](changes/2026-10-29-01-brazo-compacto-solo-ruedas.md). Solo ruedas, importacion admitida; se descartan pies y voladizo. Predimensionamiento55/50mm:1.17kg sin carga con reserva25%, margen externo2.65; resistencia, frenado, estabilidad real y nuevo CAD pendientes. atmega/esp/web/Loader sin intervencion.

> **AFR2-20261028-09:** [brazo liviano, aluminio y traccion](changes/2026-10-28-09-brazo-liviano-aluminio-traccion.md). Masa del auto1.5kg aproximada integrada; propuesta de eslabones90/75mm y apoyos desplegables, con calculos de masa/CG/cargas/secciones. Candidato preliminar, sin nuevo CAD ni aprobacion fisica. atmega/esp/web/Loader sin cambios.

> **AFR2-20261030-19:** aclaración del táctil: espera descarga/preparación y vista3D lista usando el mismo estado de la barra; luego permite mover con las condiciones de motores de Mecanum. [Ficha](changes/2026-10-30-19-touch-model-ready.md). Conserva reintento tras error; solo fuente sin build/pruebas/carga.

> **AFR2-20261030-18:** táctil y Mecanum con mismo permiso PWM; corregido bloqueo persistente por último error y cancelación por modelo3D. [Condiciones y límites](changes/2026-10-30-18-manual-touch-permissions.md). Solo fuente Web, sin build/pruebas/cargas.

> **AFR2-20261030-18:** selector común de motor030044, pictogramas coherentes del encoder y dashboard con solo modo en el cuerpo. [Ficha y límites](changes/2026-10-30-18-motor-selector-encoder-dashboard.md). Fuente AVR/Web sin compilar, probar ni cargar.

> **AFR2-20261030-17:** botón largo del encoder con bitmap13x13 del usuario, L central integrada y copia Web vinculada. [Ficha](changes/2026-10-30-17-encoder-long-icon.md). Solo fuente; sin build ni carga.

> **AFR2-20261030-16:** tamaño y progreso real de assets 3D con componente reutilizable, caché compartida y ETA de descarga. [Detalle y límites](changes/2026-10-30-16-model-asset-progress.md). Solo fuente; sin build, pruebas, generadores ni promoción.

> **AFR2-20261030-15:** WS, retorno táctil, consulta local de credenciales ESP y backup condicional del Loader adaptados en fuente; EX01v12. [Detalle y límites](changes/2026-10-30-15-connection-touch-credentials-loader.md). Sin compilación, pruebas ni cargas.

> **AFR2-20261030-14 compilado por Loader:** ATmega producto/comunicación/biblioteca, ESP y Web+LittleFS completos; errores TS y advertencias C corregidos. [Evidencia](documentation/builds/2026-10-30-14/README.md). Sin flasheo por estos trabajos; memoria dinámica/hardware pendientes.

> **AFR2-20261030-13:** referencia de avance/retroceso y giro corregida en fuentes AVR/Web, con odometría/secuencias coherentes y etiquetas Girar CW/CCW. [Ficha](changes/2026-10-30-13-motion-reference.md). Laterales y trims individuales conservados; sin generación/build/pruebas/carga.

> **AFR2-20261030-09:** Estado de red, búsqueda/credenciales, modos y desconexión Wi-Fi confirmada en fuentes AVR/ESP/Web; EX01v11. [Ficha y límites](changes/2026-10-30-09-wifi-state-flows.md). Sin generación, compilación, validadores, pruebas, promoción ni cargas.

> **AFR2-20261030-10:** registro de seis fuentes AVR revisadas actualizado tras cotejo con copia TypeScript existente; corrige bloqueo de hashes en predev sin omitir el guard. [Revisión y límites](changes/2026-10-30-10-web-predev-reviewed-sources.md). Sin ejecutar npm, generador, dev, builds, tests ni cargas. AVR/ESP sin cambios.

> **AFR2-20261030-08:** control táctil con pill de conexión del auto y Simulado solo sin enlace Web–ESP–Mega; bloqueos físicos diferenciados. [Ficha y estado](changes/2026-10-30-08-touch-connection-pill.md). Web solo fuente; sin build, navegador, promoción ni carga. AVR/ESP sin cambios.

> **AFR2-20261030-07:** reforma integral OLED/Web y diagnóstico ESP, EX01v10/EEPROMv8. [Ficha y límites](changes/2026-10-30-07-ui-integral.md). Solo fuente; sin generación, builds, pruebas, promoción ni cargas.

> **AFR2-20261030-06:** selector rápido Reposo/Manual en fuente AVR y Web; sin compilación, generador ni pruebas. [Ficha](changes/2026-10-30-06-quick-mode.md).

> **AFR2-20261030-04:** límite PWM predeterminado255 y EEPROMv7: migra el antiguo80 en RAM, preservando otros límites y calibraciones. [Contrato y estado](contracts/EEPROM_PWM_LIMIT_7.md). ATmega solo fuente; ESP/Web sin cambios. Sin compilar, probar ni cargar.

> **AFR2-20261030-03:** cinco IR por ADC10 (0..1023), A4..A0, perfil2 y EEPROMv6 con migración de modos anteriores en RAM. ADC compartido con bateríaA5; descarte por cambio de canal. ESP/Web/OLED ya admiten ese rango. [Contrato y límites](contracts/EEPROM_IR_ADC_6.md). Solo fuente; sin compilación, pruebas ni carga. No acredita la causa de los ceros.

> **AFR2-20261030-02:** cabecera dashboard con SSID e IP STA/AP y scroll independiente de ida/vuelta, recortado entre icono y batería fijos. Solo bandas de texto en redibujo parcial. Fuentes AVR/Web vinculadas, sin compilar ni cargar. Ficha en Contracts/changes/2026-10-30-02-dashboard-network-scroll.md.

> **AFR2-20261030-01:** dashboard010001 adaptado de AutoMicro con iconos Wi-Fi/AP/encoder, batería por bandas de voltaje y accesos corto Menú/largo Rápidas al soltar. AVR y copia raster Web vinculados, sin nuevo wire. Fuente sin compilar, probar ni cargar. Ver changes/2026-10-30-01-dashboard-automicro-icons.md.

> **AFR2-20261029-29:** desde dashboard, corto abre menu completo010002; largo al soltar abre pantallas rapidas010003. Ambos accesos conservan retorno al origen por historial. Sustituye el acceso comun de28. Fuente sin compilar/cargar.

> **AFR2-20261029-28:** dashboard abre menu de controles con clic corto; largo conservado al soltar. OLED/Web vinculados; sin compilacion, validadores, prueba ni carga.

> **AFR2-20261029-27 — fuente vigente:** ATmega/ESP0.11.0 y Web0.4.0; carrusel de controles, largo al soltar con retorno contextual, radar4/2/1/0.5m/rastro/editor compartido, IR compactos, PWM manual independiente de odometría/HC y atención prioritaria de sesión WS. EEPROMv5 / EX01v9, preservando batería26. Sin compilar, validadores, flasheos, pruebas físicas ni promoción Web. Recursos OLED copiados solamente; memoria y aceptación pendientes. [Contrato y límites](contracts/QUICK_RADAR_ACTUATORS_9.md).

> **Fuente AFR2-20261029-26:** Sistema → Batería (A5,10k/10k,104), lectura local/Web y pill con icono por voltaje; reinicios Web con Confirmar y reiniciar/Cancelar. [Contrato9](contracts/BATTERY_MEASUREMENT_9.md), [ficha](changes/2026-10-29-26-battery-system-restart-confirm.md). ATmega/ESP/Web solo fuente, EX01v9 conjunto; sin compilar, generador OLED, promoción, carga ni pruebas.

> **AFR2-20261029-25:** [divisor sin diodo en toma del voltímetro después de la llave](changes/2026-10-29-25-battery-switched-divider.md), corte comunicado por usuario. 10k/10k: 8 V → 4 V; 100 nF entre nodo A5 y GND. Solo documentación, sin implementación ni pruebas. Sustituye la variante con diodo24.

> **AFR2-20261029-24:** [recálculo con diodo de 0,7 V antes del divisor](changes/2026-10-29-24-battery-series-diode.md): 8 V → 3,65 V con 10k/10k. El diodo no resuelve la alimentación por el pin con Mega apagado. Solo documentación, sin implementación ni pruebas.

> **AFR2-20261029-23 — investigación:** [medición de batería por divisor y ADC](hardware/MEDICION_BATERIA.md), A5 candidato, opciones 10k/10k y 15k/10k; propuesta Sistema → Batería y Web. Protección/apagado y batería real pendientes. Solo documentación, sin implementación ni pruebas.

> **AFR2-20261029-22:** corregido en fuente el argumento PowerShell de preparación FileSystem; el perfil instalado puede ajustarse sin recompilar. Sin ejecutar scripts, compilar ni flashear. [Causa y solución](changes/2026-10-29-22-loader-powershell-filesystem.md).

> **AFR2-20261029-19 compilado y ATmega cargado:** pedido humano29/10/2026. ATmega/ESP/Web esp y LittleFS completados; promoción local Web a ESP/data/dist. Carga COM8 final13:32:50,80066bytes escritos/verificados, avrdude exit0. ESP/LittleFS sin cargar. SRAM estática7085bytes; sin prueba física/dinámica. Evidencia en AutoFinalRobo2-Contracts/documentation/builds/2026-10-29-19/README.md. Estados inferiores sin build/carga son históricos.

> **AFR2-20261029-20 compilado SIN cargar:** producto/comunicación/biblioteca Loader exit0 sin errores/advertencias, final29/10/2026 13:00:16. Flash80066bytes; SRAM7085bytes, margen1107bytes, main276 y Ui_Render133bytes según .su individuales; no certifica máximo dinámico de pila/ISR. [Evidencia](documentation/builds/2026-10-29-20/README.md). Sin tests/validadores ni ensayo físico, Web fuente vinculada sin build/promoción; placa conserva15 y el pedido de no flashear sigue vigente. Estado pendiente de compilación inferior histórico.

> **Fuente AFR2-20261029-20:** Hall una vuelta con selección de rueda, conteo manual y clic para terminar; largo cancela con aviso. Drafts por4ruedas, estado CPR con asterisco, guardado asíncrono y diálogo genérico Si/No al salir sin guardar. OLED/Web vinculados en fuente; Web sin build/promoción, recursos generados pendientes del build autorizado. [Ficha](changes/2026-10-29-20-hall-wizard-save-confirm.md). Compilación AVR pendiente al redactar; no flashear, placa conserva15.

> **AFR2-20261029-18 compilado SIN cargar:** diagnóstico BuildOnly exit0 y producto/comunicación/biblioteca Loader exit0, sin errores/advertencias. Diagnóstico flash13800/SRAM3180bytes; producto flash75600/SRAM6934bytes, margen1258bytes sin certificación dinámica de pila. Producto final12:34:40 del29/10/2026. Placa conserva15; no se abrió COM ni se ejecutó ensayo físico. Web solo fuente. Evidencia en Contracts/documentation/builds/2026-10-29-18/README.md; pendientes inferiores históricos. La prohibición de flashear sigue vigente.

> **Fuente AFR2-20261029-18:** IR1..IR5 OSOYOO digital (A4..A0), perfil predeterminado habilitado y EEPROMv4 migra solo tipos antiguos deshabilitados; calibración/seguimiento siguen pendientes. Diagnóstico servo/encoder agrega lectura RAW directa y cambios por OLED/serie, sin EEPROM. Vistas IR/Web distinguen deshabilitado/pendiente/cero válido. [Ficha](changes/2026-10-29-18-ir-input-acquisition.md). Compilaciones pendientes al registrar la fuente; no cargar por instrucción vigente. Placa conserva15; Web sin build/promoción.

> **Diagnóstico17 compilado, sin cargar:** usuario confirma puente S1–D13 instalado además de GND/5V/S1 del servo. BuildOnly exit0, sin errores/advertencias, sin COM/avrdude. Flash13424bytes; SRAM3169bytes, margen5023bytes, sin certificación de pila. HEX afr2-i2c-diagnostic.hex SHA256 03cf3a3760f3b6bc5b9b8a7ce128a56388a5bd51b69209a2ea39e895283ca573. Causa del servo inmóvil aún sin medición/prueba física. Producto16 no cargado; placa conserva15. Las notas de compilación/puente pendientes inferiores son históricas.

> **Diagnóstico AFR2-20261029-17:** encoder controla servo D13/OC1C en pasos5grados; PWM inicialmente OFF, C=centrar, S=apagar PWM. OLED y serie muestran orden/contador/botón; I2C cada10s. S1 ModelY necesita puente adicional aD13 según manual. Script agrega -BuildOnly (sin COM/flash). [Ficha](changes/2026-10-29-17-diagnostic-encoder-servo.md). Fuente lista, compilación de prueba pedida; no se autoriza carga. Producto16 sigue sin cargar; placa conserva15.

> **AFR2-20261029-16 compilado, NO cargado por indicación del usuario:** producto/comunicación/biblioteca exit0, sin errores/advertencias, final29/10/2026 12:15:28. Flash75518bytes; SRAM6918bytes, margen1274bytes; main259 y renderer73bytes según .su individuales, sin certificación dinámica de pila. COM8 conserva15 (cargada12:02:57). [Evidencia](documentation/builds/2026-10-29-16/README.md). El usuario pidió continuar con otros cambios y posponer la carga; no se solicitará flasheo hasta nueva instrucción.

> **Fuente AFR2-20261029-16:** OLED sin esperas artificiales entre etapas, radar con grilla punteada, cono visual nominal15grados por PWM aplicado y discos rellenos por eco válido. Copia Web vinculada; sin build Web/promoción ESP. [Ficha](changes/2026-10-29-16-oled-latency-radar-cone.md). Build/carga AVR COM8 autorizados, pendientes al registrar esta nota. Se conserva corrección del watchdog y migración Hall de15, ya cargada y verificada enCOM8 a12:02:57.

> **Fuente AFR2-20261029-15:** corregido bucle Bresenham del radar que bloqueaba main y podía disparar watchdog al girar desde HC-SR04; productos trigonométricos AVR en32bits y contraparte Web vinculada. Hall conectado por montaje confirmado, migración local EEPROMv3 preserva calibraciones y habilitación de control. [Ficha](changes/2026-10-29-15-radar-watchdog-hall-wiring.md). Build/carga AVR COM8 autorizados, pendientes al registrar esta nota; sin prueba física del cambio.

> **AFR2-20261029-14 compilado y cargado:** 29/10/2026 11:37:12, COM8, Loader1.1.2,74516bytes escritos y verificados, exit0. Producto/comunicación/biblioteca compilados sin errores/advertencias. SRAM estática6889bytes, margen1303bytes; main259bytes según .su individual, sin certificación de máximo dinámico. [Evidencia](documentation/builds/2026-10-29-14/README.md). La entrega sin compilar indicada debajo es histórica; sin ensayo físico posterior de UI.

> **Fuente AFR2-20261029-14:** recuperación OLED sin apagar ante error aislado, conserva páginas confirmadas y acota la postergación por trabajo pendiente. Reconfiguración sin0xAE tras fallos persistentes. Sin nueva compilación, tests, validadores, carga ni prueba física; COM8 conserva11. SRAM/pila y causa del negro permanente pendientes. [Investigación y cambios](changes/2026-10-29-14-oled-fault-recovery.md).

> **Loader 1.1.2 / AFR2-20261029-13:** intervención KEY/RST ESP obligatoria y visible al frente aun con app minimizada/omisión de confirmaciones. Sin cambio en carga ATmega. GUI/MCP compilados y distribución generada; sin ejecución ni hardware. [Ficha](changes/2026-10-29-13-loader-esp-physical-required.md).

> **Loader 1.1.1 / AFR2-20261029-12:** autorización local por sesión para omitir confirmaciones manuales desde GUI/MCP, con advertencia obligatoria y comprobaciones automáticas conservadas. GUI/MCP compilados y distribución generada; sin ejecución ni hardware. [Ficha](changes/2026-10-29-12-loader-session-checks.md).

> **AFR2-20261029-11 cargado:** compilación y carga ATmega enCOM8 completadas,
>74200bytes de flash verificados, exit0. SRAM estática6885bytes; ejecución/pila
> y UI física posteriores pendientes. [Evidencia](documentation/builds/2026-10-29-11/README.md).

> **Fuente AFR2-20261029-11:** OLED diferencial exacto y cooperativo en tramos
> de hasta16columnas; sin refresco completo periódico. Sin compilar/cargar;
> memoria/pila pendientes. [Ficha](changes/2026-10-29-11-oled-delta-cooperative.md).

> **Build producto10, 29/10/2026:** firmware ATmega recompilado por pedido
> expreso, exit0 sin advertencias; HEX actualizado. Sin COM/carga/prueba física.
> [Evidencia](changes/2026-10-29-10-product-oled-encoder-verified-setup.md).

> **AFR2-20261029-10:** configuración OLED/encoder confirmada en diagnóstico09
> aplicada al producto en fuente; migración UI EEPROMv2 conserva calibraciones.
> Sin nuevo build/carga. [Ficha](changes/2026-10-29-10-product-oled-encoder-verified-setup.md).

> **AFR2-20261029-09:** solo diagnóstico: giro invertido, OLED intercalado
> con escáner, serie sin esperas por carácter y refresco automático de imagen.
> Fuente sin compilar/cargar/probar. [Ficha](changes/2026-10-29-09-diagnostic-direction-responsive.md).

> **AFR2-20261029-08:** solo firmware de prueba: contador con encoder y estado
> PRESIONADO/SUELTO en OLED180grados. Fuente sin compilar/cargar/probar;
> producto/Web sin cambios. [Ficha](changes/2026-10-29-08-diagnostic-counter-button.md).

> **AFR2-20261029-07:** diagnóstico OLED0x3C activo con barras móviles180grados,
> encoder y pulls internos como Wire solo para este servicio. Fuente sin
> compilar/cargar/probar. [Ficha](changes/2026-10-29-07-diagnostic-active-oled.md).

> **Loader 1.1.0 / AFR2-20261029-01:** preparación de FileSystem mediante script configurable; nuestra Web también usa ese lanzador. Incluye ejemplo sin Web y guía de integración. Fuente/documentación sin compilar ni ejecutar. [Ficha](changes/2026-10-29-01-loader-filesystem-scripts.md).

> **Loader AFR2-20261028-23 / 0.9.5:** preparación previa por sección, FileSystem/capacidad confirmada, verificación ESP integrada sin reconexión adicional, aviso KEY al primer porcentaje, prompts de Servidor MCP y manual/cableado/UNER renovados. Interfaz compilada y revisada visualmente; sin COM, flash ni compilación de firmware por el agente. [Ficha y límites](changes/2026-10-28-23-loader-flujo-manual-mcp.md).

> **AFR2-20261028-22:** diagnóstico solo ante cambios; barrido I2C cada10s
> entre inicios y encoder sin impresión en reposo. Fuente sin compilar/cargar.
> [Ficha](changes/2026-10-28-22-diagnostic-changes-only.md).

> **AFR2-20261028-21:** script PowerShell para compilar y cargar el diagnóstico
> OLED/encoder con puerto COM configurable. Archivo preparado, sin ejecutar ni
> generar HEX/cargar placas. [Ficha](changes/2026-10-28-21-diagnostic-powershell-flasher.md).

> **AFR2-20261028-20:** diagnóstico ampliado con encoder D2/D3 y SW D42,
> sentidos A(+)/B(-), corto/largo y errores de captura por USART0. Solo fuente,
> sin compilar/probar/cargar. [Ficha](changes/2026-10-28-20-encoder-uart0-diagnostic.md).

> **AFR2-20261028-19:** diagnóstico I2C OLED D20/D21 por USART0 D0/D1,
> 115200 8N1, ACK/NACK, estado del bus y resumen. Fuente de servicio separada,
> sin compilar/probar/cargar. [Guía](hardware/OLED_I2C_DIAGNOSTICO.md) · [Ficha](changes/2026-10-28-19-oled-i2c-uart0-diagnostic.md).

> **Loader AFR2-20261028-17 / 0.9.5:** KEY corregido, botones y desplegables con contraste y separación propios, etapas sin scroll por operación, flasher visible y distribución revisada con capturas. Loader compilado y GUI revisada por pedido expreso; sin COM, carga ni firmware compilado. [Detalle y límites](changes/2026-10-28-17-loader-095-revision-visual.md).

> **Loader AFR2-20261028-16 / 0.9.5:** flujo ATmega/ESP, Configuración por placa, flasher separado, Preferencias en Opciones, tablas MCP/Eventos, colores/iconos y espaciado compartidos. Monitor con envío ASCII/HEX/números, CR/LF y repeticiones cancelables por el mismo dueño del COM, con registros TX separados. [Ficha y límites](changes/2026-10-28-16-loader-095-interfaz-envio-serie.md). Solo fuente, revisión manual: sin tests, validadores, compilación, GUI, COM ni flasheos.

> **Loader AFR2-20261028-15 / 0.9.4:** selección directa del BIN completo Ai-Thinker AT v1.5.4.1 descargado: reconocido por tamaño y SHA-256, mapa completo 4 MiB desde 0x0, DIO/40 MHz del archivo. Otros BIN requieren manifiesto del proveedor. Conserva consulta física, confirmaciones y verificación. Solo fuente, sin compilar, ejecutar ni flashear. [Ficha](changes/2026-10-28-15-loader-at-bin-directo.md).

> **Loader AFR2-20261028-13 / 0.9.3:** [Feedback de carga](changes/2026-10-28-13-loader-feedback-carga.md): tiempo del proceso, último mensaje, segundos sin salida y porcentaje de escritura informado por esptool. Detección por flasher reportada por el usuario; escritura y nueva interfaz pendientes de comprobar. Solo fuente; sin compilar, ejecutar ni abrir COM.

> **Loader AFR2-20261028-12 / 0.9.2:** [Conversor universal HEX / ASCII](changes/2026-10-28-12-loader-conversor-universal.md) para texto pegado, con vista legible/escapada, ASCII literal a HEX y copia normalizada. Sin asociación a comandos, COM ni jobs. Solo fuente; sin compilar/ejecutar/probar. Web/atmega/esp/CAD sin cambios de esta ficha.

> **Integración AFR2-20261028-11 / Loader0.9.1:** se incorporó origin/main2a72b9a (Loader0.9.0, servicio AVR y cambios CAD) conservando monitor HEX, USB–TTL, verificación y Protocol Studio locales. [Resoluciones y resguardo](changes/2026-10-28-11-integracion-local-remoto.md). Protocol Studio reasignado a09/10 para evitar IDs remotos repetidos. Solo fuente y Git; sin compilar, probar, validar, COM, flash ni promoción ESP.

> **AFR2-20261028-08:** [busqueda de actuadores a30grados/s e inercia](changes/2026-10-28-08-actuadores-30gs-inercia-vuelco.md). No se acepta bajar la velocidad para cerrar el objetivo. RS01 pasa un filtro nominal condicionado, con masa/estructura/apoyo pendientes. Sin cambios de producto en firmware, Web o Loader ni nuevo CAD.

> **AFR2-20261028-07:** [busqueda simultanea de motores por par, masa y resistencia](changes/2026-10-28-07-busqueda-actuadores-par-masa.md). DS51150 descartado para frenada desde catalogo; STS multivuelta con reductores pasa filtro nominal condicionado, sin seleccion final ni nuevo CAD. atmega/esp/web/Loader sin intervencion.

> **Loader AFR2-20261028-05 / 0.9.0:** backup opcional leído del equipo, aviso de omisión con No mostrar de nuevo y Preferencias, botón de firmware ATmega con último backup en desplegable. [Ficha](changes/2026-10-28-05-loader-mega-backups.md) · [Plan A/B](hardware/PLAN_PRUEBA_FLASHER_MEGA.md). Solo fuente/documentación; sin compilar, ejecutar, validar, abrir COM ni flashear. atmega/esp/web sin cambios de firmware por05. Reemplaza el mecanismo de copia HEX de03.

> **Loader AFR2-20261028-03 / 0.8.0:** instalar/restaurar flasher ATmega, selector **Método ESP** con instrucciones y casillas por conexión. [Plan A/B](hardware/PLAN_PRUEBA_FLASHER_MEGA.md), [ficha](changes/2026-10-28-03-loader-mega-flasher.md). Código y documentación; sin compilar, ejecutar, abrir COM ni flashear. atmega de servicio en fuente; esp/web sin cambios.

> **Servicio AFR2-20261028-02:** [ESP por puente de software en Mega](hardware/MEGA_ESP_PUENTE_SOFTWARE.md), RESET libre, D18/D19 y baud fijo 115200 candidato. Fuente de servicio AVR aislada y objetivo Make separados del producto; sin compilar/cargar/probar, sin cambio de firmware ESP/Web. Integración al Loader pendiente en02, incorporada en fuente03. [Ficha](changes/2026-10-28-02-mega-esp-software-bridge.md).

> **Web AFR2-20261028-10:** [Esencia de AutoMicro en Protocol Studio](changes/2026-10-28-10-protocol-studio-essence.md): formularios, estructura visual, salidas simultáneas, mapa de captura y ayuda, con identidad AFR2. [Cotejo en fuente](docs/PROTOCOL_STUDIO_AUTOMICRO.md). Sin compilación, tests, validadores ni navegador; atmega/esp y paquete ESP sin cambios.

> **Web AFR2-20261028-09:** [Protocol Studio desde Desarrollador](changes/2026-10-28-09-web-protocol-studio.md), traductor de capturas HEX/texto y constructor ROBO2 offline. [Contrato y límites](contracts/WEB_PROTOCOL_STUDIO.md). Fuente sin compilar, probar, validar ni observar en navegador; ATmega/ESP y paquete ESP sin cambios de esta ficha.

> **Loader AFR2-20261027-30 / 0.7.4:** [Monitor serie ASCII / HEX con espacios](changes/2026-10-27-30-loader-serial-hex-ascii.md), selector en vivo con bytes originales y reconversión de la ventana retenida, sin reabrir COM. Fuente sin compilar/ejecutar esta revisión; atmega/esp/web/CAD sin cambios por esta ficha.

> **Loader AFR2-20261027-29 / 0.7.3:** [comprobar cargador antes de verificar](changes/2026-10-27-29-loader-verification-bootloader-gate.md). Consulta nueva y, sin respuesta confirmada, pausa y aviso/asistente KEY/RST; después continúa solo la verificación. Fuente sin compilar/ejecutar esta revisión; atmega/esp/web/CAD sin cambios por esta ficha.

> **Loader AFR2-20261027-28 / 0.7.2:** [botón Limpiar ventana en Operación](changes/2026-10-27-28-loader-clear-operation-view.md), disponible durante trabajos; vacía solo consola/cola de visualización y conserva logs e historial. Fuente sin compilar/ejecutar esta revisión; atmega/esp/web/CAD sin cambios por esta ficha.

> **Loader AFR2-20261027-27 / 0.7.1:** [comando ESP y KEY sostenido](changes/2026-10-27-27-loader-esp-command-key.md). Python `-u`, conexión USB–TTL/puente Mega configurable y KEY hasta terminar escritura/verificación. Fuente sin compilar/ejecutar esta revisión. El usuario aportó escritura manual COM5/115200 de firmware con hash verificado; no valida el Loader corregido, arranque, Wi-Fi ni LittleFS. atmega/web sin cambios por esta ficha.

> **Loader AFR2-20261027-24 / 0.7.0:** [alta y prueba del servidor MCP](changes/2026-10-27-24-loader-mcp-connection-test.md), estado conectado con evidencia por sesión y autoaceptación visible desde el inicio. Solo fuente/documentación; sin compilar, ejecutar GUI/MCP, tests, validadores, COM ni flasheos de esta revisión. atmega/esp/web sin cambios por esta ficha.

> **Loader AFR2-20261027-23 / 0.6.1:** [Paquete AT visible solo al seleccionar AT](changes/2026-10-27-23-loader-at-package-visibility.md), oculto en Web propio. Fuente y documentación; sin compilar, ejecutar GUI/MCP, tests, validadores, COM ni flasheos de esta revisión. atmega/esp/web sin cambios por esta ficha.

> **Loader AFR2-20261027-22 / 0.6.0:** selección Web/AT por USB, filesystem/tamaños y resumen de memoria; reglas separadas de linker y paquete AT, verificación multi-BIN y lanzador con perfil/familia. [Ficha](changes/2026-10-27-22-loader-web-at-usb-layouts.md), [contrato](contracts/LOADER_FLASH_MODES.md). Solo fuente/documentación; sin compilar, ejecutar GUI/MCP, tests, validadores, COM ni flasheos de esta revisión. Firmware atmega/esp y Web sin cambios por esta ficha.

> **Loader AFR2-20261027-21 / 0.5.0:** autoaceptación MCP opcional por sesión, historial Ctrl/Shift y copia con toast, selector de perfiles locales, aviso del monitor alineado y asistentes sin texto seleccionado. [Ficha](changes/2026-10-27-21-loader-autoaccept-profiles-history.md), [MCP](../AutoFinalRobo2-Loader/MCP.md). Solo fuente/documentación; sin compilar, ejecutar GUI/MCP, tests, validadores, COM ni flasheos de esta revisión. atmega/esp/web sin cambios de esta ficha.

> **Loader AFR2-20261027-20 / 0.4.0:** barra de estado, etapas laterales, scroll con posición conservada, dropdown de compilación y destino de flasheo explícito, detección ATmega/ESP junto a Destino y guía K/RST con aviso de puerto listo. [Ficha](changes/2026-10-27-20-loader-workflow-scroll.md), [manual](../AutoFinalRobo2-Loader/MANUAL.html#resultado). Solo fuente/documentación; sin compilación, ejecución, tests, validadores, COM ni flasheos. Firmware/Web sin cambios de esta ficha.

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

> **Diagnóstico ESP AFR2-20261027-25:** [consultas USB–TTL COM5 aportadas por el usuario](hardware/diagnostics/20261027-com5-usbttl-user-report/report.md), ESP8266EX y flash **4 MiB**, IDs `5e / 4016`. Comunicación bidireccional observada; sin carga ni verificación de contenido. Layout/SPI y causa del fallo COM8 por Mega/selector pendientes. [Ficha](changes/2026-10-27-25-esp-usbttl-identification.md). Firmware/Web/Loader sin cambios de esta ficha.

> **Brazo AFR2-20261028-01:** [comprobaciones de J1 y uniones](../AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/analisis_J1_uniones/README.md), [ficha](changes/2026-10-28-01-j1-joint-strength-study.md). Secciones/barras/fijaciones y estudio temporal de espacio, sin modificar v08. Acople D, postes/nervios y contacto al chasis requieren rediseño; material/perfil aún indefinidos. Sin FEA de sólidos/contactos ni capacidad física aprobada; demás módulos sin cambios.

> **Brazo AFR2-20261027-21:** [housing GX3270BLS y revisión CAD v08](../AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/revision_v08/README.md), [ficha](changes/2026-10-27-21-arm-housing-rechecked.md). Interferencias nuevas corregidas, 3.141 configuraciones sin penetraciones reportables y ejes/apoyos completos; masas y cargas recalculadas. Sin FEA ni aceptación física de carga/frenada. Atmega/ESP/Web/Loader sin cambios.

> **Brazo AFR2-20261027-20:** [v07 implementada: GX3270BLS en el codo y refuerzos locales](../AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/revision_v07/README.md). [Ficha](changes/2026-10-27-20-arm-gx3270bls-reinforcement.md) y [cargas actualizadas](../AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/analisis_cargas_200g/07_implementacion_y_cargas_v07.md). F3D completo/motor independiente, parámetros y cálculo estático/dinámico bajo hipótesis. J1, apoyos y alimentación siguen abiertos; sin barridos CAD, FEA ni capacidad física acreditada. Atmega/ESP/Web/Loader sin cambios.

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

- **AFR2-20261101-05:** [pantalla QR de contenido elegido](.agents/skills/oled128-ui-design/references/qr.md),
  generador local y bitmaps62×62/128×64. Ejemplo creado; escaneo físico pendiente,
  sin integrar en firmware/Web ni ejecutar tests/validadores.

- **AFR2-20261101-02:** [componentes OLED128-2](.agents/skills/oled128-ui-design/references/components.md)
  y [atlas](.agents/skills/oled128-ui-design/assets/component-atlas.png).
  Cinco/ocho sensores simultáneos desde referencia AutoMicro, barras y gestos
  consistentes. Solo diseño/skill; navegación de firmware/Web todavía sin migrar.

- **AFR2-20261101-01:** [skill de diseño OLED128](.agents/skills/oled128-ui-design/SKILL.md).
  Guía de iconos, plantillas y flujo para interfaces consistentes entre proyectos;
  borrador documentado, sin migrar pantallas ni ejecutar validaciones.

- **AFR2-20261031-01:** [Simulador UI del ATmega en Desarrollador](changes/2026-10-31-01-developer-ui-simulator.md).
  Fuente Web con los tres controles de la vista en vivo y servicios ficticios;
  sin tests, compilacion, validaciones ni promocion ESP. atmega/esp sin cambios.

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








## AFR2-20261029-19 — OLED táctil y visor bajo demanda

Fuente adaptada con Web/ATmega/ESP: overlay scrim+blur en modales, Volver por ítem del menú, ScreenCode0x030035 con aviso/bitmap/chevrones y suscripción explícita a capturas/metadatos. Contrato en AutoFinalRobo2-Contracts/contracts/TOUCH_OLED_19.md; procedencia por archivo en provenance/touch-oled-20261029-19.json de Contracts. El rumbo es solicitado, no medido, y no agrega giro físico. Sin tests, compilación, validadores, navegador, ejecución del generador OLED, promoción ESP ni flasheo de esta entrega. Memoria/paridad/latencia físicas pendientes.

## Aplicación visual global — AFR2-20261102-15

Manual 0.1.11 autorizado para presentación de toda la Web. Cabeceras, tarjetas, controles, campos, navegación y paneles con lenguaje de Infraestructura; comportamiento conservado. Solo fuente, ejecución No verificada; sin tests/build/validadores/navegador ni promoción. [Ficha](changes/2026-11-02-15-web-infrastructure-visual-language.md).
