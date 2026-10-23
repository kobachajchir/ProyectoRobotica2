# AMI-20261008-03 — Wi-Fi temporal en el bypass local ESP

- Fecha: 2026-10-08 (America/Buenos_Aires).
- Repositorios de origen: Firmware ESP01 y Web ESP01.
- Referencia de codigo: fuentes locales pendientes de commit y publicacion en ambos consumidores.
- Estado del origen: implementado en fuentes, sin compilacion ni carga.
- Area: autenticacion, API WebSocket JSON v1, configuracion y ciclo de vida Wi-Fi.
- Antecedentes: AMI-20261008-02 (permiso local) y AMI-20261008-01 (buffers TCP).

## Contrato anterior y nuevo

El permiso local anterior abria la interfaz, pero AP/fallback dependian de F4 y las credenciales STA conservaban su flujo persistente. Ahora el bypass utiliza las mismas pantallas y formularios, activa modo desarrollador y limita las operaciones reales a Wi-Fi nativo ESP. Sus ajustes se aplican en RAM, sin NVS_Save, preferencias F4 ni reinicio persistente. No se agrega una seccion de navegacion.

El login normal sigue autenticado por F4. Mantiene la persistencia STA existente, AP/fallback mediante F4 y el cambio de modo persistente con reinicio. No se modifican IDs ni payloads UNER.

Comandos nuevos opcionales, destino esp y resolucion local:

| Comando | Args | Resultado |
| --- | --- | --- |
| esp.wifi.debug.ap.set | {ssid:string,password:string,protected:boolean,ip:string} | Aceptacion diferida; sessionOnly=true,persisted=false. Configura AP temporal. |
| esp.wifi.debug.ap.get | {} | status=0,ssid,ip,protected,sessionOnly=true,persisted=false; lastApplyStatus cuando existe resultado. |
| esp.wifi.debug.mode.set | {mode:"ap" o "normal"} | state="scheduled",reboot=false,sessionOnly=true,persisted=false y campos de destino. |
| esp.wifi.debug.fallback.get | {} | status=0,enabled:boolean,syncState=2,sessionOnly=true,persisted=false. |
| esp.wifi.debug.fallback.set | {enabled:boolean} | Igual esquema de respuesta; cambia la politica solo en RAM. |

wifi.credentials.submit y sus alias mantienen sus argumentos existentes (SSID, password, direccionamiento y opciones existentes de reutilizacion/BSSID). En bypass marcan la transaccion session_only, omiten la persistencia y no notifican credenciales a F4. Aceptacion y evento de resultado incluyen sessionOnly; persisted solo es true al completar con exito una transaccion normal persistente. esp.device.getStatus distingue storage="session" o "persistent". El reintento STA usa la configuracion efectiva de RAM en bypass.

## Flujo completo

1. UserContext abre dev-session con modo desarrollador ON y solicita esp.auth.debug.login, con requestId y timeout existente de 3000 ms. No declara autenticacion STM. Las operaciones Wi-Fi esperan la confirmacion local.
2. La ESP asigna un unico propietario de los ajustes temporales de la radio. Copia la configuracion persistente a RAM y conserva modo/reconexion anteriores. Rechaza con busy otra sesion propietaria o transacciones normales que ya esten en curso.
3. Los formularios existentes STA/AP/modo/fallback eligen las rutas temporales cuando user.id es dev-session; el resto conserva las rutas normales. La Web distingue aplicar a esta sesion de guardar o reiniciar. Suprime solicitudes de credenciales originadas en F4 y acciones de reset del sistema en bypass.
4. STA espera el resultado real de conexion. AP se aplica tras 75 ms para responder primero; si AP no esta activo, actualiza su configuracion temporal sin encenderlo. wifi.ap.credentials.result informa el resultado con sessionOnly=true,persisted=false. Un fallo STA restaura la conexion previa conservando AP.
5. El cambio de modo espera 75 ms y opera sobre la radio sin reiniciar la placa. wifi.debug.mode.result informa success,mode (AP/RED),sessionOnly=true,persisted=false. La aceptacion scheduled no equivale a resultado exitoso. Cambiar la radio puede interrumpir la conexion Web.
6. Se serializan las mutaciones mientras hay STA/AP/modo pendiente; consultas y cancelacion permitidas siguen disponibles. Los comandos debug nuevos requieren al propietario local; mutaciones nativas de otros endpoints reciben busy mientras la radio pertenece al bypass.
7. Los comandos F4, raw UNER y reinicios persistentes se rechazan con ERR_AUTH_REQUIRED durante el bypass, incluso ante una respuesta STM tardia. Se conserva el limite de intentos del login local. No se envia logout UART por una sesion que utilizo el bypass.
8. Logout descarta la configuracion temporal. Una desconexion permite resumir durante la gracia existente de 20 segundos; al vencer se libera el scope. Si hubo cambios de radio, restaura la configuracion persistente vigente y el modo/reconexion anteriores; si no hubo cambios, solo retira el override. Reiniciar tambien pierde RAM. No se guarda ningun ajuste temporal en Flash.

