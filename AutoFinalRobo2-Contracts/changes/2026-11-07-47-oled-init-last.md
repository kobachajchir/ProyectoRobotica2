# AFR2-20261107-47 — Pantalla inicializada al final y configuración confirmada

07/11/2026. Pedido del usuario: cambiar el orden de inicialización y dejar la pantalla al final. A veces inicia y a veces no, y el usuario indica que no es el cableado eléctrico.

Estado real: atmega = fuente editada, sin compilar ni cargar; web = hashes del manifiesto del renderer actualizados, sin compilar; esp = sin cambios. Sin pruebas físicas.

## Hipótesis de software

Se descartaron otras causas: ninguna inicialización posterior pisa `PRR0`, `DDRD/PORTD` ni `TWCR`; `Afr2App_Init` no mueve el servo; y el Loader abre los puertos con DTR/RTS apagados, así que no reinicia el Mega.

La pantalla se configuraba a los 500 ms. A esa altura el panel puede estar todavía saliendo de su propio reset de encendido, y el ESP hace su arranque y calibración de RF. Si el panel confirma (ACK) los comandos pero no los aplica, el driver lo da por inicializado y no repite la configuración completa: la pantalla queda en negro o mal configurada hasta un reinicio. El refresco liviano de cada segundo (AFR2-20261107-46) no repone el oscilador, la precarga ni el VCOMH.

## Cambios

- `src/main.c`: TWI, I2C Manager y OLED se inicializan al final, después de placa, producto (preferencias, motores y sensores), UART1, UI, app, enlace y encoder. `Afr2App_Init` solo guarda el puntero de la pantalla.
- `src/display/afr2_oled.c`: `AFR2_OLED_STARTUP_MS` pasa de 500 a 1200 ms, contados desde `sei()`. La pantalla queda en negro un poco más al encender.
- `src/display/afr2_oled.c` e `include/afr2_oled.h`: nuevo campo `full_refresh`. Tras la primera inicialización en frío, 300 ms después (`AFR2_OLED_CONFIRM_MS`) se repite una configuración completa (oscilador, precarga y VCOMH incluidos). Se da por hecha solo cuando el panel confirma la transferencia; si falla, se reintenta completa. Los refrescos siguientes vuelven a ser livianos.
- `AutoFinalRobo2-Web/src/features/oled/firmwareRenderer.sources.json`: hashes de `afr2_oled.c` y `afr2_oled.h` actualizados. El dibujo no cambia. Los 17 hashes coinciden.

## Pendiente

- Compilar, cargar y probar varios arranques en frío y en caliente.
- Si sigue fallando, mirar en el diagnóstico OLED «OLED listo», «Errores» y «Reset:» justo después de un arranque fallido.
