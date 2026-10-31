# Brazo v17 blanco: Blender y simulador Web

> Entrega anterior: el activo Web vigente fue sustituido por la
> [reexportacion completa](../web_blanco_completo/README.md),425solidos
> y780972bytes. Las cifras siguientes describen esta primera entrega parcial.

AFR2-20261031-02. Fuente: documento Fusion abierto
`AFR2_Brazo_v17_Base_V7_J3_Remoto`, con la geometria y postura manuales
vigentes al exportar el 31/10. No se reconstruye desde el checkpoint de ayer.

Se sustituyen las apariencias `AFR2_PLA_PrintALot_Rojo` y `AFR2_PETG_Rojo`
por `AFR2_PLA_PrintALot_Blanco`, RGB242/242/238. Cambian30cuerpos; materiales
fisicos, geometria, pose y visibilidad del auto conservados. El documento
Fusion abierto conserva sus cambios manuales; el maestro anterior no se
sobrescribe. La entrega contiene exclusivamente las mallas del brazo.

## Entregas

- `AFR2_Brazo_v17_Piezas_y_Web.blend`:392piezas en la coleccion de origen,
  con321mallas unicas. Coleccion Web separada con simplificacion editable.
- `AFR2_Brazo_v17_Blanco.glb`: **596.484bytes**, menor que600.000bytes.
  223.231triangulos y9materiales. Misma copia en `Web/public/models/`.
- `rig_v17.json`: postura exportada, origenes de articulaciones y
  transmisiones; cantidades en metros/radianes segun etiqueta.
- `entrega_glb.json` y `promocion_web.json`: tamano, compresion, procedencia
  y SHA256 del archivo final.

El presupuesto800k inicial se interpreto como800.000bytes; el usuario
pidio despues600k. La primera entrega de789.660bytes se reduce a596.484,
aproximadamente24.5%menos. Esto reduce peso del archivo, no masa del brazo.

## Piezas y materiales

`fusion_exportar_partes.py` tesela cada solido en coordenadas locales,
con tolerancia0.05mm, escribeOBJ/MTL por definicion y manifiesto de
instancias. `exports/brazo-v17-blanco-20261031/` es salida regenerable.
Excluye explicitamente80_Auto_referencia,81_Pan_HCSR04,
82_Envolvente_pan_REFERENCIA y90_Pieza_REFERENCIA; la visibilidad por si
sola no decide si algo pertenece al brazo. No se exportan el auto ni su pan.

`blender_preparar.py` importa las piezas y conserva nueve materialesPBR
derivados de sus apariencias. Convierte RGBsRGB a color lineal para Blender.
La entrega agrupa por eslabon, mordaza o transmision para reducir llamadas
de dibujo; la coleccion de origen mantiene cada pieza individual.
Se consolidan materiales duplicados de la importacionOBJ.

Version final: disolver triangulacion coplanar0.3grados, Decimate0.128,
Draco nivel10, posiciones11bits y normales7bits. Son aproximaciones
visuales: detalles pequenos se simplifican, no es geometria de fabricacion.
Las piezas originales y el documentoFusion se conservan para recuperar detalle.
`blender_vista_final.py` reimporta el GLB comprimido para mostrar la entrega;
su coleccion queda oculta, con el modelo Web editable visible en Blender.

## Integracion Web

`integrar_web.py` copia el activo a `AutoFinalRobo2-Web/public/models/`
y genera `src/features/collector/models/pvc25-v17-blanco.json`.
El descriptor v17 queda seleccionado inicialmente; v03 sigue disponible.
J1..J6, mordazas y cuatro transmisiones visuales tienen nodos separados.
Las transmisiones siguen sus ejes/relaciones, incluida la apertura de pinza.
Los limites nativos se conservan en el descriptor; el seguidor de J3 no
impone otro tope al codo. Un nodo ausente bloquea movimiento mediante
el mecanismo existente del adaptador.

GLB usaYarriba; `restToMount` restauraZarriba para el motor de movimiento.
La postura actual se registra como referencia de la malla, separada de la
postura inicial del simulador. El brazo aislado se eleva18mm para apoyar
la contraplaca inferior en el piso; el montaje sobre auto conserva el
marco relativo a cubierta. Colisiones Web siguen siendo proxies aproximados.

## Estado

Importado, trabajado y guardado en Blender. Archivo final reimportado y
visualizado mediante render nativo, conservando los9materiales y sin auto.
Integracion en fuente Web y archivo publico; sin tests, compilacion ni
validacion en navegador. No se promueve aESP/data/dist ni se modifica
firmware. Sin calculos mecanicos, tensiones ni nuevos estudios de aligerado.

Para regenerar: fasesFusion `appearance`, `export` sobre el
documento indicado; Blender en escena nueva: `import`, `delivery`,
`optimize` con parametros finales; luego `integrar_web.py`.
Las salidasCAD son propias, derivadas del modelo del proyecto; las
referencias comerciales conservan la procedencia documentada en v17.

La fase opcional `archive` solo genera un resguardo local del documento
completo en `.downloads/`, no forma parte de la entrega de partes ni se
importa enBlender/Web. Auto y referencias ocultas no entran en las mallas.
