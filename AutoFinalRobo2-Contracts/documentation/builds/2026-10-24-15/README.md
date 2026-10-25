# Compilación y revisión — AFR2-20261024-15

24/10/2026. Pedido previo expreso de compilar la integración conservado.
Web npm run build:esp y ESP platformio run -e esp12s_candidate: exit0.
TypeScript/Vite, recursos OLED, minificación JS/CSS, gzip y copia local final
completados sin errores. Se recompiló tras los ajustes finales de fuente.

| Resultado | Valor |
| --- | --- |
| Activos Web | 7 |
| Raw / gzip | 3038046 / 1480773 bytes |
| Estimación LittleFS / presupuesto | 1564672 / 2867200 bytes |
| Margen estimado | 1302528 bytes |
| ESP RAM / flash | 46268 /425809bytes |
| AVR conservado de12, flash / SRAM estática |64406 /4891bytes, incluyendo noinit |

[Log Web](web-build.txt), [log ESP](esp-build.txt), [manifest](manifest.json),
[resumen](summary.json). Lectura de hashes registra igualdad de los siete
activos entre paquete y ESP/data/dist; promoción local, sin cargar una placa.
Metadatos opcionales: versión nominal Web 0.1.0, empaquetado
24/10/2026 23:47:14 (America/Buenos_Aires). No equivale a compilación/identidad de placas.

AVR sin nueva fuente ni compilación15; conserva [build12](../2026-10-24-12/README.md),
incluye IRQ11/distancia12 con Hall/memoria0. Rama física habilitada no compilada;
memoria estática no certifica pila/ISR/tiempos. No habilitar gates por el informe.
Modelo/recursos/ScreenCodes/framing sin cambios. FS nominal3MiB candidato;
flash/SPI físicos sin identificar. No nuevo activo/dependencia ni CAD geométrico.

Matriz33comandos/59identidades contrastada por lectura de fuente/contratos,
no mediante un validador automatizado. Sin tests, navegador, paridad diferencial,
serie, reinicios, pruebas físicas, imagen FS, upload o publicación. Se conserva
la denegación previa de navegador. No se declara aceptado todo el plan maestro.
