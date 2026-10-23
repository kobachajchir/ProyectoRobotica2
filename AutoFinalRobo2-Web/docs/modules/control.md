# Control, TEST, línea y GOTO

[Maestro](../../../INFORME_MAESTRO.md) · [Informe del repositorio](../../INFORME_ADAPTACION.md).

## Utilidad

Flujos de control, paneles de pruebas, calibración y reportes son referencia visual y funcional. Balance y calibración IMU no corresponden a este robot.

## Adaptación

Cuatro ruedas y control lateral Mecanum, diagnóstico independiente por rueda, cinco IR y HC-SR04 con validez. GOTO requiere odometría y muestra estimación; quitar guardas/indicadores MPU y crear requisitos reales. Prohibir habilitar controles por datos simulados. No mapear botones Obstacle/Tracking del proveedor como modos autónomos: el PDF muestra desplazamientos laterales.

## Evidencia y alcance

- [reference/web/src/features/autoMotion](../../reference/web/src/features/autoMotion)
- [reference/web/src/features/motion](../../reference/web/src/features/motion)
- [reference/web/src/features/lineFollow](../../reference/web/src/features/lineFollow)
- [reference/web/src/pages](../../reference/web/src/pages)

Hallazgos: AFRDOC-002, AFRDOC-006. [Registro](../../../AutoFinalRobo2-Contracts/documentation/review/FINDINGS.md).

Estado: lectura/copia o fuente parcial indicada; sin compilación, tests ni prueba física. La API detallada de origen está en el corpus AMDOC importado; no equivale a API AVR adaptada.
