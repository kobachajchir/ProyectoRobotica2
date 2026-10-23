# AMI-20261005-03 — óptica inferior V2 y control de LED blancos en Web

- Fecha: 2026-10-05 (America/Argentina/Buenos_Aires)
- Repositorio de origen: Web ESP01 / WebESP01AutoMicro2025
- Referencia de código: cambios locales en irEmitterGeometry.ts, UnderbodySensorBeams.tsx, MpuIrScene3D.tsx, IrEmittersControls.tsx y EstadoSection.tsx; elevación en ControlSection y TrackFollowerSection
- Estado del origen: implementado en fuente; sin nueva compilación ni despliegue
- Área: flujo consumidor WebSocket JSON / interpretación del estado / geometría CAD

## Contrato anterior y nuevo

Antes: Web ofrecía tres botones «Objetos», «Línea», «Luces» y mostraba placas de piso derivadas de los sensores delanteros. El modelo tenía elevaciones visuales arbitrarias.

Ahora: panel «Emisión e iluminación» con switches separados para cinco IR de objetos, tres IR de línea y tres LED blancos inferiores. La luz blanca del dibujo depende de `lights=true` confirmado; desconexión/estado desconocido no se interpreta como encendido. Se incorporan las tres posiciones de emisores inferiores, su inclinación hacia los receptores y los centros de tres LED blancos. No se cambian IDs, envelopes, versiones ni formatos del wire. No se agrega un comando de LED porque el campo `lights` ya controla esa salida.

| Operación | JSON | UNER / payload |
| --- | --- | --- |
| Consultar | `stm.irEmitters.get`, args `{}` | 0x79, request vacío; response `[objects:u8,line:u8,lights:u8]`, 3 bytes |
| Cambiar | `stm.irEmitters.set`, args exactos `{objects:boolean,line:boolean,lights:boolean}` | 0x7A, request de 3 bytes `[objects,line,lights]`; response de 4 `[status,objects,line,lights]` |
| Evento | `stm.irEmitters.changed`, data tres booleanos | 0x98, 3 bytes `[objects,line,lights]` |

Booleans UNER 0/1, sin unidades; F4 normaliza cualquier no-cero recibido. JSON mantiene booleanos estrictos. Status 0 es éxito, los demás se rechazan. Para cambiar los blancos se conserva el último estado confirmado de `objects` y `line`, se cambia `lights` y se envía el conjunto completo. El ángulo de LED blanco es una preferencia visual local de esta sección: no se agrega a args, no se envía a F4 y no se persiste.

## Flujo completo

1. Al conectar y autenticar STM, IrEmittersContext consulta GET; sin conexión, sesión o estado conocido, o en mock, los switches de hardware están bloqueados.
2. Web solicita por JSON v1 `{api:1,type:"request",requestId,command,args}`. ESP conserva requestId y traduce a UNER hacia F4; UNER correlaciona comando/ruta, no lleva requestId propio.
3. F4 aplica `APP_SetIrLightsEnabled` y `APP_ApplyIrState`; el tercer flag corresponde a `Luces_IR_GPIO_Port/Luces_IR_Pin`. Los grupos de cinco/tres conservan `TCRT5000_EmittersGroup5Set/Group3Set`. No cambia código de firmware.
4. SET espera respuesta final por 0x7A y status 0. Aceptar cola o transporte no cambia visualmente el switch como éxito. Mientras está pendiente se bloquean nuevas operaciones. El evento 0x98 puede actualizar el estado, pero no completa la transacción SET.
5. ESP exige sesión STM, timeout 500 ms, GET permite un reintento y SET ninguno; Web usa deadline total de 2000 ms. Los errores se muestran junto a los controles. Al perder disponibilidad se borra el estado conocido y no se dibujan luces blancas encendidas.
6. El visor «Mostrar haces» agrupa los conos delanteros, inferiores y blancos. Con IR `false` se oculta su grupo; sin confirmación se permite una guía IR tenue, sin afirmar emisión. Los blancos aparecen solo con estado lógico `lights=true` confirmado. La confirmación no certifica cableado ni brillo físico.

