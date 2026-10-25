# Control táctil y ruedas simuladas — AFR2-20261025-11

25/10/2026. Actualización11 agrega giro local con dos dedos, cámara10/chevrones09 conservados; conducta04 conservada. Actualización04 elimina una falsa indisponibilidad3D y desacopla la entrada local del visor. Captura/foco/feedback03 conservados. Extiende Control sin modificar WS/EX01v7, firmware0.9.0,
los30comandos ni las59identidades OLED. Reemplaza la indisponibilidad sin robot
y el encuadre oblicuo de13. No introduce un permiso de control.

## Sesión táctil

Movimiento → Control táctil abre el dialog existente. Con robot disponible
al abrir, sigue enviando robot.drive mediante el flujo directo existente;
sin robot/capacidad, abre Simulación local. La aplicación debe estar cargada/accesible; el GLB es necesario para ver
el auto, pero su carga no bloquea input local. No agrega instalación PWA
ni caché offline del sitio.

El modo se fija al abrir; entrar en giro con dos dedos o Q/E/0 lo convierte
a local después de cancelar la consigna anterior. Una sesión local sigue local al reconectar; para
usar el robot hay que cerrar y abrir cuando esté disponible. Si una sesión
real pierde disponibilidad o la consigna falla, cancela el gesto y pasa a
local. Requiere un gesto nuevo. Nunca reanuda una orden automáticamente.
Las callbacks de dirección y liberación físicas solo se invocan cuando
corresponde al modo real; un gesto local no manda ni cero ni movimiento.
La supervisión de conexión existente es independiente del gesto simulado.

La simulación local se habilita al abrir, sin conexión, capacidad de placa ni
señal de disponibilidad del modelo. Tocar/clic no mueve, arrastre12px elige una
de8direcciones y volver a8px libera; histéresis30°. Soltar, cancelar,
perder captura/foco/visibilidad, salir del área, resize o STOP cancelan.
Solo el envío físico requiere modelo listo y cancela al perder esa condición.
Si el visor falla en una sesión física se cancela la orden y pasa a local;
una sesión ya local conserva su gesto ante carga/remontaje/fallo del visor.
La dirección inferior (Simulando hacia… / Simulación detenida) no se reemplaza
por el estado del visor. Un fallo real conserva aviso visual y Reintentar.

Corrección04: R3F monta Canvas.fallback como hijos del canvas nativo incluso
si WebGL funciona. El fallback anterior contenía Unavailable con un efecto
onFailure y marcaba modelFailed=true al montar, bloqueando toda la entrada.
Ahora ese contenido es texto pasivo; solo RenderBoundary monta el componente
que notifica un fallo real. El cambio no reemplaza el modelo ni sus recursos.

La captura vive en una superficie DOM transparente sobre el modelo; Canvas
tiene pointerEvents:none explícito, en lugar de depender de herencia CSS.
El puntero se captura y pointerup/cancel se observan también en window. El
estado buttons se exige solo a mouse, sin rechazar un touch activo por él.
Dos contactos touch dentro del área entran en giro local11. Un segundo
puntero fuera del área/de otro tipo, o un tercero, cancelan; hay que soltar
los contactos restantes antes de otro gesto. No hay fallback a un dedo. pointerout con
relatedTarget nulo solo cancela en un borde real de ventana: un cambio interno
de destino de eventos no cancela por sí solo.

El dialog enfoca inicialmente la superficie de conducción (initialFocusRef
optativo del modal, conserva el cierre por defecto en otros usos). Flechas
combinables se capturan antes de navegación/scroll dentro del modal abierto;
no dependen de que el foco siga en el modelo. Se eliminan listeners al cerrar.
El deslizador y otros campos conservan flechas nativas. Tab mueve foco y
cancela el gesto; Escape cierra. Mantener una tecla no repite órdenes.
La actividad local participa del STOP contextual existente.

Texto inferior informa el movimiento y Simulación detenida en reposo local. La velocidad
simulada10..800mm/s, inicial400, es independiente de los límites y la velocidad
físicos. Diagonales reparten el límite por suma de los ejes, igual que el
control existente. En real conserva heartbeat180ms/TTL400ms y límites de placa.

## Giro con dos dedos — AFR2-20261025-11

Twist de dos PointerEvents touch, sin zoom por pinch ni giro físico. Separación
mínima40px y umbral3°. Orden de contactos estable; delta de línea normalizado
por atan2(sinΔ,cosΔ), con signo opuesto al reloj de pantalla. Orientación
acumulada sin salto en±180°; grupo del modelo rota sobre su Y central, no la
cámara. RAF agrupa entradas, sin reloj de movimiento adicional o inercia.
La pose se conserva al soltar y vuelve a0 al abrir otra sesión. El rumbo
normalizado0..360° se lee en la chapa de Simulación local.

Entrar cancela la consigna previa y fija preview aun conectado; solo se llama
la liberación física existente si había target real. Ningún incremento angular
se envía a robot.drive ni a otro comando. Se exige cerrar/reabrir para volver
a una sesión física disponible. La traslación posterior es relativa a pantalla;
se aplica la inversa del rumbo a la velocidad del cuerpo antes de Mecanum.

