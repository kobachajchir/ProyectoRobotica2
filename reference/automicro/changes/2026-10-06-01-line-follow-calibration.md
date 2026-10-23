# AMI-20261006-01 — calibración óptica persistente y seguimiento con balance

- Fecha: 2026-10-06, America/Argentina/Buenos_Aires.
- Origen: FirmwareF4; adaptación ESP, Web y AutoMicroQtDesktop.
- Referencia: fuentes locales modificadas, sin commit de código consumidor en esta entrega. Se preserva trabajo previo en los cuatro repositorios.
- Estado: implementado en fuentes, sin compilación, tests, validación visual ni prueba física por pedido del usuario.
- Área: UNER, WebSocket JSON, API, screen codes, adquisición DMA, preferencias y control.

## Contrato anterior y nuevo

FOLLOW deja de activar automáticamente el seguidor. Entrar al modo permite operar; activar balance y comenzar seguimiento son acciones separadas. Detener seguimiento anula avance/giro y conserva balance. Se reemplaza la clasificación fija del chequeo de cinta y del reporte IR por el evaluador calibrado compartido con el seguidor.

Nuevos comandos F4, autenticados por sesión STM, respuesta final en el mismo ID:

| ID | API semántica Web/Qt | Request | Respuesta |
| --- | --- | --- | --- |
| 0x1D | `stm.ir.lineCalibration.control` | 5 bytes: operation:u8, sessionId:u32 LE | estado v1, 168 bytes |
| 0x1E | `stm.ir.lineCalibration.get` | vacío | estado v1, 168 bytes |
| 0x1F | `stm.lineFollow.setEnabled` | enabled:u8, exclusivamente 0/1 | estado v1, 168 bytes |

JSON de control: `{operation:0|1|2|3, sessionId:number}`. 0 abre blanco y 1 abre pista, con sessionId=0. 2 confirma OK y 3 cancela, con el ID no nulo obtenido al abrir. F4 correlaciona confirmación/cancelación con ID, transporte y nodo propietarios. Una sesión OLED nueva no puede confirmarse con la propiedad remota anterior. Los clientes conservan propiedad únicamente tras apertura confirmada.

JSON de seguimiento: `{enabled:boolean}`. Esquema de respuesta ESP: `uner.line.state.v1`. Una respuesta bien formada con status=1 informa rechazo de dominio y el estado real; no se convierte en aceptación ni en error de transporte.

### Estado binario v1

Enteros multibyte little endian; ángulos float32 IEEE754 en grados. Matrices en orden fase A/B/C/D y sensores izquierdo/central/derecho.

| Offset | Campo |
| --- | --- |
| 0 | status:u8, 0 aceptado/lectura, 1 rechazado |
| 1 | version:u8=1 |
| 2..7 | calibrationState, target, phase, progress, calibrationReason, followReason: seis u8 |
| 8 | flags:u8: blanco válido bit0, pista válida bit1, balance estable bit2, siguiendo bit3, armado bit4, balance activo bit5, parada seguridad bit6, ruta válida bit7 |
| 9..11 | carMode, pattern (izquierda=4, centro=2, derecha=1), confidence 0..100 |
| 12..23 | sessionId, whiteRevision, trackWhiteRevision: tres u32 |
| 24..35 | usefulScans actual, whiteUsefulScans, trackUsefulScans: tres u32 por sensor |
| 36..47 | neutralPitchDeg, referenceRollDeg, referenceYawDeg: tres float32 |
| 48..71 | blanco: medias 4×3 u16 ADC |
| 72..95 | blanco: ruido 4×3 u16 ADC |
| 96..119 | pista: medias 4×3 u16 ADC |
| 120..143 | pista: ruido 4×3 u16 ADC |
| 144..155 | blanco: pitch, roll, yaw, tres float32 |
| 156..167 | pista: pitch, roll, yaw, tres float32 |

JSON mantiene los nombres superiores `sessionId`, `neutralPitchDeg`, `referenceRollDeg`, `referenceYawDeg`; `white` y `track` contienen `means`, `noise`, `usefulScans`, `pitchDeg`, `rollDeg`, `yawDeg`. No se importa el historial local Web/Qt como calibración del dispositivo.

calibrationState: 0 inactivo, 1 preparar/OK, 2 quietud, 3 captura, 4 guardado, 5 completado, 6 fallido, 7 cancelado. target 0 blanco/1 pista; phase 0..3 durante captura, 4 tras terminar; progress 0..100.

