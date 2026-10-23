# Documentación autocontenida AutoMicro

La fuente documental vive en Contracts. Se prepara en Markdown y JSON para
integrarla después en la Web, incluidos los tres destinos F4, ESP y Web.

- [Método y nomenclatura AMDOC 1](METHOD.md): reglas compartidas para toda ampliación.
- [FirmwareF4: índice de 39 unidades](published/f4/README.md): librerías, servicios, integración y plataforma.
- [Arquitectura F4 y dependencias reales](published/f4/architecture.md).
- [Contratos F4 ↔ ESP ↔ Web](published/shared/wire-contracts.md).
- [PIN: strings, dígitos wire y autoridad](published/shared/contracts/auth-pin-v1.md).
- [OLED Canvas: formato, CRC y transferencia](published/shared/contracts/oled-canvas-v1.md).
- [Catálogo documental de comandos registrados](published/shared/command-registry.md).
- [Inconsistencias y revisiones con enlaces a documentación](review/FINDINGS.md).
- [Firmware ESP01: índice de 28 unidades](published/esp/README.md) y [arquitectura](published/esp/architecture.md).
- [Cliente Web: índice de 34 unidades](published/web/README.md) y [arquitectura/porteo de Docs](published/web/architecture.md).
- [Sistema de pantallas, estado y controles](published/shared/SCREEN_SYSTEM.md).
- [Registro de 147 identidades de pantalla](published/shared/SCREEN_CODE_REGISTER.md).
- [Conexiones e impacto por archivo o hallazgo](published/shared/CONNECTIONS.md).
- [Cobertura y límites editoriales](published/coverage.md).

Fotografía inicial: **20 de octubre de 2026**, código del checkout de trabajo.
Las fichas no certifican aislamiento universal ni interoperabilidad. Explican
dependencias concretas, puertos, globals de contexto, buffers y callbacks, con
las partes inferidas identificadas. El inventario de soporte está en
`published/{f4,esp,web}/source-map.json`; cada ficha conserva hashes de sus fuentes.

Las anotaciones de código no se aplicaron a los firmware. No se ejecutaron tests,
compilaciones ni validaciones automáticas. El selector de tres opciones y su
píldora móvil quedan registrados para la etapa de interfaz.

El objetivo del visor es portar toda la biblioteca OLED F4 con su estado y
variantes. El SVG/los builders actuales no se presentan como réplica exacta.
PIN, OSK, selección/scroll, GOTO, datos de sensores y línea tienen contratos y
hallazgos enlazados para corregirlos de forma focalizada. El manual interno Qt
sigue pendiente; el contenido todavía no se integró en el bundle del producto.

No se modifica el registro AMI por crear este manual: esta entrega no cambia
IDs, mensajes ni comportamiento. Las fichas históricas de `changes/` y
`CHANGELOG.md` siguen siendo el registro de cambios efectivos del contrato.
