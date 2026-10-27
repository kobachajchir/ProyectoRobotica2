# AFR2-20261027-11 — Puntos cartesianos y trayectorias independientes del modelo

27/10/2026. Implementacion pedida del plan acordado. [Contrato y guia para otros
modelos](../contracts/WEB_ARM_POINTS.md). Primera entrega: puntos individuales
en Brazo y Conjunto. No secuencias ni movimiento fisico.

## Cambios

- Descriptor de robot desacoplado del archivo visual: cadena, limites, montaje,
  TCP, conversion de coordenadas, mordazas, proxies y posturas. Perfil inicial
  v03; dimensiones y matrices del rig simplificado alineadas con sus parametros.
  CAD v04 concurrente conservado sin sustituir silenciosamente la base acordada.
- Biblioteca compartida en localStorage, con guardar/actualizar/renombrar/eliminar,
  importacion/exportacion JSON, resguardo de datos dañados y trabajo en memoria
  cuando falla almacenamiento. Recargar recupera datos sin movimientos.
- Destinos XYZ/cuaternion respecto del montaje. Cambiar dimensiones recalcula
  angulos; cambiar marcos/TCP exige revision explicita. Angulos enseñados solo
  como referencia/semilla compatible, no recuperacion ciega en otro brazo.
- IK por minimos cuadrados amortiguados y limites en Worker inline. Recorrido
  directo o alternativas intermedias, con comprobaciones geometricas aproximadas.
  Error maximo aceptado1 mm/1°. Fallo numerico no equivale a imposibilidad.
- Tarjeta Puntos guardados compartida; marcadores clicables, recorrido del TCP y
  postura final transparente. Vista previa y ejecucion usan las mismas etapas.
  Cancelacion visible junto al visor, con postura conservada e interrupciones
  existentes. Sin objeto sujeto, sin movimiento simultaneo de base.
- Adaptador para GLB rigidos articulados con nombres/pivotes definidos por datos,
  incluyendo postura de exportacion. Nodos incompletos bloquean movimiento y
  conservan visualizacion estatica. Activos registrados incluidos por Vite en
  futuras compilaciones, sin agregar activos nuevos ni dependencias ahora.

## Diseno y evidencia

VIS-044, identidad0.1.9 autorizada, skills visual-identity-system y apple-design.
No habia skill instalada con nombre exacto Visual Consistency; se cubre esa
intencion con el manual y componentes compartidos del proyecto. Feedback inmediato,
plegables y campos existentes; sin inercia/rebotes que alteren limites o trayectorias.
Composicion Propuesta, fuente implementada, aspecto/interaccion No verificados.

## Estado por modulo

| Modulo | Esta ficha |
| --- | --- |
| Web | Fuente implementada; sin ejecucion de pruebas, build, validadores ni navegador por esta entrega |
| Contracts | Contrato, guia de modelos, ficha y cobertura visual |
| Atmega / ESP | Sin cambios de esta ficha; sin comandos nuevos ni promocion a data/dist |
| CAD | Sin cambios; v03 usado como fuente y v04 concurrente preservado |

No se ejecutaron scripts de CAD, calculos masivos de envolvente, vistas tecnicas,
flasheos, hardware, commit ni push. La envolvente definitiva y laminas del
pendiente08 siguen pendientes. No se afirma validacion continua, carga o espacio
ESP suficiente: el Worker queda configurado inline pero no se compilo ni midio
el paquete en esta entrega. Trabajo concurrente ajeno preservado.
