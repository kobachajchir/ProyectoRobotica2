# AMI-20261002-02 — C/D: finalización Qt y rechazos de acciones remotas

- Fecha: 2026-10-02 (America/Argentina/Buenos_Aires)
- Repositorio de origen: AutoMicroQtDesktop
- Referencia de código: checkouts locales modificados sobre revisiones documentadas en docs/PROTOCOL_A_F.md
- Estado del origen: completado en fuente/documentación; pruebas y despliegue diferidos
- Área: UNER / WebSocket / pantalla / documentación

## Contrato anterior y nuevo

Qt: 0x74 SameCommand; interpretar status. Acciones 0x53..0x58: éxito solo con status 0; rechazo funcional y resincronización distintos de NACK.
Los formatos exactos se mantienen en el catálogo central de esta entrega.

## Flujo completo

Qt: 0x74 SameCommand; interpretar status. Acciones 0x53..0x58: éxito solo con status 0; rechazo funcional y resincronización distintos de NACK.
Se conservan las guardas del productor, el envelope UNER v2 y JSON v1.
ACK de cola o transporte no equivale a éxito funcional.

## Impacto en repositorios

| Repositorio | Estado | Acción |
| --- | --- | --- |
| FirmwareF4 | no_aplica | C/D adaptan consumidor Qt al wire existente; no cambian productor. |
| Firmware ESP01 | no_aplica | Corrección de finalización interna Qt por USB; API puente sin cambio. |
| Web ESP01 | no_aplica | No consume señales internas de broker/controlador Qt. |
| AutoMicroQtDesktop | adaptado | SameCommand 0x74; status UI no-cero falla y resincroniza una vez. |

## Compatibilidad y orden

Qt: 0x74 SameCommand; interpretar status. Acciones 0x53..0x58: éxito solo con status 0; rechazo funcional y resincronización distintos de NACK.
No se aceptan ACK de otro comando como fallback.

## Evidencia y límites

Inspección y edición de fuente únicamente. Sin tests, compilaciones, validaciones,
flasheo ni prueba física por instrucción del usuario. Una etapa completada describe
código/documentación, no un firmware instalado ni una prueba de interoperabilidad.

## Actualizaciones posteriores

- 2026-10-02: tarea autorizada; etapas abiertas. Pendiente implementación.

- 2026-10-02: Qt C/D/E completados en fuente: SameCommand 0x74, rechazo funcional con status y refresco, IR en panel Sensores de Telemetría QML. Sin compilación/USB físico.

### Cierre de implementación 2026-10-02

[Campos exactos A–F](../docs/PROTOCOL_A_F.md), [catálogo](../catalog/commands.json), [transiciones de pantallas](../docs/SCREEN_BEHAVIOR.md), [avance y pendientes](../IMPLEMENTATION.md). Sin tests/builds/validaciones/flasheo/CI por instrucción del usuario. Adaptado describe fuente local; publicar Contracts no publica consumidores ni demuestra interoperabilidad.

- 2026-10-02, pedido posterior de compilación: F4+empaquetado, ESP01, Web producción y Qt MSVC completaron build. Se corrigieron imports/WebGL Web y SHA256 post-build F4; wire sin cambios. [Detalle](../docs/BUILD_RESULTS_20261002.md). Tests, CI, hardware y despliegue no ejecutados. MinGW carece de Qt6WebSockets; MSVC completó enlace.

- 2026-10-02: instalado Qt WebSockets 6.9.0 para MinGW por pedido expreso. Config CMake y DLL presentes; no se ejecutó nuevo build MinGW. Sin modificación del contrato.
