# AFR2-20261103-25 — Loader más directo y sin carteles de fuente

03/11/2026. Pedido humano:
- quitar los carteles «Fuente» de Explorar y Ayuda;
- Loader más directo: viñetas en cada etapa, operaciones en un desplegable, banner «Totalmente automatizable con IA»;
- separar los nodos ATmega2560 y ESP8266 de la portada;
- cableado y verificación previa dichos en una línea.

Continúa AFR2-20261102-24.

## Cambios en Web/src
- `features/loader/loaderFlow.ts`: cada etapa pasa de párrafo a `points` (viñetas cortas); se elimina `source`; textos de motivos y herramientas acortados; nueva lista `LOADER_OPERATIONS` (nombre + efecto en una línea).
- `features/loader/LoaderPage.tsx`: viñetas en la ficha, desplegable «Operaciones (14)», banner de IA con chips (stdio, 10 herramientas, 14 operaciones), portada y notas más breves.
- `features/docs/DocsPage.tsx`: ya no muestra la línea «Fuente» de cada capítulo (los datos quedan en `helpContent.ts`).
- `styles/visual-language.css`: se quita la barra lateral desfasada (`inset 3px`) de la opción activa de la barra lateral; queda solo el fondo y el color de selección.
- `styles/workspace.css`: separación de 12 px entre los nodos del carril de placas, viñetas, tabla de operaciones y banner de IA.

## Estado por repo
- **atmega / esp / loader:** sin cambios.
- **web:** fuente modificada. Verificado en el navegador local a 1440 px: sin «Fuente» en Loader ni Ayuda, desplegable de operaciones, banner y separación de nodos (12 px). No verificado: compilación TypeScript ni celular.
- Sin commit.
