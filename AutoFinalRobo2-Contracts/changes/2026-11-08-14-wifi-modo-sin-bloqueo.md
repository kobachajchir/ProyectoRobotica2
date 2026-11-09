# AFR2-20261108-14 - Cambiar el modo Wi-Fi desde el OLED sin "Detener / ver red"

Pedido del usuario (08/11/2026): al elegir STA, AP o AP+STA desde el auto aparecía "Detener / ver red";
si el usuario elige un modo, el auto debe detener lo que haga falta y pasar a ese modo.

Versión: 1.7.1 (subversión mínima).

## Causa

`AFR2_A_WIFI_CHANGE_MODE` / `AFR2_A_WIFI_SET_MODE` (`afr2_app.c`, `invoke`) devolvía `AFR2_BAD` si el
auto no estaba detenido o si había un trabajo Wi-Fi pendiente (búsqueda, «Sin red Wi-Fi», estado...).
`reason()` lo mostraba como "Detener / ver red". `AFR2_A_WIFI_DISCONNECT` hacía lo mismo con
`AFR2_BUSY`.

## Cambio (ATmega)

- Elegir modo o Desconectar detiene el movimiento (`Afr2Product_Stop`) si hacía falta y reemplaza el
  trabajo Wi-Fi pendiente por el nuevo (id nuevo).
- El ESP no necesita cambios: si llega tarde una respuesta del trabajo reemplazado, el ATmega la rechaza
  por id (`UNAVAILABLE`); `finishWifi` del ESP descarta ese trabajo y en el siguiente `WIFI_POLL`
  (400 ms) toma el nuevo cambio de modo.
- Sigue rechazando un modo fuera de 1..3.

## Estado

atmega=fuente editada, `avr-gcc -fsyntax-only` de `afr2_app.c` sin errores; sin compilar el HEX ni
cargar. esp=sin cambios (solo versión compartida). web=solo versión. loader=sin cambios.
