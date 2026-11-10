# 15. Gravedad, dinámica y estabilidad: resultados disponibles

Para masa m_i y centro r_i en el mismo marco, el centro de masa global es el promedio ponderado. El peso debe proyectarse con la gravedad y las orientaciones reales de cada pieza. Las densidades CAD no describen automáticamente el relleno, paredes, tornillería faltante o cables de una impresión real.

```text
r_G = (Σ_i m_i r_i) / (Σ_i m_i)
```

```text
τ_g = Σ_i J_v,i(q)ᵀ m_i g ;   g = (0,0,−9,80665) m/s²
```

La expresión es el par de fuerzas gravitatorias según la convención del Jacobiano; el actuador debe ejercer el par opuesto para sostener el equilibrio. En dinámica completa se agregan inercia, Coriolis y rozamiento:

```text
M(q) d²q/dt² + C(q,dq/dt) dq/dt + g_q(q) + τ_f = τ_act + Jᵀ F_ext
```

M es simétrica definida positiva para un mecanismo no degenerado. No se dispone aquí de una identificación dinámica ni de una caracterización del rozamiento, por lo que la ecuación expresa la estructura del modelo y no resultados medidos.

## 15.1 Análisis histórico del 01/11/2026

| Resultado histórico | Valor / significado |
| --- | --- |
| Masa estimada de brazo, montura y control | 2,34499 kg a partir de volúmenes y densidades CAD. |
| Hipótesis de masa del autito y carga | 1,5 kg y 0,2 kg respectivamente; total de referencia 4,04499 kg. |
| Apoyo de referencia | Rectángulo nominal 214 × 124 mm; no sustituye el contacto real de ruedas. |
| Pose extendida hacia adelante | Margen proyectado al borde ≈0,85 mm; factor estático 1,023. |
| Criterio usado en ese análisis | Factor requerido 1,2: la pose anterior no lo satisface. |
| Barrido de ese informe | 43.798 configuraciones muestreadas; no una prueba continua de todo el espacio. |
| Márgenes de actuadores en una pose | J2 2,58; J3 2,63; J4 19,91; J5 3,51 según hipótesis del análisis. |

Estos números proceden del informe histórico conservado en analisis_conjunto_20261101/INFORME.md. No se recalcularon en esta entrega. El resultado frontal impide describir el conjunto como estable en todo el alcance. Un margen de par favorable tampoco corrige un riesgo de vuelco.

## 15.2 Modelo de vuelco y marcha

```text
M_estabilizante − M_volcante > 0 ;   FS = M_estabilizante / M_volcante
```

La proyección del centro de masa debe permanecer dentro del polígono de apoyo con margen. En aceleración, el centro de presión y las fuerzas inerciales cambian respecto del caso estático. La fuerza aproximada de marcha F = M[a + g sin(α) + Crr g cos(α)] y el par de rueda τ ≈ Fr/4 solo sirven como estimación bajo reparto uniforme; el desplazamiento lateral Mecanum y el deslizamiento requieren ensayos específicos.
