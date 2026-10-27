# Robot recolector — AFR2-20261027-01

> **Vigente27-04:** Simulaciones concentra Auto, Brazo y Conjunto en
> `/simulaciones?tipo=auto|brazo|conjunta`. Las antiguas /brazo y /simulacion
> redirigen a su pill. Controles derechos, articulaciones arrastrables, gripper
> independiente y matrices homogeneas provisionales en vivo. Control sigue
> separado. Modelos3D conserva referencias y prepara selector Auto/Brazo sin
> fabricar el archivo final ni sus anclas. Referencias de burbuja recibidas el 27/10; la ejecución definitiva espera diseño listo y aviso del usuario. [Pendiente08](pendientes/espacio-trabajo-brazo/README.md).
> [Ficha04](../changes/2026-10-27-04-simulations-models-interface.md). Sin pruebas,
> compilacion o validaciones; sustituye la organizacion02 inferior.

> **Vigente27-02:** Control vuelve a su estado anterior, con control tactil intacto.
> Solo se agregan Brazo robotico y Simulacion conjunta. Controles a la derecha,
> articulaciones seleccionables, gripper y cubos editables/tomables por alcance.
> [Correccion y limites](../changes/2026-10-27-02-preserve-control-arm-combined.md).
> Sin tests/build/validaciones/navegador. Las rutas/disposicion01 inferiores quedan
> sustituidas por la seccion de uso actualizada; geometria propuesta conservada.

27/10/2026. Pedido humano: integrar conceptualmente brazo y deposito sobre el auto,
con simulador 3D y paneles separados Robot / Brazo robotico. El pedido posterior
autoriza adelantar la interfaz del simulador. No sustituye las etapas fisicas ni
el informe de cinematica que se esta preparando en otro trabajo.

## Estado y orden de trabajo

| Etapa | Entrega actual | Pendiente |
| --- | --- | --- |
| HC-SR04 | Procedimiento manual inferior; acceso Web existente | Comprobar sensor real, altura y deteccion del cubo |
| Servo / barrido | Procedimiento manual; acceso Web existente | Relevar centro, sentido, extremos y estabilizacion reales |
| Dibujos / planos | [Esquema editable nominal](../../AutoFinalRobo2-CAD/mecanica/robot-recolector/propuesta_montaje.svg), superior y lateral | Planos de montaje y fabricacion con medidas reales |
| Ensamble CAD | Distribucion propuesta del simulador y dibujo | Ensamble nativo Fusion, soporte, fijaciones, estabilidad y alimentacion |
| Matematica conjunta | Cadena nominal v02, rama inversa vertical y Mecanum en fuente Web | Incorporar el informe en curso; resolver carga y restricciones completas |
| Simulador | Fuente 3D, controles, radar ideal, alcance muestreado y mision local | Compilacion, navegador, aceptacion y ajuste con resultados anteriores |

No se ejecutaron tests, compilaciones, validadores, navegador, puertos serie,
flasheos ni pruebas fisicas. Ningun resultado del CAD v02 anterior acredita este
montaje nuevo. No se modifica Documentacion/ ni AutoMicro.

## Uso de la Web

- `#/control`: Control original, con su navegacion, control tactil y tareas intactos.
- `#/brazo`: brazo individual con seis ejes seleccionables por clic, controles a
  la derecha y apertura/cierre del gripper; sin auto ni cubos en esta vista.
- `#/simulacion`: conjunto robot/brazo/deposito, ruedas simuladas y posiciones de
  cubos editables por X/Y, spawn por semilla y colocacion con clic en suelo.
  Seleccionar auto y activar Mover auto permite arrastrarlo. Los cubos verdes
  tienen soluciones nominales de toma; al elegir Recoger se planifica y anima
  la toma/deposito, o se acerca el robot si el objeto esta fuera de alcance.
  Ambos espacios nuevos conservan estados separados y se detienen al cambiar ruta.
- Mostrar/Ocultar referencias siempre reversible; seleccion por clic o lista.
  Perspectiva, Superior, Lateral y Centrar. Alcance como nube calculada de tomas
  verticales: no se representa una esfera de radio maximo como zona util.
