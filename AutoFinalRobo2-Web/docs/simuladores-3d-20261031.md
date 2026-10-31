# Simuladores3D — AFR2-20261031-06

Corrección07: la posición del eje del SG90 se convierte de BlenderZ-up a
glTFY-up antes de aplicarla al nodo CapaSuperior. El marco fijo conserva
posición/orientación del eje; un hijo gira únicamente alrededor de suY local.
La versión06 mezclaba esas convenciones y desplazaba el centro de giro.
Corrección de fuente, sin nueva exportación ni comprobación en navegador.

Se reorganizan Auto, Brazo y Conjunto con la identidad0.1.9 existente y las
skills Emil, Apple Design y Visual Identity System. La escena es el elemento
principal; los detalles técnicos se abren cuando hacen falta.

| Before | After | Why |
|---|---|---|
| Matemática abierta ocupa el costado de entrada | Botón Matemática abre/cierra un panel junto a la escena | Dar espacio al modelo sin perder el análisis en vivo |
| Fondo y retícula comparten el mismo color base | Cielo celeste y plano crema, con retícula de100mm | Distinguir brazo blanco, auto y piezas |
| Controles y datos con jerarquías similares | Encabezado contextual, estado, cámara, escena, lecturas y controles por tarea | Separar observar, mover y editar |
| Motores individuales y puntos abiertos al entrar | Opciones avanzadas y biblioteca plegadas inicialmente | Mantener las acciones frecuentes accesibles |
| Leyenda verde/ocre aplicada a cubos GLB dorados | Leyenda de pieza50mm y zona de toma | Corresponder con los materiales reales del modelo |

## Montaje y servo

La conversión nativa es `arm=(carX,-carZ-32mm,carY-26mm)`. El GLB del auto
agrega53.051345mm de apoyo al piso; `useRobotModel` luego centra sus límites,
incluyendo la placa OLED. No se puede copiar directamente el origen del brazo.
La montura anterior `[-14,0,94.64]mm` se sustituye por
`[27.819,0,79.041]mm` en coordenadas del robot. La rotaciónZ de−90° se conserva.
El marco CAD original y el marco recortado del GLB se documentan por separado.
El generador del descriptor incorpora la misma corrección.

El SG90 usa la línea cilíndrica del eje de salida del CAD:
`X=-5.15002937mm,Y=42.999972322mm` en el plano del brazo. Se expresa respecto
de CapaSuperior y se transforma con su matriz; no depende de los límites del
servo ni del centro del sensor. El helper compartido también admite los nombres
normalizados por GLTFLoader. Control y Simulación emplean el mismo pivote.

Blender separa tres cuerpos móviles del SG90 —salida, horn y tornillo central—
en el nodo `AFR2_SG90_Salida`. El helper lo articula con el HC-SR04 y su soporte;
la carcasa permanece fija. Se preservan los materiales, sin nueva decimación.
El auto se recomprime con los parámetros Draco previos:880232bytes.
El brazo permanece en780972bytes y el cubo en1784bytes.

La firma existente de los puntos guardados incluye el montaje: los puntos
capturados con el marco anterior pedirán revisar su referencia antes de usarlos.
No se borran bibliotecas. El rayo/altura del sensor continúan siendo la hipótesis
virtual configurable del simulador, no una calibración física del HC-SR04.

## Entrega

Fuentes React/CSS, descriptor, catálogo y auto GLB depublic actualizados.
El nativo del brazo no se modifica. La fuente editable del auto corregido está
enCAD/mecanica/auto-completo/servo_web_20261031/AutoFinalRobo2_Servo_CAD.blend;
su script y procedencia están junto a ella. Original25/10 conservado.

Sin tests, compilación, validadores, navegador ni flasheo por instrucción del
usuario. Apariencia, animación y montaje implementados, **no comprobados en
ejecución**. Dist/artifacts/ESP conservan la entrega05, incluidos sus modelos;
se actualizarán cuando se solicite compilar/promover esta revisión.
