# Filmina 7 · Matemática del brazo y del conjunto

**Mensaje clave:** la cadena Z–Y–Y–Z–Y–Z del brazo se resuelve en forma cerrada (muñeca esférica); al montarla sobre la base
Mecanum el conjunto tiene 9 grados de libertad de configuración y una sola cadena mundo → TCP.

## Layout (grilla 12 × 6)

```text
┌──────────────────────────────────────────────────────────────────────────┐
│ TÍTULO  c1–c12 · f1   «Cinemática del brazo y del conjunto»               │
├──────────────────────────────────────────────────────────────────────────┤
│ A  D-08 cadena cinemática (banda ancha)  c1–c12 · f2–f3                  │
├────────────────────┬───────────────────────┬─────────────────────────────┤
│ B  DIRECTA         │ C  INVERSA            │ D  CONJUNTO + JACOBIANO     │
│ c1–c4 · f4–f6      │ c5–c8 · f4–f6         │ c9–c12 · f4–f5              │
│                    │                       ├─────────────────────────────┤
│                    │                       │ E  FOTO-25 / FOTO-26 mini   │
│                    │                       │ c9–c12 · f6                 │
└────────────────────┴───────────────────────┴─────────────────────────────┘
```

## Contenido por zona

### A · D-08 ([D-08_cadena_cinematica.svg](diagramas/D-08_cadena_cinematica.svg))
Banda horizontal: Mundo → T_W,B(x,y,ψ) → T_B,M → Tz(h) → Rz(q1) → Ry(q2) → Tz(L1) → Ry(q3) → Tz(L2) → Rz(q4) → Ry(q5) → Rz(q6) → Tz(d) → TCP.
Parámetros (agrupando tramos colineales): **h = 0,209 m · L₁ = 0,070 m · L₂ = 0,115 m · d = 0,1435 m**.

### B · Cinemática directa
Transformación de B a A: `p_A = R_AB p_B + t_AB` ; `T_AC = T_AB T_BC` ; `T⁻¹ = [Rᵀ, −Rᵀt ; 0, 1]`.

`T_M,TCP = Tz(h) Rz(q1) Ry(q2) Tz(L1) Ry(q3) Tz(L2) Rz(q4) Ry(q5) Rz(q6) Tz(d)`

Centro de muñeca W (con β = q₂ + q₃):
`ρ = L₁ sin q₂ + L₂ sin(q₂+q₃)` ; `z_W = h + L₁ cos q₂ + L₂ cos(q₂+q₃)` ; `W = (ρ cos q₁, ρ sin q₁, z_W)`

Orientación y herramienta: `R = Rz(q₁) Ry(q₂+q₃) Rz(q₄) Ry(q₅) Rz(q₆)` ; `p_TCP = p_W + d · R · e_z`

**Ejemplo (pose horizontal q₂ = 90°, resto 0):** ρ = 0,070 + 0,115 = 0,185 m · z = 0,209 m → **TCP = (0,3285 ; 0 ; 0,209) m**
(alcance horizontal 328,5 mm desde el eje J1).
*DH equivalente:* tabla de 6 filas en el informe (cap. 6.1); mostrar solo en notas.

### C · Cinemática inversa (desacople en la muñeca)
1. `p_W = p_d − d · R_d · e_z`
2. `q₁ = atan2(y_W, x_W)` ; `ρ = √(x_W² + y_W²)` ; `s = z_W − h`
3. `cos q₃ = (ρ² + s² − L₁² − L₂²) / (2 L₁ L₂)` ; `q₃ = ± arccos(·)` (dos codos, requiere |cos q₃| ≤ 1)
4. `q₂ = atan2(ρ, s) − atan2(L₂ sin q₃, L₁ + L₂ cos q₃)`
5. Muñeca Z–Y–Z: `q₅ = atan2(√(r₁₃² + r₂₃²), r₃₃)` ; `q₄ = atan2(r₂₃, r₁₃)` ; `q₆ = atan2(r₃₂, −r₃₁)`
6. Se filtran las ramas por límites, autocolisión y continuidad. Singularidades: sin q₃ = 0 (brazo estirado) · ρ = 0 · sin q₅ = 0.

*Alcance de la implementación Web:* solo toma vertical con J4 y J6 fijados (resuelve dos eslabones y herramienta hacia abajo); **no**
es un solver general de seis ejes.

### D · Conjunto y Jacobiano
- Configuración: `Q = (x, y, ψ, q₁ … q₆)` — 9 DOF (la pinza no suma pose).
- `T_W,TCP(Q) = T_W,B(x,y,ψ) · T_B,M · T_M,TCP(q)`
- Velocidad de la herramienta: `V_TCP = J_base · V_base + J_brazo · q̇`, con aporte de la base `(vₓ − ω p_y, v_y + ω pₓ, 0)`.
- Columna i del Jacobiano de una junta rotativa: `[ zᵢ × (p_TCP − oᵢ) ; zᵢ ]`; velocidades `q̇ = Jᵀ(JJᵀ + λ²I)⁻¹ v_d` (pseudoinversa amortiguada, **propuesta**).
- Pares cuasiestáticos: `τ = Jᵀ [F ; M]`. Trayectoria articular quíntica: `q(t) = q₀ + (q_f − q₀)(10σ³ − 15σ⁴ + 6σ⁵)`, σ = t/T.
- Límites: J1 ±90° · J2 −10…+125° · J3–J6 ±90°.

### E · FOTO-25 / FOTO-26
Capturas de `#/simulacion/brazo` (articulaciones arrastrables y matrices homogéneas en vivo) y `#/simulacion/conjunta`.
Opcional **FOTO-27** (una lámina del libro de planos) y **FOTO-28** (Fusion con ejes J1–J6 rotulados).

## Guion del orador (≈ 100 s)

«El brazo es una cadena de seis rotaciones: Z, Y, Y, Z, Y, Z. Con la muñeca de ejes concurrentes podemos separar posición y
orientación: primero calculamos hasta dónde llega el centro de muñeca con dos eslabones de 70 y 115 milímetros equivalentes, y la
herramienta suma 143 milímetros. En la directa es un producto de transformaciones; en la inversa se resta el offset de la
herramienta, se resuelve el triángulo del codo con el teorema del coseno y se obtienen los ángulos de muñeca de la matriz de
rotación. Al montar el brazo sobre el auto, la pose del TCP en el mundo es el producto de tres transformaciones: la base, la montura
y el brazo. Ahí aparece la redundancia: base y brazo pueden compartir la tarea, pero eso arrastra el centro de masa, tema de la
próxima filmina.»

## Fuentes

`AFR2_Informe_Borrador_D1.md` cap. 5–9 y 14 · `AutoFinalRobo2-Web/src/features/collector/armKinematics.ts`, `models/pvc25-v17-blanco.json` ·
`docs/ROBOT_RECOLECTOR.md` (matemática implementada).

## Advertencias

- Las ecuaciones son el **desarrollo propuesto** del informe D1; no hubo comparación numérica independiente con el descriptor ni medición
  de poses en un brazo real. No dar precisión de TCP.
- Comentarios históricos del código mencionan eslabones 130/120 mm: el descriptor v17 vigente es 70/60 (+55/65/78,5).
- Pseudoinversa amortiguada, planificador base+brazo y envolventes de trabajo: **no implementados**.
