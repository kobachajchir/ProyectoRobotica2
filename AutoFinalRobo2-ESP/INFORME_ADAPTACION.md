> **Compilación14 posterior al pedido de fuente:** ATmega/ESP/Web compilados sin errores ni advertencias finales; carpeta build Windows y siete advertencias de indentación corregidas. Sin tests, navegador, flash ni promoción a ESP/data/dist. Ver Contracts/documentation/builds/2026-10-25-14/README.md. La falta de compilación mencionada más abajo describe la entrega previa.

> **Fuente vigente AFR2-20261025-14:** [sensores, servo y radar autónomo EX01v8](../AutoFinalRobo2-Contracts/contracts/SENSORS_RADAR_8.md). Firmware nominal0.10.0/Web0.3.0,41comandos/64identidades OLED. Fuentes vinculadas y recursos Web copiados; sin compilar, ejecutar tests/validadores, renderizar ni flashear. Paquete ESP anterior; los estados inferiores son históricos.

> **Vigente AFR2-20261025-01:** [control Web directo](../AutoFinalRobo2-Contracts/contracts/DIRECT_CONTROL_7.md), sin permiso previo ni habilitación desde OLED. Firmware0.9.0/EX01v7 y Web0.2.0,30comandos/59ScreenCodes, compilados juntos. STOP/caducidad/watchdog y requisitos físicos conservados; sin flash/tests/placas. Estados inferiores históricos.

> **Vigente AFR2-20261024-15:** [Matriz del maestro](../AutoFinalRobo2-Contracts/docs/ALINEACION_MAESTRO.md):33comandos y59identidades OLED con fuentes complementarias. Calibración Web adquiere control al escribir; ESP/Web informan FS/paquete. Web/ESP compilados15; AVR sin nueva fuente, build12/EX01v6/firmware0.8.0/gates0. Sin brecha software definida identificada tras lectura; aceptación física, memoria dinámica, flash/SPI y planos CAD pendientes. Solo documentación en CAD, sin diseño nuevo. Estados inferiores históricos.

> **Vigente AFR2-20261024-12:** mapa/comando actual juntos y coordenadas laterales; Hardware/Reinicio anidados, editor de pistas reversible y distancia por rueda EX01v6. Firmware0.8.0 y Web compilados; activos promovidos localmente. Build AVR incluye fuente IRQ11 con gates0; sin navegador, Hall habilitado, hardware, flasheo ni CAD. [Ficha12](../AutoFinalRobo2-Contracts/changes/2026-10-24-12-control-hardware-distance.md). Los estados inferiores son históricos.

> **AFR2-20261024-10:** activos Web con simulador GOTO/lista actualizados localmente en data/dist. Fuente/firmware ESP sin cambios; no se agrega comando de cola físico ni se habilitan gates. [Ficha10](../AutoFinalRobo2-Contracts/changes/2026-10-24-10-goto-map-queue.md). Sin imagen LittleFS, flasheo ni prueba de placas.

> **AFR2-20261024-09:** paquete Web con catálogo59 y copia de biblioteca AVR actualizado localmente en data/dist. Fuente/wire ESP sin cambios09; conserva build08 EX01v5. Visor remoto framebuffer y privacidad AP intactos. [Ficha09](../AutoFinalRobo2-Contracts/changes/2026-10-24-09-linked-oled-library.md), [build09](../AutoFinalRobo2-Contracts/documentation/builds/2026-10-24-09/README.md). Sin imagen LittleFS, flasheo ni pruebas físicas.

> **Vigente AFR2-20261024-08:** IP/AP en OLED,11tipos de aviso de red,14iconos F4 en flash y scroll de opción seleccionada. Sistema → Reinicio → Reiniciar ATMEGA/Reiniciar ESP/Volver. ATmega/ESP/Web compilados con EX01v5; sin placas, tests ni flasheos. Clave AP solo en OLED local; STA oculta. [Estado por módulo](../AutoFinalRobo2-Contracts/docs/ESTADO_IMPLEMENTACION.md) · [Contrato v5](../AutoFinalRobo2-Contracts/contracts/EX01_NETWORK_UI_5.md). Estados inferiores históricos.

> **AFR2-20261024-07:** servicios de sistema, identidad/boot y reinicio diferido, info paginada del AVR y etiquetas de segmento v4 implementados/compilados. RAM45476/81920bytes, programa419945/1044464bytes del candidato; no acredita carga real. [Procedencia07](../AutoFinalRobo2-Contracts/provenance/control-system-sequence-20261024.json) · [Estado por módulo](../AutoFinalRobo2-Contracts/docs/ESTADO_IMPLEMENTACION.md). Sin pruebas físicas, imagen FS ni flasheo. Estados inferiores históricos.

