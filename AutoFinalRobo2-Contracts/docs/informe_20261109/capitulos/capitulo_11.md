# 11. Cinemática del autito con ruedas Mecanum

La base se modela como un sólido rígido planar con velocidad de cuerpo V = (v_x, v_y, ω). Se adopta la disposición de rodillos que corresponde al mezclador del proyecto y el orden lógico de ruedas FL, FR, RL, RR. El radio nominal es r = 0,040 m y la suma de semidistancias geométricas es L = 0,170 m. [R1, S3]

Mezclador geométrico canónico A:

```text
[ 1 | −1 | −L ]
[ 1 | 1 | L ]
[ 1 | 1 | −L ]
[ 1 | −1 | L ]
```

```text
u = A S V ;   ω_ruedas = u / r
```

Corrección de sentidos vigente en motion_reference.json:

```text
[ −1 | 0 | 0 ]
[ 0 | −1 | 0 ]
[ 0 | 0 | −1 ]
```

S = diag(s_f, s_l, s_ω) y en el contrato consultado los tres signos valen −1. Es una convención del proyecto basada en sentidos informados por el usuario; el archivo mantiene physicalVerification:false. Es indispensable aplicarla tanto en mezcla como en reconstrucción. u es velocidad periférica en la base de signos del contrato, no ángulo ni PWM.

Inversa geométrica A⁺:

```text
[ 1/4 | 1/4 | 1/4 | 1/4 ]
[ −1/4 | 1/4 | 1/4 | −1/4 ]
[ −1/(4L) | 1/(4L) | −1/(4L) | 1/(4L) ]
```

```text
V = S⁻¹ A⁺ u = S A⁺ u
```

Los signos de calibración de cada encoder y de cada puente H deben llevar la medición a la misma base antes de aplicar la inversa. No se introduce una segunda inversión por conveniencia visual. La matriz es válida para la disposición de ruedas definida; invertir la orientación física de los rodillos cambia el modelo.

## 11.1 Límites de velocidad

```text
γ = max(1, max_i |u_i| / u_max) ;   u_aplicada = u / γ
```

El escalado uniforme conserva la proporción entre avance, lateral y giro. La limitación independiente de cada rueda puede alterar la dirección solicitada. El valor de configuración de 150 mm/s es una consigna nominal de software, no una velocidad máxima física caracterizada.
