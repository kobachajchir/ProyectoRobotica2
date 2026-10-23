# AutoMicro API Integrity — central de mensajes y cambios

Este registro maestro reúne los cambios del contrato compartido entre FirmwareF4, Firmware ESP01, Web ESP01 y AutoMicroQtDesktop. Comienza el 2026-09-29; los cambios anteriores no están inventariados ni se consideran sincronizados por este archivo.

La [auditoría de línea de base del 2026-09-29](audits/2026-09-29-contract-baseline.md) describe el estado encontrado en los cuatro repositorios. El [informe detallado de choques](audits/2026-09-29-contract-collisions-detailed.md) desglosa cada implementación y sus diferencias. Ambos son referencias iniciales, no fichas de cambio nuevo.

Cada fila apunta a una ficha en `changes/`. El repositorio de origen conserva el cambio de código. La ficha explica el contrato anterior y nuevo, el flujo completo y qué consumidores siguen pendientes. `Pendiente` significa que aún no se adaptó o no se pudo comprobar su compatibilidad; no equivale a un fallo confirmado.

| Fecha | ID | Origen | Cambio breve | F4 | ESP | Web | Qt | Detalle |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 2026-10-22 | AMI-20261022-01 | FirmwareF4 | Recuperacion I2C en main, buffer DMA UART propio y fallo del reinicio ESP automatico diferido hasta la gracia; fuentes sin build ni carga | origen | compatible | compatible | compatible | [Ficha](changes/2026-10-22-01-f4-esp-startup-i2c-recovery.md) |
| 2026-10-19 | AMI-20261019-01 | FirmwareF4 | Sentido base del encoder fisico invertido antes de preferencias; controles remotos conservados; sin build ni carga | origen | no_aplica | no_aplica | no_aplica | [Ficha](changes/2026-10-19-01-encoder-base-direction.md) |
| 2026-10-09 | AMI-20261009-01 | Firmware ESP01 | HTTP copia buffers TCP y conserva el ultimo bloque pendiente; build esp01_1m SUCCESS, RAM 85,5 %, sin upload ni prueba fisica | no_aplica | origen | compatible | compatible | [Ficha](changes/2026-10-09-01-http-response-buffer-copy.md) |
| 2026-10-08 | AMI-20261008-03 | ESP / Web | Bypass existente: Wi-Fi temporal en RAM; catalogo Qt documenta comandos/eventos JSON como no aplicables a su ejecucion; fuentes sin build | no_aplica | origen | adaptado | adaptado | [Ficha](changes/2026-10-08-03-session-only-debug-wifi.md) |
| 2026-10-08 | AMI-20261008-02 | Web / ESP | Acceso local de depuracion servido por ESP, devMode ON; catalogo Qt documentado sin habilitar bypass; fuentes sin build ni upload | no_aplica | origen | adaptado | adaptado | [Ficha](changes/2026-10-08-02-local-esp-debug-wifi.md) |
| 2026-10-08 | AMI-20261008-01 | Firmware ESP01 | Copia explicita de buffers TCP de WebSocket/upgrade en cada build; fuentes preparadas, sin build ni upload; debug servido registrado en AMI-20261008-02 | no_aplica | origen | compatible | compatible | [Ficha](changes/2026-10-08-01-esp-websocket-buffer-copy.md) |
| 2026-10-07 | AMI-20261007-02 | Web ESP01 | Calibración sin recarga que pierde sesión; selector Studio, flujo y consistencia visual de Control/GOTO/calibración; fuente sin build | no_aplica | no_aplica | origen | no_aplica | [Ficha](changes/2026-10-07-02-web-control-navigation.md) |
| 2026-10-07 | AMI-20261007-01 | FirmwareF4 | GOTO estimado, calibración de un metro y requisitos accionables; cuatro builds completos, JSX/QML corregidos y gzip ESP actualizado; sin hardware | origen | adaptado | adaptado | adaptado | [Ficha](changes/2026-10-07-01-goto-motor-calibration.md) |
| 2026-10-06 | AMI-20261006-02 | FirmwareF4 | App de 480 KiB y preferencias V12 en sector 1; Debug -O0 compilado, 116,36 KiB libres; migración física pendiente | origen | no_aplica | no_aplica | no_aplica | [Ficha](changes/2026-10-06-02-app-sector7-preferences.md) |
| 2026-10-06 | AMI-20261006-01 | FirmwareF4 | Calibración blanco/pista DMA, MPU y balance estable; seguimiento Web/Qt; cuatro builds completados, sin hardware | origen | adaptado | adaptado | adaptado | [Ficha](changes/2026-10-06-01-line-follow-calibration.md) |
| 2026-10-05 | AMI-20261005-08 | Web ESP01 | Herramientas Seguidor, burbujas comunes, política/reintento WiFi y cámara libre con Ctrl + arrastrar/restauración de vistas; wire vigente | compatible | compatible | origen | no_aplica | [Ficha](changes/2026-10-05-08-track-tools-wifi-policy.md) |
| 2026-10-05 | AMI-20261005-05 | F4 / ESP / Web | Avisos separados Cliente Wi-Fi N y Web N; página cargada confirma presencia, cierre informa salida y cola conserva número; fuentes sin build | origen | adaptado | adaptado | pendiente | [Ficha](changes/2026-10-05-05-numbered-network-and-web-notifications.md) |
| 2026-10-05 | AMI-20261005-07 | Web ESP01 | Balance alternante, Detener unico, ajustes plegables y estado confirmado rojo/verde; wire vigente | compatible | compatible | origen | no_aplica | [Ficha](changes/2026-10-05-07-motor-controls-balance-disclosure.md) |
| 2026-10-05 | AMI-20261005-06 | Web ESP01 | PIN local errado sin conexion informa Codigo incorrecto; error compacto en login y chevron QT | no_aplica | no_aplica | origen | no_aplica | [Ficha](changes/2026-10-05-06-login-invalid-code-qt-chevron.md) |
| 2026-10-05 | AMI-20261005-04 | F4 / ESP | Máximo dos WebSockets, PIN y reconexión independientes, streams por endpoint y Qt USB simultáneo; fuentes sin build | origen | adaptado | adaptado | adaptado | [Ficha](changes/2026-10-05-04-concurrent-interfaces-auth-streams.md) |
| 2026-10-05 | AMI-20261005-03 | Web ESP01 | Óptica V2, LED blancos con toggle y ±30° desde el eje sin cono ni preview; detección axial, vistas y gráfica simulada; actualización 2026-10-06, wire existente | compatible | compatible | origen | compatible | [Ficha](changes/2026-10-05-03-underbody-optics-white-leds.md) |
| 2026-10-05 | AMI-20261005-02 | Web ESP01 | Modelo V2 de 883.260 bytes y copia recurrente del GLB vigente a data ESP; sin nuevo build ni carga a placa | no_aplica | adaptado | origen | no_aplica | [Ficha](changes/2026-10-05-02-web-model-v2-data-sync.md) |
| 2026-10-05 | AMI-20261005-01 | F4 / Qt | Qt operativo USB/AT, finales del módem, GET_AP_FALLBACK y estado F4 marcado; fuentes sin build | origen | adaptado | adaptado | adaptado | [Ficha](changes/2026-10-05-01-qt-operational-at-ap-fallback.md) |
| 2026-10-04 | AMI-20261004-01 | F4 / ESP | AP fallback persistente configurable y reintento Wi-Fi; ESP v2.4, fuente local sin nuevo build | origen | adaptado | adaptado | adaptado | [Ficha](changes/2026-10-04-01-wifi-ap-fallback.md) |
| 2026-10-03 | AMI-20261003-02 | FirmwareF4 | Scroll SSID/IP limitado a x<64, reserva fija para todos los indicadores; representación Web pendiente | origen | no_aplica | pendiente | no_aplica | [Ficha](changes/2026-10-03-02-dashboard-connectivity-scroll.md) |
| 2026-10-03 | AMI-20261003-01 | FirmwareF4 | MPU reciente, balance/marcha/TEST remotos y controles Web/Qt; implementación local sin pruebas físicas | origen | adaptado | adaptado | adaptado | [Ficha](changes/2026-10-03-01-motor-balance-control.md) |
| 2026-10-02 | AMI-20261002-07 | Web ESP01 | Protocol Studio: 106 IDs asignados, formularios y codecs; metadata ESP pendiente | no_aplica | pendiente | origen | no_aplica | [Ficha](changes/2026-10-02-07-protocol-studio-catalog.md) |
| 2026-10-02 | AMI-20261002-06 | Firmware ESP01 | Modelo GLB final servido sin caché immutable; firmware compilado | no_aplica | origen | compatible | no_aplica | [Ficha](changes/2026-10-02-06-final-model-route.md) |
| 2026-10-02 | AMI-20261002-05 | Web ESP01 | Postbuild copia solo gzip a data/dist y modelo final actualizado una vez | no_aplica | adaptado | origen | no_aplica | [Ficha](changes/2026-10-02-05-web-data-copy.md) |
| 2026-10-02 | AMI-20261002-04 | Web ESP01 | F: catálogo, comportamiento de pantallas y skill central | compatible | compatible | adaptado | compatible | [Ficha](changes/2026-10-02-04-screen-system.md) |
| 2026-10-02 | AMI-20261002-03 | FirmwareF4 | E: emisores IR en F4, ESP, Web y Qt | origen | adaptado | adaptado | adaptado | [Ficha](changes/2026-10-02-03-ir-emitters.md) |
| 2026-10-02 | AMI-20261002-02 | AutoMicroQtDesktop | C/D: finalización Qt y rechazos de acciones remotas | no_aplica | no_aplica | no_aplica | adaptado | [Ficha](changes/2026-10-02-02-qt-completion.md) |
| 2026-10-02 | AMI-20261002-01 | FirmwareF4 | A/B: reinicio de perfil 0x76 y ACK correlacionado | origen | adaptado | compatible | compatible | [Ficha](changes/2026-10-02-01-reboot-profile.md) |
| 2026-10-01 | AMI-20261001-02 | Web ESP01 | Objetos/Línea, superficies, GET/SET PID y registros procesados con conector MCP futuro | compatible | compatible | origen | adaptado | [Ficha](changes/2026-10-01-02-ir-groups-pid-processing.md) |
| 2026-10-01 | AMI-20261001-01 | Web ESP01 | Calibración IR/Balance, sesión v1, diagnóstico y análisis PID HTTP/MCP; captura y ensayo F4/ESP pendientes | pendiente | pendiente | origen | adaptado | [Ficha](changes/2026-10-01-01-calibration-balance-analysis.md) |

