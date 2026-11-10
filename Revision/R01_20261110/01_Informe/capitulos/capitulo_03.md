# 3. Configuración mecánica v17

![Figura 2. Configuración CAD del brazo. El libro complementario individualiza las piezas y seis conjuntos; las dimensiones de este capítulo se expresan en milímetros.](figuras/brazo_v17.png)

| Magnitud | Valor nominal | Interpretación |
| --- | --- | --- |
| Hombro–codo | 70 mm | Eslabón geométrico L1. |
| Codo–eje J4 | 60 mm | Primer tramo distal al codo. |
| J4–J5 | 55 mm | Separación longitudinal hasta el centro de muñeca. |
| J5–J6 | 65 mm | Tramo sobre el eje distal; interviene en el offset del TCP. |
| J6–TCP | 78,5 mm | Distancia de la herramienta definida en el descriptor. |
| PVC del brazo | Ø25 × 1,2 mm | Interior nominal 22,6 mm. |
| PVC del antebrazo | Ø20 × 1 mm | Interior nominal 18 mm. |
| Cortes de tubos | 42 mm y 28,5 mm | Longitudes de pieza; no confundir con distancia entre ejes. |
| Accionamientos | 5 STS3250 + 2 STS3032 A0 | J1–J5; J6 y pinza respectivamente. |

Los parámetros antiguos pvc_od32 y otros nombres heredados permanecen en el documento nativo. La interpretación de la configuración vigente se realiza a partir de los sólidos, las interfaces y el descriptor v17, no solamente por el nombre de un parámetro o del archivo.

| Conjunto | Función principal |
| --- | --- |
| A100 · Montura y control fijo | Une el brazo a la plataforma y reserva los componentes de control. |
| A200 · Base y hombro | J1 y transmisión de J2 mediante engranajes 18/72. |
| A300 · Brazo | Primer eslabón y accionamiento remoto de J3 por correa. |
| A400 · Antebrazo y J4 | Segundo eslabón y giro de muñeca. |
| A500 · J5 y soporte J6 | Inclinación de muñeca y alojamiento del eje distal. |
| A600 · Herramienta | Giro J6, piñón y dos mandíbulas de la pinza V7. |

El accionamiento de J3 está montado sobre el eslabón anterior y acompaña a J2. La correa no atraviesa la articulación J4. Esta ubicación afecta la masa transportada por el hombro y debe respetarse al construir un modelo dinámico.
