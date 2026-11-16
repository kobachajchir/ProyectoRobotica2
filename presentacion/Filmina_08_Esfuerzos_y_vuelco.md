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
