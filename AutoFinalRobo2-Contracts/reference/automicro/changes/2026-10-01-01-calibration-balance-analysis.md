# AMI-20261001-01 — Workspace IR/Balance, sesiones y análisis PID experimental

- Fecha: 2026-10-01 (America/Buenos_Aires)
- Origen: Web ESP01 / WebESP01AutoMicro2025.
- Referencia: cambios de trabajo en src/features/calibration, src/features/balanceCalibration, src/features/ir, src/pages/EstadoSection.tsx y tools/balance-analysis. Sin commit de código Web en esta ficha.
- Estado: workspace, captura limitada, almacenamiento, análisis local, HTTP y MCP implementados en fuente; adquisición sincronizada y ensayo temporal solo especificados como pendientes.
- Área: API de análisis, formato de sesión y flujo Web -> ESP -> F4.

## Anterior y nuevo

Antes: calibración IR en modal, perfil local e importación/exportación JSON/C. No existía workspace de Balance, historial de sesiones ni servicio de análisis PID integrado.

Ahora: workspace en /statics con Sensores IR / Balance. IR se presenta por pasos, conserva su esquema v2 y admite perfiles anteriores. El máximo activo es 5 cm, centralizado en IR_MAX_RANGE_CM; puntos históricos más lejanos quedan visibles y exportables en JSON, excluidos de curvas activas y tabla C. Exportar C no instala datos en F4. La captura IR usa los norm de canales de objetos como el código anterior: no redefinirlos como ADC crudo.

Se registra también el contexto visual relacionado: conos métricos desde centros emisores CAD, apertura total 32°, inclinación óptica incorporada a cada yaw; mayor transparencia en reposo. La web usa un único modelo embedded y se retiraron preferencias y rutas de carga de modelo HD. Estos cambios no reservan IDs UNER ni implican nueva geometría física validada.

### Captura actual de Balance

Web consulta getTelemetrySnapshot con args {}, envelope API 1, requestId y timeout Web 3000 ms. Reutiliza UNER GET_TELEMETRY_SNAPSHOT 0x64: request vacío, respuesta 92 bytes, little endian, status y floats según contrato existente. ESP mantiene SAME_COMMAND y privilegio STM_SESSION; su registro observado tiene timeout 80 ms. No se modifica longitud ni significado del snapshot.

Campos JSON usados: status:number=0, balanceActive:true, safetyStop:false, pitchMeasuredDeg, pitchSetpointDeg, balanceOutputPercent, leftOutputPercent, rightOutputPercent, pid:{kp,ki,kd}, pidErrorDeg, pidIntegralDegS y pidDerivativeDegS. Todos los campos numéricos deben ser finitos; estados deben ser booleanos. Se detiene el registro si balance no está activo o hay parada; no activa balance ni ordena parar motores.

Se consulta una muestra por vez, con espera de 200 ms tras cada respuesta; máximo 600 muestras o 120 s. Se detiene por desconexión, rechazo o timeout. Es registro de llegada Web, no stream sincronizado F4; no produce recomendaciones de ganancias.

### Formato JSON de sesión v1

Objeto raíz: schema:"automicro.balance-session", version:1, id:string, createdAt:string ISO8601 producido por Web, name:string, notes:string, firmware:string, timing:"web-arrival"|"f4-control-cycle", controller:null|objeto, samples:array (máximo 10000).

controller:{derivativeTauS:number segundos >=0, integralMin:number grados*s, integralMax:number grados*s, outputLimit:number porcentaje en (0,100], antiWindupGain:number >=0, inverted:boolean}. integralMin<=integralMax. Describe el controlador observado en F4, no un filtro ficticio. Captura Web actual usa controller:null y firmware:"no informado".

Cada muestra: {timeMs:number ms monotónicos relativos al registro, sequence:number entero seguro >=0, pitch:number grados, target:number grados, output:number porcentaje después de inversión/límites de balance, left:number porcentaje aplicado izquierdo, right:number porcentaje aplicado derecho, error:number grados, integral:number grados*s, derivative:number grados/s filtrada sobre medición, gains:{kp:number,ki:number,kd:number}, stopped:boolean}. Todos los números finitos. timeMs y sequence estrictamente crecientes. error=target-pitch; P=kp*error, I=ki*integral, D=kd*derivative describen aportes previos a límites e inversión, no salidas motor independientes.

