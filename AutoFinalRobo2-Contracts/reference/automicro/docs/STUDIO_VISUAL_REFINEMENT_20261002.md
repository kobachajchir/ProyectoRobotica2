# Protocol Studio / OLED Studio — cambio de presentación

Fecha: 2026-10-02. Complemento visual de AMI-20261002-07; no crea una nueva versión del contrato.

Web aplica su manual existente con visual-identity-system, Apple Design y Emil Design Engineering. Cambia la presentación de campos UNER: offsets, etiquetas, colores contenidos, detalles desplegables y bytes con reflujo; reorganiza salidas/copia y navegación. OLED redistribuye lienzo, capas e inspector, elimina la altura mínima rígida, corrige controles anidados y mejora textos/foco. Registro detallado: Web docs/design-system/studio-refinement-2026-10-02.md, decisión D-15.

- [x] Implementación local de estilos/primitivas compartidas en Web.
- [x] Documentación de decisiones, alcance y cobertura en el manual Web.
- [x] Compilación TypeScript/Vite y copia automática solo del gzip al ESP (1.761.269 bytes).
- [ ] Revisión visual en navegador, móvil/escritorio, claro/oscuro y teclado.
- [ ] Carga de filesystem y prueba física OLED.

No cambian IDs, schemas, serialización, correlación ni permisos de firmware. F4 y Qt no requieren adaptación por esta entrega. ESP solo recibe data/dist/index.html.gz mediante postbuild; no se recompiló ni flasheó firmware. Las inconsistencias de metadata ESP y demás pendientes de AMI-20261002-07 siguen abiertos.

Código Web local sin commit ni publicación para preservar el checkout con cambios previos. Documentación central publicada por separado. Tests/escáneres y validación visual no ejecutados. La compilación no prueba el resultado estético ni funcionamiento físico.
