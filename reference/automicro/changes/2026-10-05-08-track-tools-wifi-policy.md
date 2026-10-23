# AMI-20261005-08 — herramientas del Seguidor y política WiFi integrada

- Fecha: 2026-10-05 (America/Buenos_Aires).
- Origen: Web ESP01.
- Referencia: cambios locales Web, preservando trabajo previo, sin commit/push de implementación.
- Estado: implementado en fuente/documentación; sin compilación ni revisión visual.
- Área: composición UI, representación IR y uso de consultas/acciones WiFi existentes.

## Contrato anterior y nuevo

Seguidor presentaba Puntos, Ribbon y Debug sensores como tres modos de Render.
Debug dibujaba tres puntos sintéticos y activaba Ribbon; los puntos de pista se
mantenían visibles también con Ribbon. Ahora Puntos y Ribbon son opciones
exclusivas del mismo selector. Debug es un toggle independiente y muestra las
burbujas axiales por sensor usadas en MPU + IR, mediante un componente compartido.
Herramientas queda debajo de Iniciar IR, Grabar, Pausar, Reset y Exportar e incluye
representación, Debug, Simulación local y período IR. Diagnóstico pasa al lateral.

Las burbujas conservan estimador, disco/anillo, anclajes CAD y materiales de
AMI-20261005-03. Comparten la lectura de calibración persistida de MPU + IR,
conservando clave, formato y polaridad anteriores; no se escribe otro perfil.
Seguidor usa el límite visual de 5 cm. Las distancias siguen siendo estimaciones
Web: no hay campo nuevo de distancia F4. Las muestras reales requieren conexión,
sesión STM, stream solicitado/activo y muestra confirmada. Sin disponibilidad o
con objects=false confirmado se retiran marcadores de detección. En simulación
se usa su muestra local. Los marcadores heredan la normalización del GLB sin
alterar su escala; se dibujan al cargarlo, sin posiciones inventadas en el cuerpo
fallback. Se conserva el marcador existente de error lateral de línea.

WiFi retira Recuperación de conexión como tarjeta separada. La tarjeta de modo
físico integra AP fallback como botón de estado, consulta de política/conexión y
mensajes de aplicación/error. Carga inicial agrega GET política a las consultas
existentes de configuración. El botón muestra Leyendo configuración durante esa
lectura y después Consultar política; consulta posterior refresca política y
estado de conexión, preservando campos editados. Cambiar modo mantiene su botón
separado y confirmación anterior. La sección AP conserva formulario y clientes.

Cuando STA está desconectada, hay red guardada y el formulario coincide con lo
persistido, el botón de conexión pasa a Reintentar estación y usa credenciales
guardadas. Con campos editados se conserva el envío de credenciales; en pantalla
provisioning se mantiene Conectar y salir del modo AP. La aceptación de un
reintento no se presenta como conexión conseguida.

## Payloads, respuestas y transiciones

No cambian APIs, IDs, payloads, longitudes, endian, eventos, timeouts remotos,
códigos OLED ni formatos de calibración. Se conservan:

- IR: `irSample`, UNER 0x91 de 56 bytes; SET/STOP del broker vigente 0x6B/0x6C
  con propiedad por endpoint de AMI-20261005-04. Selector/Debug no mandan comandos,
  no activan emisores y no crean/liberan suscripciones.
- `stm.wifi.apFallback.get` / 0x1C: request vacío; respuesta de tres u8
  `[status,enabled,syncState]`, status 0, enabled 0/1, syncState 0..3. JSON conserva
  `status`, `enabled:boolean`, `syncState:number`.
- `stm.wifi.apFallback.set`: JSON `{enabled:boolean}`; bridge vigente lo convierte
  en SET_PREFERENCES 0x63 con cinco bytes `[0,0,0,1,enabled]` (máscara bit 24 en
  little endian más bool). Conserva respuesta de preferencias existente. F4
  persiste y sincroniza con ESP por 0x1B como AMI-20261004-01. No se envía SET 0x1B
  directamente desde la nueva pill Web.
- `esp.wifi.connect`: ruta local ESP con params `{}`; conserva respuesta y
  finalización del handler existente. Web espera hasta 25000 ms, igual que la
  acción previa de reintento. No sustituye el flujo de nuevas credenciales.
- Consultar conexión reutiliza `esp.status.get` y el provider de estado WiFi.
  No se cambian las consultas existentes de configuración AP ni sus payloads.

## Flujo completo

1. El usuario cambia representación o Debug; solo se altera estado visual local.
   Muestras IR y calibración existentes producen los marcadores compartidos.
