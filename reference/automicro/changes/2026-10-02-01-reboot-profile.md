# AMI-20261002-01 — A/B: reinicio de perfil 0x76 y ACK correlacionado

- Fecha: 2026-10-02 (America/Argentina/Buenos_Aires)
- Repositorio de origen: FirmwareF4
- Referencia de código: checkouts locales modificados sobre revisiones documentadas en docs/PROTOCOL_A_F.md
- Estado del origen: completado en fuente/documentación; pruebas y despliegue diferidos
- Área: UNER / WebSocket / pantalla / documentación

## Contrato anterior y nuevo

F4: ACK con packet->cmd. ESP: mover reboot de 0x75 a 0x76 en ambos catálogos. Qt/Web: conservar correlación estricta. Instalar F4 antes de ESP.
Los formatos exactos se mantienen en el catálogo central de esta entrega.

## Flujo completo

F4: ACK con packet->cmd. ESP: mover reboot de 0x75 a 0x76 en ambos catálogos. Qt/Web: conservar correlación estricta. Instalar F4 antes de ESP.
Se conservan las guardas del productor, el envelope UNER v2 y JSON v1.
ACK de cola o transporte no equivale a éxito funcional.

## Impacto en repositorios

| Repositorio | Estado | Acción |
| --- | --- | --- |
| FirmwareF4 | origen | ACK identifica packet->cmd; reset diferido conservado. |
| Firmware ESP01 | adaptado | ID 0x76 en ambos catálogos; 0x75 solo declarado. |
| Web ESP01 | compatible | SystemResetActions usa stm.extensionProfile.reboot con profileId; ESP resuelve ID. |
| AutoMicroQtDesktop | compatible | UnerTypes/F4ApiCodec ya usan 0x76 y ACK originalCmd correlacionado. |

## Compatibilidad y orden

F4: ACK con packet->cmd. ESP: mover reboot de 0x75 a 0x76 en ambos catálogos. Qt/Web: conservar correlación estricta. Instalar F4 antes de ESP.
No se aceptan ACK de otro comando como fallback.

## Evidencia y límites

Inspección y edición de fuente únicamente. Sin tests, compilaciones, validaciones,
flasheo ni prueba física por instrucción del usuario. Una etapa completada describe
código/documentación, no un firmware instalado ni una prueba de interoperabilidad.

## Actualizaciones posteriores

- 2026-10-02: tarea autorizada; etapas abiertas. Pendiente implementación.

- 2026-10-02: F4 B completado en fuente (Core/Src/uner_app.c). RESET_MCU conserva ACK [0x19,0]; reboot produce [0x76,0]. ESP A sigue pendiente.

- 2026-10-02: etapa ESP A/E completada en fuente. Docs locales enlazan el contrato. Mantener orden F4 antes de ESP.

### Cierre de implementación 2026-10-02

[Campos exactos A–F](../docs/PROTOCOL_A_F.md), [catálogo](../catalog/commands.json), [transiciones de pantallas](../docs/SCREEN_BEHAVIOR.md), [avance y pendientes](../IMPLEMENTATION.md). Sin tests/builds/validaciones/flasheo/CI por instrucción del usuario. Adaptado describe fuente local; publicar Contracts no publica consumidores ni demuestra interoperabilidad.

- 2026-10-02, pedido posterior de compilación: F4+empaquetado, ESP01, Web producción y Qt MSVC completaron build. Se corrigieron imports/WebGL Web y SHA256 post-build F4; wire sin cambios. [Detalle](../docs/BUILD_RESULTS_20261002.md). Tests, CI, hardware y despliegue no ejecutados. MinGW carece de Qt6WebSockets; MSVC completó enlace.
