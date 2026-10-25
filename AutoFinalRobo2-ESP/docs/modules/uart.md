> **AFR2-20261024-08:** IP/AP y avisos con iconos F4, scroll seleccionado y reinicio local por Sistema → Reinicio, compilados. [EX01v5](../../../AutoFinalRobo2-Contracts/contracts/EX01_NETWORK_UI_5.md) · [Estado real](../../../AutoFinalRobo2-Contracts/docs/ESTADO_IMPLEMENTACION.md). En ATmega, FillRect/TextWindow, iconos y snapshot de red quedan separados del transporte/I2C. Sin prueba de placa ni flasheo; estados inferiores históricos.

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


Fuente vigente AFR2-20261023-11: src/mega_link.* usa el header único Contracts/contracts/afr2_wire.h y UART_PROFILE_1.md. Sesión/transacción sustituyen ACK genérico; USART1 AVR y composición de comunicaciones están escritas. Sin compilar ni probar. Leer README e informe actuales antes de continuar.


AFR2-20261023-15: perfil 1 sustituido por ROBO2/perfil 2; documento de implementación y recepción en [PROTOCOLO_ROBO2.md](../PROTOCOLO_ROBO2.md). Header compartido con AVR y validación/correlación en mega_link.cpp, sin compilar/probar.
