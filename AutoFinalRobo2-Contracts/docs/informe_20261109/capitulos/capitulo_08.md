# 8. Cinemática inversa y selección de soluciones

Para una pose deseada (p_d, R_d), primero se resta el offset de herramienta y se obtiene el centro de muñeca. Esto desacopla idealmente posición y orientación en el modelo de ejes de muñeca concurrentes. El desacople no elimina los límites físicos ni las colisiones.

```text
p_W,d = p_d − d R_d e_z
```

```text
q1 = atan2(y_W, x_W) ;   ρ = √(x_W² + y_W²) ;   s = z_W − h
```

```text
c3 = (ρ² + s² − L1² − L2²) / (2 L1 L2)
```

```text
q3 = ± acos(c3)
```

```text
q2 = atan2(ρ, s) − atan2(L2 sin(q3), L1 + L2 cos(q3))
```

Debe cumplirse |c3| ≤ 1 dentro de la tolerancia numérica elegida. Las dos ramas representan configuraciones de codo diferentes. Si se admite un alcance radial firmado, también debe contemplarse la rama asociada al giro alternativo de base; la solución debe evaluarse con los límites reales y continuidad respecto de la pose actual.

```text
R_W = [Rz(q1) Ry(q2 + q3)]ᵀ R_d = Rz(q4) Ry(q5) Rz(q6)
```

Para una rama Z–Y–Z no singular con 0 < q5 < π y elementos r_ij de R_W:

```text
q5 = atan2(√(r13² + r23²), r33)
```

```text
q4 = atan2(r23, r13) ;   q6 = atan2(r32, −r31)
```

La rama equivalente (q4 + π, −q5, q6 + π) permite buscar soluciones compatibles con el rango documentado de q5. Si sin(q5) = 0, q4 y q6 no se determinan por separado: debe elegirse una continuidad o criterio adicional. El conjunto de candidatos se filtra por límites, autocolisión, entorno, estabilidad y variación mínima entre consignas.

## 8.1 Alcance de la implementación disponible

La implementación Web consultada resuelve un problema restringido de herramienta orientada hacia abajo con J4 y J6 fijados. No se declara implementado un solucionador numérico general de seis grados de libertad. Las ecuaciones de esta sección son el desarrollo propuesto para el informe y una base para una implementación posterior expresamente autorizada.

Conviene reemplazar comentarios históricos con dimensiones 130/120 por una referencia al descriptor: el descriptor vigente tiene tramos de 70/60 mm y los agrupamientos equivalentes explicados antes. En esta entrega se registra la discrepancia sin modificar código.
