# AFR2-20261031-04 - Cubo como modelo auxiliar del simulador

El usuario solicita exportar el cubo y agregarlo como modelo auxiliar.
Se extrae solo el solido de referencia deFusionv17, en coordenadas locales
centradas:50x50x50mm. Se conserva su apariencia y se exporta mediante
Blender como GLB independiente:1784bytes,12triangulos y1material.

La Web agrega registro de auxiliares independiente de los modelos de brazo.
El simulador utiliza el GLB en lugar de la caja procedural para sus cubos,
con una instancia de transformacion por objeto y geometria/material en cache.
Conserva logica de ubicacion, agarre, deposito, seleccion y etiquetas.
La medida comun de la pieza procede del descriptor del auxiliar.

No se incluye auto ni brazo en este activo, ni se modifica el GLB del brazo.
Fuente y activo Web incorporados; sin tests, build, navegador, flash ni
promocionESP. CAD:fuentes y bibliotecaBlend auxiliar; Web:fuente/GLB;
atmega/esp/Loader:sin cambios.

[Fuentes y entrega](../../AutoFinalRobo2-CAD/mecanica/modelos-auxiliares/cubo-50mm/README.md).
