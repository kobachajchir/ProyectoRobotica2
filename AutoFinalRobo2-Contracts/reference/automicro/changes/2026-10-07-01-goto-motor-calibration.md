# AMI-20261007-01 — GOTO estimado y calibración de motores de un metro

- Fecha: 2026-10-07 (America/Buenos_Aires).
- Origen: FirmwareF4, rama work/nueva-etapa, commit b9ef8d4.
- Estado: implementado en fuente; sin tests, compilaciones ni validaciones.
- Área: UNER, WebSocket, API Qt, navegación, preferencias y screen code.
- Implementación y decisiones: [Control y calibración](../docs/GOTO_MOTOR_CALIBRATION_20261007.md).

## Contrato anterior y nuevo

Antes GOTO era un modo seleccionable y existían balance/marcha/TEST remotos y seguimiento calibrado. No había una misión XY con perfil de avance persistente ni medición de un metro entre marcas. Se añade servicio local F4; se conservan IDs de telemetría histórica 0x20..0x22 y se asignan 0x23..0x2B.

Todos los números multibyte usan little-endian; f32 es IEEE754 de 32 bits. Requests y respuestas tienen longitud exacta:

| ID | Nombre JSON / API Qt | Request bytes | Response bytes |
| --- | --- | --- | --- |
| 0x23 | stm.goto.get | 0 | 128 |
| 0x24 | stm.goto.start | 8: xCm f32, yCm f32 | 128 |
| 0x25 | stm.goto.cancel | 0 | 128 |
| 0x26 | stm.goto.irCalibration.set | 186: payload, formato IR abajo | 128 |
| 0x27 | stm.motorCalibration.get | 0 | 128 |
| 0x28 | stm.motorCalibration.control | 13: operation u8, percent f32, rampMs u32, timeoutMs u32 | 128 |
| 0x29 | stm.motorCalibration.profile.get | 0 | 88 |
| 0x2A | stm.motorCalibration.profile.set | 88: payload, formato de perfil abajo | 88 |
| 0x2B | stm.mpu.calibration.request | 0 | 128 |

JSON de inicio: {xCm,yCm}; control: {operation,percent,rampMs,timeoutMs}; setters compactos: {payload:[bytes]}; GET/cancel/MPU sin parámetros. Operaciones: 0 comenzar, 1 cancelar, 2 guardar/promediar con perfil existente del mismo nivel y rampa, 3 guardar/reemplazar ese nivel. El cliente usa 3 en ensayo básico/primera repetición y 2 en repeticiones posteriores. Guardar/importar/transferir requiere desarmado. Intensidad 1..20%, rampa 0..5000 ms, timeout 1000..30000 ms; op guardar utiliza el ensayo terminado, no los parámetros actuales para alterar su medición.

### Estado común v1 — 128 bytes

| Offset | Campo y tipo |
| --- | --- |
| 0..7 | status u8 (0 éxito/1 rechazo), version u8=1, kind u8 (0 GOTO/1 calibración), state u8, reason u8, carMode u8, flags u8, obstacleMask u8 |
| 8,12 | missionId u32, elapsedMs u32 |
| 16,20,24,28 | xCm, yCm, goalXCm, goalYCm f32 |
| 32,36,40 | yawDeg relativo, speedCmS, percent f32 |
| 44,48 | rampMs, measurementMs u32 |
| 52 | pwmHz f32 |
| 56,60 | leftDutyMean, rightDutyMean f32 (%) |
| 64,68 | leftDutyMax, rightDutyMax f32 (%) |
| 72,76 | leftDutySeconds, rightDutySeconds f32 (integral de duty fraccional por segundos) |
| 80,84 | leftPwmCycles, rightPwmCycles f32 (ciclos eléctricos estimados durante duty no nulo) |
| 88,92,96,100 | leftForwardMs, leftReverseMs, rightForwardMs, rightReverseMs u32 |
| 104..107 | pointCount, phase, persisted, resultValid u8 |
| 108,112 | referenceCm=100, yawErrorMax f32 |
| 116 | setupMs u32 (previo a la ventana del metro) |
| 120..123 | objectsEnabled, lightsEnabled, objectsFresh, lineFresh u8 |
| 124 | revision u32 del perfil activo |

flags: bit0 mpuReady; bit1 mpuCalibrated; bit2 motorCalibrated; bit3 irCalibrated; bit4 balanceActive; bit5 balanceSettled; bit6 safetyStop; bit7 lineCalibrated. JSON expande estas condiciones a booleanos y añade positionEstimated=true. State: 0 reposo, 1 recorrido, 2 evasión, 3 completado, 4 cancelado, 5 fallo. Phase de calibración: 0 salir de primera marca, 1 entre marcas, 2 cruzar segunda marca.

Reason: 0 disponible; 1 modo; 2 MPU; 3 balance; 4 perfil motores; 5 perfil IR; 6 línea/marca; 7 ocupado; 8 argumentos; 9 obstáculo/lectura fuera del perfil; 10 timeout; 11 guardado; 12 cancelado; 13 intención rechazada por MotionControl; 14 desvío de rumbo durante calibración. En GET, status=0 describe la consulta; reason puede conservar el fallo de la misión anterior.

### Perfil motores — 88 bytes

Offsets 0 status u8; 1 version=1; 2 pointCount u8 0..5; 3 irCalibrated u8; 4 revision u32. Desde offset8, cinco slots de16 bytes: percent f32, speedCmS f32, rampMs u32, runs u32. Slots no activos no participan. Percent ascendente 1..20; velocidad >0..300 cm/s; rampa 0..5000; runs 1..1000. Importación conserva el perfil IR existente y genera nueva revisión; no toma la revisión del archivo como identidad del equipo. Rechazo de este formato solo expone status, no reason; clientes dan diagnóstico genérico y permiten consultar estado.

