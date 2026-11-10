# 17. Sensores: modelos y calidad del dato

## 17.1 Ultrasonido

```text
ρ = c Δt / 2 ;   c ≈ 331,3 + 0,606 T [m/s]
```

T se expresa en °C en la aproximación del aire. A 20 °C, c ≈343,4 m/s. La mitad corresponde al viaje de ida y vuelta. Si el temporizado se cuantiza a 25 μs, el incremento nominal de distancia es aproximadamente 4,29 mm. Ese incremento no es exactitud: inclinación, textura, ancho de haz, ecos múltiples y temperatura afectan la medida.

Una muestra inválida o vencida no se interpreta como camino libre. El paquete de radar debe distinguir validez, instante de adquisición, edad y consigna del servo. La posición comandada por PWM no constituye una lectura de ángulo real; holgura y tiempo de establecimiento afectan la ubicación del eco.

```text
x_obj = ρ cos(α) ;   y_obj = ρ sin(α)
```

La transformación debe incluir la posición real del sensor y su cero angular sobre el chasis. La nube polar mostrada en pantalla es una representación de medidas temporales; no se considera un mapa simultáneo perfecto del entorno.

## 17.2 Sensores de línea

```text
e_línea = (Σ_i w_i x_i) / (Σ_i w_i)
```

x_i es la posición transversal del sensor y w_i su evidencia de línea. Para señal analógica se requiere calibrar blanco/negro, polaridad y rango. En la configuración actual consultada line_kind = 1 corresponde a lectura digital y line_invert = 31; no debe describirse esa configuración como una matriz de mediciones ADC continuas solo porque el hardware dispone de pines analógicos.

Cuando Σw_i = 0 debe existir un estado de línea no detectada. El valor anterior, una búsqueda o una detención son decisiones de control que deben documentarse; no se asigna e = 0 como si la línea estuviera centrada. Para seguimiento PID debe informarse período de muestreo, saturación y criterio de pérdida.
