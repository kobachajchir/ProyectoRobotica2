> AFR2-20261025-01: las reglas anteriores de acceso quedan sustituidas por [control directo EX01v7](DIRECT_CONTROL_7.md). Sin adquisición, renovación, liberación ni habilitación OLED. Los formatos anteriores solo se conservan donde DIRECT_CONTROL_7 mantiene compatibilidad.

> Estado vigente **AFR2-20261024-02**, 24/10/2026: integración ROBO2/perfil2 + EX01v3 compilada en Atmega/ESP/Web; GOTO completo, configuración por catálogo compartido y jobs Wi-Fi OLED conectados. Paquete Web actualizado en ESP/data/dist. Sin pruebas de navegador/hardware ni flasheo; secuencias físicas mantienen bloqueo de memoria. Ver [matriz vigente](EX01_ALIGNMENT_3.md) y [evidencia](../changes/2026-10-24-02-project-alignment.md). Los estados previos que siguen son históricos.

# Integración Web–ESP–ATmega: EX01 v2

Estado vigente07: formato histórico v2 sustituido en longitudes/servicios por
[EX01v4](EX01_SYSTEM_SEQUENCE_4.md), conservando framing ROBO2/perfil2 y
afr2-ws-1. Atmega/ESP/Web compilados, sin ensayo de interoperabilidad física.
[Matriz actual](../docs/ESTADO_IMPLEMENTACION.md).

AFR2-20261024-01. Fuentes adaptadas, sin compilar, validar ni probar esta revisión.
Conserva ROBO2/perfil **2** y `afr2-ws-1`. El firmware de producto de la ficha 19
ya ocupa 0x10..0x1B: el diseño UART_PROFILE_3.md de la ficha 21 colisionaba con él.
No se activa perfil 3 ni se reinterpretan IDs de EX01 v1.

## Negociación y compatibilidad

Primero hello ROBO2/perfil2; después EXT_HELLO 0x10 con cuerpo `EX01`.
Respuesta cuerpo: EX01, versión u16 LE (1 o 2), features u32 LE. Las rutas,
sesión/transacción, XOR, máximo64 bytes y timeout UART800ms se conservan.
EX01 v1 permite los servicios de producto existentes; v2 agrega secuencias.
Flags existentes 0x01..0x80 se conservan; 0x100 anuncia secuencias disponibles.
Cada respuesta exige comando/transacción/sesión, longitud y cuerpo coherentes.
No se aceptan ACK arbitrarios ni se reintentan movimientos automáticamente.

## Control y estado

ESP asigna un token u32 no cero al responsable del control. No es autenticación. Traduce
acquire/renew/release a EXT_LEASE 0x12; renew solo renueva al dueño actual, nunca
adquiere implícitamente. Lease1500ms, renovación Web500ms. La expiración local o
desconexión WS invalida la generación de órdenes y solicita STOP reservado.
ATmega supervisa independientemente. Ningún alive renueva el lease de secuencias.
Las respuestas de control y eventos personalizados actualizan controlLease antes
de que la Web emita la siguiente consigna. Una segunda sesión no obtiene control
ni puede modificar el programa del dueño. STOP sigue sin requerir lease.

robot.state utiliza EXT_STATE 0x11 y, si hay pose válida anunciada, EXT_POSE0x18.
Modo6=sequence, conserva fault5. Pose usa el perfil afr2-pose-1, mm/mrad, edad,
marco y secuencia de origen; no se integra una orden para inventar odometría.
Drive0x13, prueba de rueda0x14 y modo0x15 respetan validación/configuración AVR.
Prueba de rueda devuelve accepted: ACK de inicio no confirma haber terminado.
GOTO X/Y existente no implementa todos los argumentos del formulario Web:
theta/velocidad por misión se rechazan explícitamente y la capability GOTO Web
no se habilita por el solo hecho de tener motores/pose.

## OLED ampliada

robot.screen.get es una operación compuesta: UI_META0x20 → UI_PAGE0x21 en bloques
de hasta40 bytes → UI_RELEASE0x23. Se conservan la revisión y el framebuffer
coherente de 1024bytes del AVR. Cada bloque comprueba revisión, offset y tamaño;
un fallo descarta la captura completa. El productor vence su bloqueo a600ms y
el consumidor tiene un límite global1800ms, sin prometer rendimiento medido.
Pérdida de captura por entrada local o vencimiento no produce una imagen parcial.

