# AFR2-20261110-14 · Limpiar fallo desde Web y registrar la demora real

Versión: 1.22.0 (subversión).

La captura del usuario mostraba F1 y ausencia de limpieza Web. Se añade acción real de limpieza por CMD 0x56, con código esperado y salidas detenidas, compartiendo servicio con el encoder. Las acciones y el resultado se colocan antes de la explicación larga del panel.

El ATmega conserva el umbral de 100 ms, vuelve a atender control antes del render si corresponde y registra separación de control y etapas del ciclo al disparar F1. Los tiempos sobreviven a la limpieza y pueden verse en Web o log USB. La captura no determina la causa exacta: no se afirma que esta revisión elimine toda demora. La hipótesis de aplicación inmediata de PWM después de limpiar fue descartada por lectura del flujo real.

[Contrato, layout, semántica y límites](../contracts/FAULT_RECOVERY_TIMING_20261110.md).

atmega/esp/web: fuente modificada y coordinada; ningún binario ni paquete regenerado. No se ejecutaron tests, compilaciones, validaciones, generadores, navegador, puertos ni flasheos. Sin commit/push. Se conserva la limitación del usuario de trabajar solo en fuentes.
