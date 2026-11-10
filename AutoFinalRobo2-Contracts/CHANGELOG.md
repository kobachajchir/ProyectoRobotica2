## AFR2-20261110-08 — 10/11/2026

Desarrollador: nueva tarjeta del repositorio con QR SVG local y enlace directo a GitHub, siguiendo las tarjetas, tokens y temas existentes. Disposición adaptable a móvil, foco visible y código negro/blanco sin servicios externos ni dependencias nuevas. [Ficha](changes/2026-11-10-08-developer-repository-qr.md).
Versión: 1.16.0 (subversión). Web fuente; ATmega/ESP solo versión compartida. Sin tests, compilaciones, validaciones, navegador, escaneo ni cargas.

## AFR2-20261110-07 — 10/11/2026

Paquete común [Revision/R01_20261110](../Revision/R01_20261110/README.md): informe D1 editable, planos completos y seis tomos, fichas, fuente CAD y pinout actualizado con antecedentes identificados. Índice, observaciones, historial y procedencia para continuar con R02/R03 hasta el informe final. Originales conservados, sin editar contenido técnico ni regenerar PDF. [Ficha](changes/2026-11-10-07-paquete-revision-informe-planos-pinout.md).
Solo organización documental; atmega/esp/web/Loader sin cambios ni versión nueva. Sin tests, compilaciones, validaciones o flasheos.

## AFR2-20261109-13 — 09/11/2026

Brazo v17: 224 hojas A3 en seis tomos, con 66 definiciones fabricadas/adaptadas, 30 impresas, seis conjuntos, cortes, interfaces, BOM y despieces desde la copia CAD real. Primer borrador independiente del informe: 32 páginas, PDF/DOCX y tres partes, con cinemática de brazo y base Mecanum, modelo integrado y resultados históricos. [Ficha](changes/2026-11-09-13-planos-v17-informe-borrador.md).
Estado: documentación nominal generada; ajustes/proceso de fabricación pendientes. ATmega/ESP/Web/Loader sin cambios ni versión nueva. Sin tests, compilaciones, validaciones, pruebas físicas o flasheos.

## AFR2-20261109-03 — 09/11/2026

Fuentes coordinadas y recursos OLED/3D regenerados. PWM exige habilitación explícita y respeta fallos/tareas; slot ESP fluido conservado. OFF/estado desconocido Wi-Fi y simulador corregidos.
Versión: 1.9.1 (subversión mínima).
Estado: atmega/esp/web=fuentes editadas y recursos regenerados; sin compilación, tests, validadores independientes ni carga. Usuario compila y sube. [Ficha](changes/2026-11-09-03-source-completion.md).

## AFR2-20261109-02 — 09/11/2026

AP manual/guardado sin STA ni búsquedas. Preguntar por AP es una política persistente independiente del modo; desmarcada inicia AP automáticamente al fallar STA. AP activo ofrece Detener AP. OLED/Web y bitmaps exactos vinculados; contrato WIFI_AP_POLICY_20261109.md.
Versión: 1.9.0 (subversión).
Estado: atmega/esp/web=fuente editada, sin compilar ni cargar; sin tests, validadores, generadores ni pruebas físicas.

## AFR2-20261109-01 - 09/11/2026

Radar y Ángulo del servo piden ecos mientras están abiertos (dejaron de actualizar en 1.5.0); valor `--` si el eco tiene más de 1 s; Distancia muestra hacia dónde apunta el servo; el servo arranca centrado en 90°, así el cono se ve desde el inicio. Wi-Fi: el ESP cambia de modo aunque haya una búsqueda sin leer o no haya red guardada para ese modo (usa el AP abierto o la red por defecto); si nadie responde «Sin red Wi-Fi» en 20 s levanta el AP; el OLED avisa «Cambiando Wi-Fi», «Iniciando AP» y «Reconectando a <red>». Ficha changes/2026-11-09-01-radar-servo-wifi-modos.md.
Versión: 1.8.0 (subversión).
Estado: atmega=compilado (flash 57,5 %, SRAM 85,7 %), sin cargar; esp=compilado (RAM 62,1 %, flash 43,7 %), sin flashear; web=fuente editada, sin build; loader=sin cambios.

## AFR2-20261108-14 - 08/11/2026

Elegir STA, AP o AP+STA (o Desconectar) en el OLED ya no responde «Detener / ver red»: detiene el movimiento si hace falta y reemplaza el trabajo Wi-Fi pendiente por el cambio de modo. El ESP descarta solo la respuesta del trabajo viejo y toma el nuevo en el siguiente WIFI_POLL. Ficha changes/2026-11-08-14-wifi-modo-sin-bloqueo.md.
Versión: 1.7.1 (subversión mínima).
Estado: atmega=fuente editada, chequeo de sintaxis sin errores, sin HEX ni carga; esp=solo versión; web=solo versión.

## AFR2-20261108-13 - 08/11/2026

Loader 2.4.0: botón «Reinicios (n)» en el Monitor serie con vista propia: razón del reset, sesión nueva (HELLO, ms desde el arranque), enlace perdido antes, excepción, epc1 y línea de tiempo por reinicio. ATmega/Web: la página Línea de Controles rápidos enciende la lectura IR sin motores (acción 108; IR apagado por defecto, pantallas de línea lo leen mientras están abiertas, seguir línea lo enciende); el radar enciende servo y HC-SR04, y apagar cualquiera de los dos lo detiene (restaura el HC-SR04 al parar). Ficha changes/2026-11-08-13-reinicios-ir-radar.md.
Versión: 1.7.0 (subversión). Loader 2.4.0.
Estado: atmega=compilado sin errores (flash 57,2 %, SRAM 7016 B / 85,6 %), sin cargar; web=fuente editada, sin build; esp=solo versión; loader=2.4.0 compilado e iniciado, sin abrir puertos.

## AFR2-20261108-12 - 08/11/2026

Controles rápidos en el orden Modo, Motores, Ultrasonido, Radar, Servo, Línea, Rastro, Ajustes (abre en Modo; `quick_flags` con bit = página). La tarjeta del dashboard recorre siempre los seis sistemas con ícono y estado (Motores OFF/ON/RUN, US, Radar, Servo, IR, RS), una página cada 4 s tras 3 s sin tocar, salta al que cambia y FALLO tiene prioridad. ATmega y Web iguales. Ficha changes/2026-11-08-12-rapidas-orden-tarjeta-estados.md.
Versión: 1.6.0 (subversión).
Estado: atmega=fuente editada, sin compilar ni cargar; web=fuente editada, sin build; esp=solo versión; loader=sin cambios.

## AFR2-20261108-11 - 08/11/2026

Loader 2.3.0: la pestaña Resets ESP pasa a «Arranques ESP / ATmega», siempre activa con el puerto abierto: una fila por arranque de cada chip (AVR BOOT con bits MCUSR, ESP BOOT, volcado de excepción, ESP ROM probable, trama BOOT), graves en rojo, detalle con las 30 líneas previas y hasta 20 posteriores, Copiar detalle, y aviso en Eventos (salvo el auto-reset del Mega al abrir el puerto). La ficha explica por qué el ESP manda cada comando (EXT_STATE 500 ms, DIAG_POLL y WIFI_POLL 400 ms, WIFI_UPDATE 2 s, NETWORK_STATE 5 páginas, LIMITS 3 s, US_GET propio 1 s + Web). Ficha changes/2026-11-08-11-loader-arranques.md.
Versión: sin cambio de producto. Loader 2.3.0.
Estado: loader=compilado en carpeta temporal, sin abrir puertos; atmega/esp/web=sin cambios.

## AFR2-20261108-10 - 08/11/2026

El HC-SR04 deja de medir por defecto: solo con el nuevo interruptor «Ultrasonido» de Controles rápidos (8 páginas; MIDIENDO/APAGADO, acción 107, apagado tras reset), con el radar en marcha o con la pantalla Distancia numérica abierta. Vuelve el ícono de pulsación larga junto a «Rapidas» en el dashboard (11 px, sin tocar la esquina del anillo). TAP: log y modos de un sentido a 115200; enlace completo pasa a 250000 tras confirmar (500 ms de silencio) y vuelve al salir; el Loader 2.2.1 cambia la velocidad del mismo COM sin reabrirlo y, si a 250000 no recibe nada válido en 7 s, vuelve a 115200. DIAG_POLL y WIFI_POLL: cada 400 ms (sin cambios). Ficha changes/2026-11-08-10-ultrasonido-rapidas-tap-baudios.md.
Versión: 1.5.0 (subversión). Loader 2.2.1.
Estado: atmega=compilado sin errores (flash 57,2 %, SRAM 85,6 %), sin cargar; web=fuente editada, sin build; esp=solo versión; loader=2.2.1 compilado e iniciado, sin abrir puertos.

## AFR2-20261108-09 - 08/11/2026

Loader como depurador USB con el firmware del auto. Catálogo `contracts/wire_catalog.json` (53 comandos, campos y largos cotejados con el ATmega) y skill `afr2-wire-commands`: todo ABM de un CMD lo actualiza. El ATmega (`AFR2_DEBUG_TAP=1`, log a 250000) elige por una línea `#AFR2 TAP n` desde el USB qué copiar: log, tramas ESP→ATmega, ATmega→ESP, ambas o bytes crudos, como registros TAP1 con ms y redacción de Wi-Fi/clave del AP; avisa `ESP ROM probable` y cuenta los reinicios del ESP. Loader 2.2.0: Ver intérprete ↔ Enviar, intérprete desactivado por defecto y habilitado con catálogo, selector Fuente USB (limpia log, conserva catálogo), tramas decodificadas con hora y origen, contadores por comando y pestaña Resets ESP. Protocol Studio sincronizado (EXT_STATE 19 B y 5 comandos que faltaban). BT02 (EX01 v14) queda pendiente de decisión. Plan docs/PLAN_LOADER_INTERPRETE_Y_TAP.md, ficha changes/2026-11-08-09-loader-interprete-tap.md.
Versión: 1.4.0 (subversión). Loader 2.2.0.
Estado: atmega=fuente editada, sin compilar ni cargar; esp=solo versión, sin compilar; web=fuente editada, sin build ni navegador; loader=fuente sin compilar.

## AFR2-20261108-08 - 08/11/2026

