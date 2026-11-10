# 5. Convenciones, unidades y transformaciones homogéneas

Se usan metros, segundos y radianes en las ecuaciones de movimiento; los planos usan milímetros. En la base móvil, X apunta hacia adelante, Y a la izquierda y Z hacia arriba. El giro positivo alrededor de Z es antihorario visto desde arriba. En la captura CAD, X es transversal, Y hacia adelante y Z hacia arriba; por eso la conversión entre ambas referencias se declara explícitamente.

Transformación de A a B, que lleva coordenadas expresadas en B al marco A:

```text
[ r11 | r12 | r13 | px ]
[ r21 | r22 | r23 | py ]
[ r31 | r32 | r33 | pz ]
[ 0 | 0 | 0 | 1 ]
```

```text
p_A = R_AB p_B + t_AB ;   T_AC = T_AB T_BC
```

```text
T_AB⁻¹ = [R_ABᵀ, −R_ABᵀ t_AB; 0, 1]
```

Una columna de R indica la dirección de un eje local expresado en el marco anterior. El orden de las transformaciones importa: girar y luego trasladar en ejes locales puede producir un resultado distinto de trasladar en ejes fijos. El cuaternión del descriptor se interpreta como (x, y, z, w).

Rotación elemental alrededor de Z:

```text
[ cos θ | −sin θ | 0 ]
[ sin θ | cos θ | 0 ]
[ 0 | 0 | 1 ]
```

Rotación elemental alrededor de Y:

```text
[ cos θ | 0 | sin θ ]
[ 0 | 1 | 0 ]
[ −sin θ | 0 | cos θ ]
```

## 5.1 Transformaciones de montaje usadas en la visualización

| Transformación | Traslación [m] | Cuaternión (x,y,z,w) |
| --- | --- | --- |
| Montura del brazo en robot | (0,027819; 0; 0,079041) | (0; 0; −0,70710678; 0,70710678) |
| Conversión CAD al robot | (0,041819; 0; 0,079041) | (0; 0; −0,70710678; 0,70710678) |
| Montura en escena individual | (0; 0; 0,018) | (0; 0; 0; 1) |

La rotación de presentación renderBasis (−0,5; 0,5; 0,5; 0,5) pertenece al visor. No debe aplicarse de nuevo a magnitudes físicas si la conversión CAD→robot ya fue realizada. Las traslaciones de montura y de conversión cumplen funciones distintas; no se suman como dos desplazamientos físicos del mismo origen.
