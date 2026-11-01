# OLED sin pausas artificiales y radar nominal — compilado, SIN carga

AFR2-20261029-16. Usuario pidió explícitamente «No lo subas aun, continua con
lo siguiente» antes de solicitar flash. No se creó job de carga16. COM8 conserva
15, verificado a12:02:57. La autorización previa de carga al terminar queda
pospuesta por esta instrucción; esperar nueva indicación antes de flashear.

Build validado `12004dc5a7504dcbacdc832763cd6fa1`,29/10/2026 12:15:28,
producto/comunicación/biblioteca, exit0 sin errores/advertencias.
Primer build `b8685d7278fd49e1ac9944bfab3b6307`: compilador exit0, preparación
rechazada por fingerprint de entradas cambiado; los .su generados están
incluidos por el Loader. Segundo build validó preparación; ninguna escritura.

- ELF text74390+data1128=75518bytes flash.
- SRAM data1128+bss5790=6918, margen1274bytes.
- .su main259, renderer73 dynamic,bounded, line29, hit19. Valores individuales,
  no máximo dinámico de llamadas/ISR ni certificación de memoria suficiente.
- HEX SHA256 `2c06560559618aa89ab94d663ce0b567803fdf865148b8835d222478ab49b6be`.

Sin tests, validadores ni ensayo físico del agente. Web fuente vinculada, no
compilada ni promovida a ESP. No se mide tiempo real ni se valida haz acústico.

[Primer build rechazado](atmega-build-first-rejected.txt) ·
[Build validado](atmega-loader-build.txt).
