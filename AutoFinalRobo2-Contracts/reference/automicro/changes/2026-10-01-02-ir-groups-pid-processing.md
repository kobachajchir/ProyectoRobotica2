# AMI-20261001-02 — Objetos/Línea y edición PID con registros procesados

- Fecha: 2026-10-01 (America/Buenos_Aires).
- Origen: Web ESP01 / WebESP01AutoMicro2025.
- Código: src/features/calibration/MaterialPicker.tsx, LineWorkbench.tsx, CalibrationWorkspace.tsx; src/features/ir/IrCalibrationWorkbench.tsx; src/features/balanceCalibration/useBalancePid.ts, BalanceWorkbench.tsx, session.ts; src/protocol/wsApi.ts. Cambios de trabajo Web, sin commit de código en esta ficha.
- Estado: implementado en fuente; procesamiento con servidor MCP futuro/no configurado por defecto. Sin tests, build ni validaciones.
- Área: flujo API WebSocket existente, formatos locales de calibración e intercambio de sesiones.

## Anterior y nuevo

Antes: dos procedimientos IR/Balance, selector libre de polaridad en Objetos, material de texto libre. Balance no consultaba ni escribía ganancias desde la UI. El informe anterior AMI-20261001-01 debe leerse como estado previo de esa implementación.

Ahora: Objetos / Línea / Balance son procedimientos independientes. Material: Blanco o Negro; checkbox Personalizado revela nombre obligatorio para capturar. No se ofrece selector eléctrico manual.

Fuente de polaridad: FirmwareF4/Core/Src/tcrt5000.c tabla de sensores; PA1/PA2/PA0 pull-up (Línea), PB1/PA4/PA5/PA6/PA7 pull-down (Objetos). TCRT5000_NormalizeForDisplay devuelve 4095-raw para pull-up y raw para pull-down. uner_app.c UNER_App_NormalizeIrRaw ya normaliza el snapshot. Web captura norm sin volver a invertir Línea; Objetos fija higher-is-closer al importar/cargar perfil y al definir material. La documentación antigua de selección libre queda sustituida para esta UI. No se cambia ADC ni firmware.

### Línea: referencias locales v1

Storage local automicro.ir-line-calibration.v1. Exportación JSON: {schema:"automicro.ir-line-calibration",version:1,group:"line",electricalPolarity:"pull-up",records:[...]}.

Registro: {id:string UUID,name:string,material:string,createdAt:string ISO8601,count:32,raw:[number,number,number],normalized:[number,number,number]}. Orden izquierdo/centro/derecho; claves del snapshot lineLeft/lineCenter/lineRight. Valores son medianas ADC 0–4095; capturas descartan 8 muestras y usan 32 útiles, con deduplicación sampleSeq/tickMs. Máximo 100 registros recientes. Blanco/Negro/Personalizado quedan separados en registros. No se usan distancias de objetos ni se instala un umbral en F4. Exportación conserva raw y norm.

### PID: comandos existentes reutilizados

Web agrega constantes GET_BALANCE_PID=getBalancePid y SET_BALANCE_PID=setBalancePid. Alias existentes ESP stm.balance.pid.get/set también disponibles en productor, pero la Web usa los nombres cortos.

GET: args {}, UNER 0x65 request 0 bytes. Respuesta por mismo comando 13 bytes: status:u8, kp:f32 LE, ki:f32 LE, kd:f32 LE. JSON final {status,kp,ki,kd}. Lectura automática al abrir Balance con conexión real; actualizar manual disponible. Los valores mostrados son respuesta de placa, no valores predeterminados. El log incorpora initialPid leído y mantiene gains por snapshot.

SET: args {kp:number,ki:number,kd:number}; todos finitos 0..1000. ESP codifica máscara 0x07 seguida por tres f32 little-endian, request 13 bytes. Su registro conserva ACK_THEN_SAME_COMMAND. Respuesta final 0x66, 14 bytes: status:u8, changedMask:u8, kp/ki/kd:f32 LE. JSON {status,changedMask,kp,ki,kd}. Web requiere status=0 y respuesta que coincida con f32 enviado (tolerancia 1e-4). Nunca interpreta un ACK intermedio como confirmación final.

Firmware observado: UNER_App_HandleSetBalancePid llama AppPreferences_SetBalancePid, obtiene los valores guardados, BalanceControl_SetPid y BalanceControl_Reset. Este comando guarda preferencias y reinicia el estado del PID: NO es un ensayo temporal con reversión automática. El GET obtiene el PID actual del productor. No se modifica ningún ID, unidad o payload existente.

Checkbox Modificar habilita campos P/I/D; aparece Enviar. Se bloquea edición/envío durante captura o análisis y sin conexión real. Se permite enviar cero y se rechaza vacío, NaN o fuera de rango. Ante error/timeout, la UI invalida valores actuales y ofrece volver a leer: no puede asumir que F4 no aplicó ante pérdida de respuesta.

