# Backend TWI AVR escrito — AFR2-20261023-15

`afr2_twi.c`/`include/afr2_twi.h`: reemplazo nuevo C/avr-libc de la dependencia
HAL/DMA F4. El define sin DMA F4 conserva mem_write bloqueante y no se usa.
Un master, OLED único; start/poll/abort implementan Afr2I2cHardware para Manager.
TWI_vect avanza START, SLA+W y DATA. Buffer prestado estable, resultado diferido
a main. poll solo informa OK al despejarse TWSTO, sin esperar otra IRQ de STOP.
NACK/bus error fallan; arbitraje perdido libera sin emitir STOP como master.

Manager impone deadline40 ms. abort desactiva TWCR/IRQ y suelta punteros antes
de retornar. Afr2Twi_Task recupera bus en pasos de1 ms en main: comprueba SCL,
hasta9 pulsos y STOP open drain; total<=50 ms. Nunca conduce línea alta ni
habilita pull-ups internos I2C. Si falla, deja líneas liberadas; siguiente
reintento OLED agenda nueva recuperación. Sin spin/delay, DMA ni callbacks ISR.
Multi-master no soportado. Los límites son política fuente sin medición.

Referencia de registros/estados: [Microchip ATmega2560, sección24](https://ww1.microchip.com/downloads/aemDocuments/documents/OTH/ProductDocuments/DataSheets/ATmega640-1280-1281-2560-2561-Datasheet-DS40002211A.pdf).
Bitrate100 kHz/16 MHz, sin detección de controlador/dirección. SSD1306 0x3C es
candidato; tensión, pull-ups y conexión física aún pendientes. Ver
[composición](../../docs/COMPOSICION.md). Fuente no compilada ni probada; no se
ejecutaron validadores/tests/flasheos ni se acredita recovery físico.
