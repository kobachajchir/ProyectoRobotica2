# ADJUNTO 2 de 4 · contiene las fichas de las filminas 5 a 8.

===== INICIO DEL ARCHIVO: Filmina_05_Matematica_auto_y_odometria.md =====

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


===== FIN DEL ARCHIVO: Filmina_05_Matematica_auto_y_odometria.md =====

===== INICIO DEL ARCHIVO: Filmina_06_GOTO_y_odometria.md =====

# Filmina 6 · Programa GOTO y funcionamiento de la odometría

**Mensaje clave:** el auto se lleva a un punto relativo con un control proporcional sobre la pose odométrica; la Web puede armar
rutas con curvas y enviarlas como tramos.

## Layout (grilla 12 × 6)

```text
┌──────────────────────────────────────────────────────────────────────────┐
│ TÍTULO  c1–c12 · f1   «GOTO y odometría en funcionamiento»               │
├──────────────────────┬───────────────────────────┬───────────────────────┤
│ A  D-10 pipeline     │ B  D-04 flujo GOTO        │ C  Ley de control     │
│ odometría            │ c5–c8 · f2–f6 (vertical)  │ (fórmulas) c9–c12     │
│ c1–c4 · f2–f6        │                           │ f2–f4                 │
│ (vertical)           │                           ├───────────────────────┤
│                      │                           │ D  FOTO-21 / FOTO-22  │
│                      │                           │ capturas Web          │
│                      │                           │ c9–c12 · f5–f6        │
└──────────────────────┴───────────────────────────┴───────────────────────┘
```
Si D-10 y D-04 no entran en vertical, usar dos filas: arriba A y B en miniatura (c1–c8), abajo C y D.

## Contenido por zona

### A · D-10 ([D-10_pipeline_odometria.svg](diagramas/D-10_pipeline_odometria.svg))
Pulsos Hall (IRQ PCINT2, máscara 0x0F) → Δn → normalización de signos → Δs → mezcla inversa → factor direccional → rumbo medio →
**pose (x, y, ψ)**. Bajo el diagrama: *cada 10 ms; |Δn| > 100 000 invalida la pose (falla F2).*

### B · D-04 ([D-04_flujo_GOTO.svg](diagramas/D-04_flujo_GOTO.svg))
Pedido (0x16) → validaciones → objetivo en el mundo → lazo cada 10 ms: ¿pose válida y t < 60 s? → error → ¿llegó? → marco del
robot → mínimo 40 mm/s y normalización → `Drive`.

### C · Ley de control (Insertar → Ecuación)
1. Objetivo en el mundo (al aceptar el pedido, con X, Y relativos al rumbo actual):
   `x_m = x + X cosψ − Y sinψ` ; `y_m = y + X sinψ + Y cosψ` ; `ψ_m = ψ + θ`
2. Error: `eₓ = x_m − x` ; `e_y = y_m − y` ; `e_ψ = ψ_m − ψ` (a ±π).
3. Al marco del robot: `vₓ = eₓ cosψ + e_y sinψ` ; `v_y = −eₓ sinψ + e_y cosψ` ; `ω = e_ψ`
   (control **proporcional**: ≈ error en mm/s, saturado a la velocidad pedida).
4. Mínimo 40 mm/s mientras esté fuera de tolerancia (por debajo, los motores no vencen la zona muerta).
5. Normalización: `|vₓ| + |v_y| + |ω|·L ≤ v` (conserva la dirección).
6. **Llegada:** `|eₓ|,|e_y| ≤ 10 mm` y `|e_ψ| ≤ 25 mrad` (≈ 1,4°); **tiempo máximo 60 s**; giro puro si X = Y = 0.
7. Límites del pedido: `|X|,|Y| ≤ 5000 mm`, `|θ| ≤ 3140 mrad`, `10 ≤ v ≤ límite`; requiere motores calibrados y pose válida.

Cadena aguas abajo: (vₓ, v_y, ω) → cinemática inversa (filmina 5) → uᵢ → PID por rueda (Kp 128, Ki 8, Kd 0, Q8 ÷256) → rampa → PWM.

### D · Programa en la Web (capturas)
- **FOTO-21** panel GOTO (`#/programas`): lista de comandos (mover por dirección y distancia, punto X/Y, arcos y curvas), mapa con la
  ruta simulada.
- **FOTO-22** *Puesta a punto → Odometría*: prueba de distancia que mide la deriva.
- Texto: *«La ruta simulada se envía como tramos consecutivos `robot.goto.start`; el auto acepta un objetivo recto por vez, la Web
  espera `robot.state` antes del siguiente. Arcos y curvas se envían como polilínea (paso 150 mm; ≥ 20 mm por tramo; ≤ 5000 mm).»*
