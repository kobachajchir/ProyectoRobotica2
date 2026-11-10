# 7. Cinemática directa: posición y orientación del TCP

El centro de muñeca W es el punto hasta el cual llegan L1 y L2. Sea β = q2 + q3 y e_z = (0,0,1). La orientación previa a la muñeca es R_pre = Rz(q1) Ry(β). La orientación final incorpora los tres ejes distales.

```text
ρ = L1 sin(q2) + L2 sin(q2 + q3)
```

```text
z_W = h + L1 cos(q2) + L2 cos(q2 + q3)
```

Centro de muñeca expresado en la montura:

```text
[ ρ cos(q1) ]
[ ρ sin(q1) ]
[ z_W ]
```

```text
R = Rz(q1) Ry(q2 + q3) Rz(q4) Ry(q5) Rz(q6)
```

```text
p_TCP = p_W + d R e_z
```

Las dos expresiones determinan la pose completa. Para obtener la pose respecto de la plataforma se antepone la transformación fija de montaje. Para expresarla en un mapa o referencia mundial se antepone además la pose planar estimada del autito.

```text
T_mundo,TCP = T_mundo,base T_base,montura T_montura,TCP(q)
```

## 7.1 Pose de transporte y límites

| Eje | Rango geométrico documentado |
| --- | --- |
| J1 | −90° a +90° |
| J2 | −10° a +125° |
| J3, J4, J5, J6 | −90° a +90° |
| Pinza auxiliar | Accionamiento separado; referencia geométrica heredada de 0° a 180°. |

La pose de transporte del descriptor es [0°, 15°, −40°, 0°, 25°, 0°]. Los límites son datos del modelo y no garantizan ausencia de colisiones, límites de cableado o funcionamiento bajo carga. El documento nativo conserva relaciones de movimiento, pero eso no demuestra un barrido continuo libre de interferencias.

Una comprobación posterior deberá comparar poses conocidas entre la ecuación, el descriptor Web y mediciones del mecanismo. No se presentan errores RMS ni precisiones del TCP porque no se realizaron esas mediciones.
