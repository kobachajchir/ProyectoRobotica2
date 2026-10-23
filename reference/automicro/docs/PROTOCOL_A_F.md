# Contrato de implementaciÃ³n Aâ€“F â€” 2026-10-02

Este documento define los campos de los cambios realizados. No certifica ejecuciÃ³n ni instalaciÃ³n. Productor: FirmwareF4; puente: ESP; consumidores: Web por JSON v1 y Qt por USB UNER. Todos los enteros multibyte y float de UNER son little-endian. Offsets desde el primer byte del payload; CMD no pertenece al payload.

## Envelope y finalizaciÃ³n

UNER v2: cabecera "UNER", longitud payload u8 (mÃ¡ximo 255), token ':' offset5, versión 0x02 offset6, ruta u8 offset7 (src nibble alto, dst bajo), comando u8 offset8, payload y checksum XOR. Total 10 + longitud. UNER no contiene secuencia/requestId; la transacción correlaciona comando/ruta y ACK originalCmd. JSON requestId se mantiene en el puente. ACK E0/NACK E1: offset 0 originalCmd:u8, offset 1 status:u8. Nunca aceptar ACK de 0x19 para una solicitud 0x76. AceptaciÃ³n de cola JSON no es finalizaciÃ³n funcional. No hay una interpretaciÃ³n universal de primer byte status: GET IR comienza con objects.

## Perfiles y reinicio

| CMD | Request exacto | Respuesta final | InterpretaciÃ³n |
|---|---|---|---|
| 0x19 RESET_MCU | vacÃ­o (0 bytes) | E0 [0x19,0] | ACK antes de reset diferido |
| 0x73 GET_EXTENSION_PROFILE | vacÃ­o | 0x73 [status:u8,profileId:u8] (2) | status 0 Ã©xito; profile 0 NRF24, 1 Beeper |
| 0x74 SET_EXTENSION_PROFILE | offset 0 profileId:u8 (1) | 0x74 offset 0 status:u8 (1) | 0 guardado, 2 perfil invÃ¡lido, 3 persistencia fallida; handler inicializa 1 para forma invÃ¡lida |
| 0x75 GET_EXTENSION_PIN_MAP | solo declarado | ninguna forma operativa | No usarlo para reiniciar ni inventar payload |
| 0x76 REBOOT_INTO_PROFILE | offset 0 profileId:u8 (1) | E0 [0x76,0] (2) | persistir y luego reset diferido; errores conservan guardas existentes |

ESP requiere sesiÃ³n STM. Timeouts ESP: 0x19 250 ms; 0x73 500 ms; 0x74 y 0x76 3500 ms. Operaciones SET/reboot no se reintentan automÃ¡ticamente. Para despliegue: compilar/instalar F4 corregido primero, luego ESP con ID 0x76. Qt ya usaba 0x76; Web invoca stm.extensionProfile.reboot y el puente decide el ID. El ACK de reboot confirma aceptaciÃ³n, no que el nuevo firmware haya arrancado.

## Emisores IR

| DirecciÃ³n | CMD | Longitud | Campos y offsets |
|---|---|---|---|
| GET request | 0x79 | 0 | vacÃ­o |
| GET response | 0x79 | 3 | 0 objects:u8; 1 line:u8; 2 lights:u8 |
| SET request | 0x7A | 3 | 0 objects:u8; 1 line:u8; 2 lights:u8 |
| SET response | 0x7A | 4 | 0 status:u8; 1 objects:u8; 2 line:u8; 3 lights:u8 |
| evento | 0x98 | 3 | 0 objects:u8; 1 line:u8; 2 lights:u8 |

Booleans del contrato: 0 apagado, 1 encendido; sin unidades. F4 acepta no-cero como encendido en SET y devuelve 0/1 normalizado. Los consumidores envÃ­an estrictamente 0/1. SET responde por mismo comando; no espera ACK separado. Status 0 Ã©xito; otros valores se rechazan conservando su nÃºmero, no se inventan significados. Evento se emite despuÃ©s de SET por UART cuando ESP estÃ¡ conectado y por USB cuando DTR estÃ¡ activo. Sin evento inicial garantizado: hacer GET al conectar/autenticar. Evento y respuesta comparten estado efectivo, sin transactionId de aplicaciÃ³n; un evento no completa un SET pendiente.

