# AMI-20261003-01 — control de motores y balance en F4, ESP, Web y Qt

- Fecha: 2026-10-03 (America/Buenos_Aires).
- Origen: FirmwareF4.
- Referencia de código: cambios locales de los cuatro checkouts; commits de implementación pendientes. La central se publica por separado.
- Estado: implementado en fuente; sin compilación ni prueba física.
- Área: UNER v2, WebSocket JSON v1, USB, telemetría y flujo de movimiento.
- [Auditoría y límites](../audits/2026-10-03-mpu-motors-balance.md).

## Contrato anterior y nuevo

Antes había PID, arbitraje local de marcha y driver PWM; no existían requests remotos dedicados de balance/marcha/TEST. Web mostraba simulación y secuencias pendientes de ejecución física. Se asignan cinco IDs previamente reservados:

| UNER | JSON | Request exacto |
| --- | --- | --- |
| 0x7B GET_MOTION_STATE | `stm.motion.get` | Vacío. |
| 0x7C SET_BALANCE_ENABLED | `stm.balance.setEnabled` | `enabled:u8` 0/1; JSON `{enabled:boolean}`. |
| 0x7D SET_MOTION | `stm.motion.set` | `forward_percent:f32 LE`, `turn_percent:f32 LE`, `duration_ms:u16 LE`: 10 bytes. JSON `{forwardPercent:number,turnPercent:number,durationMs:integer}`. |
| 0x7E STOP_MOTION | `stm.motion.stop` | `disable_balance:u8` 0/1; JSON `{disableBalance:boolean}`. |
| 0x7F SET_MOTOR_TEST | `stm.motor.test` | `left_percent:f32 LE`, `right_percent:f32 LE`, `duration_ms:u16 LE`: 10 bytes. JSON `{leftPercent:number,rightPercent:number,durationMs:integer}`. |

Porcentajes finitos en [-100,100]; duración en [50,2000] ms, sin variante indefinida. En marcha, forward/100 × 6° establece el objetivo de pitch; turn/100 × 25% solicita diferencial sujeto al margen del balance. No se envía avance como PWM común adicional. TEST usa porcentajes de esfuerzo con signo por rueda, no velocidades medidas.

Los cinco comandos responden por **el mismo ID**, siempre con ocho u8: `[status,armed,balance_active,command_active,safety_stop,reject_reason,car_mode,motor_test_active]`. Los booleanos son 0/1; `car_mode`: 0 IDLE, 1 FOLLOW, 2 TEST, 3 GOTO. JSON entrega `status`, `motionArmed`, `balanceActive`, `commandActive`, `safetyStop`, `rejectReason`, `carMode`, `motorTestActive`.

`status`: 0 aplicado/consulta, 1 argumento inválido, 2 modo incompatible, 3 rechazo de control/armado. `reject_reason` conserva el enum F4: 0 ninguno, 1 sin inicializar, 2 MPU no listo, 3 calibrando, 4 inclinación límite, 5 deshabilitado, 6 desarmado; se agregan 7 lectura vencida, 8 parada enclavada, 9 orden inválida. Es contexto del control: con status 1/2 puede conservar el motivo anterior. El motivo no sustituye a status.

0x60/0x90 conservan 42 bytes, float32 LE en grados, g y grados/s, secuencia u16 y dt_us u16. Status 3 indica muestra no disponible/vencida y no se marca sample_valid. Sus flags reflejan estado actual. 0x64 conserva 92 bytes: ahora status 3 expresa orientación MPU no disponible/vencida; posición de campos, longitudes y unidades se conservan. Pitch medido es MPU actual incluso desarmado; salidas izquierda/derecha corresponden al esfuerzo aplicado por el driver, incluido TEST. Se inicializa orientación antes de leerla para evitar datos indeterminados. No son mediciones de RPM.

## Flujo completo