Motivos: 0 ninguno, 1 detenido, 2 MPU no listo, 3 desarmado, 4 pitch inseguro, 5 ancho inválido, 6 calibración pendiente, 7 IR obsoleto, 8 ambiguo, 9 línea perdida, 10 movimiento rechazado, 11 modo incorrecto, 12 balance no estable, 13 calibración ocupada, 14 no centrado, 15 iluminación asentándose, 16 postura inestable, 17 calidad insuficiente, 18 guardado fallido, 19 sesión inválida, 20 timeout. El rechazo puntual del request se informa en ambos campos de motivo de su respuesta; lecturas posteriores muestran los motivos propios de cada servicio.

### Calibración y persistencia

Dos operaciones separadas: todos sobre blanco, luego pista ideal blanco/negro/blanco, auto vertical, quieto y alineado al rumbo de la cinta. Cada operación desarma motores y detiene seguimiento/TEST. La captura controla exclusivamente emisores inferiores y luces blancas; apaga emisores de objetos y restaura la iluminación física anterior al terminar, cancelar o fallar. SET_IR_EMITTERS devuelve status=1 durante captura y no cambia luces.

Fases: A IR apagados/luces apagadas; B IR encendidos/luces apagadas; C IR encendidos/luces encendidas; D IR apagados/luces encendidas. Sí se mide IR apagado con luces encendidas para registrar ese fondo.

TIM3 configura 4.000 scans ADC/s. Cada fase deja 50 ms de asentamiento y 80 ms de descarte, luego acumula **1.280 scans útiles reales por sensor**, aproximadamente 320 ms: 450 ms/fase, 1.800 ms para cuatro fases. Antes exige 500 ms de quietud: aproximadamente **2,3 s por operación más guardado**, 4,6 s ambas, excluyendo preparación humana. El progreso depende de scans completados, no de asumir una frecuencia. Timeout de preparación 60 s, quietud 3 s, fase 2 s. No hay flash dentro de ISR ni con motores armados.

Mediana de 32 medias de ventanas de 40 scans; ruido como desviación estándar de los 1.280 scans. Se rechazan saturación, ruido activo >40 ADC, respuesta blanca menor a max(20 ADC,6×ruido), contraste central menor a max(100 ADC,8×ruido), laterales de pista alejados de su blanco y MPU inválido/móvil. Límites iniciales sujetos a ajuste físico posterior.

Preferencias pasan V11→V12, conservando lector V11 y anteriores. Se guardan dos perfiles y revisiones; blanco nuevo invalida pista. Fallo conserva referencias previas. Cambiar/borrar calibración MPU invalida pista en la misma transacción de preferencias, porque cambian los ángulos de referencia. La calibración MPU que ya cambió RAM invalida también pista RAM aunque falle Flash. No se arma ni comienza seguimiento automáticamente después de calibrar.

### Control y pantallas

El centro tiene blanco/negro propios; los laterales usan desviación respecto de su propio blanco, sin inventar referencias negras laterales. Se selecciona el perfil de trabajo B o C según luces blancas. A/D sirven para calidad/fondo de calibración; no hay medición ambiente continua con emisores apagados durante seguimiento.

MPU aporta pitch, roll, yaw y gyro Z con frescura. Neutral de balance deriva del pitch ideal de pista. Para iniciar: modo FOLLOW, dos calibraciones vigentes, IR inferiores activos/recientes, patrón 010 centrado 20 ms, MPU válido ≤50 ms, armado y balance activo, 500 ms continuos con error pitch ≤2°, velocidad pitch ≤5°/s y desvío roll ≤3°. La F4 vuelve a comprobar al recibir la orden; la interfaz no puede eludirlo.

Giro positivo del puerto significa antihorario visto desde arriba: negro a izquierda genera giro positivo y a derecha negativo. El adaptador usa mezcla izquierda=común−giro, derecha=común+giro. Balance conserva autoridad sobre salida común y margen de giro. Gyro Z sigue la convención del estimador MPU existente; polaridad/montaje requieren prueba física. Avance inicial 12%, PID lateral y amortiguación de yaw rate; no aplica rumbo magnético absoluto como target permanente en curvas.

