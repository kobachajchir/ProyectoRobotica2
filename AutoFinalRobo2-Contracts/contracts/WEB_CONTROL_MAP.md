> **AFR2-20261030-13:** X adelante/Y izquierda/ángulo CCW conservados. Mezcla y odometría AVR y cinemática Web aplican el mismo ajuste de referencia longitudinal/yaw. [Contrato](MOTION_REFERENCE_13.md). No invertir otra vez botones, objetivos o pose. Solo fuente sin prueba física.

> **Vigente AFR2-20261025-02:** [ruedas simuladas](WEB_TOUCH_SIMULATION.md) en GOTO/secuencias y Control táctil. GOTO usa recorrido analítico y secuencias integran cada rueda con su reloj existente; pausa/reinicio/repetición conservan ángulos deterministas. No nuevo contrato de pose ni feedback real; vistas anteriores conservadas. Web compilada, visual No verificada.

> AFR2-20261025-01: las reglas anteriores de acceso quedan sustituidas por [control directo EX01v7](DIRECT_CONTROL_7.md). Sin adquisición, renovación, liberación ni habilitación OLED. Los formatos anteriores solo se conservan donde DIRECT_CONTROL_7 mantiene compatibilidad.

> Actualización13: Control táctil de Movimiento usa el mismo modelo final como referencia fija en un dialog, sin integrarlo como pose/simulación de este mapa. Reutiliza robot.drive existente. [Ficha13](../changes/2026-10-24-13-fullscreen-touch-drive.md).

# Mapa de Control y perfil de pose

## Vigente AFR2-20261024-12 — mapa y comando simultáneos

Mapa y comando actual comparten la columna principal. X/Y/rumbo/tiempo se
apilan en barra derecha junto al reproductor; en ancho<620px se apila debajo.
Recorrido cerrado muestra actual y medias vecinas sin scrollbar; abierto
incluye lista completa y Seguir activo. Recorrido local y detalles se pliegan.
Esto sustituye la disposición10 de coordenadas en fila. No cambia unidades,
modelo, cinemática local ni contrato de pose. La lista sigue sin emitir órdenes
al auto. Web compilada/promovida; sin comprobación visual/gestos/placas.
Distancia de bloques del editor usa [EX01v6](EX01_SEQUENCE_DISTANCE_6.md),
separado de esta cola GOTO local. Apartados inferiores conservan su historia.

## AFR2-20261024-10 — GOTO local por puntos y lista

Control/GOTO incluye simulador sin conexión: clic/toque sobre el suelo agrega
un punto absoluto del plano simulado, ajustado a100mm. Tres.js convierte
`worldX=-Y/1000, worldZ=-X/1000`; SVG invierte su propia matriz de pantalla.
La alternativa por teclado permite X/Y en metros. No se envía robot.goto.start
al seleccionar, editar o reproducir la lista. El formulario físico existente
permanece en su sección explícita con los permisos/calibración vigentes.

Comandos locales `move{direction,distanceMm}` y `point{xMm,yMm}` con ID de
edición. Forward=+X, backward=−X, left=+Y, right=−Y; pasos relativos al extremo
anterior, puntos absolutos del mismo marco simulado. Rumbo constante0, traslación
Mecanum ideal sin dinámica/obstáculos/odometría. Velocidad máxima50..500mm/s;
duración por tramo ceil(distancia/velocidad) en múltiplos de10ms, sin superar
la velocidad elegida. Interpolación analítica llega exactamente al destino y
conserva cada esquina en el recorrido. Datos solo de simulación, nunca robotState.

Límites:64comandos,600000ms, desplazamiento relativo100..10000mm, destino por
eje±50000mm. Destino coincidente, no finito o recorrido fuera de límite rechaza
el agregado/inicio con motivo; reordenar puede requerir corregir la lista.
Comandos y velocidad se conservan al cambiar de tarea en Control; el reproductor
se desmonta detenido y vuelve al origen. No persiste un run ni lo reanuda al volver.
Pausar/continuar, reiniciar, repetir, quitar/reordenar/vaciar; editar reinicia el
cursor y está bloqueado durante reproducción. STOP global, blur, ocultamiento
y salida pausan/cancelan RAF. La simulación usa actividad local simulation.

