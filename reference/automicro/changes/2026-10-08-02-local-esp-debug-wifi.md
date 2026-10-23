# AMI-20261008-02 — interfaz de depuracion y Wi-Fi local sin F4

- Fecha: 2026-10-08 (America/Buenos_Aires).
- Origen: Web ESP01 y Firmware ESP01.
- Referencia: cambios locales de fuentes; codigo de consumidores sin commit ni publicacion en este turno.
- Estado: implementado en fuentes; sin build, tests, runtime ni upload.
- Area: acceso local a interfaz y permisos WebSocket de depuracion ESP.

## Contrato anterior y nuevo

El acceso local de depuracion solo existia bajo import.meta.env.DEV. La SPA compilada y servida desde LittleFS necesitaba autenticacion F4. Ademas, fuera del aprovisionamiento AP, los comandos de Wi-Fi requerian una sesion STM autenticada.

Por autorizacion explicita del usuario, la entrada de depuracion funciona tambien en la SPA compilada. Crea dev-session, activa devMode, limpia el estado de autenticacion STM y permite explorar la interfaz sin esperar a la F4. El valor de acceso no se registra en esta central.

Se agregan comandos opcionales de JSON API v1:

- esp.auth.debug.login: args={pin:string de cuatro digitos}. La ESP compara el codigo de depuracion y aplica el limite de intentos existente en la primera habilitacion. Respuesta exitosa data={debugWifi:true, authenticated:false}; errores invalid_pin, busy o ERR_BAD_PARAMS.
- esp.auth.debug.logout: args={}. Respuesta exitosa data={debugWifi:false}. Revoca exclusivamente el permiso local de depuracion.

No se cambia esp.auth.pin.login, su PIN F4 ni los payloads UART. Ambos comandos nuevos se resuelven en la ESP, sin solicitud UNER a la F4.

## Flujo completo

1. Login resuelve localmente la entrada de depuracion, configura devMode=true y abre las rutas Web. remotePinAuthenticated permanece false.
2. Con WebSocket listo, UserContext envia esp.auth.debug.login con requestId mediante el cliente API v1. Usa timeout de 3000 ms; si falla, la interfaz sigue disponible y Wi-Fi no se declara habilitado.
3. La ESP revoca el estado STM de ese endpoint y activa esp_debug_wifi, separado de stm_authenticated. No anuncia una autenticacion F4 falsa ni transmite el codigo a la F4.
4. La autorizacion de depuracion permite solo comandos conocidos con ruta BRIDGE_ROUTE_LOCAL_ESP de las familias wifi.*, esp.wifi.*, esp.ap.*, mas los alias de reinicio ESP necesarios para cambiar modo. Tambien admite su login/logout, esp.web.page.ready y esp.device.getStatus. Verifica destino esp y ruta local, no solo el prefijo.
5. Una restriccion explicita rechaza cualquier otro comando con ERR_AUTH_REQUIRED mientras esp_debug_wifi esta activo, incluso si una respuesta tardia cambia stm_authenticated. Incluye rutas STM, autenticacion/PIN F4 y OLED.
6. El scan considera el permiso local de depuracion para continuar y entregar resultados sin F4. WifiCredentialsContext sincroniza datos cuando la ESP confirma debugWifi. WifiSection espera esa confirmacion para habilitar su flujo real.
7. Una reconexion conserva el mecanismo existente de resumeToken y generacion, y la Web solicita otra vez el permiso local. Salir desactiva devMode, limpia el permiso en UI y solicita debug.logout cuando hay conexion. El estado local de depuracion no se persiste como credencial NVS.
8. Los eventos de expiracion de autenticacion F4 no expulsan dev-session, que nunca dependio de ese grant. La sesion real mantiene su flujo previo.

## Impacto

| Repositorio | Estado | Accion o motivo |
| --- | --- | --- |
| FirmwareF4 | no_aplica | No recibe los comandos de depuracion ni cambios de PIN, permisos o wire. |
| Firmware ESP01 | origen | Flag de permiso local, handlers debug y restricciones; scan sin grant F4. |
| Web ESP01 | adaptado | Login servido, modo desarrollador ON, apertura/reapertura del permiso local y gates Wi-Fi. |
| AutoMicroQtDesktop | adaptado | Catalogo documental actualizado el 2026-10-08; ejecucion del bypass no aplica, autenticacion normal sin cambios. |

## Compatibilidad y despliegue

La extension es opcional; clientes existentes conservan su autenticacion normal. Una Web nueva con firmware viejo abre la interfaz local pero no puede confirmar el nuevo permiso Wi-Fi. Actualizar firmware ESP y regenerar/copiar la Web a data, luego cargar firmware y LittleFS. F4 y Qt no requieren cambios para este flujo.

Las opciones wifi.ap.credentials.set, esp.wifi.setAp y wifi.ap.credentials.result.get son aliases que pasan por F4 en el contrato actual. Tampoco stm.wifi.apFallback.get/set es nativo ESP. El acceso local no las autoriza. No se declara la pantalla Wi-Fi completa independiente de F4.

Actualizacion 2026-10-08: AMI-20261008-03 amplia este acceso con rutas locales de AP y fallback, y ajustes Wi-Fi exclusivamente en RAM. La Web reutiliza las pantallas existentes. El bypass ya no autoriza los alias de reinicio persistente: cambia la radio mediante un comando temporal sin reboot. Los alias normales AP/fallback siguen delegados a F4. Esta actualizacion sustituye las limitaciones anteriores para el modo de depuracion; conserva el contrato del login normal.

## Evidencia y limites

Actualizacion 2026-10-08 por pedido expreso: WebApiContract::espDebugCommands registra login/logout local con esquemas, permisos, tiempos, errores y ciclo de vida. ProtocolCommandModel y ProtocolPage muestran estas entradas en el catalogo existente, sin ID UNER ni envio manual. Codigo Qt local sin commit ni push, sin build/tests/validaciones. Esto adapta el inventario; no habilita el bypass en Qt.

Implementacion de fuentes tras aprobacion explicita de la propuesta por el usuario, luego de un rechazo inicial del control automatico. El nuevo permiso no equivale a autenticacion STM. No se realizaron compilaciones, tests ni validaciones de navegador o hardware. El gzip existente en data no se regenero. La correccion de buffers WebSocket se documenta separadamente en AMI-20261008-01; su efecto sobre el reinicio aun no fue probado en placa.
