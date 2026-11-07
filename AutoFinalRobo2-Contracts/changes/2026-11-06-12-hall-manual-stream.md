# AFR2-20261106-12 — Calibración Hall independiente y PWM continuo

Pedido: revisar/configurar Hall por motor, finalizar con Confirmar/Descartar,
persistir sin invalidar otras ruedas, fijar CW exterior positivo, documentar
restricciones de odometría y resolver espera USART y giros del táctil.

ATmega: menú Hall de tres opciones, confirmación contextual por motor,
estado conjunto y persistencia independiente EEPROMv10; máscara de convención
para migración por rueda y adaptación de signos a odometría. Mantiene captura
x1, sensores por IRQ y OLED/I2C Manager. Sin cambiar base de mando de motores.

ESP: un slot PWM reciente con caducidad; aceptación local diferenciada de
confirmación AVR; transmisión sin espera de respuesta PWM, manteniendo
transacciones de ALIVE/WEB_PULSE/STOP. Protege generación/sesión y no reproduce
consignas viejas tras otra acción de movimiento.

Web: raster Hall portado junto al AVR y simulador de flujo adaptado; PWM no
dispara emergencia por BUSY/TIMEOUT transitorios. Presentación OLED acotada
a200ms; giro continuo por defecto, con giro Hall por ángulo opcional existente.

Estado de los tres módulos: **solo fuente, sin compilar, ejecutar tests,
validadores, generadores, navegador, promover LittleFS, cargar ni probar
hardware**. No acredita corrección física de los síntomas. Historial anterior
conservado. Detalles, límites y pendientes en
[contrato](../contracts/HALL_MANUAL_STREAM_20261106.md); autores y archivos en
[procedencia](../provenance/hall-motion-20261106-12.json).
