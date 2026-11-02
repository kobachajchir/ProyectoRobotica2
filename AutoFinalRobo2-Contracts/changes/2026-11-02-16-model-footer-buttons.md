# AFR2-20261102-16 — Accesos de Modelos como botones

Pedido humano: convertir los dos enlaces del pie de Modelos en botones con
los títulos «Simulador» y «Explorar infraestructura», siguiendo el manual.

`Web/src/features/models/ModelsPage.tsx`: usa la primitiva compartida Button,
variante secundaria, iconos UiIcon y agrupación afr2-action-row. Navegación
React Router a las mismas rutas anteriores. Referencia: catálogo Botón/enlace
de `docs/design-system/components.md` y receta de `Web/src/components/ui/Button.tsx`.
Conserva tokens, tamaño mínimo de 44 px, foco y feedback de la primitiva vigente.

atmega/esp sin cambios; web solo fuente. Sin tests, compilación, validaciones,
navegador, promoción ni flasheos. Sin commit/publicación.
