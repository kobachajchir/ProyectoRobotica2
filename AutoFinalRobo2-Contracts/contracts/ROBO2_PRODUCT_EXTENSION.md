> AFR2-20261025-01: las reglas anteriores de acceso quedan sustituidas por [control directo EX01v7](DIRECT_CONTROL_7.md). Sin adquisición, renovación, liberación ni habilitación OLED. Los formatos anteriores solo se conservan donde DIRECT_CONTROL_7 mantiene compatibilidad.

> Estado vigente **AFR2-20261024-02**, 24/10/2026: integración ROBO2/perfil2 + EX01v3 compilada en Atmega/ESP/Web; GOTO completo, configuración por catálogo compartido y jobs Wi-Fi OLED conectados. Paquete Web actualizado en ESP/data/dist. Sin pruebas de navegador/hardware ni flasheo; secuencias físicas mantienen bloqueo de memoria. Ver [matriz vigente](EX01_ALIGNMENT_3.md) y [evidencia](../changes/2026-10-24-02-project-alignment.md). Los estados previos que siguen son históricos.

# ROBO2 EX01: producto ATmega y complemento ESP/Web

AFR2-20261023-19. Fuente ATmega escrita, sin compilar/probar. ESP/Web NO implementados
por esta ficha. Este contrato es la entrega para ese trabajo paralelo.

## Compatibilidad y reglas comunes

Se mantiene el [perfil 2](UART_PROFILE_2.md): cabecera ROBO2, N<=64, tokens3A/A5,
versión01, rutas31 petición/13 respuesta, XOR y prefijo sesión/transacción u32LE
no cero. No se modifica `afr2_wire.h` ni sus capabilities legacy. Nuevas constantes
en [afr2_product_wire.h](afr2_product_wire.h). Mega firmware0.4.0.

Primero hello01 del perfil2. Después CMD10 con cuerpo ASCII EX01. Solo entonces
se aceptan comandos EX01; no inferir soporte por versión o por conectar Web.
ESP actual restringe IDs/longitudes a01..06: debe incorporar esta extensión antes
de usarla; NO enviar tramas nuevas con su receptor anterior y asumir compatibilidad.
La composición UART sola conserva sus servicios anteriores sin extensión.

En tablas: `q` es cuerpo de petición desde payload[8]; `b` cuerpo de respuesta
desde payload[9]. N petición=8+q, N respuesta OK=9+b. Error N=9, sin cuerpo, result
legacy0..4. BAD_REQUEST=1 para forma/rango; UNAVAILABLE=2 también para falta de
permiso/requisitos/busy en esta extensión; SESSION=3; INTERNAL=4. El ESP puede
precisar el mensaje usando EX_STATE, pero no presentar un rechazo como aceptación.

Se validan sesión, transacción creciente, ruta completa, checksum, longitud exacta,
rangos y permisos al recibir. Un error de extensión no renueva supervisión. STOP06
sigue reservado, sin lease, y no espera completar una captura, escaneo o borrador.
Una acción confirmada no se reintenta automáticamente. Espacios no asignados quedan
reservados; no importar catálogo F4.

## Comandos implementados en ATmega