## Flujo completo

1. Balance abre -> getBalancePid -> ESP -> F4 -> respuesta final correlacionada por requestId -> campos de solo lectura.
2. Usuario activa Modificar, cambia P/I/D y pulsa Enviar -> setBalancePid -> validación/permisos STM_SESSION existentes -> ACK intermedio y respuesta final -> ganancias confirmadas. Timeout Web 3000 ms; backend sigue con tiempos del registro existente (GET 50 ms; SET 80 ms). No se añade un evento PID nuevo.
3. La sesión registrada incluye optional initialPid:{kp,ki,kd}, y las ganancias de cada muestra. Requiere lectura actual exitosa antes de iniciar registro. No se expone PIN al servicio de procesamiento.
4. Usuario completa Nombre del registro procesado y pulsa Procesar con MCP. El campo de conector HTTP arranca vacío: sin servidor configurado NO envía datos ni inventa respuesta. El adaptador futuro debe devolver el contrato de análisis versionado de AMI-20261001-01; no se declara una sesión MCP activa desde el navegador.
5. Cuando se configura conector HTTP y retorna status:proposal, Web guarda registro procesado en IndexedDB. Se muestra respuesta P/I/D y botón Setear valores. Su pulsación usa el mismo 0x66, guarda la confirmación del dispositivo en el registro y actualiza el PID mostrado. Persistencia local fallida se informa y permite exportar.

## Sesión v1: extensiones opcionales compatibles

initialPid:{kp:number,ki:number,kd:number} para valores leídos antes del registro.
processed:{name:string,processedAt:string ISO8601,algorithm:string,candidate:{kp,ki,kd},confirmedPid?:{kp,ki,kd},appliedAt?:string ISO8601}. candidate proviene del resultado; confirmedPid y appliedAt aparecen después de respuesta final exitosa de F4. Nuevos campos opcionales: sesiones anteriores siguen importándose; las exportaciones nuevas conservan procesamiento y aplicación confirmada. Guardar un registro procesado es distinto de enviar sus ganancias al dispositivo.

No se cambia HTTP /v1/balance/analyze ni algoritmo arx2-grid-v1. El flujo conector MCP sigue usando el adaptador HTTP del análisis; un servidor MCP remoto con gateway propio es futuro. El botón no conecta stdio del navegador ni instala el plugin. Las sesiones web-arrival siguen siendo insuficientes para optimizar con el motor local. No se elimina esa protección para simular una respuesta MCP.

## Impacto

| Repositorio | Estado | Acción o motivo |
| --- | --- | --- |
| FirmwareF4 | compatible | Productor existente de polaridades y 0x65/0x66 leído; no cambia wire. Prueba física no ejecutada. Captura sincronizada/ensayo temporal continúan pendientes en AMI-20261001-01. |
| Firmware ESP01 | compatible | Validadores, dispatcher y decoders existentes leídos; nombres/payloads sin cambios. No se afirma despliegue de esa versión en la placa. |
| Web ESP01 | origen | UI de grupos/material, captura Línea, GET/SET PID y extensiones de sesión implementadas en fuente. |
| AutoMicroQtDesktop | pendiente | Evaluar soporte del nuevo formato de sesión y referencias Línea; no inspeccionado en esta tarea. |

## Compatibilidad y orden

Puede actualizarse Web sin nuevo firmware cuando la placa soporte ya los comandos existentes. Si ESP/F4 rechazan por falta de soporte o sesión, se muestra error; no se simulan valores. No requiere nuevas capacidades ni IDs. Consumidores de archivos deben aceptar campos opcionales o adaptarse. El servidor/gateway MCP se integra después; captura sincronizada y ensayo con reversión requieren desarrollo explícito en F4/ESP y otro contrato.

## Evidencia y límites

Lectura estática de tcrt5000.c, uner_app.c, ws_json_bridge.cpp, f4_command_registry.c y consumidor Web. La fuente F4 demuestra intención de guardar/resetear, no éxito físico de flash. No tests, compilaciones, validaciones, ejecución de servicios ni tráfico real. Cambios de presentación mantienen identidad y son extensión propuesta no revisada visualmente. Solo Web y registro central modificados.


2026-10-05: adaptación Qt en fuentes y controles AP fallback Web registrados en AMI-20261005-01. Captura local IR/Balance, archivos interoperables y formulario PID; sin protocolo nuevo de ciclos F4 ni ensayo físico. Distancias Qt usan interpolación nativa, sin equivalencia acreditada del solucionador Web completo. Sin build, tests, validaciones ni hardware. La historia de pendientes de sincronización física permanece.
