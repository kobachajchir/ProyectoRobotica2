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
