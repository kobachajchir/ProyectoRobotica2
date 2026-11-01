# AFR2-20261029-09 — Giro invertido y actualización continua del diagnóstico

Autoría: Koba Chajchir / AFR2; adaptación mediante Codex, 29/10/2026.

Pedido explícito: invertir suma/resta y corregir que a veces al pulsar/soltar
la imagen queda sin actualizar; alcance conservado solo firmware de prueba.

## Hallazgos y cambios

La composición08 suspendía Afr2Oled_Task durante el barrido completo112
direcciones, aunque continuaba capturando encoder y estado del botón. Esto
introducía una pausa visible que podía confundirse con cuelgue. put() además
esperaba UDRE por carácter: una línea serie retrasaba el muestreo SW en main.
Son problemas identificados por lectura; no se confirma que expliquen todos
los cuelgues reportados ni se descarta ruido/fallo de bus en esta revisión.

## Adaptaciones por archivo

- `AutoFinalRobo2-Atmega/service/i2c_uart0_diagnostic.c`: niega el resultado
  del decoder solo en el servicio, conservando saturación int32. A(+)/B(-)
  serie reflejan la nueva dirección. Sustituye espera UART por cola512bytes,
  511útiles, atendida por polling sin espera; contador tx_dropped saturado
  si se llena, banner acotado para entrar en cola. No agrega UART a ISR.
  Mantiene escaneo cada10s pero deja avanzar OLED en intervalos entre probes,
  por un único Manager y sus callbacks en main. No cuenta transferencia OLED
  legítima como timeout de admisión del escáner. Reenvía imagen actual cada1s
  sin volver a imprimir el estado en reposo. Botón conserva antirrebote20ms,
  boot-held y corto/largo exclusivos; no ejecuta acciones. Banner09.
- `AutoFinalRobo2-Atmega/tools/flash-i2c-encoder-diagnostic.ps1`: banner09 y
  descripción de inversión. Conserva compilación forzada antes de carga.
- `AutoFinalRobo2-Atmega/service/README.md`, guía
  `AutoFinalRobo2-Contracts/hardware/OLED_I2C_DIAGNOSTICO.md`, esta ficha y
  README/CHANGELOG Contracts registran comportamiento y estado.

Adaptación original AFR2, comentarios/autorías previos conservados. No se
modifica driver/decoder compartido, producto, Web, pantallas o navegación.

## Estado real

ATmega: solo fuente y revisión manual. Sin tests, compilaciones, validadores,
COM, flasheos ni pruebas físicas. No se generó HEX nuevo por el agente. El
usuario debe ejecutar el script y comprobar respuesta en ambos sentidos y
estado al pulsar/soltar, también durante el barrido. Cola añade512bytes más
índices/reloj de refresco; presupuesto total SRAM/pila/flash no medido.
ESP/Web/Loader/CAD sin cambios por esta ficha. Si persiste el síntoma se
necesita el registro serie09 y si cambia contador/estado mientras falla OLED;
no se marca resuelto físicamente por editar fuentes.