| CMD | q exacto | b OK / semántica |
| --- | --- | --- |
| 10 EXT_HELLO | 4: EX01 | 10: EX01[0..3], versiónu16=1[4..5], featuresu32[6..9] |
| 11 STATE | 0 | 16: mode[0],owner[1],outputsOff[2],fault[3],remoteEnabled[4],poseValid[5],gotoState[6],prefsState[7],featuresu32[8..11],uptimeu32[12..15] |
| 12 LEASE | 7: opu8[0] 0acquire/renew o1release, tokenu32[1..4], ttlu16[5..6] | 3: owneru8,ttlu16; token no cero, TTL500..5000ms, remoteEnabled requerido |
| 13 DRIVE | 12: tokenu32,vxi16,vyi16,wzi16,ttlu16 | 0: consigna aceptada; unidades mm/s,mm/s,mrad/s; TTL200..1000ms; límites de configuración |
| 14 WHEEL | 9: tokenu32,wheelu8,pwmi16,durationu16 | 0: prueba aceptada, rueda0FL/1FR/2RL/3RR, PWM±límite, duración100..2000ms |
| 15 MODE | 5: tokenu32,modeu8 | 0: solo0idle o2line-follow; otros modos por su comando dedicado |
| 16 GOTO | 12: tokenu32,Xi32,Yi32 | 0: objetivo relativo al robot al iniciar, mm, cada eje±5000; no ambos0; requiere odometría |
| 17 CANCEL | 0 | 1: gotoState tras detener; no exige lease |
| 18 POSE | 0 | 25: validu8,Xi32,Yi32,yawi32,ageu32,frameu32,sequenceu32; mm/mm/mrad/ms; ageFFFFFFFF si inválido |
| 19 HALL_CAL | 6: tokenu32,opu8,wheelu8 | 6: measuringu8,badu8,deltai32; op0iniciar/1finalizar/2guardar en EEPROM/3descartar; token/lease válidos |
| 1A CONFIG | 10: tokenu32,opu8,actionu8,valuei32 | 4: valor actuali32; op0leer/1escribir; escritura solo parado/con lease; solo acciones numéricas del catálogo |
| 1B PREFS | 5: tokenu32,opu8 | 0: op0guardar/1descartar; solo parado, sin guardado pendiente |
| 1C LINE_CAL | 5: tokenu32,opu8 | 1: calibratedu8; op0confirmar tipo/umbrales/polaridad configurados, op1invalidar; solo parado/con lease y sin EEPROM/Hall en curso; no guarda EEPROM |
| 20 UI_META | 0 | 13: revisionu32,screenu32,selectionu8,firstVisibleu8,sizeu16=1024,notificationActiveu8 |
| 21 UI_PAGE | 7: revisionu32,offsetu16,countu8 | 7+count: revisionu32,offsetu16,countu8,bytes[count]; count1..40, offset+count<=1024 |
| 22 UI_ACTION | 6: revisionu32,actionu8,deltai8 | 5: screenu32,selectionu8; action0rotate/1select/2menu, delta±1 para rotate y0 para resto |
| 23 UI_RELEASE | 4: revisionu32 | 0: liberar captura retenida |
| 30 WIFI_POLL | 1: offsetu8 | 14+count: pending[0],jobIdu32[1..4],op[5],total[6],offset[7],count[8],requestedMode[9],draftIdu32[10..13],data[count] |
| 31 WIFI_UPDATE | Variable por subtipo de abajo | 0: dato/resultado aceptado por Mega |

Mode:0idle/1manual/2line/3wheel-test/4goto/5fault. Owner0none/1local/2remote.
`outputsOff` acredita salidas/consignas desactivadas por firmware; NO mide rueda
inmóvil ni frenado físico. Legacy STOP06 conserva su respuesta11bytes con
stopped1/mode0; leer STATE para ver un fallo retenido. Lease legacy sigue0 según
perfil2: el puente nuevo debe usar EX_STATE para representar dueño real.

Features: bit0UI,1STOP,2línea válida,3Hall disponible,4movimiento preparado,
5pose válida,6interfaz Wi-Fi implementada en Mega,7preferencias. Son condiciones
del runtime; bit Wi-Fi no acredita puente ESP disponible ni conexión STA.
Hall solo disponible con macroAFR2_HALL_CONNECTED=1 y configuración hall_wired=1.
Motion necesita configuración motor_ready y pose calibrada. Faltan ensayos reales.

GotoState0sin misión/1activa/2completada/3cancelada/4fallida. PrefsState0idle/
1escribiendo/2completado. Fault0ninguno/1latencia de control>100ms durante marcha/
2odometría no fiable/3consigna sin feedback Hall durante>500ms con PWM>=40.

### CONFIG: identificadores estables de esta entrega

`action` es el valor explícito de enumeración en `afr2_menu_catalog.h` entregado
con esta ficha. El [catálogo JSON de implementación](ATMEGA_CONFIG_ACTIONS.json)
fija IDs, mínimo, máximo, paso y unidad. No asignar desde la posición visible
de un ítem ni desde textos. Ampliaciones deben preservar IDs, no regenerarlos
reordenando entradas. Leer no requiere token; escribir sí. El paso es resolución
del editor, no obligación de múltiplo para parámetros enteros salvo encoder2/4.

## Proyección de pose para el mapa Web

EX_POSE lleva frame[17..20] y sequence[21..24]. Sequence aumenta al adquirir
odometría, no al consultar. Una recalibración/origen nuevo incrementa frame y
reinicia sequence; antes de wrap de sequence también cambia frame. X hacia
adelante, Y izquierda, yaw antihorario, igual a WEB_CONTROL_MAP.md. ESP debe
proyectar `pose.schema=afr2-pose-1`, xMm/yMm/thetaMrad, valid y ageMs, y construir
frameId con su generación de enlace/arranque Mega y el frame recibido. Cambiar
la generación al renegociar tras reset/pérdida de enlace evita confundir un
contador reiniciado con el marco anterior. Nunca generar sequence desde el
polling ni sustituir pose inválida por cero. Publicar wheel_odometry válida
solo con pose válida y fresca; no elevarla por órdenes enviadas.

