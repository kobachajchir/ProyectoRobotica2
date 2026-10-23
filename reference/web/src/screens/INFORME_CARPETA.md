# Carpeta reference/web/src/screens

[Maestro](../../../../../INFORME_MAESTRO.md) · [Informe del repositorio](../../../../INFORME_ADAPTACION.md).

Fuentes históricas de referencia: no forman parte del producto adaptado.

## Utilidad y reformas específicas

### [Visor OLED, pantallas y acciones](../../../../docs/modules/oled-ui.md)

Identidades, builders, rasterizador/editor y controles permiten estudiar la biblioteca completa; SVG no es captura de pantalla física.

Reformas: Portar motor/recursos/estado o transportar framebuffer si se requiere fidelidad. Distinguir acción/invocador/vista. Mismo esquema de navegación que encoder: menú explícito, atrás por ítem; no simular botón físico inexistente. Implementar estados nuevos solo si Mega los reporta. No mantener sensores falsos para rellenar una pantalla.

## Archivos vecinos

- [bitmapAssets.ts](<bitmapAssets.ts>)
- [connectivityScreens.ts](<connectivityScreens.ts>)
- [dashboardScreens.ts](<dashboardScreens.ts>)
- [displayScreens.ts](<displayScreens.ts>)
- [helpers.ts](<helpers.ts>)
- [identifiedScreens.ts](<identifiedScreens.ts>)
- [index.ts](<index.ts>)
- [menuScreens.ts](<menuScreens.ts>)
- [motorScreens.ts](<motorScreens.ts>)
- [notificationScreens.ts](<notificationScreens.ts>)
- [oledBitmapFont.ts](<oledBitmapFont.ts>)
- [remediatedScreens.ts](<remediatedScreens.ts>)
- [SCREEN_INVENTORY.md](<SCREEN_INVENTORY.md>)
- [screenCodes.ts](<screenCodes.ts>)
- [screenRenderRegistry.ts](<screenRenderRegistry.ts>)
- [sensorScreens.ts](<sensorScreens.ts>)
- [settingsScreens.ts](<settingsScreens.ts>)
- [types.ts](<types.ts>)
- [warningScreens.ts](<warningScreens.ts>)

Estado: material preparado sin build, tests, validadores ni prueba física. Los hashes de fuentes copiadas están en [manifest](../../../../../AutoFinalRobo2-Contracts/provenance/files.json).