## Geometría compartida para consumidores

Fuente: `Exports a Blender/V2/Blender_V2_20261005_013012/AutoMicro_V2_ORIGINAL.blend`. Medición de vértices originales y componentes conexas, sin sobrescribir Blender. Conversión: glTF metros Y arriba = `[Blender X, Blender Z, -Blender Y] / 1000`. Coordenadas locales antes de centrar, escalar y aplicar pose; el modelo y todos los anclajes comparten transformación.

| Elemento | X m | Y m | Z m |
| --- | ---: | ---: | ---: |
| TCRT emisor lineLeft | 0.120588668823 | -0.046215000153 | 0.090131706238 |
| TCRT emisor lineCenter | 0.140592773438 | -0.046215000153 | 0.090131706238 |
| TCRT emisor lineRight | 0.160598999023 | -0.046215000153 | 0.090131706238 |
| TCRT receptor lineLeft | 0.120588668823 | -0.046215000153 | 0.087053833008 |
| TCRT receptor lineCenter | 0.140592773438 | -0.046215000153 | 0.087053833008 |
| TCRT receptor lineRight | 0.160598999023 | -0.046215000153 | 0.087053833008 |
| LED whiteLeft | 0.120593673706 | -0.048694999695 | 0.079567771912 |
| LED whiteCenter | 0.140592773438 | -0.048694999695 | 0.079572769165 |
| LED whiteRight | 0.160593994141 | -0.048694999695 | 0.079567771912 |

