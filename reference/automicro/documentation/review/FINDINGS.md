# Registro de inconsistencias y revisiones de código

Fuente editorial: `findings.json`. IDs persistentes; no se eliminan al cerrar. Fotografía inicial: 2026-10-20. Lectura de código, sin tests, compilación ni ejecución de paridad. Los fallos condicionales y revisiones no equivalen a incidentes observados en hardware.

| ID | Prioridad | Estado / certeza | Hallazgo |
| --- | --- | --- | --- |
| [AMDOC-F4-001](#amdoc-f4-001) | alta | abierto / observed | Escala cero aceptada por el encoder |
| [AMDOC-F4-002](#amdoc-f4-002) | media | abierto / observed | El acumulador de pulsos reduce un delta de 32 bits a 8 bits |
| [AMDOC-F4-003](#amdoc-f4-003) | alta | abierto / resolved | La marca RESP permite longitudes fuera del rango de solicitud |
| [AMDOC-F4-004](#amdoc-f4-004) | alta | abierto / observed | BuildFrame acepta payload nulo con longitud positiva |
| [AMDOC-F4-005](#amdoc-f4-005) | media | abierto / observed | La prioridad registrada del dispositivo no participa del arbitraje |
| [AMDOC-F4-006](#amdoc-f4-006) | media | abierto / contradicted | 0x74 declara ACK en F4 pero el handler responde por el mismo comando |
| [AMDOC-F4-007](#amdoc-f4-007) | media | revisar / observed | Los contextos activos de UI limitan independencia y reentrada |
| [AMDOC-F4-008](#amdoc-f4-008) | media | revisar / observed | El reloj now_us tiene resolución efectiva de milisegundos |
| [AMDOC-F4-009](#amdoc-f4-009) | baja | revisar / observed | GetScaledSteps devuelve 1 con desplazamiento cero |
| [AMDOC-F4-010](#amdoc-f4-010) | baja | abierto / observed | sampleTenMs se almacena pero no gobierna el muestreo |
| [AMDOC-ESP-001](#amdoc-esp-001) | media | abierto / resolved | El validador AutoMotion acepta formas de argumentos de otros comandos |
| [AMDOC-WEB-001](#amdoc-web-001) | media | reportado / observed-partial | Docs requiere tercer destino ESP y selector visual estable |
| [AMDOC-ESP-002](#amdoc-esp-002) | media | abierto / resolved | Los reportes sin selección publican selectedIndex/itemCount como cero |
| [AMDOC-WEB-002](#amdoc-web-002) | alta | abierto / resolved | El visor rellena estados que el reporte de pantalla no transporta |
| [AMDOC-WEB-003](#amdoc-web-003) | alta | abierto / resolved | El renderer SVG actual no constituye el port completo de OLED F4 |
| [AMDOC-WEB-004](#amdoc-web-004) | alta | abierto / resolved | La ventana del menú reconstruida en Web puede ejecutar otra fila en F4 |
| [AMDOC-WEB-005](#amdoc-web-005) | media | abierto / resolved | Normalizadores del visor pierden el modo GOTO |
| [AMDOC-WEB-006](#amdoc-web-006) | alta | abierto / resolved | Los builders de sensores no reciben el mismo modelo que la telemetría actual |
| [AMDOC-WEB-007](#amdoc-web-007) | baja | revisar / observed | El umbral de pulsación larga del visor difiere del botón físico |
| [AMDOC-WEB-008](#amdoc-web-008) | baja | abierto / observed | Dos identidades de acción F4 no están declaradas en screenCodes Web |
| [AMDOC-WEB-009](#amdoc-web-009) | media | abierto / observed | El catálogo de línea no tiene bindings específicos de visor localizados |
| [AMDOC-F4-011](#amdoc-f4-011) | media | abierto / resolved | USER largo sale del OSK por el router sin ejecutar su cancelación |
| [AMDOC-F4-012](#amdoc-f4-012) | media | abierto / resolved | Inverse cambia de semántica entre píxel y líneas rápidas OLED |

<a id="amdoc-f4-001"></a>
## AMDOC-F4-001 — Escala cero aceptada por el encoder

**Clasificación:** guarda de argumentos ausente. **Estado:** abierto. **Certeza:** observed.

**Observación:** ENC_Init guarda calibrateCountPerStep sin rechazar cero; ENC_Task_N10ms divide y calcula módulo por ese campo.

**Disparador y efecto:** Un integrador que inicialice escala 0 llega a división por cero al muestrear. El fallo bajo ese argumento se desprende del código; no se afirma que la configuración actual lo envíe.

**Cambio a considerar:** Definir rechazo o valor seguro para cero y hacer observable el fallo de inicialización; conservar escala válida antes de activar tareas.

**Código que habría que revisar:**

- `f4:Core/Src/encoder.c:ENC_Init`
- `f4:Core/Src/encoder.c:ENC_Task_N10ms`
- `f4:Core/Inc/encoder_types.h:ENC_Handle_t.calibrateCountPerStep`

**Puntos documentales que deben cambiar junto con el código:**

- [Encoder / inicialización](../published/f4/libraries/encoder.md#fn-enc_init) · selector estable: `f4:module/Core/Inc/encoder.h / functions[name=ENC_Init]`.
- [Encoder / ciclo de vida](../published/f4/libraries/encoder.md#lifecycle) · selector estable: `f4:module/Core/Inc/encoder.h / lifecycle`.

**Repositorios relacionados:** FirmwareF4.

**Cierre:** Pendiente de decisión e implementación. Al corregir, actualizar overlay de encoder, regenerar solo esa ficha y registrar commit; prueba únicamente con autorización.

<a id="amdoc-f4-002"></a>
## AMDOC-F4-002 — El acumulador de pulsos reduce un delta de 32 bits a 8 bits

**Clasificación:** pérdida potencial de conteo. **Estado:** abierto. **Certeza:** observed.

**Observación:** El delta con wrap se calcula como int32_t y se suma a accumCount, declarado int8_t, antes de extraer pasos completos.

**Disparador y efecto:** Si delta más resto supera el rango int8_t, la conversión pierde información y puede producir conteo o sentido incorrecto. No se midió si el montaje actual alcanza ese rango entre muestreos.

**Cambio a considerar:** Conservar acumulación en un tipo suficientemente ancho y decidir límites explícitos antes de derivar pasos; revisar compatibilidad de getters históricos unsigned.

**Código que habría que revisar:**

- `f4:Core/Inc/encoder_types.h:ENC_Handle_t.accumCount`
- `f4:Core/Src/encoder.c:ENC_Task_N10ms`

**Puntos documentales que deben cambiar junto con el código:**

- [Encoder / estado](../published/f4/libraries/encoder.md#state) · selector estable: `f4:module/Core/Inc/encoder.h / state; types[name=ENC_Handle_t]`.
- [Encoder / muestreo](../published/f4/libraries/encoder.md#fn-enc_task_n10ms) · selector estable: `f4:module/Core/Inc/encoder.h / functions[name=ENC_Task_N10ms]`.

**Repositorios relacionados:** FirmwareF4.

**Cierre:** Sin cambio de código. Si se amplía tipo, actualizar atributos y explicación del cálculo, sin alterar la convención física/remota por implicación.

<a id="amdoc-f4-003"></a>
## AMDOC-F4-003 — La marca RESP permite longitudes fuera del rango de solicitud

**Clasificación:** frontera de parsing y despacho. **Estado:** abierto. **Certeza:** resolved.

**Observación:** UNER_Handle_IsLengthValid acepta cualquier len>=1 cuando el CommandSpec tiene UNER_SPEC_F_RESP, aunque no cumpla min_args/max_args. La condición no distingue por sí misma solicitud entrante de respuesta.

**Disparador y efecto:** Se siguió un camino concreto: 0x24 con len=1 cumple la excepción RESP, llega al switch y APP_AutoMotionRequest copia dos floats de 4 bytes sin comprobar length. 0x28 y el control de calibración de línea también leen campos sin guarda local de longitud. Eso permite leer fuera de los bytes válidos declarados del paquete, posiblemente datos residuales del slot. No equivale por sí mismo a demostrar acceso fuera de toda memoria asignada ni un fallo físico; siguen aplicándose autorización y otras guardas.

**Cambio a considerar:** Separar reglas de solicitud y respuesta según ruta/propósito, y mantener guardas locales de tamaño en cada handler. Revisar también paquetes asincrónicos/proxy para no bloquear respuestas legítimas.

**Código que habría que revisar:**

- `f4:Core/Src/uner_handle.c:UNER_Handle_IsLengthValid`
- `f4:Core/Src/uner_handle.c:UNER_Handle_ProcessPending`
- `f4:Core/Src/uner_app.c:uner_commands/UNER_App_ExecuteCommand`
- `f4:Core/Src/app_tasks.c:APP_AutoMotionRequest`

**Puntos documentales que deben cambiar junto con el código:**

- [Router / validación de longitud](../published/f4/libraries/uner-router.md#fn-uner_handle_islengthvalid) · selector estable: `f4:module/Core/Inc/uner_handle.h / internalFunctions[name=UNER_Handle_IsLengthValid]`.
- [Aplicación / decodificación GOTO](../published/f4/libraries/application-composition.md#fn-app_automotionrequest) · selector estable: `f4:module/Core/Inc/app_tasks.h / functions[name=APP_AutoMotionRequest]`.
- [Contrato / longitudes](../published/shared/wire-contracts.md#length-guards) · selector estable: `shared:wire / length-guards`.

**Repositorios relacionados:** FirmwareF4, Firmware ESP01, Web ESP01, AutoMicroQtDesktop.

**Cierre:** Revisión de handlers pendiente. Si cambia aceptación/rechazo observable en el wire, crear ficha AMI y actualizar estados reales de consumidores.

<a id="amdoc-f4-004"></a>
## AMDOC-F4-004 — BuildFrame acepta payload nulo con longitud positiva

**Clasificación:** guarda de buffer ausente. **Estado:** abierto. **Certeza:** observed.

**Observación:** UNER_BuildFrame solo copia payload si len>0 y payload!=NULL; con len positivo y puntero nulo conserva bytes previos del buffer y calcula XOR sobre ellos. UNER_Send tampoco rechaza esa combinación.

**Disparador y efecto:** Un llamador con esa combinación puede emitir un frame de checksum válido con datos residuales en lugar de rechazar argumentos. No se localizó en esta revisión una llamada activa que la produzca.

**Cambio a considerar:** Rechazar len>0 con payload nulo antes de construir el frame; mantener permitido payload nulo con len=0.

**Código que habría que revisar:**

- `f4:Core/Src/uner_core.c:UNER_BuildFrame`
- `f4:Core/Src/uner_handle.c:UNER_Send`

**Puntos documentales que deben cambiar junto con el código:**

- [Core / construcción de frame](../published/f4/libraries/uner-core.md#fn-uner_buildframe) · selector estable: `f4:module/Core/Inc/uner_core.h / functions[name=UNER_BuildFrame]`.
- [Router / envío](../published/f4/libraries/uner-router.md#fn-uner_send) · selector estable: `f4:module/Core/Inc/uner_handle.h / functions[name=UNER_Send]`.

**Repositorios relacionados:** FirmwareF4.

**Cierre:** Sin corrección aplicada. Actualizar precondiciones y retorno documentado junto al cambio del guard.

<a id="amdoc-f4-005"></a>
## AMDOC-F4-005 — La prioridad registrada del dispositivo no participa del arbitraje

**Clasificación:** API y política de planificación. **Estado:** abierto. **Certeza:** observed.

**Observación:** RegisterDevice conserva priority en la entrada, pero effective_prio usa pending_prio y policy; la prioridad base guardada no interviene en esa selección.

**Disparador y efecto:** Cambiar priority al registrar un dispositivo no produce la preferencia que el parámetro sugiere. La prioridad de request y las políticas OLED/MPU sí tienen efecto.

**Cambio a considerar:** Decidir si priority debe combinarse con la solicitud o si se retira/depreca como parámetro sin efecto. No cambiar la equidad ni prioridad MPU por accidente.

**Código que habría que revisar:**

- `f4:Core/Src/i2c_manager.c:I2C_Manager_RegisterDevice`
- `f4:Core/Src/i2c_manager.c:effective_prio`
- `f4:Core/Src/i2c_manager.c:pick_next_pending_index`

**Puntos documentales que deben cambiar junto con el código:**

- [I2C / política](../published/f4/libraries/i2c-manager.md#modularity) · selector estable: `f4:module/Core/Inc/i2c_manager.h / modularity; types[name=I2C_DeviceEntry]`.

**Repositorios relacionados:** FirmwareF4.

**Cierre:** Falta decisión de política. Documentar después fórmula efectiva y significado de cada prioridad.

<a id="amdoc-f4-006"></a>
## AMDOC-F4-006 — 0x74 declara ACK en F4 pero el handler responde por el mismo comando

**Clasificación:** contradicción de metadata. **Estado:** abierto. **Certeza:** contradicted.

**Observación:** uner_commands registra SET_EXTENSION_PROFILE con UNER_SPEC_F_ACK. HandleSetExtensionProfile envía cmd 0x74 y un byte status. El registro actual ESP ya usa F4_RESPONSE_SAME_COMMAND para ese ID.

**Disparador y efecto:** Un consumidor/generador que derive finalización de la tabla F4 puede esperar un ACK inexistente. No se atribuye ese fallo al ESP actual, que describe SameCommand. Qt no fue reinspeccionado aquí.

**Cambio a considerar:** Alinear metadata con la respuesta efectiva y documentar diferencias entre flags F4 y completion policy; no cambiar el wire actual solo para igualarlo a una bandera obsoleta.

**Código que habría que revisar:**

- `f4:Core/Src/uner_app.c:uner_commands[UNER_CMD_ID_SET_EXTENSION_PROFILE]`
- `f4:Core/Src/uner_app.c:UNER_App_HandleSetExtensionProfile`
- `esp:src/f4_command_registry.c:kF4Commands[0x74]`

**Puntos documentales que deben cambiar junto con el código:**

- [Aplicación / perfil](../published/f4/libraries/uner-application.md#fn-uner_app_handlesetextensionprofile) · selector estable: `f4:module/Core/Inc/uner_app.h / internalFunctions[name=UNER_App_HandleSetExtensionProfile]`.
- [Contrato / finalización](../published/shared/wire-contracts.md#completion) · selector estable: `shared:wire / completion`.
- [Catálogo de registro](../published/shared/command-registry.md#command-0x74) · selector estable: `shared:registry / command[id=0x74]`.

**Repositorios relacionados:** FirmwareF4, Firmware ESP01, AutoMicroQtDesktop.

**Cierre:** No se reabre como vigente el choque histórico Qt sin revisar su código actual. Actualizar esta entrada y metadata tras la corrección.

<a id="amdoc-f4-007"></a>
## AMDOC-F4-007 — Los contextos activos de UI limitan independencia y reentrada

**Clasificación:** límite arquitectónico. **Estado:** revisar. **Certeza:** observed.

**Observación:** Screens_Context, Permission_Context, Favorites_Context, UiRouter_Context y Ap_Context seleccionan instancias globales. Sus handles conservan SystemHandle/MenuSystem u otros tipos concretos aunque las operaciones se inyecten.

**Disparador y efecto:** El código no cumple independencia completa de modelos ni varias instancias simultáneas por API. No es un fallo demostrado del uso actual con un contexto; sí afecta un port o refactor que asuma reentrada.

**Cambio a considerar:** Si se requiere portabilidad/múltiples instancias, diseñar APIs con handle explícito y modelos de vista estrechos; conservar wrappers históricos solo en integración.

**Código que habría que revisar:**

- `f4:Core/Inc/screen_context.h:Screens_Handle/Screens_Context`
- `f4:Core/Inc/screens_internal.h`
- `f4:Core/Inc/permission_context.h`
- `f4:Core/Inc/favorites_context.h`
- `f4:Core/Inc/uirouter_context.h`
- `f4:Core/Inc/ap_context.h`

**Puntos documentales que deben cambiar junto con el código:**

- [Pantallas / modularidad](../published/f4/libraries/screen-context.md#modularity) · selector estable: `f4:module/Core/Inc/screen_context.h / modularity; state`.
- [Arquitectura / clasificación](../published/f4/architecture.md#boundaries) · selector estable: `f4:architecture / boundaries`.

**Repositorios relacionados:** FirmwareF4.

**Cierre:** Refactor no autorizado en esta etapa. Cada ficha afectada lista este ID; actualizar las cinco fronteras documentales si se rediseñan.

<a id="amdoc-f4-008"></a>
## AMDOC-F4-008 — El reloj now_us tiene resolución efectiva de milisegundos

**Clasificación:** precisión temporal del adaptador. **Estado:** revisar. **Certeza:** observed.

**Observación:** App_I2cUs devuelve App_I2cMs(ctx)*1000 y App_I2cMs usa HAL_GetTick. El nombre/unidad son microsegundos, pero no aporta resolución submilisegundo.

**Disparador y efecto:** Cuantiza intervalos de muestra y puede afectar algoritmos que supongan precisión de microsegundos; magnitud física del efecto no medida. No es un error de conversión de unidad.

**Cambio a considerar:** Decidir si la precisión actual basta o si debe usarse un contador de mayor resolución, definiendo wrap y contexto de lectura.

**Código que habría que revisar:**

- `f4:Core/Inc/app_i2c_ports.h:App_I2cUs`
- `f4:Core/Src/mpu9250.c:MPU_UpdateSampleTiming`

**Puntos documentales que deben cambiar junto con el código:**

- [Adaptadores / reloj](../published/f4/libraries/application-adapters.md#fn-app_i2cus) · selector estable: `f4:module/Core/Inc/app_stm32_ports.h / functions[name=App_I2cUs]`.
- [MPU / temporización](../published/f4/libraries/mpu9250.md#lifecycle) · selector estable: `f4:module/Core/Inc/mpu9250.h / lifecycle; limitations`.

**Repositorios relacionados:** FirmwareF4.

**Cierre:** Sin mediciones ni cambio de reloj. Actualizar unidad, resolución y wrap juntos si se cambia el backend.

<a id="amdoc-f4-009"></a>
## AMDOC-F4-009 — GetScaledSteps devuelve 1 con desplazamiento cero

**Clasificación:** precondición implícita. **Estado:** revisar. **Certeza:** observed.

**Observación:** ENC_GetScaledSteps parte de multiplier=1 y no trata steps==0. Encoder_MainTask lo llama bajo ENC_FLAG_UPDATED, por lo que existe una guarda en ese consumidor actual.

**Disparador y efecto:** Otro consumidor que use el getter como delta puede fabricar movimiento lógico en reposo. No se afirma un evento fantasma actual porque se encontró la guarda del llamador.

**Cambio a considerar:** Definir si la API devuelve multiplicador para evento existente o delta escalado; documentar precondición o devolver cero según la decisión.

**Código que habría que revisar:**

- `f4:Core/Src/encoder.c:ENC_GetScaledSteps`
- `f4:Core/Src/app_tasks.c:Encoder_MainTask`

**Puntos documentales que deben cambiar junto con el código:**

- [Encoder / escalado](../published/f4/libraries/encoder.md#fn-enc_getscaledsteps) · selector estable: `f4:module/Core/Inc/encoder.h / functions[name=ENC_GetScaledSteps]; limitations`.

**Repositorios relacionados:** FirmwareF4.

**Cierre:** No modificar el llamador por suposición; conservar su guarda y actualizar contrato del getter al resolver.

<a id="amdoc-f4-010"></a>
## AMDOC-F4-010 — sampleTenMs se almacena pero no gobierna el muestreo

**Clasificación:** parámetro sin efecto en la cadencia. **Estado:** abierto. **Certeza:** observed.

**Observación:** ENC_Init recibe uint32_t sampleTenMs y lo guarda en uint8_t. ENC_Task_N10ms no lo consulta y la tarea de timer lo llama cada 10 ms; el pulsador se muestrea dentro de esa llamada.

**Disparador y efecto:** Cambiar el argumento no cambia la cadencia como sugiere la API. Si se empieza a usar para espaciar toda la tarea, también cambiarían las duraciones de pulsación contadas por el botón integrado.

**Cambio a considerar:** Decidir si se elimina/depreca el parámetro o se implementa un divisor solo para el contador, conservando pulsador a 10 ms y límites de tipo explícitos.

**Código que habría que revisar:**

- `f4:Core/Src/encoder.c:ENC_Init/ENC_Task_N10ms`
- `f4:Core/Inc/encoder_types.h:ENC_Handle_t.sampleTenMs`
- `f4:Core/Src/app_tasks.c:callback timer de 10 ms`

**Puntos documentales que deben cambiar junto con el código:**

- [Encoder / cadencia](../published/f4/libraries/encoder.md#lifecycle) · selector estable: `f4:module/Core/Inc/encoder.h / lifecycle; types[name=ENC_Handle_t]`.

**Repositorios relacionados:** FirmwareF4.

**Cierre:** Sin modificación de la cadencia. Actualizar firma, unidad y ciclo de vida cuando se decida su contrato.

<a id="amdoc-esp-001"></a>
## AMDOC-ESP-001 — El validador AutoMotion acepta formas de argumentos de otros comandos

**Clasificación:** desacuerdo entre validador y codificador JSON. **Estado:** abierto. **Certeza:** resolved.

**Observación:** validate_auto_params acepta cualquiera de tres formas: payload, xCm/yCm o parámetros de calibración. No conoce el comando. cmd_stm_auto selecciona campos según command; stm.goto.start lee xCm/yCm aunque otra forma haya pasado el validador.

**Disparador y efecto:** Una solicitud stm.goto.start con forma de calibración puede pasar y codificar coordenadas ausentes como cero mediante as<float>(). La Web normal envía xCm/yCm; no se afirma un envío incorrecto desde su UI actual.

**Cambio a considerar:** Usar validadores específicos por operación o hacer depender la forma aceptada del comando; rechazar campos ausentes antes de convertirlos.

**Código que habría que revisar:**

- `esp:src/ws_bridge/dispatch.inc:validate_auto_params`
- `esp:src/ws_bridge/dispatch.inc:cmd_stm_auto`
- `esp:src/ws_bridge/dispatch.inc:tabla de rutas stm.goto.start/stm.motorCalibration.*`

**Puntos documentales que deben cambiar junto con el código:**

- [Contrato / AutoMotion](../published/shared/wire-contracts.md#auto-motion) · selector estable: `shared:wire / auto-motion`.
- [GOTO / frontera de argumentos](../published/f4/libraries/auto-motion.md#lifecycle) · selector estable: `f4:module/Core/Inc/auto_motion.h / lifecycle; limitations`.
- [ESP / bridge y validadores](../published/esp/libraries/json-bridge.md#lifecycle) · selector estable: `esp:module/src/ws_json_bridge.h / lifecycle; limitations`.

**Repositorios relacionados:** Firmware ESP01, Web ESP01, FirmwareF4.

**Cierre:** Solo registro documental. Requiere edición explícita del firmware ESP en la etapa de correcciones; conservar la semántica wire de argumentos válidos.

<a id="amdoc-web-001"></a>
## AMDOC-WEB-001 — Docs requiere tercer destino ESP y selector visual estable

**Clasificación:** pedido de interfaz diferido. **Estado:** reportado. **Certeza:** observed-partial.

**Observación:** docsContent.ts limita DocFirmwareTarget a web|f4 y variants usa Record de ambos. El usuario informa pérdida del divisor/fondo del selector; no se ejecutó inspección visual para atribuir causa CSS.

**Disparador y efecto:** La documentación ESP no tiene destino representable en el modelo actual. El defecto visual del divisor está reportado, no diagnosticado por ejecución.

**Cambio a considerar:** Tras completar contenido, incorporar esp al modelo/catálogo y adaptar el selector con píldora móvil centrada en la opción elegida; revisar fondo/divisor, teclado y movimiento reducido cuando el usuario autorice esa etapa.

**Código que habría que revisar:**

- `web:src/features/docs/docsContent.ts:DocFirmwareTarget/DocTopic.variants`
- `web:src/features/docs/docsCatalog.tsx`

**Puntos documentales que deben cambiar junto con el código:**

- [Web / porteo de Docs](../published/web/architecture.md#docs-adapter) · selector estable: `web:architecture / docs-adapter`.
- [Docs / catálogo de destinos](../published/web/libraries/documentation-ui.md#limitations) · selector estable: `web:module/src/features/docs/docsContent.ts / limitations`.

**Repositorios relacionados:** Web ESP01.

**Cierre:** Etapa de UI pendiente por decisión del usuario. Sin cambios de Web ni comprobación visual.

<a id="amdoc-esp-002"></a>
## AMDOC-ESP-002 — Los reportes sin selección publican selectedIndex/itemCount como cero

**Clasificación:** ausencia de dato convertida en valor. **Estado:** abierto. **Certeza:** resolved.

**Observación:** add_screen_report inicializa ambos campos en cero y siempre los emite. Un 0x95 de cinco bytes solo transporta código/source; la variante antigua 0x96 de seis bytes tampoco lleva índice. ScreenContext interpreta los campos numéricos emitidos como disponibles.

**Disparador y efecto:** Se pierde la distinción entre selección cero observada y selección ausente. Puede aparecer cursor o conteo transitorio falso al procesar eventos cortos; no se ejecutó una sesión para medir su manifestación.

**Cambio a considerar:** Conservar presencia/validez por longitud y tipo de reporte; omitir lo ausente o exponer un discriminante explícito, y definir en Web cuándo preservar o invalidar la última selección.

**Código que habría que revisar:**

- `esp:src/ws_bridge/reports.inc:add_screen_report`
- `web:src/contexts/ScreenContext.tsx:enrichCurrentScreen`

**Puntos documentales que deben cambiar junto con el código:**

- [Pantallas / reports](../published/shared/SCREEN_SYSTEM.md#reports) · selector estable: `shared:screen-system / reports`.
- [ESP / traductor de reportes](../published/esp/libraries/json-bridge.md#limitations) · selector estable: `esp:module/src/ws_json_bridge.h / limitations`.

**Repositorios relacionados:** Firmware ESP01, Web ESP01, FirmwareF4.

**Cierre:** Actualizar decoder, presencia de campos y normalizador juntos; crear ficha AMI si cambia el JSON público. No resolver fabricando estado de menú.

<a id="amdoc-web-002"></a>
## AMDOC-WEB-002 — El visor rellena estados que el reporte de pantalla no transporta

**Clasificación:** réplica con estado incompleto. **Estado:** abierto. **Certeza:** resolved.

**Observación:** 0x52/0x95/0x96 no transportan snapshot general de renderer, buffer/cursor OSK ni digits/pinIndex. readPinFormArgs fabrica cuatro ceros e índice cero; otros builders usan valores de muestra. remediatedScreens ya mantiene identidad sola para parte de las vistas sin datos.

**Disparador y efecto:** El PIN físico o una variante dinámica puede dibujarse con un estado inventado. Compartir código numérico no permite afirmar que la imagen coincida con la pantalla real; el formulario de login Web es una entrada local distinta.

**Cambio a considerar:** Definir estado discriminado por familia/variante y procedencia reported/query/simulation/unavailable. Mientras falte información, documentar y mostrar identidad/disponibilidad; cualquier nuevo snapshot requiere contrato y política de datos sensibles explícitos.

**Código que habría que revisar:**

- `web:src/screens/screenRenderRegistry.ts:readPinFormArgs`
- `web:src/screens/warningScreens.ts:buildPermissionPinFormCommands`
- `web:src/screens/remediatedScreens.ts:resolveRemediatedScreen`
- `esp:src/ws_bridge/reports.inc:add_screen_report`
- `f4:Core/Src/uner_app.c:UNER_CMD_GET_CURRENT_SCREEN`

**Puntos documentales que deben cambiar junto con el código:**

- [Pantallas / state-model](../published/shared/SCREEN_SYSTEM.md#state-model) · selector estable: `shared:screen-system / state-model`.
- [Pantallas / pin-osk](../published/shared/SCREEN_SYSTEM.md#pin-osk) · selector estable: `shared:screen-system / pin-osk`.
- [Web / resolver](../published/web/libraries/screen-resolver.md#limitations) · selector estable: `web:module/src/screens/screenRenderRegistry.ts / limitations`.

**Repositorios relacionados:** Web ESP01, Firmware ESP01, FirmwareF4.

**Cierre:** Separar disponibilidad de estado y port de raster. Una entrada PIN transmitida correctamente no demuestra que el visor copie el formulario físico.

<a id="amdoc-web-003"></a>
## AMDOC-WEB-003 — El renderer SVG actual no constituye el port completo de OLED F4

**Clasificación:** fidelidad del motor de dibujo. **Estado:** abierto. **Certeza:** resolved.

**Observación:** OledCommandPreview traduce OledCommand a SVG; usa geometría SVG, paint() de dos colores y placeholders de bitmap desconocido. F4 modifica framebuffer y tiene primitivas Inverse por XOR (aunque DrawPixel_H diverge: F4-012). Los builders TS son adaptaciones; compartir fuentes/bitmaps no copia toda la implementación.

**Disparador y efecto:** No está establecida igualdad de píxeles, clipping, redondeo, inversión ni recursos. Un dibujo visualmente parecido o el envío OLED Canvas no acredita réplica de la pantalla F4.

**Cambio a considerar:** Mantener objetivo de copiar/adaptar toda la biblioteca OLED y sus renderers/recursos, con capa de plataforma separada y framebuffer equivalente. Conservar procedencia/revisión por archivo; decidir explícitamente inconsistencias F4 antes de corregirlas en un solo lado.

**Código que habría que revisar:**

- `web:src/components/OledCommandPreview.tsx:renderCommands/paint/bitmapNode`
- `web:src/screens/types.ts:OledCommand`
- `web:src/features/oledEditor/oledCanvasRasterizer.ts`
- `f4:Core/Src/ssd1306.c:ssd1306_DrawPixel_H/ssd1306_DrawHorizontalLine_H`

**Puntos documentales que deben cambiar junto con el código:**

- [Pantallas / exact-port](../published/shared/SCREEN_SYSTEM.md#exact-port) · selector estable: `shared:screen-system / exact-port`.
- [Renderer Web](../published/web/libraries/oled-renderer.md#limitations) · selector estable: `web:module/src/components/OledCommandPreview.tsx / limitations`.

**Repositorios relacionados:** Web ESP01, FirmwareF4.

**Cierre:** Futuras comparaciones de framebuffer solo con autorización del usuario. No etiquetar exact por contar constantes ni por portar únicamente el rasterizador de Canvas.

<a id="amdoc-web-004"></a>
## AMDOC-WEB-004 — La ventana del menú reconstruida en Web puede ejecutar otra fila en F4

**Clasificación:** contrato de scroll y selección. **Estado:** abierto. **Certeza:** resolved.

**Observación:** F4 desplaza firstVisibleItem conservando historia y max_visible_items (favoritos usa dos). Web calcula bloques floor(selectedIndex/3) y usa tres posiciones. El reporte no lleva origen/tamaño real de ventana; 0x53 se interpreta en F4 como posición de su ventana actual.

**Disparador y efecto:** Con ítems visibles consecutivos, después de avanzar de índice 2 a 3 F4 puede mostrar 1..3 y Web 3..5. Un click Web sobre su fila 1 nombra el ítem 3 pero F4 puede seleccionar el 1. Es una derivación de las rutas leídas, sin ejecución física.

**Cambio a considerar:** Compartir ventana efectiva, catálogo visible y número de filas, o cambiar explícitamente el contrato para seleccionar identidad/índice confirmado. No reemplazar solo el dibujo por bloques ni suponer tres filas en todos los menús.

**Código que habría que revisar:**

- `f4:Core/Src/menusystem.c:MenuSys_EnsureCurrentSelectionVisible/MenuSys_SelectVisibleItem`
- `f4:Core/Src/menu_favorites.c`
- `web:src/protocol/screenMenuState.ts:resolveVisibleMenuWindow`
- `web:src/contexts/ScreenContext.tsx:enrichCurrentScreen`
- `web:src/components/ScreenStreamWorkspace.tsx:MENU_VISIBLE_ITEMS`
- `esp:src/ws_bridge/reports.inc:add_screen_report`

**Puntos documentales que deben cambiar junto con el código:**

- [Pantallas / menu-window](../published/shared/SCREEN_SYSTEM.md#menu-window) · selector estable: `shared:screen-system / menu-window`.
- [Pantallas / remote-actions](../published/shared/SCREEN_SYSTEM.md#remote-actions) · selector estable: `shared:screen-system / remote-actions`.
- [Web / estado de pantalla](../published/web/libraries/screen-state.md#limitations) · selector estable: `web:module/src/contexts/ScreenContext.tsx / limitations`.
- [F4 / menús](../published/f4/libraries/menus.md#lifecycle) · selector estable: `f4:module/Core/Inc/menusystem.h / lifecycle`.

**Repositorios relacionados:** FirmwareF4, Firmware ESP01, Web ESP01.

**Cierre:** Actualizar ventana, render y click en conjunto. Una ficha AMI debe describir cualquier ampliación de reporte o cambio de selección antes de portar consumidores.

<a id="amdoc-web-005"></a>
## AMDOC-WEB-005 — Normalizadores del visor pierden el modo GOTO

**Clasificación:** enum representado de forma distinta. **Estado:** abierto. **Certeza:** resolved.

**Observación:** F4/ESP y CarModeContext reconocen modo 3 GOTO. ScreenContext.carModeLabelFromByte y readCarModeLabelFromScreenData solo reconocen IDLE/FOLLOW/TEST; el resolver readCarMode/readMode también limita esos tres y cae a IDLE.

**Disparador y efecto:** El dashboard/vista de cambio de modo puede dibujar IDLE o un fallback aunque el estado físico reportado sea GOTO. El contexto global puede seguir mostrando GOTO en otro componente.

**Cambio a considerar:** Usar una normalización exhaustiva común con UNKNOWN separado de IDLE y adaptar tipos/builders a los cuatro modos manteniendo la representación wire.

**Código que habría que revisar:**

- `web:src/contexts/ScreenContext.tsx:carModeLabelFromByte/readCarModeLabelFromScreenData`
- `web:src/screens/screenRenderRegistry.ts:readCarMode/readMode`
- `web:src/contexts/CarModeContext.tsx:CarModeLabel`
- `esp:src/ws_bridge/reports.inc:car_mode_name`

**Puntos documentales que deben cambiar junto con el código:**

- [Pantallas / state-model](../published/shared/SCREEN_SYSTEM.md#state-model) · selector estable: `shared:screen-system / state-model`.
- [Web / dashboard y modos](../published/web/libraries/dashboard-builders.md#limitations) · selector estable: `web:module/src/screens/dashboardScreens.ts / limitations`.

**Repositorios relacionados:** Web ESP01, Firmware ESP01, FirmwareF4.

**Cierre:** Actualizar enum, normalizadores y representación juntos; no renumerar el modo 3 ni convertir desconocido en IDLE por conveniencia.

<a id="amdoc-web-006"></a>
## AMDOC-WEB-006 — Los builders de sensores no reciben el mismo modelo que la telemetría actual

**Clasificación:** datos y unidades de render. **Estado:** abierto. **Certeza:** resolved.

**Observación:** El reporte MPU JSON publica accel/gyro anidados en g y dps. readMpuArgs busca accelX/accelY/accelZ y gyroX/Y/Z y usa 120,-40,980,3,-2,0 si faltan. El visor pasa rawData de pantalla y estado IR; no se localizó una unión completa con la muestra MPU/variante local que usa el renderer F4. IR también tiene muestra escalonada de fallback.

**Disparador y efecto:** Una pantalla de sensores puede presentar ejemplos como si fueran datos actuales o escalar incorrectamente si se conectan campos sin conversión. Una suscripción de otro panel no garantiza que el visor use esa misma muestra ni su fase de render.

**Cambio a considerar:** Documentar/adaptar nombres, unidades, seq/instante, variante y validez por renderer. Separar ejemplo de dato recibido; no basta aplanar el JSON sin revisar la unidad y el estado F4 requerido.

**Código que habría que revisar:**

- `esp:src/ws_bridge/reports.inc:add_mpu_snapshot_fields`
- `web:src/screens/screenRenderRegistry.ts:readMpuArgs/resolveOledScreen`
- `web:src/screens/sensorScreens.ts:MpuValuesArgs`
- `web:src/components/ScreenStreamWorkspace.tsx:rawData`

**Puntos documentales que deben cambiar junto con el código:**

- [Pantallas / sensor-model](../published/shared/SCREEN_SYSTEM.md#sensor-model) · selector estable: `shared:screen-system / sensor-model`.
- [Web / builders de sensores](../published/web/libraries/sensor-settings-builders.md#limitations) · selector estable: `web:module/src/screens/sensorScreens.ts / limitations`.

**Repositorios relacionados:** Web ESP01, Firmware ESP01, FirmwareF4.

**Cierre:** Mantener relaciones con wire-contracts / samples; no modificar unidades del productor para acomodar un valor de ejemplo del consumidor.

<a id="amdoc-web-007"></a>
## AMDOC-WEB-007 — El umbral de pulsación larga del visor difiere del botón físico

**Clasificación:** semántica de interacción a decidir. **Estado:** revisar. **Certeza:** observed.

**Observación:** El visor clasifica long press a 1500 ms. El encoder F4 cuenta ciclos de 10 ms y clasifica counter>=30 como largo (también trata pulsación mantenida). El comando remoto transmite pressKind, no duración.

**Disparador y efecto:** Una pulsación de la misma duración puede producir acciones distintas en Web y placa. Puede ser una decisión de UX válida; no se atribuye fallo de transporte ni necesidad automática de igualar temporizadores.

**Cambio a considerar:** Decidir y documentar si la equivalencia buscada es evento lógico o gesto físico. Si se exige equivalencia temporal, acordar límites de ambos pulsadores y teclado sin cambiar IDs.

**Código que habría que revisar:**

- `web:src/components/ScreenStreamWorkspace.tsx:LONG_PRESS_THRESHOLD_MS`
- `f4:Core/Src/encoder.c:ENC_UpdateButtonState`

**Puntos documentales que deben cambiar junto con el código:**

- [Pantallas / remote-actions](../published/shared/SCREEN_SYSTEM.md#remote-actions) · selector estable: `shared:screen-system / remote-actions`.
- [Web / controles](../published/web/libraries/screen-controls.md#limitations) · selector estable: `web:module/src/components/ScreenStreamWorkspace.tsx / limitations`.

**Repositorios relacionados:** Web ESP01, FirmwareF4.

**Cierre:** Pendiente decisión funcional. Registrar ambos umbrales aunque se conserve la diferencia por diseño.

<a id="amdoc-web-008"></a>
## AMDOC-WEB-008 — Dos identidades de acción F4 no están declaradas en screenCodes Web

**Clasificación:** catálogo de identidades. **Estado:** abierto. **Certeza:** observed.

**Observación:** No están declarados SCREEN_CODE_CONNECTIVITY_WIFI_RETRY (0x020211) ni SCREEN_CODE_SETTINGS_PREFS_WIFI_AP_FALLBACK (0x05041D) en screenCodes.ts. Se localizaron como identidades de acciones MenuItem en menu_definition.c, sin renderer dedicado localizado.

**Disparador y efecto:** El catálogo Web queda incompleto para esas identidades y futuras herramientas pueden desconocerlas. La ausencia no demuestra por sí sola una pantalla activa sin renderer: Retry y el toggle pueden conservar el código del menú.

**Cambio a considerar:** Agregar al catálogo cuando se porte el contenido, conservando la clasificación action-only y los valores canónicos F4. No inventar una pantalla dedicada para completar una tabla.

**Código que habría que revisar:**

- `f4:Core/Inc/screen_codes.h:SCREEN_CODE_CONNECTIVITY_WIFI_RETRY/SCREEN_CODE_SETTINGS_PREFS_WIFI_AP_FALLBACK`
- `f4:Core/Src/menu_definition.c:submenuWifiItems/submenuPreferencesFlagsItems`
- `web:src/screens/screenCodes.ts`

**Puntos documentales que deben cambiar junto con el código:**

- [Pantallas / code-register](../published/shared/SCREEN_SYSTEM.md#code-register) · selector estable: `shared:screen-system / code-register`.
- [Catálogo 0x020211](../published/shared/screens/020211.md) · selector estable: `shared:screen/020211`.
- [Catálogo 0x05041D](../published/shared/screens/05041d.md) · selector estable: `shared:screen/05041d`.

**Repositorios relacionados:** Web ESP01, FirmwareF4.

**Cierre:** No se modificaron constantes Web. Mantener la distinción entre declaración, identidad de acción y pantalla efectivamente renderizada.

<a id="amdoc-web-009"></a>
## AMDOC-WEB-009 — El catálogo de línea no tiene bindings específicos de visor localizados

**Clasificación:** cobertura de render y acciones. **Estado:** abierto. **Certeza:** observed.

**Observación:** Los cinco códigos LINE 0x030204..0x030208 están declarados en ambos repositorios pero no se localizaron en el resolver de visor/remediated. F4 usa menú de calibración, render blanco/pista y aviso con estado/motivo propios. LINE_FOLLOW se localizó como identidad de acción que alterna seguimiento, no como renderer dedicado.

**Disparador y efecto:** Las pantallas activas de calibración/aviso caen al fallback del visor aunque la página Web de seguimiento tenga operaciones. La página de control y la réplica del OLED son consumidores distintos.

**Cambio a considerar:** Portar renderers y discriminantes F4 de menú, blanco, pista y aviso con su modelo. Conservar LINE_FOLLOW como acción mientras no exista evidencia de una vista dedicada; mapear sus confirmaciones/rechazos al estado efectivo.

**Código que habría que revisar:**

- `f4:Core/Src/menu_definition.c:MenuRender_LineCalibration/MenuRender_LineNotice/MenuAction_LineFollow/MenuLineCalEvents`
- `f4:Core/Src/oled_utils.c:OledUtils_RenderLineCalibration/OledUtils_RenderLineNotice`
- `web:src/screens/screenRenderRegistry.ts:resolveOledScreen`
- `web:src/features/lineFollow/useLineDevice.ts`

**Puntos documentales que deben cambiar junto con el código:**

- [Pantallas / line-family](../published/shared/SCREEN_SYSTEM.md#line-family) · selector estable: `shared:screen-system / line-family`.
- [Línea 0x030205](../published/shared/screens/030205.md) · selector estable: `shared:screen/030205`.
- [Línea 0x030208](../published/shared/screens/030208.md) · selector estable: `shared:screen/030208`.

**Repositorios relacionados:** Web ESP01, FirmwareF4, Firmware ESP01.

**Cierre:** Sin port de código en esta etapa. Actualizar estado, acciones y resolución por separado al implementar cada vista.

<a id="amdoc-f4-011"></a>
## AMDOC-F4-011 — USER largo sale del OSK por el router sin ejecutar su cancelación

**Clasificación:** cleanup de contexto UI. **Estado:** abierto. **Certeza:** resolved.

**Observación:** UiEventRouter_HandleEvent consume UE_LONG_PRESS antes del handler activo y vuelve al dashboard. OSK_Integration_EventManager tiene rama UE_LONG_PRESS que llama OnBack, pero no se alcanza por ese router. MenuSys_ResetMenu no cancela OSK; OnBack y osk_InternalOnCancel limpian active/callbacks y desbloquean notificaciones.

**Disparador y efecto:** Durante un OSK, si Permission/ScreenLock no consumen antes, USER largo puede cambiar pantalla conservando estado OSK activo, bloqueo de notificaciones o callbacks pendientes. Es un camino de fuente; no se reprodujo físicamente.

**Cambio a considerar:** Definir salida global con cancelación/cleanup explícito del contexto activo antes de dashboard, evitando restaurar una pantalla anterior después del reset. Mantener coherencia entre input físico y remoto.

**Código que habría que revisar:**

- `f4:Core/Src/ui_event_router.c:UiEventRouter_HandleEvent`
- `f4:Core/Src/menusystem.c:MenuSys_ResetMenu`
- `f4:Core/Src/osk_integration.c:OSK_Integration_EventManager/osk_InternalOnCancel/osk_RestoreUiContext`
- `f4:Core/Src/osk.c:OSK_OnBack`

**Puntos documentales que deben cambiar junto con el código:**

- [Pantallas / pin-osk](../published/shared/SCREEN_SYSTEM.md#pin-osk) · selector estable: `shared:screen-system / pin-osk`.
- [F4 / integración OSK](../published/f4/libraries/osk-integration.md#lifecycle) · selector estable: `f4:module/Core/Inc/osk_integration.h / lifecycle`.
- [F4 / prioridad de eventos](../published/f4/libraries/ui-router.md#lifecycle) · selector estable: `f4:module/Core/Inc/ui_event_router.h / lifecycle`.

**Repositorios relacionados:** FirmwareF4, Web ESP01.

**Cierre:** Decidir orden de cancelación y restauración; no cambiar únicamente la rama local que el router no deja ejecutar.

<a id="amdoc-f4-012"></a>
## AMDOC-F4-012 — Inverse cambia de semántica entre píxel y líneas rápidas OLED

**Clasificación:** consistencia interna del raster. **Estado:** abierto. **Certeza:** resolved.

**Observación:** ssd1306_DrawPixel_H compara color==White y en cualquier otro caso borra el bit (tras posible !color). Las primitivas rápidas de línea tienen case Inverse con XOR del framebuffer. El SVG paint Web trata Inverse como White salvo inverted.

**Disparador y efecto:** El mismo color Inverse no tiene efecto uniforme según la primitiva y difiere también del SVG. Un port que lo convierta siempre a XOR cambiaría silenciosamente el comportamiento actual del productor.

**Cambio a considerar:** Acordar semántica de Inverse por primitiva y distinguir port literal de corrección funcional. Si se corrige, actualizar F4 y réplica en la misma revisión documentada; preservar clipping/operaciones de buffer.

**Código que habría que revisar:**

- `f4:Core/Src/ssd1306.c:ssd1306_DrawPixel_H`
- `f4:Core/Src/ssd1306.c:ssd1306_DrawHorizontalLine_H/ssd1306_DrawVerticalLine_H`
- `web:src/components/OledCommandPreview.tsx:paint`

**Puntos documentales que deben cambiar junto con el código:**

- [Pantallas / exact-port](../published/shared/SCREEN_SYSTEM.md#exact-port) · selector estable: `shared:screen-system / exact-port`.
- [F4 / primitivas OLED](../published/f4/libraries/ssd1306.md#fn-ssd1306_drawpixel_h) · selector estable: `f4:module/Core/Inc/ssd1306.h / functions[name=ssd1306_DrawPixel_H]`.

**Repositorios relacionados:** FirmwareF4, Web ESP01.

**Cierre:** No se corrigió el raster. Una decisión de semántica debe quedar separada de la afirmación futura de fidelidad exacta.
