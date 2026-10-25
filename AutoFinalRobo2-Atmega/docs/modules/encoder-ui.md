> AFR2-20261024-11: [captura IRQ y Hall](../../../AutoFinalRobo2-Contracts/hardware/INTERRUPCIONES_ENCODER_HALL.md); fuente actual sin compilar ni probar.

# Encoder de interfaz y pulsador único

AFR2-20261023-19, fuente integrada sin compilar/probar: Encoder CLK/DT/SW separado de Hall motrices; giro/clic/largo y pruebas de contadores en pantalla, sin prueba física.
[Producto vigente](../PRODUCTO_ATMEGA.md). El diseño histórico siguiente conserva su evidencia.


Estado vigente AFR2-20261023-15: backend/composición integrados en fuente; el texto anterior de migración se conserva debajo como antecedente. Sin compilar/probar.

[Maestro](../../../INFORME_MAESTRO.md) · [Informe del repositorio](../../INFORME_ADAPTACION.md).

## Utilidad

encoder.c F4 procesa un encoder rotativo y su pulsador. Con la ampliación solicitada vuelve a ser una referencia útil para UI; no mide las cuatro ruedas. user_button queda como antecedente del botón eliminado.

## Adaptación

CLK D2/PE4/INT4 y DT D3/PE5/INT5 se capturan por cualquier cambio en una lectura PINE; FIFO32bytes/31útiles y acciones/decodificación en main. Overflow descarta y resincroniza el paso parcial. SW D42/PL7 muestreado con objetivo1ms,20ms debounce,800ms largo exclusivo y supresión al arrancar pulsado. Hall PCINT2 A8–A15 x1, captura pendiente antes de UI; prioridad hardware fija sin ISR anidadas. Hall/memoria0 y fuente sin compilar/probar.

## Evidencia y alcance

- [reference/f4/Core/Src/encoder.c](../../reference/f4/Core/Src/encoder.c)
- [reference/f4/Core/Inc/encoder_types.h](../../reference/f4/Core/Inc/encoder_types.h)
- [reference/f4/Core/Src/user_button.c](../../reference/f4/Core/Src/user_button.c)
- [src/input/afr2_encoder_ui.c](../../src/input/afr2_encoder_ui.c)

Hallazgos: AFRDOC-003, AFRDOC-010. [Registro](../../../AutoFinalRobo2-Contracts/documentation/review/FINDINGS.md).

Estado: lectura/copia o fuente parcial indicada; sin compilación, tests ni prueba física. La API detallada de origen está en el corpus AMDOC importado; no equivale a API AVR adaptada.


AFR2-20261023-15: TWI AVR por IRQ y composición OLED/encoder/USART1 implementados en fuente, sin DMA ni Arduino. Ver docs/COMPOSICION.md y Contracts/changes/2026-10-23-15-robo2-atmega-ui.md. Sin builds/tests/validadores/flasheos; hardware y recursos finales pendientes.