El flasher del ATmega usa la OLED por I2C: en reposo muestra «AFR2» / «Loader firmware» / la versión, y al cargar el ESP el título de la operación, qué imagen se sube (Firmware ESP, Web LittleFS o la región AT), la fase, una barra de progreso y bloques/KB, y al final si esptool terminó con o sin error. El progreso sale de observar el protocolo de esptool sin tocar bytes; las dos UART pasan a interrupciones para que dibujar no frene la carga. El Loader (2.1.0) manda los nombres y el resultado solo si el flasher lo anuncia. Ficha changes/2026-11-08-08-flasher-oled.md.
Versión: 1.3.0 (subversión). Loader 2.1.0.
Estado: atmega=flasher reescrito, sin compilar ni cargar; loader=fuente editada, sin compilar; esp=sin cambios; web=solo versión.

## AFR2-20261108-07 - 08/11/2026

Pantalla de inicio de 3 s (0x010004): barra superior de tiempo como la de los avisos, «AFR2» grande que aparece letra por letra, «ATmega» y «Firmware 1.2.0» debajo; corto o largo la saltean. Política de versiones en docs/VERSIONADO.md: una versión de producto única para ATmega, ESP y Web; versión (MAYOR) si rompe compatibilidad, subversión (MENOR) por función nueva, subversión mínima (PARCHE) por corrección; documentación, CAD y Loader no la suben. Ficha changes/2026-11-08-07-pantalla-inicio-versionado.md.
Versión: 1.2.0 (subversión).
Estado: atmega=fuente editada, sin compilar ni cargar; esp=solo versión, sin compilar; web=fuente editada, sin build ni navegador.

## AFR2-20261108-06 - 08/11/2026

Si la red Wi-Fi elegida no conecta, el ESP ya no levanta el AP solo: el auto muestra «Wi-Fi no encontrado» (aviso 0x04000C, con el motivo) y luego «Sin red Wi-Fi» (0x03004B, en Wi-Fi > AP / fallback) con «Iniciar AP» (defecto, a los 5 s sin tocar) y «Seguir esperando». Girar o apretar detiene la cuenta y quita la barra; el largo responde Seguir esperando. Iniciar AP muestra el aviso «Red AP creada» de siempre; Seguir esperando vuelve a preguntar tras Preferencias > «Reintento Wi-Fi s» (10–300 s, defecto 30, EEPROM v11). Sin OLED o con un ATmega anterior, AP automático como antes. Ficha changes/2026-11-08-06-wifi-sin-red-selector.md.
Versión: 1.1.0 (subversión).
Estado: atmega=fuente editada, sin compilar ni cargar; esp=fuente editada, sin compilar ni flashear; web=fuente editada, sin build ni biblioteca regenerada.

## AFR2-20261108-04 - 08/11/2026

La Web propone la prueba de odometría (dirección y distancia, desde Control > Calibración) y el auto abre una pantalla de confirmación: clic del encoder inicia y abre «Prueba en curso», pulsación larga descarta. La Web consulta el resultado (lanzada, descartada, no se pudo lanzar, vencida a los 60 s). Sin trama nueva: usa `robot.calibrate` acción 102 y `robot.config.get` acción 103 sobre EX01 CONFIG; el ESP solo amplía el rango de acciones a 103. Ficha changes/2026-11-08-04-web-odometria-propuesta.md.
Estado: atmega=fuente editada, sin compilar ni cargar; esp=fuente editada (rango de acciones), sin compilar ni flashear; web=fuente editada, sin build ni navegador.

## AFR2-20261108-03 - 08/11/2026

Nueva pantalla «Prueba en curso» (0x030049): al tocar Iniciar en Calibración > Odometría muestra la odometría actual, el objetivo y la distancia que falta; al llegar sale sola, vuelve a Odometría y avisa «Llegó al objetivo» con la pose final (o «No llegó» / «Interrumpido» si falla). Clic o largo detienen la prueba y vuelven. Ficha changes/2026-11-08-03-odometria-en-curso.md.
Estado: atmega=fuente editada, sin compilar ni cargar; web=fuente editada, sin build ni navegador; esp=sin cambios.

## AFR2-20261108-02 - 08/11/2026

Tras probar el 08/11: Sí/No que ahora gira (el nodo contaba 1 fila y el render reajustaba la selección), radar que grafica con el servo apagado, ícono de pulsación larga con su L restaurada y salida del QR reforzada (sin reproducir). Pruebas sin «Hall una vuelta» (solo en Calibración); Calibración > Odometría pasa a ser una lista con Dirección (8), Distancia (5-100 cm) e Iniciar, que ejecuta un GOTO relativo. Pulsos por vuelta de fábrica 370 en las 4 ruedas. Versión 1.0.0 en ATmega, ESP y Web. Listado de trabajo de GoTo en docs/PLAN_GOTO_ARCOS_Y_ENVIO.md. Ficha changes/2026-11-08-02-menus-odometria-v1.md.
Estado: atmega=fuente editada, sin compilar ni cargar; esp=solo versión, sin compilar; web=fuente editada, sin build ni navegador.

## AFR2-20261108-01 - 08/11/2026

Librería `afr2_usart` (AVR USART0..3: handles, eventos diferidos, inicializador, ISR enlazada por puerto) y log de depuración del ATmega por USART0/D1 a 115200: arranque AVR/ESP con causa de reinicio, volcado de excepción del ESP capturado del enlace, sesión/pérdida/fallas/rechazos del enlace, eventos de red, captura OLED, encoder, loop lento y pila mínima. Enlace ESP en USART1 sin cambios de protocolo. Ficha changes/2026-11-08-01-usart-library-debug-log.md.
Estado: atmega=fuente editada, sin compilar ni cargar; esp/web=sin cambios.

## AFR2-20261107-53 - 07/11/2026

ESP: se reiniciaba al descargar el modelo 3D. Con la red por defecto en RAM (sin `wifi.json`) o si fallaba la escritura, `WifiService::tick` abría LittleFS en cada vuelta del loop para marcar `staGood`. Eso chocaba con las lecturas del servidor HTTP asincrónico. Ahora se intenta una sola vez por arranque y solo con la configuración guardada. Ficha changes/2026-11-07-53-esp-littlefs-loop-reset.md.
Estado: esp=fuente corregida, sin compilar ni cargar; atmega/web=sin cambios.

## AFR2-20261107-52 - 07/11/2026

Hall de una vuelta: tolera la holgura y solo invalida si la cuenta retrocede respecto del máximo más de max(8, máximo/32) pulsos; antes bastaba un pulso. Prueba de encoder con «Clicks: N», «Giro: N» desde 0 y largo «Pulse» → «Correcto» → salir. Ícono de pulsación larga centrado. Web: zoom en el árbol de pantallas y páginas por giro bajo el mismo padre; márgenes de Infraestructura y tarjeta del paquete; foto de los modelos fuera del texto en pantallas chicas; captura del Loader en WebP en su banner. Ficha changes/2026-11-07-52-hall-tolerance-ui-polish.md.
Estado: atmega=fuente editada, sin compilar ni cargar; web=fuente editada, revisada en Vite, sin build; esp=sin cambios.

## AFR2-20261107-51 - 07/11/2026

La prueba OLED/I2C no encontró dispositivos en el bus (`found=0 nack=112`, SDA/SCL en alto). La causa era un conector flojo: el usuario lo ajustó y la OLED volvió a funcionar. Se volvió a cargar el firmware del auto con el Loader 2.0.0 por MCP (`flash_mega`, COM16), con aprobación manual: firma, escritura y verificación correctas. El HEX cargado tiene SHA-256 16826727…7ca0a4, y ese es el hash válido en lugar del anotado en AFR2-20261107-49. Red STA por defecto «Koba» en `wifi_defaults.local.h`, archivo ignorado por Git y sin la clave en Contracts. Ficha changes/2026-11-07-51-mega-restore-loader-wifi.md.
Estado: atmega=cargado y verificado, sin prueba física completa; esp=red local configurada, sin compilar ni cargar; web=sin cambios.

## AFR2-20261107-50 - 07/11/2026

Compilación de la prueba de servicio OLED/I2C (`service/i2c_uart0_diagnostic.c`, versión AFR2-20261029-18) para descartar hardware o cableado: búsqueda I2C, OLED 0x3C, encoder, IR y servo, con salida serie a 115200. Sin errores ni avisos. Flash 14 554 B (5,6 %), SRAM 3 198 B (39,0 %). HEX build/afr2-i2c-diagnostic.hex, SHA-256 a4c1d5cc…143a69. Ficha changes/2026-11-07-50-oled-i2c-diagnostic-build.md.
Cargada luego en COM16 con avrdude (14 554 B escritos y verificados, fusibles sin cambios); el Mega queda con la prueba en lugar del auto.
Estado: atmega=imagen de servicio compilada y cargada, sin resultados físicos; esp/web=sin cambios.

## AFR2-20261107-49 - 07/11/2026

Compilación del firmware ATmega con avr-gcc 7.3.0, incluyendo AFR2-20261107-41, -43, -46, -47 y -48: sin errores y con un aviso de estilo preexistente. Flash 132 054 B (50,4 %), SRAM estática 6 329 B (77,3 %), 1 863 B libres para la pila. HEX SHA-256 167cc564…eaa346. Ficha changes/2026-11-07-49-atmega-build.md.
Estado: atmega=compilado, sin cargar; esp/web=sin cambios.

## AFR2-20261107-48 - 07/11/2026

OLED: un clic o una pulsación larga sobre una notificación de red cierra todas las notificaciones de red en cola y silencia 5 s los eventos frecuentes del ESP (WebSocket, clientes, Web servida). Antes cada clic cerraba una sola y la siguiente aparecía al instante, así que la pantalla parecía trabada. Hashes del renderer Web actualizados. Ficha changes/2026-11-07-48-network-notice-click.md.
Estado: atmega=fuente editada, sin compilar ni cargar; web=manifiesto actualizado, sin compilar; esp=sin cambios.

## AFR2-20261107-47 - 07/11/2026

ATmega: la pantalla se inicializa al final (TWI, I2C Manager y OLED después del resto), espera 1200 ms en lugar de 500 ms y, tras la primera inicialización en frío, repite una configuración completa a los 300 ms, que se da por hecha solo con ACK. Apunta a que el panel no confirme comandos mientras sale de su reset. Hashes del renderer Web actualizados. Ficha changes/2026-11-07-47-oled-init-last.md.
Estado: atmega=fuente editada, sin compilar ni cargar; web=manifiesto actualizado, sin compilar; esp=sin cambios.

