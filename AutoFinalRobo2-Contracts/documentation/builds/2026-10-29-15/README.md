# Radar sin bucle y Hall instalado — COM8

AFR2-20261029-15. Build validado `bcf080ac9ac14e17ad650a956076c42e`,
producto/comunicación/biblioteca, exit0 sin errores/advertencias. Primer build
`bc14f34ce65146af8afca3ba7ef9abf0` tuvo compilador exit0, pero el Loader rechazó
la preparación al cambiar el fingerprint de entradas (.su generados incluidos).
Se repitió antes de cargar. No se escribió flash en el intento rechazado.

Carga `12981f5db5e542f2be21cc394530c287`, COM8, final29/10/2026 12:02:57,
Loader1.1.2 con autoAcceptRequests y skipManualChecks ya habilitados. Firma
0x1e9801,74796bytes escritos12.00s y verificados9.60s, exit0. Fuses FF/D8/FF
conservados; sin órdenes de escritura EEPROM. La migración Hall actúa en RAM
al arrancar; no se interrogó el estado resultante por UART.

HEX SHA256 `05578b41ac526dca6c890eff81c7c9aec9e3e729b23267170e7d6e74daf7614f`.
ELF: text73668, data1128, bss5795. Flash74796; SRAM estática6923, margen1269.
.su main259; renderer70 dynamic,bounded; line29. No certifican pila máxima.

Sin tests/validadores ni ensayo físico posterior del agente. Web fuente vinculada,
sin build/promoción ESP; ESP y Loader no modificados. El usuario informó mayor
latencia después: se atiende en AFR2-20261029-16. La verificación flash no prueba UI.

[Build rechazado](atmega-build-first-rejected.txt) ·
[Build validado](atmega-loader-build.txt) · [Flash](atmega-com8-flash.txt).
