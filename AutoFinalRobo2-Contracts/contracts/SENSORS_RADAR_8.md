> AFR2-20261029-27: navegación, escala y editor vigentes en [QUICK_RADAR_ACTUATORS_9](QUICK_RADAR_ACTUATORS_9.md). Offsets de telemetría anteriores conservados; hardware pendiente.

> **Fuente AFR2-20261029-16:** OLED sin esperas artificiales entre etapas, radar con grilla punteada, cono visual nominal15grados por PWM aplicado y discos rellenos por eco válido. Copia Web vinculada; sin build Web/promoción ESP. [Ficha](../changes/2026-10-29-16-oled-latency-radar-cone.md). Build/carga AVR COM8 autorizados, pendientes al registrar esta nota. Se conserva corrección del watchdog y migración Hall de15, ya cargada y verificada enCOM8 a12:02:57.

> **Actualización de compilación14:** pedido humano posterior autoriza compilar/corregir. ATmega/ESP/Web full: exit0, sin errores ni advertencias finales. [Informe](../documentation/builds/2026-10-25-14/README.md). Sin tests, validadores independientes, navegador, montaje físico ni flasheos. Paquete local full generado; no promovido a ESP/data/dist. El texto de entrega sin compilar inferior es histórico.

# Sensores y radar autónomo — EX01 v8

AFR2-20261025-14, 25/10/2026. Fuente integrada, **sin compilar, probar ni flashear**. Firmware nominal 0.10.0, Web 0.3.0. Sustituye solamente la supervisión de parada y las pantallas de sensores de EX01v7; continúa el control directo sin permisos/leases. ROBO2/perfil2 y afr2-ws-1 conservados.

## Propiedad y tiempos

ATmega es el único productor de IR, ECHO y PWM. ESP traduce y consulta; Web no calcula mediciones a partir de la animación. HC-SR04 TRIG D30 / ECHO D31, Timer3 CTC 25 µs durante adquisición: subida detectada por muestreo, duración hasta bajada, TRIG 25 µs, timeout total 32 ms. Resolución temporal nominal 25 µs, distancia width*250/58 mm: pasos de unos 4,31 mm, sin certificación de exactitud.