Estados permitidos por repositorio: `origen`, `pendiente`, `adaptado`, `compatible`, `no_aplica`. `Adaptado` describe código actualizado; las compilaciones y pruebas, cuando se soliciten, se informan por separado en la ficha. No marcar `compatible` por una suposición ni por una copia documental.

2026-10-05: por pedido expreso, Qt Release MSVC compilado con salida 0. Corrección de llave faltante en servicio OLED, commit Qt local df666e6, sin cambio wire. Ejecutable generado; sin tests, ejecución visual ni hardware. F4/ESP/Web no se recompilaron. Resultado incorporado a AMI-20261005-01 y al informe Qt.


Implementación A–F completada en fuente/documentación; [acciones y pendientes](IMPLEMENTATION.md). Esquemas ajenos a A–F y overlays completos siguen pendientes. Compilación completada por pedido posterior; pruebas, CI y despliegue diferidos.

2026-10-02: compilaciones F4 (incluido empaquetado), ESP01, Web y Qt MSVC completadas. Correcciones sin cambio de wire; [resultados y advertencias](docs/BUILD_RESULTS_20261002.md). MinGW pendiente de módulo Qt6WebSockets. Filas -02/-04 alineadas con los estados ya documentados en sus fichas.

2026-10-02: por pedido expreso, instalado Qt WebSockets 6.9.0 para MinGW. Resuelto el módulo faltante señalado en compilación; no se volvió a compilar MinGW. [Detalle de entorno](docs/BUILD_RESULTS_20261002.md). Sin cambio de contrato ni nueva ficha AMI.

2026-10-02: refinamiento visual de Protocol Studio/OLED Studio, sin cambio wire. Web compilada y gzip actualizado en data ESP. [Implementación, cobertura y pendientes visuales](docs/STUDIO_VISUAL_REFINEMENT_20261002.md).


2026-10-04: refactorización interna de FirmwareF4 3.0.0 implementada: System acotado,
estado privado por handle y operaciones de hardware/servicios externas. Compilación
Debug completa y empaquetado finalizados con salida 0; aviso de linker RWX, sin
tests ni prueba física. [Implementación y límites](docs/F4_MODULARIZATION_IMPLEMENTATION_PLAN_20261003.md).
No se crea ficha AMI de wire nuevo: no se rediseñaron IDs, esquemas ni secuencias
compartidas. ESP/Web/Qt no se modificaron; los pendientes anteriores conservan sus estados.