- También por OLED: *Estado → Modos/misiones → GOTO relativo* (**FOTO-23**).

## Guion del orador (≈ 90 s)

«La odometría convierte pulsos en pose cada diez milisegundos; con la pose, el GOTO es un control proporcional: calculo el
error al punto, lo roto al marco del robot y lo mando como velocidad, con un mínimo de 40 mm/s para vencer la zona muerta. Se
considera llegado a 10 milímetros y 25 miliradianes, con un máximo de un minuto. Si falla la pose o el tiempo, se detiene y queda
registrado. En la Web armamos rutas, incluso con arcos y curvas, las simulamos y las enviamos como tramos rectos consecutivos. Sin
giroscopio, la deriva de rumbo crece con la distancia, por eso medimos con la prueba de odometría antes de confiar en rutas largas.»

## Fuentes

`afr2_product.c` líneas 176–196 (`StartGotoFull`) y 352–381 (modo GOTO) · `AutoFinalRobo2-Web/src/features/control/goto/useGotoRouteRunner.ts`,
`gotoSimulation.ts` · `docs/PLAN_GOTO_ARCOS_Y_ENVIO.md` · `documentation/UI_SCREEN_FLOW.md` (GOTO relativo 03000d) · `changes/2026-11-10-16-goto-envio-giros-rail-tactil.md`.

## Advertencias

- Código leído, **sin prueba física de llegada**: no dar errores de posición medidos si no se tienen.
- El plan antiguo dice «GOTO solo simulado»; quedó superado por el envío por tramos (1.23.x). Usar el estado actual.
- Curvas y arcos pasan por detenciones breves en cada vértice y rumbo constante (θ = 0).


===== FIN DEL ARCHIVO: Filmina_06_GOTO_y_odometria.md =====

===== INICIO DEL ARCHIVO: Filmina_07_Matematica_brazo_y_conjunto.md =====

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


===== FIN DEL ARCHIVO: Filmina_07_Matematica_brazo_y_conjunto.md =====

===== INICIO DEL ARCHIVO: Filmina_08_Esfuerzos_y_vuelco.md =====

# Filmina 8 · Esfuerzos, tensiones, vuelco y estabilidad

**Mensaje clave:** los servos tienen margen de par, pero **con el brazo extendido hacia adelante el conjunto queda al límite del
vuelco** (factor 1,023 frente a 1,2 exigido). Es el resultado más importante de la mecánica y se muestra con honestidad.

## Layout (grilla 12 × 6)

```text
┌──────────────────────────────────────────────────────────────────────────┐
│ TÍTULO  c1–c12 · f1   «Esfuerzos, pares y estabilidad al vuelco»          │
├────────────────────────┬─────────────────────────┬───────────────────────┤
│ A  MASAS Y CARGAS      │ B  PARES Y MÁRGENES     │ C  VUELCO             │
│ tabla c1–c4 · f2–f4    │ tabla c5–c8 · f2–f4     │ FOTO-29 c9–c12 · f2–f4│
├────────────────────────┴─────────────────────────┼───────────────────────┤
│ D  MÉTODO (fórmulas) c1–c8 · f5–f6               │ E  Tabla de vuelco    │
│                                                  │ c9–c12 · f5–f6        │
└──────────────────────────────────────────────────┴───────────────────────┘
```

## Contenido por zona

### A · Masas y cargas (CAD; PLA macizo 1,24 g/cm³)
| Elemento | Masa |
| --- | ---: |
| PLA (volumen sólido modelado) | 788,97 g |
| Acero (ejes, tornillos, herrajes) | 566,78 g |
| 5 × STS3250 (75,5 g) + 2 × STS3032 (21,6 g) | 377,50 + 43,20 g |
| Aluminio · rodamientos · electrónica | 217,76 · 155,00 · 177,00 g |
| **Brazo con montura/control, sin carga** | **2.344,99 g** |
| Carga (cubo 50 mm) + auto (dato aproximado) | 200 g + 1.500 g |
| **Total del conjunto** | **4.044,99 g (39,67 N)** |

### B · Pares y márgenes (STS3250: **16 kgf·cm nominal a 12 V = 1,569 N·m**, no el de bloqueo)
| Eje | Reducción | Par exigido al servo (pose horizontal, 200 g) | Margen estático |
| --- | --- | ---: | ---: |
| J2 | 4 : 1 (η 0,8) | 0,609 N·m | 2,58 |
| J3 | 2,5 : 1 (η 0,8) | 0,596 N·m | 2,63 |
| J4 | 1 : 1 (η 0,8) | 0,079 N·m | 19,9 |
| J5 | 1 : 1 (η 0,95) | 0,447 N·m | 3,51 |

