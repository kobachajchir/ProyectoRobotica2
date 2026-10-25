# Cinco sensores de línea

AFR2-20261023-19, fuente integrada sin compilar/probar: Cinco canales PF4..PF0 digital/ADC, mayoría de3 scans, inversión/umbrales y confirmación manual. Tipo/polaridad sin comprobar.
[Producto vigente](../PRODUCTO_ATMEGA.md). El diseño histórico siguiente conserva su evidencia.


[Maestro](../../../INFORME_MAESTRO.md) · [Informe del repositorio](../../INFORME_ADAPTACION.md).

## Utilidad

Reutilizar normalización, estado/calibración, filtrado y separación sensor/control. El origen modela ocho TCRT5000 (línea/objetos), adquisición ADC DMA y emisores por grupo. El módulo OSOYOO de cinco puntos no prueba esas mismas capacidades.

## Adaptación

Confirmar salida digital/analógica y polaridad. A4..A0 son canales de línea, no dirección I2C. Si digital: bitmask de cinco con validez/edad y calibración de polaridad; no inventar raw ADC. Si analógico: ADC10 bits sin DMA, secuencia iniciar/esperar/publicar y umbrales recalibrados. Retirar tres sensores de objetos y control de emisores si no existe cableado. Adaptar seguimiento a cuatro ruedas/robot estable sin balance.

## Evidencia y alcance

- [reference/f4/Core/Src/tcrt5000.c](../../reference/f4/Core/Src/tcrt5000.c)
- [reference/f4/Core/Inc/tcrt5000_data.h](../../reference/f4/Core/Inc/tcrt5000_data.h)
- [reference/f4/Core/Src/line_follow_control.c](../../reference/f4/Core/Src/line_follow_control.c)
- [reference/f4/Core/Inc/line_calibration.h](../../reference/f4/Core/Inc/line_calibration.h)

Hallazgos: AFRDOC-006. [Registro](../../../AutoFinalRobo2-Contracts/documentation/review/FINDINGS.md).

Estado: lectura/copia o fuente parcial indicada; sin compilación, tests ni prueba física. La API detallada de origen está en el corpus AMDOC importado; no equivale a API AVR adaptada.