Mapa ancho completo con X/Y/rumbo/tiempo en fila. Lista debajo y controles a la
derecha en escritorio; se apilan en móvil. Comando activo con aria-current=step,
texto de estado y acento. Fade por capas opacity140ms ease; scroll local por
FLIP transform220ms easeInOut, tokens compartidos. Interrumpible, sin desplazar
página/foco. Scroll manual desactiva Seguir activo; reduced motion/teclado evita
transform y conserva estado. Fuente de animación: skill animate del usuario.

Web compilada y paquete promovido localmente. ATmega/ESP sin cambios de fuente,
EX01v5/ScreenCodes/gates conservados. Sin pruebas de navegador, comparación física
ni flasheos. [Ficha10](../changes/2026-10-24-10-goto-map-queue.md).

**Vigente AFR2-20261024-07:** Control muestra exclusivamente simulación local.
Se elimina el selector Robot real y el consumo de pose física por el mapa; las
consignas no se integran como mediciones. Conserva GLB compartido, metros, zoom
de cámara y grilla0,5m. El apoyo usa Box3 precise=true, desde vértices reales,
evitando esquinas inferiores artificiales de cajas de piezas giradas. Escala1.
Web compilada/promovida; render no verificado por denegación del navegador.
El perfil de pose inferior queda como contrato disponible para otros servicios,
sin implicar feedback conectado en Control ni odometría calibrada.

Vigente **AFR2-20261024-06**: Superior usa cámara ortográfica y Perspectiva/Seguir
auto usan perspectiva, todas con el GLB final de AutoFinalRobo2. Escala del
exportado:1unidad=1m; pose mm→m por1000; cuadrícula0,5m. Base del modelo en Y=0,
centrado solo horizontal; zoom afecta cámara, no dimensiones del robot. El SVG
se conserva únicamente como esquema de respaldo cuando3D no está disponible.
Pose, fuente Simulación, frescura y corte de control conservados. Web compilada y
promovida, sin prueba visual ni física. [Evidencia](../changes/2026-10-24-06-grounded-model-control.md).
Las descripciones iniciales inferiores de chasis esquemático/GLB independiente
corresponden a hitos anteriores. Las unidades CAD no acreditan calibración física.

AFR2-20261023-19: productor ATmega EX01 escrito, sin compilar/probar.
[Mapeo, captura/pose, permisos y complemento ESP pendiente](ROBO2_PRODUCT_EXTENSION.md).
No modifica el consumidor Web ni activa capacidades del broker por esta ficha.


AFR2-20261023-18. Diseño del consumidor Web; no implementa productores de posición, no asigna ID UART ni cambia el perfil ROBO2 de firmware. La posición no se obtiene de órdenes de movimiento ni de un modelo 3D.

## Mapa y datos

Control muestra una vista superior SVG y vistas Perspectiva/Seguir auto con Three.js bajo demanda. Cámara, zoom y centrado son acciones del visor; nunca envían movimiento o GOTO. Al cambiar cámara/zoom/centrado se cancela un hold manual si existía. El chasis representado es un esquema de cuatro ruedas con escala de referencia, no un CAD validado. El GLB de prueba sigue en su visor independiente, sin modificaciones.

Sin pose: esquema identificado y coordenadas «—». Con pose válida/fresca: posición/orientación y hasta256 puntos de trazo; el último punto no se desplaza al pulsar un control. Las consignas activas se identifican «Orden solicitada», con los valores que inició ese hold. STOP o pérdida de control limpia esa indicación; no se anuncia posición/movimiento confirmado por una orden.

## Lectura compartida

`ConnectionProvider` consulta `robot.state {}` por el Afr2Client existente: un único request en vuelo, timeout2500ms y siguiente lectura1000ms tras terminar. Solo ambos enlaces ready y página visible; sin toast por tick, sin segundo lector de modo en cabecera. `accepted`/error descarta proyección actual, no confirma resultado. Desconexión/cambio de endpoint limpia el snapshot. Cambios de visibilidad invalidan respuestas iniciadas antes de ocultarse. El modo conserva caducidad6000ms; el mapa exige pose≤2000ms.

