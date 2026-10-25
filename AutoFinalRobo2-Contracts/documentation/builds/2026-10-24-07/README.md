# Compilaciones — AFR2-20261024-07

24/10/2026. Se compiló por autorización humana previa para alinear los tres
módulos y corregir errores. No se ejecutaron tests, validadores independientes,
monitor serie, reinicios, movimiento, imagen LittleFS ni flasheos.

| Módulo | Resultado final | Tamaño |
| --- | --- | --- |
| ATmega | make firmware communication-firmware library, exit0 | .text53182+.data452=53634bytes flash; SRAM estática452+3834+9=4295/8192bytes |
| ESP | PlatformIO esp12s_candidate, exit0 | RAM45476/81920; programa419945/1044464bytes |
| Web | npm run build:esp (tsc, Vite, paquete, copy:esp), exit0 | 7activos;1452387bytes gzip; estimación FS1540096bytes |

AVR usa avr-gcc7.3.0-atmel3.6.1-arduino7 y make de AtmelStudio7.0. Se recompiló
tras ajustar el buffer del nombre de error OLED; communication-firmware ya
estaba al día. ESP no necesitó más cambios tras su último build correcto.
Dos avisos de indentación del primer build ESP se corrigieron antes del build
final. Las compilaciones finales no muestran advertencias del compilador.

LittleFS candidato `eagle.flash.4m3m.ld`; presupuesto de paquete2867200bytes.
Estimación incluye redondeo por archivo y reserva; no sustituye imagen real ni
identificación física de flash. La SRAM estática no prueba el peor uso de pila.
`AFR2_SEQUENCE_MEMORY_APPROVED=0` y `AFR2_HALL_CONNECTED=0` se conservan.

- [AVR log](atmega-build.txt) y [secciones ELF](atmega-size.txt).
- [ESP log](esp-build.txt).
- [Web log](web-build.txt), [manifest final](manifest.json) y [resumen/hashes](summary.json).

Modelo final fuente827232bytes, SHA256
`d10684704200caa4da0a6a98715e52abc446b9dee9cccdde32f33ee05fc927c7`.
GLB sin editar ni duplicar; WebP/Draco locales conservados. Manifiesto Web y
promovido al ESP son idénticos; detalles en summary.json.

La herramienta de navegador rechazó localhost por falta de autorización; no
se intentó otro navegador/superficie para eludirlo. Apoyo del modelo, composición,
interacción, estados y contraste visual quedan **No verificados**. Fuente y
compilación no acreditan funcionamiento de placas ni calidad visual final.
