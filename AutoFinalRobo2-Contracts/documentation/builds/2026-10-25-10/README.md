# Compilación — modelo manual mayor

25/10/2026, AFR2-20261025-10. Web build:esp exit0; [log](web-build.txt)
final sin errores. Pedido previo de compilar/corregir vigente.

| Resultado | Valor |
| --- | --- |
| Web / firmware conservado / EX01 |0.2.0 /0.9.0 /7 |
| Activos / gzip |7 /1482080 bytes |
| Raw |3043188 bytes |
| LittleFS estimado / presupuesto |1572864 /2867200 bytes |

[Manifest](manifest.json)/[resumen](summary.json): siete activos y manifest
coinciden en paquete/ESP data. Firmware ESP/AVR conserva build01/gates0.
Única fuente: ManualDriveModel cambia factores52%→60%; otras líneas iguales.
Overlay/CSS de chevrones intactos por hash. Solo cámara manual, no malla,
unidades, otras cámaras, input/local04 o físico. Sin dependencias/activos nuevos.
Sin tests/validadores independientes, navegador/teléfono, serie, imagen FS,
flasheo, pruebas físicas o CAD.≈15,4% lineal es de fórmula, visual no medida.
