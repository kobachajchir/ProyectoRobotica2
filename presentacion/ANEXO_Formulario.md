# Anexo · Formulario completo con símbolos, unidades y ejemplos

Unidades: ecuaciones en **m, s, rad** (los planos en mm). Marco: X adelante, Y izquierda, Z arriba; ψ positivo antihorario.
Todas las fórmulas están pensadas para pegarse con *Insertar → Ecuación*.

## 1. Transformaciones
| Fórmula | Significado |
| --- | --- |
| `p_A = R_AB p_B + t_AB` | coordenadas de B expresadas en A |
| `T_AC = T_AB T_BC` | composición (el orden importa) |
| `T_AB⁻¹ = [R_ABᵀ, −R_ABᵀ t_AB ; 0, 1]` | inversa |
| `Rz(θ) = [cθ −sθ 0 ; sθ cθ 0 ; 0 0 1]` · `Ry(θ) = [cθ 0 sθ ; 0 1 0 ; −sθ 0 cθ]` | rotaciones elementales |
| `A_i = Rz(θᵢ) Tz(dᵢ) Tx(aᵢ) Rx(αᵢ)` | DH estándar (tabla en informe D1, cap. 6.1) |

## 2. Brazo (descriptor v17)
`h = 0,209 · L₁ = 0,070 · L₂ = 0,115 · d = 0,1435` (m)

- Directa: `ρ = L₁ sin q₂ + L₂ sin(q₂+q₃)` ; `z_W = h + L₁ cos q₂ + L₂ cos(q₂+q₃)` ; `p_TCP = p_W + d R e_z`
- Inversa: `q₁ = atan2(y_W, x_W)` ; `cos q₃ = (ρ² + s² − L₁² − L₂²)/(2L₁L₂)` ; `q₂ = atan2(ρ, s) − atan2(L₂ sin q₃, L₁ + L₂ cos q₃)` ;
  `q₅ = atan2(√(r₁₃²+r₂₃²), r₃₃)` ; `q₄ = atan2(r₂₃, r₁₃)` ; `q₆ = atan2(r₃₂, −r₃₁)`
- Jacobiano: columna `[z_i × (p_TCP − o_i) ; z_i]` ; `[v ; ω] = J q̇` ; `τ = Jᵀ [F ; M]` ; `q̇ = Jᵀ (JJᵀ + λ²I)⁻¹ v_d`
- Incertidumbre: `Σ_p ≈ J_v Σ_q J_vᵀ + J_l Σ_l J_lᵀ`
- Quíntica: `q(t) = q₀ + (q_f − q₀)(10σ³ − 15σ⁴ + 6σ⁵)`, `σ = t/T`
- Transmisión: `|ω_sal| = |ω_mot|/i` ; `|τ_sal| = η i |τ_mot|` ; `1 kgf·cm = 0,0980665 N·m`
- Pinza: `g(θ) = g₀ + 2 r_p θ` (r_p = 12 mm) ; `F_mandíbula ≈ η τ_piñón/(2 r_p)` ; apertura ≈ 4,007…79,405 mm

**Ejemplo (q₂ = 90°, resto 0):** ρ = 0,185 m, z = 0,209 m ⇒ TCP = (0,3285 ; 0 ; 0,209) m.
**Ejemplo (pose de transporte [0, 15°, −40°, 0, 25°, 0]):** β = q₂+q₃ = −25° ; ρ = 0,070 sin15° + 0,115 sin(−25°) = −0,0305 m ; z_W = 0,209 + 0,070 cos15° + 0,115 cos25° = 0,3808 m ;
con q₅ = 25°, `Ry(β)·Ry(q₅) = I` y la herramienta apunta hacia arriba: TCP = (−0,0305 ; 0 ; 0,5243) m.

## 3. Base Mecanum
- `u = A S V` ; `ω_rueda = u/r` ; `V = S A⁺ u` ; `A = [1 −1 −L ; 1 1 L ; 1 1 −L ; 1 −1 L]` ; `r = 0,040 m`, `L = 0,170 m`
- `γ = max(1, max|uᵢ|/u_max)` ; `u_ap = u/γ`
- **Ejemplos:** vₓ = 0,1 m/s ⇒ uᵢ = 0,1 m/s ⇒ ω = 2,5 rad/s = 23,9 rpm · ω = 0,5 rad/s ⇒ |uᵢ| = 0,085 m/s.

