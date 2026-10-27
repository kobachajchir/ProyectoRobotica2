# Montura bilateral y giro del HC-SR04 — v04

Corrección pedida el 27/10/2026. La v03 trataba el auto como referencia fija: su muestreo no incluía el giro del HC-SR04. Esta revisión incorpora ese movimiento y agrega la fijación izquierda. Se trabaja en una copia; el auto maestro y el brazo v03 se conservan.

## Montaje

Se mantienen la distribución vertical de Nano, PCA9685 y tres XL4016, los cuatro anclajes derechos y el apriete central con casquillo. Se agregan dos M3 en los agujeros del chasis debajo del voltímetro, con separadores y contrabarra inferior de aluminio de 3 mm. Hay seis M3 laterales más el M10 central. Esto distribuye las fijaciones a ambos lados; no demuestra una distribución uniforme de cargas.

Coordenadas extraídas del CAD del auto, expresadas en el sistema del brazo: X transversal, Y hacia el sensor frontal, Z hacia arriba; Z=0 en la cara superior del chasis.

| Anclaje | X (mm) | Y (mm) |
| --- | ---: | ---: |
| A | 31,051 | 19,739 |
| B | 67,997 | 19,739 |
| C | 31,051 | −21,036 |
| D | 68,418 | −21,036 |
| E, nuevo izquierdo | −68,298597443 | −12,157024165 |
| F, nuevo izquierdo | −31,142269400 | −12,199106379 |

Los agujeros E/F del modelo del chasis tienen aproximadamente Ø3,366 mm; los pasos en las piezas nuevas son Ø3,4 mm. No se toma la fotografía como plantilla de perforado. La contrabarra izquierda mide aproximadamente 49,156 × 12 × 3 mm.

## Espacio para el sensor

El eje del pan está en X=−5,150029370 mm, Y=42,999972322 mm. El conjunto móvil incluye eje de salida/horn del SG90, tornillo central, soporte y todos los sólidos del HC-SR04. La unión `J8_Pan_HCSR04` permite −90°…+90° respecto del frente; equivale a comandos nominales 0°…180° con centro en 90°. Un ángulo CAD o una orden PWM no es una medida física de posición.

La envolvente horizontal conservadora de las cajas de los sólidos móviles tiene radio menor a 31,4 mm. Se reserva además un margen radial de 3 mm. El cilindro de referencia oculto `82_Envolvente_pan_REFERENCIA` representa el círculo completo de Ø68,8 mm, entre Z=5 y 51 mm. Puede activarse para revisar futuros cambios.

Se eleva 32 mm el conjunto superior de la montura, manteniendo su disposición: su cara inferior pasa de Z=22 a Z=54 mm. Quedan 3 mm verticales sobre la altura máxima de 51 mm del conjunto móvil. Los seis separadores pasan a 54 mm y el casquillo central a 57 mm; se representan M3×70 y M10×80 para conservar las tuercas debajo de las contraplacas. El eje de hombro pasa de 225 a 257 mm sobre el chasis. La torre local de J1 sigue midiendo 113 mm y los eslabones conservan 130/120 mm entre ejes.

## Edición

- [Maestro Fusion editable](../AFR2_Brazo_PVC32_AGV_v04.f3d).
- [Parámetros exportados](../parametros_prototipo_v04.csv).
- [Fuente de las modificaciones](../mount_pan_v04.py). Usa los auxiliares de construcción v01/v02/v03; las fases se aplican una sola vez a la copia v04.
- [Fuente de la comprobación CAD](../check_pan_v04.py).

`pan_axis_x/y`, `pan_radius_bound`, `pan_clearance`, `pan_top_z` y `pan_bottom_z` definen el espacio reservado. `agv_pan_lift = pan_top_z + pan_clearance − agv_mount_z` gobierna la elevación del conjunto superior mediante operaciones nativas de traslación. `agv_mount_z_final` gobierna los separadores y tornillos. Las piezas conservan croquis y operaciones; las traslaciones se aplican al final del historial de cada componente. No se ensayaron variaciones arbitrarias de todos los parámetros.

## Evidencia y límites

Maestro guardado en Fusion y F3D local: **126 parámetros, 332 componentes contando raíz y referencias, 14 uniones y 4 vínculos de movimiento**. El historial no presenta advertencias ni errores de cálculo. La envolvente queda oculta y el sensor centrado a 0°; para accionarlo usar `J8_Pan_HCSR04` en las uniones de la raíz.

| Comprobación autorizada | Resultado | Evidencia |
| --- | --- | --- |
| Cilindro completo de 360°, Ø68,8 mm | Contiene los 25 sólidos móviles y no penetra la montura, incluido el apriete inferior | [envolvente](envolvente_circular.json) |
| Pan −90°…+90°, cada 1°, con dos posturas del brazo | 362 casos, 0 penetraciones mayores a 0,001 mm³ | [barridos](barrido_pan.json) |
| Postura completa de presentación | 1 caso adicional sin penetraciones reportables; total 363 | [muestreo](poses_completas.json) |
| Interferencias internas de los conjuntos del brazo/montura | 0 reportables | [montajes](montajes_internos.json) |
| Interferencia nativa Fusion entre conjuntos, postura de presentación | 0 reportables | [análisis nativo](pose_nativa_interferencias.json) |
| Accionamiento nativo de J8 | −90°, 0° y +90° alcanzados; se registra la transformación del conjunto | [extremos](extremos_pan_nativos.json) |
| Concordancia de cinemática de muestreo y ensamble nativo | Error máximo de elementos de matriz menor a 5×10⁻¹⁴ | [paridad](paridad_cinematica.json) |

Al elevar la montura se detectó una interferencia de 274,45 mm³ en la pestaña de la tapa de J1. La operación anterior usaba el cuerpo de la tapa como herramienta y dependía de su posición final. Se suprime esa dependencia y se agrega un rebaje nativo Ø22,25 mm situado en `base_top − 7 mm`, con 1,2 mm de profundidad. Las comprobaciones de la tabla corresponden a la geometría final con esta corrección; los resultados históricos de la v03 no incluyen el pan del auto.

Vistas del modelo, sin ilustración generada:

- [Ensamble completo](../../../renders/2026-10-27/brazo_agv_v04.png).
- [Anclajes y distribución de electrónica](../../../renders/2026-10-27/anclajes_bilaterales_v04.png).
- Giro del sensor: [−90°](../../../renders/2026-10-27/pan_hcsr04_v04_menos90.png), [0°](../../../renders/2026-10-27/pan_hcsr04_v04_0.png), [+90°](../../../renders/2026-10-27/pan_hcsr04_v04_90.png).

Inventario y cotas: [modelo_v04.json](modelo_v04.json); modificaciones aplicadas: [registro](authoring_journal.jsonl). Los barridos mantienen dos posturas concretas del brazo; no recorren todas las combinaciones de sus articulaciones. La envolvente conservadora sí reserva espacio continuo para el giro completo del sensor frente a la montura fija.

Se trata de comprobación geométrica CAD. No acredita resistencia de tornillos o piezas impresas, estabilidad del auto con el brazo elevado, torque, corriente, fuerza de agarre ni capacidad real para 100 g. La elevación aumenta la altura del centro de masa. La longitud y la disponibilidad comercial de la tornillería y las tolerancias de fabricación deben confirmarse antes de construir.