## Captura OLED y acciones Web

Representación: 1024bytes, páginas verticales LSB, mismo framebuffer de dibujo.
UI_META se acepta tras render completo, fuera del teclado/editor y sin otra
captura. Retiene la revisión hasta UI_RELEASE o600ms, sin copiar otro framebuffer.
Durante retención se suspenden solo redibujos; seguridad, sensores, UART y OLED
Task siguen funcionando. Los fragmentos nunca mezclan revisiones. META repetido
no extiende el plazo: devuelve unavailable. Descartar transferencia incompleta.

Una entrada física cancela la retención y tiene prioridad. ESP lee fragmentos
secuenciales de hasta40bytes tan pronto como cada respuesta llega, dentro de
600ms; luego libera. No usar el intervalo del polling Web entre fragmentos.
Codificar Base64 una sola vez en ESP al completar los1024bytes y entregar a Web.
No existe promesa de captura física atómica del SSD1306.

UI_ACTION exige referencia no cero a la última captura, edad<=4000ms, ninguna entrada
local ni cambio semántico de navegación/notificaciones desde esa captura, ninguna retención activa y salidas apagadas sin control
local. Una captura no otorga lease. Se permiten navegación y solicitudes Wi-Fi,
pero se rechazan teclado/editor numérico remoto y acciones que tomarían control
local, iniciarían motores, moverían servo o harían calibración física. Para
control/configuración usar comandos dedicados con token remoto, no simular clics
en el menú local. Acciones no idempotentes: no reintentar tras timeout.
Guardar/Descartar EEPROM, confirmar calibración, limpiar fallo y Conectar Wi-Fi
también se rechazan por UI_ACTION: la conexión del borrador requiere clic físico
en el auto; configuración/calibración remota usa CONFIG/LINE_CAL/HALL_CAL/PREFS.

El catálogo creció: Web debe aceptar IDs propios registrados aunque no tenga
renderer semántico para ellos, utilizando framebuffer. No reconstruir pantallas
nuevas con el rasterizador antiguo. `notificationActive` indica una superposición;
screen/selection se refieren al contexto subyacente conservado.

## Wi-Fi: tareas del auto, puente y modal Web

ESP sigue iniciando transacciones UART31→13. Mega deja solicitudes en un outbox;
ESP consulta WIFI_POLL cada250ms, sin desplazar STOP, y reporta resultados con
WIFI_UPDATE. Así el auto puede solicitar operaciones sin crear dos gestores de
transacción enfrentados. EX_HELLO + un status inicial habilitan las opciones Wi-Fi
locales; sin complemento las opciones muestran indisponibilidad real.

Ops:1status/2scan/3modalWeb/4borradorOLED/5conectar/6cancelar/7reintentar.
requestedMode:1STA/2AP/3AP_STA. Cada solicitud tiene jobId no cero, estable hasta
resultado, y pertenece a la sesión UART. El ESP deduplica jobId/op; releer POLL
no ejecuta otra configuración ni abre otro modal. Los resultados deben mencionar
el mismo jobId y requieren que la solicitud se haya leído primero.

POLL retorna data para op3 (SSID seleccionado,0..32bytes sin terminador) y para
op4: `SSID\0password`, máximo96bytes. Leer offset0,
luego los siguientes hasta total, ensamblar una sola vez y validar longitudes
SSID1..32/password0o8..63. Cada respuesta lleva como máximo40bytes de datos.
Las otras peticiones sin datos usan offset0. No loguear payloads de este comando. Una vez
confirmado el borrador por ESP, Mega borra su password RAM. Ninguna contraseña
de Web vuelve al Mega. El identificador draftId es opaco, temporal y de ESP.

### WIFI_UPDATE (q desde subtipo)

| subtype | q exacto y offsets | Uso |
| --- | --- | --- |
| 0 status | 14+L: subtype[0],mode[1]0OFF/1STA/2AP/3AP_STA,connected[2]0/1,rssii16[3..4],L[5]0..32,ssid[6..5+L],ip4[6+L..9+L],apIp4[10+L..13+L] | Estado real cada1s; válido5s. No modifica requestedMode ni confirma un borrador |
| 1 network | 3+L: subtype[0],index[1]0..7,L[2]0..32,ssid[3..] | Resultados de scan en orden desde index0; máximo8 redes locales |
| 2 result | 11+L: subtype[0],jobIdu32[1..4],state[5],draftIdu32[6..9],L[10]0..32,ssid[11..] | Estado del trabajo; draft ready requiere token y SSID no vacíos |
| 3 version | 3+L: subtype[0],source[1]0ESP/1Web,L[2]0..15,version[3..] | Versiones declaradas por sus productores; ausencia muestra-- |