- Sensores reales siguen en `#/sensores`. No se conecta el brazo fisico: el nuevo
  control de articulaciones actua sobre el modelo3D local.
- La escena vive en memoria; recargar comienza detenida. Cambiar ruta, abrir un
  modal, perder foco o emitir STOP detiene el simulador. Detener simulacion solo
  detiene el modelo local; el STOP fisico conserva su servicio independiente.

## Procedimiento para retomar HC-SR04 y servo

Estas son acciones futuras, **no evidencia de pruebas ejecutadas**.

1. Auto detenido: abrir Sensores → Distancia y servo y registrar si la lectura es
   valida, su antiguedad y distancia frente a una superficie a distancia conocida.
   Repetir en varias posiciones y sin blanco. Registrar sin eco como invalido.
2. Colocar el cubo de 50 mm sobre el suelo, manteniendo el montaje real del sensor.
   Registrar altura del emisor, distancia, orientacion y si el haz lo intercepta.
   La visualizacion del radar no demuestra que se haya detectado el cubo.
3. Con el sensor resuelto, accionar el servo por posiciones con traccion detenida.
   Registrar centro, sentido, recorrido util y tiempo de llegada. No inferir
   posicion fisica a partir del PWM aplicado.
4. Barrer con el auto detenido y registrar cada eco con angulo y antiguedad.
   Antes de conducir: centrar, estabilizar y recibir medicion frontal reciente,
   segun SENSORS_RADAR_8.md. No convertir ecos laterales en permiso frontal.

Ficha a completar por observacion: fecha, montaje, distancias de referencia,
lecturas/errores, visibilidad del cubo, centro/extremos del servo y conclusiones.
Los margenes operativos se definiran con esas observaciones, no con valores
inventados a partir del simulador.

## Geometria y matematica implementadas

Convencion de calculo: X adelante, Y izquierda, Z arriba; metros y radianes para
pose, grados articulares y milimetros de apertura. Three: `(X,Y,Z) → (-Y,Z,-X)`.
Ruedas: FL/FR/RL/RR, radio visual 40 mm, `Lx+Ly=220 mm` nominal propuesto. Se
reutilizan cinematica directa/inversa Mecanum existentes, sin calibrar hardware.

`T_mundo_pinza = T_mundo_chasis · T_chasis_montaje · T_J1 · … · T_J6 · T_TCP`.
El sensor y el deposito son ramas del chasis. Los ecos guardan el punto del
mundo calculado con la pose de adquisicion; el objetivo se transforma al brazo.

Cadena adaptada del v02: base 72 mm + torre 113 mm = hombro 185 mm sobre su
montaje; eslabones 130 / 120 mm; separaciones J4–J5 55, J5–J6 65, J6–TCP 78,5 mm.
J1/J4/J6 rotan sobre Z local; J2/J3/J5 sobre Y local. Limites:
J1/J3/J4/J5/J6 ±90°, J2 -10…125°. La pinza cumple
`apertura_mm = 4,00708355144542 + 24 · angulo_servo_rad`.

Para toma vertical se fijan J4=0 y la direccion final hacia abajo. La inversa
resuelve dos eslabones de 130 y 175 mm hasta J5 y herramienta de 143,5 mm.
Rechaza soluciones fuera de limite. J6 alinea las mordazas con el cubo cuadrado
conocido. Es una rama de IK especifica de esta tarea, no un solver general de
seis ejes ni una transcripcion del informe aun no disponible.

El alcance se muestrea en grilla de 25 mm, separando soluciones articulares y
exclusiones aproximadas de suelo/chasis/torre/deposito. Las trayectorias se
interpolan a pasos articulares de hasta 3° y usan ese filtro. No hay demostracion
de continuidad, autocollisiones BRep, deformacion ni cobertura lateral real.
Superior/Lateral son proyecciones de la nube 3D, no cortes certificados.

### Distribucion propuesta, editable en fuente

Chasis de referencia 245 × 200 mm; montaje del brazo `(-35,0,90) mm` en el chasis.
Base del brazo 160 × 160 × 72 mm. Bandeja de 118 × 118 mm centrada en
`(177,5,-130,130) mm`, borde a Z190 mm; cuatro celdas de una sola capa, centros
X150/205 e Y-102,5/-157,5 mm. La bandeja sobresale hacia delante y la derecha.
El apoyo dibujado es conceptual: no acredita fijacion ni estabilidad.

