# AFR2-20261027-02 — Control original, brazo individual y simulacion conjunta

27/10/2026. Correccion humana: Control debe permanecer tal cual estaba, incluido
su control tactil. Solo se agregan Brazo robotico y Simulacion conjunta; el brazo
debe permitir seleccionar articulaciones y usar controles a la derecha.
Esta ficha sustituye la reorganizacion de rutas y disposicion de27-01.

## Cambios

- Se restituye ControlPage a su fuente previa y tambien RobotModel/OrbitControls
  compartidos. Los adaptadores de auto/servo/orbita nuevos viven exclusivamente
  en features/collector/SimulationModel.tsx. No se edita manual/ ni la implementacion
  tactil, secuencias, ruedas, GOTO o calibracion de Control.
- Inicio/barra recuperan Control en /control, con su titulo y descripcion previos.
  Se agregan /brazo y /simulacion; se retira /robot. El proveedor nuevo solo monta
  para esas dos rutas, sin envolver las pantallas anteriores.
- Brazo: modelo individual apoyado en el suelo, articulaciones seleccionables
  por sus mallas aun con referencias ocultas, aro del eje activo, seis selectores,
  deslizador/numero para el eje elegido y apertura/cierre explicitos del gripper.
  Controles a la derecha en escritorio; debajo en pantallas estrechas.
- Simulacion conjunta: robot, brazo, deposito y cubos. Permite generacion por
  semilla, agregar por X/Y, colocar con clic en suelo, editar posiciones de cubos
  y posicion/rumbo del robot. No se admite superposicion con cubos o chasis.
- Zona de toma proyectada en suelo y volumen muestreado. Los cubos del suelo se
  colorean verdes cuando sus posturas de preagarre/toma/elevacion tienen soluciones
  articulares admitidas; ocre cuando requieren acercar el robot. La trayectoria
  se planifica al iniciar la toma, con fallos explicitos si no se encuentra ruta.
- El cubo elegido dentro del alcance inicia preagarre → toma → elevacion → celda
  del deposito → apertura → transporte, sin trasladar la base. Fuera de alcance,
  el robot recoge el brazo, se orienta, exige eco frontal y se aproxima antes de
  tomarlo. Un objetivo seleccionado por el usuario usa sus coordenadas conocidas;
  la mision por radar sigue eligiendo candidatos a partir de ecos.
- La interpolacion usa una fraccion comun a todos los ejes y la misma trayectoria
  muestreada por el planificador. Hay alternativas finitas de posturas intermedias.
  Se corrige la llegada asintotica: tolerancia3 mm para detener la aproximacion.

Brazo individual y conjunto conservan estados separados en memoria al alternar
entre estas dos rutas. Cambiar ruta detiene el trabajo activo. Sin reanudacion
de misiones al recargar ni envio de movimientos a placas.

## Estados reales

| Modulo | Estado |
| --- | --- |
| atmega | Sin cambios |
| esp | Sin cambios ni promocion de data/dist |
| web | Fuente modificada; sin tests, builds, validadores ni navegador |
| shared | Correccion registrada; sin nuevos comandos/capacidades/wire/OLED |
| cad | Sin modificaciones de esta ficha; montaje nativo definitivo pendiente |

Rig simplificado del brazo v02 y sensado ideal de la ficha01, con restricciones
aproximadas; no certifica autocollisiones continuas, capacidad de carga, agarre
fisico ni estabilidad. C1 se genera cerca para permitir una toma inicial; los
otros cubos se distribuyen por semilla. Hasta24 cubos al agregar manualmente,
cuatro posiciones de almacenamiento sin apilar.

No se ejecutan tests, compilaciones, validaciones, navegador, flasheos ni puertos
serie por instruccion humana. Sin commit ni push. Cambios ajenos de PCB/brazo
preexistentes preservados. VIS-040, cobertura nueva y aspecto No verificado.
