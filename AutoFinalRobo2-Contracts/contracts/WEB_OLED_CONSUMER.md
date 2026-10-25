> AFR2-20261025-01: las reglas anteriores de acceso quedan sustituidas por [control directo EX01v7](DIRECT_CONTROL_7.md). Sin adquisición, renovación, liberación ni habilitación OLED. Los formatos anteriores solo se conservan donde DIRECT_CONTROL_7 mantiene compatibilidad.

# Perfil consumidor OLED y modo de cabecera

Actualización14: catálogo como árbol59 derivado de padres, avisos contextuales,
biblioteca local navegable y controles de fila/scroll laterales adaptados de
AutoMicro. Conserva algoritmos C/TypeScript, glifos/XBM y framebuffer remoto.
Números resaltan; SW confirma. Flechas avanzan hasta3giros individuales con
captura/revisión nueva entre pasos, no paginación independiente. Cuenta dinámica
desconocida mantiene encoder básico. Local no invoca acciones ni inventa datos.
[Ficha14](../changes/2026-10-24-14-developer-oled-tree.md). Sin paridad física probada.

Actualización12: EX01v6 conserva los cuerpos OLED y las59 identidades. No hay
nuevo layout ni ScreenCode. La copia AVR/Web09 permanece vigente; ejecución
por rueda usa el wrapper de ruedas y tiempo máximo. Fuente compilada con
gates0, sin cotejo físico. Ver [v6](EX01_SEQUENCE_DISTANCE_6.md).

## Vigente AFR2-20261024-09

Web consume las59 identidades de `documentation/screens.json`:48nodos y11avisos,
normalizando los códigos hexadecimales de6/8cifras. La biblioteca local copia
las primitivas y layouts C actuales, con catálogo/glifos/XBM/secuencias extraídos
literalmente del AVR mediante `Web/tools/sync-oled-library.mjs`. Los cambios de
algoritmo C requieren portar/revisar `firmwareRenderer.ts` antes de actualizar sus
hashes revisados. El build comprueba que los códigos AVR tienen entrada compartida.

El visor conectado exige `framebufferBase64` de1024bytes verticales LSB. Conserva
los bytes recibidos; no ejecuta la copia local como sustituto ni predice navegación.
Se elimina la reconstrucción obsoleta de4pantallas, que ya no correspondía al firmware.
La biblioteca ofrece una vista local identificada de menús estáticos; no inventa
datos/avisos/red ni ejecuta comandos. Sus renderers completos requieren el mismo
snapshot preparado, estado UI, reloj y framebuffer anterior que el AVR.

Wire EX01v5 y permisos sin cambios. Clave AP y teclado siguen protegidos; no se
transportan credenciales para alimentar la copia local. La igualdad de bytes del
framebuffer recibido no demuestra correspondencia temporal de todas las páginas
físicas de la OLED. Comparación AVR/JS, revisión visual y hardware pendientes.

La skill local obliga a entregar pantalla ATmega y copia Web en la misma tarea.
[Ficha09](../changes/2026-10-24-09-linked-oled-library.md).
Las descripciones de propuesta/state y productor pendiente inferiores son históricas.

Vigente AFR2-20261024-08: se reciben también nuevos IDs0x03002d..2f y
0x040001..0b mediante framebuffer existente. Sin réplica JavaScript nueva.
EX01v5 protege la captura durante clave AP local y rechaza abrir Datos AP
con ui.action. Temporalmente ocupado no equivale a pantalla inventada ni a
habilitación de reinicio. [Contrato v5](EX01_NETWORK_UI_5.md).

Vigente AFR2-20261024-07: productor AVR, puente ESP y consumidor remoto Web
compilados con EX01v4; pantalla de secuencias0x03002c se transporta por el mismo
framebuffer sin réplica Web nueva. [Estado por módulo](../docs/ESTADO_IMPLEMENTACION.md)
y [formato v4](EX01_SYSTEM_SEQUENCE_4.md). Sin prueba física/interactiva; gates
de memoria/Hall conservados. Los estados pendientes inferiores son históricos.

AFR2-20261023-19: productor ATmega EX01 escrito, sin compilar/probar.
[Mapeo, captura/pose, permisos y complemento ESP pendiente](ROBO2_PRODUCT_EXTENSION.md).
No modifica el consumidor Web ni activa capacidades del broker por esta ficha.


AFR2-20261023-14. Este documento precisa lo que acepta la nueva Web; no asigna IDs UART ni implementa productores. `robot.screen.get` y `robot.ui.action` siguen con wire_id null y rechazo explícito CAPABILITY_UNAVAILABLE en el ESP actual. El perfil semántico de `state` es diseño pendiente de acordar e implementar en ESP/ATmega.

## Enlace, consultas y disponibilidad

Un único Afr2Client compartido, sobre `afr2-ws-1`. Mantiene correlación por requestId/cmd y descarte de sesiones anteriores. `requestCommand` expone el mismo cliente sin toast por cada consulta. No crea otro socket.

