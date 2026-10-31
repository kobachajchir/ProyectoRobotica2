# AFR2-20261031-02 - Brazo blanco y activo articulado Web de600k

## Cambio

El usuario pide cambiar el plastico rojo a aparienciaPLAblanco, exportar
por piezas el brazo abierto excluyendo el auto oculto, trabajarlo en
Blender y llevarlo al simulador Web. Reduce posteriormente el presupuesto
de800k a600k, manteniendo calidad visual.

Se exporta la geometria y pose manuales actuales deFusionv17:392piezas,
321mallas unicas,9materiales. Se excluyen auto80,pan81,envolvente82 y
pieza90. Treinta cuerpos pasan a apariencia blanca sin cambiar material
fisico. DocumentoFusion y piezas originales conservan los cambios manuales.

Blender conserva todas las piezas originales y una coleccion Web separada.
EntregaGLB **596.484bytes**,223.231triangulos,9materiales; compresionDraco
y simplificacion editable. El archivo de789.660bytes previo se reduce24.5%.
No representa una reduccion de masa fisica ni una pieza para imprimir.

La Web registra el descriptorv17 como inicial y conserva elv03 opcional.
El adaptador mueve eslabones, mordazas, engranajes y polea de entrada por
sus respectivos vinculos visuales; respeta los limites del descriptor.
El archivo se integra en `public/models/`, sin sustituir el modelo del auto.
Nodos/pose/origenes de montaje proceden de la exportacion actual;
motor de movimiento e interfaz conservan su separacion.

## Evidencia y limites

GLB reimportado enBlender y mostrado con render nativo;9materiales,
26objetos y13mallas agrupadas. Coleccion de origen392piezas preservada.
Integracion en fuente Web y activo publico; **sin tests, compilacion,
navegador ni promocion aESP**. No se ejecutan calculos mecanicos ni estudios.

[Fuentes y entrega](../../AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/revision_v17/web_blanco/README.md).
`promocion_web.json` registra SHA256 y presupuesto final.

## Estados

CAD: apariencia modificada yBlend editable guardado; derivado visual.
Web: fuente/GLB incorporados; no build ni runtime comprobado.
atmega/esp/Loader: sin cambios; ESPdist conserva su estado anterior.
Los cambios preexistentes de SimuladorUI y sus documentos se preservan.
