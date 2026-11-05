# Protocolo AutoFinalRobo2 — control directo

Vigente25/10/2026, AFR2-20261025-01. ROBO2/perfil2 + EX01v7,
afr2-ws-1, firmware0.9.0 y Web0.2.0. [Contrato normativo](DIRECT_CONTROL_7.md).

## Web y ESP

/ws del mismo origen, una sesión, hello/alive, req/res/err/evt y requestId.
30comandos activos en [commands.json](commands.json). No adquirir/renovar/liberar
control ni habilitar acceso desde OLED. La respuesta hello indica controlMode=direct;
el navegador rechaza un ESP que no anuncie ese modo. Servicios de radio/FS siguen
disponibles sin Mega; robot requiere negociación EX01v7 y capacidades reales.

system.alive cada500ms informa interactive=true solo con foco/página visible.
ESP retransmite edad de presencia mediante WEB_PULSE0x12. Una operación Web
caduca a1500ms sin presencia; el latido no activa salidas ni concede derechos.
Drive conserva TTL400ms, un envío en vuelo y última consigna reemplazable.
STOP prioritario, generation fence, timeout/watchdog y ausencia de reanudación
automática conservados. Desconexión cancela programas/ejecuciones y pide STOP.

## UART y aplicación

Framing ROBO2/perfil2, sesiones/transacciones, rutas0x31/0x13 y payload64bytes.
EX01v7 requiere ambos firmwares actualizados. Prefijo antiguo de autorización
en acciones pasa a reservado0:u32LE; validación de sesión/revisión/programa es
correlación, no permiso. STATE conserva16bytes: origen0/1/2 y byte4 reservado0.
El ESP publica origin, sin controlLease/lease/remoteEnabled. Sensores/pose/red/
framebuffer y registros de secuencias conservan offsets/unidades documentados.

Límites, calibración, memoria aprobada, estado ocupado, EEPROM y privacidad se
comprueban en el ATmega. El encoder remoto no se convierte en invocador arbitrario
de acciones físicas ni expone claves. Giro/corto/largo/Volver no cambian.

## HTTP/LittleFS

Whitelist por manifest afr2-littlefs-manifest-1, hasta 24 activos/12288 bytes de
manifiesto. Esquema/rutas/MIME/gzip/presencia/tamaño comprobados; no SHA recalculado
en placa. Sin autoformat/upload/editor, /config privado. / sirve index.html.gz,
Accept-Encoding gzip requerido,406 si se excluye. MIME/encoding/Vary explícitos,
index no-store, activos no-cache/ETag débil,404 exacto sin fallback SPA.
Salud en /api/health, hello y system.info: [FS/paquete](SYSTEM_INFO_FILESYSTEM_15.md).
No se afirma estado de control desde un valor fijo HTTP. Sin credenciales en logs.

[Matriz](../docs/ALINEACION_MAESTRO.md), [evidencia](../documentation/builds/2026-10-25-01/README.md).
Compilado, sin pruebas de protocolo, navegador, físicas o flasheo. Hardware y CAD
diferido conservan pendientes reales; archivo de antes de v7 en docs/history/direct-control7/.
