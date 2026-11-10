# 14. Modelo integrado: plataforma y manipulador móvil

La configuración general incluye la pose planar (x,y,ψ), los seis ángulos del brazo y el accionamiento auxiliar de pinza. Una rueda pasiva o la articulación de pinza no se suma como un nuevo grado de libertad de pose del TCP. El desplazamiento del autito modifica a la vez la posición y la orientación mundial de la herramienta.

```text
Q = (x, y, ψ, q1, q2, q3, q4, q5, q6)
```

```text
T_W,TCP(Q) = T_W,B(x,y,ψ) T_B,M T_M,TCP(q)
```

Transformación planar de la base:

```text
[ cos ψ | −sin ψ | 0 | x ]
[ sin ψ | cos ψ | 0 | y ]
[ 0 | 0 | 1 | 0 ]
[ 0 | 0 | 0 | 1 ]
```

Para una herramienta situada en p = (p_x,p_y,p_z) respecto de la base y velocidades de cuerpo, la contribución de la base a la velocidad lineal es (v_x − ω p_y, v_y + ω p_x, 0). La contribución angular es (0,0,ω). Ambas se rotan al marco de expresión elegido.

```text
V_TCP = J_base(Q) V_base + J_brazo(Q) dq/dt
```

Esta redundancia permite repartir una tarea entre base y brazo, pero una solución que minimiza velocidad articular puede acercar el centro de masa al borde de apoyo. Un planificador futuro debería combinar alcance, estabilidad, obstáculos, singularidades y postura de transporte. No se declara implementado ese planificador ni una coordinación física simultánea de los nueve grados de libertad.

## 14.1 Uso en una secuencia de recogida

Una secuencia justificable es aproximación de la base, detención y confirmación de estado, posicionamiento del brazo, cierre de pinza con criterio de contacto, retorno a transporte y continuación. La proximidad por ultrasonido no garantiza por sí sola que el objeto esté en la apertura de la pinza. Deben definirse el tamaño de objeto, superficie de contacto y criterio de agarre antes de evaluar éxito de recogida.
