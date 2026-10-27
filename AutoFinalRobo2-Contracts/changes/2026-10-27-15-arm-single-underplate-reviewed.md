# AFR2-20261027-15 — Contraplaca única y revisión CAD v06

27/10/2026. El usuario pide unir las zonas izquierda y central/derecha de apoyo debajo del acrílico, revisar geometría, rodamientos, ejes, engranajes y movimiento, y planificar láminas técnicas. Indica expresamente esperar su señal antes de producir despieces.

## Cambio y entrega

Se conserva la v05 baja y se crea `AFR2_Brazo_PVC32_AGV_v06`. Una contraplaca PETG roja, plana de 3 mm, sustituye las dos referencias inferiores de aluminio; el puente de unión tiene 12 mm de ancho. Un cuerpo continuo, seis pasos M3 y uno M10; las tuercas/arandelas metálicas y el casquillo central permanecen separados. No se cambia la montura baja a Z=6 mm ni se agrega alza. La sustitución de material no acredita equivalencia resistente.

Maestro guardado en Fusion y [F3D editable](../../AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/AFR2_Brazo_PVC32_AGV_v06.f3d), parámetros CSV, fuentes de autoría y [evidencia v06](../../AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/revision_v06/README.md). Se conservan 132 parámetros, 14 uniones y 4 vínculos de movimiento.

## Evidencia geométrica autorizada

- 25 sólidos impresos continuos, 6 ejes continuos y 12 rodamientos completos con contacto de eje/apoyo. Tubos PVC 80/70 mm y cuatro pernos de abrazadera completos.
- Sin advertencias/errores de cálculo en el historial ni penetraciones reportables en los montajes internos. Grafo de contacto con una cadena principal y dos conjuntos de mordaza/almohadilla separados por el juego de las guías.
- 3.141 configuraciones CAD muestreadas sin penetraciones mayores a 0,001 mm³: 524 de ejes, 181 de pinza, 2.197 de muñeca, 181 del pan, 57 combinaciones J1/pan y una presentación. Los barridos mantienen otros ejes en posturas documentadas; no cubren todo el espacio articular continuo.
- Envolvente conservadora del pan de 360° libre de la montura fija. Análisis de interferencia nativo de la presentación sin penetraciones reportables entre conjuntos.
- Extremos nativos J1…J6 alcanzados y pinza accionada ida/vuelta: 50,000 mm a 109,8°, 60,011 mm a 133,7° y 79,405 mm a 180°.
- Transmisiones nominales: hombro 20/40T módulo 1,5, 45 mm entre ejes; J4 24/24T módulo 1,5, 36 mm; pinza 24T módulo 1 con dos cremalleras. Se conservan relaciones y sentidos de los vínculos y se incluye la geometría de dientes en el muestreo.

## Láminas, siguiente etapa

[Plan de skill](../../AutoFinalRobo2-CAD/docs/PLAN_SKILL_LAMINAS_TECNICAS.md) basado en la referencia gráfica del usuario. Dos modos: ensamble con piezas acotadas en cuadros a la izquierda y despiece numerado a la derecha, y pieza individual con vistas/cortes a la izquierda e isométrica a la derecha. Rótulo, códigos y BOM coherentes, fuentes de cotas trazables. La skill está planificada, no instalada. No se generan despieces ni planos nuevos en esta revisión.

| Módulo | Estado de esta ficha |
| --- | --- |
| CAD | v06 editable guardada, contraplaca integrada y revisión geométrica muestreada |
| Atmega | Sin cambios de esta ficha |
| ESP | Sin cambios de esta ficha |
| Web | Sin cambios de esta ficha ni promoción del ensamble |
| Contracts | Alcance y evidencia registrados |

Sin fabricación ni ensayo físico. Resistencia/precarga del PETG, capacidad real de 100 g, torque, desgaste y estabilidad del AGV continúan pendientes. PWM representa posición solicitada; no telemetría del ángulo real.
