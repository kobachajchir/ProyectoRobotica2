# AFR2-20261107-48 — La OLED parecía trabada al cerrar una notificación con el encoder

07/11/2026. Pedido del usuario: la pantalla se traba cuando muestra una notificación y hace clic en el encoder.

Estado real: atmega = fuente editada, sin compilar ni cargar; web = hashes del manifiesto del renderer actualizados, sin compilar; esp = sin cambios. Sin pruebas físicas.

## Causa encontrada (lectura de fuente)

Con la Web abierta, el ESP manda eventos de red uno detrás de otro: WebSocket abierto o cerrado, cliente que entra o sale, «Web servida». El ATmega acepta hasta 3 notificaciones en cola y responde `UNAVAILABLE` a las demás, que el ESP reintenta. Cada clic cerraba una sola notificación (`dismiss`); en ese mismo momento aparecía la siguiente de la cola o una nueva reintentada, así que el clic parecía no tener efecto. El arreglo de AFR2-20261106-09 resolvía esto solo en las pantallas QR.

No se encontró un bloqueo real del loop: el descarte, el render del dashboard tras la notificación y el anillo de pulsación se revisaron sin hallar fallas.

## Cambios

- `src/ui/afr2_screens.c`: nuevo `dismiss_input`, usado por el clic en todas las ramas de `Afr2Ui_Apply` y por la pulsación larga (`go_back`). Si la notificación visible es de red, se descartan todas las notificaciones de red en cola de una vez. Las demás, como «Cambios guardados» o los errores, conservan su orden y se siguen cerrando de a una. Se abre una ventana de silencio de 5 s. El cierre por tiempo (`Afr2Ui_Tick`) no cambia. La lógica de destino tras la última notificación (`notice_target`) pasa a `after_notices`, sin cambios.
- `include/afr2_screens.h`: campo `network_quiet_until_ms` (+4 B de RAM) y `Afr2Ui_NetworkQuiet()`.
- `src/services/afr2_network.c`: durante la ventana de silencio, los eventos frecuentes (`WS_OPEN`, `WS_CLOSE`, `WEB_SERVED`, `CLIENT_JOIN`, `CLIENT_LEAVE`) se aceptan, sin que el ESP reintente, pero no se muestran. `GOT_IP`, `AP_READY`, `STA_LOST`, `AP_STOPPED`, `WEB_READY` y `BOOT` siguen apareciendo.
- `AutoFinalRobo2-Web/src/features/oled/firmwareRenderer.sources.json`: hashes de `afr2_screens.c` y `afr2_screens.h` actualizados. El dibujo no cambia. La simulación de la Web (`uiSimulation.ts`) usa una sola notificación y no simula eventos de red, así que no requiere cambios.

## Pendiente

- Compilar, medir RAM y probar en el auto con la Web abierta: clic sobre «Conectividad» y verificar que la pantalla vuelve al contexto.
