# Carpeta reference/web/src/features/oledEditor

[Maestro](../../../../../../INFORME_MAESTRO.md) · [Informe del repositorio](../../../../../INFORME_ADAPTACION.md).

Fuentes históricas de referencia: no forman parte del producto adaptado.

## Utilidad y reformas específicas

### [Visor OLED, pantallas y acciones](../../../../../docs/modules/oled-ui.md)

Identidades, builders, rasterizador/editor y controles permiten estudiar la biblioteca completa; SVG no es captura de pantalla física.

Reformas: Portar motor/recursos/estado o transportar framebuffer si se requiere fidelidad. Distinguir acción/invocador/vista. Mismo esquema de navegación que encoder: menú explícito, atrás por ítem; no simular botón físico inexistente. Implementar estados nuevos solo si Mega los reporta. No mantener sensores falsos para rellenar una pantalla.

## Archivos vecinos

- [assets.ts](<assets.ts>)
- [codegen.ts](<codegen.ts>)
- [document.ts](<document.ts>)
- [geometry.ts](<geometry.ts>)
- [LopakaLikeEditor.tsx](<LopakaLikeEditor.tsx>)
- [oledCanvasProtocol.ts](<oledCanvasProtocol.ts>)
- [oledCanvasRasterizer.ts](<oledCanvasRasterizer.ts>)
- [OledCanvasTransferPanel.tsx](<OledCanvasTransferPanel.tsx>)
- [OledStudioWorkspace.tsx](<OledStudioWorkspace.tsx>)
- [types.ts](<types.ts>)
- [useOledCanvasTransfer.ts](<useOledCanvasTransfer.ts>)
- [useOledEditorState.ts](<useOledEditorState.ts>)

Estado: material preparado sin build, tests, validadores ni prueba física. Los hashes de fuentes copiadas están en [manifest](../../../../../../AutoFinalRobo2-Contracts/provenance/files.json).
