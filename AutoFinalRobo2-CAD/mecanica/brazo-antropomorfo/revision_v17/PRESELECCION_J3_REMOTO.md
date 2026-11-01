# J3 remoto sobre el primer eslabon: preseleccion vigente

29/10/2026. Se conserva la pinza V7 y la cadena antropomorfa. J1 gira axialmente; J2 es hombro; J3 es codo. El motor de J3 pasa al extremo proximal del primer eslabon, junto a J2, y gira solidario con ese eslabon. La correa no atraviesa el giro axial de muñeca J4.

## Criterio aceptado

El usuario acepta un margen mayor que1 o1.2. Se toma **1.2** como minimo de preseleccion de actuadores, en lugar del anterior objetivo2. Margen = par nominal disponible tras la reduccion y la eficiencia presupuestada / maximo par articular calculado. No se usa par de bloqueo como nominal.

| Eje | Actuador propuesto | Reduccion | Demanda pico N m | Disponible N m | Margen |
| --- | --- | --- | --- | --- | --- |
| J1 | STS3250 | 1:1 | 0.8797 | 1.5691 | 1.784 |
| J2 | STS3250 | 3.5:1 | 2.8289 | 4.3934 | 1.553 |
| J3 | STS3250 | 2:1 | 2.0071 | 2.5105 | 1.251 |
| J4 | STS3032 | 1:1 | 0.0740 | 0.1471 | 1.989 |
| J5 | STS3250 | 1:1 | 0.9101 | 1.5691 | 1.724 |
| J6 | STS3032 | 1:1 | 0.0076 | 0.1471 | 19.450 |

Los seis ejes pasan este filtro presupuestario. J3 es el limitante, con1.251. Esto **no cierra la seleccion fisica**: faltan capacidad de correa y acoples, rango y referencia multivuelta de servos con reduccion, comportamiento de frenado y masa del conjunto final.

## Efecto de trasladar J3

Con el mismo inventario, el pico de J2 baja de2.9088 a2.8289 N m, una reduccion del2.74%. J3 queda en2.0071 N m: el traslado no reduce la carga que mueve el codo.

Masa presupuestada sin carga: **1452.5g**. No es masa obtenida del nuevo CAD. El traslado por si solo no aligera el brazo; redistribuye motor, soporte y elementos de entrada. Centro de masa del brazo mas carga extendidos: X pasa de92.14 a87.78mm en el marco del modelo; no es el centro de masa del auto completo.

La fila codo es una comparacion virtual de distribucion de masas, manteniendo el mismo inventario para aislar el traslado. No representa una segunda transmision construible: colocar ambas poleas en el mismo centro no define una correa funcional.

## Hipotesis y estado real

Carga200g; eslabones propuestos70/60mm; muñeca55/65mm; pinza existente de110.536g conservada con sus tensores extraidos de v09. Velocidad articular30 grados/s y frenada modelada de50ms. Son valores de operacion propuestos, no velocidad nominal cargada ni frenada certificada de cada fabricante.

El resto son presupuestos de piezas, con eficiencia0.8 en reducciones. Los casos evaluados no demuestran un maximo global para todo movimiento. La estabilidad sobre ruedas sigue condicionada a contactos reales y centro de masa del auto; aceptar margen1.2 de actuador no demuestra ausencia de vuelco.

El F3D v17 guardado sigue siendo la copia de la arquitectura V7 con correcciones v09. **No incorpora todavia esta transmision remota ni las masas aligeradas del presupuesto.** No se presenta como brazo terminado. No se realizaron tests de software, compilaciones ni flasheos en esta actualizacion.

Fuentes de parametros de actuadores: [FEETECH STS3250](https://www.feetechrc.com/Data/feetechrc/upload/file/20240120/6384135881578380868917773.pdf) y [STS3032 A/0](https://akizukidenshi.com/goodsaffix/STS3032%20SPEC.pdf). Confirmar variante y alimentacion antes de compra.