Lease de movimiento 50 ms, renovado con nueva lectura; IR máximo 30 ms. Ambigüedad sostenida 100 ms y postura fuera de margen 100 ms detienen seguimiento. Recuperación hacia último lado hasta 700 ms sin avance; vencimiento requiere reinicio explícito. La protección final de caída/MPU del balance existente conserva autoridad.

OLED: Infrared→Calibrar→Blanco/Pista; OK comienza medición. FOLLOW añade Seguir pista/Detener seguimiento y acceso IR. Nuevos códigos: 0x030204 menú calibrar, 0x030205 blanco, 0x030206 pista, 0x030207 acción seguimiento, 0x030208 aviso. Web incorpora constantes, controles nativos y modal; Qt incorpora controles nativos QML. No se afirma reproducción exacta remota de estas nuevas pantallas OLED; sus identidades son adicionales, sus parámetros de captura se consultan con 0x1E.

Web/Qt muestran pendientes en FOLLOW sin abrir modal al entrar. El modal aparece **al intentar seguir** sin calibración; permite ir a calibrar. Seguir línea cambia a Detener seguimiento únicamente tras estado confirmado. El balance se activa por separado; calibrar no deja seguimiento pendiente de inicio.

## Flujo completo

1. Web→ESP mediante request JSON correlacionado; Qt→F4 por su façade USB/AT y codec vigente. Destino dueño: MCU/F4.
2. ESP empaqueta request UNER y espera el mismo comando; no hay ACK intermedio que habilite motores.
3. Abrir/OK devuelven estado inmediato; captura se consulta con 0x1E. La respuesta a OK no significa captura guardada. Consulta UI cada 300 ms, una secuencia de consultas por controlador; Web reduce a 5 s ante fallo. Mutaciones sin reintento automático.
4. Captura completa solo después de calidad y transacción Flash; cancelación/timeout restaura luces. No se agrega evento de línea; consultas informan estado.
5. Timeout de catálogo ESP/Qt 500 ms; envoltorio Web 2 s. Se mantiene sesión STM y autorización por endpoint del sistema existente; no se amplía acceso público.

## Impacto en repositorios

| Repositorio | Estado | Acción |
| --- | --- | --- |
| FirmwareF4 | origen | Core portable, puertos app, DMA, balance, preferencias, OLED y endpoints |
| Firmware ESP01 | adaptado | Registro 0x1D..1F, validadores JSON, routing y decoder 168 bytes |
| Web ESP01 | adaptado | Hook y controles reales, calibración IR/FOLLOW, modal, catálogo y tokens |
| AutoMicroQtDesktop | adaptado | Catálogo/codec/broker, controlador y componentes QML |

## Compatibilidad y orden de integración

Actualizar F4, ESP, después Web/Qt. Firmware anterior carece de los tres comandos y no puede habilitar esta función. Se mantiene el tamaño de respuestas motion/control/IR existentes (8/64/56 bytes); cambia la semántica de ruta válida/clasificación a calibración real. Para volver a firmware anterior, el lector anterior no leerá registros V12: no se afirma downgrade transparente.

El bundle Web alojado en ESP no se regeneró: requiere build/copia posterior. No se flashearon placas ni generó ejecutable Qt. Código de los cuatro repositorios queda local y sin commit para no incorporar modificaciones previas ajenas. Solo el registro central se publica por la política del repositorio.

## Evidencia y límites

Cambios de fuente y documentación. No se ejecutaron tests, builds, typecheck, auditorías, comparación de paridad ni validación visual/física. Compilación, recursos, signado de giro real, calidad óptica, ajuste PID, persistencia física y ejecución simultánea deben comprobarse cuando el usuario lo solicite. La implementación no se declara lista para circulación física sin esa comprobación.

## Actualizaciones posteriores

- 2026-10-06: implementación en fuentes de los cuatro repositorios y registro inicial; visual-identity-system aplicado sobre identidad existente en Web y Theme Qt, sin render.
- 2026-10-06, pedido posterior expreso: F4 Debug, ESP esp01_1m, Web build:esp y Qt MSVC Release completaron con exit 0 después de corregir acceso a helpers OLED, exceso Flash Debug (-Og), encabezado C/C++ ESP y errores TypeScript. Bundle gzip copiado a ESP; ejecutable Qt y binarios generados. Las afirmaciones iniciales de falta de compilación/bundle quedan superadas por este resultado. Sin tests, ejecución visual, flasheo ni hardware. [Resultados, comandos, tamaños y límites](../docs/BUILD_RESULTS_20261006.md).
