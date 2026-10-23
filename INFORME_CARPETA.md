# Carpeta .

[Maestro](../INFORME_MAESTRO.md) · [Informe del repositorio](INFORME_ADAPTACION.md).

Base nueva de trabajo; ver estado de implementación en el informe del repositorio.

## Utilidad y reformas específicas

### [Recursos, estilos y empaquetado](docs/modules/assets.md)

Se conservan recursos necesarios para leer/reutilizar la UI anterior; el GLB es el robot previo y no representa Mecanum.

Reformas: Crear identidad/modelo del nuevo robot; retirar 3D pesado si excede LittleFS/bundle. El package activo es React/Vite mínimo, sin postbuild hacia otro repo. El empaquetador produce artifacts/littlefs/dist dentro de Web, no publica ni flashea. Versiones iniciales tomadas del manifest origen; falta instalar/fijar lock propio y revisar requisitos Node. No se ejecutó build/npm.

### [Control, TEST, línea y GOTO](docs/modules/control.md)

Flujos de control, paneles de pruebas, calibración y reportes son referencia visual y funcional. Balance y calibración IMU no corresponden a este robot.

Reformas: Cuatro ruedas y control lateral Mecanum, diagnóstico independiente por rueda, cinco IR y HC-SR04 con validez. GOTO requiere odometría y muestra estimación; quitar guardas/indicadores MPU y crear requisitos reales. Prohibir habilitar controles por datos simulados. No mapear botones Obstacle/Tracking del proveedor como modos autónomos: el PDF muestra desplazamientos laterales.

### [Visor OLED, pantallas y acciones](docs/modules/oled-ui.md)

Identidades, builders, rasterizador/editor y controles permiten estudiar la biblioteca completa; SVG no es captura de pantalla física.

Reformas: Portar motor/recursos/estado o transportar framebuffer si se requiere fidelidad. Distinguir acción/invocador/vista. Mismo esquema de navegación que encoder: menú explícito, atrás por ítem; no simular botón físico inexistente. Implementar estados nuevos solo si Mega los reporta. No mantener sensores falsos para rellenar una pantalla.

### [Cliente WebSocket y estado](docs/modules/protocol.md)

Reutilizar promesas por requestId, respuesta/error/evento, cancelación al desconectar y estados de conexión. Contextos capturan APIs históricas y simulación.

Reformas: Crear cliente de producto/capabilities nuevo; no mandar stm.* antes del port ESP/Mega. Separar aceptación y final. Unknown/stale no se rellena con datos de demo. Adaptar dueño de mando/lease, reconexión y permisos. No inferir compatibilidad del catálogo TypeScript.

## Archivos vecinos

- [.gitignore](<.gitignore>)
- [AGENTS.md](<AGENTS.md>)
- [index.html](<index.html>)
- [INFORME_ADAPTACION.md](<INFORME_ADAPTACION.md>)
- [package.json](<package.json>)
- [README.md](<README.md>)
- [vite.config.js](<vite.config.js>)

## Subcarpetas

- [docs](docs/INFORME_CARPETA.md)
- [reference](reference/INFORME_CARPETA.md)
- [src](src/INFORME_CARPETA.md)
- [tools](tools/INFORME_CARPETA.md)

Estado: material preparado sin build, tests, validadores ni prueba física. Los hashes de fuentes copiadas están en [manifest](../AutoFinalRobo2-Contracts/provenance/files.json).
