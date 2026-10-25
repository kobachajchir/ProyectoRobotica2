# Evidencia de compilación — AFR2-20261024-08

24/10/2026. Autorización: pedido humano previo de compilar y corregir errores
al integrar los módulos. No se ejecutan tests ni hardware por compilarlos.

| Módulo | Comando / resultado | Tamaño final |
| --- | --- | --- |
| ATmega | make firmware communication-firmware library, exit0 | Flash64232bytes; SRAM estática4797/8192bytes |
| ESP | PlatformIO run -e esp12s_candidate, exit0 | RAM45852/81920; programa423665/1044464bytes |
| Web | npm run build:esp (tsc/Vite/paquete/copy:esp), exit0 | 1452714bytes gzip,7activos; FS estimado1540096bytes |

No hay errores ni advertencias de compilador en los logs finales. AVR se
recompiló tras reducir redibujos completos durante scroll. ESP y Web pasaron
en su compilación final. Runtime de placas y aspecto de OLED no comprobados.

- [Log AVR](atmega-build.txt), [secciones](atmega-size.txt), [recursos de flash](avr-flash-resources.txt) y [frames de pila](avr-stack-frames.txt).
- [Log ESP](esp-build.txt).
- [Log Web](web-build.txt), [manifest](manifest.json), [resumen/hashes](summary.json).

AVR .text63422, .data810, .bss3978, .noinit9. Datos estáticos4797, margen3395;
no es máximo de pila. .su main237 y renderer120(dynamic,bounded), sin prueba
del peor anidamiento/ISR. Iconos/tablas/fuente enumerados terminan en0x3494;
accesores PROGMEM usados no cruzan64KiB. Las macros de memoria/Hall siguen0.

Paquete Web y ESP/data/dist con archivos/manifiesto idénticos (hashes en summary).
LittleFS nominal3MiB sobre flash supuesta4MiB, linker candidato4m3m conservado.
Modelo final827232bytes, SHA256
`d10684704200caa4da0a6a98715e52abc446b9dee9cccdde32f33ee05fc927c7`.

Solo compilación e inspección de fuente/artefactos. Sin suite de tests,
validadores independientes, preview/render, captura física, radio/UART,
reinicios, imagen LittleFS ni flasheo. No se intentó reabrir el navegador
previamente denegado. Memoria/calibración y pruebas físicas continúan pendientes.
