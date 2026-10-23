# AutoMicro: explicación detallada de contratos y choques

**Fecha de la fotografía:** 2026-09-29. **Estado:** lectura estática del código de cuatro repositorios. No se compilaron proyectos, ejecutaron tests, conectaron placas ni observaron paquetes reales. Este documento amplía la [línea de base](2026-09-29-contract-baseline.md); describe lo que el código intenta hacer y distingue un choque confirmado de un efecto de ejecución probable.

## 1. Cómo leer este documento

Cada repositorio cumple un papel distinto:

| Clave | Repositorio y revisión | Qué controla |
| --- | --- | --- |
| **F4** | [FirmwareF4](https://github.com/kobachajchir/ProyectoAutoMicro2026_F411CEU6/tree/061a1b2f6bb4) · `061a1b2f6bb4` | El STM32: handlers UNER, permisos locales, movimiento, OLED, estado físico y códigos de pantalla. |
| **ESP** | [FirmESP01MicroArduino2025](https://github.com/kobachajchir/FirmESP01MicroArduino2025/tree/61a1ab9fd3b8) · `61a1ab9fd3b8` | El servidor HTTP/WebSocket, la sesión Web, el broker UART, la cola y las respuestas JSON. |
| **Web** | [WebESP01AutoMicro2025](https://github.com/kobachajchir/WebESP01AutoMicro2025/tree/7aac41cd1876) · `7aac41cd1876`, con cambios locales de interfaz | El cliente JSON, sus promesas, las pantallas y los controles que ve el usuario. |
| **Qt** | [AutoMicroQtDesktop](https://github.com/kobachajchir/AutoMicroQtDesktop/tree/bfe316000c1c) · `bfe316000c1c` | El cliente de escritorio UNER por USB y su catálogo/codec. También contiene un bridge WebSocket local que no está montado en la ventana principal inspeccionada. |

Una referencia como `ESP:src/ws_json_bridge.cpp:7020` indica ruta y línea dentro del repositorio nombrado; los enlaces de la tabla fijan las revisiones utilizadas. Los cambios locales de Web no se tocaron. `Implementado` significa que hay un recorrido ejecutable en el código; `declarado` significa que solo hay ID, metadata o documentación; `compatible` requeriría comprobar además respuesta y efecto en ejecución. No se presume compatibilidad física a partir de estas lecturas.

### 1.1 Dos caminos, dos formas de correlación

`Navegador → ESP → F4` utiliza JSON WebSocket v1 hasta ESP y UNER v2 por UART1 hasta F4. `Qt → F4` utiliza UNER v2 directamente por USB CDC. F4 es nodo `0x1`, Qt `0x2`, broker ESP `0x3` y las sesiones Web pueden recibir nodos `0x5..0xC`. ESP asigna el nodo Web; el navegador no lo elige. Aunque hay ocho valores de nodo previstos, el servidor ESP actual limita a una sesión Web activa. F4 enruta el nodo Qt a USB y el broker a UART. Fuentes: `F4:Core/Src/uner_app.c:1591-1645`; `F4:Docs/integration_handoff_esp_web/F4_WIRE_CONTRACT.md:7-50`; `ESP:docs/integration/ESP_WS_COMMAND_CONTRACT_V1.md:54-72`.

Un frame UNER tiene `UNER`, `payload_len:u8`, `:`, versión `2`, ruta `src/dst` en un byte, ID, payload y XOR. Ocupa `10 + payload_len` bytes; el payload máximo es 255. Los enteros multibyte van little endian. **No lleva `requestId`.** Una respuesta se relaciona con el comando pendiente por ruta e ID. ESP puede encolar solicitudes Web, pero mantiene una sola transacción F4 en vuelo; Qt `RequestManager` mantiene una pendiente por instancia. Si llega una respuesta con otro ID, no es la final de esa solicitud. Fuentes: `F4:Docs/integration_handoff_esp_web/F4_WIRE_CONTRACT.md:52-89`; `ESP:src/uart_transaction_manager.c:300-342`; `Qt:src/services/RequestManager.cpp:12-40,51-103`.

En Web la solicitud lleva `{"api":1,"type":"request","requestId":"...","command":"...","args":{}}`. ESP puede emitir primero `requestAccepted` con `queued/accepted`; **eso solo confirma la cola**. El final es `type:"response"` con `ok:true` y `data`, o `type:"error"` con `code/message/details`. `EspClient` mantiene la promesa pendiente hasta ese final, comprueba `requestId` y `command` y aplica timeout. Por defecto son 10 s; el commit OLED usa 20 s. Fuentes: `ESP:src/ws_json_bridge.cpp:632-710,2859-2962`; `Web:src/protocol/wsApi.ts:1-164`; `Web:src/protocol/espClient.ts:35-155`; `Web:src/features/oledEditor/useOledCanvasTransfer.ts:190-192`.

### 1.2 ACK, NACK, respuesta y error no son equivalentes

F4 puede devolver `ACK 0xE0 [comando_original,status]`, `NACK 0xE1 [comando_original,status]` o un frame con el **mismo ID de comando** y payload específico. Algunos comandos tienen ACK intermedio seguido de respuesta por el mismo ID. Un comando o longitud desconocidos pueden ser descartados por F4 sin NACK: el timeout también forma parte del contrato. El significado de `status` depende de cada comando. ESP traduce el resultado UNER al sobre JSON; Qt usa su `CompletionPolicy` (`Ack`, `SameCommand`, `AckThenSameCommand` o `None`). Un catálogo equivocado puede producir timeout aunque F4 haya ejecutado la operación. Fuentes: `F4:Core/Src/uner_handle.c:578-630`; `F4:Core/Src/uner_app.c:3082-3115`; `ESP:src/f4_command_registry.c:20-99`; `Qt:src/protocol/CommandCatalog.cpp:48-91`.

### 1.3 Autorización en capas

ESP distingue sesión WebSocket activa, privilegios propios del servidor y sesión STM autenticada tras PIN. F4 vuelve a verificar nodo de origen, transporte, PIN local en curso y autorización de ese nodo. Qt tiene su propia sesión de origen `0x2` y no hereda la sesión del navegador. Por eso `requestAccepted` tampoco prueba que F4 haya autorizado la acción. Fuentes: `ESP:src/ws_json_bridge.cpp:2890-2954`; `F4:Core/Src/uner_app.c:1030-1100`; `Qt:src/services/F4ApiCodec.cpp:401-470`.

## 2. Choque A: dos IDs para reiniciar en un perfil

### Intención del comando

La opción de perfil de extensión elige el pinout que cargará F4 en el próximo arranque: NRF24 (`profileId=0`) o beeper PB12 (`profileId=1`). Se puede **seleccionar** sin reiniciar (`0x74`) o **seleccionar y reiniciar**. La operación de reinicio recibe un byte `profileId`; su final previsto es ACK del mismo comando.

| Repositorio | Implementación encontrada |
| --- | --- |
| **F4** | `wifi_bridge_types.h` define `GET_EXTENSION_PIN_MAP=0x75` y `REBOOT_INTO_PROFILE=0x76`. `uner_app.c` registra y ejecuta `0x76`. `0x75` solo tiene definición: no aparece como handler local de mapa de pines. |
| **ESP** | `uner_cmdspec.h` define `REBOOT_INTO_PROFILE=0x75`; `f4_command_registry.c` marca `0x75` implementado y `0x76` reservado. `F4_ExtensionProfile_BuildRebootRequest` usa el enum viejo; `stm.extensionProfile.reboot` toma ese request y lo encola. |
| **Web** | `wsApi.ts` ofrece el nombre JSON `stm.extensionProfile.reboot`. No construye bytes UNER: delega a ESP y espera su respuesta JSON. |
| **Qt** | `UnerTypes.h` y `F4ApiCodec.cpp` usan `0x76`, igual que F4. El catálogo reserva `0x75` para el mapa de pines declarado, todavía sin payload estable. |

**Choque concreto:** Web pide `stm.extensionProfile.reboot` → ESP envía `0x75 [profileId]` → F4 no tiene un handler local para `0x75` → el pedido puede acabar por timeout. ESP también anuncia `hello.features.extensionProfiles=true` consultando **su propio registro**, que considera válido `0x75`; esa bandera no prueba que la F4 instalada comparta el ID. Cambiar solo la constante Web no serviría: Web no codifica el ID. Fuentes: `F4:Core/Inc/wifi_bridge_types.h:340-345`; `F4:Core/Src/uner_app.c:343-348,1122-1127`; `ESP:src/uner_cmdspec.h:89-94`; `ESP:src/f4_command_registry.c:91-97`; `ESP:src/f4_extension_profiles.c:32-46`; `ESP:src/ws_json_bridge.cpp:4126-4155,4538-4543,6838-6846`; `Web:src/protocol/wsApi.ts:32-37`; `Qt:src/protocol/UnerTypes.h:94-99`.

**Decisión sugerida:** conservar `0x75` para el futuro mapa de pines y `0x76` para reinicio, porque F4 y Qt ya coinciden ahí. ESP tendría que actualizar enum, registro, tabla local y rutas de construcción/recepción. Este cambio debe coordinarse con el choque B.

## 3. Choque B: F4 reinicia por `0x76` pero confirma `0x19`

### Qué hace F4 exactamente

`UNER_App_HandleRebootIntoProfile` guarda el perfil pedido y llama a `UNER_App_HandleResetMcu(packet)`. El segundo handler detiene y desarma el movimiento, prepara un ACK y programa el reset diferido. Pero llena el ACK con `payload[0] = UNER_CMD_ID_RESET_MCU`: siempre `0x19`, aunque el paquete original fuera `0x76`. Para UART espera a que termine TX; para USB exige DTR activo y que no quede TX pendiente. El reset se inicia después de esa espera. Si guardar el perfil falla, devuelve NACK; si el reset ya está pendiente, puede devolver NACK busy. El guardado precede a esa comprobación de busy, otro detalle a considerar al implementar la corrección. Fuentes: `F4:Core/Src/uner_app.c:590-635,3043-3078,4596-4605`.

`Solicitud: cmd=0x76, payload=[profileId]`  
`Respuesta esperada: cmd=0xE0, payload=[0x76,0]`  
`Respuesta construida por F4: cmd=0xE0, payload=[0x19,0]`

| Repositorio | Qué ocurre con ese ACK |
| --- | --- |
| **F4** | Lo emite y, si la transmisión se acepta y se cumplen las condiciones del transporte, se reinicia. Para F4 el envío del ACK `0x19` fue exitoso. |
| **ESP** | Hoy falla antes, porque envía `0x75` (choque A). Si se arreglara solo ese ID, `UartTransactionManager` compararía `payload[0]` con el comando pendiente `0x76` y clasificaría `[0x19,0]` como no correlacionado; la respuesta JSON final podría faltar. |
| **Web** | Espera el resultado final de ESP. Puede ver timeout o desconexión pese a que F4 haya aplicado el perfil y reiniciado. |
| **Qt** | Ya envía `0x76`. `RequestManager` también compara el primer byte del ACK con `pendingCmd_=0x76`; deja pasar el ACK `0x19` como evento ajeno. Qt puede informar timeout o desconexión mientras F4 efectivamente se reinicia. |

Fuentes de correlación: `ESP:src/uart_transaction_manager.c:300-342`; `Qt:src/services/RequestManager.cpp:51-81`. **Es una violación del propio formato general de ACK**, que identifica al comando original. La corrección mínima es construir `[packet->cmd,status]` en el reset reutilizado. Conviene decidir además qué reportar si el perfil se guardó pero el reset no llegó a programarse.

## 4. Choque C: seleccionar perfil `0x74` completa en Web y expira en Qt

F4 `UNER_App_HandleSetExtensionProfile` recibe un byte: acepta perfiles 0 o 1, escribe preferencia y envía **respuesta por el mismo ID `0x74`** con un solo byte. `0` significa guardado; `2`, perfil inválido; `3`, fallo de persistencia. No envía ACK como final. Este comando no reinicia: el perfil se aplica al siguiente arranque. Fuente: `F4:Core/Src/uner_app.c:4576-4594`.

ESP cataloga `0x74` como `SAME_COMMAND`, espera el frame `0x74 [status]`, interpreta el status y responde al navegador por `stm.extensionProfile.set`. Web consume ese resultado JSON. Qt codifica correctamente `0x74 [profileId]`, pero `CommandCatalog::completionFor` lo clasifica como `Ack`. `RequestManager` recibe el frame de mismo ID, no lo considera final para una solicitud configurada como ACK, y termina por timeout. `F4ApiCodec` contiene un decoder para el byte de status, pero el flujo normal no llega a entregárselo como resultado de la solicitud. Fuentes: `ESP:src/f4_command_registry.c:92`; `ESP:src/ws_json_bridge.cpp:4126-4155,5550-5570`; `Web:src/protocol/wsApi.ts:32-37`; `Qt:src/protocol/CommandCatalog.cpp:60-75,521-531`; `Qt:src/services/F4ApiCodec.cpp:701-728,1288-1294`; `Qt:src/services/RequestManager.cpp:79-103`.

**Efecto:** Qt puede decir «tiempo agotado» después de que F4 haya guardado el perfil. La salida correcta para Qt es esperar `SameCommand` y evaluar el byte `status` antes de mostrar éxito.


## 5. Choque D: un ACK puede significar «acción rechazada»

### Qué intenta proteger F4

Los comandos `0x53..0x58` representan click de menú, botón de encoder, botón USER, cambio de página y rotaciones. El request incluye el `screenCode:u32 LE` que el cliente cree estar viendo; algunos agregan un byte de índice, tipo de pulsación o dirección. F4 compara el código con la pantalla actual **antes** de despachar el evento. Su handler devuelve `ACK [cmd,status]` aun cuando la acción no se aplicó:

| Status | Significado en estas acciones |
| --- | --- |
| `0` | Acción aceptada y consumida. |
| `2` | La pantalla actual ya no coincide con la enviada. |
| `3` | Argumento, pantalla o estado inválido para esa acción. |
| `4` | El evento llegó al router de UI, pero ningún handler lo consumió. |

Además, las guardas previas pueden devolver **NACK**, por ejemplo `7` sesión requerida, `8` PIN local activo, `9` nodo de origen inválido o `5` transporte no soportado. Estos últimos son rechazos de acceso/transporte; `ACK [cmd,2]` es una respuesta funcional del handler y tampoco significa éxito. Fuentes: `F4:Core/Inc/uner_app.h:43-48`; `F4:Core/Src/uner_app.c:1030-1100,1360-1426,3160-3172`.

| Repositorio | Interpretación |
| --- | --- |
| **F4** | Envía el status específico con `UNER_App_SendCommandStatus`. No repite necesariamente el screen code en el ACK. |
| **ESP** | `complete_response_from_manual_status` pone `ok = (status == 0)`. Para un status no cero arma un sobre JSON `type:"error"` y conserva el número en `details.status`/`details.stmCode`. Puede programar un refresh de pantalla. |
| **Web** | `EspClient` rechaza la promesa de ese request. `getStmRemoteActionFeedback` distingue pantalla distinta, estado inválido, acción no consumida, PIN vencido y otros rechazos; `ScreenStreamWorkspace` muestra el mensaje y vuelve a consultar pantalla cuando corresponde. |
| **Qt** | `RequestManager` reconoce el ACK y `RequestBroker` emite `responseReady` para todo ACK con política `Ack`, sin comprobar si `status` es cero. `ScreenController` finaliza la acción como aceptada cuando recibe `responseReady`. |

Ejemplo: Qt envía click `0x53` con pantalla vieja; F4 contesta `0xE0 [0x53,0x02]`. Web, usando ESP, muestra «la pantalla cambió» y resincroniza. Qt puede dar por completado el click con `status:2` en los datos y no recorrer la ruta de error/resincronización. Fuentes: `ESP:src/ws_json_bridge.cpp:5475-5505`; `Web:src/protocol/espClient.ts:88-125`, `src/utils/stmRemoteActionFeedback.ts:1-116`, `src/components/ScreenStreamWorkspace.tsx:350-382`; `Qt:src/communication/CommunicationHub.cpp:88-99`, `src/controllers/ProtocolControllers.cpp:230-259`.

**Decisión sugerida:** para acciones de UI, Qt debe conservar el status visible pero tratar `status != 0` como resultado rechazado y solicitar pantalla real, igual que Web. No debe equiparar «llegó un ACK» con «se ejecutó la intención del usuario».

## 6. Choque E: los tres emisores IR existen en F4 y no en sus consumidores

F4 añadió tres controles independientes para emisores IR: objetos, línea y luces. `GET_IR_EMITTERS 0x79` no recibe payload y devuelve tres bytes `[objects,line,lights]`. `SET_IR_EMITTERS 0x7A` recibe esos tres bytes; cualquier valor no cero se interpreta como habilitado. Responde por `0x7A` con cuatro bytes `[status,objects,line,lights]`; el handler observado usa `status=0` y reporta el estado real leído después de aplicar. A continuación emite `IR_EMITTERS_CHANGED 0x98 [objects,line,lights]`. Envía el evento a Qt por USB si hay DTR y por UART broadcast si ESP está conectado. Fuentes: `F4:Core/Inc/wifi_bridge_types.h:344-350`; `F4:Core/Src/uner_app.c:347-348,955-980,1128-1154`.

| Repositorio | Situación actual |
| --- | --- |
| **F4** | Registra GET, SET y evento; los handlers leen/cambian el estado de los tres emisores. Este es el productor adelantado. |
| **ESP** | `f4_command_registry.c` reserva `0x79` y `0x7A`, y no registra `0x98`. `start_stm_request` rechazaría esos comandos como `unsupported_by_f4`. Al recibir `0x98` de F4, `WSJsonBridge_OnUNERPacket` lo descarta en el filtro inicial de comandos no implementados: no llega ni siquiera a `stm.event`. |
| **Web** | No define nombres de API, tipos de payload, controles ni handler de `0x98` para estos emisores. Su IR de sensores (`0x6A/0x91`) es otro contrato: muestras de receptores, no habilitación de emisores. |
| **Qt** | `UnerTypes`, `CommandCatalog` y `F4ApiCodec` no nombran `0x79/0x7A/0x98`. El transporte USB puede recibir bytes, pero no hay operación o evento semántico listo en la interfaz. |

**Choque concreto:** una función que existe en el productor físico no es operable desde Web y no se comunica a los otros clientes. Si se integra, el contrato debe fijar orden de los tres bytes, valores admitidos, política de respuesta (el handler F4 observado envía respuesta del mismo ID), significado de `status`, autorización y destino del evento. El registro F4 marca `0x7A` con flags ACK y RESP, pero su handler mostrado no envía un ACK separado; conviene documentar el flujo observado de respuesta por `0x7A`, o cambiar deliberadamente ambos lados. Fuentes de descarte: `ESP:src/f4_command_registry.c:96-124`, `src/ws_json_bridge.cpp:2890-2930,7020-7042,7148-7160`.

## 7. Choque F: la Web conoce menos screen codes que F4

### Identidad, reporte y render son tres operaciones distintas

`screenCode` es un `u32` de identidad con bytes conceptuales `0xMMSSPP`: menú, submenú y página. F4 lo escribe little endian en el payload. `GET_CURRENT_SCREEN 0x52` devuelve por lo menos cuatro bytes de código y un byte `source`; puede anexar selección de menú, progreso de notificación o estado del dashboard. Sus longitudes válidas son `5`, `7`, `9` o `13..45`. Los eventos `SCREEN_CHANGED 0x95` (`5/9` bytes) y `MENU_SELECTION_CHANGED 0x96` (`7`) mantienen a los clientes informados. `source` identifica menú, render, notificación, permiso o sistema. Fuentes: `F4:Core/Inc/screen_codes.h:18-35`; `F4:Core/Src/uner_app.c:1259-1318,3020-3040`; `ESP:src/f4_command_registry.c:57-61,119-122,208-220`.

ESP decodifica el reporte y lo emite como `screen.current` o `screen.changed`; no dibuja la OLED. Web `ScreenContext` conserva el código y los datos; `screenRenderRegistry` intenta encontrar un builder exacto o identificado por código. Si devuelve `null`, `ScreenStreamWorkspace` presenta «Sin comandos para renderizar». Qt mantiene un modelo numérico/semántico de pantalla para controles USB; no es la réplica gráfica Web. Fuentes: `ESP:src/ws_json_bridge.cpp:5320-5425,5450-5473,7078-7115`; `Web:src/contexts/ScreenContext.tsx:135-176`, `src/screens/screenRenderRegistry.ts:337-353,505-570`, `src/components/ScreenStreamWorkspace.tsx:873-881`; `Qt:src/services/F4ApiCodec.cpp:1415-1450`.

La comparación de constantes directas encontró **139 en F4 y 122 en Web**. Las 122 con el mismo nombre tienen el mismo valor. Estas 17 faltan en la lista Web:

| Screen code | Nombre F4 | Qué representa |
| --- | --- | --- |
| `0x010202` | `SCREEN_CODE_CORE_FAVORITES` | Favoritos del menú principal. |
| `0x02020E` | `SCREEN_CODE_CONNECTIVITY_OSK_WIFI_SSID` | Teclado para SSID WiFi. |
| `0x02020F` | `SCREEN_CODE_CONNECTIVITY_OSK_WIFI_PASSWORD` | Teclado para contraseña WiFi. |
| `0x020210` | `SCREEN_CODE_CONNECTIVITY_OSK_WIFI_IPV4` | Entrada IPv4 WiFi. |
| `0x030203` | `SCREEN_CODE_SENSORS_IR_AMBIENT` | Vista de IR ambiente. |
| `0x030600` | `SCREEN_CODE_SENSORS_KEYBOARD_MENU` | Menú de pruebas de teclado. |
| `0x030601` | `SCREEN_CODE_TEST_KEYBOARD_ASCII` | Prueba de teclado ASCII. |
| `0x030602` | `SCREEN_CODE_TEST_KEYBOARD_NUMERIC` | Prueba de teclado numérico. |
| `0x050204` | `SCREEN_CODE_SETTINGS_ABOUT_BOOTLOADER` | Información de bootloader. |
| `0x050419` | `SCREEN_CODE_SETTINGS_PREFS_IR_MENU` | Menú de emisores IR. |
| `0x05041A` | `SCREEN_CODE_SETTINGS_PREFS_IR_OBJECTS` | Preferencia del emisor de objetos. |
| `0x05041B` | `SCREEN_CODE_SETTINGS_PREFS_IR_LINE` | Preferencia del emisor de línea. |
| `0x05041C` | `SCREEN_CODE_SETTINGS_PREFS_IR_LIGHTS` | Preferencia del emisor de luces. |
| `0x050500` | `SCREEN_CODE_SETTINGS_SYSTEM_MENU` | Menú de sistema. |
| `0x050501` | `SCREEN_CODE_SETTINGS_SYSTEM_BOOTLOADER_CONFIRM` | Confirmación de bootloader. |
| `0x090101` | `SCREEN_CODE_INPUT_KEYBOARD_ALPHA` | Entrada alfabética genérica. |
| `0x090102` | `SCREEN_CODE_INPUT_KEYBOARD_NUMERIC` | Entrada numérica genérica. |

No aparecieron referencias a esos nombres ni a esos literales en los módulos de pantallas Web inspeccionados. **La recepción del código numérico y su representación visual son capacidades distintas**: F4 y ESP pueden informar correctamente `0x05041A` mientras la Web no sabe dibujarlo. La documentación Web de inventario afirma una cobertura comprobada por un test histórico; no ejecutamos ese test ni lo usamos como prueba del checkout actual. Fuentes: `F4:Core/Inc/screen_codes.h:36-197`; `Web:src/screens/screenCodes.ts`, `src/screens/SCREEN_INVENTORY.md:22-25`.

**Decisión sugerida:** sincronizar primero el catálogo de identidad. Para cada uno de los 17, decidir si el evento disponible trae datos suficientes para un builder exacto o si se necesita un builder identificado y honesto que no invente valores dinámicos. La corrección de pantalla es independiente del control de emisores IR, aunque cuatro códigos nuevos pertenezcan a su menú.

## 8. Choque G: F4 pide assets por UNER `0x5A` y ESP no los sirve

`UNER_App_RequestBinaryAsset(asset_id)` envía un request de **un byte** a ESP. F4 prepara un buffer de hasta 1024 bytes y espera uno o varios paquetes `0x5A`. Cada chunk lleva un encabezado packed de nueve bytes: `status:u8, asset_id:u8, width:u8, height:u8, total_len:u16, offset:u16, data_len:u8`, seguido de los datos. El receptor comprueba longitud total, offset y tamaño de chunk, copia bytes al buffer y declara el asset listo cuando el mayor offset final alcanza `total_len`. Fuentes: `F4:Core/Inc/uner_app.h:15-31,140-141`; `F4:Core/Inc/wifi_bridge_types.h:374`; `F4:Core/Src/uner_app.c:2023-2050,2950-3015`.

ESP conserva el ID en sus tablas, pero el registro actual lo marca `implemented=false`, sin proveedor de chunks. Web obtiene sus recursos de sitio por HTTP/LittleFS, que no responde una petición UNER de asset enviada por F4. Qt declara el ID como parcial/de entrada, sin convertir a Qt en el proveedor que F4 espera. Es un flujo **diferente** de `OLED_CANVAS_TRANSFER 0x6D`: allí Web sube un framebuffer a F4; aquí F4 solicita a ESP un asset por ID. Fuentes: `ESP:src/f4_command_registry.c:66`, `src/uner_cmdspec.c:49`; `Qt:src/protocol/CommandCatalog.cpp:125-150,397-402`.

Hay además un detalle interno que conviene resolver si se implementa el proveedor: el documento F4 dice que los chunks deben ser contiguos y ordenados, pero el receptor mostrado usa el **máximo offset alcanzado** para decidir que el asset está listo; no lleva un mapa de cobertura de todos los offsets. Un último chunk recibido prematuramente podría dejar huecos en el buffer y aun así marcarlo listo. Esto es una diferencia entre documentación e implementación, todavía latente mientras ESP no entregue el asset. Fuente: `F4:Docs/integration_handoff_esp_web/F4_WIRE_CONTRACT.md:770-778` frente a `F4:Core/Src/uner_app.c:2994-3011`.

**Decisión pendiente:** implementar en ESP una fuente UNER de assets y definir integridad/orden, o retirar/desactivar la expectativa `0x5A` de F4. Servir imágenes al navegador por HTTP no resuelve la petición del firmware.

## 9. Choque H: `0x61/0x6B` aceptan longitudes diferentes

`SET_MPU_STREAM 0x61` y `SET_IR_STREAM 0x6B` reciben un byte de activo/inactivo y opcionalmente `period_ms:u16 LE`. Con un byte, F4 usa período por defecto; con tres, lee los dos bytes del período y lo limita al rango permitido. Su tabla de entrada, sin embargo, acepta **1, 2 o 3** bytes, y el handler solo lee el período si hay al menos tres. Con **dos** bytes acepta la solicitud e ignora el segundo, usando el valor por defecto. La respuesta por el mismo ID contiene cuatro bytes: `[status,active,period_ms:u16 LE]`. Fuentes: `F4:Core/Src/uner_app.c:380-391,3904-3935,4076-4113`.

ESP codifica el contrato como **exactamente 1 o 3** y rechaza dos bytes con `invalid_request` antes de mandar UART. Los builders Web normales usan las formas aceptadas por ESP. Qt presenta rango `1..3` y su consola de protocolo permite enviar manualmente dos por USB; F4 los aceptaría y usaría el período por defecto. Este choque importa para clientes manuales y para escribir una especificación única: `2` hoy significa «aceptado pero byte extra ignorado» en F4 y «request inválido» en ESP. Fuentes: `ESP:src/f4_command_registry.c:199-207`; `Qt:src/protocol/CommandCatalog.cpp:430-438,480-488`, `src/controllers/ProtocolControllers.cpp:43-109`.

Otros catálogos expresan rangos amplios donde el protocolo tiene longitudes discretas, por ejemplo `0x51` admite 6 o 10 y `0x6D` 3/12/22/118. Los flujos guiados/especializados observados construyen esas formas concretas; no se atribuye otro fallo de uso normal solo por una descripción de rango. **Decisión sugerida:** hacer que F4 rechace longitud 2 o documentarla explícitamente; preferimos rechazo, ya que el byte ignorado crea una falsa impresión de que el período se aplicó.


## 10. Diferencia I: Qt contiene un bridge WebSocket que no es su ruta principal actual

Qt incluye `WebApiContract`, `F4ApiCodec`, `UsbWebSocketBridge`, `LocalWebAppServer` y `WebAppPage`. Ese conjunto puede aceptar envelopes JSON de forma similar a ESP, codificar UNER y hablar con F4 por USB. Incluso contiene una máquina de transferencia OLED Canvas. No es código ficticio: los archivos existen y están en el proyecto. Fuentes: `Qt:CMakeLists.txt:58-65`; `Qt:src/services/UsbWebSocketBridge.cpp:120-190,190-255,405-538`; `Qt:src/services/LocalWebAppServer.cpp:23-55`; `Qt:src/ui/pages/WebAppPage.cpp:38-97`.

El `MainWindow` inspeccionado registra Home, Telemetry, Route Planning, Network, Configuration, System, Protocol y Terminal mediante `CommunicationHub`. **No instancia `WebAppPage`** en ese recorrido. El cliente Qt operativo que se puede trazar desde esa ventana es `RequestBroker → RequestManager → UnerSession → USB CDC → F4`. Por eso la respuesta de `UsbWebSocketBridge` no debe confundirse con lo que ve hoy el usuario de las páginas Qt montadas. En particular, el bridge local sí utiliza `F4ApiCodec::responseIsSuccess` para rechazar algunos status no cero, pero la ruta activa `RequestBroker` de acciones de pantalla tiene el comportamiento del choque D. Fuentes: `Qt:src/ui/MainWindow.cpp:38-90`; `Qt:src/communication/CommunicationHub.cpp:68-115,119-175`; `Qt:src/services/UsbWebSocketBridge.cpp:600-635`.

| Repositorio | Relación con esta diferencia |
| --- | --- |
| **F4** | Acepta USB CDC desde nodo Qt y sirve de destino común. |
| **ESP** | Es el backend WebSocket usado por el navegador en el recorrido Web normal. |
| **Web** | Su cliente API v1 se comunica con el servidor al que apunte la URL; no se debe afirmar equivalencia funcional con Qt solo por tener el mismo formato de envelope. |
| **Qt** | Posee el bridge alternativo, pero no hay evidencia de que la ventana principal lo active. |

**Decisión:** si se quiere usar Web dentro de Qt, primero hay que integrar explícitamente la página/servidor y comparar sus nombres de comando, seguridad, capacidades, timeouts y eventos con ESP. Si no se piensa usar, etiquetar el bridge como experimental o no integrado para evitar dos supuestas autoridades de API.

## 11. Diferencia J: ESP mantiene dos catálogos UNER con roles distintos

`f4_command_registry.c` es el contrato que consulta el bridge JSON para decidir si un comando puede salir hacia F4: propietario, implementación, longitud, respuesta, timeout, idempotencia y privilegio. `uner_cmdspec.c` alimenta la tabla del dispatcher UNER local del ESP. En la recepción, el `packet_interceptor` del bridge corre **antes** que la búsqueda del handler local; por eso las dos tablas no significan necesariamente que un request Web vaya a ser validado dos veces en serie. Fuentes: `ESP:src/f4_command_registry.c:20-29,199-222`; `ESP:src/uner_app_handlers.cpp:1629-1696`; `ESP:src/uner_handle.cpp:112-146`; `ESP:src/ws_json_bridge.cpp:7020-7042`.

Ejemplos de deriva:

| ID | Registro de bridge F4 | Tabla local/enum ESP |
| --- | --- | --- |
| `0x75/0x76` | Reinicio en `0x75`, `0x76` reservado: ambas difieren de F4. | Enum y tabla local también usan `0x75`. Hay que mover ambas al corregir. |
| `0x51` | Request de 6 **o** 10 bytes. | Entrada local de 6 bytes. El tráfico Web/F4 pertinente pasa por interceptor; no se infiere un fallo de login actual solo por esto. |
| `0x52` | Request vacío; respuesta F4 válida de 5/7/9/13..45. | Tabla local declara 0..45 como entrada. |
| `0x5A` | Marcado no implementado. | El ID persiste en la tabla, pero no hay handler que provea el asset. |
| `0x4F/0x50` | Reservados o fuera del contrato vigente. | Enum conserva `BOOT_COMPLETE` y `NETWORK_IP` heredados; sus eventos vigentes están en `0x94/0x93`. |
| `0x79/0x7A/0x98` | Sin contrato nuevo. | Tampoco se integraron al dispatcher local. |

Esta diferencia es **riesgo de mantenimiento**, no prueba de que todas esas rutas fallen hoy. La decisión recomendable es definir qué tabla es autoridad de cada dirección (`Web→F4`, `F4→ESP`, eventos, ESP local), eliminar entradas obsoletas cuando no haya compatibilidad que preservar y revisar ambos puntos al incorporar un comando nuevo.

## 12. Diferencia K: `ECHO 0x42` no devuelve un eco

Un nombre `ECHO` suele sugerir que el receptor devuelve exactamente los bytes enviados. El handler F4 observado, en cambio, muestra una notificación OLED de «comando recibido» y sale sin mandar frame de respuesta. ESP lo clasifica con política `NONE` para requests a F4. Qt también usa política `None` aunque conserva un decoder y una descripción histórica que mencionan variantes ESP que podían responder. Web no ofrece un comando tipado habitual para este diagnóstico; podría existir acceso raw bajo privilegios, que no cambia la semántica F4. Fuentes: `F4:Core/Src/uner_app.c:1155-1158`; `ESP:src/f4_command_registry.c:43`; `Qt:src/protocol/CommandCatalog.cpp:64-77,286-290`; `Web:src/protocol/wsApi.ts:1-40`.

No hay aquí un fallo actual demostrado de Web/Qt: los catálogos actuales ya saben que no deben esperar respuesta F4. El choque es entre **nombre o textos antiguos** y el efecto implementado. Para evitar una expectativa falsa, conviene documentarlo como «mostrar aviso diagnóstico» o definir en una futura versión una respuesta de eco real, con ID/payload/política explícitos.

## 13. Diferencia L: los documentos de handoff no son una versión viva del código

`F4_WIRE_CONTRACT.md` y `ESP_WS_COMMAND_CONTRACT_V1.md` fijaron una fecha de corte anterior a varias incorporaciones. Web conserva `FIRMWAREF4_UNER_CONTRACT_SNAPSHOT.md` como copia de referencia. Son útiles para entender rutas, longitudes y motivos históricos, pero no cubren por igual el código posterior de OLED Canvas, boot, AP, emisores IR y pantallas nuevas. Qt mezcla metadata actual con algunas descripciones `declared only` o `deprecated`. Fuentes: `F4:Docs/integration_handoff_esp_web/F4_WIRE_CONTRACT.md:1-7`; `ESP:docs/integration/ESP_WS_COMMAND_CONTRACT_V1.md:1-40`; `Web:docs/integration/FIRMWAREF4_UNER_CONTRACT_SNAPSHOT.md`; `Qt:src/protocol/CommandCatalog.cpp:125-150`.

| Repositorio | Autoridad cuando documento y código difieren |
| --- | --- |
| **F4** | Handler y emisor de payload del firmware para el comportamiento de F4. |
| **ESP** | Handler/registro/bridge para lo que realmente acepta, responde y descarta el servidor. |
| **Web** | Cliente y componentes para el efecto visible; copiar una especificación no crea un builder. |
| **Qt** | Ruta montada desde `MainWindow` y política efectiva de `RequestManager`, no solo descripción de catálogo. |

La [línea de base](2026-09-29-contract-baseline.md), este detalle y futuras fichas AMI ofrecen una referencia central. **No convierten por sí mismos los cuatro ejecutables en compatibles.** Una ficha nueva corresponde cuando se cambia el contrato de código; esta explicación registra el estado anterior.

## 14. Contratos que sí tienen un recorrido coherente en fuente

Es importante no tratar cualquier diferencia de catálogo como una rotura de todo el sistema:

| Área | Recorrido observado | Límite de la evidencia |
| --- | --- | --- |
| **UNER v2** | F4 y Qt comparten header `UNER`, token, versión, ruta y checksum; ESP usa el mismo formato por UART. | No se capturaron frames de una placa real. |
| **JSON v1** | Web crea envelope `api:1`, ESP lo parsea y devuelve response/error/event; `EspClient` correlaciona finales por `requestId`. | Un cliente externo podría reutilizar IDs: ESP no mantiene una prohibición global de duplicados. |
| **Pantalla actual** | F4 `0x52` y eventos `0x95/0x96` → ESP los convierte → Web conserva identidad; Qt decodifica USB. | Que llegue un screen code no implica que Web pueda dibujarlo: choque F. |
| **Snapshots** | F4 produce MPU 42, IR 56, telemetría 92 y control 64 bytes; ESP fija longitudes y Web tiene decoders; Qt tiene codec/modelos. | La estructura observada en fuente no prueba mediciones físicas correctas. |
| **OLED Canvas `0x6D`** | Web comprueba conexión, sesión, modo TEST y pantalla preparada; envía dos bloques Web de 512 bytes. ESP convierte a BEGIN + diez CHUNK + COMMIT UNER; F4 responde con 10 bytes por fase. Web pide 20 s para el commit; ESP reserva la UART durante la operación. | Es un flujo distinto de assets `0x5A`. No se verificó en esta auditoría la pantalla física ni la secuencia UART real. |
| **Reset general `0x19`** | F4 detiene actuadores, envía `ACK [0x19,0]` y difiere reset hasta salida TX; ESP/Web y Qt tienen operaciones para pedirlo. | El bug de etiqueta del choque B ocurre cuando se reutiliza ese handler para `0x76`. |
| **AP `0x77/0x78/0x4A`** | F4 conserva configuración AP deseada y responde al cliente; ESP aplica su estado NVS/runtime mediante su comando operativo `0x4A`. | Es un flujo de dos propietarios, no un único setter en ESP. |

Fuentes agrupadas: `F4:Core/Src/uner_app.c:333-424,1259-1318,3043-3080,3670-3780`; `ESP:src/f4_command_registry.c:20-124,199-222`, `src/ws_json_bridge.cpp:2859-2962`; `Web:src/protocol/f4Payloads.ts`, `src/features/oledEditor/useOledCanvasTransfer.ts:110-198`; `Qt:src/services/F4ApiCodec.cpp:401-858,1330-1450`.

## 15. Decisiones que conviene consensuar

### 15.1 Correcciones mínimas para que las respuestas signifiquen lo mismo

1. **Perfil, IDs:** conservar `0x75` como mapa de pines aún no implementado y `0x76` para reiniciar. Actualizar enum, registro y tabla ESP; Web puede conservar el nombre JSON. Qt ya manda `0x76`.
2. **Perfil, ACK:** F4 debe etiquetar el ACK de reboot con `packet->cmd`, no siempre `0x19`. Definir el resultado si la preferencia se guarda pero el reset no se programa. Los dos cambios deben desplegarse coordinados: corregir solo ESP revela el segundo fallo.
3. **Perfil, SET:** Qt debe esperar respuesta `0x74 [status]` y no ACK. Mostrar fallo si `status` no es cero.
4. **Acciones remotas:** Qt debe distinguir `ACK [cmd,0]` de `ACK [cmd,2/3/4]` y resincronizar cuando F4 rechaza. Mantener los NACK de permisos como categoría distinta.

Estos son cambios de firmware/aplicación, todavía **no realizados** por este documento. Modificar F4 o ESP requiere recompilar y reflashear sus placas; modificar Qt requiere entregar un ejecutable actualizado. El orden preciso de despliegue se decidirá con la implementación concreta y sus compatibilidades.

### 15.2 Funcionalidades que necesitan decisión de producto

5. **Emisores IR:** decidir si Web y Qt deben controlar los tres emisores y mostrar cambios espontáneos. Si sí, fijar payload/status/evento, registrar `0x79/0x7A/0x98` en ESP y añadir consumidor semántico en cada cliente. F4 ya es el productor.
6. **Pantallas:** decidir si Web debe replicar visualmente las 17 pantallas nuevas o solo mostrar identidad para algunas. No dibujar valores inventados cuando `0x52/0x95` solo informan código y source.
7. **Assets `0x5A`:** implementar un proveedor ESP con integridad por cobertura de chunks, o retirar/desactivar la petición F4. El flujo OLED Canvas `0x6D` no lo sustituye.
8. **Qt como host Web:** integrar el bridge Qt explícitamente o etiquetarlo como módulo no activo; no anunciarlo como alternativa al ESP mientras la ventana principal no lo monte.

### 15.3 Limpieza de contrato

9. Elegir si `0x61/0x6B` de dos bytes se rechaza en F4 o se documenta como forma válida. Rechazarlo evita ignorar un byte silenciosamente.
10. Separar con claridad el catálogo de comandos F4, los comandos locales ESP y los eventos; retirar alias viejos solo cuando se entienda su compatibilidad.
11. Actualizar textos de `ECHO` y snapshots documentales una vez fijada la implementación.

## 16. Qué quedó fuera

No se ejecutaron tests, validadores de la skill, builds, simuladores, capturas USB/UART, WebSocket real ni pruebas físicas. En consecuencia, «puede terminar en timeout» indica una consecuencia razonada desde las políticas de correlación; no es un registro de falla reproducida en placa. El recorrido fue focalizado en contratos compartidos, handlers y consumidores; no afirma cobertura de cada función privada de los cuatro proyectos. Las modificaciones locales preexistentes de Web permanecen intactas. Ningún comando, evento, pantalla ni payload de los cuatro repositorios fue cambiado por esta documentación.

