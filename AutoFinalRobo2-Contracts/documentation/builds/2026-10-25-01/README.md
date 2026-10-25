# Compilación — control Web directo7

25/10/2026, AFR2-20261025-01. Compilación autorizada por el pedido previo de
compilar/corregir la integración. Web build:esp, ESP platformio run y AVR make
firmware communication-firmware library: exit0. No errores finales. Los últimos
cambios de wrapper OLED y copia Web están incluidos; recursos regenerados.

| Resultado | Valor |
| --- | --- |
| Firmware / Web / EX01 |0.9.0 /0.2.0 /7 |
| Comandos / pantallas |30 /59 |
| Activos / gzip |7 /1479980 bytes |
| Raw |3035543 bytes |
| LittleFS estimado / presupuesto |1564672 /2867200 bytes |
| AVR .text/.data/.bss/.noinit |62966 /798 /4072 /9bytes |
| AVR flash / SRAM estática incl.noinit |63764 /4879bytes |
| ESP RAM / flash |46116 /423273bytes |

[Web](web-build.txt), [ESP](esp-build.txt), [AVR](atmega-build.txt),
[secciones AVR](atmega-size.txt), [manifest](manifest.json), [resumen](summary.json).
Hashes de los siete activos coinciden entre paquete y ESP/data/dist. Promoción
local, no flasheo. Ambos firmwares y Web requieren control directo7 juntos.

Se eliminan permiso/token de acceso/operaciones de adquisición y habilitación
OLED. Presencia500ms/caducidad1500ms no concede derechos ni inicia/reanuda salidas.
STOP, watchdog, TTL, límites/calibración y correlación preservados. Tabla C/TS y
catálogo coinciden en59identidades, Control4ítems/Sistema7, IDs2/3/56 retirados.

Solo compilación y lectura de fuente/artefactos. Sin tests, validadores
independientes, navegador, raster diferencial, serie, reinicios, imagen FS,
flasheos o pruebas físicas. Memoria/Hall0 y rama habilitada sin compilar/probar;
SRAM estática no certifica pila/ISR/tiempos. Layout3MiB candidato, flash/SPI
físicos sin identificar. Sin CAD, commit o publicación.