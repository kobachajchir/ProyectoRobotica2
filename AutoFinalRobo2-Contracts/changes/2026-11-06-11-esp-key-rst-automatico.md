# AFR2-20261106-11 · KEY/RST del ESP automáticos (opcional)

Plan y circuito: [PLAN_KEY_RST_AUTOMATICO.md](../hardware/PLAN_KEY_RST_AUTOMATICO.md). Pedido del usuario; implementación en **fuente**, sin compilar, cargar ni probar (AGENTS.md).

## Qué cambia

- **Hardware (a armar por el usuario):** dos NPN en paralelo con los pulsadores KEY y RST del shield OSOYOO; bases desde D32 (KEY) y D33 (RST) por 1 kΩ, con 10 kΩ de cada base a GND del lado del transistor. Apagado por defecto: sin cables de control, con otro firmware o con firmware AT del ESP no se aprieta nada.
- **Atmega** `service/esp_uart_bridge.c` (flasher de servicio): trama de control opcional `A5 "AFR2CTL" cmd ~cmd` con órdenes QUERY, ENTER_BOOT, RUN, RELEASE y RST_PULSE. D32/D33 son entradas en alta impedancia hasta recibir una orden, vuelven a entrada al soltar, por tiempo máximo (~600 s) y en cualquier reinicio del Mega. Resto de bytes reenviados sin cambios (un `0xA5` suelto al final espera al siguiente byte). `include/board_pinmap.h` reserva D32/D33; el firmware de producto no los configura.
- **Loader** `tools/esp_bridge_esptool.py` + `Operations*.cs` + `Profile.cs`: opción de perfil `EspKeyRst` = `manual` (predeterminado, no envía nada) o `auto` (solo con `mega_bridge`). En auto: el wrapper identifica el flasher AFR2 con QUERY; si no responde, **no envía órdenes** y falla con mensaje claro. Entra en carga tras la espera de arranque del Mega con el mismo COM, y al cerrar el puerto (o salir esptool) ejecuta RUN: suelta KEY y reinicia el ESP. En auto no se piden KEY/RST ni el aviso de soltar KEY.
- **Configuración desde la PC:** el flasher arranca siempre con el modo automático **desactivado** (RAM, se pierde en cada reinicio del Mega) y rechaza ENTER_BOOT/RUN/RST_PULSE con estado 1 hasta que la PC envía `AUTO_ON` (0x21); `AUTO_OFF` (0x20) suelta los pines y vuelve a rechazar. Cada respuesta informa el estado (bit2 de flags), y el Loader lo lee de vuelta: si no coincide con lo pedido, aborta. En la pestaña «Flasher ATmega» hay selector Manual/Automático y botones «Aplicar y consultar KEY/RST» y «Solo consultar KEY/RST» (`wrapper --flasher-keyrst query|on|off`). En una operación ESP automática el wrapper fija `AUTO_ON` antes de la secuencia.
- **Avisos en modo automático:** con `EspKeyRst=auto` no aparecen el asistente de apretar KEY/RST, el aviso «Ya podés soltar KEY» (primer porcentaje, fin de operación y mensajes finales) ni las instrucciones de mantener KEY en los checklists y estados; esos textos pasan a indicar que el flasher lo hace. En manual se conservan sin cambios.
- **Retiro de D0/D1 (Loader):** eliminado el método `mega_reset` (D0/D1 + RESET-GND): opción del selector, checklists, textos y ramas de `Operations*.cs`/`MainForm*.cs`. `EspTransport` acepta solo `mega_bridge` y `usb-ttl`; un perfil con `mega_reset`/`mega-usb` se migra a `mega_bridge` al abrirse, sin aviso propio: el usuario debe revisar el cableado. Se conservan las advertencias de tener el ESP **fuera** de D0/D1 al cargar el ATmega por bootloader. Perfil de ejemplo, README, MCP.md, AGENTS.md y el campo del MANUAL actualizados.

## Estado (atmega / esp / web)

- atmega: fuente editada; **no compilada ni cargada**.
- esp: sin cambios (el ESP no ve el circuito).
- web: sin cambios.
- Loader: fuente editada; **no compilado ni ejecutado**.
- Hardware: **no armado ni medido**. Los controles de continuidad del plan siguen pendientes.

## Límites conocidos

- La orden viaja en banda por el mismo USB; no probado contra datos reales de esptool.
- Abrir el COM reinicia el Mega por DTR: la secuencia ocurre después de la espera de arranque (`BridgeStartupSeconds`).
- Si hay cables de control conectados y otro firmware usa D32/D33 como salida en alto, el transistor conduce. Desconectar los cables de control en ese caso.
- Quedan menciones del método D0/D1 en documentación de hardware (CARGA_USB_COM16.md, PINOUT, MEGA_ESP_PUENTE_SOFTWARE.md, PLAN_PRUEBA_FLASHER_MEGA.md), ayudas Web, READMEs de Atmega/ESP y `service/` (diagnóstico I2C por USART0): sin depurar archivo por archivo. FLASHING.md lleva aviso de retiro.
- El código del Loader no se compiló: la eliminación de ramas puede tener errores de sintaxis.
