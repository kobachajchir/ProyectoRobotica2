# AutoFinalRobo2-atmega

Leer [informe de adaptación](INFORME_ADAPTACION.md) y [maestro](../INFORME_MAESTRO.md).

Se copió todo Core propio F4 como referencia autocontenida de librerías y composición; HAL/CMSIS/USB middleware y build STM32 no se migran. MPU, balance, AT y botón menú permanecen solo para localizar acoplamientos históricos. La base src/ contiene una adaptación parcial nueva, no el firmware del vehículo.

Código nuevo: services/afr2_i2c_manager.c, display/afr2_oled.c y font7x10.c, input/afr2_encoder_ui.c y ui/afr2_screens.c. API pública en include/. El Manager está reducido a un OLED, con start/poll/abort y callbacks en main. Wrappers usan snapshots. Fuente 7x10 con PROGMEM. Encoder largo/corto excluyentes y Volver por ítem. Falta backend TWI, main/reloj/GPIO/UART, sensores/motores/odometría/GOTO.

Makefile tiene ayuda por defecto y objetivo library para producir biblioteca con avr-gcc cuando se autorice. No hay .hex ni upload; no se ejecutó make. [Composición](docs/COMPOSICION.md).

[Contrato compartido](../AutoFinalRobo2-Contracts/README.md). No se han ejecutado compilaciones, tests, validaciones ni flasheos.
