# AMI-20261002-04 — F: catálogo, comportamiento de pantallas y skill central

- Fecha: 2026-10-02 (America/Argentina/Buenos_Aires)
- Repositorio de origen: Web ESP01
- Referencia de código: checkouts locales modificados sobre revisiones documentadas en docs/PROTOCOL_A_F.md
- Estado del origen: completado en fuente/documentación; pruebas y despliegue diferidos
- Área: UNER / WebSocket / pantalla / documentación

## Contrato anterior y nuevo

Inventariar códigos y variantes reales. Documentar transiciones, teclado y prioridad del router. Representación de identidad si faltan datos remotos. Skill central con manifiestos reproducibles.
Los formatos exactos se mantienen en el catálogo central de esta entrega.

## Flujo completo

Inventariar códigos y variantes reales. Documentar transiciones, teclado y prioridad del router. Representación de identidad si faltan datos remotos. Skill central con manifiestos reproducibles.
Se conservan las guardas del productor, el envelope UNER v2 y JSON v1.
ACK de cola o transporte no equivale a éxito funcional.

## Impacto en repositorios

| Repositorio | Estado | Acción |
| --- | --- | --- |
| FirmwareF4 | compatible | Códigos/payloads preservados; no se agrega snapshot; AGENTS referencia skill. |
| Firmware ESP01 | compatible | add_screen_report conserva u32 screenCode/source sin lista cerrada; AGENTS referencia skill. |
| Web ESP01 | adaptado | 17 constantes+metadata+resolver; menús reportados o identity-only, sin estado OSK inventado. |
| AutoMicroQtDesktop | compatible | Decodificador conserva screenCode u32 y acción correlacionada; AGENTS referencia skill. |

## Compatibilidad y orden

Inventariar códigos y variantes reales. Documentar transiciones, teclado y prioridad del router. Representación de identidad si faltan datos remotos. Skill central con manifiestos reproducibles.
No se aceptan ACK de otro comando como fallback.

## Evidencia y límites

Inspección y edición de fuente únicamente. Sin tests, compilaciones, validaciones,
flasheo ni prueba física por instrucción del usuario. Una etapa completada describe
código/documentación, no un firmware instalado ni una prueba de interoperabilidad.

## Actualizaciones posteriores

- 2026-10-02: tarea autorizada; etapas abiertas. Pendiente implementación.

- 2026-10-02: Web completado en fuente. Estado IR confirmado y tres controles; cobertura de códigos faltantes con menús respaldados por reportes o identidad explícita. Sin compilación ni pruebas.

### Cierre de implementación 2026-10-02

[Campos exactos A–F](../docs/PROTOCOL_A_F.md), [catálogo](../catalog/commands.json), [transiciones de pantallas](../docs/SCREEN_BEHAVIOR.md), [avance y pendientes](../IMPLEMENTATION.md). Sin tests/builds/validaciones/flasheo/CI por instrucción del usuario. Adaptado describe fuente local; publicar Contracts no publica consumidores ni demuestra interoperabilidad.

- 2026-10-02, pedido posterior de compilación: F4+empaquetado, ESP01, Web producción y Qt MSVC completaron build. Se corrigieron imports/WebGL Web y SHA256 post-build F4; wire sin cambios. [Detalle](../docs/BUILD_RESULTS_20261002.md). Tests, CI, hardware y despliegue no ejecutados. MinGW carece de Qt6WebSockets; MSVC completó enlace.