### Perfil IR — 186 bytes

Byte0 version=1. Cinco canales de37 bytes desde offset1: baseline ADC u16, noise ADC u16, count u8 y ocho slots(distanceMm u16, adc u16). Orden: objectLeft45, objectLeftCenter, objectCenter, objectRightCenter, objectRight45. Entre2 y8 muestras activas por canal, distancia positiva ascendente, ADC0..4095. Cliente convierte distanciaCm a mm, mediana ADC y desvío estándar redondeado hacia arriba. Perfil mayor no se recorta automáticamente. F4 persiste los cinco canales junto al perfil motores.

### Persistencia y pantalla

Preferencias pasan de V12 a V13 con lector/migración V12 y perfil autónomo inicialmente vacío. No se escribe Flash mientras el vehículo está armado. El fallo al guardar conserva el perfil anterior; no hay vencimiento temporal automático ni tensión de batería obtenida por ADC.

Nueva pantalla MOTOR_CALIBRATION 0x040104 (menu04/submenu01/page04), renderer OledUtils_RenderMotorCalibration y entrada Calibración1m con icono Tool en Motores. Encoder modifica porcentaje/rampa y pulsación corta cambia campo; USER corto inicia/cancela/guarda; encoder largo cancela y vuelve. Web y Qt identifican el código. No se modifica 0x52/0x95/0x96 para publicar un framebuffer o todos sus valores; métricas llegan por consultas nuevas. No afirmar paridad visual completa.

## Flujo completo

1. Web pide por WebSocket autenticado al ESP; Qt por ProtocolFacade y su transporte actual. Ambos usan sesión STM existente. ESP autoriza la entrada y enruta UNER a F4, dueña del movimiento.
2. Comandos nuevos completan por el mismo ID, longitud fija y respuesta correlacionada. No tienen ACK intermedio ni eventos nuevos. El servicio continúa localmente y clientes consultan estado aproximadamente cada600 ms.
3. Estado completo incluye status de operación. El envelope JSON del bridge puede confirmar entrega aunque status=1; clientes deben interpretar status/reason de F4, no solo ok del transporte.
4. ESP usa timeout500 ms; Web requests2000 ms. Qt usa política existente del catálogo. Consultas son idempotentes; acciones de movimiento/guardado no se reintentan automáticamente. Ante timeout, consultar antes de repetir.
5. GOTO requiere modo3, MPU fresco/calibrado, perfil motores/IR, emisores de objetos, balance estable y ausencia de bloqueo. Calibración permite modo0 o3 y añade línea inferior calibrada y centro inicialmente sobre negro.
6. Origen al inicio, X derecha/Y adelante; distancia10..200 cm, tolerancia estimada10 cm, límite30 s. Evasión reactiva conservadora con cinco sensores. Lecturas no recientes/fuera de curva, desvío de calibración >12 grados, bloqueo o timeout terminan la intención.
7. Cancelar conserva balance. Parada general desarma; transiciones de modo y preparación incompatible cancelan. Desconexión del cliente no cancela la misión F4.

## Impacto en repositorios

| Repositorio | Estado | Acción / referencia |
| --- | --- | --- |
| FirmwareF4 | origen | b9ef8d4: auto_motion, puertos en app_tasks, arbiter MotionControl, UNER, preferenciasV13, menú/OLED |
| Firmware ESP01 | adaptado | 36ef92a: catálogo, dispatch y respuesta JSON del bridge |
| Web ESP01 | adaptado | e216a1f: Libre/GOTO, ruta /control/calibracion, perfil/historial, requisitos y selector modal compartidos con Seguidor, Protocol Studio |
| AutoMicroQtDesktop | adaptado | 0963d75: catálogo/codec/controlador, QML GOTO/calibración y requisitos compartidos, identidad OLED |

## Compatibilidad y orden de integración

Actualizar F4/ESP y luego Web/Qt. Firmware viejo no confirma el estado v1; las interfaces muestran estado desconocido y bloquean inicio. Los comandos anteriores conservan wire y el enum de fuente Safety mantiene su valor anterior. La funcionalidad nueva requiere los cuatro consumidores actualizados. El bundle LittleFS no fue generado ni copiado en esta tarea: sigue pendiente de un build autorizado. Preferencias anteriores necesitan migración física por comprobar, no declarada compatible por inferencia.

## Evidencia y límites

Cambios de fuente y commits creados, junto con esta ficha y el plan ejecutado. No se ejecutaron tests, builds, typechecks, validaciones, navegación visual, ensayo físico ni reflasheo. No hay encoders de rueda ni posición absoluta medida; velocidades y llegada son estimaciones. La curva del metro no demuestra aceleración instantánea. Permanecen pendientes compilaciones, bundle ESP, migración Flash y comprobación física de distancia/giro/IR/seguridad en los cuatro componentes.

## Actualizaciones posteriores

- 2026-10-07: implementación inicial en las cuatro fuentes. Solo la central se publica; commits de código quedan locales.
- 2026-10-07, pedido posterior de compilación: F4 Debug, ESP esp01_1m, Web build:esp y Qt Release completados con exit0. Corregidos cierre JSX de Control (443e729) y separador QML (2cff854); gzip copiado a ESP (1d3d2a5). Se incluyó auto_motion.c en las recetas Debug generadas. [Resultados, artefactos y límites](../docs/BUILD_RESULTS_20261007.md). Sin tests, ejecución visual, flasheo ni hardware; reemplaza el pendiente de compilación/bundle, conservando la evidencia inicial como historia.

