> AFR2-20261025-01: las reglas anteriores de acceso quedan sustituidas por [control directo EX01v7](DIRECT_CONTROL_7.md). Sin adquisición, renovación, liberación ni habilitación OLED. Los formatos anteriores solo se conservan donde DIRECT_CONTROL_7 mantiene compatibilidad.

# Diagnóstico del paquete y LittleFS — AFR2-20261024-15

Extensión compatible de system.info (afr2-ws-1), sin nuevo comando, requisito
de lease, perfil UART o ScreenCode. Storage::health del ESP produce el mismo
objeto filesystem en system.info, system.hello y /api/health. No consulta Mega.
La Web lo muestra en Conexión y apariencia → Información de hardware.

| Campo | Tipo / significado |
| --- | --- |
|mounted|boolean, resultado del montaje LittleFS sin autoformat|
|webReady|boolean, manifiesto y activos aceptados por Storage|
|fault|string, estado del montaje/manifiesto/activos; none al aceptar|
|totalBytes, usedBytes|número o null; FSInfo del filesystem montado, no identificación de flash física|
|assetIntegrity|manifest-and-size-only; no recalcula SHA-256 de bytes en placa|
|assetCount|cantidad de activos servibles;0 si el paquete no fue aceptado|
|gzipBytes|suma declarada de tamaños presentes o null sin paquete aceptado|
|webVersion|string nominal de package.json o null; no es versión de firmware|
|packagedAt|fecha ISO del empaquetado, o null; no fecha de compilación de placas|
|buildProfile|esp/full o null si ausente/desconocido|
|indexSha256|hash raw del índice declarado en manifiesto, o null; identifica revisión, no prueba integridad recalculada|

Manifiesto afr2-littlefs-manifest-1 mantiene sus campos anteriores. Se añaden
webVersion y packagedAt opcionales; un manifiesto antiguo válido sigue siendo
aceptado. Metadatos ausentes o no representables en buffers acotados no se
inventan. El índice conserva su ETag declarado; rutas, MIME, gzip, presencia y
tamaño se comprueban antes de hacer webReady=true. No se exponen /config ni
credenciales, no se formatea ni escribe por consultar estos servicios.

/api/health no publica controlLease=none fijo: el estado de control autoritativo
se consulta por WS al broker. El endpoint de salud no es confirmación de STOP.

Calibración Web usa los comandos existentes: consulta sin lease; escritura
adquiere control, consulta robot.state y solo continúa con salidas apagadas.
Cambiar de parámetro requiere nueva lectura; aplicar/guardar/descartar invalida
la anterior. STOP, pérdida de foco/conexión o cambio de tarea cancelan pasos
pendientes. Una petición ya transmitida no se deshace por cancelar la interfaz;
el AVR conserva su validación/autoridad. EEPROM accepted no es guardado completado.

Fuente ESP/Web compilada15; funcionamiento conectado y fallos de FS no probados.
AVR sin nueva fuente, build12 y permisos/gates conservados.
