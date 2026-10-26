> **Actualización de compilación14:** pedido humano posterior autoriza compilar/corregir. ATmega/ESP/Web full: exit0, sin errores ni advertencias finales. [Informe](../../../AutoFinalRobo2-Contracts/documentation/builds/2026-10-25-14/README.md). Sin tests, validadores independientes, navegador, montaje físico ni flasheos. Paquete local full generado; no promovido a ESP/data/dist. El texto de entrega sin compilar inferior es histórico.

# HC-SR04, servo y radar

AFR2-20261025-14. Implementación en fuente, sin nuevo build ni prueba física. [Contrato de cabeza EX01v8](../../../AutoFinalRobo2-Contracts/contracts/SENSORS_RADAR_8.md).

Driver en src/platform/afr2_sensors.c: D30 TRIG, D31 ECHO muestreado por Timer3 cada25µs, subida/bajada y timeout32ms. Disparos70..1000ms, default70. Distancia nominal en mm con grano4,31mm; no se afirma exactitud física. Medidas20..4000mm; errores y edad explícitos.

Servo D13/Timer5:50Hz,1000..2000µs; pending y applied separados y cambio al inicio del pulso. Snapshot publica PWM aplicado/ángulo nominal; sin feedback físico. ADC de línea5canales10bits/digital sin acoplar rawValid a calibración.

Radar local no bloqueante, por pasos5°, configuración RAM y37bins/cola16. Permanece activo al salir/desconectar ESP/Web; STOP/fallo/reinicioATmega lo apaga. Tracción bloqueada durante radar. Datos laterales no habilitan protección frontal. Centrar y esperar medición fresca antes de conducir; apagar PWM no centra ni garantiza orientación.

APIs públicas: Afr2Head_State/Servo/Interval/Configure/Start/Stop/Read y Afr2Sensors_Init/Task. Comandos de aplicación en src/services/afr2_sensor_commands.c. Render puro en src/ui/afr2_sensor_screens.c y contraparte Web firmwareSensorRenderer.ts. Pruebas/captura/recursos y límites en ficha14.
