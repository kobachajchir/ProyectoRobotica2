# 1. Cargas, eje y rodamientos de J1

[Índice](README.md) · [Uniones y materiales](02_uniones_y_materiales.md)

## Estados y sistemas

Se reutilizan las masas, centroides, inercias y trayectorias de `revision_v08`, con cubo de 200 g, PETG equivalente macizo y demás hipótesis allí documentadas. La postura de referencia totalmente extendida es q=(90°,90°,0°,0°,0°,0°). Se rota el conjunto y sus cargas alrededor de J1 a 19 orientaciones finales entre −90° y +90°, cada 10°. Se incluyen 200, 100 y 50 ms desde 30°/s, un eje por vez, con ambos sentidos cuando los límites nativos permiten llegar a esa postura. Se parte de 379 estados fuente de 19 casos (incluida estática), incorporando 63 instantes nuevos del sentido negativo de J1 mediante el mismo modelo dinámico. Se evalúan 7.075 estados tras excluir trayectorias fuera de límites. No se hizo barrido de colisiones de las trayectorias adicionales; la admisibilidad geométrica continua queda pendiente.

Las fuerzas/momentos de este estudio son las cargas del brazo móvil **sobre el soporte fijo**, opuestas a las que el soporte ejerce sobre el brazo. Se expresan en N y N·mm en el cálculo. El punto J1 está en (0,−14,96) mm. El par máximo alrededor de Z y la máxima flexión provienen de estados diferentes; no se suman sus máximos como si fueran simultáneos.

| Caso, t=25 ms | Resultado principal |
| --- | --- |
| Frenada J1, 50 ms | Par alrededor de Z=1,596 N·m=16,27 kgf·cm |
| Frenada J2, 50 ms | Flexión perpendicular a J1=4,522 N·m; axial ≈21,56 N |
| Estática | Flexión perpendicular a J1=2,962 N·m |

El MG946R anuncia 13 kgf·cm a 6 V. La demanda calculada supera ese valor, por lo que no puede suponerse que el motor siga esa trayectoria. Se trata de un **caso de diseño solicitado al mecanismo**, no de una frenada medida que el motor actual haya realizado.

## Separación de caminos resistentes

Los 608 permiten girar alrededor de Z. En el modelo idealizado, llevan la resultante radial/axial y el par de flexión; no se les asigna el par de accionamiento completo alrededor de Z. Ese par se transmite desde la carcasa del motor a sus fijaciones y postes.

El par del accionamiento llega al plato móvil por horn, eje y acople D. Por eso sustituir un servo aumenta la capacidad disponible, pero exige revisar por separado carcasa/postes y acople de salida. El marco de columnas del cartucho no recibe artificialmente todo el torque del servo en este cálculo.

## Reacciones de los dos 608

El CAD sitúa los centros de los rodamientos en Z=77,5 y 85,5 mm: separación 8 mm. Se resuelven equilibrio de fuerza transversal y momento, incluyendo la distancia al punto de aplicación en Z=96 mm. En coordenadas relativas a ese punto, zₐ=−18,5 y zᵦ=−10,5 mm:

```text
Rax + Rbx = −Fx
zₐ Rax + zᵦ Rbx = −My
Ray + Rby = −Fy
zₐ Ray + zᵦ Rby = Mx
```

Aquí F/M son las cargas externas sobre el eje y R las reacciones de los apoyos sobre él. Se asigna conservadoramente toda la carga axial a cualquiera de los dos rodamientos para la comparación individual. Para la referencia de ranura profunda se usa P₀=max(Fr; 0,6Fr+0,5Fa).

El máximo es **Fr=P₀≈565,20 N** en el apoyo inferior, durante la frenada de J2. Con C₀ de referencia 1.370 N resulta C₀/P₀≈2,42. Es una comparación de selección para esos estados, no un factor de seguridad del conjunto ni una garantía de un rodamiento genérico. No se calculan vida, juego, desalineación, precarga o impacto. [Fuentes](FUENTES.md).

La presión media proyectada de asiento Fr/(D·B), usando 22×7 mm, es ≈3,67 MPa. No equivale a la máxima presión real del aro sobre PETG.

| Separación entre centros | Fuerza del par ideal M/d, con M=4,522 N·m | Presión proyectada 608 |
| --- | ---: | ---: |
| 8 mm actual | 565,24 N | 3,67 MPa |
| 16 mm estudiada como objetivo | 282,62 N | 1,84 MPa |
| 24 mm, sensibilidad | 188,41 N | 1,22 MPa |
| 32 mm, sensibilidad | 141,31 N | 0,92 MPa |

Las últimas filas cambian solo la separación en la fórmula; no representan montajes construidos ni incorporan fuerzas de una transmisión nueva.

## Eje Ø8, plano y agujero axial

Se utilizan las secciones reales extraídas del eje nativo, incluyendo plano D a 3,3 mm del centro y agujero axial Ø3,3. Las integrales de área y segundos momentos se obtienen de los contornos; la tensión normal se calcula con flexión no simétrica y carga axial:

```text
sigma(x,y) = N/A + a(x−xc) + b(y−yc)
[ Iy  Ixy ] [a] = [−My]
[ Ixy Ix  ] [b]   [ Mx]
```

Para las secciones por debajo de cada centro de rodamiento se incluyen sus reacciones. En la parte superior se conserva la envolvente de momento transmitido: la distribución real de contacto a lo largo del cubo y la tensión en escalones requieren análisis local posterior.

La máxima tensión normal nominal obtenida es **94,13 MPa**, en una sección superior con plano/agujero, frente a ≈61,69 MPa en estática. La torsión se informa como referencia circular Ø8; **no se presenta como solución exacta de torsión del perfil D**. En el estado que gobierna la flexión, el torque Z es pequeño. Los CSV permiten revisar otros instantes y combinaciones de componentes del mismo estado.

Con un criterio provisional n=2 y concentración Kt=2, la referencia equivalente exige aproximadamente **377 MPa de fluencia**. Son factores de estudio, no valores calculados del detalle ni una norma aplicada. No se conoce el acero real del eje y no se puede atribuirle esa resistencia por tener apariencia metálica en Fusion.

Como sensibilidad de predimensionado para una sección circular maciza, se obtienen aproximadamente:

| Diámetro | Tensión equivalente nominal del caso gobernante | Fluencia requerida con n=2 y Kt=2 |
| --- | ---: | ---: |
| 8 mm | 90,39 MPa | 361,56 MPa |
| 10 mm | 46,33 MPa | 185,34 MPa |
| 12 mm | 26,85 MPa | 107,38 MPa |

Esta tabla no incluye nuevos planos, roscas, pasadores, brida o concentraciones y no sustituye el cálculo del nuevo eje completo. Sustenta estudiar **Ø10 mm** con acero identificado y apoyos 6000, antes que conservar un Ø8 de grado desconocido y modificarlo con más muescas.

## Consistencia numérica

Los residuos máximos de equilibrio radial son ≈1,17×10⁻¹³ N y 2,73×10⁻¹² N·mm. Los finales de frenada, con velocidad y aceleración nulas, coinciden con la estática hasta ≈7,1×10⁻¹⁵ N y 9,1×10⁻¹³ N·mm. Esto comprueba signos y equilibrio de las ecuaciones utilizadas, no la fidelidad física de las hipótesis.
