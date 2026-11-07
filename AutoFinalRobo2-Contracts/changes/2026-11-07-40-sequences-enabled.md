# AFR2-20261107-40 - Secuencias habilitadas en el firmware ATmega

Pedido del usuario: habilitar la ejecución de secuencias (programas de la Web) en el auto.

- `include/afr2_sequence.h`: `AFR2_SEQUENCE_MEMORY_APPROVED` pasa a 1 por defecto (32 pasos de 20 B en unión + 16 B de tipos = 656 B). `-DAFR2_SEQUENCE_MEMORY_APPROVED=0` recupera el perfil sin almacenamiento.
- Sin cambios de protocolo: ESP (`robot.sequence.begin/append/commit/start/status/cancel`, EX01 >= 4/6) y Web (`SequenceEditorPanel`) ya existían. La capacidad se anuncia con `AFR2_EXT_F_SEQUENCES|AFR2_EXT_F_DISTANCE` solo cuando `Afr2Sequence_Available`: motores habilitados, pose válida, geometría, Hall disponible, CPR en las cuatro ruedas y sin fallo. Es decir, requiere la calibración Hall de AFR2-20261107-39; el ESP refresca las capacidades con cada `robot.state`.
- La protección de AFR2-20261107-39 (fallo 4, sentido PWM/Hall cruzado) también cubre el modo secuencia.

Memoria (avr-gcc 7.3.0): flash 131946 B (50,3 %); SRAM estática 6315 B (77,1 %, +656 B), 1877 B libres para pila. Pila estimada con `.su`: main 309 + Link_Push 141 + App_Command 123 + Sequence_Command 77 + ISR ~60, alrededor de 800 B en el peor camino identificado (sin análisis de llamadas indirectas). Queda margen, pero por debajo del objetivo de ~2 KiB de AVR_RAM_AUDIT_20261106; si aparece inestabilidad, primera medida: volver a 0 o recortar a 24 pasos.

Estado: atmega=fuente editada y compilada, sin cargar ni probar; esp/web=sin cambios.
