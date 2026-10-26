# Inspección interactiva del modelo — AFR2-20261026-01

Actualización: 26/10/2026; base AFR2-20261025-15. Implementación en fuente Web; sin compilación, tests,
validaciones, navegador, promoción LittleFS ni flasheo en esta entrega.
ATmega/ESP no cambian. Este documento no modifica ROBO2 ni EX01.

## Puntos y ficha

El visor `#/modelo-3d` ofrece ocho selectores: unión del servo y soporte,
HC-SR04, una rueda representativa FL, Mega2560, ESP/shield, OLED, encoder de interfaz y
arreglo inferior de cinco IR. Cada selector abre una única ficha no modal,
contenida en el visor, con identificación, explicación, datos y acceso a su tarea.

El punto pequeño representa una referencia tridimensional del ensamble, que se
proyecta con la cámara al orbitar, desplazar o acercar. Los selectores numerados
conservan un área táctil de 44px; cuando se aproximan, se separan en pantalla
mediante líneas que mantienen visible el punto de referencia. Las referencias
se muestran sobre el modelo, incluso para piezas tapadas por otras piezas;
no se determina visibilidad por oclusión. Fuera del encuadre se oculta el selector.
Un desplegable permite elegir las mismas piezas con teclado o sin localizar su
punto en pantalla.

El botón a la derecha del selector alterna entre **Ocultar referencias** y
**Mostrar referencias**. Inicialmente se muestran. Ocultarlas retira puntos,
líneas y fichas explicativas, limpia la selección y desactiva el desplegable.
Al mostrarlas se recuperan los puntos sin reabrir una ficha anterior ni cambiar
la cámara. No se proyectan los puntos mientras están ocultos. El botón comunica
el estado con `aria-pressed`. La única rueda señalada es FL; su ficha explica
el diseño común a las cuatro ruedas, que conservan sus controles individuales.

La ficha se mantiene arriba a la derecha en escritorio y abajo en móvil,
independiente de cámara y punto. Puede cambiarse de componente sin cerrarla.
La X, Escape o un clic fuera la cierran. Un arrastre de más de 6px, un gesto
multitáctil o zoom no se interpreta como clic exterior: orbitar con la ficha
abierta la conserva. La X/Escape devuelven el foco al selector de apertura.
El contenido de la ficha tiene scroll propio, sin bloquear la cámara exterior.

## Accesos

| Componente | Destino interno | Resultado al entrar |
|---|---|---|
| SG90 / HC-SR04 | `/sensores?seccion=distancia` | Tarea Distancia y servo |
| Línea inferior | `/sensores?seccion=linea` | Tarea Línea IR |
| Ruedas Mecanum (referencia FL) | `/control?tarea=wheel&rueda=FL` | Prueba de rueda FL; otras ruedas elegibles desde el control |
| ATmega | `/control` | Control general del robot |
| ESP | `/wifi` | Configuración Wi-Fi |
| OLED / KY-040 | `/oled` | Pantalla y controles del encoder |

`Sensores` conserva también `seccion=ruedas`. Los valores de sección desconocidos
abren Distancia; los códigos de rueda desconocidos abren FL. Control interpreta
los parámetros de entrada al montar la página. Navegar no ejecuta movimientos,
pruebas de rueda, órdenes de servo ni reinicios; esas acciones siguen en sus
controles existentes y con su disponibilidad real.

## Procedencia y precisión

La geometría se conserva en `/models/AutoFinalRobo2.glb`. Los puntos se calculan
en la escena original con el mismo desplazamiento de suelo/centrado de
`useRobotModel`. La búsqueda aplica la misma normalización de nombres CAD que
`GLTFLoader` (`PropertyBinding.sanitizeNodeName`), incluida la eliminación de
dos puntos y puntos del nombre exportado. El visor mantiene `animateServo=false`; no se cambian las
instancias animadas de Control/radar.

Servo: extremo superior del SG90 con el eje visual del soporte ya usado por el
rig. Ultrasonido/línea: frente de sus límites de geometría. Rueda: cara exterior
de FL, sin referencias para FR/RL/RR. Mega: borde exterior de la placa. ESP: referencia
local inferida del módulo sobre el shield. OLED y encoder: referencias locales
inferidas de la cara de pantalla y perilla del nodo exportado conjunto
`AFR2_AFR2_UI_B2_Fondo_OLED15_Auto5:1`. No hay nodos independientes para estas
dos piezas en el GLB actual. Son referencias visuales del activo, sin acreditación
metrológica ni de montaje físico. Si cambia el exportado o sus nombres, se deben
adaptar estas referencias; una pieza sin referencia se omite del selector.

Las fichas reutilizan datos del proyecto: servo SG90 y recorrido del programa
0–180° (sin afirmar recorrido mecánico calibrado), ultrasonido HC-SR04, ruedas
Mecanum80/GM25-370, Mega2560, ESP-12S/ESP8266 sobre shield OSOYOO, OLED0,96″
128×64, KY-040 sin breakout y cinco IR. SW corto confirma, largo800ms abre menú,
Volver por opción; no cambia la semántica OLED ni requiere nuevas pantallas AVR.

Identidad: manual0.1.9 autorizado, VIS-036; material popover neutro, acento propio,
iconos existentes, superficies y tipografía con tokens, alternativa de teclado,
sin animar el seguimiento geométrico y con movimiento/transparencia reducidos.
Composición y referencias nuevas Propuestas; comportamiento solicitado Confirmado;
aspecto e interacción en ejecución No verificados.
