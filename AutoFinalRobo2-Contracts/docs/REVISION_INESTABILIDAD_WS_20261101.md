# Revision de inestabilidad al abrir WebSocket

01/11/2026. Pedido: revisar el codigo y buscar la causa de la desconexion y
reconexion repetidas en la primera apertura. El usuario confirma que sucede
desde la IP del ESP, no desde Vite. Revision estatica, sin ejecutar pruebas,
servidores, compilaciones, validadores, monitores, conexiones ni cargas.
No se modifican fuentes Web/ESP/ATmega por esta revision.

## Conclusion y grado de certeza

Hay un mecanismo concreto en las fuentes que puede producir el sintoma:
rafaga inicial → cola de salida llena → cierre del ESP → reconexion rapida →
misma rafaga inicial. Es el principal candidato por codigo y por el momento
descrito del fallo. La existencia de las ramas y sus limites esta comprobada
por lectura; que esa rama haya causado el corte observado sigue sin confirmar.
No se dispone del codigo de cierre del fallo ni de una captura del trafico.
Tampoco se afirma que las fuentes actuales coincidan con el firmware/LittleFS
instalados en el auto.

## Hallazgos prioritarios

### 1. Cierre de la sesion ante presion transitoria de salida

`AutoFinalRobo2-ESP/platformio.ini:22` fija
`WS_MAX_QUEUED_MESSAGES=4`. Es el limite de mensajes de salida de la biblioteca,
distinto de la cola de cuatro peticiones de entrada de `Broker`.

`src/broker.cpp:162`: `answer()` cierra con 1013 / `Slow consumer` si
`client->queueIsFull()` al entregar cualquier respuesta. No espera drenaje,
reserva capacidad para hello/alive ni difiere esa respuesta. La biblioteca
instalada `ESPAsyncWebServer/src/AsyncWebSocket.cpp:435` considera llena la cola
por numero de mensajes o por estado distinto de conectado. Sus colas contienen
tambien mensajes aun pendientes de ACK TCP; no son solo mensajes no iniciados.

`broker.cpp:227`: hello contesta con estado/capacidades/FS y publica identidad
ESP y, si esta disponible, ATmega. El estado periodico se publica cada segundo
(`broker.cpp:404`), y cada alive de 500 ms vuelve a generar estado/capacidades.
Los eventos de arranque/sistema y las respuestas comparten la salida limitada.

Los broadcasts actuales ya omiten eventos si la cola esta llena
(`broker.cpp:377`, `broker_product.cpp:121`). Esa proteccion existe y debe
conservarse, pero no evita que ocupen las cuatro posiciones antes de la proxima
respuesta obligatoria. No hay que confundirlo con una solucion completa de
backpressure. El cierre de una sesion tambien activa `stopWebOperations()`;
esa revocacion de operaciones es una consecuencia de seguridad, no un reinicio.

### 2. Ráfaga de consultas al pasar a ready

`Web/src/features/connection/ConnectionContext.tsx` inicia lectores separados:

| Lector | Apertura / frecuencia |
| --- | --- |
| wifi.info | Inmediato al ready; despues cada 3 s (linea 219) |
| robot.state | Inmediato si Mega ready; despues espera 1 s tras terminar (242) |
| system.info | Inmediato; se repite al cambiar enlace/bootRevision (446) |
| Bateria | Inmediato si capability disponible; despues cada 1 s (385) |
| Suscripcion OLED | En ready, incluso envia disable sin visor (395) |
| Cabezal | Inmediato si disponible; despues cada 100 ms (433) |

`requestCommand()` serializa algunas consultas (348), pero wifi.info,
robot.state, system.info y heartbeat llaman directamente a `client` y eluden
esa cola. Por eso tener `requestTail` no significa que todo el arranque sea
serial. El lector de cabezal esta en el provider global, no condicionado a
tener visible un visor que lo necesite.

Cada `system.boot` incrementa bootRevision (169), incluso si es la misma
identidad consultada al reconectar. Los snapshots ESP/ATmega de hello pueden
rearmar efectos y volver a solicitar datos al principio de la sesion. La carga
es finita por lector, pero se concentra en la ventana mas sensible al abrir.
Los errores por cola de entrada llena son respuestas BUSY; no cierran por si
solos el transporte. La salida de esas respuestas tambien usa `answer()`.

### 3. El backoff no crece si cada intento alcanza ready brevemente

