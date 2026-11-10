# AFR2-20261109-06 — Navegación fija en submenús Wi-Fi, modo STA sin AP fallback, clave directa con autoconexión e ingreso desde la Web

Versión: **1.10.2 (subversión mínima)**.

## Resumen de correcciones y ajustes

1. **Retorno fijo («Volver» y pulsación larga) en árboles de submenús sin loops por notificaciones:**
   - En [`afr2_screens.c`](../../AutoFinalRobo2-Atmega/src/ui/afr2_screens.c), [`uiSimulation.ts`](../../AutoFinalRobo2-Web/src/features/developer/uiSimulation.ts) y [`FirmwareLibraryPreview.tsx`](../../AutoFinalRobo2-Web/src/features/oled/FirmwareLibraryPreview.tsx), todos los submenús de Wi-Fi y el menú principal regresan siempre a su nodo padre fijo del catálogo (`n.parent`), tanto al elegir **«Volver»** como al soltar una pulsación larga (`>= 800 ms`).
   - `back()` ya no invoca `Afr2Ui_Open()` cuando `u->depth == 0`, evitando que el nodo hijo se apile sobre `u->history[0]` y genere un bucle infinito entre `Redes encontradas` (`0x030021`) y `Buscar redes` (`0x030042`) o `Ingresar credenciales` (`0x030022`).
   - `Afr2Ui_Open()` mantiene `u->history` acíclico y reinicia la pila al entrar a `Inicio` (`0x010001`), `Controles rápidos` (`0x010003`) o `Menú principal` (`0x010002`).
   - Cerrar o vencer una notificación (`dismiss`, `dismiss_input`, `Afr2Ui_AfterNotice`) solo cierra el aviso en pantalla sin reescribir `u->node` ni alterar el camino de salida hacia `Menú principal` e `Inicio`.

2. **Modo `STA` desactiva el AP fallback:**
   - En [`wifi_service.cpp`](../../AutoFinalRobo2-ESP/src/wifi_service.cpp), al aplicar o cambiar al modo `STA` puro (`mode == 1` / `"STA"`), se desactiva `askFallback_` (`false`), se marca `apStopped_ = true` y se limpia `askState_` (`0`), de modo que si no encuentra la red configurada no levanta su propio punto de acceso ni dispara el selector «Sin red Wi-Fi».
   - `changeMode(1, ...)` permite pasar a `STA` incluso si todavía no había una red STA guardada en `LittleFS`, dejando la radio en `WIFI_STA` sin AP.
   - En [`afr2_app.c`](../../AutoFinalRobo2-Atmega/src/services/afr2_app.c) y [`afr2_network.c`](../../AutoFinalRobo2-Atmega/src/services/afr2_network.c), el ATmega tampoco abre `AFR2_N_WIFI_ASK` (`0x03004B`) cuando el modo observado es `STA` (`mode == 1`).

3. **Teclado directo en contraseña y conexión automática tras elegir una red escaneada:**
   - Al seleccionar una red en `Redes encontradas` (`AFR2_A_WIFI_NETWORK`), el SSID elegido queda cargado en `draft_ssid` y se abre `Credenciales` (`0x030022`) con la fila **«Ingresar aquí»** preseleccionada.
   - Al pulsar **«Ingresar aquí»** (`AFR2_A_KEYBOARD`), si el SSID ya fue elegido (`u->credentials.ssid[0] != 0`), el teclado en pantalla se abre directamente en **«Clave Wi-Fi»** (`u->keyboard_field = 1`, `u->length = 0`) sin volver a pedir el SSID.
   - La pantalla de ingreso permanece activa sin ser interrumpida por avisos de fondo hasta que el usuario confirma **`OK`** o cancela con pulsación larga.
   - Al confirmar **`OK`** en la contraseña, el ATmega envía `AFR2_WIFI_LOCAL_DRAFT` (`op == 4`) y el ESP ([`broker_wifi.cpp`](../../AutoFinalRobo2-ESP/src/broker_wifi.cpp)) ejecuta `wifi_.stage(...)` de inmediato (`wifiResult_ = 2`), iniciando la conexión a la red seleccionada y regresando a `Wi-Fi` (`0x03001F`) con el aviso **«Conectando...»**.

