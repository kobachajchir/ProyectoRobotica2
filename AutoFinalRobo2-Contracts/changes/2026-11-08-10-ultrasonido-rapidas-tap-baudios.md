# AFR2-20261108-10 - HC-SR04 bajo pedido, ícono de largo en el dashboard y baudios por fuente TAP

Pedido del usuario (08/11/2026), tras cargar el firmware 1.4.0:

1. El HC-SR04 medía siempre por defecto; debe ser una opción de las pantallas rápidas.
2. Volver a poner el ícono de pulsación larga en el dashboard.
3. ¿Cada cuánto se pide DIAG_POLL?
4. El selector de fuente no confirmaba. Log y un solo sentido deben ir a 115200; enlace completo a
   250000, con el Loader y el firmware cambiando de velocidad juntos.

Versión: 1.5.0 (subversión). Loader 2.2.1.

## Qué cambió

### HC-SR04 (ATmega + Web)
- Causa: `Afr2Sensors_Task` disparaba un eco cada `interval_ms` (70 ms) siempre que el servo estuviera
  quieto, aunque nadie pidiera distancia.
- Ahora solo mide si `head.sampling` (interruptor nuevo), el radar está en marcha o está abierta la
  pantalla Distancia numérica (`AFR2_N_US_VIEW`, demanda `head.view`). Todo apagado tras cada reset.
- Controles rápidos (0x010003) pasa de 7 a 8 páginas: Radar, Servo, Motores, Línea, Rastro,
  **Ultrasonido** (MIDIENDO/APAGADO, acción nueva `AFR2_A_US_TOGGLE = 107`, ícono sensores), Ajustes,
  Modo. `quick_flags` bit 5. Ítem en `extra[48]` (los índices existentes no se mueven).
- Con el interruptor apagado `ultrasonic_valid` cae a los 200 ms: SENSORS, US_GET y la Web muestran la
  distancia como no válida, que es el estado real.
- Web: `firmwareRenderer.ts` (carrusel 8 páginas, ítems 5..7, ícono), `uiSimulation.ts` (acción 107,
  límites del carrusel), `generated/firmwareLibrary.ts` editado a mano (símbolo 107, `extra[48]`,
  nodo con 8 ítems y hashes de fuentes). Conviene regenerarlo con `tools/sync-oled-library.mjs`
  cuando se autorice ejecutarlo. `screens.json` 010003 con 8 ítems.

### Ícono de largo en el dashboard (ATmega + Web)
- Se había quitado en AFR2-20261106-01 para liberar la esquina inferior derecha del anillo de progreso.
- Vuelve como ícono de largo de 11 px en x=47 con «Rapidas» en x=60..109; la esquina 106..127 sigue
  siendo del anillo, que solo aparece mientras se mantiene apretado (puede tapar la última letra en
  ese momento). `afr2_dashboard.c` y `dashboardRenderer.ts` con las mismas coordenadas.

### Baudios por fuente TAP (ATmega + Loader 2.2.1)
- Causa del "no confirmó": el 1.4.0 dejaba el log a 250000 en todos los modos y el monitor estaba a
  115200.
- Ahora el log y los modos 0, 1, 2 y 4 usan **115200**. Solo el modo 3 (enlace completo) usa
  **250000** (`AFR2_DEBUG_FAST_BAUD`).
- La confirmación `TAP mode=n <nombre> baud=<b>` sale a la velocidad vieja; el ATmega calla 500 ms,
  cambia el puerto y escribe `TAP baud=<b>`. Lo descartado en esa ventana se cuenta en `tapLost`.
- El Loader lee `baud=` y cambia la velocidad del mismo COM **sin cerrarlo** (`ChangeBaudAsync`;
  reabrir puede reiniciar el Mega). El cuadro Baudios se actualiza solo.
- Un reset del ATmega en modo 3 lo devuelve a 115200; a 250000 eso solo se ve como silencio o ruido.
  Si el Loader no recibe líneas válidas ni registros sanos en 7 s, vuelve a 115200 y a la fuente log, y
  lo avisa.

### DIAG_POLL (sin cambios)
- El ESP pide `DIAG_POLL` cada 400 ms y `WIFI_POLL` cada 400 ms, siempre que el enlace de producto esté
  listo (`broker_product.cpp`, `broker_wifi.cpp`). Son unas 5 tramas por segundo más sus respuestas.

## Estado

atmega=**compilado** 08/11 (`make -B firmware communication-firmware library`, avr-gcc 7.3.0): sin
errores; una advertencia previa `-Wmisleading-indentation` en `afr2_product.c:213`, ajena a este
cambio. Flash 149844 B (57,2 %), SRAM estática 7012 B (85,6 %): ~1180 B para pila, a medir con
`STAT stack>=`. **Sin cargar ni probar.** web=fuente editada (biblioteca generada a mano), sin build ni
navegador; esp=solo versión compartida, sin compilar; loader=2.2.1 compilado e iniciado, sin abrir
puertos.