Pares gravitatorios en la articulación: J2 1,949 · J3 1,192 · J5 0,425 N·m. Reacción en J1: 19,2 N vertical, flexión ≈ 1,97 N·m.
Frenada hipotética 30 °/s en 50 ms (900 °/s² pico): márgenes de actuador 1,27–1,59 (v17 base).
Esfuerzos de tubo/uniones **no aprobados**: falta FEA, PLA real (E = 1500 MPa hipotético), tolerancias.

### C · FOTO-29 (proyección del centro de masa sobre el apoyo)
Pie: *Apoyo nominal 214 × 124 mm inferido del CAD, no es la huella real de los rodillos.* Marcar en la imagen (con color) el punto rojo
«brazo hacia adelante: 0,85 mm del borde». Opcional **FOTO-30** (cargas y momentos) y **FOTO-31** (render del brazo extendido).

### D · Método (Insertar → Ecuación)
- Centro de masa: `r_G = Σ mᵢ rᵢ / Σ mᵢ`
- Peso: `Wᵢ = mᵢ g` , g = 9,80665 m/s²
- Momentos respecto del borde: `M_est = Σ Wᵢ · max(dᵢ, 0)` ; `M_vol = Σ Wᵢ · max(−dᵢ, 0)` ; **`FS = M_est / M_vol`**
- Par gravitatorio de articulación: `τ_g = Σ J_v,iᵀ mᵢ g` ; dinámica completa:
  `M(q) q̈ + C(q,q̇) q̇ + g(q) + τ_f = τ_act + Jᵀ F_ext`
- Selección: `τ_servo = τ_art / (η · i)` ≤ par nominal / margen 1,2
- Aceleración crítica de vuelco: `a_crit = g (b − y_CG) / h_CG`

### E · Tabla de vuelco (brazo horizontal, 200 g)
| Dirección del brazo | Distancia del CG al borde | FS |
| --- | ---: | ---: |
| Lateral (J1 = 0°) | 59,42 mm | 3,63 |
| Atrás (J1 = −90°) | 24,07 mm | 1,86 |
| **Adelante (J1 = +90°)** | **0,85 mm** | **1,023** |

Frente: M_est 1,4855 N·m vs M_vol 1,4518 N·m → diferencia 0,034 N·m (equivale al peso de ≈ 28 g). `a_crit ≈ 0,05 m/s²`; pendiente de 0,30°
consume el margen. Auto de 1,445 kg daría equilibrio neutro. Aligerando PLA al 50 % de masa: FS 1,112 (sigue < 1,2). Barrido: 43.798 configuraciones.

## Guion del orador (≈ 100 s)

«Con las masas del CAD —2,3 kilos de brazo, 1,5 del auto y 200 gramos de carga— calculamos pares y estabilidad. Los servos pasan
con margen: entre 2,6 y casi 20 veces el par estático requerido, usando el par nominal, no el de bloqueo. El problema es el vuelco:
con el brazo extendido hacia adelante, el centro de masa queda a menos de un milímetro del borde del apoyo, factor 1,02 frente a
un criterio de 1,2. Un brazo hacia el costado tiene factor 3,6. La conclusión de diseño es que el alcance geométrico no es alcance
operativo seguro: hay que limitar la postura de transporte o agregar apoyos. Esto lo marcamos como el primer punto a resolver antes de
cargar el brazo en el auto.»

## Fuentes

`AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/revision_v17/analisis_conjunto_20261101/INFORME.md` · `revision_v17/INFORME_PESOS_Y_ESFUERZOS_v17.md` ·
`analisis_cargas_200g/INFORME_ITERACION_DISENO.md` · `AFR2_Informe_Borrador_D1.md` cap. 15 · `estabilidad_planta.png`.

## Advertencias

- Resultados **históricos del 01/11/2026**, calculados sobre CAD (no sobre un prototipo pesado). No se recalcularon.
- Auto de 1,5 kg es dato aproximado del usuario; apoyo y altura de CG son hipótesis (30–60 mm de sensibilidad).
- Márgenes de par son estáticos; no cubren frenada ni resistencia del PLA. **No hay FEA ni ensayo de rotura.**
- Los textos de la v13/v14 del repositorio (otros alcances/masas) son anteriores: usar solo v17.


===== FIN DEL ARCHIVO: Filmina_08_Esfuerzos_y_vuelco.md =====