Se preservan `mode`, `stopped`, `lease` y `ageMs` existentes. `stopped` se presenta como «Parada informada»; false no se toma como medición de velocidad física.

## Campo opcional `pose` — Propuesta, productores pendientes

Dentro del payload de `robot.state`, sobre `afr2-ws-1` y respuesta completed:

```json
{
  "pose": {
    "schema": "afr2-pose-1",
    "valid": true,
    "frameId": "identidad-del-origen-local",
    "sequence": 1,
    "xMm": 120,
    "yMm": -30,
    "thetaMrad": 80,
    "ageMs": 0
  }
}
```

Ejemplo de estructura de contrato, no datos usados en la interfaz. `pose` es opcional: ausente, inválido o incompleto produce desconocido, nunca ceros por defecto. valid debe ser true; schema exacto; frameId no vacío de hasta64 caracteres; sequence/ageMs uint32; xMm/yMm finitos dentro del rango int32 positivo/negativo simétrico ±2147483647; thetaMrad finito en ±3142. X avanza, Y apunta a izquierda; ángulo positivo antihorario mirando desde arriba. Marco local del controlador, no GPS. Unidades: mm y mrad; ageMs representa antigüedad de adquisición al preparar el reporte.

El consumidor exige wheel_odometry present/implemented/calibrated/valid y freshness=fresh, además de la validez de pose. No se eleva capability desde la Web. La edad visible suma ageMs y tiempo transcurrido desde recepción. No se extrapola ni interpola para aparentar mayor frecuencia.

Sequence debe aumentar dentro de frameId: duplicados/no crecientes no añaden trazo ni refrescan la edad aceptada. Antes de un wrap/reinicio de secuencia u origen, el productor cambia frameId. Un cambio de frame limpia trazo/centrado; desconexión/endpoint también. Cortes de frescura o intervalos de recepción>2500ms crean otro segmento, sin línea que invente recorrido durante el hueco. Trazo limitado a256 muestras. Dato caducado puede conservarse como captura etiquetada, con coordenadas actuales «—», y no sigue moviendo la cámara.

## Entrega a ESP/ATmega

Estado de esta ficha: Web con consumidor en fuente, no compilado/probado; shared diseño Propuesta; ESP/ATmega sin productor pose ni cambio binario. Los perfiles existentes no transportan este campo por sí solos. Deben acordarse adquisición y cálculo de odometría, identidad del origen, formato/versionado UART, buffers, frecuencia, edad y tratamiento de reset/errores antes de activar capacidades. No hay presupuesto AVR/flash ni exactitud física certificados aquí.

Aceptación pendiente bajo pedido: parsing inválido/ausente, duplicados, reset de marco, stale/disconnect, huecos de trazo, independencia entre cámara/órdenes, consulta única y fallback SVG sin WebGL. La existencia del mapa no acredita navegación ni seguimiento físico.

## Separación entre Simulación y Robot conectado — AFR2-20261023-21

En la tarea **Secuencias** de Control, el mapa reutiliza las cámaras **Superior**, **Perspectiva** y **Seguir auto**, separando explícitamente las fuentes **Simulación** y **Robot conectado**:
- La pose calculada por el simulador cinemático Mecanum determinista (pasos de `10 ms`, perfil ilustrativo no calibrado `lxPlusLyMm = 300`, `maxWheelSpeedMmS = 800`) se alimenta únicamente a la vista de **Simulación** y jamás ingresa al estado de telemetría (`robotState`).
- En modo **Robot conectado** sin odometría válida/fresca, no se dibuja una trayectoria física como observada.


## Integración posterior — AFR2-20261024-01

La ruta vigente se precisa en [EX01_WEB_INTEGRATION.md](EX01_WEB_INTEGRATION.md).
Se conserva perfil2; el borrador3 se sustituye por EX01v2 sin colisión de IDs.
OLED usa framebuffer del catálogo ampliado y revision por acción; estado/pose
provienen del producto19. Fuente adaptada sin nuevo build/ensayo. No se aplican
los estados históricos de productor pendiente a la fuente actual ni se los
confunde con funcionamiento físico comprobado.

