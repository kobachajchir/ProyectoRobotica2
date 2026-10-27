# AFR2-20261027-03 — Brazo sobre AGV y continuidad mecánica v03

27/10/2026. El usuario pide corregir guías flotantes, apoyos/ejes incompletos y soportes impresos separados; revisar conexiones e interferencias y montar el brazo sobre el auto. El pedido autoriza esta revisión geométrica y sus despieces. No incluye fabricación ni ensayos físicos.

## Diseño

Se conserva la v02 como antecedente y se trabaja en `AFR2_Brazo_PVC32_AGV_v03`. Los labios de la pinza reciben postes de unión a la palma. Se restauran las envolventes de rodamientos dañadas por cortes de otras piezas; los cortes nuevos limitan sus participantes al componente correspondiente. J1, hombro, codo, J4, J5 y J6 llevan ejes continuos, apoyos dobles y retención axial representada. Los acoples incorporan sección plana, fijación al horn y tornillo central de referencia. Las tapas desmontables se atornillan; los volúmenes que pertenecen a una misma pieza impresa se integran.

La base de mesa se sustituye por una placa elevada, cuatro anclajes M3 y un apriete central M10 con casquillo de compresión y contraplaca de aluminio. Se usan las posiciones de agujeros extraídas del CAD `AutoFinalRobo2`; no se toma la foto como plano de fabricación. Tres XL4016, Nano y PCA9685 se distribuyen verticalmente junto al eje. El maestro del auto se conserva y sus sólidos visibles se copian como referencia al documento nuevo.

Se mantienen PVC Ø32/28,3 mm, distancias de ejes 130/120 mm, cortes 80/70 mm y pieza de referencia 50 × 50 × 50 mm / 100 g. El eje de hombro queda a225 mm sobre la cara superior del chasis en esta propuesta. No se agrega sensado externo; PWM representa posición solicitada, no posición física medida.

## Entrega y evidencia

Maestro v03 guardado en Fusion y [F3D editable local](../../AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/AFR2_Brazo_PVC32_AGV_v03.f3d).114 parámetros,323 componentes incluyendo referencia del auto,13 uniones y4 vínculos. La [revisión v03](../../AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/revision_v03/README.md) reúne evidencia:24 sólidos impresos conectados,6 ejes continuos,12 rodamientos completos con apoyos, caños80/70 mm y pernos completos,0 penetraciones internas y2.904 posturas CAD muestreadas sin penetraciones reportables. Pinza nativa50,000/60,011 mm a109,8/133,7° y18 accionamientos de extremos de ejes. Se generan vistas y dos despieces derivados. El historial se revisa sin advertencias/errores de cálculo. Esto no certifica trayectorias continuas ni capacidad física.

## Estados

| Módulo | Estado de esta ficha |
| --- | --- |
| CAD | Maestro editable v03, montaje sobre referencia del auto, revisión geométrica muestreada y despieces |
| Atmega | Sin cambios de esta ficha |
| ESP | Sin cambios de esta ficha |
| Web | Sin cambios de esta ficha; las fichas01/02 tienen alcance propio |
| Contracts | Registro de alcance, decisiones y evidencia CAD |

No se calculan estabilidad del auto, resistencia del montaje, fuerza de agarre, torque/corriente ni batería. Las roscas, estriados y cuerpos comerciales son referencias simplificadas; las tolerancias finales requieren componentes e impresión reales. La conservación de uniones y parámetros no demuestra que cualquier combinación de parámetros sea fabricable.
