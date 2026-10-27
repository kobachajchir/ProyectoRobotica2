# Puntos cartesianos del brazo y perfiles de modelo — v1

Estado AFR2-20261027-11: implementado en fuente Web. Sin tests, compilacion,
validadores, navegador ni flasheos ejecutados por esta entrega. No acredita
funcionamiento en navegador ni placas. No agrega comandos afr2-ws-1/ROBO2.

## Comportamiento

Brazo y Conjunto comparten una biblioteca de destinos independientes del archivo
visual. Guardar captura el TCP de la postura actual respecto del montaje, su
orientacion y apertura. Se puede renombrar, actualizar, eliminar, importar y
exportar. Actualizar/eliminar y aceptar un marco nuevo tienen confirmacion inline.

El destino principal es cartesiano: al modificar longitudes se conserva XYZ y
orientacion y se resuelven nuevos angulos. Los angulos enseñados son referencia
y semilla compatible, no una orden para el brazo nuevo. Cambiar el GLB sin cambiar
geometria/marcos no transforma los destinos. Un cambio de montaje o TCP requiere
revision explicita del usuario; aceptarlo conserva los numeros del destino.

Solo se recuperan puntos individuales, sin objeto sujeto, barrido, mision ni base
en movimiento. No hay secuencias, movimiento cartesiano rectilineo ni puntos
fijos del mundo. La base queda bloqueada durante la reproduccion. La apertura
se restaura solo al finalizar y si se eligio esa opcion (inicialmente apagada).
Se comprueba tambien su recorrido de apertura con pasos de hasta1 mm. Recuperar
un punto nunca ejecuta automaticamente agarre/liberacion ni comandos fisicos.

## Modelo y fuentes

`RobotModelDefinition` en `Web/src/features/collector/robotModel.ts` es la fuente
de articulaciones, dimensiones, marcos, TCP, mordazas, velocidad, referencias y
proxies. Motor y persistencia no importan nombres de piezas CAD. El registro
inicial contiene PVC32 v03 por decision humana, aunque existe CAD v04 posterior.
No se altera ni promueve ese CAD concurrente.

Fuentes v03: `AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/parametros_prototipo_v03.csv`,
`revision_v03/modelo_v03.json`, README v03 y `check_motion_v03.py` leidos como fuente,
sin ejecutar esos scripts. Base/J1=112 mm y hombro=225 mm sobre el plano de referencia
del chasis; eslabones130/120 mm y TCP78,5 mm. El soporte y electronica se dibujan
con proxies simplificados. La adaptacion Web no es exportacion BRep del ensamble.
La altura suelo/deck94,64 mm y ubicacion32 mm detras del origen se toman del marco
CAD documentado. No equivalen a montaje fisico medido ni comparacion render/CAD.

Motor SI: metros, radianes para calculo; articulaciones en grados y apertura
en mm en UI/registros. Canonico X adelante/Y izquierda/Z arriba. Conversion a
Three por cuaternion del descriptor. La cadena serial se compone como:

`T_montaje_TCP(q) = producto(T_origen_i · R_eje_i(sign_i·q_i + zero_i)) · T_TCP`.

Las matrices, render, limites de los campos y muestreo de alcance previo usan
el mismo descriptor. La rama analitica de recoleccion se habilita solo para la
familia serial Z,Y,Y,Z,Y,Z sin ceros/desplazamientos incompatibles; sus longitudes
se derivan de los origenes. Otros perfiles conservan los puntos y el solver
cartesiano generico, pero requieren un adaptador de recoleccion para esa tarea.
La nueva envolvente3D completa y laminas del pendiente08 no se ejecutan aqui.

## Incorporar otro brazo

1. Crear un descriptor tipado y añadirlo a `ROBOT_MODELS`. La UI muestra selector
   cuando existe mas de un perfil. Definir una cadena serial de articulaciones
   revolutas con IDs estables, parent anterior/null, origen y eje unitario local,
   signo, cero y limites; parametrizar TCP y ambos montajes.
2. Declarar `initial`, `transport`, semillas, velocidad, geometria visual y de
   colision. No cambiar `mountFrame`/`tcpFrame` por un cambio estetico. Modificarlos
   cuando cambie su significado; tambien se comparan las transformaciones.
3. Para el dibujo procedural, parametrizar origenes/radios, base y mordazas.
   Para GLB, usar piezas rigidas separadas; este adaptador no infiere ejes ni
   resuelve un mesh unico o un skin deformable.
4. Para GLB, poner el archivo bajo `Web/public/models/` y definir `visual.url`,
   `restToMount`, `restAngles` y `restOpening`, correspondientes a la postura de
   exportacion. `restToMount` expresa el activo completo en el marco del montaje.
   `cadToRobot` registra conversion de procedencia; no se aplica dos veces.
5. Definir `visualNode` de cada articulacion y `gripper.visualNodes` de las dos
   mordazas. Debe haber un nodo unico por ID. Se admite el nombre original o el
   saneado por GLTFLoader/PropertyBinding. El adaptador conserva transformaciones
   de reposo y aplica `T(q)·T(rest)^-1`; las mordazas se desplazan segun su eje.
6. No activar movimiento hasta que descriptor y enlaces de nodos esten completos.
   Un rig con nodos ausentes/ambiguos conserva la malla estatica y muestra el
   motivo del bloqueo. La carga GLB mantiene bloqueo hasta vincular el rig.