4. **Sin interrupciones ni inicio de AP mientras se buscan redes o se ingresan credenciales:**
   - En [`wifi_service.h`](../../AutoFinalRobo2-ESP/src/wifi_service.h) / [`wifi_service.cpp`](../../AutoFinalRobo2-ESP/src/wifi_service.cpp) se incorpora `holdFallback(now)` y se retiene el temporizador de fallback mientras hay un escaneo en curso o resultados listos (`scanPending_ || scanReady_`) o mientras el usuario está en el flujo de configuración (`op == 1, 2, 3, 4` y borrador Web pendiente en [`broker_wifi.cpp`](../../AutoFinalRobo2-ESP/src/broker_wifi.cpp)).
   - Antes de iniciar la conexión (`stage()`), si había un escaneo asíncrono en curso se limpia con `WiFi.scanDelete()` para que `WiFi.begin()` conecte de forma confiable.
   - En [`afr2_app.c`](../../AutoFinalRobo2-Atmega/src/services/afr2_app.c) y [`afr2_network.c`](../../AutoFinalRobo2-Atmega/src/services/afr2_network.c), mientras el usuario está en `Buscar redes`, `Redes encontradas`, `Credenciales`, `Configurar`, `Cambiar modo` o con el teclado abierto, se suprimen los avisos de reconexión/no encontrado en segundo plano y se responde `AFR2_WIFI_WAIT` sin abrir `Sin red Wi-Fi`.

5. **«Ingresar en Web» con AP activo y cliente conectado:**
   - En [`broker_wifi.cpp`](../../AutoFinalRobo2-ESP/src/broker_wifi.cpp) y [`broker.cpp`](../../AutoFinalRobo2-ESP/src/broker.cpp), `AFR2_WIFI_WEB_DRAFT` (`op == 3`) acepta tanto clientes con sesión WebSocket activa como estaciones conectadas al AP del robot (`WiFi.softAPgetStationNum() > 0`), publica `wifiDraft` en `system.status` y también en `wifi.info`, y al recibir `wifi.draft.submit` inicia la conexión de inmediato (`wifi_.stage(...)`, `wifiResult_ = 2`).
   - En [`ConnectionContext.tsx`](../../AutoFinalRobo2-Web/src/features/connection/ConnectionContext.tsx), [`WifiDraftModal.tsx`](../../AutoFinalRobo2-Web/src/features/wifi/WifiDraftModal.tsx) y [`WifiCredentialsFields.tsx`](../../AutoFinalRobo2-Web/src/features/wifi/WifiCredentialsFields.tsx), la Web detecta `wifiDraft` por evento o por `wifi.info`, muestra una notificación informando la red seleccionada en el OLED, abre el modal con el SSID fijado y foco directo en la contraseña, y envía la orden de conexión inmediata.
   - Además, se corrigió en [`AssemblyPage.tsx`](../../AutoFinalRobo2-Web/src/features/assembly/AssemblyPage.tsx) el tipo de `size` en `UiIcon` (`28` y `40` en lugar de `32` y `48`) que detenía `tsc -b` durante `npm run build:esp`, y se configuró localmente `AutoFinalRobo2-ESP/src/wifi_defaults.local.h` (ignorado por Git) con SSID `Koba`.

## Estado real por módulo

- **atmega:** `done` en fuente (`afr2_screens.c`, `afr2_app.c`, `afr2_app_wire.c`, `afr2_network.c`); sin compilar ni flashear en este paso.
- **esp:** `done` en fuente (`wifi_service.h`, `wifi_service.cpp`, `broker_wifi.cpp`, `broker.cpp`, `wifi_defaults.local.h`); sin compilar ni flashear en este paso.
- **web:** `done` en fuente (`ConnectionContext.tsx`, `WifiDraftModal.tsx`, `WifiCredentialsFields.tsx`, `uiSimulation.ts`, `FirmwareLibraryPreview.tsx`, `firmwareRenderer.sources.json`, `AssemblyPage.tsx`); sin ejecutar build en este paso.
- **loader:** compilado previamente a pedido expreso del usuario en `AutoFinalRobo2-Loader/AFR2Loader.exe` y `AFR2 Loader.exe`.
- **cad:** sin cambios.
