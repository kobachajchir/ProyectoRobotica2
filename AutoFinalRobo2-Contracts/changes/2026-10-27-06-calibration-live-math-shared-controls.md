# AFR2-20261027-06 — Calibracion explicada y componentes compartidos

27/10/2026. Pedido humano: explicar los parametros de calibracion en una zona
derecha, destacar la matematica con la misma presentacion en las tres simulaciones
y reutilizar el movimiento del auto simulado en Control.

## Resultado en fuente

- Calibracion incorpora una ficha derecha para cada uno de los 29 parametros
  numericos del catalogo actual. Cambia al seleccionar el parametro y muestra
  significado, unidad, rango, valor consultado, valor del editor, interpretacion,
  efecto, ejemplo y dependencias. En pantallas estrechas pasa debajo del editor.
- Los textos se basan en ATMEGA_CONFIG_ACTIONS.json y los servicios AVR actuales.
  Q8 se explica con division por 256; las mascaras se decodifican por canal o
  rueda. Se diferencia el encoder de navegacion de los Hall de ruedas, el PWM
  preparado de una accion ejecutada y las declaraciones de configuracion de una
  medicion fisica. No se proponen ganancias ni capacidades fisicas sin evidencia.
- La ficha no envia comandos. Se mantienen consulta previa, salidas apagadas,
  exclusion de operaciones, interrupcion por foco/enlace y acciones RAM/EEPROM.
  Un valor del editor no se presenta como lectura recibida del controlador.
- Auto, Brazo y Conjunto usan el mismo SimulationMatrix y el nuevo dibujo
  HomogeneousMatrix: mismo selector, matriz centrada, leyenda, lecturas de angulo,
  seno y coseno y expresion simbolica desplegable. Solo cambian las transformaciones
  disponibles y los datos propios de cada simulacion.
- La columna matematica crece a 370–500 px en escritorio (350 px en ancho
  intermedio), con tipografia mayor, corchetes dibujados y bloques de rotacion y
  posicion diferenciados por color y leyenda. Conserva plegado con chevron/riel;
  debajo de 760 px se apila. Traslaciones en mm; rotacion sin unidad. Los numeros
  proceden del modelo nominal simulado, no de posicion medida del hardware.
- MecanumDrivePad sustituye los pads duplicados de Control y simulaciones.
  Giros izquierdo/derecho quedan en fila cuatro, columnas primera/tercera,
  alineados con las flechas. Mantener solicita; soltar, cancelar, perder captura,
  salir del boton, desenfocar o desmontar libera la consigna iniciada.
- HoldActionButton comparte captura y teclado; RangeNumberField comparte numero
  compacto/barra sincronizados entre simulaciones y velocidades de Control.
  Los motores por rueda tambien usan el boton mantenido comun. Se retiran las
  implementaciones y estilos duplicados de esos controles en CollectorPage.

## Limites de la reutilizacion

Los adaptadores conservan sus responsabilidades: Control transforma factores
en mm/s y mrad/s y usa sus guardas, parada y heartbeat de 180 ms existentes;
el simulador integra ruedas y respeta sus estados de manipulacion. No se unen
transporte fisico y estado simulado. El overlay tactil conserva sus gestos propios.
Los formularios de ensayo y calibracion mantienen acciones distintas y no se
convierten en movimiento mantenido. No se agrega ninguna dependencia o activo3D.

## Estado de entrega

Web: implementado en fuente. Contracts: ficha y cobertura VIS-043 sobre identidad
0.1.9 previamente autorizada. Atmega/ESP/CAD: sin cambios de esta ficha; sin
cambio de comandos/wire/OLED ni promocion a ESP/data/dist. Trabajo concurrente
ajeno preservado. Sin tests, compilaciones, validadores, navegador, movimientos
fisicos, commit ni push por esta entrega. Aspecto e interaccion No verificados.

Pendiente solo cuando el usuario lo solicite: observar ayuda al cambiar campos,
tamano/legibilidad de matrices y plegado en escritorio/movil, y comportamiento
de las entradas mantenidas en los adaptadores fisico y simulado.
