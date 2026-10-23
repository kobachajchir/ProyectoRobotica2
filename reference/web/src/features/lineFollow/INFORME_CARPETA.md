# Carpeta reference/web/src/features/lineFollow

[Maestro](../../../../../../INFORME_MAESTRO.md) · [Informe del repositorio](../../../../../INFORME_ADAPTACION.md).

Fuentes históricas de referencia: no forman parte del producto adaptado.

## Utilidad y reformas específicas

### [Control, TEST, línea y GOTO](../../../../../docs/modules/control.md)

Flujos de control, paneles de pruebas, calibración y reportes son referencia visual y funcional. Balance y calibración IMU no corresponden a este robot.

Reformas: Cuatro ruedas y control lateral Mecanum, diagnóstico independiente por rueda, cinco IR y HC-SR04 con validez. GOTO requiere odometría y muestra estimación; quitar guardas/indicadores MPU y crear requisitos reales. Prohibir habilitar controles por datos simulados. No mapear botones Obstacle/Tracking del proveedor como modos autónomos: el PDF muestra desplazamientos laterales.

## Archivos vecinos

- [LineDeviceCalibration.tsx](<LineDeviceCalibration.tsx>)
- [lineFollow.css](<lineFollow.css>)
- [LineFollowPanel.tsx](<LineFollowPanel.tsx>)
- [useLineDevice.ts](<useLineDevice.ts>)

Estado: material preparado sin build, tests, validadores ni prueba física. Los hashes de fuentes copiadas están en [manifest](../../../../../../AutoFinalRobo2-Contracts/provenance/files.json).
