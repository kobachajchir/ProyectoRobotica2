# Carpeta reference/web/src/features

[Maestro](../../../../../INFORME_MAESTRO.md) · [Informe del repositorio](../../../../INFORME_ADAPTACION.md).

Fuentes históricas de referencia: no forman parte del producto adaptado.

## Utilidad y reformas específicas

### [Control, TEST, línea y GOTO](../../../../docs/modules/control.md)

Flujos de control, paneles de pruebas, calibración y reportes son referencia visual y funcional. Balance y calibración IMU no corresponden a este robot.

Reformas: Cuatro ruedas y control lateral Mecanum, diagnóstico independiente por rueda, cinco IR y HC-SR04 con validez. GOTO requiere odometría y muestra estimación; quitar guardas/indicadores MPU y crear requisitos reales. Prohibir habilitar controles por datos simulados. No mapear botones Obstacle/Tracking del proveedor como modos autónomos: el PDF muestra desplazamientos laterales.

### [Visor OLED, pantallas y acciones](../../../../docs/modules/oled-ui.md)

Identidades, builders, rasterizador/editor y controles permiten estudiar la biblioteca completa; SVG no es captura de pantalla física.

Reformas: Portar motor/recursos/estado o transportar framebuffer si se requiere fidelidad. Distinguir acción/invocador/vista. Mismo esquema de navegación que encoder: menú explícito, atrás por ítem; no simular botón físico inexistente. Implementar estados nuevos solo si Mega los reporta. No mantener sensores falsos para rellenar una pantalla.

## Archivos vecinos

- [studio.css](<studio.css>)

## Subcarpetas

- [autoMotion](autoMotion/INFORME_CARPETA.md)
- [balanceCalibration](balanceCalibration/INFORME_CARPETA.md)
- [calibration](calibration/INFORME_CARPETA.md)
- [docs](docs/INFORME_CARPETA.md)
- [ir](ir/INFORME_CARPETA.md)
- [lineFollow](lineFollow/INFORME_CARPETA.md)
- [motion](motion/INFORME_CARPETA.md)
- [oledEditor](oledEditor/INFORME_CARPETA.md)
- [protocolStudio](protocolStudio/INFORME_CARPETA.md)

Estado: material preparado sin build, tests, validadores ni prueba física. Los hashes de fuentes copiadas están en [manifest](../../../../../AutoFinalRobo2-Contracts/provenance/files.json).
