# Comandos, parser y transporte

Estado vigente AFR2-20261023-15: backend/composición integrados en fuente; el texto anterior de migración se conserva debajo como antecedente. Sin compilar/probar.

[Maestro](../../../INFORME_MAESTRO.md) · [Informe del repositorio](../../INFORME_ADAPTACION.md).

## Utilidad

Reutilizar parser de bytes, framing, cola, tablas de handlers y separación de transporte. UNER_CoreConfig permite aportar slots/pool; no hace falta HAL para el núcleo. uner_app.c es composición del robot anterior, no biblioteca neutra.

## Adaptación

Reemplazar UART DMA por USART1 con ISR RX/UDRE y rings estáticos. ISR solo mueve bytes/contadores; PushByte/dispatch en main con presupuesto. Tratar overflow y resincronización. Reducir router/transportes/snapshots a lo necesario, conservar lifetime de payloads hasta ReleasePacket. Serializar LE con tipos de tamaño fijo y evitar shifts promocionados a int AVR de 16 bits. No copiar sizeof(struct). Elegir IDs/longitudes en Contracts antes de activar handlers. Retirar STM32 USB CDC, NRF, bootloader y comandos MPU/balance.

## Evidencia y alcance

- [reference/f4/Core/Src/uner_core.c](../../reference/f4/Core/Src/uner_core.c)
- [reference/f4/Core/Src/uner_handle.c](../../reference/f4/Core/Src/uner_handle.c)
- [reference/f4/Core/Src/uner_app.c](../../reference/f4/Core/Src/uner_app.c)
- [reference/f4/Core/Src/uner_transport_uart1_dma.c](../../reference/f4/Core/Src/uner_transport_uart1_dma.c)

Hallazgos: AFRDOC-001, AFRDOC-005, AFRDOC-008. [Registro](../../../AutoFinalRobo2-Contracts/documentation/review/FINDINGS.md).

Estado: lectura/copia o fuente parcial indicada; sin compilación, tests ni prueba física. La API detallada de origen está en el corpus AMDOC importado; no equivale a API AVR adaptada.


AFR2-20261023-15: TWI AVR por IRQ y composición OLED/encoder/USART1 implementados en fuente, sin DMA ni Arduino. Ver docs/COMPOSICION.md y Contracts/changes/2026-10-23-15-robo2-atmega-ui.md. Sin builds/tests/validadores/flasheos; hardware y recursos finales pendientes.
