# PVC de menor diámetro para el brazo

Indicación reiterada por el usuario el29/10/2026: después de completar la
transmisión entre hombro y codo, reformar el brazo conservando la pinzaV7,
la arquitectura antropomorfa y sus recorridos, usando tubos más finos.

Se identificaron dimensiones comerciales en el catálogo argentino de
[Tigre, Línea Soldable2025, página16](https://tigresite.s3.amazonaws.com/2025/01/Cata%CC%81logo-Li%CC%81nea-SOLDABLE-2025-AR-WEB.pdf):

| Eslabón | Producto y código | Exterior | Pared | Interior calculado |
| --- | --- | --- | --- | --- |
| Brazo | SD-TUBO PVC CL10,10162432 | 25mm | 1,2mm | 22,6mm |
| Antebrazo | SD-TUBO PVC CL10,10162408 | 20mm | 1,0mm | 18,0mm |

Estos espesores sustituyen la aproximación inicial25/22 y20/17mm para
esta selección comercial. El diámetro interior se deriva restando dos
paredes; no se presenta como una medición de la unidad comprada.
La línea y las medidas también se ofrecen en el mercado argentino:
[Pérsico, tubo PVC soldable CL10 Tigre](https://persicosanitarios.com.ar/productos/tubo-pvc-soldable-cl10-tigre/).
La oferta no acredita stock confirmado ni una compra realizada.

Para los cortes propuestos de42 y28.5mm, las masas de tubo estimadas son
5,46g y2,47g, respectivamente, usando provisionalmente densidad1,45g/cm³.
Se recalcularán con las longitudes que resulten del ensamble terminado.
Los números reproducibles están en `pvc_seleccionado.json`.

La reducción no consiste en cambiar el tubo dejando encastres de32mm.
Se necesitan parámetros separados para ambos exteriores y espesores,
interiores derivados, encastres y topes correspondientes, holguras de
impresión y longitudes de inserción. Se conservarán los ejes articulares
y límites de giro. Se revisarán aplastamiento local del tubo, flexión,
torsión y retención de las uniones. Un mandril interior corto integrado
al encastre es una opción para repartir el apriete sin añadir herrajes.

La corredera del motorJ3, ambos apoyos y la polea de entrada deben seguir
adaptándose conjuntamente al primer eslabón y conservar su alineación.
Las distancias70/60mm estan aplicadas. Se reconstruyeron los encastres
proximales y la horquilla de codo para conservar continuidad y los apoyos.
El corte28.5mm deja1.5mm hasta el608 inferior del eje axial de muneca.

## Estado y procedencia

Dimensiones verificadas contra el catalogo y su tabla visual. Aplicadas a
los tubos y encastres del F3Dv17, con parametros independientes por eslabon. La clasificaciónCL10 es hidráulica: por sí sola
no acredita resistencia estructural del brazo ni de un tubo perforado.
La masa principal también se reducirá en housings, horquillas, plataforma
y actuadores; el ahorro de los tubos no basta para justificar todo el peso.

Catálogo recuperable en `.downloads/actuadores/Tigre_Soldable_AR_2025.pdf`,
excluido deGit. SHA256:
`97ee5ca56d1f023419148b1b06bfd9852eea70e245a143e17495309705192bcf`.
Documento del fabricante; no se redistribuye como obra propia. Consulta:
29/10/2026. No se hicieron ensayos físicos ni tests de software.
