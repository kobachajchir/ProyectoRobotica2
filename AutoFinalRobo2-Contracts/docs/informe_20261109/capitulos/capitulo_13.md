# 13. Control, trayectorias y estados

La relación cinemática genera consignas de rueda; el PWM es una acción eléctrica y no una medida de velocidad. Una eventual regulación por rueda debe comparar la velocidad estimada por Hall con la consigna. La existencia de encoders cableados o de una fórmula de PI en el informe no prueba que el lazo esté ajustado en hardware.

```text
e_i(k) = u_ref,i(k) − u_med,i(k)
```

```text
a_i(k) = sat[Kp e_i(k) + I_i(k) + a_ff,i(k)]
```

```text
I_i(k+1) = I_i(k) + Ki Δt e_i(k), con antiwindup cuando la salida satura
```

La compensación de zona muerta, inversión de sentido y frenado requiere caracterización por rueda. Si faltan pulsos o envejece la telemetría se debe informar la condición; no reemplazarla por una velocidad cero confiable. El informe final deberá identificar los estados efectivos de control en firmware y separar las propuestas de mejora.

## 13.1 Interpolación de articulaciones

```text
q(t) = q0 + (qf − q0) [10σ³ − 15σ⁴ + 6σ⁵] ;   σ = t/T
```

El polinomio quíntico permite velocidad y aceleración nulas en los extremos. La duración T debe elegirse respetando velocidad y aceleración máximas de todos los ejes, carga y estabilidad. La interpolación independiente de ángulos no produce necesariamente una línea recta del TCP; una trayectoria cartesiana requiere resolver la cinemática inversa a lo largo del recorrido.

## 13.2 Estados y autorización de movimiento

La operación debe distinguir reposo, movimiento, parada, error y mantenimiento. Las consignas deben tener unidades, vigencia y confirmación de estado. Los cambios pendientes de interfaz requieren confirmación Sí/No. En la OLED, el corto desde dashboard abre el menú completo; el largo, al soltar con duración ≥800 ms, abre controles rápidos desde dashboard y retorna al contexto anterior desde el resto, sin omitir esa confirmación.