Soltar/cancelar/perder captura de cualquier dedo, salir del área, blur/hidden,
resize/STOP, tercer contacto o mezcla de fuentes cancela y espera a que terminen
los demás contactos. No convierte al restante en traslación. Se libera captura
y reinicia el ciclo ante un nuevo contacto primario o nueva apertura.

Q/E giran localmente±15° por nueva pulsación;0 centra. Auto-repeat no gira.
Campos y modificadores conservan teclas nativas. No se modifica la cámara60%
o la geometría/color de los chevrones09. No se promete twist de trackpad nativo
sin dos eventos touch. Área propia touch-action:none, zoom de página fuera intacto.

WheelAngleOffsets opcional suma diferencias de offsets al rig de ruedas;
no reinicia su fase al pasar de traslación a giro. Paraθ, FL/RL=-θL/R y
FR/RR=+θL/R, con L300mm/R40mm ilustrativos. Malla/calibración/controlador
intactos, sin feedback real. La velocidad del giro la fija el gesto, no la
barra de traslación. Reduced motion conserva la pose manipulada directamente
y congela las ruedas secundarias. Rumbo aria-live=off, estado anuncia sentido.
Pruebas táctiles/captura/fases/lectores/ambos temas pendientes; solo compilado.

## Encuadre e identidad

Vista superior ortográfica real: cámara sobre+Y y up=-Z, frente del GLB hacia
arriba. Centrado/zoom por dimensiones reales; no escala la malla ni altera
su archivo. Franja48px separa el indicador de modo de modelo/flechas.
Vigente09: por preferencia humana se restaura el diseño07 de los chevrones.
Geometría360×240 y tres cintas anchas, longitud49% del menor lado local
(máximo480px), altura50% (máximo280px), gradiente longitudinal y máscara
transversal de07, sin campos por brazo, V de stroke o desplazamiento22%08.
El único ajuste visual añadido sobre07 es opacidad0,58 en el SVG completo.
Se conserva el degradé y la forma previos; transparencia suaviza el acento.
Cámara60%10 (≈15,4% más tamaño lineal que52%), capas bajo chasis06, fade220ms y ciclo1200ms/±12unidadesSVG
con fases/pause anteriores. Teclado/reduced motion estáticos. Componente08
retirado del runtime. Entrada local04/foco/captura03 y físico intactos.
Inspección de hashes confirma Overlay idéntico a07 y CSS idéntico salvo
opacidad/comentario de registro. No nueva dependencia ni activo. Visual pendiente.
Captura y touch-action solo en el área táctil; safe areas y dialog conservados.

## Giro de ruedas

El GLB final tiene cuatro nodos AFR2_Rueda80_ManoA/B y cuatro GM25 separados.
Se clona por instancia y se crean pivotes sobre el eje X de cada motor.
Attach preserva las transformaciones originales: el origen desplazado del
CAD no se usa como eje. Clasificación por posición: frente=-Z, izquierda=-X.
No se toca geometría/materiales cacheados ni el GLB/CAD. Apoyo/escala del
modelo estático siguen basados en sus vértices, no se recalculan por frame.

Radio nominal visual40mm (Rueda80), ángulo=recorrido de rueda/radio. No sustituye
el diámetro/Hall calibrado del firmware. Las ruedas giran sobre su eje fijo
Mecanum; no se orientan como dirección de automóvil. Rodillos no articulados
individualmente, sin contacto/dinámica/derrape: animación ideal aproximada.

- Táctil: cinemática inversa desde la dirección/velocidad; useFrame actualiza
  pivotes sin estado React por frame. Canvas demand invalida solo mientras
  gira. Primera imagen no integra tiempo de inactividad; delta acotado50ms.
  El chasis queda centrado, sin inventar pose física.
- Secuencias: recorrido por rueda integrado con las mismas velocidades y
  pasos10ms del simulador. Ángulos absolutos desde cursor, también rampas,
  pivotes y pistas independientes. Pausa/scrub/reinicio/repetición no acumulan
  un segundo reloj ni consumen telemetría del robot.
- GOTO: rumbo constante0 permite recorrido analítico FL/RR=X−Y, FR/RL=X+Y,
  en mm; ángulos ligados al cursor. La lista sigue siendo solo local.

prefers-reduced-motion congela el giro de ruedas y elimina las transiciones
y la oscilación de flechas; dirección/texto, controles y trayectoria calculada siguen disponibles.
Los visores de inspección sin entrada de ruedas permanecen estáticos.

## Estado por módulo

Web: fuente TypeScript/Vite compilada y paquete esp promovido localmente.
ESP: solo data/dist; firmware0.9.0/build01 sin nueva compilación.
ATmega: sin cambio, firmware0.9.0/build01 y gates Hall/memoria0.
Sin pruebas de protocolo, navegador/teléfono, flasheo ni prueba física. Lectura
de nombres/transforms y compilación no acreditan una animación visual aceptada.
[Ficha](../changes/2026-10-25-11-two-finger-local-rotation.md),
[build](../documentation/builds/2026-10-25-11/README.md).
