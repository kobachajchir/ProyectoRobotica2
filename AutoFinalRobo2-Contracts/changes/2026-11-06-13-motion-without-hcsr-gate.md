# AFR2-20261106-13 — Movimiento sin requisito HC-SR04

Por pedido explícito, ningún modo físico vigente depende de una lectura
HC-SR04 válida ni de una distancia frontal mínima de150mm para mover motores.
Sustituye el requisito descrito anteriormente en AFR2-20261106-12.

ATmega, solo fuente:
- Retirado el bloqueo al iniciar Drive de velocidad, seguimiento de línea
  y GOTO, tanto traslación como giro.
- Retirada la parada periódica por eco inválido o distancia frontal<150mm;
  esto también alcanza secuencias que pasaban por ese guard común.
- Retirados los motivos OLED «Centrar / ver HC» y «Obstaculo frontal»
  asociados a esas validaciones.
- PWM manual/táctil y prueba individual ya no dependían de HC-SR04.

Web, solo fuente: simulador del flujo OLED alineado con firmware, sin
«Lectura HC-SR04» como requisito ficticio de movimiento. Se conserva el
comportamiento propio de simulación de movimiento/aproximación.

ESP sin cambios. Adquisición y visualización HC-SR04, radar, telemetría y
comandos de sensores se conservan. No se implementa todavía un modo físico
de aproximación que ubique un objeto dentro del alcance del brazo.

Se conservan los demás requisitos vigentes de cada modo: configuración de
motores, Hall/CPR/pose cuando corresponde, línea IR, geometría, límites,
supervisión, TTL y STOP; el bypass manual previo continúa según su contrato.
No cambia wire, ScreenCode ni EEPROMv10.

Sin tests, compilaciones, validadores, generadores, navegador, promoción
LittleFS, flasheos ni pruebas físicas. La lectura de fuente no acredita
funcionamiento en el auto. Adaptaciones por archivo en
[procedencia](../provenance/motion-without-hcsr-20261106-13.json).
