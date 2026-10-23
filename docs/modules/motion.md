# Motores, PID, Mecanum y GOTO

[Maestro](../../../INFORME_MAESTRO.md) · [Informe del repositorio](../../INFORME_ADAPTACION.md).

## Utilidad

Reutilizar arbitraje de fuentes, estados de misión/cancelación, límites, PID y estructura de perfiles. El origen es diferencial/balance; AutoMotion ready exige MPU/calibración/balance y objetos IR. El estimador no sustituye odometría de cuatro Hall.

## Adaptación

Crear driver cuatro ruedas FL/FR/RL/RR y mezclar vx/vy/wz con geometría/signos calibrados. Usar PID de velocidad por rueda, no balance. Captura Hall por diseñar: variante/PPR/niveles/cuentas no verificados. Sin IMU, orientación solo estimada por ruedas y susceptible al deslizamiento. GOTO requiere encoder/odometría válida; cancelar/STOP/obstáculo con timeout. No falsificar ready de MPU para saltar guardas. Evitar floats costosos en ISR; unidades y saturación explícitas. MPU/balance se conservan solo como referencias de acoplamientos, fuera del producto.

## Evidencia y alcance

- [reference/f4/Core/Src/motor_control.c](../../reference/f4/Core/Src/motor_control.c)
- [reference/f4/Core/Src/motion_control.c](../../reference/f4/Core/Src/motion_control.c)
- [reference/f4/Core/Src/pid_controller.c](../../reference/f4/Core/Src/pid_controller.c)
- [reference/f4/Core/Src/motion_estimator.c](../../reference/f4/Core/Src/motion_estimator.c)
- [reference/f4/Core/Src/auto_motion.c](../../reference/f4/Core/Src/auto_motion.c)

Hallazgos: AFRDOC-002, AFRDOC-003, AFRDOC-005. [Registro](../../../AutoFinalRobo2-Contracts/documentation/review/FINDINGS.md).

Estado: lectura/copia o fuente parcial indicada; sin compilación, tests ni prueba física. La API detallada de origen está en el corpus AMDOC importado; no equivale a API AVR adaptada.
