# AFR2-20261108-06 - Selector «Sin red Wi-Fi»: Iniciar AP o Seguir esperando

Pedido del usuario: si no encuentra la red Wi-Fi elegida, mostrar «Wi-Fi no encontrado»; luego una pantalla selectora
«Iniciar AP» / «Seguir esperando» con 5 s de cuenta. Sin interacción inicia el AP (primera opción y defecto). Si se
interactúa, la cuenta se detiene, la barra desaparece y queda esperando la elección. En Preferencias se configura el
tiempo de reintento. Al iniciar el AP se muestra el aviso de siempre. Ubicar la pantalla en el árbol con criterio.
Solo fuente: sin compilar, flashear ni probar con el auto.

Versión: 1.1.0 (subversión: función nueva compatible). El código quedó en 1.2.0 porque AFR2-20261108-07 se hizo en la
misma tarea, antes de cargar (docs/VERSIONADO.md).

## Comportamiento

1. El ESP, con la STA configurada y sin AP, no conecta en la gracia de siempre (15 s; 30 s si prueba la red anterior).
   Antes levantaba el AP de provisión solo. Ahora, si hay un OLED que puede responder (EX01 listo, v>=5,
   `AFR2_EXT_F_NETWORK_UI`), envía el evento de red nuevo `AFR2_NET_STA_NOT_FOUND` (12) con el motivo en `subject`:
   1 red no visible, 2 clave incorrecta, 3 no se pudo unir. La STA sigue intentando cada 30 s como antes.
2. El ATmega muestra el aviso 0x04000C (3 s): «Wi-Fi no» / «encontrado» y el motivo.
3. Al cerrarse el aviso, la app abre «Sin red Wi-Fi» (0x03004B, nodo `AFR2_N_WIFI_ASK`=74) solo si la UI está libre: no
   la abre sobre Sí/No, edición, teclado, táctil, Hall, prueba de encoder, odometría ni secuencia (espera a que terminen).
4. Pantalla: encabezado «Sin red Wi-Fi», «Inicia AP en N s» con barra de 5 s, filas «Iniciar AP» (seleccionada) y
   «Seguir esperando».
   - Sin interacción 5 s: Iniciar AP.
   - Giro: cambia la fila y detiene la cuenta; la barra desaparece y el texto pasa a «Elija una opcion». Apretar el
     encoder también la detiene. No vence más.
   - Corto: ejecuta la fila elegida.
   - Largo al soltar: Seguir esperando (no hay opción de salir sin decidir; el ESP quedaría sin respuesta).
   - Un aviso encima pausa la cuenta. Si la STA conecta o aparece un AP, la pantalla se cierra sola.
   - Si otra pantalla obligatoria la desplaza sin respuesta, la app envía Seguir esperando.
   - Vuelve a la pantalla anterior (historial); sin historial, a Wi-Fi.
5. Iniciar AP: trabajo Wi-Fi op 13; el ESP levanta el AP de provisión (igual que el automático) y llega el aviso
   «Red AP creada» (0x040004) con SSID, clave e IP, como siempre. La respuesta del trabajo no agrega aviso propio.
6. Seguir esperando: trabajo op 14 con 2 bytes (segundos u16LE de la preferencia). El ESP hace un intento inmediato,
   sigue con la cadencia de 30 s y, si al vencer el tiempo no conectó, vuelve a enviar el evento (paso 2). El auto muestra
   «Seguir esperando / Reintento: N s».

## Ubicación en el árbol

Hija de Wi-Fi > AP / fallback (0x030024), porque decide justamente si se levanta el AP de respaldo. No es una fila del menú:
la abre el evento, como «Prueba odometria» (0x03004A) o «Secuencia». Retorno por contexto.

## Preferencia