> Vigente **AFR2-20261024-04**: **modelo final incluido en ESP**, paquete **1.450.201 bytes gzip** (49,96% menos que los 2,90 MB iniciales). WebP, JS/CSS minificados y dependencias/componentes optimizados. Layout candidato: flash 4 MiB / LittleFS nominal 3 MiB, solicitado por el usuario; flash fisica sin identificar. Web full/esp y firmware ESP compilados; activos promovidos a data/dist. Sin navegador, imagen FS ni flasheo. Atmega y comandos conservan el estado de02. [Evidencia](../AutoFinalRobo2-Contracts/changes/2026-10-24-04-final-model-littlefs.md). Los estados previos inferiores son historicos.

> Estado vigente **AFR2-20261024-02**, 24/10/2026: integración ROBO2/perfil2 + EX01v3 compilada en Atmega/ESP/Web; GOTO completo, configuración por catálogo compartido y jobs Wi-Fi OLED conectados. Paquete Web actualizado en ESP/data/dist. Sin pruebas de navegador/hardware ni flasheo; secuencias físicas mantienen bloqueo de memoria. Ver [matriz vigente](../AutoFinalRobo2-Contracts/contracts/EX01_ALIGNMENT_3.md) y [evidencia](../AutoFinalRobo2-Contracts/changes/2026-10-24-02-project-alignment.md). Los estados previos que siguen son históricos.

# Adaptación ESP — AFR2-20261023-11

Estado vigente AFR2-20261023-15: ROBO2/perfil 2 y composición AVR UI/TWI escritos, sin compilar/probar. Sustituye menciones anteriores a UNER/perfil 1 y backend TWI pendiente. Ver actualización al final.


[Contracts](../AutoFinalRobo2-Contracts/README.md) · [Ficha](../AutoFinalRobo2-Contracts/changes/2026-10-23-11-esp-server-and-mega-link.md) · [Plan](../AutoFinalRobo2-Contracts/docs/PLAN_ESP.md).

Se revisó primero la Web implementada, sus mensajes afr2-ws-1, el manifiesto ya promovido y las guías/código ESP01 conservados. La fuente activa ahora implementa servidor HTTP :80 + /ws, LittleFS, preferencias, Wi-Fi asíncrono y broker UART hacia el Mega. No se modificaron referencias ni repositorios AutoMicro.

| Fuente activa | Adaptación y límite |
| --- | --- |
| src/storage.h/.cpp | Dueño del montaje, autoformat=false, whitelist manifest, esquema/rutas/MIME/tamaños; no certifica SHA del contenido; /config no servido |
| src/wifi_service.h/.cpp | STA/AP/AP_STA, scan, fallback y config propia sin defaults privados; accepted no significa conectado |
| src/mega_link.h/.cpp | Sesión/transacción, negociación, heartbeat y STOP, TX parcial sin intercalar; no catálogo F4 |
| src/broker.h/.cpp | Sobre AFR2, límites de sesión/colas, unknown/null y errores explícitos; no concede lease |
| src/main.cpp | Composición y rutas HTTP gzip del paquete existente; /api/health sin secretos |
| platformio.ini | Versiones exactas y header compartido Contracts; perfil físico/layout sin confirmar |
| tools/async_tcp_copy_buffers.py | Conserva arreglo ESP01 de ownership TCP y avance HTTP parcial; no ejecutado |

Autores y transformaciones por archivo en [provenance/esp-firmware.json](../AutoFinalRobo2-Contracts/provenance/esp-firmware.json), sin sustituir el manifest inicial. El framing UNER se adaptó; los payloads AFR2 son nuevos y no se asume compatibilidad con AutoMicro.

## Estado y pendientes

Fuente implementada sin compilar, tests, validadores, puertos serie, imagen FS ni flasheo. Falta identificar flash/modo SPI/layout y medir memoria; el paquete existente ocupa 2.925.405 bytes gzip sin overhead FS. El layout esp12e por defecto puede ser insuficiente. No se heredó el linker ESP01.

AVR incorpora endpoint y USART1 por IRQ en fuente, más composición de comunicaciones opcional. Carece de productores reales y callback STOP de potencia: no se anuncian motores, lease, GOTO, UI remota ni parada física confirmada. El Mega conserva autoridad y supervisión. La Web fuente se ajustó para capacidades dinámicas/STOP; paquete nuevo pendiente de pedido de build.

## Guías junto al código

