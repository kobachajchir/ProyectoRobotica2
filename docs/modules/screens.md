# Wrappers, menús y pantallas

[Maestro](../../../INFORME_MAESTRO.md) · [Informe del repositorio](../../INFORME_ADAPTACION.md).

## Utilidad

Wrappers preparan datos/contexto; renderer dibuja; navegación decide pantalla/selección; acciones llaman servicios. Reutilizar conceptos y recursos, no System/globales completos. La referencia incluye menús/OSK/avisos y toda la biblioteca propia para port posterior.

## Adaptación

Nueva UI usa snapshot explícito sin consultas de hardware en renderer. Mantener identidad/rol/estado/acciones por separado. Largo SW abre menú; corto selecciona; atrás solo por ítem Volver. Retirar llamadas al botón menú y no reinterpretar largo como atrás. Main procesa acciones y respeta valid/unknown. Llevar menús/textos a flash; decidir contexto de edición/confirmación sin ejecutar motor al navegar. Skill en Contracts documenta cada pantalla; paridad Web pendiente.

## Evidencia y alcance

- [reference/f4/Core/Src/screenWrappers.c](../../reference/f4/Core/Src/screenWrappers.c)
- [reference/f4/Core/Src/oled_utils.c](../../reference/f4/Core/Src/oled_utils.c)
- [reference/f4/Core/Src/menusystem.c](../../reference/f4/Core/Src/menusystem.c)
- [reference/f4/Core/Inc/screen_context.h](../../reference/f4/Core/Inc/screen_context.h)
- [src/ui/afr2_screens.c](../../src/ui/afr2_screens.c)

Hallazgos: AFRDOC-009, AFRDOC-010. [Registro](../../../AutoFinalRobo2-Contracts/documentation/review/FINDINGS.md).

Estado: lectura/copia o fuente parcial indicada; sin compilación, tests ni prueba física. La API detallada de origen está en el corpus AMDOC importado; no equivale a API AVR adaptada.
