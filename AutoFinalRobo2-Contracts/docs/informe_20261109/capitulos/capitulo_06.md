# 6. Modelo cinemático del brazo: cadena y parámetros

Se definen q1…q6 como los ángulos geométricos del descriptor v17, con signo positivo y offset cero en ese archivo. Se fija h = 0,096 + 0,113 = 0,209 m; L1 = 0,070 m; L2 = 0,060 + 0,055 = 0,115 m; d = 0,065 + 0,0785 = 0,1435 m. La agrupación es válida porque los tramos agrupados son colineales y las rotaciones intermedias correspondientes no alteran ese eje de traslación.

```text
T_montura,TCP = Tz(h) Rz(q1) Ry(q2) Tz(L1) Ry(q3) Tz(L2) Rz(q4) Ry(q5) Rz(q6) Tz(d)
```

Esta expresión desarrolla la cadena del descriptor. La distancia física codo–J4 sigue siendo 60 mm: el valor L2 = 115 mm corresponde al modelo equivalente hasta el centro de muñeca, no a un tubo de 115 mm. De forma análoga, d agrupa el tramo distal y el TCP porque la rotación J6 no cambia una traslación sobre su propio eje.

El uso de marcos y productos ordenados de movimientos rígidos sigue la formulación general de cinemática de robots. La cadena particular y la tabla equivalente de esta sección se derivan de los datos propios de AutoFinalRobo2; no se toman como dimensiones de un ejemplo bibliográfico. [R2]

## 6.1 Tabla Denavit–Hartenberg equivalente

Para una exposición académica con DH estándar se propone la tabla siguiente, usando A_i = Rz(θ_i) Tz(d_i) Tx(a_i) Rx(α_i). Son marcos elegidos para esta derivación; θ_i no equivale a la lectura bruta de servo ni a un límite de articulación sin aplicar los offsets de la tabla. No es una tabla exportada por Fusion.

| i | θ_i | d_i [m] | a_i [m] | α_i |
| --- | --- | --- | --- | --- |
| 1 | q1 | 0,209 | 0 | π/2 |
| 2 | π/2 − q2 | 0 | 0,070 | 0 |
| 3 | π/2 − q3 | 0 | 0 | π/2 |
| 4 | q4 − π | 0,115 | 0 | −π/2 |
| 5 | q5 | 0 | 0 | π/2 |
| 6 | q6 | 0,1435 | 0 | 0 |

Matriz DH estándar de una articulación:

```text
[ cθ | −sθ cα | sθ sα | a cθ ]
[ sθ | cθ cα | −cθ sα | a sθ ]
[ 0 | sα | cα | d ]
[ 0 | 0 | 0 | 1 ]
```

c y s abrevian coseno y seno. La cadena completa resulta del producto ordenado A1…A6. La formulación por transformaciones explícitas se mantiene como referencia de lectura porque muestra directamente los ejes Z–Y–Y–Z–Y–Z empleados en el descriptor.