`Web/src/protocol/afr2Client.ts:241` pone reconnectAttempt en cero al completar
hello. `scheduleReconnect()` (298) calcula el siguiente intento desde ese
contador, con base 500 ms y jitter 20%. Una sesion que llega a ready y cae
inmediatamente vuelve a esperar aproximadamente 400..600 ms, sin acumular
backoff de inestabilidad. No es el periodo total de cada ciclo: hay que sumar
los tiempos de apertura, handshake y cierre.

Esto no causa el primer corte; amplifica la repeticion de una condicion de
arranque que siga presente. Restablecer el contador tras una ventana realmente
estable evitaria tratar una conexion de milisegundos como recuperacion completa.

## Otros caminos diferenciados

- Una sola sesion del broker (`broker.h:36`). Otra pestaña/dispositivo recibe
  1013 / `Session limit`, sin reemplazar al cliente vigente. Tambien puede
  afectar una recarga mientras la sesion anterior aun no se libero. Es una
  politica acotada existente; no se propone aumentar sesiones sin presupuesto.
- 1001 / `Heartbeat expired`: broker sin peticiones admitidas por 30 s
  (`broker.cpp:392`). La Web tambien expira despues de 30 s sin mensajes
  validos. No es la explicacion directa de cortes inmediatos tras hello.
- La validacion Origin es same-origin HTTP. El caso Vite sin proxy no explica
  el escenario confirmado de servir la Web desde la propia IP del ESP.
- Hello no llama WiFi.begin/reconnect ni pide reinicios. WifiService tiene
  reintento STA cada 30 s si falta conexion; los cambios de modo/configuracion
  requieren una peticion explicita. Esto no descarta RF, alimentacion o resets
  reales: no hay mediciones de esos factores en esta revision.
- UART es otro enlace. `mega_link.cpp:181` revoca negociacion tras timeout de
  una operacion de 800 ms y `:186` tras 1500 ms sin respuesta buena. El ATmega
  tambien supervisa 1500 ms (`afr2_link.c:18`). Eso puede alternar megaLink y
  capacidades sin cerrar el WS. `ManualDriveOverlay.tsx:43` muestra conectado
  solo si ambos enlaces estan ready: ese indicador no demuestra un cierre TCP.
- La proteccion de generacion y heartbeat unico en vuelo ya existen en
  afr2Client; no se encontro un segundo bucle automatico de connect por renders.
  StrictMode puede repetir efectos en desarrollo, pero no justifica por si
  solo una oscilacion persistente de la Web de produccion del ESP.

## Dato disponible para identificar el camino real

La pantalla Sistema → Informacion de hardware → Diagnostico WebSocket ya
expone el ultimo cierre observado y el ultimo cierre ordenado por ESP
(`SystemPanel.tsx:60`). `system.hello`, alive y status incluyen diagnosticos;
`afr2Client.handleRawMessage()` aplica el estado de respuestas completadas con
megaLink, incluido hello. Por tanto no se atribuye el problema a ausencia de
lectura del diagnostico de hello.

| Evidencia | Interpretacion |
| --- | --- |
| 1013 / Slow consumer | Rama de saturacion de salida |
| 1013 / Session limit | La unica sesion sigue ocupada |
| 1001 / Heartbeat expired | Expiracion de sesion, distinta del arranque inmediato |
| 1006 | Cierre anormal observado por navegador; por si solo no identifica causa |
| WS ready pero Mega stale/disconnected | Revisar UART/AVR; no equivale a corte WS |
| Cambio del bootId ESP | Evidencia de nueva instancia ESP; falta causa fisica/software |

## Correccion indicada por la revision, pendiente de implementar

1. Gestionar presion de salida con prioridad de respuestas/hello/alive y reserva
   de capacidad frente a snapshots opcionales, con limites de memoria explicitos.
   No basta aumentar la cola ni quitar el cierre descartando ACK silenciosamente.
2. Escalonar y deduplicar lecturas iniciales; coordinar las que hoy eluden la
   cola compartida y evitar rearmarlas por identidades de boot ya conocidas.
   Preservar STOP prioritario, epochs y frescura de datos.
3. Reiniciar el contador de backoff despues de estabilidad sostenida, no solo
   por haber terminado hello. Evitar una tormenta de reconexion tras cortes breves.
4. Registrar ocupacion/motivo al cerrar para confirmar el camino en una prueba
   posteriormente autorizada. No debilitar las supervisiones de movimiento/UART.

No se presentan estas recomendaciones como correcciones aplicadas ni como
prueba de estabilidad fisica. La revision cubre cliente/proveedor Web,
lectores/suscripciones, broker/colas/biblioteca instalada, radio, HTTP/proxy,
identidad de arranque, scheduler UART y supervision AVR; no todo el producto
ajeno a la conexion ni binarios instalados desconocidos.
