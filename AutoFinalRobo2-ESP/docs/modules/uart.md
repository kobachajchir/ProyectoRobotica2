# UART, parser y transacciones

[Maestro](../../../INFORME_MAESTRO.md) · [Informe del repositorio](../../INFORME_ADAPTACION.md).

## Utilidad

Parser/cola/transporte Arduino y broker separan aceptación, respuesta y timeout. Tablas f4_* son del robot anterior; no basta cambiar su nombre.

## Adaptación

Nuevo catálogo/payloads Mega con capabilities. Mantener FIFO y una transacción correlacionable en vuelo; resolver ACK tardíos. Revisar try_send que exige availableForWrite>=frame completo; si buffer es menor que frame, segmentar TX sin intercalar tramas. No reutilizar streams MPU ni IR8. UART0 ESP limpia de logs; residuos ROM resincronizados. Reutilizar decoders solo tras fijar wire propio.

## Evidencia y alcance

- [reference/esp01/src/uner_core.c](../../reference/esp01/src/uner_core.c)
- [reference/esp01/src/uner_transport_serial.cpp](../../reference/esp01/src/uner_transport_serial.cpp)
- [reference/esp01/src/uart_transaction_manager.c](../../reference/esp01/src/uart_transaction_manager.c)
- [reference/esp01/src/f4_command_registry.c](../../reference/esp01/src/f4_command_registry.c)

Hallazgos: AFRDOC-006, AFRDOC-008. [Registro](../../../AutoFinalRobo2-Contracts/documentation/review/FINDINGS.md).

Estado: lectura/copia o fuente parcial indicada; sin compilación, tests ni prueba física. La API detallada de origen está en el corpus AMDOC importado; no equivale a API AVR adaptada.
