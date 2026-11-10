# 9. Jacobiano, singularidades y propagación de errores

Para una articulación revoluta i, se expresa en un marco común el eje unitario z_i y un punto o_i de dicho eje. La columna lineal del Jacobiano es el producto vectorial del eje por el vector hasta el TCP; la columna angular es el propio eje. Esta forma admite los ejes Y del modelo al usar su dirección real transformada, sin asumir que todos coinciden con el Z global.

Columna del Jacobiano geométrico:

```text
[ z_i × (p_TCP − o_i) ]
[ z_i ]
```

```text
[v_TCP ; ω_TCP] = J(q) dq/dt
```

```text
τ = J(q)ᵀ [F_TCP ; M_TCP]
```

La segunda ecuación vincula una fuerza y momento externos con pares articulares cuasiestáticos mediante trabajo virtual. Las unidades son N, N·m y radianes. No incluye por sí sola gravedad de eslabones, rozamiento ni inercia.

## 9.1 Singularidades relevantes

En el subproblema de posición, sin(q3) = 0 corresponde a eslabones alineados y pérdida de capacidad en una dirección. Cuando ρ = 0, la posición no fija el ángulo de base de forma única. En la muñeca Z–Y–Z, sin(q5) = 0 hace coincidir las direcciones de los giros primero y tercero. Cerca de estas situaciones pueden crecer las velocidades articulares requeridas para una pequeña velocidad cartesiana.

```text
dq/dt = Jᵀ (J Jᵀ + λ² I)⁻¹ v_d
```

La pseudoinversa amortiguada es una opción de control futuro; λ debe justificarse por precisión, límites de velocidad y cercanía a singularidades. No se afirma que esa ley esté implementada en el robot.

## 9.2 Incertidumbre de posición

```text
δp ≈ J_v δq + J_l δl ;   Σ_p ≈ J_v Σ_q J_vᵀ + J_l Σ_l J_lᵀ
```

δl reúne errores de longitudes, montaje y TCP. La resolución angular del servo no equivale al error del efector: holguras de engranajes, flexión, juego de rodamientos e impresión 3D también contribuyen. Antes de publicar una precisión se deben medir puntos de referencia y separar repetibilidad de exactitud.
