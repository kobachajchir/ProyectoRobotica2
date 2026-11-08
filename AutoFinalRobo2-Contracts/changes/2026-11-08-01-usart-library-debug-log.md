# AFR2-20261108-01 - Librería afr2_usart y log de depuración del ATmega por USART0

Pedido del usuario: una librería USART reutilizable (instancias, handles,
eventos e inicializadores, parametrizable para todos los USART) y aplicarla en
el ATmega para ver paso a paso qué pasa, en particular cuando se reinicia el ESP.

## Qué se agregó

- `AutoFinalRobo2-Atmega/lib/afr2_usart/`: API portable (`afr2_usart.h`) y
  puerto AVR (`afr2_usart_avr.h/.c`) para USART0..3 del ATmega2560. Handles
  estáticos, anillos del llamador, eventos marcados en ISR y entregados en
  `Afr2Usart_Poll`, `AFR2_USART_BIND_ISR(n)` explícito por puerto y
  `AFR2_USART_ERR_NO_ISR` si falta. README con API y guía para el puerto ESP8266.
- `src/services/afr2_debug.c` + `include/afr2_debug.h`: log por **USART0
  (TX D1) a 115200 8N1**, que sale por el USB del Mega. Nivel con
  `make AFR2_DEBUG_LOG=0|1|2` (por defecto 1). No bloquea: las líneas sin lugar
  se descartan y se cuentan.
- `afr2_link`: gancho opcional `ops.event` y contadores `requests/rejects/faults`.
  Solo diagnóstico; la lógica del enlace no cambia. El firmware de comunicación
  pasa ops nulas y no lo usa.
- El enlace ESP sigue en USART1 con `afr2_uart1.c`, sin cambios de driver ni de
  protocolo. Migrarlo a la librería queda para después de diagnosticar, para no
  cambiar el instrumento y lo medido a la vez.

## Qué registra

| Etiqueta | Evento |
|---|---|
| `AVR BOOT` | arranque del ATmega con bits MCUSR (power-on, reset-pin, brown-out, watchdog) |
| `ESP BOOT` | trama BT01 del ESP: bootId, **causa de reinicio** (0 power-on, 1 hw-wdt, 2 exception, 3 soft-wdt, 4 soft-restart/abort, 6 ext-reset), versión |
| `ESP>` / `ESP+` | texto que el ESP imprime en su UART0, que es el enlace: volcado de excepción del core (`Exception (n)`, `epc1=…`, `last failed alloc…`). Las filas de pila se cuentan y no se imprimen |
| `LINK` | sesión nueva, pérdida por supervisión (1500 ms sin pedidos), fallas UART (agrupadas), pedidos rechazados (`cmd` y resultado, repeticiones agrupadas) |
| `NET` | eventos que reenvía el ESP (cliente AP entra/sale, ws-open/close, index servido, sta-lost…) y cambios de modo (`STA/AP/AP_STA`, clientes, web) |
| `CAP` | captura OLED remota rechazada (y por qué), cancelada por el encoder local, vencida a 600 ms, página rechazada, captura lenta (≥300 ms) |
| `ENC` | giro agrupado cada 250 ms, click, largo |
| `LOOP` | vuelta del loop principal ≥100 ms (watchdog 500 ms) |
| `STAT` | cada 5 s: sesión, pedidos, rechazos, fallas, peor loop, pila nunca usada (medida por pintado), líneas perdidas, ESP actual |

Nunca registra contraseñas, la clave del AP ni contenido de tramas.

## Cómo leerlo

1. USB del Mega al PC y monitor serie a **115200** (Loader → Monitor serie, o
   cualquier terminal).
2. Abrir el puerto reinicia el Mega por DTR (auto-reset del Arduino). La
   primera línea será `AVR BOOT … reset-pin`, lo cual es esperado.
3. **Comprobar el selector UART del shield:** el ESP no debe quedar en D0/D1
   (el enlace operativo usa E_RX/E_TX → D18/D19). Si el ESP estuviera en D0/D1,
   TX0 del log chocaría con su TX.
4. Secuencia típica si el ESP se cae por una excepción (el orden puede variar):

```
40210 ESP> --------------- CUT HERE FOR EXCEPTION DECODER --------
40210 ESP+ -------
40211 ESP> Exception (28):
40211 ESP> epc1=0x4020a1b3 epc2=0x00000000 epc3=0x00000000 exc
40212 ESP+ vaddr=0x00000000 depc=0x00000000
40230 ESP> (37 stack rows omitted)
40230 ESP> <<<stack<<<
41720 LINK lost: no ESP request for 1503 ms, motion halted
41890 LINK rx-fault x1 (framing/overrun/ring full)
42050 ESP BOOT id=7a1c33f0 rst=2 exception fw=0.11.0 ext=13
42080 LINK hello, new session 7a1c33f2
43010 NET state mode=AP_STA sta=0 ap=1 clients=0 web=1
```

`epc1` se traduce a función y línea con el **mismo ELF que está cargado**:
`xtensa-lx106-elf-addr2line -pfiaC -e AutoFinalRobo2-ESP/.pio/build/esp12s_candidate/firmware.elf 0x4020a1b3`.
Si el corte es por memoria, aparece `last failed alloc call: <pc>(<bytes>)`.
Sin volcado y con `rst=0` o `rst=6`, la causa es alimentación o reset externo.
La falla RX al arrancar suele ser la ROM del ESP, que imprime a 74880 baudios.

## Memoria (estimada, sin compilar)

Unos 360 B de SRAM: anillo TX 128 + línea 80 + texto ESP 48 + estado ~60 +
handle ~31 + tabla de puertos 8 + Afr2Link +8. Sobre los 1877 B libres para pila
de AFR2-20261107-40 quedan unos 1500 B. La pila real queda medida por `STAT
stack>=`. Si falta margen: `AFR2_DEBUG_TX_SIZE=64` o `AFR2_DEBUG_LOG=0`, que
elimina todo el código y la RAM del log.

## Estado

atmega=fuente editada, **sin compilar, cargar ni probar**; esp=sin cambios;
web=sin cambios. Pendiente: compilación, revisión de SRAM y pila, y lectura del
log con el auto. El puerto ESP8266 de la librería todavía no está escrito.
