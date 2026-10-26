# AFR2-20261026-02 — Composición del radar Web

26/10/2026. Pedido humano: rehacer la pantalla del radar por su falta de
coherencia estética con la aplicación, aplicando Emil y las skills visuales.
APLICAR sobre manual0.1.9 ya autorizado. No se crea una identidad nueva.

## Diseño aplicado en fuente

| Before | After | Why |
|---|---|---|
| Pedestales semicirculares sólidos y contorno elevado | Suelo continuo con retícula de desvanecimiento radial, anillos tubulares y sector rubí | Integrar auto y campo de medición |
| Encuadre genérico inicial de4m | Escala inicial1m y encuadre por límites proyectados; Perspectiva/Superior | Dar protagonismo al auto sin agrandar la malla ni alterar metros |
| Todas las lecturas con el mismo peso | Distancia principal, ángulo aplicado y posiciones con eco; PWM/edad en Disclosure | Jerarquía de lectura y menor densidad |
| Ajustes en una columna lateral larga | Parámetros debajo del mapa en una cuadrícula adaptable | Conservar espacio para la escena y acortar la columna de operación |
| Marcas pequeñas y ecos como sólidos facetados | Etiquetas métricas mayores, marcas angulares agrupadas y ecos redondos con halo | Mejorar lectura y coherencia del trazo |

El mapa usa los tokens del mapa Control/GOTO, fondo neutro y acento rubí. La
retícula tiene antialias por derivadas y se desvanece mediante un shader fijo,
sin reloj ni animación ornamental. No requiere paquetes de postprocesamiento.
Las marcas angulares comparten una geometría. Se liberan geometrías/texturas;
Three.js sigue renderizando bajo demanda. Solo la aguja/servo conservan la
interpolación funcional; cambiar cámara/alcance es inmediato, también por teclado.

La escala inicial1m es exclusivamente de visualización. Siguen disponibles
0.5/1/2/4m; no cambia alcance físico, cadencia ni configuración del HC-SR04.
Los ecos fuera del alcance visible se cuentan en el pie. Las posiciones/escala
siguen siendo físicas: no se normaliza el tamaño del auto. El encuadre reserva
un volumen nominal para el auto; no constituye una comprobación CAD de medidas.
La sombra de contacto es gráfica. La franja de la aguja indica dirección,
no certifica un ancho de haz acústico. La altura de los ecos también es gráfica.

## Función conservada

Se mantienen consultas, cadencias, edades, histórico/snapshot/delta, controles
manuales y coalescencia, radar autónomo, STOP y bloqueos del firmware. No hay
ecos ficticios ni barrido decorativo. Sin conexión la distancia muestra «—»
y la escena indica que espera al robot; un mapa existente se identifica histórico.
Distancia sigue en mm; ángulo aplicado y solicitado permanecen diferenciados.

El selector doble0–180°, sus pasos5° y las barras/campos numéricos conservan
Enter/blur/Escape, límites y Aplicar. El sector de preparación sigue identificado
como vista previa. Modelo3D y su modificación01 no se editan en esta entrega.

## Estado real y alcance

| Módulo | Estado de esta revisión |
|---|---|
| ATmega | Sin cambios de fuente, comandos ni OLED; compilación14 anterior conservada |
| ESP | Sin cambios de firmware ni actualización de activos servidos |
| Web | Fuente02 integrada; sin compilación, ejecución, navegador ni tests |
| Contracts | Registro, contrato de presentación y cobertura VIS-038 actualizados |
| CAD | Sin intervención; fuentes y cambios locales conservados |

Archivos: `Web/src/features/sensors/RadarScene.tsx`, `SensorsPage.tsx`,
`Web/src/styles/radar.css`, `sensors.css` e importación en `index.css`.
No cambian dependencias, versiones de wire, biblioteca OLED ni modelo GLB.

Confirmada: necesidad de rediseño e identidad propia. Observada: tokens,
composición Control/GOTO y primitivas. Propuesta: nueva escena/jerarquía/encuadre.
Verificación: **No verificada**. No se ejecutaron tests, builds, validadores,
navegador, mediciones de rendimiento, promociones LittleFS ni flasheos, según
la restricción humana. La compilación14 no acredita esta nueva presentación.
Los activos que sirve el ESP siguen siendo anteriores; esta entrega es fuente.

Pendiente cuando se autorice: observación escritorio/móvil y temas claro/oscuro,
foco/táctil, reduced motion/transparency, carga/error WebGL, lectura real e
histórico. La estética no se presenta como aprobada por el usuario ni renderizada.