1. Web solicita al ESP con requestId por API v1; Qt solicita directamente a F4 por USB UNER. ESP asigna el nodo Web y usa UART; Qt usa nodo 2. F4 es la autoridad de las guardas, modos y salidas.
2. ESP registra los IDs con dueño F4, requests exactos, respuesta de ocho bytes, política SAME_COMMAND, timeout de registro 100 ms y privilegio STM_SESSION. Solo GET es idempotente; las mutaciones no se reintentan automáticamente. La tabla local `uner_cmdspec` admite recepción de requests/responses; el registry fija longitudes por dirección.
3. `requestAccepted` del ESP confirma encolado, sin implicar aplicación. No hay ACK intermedio nuevo: la respuesta final proviene de F4 y completa la operación. UNER carece de requestId; ESP serializa y correlaciona por ID/nodos, Qt usa su broker USB.
4. ESP convierte status no cero en error final y conserva los datos del resultado; Qt rechaza funcionalmente status no cero. Continúan los NACK de autenticación/transporte/origen existentes. Longitud no registrada puede descartarse por el router F4 y terminar por timeout.
5. Las interfaces usan timeout de espera Web 2000 ms / codec Qt 500 ms para órdenes nuevas, pulsos de 750 ms y consulta de estado aproximadamente cada segundo. Se requiere sesión STM. `hello.features.motionControlV1` anuncia soporte de bridge ESP, no versión o capacidad físicamente comprobada de F4; las interfaces requieren respuesta válida de GET antes de habilitar marcha/TEST.
6. ARM exige calibración, muestra de ≤50 ms y pitch dentro de ±10°. Caída >35°, MPU inválido/calibrando/vencido o control no finito detienen y enclavan; no hay reanudación automática al levantar el auto. Desarmar no detiene la adquisición MPU. Cada muestra nueva actualiza PID con tiempo de adquisición; ticks acumulados no repiten la misma lectura.
7. Marcha remota se admite en IDLE/GOTO, con balance armado y sin parada. FOLLOW mantiene su productor local de intenciones. Detener o vencer marcha borra la orden y vuelve a objetivo 0° manteniendo compensación. `disableBalance:true` apaga motores y desarma. TEST requiere modo TEST y desarmado, y apaga PWM al vencer. Entrar/salir de TEST elimina órdenes y desarma.

No hay persistencia del armado ni del pulso, eventos nuevos ni promesa de aplicación mecánica al aceptar una orden. El temporizador depende de que el lazo de aplicación F4 siga ejecutándose; no introduce un watchdog de hardware. La calibración requiere inmovilidad y la referencia vertical física correcta.

## Impacto en repositorios

| Repositorio | Estado | Acción concreta |
| --- | --- | --- |
| FirmwareF4 | origen | Comandos/guardas, reintento y frescura MPU, calibración inmóvil, arbitraje/parada enclavada y reporte del driver. |
| Firmware ESP01 | adaptado | IDs, registro, JSON, payloads/respuestas y anuncio de soporte del bridge. |
| Web ESP01 | adaptado | Control real en Motores y Balance; estado confirmado, lectura de telemetría y catálogo Protocol Studio. Las secuencias completas siguen sin upload/ejecutor. |
| AutoMicroQtDesktop | adaptado | Catálogo/codec/API USB, tratamiento de rechazo, controlador autenticado y panel QML. |

## Compatibilidad y orden de integración

Los IDs nuevos no colisionan con comandos vigentes. Clientes anteriores conservan comandos previos; reciben ahora error/status 3 al consultar orientación MPU inválida en lugar de ceros presentados como éxito. Qt nuevo también rechaza 0x64 no exitoso. F4 anterior no implementa 0x7B–0x7F: se esperan timeout/rechazo, sin habilitación optimista de acciones UI.

Integrar primero F4 y ESP, después Web/Qt. Compilar y flashear/publicar componentes cuando el usuario lo solicite; este turno no produce binarios ni actualiza `data/dist` ESP. No declarar sincronización de clones por publicar solo esta ficha.

## Evidencia y límites

Productor y consumidores enumerados en la auditoría enlazada. No tests, builds, validadores, revisión visual ejecutada, tráfico real ni ensayo físico. Defaults y signos del lazo no acreditan estabilidad mecánica. PID, dirección de motores, vertical, mínimos PWM y frecuencia real requieren puesta a punto. Captura sincronizada y ensayo de PID con reversión de AMI-20261001-01 siguen pendientes.
