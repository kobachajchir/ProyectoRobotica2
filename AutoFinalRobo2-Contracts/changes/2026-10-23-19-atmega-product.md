# AFR2-20261023-19 — Implementación de producto ATmega

23/10/2026. Pedido directo: ejecutar el plan completo ATmega y registrar progreso
y complementos ESP/Web durante el trabajo concurrente.

Continuación24/10/2026. Estado: fuente ATmega y documentación de M01..M18 integradas,
sin compilar/ejecutar. [Matriz y bitácora](../docs/PLAN_ATMEGA.md) ·
[Detalle por subsistema](../docs/ATMEGA_IMPLEMENTACION_19.md).

- Menús44 nodos, Sistema, editores, teclado SSID/password, elección Web/OLED y
  revisión/confirmación de conexión en el auto; cola4 de avisos con barra3000ms.
- Servicios PWM cuatro ruedas, STOP, dueño/lease/TTL, watchdog, línea5 digital/ADC,
  HC-SR04/servo opcional, Hall x1/calibración por vuelta, PID, Mecanum, odometría,
  línea y GOTO relativo. Capacidades condicionadas; Hall sin conectar/macro0.
- EEPROM con CRC/dos registros/commit, sin contraseñas, guardar/descartar separados.
- [EX01](../contracts/ROBO2_PRODUCT_EXTENSION.md) opt-in sobre perfil2 con comandos,
  estados, captura OLED y outbox Wi-Fi. [CONFIG](../contracts/ATMEGA_CONFIG_ACTIONS.json)
  congela IDs explícitos. No modifica framing/parser común ni perfil legacy.
- [Manifest por archivo](../provenance/atmega-product.json), respaldo de las fuentes
  reemplazadas, autorías de referencia preservadas. Sin alterar reference/AutoMicro.

ESP/Web: no se editaron fuentes ni se generaron paquetes. Se entrega guía para
implementar negociación, codecs/validaciones, dueño/pose/framebuffer y flujo Wi-Fi
del auto con único WebSocket. Las compilaciones20/21 no acreditan esa integración.
Perfil3/secuencias de21 sigue diseñado, separado: sus IDs10..15 no son EX01.

Verificación realizada: lectura de fuentes/documentación y control SHA exclusivamente
para conservar cambios concurrentes antes de escribir. Sin tests, compilaciones,
validadores, puertos, flasheos ni pruebas físicas. No hay ELF/HEX/mapa, medición de
SRAM/pila/IRQ ni calibración en vehículo. STOP confirma salidas apagadas, no velocidad
física cero; GOTO no acredita precisión. M19/aceptación física y complemento siguen pendientes.

Guía solicitada guardada en firmware ESP:
[ROBO2_EX01_ATMEGA.md](../../AutoFinalRobo2-ESP/docs/ROBO2_EX01_ATMEGA.md).
Solo documentación; ninguna fuente ESP/Web modificada por19.
