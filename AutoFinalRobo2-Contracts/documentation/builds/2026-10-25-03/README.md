# Compilación — captura táctil y flecha activa

25/10/2026, AFR2-20261025-03. Web build:esp exit0, TypeScript/Vite, minificación,
gzip y copia local a ESP/data. Compilación autorizada por el pedido previo de
compilar/corregir. Primer build exit0; se repitió tras conservar la cancelación
en el borde real de ventana sin confundirla con cambios internos de captura.
El [log](web-build.txt) corresponde a esa fuente final, sin errores.

| Resultado | Valor |
| --- | --- |
| Web / firmware conservado / EX01 |0.2.0 /0.9.0 /7 |
| Activos / gzip |7 /1481624 bytes |
| Diferencia gzip respecto02 |+570bytes |
| Raw |3041224 bytes |
| LittleFS estimado / presupuesto |1572864 /2867200 bytes |
| GLB final sin cambio |827232bytes |

[Manifest](manifest.json) y [resumen](summary.json) registran hashes coincidentes
de los siete activos y manifest entre paquete y ESP/data/dist. Sin nuevas
dependencias ni activo gráfico. Firmware AVR/ESP sin cambios ni recompilar,
conserva [build01](../2026-10-25-01/README.md) y gates Hall/memoria0.

Foco inicial en conducción, captura de teclado en modal abierto y superficie
DOM sobre Canvas; flecha activa animada y ruedas simuladas conservadas.
Hallazgos de fuente: foco previo en Cerrar y pointerEvents auto del wrapper
Canvas. No se reprodujo ni comprobó el comportamiento en navegador/teléfono.
Sin tests/validadores independientes, serie, imagen FS, flasheo o pruebas
físicas. Compilar no acredita interacción, dinámica ni hardware. Layout3MiB
sigue candidato con flash/SPI físicos sin identificar.