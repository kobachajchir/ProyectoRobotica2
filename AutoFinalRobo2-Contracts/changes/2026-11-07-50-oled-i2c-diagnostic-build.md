# AFR2-20261107-50 — Compilación de la prueba OLED/I2C para descartar hardware

07/11/2026. Pedido del usuario: recuperar la prueba de OLED/I2C usada cuando se revisó el bus y la dirección, y compilarla para descartar hardware o cableado como causa de los arranques irregulares de la OLED.

Estado real: atmega = imagen de servicio compilada y cargada en el Mega (COM16), sin resultados físicos todavía; esp/web = sin cambios. El firmware del auto (`build/afr2.hex`, AFR2-20261107-49) no se tocó.

## Qué es

`AutoFinalRobo2-Atmega/service/i2c_uart0_diagnostic.c`, versión AFR2-20261029-18 (banner `AFR2 SERVO+IR+ENC DIAG AFR2-20261029-18`). Imagen aparte, sin menú, EEPROM, enlace con el ESP ni motores. Busca direcciones I2C en D20/D21 cada 10 s (y con `R`), inicia la OLED en 0x3C a 180° con un contador del encoder y PRESIONADO/SUELTO, lee IR1..IR5 en crudo y mueve el servo D13 con el encoder. Salida por USART0 (USB del Mega) a 115200 8N1. Guía: `hardware/OLED_I2C_DIAGNOSTICO.md`.

## Cómo se compiló

En esta PC no hay `make`, así que `tools/flash-i2c-encoder-diagnostic.ps1` no corre tal cual. Se replicó el target `i2c-diagnostic-firmware` del `Makefile` con avr-gcc 7.3.0: mismas fuentes (`I2C_DIAG_SOURCES`, incluido el driver `src/display/afr2_oled.c` del working tree), `-DAFR2_TWI_INTERNAL_PULLUPS=1 -DAFR2_OLED_STARTUP_MS=500u`, `CFLAGS` sin `-fstack-usage -MMD -MP`, `-Wl,--gc-sections,-Map,build/afr2-i2c-diagnostic.map` y `avr-objcopy -O ihex -R .eeprom`.

## Resultado

- Sin errores ni avisos.
- Flash: 14 554 B (5,6 %). SRAM estática: 3 198 B (39,0 %).
- `build/afr2-i2c-diagnostic.hex`: 40 964 B, SHA-256 `a4c1d5ccdc456a3c199e3e737d340669c7524489fb197e6f11438e3a3d143a69`.

## Carga

Pedido posterior del usuario: «cargalo con el comando, COM16». El script no corre sin `make`, así que se usó directamente su mismo comando de avrdude 6.3.0-arduino17: `-p atmega2560 -c wiring -P COM16 -b 115200 -D -U flash:w:build/afr2-i2c-diagnostic.hex:i`, con verificación por defecto y sin borrar EEPROM, fuses ni bootloader. Firma 0x1e9801, 14 554 B escritos y verificados, fusibles E:FF H:D8 L:FF sin cambios. El Mega ya no tiene el firmware del auto.

## Pendiente

Observar la OLED y el monitor serie con varios arranques en frío, y volver a cargar `build/afr2.hex` al terminar. Si la falla aparece también con esta imagen, apunta a hardware o alimentación; si no aparece, al firmware del auto.
