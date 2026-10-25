# Plan ESP vigente — AFR2-20261024-15

ESP-12S con Arduino ESP8266 propio, sin AT. Proyecto/Git independientes de
AutoMicro. [Plan anterior conservado](history/alignment15/AutoFinalRobo2-Contracts__docs__PLAN_ESP.md).
La revisión15 sustituye los pendientes históricos de puente/productores/UI.

| Área | Fuente activa y consumidor | Estado / aceptación pendiente |
| --- | --- | --- |
|FS/HTTP|storage/main, empaquetador y SystemPanel|Montaje sin autoformat, manifiesto/rutas/MIME/gzip/cache; salud y paquete visibles en Web15. Fallos reales sin ensayar|
|Wi-Fi|wifi_service, WifiPage y jobs OLED|STA/AP/AP_STA, scan asíncrono, borradores/persistencia/fallback. Radio y cortes/reconexión pendientes|
|WS|broker y Afr2Client|afr2-ws-1, una sesión, tamaños/colas, correlación, alive y snapshots. Sin prueba de saturación/fallos|
|UART|mega_link, cabeceras Contracts y afr2_app_wire AVR|ROBO2/perfil2+EX01v6, transacciones/validación/STOP. Framing anterior solo histórico|
|Robot|broker_product, Web control y producto AVR|33comandos definidos conectados a sus dueños; servicios condicionales, no motores habilitados por WS|
|OLED|captura/meta/páginas y LiveOledViewer|Framebuffer1024bytes original, revisión/permisos, credenciales protegidas. Paridad/físico pendientes|
|Red hacia OLED|broker_network/broker_wifi y afr2_network|IP/AP/SSID/clave AP local, avisos y borrador con Conectar físico. Sin radio comprobada|
|Sistema|broker_system y Hardware/Reinicio Web|Identidades/boot/reinicio y FS/paquete15; aceptación no equivale a arranque completado|
|Entrega|build:esp y copy-web-to-esp|Siete activos Web promovidos localmente; sin imagen FS/upload|

system.info añade filesystem con montaje, disponibilidad, ocupación, versión
Web, fecha de empaquetado, perfil, hash del índice declarado y tamaño/cantidad.
El mismo Storage::health se usa en hello y /api/health. Metadatos de manifiesto
opcionales; paquetes anteriores aceptados, faltantes desconocidos. Se comprueba
manifiesto/tamaño/presencia, **no hash de contenido recalculado en ESP**.

Pendientes reales: identificar flash/SPI/layout físico; aceptar radio/HTTP/WS,
colas/sesiones/STOP/timeout y persistencia bajo fallos. Compilación candidata
no identifica chip ni acredita funcionamiento. Linker4m3m/FS nominal3MiB y
presupuesto2800KiB no son mediciones físicas. Diagnóstico preparado no ejecutado.
Preservar configuración antes de una futura carga completa expresamente pedida.

Hardware: operación Mega D18/D19→selector E_RX/E_TX; puerto ESP UART3V3 es
distinto. Carga USB temporal requiere cruce D0/D1/niveles según FLASHING.
No trasladar mapa UNO ni interpretar fotos comerciales como continuidad real.

[Matriz vigente](ALINEACION_MAESTRO.md), [protocolo](../contracts/PROTOCOL.md),
[v6](../contracts/EX01_SEQUENCE_DISTANCE_6.md), [build15](../documentation/builds/2026-10-24-15/README.md).
Sin nueva función pendiente de código identificada dentro del alcance definido;
aceptación física/software y CAD diferido permanecen separados.