El parser controla forma, tamaños y orden como defensa de entrada en runtime; no se ejecutaron validadores como tarea de comprobación. Textos máximos 4000 caracteres. Importación UI máximo 8 MiB. IndexedDB automicro-calibration v1, store sessions keyPath id. JSON importado conserva su origen declarado; no implica captura físicamente acreditada.

### Análisis

Motor arx2-grid-v1, mismo código usado por worker inline Web, servidor HTTP y MCP. Sesiones web-arrival se rechazan para optimización. Exige f4-control-cycle, firmware informado, controller completo, >=200 ciclos, >=2 s, secuencias consecutivas, período medio 1–50 ms con variación <=10%, objetivo/ganancias constantes, sin parada y sin giro marcado (diferencia salidas >5 puntos porcentuales) ni salida >95% del límite. No hay productor de esta sesión sincronizada integrado hoy.

Ajusta ARX2: desviación angular actual desde dos desviaciones previas, salida previa normalizada y offset, con mínimos cuadrados sobre primer 70%. Mide error de predicción de un paso en último 30%; rechaza sobre max(0.1°, 25% RMS angular). No garantiza identificación no sesgada en lazo cerrado ni márgenes de estabilidad.

Búsqueda conjunta: factores P/D 0.8,1,1.2; I 0,0.8,1,1.2. No inicia un término que estaba en cero. Simula ±1°, variación de respuesta ±10%, horizonte 5 s (máximo 5000 pasos). Implementa derivada sobre medición, filtro, límites y anti-windup de la ecuación observada en pid_controller.c. Costo por paso: error² + 0.01*velocidadAngular² + 0.05*salidaNormalizada² + 2 si saturado; toma peor escenario. Rechaza divergencia >10° o residuo final >0.2°. Estos son límites del modelo, no guardas de firmware. Requiere mejora simulada >=2%. Sensibilidad de cada ganancia +/-10%.

Resultado: {algorithm:"arx2-grid-v1",sessionId:string,status:"insufficient-data"|"no-candidate"|"proposal",reasons:string[],candidate:null|{kp,ki,kd},baselineCost:number|null,candidateCost:number|null,fitErrorDeg:number|null,sensitivity:array de {term:"kp"|"ki"|"kd",minusCost:number|null,plusCost:number|null}}. null en costos de sensibilidad significa escenario no admisible. Todos los costos son métricas simuladas, no porcentajes de estabilidad física. Ningún consumidor debe aplicar candidate por el solo hecho de recibir status:proposal.

### HTTP y MCP implementados

POST /v1/balance/analyze recibe la sesión completa como JSON; devuelve resultado final síncrono HTTP 200 para los tres estados de análisis. Sin ACK intermedio, eventos ni jobId. Errores 400 invalid_session, 401 unauthorized, 403 origin_not_allowed, 404 not_found, 405 method_not_allowed, 413 session_too_large, 415 json_required. Token Bearer opcional solo en loopback; obligatorio al escuchar externamente. Origen permitido exacto configurable, por defecto http://localhost:5173. No es autenticación PIN ESP. Web solo envía al pulsar Enviar sesión y analizar; timeout cliente 30 s, cancelación AbortController. Cancelar la espera no interrumpe necesariamente cálculo del servidor.

Servidor local Node 22.18+, default 127.0.0.1:9877; variables BALANCE_HOST, PORT, BALANCE_ALLOWED_ORIGIN, BALANCE_API_TOKEN. No se arrancó ni desplegó. No implementa cola cloud, TLS propio, historial remoto ni multitenancy: necesarios antes de ofrecer servicio cloud compartido.

MCP stdio local JSON-RPC, protocolVersion 2025-06-18, initialize/ping/tools/list/tools/call. Herramienta analyze_balance_session, input {session:objeto}, resultado en content text JSON; errores de sesión como isError:true. Sin herramientas de escritura al vehículo. No se instaló/conectó automáticamente en Codex ni se arrancó. Worker Web inline conserva distribución de la web en HTML único.

## Flujo completo