[UART y transacciones](docs/modules/uart.md) · [Wi-Fi y preferencias](docs/modules/wifi.md) · [HTTP/WS](docs/modules/http-ws.md). El detalle vigente de offsets/tiempos/códigos es [UART_PROFILE_1.md](../AutoFinalRobo2-Contracts/contracts/UART_PROFILE_1.md). No usar los estados históricos como evidencia de funcionamiento en el nuevo hardware.

## Compilación comprobada — AFR2-20261023-15

Estado vigente: firmware compilado con PlatformIO/esp12s_candidate, sin errores y sin warnings en la recompilación final. Corregidas las reinicializaciones de objetos C++ en src/broker.cpp y src/wifi_service.cpp mediante asignación tipada. RAM estática 39.980/81.920 bytes; programa 385.481/1.044.464 bytes del perfil candidato; BIN 389.632 bytes. No acredita margen de heap/pila, flash física ni partición LittleFS. El middleware de buffers declarado sí se ejecutó en este build sobre dependencias instaladas.

[Resultados y logs](../AutoFinalRobo2-Contracts/documentation/builds/2026-10-23-15-esp-web/README.md). Web fue compilada/empaquetada localmente, pero ESP/data/dist no fue actualizado; tampoco se generó imagen FS ni hubo upload o apertura de puertos. Las afirmaciones anteriores de no compilación describen AFR2-20261023-11. ATmega se trabaja por separado; el nuevo protocolo se adaptará cuando el usuario entregue su documentación, sin atribuirle compatibilidad por haber compilado el perfil provisional.


## ROBO2 — AFR2-20261023-15

El header compartido y `src/mega_link.cpp` usan ahora ROBO2 (ASCII de cinco bytes),
tokens 0x3A/0xA5, versión 1/perfil 2. Emisor en nibble alto, receptor en bajo;
ESP transmite 0x31 y recibe 0x13. Se controla forma, resultado y correlación antes
de aceptar respuestas. Error RX revoca negociación y termina la petición con
MEGA_LINK_DOWN. El broker conserva `afr2-ws-1`; UI remota sigue pendiente.

Leer **[docs/PROTOCOLO_ROBO2.md](docs/PROTOCOLO_ROBO2.md)** para retomar desde este
firmware: describe qué enviar/esperar, validaciones de ambos lados y pendientes.
Ambos firmwares deben actualizarse juntos. Fuente sin compilar/probar; flash,
layout y carga siguen pendientes. Los archivos históricos no se modificaron.

## ROBO2 compilado — AFR2-20261023-17

Retoma desde docs/PROTOCOLO_ROBO2.md: la fuente ROBO2 vigente fue revisada y compilada con éxito, sin warnings/errors. RAM estática 40.096 bytes y programa 386.189 bytes del perfil candidato. Sin modificaciones de fuente en esta ficha, pruebas de comportamiento, hardware ni build AVR. [Log y límites](../AutoFinalRobo2-Contracts/documentation/builds/2026-10-23-17-robo2-esp/README.md). Las afirmaciones anteriores de ROBO2 sin compilar son históricas de la ficha inicial; no se confunde este build con validación física.

## Wi-Fi independiente del Mega — AFR2-20261023-20

Completado el flujo local ESP/Web: scan espera resultado terminal, estado real de radio/fallback/aplicación, selección Web de guardado temporal o permanente y validación UTF-8. Fuente compilada en ESP y Web, sin warnings/errors ESP ni errores TypeScript/Vite/postbuild. Sin hardware ni renderizado. UART/ROBO2, broker y módulo ATmega sin ediciones en esta ficha. [Contrato, procedencia y pendientes](../AutoFinalRobo2-Contracts/changes/2026-10-23-20-wifi-esp-web.md). Paquete Web local actualizado (6 archivos, 2.855.479 bytes gzip), ESP/data/dist sin promover.


## Integración vigente — AFR2-20261024-01

Revisadas fichas19/20/21. La Web21 ya contiene los cambios visuales/editor/simulador;
Wi-Fi20 se conserva. La integración de firmware pendiente se adapta en fuente:
ROBO2/perfil2 + EX01 v2, estado/pose, control supervisado, framebuffer OLED y
secuencias0x40..0x45 sin colisión con comandos de producto19.
Sin nuevo build, validador, tests, flasheo o promoción LittleFS. Secuencias físicas
permanecen bloqueadas por defecto hasta aprobar SRAM/pila y calibración real.
Contrato en AutoFinalRobo2-Contracts/contracts/EX01_WEB_INTEGRATION.md y ficha
AutoFinalRobo2-Contracts/changes/2026-10-24-01-firmware-web-integration.md.