- Modo: `robot.state {}`, timeout 2500ms, nueva lectura 2500ms después de terminar la anterior, solo con Web–ESP y ESP–ATmega ready y página visible. Respuesta completed con mode del catálogo y ageMs finito ≥0 y <6000. Se elimina al vencer `6000-ageMs`, perder enlace o recibir error. Sin modo válido: «Sin datos». No se crea un evento nuevo ni se inventa estado del motor.
- OLED: solo con visor montado/activo, ambos enlaces ready y capability oled_128x64 implementada. `robot.screen.get {}`, timeout 2500ms; intervalo 1200ms tras completar la consulta anterior, sin superponer lecturas. Pausa al ocultar la página o cerrar modal. CAPABILITY_UNAVAILABLE detiene el bucle hasta reintento explícito, reapertura o cambio de sesión/capacidad. Sin productor, se muestra motivo y no se dibuja una captura ficticia.
- Snapshot retenido: antigüedad local desde recepción. Más de 4000ms, error o enlace no disponible impide acciones; una captura retenida se identifica caducada. Al cambiar sesión/endpoint o desactivar el visor se borra. Esta edad local no garantiza la fecha de adquisición del productor: un timestamp/revisión de origen sigue pendiente si se necesita esa garantía.

## `robot.screen.get`

Sobre res/ok/completed del esquema existente. Campos comunes obligatorios: screenCode del catálogo propio, selection y firstVisible uint8. `framebufferBase64`, si se incluye, debe usar Base64 canónico de 1368 caracteres y decodificar exactamente 1024 bytes (páginas verticales LSB), sin completar bytes ausentes. Se muestra como framebuffer recibido; no se promete captura física atómica del SSD1306.

Alternativa sin framebuffer, **Propuesta** de perfil Web para las cuatro pantallas iniciales: `state` completo con `lineValid:boolean`, `lineBits:uint5`, `ultrasonicValid:boolean`, `ultrasonicMm:uint16`, `oledReady:boolean`, `oledErrors:uint16`. `firstVisible=0`; selection 0..2 en menú, 0 en las otras tres vistas. Todos los campos son obligatorios; indicadores de invalidez no habilitan valores inventados. Se usa el renderer puro existente y se identifica «Reconstrucción del estado recibido». Una ventana distinta o estado incompleto exige framebuffer o un renderer/contrato posterior; no se asume compatibilidad.

## `robot.ui.action`

Payload existente `{action:'rotate'|'select'|'menu',delta:-1|0|1}`. Rotación usa ±1; select/menu usan 0. Timeout 2000ms, no idempotente: sin reintento automático. Requiere captura fresca y capability ui_encoder_sw implementada. Una acción a la vez; espera la lectura en vuelo y pausa nuevas lecturas durante su envío. completed permite volver a consultar snapshot; accepted o error conserva el último resultado confirmado y muestra el motivo. La Web no ejecuta Afr2UiApply como predicción remota.

SW único: menos de 800ms confirma al soltar; 800ms o más emite menu una sola vez. Soltar tras largo nunca confirma. Pérdida de foco, visibilidad, conexión/capacidad, cancelación del puntero, salida del botón, cierre/desmontaje del visor o entrada disabled cancela el gesto. El control no agrega botón Menú ni Atrás; Volver existe solo como ítem de pantalla. El progreso del hold modifica transform del indicador, sin render React en cada frame.

Actualización AFR2-20261023-21: el componente común de controles OLED (usado en la página `/oled` y en el modal flotante) muestra siempre **Giro izquierdo, SW y Giro derecho** junto con el botón de **Sincronizar pantalla (Actualizar)**, incluso cuando estén deshabilitados, indicando el motivo junto al control. Durante una pulsación larga (`>= 800 ms`), el botón `SW` cambia a color de atención (`--warning`) y texto específico, y conserva ese estado visual hasta que el usuario suelta el botón o tecla, aunque la solicitud ya esté en curso. La pantalla se actualiza exclusivamente con la respuesta/snapshot del controlador, sin navegación optimista.

## Entrega al ESP/ATmega y aceptación pendiente

Conservar command names, esquema, correlación y errores existentes. Cerrar productor/representación, versión del reporte, coherencia de framebuffer, unidades y campos de estado, persistencia de selección y firstVisible antes de habilitar capabilities. No anunciar implementado por la sola presencia de este consumidor.

Estados actuales: atmega fuente parcial sin reporte remoto integrado; esp rechazo explícito; web fuente adaptada sin compilar ni probar; shared perfil consumidor diseñado. Aceptación futura: sesión vieja ignorada, concurrentes correlacionadas, ausencia/invalidación de snapshot, no polling en background, no éxito optimista, corto/largo excluyentes, cancelación y ausencia de reenvío de acciones.


## Integración posterior — AFR2-20261024-01

La ruta vigente se precisa en [EX01_WEB_INTEGRATION.md](EX01_WEB_INTEGRATION.md).
Se conserva perfil2; el borrador3 se sustituye por EX01v2 sin colisión de IDs.
OLED usa framebuffer del catálogo ampliado y revision por acción; estado/pose
provienen del producto19. Fuente adaptada sin nuevo build/ensayo. No se aplican
los estados históricos de productor pendiente a la fuente actual ni se los
confunde con funcionamiento físico comprobado.

