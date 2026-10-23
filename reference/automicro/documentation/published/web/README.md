# Cliente Web — manual de unidades

Metodología [AMDOC 1](../../METHOD.md). Fotografía de fuente: 2026-10-20. Las inferencias y limitaciones se indican por ficha.

Ver [arquitectura](architecture.md), [conexiones entre manuales](../shared/CONNECTIONS.md) y [pantallas F4 ↔ ESP ↔ Web](../shared/SCREEN_SYSTEM.md).

| Unidad | Capa | Funciones públicas | Internas |
| --- | --- | --- |
| [Cámara y controles de orientación](libraries/camera-controls.md) | integración | 10 | 15 |
| [Escena 3D y carga de modelos](libraries/scene-3d.md) | integración | 28 | 17 |
| [Primitivas OLED, fuentes y representación SVG](libraries/oled-renderer.md) | núcleo | 36 | 0 |
| [Layout y navegación de secciones](libraries/navigation-shell.md) | integración | 54 | 13 |
| [Visor de pantalla y controles remotos](libraries/screen-controls.md) | integración | 26 | 31 |
| [Componentes comunes y estados visuales](libraries/shared-ui.md) | integración | 17 | 11 |
| [Modo del auto y control de movimiento](libraries/motion-control.md) | integración | 58 | 38 |
| [Estado Wi-Fi, AP y credenciales Web](libraries/wifi-interface.md) | integración | 72 | 9 |
| [Telemetría y estado de sensores](libraries/sensor-state.md) | servicio | 47 | 11 |
| [Estado normalizado de pantalla remota](libraries/screen-state.md) | servicio | 44 | 0 |
| [Contexto UNER y superficies históricas](libraries/legacy-protocol-context.md) | integración | 3 | 5 |
| [Usuario, PIN y permisos remotos](libraries/authentication.md) | integración | 19 | 5 |
| [GOTO y calibración de motores Web](libraries/auto-motion.md) | integración | 16 | 6 |
| [Balance PID, sesiones y análisis](libraries/balance-workbench.md) | integración | 25 | 9 |
| [Herramientas de calibración IR y línea](libraries/calibration-tools.md) | integración | 37 | 9 |
| [Contenido Docs y selector de destino](libraries/documentation-ui.md) | integración | 14 | 2 |
| [Geometría y modelo visual de sensores IR](libraries/ir-visual-model.md) | núcleo | 37 | 0 |
| [Calibración y seguimiento de línea del dispositivo](libraries/line-follow.md) | integración | 35 | 12 |
| [Modelo y edición de documentos OLED](libraries/oled-document.md) | núcleo | 82 | 16 |
| [Raster binario y protocolo de canvas Web](libraries/oled-rasterizer.md) | núcleo | 35 | 0 |
| [Transacción de transferencia canvas Web](libraries/oled-transfer.md) | integración | 55 | 25 |
| [Catálogo, codecs y herramientas UNER](libraries/protocol-tools.md) | núcleo | 93 | 23 |
| [Interfaz Protocol Studio](libraries/protocol-studio.md) | integración | 26 | 13 |
| [Simulación y datos de desarrollo](libraries/simulation.md) | soporte | 4 | 1 |
| [Arranque, proveedores y rutas Web](libraries/application.md) | composición | 2 | 0 |
| [API JSON, Promises y WebSocket](libraries/websocket-client.md) | servicio | 11 | 10 |
| [Renderers Web de conectividad y avisos](libraries/connectivity-builders.md) | núcleo | 44 | 2 |
| [Renderers Web de dashboard y modos](libraries/dashboard-builders.md) | núcleo | 5 | 0 |
| [Constructores de menús OLED](libraries/menu-builders.md) | núcleo | 12 | 0 |
| [Códigos e identidad de pantallas](libraries/screen-codes.md) | contrato | 8 | 0 |
| [Resolución de variantes y fallback de pantalla](libraries/screen-resolver.md) | integración | 31 | 0 |
| [Renderers Web de sensores, ajustes y PIN](libraries/sensor-settings-builders.md) | núcleo | 34 | 0 |
| [Tipos y feedback de acciones remotas](libraries/remote-actions.md) | contrato | 12 | 2 |
| [Preferencias locales, tema y utilidades](libraries/local-preferences.md) | servicio | 14 | 2 |
