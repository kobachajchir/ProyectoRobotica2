# AFR2-20261102-11 — Extracción de identidad visual de Infraestructura

02/11/2026. Pedido humano de extraer detalladamente toda la identidad de la parte de Infraestructura: divs, colores, layout, títulos/subtítulos, separadores, márgenes, padding, selectores con iconos, animaciones de selección/clic, barra creciente y capas/degradados/overlays de la portada.

## Antes / después

La implementación estaba repartida entre página/CSS de Infraestructura, tokens, primitivas y estado compartido. Se añade un [dossier independiente](../docs/design-system/infraestructura/index.md) con lectura estática del código actual: árbol DOM, materiales, colores por tema/zona, capas del banner, escala tipográfica, dimensiones, estados de cada control, fórmula del progreso, barras y anillo, tabla completa de movimiento y responsive.

Incluye manual, componentes, fuentes, tokens documentales, decisiones, adopción y configuración de auditoría inactiva. Se distinguen Observada/Inferida/Propuesta, aprobación Pendiente y ejecución No verificada. No se sustituye el manual general ni se modifica la interfaz.

La línea de estaciones escala un degradado fijo; las barras de GLB/gzip tienen tiempos distintos; el anillo de almacenamiento no declara animación de crecimiento. Se registran por separado particularidades de código, como tamaño de iconos frente al tipo declarado, precedencia hover/active, reducción de movimiento parcial y propagación de errores de consulta.

## Productores / consumidores / wire

Sin cambios de API, IDs de protocolo, mensajes, comandos, campos ni persistencia. El único consumidor nuevo es quien lee el dossier. Los tokens extraídos no se importan en la aplicación.

## Secuencia y alcance

Lectura de fuentes directas y dependencias visuales; escritura documental en Contracts. Base Git leída `3c30707dcd56122bd03c8cce8de87d7d43d25b7b`, con modificaciones locales previas relevantes en Infraestructura y cambios ajenos de CAD/documentación preservados. No se usa ese commit como snapshot exacto de los archivos de trabajo.

No se ejecutaron tests, builds, validadores, resolvedor/escáner de skill, navegador, render, mediciones de contraste, consultas al robot ni flasheos. Las fichas 07/10 conservan sus evidencias históricas; no se reclaman como comprobaciones de esta entrega.

## Estado por módulo

| Módulo | Estado en esta entrega |
| --- | --- |
| atmega | Sin cambios nuevos. |
| esp | Sin cambios nuevos, sin promoción de paquete. |
| web | Sin cambios nuevos; se leyeron fuentes de trabajo con ediciones anteriores. |
| shared / Contracts | Dossier y ficha documental, `draft`, ejecución No verificada. |
| CAD / Loader | Sin intervención; cambios previos conservados. |

Sin commit ni publicación solicitados en este pedido.
