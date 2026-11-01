# Reexportacion completa del brazo, sin filtro de visibilidad

AFR2-20261031-05. Sustituye la entrega Web parcial de392solidos.
La exportacion anterior filtraba `Occurrence.isVisible` y `BRepBody.isVisible`;
se retiraron ambos filtros. Se exportan todos los solidos activos pertenecientes
al brazo, aunque esten ocultos. Auto80, pan81, envolvente82 y pieza90 siguen
excluidos del archivo del brazo; auto y cubo son activos independientes.

## Entrega actual

- **425solidos instanciados**,351mallas unicas en origen. Se recuperan33
  solidos ausentes en la primera exportacion: motor y fijaciones deJ5,
  eje/retencionesJ4, apoyo625, tapas, insertos y tornilleria.
- `AFR2_Brazo_v17_Blanco.glb`: **780.972bytes**, menor que800.000bytes.
  268.928triangulos,9materiales,13grupos del rig. PLAblanco conservado.
- `AFR2_Brazo_v17_Completo.blend`: escena independiente con las425piezas
  originales y coleccion Web simplificada, separada de las revisiones previas.
  Incluye camara; se puede abrir directamente, o importar colecciones porAppend.
- `rig_v17.json`: postura actual, marcos de articulaciones y transmisiones.
  El descriptor Web se regenera para esta postura; no reutiliza la pose anterior.
- `inventario_reexportacion.json`: comparacion contra las392instancias.
  `entrega_glb.json`, `promocion_web.json`, `copias_modelos.json`: parametros,
  hashes y destinos de carga.

Calidad: disolucion coplanar0.3grados, Decimate0.145, Draco10,
posiciones12bits/normales8bits. No sirve para fabricar ni calcular volumen.
Los nombres del rig se normalizan en elGLB para no heredar sufijos de otras
revisiones abiertas en Blender. No se cambia geometria mecanica ni peso fisico.

## Proceso reproducible

`../web_blanco/fusion_exportar_partes.py` recibe `ARM_EXPORT_HERE` apuntando
a esta carpeta y `ARM_EXPORT_OUT` a `CAD/exports/brazo-v17-completo-20261031`.
Fasesappearance/export; ninguna filtra por visibilidad. Se conserva la pose
y visibilidad del documentoFusion.

`blender_reexportar.py` reutiliza el procesado anterior con nuevas colecciones
para import/delivery/optimize. FijarWEB_RATIO=.145,WEB_BUDGET=800000,
POSITION_BITS=12,NORMAL_BITS=8. `integrar_web.py` promueve este GLB y su rig.
La version anterior deBlender se conserva, sin sobrescribir su archivo.

## Web y paquetes

`Modelos 3D` muestra Auto/Brazo/Cubo. El auto conserva sus referencias;
el brazo tiene13referencias por ensamble/transmision y el cubo una referencia.
El simulador usa los mismosGLB y el descriptor v17-complete.
Rutas compartidas en `components/three/modelAssets.ts` y catalogo publico
`models/catalog.json`, con tamano, URL ySHA256 de cada activo.

`tools/sync-model-assets.py` copia modelos/catalogo/Draco apublic ydist,
y susgzip aartifacts/littlefs/dist yESP/data/dist. Actualiza solo esas
entradas y totales de los manifiestos. No sustituye HTML/JS/CSS compilados.
Estimacion del paquete despues de copiar:2.383.872bytes sobre presupuesto
2.867.200. El auto conserva871.756bytes; el cubo1.784bytes.

**No se ejecuta build, test, navegador, generacion de imagenLittleFS ni
flasheo.** LosGLB estan copiados para cargar; el HTML/JS de los paquetes
mantiene el build anterior. Para que la nueva interfaz de inspeccion quede
en ese paquete hace falta una compilacion posterior autorizada.

El GLB final se reimporto enBlender:26objetos,13mallas,9materiales,
sin auto ni cubo. Render nativo de esta reimportacion en
`exports/brazo-v17-completo-20261031/vista_glb_completo.png`.
No se deduce resistencia, capacidad fisica o recorrido libre de choques.
