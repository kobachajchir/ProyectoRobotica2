# AFR2-20261031-08 — Render frontal del conjunto

Pedido humano: imagen final de alta calidad conservando el angulo frontal
actual. Se captura la camara ortografica de Fusion del31/10/2026 y se prepara
una escena de estudio independiente en Blender. No se cambia el CAD nativo.

Se usan los425solidos completos del brazo,27mallas del auto detallado y el cubo
auxiliar50mm, con su postura actual y materiales originales. El auto se convierte
del marco Blender sobre el piso al datum de cubierta del brazo. Las normales
de copias de malla se suavizan por angulo, conservando los originales.

Salida:4096x3072, PNG sin perdida, Cycles CPU,128muestras maximas, muestreo
adaptativo y denoising OpenImageDenoise. Tres luces de area, suelo gris azulado
y gestion AgX. Vista previa para encuadre/iluminacion autorizada por el pedido
de render; no se ejecutan tests, builds, validadores ni revisiones mecanicas.

Escena, imagen y registro regenerables en
`AutoFinalRobo2-CAD/renders/frontal-20261031/AFR2_Frontal_4K_20261031.*`.
Script versionado:
`AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/revision_v17/render_frontal_actual_20261031.py`.
La escena se guarda en un archivo propio sin reemplazar el Blender del brazo.
atmega/esp/web/Loader sin modificaciones por esta entrega.
