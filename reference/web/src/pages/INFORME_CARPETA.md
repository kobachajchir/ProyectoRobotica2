# Carpeta reference/web/src/pages

[Maestro](../../../../../INFORME_MAESTRO.md) · [Informe del repositorio](../../../../INFORME_ADAPTACION.md).

Fuentes históricas de referencia: no forman parte del producto adaptado.

## Utilidad y reformas específicas

### [Control, TEST, línea y GOTO](../../../../docs/modules/control.md)

Flujos de control, paneles de pruebas, calibración y reportes son referencia visual y funcional. Balance y calibración IMU no corresponden a este robot.

Reformas: Cuatro ruedas y control lateral Mecanum, diagnóstico independiente por rueda, cinco IR y HC-SR04 con validez. GOTO requiere odometría y muestra estimación; quitar guardas/indicadores MPU y crear requisitos reales. Prohibir habilitar controles por datos simulados. No mapear botones Obstacle/Tracking del proveedor como modos autónomos: el PDF muestra desplazamientos laterales.

## Archivos vecinos

- [ControlSection.tsx](<ControlSection.tsx>)
- [DocsSection.tsx](<DocsSection.tsx>)
- [DocTopicSection.tsx](<DocTopicSection.tsx>)
- [EstadoSection.tsx](<EstadoSection.tsx>)
- [Home.tsx](<Home.tsx>)
- [Login.tsx](<Login.tsx>)
- [MotorCalibrationSection.tsx](<MotorCalibrationSection.tsx>)
- [NotFound.css](<NotFound.css>)
- [NotFound.tsx](<NotFound.tsx>)
- [OledEditorSection.tsx](<OledEditorSection.tsx>)
- [ProtocolSection.tsx](<ProtocolSection.tsx>)
- [TrackFollowerSection.tsx](<TrackFollowerSection.tsx>)
- [WifiSection.tsx](<WifiSection.tsx>)

Estado: material preparado sin build, tests, validadores ni prueba física. Los hashes de fuentes copiadas están en [manifest](../../../../../AutoFinalRobo2-Contracts/provenance/files.json).
