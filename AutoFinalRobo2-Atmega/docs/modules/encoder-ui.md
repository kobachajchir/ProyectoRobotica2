# Encoder de interfaz y pulsador único

[Maestro](../../../INFORME_MAESTRO.md) · [Informe del repositorio](../../INFORME_ADAPTACION.md).

## Utilidad

encoder.c F4 procesa un mando rotativo y su pulsador. Con la ampliación solicitada vuelve a ser una referencia útil para UI; no mide las cuatro ruedas. user_button queda como antecedente del botón eliminado.

## Adaptación

CLK D40/PG1 y DT D41/PG0 se leen juntos; SW D42/PL7 activo bajo. Inicializar desde niveles reales, evitar gesto de arranque pulsado, rechazar transición cuadratura inválida y ajustar signo/transiciones por detent. Base: 1 ms poll, 20 ms debounce, 800 ms largo exclusivo; ajustar sin asumir hardware probado. El corto sale al soltar solo si no se emitió largo. No usar polling de UI para Hall motriz.

## Evidencia y alcance

- [reference/f4/Core/Src/encoder.c](../../reference/f4/Core/Src/encoder.c)
- [reference/f4/Core/Inc/encoder_types.h](../../reference/f4/Core/Inc/encoder_types.h)
- [reference/f4/Core/Src/user_button.c](../../reference/f4/Core/Src/user_button.c)
- [src/input/afr2_encoder_ui.c](../../src/input/afr2_encoder_ui.c)

Hallazgos: AFRDOC-003, AFRDOC-010. [Registro](../../../AutoFinalRobo2-Contracts/documentation/review/FINDINGS.md).

Estado: lectura/copia o fuente parcial indicada; sin compilación, tests ni prueba física. La API detallada de origen está en el corpus AMDOC importado; no equivale a API AVR adaptada.