Strings por longitud, sin NUL interior; IP como cuatro octetos, no texto en wire.
state:0accepted/1draftReady/2connecting/3completed/4failed/5cancelled. `completed`
significa estado recibido/scan terminado para op1/2; para op5/7 solo significa
red realmente aplicada/conectada según modo. No enviar connected por mero ACK.

### Flujo OLED

Elegir red/configuración → Ingresar aquí → teclado SSID con OK → teclado password
con OK → solicitud op4 → ESP conserva borrador y responde draftReady → OLED
muestra revisión → usuario elige Conectar → op5 con draftId → ESP aplica y envía
connecting/resultados reales. Password enmascarado. SW largo en teclado cancela
y vuelve sin enviar/aplicar; en el resto abre menú principal. Modos AP y STA
usan el significado SSID/password ya definido para `wifi.configure`.

### Flujo Web solicitado por el auto

Elegir Ingresar en Web → op3 → ESP verifica Web lista → evento JSON
`wifi.credentials.requested` con `{jobId,mode,ssid,expiresInMs}` → Web abre modal
por esa solicitud. **Una sola sesión WebSocket**, segundo intento rechazado
manteniendo la primera. Sin sesión lista, informar failure; OLED permite elegir
teclado local. Abrir el modal no concede control ni modifica la red.

Nueva petición JSON `wifi.credentials.stage` con `{jobId,mode,ssid,password}`:
ESP valida sesión/jobId/rangos, conserva en RAM y devuelve `{jobId,draftId}`;
reporta draftReady al Mega SIN password. El modal solo prepara: nunca llama
automáticamente a wifi.configure. Mega abre revisión; **Conectar se confirma
desde el auto**. ESP aplica únicamente cuando POLL devuelve op5/draftId vigente.
Al cerrar modal emitir `wifi.credentials.cancel {jobId}` y reportar cancelled.
El ingreso Wi-Fi independiente ya existente en Web sigue permitido, pero ESP
debe rechazar configuraciones competidoras mientras haya un borrador del auto,
en lugar de aplicar silenciosamente un contenido distinto.

Caducidad: solicitud modal120s, resto trabajos30s, borrador listo120s. No extender
el timeout de un trabajo porque llegan estados de radio. Cambio de sesión UART,
desconexión Web/cancelación o caducidad invalidan tokens y borradores. Mega
manda op6 al cancelar: esa solicitud reemplaza el trabajo previo. ESP debe
cerrar el modal con `wifi.credentials.closed {jobId,reason}` y borrar secretos.

Notificaciones OLED: aceptación no equivale a conexión. connecting muestra
barra temporal de3000ms; resultado posterior muestra éxito/fallo/cancelación
con igual barra y restaura el contexto. draftReady abre revisión. La cola no
bloquea UART, seguridad ni sensores. Credenciales nunca en notificaciones,
snapshots de editor, EEPROM, logs o repositorio.

## Entrega, aceptación y pendientes

Fuente AVR: endpoint EX01, outbox, teclado/revisión y servicios implementados.
ESP debe implementar negociación/tabla/cuerpos/resultados, outbox/credenciales,
único WebSocket y correlación Web; Web debe implementar modal y catálogo/framebuffer
ampliado, estados/pose y errores. No editar el header común para cambiar perfil2
por implicación ni anunciar estas capacidades en el ESP actual.

Pruebas futuras bajo pedido: todos los casos ROBO2 previos, desconocido EX antes
de negociar, lease/token, STOP durante captura/trabajo, fragmentos repetidos,
offset/rango/revisión caducada, prioridad local, modos/rangos, borrador reemplazado,
Web desconectada, segundo socket, timeout y reconexión sin reaplicar secretos.
No se ejecutó ninguno ni se generó firmware AVR o paquete Web en esta ficha.

## Separación respecto al diseño perfil3 de AFR2-20261023-21

EX01 está implementado en fuente AVR SOBRE perfil2 y requiere su propio HELLO.
UART_PROFILE_3.md define otro conjunto/hello, todavía diseñado para ESP/AVR.
Por ejemplo CMD10 significa EX_HELLO aquí y secuencias en perfil3: el número solo
no identifica un comando. No activar ese perfil cambiando un define ni traducir
sus comandos a EX01 por semejanza. El ejecutor de secuencias no está implementado
por19 y no se anuncia. Decidir una migración/coexistencia explícita con ficha,
versiones, memoria y codecs de ambos extremos antes de integrar21.