Sistema > Preferencias, fila 2: «Reintento Wi-Fi s», 10..300 s, paso 10, defecto 30 (acción 106 `AFR2_A_WIFI_RETRY_S`).
Se inserta por código en `Afr2Menu_Item` (`extra[47]`) sin mover `reform[]`; «Descartar cambios» pasa de la fila 3 a la 4.
EEPROM v11: payload 66 bytes (+`wifi_retry_s` u16 al final). Un registro v10 se lee con 30 s; no se reescribe solo.
La acción 106 también es editable por CONFIG desde la Web (sección preferencias) porque es un editor numérico del menú;
el ESP amplía el rango de acciones de `robot.calibrate`/`robot.config.get` a 106.

## Compatibilidad

- No hay bit de capacidad nuevo: un ESP anterior rechaza en el HELLO cualquier bit fuera de `AFR2_EXT_FEATURE_MASK`.
- ESP nuevo con ATmega anterior: el ATmega responde BAD_REQUEST al evento 12 (fuera de `AFR2_NET_EVENT_MAX`); el ESP lo
  descarta, deja de preguntar y levanta el AP automático (no traba la cola de eventos).
- ESP anterior con ATmega nuevo: el evento nunca llega; el AP automático sigue igual.
- Sin OLED enlazado (nunca o 10 s sin enlace mientras pregunta o espera): AP automático.
- `wifi.status` agrega `apDecision` (`none`/`asking`/`waiting`) y `apDecisionInMs` en espera.

## Archivos

- Contracts: `contracts/afr2_network_wire.h` (evento 12, `EVENT_MAX`), `contracts/ATMEGA_CONFIG_ACTIONS.json` (105, 106),
  `documentation/screens.json` (0x03004B, 0x04000C, Preferencias 7 filas), `documentation/UI_SCREEN_FLOW.md`, CHANGELOG.
- ATmega: `afr2_menu_catalog.h/.c` (nodo 74, acciones 105/106, `extra[47]`, Preferencias), `afr2_product.h/.c`
  (`wifi_retry_s`, ops 13/14, EEPROM v11, validación), `afr2_preferences.c` (v11), `afr2_app.h/.c` (estado `wifi_ask`,
  apertura/cierre, invocación, preferencia, dirty), `afr2_app_wire.c` (payload op 14, sin aviso propio al resultado),
  `afr2_network.c` (evento 12 → aviso y selector), `afr2_network_screen.c` (aviso), `afr2_debug.c` (nombre en el log), `afr2_screens.h/.c` (pantalla, cuenta,
  giro/corto/largo, pausa, Preferencias fila 4, cierre al reiniciar el ESP; `AFR2_WIFI_ASK_MS`).
- ESP: `wifi_service.h/.cpp` (pregunta en vez de AP automático, espera, motivo, respaldo sin OLED, estado),
  `broker.cpp` (modo pregunta según enlace), `broker_network.cpp` (envío del evento, BAD_REQUEST → AP automático),
  `broker_wifi.cpp` (ops 13/14), `broker_product.cpp` (rango de acciones hasta 106).
- Web: `features/oled/firmwareRenderer.ts` (pantalla 0x03004B, aviso 0x04000C, Preferencias; el nodo virtual del
  selector de motor pasa de 74 a 75), `firmwareRenderer.sources.json` (hashes de los 4 archivos C portados),
  `features/developer/uiSimulation.ts` (valor 106 y versión EEPROM 11), `features/control/calibrationGuide.ts` (106).
  `generated/firmwareLibrary.ts` no se regeneró: lo hace `sync-oled-library.mjs` al iniciar dev/build.

## Estado y límites

atmega=fuente editada, sin compilar ni cargar; esp=fuente editada, sin compilar ni flashear; web=fuente editada, sin
build, sin regenerar la biblioteca ni navegador. SRAM/flash sin medir (+8 bytes en `Afr2Ui`, +1 en `Afr2App`,
+2 en `Afr2Config`, una fila PROGMEM). El simulador del desarrollador no modela el evento ni el selector. Hay que cargar
ATmega y ESP para usar el selector; con uno solo se mantiene el AP automático. Sin prueba física.
