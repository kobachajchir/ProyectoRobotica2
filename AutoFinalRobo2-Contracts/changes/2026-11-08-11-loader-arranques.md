# AFR2-20261108-11 - Loader: arranques del ESP y del ATmega con detalle

Pedido del usuario (08/11/2026): una sección del Loader que muestre los arranques del ESP y del
ATmega, por si se reinician, con opción de ver detalles. Incluye la explicación de por qué el ESP
manda cada comando que se ve en "RX ATmega" (abajo).

Loader 2.3.0. Versión de producto sin cambio (no toca firmware).

## Qué cambió (Loader)

- La pestaña "Resets ESP" del intérprete pasa a **Arranques ESP / ATmega**: grilla (Nº, hora PC,
  ms AVR, equipo, razón, excepción, epc1, boot ID, fuente, detalle) y panel de detalle con
  **Copiar detalle** y **Limpiar arranques**. Las filas graves (excepción, watchdog, brown-out,
  memoria agotada, razones 1..4 del ESP) van en rojo.
- El detalle muestra razón, excepción, epc1, excvaddr, `last failed alloc`, fuente y **las 30 líneas
  del log anteriores y hasta 20 de los 3 s posteriores** a cada arranque.
- ATmega: línea `AVR BOOT` con los bits de MCUSR. Un `reset-pin` dentro de los 4 s de abrir el puerto se
  marca como auto-reset del USB y no genera aviso. Watchdog y brown-out se explican en el detalle.
- ESP: `ESP BOOT` (razón), volcado `ESP>`/`ESP+` (`Exception (n)`, `epc1`, alloc, wdt), `ESP ROM
  probable` y la trama BOOT `0x52` del modo TAP, unidos en una sola fila por boot ID.
- Se registra **siempre** que el puerto esté abierto, con el intérprete apagado también. Cada arranque
  nuevo deja un aviso en Eventos y errores; **Ver detalles** muestra el mismo informe.
- `WireDecoder.cs`: `BootTracker`/`BootEvent` reemplazan `EspResetTracker`.

## Por qué el ESP manda cada comando (leído en `AutoFinalRobo2-ESP/src`)

El ESP es el maestro del enlace: el ATmega solo responde, así que todo lo que el ATmega quiera avisar
lo tiene que preguntar el ESP. El "8 B" de cada pedido es sesión + transacción, sin cuerpo.

| CMD | Cada | Para qué | Dónde |
|---|---|---|---|
| `0x11 EXT_STATE` | 500 ms | Estado del producto (modo, origen, parada, falla, pose, GOTO, preferencias) para `robot.state` de la Web y para detectar cambios. Es el pedido de base cuando no hay otro pendiente. | `broker_product.cpp`, `tickProduct` |
| `0x32 DIAG_POLL` | 400 ms | Preguntar si en el OLED se pidió **Prueba ESP**; el ATmega no puede iniciar la prueba por su cuenta. | `tickProduct` |
| `0x30 WIFI_POLL` | 400 ms | Preguntar si en el OLED se inició un trabajo Wi-Fi (buscar redes, conectar, credenciales escritas en el OLED, Iniciar AP / Seguir esperando). Redactado porque la respuesta puede traer SSID y clave. | `broker_wifi.cpp`, `tickWifi` |
| `0x31 WIFI_UPDATE` | 2 s, de a dos | Estado Wi-Fi (modo, STA conectada, RSSI, SSID, IP STA/AP) y detalles (seguridad, clientes, máscara, puerta de enlace) para las pantallas Wi-Fi del OLED. | `tickWifi` |
| `0x53 NETWORK_STATE` | 5 páginas cada 200 ms; se repite cada 3 s o al cambiar algo | Foto de red para el dashboard, Datos de red AP y el QR: página 0 estado/IP, 1 SSID STA, 2 SSID AP, 3-4 clave del AP. Redactado. | `broker_network.cpp`, `tickNetwork` |
| `0x1D LIMITS` | 3 s | Límite de velocidad, PWM y geometría para validar y recortar los mandos de la Web. | `tickProduct` |
| `0x60 US_GET` | 1 s propio + pedidos de la Web | Uno por segundo es del ESP (`tickSensors`, `_head`). Los demás seguidos son `robot.ultrasonic.get` de la Web: el modelo del auto lo pide cada 100 ms con un solo pedido en vuelo, y el ESP los atiende cuando el enlace se libera. | `broker_sensors.cpp`, `ConnectionContext.tsx` |

No aparecen en esta captura `0x12 WEB_PULSE` (solo con una sesión Web lista e interactiva) ni
`0x02 ALIVE` (solo si el enlace queda ocioso 500 ms, y con tantos pedidos nunca queda).
En total son unas 12 tramas por segundo del ESP más sus respuestas. Los dos LIMITS seguidos y la
fuente de cada US_GET extra no se pueden asegurar solo con "RX ATmega": hace falta "Enlace completo"
para ver qué respuesta corresponde a cada pedido.

## Estado

loader=2.3.0 compilado en carpeta temporal sin errores; el ejecutable abierto del usuario no se
reemplazó; sin abrir puertos. atmega/esp/web=sin cambios.
