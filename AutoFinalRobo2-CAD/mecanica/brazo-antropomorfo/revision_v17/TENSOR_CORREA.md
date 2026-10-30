# Tensor por corredera de J3

Indicación del usuario del 29/10/2026: las fijaciones del motor y del apoyo
del eje deben permitir desplazar el conjunto al aflojarlas. Se elige mover
el conjunto de entrada completo, sin añadir una polea tensora intermedia.

La placa fija tiene cuatro ranuras **oblongas**, de 4,5 mm de ancho y
12,5 mm de longitud total. La distancia entre los centros de sus extremos
semicirculares es 8 mm. Los tornillos M4 llevan arandelas de 10 mm y
tuercas. El recorrido útil propuesto es −2 a +6 mm respecto de la posición
nominal; el extremo positivo acerca las poleas y permite instalar la correa.
La tensión final debe ajustarse para la correa comprada, sin asumir que
la posición nominal del CAD equivale a una pretensión conocida.

El carro lleva motor, acople, eje de entrada y los dos apoyos externos
625-2Z. La polea de entrada queda entre ambos apoyos. Las fijaciones
ajustables contra la estructura están sobre las ranuras; los agujeros
circulares del carro acompañan a sus tornillos durante el desplazamiento.
Los apoyos no quedan atornillados por separado a una parte inmóvil.

Los dos tornillos que anclan permanentemente la placa fija al encastre
del eslabón son distintos de los cuatro tornillos de ajuste. Quedan fuera
de la trayectoria del carro. Se prevé un tornillo M3 de ajuste fino que
empuja el carro en el sentido de tensado; los M4 fijan la posición final.

Para instalar: aflojar los cuatro M4, retroceder el tornillo de ajuste,
acercar el carro al codo, colocar la correa y alejarlo para tensar. Después
reapretar los M4. No se afloja el eje del codo ni los prisioneros que
transmiten el par: la alineación se conserva al mover el conjunto completo.

Se mantienen los ejes y límites articulares originales. La polea de salida
permanece unida al antebrazo; la placa, el motor y sus apoyos pertenecen al
primer eslabón y acompañan el giro de J2. La correa no cruza J4.

## Estado real

Las fases aplicadas y sus incidencias se registran en `progreso_J3.jsonl`.
Están aplicados y guardados motor, placa oblonga, carro, housing con dos625,
rotor de entrada, poleas, acoples, chavetas, retenciones, anclajes y tornillo
de ajuste fino. El conjunto tiene15 articulaciones y5 enlaces de movimiento.
Se corrigió el corredor lateral desplazando10mm la transmisión completa,
sin elevar la montura ni cambiar los ejes articulares. Los anclajes fijos
usanM4x40 y conservan apoyo sobre las caras de contacto.

La pinza V7 sigue conservada. Ya se guardaron ejes70/60mm, PVC25x1.2 y20x1,
cortes42/28.5mm y correa210mm; la distancia nominal entre poleas es50.4672mm.
Se simplifican los encastres afectados por el acortamiento. El estado de
cada fase consta en el journal y el último guardado en estado_cad_v17.json.
No se atribuyen al modelo la masa presupuestada1452.5g ni sus márgenes:
la ultima extraccion previa a adaptar los otros actuadores dio2207g.
La preseleccion nueva esta en SELECCION_ACTUADORES.md.
La ultima revision de J3 tras corregir encastres, taladros y holguras dio
13 contactos brutos en tres poses. Se clasifican en REVISION_CONTACTOS_J3.md.
No sustituye un barrido continuo ni la revision de otros actuadores nuevos.

La relacion vigente es20/50=2.5, con correa OMEGA HP3M210x6.
Los discos lisos de las poleas representan masa conservadora; no dentado
para fabricar. Los contactos de correa sobre esos discos se documentan.

## Referencias comerciales

- [FEETECH STS3250, especificación del fabricante](https://www.feetechrc.com/Data/feetechrc/upload/file/20240120/6384135881578380868917773.pdf):
  envolvente, estriado y par del actuador; no usar los tornillos de cierre
  de la carcasa como fijación estructural del robot.
- [Catálogo SKF, rodamientos de una hilera](https://cdn.skfmediahub.skf.com/api/public/0901d196802809de/pdf_preview_medium/0901d196802809de_pdf_preview_medium.pdf):
  625-2Z de 5 × 16 × 5 mm, masa de referencia 5 g, capacidad estática
  básica 380 N. No usar la capacidad de otra marca sin identificarla.
- [Manual técnico Optibelt](https://a.storyblok.com/f/192292/x/2d92ee5443/optibelt-tm-rubber-timing-belt-drives.pdf):
  el dimensionamiento debe corresponder a la gama y ancho de la correa
  adquirida. OMEGA HP y una HTD genérica no tienen capacidades equivalentes.

Las poleas se representan inicialmente mediante discos de aluminio a
diámetro primitivo para estimar masa de forma conservadora, con pestañas
y sus conexiones al eje. Estas envolventes no son perfiles de fabricación
del dentado comercial. El diseño del eje de entrada tiene dos apoyos externos para
reducir la carga lateral transmitida al actuador.
