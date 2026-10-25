# Compilación de la alineación AFR2-20261024-02

24/10/2026. El usuario autorizó compilar los tres módulos y corregir errores.
Los comandos siguientes terminaron con salida 0. No se ejecutaron tests,
validadores independientes, navegador, firmware, puertos serie ni flasheos.

| Módulo | Comando | Resultado final |
|---|---|---|
| ATmega | make firmware communication-firmware library | AVR C/avr-libc; ELF, HEX, mapa y biblioteca; sin warnings/errors finales |
| ESP | python de PlatformIO -m platformio run -e esp12s_candidate | BIN/ELF; sin warnings/errors finales; perfil candidato |
| Web | npm run build:esp | TypeScript, Vite, postbuild gzip y promoción ESP/data/dist; sin errores |

Toolchain AVR: Arduino avr-gcc 7.3.0-atmel3.6.1-arduino7 y GNU make de Atmel Studio.
ESP: espressif8266 4.2.1, Arduino Core 3.1.2, Xtensa GCC 10.3.0, ESPAsyncTCP 2.0.0,
ESPAsyncWebServer 3.10.3 y ArduinoJson 7.4.2. Web: TypeScript 5.9 y Vite 7.3.7
del lockfile instalado; no se actualizaron dependencias.

## Memoria del build

- AVR producto: .text 50.258 B; .data 448 B; .bss 3.808 B; .noinit 1 B.
  Flash programada (.text+.data): **50.706 B**. SRAM estática: **4.257/8.192 B**;
  quedan **3.935 B** para pila y margen, sin medición dinámica.
- AVR comunicaciones: .text 3.800 B, .data 6 B, .bss 462 B. Es una composición
  de diagnóstico separada; no sustituye el firmware de producto con EX01.
- Máximo frame propio reportado en .su: main 152 B. Hay llamadas anidadas,
  callbacks, avr-libc/libm e ISR: **no es una cota de pila total**. GCC no admite
  .su para el inicializador naked de reset, que corre antes de main; esa unidad
  omite solamente -fstack-usage. No se habilita el ejecutor de secuencias.
- ESP: RAM estática **44.396/81.920 B** y programa **414.281/1.044.464 B** del
  perfil candidato. No es medición de heap bajo clientes/carga ni identificación
  de la flash/SPI de la unidad real.
- Web: **655 módulos**, index.html 1.703,58 kB (467,30 kB gzip según Vite),
  **6 assets, 2.897.959 B gzip** en el manifiesto completo. Paquete promovido a
  `AutoFinalRobo2-ESP/data/dist`; sin imagen FS. El layout predeterminado de la
  placa candidata sigue siendo insuficiente para ese bundle: elegir layout común
  solo tras identificar flash/SPI. No se recortaron recursos por ese supuesto.

## Correcciones y archivos

[AVR](atmega-build.txt) · [ESP](esp-build.txt) · [Web y promoción final](web-build.txt) ·
[Primer build AVR](atmega-initial.txt) · [Primer build Web](web-initial.txt) ·
[Primer build ESP](esp-initial.txt) · [Inventario de artefactos](results.json).

Primer build Web: TS7053 al indexar nombres de pantallas ampliadas. Se sustituyó
por búsqueda de metadata por código; los códigos nuevos requieren framebuffer.
Primeros builds AVR/ESP: warnings de indentación de condicionales consecutivos,
corregidos sin desactivar Wall/Wextra. El empaquetador usa su fallback Windows
EPERM por archivos, con manifiesto al final; no garantiza intercambio atómico
de todo el directorio. La promoción a ESP terminó correctamente.

Secuencias físicas: AFR2_SEQUENCE_MEMORY_APPROVED=0. La revisión automática
rechazó activarlo sin aprobación específica SRAM/pila/calibración. Hall conserva
AFR2_HALL_CONNECTED=0. Compilación no acredita comunicación eléctrica, calibración,
gestos ni movimiento real; la siguiente etapa física necesita autorización expresa.
