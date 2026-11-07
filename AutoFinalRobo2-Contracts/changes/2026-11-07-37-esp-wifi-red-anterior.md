# AFR2-20261107-37 - ESP: vuelta a la última red Wi-Fi que funcionó

Pedido del usuario: si no encuentra la red configurada, buscar la anterior que estuvo conectada.

- `wifi.json` (esquema `afr2-wifi-2`, compatible hacia atrás) suma `staGood` (la red STA configurada conectó al menos una vez) y `staPrevious` {ssid,password}.
- Al guardar una red STA distinta, la anterior pasa a `staPrevious` solo si `staGood` era verdadero; la nueva empieza con `staGood=false` y se marca al primer `WL_CONNECTED`.
- Arranque: intenta la configurada. Si a los 15 s no conectó y hay anterior, prueba la anterior (otros 15 s); si tampoco, abre el AP de provisión como antes. Los reintentos cada 30 s alternan configurada / anterior, de modo que vuelve a la preferida cuando reaparece.
- `robot.wifi.status` agrega `staNetwork` (`none|configured|previous`) y `previousAvailable`; la Web (WifiPage) avisa «Conectado a / Probando la red anterior». Las contraseñas no se exponen.
- Sin credenciales en fuentes: las redes se cargan desde la Web y viven en LittleFS del ESP.

Compilación a pedido: ESP `esp12s_candidate` PASS, RAM 50808 B (62,0 %), flash 454939 B (43,6 %); Web `tsc -b` PASS. Sin flashear ni probar con redes reales.
Estado: esp=fuente editada y compilada, sin cargar; web=fuente editada y compilada; atmega=sin cambios.