## AFR2-20261107-46 - 07/11/2026

OLED: el refresco periódico ya no reenvía oscilador, precarga ni VCOMH (posible parpadeo cada segundo), y el repaso de RAM pasa de 40 a 250 ms para que el bus I2C descanse. Revisados TWI, I2C Manager y watchdog: sin bloqueos. Causas eléctricas probables (SDA/SCL del shield unidos a A4/A5, pull-ups débiles, caídas de 5 V) pendientes de medir. Se actualizaron los hashes de `afr2_oled.c` y `afr2_oled.h` en el manifiesto del renderer Web, que frenaban `build:esp`; el dibujo no cambia. Ficha changes/2026-11-07-46-oled-refresh-scrub.md.
Estado: atmega=fuente editada, sin compilar ni cargar; web=manifiesto del renderer actualizado, sin compilar; esp=sin cambios.

## AFR2-20261107-45 - 07/11/2026

Loader 2.0.0 portable: `Preparar-Distribucion.ps1` ya no fija `D:\ProyectoRobotica2` (calcula las rutas desde su carpeta), borra el build temporal, revisa texto y EXE del paquete y cancela el ZIP si encuentra rutas, usuario o equipo de quien lo arma, y genera el ZIP con separadores `/`. Corregidos `Compilar-Loader-2.cmd` (carácter de control), el ejemplo con `D:\` de README.md y la versión de PORTABILIDAD.md. Fuentes C# y perfiles de ejemplo ya eran portables. ZIP `AFR2-Loader-dist-2.0.0.zip`, SHA-256 02244b74…8b6e. Ficha changes/2026-11-07-45-loader-portable.md.
Estado: loader=compilado y ZIP verificado sin datos del equipo, sin abrir en otra PC; atmega/esp/web=sin cambios; sin COM ni flasheo.

## AFR2-20261107-44 - 07/11/2026

Informe de pinout actualizado en `hardware/INFORME_PINOUT_ROBOT_20261107.html`. Incluye mapa de cabeceras por origen, sensor de batería en A5 (esquema y lecturas esperadas), AutoKEY D32/PC5 y AutoRESET D33/PC4 (etapa NPN), y la barra IR con polaridad e IR1 a la izquierda. Ficha changes/2026-11-07-44-pinout-report-update.md.
También se actualizó el pinout de la Ayuda de la Web (`helpContent.ts`): AutoKEY/AutoRESET, IR digitales con IR1 a la izquierda, batería y retiro del método D0/D1.
Estado: documentación; web=helpContent.ts editado, sin compilar; atmega/esp=sin cambios.

## AFR2-20261107-43 - 07/11/2026

Sensores de línea OSOYOO de 5 canales (digitales, LOW sobre negro según la hoja): la línea pasa a ser válida sin calibración obligatoria. La confirmación de calibración ya no exige que los cinco canales cambien entre fondo y línea; alcanza con la muestra sobre blanco. Causa: con una línea angosta la calibración nunca se aceptaba y el seguimiento quedaba en «Calibrar línea». Además, el sentido de corrección del seguimiento estaba invertido: IR1 (A4) está a la izquierda y ahora la línea a la izquierda produce giro antihorario. Ficha changes/2026-11-07-43-ir-osoyoo-digital.md.
Estado: atmega=fuente editada, sin compilar ni cargar; esp/web=sin cambios; sin prueba física.

## AFR2-20261107-42 - 07/11/2026

Control táctil Web: paneles Información/Parámetros cerrados al abrir, joystick que sigue al dedo, estela bajo el auto según potencia, modelo que se inclina hacia la dirección y textos/paneles animados (respeta movimiento reducido). Ficha changes/2026-11-07-42-touch-drive-ui.md.
Estado: web=fuente editada, sin compilar; atmega/esp=sin cambios.

## AFR2-20261107-41 - 07/11/2026

Izquierda/derecha cruzadas al conducir: nuevo `AFR2_LATERAL_WHEEL_SIGN` -1 en la mezcla del ATmega (Drive, DrivePwm y odometría Y) y `lateralWheelSign` -1 en `motion_reference.json`, consumido por la cinemática Web. Avance y giro sin cambios. Ficha changes/2026-11-07-41-lateral-wheel-sign.md.
Estado: atmega=fuente editada, sin compilar ni cargar; web=fuente editada, sin compilar; esp=sin cambios.

## AFR2-20261107-40 - 07/11/2026

ATmega: secuencias de 32 pasos habilitadas (`AFR2_SEQUENCE_MEMORY_APPROVED=1`, +656 B de SRAM; 6315 B estáticos, 1877 B libres para pila). ESP y Web ya las soportaban; se anuncian solo con Hall calibrado. Ficha changes/2026-11-07-40-sequences-enabled.md.
Estado: atmega=compilado, sin cargar; esp/web=sin cambios.

## AFR2-20261107-39 - 07/11/2026

Hall para go-to: concordancia PWM/Hall por rueda con fallo 4 en lazo cerrado si el sentido está cruzado (diagnóstico `HP:` en la pantalla Motores y texto del fallo en la Web) y velocidad mínima de 40 mm/s en el go-to fuera de tolerancia. Ficha changes/2026-11-07-39-hall-goto-readiness.md.
Estado: atmega=compilado, sin cargar; web=compilada; esp=sin cambios.

## AFR2-20261107-38 - 07/11/2026

ESP: red STA por defecto desde `src/wifi_defaults.local.h` (ignorado por Git; plantilla `wifi_defaults.example.h`). Se usa sin configuración guardada y como respaldo cuando no hay red anterior. Credenciales fuera del repositorio. Ficha 2026-11-07-37.
Estado: esp=compilado, sin cargar.

## AFR2-20261107-37 - 07/11/2026

ESP: si la red STA configurada no conecta en 15 s, prueba la última red que funcionó (`staPrevious`) antes de abrir el AP; los reintentos alternan entre ambas. Estado expuesto como `staNetwork`/`previousAvailable` y aviso en la página Wi-Fi. Ficha changes/2026-11-07-37-esp-wifi-red-anterior.md.
Estado: esp=compilado, sin cargar; web=compilada; atmega=sin cambios.

## AFR2-20261106-36 - 06/11/2026

Control táctil: gracia de 250 ms ante pointercancel espurio del dedo de arrastre (un toque nuevo continúa el mismo vector) y el dedo que queda tras un giro de dos sigue arrastrando. Verificado con eventos sintéticos; sin prueba en teléfono. Ficha 2026-11-06-33.
Estado: web=compilada; atmega/esp=sin cambios.

## AFR2-20261106-35 - 06/11/2026

Cierre del lote: sentido del servo con el encoder invertido (ATmega + simulación Web), estados Enviada/Recibida/Aplicada/Error del servo en la Web, rumbo estimado por Hall en el control táctil, estela de barrido sincronizada con el ángulo real y ecos con desvanecimiento, y panel de muestreo HC-SR04 (fijo/cantidad/variable sobre `robot.ultrasonic.configure`). Ficha 2026-11-06-33.
Estado: atmega=compilado, sin cargar; esp=sin cambios; web=compilada, sin empaquetar; sin prueba física.

## AFR2-20261106-34 - 06/11/2026

ATmega: pulsos por vuelta ya no se abortan por detenciones de enlace con el auto detenido; OLED re-inicializa con la orientación configurada y limita el refresco a 40 ms. Fuente editada, sin compilar ni cargar; causas sin confirmar en hardware. Ver ficha 2026-11-06-33.
Estado: atmega=fuente editada; esp/web=sin cambios en esta tanda.

## AFR2-20261106-33 - 06/11/2026

Loader 2.0.0 (compilado) y control táctil Web con paneles Información/Parámetros colapsables y modelo fijo con giro animado. Lote de 15 cambios solo parcial: firmware, rumbo Hall, ACK de servo, OLED y muestreo siguen pendientes; ver ficha changes/2026-11-06-33-loader-2-web-touch-panels.md.
Estado: atmega/esp=sin cambios; web=fuente editada sin compilar; loader=compilado.

## AFR2-20261106-32 - 06/11/2026

Loader: (1) el paso de escritura ESP fallaba tras un flash_id correcto con `UnsupportedCommandError: Invalid (unsupported) command 0x8` (logs 20261106-205638/205658): por el puente Mega la ROM respondió "comando inválido" a un SYNC suelto tras el log de arranque y esptool solo reintenta `FatalError`, de modo que un único eco abortaba los `--connect-attempts 30`. `tools/esp_bridge_esptool.py` convierte ese error en un SYNC fallido reintentable; no cambia baud, KEY/RST ni offsets. (2) Pestaña Operación: Preparación (consulta de capacidad) y FileSystem (reserva y distribución de memoria) pasan a una tarjeta compacta de la barra lateral, sobre «Ejecución y resultado», visible solo si la sección incluye FileSystem; en los demás casos la consulta de capacidad queda a la izquierda y «Ejecución y resultado» ocupa todo el alto.
Estado: atmega/esp/web=sin cambios; Loader=fuente editada, sin compilar ni probar con hardware.

## AFR2-20261106-11 - 06/11/2026

KEY/RST del ESP automáticos y opcionales por transistores (D32=KEY, D33=RST) accionados por el flasher de servicio del ATmega, con opción manual/auto en el Loader y D0/D1 marcado como retirado. Ficha: [2026-11-06-11](changes/2026-11-06-11-esp-key-rst-automatico.md). Estado: atmega/Loader=fuente editada, sin compilar ni probar; esp/web=sin cambios; hardware sin armar ni medir.

## AFR2-20261106-10 - 06/11/2026

Fluidez de Conducción táctil. Síntoma: mueve una vez y no más, se detiene en marcha, el giro se ve en Web/OLED sin mover ruedas. Causas en fuente (no reproducidas físicamente):
- Web `ManualDriveOverlay`: cancelaba el gesto en cada cambio de `robotState.mode`; la primera consigna PWM pasa idle→manual y una lectura fallida da undefined, así que cortaba el dedo y frenaba. Ahora cancela solo si toma el auto otro dueño (línea, secuencia, prueba de rueda, fallo).
- ESP `Broker::tick`: las tareas de fondo (pulso, estado, Wi-Fi, red, cabezal, info) tomaban la UART antes que la cola Web; `robot.drive.pwm` esperaba hasta el timeout de 600 ms y la Web enviaba STOP. Ahora se toma la respuesta y se despacha la cola Web primero; el halt de seguridad sigue con prioridad.
- ESP pulso Web: usaba solo la primera sesión; un segundo dispositivo inactivo informaba 1500 ms y el ATmega detenía. Ahora usa la actividad más reciente de todas las sesiones.
- Web `ControlPage`: BUSY/TIMEOUT aislados no disparan STOP (latido 180 ms reintenta; TTL 400 ms del ATmega sigue protegiendo); STOP tras ~1 s de fallos seguidos o rechazo no reintentable. `afr2Client`: tras timeout se envía la última consigna en cola.
- ATmega: la reactivación de la vista táctil no detiene el PWM Web en curso.
Estado: atmega/esp/web=fuente editada y compilada; sin cargar ni probar físicamente.

Compilación a pedido (incluye AFR2-20261106-07..10): ATmega `make -B firmware` (avr-gcc 7.3.0) PASS sin advertencias, flash 125054 B (47,7 %), SRAM estática 5631 B (68,7 %, +17 B Prueba Hall). ESP `pio run -e esp12s_candidate` SUCCESS, RAM 50676/81920 B, flash 452655 B. Web `npm run build:esp` PASS (generador con guardia, tsc, Vite), 17 activos, 2488776 B gzip, FS estimado 2637824 B, copiado a ESP/data/dist; imagen LittleFS no generada. Sin COM, carga ni prueba física.

## AFR2-20261106-09 - 06/11/2026

Salida de QR (0x030045/0x030046). Síntoma informado: en el QR, corto y largo no salen y queda en un ciclo. Causa probable, no reproducida: al escanear, el teléfono se une al AP y el ESP envía avisos «Conectividad» (cliente, AP listo de 20 s); en el QR cada pulsación primero descartaba el aviso en cola y, si llegaban nuevos, nunca navegaba. Ahora en QR corto o largo siempre vuelven al contexto anterior y descartan avisos informativos en cola. ATmega `Afr2Ui_Apply` y simulación Web `uiSimulation.ts`; hash revisado de `afr2_screens.c` actualizado y biblioteca regenerada. Estado: atmega=fuente editada, esp=sin cambios, web=fuente editada; sin compilar, cargar ni probar físicamente.

## AFR2-20261106-08 - 06/11/2026

Giro táctil a ángulo con odometría Hall, desactivable. Web: casilla «Giro por ángulo con sensores Hall» en Control táctil (predeterminada activa, recordada por navegador). Activa y con odometría calibrada (`gotoAvailable`), el giro con dos dedos marca el objetivo en pasos de 15° y al soltar envía `robot.goto.start` con x=y=0 y `thetaMrad`; desactivada o sin calibración, conserva el giro PWM en lazo abierto de AFR2-20261106-07. ATmega: GOTO de giro puro no exige HC-SR04 (no avanza), nunca traslada y termina solo por error de yaw ≤25 mrad; `Afr2Product_Drive` exige HC-SR04 solo con vx/vy; GOTO admitido con la vista táctil activa. Requiere CPR medido en las cuatro ruedas, geometría y pose válida. Estado: atmega=fuente editada, esp=sin cambios, web=fuente editada; sin compilar, cargar ni probar físicamente. Nota: IDs propios renumerados (antes -06/-07) por colisión con la entrega Loader AFR2-20261106-06.

## AFR2-20261106-06 - 06/11/2026

Loader: auditoría UX y primera reorganización (barra inferior en fila propia, encabezados medidos, etapas compactas, requisito de carga visible, roles de botón, ayuda única por campo, Eventos con estado vacío). Solo fuente; atmega/esp/web sin cambios; sin compilar, abrir la interfaz ni flashear. [Ficha](changes/2026-11-06-06-loader-ux-audit.md).

## AFR2-20261106-07 - 06/11/2026

- Control táctil: la sincronización OLED (`robot.touch.ui`) y `robot.drive.pwm` compiten por el único slot UART del ESP; el `BUSY_IN_FLIGHT` resultante disparaba STOP en la Web. Ahora se tolera y el latido de 180 ms reintenta. El giro con dos dedos, con el auto conectado, gira sobre el eje en lazo abierto (`turnPct` = potencia de giro) mientras se gira; Q/E del teclado siguen locales.
- Animación táctil: las ruedas muestran avance físico (sin signos de cableado `MOTION_REFERENCE`), así adelante/atrás/lateral/giro coinciden con el desplazamiento.
- Hall: pull-ups internos en A8..A15 (salidas colector abierto flotaban). Pinout cotejado con el usuario: A=FR A8, FL A9, RR A10, RL A11; B=FR A12, FL A13, RR A14, RL A15; coincide con `board_pinmap.h`.
- Nueva vista `Prueba Hall` (0x030047, `AFR2_N_HALL_LIVE`=70), tercera en Pruebas: cuatro ruedas a la vez, cero al entrar, refresco 100 ms. Web: nodo virtual local del selector de motor movido de 70 a 72; biblioteca regenerada con `--copy-only` y hashes revisados solo de screens/menu_catalog/icons.
- Pulsación larga: sin marco; disco de radio 9+2 px limpio, ícono y anillo solo mientras se presiona; al soltar se redibuja lo de abajo salvo tras largo completo (la página destino repinta). ATmega y Web.
- Sensores de línea: sin causa encontrada en firmware (lectura digital PF4..PF0, migración a tipo 1); pendiente diagnóstico físico.
- Estado: atmega=fuente editada, esp=sin cambios, web=fuente editada; sin compilar, cargar ni probar físicamente. RAM AVR +17 B.

## AFR2-20261106-05 - 06/11/2026

Conducir (`robot.drive.pwm`) omite en el ATmega los chequeos de permiso: origen local, cabeza/radar, secuencia, calibración Hall, fallo latcheado, motores deshabilitados y preferencias pendientes. Anula lo que estorbe (Stop), limpia el fallo y habilita motores; mantiene validación de rango/TTL, STOP y vencimiento de TTL; el watchdog de Hall no corta en PWM manual. `AFR2_EXT_F_MANUAL_PWM` siempre anunciado. Estado: atmega=fuente editada, esp=sin cambios, web=sin cambios; sin compilar, cargar ni probar físicamente.

## AFR2-20261106-04 - 06/11/2026

RAM AVR7636→5614; textos PROGMEM y almacenamiento condicional de secuencias. QR finish380→29, conservando ambos buffers OLED. Builds y paridad2104/2104 PASS; sin carga ni prueba física. [Ficha](changes/2026-11-06-04-avr-ram-qr.md).

> **AFR2-20261106-03 / Loader 1.2.0:** nueva distribución compartible compilada (GUI/MCP), rutas {App}/módulos y datos de cada usuario configurables; pestaña ATmega redundante retirada, soporte conservado en Flasher/Operación. ZIP y auditoría: [ficha](changes/2026-11-06-03-loader-portable-120.md). Sin COM, firmware compilado ni flasheos por esta entrega.

## AFR2-20261106-02 - 06/11/2026

Regeneración y builds AVR/ESP/Web/Loader PASS, paridad 1680/1680 y LittleFS candidato. Presupuesto SRAM/pila AVR FAIL; sin despliegue ni prueba física. [Ficha](changes/2026-11-06-02-regeneration-checks.md).

## AFR2-20261106-01 - 06/11/2026

Correcciones coordinadas ATmega/ESP/Web: conexion WS, diagnostico HTTP, IR digital, Wi-Fi persistente/paginado y pantallas. Fuente sin generacion/build/tests/validadores ni cargas. Loader previo conservado; CAD sin cambios. [Ficha](changes/2026-11-06-01-coordinated-corrections.md).

## AFR2-20261105-01 - 05/11/2026

Loader: etapas verticales a la derecha del log, con scroll independiente y terminal más alta. Solo fuente; sin compilación, ejecución de la interfaz modificada, pruebas ni operaciones sobre placas. [Ficha](changes/2026-11-05-01-loader-checks-sidebar.md).

## AFR2-20261105-33 - 05/11/2026

El OLED se repara solo ante ruido I2C: la configuración del panel se reenvía cada segundo con la orientación y el contraste actuales, y la RAM se reescribe por bloques (pantalla completa cada ~2,6 s). La recuperación ya no invierte la imagen con `rotation=1`. Sin compilar ni flashear; se recomiendan pull-ups externos de 4,7 kΩ. [Ficha](changes/2026-11-05-33-oled-self-healing.md).

## AFR2-20261104-32 - 04/11/2026

El firmware del ESP admite 24 activos Web y un manifiesto de hasta 12 KiB (antes 16 y 8 KiB); se actualizan el empaquetador de la Web y `PROTOCOL.md`. Sin compilar ni flashear; el costo de RAM (unos 400 B fijos) es una estimación y falta medirlo en la placa. [Ficha](changes/2026-11-04-32-esp-24-assets.md).

## AFR2-20261103-31 - 03/11/2026

La Web compila (`tsc -b` y `vite build --mode esp` sin errores) tras corregir tamaños de ícono, el espacio de nombres del QR vendido y dos variables sin uso. Peso medido: 17 archivos, 4,44 MiB → 2,36 MiB con gzip (91,4 % del presupuesto LittleFS). El paquete ESP no se genera porque el firmware admite 16 archivos y hay 17; Infraestructura sigue leyendo el manifiesto anterior. [Ficha](changes/2026-11-03-31-web-build-weight.md).

## AFR2-20261103-30 - 03/11/2026

Banco OLED único (`OledWorkbench`) compartido por Pantalla en vivo y Ayuda → OLED, lista de pantallas a la altura del banco, sin leyendas grandes de gestos y modal de Configuración con márgenes y bordes parejos. Verificado en navegador local sin robot; sin compilación. [Ficha](changes/2026-11-03-30-oled-workbench.md).

## AFR2-20261103-29 - 03/11/2026

Banner de Inicio: en anchos chicos de escritorio (840–1100 px) el texto ya no se recorta ni queda corrido; la altura sigue al contenido y el título escala con el ancho útil. Verificado en navegador local; sin compilación. [Ficha](changes/2026-11-03-29-home-hero-narrow.md).

## AFR2-20261103-28 - 03/11/2026

Conexión → Estado de red muestra «Sin datos» con un ícono en todo el panel cuando no hay lectura vigente. Verificado en navegador local sin robot; sin compilación. [Ficha](changes/2026-11-03-28-wifi-no-data.md).

## AFR2-20261103-27 - 03/11/2026

Pantalla en vivo con captor de teclado: ← y → giran, Espacio es el SW (largo al mantener), Escape cierra, Enter no hace nada y nada del visor es seleccionable; con pistas de tecla en cada acción. Verificado con eventos sintéticos en navegador local; sin robot ni compilación. [Ficha](changes/2026-11-03-27-live-screen-keyboard.md).

## AFR2-20261103-26 - 03/11/2026

Ayuda reescrita con una idea por viñeta, código resaltado en celeste y tarjeta de módulo con cuarto de pinout (ATmega, ESP, Web y OLED). Verificado en navegador local; sin compilación. [Ficha](changes/2026-11-03-26-help-rewrite-pinout.md).

## AFR2-20261103-25 - 03/11/2026

Loader más directo: etapas en viñetas, operaciones en desplegable, banner de automatización con IA y nodos de la portada separados. Se quitan los carteles «Fuente» de Explorar y Ayuda. Verificado en navegador local; sin compilación. [Ficha](changes/2026-11-03-25-loader-direct-no-source.md).

## AFR2-20261102-24 - 02/11/2026

Ayuda: cada módulo abre con una tarjeta técnica (rol, nombre, descripción y cuatro especificaciones) en la familia clara/oscura de las cabeceras. Explorar suma Loader (/loader): portada con mapa, recorridos de carga del ATmega y del ESP, cifras, motivos y herramientas, con el estilo de Infraestructura. Verificado en navegador local; sin compilación. [Ficha](changes/2026-11-02-24-help-descriptors-loader-page.md).

## AFR2-20261102-23 - 02/11/2026

Ayuda técnica: pestañas en orden ATmega, ESP, Web, OLED (ATmega por defecto); cada módulo con grupos, temas desplegables, tabla de datos (pines, timers, tramas, límites, versiones) y fuente en el repo. Verificado en navegador local; sin compilación. [Ficha](changes/2026-11-02-23-web-help-technical.md).

## AFR2-20261102-22 - 02/11/2026

Revisión del manual en todas las rutas: título de Inicio visible otra vez, cabeceras de página en la familia clara/oscura de Infraestructura con ícono por zona, carga de rutas como tarjeta. Pantalla en vivo: selector Periódica / Al cambiar, lienzo que no desaparece al adquirir, barra superior con «Última entrada» y «Actualizar pantalla», encoder al lado en PC y debajo en celular, leyenda corta del SW con íconos. Verificado en navegador local sin robot ni compilación. [Ficha](changes/2026-11-02-22-web-manual-review-live-screen.md).

## AFR2-20261102-21 - 02/11/2026

Radar de la Web como instrumento: etiquetas HTML nítidas, plato con brillo radial, anillos y marcas con jerarquía, sector y haz con caída suave, ecos con halo, pulso y contorno, escala uniforme entre 25 cm y 2 m, chip y leyenda unificados y visor compacto en celular. Verificado en navegador local con ecos de muestra temporales; sin robot ni compilación. [Ficha](changes/2026-11-02-21-radar-instrument-redesign.md).

## AFR2-20261102-18 - 02/11/2026

Radar: alcances confirmados 25 cm / 50 cm / 1 m / 2 m en Web, menú ATmega y copia OLED vinculada. Reforma visual con manual 0.1.11, vista superior inicial, haz por capas y pulso breve por eco válido. Solo fuente; sin tests, build, validaciones, navegador ni flasheos. [Ficha](changes/2026-11-02-18-radar-visual-and-ranges.md) · [Contrato](contracts/RADAR_SCALES_20261102.md).

## AFR2-20261102-17 - 02/11/2026

Modelos: botones inferiores centrados, flechas derechas y hover breve al acento con texto del tema en contraste; teclado/movimiento reducido inmediatos. Solo fuente Web; sin tests, build ni validaciones. [Ficha](changes/2026-11-02-17-model-centered-actions-hover.md).

## AFR2-20261102-16 - 02/11/2026

Modelos: accesos del pie como botones secundarios compartidos, con iconos y títulos «Simulador» y «Explorar infraestructura». Misma navegación. Solo fuente Web; sin tests, build ni validaciones. [Ficha](changes/2026-11-02-16-model-footer-buttons.md).

## AFR2-20261102-15 - 02/11/2026

Modelos: render cubriendo toda la tarjeta, texto superpuesto con degradado/sombra suave y tres columnas conservadas. Retirados «También» y el enlace al cubo auxiliar del pie. Solo fuente Web; sin tests, build, validaciones ni navegador. [Ficha](changes/2026-11-02-15-model-full-image-cards.md).

## AFR2-20261102-14 - 02/11/2026

Dashboard OLED: modo y tarjeta redondeada con estados reales; rotación tras3s sin encoder y permanencia4s, SW mantenido pausa, fallo fijo y redibujo por regiones. Cabeceras/iconos/contadores/selectores/editores/retorno y Sí/No alineados en AVR/Web. Simulación vinculada con fixtures explícitos; transporte ESP sin cambios. Sin tests, builds, validadores, navegador ni flasheo. [Ficha](changes/2026-11-02-14-oled-dashboard-idle-status-family.md) · [Contrato](contracts/OLED_DASHBOARD_STATUS_20261102.md).

## AFR2-20261102-14 - 02/11/2026

Manual visual ampliado a 0.1.11 draft con integración aditiva de Infraestructura: reglas de composición/material/movimiento, atlas íntegro, contratos, tokens por capa, fuentes y decisiones VIS-057–061, dos esquemas SVG y precedencia sobre antecedentes. Se mantienen marca, componentes, formularios, overlays, navegación y OLED. Solo documentación; sin aplicación de interfaz, tests, builds, validadores, navegador ni render de comprobación. [Manual](docs/design-system/index.md) · [Ficha](changes/2026-11-02-14-visual-manual-infrastructure-integration.md).

## AFR2-20261102-13 - 02/11/2026

Modelos: tres tarjetas compactas en una fila desde 840 px, dos en ancho intermedio y una en celular. Render completo de alto acotado encima del texto, usando imágenes existentes 640; acciones al pie. Solo fuente Web, sin tests, build, validaciones ni navegador. [Ficha](changes/2026-11-02-13-compact-model-cards.md).

## AFR2-20261102-12 - 02/11/2026

Dashboard OLED: margen superior2px para SSID, IP en y14 y cabecera26px. Scroll independiente conservado para ambos textos largos, incluidas las bandas de redibujo parcial. AVR/Web en fuente, sin tests/build/validaciones/flasheo. [Ficha](changes/2026-11-02-12-dashboard-network-padding-scroll.md).

## AFR2-20261102-11 - 02/11/2026

Dashboard anterior recuperado con SSID/IP, batería y modo centrado; cabecera invertida. Visor OLED blanco/negro. En submenús, flecha anterior de16px + texto Volver; demás páginas conservan sus gestos. Fuentes AVR/Web vinculadas, sin tests, build, validaciones, navegador ni flasheo. [Ficha](changes/2026-11-02-11-oled-original-dashboard-white-back.md).

## AFR2-20261102-11 - 02/11/2026

Extracción detallada de la identidad de Infraestructura: árbol DOM, paletas por tema/zona, capas del banner, tipografía, layout y espaciado, selector con copia recortada, estaciones/progreso, estados, barras/anillo, movimiento y responsive. Dossier independiente con tokens documentales, componentes, fuentes, decisiones y límites de reutilización. Solo documentación; sin cambios nuevos en interfaz/firmware, tests, compilación, validaciones ni navegador. [Dossier](docs/design-system/infraestructura/index.md) · [Ficha](changes/2026-11-02-11-infrastructure-visual-identity-extraction.md).

## AFR2-20261102-10 - 02/11/2026

Infraestructura: el peso de la simulación conjunta suma auto, brazo y cubo; la ocupación de LittleFS se toma también de la respuesta de la placa a system.info (botón «Consultar a la placa»); sin «Fuente en el proyecto» en las etapas; el paso de Blender explica cómo se aligera el modelo. Verificado en navegador local sin robot. [Ficha](changes/2026-11-02-10-infrastructure-cube-board-fs-blender.md).

## AFR2-20261102-07 - 02/11/2026

Reforma visual de Infraestructura con el mismo contenido y orden: portada oscura con mapa de flujo animado por zonas (paquetes que viajan y nodos que abren su etapa), recorrido en línea de metro con ficha animada, barras de peso 3D, compresión y anillo de LittleFS, y glosario en tarjetas. Respeta reducir movimiento. Verificado en navegador local (1440/1000/390 px); sin compilación ni build:esp. [Ficha](changes/2026-11-02-07-infrastructure-visual-redesign.md).

## AFR2-20261102-05 - 02/11/2026

Diseños OLED128-5 vinculados en fuente AVR/Web: dashboard con paneles, cards rápidas con contador, selección invertida, barras IR y progreso común. Ayuda → OLED abre Simular flujo con servicios ficticios. QR AP y Web con datos vigentes dentro del auto, API para contenido propio y protección de captura del AP. Sin tests, compilación, validadores, navegador, promoción ni flasheo. [Ficha](changes/2026-11-02-05-oled-runtime-components-qr.md) · [Contrato](contracts/OLED_RUNTIME_QR_20261102.md).

## AFR2-20261102-04 - 02/11/2026

Explorar → Infraestructura: mapa de React/Fusion/Blender a gzip, imagen LittleFS, flash, HTTP y navegador; recorridos seleccionables con entradas/salidas, pesos GLB y conjunto, tabla de archivos antes/después de gzip y ocupación del filesystem cuando la informa la placa. Consulta HTTP de manifiesto en ESP y middleware Vite que identifica el paquete local preparado. Fuente Web/ESP y documentación; sin tests, compilación, validadores, navegador, promoción ni flasheos. [Ficha](changes/2026-11-02-04-infrastructure-explorer.md) · [Contrato](contracts/WEB_INFRASTRUCTURE.md).

## AFR2-20261102-03 - 02/11/2026

Simuladores (auto, brazo, conjunto) con la escena a la izquierda y los controles a la derecha (panel con la altura de la escena, se apila en pantallas angostas). Zoom flotante de vidrio +/− con porcentaje en el color del tema. Cámara inicial en Perspectiva igual a la de los banners. Banners de Modelos re-renderizados con más calidad, blancos sin recorte y luz más suave, mismas cámaras. Verificado en navegador local; sin compilación ni build:esp. [Ficha](changes/2026-11-02-03-simulator-side-controls-zoom-banners.md).

## AFR2-20261102-02 - 02/11/2026

Simulador UI fuera de Desarrollador (redirige a Ayuda → OLED, vista árbol). En Ayuda → OLED, la pantalla va a la izquierda y la lista a la derecha, con «Ver árbol de pantallas»; el árbol conceptual es la vista alternativa y abre la pantalla elegida en la lista. Leyenda de gestos en fila, con el icono arriba y el texto abajo. Verificado en navegador local; sin build:esp. [Ficha](changes/2026-11-02-02-oled-help-tree-view.md).

## AFR2-20261102-01 - 02/11/2026

Revisión en navegador del plan visual (5 anchos, claro/oscuro, 13 rutas) con correcciones: animación de la pill, pill en oscuro, rail, StatusNotice, pill de red y objetivos táctiles. Modelos con tres banners de renders Blender, inspección directa (/modelos/:modelo) con «Ir a simulación» y visor nuevo del conjunto; simulador separado (/simulacion/:tipo) con selector. Banner de Inicio con render. Árbol de pantallas como mapa conceptual desde el dashboard. Sin build:esp, tests ni hardware. [Ficha](changes/2026-11-02-01-browser-review-models-flow-tree.md).

## AFR2-20261101-12 - 01/11/2026

Plan visual fase 5: navegacion por tareas (Operar, Preparar, Explorar, Sistema) con rutas /conducir, /programas, /puesta-a-punto, /modelos, /conexion y /ayuda y redirecciones desde las anteriores. Barra inferior con Mas en celular, rail desde 840 px, barra expandida desde 1200 px, barra superior en una fila y barra de estado operativo en Conducir y Programas. Preferencias a Puesta a punto, diagnostico a Conexion, catalogo OLED y supervision a Ayuda, Desarrollador desde Configuracion. Breakpoints de 16 a 4 valores (hojas de componentes como complemento max-width) y 140 tamanos a la escala. Solo fuente Web y documentacion; sin tests/build/navegador. [Ficha](changes/2026-11-01-12-visual-plan-phase5-structure.md).

## AFR2-20261101-11 - 01/11/2026

Tildes recuperadas en la Web por decision humana; sustituye AFR2-20261026-03. 334 lineas restauradas desde el original y palabras en texto visible de lineas editadas despues, con revision manual de ambiguas. Excluido el texto dibujado en la replica OLED (fuente sin tildes). Quitado withoutAccents y borrado utils/displayText.ts. Solo fuente Web; sin tests/build/navegador; visualizacion de tildes No verificada. [Ficha](changes/2026-11-01-11-web-accents-restored.md).

## AFR2-20261101-10 - 01/11/2026

Plan visual fase 4: PageHeader con el unico h1 de cada pagina (--type-title) en 10 paginas; SectionHeader reducido a h2/h3 (--type-heading/--type-subheading); niveles aplanados en Wi-Fi, Sensores y fichas 3D; Modelos 3D sin GlassSurface exterior; telemetria de Control en chips Modo/GOTO/Salidas/Fallo. Tildes no recuperadas por el pedido AFR2-20261026-03; tamanos de texto fuera de cabeceras quedan para la fase 5. Solo fuente Web y documentacion; sin tests/build/navegador. [Ficha](changes/2026-11-01-10-visual-plan-phase4-headers.md).

## AFR2-20261101-09 - 01/11/2026

Plan visual fase 3: SegmentedControl unico con pill deslizante (clip-path, 250 ms, --ease-in-out; sin animar con teclado, resize ni movimiento reducido). TaskSections lo usa por dentro sin cambiar API; migrados mapa, alcance y vista del radar, camara del simulador, Apariencia, modo del robot (flechas solo mueven foco) y lado de curva/pista del editor. CSS de los selectores viejos borrado; oled-side queda para la fase 7. Solo fuente Web y documentacion; sin tests/build/navegador/build:esp. [Ficha](changes/2026-11-01-09-visual-plan-phase3-segmented.md).

## AFR2-20261101-08 - 01/11/2026

Plan visual fase 2: registro unico de iconos (Lucide lucide-static@0.460.0, ISC, y propios del dominio), UiIcon con `size` 16/20/24 y trazo 1,75 px. Reemplazados los glifos Unicode con funcion de icono (STOP, pad, GOTO, editor, toasts, Desarrollador, Wi-Fi) y los SVG sueltos de bateria, senal, toasts, refrescar, tactil, chevrons, candado y papelera. PLAN.md suma la fase 7 de pantallas OLED. Solo fuente Web y documentacion; sin tests/build/navegador/build:esp. [Ficha](changes/2026-11-01-08-visual-plan-phase2-icons.md).

## AFR2-20261101-07 - 01/11/2026

Plan visual 01-11, fase 1: VIS-050..VIS-054 registradas (iconos, tipo, espaciado, selector, breakpoints, navegacion en celular, atencion). Tokens `--type-*`, espaciado, `--bp-*`/`@theme`, `--attention*`, paleta `--map-*` en tokens.css y Segoe UI Variable. Corregidas seis variables CSS sin definir. Solo fuente Web y documentacion; AVR/ESP sin cambios, sin tests/build/validadores/navegador/build:esp. [Ficha](changes/2026-11-01-07-visual-plan-phase1-tokens.md).

## AFR2-20261101-06 - 01/11/2026

Simulador UI sale de Desarrollador y se abre desde su tarjeta debajo de Protocol Studio. Ruta propia, ayuda, retorno y pestanas Simulador/Arbol y catalogo con estado conservado entre ellas. Solo fuente Web; AVR/ESP sin cambios, sin tests/build/navegador. [Ficha](changes/2026-11-01-06-ui-simulator-workspace.md).

## AFR2-20261101-05 - 01/11/2026

Corregidos grados, separadores, flechas, desigualdades, puntos suspensivos y una palabra mal codificada en textos de Control, calibracion y encoder. Sin cambios de logica. Fuente sin tests, compilaciones ni navegador. [Ficha](changes/2026-11-01-05-web-text-encoding.md).

## AFR2-20261101-04 - 01/11/2026

Encuadre cercano/centrado por geometria para Auto/Brazo/Conjunto y visores Auto/Brazo/Cubo. Centrar y cambios de vista usan el mismo ajuste; no persigue movimientos. Fuente sin tests, builds, validadores ni navegador. [Ficha](changes/2026-11-01-04-close-model-framing.md).

## AFR2-20261101-03 - 01/11/2026

Modelos 3D: cielo/suelo compartidos con Simulaciones y fichas solidas legibles por tema para auto, brazo y cubo; foco, seleccion y acciones semanticas. AVR/ESP/CAD sin cambios. Fuente sin tests, builds, validadores ni navegador. [Ficha](changes/2026-11-01-03-models-scene-contrast.md).

## AFR2-20261101-02 - 01/11/2026

Simulador UI alineado con la navegacion AVR actual: retorno contextual, Si/No obligatorio, Hall por cuatro ruedas, rapidas, Wi-Fi y guias por pantalla. Arbol desplegable y [flujo completo](documentation/UI_SCREEN_FLOW.md). AVR/ESP sin cambios; solo fuente Web, sin ejecutar tests, builds, generadores ni hardware.

## AFR2-20261101-01 - 01/11/2026

Predev Web: cuatro firmas OLED normalizadas a LF, corrigiendo registros calculados sobre CRLF. Guard activo; AVR/ESP sin cambios. Sin ejecutar npm, generadores, tests ni builds. [Ficha](changes/2026-11-01-01-web-predev-lf-hashes.md).

## AFR2-20261031-12 - 31/10/2026

Simulador UI reorganizado: pantalla/encoder, guia contextual, barra de sesion,
catalogo filtrable y seleccion siempre visible. Integra la base local01 y sus
documentos. Fuente sin tests, builds, validadores ni navegador; paquetesESP
previos. [Ficha](changes/2026-10-31-12-ui-simulator-layout.md).

## AFR2-20261101-06 - 01/11/2026

Corrección del símbolo de largo: L interna en encoder, retirado subrayado.
Variante adaptada del bitmap base según indicación humana; original separado
conL no localizado. Skill y ejemplos regenerados, firmware/Web sin cambios,
sin tests o validadores. [Ficha](changes/2026-11-01-06-encoder-long-internal-l.md).

## AFR2-20261101-05 - 01/11/2026

OLED128-5: QR de contenido elegido, generador offline y exportación a PNG/
bitmaps C. Zona clara4módulos y escala entera, campo62×62 con texto lateral.
EjemploAFR2:DEMO generado; sin decoder, prueba con teléfono/OLED, tests o
compilaciones. Firmware/Web sin integrar.
[Ficha](changes/2026-11-01-05-oled128-qr.md).

## AFR2-20261101-04 - 01/11/2026

OLED128-4: cards de acciones rápidas con título, contador numérico arriba y
selección por franja invertida. Componentes y muestras Motor/Servo/Wi-Fi
registrados en skill/atlas. Solo diseño, sin migrar firmware/Web ni ejecutar
tests o validaciones. [Ficha](changes/2026-11-01-04-oled128-quick-action-cards.md).

## AFR2-20261101-03 - 01/11/2026

OLED128-3: dashboard inspirado en referencia del usuario, con cabecera invertida,
paneles jerarquizados y numerales de lectura principal. Carga indeterminada sin
texto sobre la ausencia de porcentaje. Atlas y tres páginas de propuesta
generados; sin modificar firmware/Web ni ejecutar tests o validadores.
[Ficha](changes/2026-11-01-03-oled128-dashboard.md).

## AFR2-20261101-02 - 01/11/2026

OLED128-2: componentes iconográficos con padding/centro común, pies del encoder,
largo=retorno como diseño nuevo, barra única de carga/progreso y cinco/ocho
sensores simultáneos con ID arriba/valor abajo. Atlas documental generado desde
recursos y tokens; sin migrar firmware/Web ni ejecutar tests o validadores.
[Ficha](changes/2026-11-01-02-oled128-components.md).

## AFR2-20261101-01 - 01/11/2026

Skill reutilizable `oled128-ui-design`: iconografía semántica, plantillas
OLED128×64 y diseño de flujo desde funciones. Copia personal y fuente en
Contracts, con recursos de referencia. Borrador sin migrar firmware/Web ni
ejecutar tests, builds o validadores.
[Ficha](changes/2026-11-01-01-oled128-ui-design-skill.md).

## AFR2-20261031-01 - 31/10/2026

Desarrollador agrega Simulador UI con tres controles compartidos del visor en
vivo, navegacion adaptada del ATmega, edicion, teclado y servicios ficticios.
Guia contextual, escenarios y catalogo para inspeccion. Fuente Web y documentos;
sin tests, builds, validaciones ni promocion ESP.
[Ficha](changes/2026-10-31-01-developer-ui-simulator.md).

## AFR2-20261029-03 - 29/10/2026

Checkpoint CADv17 sobre arquitecturaV7: codo remoto20/50, tensor con cuatro oblongas, encastres y PVC25/20, ejes70/60. PinzaV7 conservada; nueva preseleccion de actuadores con pieza200g y reservas, margen minimo1.272. J2 implementado con STS3250,18/72Al y apoyos625; J1/J4/J5 pendientes. Estudio de aligerado sin ejecutar y nuevos calculos de vuelco suspendidos por usuario. [Ficha](changes/2026-10-29-03-brazo-v7-codo-remoto-v17.md).

## AFR2-20261029-02 - 29/10/2026

Banco Fusion compacto v15: eslabones de aluminio55/50mm, pinza con bielas/pernos escalonados, retenciones y eje6mm justificado por flexion/torsion. Masas desde geometria y revision CAD local solicitada. J2 modulo0.8 requiere rediseño; sin integracion global ni aprobacion fisica. [Ficha](changes/2026-10-29-02-banco-brazo-compacto-v15.md).

## AFR2-20261029-01 - 29/10/2026

Presupuesto compacto del brazo: motores254.4g, estructura fina, menor alcance, masa1170.3g sin carga con reserva25%. Solo ruedas; pies y voladizo descartados. Filtro externo de par2.65, sin resistencia ni estabilidad fisica aprobadas y sin nuevo CAD. [Ficha](changes/2026-10-29-01-brazo-compacto-solo-ruedas.md).

## AFR2-20261030-19 — modelo listo antes del táctil

Modelo3D cargado y vista lista requeridos para habilitar gestos; reutiliza barra/store de progreso y mantiene condiciones comunes de motores y recuperación de errores. [Ficha](changes/2026-10-30-19-touch-model-ready.md). Sustituye la independencia del modelo propuesta en18-manual-touch-permissions. Solo fuente, sin compilar/probar/cargar.

## AFR2-20261030-18 — permiso manual común

Táctil permite reintento por nuevo gesto tras error transitorio cuando el mismo permiso de Mecanum vuelve a estar disponible; motivos de bloqueo comunes y modelo3D independiente. [Ficha](changes/2026-10-30-18-manual-touch-permissions.md). Guardas AVR/ESP conservadas; fuente sin compilar/probar/cargar.

## AFR2-20261030-13 — referencia de movimiento

Corrección longitudinal y yaw en mezcla PWM/velocidad y odometría, contraparte cinemática Web y fórmulas independientes de GOTO/vista manual; etiquetas Girar CW/CCW en OLED y Web. [Impacto y límites](changes/2026-10-30-13-motion-reference.md). ESP/wire/EEPROM sin cambios. Solo fuente, sin generación, compilación, pruebas, validadores o cargas.

## AFR2-20261030-09 — Wi-Fi y estado de red

Estado/Conexión/Datos/Configurar, búsqueda con nombres y candados, listado automático, cliente WebSocket obligatorio y modos/desconexión confirmados por ESP; contraparte OLED/Web. [Ficha](changes/2026-10-30-09-wifi-state-flows.md). EX01v11, EEPROMv8 sin cambios. Solo fuente; sin generación, builds, pruebas, promoción ni cargas.

## AFR2-20261030-10 — registro de revisión OLED para predev

Cotejo C/TS y actualización de seis hashes desactualizados en firmwareRenderer.sources.json, conservando protección de sincronización. [Ficha](changes/2026-10-30-10-web-predev-reviewed-sources.md). Sin ejecución de npm, generador, dev, compilaciones, validadores, tests ni flasheo. AVR/ESP sin cambios.

## AFR2-20261030-08 — conexión del auto en control táctil

Pill superior derecha con punto verde/rojo y Conectado/Desconectado. Simulado solo sin enlace; bloqueo conectado diferenciado y gestos cancelados al cambiar conexión. [Ficha](changes/2026-10-30-08-touch-connection-pill.md). Web solo fuente, sin build, pruebas, navegador, promoción ni carga. AVR/ESP sin cambios.

## AFR2-20261030-07 — reforma integral de UI

Menús/estado/pruebas, preferencias persistentes, geometría CAD nominal fija, radar por casilla angular y componente de potencia vinculados AVR/ESP/Web. [Detalle y límites](changes/2026-10-30-07-ui-integral.md). Sin generación, compilación, validadores, tests o flasheo.

> **AFR2-20261030-04:** límite PWM predeterminado255, journalv7 y migración RAM del antiguo80. Otros límites/calibraciones conservados. [Ficha](changes/2026-10-30-04-pwm-limit-255.md). Solo fuente AVR; ESP/Web sin cambios; sin build, tests, validadores ni carga.

> **AFR2-20261030-03:** cinco IR por ADC10 (0..1023), A4..A0, perfil2 y EEPROMv6 con migración de modos anteriores en RAM. ADC compartido con bateríaA5; descarte por cambio de canal. ESP/Web/OLED ya admiten ese rango. [Contrato y límites](contracts/EEPROM_IR_ADC_6.md). Solo fuente; sin compilación, pruebas ni carga. No acredita la causa de los ceros.

## AFR2-20261029-27

Controles rápidos, navegación al soltar, radar con escala/rastro, editor compartido, IR RAW centrados, PWM manual y atención WS prioritaria. Firmware0.11.0/Web0.4.0, EX01v9/EEPROMv5. Fuente y recursos copiados; sin build, validadores, promoción ni carga. Ver contracts/QUICK_RADAR_ACTUATORS_9.md.

> **AFR2-20261029-26:** [Batería local/Web, icono por voltaje y confirmación de reinicios](changes/2026-10-29-26-battery-system-restart-confirm.md). EX01v9 fuente; generador/compilación/despliegue pendientes, sin pruebas.

> **AFR2-20261029-25:** [toma conmutada y divisor sin diodo](changes/2026-10-29-25-battery-switched-divider.md), 10k/10k y 100 nF en paralelo con resistencia inferior. Corte comunicado por usuario. Solo documentación; atmega/esp/web sin cambios ni pruebas.

> **AFR2-20261029-24:** [batería con diodo en serie](changes/2026-10-29-24-battery-series-diode.md), recálculo suponiendo 0,7 V y límites de bloqueo/apagado. Solo documentación; atmega/esp/web sin cambios ni pruebas.

> **AFR2-20261029-23:** [investigación de batería por ADC](changes/2026-10-29-23-battery-adc-research.md). Divisor, A5 candidato e integración local/Web propuesta. Solo documentación, sin cambio de firmware/Web ni pruebas.

> **AFR2-20261029-22:** corregido en fuente el argumento PowerShell de preparación FileSystem; el perfil instalado puede ajustarse sin recompilar. Sin ejecutar scripts, compilar ni flashear. [Causa y solución](changes/2026-10-29-22-loader-powershell-filesystem.md).

> **Loader 1.1.2 / AFR2-20261029-13:** intervención KEY/RST ESP obligatoria y visible al frente aun con app minimizada/omisión de confirmaciones. Sin cambio en carga ATmega. GUI/MCP compilados y distribución generada; sin ejecución ni hardware. [Ficha](changes/2026-10-29-13-loader-esp-physical-required.md).

> **Loader 1.1.1 / AFR2-20261029-12:** autorización local por sesión para omitir confirmaciones manuales desde GUI/MCP, con advertencia obligatoria y comprobaciones automáticas conservadas. GUI/MCP compilados y distribución generada; sin ejecución ni hardware. [Ficha](changes/2026-10-29-12-loader-session-checks.md).

> **AFR2-20261029-11:** OLED solo diferencias, envío cooperativo y baja prioridad,
> hasta16columnas por bloque. Fuente sin compilar/cargar; memoria pendiente.
> [Ficha](changes/2026-10-29-11-oled-delta-cooperative.md).

> **AFR2-20261029-10:** configuración OLED/encoder confirmada en diagnóstico09
> aplicada al producto en fuente; migración UI EEPROMv2 conserva calibraciones.
> Sin nuevo build/carga. [Ficha](changes/2026-10-29-10-product-oled-encoder-verified-setup.md).

> **AFR2-20261029-09:** solo diagnóstico: giro invertido y actualizaciones
> OLED durante barridos, cola serie y reenvío automático. Fuente sin compilar
> ni cargar. [Ficha](changes/2026-10-29-09-diagnostic-direction-responsive.md).

> **AFR2-20261029-08:** contador y PRESIONADO/SUELTO solo en diagnóstico;
> fuente sin compilar/cargar/probar. [Ficha](changes/2026-10-29-08-diagnostic-counter-button.md).

> **AFR2-20261029-07:** diagnóstico OLED0x3C activo con barras móviles180grados,
> encoder y pulls internos como Wire solo para este servicio. Fuente sin
> compilar/cargar/probar. [Ficha](changes/2026-10-29-07-diagnostic-active-oled.md).

> **Loader 1.1.0 / AFR2-20261029-01:** preparación de FileSystem mediante script configurable; nuestra Web también usa ese lanzador. Incluye ejemplo sin Web y guía de integración. Fuente/documentación sin compilar ni ejecutar. [Ficha](changes/2026-10-29-01-loader-filesystem-scripts.md).

> **Loader AFR2-20261028-23 / 0.9.5:** preparación previa por sección, FileSystem/capacidad confirmada, verificación ESP integrada sin reconexión adicional, aviso KEY al primer porcentaje, prompts de Servidor MCP y manual/cableado/UNER renovados. Interfaz compilada y revisada visualmente; sin COM, flash ni compilación de firmware por el agente. [Ficha y límites](changes/2026-10-28-23-loader-flujo-manual-mcp.md).

> **AFR2-20261028-22:** impresión del diagnóstico solo ante cambios y barrido
> I2C cada10s entre inicios. Solo fuente; sin ejecutar/compilar/cargar.
> [Ficha](changes/2026-10-28-22-diagnostic-changes-only.md).

> **AFR2-20261028-21:** script PowerShell de compilación/carga del diagnóstico
> OLED/encoder preparado, sin ejecutar. Puerto COM obligatorio y herramientas
> configurables. [Ficha](changes/2026-10-28-21-diagnostic-powershell-flasher.md).

> **AFR2-20261028-20:** encoder D2/D3 y SW D42 en el diagnóstico USART0:
> sentidos opuestos, presión/liberación, corto/largo y errores de captura.
> Solo fuente; sin compilar/probar/cargar. [Ficha](changes/2026-10-28-20-encoder-uart0-diagnostic.md).

> **AFR2-20261028-19:** diagnóstico I2C OLED D20/D21 por USART0 D0/D1,
> 115200 8N1, ACK/NACK, estado del bus y resumen. Fuente de servicio separada,
> sin compilar/probar/cargar. [Guía](hardware/OLED_I2C_DIAGNOSTICO.md) · [Ficha](changes/2026-10-28-19-oled-i2c-uart0-diagnostic.md).

> **Loader AFR2-20261028-17 / 0.9.5:** KEY corregido, botones y desplegables con contraste y separación propios, etapas sin scroll por operación, flasher visible y distribución revisada con capturas. Loader compilado y GUI revisada por pedido expreso; sin COM, carga ni firmware compilado. [Detalle y límites](changes/2026-10-28-17-loader-095-revision-visual.md).

> **Loader AFR2-20261028-16 / 0.9.5:** flujo ATmega/ESP, Configuración por placa, flasher separado, Preferencias en Opciones, tablas MCP/Eventos, colores/iconos y espaciado compartidos. Monitor con envío ASCII/HEX/números, CR/LF y repeticiones cancelables por el mismo dueño del COM, con registros TX separados. [Ficha y límites](changes/2026-10-28-16-loader-095-interfaz-envio-serie.md). Solo fuente, revisión manual: sin tests, validadores, compilación, GUI, COM ni flasheos.

> **Loader AFR2-20261028-15 / 0.9.4:** selección directa del BIN completo Ai-Thinker AT v1.5.4.1 descargado: reconocido por tamaño y SHA-256, mapa completo 4 MiB desde 0x0, DIO/40 MHz del archivo. Otros BIN requieren manifiesto del proveedor. Conserva consulta física, confirmaciones y verificación. Solo fuente, sin compilar, ejecutar ni flashear. [Ficha](changes/2026-10-28-15-loader-at-bin-directo.md).

> **Loader AFR2-20261028-13 / 0.9.3:** [Feedback de carga](changes/2026-10-28-13-loader-feedback-carga.md): tiempo del proceso, último mensaje, segundos sin salida y porcentaje de escritura informado por esptool. Detección por flasher reportada por el usuario; escritura y nueva interfaz pendientes de comprobar. Solo fuente; sin compilar, ejecutar ni abrir COM.

## AFR2-20261028-12 — 28/10/2026

Loader0.9.2: Conversor HEX / ASCII universal, independiente de comandos,
monitor y operaciones. HEX a ASCII legible/escapado, ASCII literal a HEX,
portapapeles y copia HEX original. Sin COM, jobs ni logs de texto pegado.
Fuente sin compilar, ejecutar, tests ni validadores. Web/atmega/esp/CAD sin
cambios por esta ficha. [Ficha](changes/2026-10-28-12-loader-conversor-universal.md).

## AFR2-20261028-11 — 28/10/2026

Integración de origin/main2a72b9a y mejoras locales. Loader remoto0.9.0 más
monitor HEX, limpieza, USB–TTL, Python -u y verificación previa locales: fuente
integrada0.9.1. Se conservan backups/Preferencias, flasher ATmega y Protocol
Studio; este último pasa a fichas09/10 por colisión con IDs remotos01/02.
Sin compilar, probar, validar, ejecutar GUI, COM, flashear ni promover Web.
[Ficha y resguardo](changes/2026-10-28-11-integracion-local-remoto.md).

## AFR2-20261028-01 — 28/10/2026

Comprobación resistente de J1/uniones sobre v08: 35 secciones y 7.075 estados,
equilibrio de rodamientos, eje, acople D, modelo de barras y anclajes.
Requisitos de material y propuesta dimensional de brida/eje/apoyos más
separados sin elevar el plato; estudio temporal de espacio del motor.
V08 sin modificar; J1 sin aprobar, material/perfil pendientes. Sin FEA de
sólidos/contactos ni ensayo físico.
[Ficha](changes/2026-10-28-01-j1-joint-strength-study.md).

## AFR2-20261028-10 — 28/10/2026

Web: Protocol Studio alineado al flujo de AutoMicro. Parámetros guiados y HEX
manual, catálogo por familias, estructura por campos/offsets, cuatro salidas,
mapa seleccionable, stream continuo y ayuda. Conserva ROBO2, conversiones y
copias de01. Identidad AFR2 0.1.9 / VIS-046. Solo fuente con cotejo manual;
sin tests, compilación, validadores, navegador, COM, flasheos ni promoción ESP.
atmega/esp sin cambios. [Ficha](changes/2026-10-28-10-protocol-studio-essence.md).

## AFR2-20261028-09 — 28/10/2026

Web: Protocol Studio accesible desde Desarrollador. Traductor HEX/texto con
metadatos de monitor separados, copia multiformato, scanner ROBO2 y constructor
local con sesión/transacción LE y XOR. BOOT sin correlación y variantes de
secuencias actuales. Solo fuente/documentación; sin tests, compilación,
validadores, navegador, puertos, flasheo ni promoción a ESP. atmega/esp sin cambios.
[Ficha](changes/2026-10-28-09-web-protocol-studio.md).

## AFR2-20261027-30 — 27/10/2026

Loader0.7.4: Monitor serie con selector ASCII / HEX con espacios / Texto
decodificado. Usa bytes originales y reconvierte la ventana retenida sin
reabrir COM. Limpieza y copia siguen el formato visible; logs, parser y
journal MCP independientes de Vista. Solo fuente/documentación, sin
compilar/ejecutar ni pruebas COM/hardware. atmega/esp/web/CAD sin cambios.
[Ficha](changes/2026-10-27-30-loader-serial-hex-ascii.md).

## AFR2-20261027-29 — 27/10/2026

Loader0.7.3: consulta nueva al cargador ROM/stub antes de enviar verify_flash.
Sin respuesta confirmada, detiene la verificación y ofrece repetir KEY/RST;
tras recuperarlo comprueba otra vez y verifica sin repetir escritura.
GUI/MCP, Web/LittleFS y AT comparten el control. Solo fuente/documentación,
sin compilar/ejecutar ni COM/flasheos. atmega/esp/web/CAD sin cambios.
[Ficha](changes/2026-10-27-29-loader-verification-bootloader-gate.md).

## AFR2-20261027-28 — 27/10/2026

Loader0.7.2: botón Limpiar ventana en Operación, activo durante trabajos.
Vacía consola/cola de visualización; conserva logs, estados, historial y
operación en curso. Solo fuente/documentación, sin compilar ni ejecutar.
atmega/esp/web/CAD sin cambios por esta ficha.
[Ficha](changes/2026-10-27-28-loader-clear-operation-view.md).

## AFR2-20261027-27 — 27/10/2026

Loader0.7.1: escritura ESP con Python `-u`, mismos argumentos del comando
manual aportado y KEY sostenido hasta terminar escritura/verificación.
Conexión USB–TTL/puente Mega configurable, GUI/MCP compartidos y manuales
alineados. Fuente sin compilar/ejecutar esta revisión. Evidencia del usuario:
firmware manual COM5/115200, 437936 bytes y hash verificado; sin arranque,
Wi-Fi ni LittleFS acreditados. atmega/web/CAD sin cambios de esta ficha.
[Ficha y transcripción](changes/2026-10-27-27-loader-esp-command-key.md).

## AFR2-20261027-26 — 27/10/2026

Preferencias STA del ESP preparadas en `data/config/wifi.json` privado,
excluido de Git. El firmware ya lee ese archivo de LittleFS; C++ y BIN
sin cambios. Requiere regenerar/cargar la imagen FS; no se ejecutaron
builds, tests, validadores, generación FS, COM ni flasheos.
[Ficha](changes/2026-10-27-26-esp-private-sta-config.md).

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




## AFR2-20261102-15 - 02/11/2026

Aplicación visual del manual 0.1.11 a toda la Web: cabeceras con capas de Infraestructura, iconos, tarjetas, espaciado, superficies, estados de controles, campos, navegación y overlays. Adaptaciones para modelos, simulación, sensores, calibración y Protocol Studio. Conserva flujos, lógica, datos, escenas y raster OLED. Fuente modificada; sin tests, builds, validadores, navegador ni promoción ESP. [Ficha](changes/2026-11-02-15-web-infrastructure-visual-language.md).

## AFR2-20261102-17 - 02/11/2026

Conexión: bloque Cambiar modo/Configurar arriba de las pestañas y selección inicial desde el modo reportado por el ESP. Grupos de sidebar en color. Simulación y ejecución a la derecha del visor desde 1200 px. Retirado PVC32 v03 del catálogo; queda V17 final. Fuente sin tests/build/validaciones/navegador. [Ficha](changes/2026-11-02-17-wifi-mode-sequence-sidebar-final-model.md).

## AFR2-20261102-18 - 02/11/2026

Vista mecánica de simulación compartida: SimulationViewport/SimulationZoom para Auto, Brazo, Conjunto, secuencias y GOTO. Entorno, luces, suelo, cuadrícula y controles flotantes comunes; Programas usa también SimulationRobot/SimulationOrbit del simulador del auto. Adaptadores de estado, destinos y ejecución conservados. Fuente sin tests/build/validaciones/navegador. [Ficha](changes/2026-11-02-18-shared-simulation-viewport.md).

## AFR2-20261102-19 - 02/11/2026

Visor compartido: unidad/cuadrícula en cápsula informativa y porcentaje editable con confirmación por Enter/blur. Rango 0–100, restauración del valor anterior ante entrada inválida y Escape para cancelar. Zoom absoluto conectado a los adaptadores de simulación. Sin tests/build/validadores/navegador. [Ficha](changes/2026-11-02-19-simulation-zoom-editable.md).

## AFR2-20261102-20 - 02/11/2026

Secuencias: visor y Simulación y ejecución integrados en una única tarjeta, columnas contiguas estiradas y separador interno; apilado móvil dentro del mismo borde. Solo presentación, sin tests/build/validaciones/navegador. [Ficha](changes/2026-11-02-20-sequence-single-simulation-card.md).
