# IR digital — diagnóstico y producto compilados SIN carga

AFR2-20261029-18. Se mantiene «No lo subas aun». No se creó job de carga,
no se abrió COM ni se hicieron pruebas físicas. Placa conserva producto15.

## Diagnóstico

Script flash-i2c-encoder-diagnostic.ps1 -Port COM8 -BuildOnly: make -B
i2c-diagnostic-firmware, exit0 sin errores/advertencias. BuildOnly retorna antes
de avrdude; seleccionar COM8 como parámetro no abre ese puerto.

- afr2-i2c-diagnostic.hex SHA256
  `0d71397dbd89e4b4eefe355577c358ef02f545f0adbb710bdf6e1133887fc1f2`.
- ELF text13798, data2, bss3178: flash13800, SRAM3180, margen5012bytes.
- Lee directamente PF4..PF0 en orden IR1..5, sin EEPROM/filtro de seguimiento.
- [Registro](diagnostic-build.txt).

## Producto

Loader1.1.2, build_mega validado `35186db8b38a4d8aac4f7ae69faba447`,
29/10/2026 12:34:40, producto/comunicación/biblioteca, exit0 sin advertencias.
Primer build `fc14c7dd5c68407e9d9245f192810260` compiló exit0, pero preparación
rechazada por cambio de fingerprint (.su generados incluidos por el Loader).
Se repitió para obtener preparación validada, sin solicitar flash.

- afr2.hex SHA256
  `99f63514f4c1adaa8cd6a0b228e65e8c5eefe7e84795149144f7927e0f790ea7`.
- ELF text74456, data1144, bss5790: flash75600, SRAM6934, margen1258bytes.
- .su main259, Sensors_Task24bytes. Son valores individuales, sin límite de
  cadena completa/ISR ni prueba dinámica de SRAM suficiente.
- [Build rechazado](product-build-first-rejected.txt) ·
  [Build validado](product-loader-build.txt).

Sin tests/validadores ni medición de las entradas. Web adaptada en fuente y
fingerprint revisado tras portar el mensaje de estado; sin build/promoción ESP.
La corrección no prueba el valor actual de EEPROM ni la respuesta física IR.
