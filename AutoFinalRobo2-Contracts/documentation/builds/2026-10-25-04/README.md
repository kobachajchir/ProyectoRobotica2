# Compilación — simulación local independiente

25/10/2026, AFR2-20261025-04. Web build:esp exit0 (TypeScript/Vite,
minificación, gzip y copia local). Pedido previo de compilar/corregir vigente.
Fuente final incluida; [log](web-build.txt) sin errores.

| Resultado | Valor |
| --- | --- |
| Web / firmware conservado / EX01 |0.2.0 /0.9.0 /7 |
| Activos / gzip |7 /1481675 bytes |
| Diferencia respecto03 |+51bytes |
| Raw |3041425 bytes |
| LittleFS estimado / presupuesto |1572864 /2867200 bytes |
| GLB final sin cambios |827232bytes |

[Manifest](manifest.json) y [resumen](summary.json): hashes coincidentes entre
los siete activos/manifest del paquete y ESP/data/dist. Firmware ESP/AVR
sin cambios ni recompilar, conserva [build01](../2026-10-25-01/README.md).
No nueva dependencia, geometría, CSS, comando o ScreenCode. Gates0 conservados.

El fallback nativo de Canvas montaba el componente con onFailure aun con
WebGL disponible. Queda pasivo; el aviso de fallo real pertenece a RenderBoundary.
Entrada local independiente de conexión/modelo, dirección inferior separada
del estado visual. Órdenes físicas conservan sus condiciones y cancelaciones.

El síntoma fue informado por el usuario; causa seguida en fuentes propias y
la implementación R3F instalada, sin modificar esa librería. No se ejecutaron
tests/validadores independientes, navegador, teléfono, serie, imagen FS,
flasheos ni pruebas físicas. Compilación no acredita aceptación de interacción.
Layout3MiB candidato y flash/SPI físicos pendientes.