# Asistente Hall y guardado — AVR compilado, SIN carga

AFR2-20261029-20,29/10/2026. Se conserva pedido «No lo subas aun»:
no job de flash, puerto no abierto y ningún ensayo físico. Placa conserva15.
La compilación sigue la autorización previa de compilar al terminar.

Loader1.1.2, build validado `2da07fba389f4994aabb567f5cbcea17`, producto,
comunicación y biblioteca, exit0 sin errores/advertencias, final13:00:16.
Primer build `ccc4a9bb1cc546ba90f9f5aae3f3f598`: compilador exit0, preparación
rechazada por fingerprint modificado; .su generados están incluidos por Loader.
Se repitió para validar preparación; no se intentó carga.

- ELF text78806, data1260, bss5825: flash80066bytes.
- SRAM estática7085/8192bytes, margen1107bytes.
- .su main276, Ui_Render133 dynamic,bounded, invoke de App151, Ui_Apply85.
  Valores individuales; no máximo de cadena/ISR ni prueba dinámica de pila.
- HEX SHA256 `934532ce942375f3da715dc8c2e0f5105d415e62f6d5adb22e254b463eccd18f`.

Incluye fuentes existentes de presentación táctil19 conservadas; fuente común
del producto18/16 e investigación Hall20. No recompila el diagnóstico separado18;
su HEX sigue identificado por su propia evidencia. No se modificó Loader.

Web tiene renderer/preview/catálogo fuente vinculados y hashes revisados;
sin compilación Web ni promoción ESP. El generador de recursos de Web queda
para su siguiente build autorizado. No se ejecutaron tests, validadores ni
mediciones físicas de rueda, EEPROM o pantalla. La compilación no comprueba
que el operador haya dado una vuelta ni que los cambios persistan en placa.

[Build primero rechazado](build-first-rejected.txt) ·
[Build validado](atmega-loader-build.txt).
