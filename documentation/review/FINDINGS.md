# Hallazgos y brechas de migración

Son límites observados/diseñados, no fallos físicos comprobados. Se conserva el registro AMDOC original en reference/automicro sin editarlo.

<a id="afrdoc-001"></a>
## AFRDOC-001 - Desactivar DMA no produce un port asíncrono

Rama sin DMA usa mem_write bloqueante; backend TWI nuevo pendiente.

Código: `atmega:reference/f4/Core/Src/ssd1306.c:ssd1306_UpdateScreen_H`. [Informe afectado](../../../AutoFinalRobo2-Atmega/docs/modules/i2c-oled.md). Estado abierto; fuente sin ejecución.

<a id="afrdoc-002"></a>
## AFRDOC-002 - GOTO heredado depende de MPU/balance

Cambiar planificador/guardas para odometría Mecanum, no falsear señales ready.

Código: `atmega:reference/f4/Core/Src/auto_motion.c:ready`. [Informe afectado](../../../AutoFinalRobo2-Atmega/docs/modules/motion.md). Estado abierto; fuente sin ejecución.

<a id="afrdoc-003"></a>
## AFRDOC-003 - Encoder UI distinto de los Hall motrices

UI D40-D42 propuesta; Hall de cuatro ruedas siguen sin asignar.

Código: `atmega:reference/f4/Core/Inc/encoder.h:ENC_Task_N10ms`. [Informe afectado](../../../AutoFinalRobo2-Atmega/docs/modules/encoder-ui.md). Estado abierto; fuente sin ejecución.

<a id="afrdoc-004"></a>
## AFRDOC-004 - Captura ECHO D31 exige estrategia específica

No asumir entrada de captura/INT/PCINT; resolver timing o recableado.

Código: `atmega:include/board_pinmap.h:AFR2_HCSR04_ECHO_BIT`. [Informe afectado](../../../AutoFinalRobo2-Atmega/docs/modules/hcsr04.md). Estado abierto; fuente sin ejecución.

<a id="afrdoc-005"></a>
## AFRDOC-005 - Recursos F4 superan supuestos de AVR

Presupuesto SRAM/pila/flash, PROGMEM y colas por cerrar.

Código: `atmega:reference/f4/Core/Inc/uner_handle_types.h:UNER_Handle`. [Informe afectado](../../../AutoFinalRobo2-Atmega/docs/modules/protocol.md). Estado abierto; fuente sin ejecución.

<a id="afrdoc-006"></a>
## AFRDOC-006 - IR8/ADC/emisores no equivalen a línea5 OSOYOO

Confirmar salida/polaridad; retirar objetos IR y no fabricar distancias.

Código: `atmega:reference/f4/Core/Inc/tcrt5000_types.h:TCRT5000_Port`. [Informe afectado](../../../AutoFinalRobo2-Atmega/docs/modules/line-ir.md). Estado abierto; fuente sin ejecución.

<a id="afrdoc-007"></a>
## AFRDOC-007 - Reset Mega no conecta USB con UART1

Shield documentado con regulación/traducción y KEY/RST. Reset Mega no une USB con USART1; selector normal E_RX-D1/E_TX-D0 no corresponde al bypass USB D0-RX ESP/TX ESP-D1. Ruta física de niveles y conexión real pendientes. [Procedimiento afectado](../../hardware/FLASHING.md). Se conserva el estado abierto y el historial en JSON.

Código: `esp:reference/esp01/src/main.cpp:setup`. [Informe afectado](../../../AutoFinalRobo2-ESP/docs/modules/wifi.md). Estado abierto; fuente sin ejecución.

<a id="afrdoc-008"></a>
## AFRDOC-008 - UNER sin secuencia exige política para respuesta tardía

Definir cuarentena/resync o versión con correlación; evitar ACK viejo para pedido nuevo.

Código: `esp:reference/esp01/src/uart_transaction_manager.c`. [Informe afectado](../../../AutoFinalRobo2-ESP/docs/modules/uart.md). Estado abierto; fuente sin ejecución.

<a id="afrdoc-009"></a>
## AFRDOC-009 - Raster Web y OLED nuevo no acreditan paridad F4

Referencia histórica tiene distinta semántica Inverse y datos parciales; la base nueva declara XOR.

Código: `web:reference/web/src/screens`. [Informe afectado](../../../AutoFinalRobo2-Web/docs/modules/oled-ui.md). Estado abierto; fuente sin ejecución.

<a id="afrdoc-010"></a>
## AFRDOC-010 - Navegación se cambia a un único SW

Corto confirma, largo abre menú, atrás solo ítem; sin user_button dedicado.

Código: `atmega:src/input/afr2_encoder_ui.c:Afr2EncoderUi_Poll`. [Informe afectado](../../../AutoFinalRobo2-Atmega/docs/modules/screens.md). Estado abierto; fuente sin ejecución.

<a id="afrdoc-011"></a>
## AFRDOC-011 - Mapa del shield referido a UNO no define pines del Mega

Imagen muestra A4/A5 y D18/D19 como I2C UNO; en Mega se conservan D20/PD1 y D21/PD0, A4 es IR1. Selectores a D4/D5 se aíslan: D5 es motor RR.

Código afectado por la futura adaptación: `atmega:include/board_pinmap.h`. [Documento afectado](../../hardware/PINOUT.md), sección Adiciones acordadas y conectores. Abierto; documentación, no prueba física. AFR2-20261023-03.

<a id="afrdoc-012"></a>
## AFRDOC-012 - Puerto directo ESP y selector no tienen tolerancia eléctrica demostrada equivalente

Regulación/traducción integradas declaradas por OSOYOO, pero la ruta interna por conector no está acreditada. Puerto directo rotulado 3V3. Fritzing no contiene netlist interna suficiente.

Código afectado por la futura adaptación: `esp:platformio.ini`, `esp:src/main.cpp`. [Documento afectado](../../hardware/WIFI_SHIELD.md), sección Alimentación y niveles. Abierto; documentación, no prueba física. AFR2-20261023-03.

<a id="afrdoc-013"></a>
## AFRDOC-013 - Datos nominales ESP y AT antiguo no fijan flash ni baud del producto

ESP-12S V1.1 nominal 4 MiB; variante ESP8285/2MB diferenciada. Lectura real, modo SPI/layout pendientes. Baud AT de fábrica, UART propia y upload son distintos; DOUT en archivo AT no prueba modo para firmware nuevo.

Código afectado por la futura adaptación: `esp:platformio.ini`, `esp:src/main.cpp`. [Documento afectado](../../hardware/WIFI_SHIELD.md), sección Firmware, velocidad y flash. Abierto; documentación, no prueba física. AFR2-20261023-03.
