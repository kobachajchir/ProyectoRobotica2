---
product: AutoFinalRobo2 / Infraestructura
manualVersion: 0.1.0
status: draft
updatedAt: 2026-11-02
changeId: AFR2-20261102-11
---

> **Integrado en el manual general (AFR2-20261102-14, 02/11/2026).** El usuario autorizó incorporar esta extracción de forma aditiva. La guía vigente está en [manual.md general](../manual.md#vigencia-0111), [componentes](../components.md#infraestructura-integrada) y [tokens](../tokens.json). Este dossier conserva la evidencia de origen 0.1.0; sus límites anteriores de mantenerlo separado no describen la integración documental posterior. No se modifica la Web.


# Identidad visual extraída de Infraestructura

Extracción detallada de la ruta `/#/infraestructura`, seleccionada expresamente por el usuario como referencia. Describe el código de trabajo del 02/11/2026, incluidas sus modificaciones locales. No redefine el manual general ni modifica la interfaz.

**Evidencia:** patrones Observados en fuente; elección de esta sección como canónica Confirmada por el pedido. **Verificación de esta entrega:** No verificada en ejecución. **Aprobación del manual:** Pendiente; `draft`.

La identidad combina una portada azul noche, grilla técnica y halos suaves con un mapa interactivo por zonas; continúa con superficies sólidas, una línea de estaciones y datos representados con barras y tarjetas. Los acentos tienen una función narrativa: azul para la computadora, ámbar para el ESP, rubí para Wi-Fi y verde para el navegador. El selector destaca por una pill recortada que cambia fondo, icono y texto juntos.

## Lectura del dossier

| Archivo | Contenido |
| --- | --- |
| [manual.md](manual.md) | Desglose minucioso: DOM, capas del banner, paletas, títulos, espaciado, selectores, estados, barras, anillo, animaciones y responsive. |
| [components.md](components.md) | Anatomía y contrato de cada componente; matrices de estados, cobertura y recetas. |
| [tokens.json](tokens.json) | Inventario de valores y expresiones CSS con procedencia, decisión y estado. Es documental, no se importa desde la Web. |
| [sources.md](sources.md) | Fuentes directas, dependencias, revisión base y límites de la extracción. |
| [decisions.md](decisions.md) | Reglas extraídas, inferencias, diferencias y decisiones pendientes. |
| [adoption.md](adoption.md) | Cómo reutilizar esta identidad si posteriormente se solicita, sin ampliar su autoridad a otras pantallas. |
| [audit.config.json](audit.config.json) | Configuración acotada para una eventual revisión expresamente autorizada. No ejecutada. |

## Los cinco rasgos que conviene conservar

1. Portada oscura en ambos temas, con fondos en capas y texto claro; acentos de banner separados de los del contenido.
2. Una misma zona gobierna iconos, bordes, insignias, estaciones y ficha; los colores ayudan a seguir el relato.
3. Tarjetas sólidas y jerarquía editorial amplia: kicker, título, explicación y datos, con espacio entre grupos.
4. Selector con una copia activa recortada; estaciones con estados anterior/actual/siguiente y crecimiento desde la izquierda.
5. Movimiento ligado al propósito: viaje de paquetes, cambio de etapa y aparición de magnitudes; documentar por separado las excepciones de reducción de movimiento.

## Límites y pendientes

No se ejecutaron tests, compilación, validadores, escáneres de deriva, navegador ni consultas al robot. Las fichas históricas 07/10 describen revisiones previas en navegador; no certifican el código de trabajo ni esta entrega. Hay particularidades de fuente que no deben convertirse automáticamente en reglas: precedencia hover/pulsación de nodos, tamaños de iconos fuera del tipo declarado, reducción de movimiento incompleta y errores de consulta absorbidos por el contexto.

El siguiente paso documental de menor riesgo es utilizar las recetas y el inventario para especificar una pantalla futura cuando el usuario la identifique. Cualquier aplicación o corrección queda fuera de esta extracción.
