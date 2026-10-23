# Seguimiento de línea — implementación en fuentes

Actualización posterior 2026-10-06: el usuario solicitó compilar. Los cuatro builds finalizaron correctamente, con correcciones detalladas en [resultados](BUILD_RESULTS_20261006.md). Bundle ESP y ejecutable Qt generados; flasheo, ejecución visual y comprobación física continúan pendientes. El texto de entrega inicial de abajo conserva su contexto previo a esa autorización.

2026-10-06. Plan implementado en F4, ESP, Web y Qt. [Contrato AMI-20261006-01](../changes/2026-10-06-01-line-follow-calibration.md). Sin pruebas, compilaciones ni validaciones por instrucción del usuario.

## Bibliotecas y flujo

DMA completo → adaptador APP → calibración portable → perfiles blanco/pista → preferencias V12.
MPU existente → pitch/roll/yaw, velocidad y frescura → calibración y condiciones de estabilidad.
DMA + perfiles + luces → evaluador compartido → chequeo de cinta, reporte IR y seguidor.
Seguidor → intención de avance/giro con lease → MotionControl → BalanceControl + MotorDrive.
OLED / Web→ESP / Qt→USB o AT → endpoints F4 → servicios APP.

El core no depende de HAL, MPU ni motores concretos. Calibración recibe callbacks de iluminación/restauración/guardado; seguimiento recibe read/command/hold y geometría propia. APP conecta handles existentes; se conserva arbitraje I2C del MPU. Balance mantiene autoridad sobre salida común y margen de giro.

## Estados, propiedad y cancelación

F4 conserva calibraciones, revisiones, neutral de balance y seguimiento. Sesión remota pertenece a transporte+nodo+ID; OLED conserva ID local. Abrir presenta instrucciones; OK inicia quietud/captura/guardado. Cancelación, fallo y timeout restauran iluminación. La captura detiene seguimiento/TEST, desarma y no reanuda al terminar. La ISR acumula enteros; procesamiento y Flash permanecen en el loop. Reporte raw/ruta usa el mismo scan completo.

FOLLOW no inicia automáticamente. Para comenzar se requieren las dos referencias, IR recientes/activos, patrón centrado y balance físicamente estable. Detener seguimiento conserva balance. Fallos requieren reinicio explícito; balance puede desarmar ante su propio fallo. Recuperación hacia último lado hasta 700 ms sin avance; ambigüedad sostenida detiene. No se implementa navegación por cruces.

## Calibración

Primero tres sensores sobre blanco; después pista ideal blanco/negro/blanco, alineado al rumbo, vertical y quieto. Cada opción mide IR apagados/luces apagadas; IR encendidos/luces apagadas; ambos encendidos; IR apagados/luces encendidas.

Por fase: 50 ms de asentamiento, 80 ms de descarte y 1.280 scans útiles por sensor (320 ms a 4 kHz). Cuatro fases más 500 ms de quietud: aproximadamente **2,3 segundos por opción más guardado**, 5.120 muestras útiles por sensor. Ambas opciones requieren unos 4,6 segundos, sin colocación humana. La cantidad efectiva determina finalización; tiempo transcurrido no basta.

Se guardan mediana de medias, ruido, muestras y pitch/roll/yaw. Límites de calidad/postura/timeout detallados en la ficha. Rechazo conserva referencia anterior; blanco nuevo o recalibración MPU invalida pista. Fondos A/D sirven para calidad; no se apagan periódicamente emisores durante seguimiento para medir ambiente.

## Interfaces y archivos

OLED incorpora Infrared→Calibrar→Blanco/Pista y Seguir pista en FOLLOW. Web agrega calibración del vehículo al espacio IR y modal dentro de FOLLOW. Qt añade componentes QML en Calibración y Seguidor. Registros locales anteriores siguen como comparación y no habilitan el seguidor.

Modal de pendientes al intentar iniciar; botón Seguir línea/Detener seguimiento cambia tras confirmación F4. Balance se activa por separado. Calibrar no deja un inicio automático pendiente.

F4: line_calibration.h, line_follow_control_types.h, line_follow_control.c, app_tasks.c, app_preferences.c, motion_control.c, OLED y uner_app.c. ESP: registro, uner_cmdspec y ws_bridge dispatch/reports. Web: features/lineFollow, LineWorkbench, TrackFollowerSection, API/catálogo. Qt: MotionController, F4ApiCodec, catálogo/broker y QML LineDeviceCalibration/LineFollowPanel.

Visual-identity-system aplicado sobre manual existente autorizado y tokens/componentes; Qt usa Theme, GlassCard y ActionButton. Disposición nueva propuesta, sin render comprobado; notas de cobertura en ambos repositorios.

## Entrega y pendientes

Código local en los cuatro repositorios, preservando modificaciones previas y sin incorporarlas en commit. Registro central publicado según su política. No se regeneró bundle ESP, ejecutable Qt ni firmware.

Pendiente cuando se solicite: compilar, generar/copiar Web, flashear y comprobar montaje, polaridad real, calidad óptica, persistencia, coexistencia de interfaces y ajuste PID en pista. Convención MPU sigue estimador existente; requiere confirmación física. No hay evidencia física en esta entrega.
