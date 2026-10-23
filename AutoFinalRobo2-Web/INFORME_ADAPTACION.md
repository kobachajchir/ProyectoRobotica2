# Adaptación web

[Informe maestro](../INFORME_MAESTRO.md) · [Contracts](../AutoFinalRobo2-Contracts/README.md).

reference/web conserva src/public y manifests históricos inertes *.upstream.txt. src/ activo es una pantalla de estado React; no conecta al robot ni simula sensores. Reutilizar componentes gradualmente tras cerrar contrato. package.json propio no tiene postbuild entre repos. tools/package-web.mjs prepara gzip local (no ejecutado); promover assets a ESP en una etapa explícita. Falta instalación/lock/build y adaptación funcional.

## Guías junto al código

| Subsistema | Para qué sirve |
| --- | --- |
| [Cliente WebSocket y estado](docs/modules/protocol.md) | Reutilizar promesas por requestId, respuesta/error/evento, cancelación al desconectar y estados de conexión. Contextos capturan APIs históricas y simulación. |
| [Control, TEST, línea y GOTO](docs/modules/control.md) | Flujos de control, paneles de pruebas, calibración y reportes son referencia visual y funcional. Balance y calibración IMU no corresponden a este robot. |
| [Visor OLED, pantallas y acciones](docs/modules/oled-ui.md) | Identidades, builders, rasterizador/editor y controles permiten estudiar la biblioteca completa; SVG no es captura de pantalla física. |
| [Recursos, estilos y empaquetado](docs/modules/assets.md) | Se conservan recursos necesarios para leer/reutilizar la UI anterior; el GLB es el robot previo y no representa Mecanum. |

Cada carpeta contiene INFORME_CARPETA.md con enlaces. reference/ no participa en la configuración activa. Los archivos originales se identifican por hash en Contracts/provenance/files.json. No se modifica ni publica en AutoMicro.
