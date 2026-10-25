> AFR2-20261025-01: [control directo7](../../../AutoFinalRobo2-Contracts/contracts/DIRECT_CONTROL_7.md). Se eliminan todas las operaciones y estados de permiso y la habilitación OLED. Las acciones se envían directamente; solo permanecen conexión/capacidades, límites, estado ocupado, parada/caducidad y calibración. Fuente compilada; runtime pendiente. Estados inferiores históricos.

> AFR2-20261024-15: Calibración adquiere control al aplicar/guardar/descartar y comprueba salidas apagadas mediante consulta nueva. Parámetro previamente leído; STOP/foco/enlace/tarea cancelan pasos pendientes. No calibración Hall remota ni nuevo permiso AVR. [Matriz](../../../AutoFinalRobo2-Contracts/docs/ALINEACION_MAESTRO.md).

> Vigente13: [Control táctil](manual-touch-drive.md) abre desde Conducción manual. Mismo contrato, modelo de referencia y parada supervisada; fuente compilada, teléfono/placas sin probar.

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
