# Nuevo driver HC-SR04

AFR2-20261023-19, fuente integrada sin compilar/probar: Timer3 CTC25us, espera subida/bajada/timeout32ms en D31; servo D13/Timer5 opcional, sin barrido. Tiempos físicos sin medir.
[Producto vigente](../PRODUCTO_ATMEGA.md). El diseño histórico siguiente conserva su evidencia.


[Maestro](../../../INFORME_MAESTRO.md) · [Informe del repositorio](../../INFORME_ADAPTACION.md).

## Utilidad

No se localizó un driver reusable directo en la fuente F4. El pinout recibido asigna TRIG D30 y ECHO D31.

## Adaptación

Implementar idle, trigger, wait-rise, wait-fall, publish y timeout con reloj fino; no delay/pulseIn. D31 requiere muestreo periódico de resolución suficiente o recableado a captura con ficha. Publicar distancia/validez/edad/error; timeout no significa camino libre. Si se usa servo D13, medir solo después de estabilización y marcar orientación. Cobertura frontal no protege lateral/retroceso.

## Evidencia y alcance

- [include/board_pinmap.h](../../include/board_pinmap.h)
- [../AutoFinalRobo2-Contracts/hardware/supplied/Pinout_OSOYOO_Mecanum.csv](../../../AutoFinalRobo2-Contracts/hardware/supplied/Pinout_OSOYOO_Mecanum.csv)

Hallazgos: AFRDOC-004. [Registro](../../../AutoFinalRobo2-Contracts/documentation/review/FINDINGS.md).

Estado: lectura/copia o fuente parcial indicada; sin compilación, tests ni prueba física. La API detallada de origen está en el corpus AMDOC importado; no equivale a API AVR adaptada.