JSON v1 request: `{api:1,type:"request",requestId:string,command:string,args:object}`. GET command `stm.irEmitters.get`, args exactamente `{}`. SET `stm.irEmitters.set`, args exactamente `{objects:boolean,line:boolean,lights:boolean}`: no campos adicionales, no strings/0/1 JSON. Resultado GET data con esos tres boolean; SET aÃ±ade `status:number`. Evento `stm.irEmitters.changed`, data con tres boolean. Envelope response/event y requestId se mantienen segÃºn wsApi y make_response_base_for_api. Solo sesiones STM autenticadas; evento se filtra por autorizaciÃ³n del cliente. Cola aceptada no permite cambiar visualmente un switch: esperar estado confirmado. ESP timeout 500 ms; GET permite un reintento, SET ninguno. Web deadline total 2000 ms incluye puente/cola/reintento. Qt 500 ms por solicitud. No depender de que todos los clientes reciban eventos si su transporte estÃ¡ inactivo.

## Acciones remotas y reportes

| CMD | Request | FinalizaciÃ³n |
|---|---|---|
| 0x52 | vacÃ­o | mismo comando; formatos abajo |
| 0x53 click | screenCode:u32LE offsets0..3, visibleItem:u8 offset4, total5 | E0 [0x53,status] |
| 0x54 encoder | screenCode4 + pressKind:u8, total5 | E0 [0x54,status] |
| 0x55 USER | screenCode4 + pressKind:u8, total5 | E0 [0x55,status] |
| 0x56 pÃ¡gina | screenCode4 + direction:u8, total5 | E0 [0x56,status] |
| 0x57 giro izquierdo | screenCode:u32LE, total4 | E0 [0x57,status] |
| 0x58 giro derecho | screenCode:u32LE, total4 | E0 [0x58,status] |

pressKind 0 corto,1 largo; direction 0 arriba(-1),1 abajo(+1). visibleItem es posiciÃ³n **visible** 1..3, no Ã­ndice global ni selectedIndex. Legacy F4 acepta request4 en click (activar actual), encoder/USER (corto); ESP usa5. Metadata de pÃ¡gina admite4 pero handler lo rechaza con3: variante no operativa, documentada como discrepancia, no enviada por consumidores.

Status UI: 0 aceptado, 2 pantalla distinta, 3 argumento/estado invÃ¡lido, 4 evento no consumido. Otros no-cero tambiÃ©n fallo funcional. NACK es categorÃ­a separada de rechazo de permisos/transporte. Qt conserva nÃºmero crudo, libera pending y refresca pantalla una vez al rechazar. Status0 puede significar handler invocado sin cambio de pantalla (encoder largo OSK).

0x52 formatos en orden de precedencia: notificaciÃ³n â†’ dashboard â†’ menÃº â†’ base. Base5: offset0 screenCode:u32LE,offset4 source:u8. MenÃº7: base+offset5 selectedIndex:u8 (0-based),offset6 itemCount:u8. NotificaciÃ³n9: base+offset5 remainingMs:u16LE,offset7 totalMs:u16LE. Dashboard13+N: screenCode4, source1, flags1, backend1,carMode1,IPv4[4],ssidLength1,ssid[N]; N0..32, sin terminador. Source: 0 unknown,1 menu,2 render,3 notification,4 permission,5 system.

0x95 SCREEN_CHANGED: base5 o notificaciÃ³n9. 0x96 SELECTION_CHANGED: screenCode:u32LE offset0,selectedIndex:u8 offset4,itemCount:u8 offset5,source:u8 offset6,total7. No contiene posiciÃ³n de teclado, texto editado, modo de teclado ni elecciÃ³n de confirmaciÃ³n. No tratar selectedIndex como visibleItem. Leer SCREEN_BEHAVIOR.md para navegaciÃ³n y guardas.

## Evidencia de cÃ³digo

F4 Core/Src/uner_app.c (metadata, handlers, reset y evento); Core/Src/uner_ui.c y ui_event_router.c; Core/Src/osk.c y osk_integration.c; Core/Inc/screen_codes.h. ESP src/f4_command_registry.c, f4_payload_decoders.c, ws_json_bridge.cpp y uner_cmdspec.*. Qt CommandCatalog.cpp, F4ApiCodec.cpp, CommunicationHub.cpp, ProtocolControllers.cpp, IrEmittersController.* y TelemetryPage.qml. Web wsApi.ts, IrEmittersContext.tsx, IrEmittersControls.tsx y remediatedScreens.ts. Referencias base: F4 061a1b2f6bb4; ESP 61a1ab9fd3b8; Qt bfe316000c1c; Web 7aac41cd1876. Esta entrega modifica esos checkouts: no son snapshots limpios de esos commits.
