# Filmina 5 · Matemática del auto (Mecanum) y cálculos de odometría

**Mensaje clave:** tres velocidades de cuerpo (avance, lateral, giro) se reparten en cuatro ruedas con una matriz; con los pulsos Hall
hacemos el camino inverso y obtenemos la pose.

## Layout (grilla 12 × 6)

```text
┌──────────────────────────────────────────────────────────────────────────┐
│ TÍTULO  c1–c12 · f1   «Cinemática Mecanum y odometría»                    │
├────────────────────────────┬─────────────────────────────────────────────┤
│ A  CINEMÁTICA DIRECTA/INV. │ B  D-11 patrones de rueda                   │
│ fórmulas c1–c6 · f2–f5     │ c7–c12 · f2–f3                              │
│                            ├──────────────────────────┬──────────────────┤
│                            │ C  ODOMETRÍA (fórmulas)  │ D  FOTO-10 +     │
│                            │ c7–c10 · f4–f6           │ valores medidos  │
│                            │                          │ c11–c12 · f4–f6  │
├────────────────────────────┴──────────────────────────┴──────────────────┤
│ E  Pie: parámetros  r=40 mm · L=170 mm · CPR=370 · D=80 mm  c1–c6 · f6   │
└──────────────────────────────────────────────────────────────────────────┘
```

## Contenido por zona

### A · Cinemática (Insertar → Ecuación)
Marco: **X adelante, Y izquierda, Z arriba; ψ positivo antihorario** (visto desde arriba). V = (vₓ, v_y, ω).

1. Cinemática inversa (cuerpo → ruedas), orden FL, FR, RL, RR:

   `u = A · S · V  ;  ω_rueda = u / r`

   `A = [ 1 −1 −L ; 1 1 L ; 1 1 −L ; 1 −1 L ]`  con r = 0,040 m, L = 0,170 m (suma de semidistancias, 62 + 107,66 mm).
2. Cinemática directa (ruedas → cuerpo): `V = S · A⁺ · u` con
   `A⁺ = [ ¼ ¼ ¼ ¼ ; −¼ ¼ ¼ −¼ ; −1/(4L) 1/(4L) −1/(4L) 1/(4L) ]`.
3. Signos del contrato: `S = diag(−1, −1, −1)` (motion_reference.json, `physicalVerification: false`).
4. Límite de velocidad: `γ = max(1, max|uᵢ| / u_max)` ; `u_aplicada = u / γ` (escala todo junto: conserva la dirección). Límite
   configurado 150 mm/s (consigna de software, no máxima física).

**Ejemplos numéricos** (en notas o tarjeta pequeña): avance vₓ = 100 mm/s → uᵢ = 100 mm/s → ω = 2,5 rad/s = 23,9 rpm por rueda ·
giro ω = 0,5 rad/s → |uᵢ| = L·ω = 85 mm/s.

### B · D-11
[D-11_patrones_rueda_mecanum.svg](diagramas/D-11_patrones_rueda_mecanum.svg): tres autos vistos desde arriba (avance, lateral, giro)
con la rueda en verde (u > 0) o rojo (u < 0).

### C · Odometría (Insertar → Ecuación)
1. Pulsos → recorrido por rueda: `Δφᵢ = 2π Δnᵢ / Nᵢ` ; `Δsᵢ = Δnᵢ · π D / Nᵢ` ; `uᵢ ≈ Δsᵢ / Δt`
   (N = 370 cuentas/vuelta, D = 80 mm → **0,679 mm por cuenta**; 1000 mm ≈ 1472 cuentas).
2. Mezcla inversa (en la base de signos del código, Δs ya normalizados):
   `Δx = ¼ (Δs₁+Δs₂+Δs₃+Δs₄)` · `Δy = ¼ (−Δs₁+Δs₂+Δs₃−Δs₄)` · `Δψ = ¼ (−Δs₁+Δs₂−Δs₃+Δs₄) / L`
3. Factor direccional medido: `Δx ·= kₓ± ; Δy ·= k_y± ; Δψ ·= k_ψ±` (avance/retro 1,000 · **lateral 0,775** · giro 1,000).
4. Integración con rumbo medio:
   `ψₘ = ψₖ + Δψ/2` ; `xₖ₊₁ = xₖ + Δx cosψₘ − Δy sinψₘ` ; `yₖ₊₁ = yₖ + Δx sinψₘ + Δy cosψₘ` ; `ψₖ₊₁ = ψₖ + Δψ` (normalizado a ±π).

### D · FOTO-10 y valores medidos
**FOTO-10** (rueda Mecanum de 80 mm). Tabla «Medido por nosotros»:

| Medición | Resultado |
| --- | --- |
| Pulsos Hall por vuelta de rueda | ≈ 370 en las 4 ruedas (sin multiplicar por 4) |
| Avance / retroceso 1000 mm por odometría | 1000 mm reales |
| Lateral 1000 mm por odometría | **775 mm reales** (4 pruebas iguales, izq. y der.) |

Si se toma **FOTO-20** (el auto junto a la marca de 1000 mm en el piso), va aquí.

### E · Pie de parámetros
`r = 40 mm · L = 170 mm · N = 370 · D = 80 mm · resolución 0,679 mm/cuenta · decodificación x1 (flanco ↑ de A, B = sentido)`

## Guion del orador (≈ 90 s)

«Con ruedas Mecanum el auto controla tres velocidades —avance, lateral y giro— usando cuatro ruedas. La matriz A reparte esas tres
en cuatro velocidades de rueda; los signos de cada patrón se ven en el dibujo. El camino de vuelta es la odometría: contamos pulsos
Hall, ya sabemos que cada cuenta son 0,68 mm, y con el promedio de las cuatro ruedas sale el avance, con otra combinación el lateral
y con otra el giro. Esas velocidades se integran usando el rumbo medio. Medimos que en avance la odometría coincide con la realidad,
pero en lateral los rodillos patinan: 1000 mm contados son 775 reales, por eso hay un factor por dirección. No tenemos giroscopio,
así que el rumbo deriva con la distancia.»

## Fuentes

`afr2_geometry.h` · `afr2_motion_reference.h` / `motion_reference.json` · `AutoFinalRobo2-Atmega/src/services/afr2_product.c`
(`odometry()`, líneas 247–286) · `AFR2_Informe_Borrador_D1.md` cap. 11–12 · `contracts/DIRECTIONAL_SCALE_FACTORS_20261111.md`.

## Advertencias

- **Discrepancia a resolver antes de exponer:** el simulador de recolección (`ROBOT_RECOLECTOR.md`) propone Lx+Ly = 220 mm; el firmware y
  el contrato usan **170 mm**. Usar 170 mm y mencionar que el simulador usa un valor propuesto.
- Signos y Hall: «según sentidos informados por el usuario»; no verificado con instrumento.
- Los 370 cpr y 0,775 son mediciones propias aún sin incertidumbre documentada.
