# Wi-Fi desde AP · AFR2-20261110-13

Versión: 1.21.0 (subversión). ESP/Web solo fuente; ATmega solo versión compartida. La revisión fue por lectura y modificación de fuentes. No se ejecutaron tests, compilaciones, validadores, navegador, puertos ni flasheos; no hay confirmación de funcionamiento en el auto.

## Problemas localizados y comportamiento propuesto en fuente

| Flujo | Antes | Fuente actual |
|---|---|---|
| Buscar redes desde AP | Web ocultaba la sección y WifiService::scan rechazaba AP | La Web muestra Buscar redes. ESP habilita STA temporalmente, conserva la interfaz AP y su configuración y vuelve a AP-only al terminar el escaneo real. |
| Escaneo fallido o vencido | Restauración preparada pero nunca activada | Se usa scanRestoreAp_; no se intenta cancelar con scanDelete una operación SDK aún en curso. La restauración ocurre al completar realmente, incluso después del evento de timeout. |
| Conectar una red elegida desde AP | Selección proponía STA y la búsqueda ni siquiera era accesible | Propone AP-STA, con alternativa explícita STA. Pide SSID/contraseña, guarda en LittleFS y programa reinicio por el mecanismo existente. |
| Configurar AP | Configurar se llenaba con staSsid y dependía de un selector global cambiante | Acciones separadas Configurar AP y Configurar modo elegido. El formulario captura su propio modo y usa identidad AP o STA según corresponda. |
| Reiniciar en AP | Dependía del selector general | Botón Reiniciar en modo AP usa wifi.mode con AP; guarda ese modo conservando identidad AP y red STA separadas. |
| STA sin red guardada | changeMode podía cambiar radio en sitio sin programar reinicio; broker anunciaba un reinicio de todos modos | Rechaza la operación sin credenciales, abre el formulario del modo solicitado y solo declara restartScheduled si está realmente pendiente. |
| Confirmar conexión/reinicio | Podía observar otra red ya conectada o un estado AP anterior | wifi.info/status incluyen espBootId. La Web comprueba nuevo boot cuando pidió reinicio, modo esperado y SSID esperado cuando aplica. Si falta observación no declara éxito. |
| Ver última red conectada | Solo configuración y respaldo internos, sin consulta Web privada | Registro staLast separado, creado desde una asociación STA realmente conectada con IP. Consulta explícita y modal temporal. Los datos anteriores sin orden fiable se identifican como históricos. |
| Tamaño del archivo | Lector rechazaba más de 512 bytes; guardar podía generar un archivo luego ilegible | Límite de 1024 bytes compartido entre lector y escritor. Se comprueba tamaño antes de abrir el temporal; se conserva rename atómico. |

El escaneo conserva el AP, pero no se acredita continuidad física del cliente durante el barrido: el ESP comparte radio. Estado lógico AP permanece estable durante la habilitación STA temporal, evitando que Web/OLED crean que el usuario cambió de modo. scanTemporaryStation indica ese estado transitorio. No se ejecuta WiFi.begin para conectar durante una búsqueda AP.

## Credenciales por pedido explícito

Nuevo comando **nativo WebSocket del ESP**, `wifi.credentials.get`, sin ID ni tráfico UART. Pedido `{kind: last-sta|ap}`. Respuesta privada a la sesión que la solicita: `available`, `kind`, y si hay lectura válida, `ssid`, `password`, `source`, `confirmedLast`.

`last-sta` usa la asociación conectada actual o el registro staLast. Como migración, si el firmware anterior solo conservaba una red configurada que conectó o un respaldo confirmado, se puede mostrar ese dato con confirmedLast=false y aviso de orden no confirmado. No se presenta una red fallida como última conexión exitosa. Sin información válida devuelve available=false.

`ap` consulta la identidad AP guardada, o la identidad AP en RAM si aún no existe configuración. La Web diferencia esta consulta de Estado de red: un fallback puede tener otra identidad activa. Usar en configuración copia la lectura al formulario editable; cerrar el modal no envía cambios a la radio.

La contraseña se oculta inicialmente, puede revelarse por botón y se elimina del estado del diálogo al cerrar, perder WebSocket o a los 30 segundos. Si el usuario elige Usar en configuración, queda en el estado efímero de ese formulario hasta guardar, volver o desmontar. No se copia a localStorage/sessionStorage, estado compartido de ConnectionContext, eventos, reportes, UART ni logs. status y scans continúan sin claves. Esta consulta explícita es una extensión pedida por el usuario; no habilita exposición periódica ni un archivo público de configuración.

staLast se observa como máximo una vez por segundo y se escribe solo cuando se detecta otra asociación SSID y cambia el dato o falta marcar la conexión exitosa. No se escribe en cada iteración. El intento de escritura es único por transición observada, incluso si falla; se informa LAST_STA_SAVE_FAILED. Configuración existente que no pudo leerse no se reemplaza automáticamente. Se conserva schema afr2-wifi-2 y sus entradas AP, STA y respaldo. No cambia EEPROM ni EX01.

## Fuentes y límites

- ESP: src/wifi_service.h/cpp, src/storage.h/cpp y src/broker.cpp.
- Web: src/features/wifi/WifiPage.tsx, WifiCredentialsDialog.tsx y src/protocol/wsEnvelope.ts.
- Contracts: commands.json registra sensibilidad y estado real del comando nativo. wire_catalog.json, headers CMD y catálogo UART de Protocol Studio permanecen sin cambios, porque no se agrega ni modifica un comando ESP↔ATmega.

Se mantienen permisos existentes de configuración/cambio de modo: si el Mega está conectado, se requieren salidas detenidas y estado reciente. Escaneo y consulta Wi-Fi son servicios ESP y no exigen enlace con el ATmega.

Consulta de referencia: [implementación oficial del escaneo ESP8266](https://raw.githubusercontent.com/esp8266/Arduino/master/libraries/ESP8266WiFi/src/ESP8266WiFiScan.cpp), que habilita STA al escanear; [SSID/psk de ESP8266WiFiSTA](https://raw.githubusercontent.com/esp8266/Arduino/master/libraries/ESP8266WiFi/src/ESP8266WiFiSTA.cpp). Se usaron para fundamentar la adaptación, no como prueba del firmware instalado.

## Comprobación física pendiente para una carga autorizada

1. Arrancar AP, abrir la Web y buscar redes; comprobar nombre/IP del AP y acceso al completar o fallar el barrido.
2. Elegir red protegida y abierta, ingresar credenciales, guardar AP-STA y confirmar nuevo boot, modo, SSID e IP STA. Probar también clave incorrecta sin anunciar éxito.
3. Configurar SSID/clave propios del AP, reiniciar AP y volver a conectarse. Confirmar que no cambió la red STA guardada.
4. Conectar STA, reiniciar AP y consultar última red; verificar nombre y clave, máscara inicial, cierre y vencimiento. Revisar caso sin historial y legado sin orden confirmado.
5. Repetir desconexión/reintento, STA explícito, AP-STA y escaneo con un segundo cliente. No se ejecutaron estos pasos en esta entrega.