## Impacto en repositorios

| Repositorio | Estado | Accion o motivo concreto |
| --- | --- | --- |
| FirmwareF4 | no_aplica | El bypass no solicita autenticacion, preferencias ni acciones F4; se mantiene el wire normal. No requiere cambios de fuente. |
| Firmware ESP01 | origen | Scope RAM por sesion, propietario de radio, comandos locales, transacciones sin NVS y limpieza/restauracion. |
| Web ESP01 | adaptado | Formularios existentes eligen rutas temporales; textos y estados diferencian bypass del login normal. |
| AutoMicroQtDesktop | adaptado | Catalogo documental de comandos/eventos actualizado; ejecucion del bypass no aplica en Qt. Sus flujos normales no cambian. |

## Compatibilidad y orden de integracion

Actualizar juntos firmware ESP y bundle Web. Una Web nueva no puede completar las operaciones temporales contra firmware anterior. Las rutas normales conservan destino y significado; no se presupone soporte debug en clientes anteriores.

Pendiente regenerar el bundle Web con npm.cmd run build:esp (postbuild copia gzip a data ESP), compilar/cargar firmware y cargar LittleFS. F4 no necesita rebuild para este cambio. La correccion TCP registrada en AMI-20261008-01 sigue pendiente de prueba fisica.

## Evidencia y limites

Actualizacion 2026-10-08 por pedido expreso: el catalogo Qt existente muestra siete comandos JSON locales (login/logout, AP get/set, modo set y fallback get/set) y dos eventos (AP credentials result y debug mode result). WebApiContract::espDebugCommands conserva nombre, rol, args, todos los campos de data, finalizacion/correlacion, tiempos, permisos, errores, persistencia, expiracion y motivo de no aplicabilidad Qt. ProtocolCommandModel agrega filas JSON sin inventar IDs binarios; ProtocolPage muestra el detalle. No aparecen en el selector de envio UNER, no se agrega codec ni bypass Qt. Fuentes Qt locales sin commit ni push; sin compilaciones, tests ni validaciones.

Precision documental: AP set acepta reusePassword opcional (mismo SSID), clave vacia o de 8..63 bytes y SSID de 1..32 bytes. En JSON API v1 el envelope normaliza ERR_BAD_PARAMS/ERR_SSID_MISMATCH a invalid_request y ERR_AUTH_REQUIRED a unauthorized; busy permanece busy. invalid_pin actualmente se normaliza a uart_error; el catalogo refleja ese comportamiento de fuente y no supone un codigo distinto. Los codigos internos descritos arriba corresponden al handler/legacy. Esta actualizacion no modifica el wire.

Cambios de fuente y trazado de productores/consumidores; sin tests, compilaciones, validaciones de navegador o hardware, por instruccion del usuario. No se regenero el gzip ni se cargo la placa. El codigo ESP/Web permanece local sin commit ni push; esta ficha y el indice se publican en la central. No se afirma resuelto fisicamente el reinicio reportado.
