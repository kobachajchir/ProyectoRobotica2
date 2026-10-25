# Compilación — giro local con dos dedos

25/10/2026, AFR2-20261025-11. Web build:esp exit0; [log](web-build.txt)
final sin errores. Pedido previo de compilar/corregir vigente.

| Resultado | Valor |
| --- | --- |
| Web / firmware conservado / EX01 |0.2.0 /0.9.0 /7 |
| Activos / gzip |7 /1483767 bytes |
| Diferencia gzip respecto10 |+1687bytes |
| Raw |3048080 bytes |
| LittleFS estimado / presupuesto |1572864 /2867200 bytes |

[Manifest](manifest.json)/[resumen](summary.json): siete activos/manifest
coinciden entre paquete y ESP/data. Firmware ESP/AVR conserva build01/gates0,
sin nueva compilación. No protocolos, comandos o ScreenCodes nuevos.

Hook dual de contactos y giro local del modelo, offsets de ruedas acumulados,
rumbo y Q/E/0. No giro físico enviado; transición cancela target previo y fija
preview hasta reabrir. Chevrones09 intactos; cámara60%10 y GLB conservados.
Sin nueva dependencia/asset. Estados/cálculos revisados solo en fuente.

No se probó en navegador/teléfono, ni inyectaron eventos multitouch. Sin tests,
validadores independientes, serie, imagen FS, flasheos o pruebas físicas.
Compilar no acredita captura/ángulos/lectores/latencia aceptados. Trackpad
nativo sin dos PointerEvents touch no prometido; Q/E alternativa de teclado.
Layout3MiB candidato, flash/SPI físicos aún sin identificar.