Emisores TCRT: V2_RAW_0366_00, receptores 0367, LED blancos 0354. Dirección TCRT inferior `[0,-cos(13.5°),-sin(13.5°)]`, inclinada hacia el receptor en -Z. Apertura ±16° a media intensidad, según [Vishay 80393](https://www.vishay.com/docs/80393/windowsizecalcultator_tcrt5000_tcrt5000l.pdf); campo del receptor ±30°, inclinado en sentido opuesto. Límite visual IR 5 cm; no prueba alcance sensado. Los conos se recortan al plano del piso; se dibuja la intersección real de sus rayos con ese plano después de aplicar pose.

LED blancos dirección `[0,-1,0]`. El usuario confirmó componentes genéricos sin ficha: apertura total provisional 20°, ajustable 10°–120° en Web. Referencia orientativa [Kingbright WP7113VW1C, blanco transparente 5 mm](https://www.kingbrightusa.com/product.asp?catalog_name=LED&product_id=WP7113VW1C); no se identifica como el componente instalado. CAD D5.0mm Clear con pestaña cercana a 6 mm no define su apertura. Dibujo orientativo, no cálculo fotométrico.

Piso CAD Y = -0.0534489097595 m; centro vertical de ruedas Y = -0.0199994082451 m. Altura eje–piso = **33.4495015 mm**, en ambas ruedas. Centro del volumen completo Y = -0.009308708832 m, a **44.1402009 mm** del piso. Elevación de modelo centrado: `(centroRealDelGLB.y - pisoCAD) * escala + alturaDelPlano`. Statics usa plano Y=0; Control conserva Y=-0.025 y Seguidor -0.012 unidades de escena. Apoyo nominal en pose nivelada, sin simulación dinámica de contacto. Emisores inferiores a 7.2339096 mm del suelo; LED blancos a 4.7539101 mm.

Medidas completas y centros de ruedas disponibles en Web `docs/model-v2-underbody-measurements.json`; explicación en `docs/model-v2-optics.md`. Las coordenadas esenciales quedan en esta ficha para que otros clones puedan adoptar el dibujo.

## Impacto en repositorios

| Repositorio | Estado | Acción o motivo concreto |
| --- | --- | --- |
| FirmwareF4 | compatible | Handlers 0x79/0x7A y APP_SetIrLightsEnabled/APP_ApplyIrState ya reciben los mismos tres bytes y reportan la misma estructura; sin código cambiado. |
| Firmware ESP01 | compatible | f4_command_registry.c conserva 0x79/0x7A/0x98, privilegio STM y longitudes; ws_bridge/stm_commands.inc empaqueta objects,line,lights y reports.inc los devuelve como booleanos. |
| Web ESP01 | origen | Nuevos switches, anclajes ópticos, manchas, altura CAD y preferencia local de ángulo; fuente editada, gzip y firmware sin regenerar. |
| AutoMicroQtDesktop | compatible | IrEmittersController/F4ApiCodec/WebApiContract ya manejan las tres claves y los mismos IDs; no requieren adaptación de wire. Representar esta geometría en Qt no forma parte de la entrega. |

## Compatibilidad y orden de integración

Es una extensión del consumidor Web sobre AMI-20261002-03; no introduce cambio incompatible. No requiere una nueva versión de F4/ESP/Qt para el mismo contrato. La conexión requiere firmwares que ya implementen aquella ficha; no se certifica cuáles están instalados. La nueva interfaz solo llegará al gzip/data ESP después de una compilación solicitada y al equipo después de carga/flasheo. La copia configurada en AMI-20261005-02 incluirá el GLB vigente y la interfaz compilada.

El archivo GLB no se modifica: los conos se generan en Web y se conserva el modelo de 883.260 bytes de la entrega anterior. El ajuste visual de apertura no altera sensores ni salidas.

## Evidencia y límites

Lectura de fuente F4, ESP, Web y Qt; extracción de geometría original en Blender; edición de fuente/documentación Web. Compatible significa alineación estática concreta de los formatos existentes, no prueba de interoperabilidad. No se ejecutaron tests, compilaciones, validaciones visuales, pruebas eléctricas/fotométricas, despliegue ni flasheo, por instrucción del usuario. Se aplicó visual-identity-system con manual 0.3.0 y autorización previa del borrador; propuestas y cobertura sin revisión visual registradas en Web docs/design-system/emission-controls-2026-10-05.md.

## Actualización de interfaz posterior — 2026-10-05

El usuario reorganizó `/statics`. Se elimina el panel independiente «Control del vehículo / Emisión e iluminación». LED blancos inferiores pasa a un checkbox en Herramientas, con apertura total estimada debajo. Conserva el SET completo, permisos, espera y estado confirmado de esta ficha. Los switches físicos de objetos/línea quedan dentro de Control real. La franja de conexión muestra IR objetos, IR línea y LED blancos activados/desactivados o Sin datos.

La visibilidad del dibujo se separa en checkbox locales «Objetos» y «Ejes línea»; no envían comandos ni alteran flags F4. Las luces blancas continúan dependiendo de `lights=true` confirmado. Se elimina Congelar pose, conservando cuadrícula, origen y MPU Render. Restablecer cámara permanece en Herramientas. Limpiar sensores y Calibrar sensores pasan al mismo bloque de Sensibilidad IR, con calibración como última acción a la derecha; limpiar conserva alcance local de UI, sin borrar configuración F4.

Se agrega Vistas colapsable sobre la escena, con seis vistas principales y ocho isométricas en filas horizontales. Cámara y controles siguen siendo locales. Estado del sistema y Posición estimada pasan a la primera fila inferior; Seguimiento de línea ocupa la fila siguiente. Los tres paneles son colapsables. Contraer solo oculta contenido: no detiene streams, no modifica lecturas ni manda comandos.

Fuente Web actualizada: EstadoSection.tsx, MpuIrScene3D.tsx, IrEmittersControls.tsx, SensorDisclosure.tsx, sensorCameraViews.ts e index.css. La geometría CAD de la entrega anterior se conserva. Aplicación de identidad y cobertura documentadas en Web docs/design-system/sensor-layout-2026-10-05.md. Sin cambio de rutas, códigos OLED, payloads, respuestas, eventos, timeout ni autenticación; F4/ESP/Qt conservan su compatibilidad estática documentada. No se crea un ID de contrato adicional por una redistribución local del consumidor. Sin tests, builds, revisiones visuales ni flasheo por instrucción del usuario; gzip/data ESP no se regeneró.

## Actualización de haces, opacidad y brillo — 2026-10-05

El usuario solicita un desplegable colapsable «Haces» debajo de Sensibilidad IR y sus acciones. Contiene tres sliders independientes de 0 a 100 % para objetos, líneas y luces blancas, cada uno con checkbox «Mostrar». El desplegable inicia cerrado; contraerlo conserva los ajustes y el dibujo. Las dos casillas IR comparten estado con «Objetos» y «Ejes línea» de Herramientas. La casilla de haces blancos solo oculta su representación y no apaga la salida del vehículo.

Antes, los conos de objetos modificaban temporalmente su opacidad al ajustar sensibilidad y los inferiores aplicaban una opacidad dependiente del ADC/estado. Ahora la opacidad de cada grupo se aplica directamente desde su slider: objetos y líneas parten ambos de 2 %, blancos de 65 %. ADC y sensibilidad conservan sus funciones de lectura/representación sin alterar esta opacidad. Se retira el contorno adicional del cono de objetos para usar la misma composición de transparencia que en los inferiores. Los ajustes son locales a la sección y se reinician al volver a abrirla; no se añaden a calibración física ni al protocolo.

Las luces blancas incorporan brillo en los emisores CAD, luz dirigida hacia abajo y halo suave en la intersección con el suelo. Se añade una superficie de escena que recibe iluminación. Los efectos comparten las transformaciones del auto y la opacidad elegida. Solo aparecen cuando existe lights=true confirmado, el grupo está visible y su opacidad es mayor que cero. Ocultar el grupo, elegir 0 %, apagar el LED o perder el estado confirmado retira esos efectos del dibujo; únicamente el checkbox de hardware conserva el SET completo de esta ficha. El halo es un efecto visual orientativo para LED genéricos, sin medición fotométrica ni nueva afirmación de alcance.

Se mantienen anclajes, alturas CAD, límite visual de 5 cm, inclinaciones TCRT y apertura blanca estimada ajustable. No cambian stm.irEmitters.get/set/changed, sus IDs 0x79/0x7A/0x98, campos, respuestas, permisos ni timeouts. F4, ESP y Qt no requieren adaptación de wire. Fuente Web: SensorBeamControls.tsx, beamVisualSettings.ts, SensorDisclosure.tsx, EstadoSection.tsx, MpuIrScene3D.tsx, UnderbodySensorBeams.tsx e index.css. Aplicación de identidad y cobertura registradas en docs/design-system/sensor-layout-2026-10-05.md; comportamiento descrito en docs/model-v2-optics.md.

Solo se editaron fuente/documentación Web y este registro. Sin tests, compilaciones, validaciones visuales o físicas, despliegue ni flasheo por instrucción del usuario. El GLB permanece igual; gzip/data ESP no se regeneró. Esta extensión local actualiza AMI-20261005-03 sin crear un nuevo comando ni ID de contrato.

## Corrección de desplegables, herramientas, vistas y gráfica — 2026-10-05

Por pedido del usuario, todos los desplegables de MPU + IR empiezan cerrados: Vistas, Haces, Estado del sistema, Posición estimada, Seguimiento de línea y la gráfica; Control real/Simulado conservan su inicio cerrado. SensorDisclosure adopta ese valor inicial compartido. Contraer bloques sigue siendo una acción visual local, sin detener suscripciones ni modificar el estado físico.

Herramientas cambia «Objetos» por «Haz objetos» y «Ejes línea» por «Haz líneas». El checkbox de hardware se llama «LED blanco» y se ubica inmediatamente debajo de Haz líneas, antes de Esconder origen. La apertura total estimada blanca se mueve a Haces, dentro del grupo de opacidad blanca. Sigue siendo una preferencia local sin payload, persistencia ni efecto eléctrico. LED blanco conserva stm.irEmitters.set con los tres booleanos y el estado confirmado definidos en esta ficha.

Vistas incorpora iconos SVG proyectados desde las direcciones del catálogo. Las seis principales y los dos grupos de cuatro isométricas distribuyen botones en columnas iguales sobre todo el ancho con gaps y se reorganizan en móvil. Un movimiento manual de OrbitControls, incluyendo giro, desplazamiento o zoom, elimina la selección de preset y muestra «Vista libre». Elegir un preset o Restablecer cámara establece la vista programática sin provocar esa deselección. No cambia geometría CAD, pose MPU ni rutas.

Antes, «Lecturas MPU en tiempo real» era un panel abierto que muestreaba la pose efectiva del modelo, incluyendo valores manuales o simulados y repeticiones de la última lectura. Ahora «Gráfica en tiempo real» es un desplegable cerrado y bloqueado si no hay WebSocket conectado, modo real y sesión STM autenticada; muestra «No conectada» debajo. Al perder esa disponibilidad se cierra y limpia el historial local; reconectar no lo abre. Su adquisición visual requiere Flujo MPU real solicitado, estado activo y una muestra real válida de la conexión actual. Consume Euler recibido de la placa, independiente del offset/desacople del modelo; agrega puntos solo si hay una muestra nueva. Sin muestras presenta guiones y espera, sin asumir ceros medidos. Con adquisición detenida indica cómo activar Flujo MPU.

Abrir/cerrar la gráfica no crea ni libera suscripciones y no envía requests nuevos. Se conserva el broker existente y sus comandos/respuestas MPU, así como el contrato de emisores 0x79/0x7A/0x98. No cambian IDs, versiones, formatos, autenticación remota, timeouts ni códigos OLED. F4/ESP/Qt conservan los formatos documentados; esta corrección afecta solo a la presentación y uso local de datos en Web. No afirma detección eléctrica permanente: disponibilidad usa el enlace y la sesión STM ya reportados, y la gráfica exige muestras válidas para adquirir.

Fuente Web modificada: EstadoSection.tsx, SensorDisclosure.tsx, SensorCameraViewIcon.tsx, RealtimeEulerPanel.tsx, IrEmittersControls.tsx, SensorBeamControls.tsx, MpuIrScene3D.tsx e index.css. Identidad manual 0.3.0 y cobertura propuesta documentadas en docs/design-system/sensor-layout-2026-10-05.md; comportamiento actual en docs/model-v2-optics.md y docs/telemetry-session-protocol.md. Sin tests, compilaciones, validaciones visuales/físicas, despliegue ni flasheo por instrucción del usuario; gzip/data ESP no se regeneró. Se mantiene AMI-20261005-03 sin un nuevo ID de wire.

## Preview blanco y detección axial de objetos — 2026-10-05

El usuario pide ver apertura y opacidad blancas mientras las ajusta aunque el LED esté apagado, y representar la detección de cada objeto con una burbuja sobre el eje de su cono. Es una actualización local de Web sobre esta ficha; no agrega comandos ni campos de distancia al protocolo.

Antes, el dibujo blanco dependía exclusivamente de lights=true confirmado y Mostrar. Ahora los sliders de apertura/opacidad activan una vista previa explícita con los valores actuales: los tres conos, brillo y manchas se dibujan temporalmente incluso con lights=false, estado desconocido o dibujo oculto. La opacidad cero se respeta. La interacción funciona con puntero/teclado y mantiene el preview 1500 ms tras finalizar; un ajuste nuevo reinicia esa espera. Haces y la escena identifican la vista previa. No cambia lights ni Mostrar, no modifica el estado confirmado de la franja, no envía GET/SET ni crea suscripciones. Al terminar se recupera la regla normal de disponibilidad/estado físico. La apertura permanece estimada para LED genéricos, sin cambio de geometría CAD ni significado fotométrico.

Antes, ObstacleRay colocaba una esfera desde un mapeo lineal de intensidad con un desplazamiento mínimo del 22 % del cono, y la esfera heredaba su opacidad: con el 2 % quedaba casi invisible. Ahora cada sensor usa una burbuja con opacidad propia, disco tenue y anillo perpendicular al eje. El centro local desde su emisor CAD es [0,0,-d_cm/100], con el yaw y la misma escala/pose del modelo. El radio del corte es d_m*tan(16 grados); la esfera usa una fracción de ese radio con límites visuales pequeños. Se mantienen los materiales de los conos de objetos y líneas solicitados previamente. Ocultar Haz objetos o objects=false confirmado oculta sus marcadores; poner el cono al 0 % no quita por sí solo una detección válida.

La autoridad de la lectura sigue siendo el ADC IR existente, publicado en irSample y UNER 0x91 de 56 bytes. Web deriva únicamente una estimación axial por sensor: umbral delta ADC >=130 respecto de baseline/polaridad. Con dos o más puntos de curva del sensor, invierte segmentos lineales sin extrapolar y exige una única distancia dentro de la sensibilidad. Múltiples cruces/mesetas se indican como Ambigua; lecturas fuera de la curva, Fuera curva; distancia fuera de sensibilidad, Fuera rango. Esos casos no reciben una posición arbitraria. Sin curva suficiente invierte el modelo visual axial existente: d=R*(1-fuerza^(1/1.6)), asumiendo objeto centrado y reflectividad unitaria. No es una medición de distancia de F4 ni acredita alcance físico.

Las barras sobre la escena conservan ADC y agregan distancia aproximada por sensor, indicada con el símbolo aproximado y cm estimados, o el estado de ambigüedad/rango/curva. Solo se dibujan marcadores con muestra válida disponible y detección; limpiar datos vacíos, detener el stream real o perder disponibilidad los retira. La simulación utiliza sus muestras de la misma manera. No se modifica el perfil de calibración almacenado, la estimación conjunta anterior ni el broker/propiedad de streams de AMI-20261005-04.

Fuente Web: useWhiteBeamPreview.ts, SensorBeamControls.tsx, EstadoSection.tsx, MpuIrScene3D.tsx, irObjectModel.ts e index.css. Comportamiento y límites en docs/model-v2-optics.md; cobertura visual propuesta en docs/design-system/sensor-layout-2026-10-05.md. stm.irEmitters.get/set/changed y sus IDs 0x79/0x7A/0x98, requests, respuestas, eventos, permisos y timeouts siguen iguales. F4/ESP/Qt no necesitan adaptación de wire y no se editó su código en esta tarea. El GLB se conserva.

Implementado en fuente/documentación; sin tests, compilaciones, validaciones visuales o físicas, despliegue ni flasheo por pedido del usuario. gzip/data ESP no se regeneró. Se actualiza AMI-20261005-03, sin un nuevo ID de contrato para este comportamiento visual local.

## Apertura blanca fija e iluminación de piso y modelo — 2026-10-05

El usuario informa que el efecto blanco no aparece al modificar la opacidad y solicita fijar su apertura total en 30 grados. Se retira el slider de apertura y su estado local: WHITE_LED_FULL_ANGLE_DEG define 30 grados totales, con semiangulo de 15 grados. Haces muestra este valor como información fija junto al control de opacidad. Es un valor de representación solicitado para los LED genéricos; no identifica el componente instalado ni acredita su ficha fotométrica. Sustituye la apertura provisional ajustable descrita en las secciones anteriores.

La opacidad blanca controla conjuntamente el brillo de los tres emisores CAD, los conos dirigidos hacia abajo, las manchas del suelo y la iluminación de rebote sobre el modelo. Su interacción conserva la vista previa con puntero/teclado y los 1500 ms posteriores, incluso cuando lights=false, el estado es desconocido o Mostrar está desactivado. No enciende el LED físico ni envía requests. Fuera de esa vista previa se mantiene la disponibilidad y lights=true confirmado, junto con Mostrar y opacidad mayor que cero. Elegir cero retira todos los efectos blancos.

El GLB V2 incluye transmisión en V2_Clear. La lectura del código local de Three muestra que su pase de transmisión toma objetos opacos y transmisivos, pero excluye la lista transparente; los halos blancos anteriores pertenecían a esa lista. La representación blanca pasa a composición aditiva en el pase opaco, después de las superficies opacas, con intensidad y caída radial en RGB. Así puede participar en el fondo usado por el acrílico. Conserva prueba de profundidad y no escribe profundidad; no se modifica el material del GLB. Los haces IR conservan su composición transparente.

El emisor blanco está a 4,753910 mm del piso en la pose nominal, por lo que el corte directo de un cono de 30 grados es pequeño. Ese corte se conserva y se añade un halo de reflexión de radio mínimo 25 mm como feedback visual. Cada LED usa luz dirigida al piso y una luz de rebote de alcance visual 75 mm para iluminar las superficies del auto cercanas al suelo. La intensidad y el alcance se ajustan a la escala del modelo; estas magnitudes son efectos visuales, no potencia medida, simulación fotométrica ni nuevo alcance sensado. Las coordenadas CAD, apoyo de ruedas y GLB permanecen iguales; esta corrección no altera orientación, apertura ni detección de los TCRT.

Fuente Web: irEmitterGeometry.ts, UnderbodySensorBeams.tsx, SensorBeamControls.tsx, EstadoSection.tsx, MpuIrScene3D.tsx e index.css. Medidas y explicación actualizadas en docs/model-v2-underbody-measurements.json y docs/model-v2-optics.md; identidad y cobertura propuesta en docs/design-system/sensor-layout-2026-10-05.md y components.md.

No cambian stm.irEmitters.get/set/changed, IDs 0x79/0x7A/0x98, campos objects/line/lights, formatos de respuesta/evento, permisos ni timeouts. F4, ESP y Qt conservan el contrato existente y su código no se editó para esta corrección. Solo se modifican fuente/documentación Web y esta ficha con su entrada central. Lectura de fuente/GLB y edición implementadas; sin tests, compilaciones, validaciones visuales/físicas, despliegue ni flasheo por instrucción del usuario. gzip/data ESP no se regeneró. Se mantiene AMI-20261005-03 sin un nuevo ID de wire.
## Vistas compactas en dos bloques — 2026-10-05

Antes, Vistas presentaba tres grupos que ocupaban filas completas y etiquetas
largas. Ahora una grilla exterior reúne Principales e Isométricas lado a lado
en una fila de escritorio. Las seis principales forman una grilla de 3 por 2,
con frente/trasera, izquierda/derecha y arriba/abajo emparejados por columna.
Las ocho isométricas comparten un bloque: cuatro superiores y cuatro inferiores,
con etiquetas Sup./Inf. y grillas internas. Los botones usan etiquetas abreviadas
e iconos menores; title, nombre accesible y detalle seleccionado conservan el
nombre completo. En pantallas estrechas se apilan los bloques y en móvil pequeño
las filas isométricas pasan a dos columnas. Se conservan tokens, foco y área táctil.

Es presentación local Web: elegir un botón sigue restaurando su preset y mover
manualmente sigue deseleccionándolo. Se conservan los catorce IDs, posiciones,
orientaciones y acciones de cámara. No cambia colapso inicial, geometría, pose,
telemetría, emisores, solicitudes, payloads, respuestas, eventos, permisos ni
códigos OLED. FirmwareF4, Firmware ESP01 y Qt no necesitan adaptación y su código
no se modifica para este ajuste. No se introduce otro ID de contrato.

Fuente Web: EstadoSection.tsx, features/ir/sensorCameraViews.ts e index.css.
Identidad y cobertura en docs/design-system/sensor-layout-2026-10-05.md. Código
Web local sin publicar; solo se publica esta actualización documental y su índice.
Sin tests, compilaciones, validaciones visuales/físicas, deploy ni flasheo por
instrucción del usuario. Bundle/gzip/data ESP sin regenerar.

## Gráfica simulada y movimiento automático por eje — 2026-10-05

Antes, Gráfica en tiempo real solo estaba disponible con conexión real y sesión
STM. Ahora Emulador MPU la habilita sin placa, la abre e identifica como Simulada.
Usa Euler local, incluyendo ajustes manuales y ejes fijos, muestreado al intervalo
seleccionado para simulación. Puede contraerse. Cambiar real/simulado crea un
historial nuevo; una desconexión no interrumpe la curva local. La fuente real
conserva sus requisitos de conexión/sesión, stream MPU solicitado/activo y muestra
válida de la conexión actual; sigue agregando únicamente versiones nuevas.
El gráfico consume la fuente MPU elegida, independiente de offset o cámara.

Antes, Movimiento automático reemplazaba los tres ángulos en cada actualización.
Ahora cada slider tiene una casilla Auto/Fijo inicialmente marcada. Desmarcar un
eje conserva su ángulo actual y excluye ese eje de actualizaciones automáticas;
slider, rotaciones rápidas y Restablecer siguen disponibles para ajustarlo.
Marcarlo de nuevo lo reintegra al generador. El interruptor global conserva su
función y las elecciones por eje duran mientras la pantalla esté montada. Con
los tres ejes desmarcados se informa Todos los ejes fijos. Nuevos controles usan
borde, foco, acento y superficie semánticos del manual visual autorizado.

Son cambios locales Web. No cambian requests, payloads, IDs, respuestas, eventos,
permisos, timeouts ni códigos OLED. La gráfica y las casillas no crean/liberan
suscripciones: se conserva el flujo existente al entrar/salir de Emulador MPU.
No se modifica código F4, ESP ni Qt; estos consumidores no necesitan adaptación
por el muestreo local y el filtro de ejes. Sin nuevo ID de contrato.

Fuente Web: EstadoSection.tsx, RealtimeEulerPanel.tsx, OrientationControls.tsx,
MockEulerGenerator.tsx e index.css. Comportamiento y cobertura en telemetry-session-
protocol.md y design-system/sensor-layout-2026-10-05.md dentro de docs Web.
Implementación local sin publicar; registro central publicado por separado.
Sin tests, compilación, validación visual/física, tráfico, deploy ni flasheo por
instrucción del usuario. Bundle/gzip/data ESP sin regenerar.

## LED blancos binarios y 30° desde el eje central — 2026-10-06

El usuario pide quitar el haz blanco y controlar el LED únicamente como encendido
o apagado con un toggle, con 30 grados desde el centro del LED. Se corrige la
interpretación anterior de apertura total: ahora el semiángulo desde el eje es
**30°**, equivalente a **±30° y 60° de apertura total**. Es un parámetro visual
solicitado para los LED genéricos; no acredita su óptica física.

En Web, LED blancos usa el toggle compartido en Herramientas. Conserva el SET
existente de `lights`, los otros dos booleanos, el estado confirmado, el bloqueo
durante la espera y los mensajes de error/consulta. La escena solo ilumina cuando
el equipo está disponible y `lights=true` está confirmado. Apagado o pérdida de
disponibilidad retiran los efectos. No se agrega un estado optimista ni simulado.

Se eliminan la malla del cono blanco, el slider de opacidad blanca, su casilla
Mostrar, el estado visual independiente y el hook/etiquetas de preview. Haces
conserva únicamente objetos y líneas con sus sliders y visibilidad locales.
Los blancos mantienen brillo del emisor, luz dirigida, mancha del piso y rebote
del modelo con ganancia visual fija. SpotLight e intersección directa con el
piso comparten el semiángulo de 30° y los anclajes CAD/pose existentes. Se
conservan escala, geometría del vehículo y efectos IR.

Fuente Web: IrEmittersControls.tsx, SensorBeamControls.tsx,
UnderbodySensorBeams.tsx, MpuIrScene3D.tsx, EstadoSection.tsx,
beamVisualSettings.ts, irEmitterGeometry.ts e index.css. Se retira
useWhiteBeamPreview.ts. Documentación actualizada en model-v2-optics.md,
model-v2-underbody-measurements.json y los apuntes de controles/cobertura del
sistema visual. Esta sección reemplaza los comportamientos blancos anteriores
de apertura total 30°, opacidad y preview; se conserva el historial.

No cambian `stm.irEmitters.get/set/changed`, IDs 0x79/0x7A/0x98, flags
objects/line/lights, requests, respuestas, eventos, autenticación ni timeouts.
F4, ESP y Qt no requieren adaptación de contrato y su código no se modifica
para este pedido. Se actualiza AMI-20261005-03 y su índice, sin otro ID de wire.

Implementado en fuente/documentación Web local; el registro central se publica
por separado. Sin tests, compilaciones, validaciones visuales/físicas, tráfico,
deploy ni flasheo por instrucción del usuario. Bundle/gzip/data ESP pendiente
de regeneración y carga para llevar la interfaz al equipo.
