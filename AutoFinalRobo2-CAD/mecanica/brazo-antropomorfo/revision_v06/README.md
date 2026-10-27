# Contraplaca inferior única y revisión geométrica — v06

27/10/2026. Se conserva la v05 y se trabaja en una copia. El usuario pide unificar las dos piezas inferiores y revisar apoyos, ejes, engranajes, movilidad e interferencias antes de iniciar los despieces. **No se generan todavía despieces ni planos de fabricación.**

## Modificación

`V06_Contraplaca_inferior_unica_PETG` reemplaza la contraplaca central/derecha y la contrabarra izquierda. Un puente continuo de 12 mm de ancho integra las dos zonas. Es una sola pieza plana de PETG rojo, de 3 mm de espesor, con seis pasos Ø3,4 mm para M3 y el paso central Ø10,5 mm para M10. Conserva las coordenadas del chasis, las tuercas y arandelas metálicas y el casquillo de compresión independiente. No se modelan roscas en el plástico: el apriete se hace con las tuercas metálicas.

La envolvente de la pieza es 150,299 × 63 × 3 mm, con una zona izquierda más estrecha; no es un rectángulo macizo completo. Cara superior en Z=−3 mm e inferior en Z=−6 mm, debajo del acrílico. La pieza tiene un solo cuerpo y una región de material continua. Se propone imprimirla apoyada por su cara plana. Cambia el material propuesto de las contraplacas anteriores: eran referencias de aluminio, esta pieza se plantea para impresión. El espesor, precarga y resistencia requieren diseño de fabricación/ensayo; esta revisión no acredita equivalencia resistente con aluminio.

Se conserva la solución baja v05: montura Z=6…12 mm, separadores 6 mm, eje J1 retrasado 14 mm, interfaz J1 Z=96 mm y hombro Z=209 mm sobre el chasis. `agv_pan_lift=0 mm`. Los eslabones siguen en 130/120 mm entre ejes; PVC Ø32/28,3 mm y cortes 80/70 mm. La pieza manipulada sigue siendo 50×50×50 mm, requisito de 100 g.

## Archivos

Maestro guardado con 132 parámetros de usuario, 331 componentes incluyendo raíz y referencia del auto, 14 uniones y 4 vínculos de movimiento.

- [Maestro editable](../AFR2_Brazo_PVC32_AGV_v06.f3d), documento `AFR2_Brazo_PVC32_AGV_v06`.
- [Parámetros](../parametros_prototipo_v06.csv) e [inventario](modelo_v06.json).
- [Modificación de contraplaca](../revise_lower_v06.py), con auxiliares de las revisiones anteriores.
- [Comprobación de movimiento](../check_complete_v06.py), [continuidad y rodamientos](../audit_connections_v06.py), [contactos](../contact_graph_v06.py) y [accionamiento nativo](../review_native_v06.py).

## Revisión autorizada

| Comprobación | Resultado | Evidencia |
| --- | --- | --- |
| Piezas impresas | 25 sólidos rojos, cada uno continuo; montura y contraplaca son dos piezas fabricadas distintas | [continuidad](continuidad_y_rodamientos.json) |
| Ejes | 6 componentes, un cuerpo continuo por eje | mismo informe |
| Rodamientos | 12 completos, 8×608 y 4×625; contacto geométrico de aro interior con eje y exterior con apoyo | mismo informe |
| PVC y pernos de abrazadera | Tubos 80/70 mm completos y cuatro pernos continuos | mismo informe |
| Montajes internos | 0 penetraciones reportables entre componentes diferentes del brazo/montura | [montajes internos](montajes_internos.json) |
| Espacio del pan | Cilindro conservador completo Ø68,8 mm, Z=5…51 mm, libre de la montura baja y fijaciones | [envolvente](envolvente_circular.json) |
| Concordancia cinemática | Diferencias de matrices menores que 6×10⁻¹⁴ entre muestreo y uniones nativas | [paridad](paridad_cinematica.json) |
| Contactos | 203 componentes en una cadena principal y dos conjuntos de mordaza con almohadilla, separados por el juego de las guías | [grafo](grafo_contactos.json) |
| Historial | Sin advertencias/errores de cálculo; montura principal con un cuerpo y una región continua | [montura](continuidad_montura.json) |
| Presentación nativa de Fusion | 0 penetraciones reportables entre conjuntos | [interferencias nativas](pose_nativa_interferencias.json) |