## 4. Odometría
- `Δφᵢ = 2π Δnᵢ/Nᵢ` ; `Δsᵢ = Δnᵢ π D/Nᵢ` ; resolución `π D/N = π·80/370 = 0,679 mm/cuenta` ; 1000 mm ⇒ 1472 cuentas
- `Δx = ¼ΣΔsᵢ` ; `Δy = ¼(−Δs₁+Δs₂+Δs₃−Δs₄)` ; `Δψ = ¼(−Δs₁+Δs₂−Δs₃+Δs₄)/L`
- `Δx, Δy, Δψ ·= factor direccional (avance 1,000 · lateral 0,775 · giro 1,000)`
- `ψₘ = ψ + Δψ/2` ; `x += Δx cosψₘ − Δy sinψₘ` ; `y += Δx sinψₘ + Δy cosψₘ` ; `ψ += Δψ`
- `[ẋ ; ẏ ; ψ̇] = [cosψ −sinψ 0 ; sinψ cosψ 0 ; 0 0 1] V`

## 5. Control
- Por rueda: `e = u_ref − u_med` ; `a = sat[(Kp e + Ki ∫e + Kd Δe)/256]` ; Kp 128, Ki 8, Kd 0 ; rampa ±4 por ciclo de 10 ms ; PWM ≤ 255
- GOTO: ver filmina 6 (`eₓ, e_y, e_ψ`; llegada 10 mm / 25 mrad; 60 s)
- Seguidor de línea: `e = Σ wᵢ xᵢ / Σ wᵢ` (en firmware: posiciones 2,1,0,−1,−2 de IR1…IR5), `ω = Kp·e/n`

## 6. Sensores
- Ultrasonido: `ρ = cΔt/2` ; `c ≈ 331,3 + 0,606 T` (m/s, T en °C) ⇒ 343,4 m/s a 20 °C ; cuantización 25 µs ⇒ 4,29 mm
- Punto del eco: `x = ρ cos α` ; `y = ρ sin α` (más la posición real del sensor)
- Batería: `V_bat = ADC/(2ⁿ−1) · V_ref · (R_sup+R_inf)/R_inf` ; con 10k/10k el factor es 2

## 7. Estática, dinámica y vuelco
- `r_G = Σ mᵢ rᵢ/Σ mᵢ` ; `Wᵢ = mᵢ g` ; `g = 9,80665 m/s²`
- `τ_g = Σ J_v,iᵀ mᵢ g` ; `M(q) q̈ + C(q,q̇) q̇ + g(q) + τ_f = τ_act + Jᵀ F_ext`
- `M_est = Σ Wᵢ max(dᵢ,0)` ; `M_vol = Σ Wᵢ max(−dᵢ,0)` ; `FS = M_est/M_vol`
- `y_ZMP = y_CG + a h_CG/g` ; `a_crit = g (b − y_CG)/h_CG`
- Marcha: `F = M [a + g sin α + C_rr g cos α]` ; `τ_rueda ≈ F r/4`
- **Ejemplos del análisis del 01/11:** FS = 1,4855/1,4518 = 1,023 · a_crit = 9,80665·0,00085/0,16194 = 0,0515 m/s² ·
  STS3250: 16 kgf·cm = 1,569 N·m ; J2: 1,569 · 4 · 0,8 = 5,02 N·m (capacidad en articulación).

## 8. Conjunto
- `Q = (x, y, ψ, q₁…q₆)` ; `T_W,TCP = T_W,B(x,y,ψ) T_B,M T_M,TCP(q)`
- Aporte de la base a la velocidad lineal en el punto p: `(vₓ − ω p_y , v_y + ω pₓ , 0)` ; `V_TCP = J_base V_base + J_brazo q̇`

## 9. Conversiones
`1 mm = 10⁻³ m` · `1° = π/180 rad` · `1 rpm = 2π/60 rad/s` · `1 kgf·cm = 0,0980665 N·m`