Respuesta Web: screenCode hexadecimal, selection, firstVisible, revision:u32 y
framebufferBase64 canónico (1368caracteres/1024bytes). El visor acepta códigos del
menú ampliado solo con framebuffer; la reconstrucción local sigue limitada a
sus cuatro renderers. Ninguna nueva identidad selecciona un renderer incorrecto.

robot.ui.action agrega revision del snapshot y se traduce a UI_ACTION0x22.
ESP exige una captura completada de esa sesión; AVR comprueba generación, edad,
estado local, edición y permisos de acción. Corto/select, largo/menu800ms y giro
mantienen su semántica; la Web nunca adelanta el resultado. Teclado/editores
locales y credenciales no se exportan mediante capturas remotas.

## Secuencias: rango libre 0x40..0x45

Todos los campos siguientes son **cuerpo de aplicación**, después de los8bytes
sesión/transacción. La respuesta agrega su prefijo9bytes. Enteros LE, i16 en
complemento a dos. Tokens nunca cero; orden de ruedas FL/FR/RL/RR.

| CMD | Cuerpo petición | Cuerpo OK |
|---|---|---|
| 40 begin | owner:u32, seq:u32, count:u8, totalMs:u32 (13B) | seq:u32,state1:u8,maxCount32:u8,maxMs120000:u32 (10B) |
| 41 append | owner:u32,seq:u32,index:u8,ms:u32,from[4]:i16,to[4]:i16 (29B) | seq:u32,index:u8,state1:u8 (6B) |
| 42 commit | owner:u32,seq:u32,count:u8,totalMs:u32 (13B) | seq:u32,state2:u8,count:u8,totalMs:u32 (10B) |
| 43 start | owner:u32,seq:u32,exec:u32 (12B) | seq:u32,exec:u32,state3:u8,index0:u8,elapsed0:u32 (14B) |
| 44 cancel | owner:u32,seq:u32,exec:u32,reason:u8 (13B) | seq:u32,exec:u32,state5:u8,stopped1:u8 (10B) |
| 45 status | seq:u32,exec:u32 (8B) | seq:u32,exec:u32,state:u8,index:u8,count:u8,elapsed:u32,total:u32 (19B) |

Estados0idle/1preparing/2ready/3running/4completed/5cancelled/6failed; index255
significa sin segmento activo. Máximo32 segmentos y120000ms, múltiplos de10ms;
duración positiva por segmento y suma exacta antes de commit. Las velocidades
firmadas deben estar dentro del límite **real del AVR**, sin saturación oculta.
Se conserva propietario del programa y se rechazan cambios durante ejecución.
La Web usa strings sequenceId/executionId; ESP los asocia a tokens u32 y conserva
la identidad al resolver solicitudes/status. Cancel WS usa comando0x44 prioritario
tras comprobar dueño/identidad; puede interrumpir cualquier lectura en vuelo.

El ejecutor reside en ATmega y entrega targets al PID existente, sin renovar
su propio lease. STOP/revocación/caducidad cancelan cargas y ejecución. Al
completar, apaga salidas y libera control; no confirma inmovilidad medida.
Una respuesta tardía de commit/start no reinicia una ejecución cancelada.

La tabla estática ocupa640B (32×20); falta mapa SRAM/pila. Por ello
`AFR2_SEQUENCE_MEMORY_APPROVED` queda **0** por defecto y no se anuncia capacidad.
Habilitarla requiere revisar el mapa bajo pedido expreso y configurar el define
a1 en la compilación del producto. Además se exigen Hall disponibles, geometría,
motor_ready y pose/calibración válidas, conservando los requisitos de ficha19.
La Web no puede habilitar esos requisitos con su checkbox de perfil ilustrativo.

## Wi-Fi y pendientes separados

La ficha20 permanece: scan terminal, persist/RAM, status/fallback y credenciales
fuera de logs. No se modifica wifi_service. Se añade rechazo de NUL embebido en
la frontera JSON del broker. Las consultas Wi-Fi no exigen Mega ni EX01.
Los jobs Wi-Fi iniciados desde OLED y su modal de borrador de la ficha19 siguen
siendo un flujo separado pendiente: este cambio no los anuncia como integrados.

Pendientes: builds y mapas AVR/ESP/Web, casos de protocolo/cancelación/captura,
revisión visual y banco. Sin carga de firmware/FS ni promoción del paquete Web.