2. Al cargar WiFi se consultan estado/configuración y política. La consulta
   posterior utiliza el provider de conexión y GET política, sin sobrescribir AP
   ni STA editados. Ausencia de WebSocket bloquea operaciones remotas.
3. Click en AP fallback parte del bool confirmado; request JSON v1 con requestId
   va a ESP, su bridge transmite el patch existente a F4 y correlaciona respuesta.
   La política sigue requiriendo sesión STM; no se cambia autenticación.
4. SET satisfactorio se sigue de GET política. Solo GET válido actualiza el botón;
   aceptación intermedia no cambia optimistamente su estado. Aplicación pendiente
   o error de sincronización se describen con syncState. Fallo/incompatibilidad o
   desconexión deja Política sin confirmar. Verde/rojo representa enabled, no
   prueba de que el AP esté abierto ni sincronización física certificada.
5. Reintentar usa el handler local ESP vigente y sus credenciales guardadas;
   requestAccepted mantiene su significado previo. Status no exitoso y errores
   se informan; después se consulta conexión. No se declara STA conectada solo
   por la aceptación. Credenciales nuevas mantienen su procedimiento original.

## Impacto en repositorios

| Repositorio | Estado | Acción o motivo concreto |
| --- | --- | --- |
| FirmwareF4 | compatible | GET_AP_FALLBACK existente de tres bytes y preferencia bit 24 continúan siendo consumidos sin cambio de formato. No se modifica código. |
| Firmware ESP01 | compatible | stm_commands.inc mantiene GET 0x1C y SET vía patch 0x63; dispatch conserva rutas AP fallback y esp.wifi.connect. IR mantiene el formato vigente. No se modifica código. |
| Web ESP01 | origen | Herramientas/diagnóstico reubicados; representación exclusiva; estimador, marcadores y calibración compartidos; política integrada y reintento con red guardada; tokens comunes. |
| AutoMicroQtDesktop | no_aplica | Cambio visual exclusivo del consumidor Web; no cambia protocolo ni se modifica Qt. |

## Compatibilidad y orden de integración

Se requieren los contratos de IR, AP fallback y GET política ya documentados;
firmware previo incompatible deja la política sin confirmar. No se requiere
actualizar firmware por esta reorganización. Reconstruir/cargar la Web queda
pendiente de solicitud. Publicar documentación no actualiza bundle, binarios ni
clones; el GLB se conserva.

## Evidencia y límites

Lectura de fuente Web, cálculo axial, geometría CAD compartida y handlers/registry
ESP/F4, junto con las fichas vigentes. Fuente y documentación modificadas solo en
Web y registro central. Sin tests, compilaciones, validaciones, inspección visual
en ejecución, tráfico real, deploy/flasheo ni prueba física por instrucción del
usuario. No se regenera/copía gzip/data ESP. Compatibilidad descrita es de formato,
no demostración de interoperabilidad o medición de distancia física.

## Actualizaciones posteriores

- 2026-10-05: aplicación en fuente; revisión visual y reconstrucción/carga pendientes.

### Navegación manual de cámara — 2026-10-05

Anteriormente, el seguimiento del Seguidor recolocaba la cámara en cada cuadro
fuera de Órbita y bloqueaba el desplazamiento manual. Ahora todas sus vistas usan
el mismo controlador: Ctrl + arrastrar con el botón izquierdo desplaza cámara y
enfoque en el plano de pantalla. Un cambio manual deja el selector sin opción
activa y pausa el seguimiento. Pulsar Top, ISO, Trasera u Órbita restaura posición,
orientación y enfoque iniciales respecto de la posición actual del vehículo;
las vistas con seguimiento lo retoman. Órbita inicia en la posición ISO y permite
navegación manual. La ayuda y Vista libre usan tokens de marca.

MPU + IR conserva la deselección al mover y la restauración al pulsar un preset;
se explicita el mismo gesto y Ctrl + arrastrar no modifica el objetivo IR emulado.
La vista superior de motores habilita desplazamiento y conserva orientación fija.
El desplazamiento actúa en el plano de pantalla; el zoom existente conserva el
movimiento en profundidad. Son transiciones locales Web, sin request, payload,
evento remoto, screen code ni cambio de contrato. No cambian telemetría, emisores,
calibración, posición simulada, permisos o estado de balance.

Solo se modifica fuente y documentación Web más este registro. FirmwareF4,
Firmware ESP01 y Qt no requieren adaptación por el gesto y no se modifica su
código. Sin tests, compilación, validación, revisión visual en ejecución, tráfico,
deploy ni prueba física. Bundle/data ESP permanecen sin regenerar.
