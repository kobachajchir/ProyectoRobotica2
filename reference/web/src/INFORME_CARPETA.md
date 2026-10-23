# Carpeta reference/web/src

[Maestro](../../../../INFORME_MAESTRO.md) · [Informe del repositorio](../../../INFORME_ADAPTACION.md).

Fuentes históricas de referencia: no forman parte del producto adaptado.

## Utilidad y reformas específicas

### [Recursos, estilos y empaquetado](../../../docs/modules/assets.md)

Se conservan recursos necesarios para leer/reutilizar la UI anterior; el GLB es el robot previo y no representa Mecanum.

Reformas: Crear identidad/modelo del nuevo robot; retirar 3D pesado si excede LittleFS/bundle. El package activo es React/Vite mínimo, sin postbuild hacia otro repo. El empaquetador produce artifacts/littlefs/dist dentro de Web, no publica ni flashea. Versiones iniciales tomadas del manifest origen; falta instalar/fijar lock propio y revisar requisitos Node. No se ejecutó build/npm.

### [Control, TEST, línea y GOTO](../../../docs/modules/control.md)

Flujos de control, paneles de pruebas, calibración y reportes son referencia visual y funcional. Balance y calibración IMU no corresponden a este robot.

Reformas: Cuatro ruedas y control lateral Mecanum, diagnóstico independiente por rueda, cinco IR y HC-SR04 con validez. GOTO requiere odometría y muestra estimación; quitar guardas/indicadores MPU y crear requisitos reales. Prohibir habilitar controles por datos simulados. No mapear botones Obstacle/Tracking del proveedor como modos autónomos: el PDF muestra desplazamientos laterales.

### [Visor OLED, pantallas y acciones](../../../docs/modules/oled-ui.md)

Identidades, builders, rasterizador/editor y controles permiten estudiar la biblioteca completa; SVG no es captura de pantalla física.

Reformas: Portar motor/recursos/estado o transportar framebuffer si se requiere fidelidad. Distinguir acción/invocador/vista. Mismo esquema de navegación que encoder: menú explícito, atrás por ítem; no simular botón físico inexistente. Implementar estados nuevos solo si Mega los reporta. No mantener sensores falsos para rellenar una pantalla.

### [Cliente WebSocket y estado](../../../docs/modules/protocol.md)

Reutilizar promesas por requestId, respuesta/error/evento, cancelación al desconectar y estados de conexión. Contextos capturan APIs históricas y simulación.

Reformas: Crear cliente de producto/capabilities nuevo; no mandar stm.* antes del port ESP/Mega. Separar aceptación y final. Unknown/stale no se rellena con datos de demo. Adaptar dueño de mando/lease, reconexión y permisos. No inferir compatibilidad del catálogo TypeScript.

## Archivos vecinos

- [App.css](<App.css>)
- [App.tsx](<App.tsx>)
- [grep.exe.stackdump](<grep.exe.stackdump>)
- [index.css](<index.css>)
- [main.tsx](<main.tsx>)
- [vite-env.d.ts](<vite-env.d.ts>)

## Subcarpetas

- [api](api/INFORME_CARPETA.md)
- [assets](assets/INFORME_CARPETA.md)
- [components](components/INFORME_CARPETA.md)
- [contexts](contexts/INFORME_CARPETA.md)
- [features](features/INFORME_CARPETA.md)
- [hooks](hooks/INFORME_CARPETA.md)
- [models](models/INFORME_CARPETA.md)
- [pages](pages/INFORME_CARPETA.md)
- [protocol](protocol/INFORME_CARPETA.md)
- [screens](screens/INFORME_CARPETA.md)
- [types](types/INFORME_CARPETA.md)
- [utils](utils/INFORME_CARPETA.md)

Estado: material preparado sin build, tests, validadores ni prueba física. Los hashes de fuentes copiadas están en [manifest](../../../../AutoFinalRobo2-Contracts/provenance/files.json).
