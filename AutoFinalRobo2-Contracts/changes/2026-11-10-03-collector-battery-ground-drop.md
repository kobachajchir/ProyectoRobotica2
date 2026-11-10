# AFR2-20261110-03 — Recolección de baterías y entrega en drop circular

> **Corrección posterior del usuario:** las cargas son **cajas**, no baterías. Se retiró BatteryCargo y la extracción del portapilas/celdas; la escena usa `AuxiliaryModelAsset` con el GLB real `CUBE_MODEL` de 50 mm, coherente con las envolventes y apertura de agarre. Etiquetas C1/C2 y textos de cajas en toda la interfaz del simulador. El ciclo, drop, contador y navegación se conservan. La descripción de baterías más abajo queda como antecedente sustituido; versión 1.15.0 conservada como corrección antes de cargar. Sin tests, compilaciones, validaciones ni navegador.

Versión: **1.15.0 (subversión)**.

## Comportamiento implementado en fuente

### Aclaración posterior: avanzar y volver a escanear

El inicio automático ahora entra en exploración móvil con medición frontal cada 150 ms. Usa puntos de patrulla independientes de las posiciones ocultas de cajas y rutas libres; al terminar cada desplazamiento realiza un barrido detenido. Si el barrido completo por sectores no detecta, se traslada al siguiente punto y vuelve a barrer. Durante el desplazamiento una detección frontal interrumpe la patrulla y pasa a alineación/toma. La patrulla está acotada a 16 puntos y se detiene con explicación si no obtiene nuevas detecciones.

Durante aproximación mantiene el sensor a 0° y muestra Distancia frontal en mm. El umbral de parada se calcula desde el estacionamiento elegido, el origen real del rayo virtual (103 mm) y la cara frontal del cubo (25 mm respecto al centro). Solo detiene para tomar si coincide el objetivo, queda frontalmente centrado y existen las soluciones de agarre/elevación; después espera otro eco detenido. Otra caja muy próxima delante bloquea la toma. Tras cada entrega vuelve a explorar desplazándose. Versión 1.15.0 conservada como ajuste de la misma misión antes de cargar, sin tests, compilaciones, validaciones ni navegador.

Simulador conjunto: buscar carga → alinear el frente → aproximar el auto → volver a detectar y confirmar alcance → tomar por arriba → elevar → recoger el brazo para transporte → navegar al drop → colocar sobre el círculo → bajar → soltar y contabilizar → recoger brazo → buscar otra carga. El modo automático continúa hasta entregar todas las cargas de la escena. La acción sobre una carga seleccionada conserva el modo de una sola entrega.

La antigua canasta de cuatro celdas, montada en el auto, se reemplaza por una zona fija de radio 240 mm. Su posición depende de la semilla, detrás del origen y fuera de la distribución inicial de cargas. Aparece al iniciar una misión o entrega manual: círculo de suelo, pared cilíndrica translúcida emisiva de 160 mm, anillos y luz local. La etiqueta muestra DROP y cajas entregadas. No hay límite de cuatro entregas.

El contador aumenta únicamente al abrir la pinza con una carga sujeta cuyo centro está dentro del radio útil del drop (descontando media diagonal de la envolvente) y a 15–65 mm de altura. No basta con entrar con el auto. Una carga entregada queda registrada en coordenadas del mundo y desaparece visualmente tras 0,6 s; el drop consume cargas y no representa un apilado físico. No sigue al auto ni vuelve a participar en detección o agarre. Regenerar reinicia escena y contador.

## Detección, navegación y toma

- Detector **virtual**: conserva el rayo horizontal contra envolventes, pero entrega identidad y centro exacto del objeto detectado. Esto no representa clasificación ni estimación de centro de baterías mediante un HC-SR04 físico. Las lecturas se renuevan durante aproximación y al detenerse; una detección final diferente bloquea la toma. Si no encuentra nada, gira por sectores hasta cubrir el entorno; después se detiene con explicación.
- La distancia frontal de estacionamiento se selecciona con las soluciones de toma del modelo activo, en lugar de fijar 235 mm para cualquier brazo. La base se alinea primero y navega con esa orientación usando movimiento Mecanum.
- `collectorNavigation.ts`: ruta directa si queda libre; de lo contrario A* acotado, grilla de 100 mm, ocho vecinos y segmentos comprobados cada 15 mm contra la envolvente de base y cargas del mapa virtual. No se atraviesan esquinas por diagonales. Se simplifica la ruta mediante segmentos libres. Sin ruta, detección válida o solución de brazo, el ciclo se detiene; no teletransporta la carga ni omite restricciones.
- Tolerancia final de aproximación 3 mm y de giro 0,015 rad, ajustando únicamente ese residuo hasta el destino comprobado. Timeout de navegación 90 s. STOP, pérdida de foco y los bloqueos existentes siguen cancelando el movimiento local.
- `armKinematics.ts`: altura previa a transporte 180 mm en la rama descendente, compartida por la comprobación de alcance y la ejecución; sustituye 215 mm, incompatible con conservar el radio de toma frontal de suelo del V17. Límites articulares admiten solo residuo numérico de 1e-7 grados antes de ajustar al límite, relevante en J1=90° por el montaje a un cuarto de vuelta. No amplía límites mecánicos ni modifica el descriptor CAD.

## Representación y alcance

`CollectionDropZone.tsx` reutiliza portabaterías y celdas del GLB real del auto. Se normalizan a una envolvente virtual de 50 mm para esta misión; no es una afirmación sobre el tamaño físico del pack. Fallback al cubo existente si no hay componentes de batería. Geometrías y materiales compartidos, sin GLB, imágenes, bibliotecas ni descargas nuevas.

`CollectorScene.tsx` retira la canasta, incorpora el drop en coordenadas del mundo, marca el objetivo y encuadra inicialmente suelo, robot, cargas y zona de entrega. Las vistas independientes del auto y brazo conservan su encuadre. `CollectorPage.tsx` actualiza acciones, instrucciones, etiquetas de baterías y contador de cajas entregadas.

Estado: Web=fuente implementada; ATmega/ESP=solo versión compartida; CAD/Loader=sin cambios. Sin tests, compilaciones, validaciones, navegador, renders ni flasheos por instrucción del usuario. No se declara misión ejecutada, colisiones físicas comprobadas ni capacidad de levantar baterías reales. El paquete 1.13.0 compilado anteriormente no incluye este ciclo.