1. Usuario conectado con sesión STM inicia registro en Balance; Web serializa requests 0x64 vía ESP y guarda sesiones locales.
2. Importación/exportación no requiere Internet. El parser conserva etiquetas reales de tiempo; no convertir web-arrival a f4-control-cycle para eludir requisitos.
3. Análisis local corre en worker cancelable. Análisis remoto usa HTTP y autenticación del servicio; MCP usa el mismo motor sin controlar hardware.
4. Resultados insuficientes/rechazados explican motivos. Propuesta incluye advertencia de ensayo físico pendiente. UI no llama setBalancePid ni rawUner para aplicar.
5. Datos no sincronizados actuales no habilitan optimización. No se promete autocalibración completa disponible.

## Dependencias pendientes: captura y prueba F4/ESP

F4 debe definir captura coherente por ciclo con secuencia, tiempo de adquisición, dt real, configuración PID/filtro/límites, estado de saturación y salidas aplicadas. El JSON f4-control-cycle es formato de intercambio; no se reserva ID ni payload binario en esta ficha. Debe resolverse presupuesto UART y representación de batería/encoders según hardware real.

F4 debe implementar prueba temporal de candidato con validación de compatibilidad, vigencia, watchdog, límites físicos, transferencia sin salto y reversión local sin depender de Web/ESP. Persistencia debe ser operación separada, con lectura de confirmación. ESP debe transportar comandos/eventos/capabilities definidos por el productor. No reutilizar setPreferences para estas funciones.

Los comandos existentes getBalancePid/setBalancePid (0x65/0x66) fueron observados, pero no habilitan por sí mismos prueba temporal ni reversión. Los IDs, longitudes, capacidades, permisos, correlación, errores y orden de despliegue de funciones nuevas requieren otra ficha al implementarse. No afirmar consumidores adaptados por leer esta propuesta.

## Impacto

| Repositorio | Estado | Acción |
| --- | --- | --- |
| FirmwareF4 | pendiente | Producir captura sincronizada y ensayo temporal; código actual solo leído, no modificado. |
| Firmware ESP01 | pendiente | Transportar captura/ensayo futuros y capacidades; 0x64 existente se reutiliza sin cambios. |
| Web ESP01 | origen | Workspace, IR a 5 cm, historial, gráficos, captura de llegada, motor experimental, HTTP y MCP implementados en fuente. |
| AutoMicroQtDesktop | pendiente | Evaluar/importar formato de sesión y uso futuro de captura/ensayo; no inspeccionado en esta implementación. |

## Compatibilidad y orden

No cambia UNER existente ni API v1 de ESP. Actualizar Web conserva perfiles IR antiguos. Captura 0x64 solo lectura con capacidades existentes; si rechazada, UI muestra error y detiene registro. Para nuevas funciones: acordar e implementar productor F4, bridge ESP y consumidores Web/Qt con capabilities y ficha nueva. Desplegar primero backend con soporte antes de habilitar acciones UI. Cloud y MCP no son requisito para IR o historial.

## Evidencia y límites

Lectura de Web, registros ESP ws_json_bridge.cpp/f4_command_registry.c y F4 pid_controller.c/balance_control.c/wifi_bridge_types.h. No tests, compilación, validación visual, servidor real, tráfico físico ni ensayo en vehículo. Manual visual 0.3.0 autorizado, extensiones propuestas no verificadas. Se preservó trabajo ajeno del checkout. Publicación de esta ficha documenta coordinación; no acredita sincronización del código de otros repositorios.

## Actualización visual posterior — 2026-10-01

Web reformó la presentación de calibración por solicitud del usuario: cabecera con BrandMark, selector IR/Balance, pasos discretos, preparación con formulario y referencia de 5 cm, superficies sólidas y acciones primarias/secundarias. Estilos consolidados en src/features/calibration/calibration.css; retirados los bloques antiguos de index.css y corregido el selector del propio diálogo y su padding. No cambia schema, IDs, payloads, API HTTP/MCP, tiempos de captura ni comportamiento de aplicación de perfiles/ganancias; los estados de integración de F4/ESP/Qt siguen pendientes. Solo lectura y edición de código; sin tests, compilación, validación ni revisión visual. Esta nota comunica la reforma de presentación y no requiere adaptación de protocolo en consumidores.


2026-10-05: adaptación Qt en fuentes y controles AP fallback Web registrados en AMI-20261005-01. Captura local IR/Balance, archivos interoperables y formulario PID; sin protocolo nuevo de ciclos F4 ni ensayo físico. Distancias Qt usan interpolación nativa, sin equivalencia acreditada del solucionador Web completo. Sin build, tests, validaciones ni hardware. La historia de pendientes de sincronización física permanece.