### Movilidad y dientes

Se revisaron **3.141 configuraciones CAD**, sin penetraciones mayores a **0,001 mm³**:

- 524 posiciones de barridos individuales de J1…J6, en pasos de 2° con extremos incluidos.
- 181 aperturas de pinza vacía, servo de 0° a 180° en pasos de 1°.
- 2.197 combinaciones J4/J5/J6, cada eje de −90° a +90° en pasos de 15°, con el cubo de 50 mm sujeto.
- 181 posiciones del pan HC-SR04, de −90° a +90° en pasos de 1°.
- 57 combinaciones del giro J1 cada 10° con el pan en −90°, 0° y +90°.
- Una postura completa de presentación.

Cada barrido mantiene otros ejes en una postura concreta: el del hombro usa codo −90°; el de codo usa hombro 0°; la rejilla de muñeca usa hombro 35° y codo −15°. Los JSON guardan las posturas exactas. El resultado no acredita todas las combinaciones de los ocho movimientos ni trayectorias continuas. La envolvente del pan sí reserva geométricamente un círculo completo frente a la montura fija.

Informe acumulado: [movimiento v06](informe_movimiento_v06.json). Informes por etapa: [ejes](barrido_ejes.json), [pinza](apertura_pinza.json), [muñeca](barrido_muneca.json) y [J1/pan](barrido_J1_pan.json). Los conteos de estos archivos son acumulados hasta su etapa; no se suman entre sí.

Transmisiones: hombro módulo 1,5, piñón 20T/rueda 40T, distancia entre ejes 45 mm y reducción 2:1; J4 módulo 1,5, 24T/24T, distancia 36 mm y relación 1:1; pinza módulo 1, piñón 24T, radio primitivo 12 mm y dos cremalleras opuestas. Los cuatro vínculos nativos conservan las relaciones y sentidos opuestos. Los barridos incluyen la geometría real de los dientes. [Relaciones y vínculos](transmisiones.json).

El accionamiento nativo confirma apertura 50,000 mm a 109,8°, 60,011 mm a 133,7° y 79,405 mm a 180°; cierre mínimo vacío 4,007 mm. Se prueban mínimos, centros y máximos de J1…J6 con error angular menor a 3×10⁻¹⁴ grados. [Pinza](apertura_nativa_fusion.json), [extremos](extremos_nativos_fusion.json). Los ángulos son del ensamble virtual; PWM no mide el ángulo físico real.

Vistas armadas y de la pieza única, sin despiece: [contraplaca](../../../renders/2026-10-27/contraplaca_unica_v06.png), [montura baja](../../../renders/2026-10-27/montura_baja_v06.png) y [ensamble completo](../../../renders/2026-10-27/brazo_agv_v06.png). Pan accionado nativamente a −90°, 0° y +90°: [registro](extremos_pan_nativos.json).

## Próxima etapa, todavía sin ejecutar

[Plan de la skill de láminas técnicas](../../../docs/PLAN_SKILL_LAMINAS_TECNICAS.md): modo ensamble, con cuadros de piezas acotadas a la izquierda y despiece numerado a la derecha; modo pieza, con vistas/cortes/cotas a la izquierda e isométrica a la derecha. Se conservó la imagen aportada como referencia visual, sin reutilizar su autoría ni sus datos técnicos. La skill está planificada, no instalada. El usuario indicará cuándo producir los despieces y las hojas.

## Límites

La revisión permite continuar con la documentación del prototipo CAD dentro del alcance muestreado. No certifica resistencia del PETG, apriete, fuerza de agarre, torque, corriente, backlash impreso, estabilidad del AGV con el brazo extendido ni capacidad física de 100 g. No se fabricó ni ensayó el robot.
