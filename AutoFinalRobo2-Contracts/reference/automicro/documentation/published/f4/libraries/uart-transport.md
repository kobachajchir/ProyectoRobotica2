# Transporte UNER UART1 DMA

ID: `f4:module/Core/Inc/uner_transport_uart1_dma.h` · Capa: **plataforma** · Fotografía: 2026-10-20.

Estado: documentación por lectura de fuente; sin tests, compilación ni validación automática.

## Responsabilidad y alcance

Adapta el transporte UNER a recepción circular y transmisión DMA UART1 del STM32.

Índices DMA, aceptación TX, finalización y entrega al core; no interpreta comandos del robot.

<a id="modularity"></a>
## Modularidad observada

Adaptador STM32 con HAL explícito; no se exige independencia del periférico.

<a id="state"></a>
## Handle, propiedad y exposición de atributos

El contexto de transporte conserva el recurso UART, buffer RX, posiciones y buffer/estado TX. UNER_Transport presta funciones poll_rx/try_send y ctx.

<a id="lifecycle"></a>
## Ciclo de vida y ejecución

- Init conecta recurso y buffers; poll_rx entrega bytes nuevos al parser.
- try_send inicia/encola según disponibilidad; OnTxComplete libera el estado de transmisión.

## Trabajo en tándem

- Bridge y callbacks HAL alimentan el transporte. UNER_Handle usa el mismo contrato para UART y USB.

## Precauciones y límites

- DMA no autoriza a destruir el buffer antes de finalizar.
- IDLE, mitad y fin DMA no equivalen a límites de frame UNER.

## Hallazgos vinculados


## Archivos y dependencias de compilación

| Archivo | Includes directos | SHA-256 |
| --- | --- | --- |
| `Core/Inc/uner_transport_uart1_dma.h` | `stdint.h`, `stm32f4xx_hal.h`, `uner_transport.h` | `00813cdceca58e3c2b7c06533bc9958673a63104f158399686f5bbc96edb4585` |
| `Core/Src/uner_transport_uart1_dma.c` | `app_wifi_binding.h`, `wifi_bridge_at.h`, `uner_transport_uart1_dma.h` | `f00189bd697354810494aa2f29e5ee9d373d828b3718e177b168e868ab049621` |

## Tipos, atributos y punteros a función

Las declaraciones se extraen sin preprocesar C. Un campo documentado no es permiso para modificarlo desde fuera. Los campos sin explicación editorial se señalan como pendientes; no se inventa encapsulación. Los argumentos y retornos exactos se conservan en la firma.

### `UNER_TransportUart1Dma`

`f4:Core/Inc/uner_transport_uart1_dma.h:15`

| Atributo / firma | Función y precaución |
| --- | --- |
| `UNER_Transport base` | Dato de estado/configuración `base`; declaración estructural, interpretación específica pendiente de ampliar. |
| `UART_HandleTypeDef *huart` | Dato de estado/configuración `huart`; declaración estructural, interpretación específica pendiente de ampliar. |
| `DMA_HandleTypeDef *hdma_rx` | Dato de estado/configuración `hdma_rx`; declaración estructural, interpretación específica pendiente de ampliar. |
| `volatile uint8_t *rx_buf` | Dato de estado/configuración `rx_buf`; declaración estructural, interpretación específica pendiente de ampliar. |
| `volatile uint16_t rx_len` | Dato de estado/configuración `rx_len`; declaración estructural, interpretación específica pendiente de ampliar. |
| `volatile uint16_t last_pos` | Memoria de la observación anterior usada por la máquina de estados. |
| `volatile uint8_t *tx_busy_flag` | Dato de estado/configuración `tx_busy_flag`; declaración estructural, interpretación específica pendiente de ampliar. |
| `volatile uint16_t curr_pos` | Dato de estado/configuración `curr_pos`; declaración estructural, interpretación específica pendiente de ampliar. |

## Funciones públicas y símbolos externos

Cada función ocupa una línea. `editorial` identifica una explicación escrita para la API; `structural` describe una delegación observada; `inferred-name` requiere ampliar su semántica. Las llamadas listadas son tokens estructurales y pueden ser macros o callbacks, no un grafo resuelto. Una declaración sin implementación localizada queda visible.

| Función y firma | Descripción breve | Evidencia / descripción |
| --- | --- | --- |
| <a id="fn-uner_transportuart1dma_init"></a>`UNER_Status UNER_TransportUart1Dma_Init( UNER_TransportUart1Dma *transport, uint8_t transport_id, UART_HandleTypeDef *huart, DMA_HandleTypeDef *hdma_rx, volatile uint8_t *rx_buf, volatile uint16_t rx_len, volatile uint8_t *tx_busy_flag )` | Conecta UART1 y memoria DMA al contrato UNER_Transport. | `f4:Core/Inc/uner_transport_uart1_dma.h:26`; editorial |
| <a id="fn-uner_transportuart1dma_ontxcomplete"></a>`void UNER_TransportUart1Dma_OnTxComplete(UNER_TransportUart1Dma *transport)` | Atiende finalización TX y libera el transporte para el siguiente frame. | `f4:Core/Inc/uner_transport_uart1_dma.h:36`; editorial |

## Funciones internas

| Función | Descripción breve | Evidencia |
| --- | --- | --- |
| <a id="fn-uner_transportuart1dma_queuefull"></a>`static uint8_t UNER_TransportUart1Dma_QueueFull(const UNER_Core *core)` | Operación Queue Full de Transporte UNER UART1 DMA; semántica fina pendiente de ampliación. | `f4:Core/Src/uner_transport_uart1_dma.c:8`; inferred-name |
| <a id="fn-uner_transportuart1dma_feedrange"></a>`static UNER_Status UNER_TransportUart1Dma_FeedRange( UNER_Transport *transport, UNER_Core *core, uint16_t start_pos, uint16_t end_pos)` | Coordina `UNER_Core_PushByte`, `UNER_TransportUart1Dma_QueueFull` para Transporte UNER UART1 DMA. | `f4:Core/Src/uner_transport_uart1_dma.c:13`; structural |
| <a id="fn-uner_transportuart1dma_pollrx"></a>`static UNER_Status UNER_TransportUart1Dma_PollRx(UNER_Transport *transport, UNER_Core *core)` | Coordina `UNER_TransportUart1Dma_FeedRange`, `UNER_TransportUart1Dma_QueueFull`, `WifiBridge_GetActiveBackend` para Transporte UNER UART1 DMA. | `f4:Core/Src/uner_transport_uart1_dma.c:38`; structural |
| <a id="fn-uner_transportuart1dma_trysend"></a>`static UNER_Status UNER_TransportUart1Dma_TrySend(UNER_Transport *transport, const uint8_t *buf, uint16_t len)` | Coordina `HAL_UART_Transmit_DMA`, `WifiBridge_AT_QueueFrame`, `WifiBridge_GetActiveBackend` para Transporte UNER UART1 DMA. | `f4:Core/Src/uner_transport_uart1_dma.c:84`; structural |
