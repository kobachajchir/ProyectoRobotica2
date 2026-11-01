# Cubo50mm: modelo auxiliar de simulacion

AFR2-20261031-04. Exportado desde la pieza de referencia del documento
Fusionv17 abierto, no recreado desde una caja Web. Solido:
`90_Pieza_REFERENCIA_50mm_100g/Pieza_50mm_REFERENCIA/Bloque`.

- `AFR2_Cubo_50mm.glb`:1784bytes,12triangulos,1material. Escala1unidad=1m,
  dimensiones50x50x50mm y origen en el centro del cubo.
- Apariencia original del objeto preservada; `100g` es un nombre historico,
  no asigna por si solo masa a la simulacion.
- `AFR2_Cubo_50mm.blend`: biblioteca Blender con la coleccion del auxiliar,
  disponible para Append; sin escena del brazo ni auto.
- `cubo_fusion.json`: teselacion local original y apariencia. Scripts de
  exportacion para Fusion yBlender; hashes en `entrega_cubo.json`.

El GLB se copia a `AutoFinalRobo2-Web/public/models/`. Registro independiente
`auxiliaryModels.ts`; `AuxiliaryModelAsset.tsx` instancia el activo con
geometria/material compartidos y arbol de transformacion por cubo.
`CollectorScene` lo utiliza para los objetos de la simulacion conjunta,
conservando la posicion, seleccion y orientacion en piso/pinza/deposito.
El color del material no se sustituye por el estado de alcance; los
indicadores de seleccion y las etiquetas siguen en la escena.

El auxiliar no se agrega al selector de modelos roboticos. No se incluye
el cubo en el GLB del brazo, que sigue pesando596484bytes. Sin tests,
compilacion, navegador, calculos de carga ni promocion aESP.
