# Reutilización y límites de adopción

> **Integrado en el manual general (AFR2-20261102-14, 02/11/2026).** El usuario autorizó incorporar esta extracción de forma aditiva. La guía vigente está en [manual.md general](../manual.md#vigencia-0111), [componentes](../components.md#infraestructura-integrada) y [tokens](../tokens.json). Este dossier conserva la evidencia de origen 0.1.0; sus límites anteriores de mantenerlo separado no describen la integración documental posterior. No se modifica la Web.


02/11/2026 · versión 0.1.0 · `draft` · INF-VIS-014.

## Situación actual

Infraestructura tiene estilos propios en `infrastructure.css`, un componente de página y datos/hook locales. Consume primitivas comunes UiIcon, Button y SegmentedControl. Colores por tema, banner, fuentes, radios y parte de los tiempos salen de `styles/tokens.css`; numerosas medidas, sombras, degradados y tiempos propios están escritos directamente en el CSS de la sección.

El selector ya es compartido. Su pill se apoya en tokens específicos y en una copia con clip-path. Las fichas, carriles, barras, anillo y reveal se definen localmente. No hay en esta extracción una nueva librería de componentes ni un archivo de estilos que la Web importe.

Este dossier conserva el código actual como evidencia sin reemplazar el manual general. Los archivos `tokens.json` y `audit.config.json` son inventarios/configuración documental: su presencia no cambia el render, no acredita una auditoría y no autoriza aplicar reglas fuera de la sección.

## Arquitectura posible si se solicita reutilizar

**Propuesta, no implementada:** mantener las familias semánticas existentes; convertir decisiones locales que se repitan en aliases de componente; reutilizar primero las primitivas ya comunes. Extraer componentes nuevos solo cuando un segundo caso real necesite su contrato. Evitar copiar toda la página para obtener un selector o una tarjeta.

| Orden futuro | Pieza | Motivo | Dependencias / límite | Evidencia de esta entrega |
| --- | --- | --- | --- | --- |
| 1 | Delimitar la pantalla destino | Distinguir qué aporta Infraestructura y qué requiere otra referencia. | Pedido concreto; no está elegida una nueva pantalla. | Extracción terminada, destino pendiente. |
| 2 | SegmentedControl y Button existentes | Ya concentran comportamiento compartido. | Conservar su API; resolver límites solo si se solicita. | Leídos, sin modificar. |
| 3 | Tipografía, zonas y material de tarjetas | Son bases de composición reutilizables. | Distinguir tema, banner y significado de zona. | Documentados en manual/tokens. |
| 4 | Patrón de ficha y navegación por etapas | Útil para otro recorrido secuencial. | No confundir done con ejecución real. | Contrato local descrito. |
| 5 | Barras y estados de procedencia | Útil si el nuevo contenido representa magnitudes comparables. | Unidades, baseline y ausencia explícitos. | Fórmulas y límites descritos. |
| 6 | Portada por capas | Útil para introducción narrativa de suficiente importancia. | Evitar convertir todas las pantallas en portadas. | Capas y proporciones descritas. |

No se ejecutan comprobaciones en ninguna fila. Si luego se pide comprobar el resultado, el alcance de esa revisión se decide entonces; no se incluye como acción automática en este trabajo.

## Componentes piloto posibles

**Propuesta:** una futura pantalla explicativa con cabecera, selector compartido, una ficha por etapa y un bloque de datos con vacío permitiría comprobar la reutilización conceptual sin copiar los cinco bloques completos. No se ha creado ese piloto ni elegido una pantalla.

## Deuda separada de identidad

| Particularidad | Por qué no copiarla como regla | Tratamiento actual |
| --- | --- | --- |
| UiIconSize vs tamaños usados | Es una diferencia entre tipo y uso, no un principio visual. | Registrada, sin corrección. |
| Hover done / active de nodo | La cascada puede ocultar o alterar feedback. | Registrada sin presentar resultado probado. |
| Reducción de movimiento parcial | La intención general excede las excepciones implementadas. | Inventario exacto de lo que queda. |
| `data-zone` global | Puede extender alcance fuera del componente. | Señalado para futura extracción técnica si se pide. |
| Fills mínimos y radios de suma | Favorecen visibilidad del cubo frente a escala estricta. | Excepción específica de estas barras. |
| Hover en glosario estático | Puede sugerir acción sin que exista. | Documentado como tarjeta editorial. |
| Error absorbido por contexto | El estado visual puede no alcanzarse. | Se distingue declaración de comportamiento acreditado. |
| Datos reportados/catálogo | No equivalen a medición física ni instalación. | Procedencia textual conservada. |

## Qué se entregó

Solo documentación bajo Contracts: portada, manifiesto, manual, contratos, tokens, decisiones, este mapa y configuración de auditoría inactiva, con ficha AFR2. Sin cambios nuevos en Web/ATmega/ESP/CAD/Loader, sin commit ni publicación. Las modificaciones previas del espacio de trabajo se conservaron.
