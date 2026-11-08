# afr2_usart

AFR2-20261108-01. Librería USART por instancias, pensada para separarse del
proyecto y reutilizarse. Hoy tiene puerto AVR (ATmega2560/1280/640, USART0..3).
El ESP8266 implementará la misma API (ver [Portar a otro micro](#portar-a-otro-micro)).

Estado: fuente escrita, **sin compilar ni probar en placa**.

## Ideas principales

- **Un handle por puerto** (`Afr2Usart`), reservado por la aplicación (estático o
  global). Sin memoria dinámica.
- **Configuración por inicializador**: `AFR2_USART_CONFIG_INIT(puerto, baudios)`
  da 8N1 con los pines del puerto; después se ajustan los campos necesarios.
- **Buffers del llamador**: anillos de 2..256 bytes, potencia de dos. Un sentido
  sin buffer queda deshabilitado (por ejemplo, un log solo TX).
- **Eventos diferidos**: las interrupciones solo marcan eventos;
  `Afr2Usart_Poll()` los entrega al callback desde el `main`. Nunca se ejecuta
  lógica de aplicación dentro de una ISR (regla del proyecto).
- **No bloqueante**: `Write` es todo o nada y devuelve 0 si no hay lugar.
- **Propiedad explícita de interrupciones**: cada puerto usado se enlaza una
  sola vez con `AFR2_USART_BIND_ISR(n)`. Si un puerto no está enlazado, `Init`
  devuelve `AFR2_USART_ERR_NO_ISR` en vez de habilitar una interrupción sin
  vector (en AVR eso reinicia el micro). Así conviven drivers distintos: hoy el
  USART1 del enlace ESP sigue en `src/platform/afr2_uart1.c`.

## Puertos del ATmega2560

| Puerto | RX | TX | Uso en AFR2 |
|---|---|---|---|
| 0 | D0 (PE0) | D1 (PE1) | Log de depuración → USB del Mega (`afr2_debug`) |
| 1 | D19 (PD2) | D18 (PD3) | Enlace ESP (driver propio `afr2_uart1`) |
| 2 | D17 (PH0) | D16 (PH1) | libre |
| 3 | D15 (PJ0) | D14 (PJ1) | libre |

En AVR los pines son fijos por silicio. `rx_pin`/`tx_pin` aceptan
`AFR2_USART_PIN_DEFAULT` o el número de pin correcto; cualquier otro valor
devuelve `AFR2_USART_ERR_PIN`. En el ESP8266 esos mismos campos sí elegirán
pines (UART0 normal o intercambiado a GPIO13/15, UART1 solo TX en GPIO2).

## Uso mínimo

```c
#include "afr2_usart.h"

AFR2_USART_BIND_ISR(2)                 /* este archivo es dueño de USART2 */

static Afr2Usart port2;
static uint8_t rx_ring[64], tx_ring[64];

static void on_usart(Afr2UsartHandle u, uint8_t events, void *ctx) {
    (void)ctx;
    if (events & AFR2_USART_EVT_RX) {
        uint8_t b;
        while (Afr2Usart_Read(u, &b)) { /* procesar */ }
    }
    if (events & AFR2_USART_EVT_RX_ERROR) (void)Afr2Usart_TakeErrors(u);
}

void setup_port2(void) {
    Afr2UsartConfig c = AFR2_USART_CONFIG_INIT(2, 115200UL);
    c.rx_buffer = rx_ring; c.rx_size = sizeof(rx_ring);
    c.tx_buffer = tx_ring; c.tx_size = sizeof(tx_ring);
    c.event_mask = AFR2_USART_EVT_RX | AFR2_USART_EVT_RX_ERROR;
    c.on_event = on_usart;
    if (Afr2Usart_Init(&port2, &c) != AFR2_USART_OK) { /* informar */ }
}

/* en el loop principal */
Afr2Usart_Poll(&port2);
Afr2Usart_Write(&port2, "hola\r\n", 6);
```

## API

| Función | Qué hace |
|---|---|
| `Afr2Usart_Init(h, &cfg)` | Valida, reserva el puerto, configura pines/divisor/trama y habilita. Reinicializar el mismo handle libera antes el puerto anterior. |
| `Afr2Usart_Deinit(h)` | Deshabilita RX/TX y sus IRQ y libera el puerto. |
| `Afr2Usart_Write / PutByte` | Encolan sin bloquear; todo o nada. |
| `Afr2Usart_TxFree / TxIdle` | Espacio libre; `TxIdle` = anillo vacío y último bit fuera del pin. |
| `Afr2Usart_Read / ReadInto / Available / FlushRx` | Lectura no bloqueante. |
| `Afr2Usart_Poll` | Entrega `pendientes & event_mask` al callback (contexto main). |
| `Afr2Usart_TakeEvents` | Alternativa sin callback: devuelve y limpia los pendientes. |
| `Afr2Usart_TakeErrors` | Devuelve y limpia `ERRF_FRAME/OVERRUN/PARITY`. |
| `Afr2Usart_GetStats` | Contadores de desborde RX, errores RX y escrituras rechazadas. |
| `Afr2Usart_ActualBaud` | Baudios reales tras redondear el divisor. |
| `Afr2Usart_PortCount / PortInfo` | Puertos disponibles y sus pines. |

**Eventos:** `RX`, `RX_OVERFLOW`, `RX_ERROR`, `TX_EMPTY`, `TX_DONE`. Pedir
`TX_DONE` en `event_mask` habilita además la IRQ de fin de transmisión.

**Baudios:** se elige el divisor más cercano entre modo normal (/16) y doble
velocidad (/8); ante empate, /16. Si el error supera
`max_baud_error_permille` (25 = 2,5 % por defecto), `Init` devuelve
`AFR2_USART_ERR_BAUD`. A 16 MHz, 115200 da 117647 (+2,1 %), igual que el
enlace actual; 250000, 500000 y 1000000 son exactos.

**Errores recibidos:** un byte con error de trama o paridad se descarta salvo
`AFR2_USART_RX_KEEP_ERRORS`. El overrun indica bytes perdidos antes del actual,
que es válido y se conserva.

## Concurrencia (AVR)

- Productor y consumidor únicos por anillo; índices de 8 bits (lecturas atómicas).
- `Write` copia sin bloquear y publica la cabeza en una sección atómica junto
  con `UDRIE`, porque la ISR también modifica `UCSRnB`.
- Los contadores de 16 bits se leen bajo `ATOMIC_BLOCK`.
- Las ISR se generan en línea dentro de `AFR2_USART_BIND_ISR` con acceso directo
  a los registros del puerto, sin marco de llamada completo.

## Portar a otro micro

1. Crear `afr2_usart_<target>.h` con `struct Afr2Usart`, `AFR2_USART_PORT_COUNT`
   y, si corresponde, `AFR2_USART_BIND_ISR`.
2. Implementar las funciones de `afr2_usart.h` en `afr2_usart_<target>.c/.cpp`.
3. Agregar el `#elif` del target al final de `afr2_usart.h`.

**ESP8266 (pendiente):** puerto 0 = UART0 (GPIO1/3, o GPIO15/13 con
`rx_pin=13, tx_pin=15`), puerto 1 = UART1 solo TX (GPIO2). Se puede montar sobre
`uart.h` del core (`uart_init`, `uart_write`, `uart_read`, `uart_rx_available`).
Los eventos se calculan en `Poll` comparando contadores, sin ISR propia.
`BIND_ISR` queda vacío. En el ESP, UART0 transporta el enlace con el Mega; el
log del ESP debería salir por UART1/GPIO2.

**Otros AVR:** agregar la tabla de registros/pines y los nombres de vectores.
Por ejemplo, el ATmega328P usa `USART_RX_vect` y no `USART0_RX_vect`.

## Memoria

Por handle, unos 25 B de SRAM más los anillos que reserve la aplicación. La
tabla de puertos está en flash. Cada puerto enlazado agrega 3 ISR.
