# AFR2-20261024-11 — Encoder por IRQ y preferencia de captura Hall

Pedido humano: implementar captura del encoder por interrupciones y consumo en main,
revisar los pines Hall y darles preferencia sobre el encoder de interfaz.

## Resultado

CLK D2/PE4/INT4 y DT D3/PE5/INT5 reemplazan D40/D41. SW conserva D42/PL7.
Ambas ISR capturan AB en una lectura PINE y publican en FIFO32bytes/31útiles;
main decodifica y ejecuta acciones con presupuesto32entradas/vuelta. Overflow
provoca descarte/resincronización; no se inventan pasos. Se conserva API Poll
como compatibilidad y se separan Transition/ResetRotation/Button en producto.
SW conserva20ms/800ms y espera que se consuman giros anteriores para confirmar.

Hall conserva FL A8/A9, FR A10/A11, RL A12/A13, RR A14/A15. Las ocho entradas
son PK0..7/PCINT16..23. No hay superposición en el mapa fuente/documental actual;
no acredita accesibilidad del shield ni continuidad/niveles. PCINT2 usa máscara
0x55 y x1, subida A + dirección B; no cambia calibración a x4. Captura/counters
pasan de sensores a board para compartir el servicio prioritario con UI.

AVR tiene prioridad fija: INT4/5 preceden a PCINT2. Se implementa preferencia de
servicio: toda entrada del encoder atiende Hall ya pendiente ANTES de publicar
UI. No se altera la prioridad hardware ni hay ISR anidadas; Hall que llega después
de esa comprobación espera el resto de la ISR. Captura Hall desenrollada y
snapshots atómicos; PID/odometría/UI permanecen en main. No se garantiza frecuencia
máxima ni ausencia de pérdidas sin medir. [Contrato y fuentes primarias](../hardware/INTERRUPCIONES_ENCODER_HALL.md).

## Estados

| Módulo | Estado real11 |
| --- | --- |
| ATmega | Fuente adaptada, sin compilar ni ejecutar; build08 anterior |
| ESP | Sin cambios; firmware08 y activosWeb10 conservados |
| Web | Sin cambios; entrega10 conservada |
| Contracts | Pinout/PCB documental, recursos, API/estado y procedencia actualizados |
| CAD | Sin cambios, sin esquema/PCB nuevo |

AFR2_HALL_CONNECTED=0 y AFR2_SEQUENCE_MEMORY_APPROVED=0 se conservan. Este cambio
requiere recablear CLK/DT; no habilita Hall, potencia ni secuencias. No cambia wire
ROBO2/EX01v5, IDs CONFIG, EEPROM, renderers, ScreenCodes ni navegación. Trabajo
funcional de entrada, sin nuevas pantallas ni contraparte visual que sincronizar.

## Revisión y límites

Lectura de fuente y comparación de cambios únicamente. No se ejecutaron tests,
compilaciones, validadores, flasheos ni ensayos físicos. También queda sin compilar
la rama AFR2_HALL_CONNECTED=1. SRAM/pila/ciclos actuales desconocidos; los valores
de build08 se conservan exclusivamente como evidencia histórica. ISR no preemptivas,
cola finita y cadencia SW objetivo1ms requieren medición bajo carga autorizada.

Autorías previas conservadas. Nuevas adaptaciones AVR propias con Codex sobre la
fuente AFR2 existente; reference/ y D:/AutoProyecto sin cambios. Registro por archivo:
[procedencia11](../provenance/encoder-hall-interrupts-20261024.json).
