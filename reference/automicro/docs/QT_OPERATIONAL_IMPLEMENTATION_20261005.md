# Qt operativo, identidad Web y transportes USB / ESP-AT

## Actualización de compilación — 2026-10-05

Por pedido posterior del usuario se compiló exclusivamente el target AutoMicroQtDesktop en build-msvc, configuración Release, Qt 6.9.0 y Visual Studio 2022/MSVC. La primera pasada detectó C1075 por una llave faltante en OledTransferService::sendOperation; se corrigió en Qt df666e6 y la segunda pasada terminó con salida 0, generando build-msvc/Release/AutoMicroQtDesktop.exe. Aviso no bloqueante de cabeceras Vulkan ausentes durante configuración. Sin tests, ejecución de la interfaz, revisión visual, compilación de otros repositorios ni prueba física. Las afirmaciones de ausencia de build siguientes describen la entrega original previa a este pedido.

Fecha: 2026-10-05. Estado: cambios en fuentes locales. No se ejecutaron compilaciones, tests, validaciones automáticas, revisión visual en ejecución ni pruebas físicas.

## Interfaz y modelo

Inicio, WiFi + AP, Configuración, Control, Calibración y Seguidor se incorporan al host QML existente; MPU e IR y Recorrido conservan sus páginas QML. Sistema, Protocolo y Terminal conservan sus herramientas Widgets. La navegación comparte un único backend y no abre una conexión por pantalla.

El activo Qt se obtiene del `public/models/auto_micro.glb` actual de Web. La herramienta `tools-convert-web-model.cjs` decodifica su geometría Draco y conserva escena, materiales y nodos en GLB sin Draco para el RuntimeLoader. La conversión se ejecutó para producir el recurso, sin ejecutar el renderer. La composición de orientación usa Y-X-Z. Los puntos seleccionables distinguen chasis, F4, ESP, dos ruedas y los dos grupos IR; no se presenta el auto como un vehículo de cuatro ruedas. La animación interna de las ruedas del GLB no está conectada a los encoders.

La referencia visual es el manual Web v0.3.0 draft autorizado para esta adaptación. QML centraliza fondo, superficies, texto, acentos, radio de 8 px y tiempos en Theme; Widgets reciben QSS claro/oscuro y paleta nativa. Se usa el logo actual de Web. Tema y reducción de movimiento persisten con QSettings. La respuesta de pulsación es inmediata y la duración se reduce a cero al activar movimiento reducido. No se acredita equivalencia visual final sin abrir la aplicación.

El visor reserva espacio para el inspector en pantallas amplias y mantiene el esquema de paneles flotantes de MPU/Recorrido. Los formularios largos tienen scroll y las acciones de calibración usan Flow para evitar una fila rígida. Los datos viejos dejan de considerarse actuales: el puente QML actualiza la edad cada 500 ms.

## Comunicación y operación

USB CDC y ESP-AT TCP/UDP conservan el protocolo UNER nativo; TCP usa el puerto existente 3333. ESP Web mantiene su canal JSON. Un enlace físico abierto y una sesión de comunicación lista tienen indicadores separados. La sesión PIN F4 continúa siendo una condición adicional para operar.

Heartbeat y solicitudes usan un único broker. Los callbacks de finalización se difieren para que los controladores conserven primero el ID. STOP tiene prioridad, elimina marcha, TEST y balance en espera e interrumpe la espera de otra solicitud. La operación interrumpida se informa como resultado remoto incierto; no se reenvía automáticamente. Se mantiene una parada global visible fuera de cada página. La F4 continúa imponiendo modos, duración de pulsos y condiciones de seguridad.

En AT, F4 ejecuta WiFi mediante el módem y devuelve una aceptación intermedia y un resultado final. El escaneo y el detalle consultan la caché F4. Credenciales AT se envían al contexto de ejecución antes de CONNECT; la interfaz no afirma que esa entrega sea una escritura persistente F4. Borrar credenciales y restaurar módem también limpian el contexto F4 cuando el módem confirma. Los comandos exclusivos de ESP Web conservan su indisponibilidad en AT; la gestión de clientes AP se deshabilita en esa interfaz.