El sensor virtual esta en `(103,0,45) mm`, altura ajustable 20…200 mm en la Web.
Es un **montaje bajo propuesto** para poder estudiar cubos de 50 mm. El GLB
conserva la ubicacion de su sensor original; no se afirma coincidencia entre
ambos ni que el sensor real pueda observar el cubo desde su ubicacion actual.

## Percepcion y mision

Generacion LCG de semilla repetible, 1…12 cubos de 50 mm, separados al menos
150 mm entre centros y fuera del chasis inicial. C1 aparece cerca del robot para
permitir una toma inicial; los demas se distribuyen por el suelo. Hasta24 cubos
con agregado manual, evitando superposiciones. Semilla inicial42, seis cubos;
objetos sobre suelo, alineados con el mundo, sin apilado. Requisito de masa100 g;
no se integra dinamica ni fuerza. Regenerar restablece robot y vacia deposito.

Modelo de sensor: rayo horizontal central ideal contra las cajas, sin LiDAR,
camara, ruido ni cono acustico calibrado. Sin eco queda null. Un barrido toma37
direcciones a5°, cada250 ms, tras posicionamiento300 ms, con base detenida.
La busqueda autonoma por radar usa ecos/estimacion por tamaño conocido; no recibe
IDs o centros del generador como detecciones. El modo por posiciones de27-02
permite seleccionar expresamente un cubo y usa ese objetivo conocido para la
toma. El modelo de eco y el contacto de la pinza consultan la geometria de escena.

Secuencia: recoger brazo → barrer → candidato → orientar/centrar → eco frontal
reciente → aproximar → detener → medir otra vez → preagarre → tomar → elevar →
llevar sobre celda → soltar → recoger brazo. La toma, elevacion y descarga usan
IK y una trayectoria admitida por el filtro antes de moverse. La base permanece
detenida mientras manipula. No se arranca ni se reanuda hardware.

Limites explicitos: objetivo perdido, toma inalcanzable, trayectoria bloqueada,
agarre fallido y deposito lleno detienen la mision con motivo visible. Sin una
trayectoria intermedia adecuada puede detenerse aunque exista otra solucion.
El siguiente desarrollo sera ampliar el planificador a partir del informe y
del ensamble, conservando los fallos como resultados posibles.

## Comprobaciones futuras, solo por pedido expreso

- Ambos paneles: seleccion, ocultar/mostrar referencias, cambio de camara,
  posiciones conservadas y parada en salida/foco/modal/STOP.
- Motores: cada sentido por rueda, traslacion/giro Mecanum y corte al soltar;
  arrastre con obstaculos, borde y brazo recogido.
- Brazo: seis limites, apertura, agarre sin cubo, transporte de cubo sujeto,
  deposito por celdas, rechazo de colision y alcance inalcanzable.
- Semilla repetible; cubos no superpuestos; sin eco al elevar el sensor por encima
  del cubo; aproximacion dependiente del eco y sin lectura directa del generador.
- Mision completa y cada fallo; deposito lleno con cubos pendientes; reinicio
  detenido; WebGL/carga GLB y recuperacion, escritorio/movil, ambos temas y teclado.

## Interfaces y autoria

No cambia ROBO2, EX01, afr2-ws-1, comandos, capacidades ni catalogo OLED. El estado
del simulador es local y no se serializa como telemetria. El servicio de sensores
fisicos conserva SENSORS_RADAR_8.md.

Fuentes nuevas de autoria propia: Web/src/features/collector/* y styles/collector.css.
Reutilizacion interna: RobotModel, OrbitControls, MODEL_COMPONENTS, cinematica
Mecanum, componentes UI y tokens0.1.9 autorizados. La ficha02 restaura los
componentes3D compartidos y usa adaptadores locales SimulationModel para el
simulador, dejando Control sin modificaciones. Cadena adaptada de los archivos
propios v02 del CAD, sin modificar sus maestros ni informes. Esquema SVG nuevo
de autoria propia, sin descargas ni nuevas dependencias. Cambios de navegacion,
Inicio y titulo del Control fisico enlazan los nuevos espacios.

