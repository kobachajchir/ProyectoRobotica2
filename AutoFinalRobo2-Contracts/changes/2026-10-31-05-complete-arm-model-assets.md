# AFR2-20261031-05 - Reexportacion completa y referencias de modelos3D

## Correccion

El usuario detecta partes del brazo ausentes por estar ocultas durante la
exportacion. Se eliminan los filtros de visibilidad: se incluyen todos los
solidos activos del brazo, manteniendo las exclusiones explicitas del auto,
pan, envolvente y cubo, que no pertenecen alGLB articulado.

Fusion425solidos -> Blender425instancias/351mallas originales. Se recuperan
33solidos de motor, apoyos, ejes y fijaciones frente a la entrega392.
Malla Web:780972bytes,268928triangulos,9materiales,13grupos, dentro800k.
Se conserva PLAblanco, pinzaV7 y rig con los recorridos configurados.
Postura y descriptor se regeneran desde el documento abierto.

## Integracion y carga

El visorModelos3D ofreceAuto/Brazo/Cubo. El auto conserva sus referencias;
el brazo incorpora13referencias y el cubo una. Se comparten rutas y
se publica catalogoJSON con hashes. El cubo sigue siendo auxiliar del
simulador, no otro modelo de brazo.

Modelos/catalogo/Draco copiados adist, al paquete gzipWeb y aESP/data/dist.
Solo esas entradas de los manifiestos se actualizan; HTML/JS/CSS conservan
el build anterior. EstimacionFS2383872bytes, presupuesto2867200bytes.
Copias y hashes documentados, sin compilacion, tests, navegador, generacion
de imagenLittleFS ni flash. La interfaz nueva queda en fuente hasta build.

CAD:exportado, reimportado y renderizado enBlender; biblioteca editable.
Web:fuente y activos incorporados. ESP:solo activos filesystem copiados,
sin cambios de firmware ni comprobacion fisica. atmega/Loader:sin cambios.
No se recalculan masas/esfuerzos ni se da por aprobado el recorrido mecanico.

[Entrega completa](../../AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/revision_v17/web_blanco_completo/README.md).
[Rutas y carga](../../AutoFinalRobo2-Web/docs/modelos-3d.md).