El GET_STATUS local F4 agrega una marca al final del bloque fijo, preservando sus seis contadores. Qt no decide el formato únicamente por tener 92 bytes. El broker reconoce el backend AT también cuando el transporte físico es USB.

## Calibración y PID

IR Objetos usa cinco canales físicos `[6,4,2,5,7]`; Línea usa `[3,0,1]`. Cada referencia descarta 8 muestras de asentamiento y conserva 32 muestras útiles, deduplicadas por secuencia/tick. Objetos guarda estadísticas por sensor y distancia en schemaVersion 2; Línea guarda referencias de superficie en `automicro.ir-line-calibration` v1. La importación acepta el perfil de objetos anterior v1 y conserva el soporte v2. La persistencia local usa escritura atómica y permite importar/exportar JSON.

La visualización de distancias Qt interpola medianas de referencias reales dentro del intervalo calibrado. No usa una conversión lineal arbitraria de ADC a centímetros. Esta interpolación nativa no es una copia completa del solucionador de objetos Web ni acredita la misma estimación de incertidumbre. Fuera del intervalo o sin dos referencias devuelve un dato no disponible.

Balance captura hasta 600 muestras/120 s con solicitudes separadas y tiempos de llegada al cliente. Conserva `automicro.balance-session` v1 y marca `web-arrival` por compatibilidad del esquema; el texto aclara que el cliente es Qt. No se inventan ciclos F4 sincronizados ni un ejecutor físico de ensayos. El análisis local conserva el algoritmo `arx2-grid-v1` y sus condiciones de datos; los registros de llegada no habilitan una propuesta que requiere ciclos F4. Se puede importar un registro de ciclos compatible. El procesador HTTP es opcional, explícito, con timeout y sin endpoint ni credenciales embebidas.

El candidato PID se copia al formulario. Aplicarlo requiere la acción explícita de guardado. El registro procesado solo anota aplicación tras una respuesta final satisfactoria SET_PID que devuelve ganancias coincidentes; una lectura GET_PID no acredita aplicación.

## OLED y herramientas

La herramienta Protocolo mantiene las acciones remotas de la pantalla y agrega envío de un framebuffer local de 128x64. Importa imágenes preparadas o BIN SSD1306 de exactamente 1024 bytes. La vista previa está rotulada como local. El estado remoto comunica identidad/selección; no se presenta como framebuffer leído ni como representación exacta de todos los renderers F4.

La transferencia usa BEGIN, CHUNK de hasta 112 bytes, COMMIT y CANCEL, con una operación en espera por vez, offset confirmado y CRC32 final. Requiere autorización y pantalla canvas reciente; F4 impone TEST/canvas. Un timeout no provoca reenvío automático. No se incorporaron Documentación, Protocol Studio ni OLED Studio como nuevas páginas Qt.

Recorrido conserva su planificación/simulación local. Seguidor muestra los tres sensores y el control reportado y permite seleccionar FOLLOW. No se agrega un protocolo inexistente de ejecución física de rutas.

## Entrega y siguientes comprobaciones

F4 conserva modificaciones anteriores del usuario en los mismos archivos. Los cambios de esta tarea no se deben publicar mediante un commit indiscriminado de todo ese árbol. Los logos y `stats.html` previamente modificados en Web se conservan fuera de la entrega de código nueva.

Los binarios anteriores no contienen estas fuentes. Para usarlas se necesita compilar Qt y ambos firmwares, instalar/flashear y ejecutar la revisión visual y las pruebas USB/TCP/UDP con hardware cuando el usuario las autorice. Ningún estado `adaptado` del registro central significa compilación o compatibilidad física demostrada.

Contrato nuevo y flujo: AMI-20261005-01. No se afirma paridad absoluta de algoritmos visuales, renderers OLED o ejecución física que la Web actual tampoco entrega.
