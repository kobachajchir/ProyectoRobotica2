# Compilación — táctil local y giro de ruedas

25/10/2026, AFR2-20261025-02. Pedido previo de compilar/corregir sigue vigente.
Web build:esp: exit0 (TypeScript, Vite, minificación, gzip y copia a ESP/data).
La primera compilación también terminó0; se repitió tras ajustar texto/franja
de modo para que el paquete incluya el último cambio. Log adjunto es el final.
No errores finales. No nuevas dependencias ni modificación del GLB.

| Resultado | Valor |
| --- | --- |
| Web / firmware conservado / EX01 |0.2.0 /0.9.0 /7 |
| Comandos / identidades OLED |30 /59 |
| Activos / gzip |7 /1481054 bytes |
| Diferencia gzip respecto01 |+1074bytes |
| Raw |3038594 bytes |
| LittleFS estimado / presupuesto |1564672 /2867200 bytes |
| Margen FS estimado |1302528 bytes |
| GLB final sin cambio |827232bytes |

Los hashes de los siete activos y manifest coinciden en paquete y ESP/data/dist;
detalle en [resumen](summary.json), [manifest](manifest.json), [log](web-build.txt).
Promoción local, sin imagen LittleFS ni upload. Firmware ESP/ATmega no cambia
ni se recompila: conserva [build01](../2026-10-25-01/README.md), gates0.

Táctil local no invoca callbacks físicos; reconectar no cambia ese modo.
Ruedas por cursor/gesto, radio nominal40mm, Mecanum ideal. Se inspeccionaron
nombres/transforms del GLB para construir ejes; no se comprobó el render ni
la interacción de teléfono. Sin navegador, tests/validadores independientes,
serie, flasheos o pruebas físicas. Compilar no acredita aceptación visual ni
contacto/dinámica de ruedas; flash/SPI/layout3MiB físico sigue sin confirmar.