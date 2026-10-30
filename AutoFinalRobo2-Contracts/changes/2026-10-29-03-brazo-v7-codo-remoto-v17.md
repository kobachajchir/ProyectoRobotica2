# AFR2-20261029-03 - Brazo V7, PVC menor y codo remoto v17

## Problema y cambio

El usuario descarto la pinza compacta v15 y el banco alternativo v16.
Se retoma una copia independiente del maestro v09, que conserva pinza,
arquitectura antropomorfa y correcciones de V7. El motor del codo J3 se
traslada al primer eslabon junto a J2, solidario con ese eslabon. Correa
y poleas de aluminio transmiten hasta J3 sin cruzar el giro axial J4.

Se modelan transmision20/50=2.5, HP3M210x6, ambos625 externos, acople,
ejes, chavetas y retenciones. El carro completo se ajusta con cuatroM4
en oblongas4.5x12.5 y tornilloM3; no quedan apoyos fijos fuera del carro.
Se aplican PVC25x1.2 y20x1, ejes70/60 y cortes42/28.5mm, reconstruyendo
encastres y horquilla para conservar continuidad y asientos608.
La montura bilateral baja del auto se conserva.

## Calculo y estado real

La preseleccion incluye pieza200g, masas de los nuevos motores y reservas
de montajes/cableado. Propone STS3250 en J1..J5, STS3032 en J6/pinza,
reducciones J2=4 yJ3=2.5. Margen minimo externo1.272 en el muestreo declarado;
no es aprobacion de frenado real, resistencia PLA ni brazo completo.
Se completaron y revisaron los montajes nativos de J6/pinza con STS3032;
J2 tambien integrado con STS3250,18/72mod0.8Al2024 y ambos625.
Actualizacion30/10: CAD2352g y presupuesto2495g sin pieza; restanJ1/J4/J5.
RevisionJ2:9poses y28angulos del hombro, contactos clasificados en su informe.
Se corrigen asientos de arandelas, borde de travesano a125grados, puente625,
accesoTTL y caras planas reales para los prisioneros de los collares.

La revision nativa local de J3 en tres poses confirma la desaparicion de
cuatro choques corregidos y conserva13 contactos brutos por pose, con su
clasificacion expresa. No acredita recorrido continuo del conjunto.
El usuario suspendio nuevos calculos de vuelco: los archivos anteriores
se conservan como antecedentes, sin seguir ejecutandolos.

Shape Optimization esta identificado en Simulation, con candidatas y
regiones a preservar documentadas. Estudio de aligerado sin ejecutar.
Los STS usan busTTL y12V/6V: interfaz y alimentacion anteriores aun no
representan una integracion electrica final.

Entrega editable, calculos, origen de catalogos y limites:
[revision_v17](../../AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/revision_v17/README.md).
`estado_cad_v17.json` y el journal identifican el guardado y fases aplicadas.

## Estados compartidos

CAD: checkpoint de rediseño en curso, no brazo terminado ni fabricacion
aprobada. v07/v09 conservados; v15/v16 alternativos descartados por usuario.
atmega/esp/web/Loader: sin cambios ni nuevas comprobaciones de software.
Sin tests, compilaciones, flasheos ni ensayo fisico. Se hicieron unicamente
CAD y calculos mecanicos pedidos expresamente. Despieces finales diferidos.
