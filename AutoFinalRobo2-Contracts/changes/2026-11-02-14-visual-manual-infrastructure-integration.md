# AFR2-20261102-14 — Manual visual ampliado con Infraestructura

02/11/2026. El usuario solicita incorporar todo el desglose anterior al manual actual, reformándolo donde corresponda de forma aditiva y conservando sus contenidos/componentes. Se usa la skill visual-identity-system para integración documental; sin aplicación de interfaz.

## Antes / después

La extracción estaba en un dossier independiente. El manual general pasa de 0.1.10 a **0.1.11 draft**, con vigencia y precedencias explícitas. Se amplían sus secciones de material/tema, tipografía/ritmo, movimiento/acceso, responsive y gobernanza, y se incorpora dentro de manual.md el atlas completo de Infraestructura. No se limita a enlazar la extracción.

components.md incorpora contratos, estados y recetas dentro del catálogo existente; tokens.json conserva todos los valores previos y añade las capas infrastructure de primitivas, semánticos por tema, sección, componentes y movimiento. Fuentes, decisiones VIS-057–061, adopción, índice, cobertura documental y configuración inactiva acompañan la ampliación. Dos láminas SVG didácticas hacen visibles la gramática y las capas del hero; son ilustraciones Propuestas, no capturas de la aplicación.

Se conservan marca, paleta, formularios, overlays, notificaciones, escenarios, navegación, accesibilidad y OLED. La composición narrativa amplía Explorar; las pantallas operativas mantienen acceso inmediato y sin loops decorativos. Las excepciones de código observadas se documentan como deuda y no se incorporan como normas deseables. El dossier original permanece como evidencia con enlace a la guía integrada.

## Productores / consumidores / wire

Sin cambios de API, comandos, IDs, serialización, datos persistidos o firmware. Consumidor de la ampliación: quien consulta el manual. Los tokens documentales añadidos no se importan desde la Web.

## Secuencia y estado

Lectura de manual y extracción; integración aditiva por contexto y por capa; escritura de documentación y esquemas. Modificaciones locales ajenas de AVR/Web/CAD/documentación preservadas. Las reglas históricas se mantienen con su fecha; la precedencia actual evita aplicar variantes antiguas como si fueran nuevas.

| Módulo | Estado de esta entrega |
| --- | --- |
| atmega | Sin cambios nuevos. |
| esp | Sin cambios nuevos ni promoción de paquete. |
| web | Sin cambios nuevos; no reforma visual aplicada. |
| shared / Contracts | Manual 0.1.11 draft ampliado, fuentes y decisiones; ejecución No verificada. |
| CAD / Loader | Sin intervención. |

No se ejecutaron tests, compilaciones, validadores, comprobaciones de enlaces/JSON, escáneres, navegador, render de comprobación, consultas al robot ni flasheos. Cobertura es inventario documental, no resultado de validación. Las evidencias históricas mantienen su alcance; no certifican esta revisión. Sin commit ni publicación solicitados.

[Manual](../docs/design-system/manual.md#vigencia-0111) · [Índice](../docs/design-system/index.md) · [Componentes](../docs/design-system/components.md#infraestructura-integrada) · [Decisiones](../docs/design-system/decisions.md).
