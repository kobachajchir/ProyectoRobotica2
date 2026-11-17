# AFR2-20261117-08 - «Reconectando a» muestra el nombre del AP en vez de la red Wi-Fi

Autoría: Koba Chajchir / AFR2.
Versión: 1.35.3 (subversión mínima).

Estados:
- esp = compilado sin cargar;
- atmega = solo versión, compilado;
- web = solo versión, compilada para el ESP.

Sin cambio de contrato UART: cambia el valor del campo SSID STA, no su formato.

## Pedido

En modo STA+AP, el OLED muestra el aviso «Reconectando a» con el nombre del AP del auto en lugar de la red Wi-Fi a la que intenta conectarse.

## Causa

El ATmega guarda la red STA (`wifi.ssid`) desde dos mensajes del ESP:

1. `NETWORK_STATE` página 1 (`displayInfo().staSsid`): solo con la STA conectada; si no, va vacío.
2. `WIFI_UPDATE` subtipo 0 (estado, cada 2 s), armado en `broker_wifi.cpp`. Hacía `staSsid | apSsid`: sin STA conectada (en el JSON, `staSsid` es `null`) mandaba **el nombre del AP** en el campo de la red STA.

Al terminar las 5 páginas del estado de red, `afr2_network.c` calcula la fase: con STA pedida, sin conectar y `ssid` no vacío, muestra «Reconectando a» + `ssid`. Si el estado Wi-Fi llegaba entre la página 1 y el final, `ssid` tenía el nombre del AP. Además, los dos mensajes se pisaban el `ssid` entre vacío y el AP, y eso invalidaba `details_valid` en cada vuelta.

## Corrección (ESP)

- `src/broker_wifi.cpp`: el estado Wi-Fi manda `staSsid` si hay conexión. Si no, y el modo es STA o AP_STA, manda la red de estación configurada (`WiFi.SSID()`, la que el ESP intenta). En modo AP va vacío. Nunca manda el AP.
- `src/wifi_service.cpp`: `displayInfo()` hace lo mismo cuando la STA está pedida (`staWanted_`) y no conectada. Los dos mensajes coinciden.
- El ATmega no cambia. Sus otros usos del `ssid` (dashboard, avisos de IP, etiqueta de red) ya dependen de `sta_connected`. El JSON de la Web (`wifi.info`) no cambia: `staSsid` sigue en `null` sin conexión.

## Estado de validación

Compilado el 17/11/2026, a pedido del usuario:

| Módulo | Resultado |
|---|---|
| ESP | `platformio run -e esp12s_candidate`: SUCCESS. RAM 58,6 %; flash 479 255 B (45,9 %). Una advertencia previa ajena: `draftToken` sin uso |
| ATmega | `make -B firmware communication-firmware library`: exit 0. `.data` 10 B, `.bss` 6 848 B |
| Web ESP | `npm run build:esp` (con `tsc`): exit 0. Paquete 1.35.3 en `ESP/data/dist`; estimación FS 2 859 008 de 2 867 200 B |
| LittleFS | `-t buildfs`: SUCCESS |

`build:esp` primero se detuvo por el control de huellas del renderer OLED, porque 1.35.2 (AFR2-20261117-07) cambió 8 archivos del ATmega que la Web registra:
- `afr2_screens.c`, `afr2_network_screen.c`, `afr2_icons.c`, `afr2_menu_catalog.c`;
- `afr2_sensor_screens.c`, `afr2_qr_screen.c`, `qrcodegen.c`, `afr2_dashboard.c`.

Se revisó cada cambio: solo mueven literales y tablas a flash (`PSTR`/`PROGMEM`). Los caracteres, las coordenadas, el desplazamiento y los bytes del QR son los mismos, así que `firmwareRenderer.ts` no requiere portar nada. Por eso se actualizaron las 8 huellas en `Web/src/features/oled/firmwareRenderer.sources.json`.

Hosting no se volvió a publicar: sigue en 1.35.1.

No probado en el auto con STA+AP y la red ausente.

## Pendiente

- Cargar ESP, LittleFS y ATmega (1.35.3).
- Probar en STA+AP con la red apagada: el aviso debe decir «Reconectando a <red Wi-Fi>».
