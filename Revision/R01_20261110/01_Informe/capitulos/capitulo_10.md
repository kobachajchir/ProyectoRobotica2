# 10. Transmisiones, actuadores y pinza

| Eje | Transmisión nominal | Relación motor/salida |
| --- | --- | --- |
| J1 | Directa | 1 |
| J2 | Piñón 18 / rueda 72, módulo 0,8 | 4 |
| J3 | Correa HP3M 210 × 6; poleas 20/50 | 2,5 |
| J4 | Engranajes 24/24 | 1 |
| J5 y J6 | Directa | 1 |

```text
i = ω_motor / ω_salida ;   |ω_salida| = |ω_motor| / i ;   |τ_salida| = η i |τ_motor|
```

η es la eficiencia efectiva. Un engrane exterior cambia el sentido relativo; una correa abierta normalmente lo conserva. El signo definitivo de cada articulación requiere el cero mecánico, el montaje del motor y el sentido del controlador. No se sustituyen esos signos por el cociente de dientes.

La hoja del STS3250 informa, a 12 V, un par nominal de 16 kgf·cm y un par de bloqueo de 50 kgf·cm. La conversión es 1 kgf·cm = 0,0980665 N·m. El par de bloqueo no es un punto de operación continua. La selección debe comparar el par exigido con el nominal y el ciclo de trabajo, temperatura, velocidad y tensión efectiva. [R3]

## 10.1 Apertura de pinza

La pinza V7 conserva dos cremalleras con un piñón de 24 dientes, módulo 1 y radio primitivo de 12 mm. El intervalo geométrico documentado de apertura es aproximadamente 4,007 a 79,405 mm. La referencia de servo 109,8° para contacto sobre 50 mm procede de la parametrización geométrica heredada; no demuestra la conversión de consignas del bus STS.

```text
g(θ) = g0 + 2 r_p θ ;   dg/dt = 2 r_p dθ/dt
```

```text
F_mandíbula ≈ η τ_piñón / (2 r_p)
```

θ representa el giro respecto de una referencia elegida y se expresa en radianes. La relación de fuerza supone dos cremalleras simétricas con la misma carga. El coeficiente de fricción en las almohadillas, rigidez y par continuo disponible determinan la retención del objeto; no se atribuye una carga máxima a partir de la apertura o del par de bloqueo.
