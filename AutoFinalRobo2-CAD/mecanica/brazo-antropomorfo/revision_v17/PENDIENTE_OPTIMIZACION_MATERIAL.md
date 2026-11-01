# Diferido: estudio de aligerado selectivo en Fusion

IndicaciÃ³n expresa del usuario,29/10/2026: dejar anotada una pasada
posterior de optimizaciÃ³n de forma/topolÃ³gica para algunas piezas del
brazo. Otras piezas serÃ¡n aligeradas manualmente por el usuario.
**No ejecutar el estudio en esta pasada.** Confirmar herramienta disponible,
licencia y piezas seleccionadas cuando se retome; no darlo por iniciado.

Antes del estudio se deben congelar la revisiÃ³n geomÃ©trica y los casos de
carga del brazo terminado, con200g y frenada finita declarada. Definir
material, perfil/orientaciÃ³n de impresiÃ³n o aleaciÃ³n y proceso reales.
Preservar alojamientos de rodamientos, interfaces de motores, caras de
apoyo, agujeros/roscas, encastresPVC, raÃ­les de la pinza, ranuras del tensor
y espacio para herramientas y montaje. El usuario elegirÃ¡ las piezas.

El resultado sugerido por el estudio requiere rediseÃ±o fabricable y una
nueva revisiÃ³n de cargas, rigidez, contactos y choques. No se considera
una aprobaciÃ³n automÃ¡tica de las piezas impresas ni del vuelco del auto.

## Alcance vigente despues de la ultima respuesta

El usuario suspendio nuevos calculos de vuelco. Se priorizan actuadores
capaces de mover200g mas la masa propia del brazo, y revisar la herramienta
de aligerado de Fusion. Esto no significa estabilidad aprobada.
La investigacion y preparacion estan en alcance. Se conserva la indicacion
anterior de dejar el estudio para una pasada posterior; no se resuelve
ningun trabajo de optimizacion en nube en esta entrega.

## Herramienta y preparacion comprobadas29/10

La herramienta oficial se llama **Shape Optimization** en el entorno
Simulation. La consulta nativa de esta instalacion encontro el workspace
SimulationEnvironment y los comandosOptimize Shape yShape Optimization
Criteria. Solo se leyeron sus registros: no se activo ni resolvio un estudio.
El snapshot de licencia indicaStudent; la disponibilidad de una resolucion
en nube se confirmara al retomar, no se deduce de que exista el comando.

Autodesk indica que el resultado de optimizacion no muestra las tensiones
producidas por las cargas. Hay que reconstruir un CAD fabricable y despues
comprobarlo con un estudio estructural separado. Las regiones preservadas
protegen interfaces; Min Member Size impone un espesor minimo a la propuesta.

Fuentes oficiales consultadas:

- [Shape Optimization](https://help.autodesk.com/view/fusion360/ENU/?contextId=SIM-SHAPE-OPTIMIZATION).
- [Preparacion del estudio](https://help.autodesk.com/view/fusion360/ENU/?contextId=SIM-SHAPE-OP).
- [Regiones preservadas](https://help.autodesk.com/cloudhelp/ENU/Fusion-Simulate/files/SIM-CREATE-PRESERVE-REGION-TASK.htm).
- [Espesor minimo](https://help.autodesk.com/cloudhelp/ENU/Fusion-Simulate/files/MIN-MEMBER-SIZE-CONCEPT.htm).

Candidatas para seleccionar mas adelante, sin quitar material ahora:
plataforma/horquillaA01, soporte de munecaA15 y carro del motorJ3.
Preservar especialmente diagonales nuevas, asientos608/625, anclajes,
ranuras y caras que comprimen motores. La pinzaV7 no se rediseña en esta
preparacion. No usar propiedades isotropicas genericas como demostracion
de resistencia intercapas del PLA PrintALot con perfil aun indefinido.

## Orden de seleccion propuesto

Masas anteriores al montaje de los nuevos actuadores, desde volumen CAD
macizo y1.24g/cm3; no son pesajes ni estimaciones del laminador:

| Pieza candidata | Masa PLA de referencia | Motivo |
| --- | ---: | --- |
| A15, horquilla de muneca | 91.5g | Masa distal: quitarla reduce tambien la demanda de J2/J3. Preservar engranaje, orejas625 y camino de carga. |
| A01, plataforma y torre de hombro | 239.8g | Mayor volumen impreso; preserve apoyos608, diagonales y anclajes. Tiene menos efecto sobre el par de J3. |
| Montura baja V05 | 128.3g | Gran pieza fija; candidata secundaria, sin tocar el patron bilateral de fijacion al auto. |

El carroJ3 es de aluminio y se tratara con sus propiedades propias, no
como PLA. Las nuevas cunas de motor deben terminarse antes de seleccionar
regiones removibles. No perseguir un porcentaje de reduccion prefijado
sin revisar primero las regiones preservadas y los espesores fabricables.