Ficha ElecFreaks: [datasheet](https://cdn.sparkfun.com/datasheets/Sensors/Proximity/HCSR04.pdf) y [guía](https://elecfreaks.com/download/EF03085-HC-SR04_Ultrasonic_Module_User_Guide.pdf): TRIG >=10 µs, rango nominal 20–4000 mm, recomendación de ciclo >60 ms. El firmware conserva mínimo/default 70 ms, máximo configurable de producto 1000 ms. El timeout32 no es el intervalo de disparo ni un valor garantizado por esa ficha. Distancias fuera de rango son inválidas, no se saturan a un obstáculo de 4 m ni equivalen a camino libre.

Servo D13 / Timer5: periodo20 ms, pulsos1000–2000 µs; target pendiente se aplica al inicio del siguiente pulso. Ángulo nominal0–180°, centro90°/1500 µs, inversión configurable. La posición publicada proviene del PWM aplicado; no mide el eje ni detecta giros manuales. La calibración geométrica y los topes reales siguen pendientes. Desactivar no centra.

IR1..IR5: digital0/1 o ADC0..1023, originales disponibles independientemente de la calibración de seguimiento. No se copian los ocho canales/ADC12 bits ni control de emisores del F4. `rawValid` no equivale a `valid` para control de línea.

## Barrido y parada

Paso5°, ida/vuelta con límites múltiplos de5 (min0..175 < max5..180). Default0..180°, velocidad10°/s (1..30), pausa100 ms (50..500), posicionamiento inicial300 ms. Se espera PWM aplicado, pausa, intervalo de disparo y final de eco antes de avanzar. Se descarta una adquisición anterior al iniciar radar. En extremos cambia dirección sin duplicar la posición. Configuración RAM: arranque detenido, sin reanudación y PWM apagado; no cambia EEPROM.

Tiempo por paso >=max(5000/speedDegS, settleMs+duración real del eco, intervalo entre disparos), con tarea principal y cuantización añadidas. La UI calcula un techo conservador usando32 ms como duración máxima del eco; no afirma que sea una velocidad medida.

Se exige tracción detenida para servo/radar. Radar activo bloquea movimiento, ruedas y secuencias; no comparte origen de actividad de los motores. Detener radar cancela barrido/PWM y conserva histórico. STOP explícito apaga tracción, cancela misiones/secuencias y apaga radar/PWM. Pérdida de Web o UART cancela solo tracción/misiones: el radar autónomo continúa. Fallo general o reinicio ATmega termina todo; reiniciar ESP no reanuda ni centra el servo.

Protección frontal: muestra válida <=200 ms, PWM aplicado1450..1550, sin target pendiente y estabilizado300 ms. Una medición lateral válida para radar nunca sustituye esa condición. Antes de conducir, centrar y recibir una medición frontal fresca. Desactivar PWM deja de acreditar orientación frontal.

## UART

Constantes compartidas en afr2_sensor_wire.h. Todos los enteros LE. Números siguientes cuentan exclusivamente el cuerpo después de session:u32,transaction:u32,result:u8 de respuesta; solicitudes conservan session/transaction de8 bytes. Cuerpo máximo55 bytes. No transmitir estructuras con sizeof.

| ID | Comando | Petición | Respuesta |
|---|---|---|---|
| 60 | robot.ultrasonic.get | 0 | 35 |
| 61 | robot.ultrasonic.configure | interval:u16 | interval:u16 |
| 62 | robot.ir.get | 0 | 27 |
| 63 | robot.servo.get | 0 | 18 |
| 64 | robot.servo.set | operation:u8,angle:u8 | 18, mismo estado servo |
| 65 | robot.radar.configure | write:u8,min:u8,max:u8,speed:u8,settle:u16,interval:u16,invert:u8 | min:u8,max:u8,speed:u8,settle:u16,interval:u16,invert:u8 (8) |
| 66 | robot.radar.start | 0 | running:u8=1 |
| 67 | robot.radar.stop | 0 | running:u8=0 |
| 68 | robot.radar.get | mode:u8,cursor:u32 | 23+16*count, count0..2 |
| 69 | robot.screen.status | 0 | screenCode:u32,navigationRevision:u32,selection:u8,firstVisible:u8,private:u8 (11) |
| 6A | MOTION_HALT, interno ESP | 0 | stopped:u8=1 |

Muestra de16 bytes: id:u32[0], measuredMs:u32[4], distanceMm:u16[8], echoTicks25us:u16[10], appliedPulseUs:u16[12], angleDeg:u8[14], error:u8[15]. id0 significa desconocido. Errores0 none,1 no-rise,2 no-fall,3 echo-high,4 out-of-range,5 moving,6 stale,7 unknown. Distancia solo válida con id!=0/error0/rango20..4000. La API JSON usa distanceMm:null para inválido y echoUs=ticks*25.

US35: boot:u32[0], now:u32[4], sample16[8], interval:u16[24], frontalEligible:u8[26], angle:u8[27], enabled:u8[28], appliedUs:u16[29], targetUs:u16[31], radarRunning:u8[33], scanRevision:u8[34].

IR27: boot:u32[0], sampleId:u32[4], age:u32[8] (FFFFFFFF desconocido), kind:u8[12] (0off/1digital/2adc), rawValid:u8[13], calibrated:u8[14], bits:u8[15], raw5*u16[16], controlValid:u8[26].

Servo18: boot:u32[0], appliedSequence:u32[4], appliedUs:u16[8], targetUs:u16[10], angle:u8[12], enabled:u8[13], age:u32[14]. SET operation0 apaga,1 ángulo0..180,2 centro. Respuesta SET no afirma que el nuevo target ya se haya aplicado ni que el eje haya llegado.

Radar: mode0 snapshot con cursor0..37 como siguiente bin; mode1 delta con cursor de muestra. Respuesta: boot:u32[0], now:u32[4], nextCursor:u32[8], dropped:u16[12], count:u8[14], running:u8[15], angle:u8[16], enabled:u8[17], appliedUs:u16[18], targetUs:u16[20], scanRevision:u8[22], records16[count] desde23. Snapshot recorre37 bins de0..180°; bins desconocidos llevan id0/error7. Se conserva medida/edad por bin, sin fingir un mapa instantáneo. Delta usa cola16, ordenada; informa pérdida saturada a65535 y Web repide snapshot. Cada inicio incrementa scanRevision (u8,0 reservado), que permite descartar el mapa anterior aunque Web no haya observado el estado intermedio detenido. Al cambiar boot se descartan secuencias/mapa de la sesión anterior. La comparación de secuencias usa resta modular.

Bits nuevos: SENSOR_HEAD0x8000, RADAR0x10000. Máscara0x1ffff. Negociación EX01v8 exacta en ESP y ATmega; `controlMode=direct` continúa. Requests inválidos usan BAD_REQUEST; ocupado/condición no cumplida usa UNAVAILABLE, que ESP expone como capacidad no disponible. No añadir permisos genéricos a UI_ACTION.

## WebSocket y Wi-Fi

Catálogo de41 comandos públicos. Payloads:
- ultrasonic.configure `{intervalMs}`; servo.set `{operation:'angle'|'center'|'off',angleDeg?}`.
- radar.configure `{write:false}` consulta; `{write:true,minAngleDeg,maxAngleDeg,speedDegS,settleMs,intervalMs,inverted}` aplica con PWM apagado.
- radar.get `{mode:'snapshot'|'delta',cursor:uint32}`; get/start/stop y consultas restantes sin argumentos adicionales.
- Cada muestra JSON incluye sampleId,angleDeg,pulseUs,distanceMm,echoUs,valid,error,ageMs,measuredAtMs; servo indica `positionSource:applied-pwm` cuando se consulta por su comando.
- screen.status no captura framebuffer. ESP emite robot.screen.changed al observar cambio de código o revisión. La acción remota sigue requiriendo la captura completa vigente; metadatos no dan autorización.

wifi.info es servicio ESP local, sin UART; reutiliza wifi.status y conserva ese comando. Añade connectionInterface de la IP local del socket Web, connectionRssiDbm (null en AP), apClients,channel,rssiSource=esp-sta. rssiDbm original refiere al enlace ESP-router en dBm; no dBi y no al cliente AP. No incluye claves.

Un lector Web compartido consulta cabeza100 ms y navegación250 ms; IR100 ms o ruedas200 ms solo en su tarea visible; Wi-Fi3 s. Sin consultas solapadas por lector, peticiones nuevas serializadas, sin toasts periódicos. Las lecturas poseen antigüedad propia. Se detiene el polling al ocultar la página, sin detener radar. STOP/cambio de sesión invalida peticiones Web pendientes.

## OLED y modelo

53 nodos /64 identidades (11 avisos conservados). Sensores lista Distancia y servo, Línea IR, Ruedas y Volver. Distancia y servo contiene vistas, control y parámetros. Códigos existentes030016 IR gráfica,030017 distancia numérica,030019 servo; nuevos030030 grupo,030031 radar,030032 IR valores,030033 ángulo,030034 radar automático.

Radar↔numérica e IR gráfica↔valores son hermanas: cada detent en cualquier sentido alterna sin apilar historial; corto restaura listado, selección y scroll. Largo800 ms abre principal y no produce corto al liberar. Ángulo: giro actúa localmente; corto vuelve. Web UI_ACTION puede navegar pero no girar el actuador indirectamente: se usa servo.set. `screens.json` es catálogo único.

Renderers enteros C/TypeScript y recursos16 iconos sincronizados. La copia local muestra diseño sin fabricar datos; el visor conectado conserva1024 bytes del AVR. `--copy-only` del sincronizador copia recursos sin ejecutar sus comprobaciones de build. La generación no acredita paridad visual.

Modelo Web: clones independientes, HC-SR04 y soporte giran alrededor de una referencia inferida del sensor/SG90 del GLB. Cuerpo del servo fijo; ese asset no separa un brazo animable. Coordenadas/pivote nominales, sin metrología ni modificación de CAD. Interpolación visual críticamente amortiguada, conservando velocidad al redirigir; reduced motion actualiza ángulo sin transición. Todas las secciones que usan RobotModel reciben ángulo compartido, excepto Modelo3D con animateServo=false. Nunca se generan muestras desde la animación.

## Presentación vigente Web — AFR2-20261026-02

La composición usa campo continuo, retícula de desvanecimiento radial, anillos
volumétricos y sector rubí de los tokens Control/GOTO. Perspectiva/Superior y
encuadre por límites proyectados; alcance visual inicial1m, opciones0.5/1/2/4m.
No cambia el tamaño del modelo, alcance físico ni tiempos del sensor. Los ecos
fuera del alcance visual se cuentan. La franja de dirección y altura de hitos
son recursos gráficos; no significan haz acústico ni altura medidos.

Distancia en mm como dato principal, ángulo aplicado diferenciado del solicitado,
PWM/antigüedad en Detalles. Parámetros en región inferior: selector doble y
barras/campos conservan los límites y la aplicación explícita de16. Sin conexión
se muestran desconocidos y el mapa existente permanece histórico. Sin lecturas
ficticias ni barrido decorativo; servo/aguja siguen datos y reduced motion.

Fuente26-02 sin compilación, navegador ni promoción ESP. [Ficha](../changes/2026-10-26-02-radar-composition.md).

## Presentación anterior Web — AFR2-20261025-16

Radar con proyección oblicua, plataforma y anillos tubulares de acento rubí,
contorno elevado y ecos como hitos 3D; la altura de los hitos es gráfica, no
medida. Mantiene escala física, procedencia y antigüedad de muestras. La aguja
sigue el ángulo recibido; ninguna medición proviene de su interpolación.

Desde/Hasta comparten barra0–180°, paso y separación mínima5°, con entradas
numéricas. Velocidad1–30°/s, estabilización50–500ms e intervalo70–1000ms usan
barra/campo vinculado, paso entero. Enter/blur limita y redondea; vacío/NaN
restaura el valor anterior y Escape descarta. El sector se actualiza al editar,
marcado como vista previa hasta aplicar. El histórico permanece visible.
Aplicar conserva `radar.configure`, RAM y bloqueo con radar/PWM activos. No se
inicia el radar con un borrador diferente a la configuración aceptada.
«Corregir giro invertido» explica el intercambio de pulsos extremos y centro90°.
No cambia el contrato UART/WS, los códigos OLED ni las capacidades físicas.

Fuente Web16 sin compilación, navegador ni promoción; [detalle](../changes/2026-10-25-16-radar-volume-scan-controls.md).

## Estado y límites de entrega

Fuente ATmega/ESP/Web y documentación integradas; recursos OLED copiados. No se ejecutaron tests, compilaciones, validadores, render/browser, promoción LittleFS, flasheos ni pruebas físicas. El paquete de ESP/data/dist y binarios siguen siendo los anteriores. Configuración nueva solo en RAM. Memoria adicional acotada (37 bins +16 registros), sin acreditar presupuesto SRAM/pila. La siguiente pasada requiere autorización expresa y está en la ficha14.