El cambio de modelo detiene movimientos, descarta planes y reinicia los espacios
simulados en la postura inicial del perfil, conservando la biblioteca. Encuadre
y escala de muestreo se derivan de sus dimensiones. La configuracion de Vite
incluye GLB locales registrados en la lista de activos para una compilacion futura.
No se agrego ningun GLB ni dependencia en esta revision.

## Persistencia e intercambio

Clave `afr2-arm-points-v1`, documento `{format:"afr2-arm-points", version:1, points}`.
Cada `SavedArmPoint` contiene:

| Campo | Significado |
| --- | --- |
| id, name, createdAt, updatedAt | Identidad UUID, nombre y fechas ISO |
| position | XYZ en metros respecto del montaje |
| quaternion | XYZW unitario de la orientacion del TCP |
| opening | Apertura de mordazas en mm |
| source.modelId/revision | Procedencia geometrica |
| source.frameSignature | Definicion de marcos/montajes/TCP al guardar o revisar |
| source.jointIds/angles | Identificadores y angulos enseñados, solo referencia/semilla |
| source.simulation | Brazo/Conjunto de origen, informativo |

Carga e importacion revisan formato, version, numeros finitos, cuaternion
unitario, nombres/fechas y unicidad de IDs. No se rechaza un destino solo por
haber cambiado las dimensiones: sus limites actuales se resuelven al calcular.
Importacion atomica, archivo hasta2 MiB, biblioteca hasta1000 puntos; IDs nuevos
para los registros importados, sin sobrescribir existentes. Las aperturas fuera
del rango nuevo no se aplican silenciosamente.

No se guardan seleccion, planes, progreso, timers ni ordenes. La persistencia
se realiza en cambios de biblioteca, sin efecto de escritura al montar/recargar.
Datos dañados se conservan sin reemplazar la clave automaticamente: biblioteca
en memoria, aviso y opcion de resguardar el texto original. Reemplazar esa copia
local requiere accion explicita confirmada. Cuota/almacenamiento no disponible
dejan los puntos en memoria con exportacion disponible.

El almacenamiento pertenece al navegador y al origen (host/puerto/protocolo),
no a una cuenta ni al firmware. Importar/exportar permite pasar de Web local a
ESP u otro navegador. El fallback de IDs usa getRandomValues cuando randomUUID
no esta disponible, por ejemplo en algunos origenes LAN sin contexto seguro.

## Resolucion, plan y reproduccion

`MotionRequest` lleva descriptor completo, postura inicial, destino, apertura,
entorno, ID y huella de contexto. Worker inline de Vite: sin trabajo numerico
pesado en el hilo de interfaz y sin endpoint fisico. Se cancela terminando el
Worker; respuestas antiguas se descartan por generacion y contexto.

IK numerica: Jacobiano por diferencias finitas y minimos cuadrados amortiguados,
semillas deterministas actual/enseñada compatible/inicial/transporte/alternativas.
Hasta240 iteraciones por semilla, incremento limitado a0,12 rad por iteracion.
Aceptacion: error de posicion<=1 mm y de orientacion<=1°. Se distingue cota radial
fuera de alcance, busqueda afectada por limites, falta de solucion numerica y
postura encontrada sin recorrido. Las ultimas dos no prueban imposibilidad global.

`ArmMotionPlan` contiene modelo/revision, destino, huella, inicio, solucion,
etapas, muestras del TCP, apertura inicial/final opcional, duracion y errores.
Busca directo, via Transporte y via semillas del perfil, con muestras a<=3°
de variacion maxima por articulacion. Las alternativas son finitas; no es un
planificador completo de espacio de configuraciones.

Proxies: capsulas de eslabones y torre, exclusiones entre enlaces no adyacentes,
pares permitidos del descriptor, AABB del chasis/deposito/cubos, suelo y muestras
del volumen de la pinza. Cubos en el mundo se transforman al chasis, usando una
envolvente AABB conservadora al girar la base. Estas aproximaciones pueden dar
falsos bloqueos y omitir interferencias entre muestras; no son BRep ni prueba
continua. No hay afirmacion de carga, estabilidad, resistencia o agarre fisico.

Vista previa y reproduccion consumen exactamente las mismas etapas calculadas.
La interpolacion usa el limite del perfil (v03:35°/s) y comprueba la configuracion
durante el avance. Se invalida el plan al cambiar modelo, inicio, apertura,
destino, marcos, obstaculos, entorno, base o epoca de interrupcion. La huella no
depende del reloj, lecturas de sensores o seleccion de la escena.

Cancelar, STOP, desenfoque, documento oculto, cambio de simulacion/ruta o apertura
de overlay limpian la ejecucion. La postura alcanzada se conserva, sin reanudacion
automatica. Hay cancelacion junto al visor aun con la tarjeta plegada o cambiando
a otra tarea del conjunto. Seleccionar un marcador abre los controles del brazo.

## Aceptacion pendiente

Solo ejecutar cuando el usuario lo pida: guardar/recargar/exportar/importar;
modificar longitudes y resolver el mismo destino; cambiar solo el activo visual;
probar nodos renombrados y rig incompleto; destino fuera de alcance/no convergente;
recorrido bloqueado; cambios de contexto durante Worker; cancelar/STOP/foco;
restauracion de apertura; errores de almacenamiento; ambas simulaciones, temas,
teclado, tamaños y movimiento reducido. No se ejecutaron en esta entrega.
