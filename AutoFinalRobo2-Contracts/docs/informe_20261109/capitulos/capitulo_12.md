# 12. Encoders, odometría y estimación de pose

La configuración actual parte de 370 cuentas por vuelta por rueda, informadas por el usuario, con decodificación x1: flanco ascendente de A y lectura de B para el sentido. No se multiplican nuevamente esas cuentas por cuatro ni se reemplazan por hipótesis antiguas de 1320 o 3300. El diámetro nominal de rueda es 80 mm. [S4]

```text
Δφ_i = 2π Δn_i / N_i ;   Δs_i = r_i Δφ_i ;   u_i ≈ Δs_i / Δt
```

```text
Resolución lineal nominal = π D / N ≈ 0,679 mm/cuenta, para D = 80 mm y N = 370
```

Ese valor es una cuantización nominal. Deslizamiento Mecanum, tolerancia del diámetro efectivo, carga y piso pueden dominar el error. El orden físico de conectores y máscaras de encoder debe convertirse al orden lógico FL,FR,RL,RR; no se aplica directamente el índice de un conector a una fila del Jacobiano.

Paso de velocidad del cuerpo al marco mundial:

```text
[ dx/dt ]
[ dy/dt ]
[ dψ/dt ]
```

```text
dx/dt = cos(ψ) v_x − sin(ψ) v_y
```

```text
dy/dt = sin(ψ) v_x + cos(ψ) v_y ;   dψ/dt = ω
```

## 12.1 Integración temporal propuesta

```text
ψ_m = ψ_k + ω Δt/2
```

```text
x_(k+1) = x_k + [cos(ψ_m) v_x − sin(ψ_m) v_y] Δt
```

```text
y_(k+1) = y_k + [sin(ψ_m) v_x + cos(ψ_m) v_y] Δt ;   ψ_(k+1) = ψ_k + ω Δt
```

Se propone la orientación de punto medio para reducir el error de integración respecto de usar únicamente el ángulo inicial. Deben preservarse el tiempo real de muestreo y la detección de muestras inválidas. Una odometría sin referencia externa acumula deriva; no equivale a localización absoluta ni demuestra precisión de navegación.

## 12.2 Calibración posterior

Conviene medir una trayectoria recta, una lateral y un giro conocido para estimar diámetro efectivo por rueda y L efectivo. El conjunto de datos debe guardar cuentas crudas, tiempos, consignas y referencia externa. Las covarianzas y errores se calcularán con esas mediciones, no con la apariencia del recorrido en el visor.
