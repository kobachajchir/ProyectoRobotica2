# Geometría nominal fija — AFR2-20261030-07

Fuente: exportación del CAD final conservada como Web/public/models/AutoFinalRobo2.glb y documentada en CAD/docs/modelo-final-entrega-20261025.json. Lectura de metadatos GLB, accessors min/max y transformaciones; no se abrió Fusion, ejecutó scripts CAD ni realizó validación o ensayo físico.

Se usó la envolvente del material oscuro de rodillos (Opaque646464) de cada rueda, evitando centrar la envolvente del conjunto que incluye acople/prisionero. Centros nominales aproximados transformados, en mm del modelo Web:

| Rueda del modelo | X lateral | Y altura | Z longitudinal |
|---|---:|---:|---:|
| ManoA:1 | -107.6624 |38.8766|-62.1815|
| ManoB:1 |107.6268|38.9182|-62.2261|
| ManoA:2 |107.6624|38.8766|62.1815|
| ManoB:2 |-107.6268|38.9182|62.2261|

Simetrización nominal: semidistancia entre ejes62.20mm y semitrocha107.64mm; Lx+Ly169.84mm se redondea a170mm en el firmware entero. Trocha nominal215.28mm y distancia entre ejes124.40mm. Diámetro nominal80mm confirmado en documentación de ruedas; no se deduce diámetro físico de la teselación. La referencia axial de montaje±81mm no se utilizó como centro de rueda. Ejes del controlador longitudinal/lateral se corresponden con Z/X del modelo; el signo se conserva según sus convenciones de control.

Es una inferencia nominal de envolvente del CAD exportado, no extracción metrológica de ejes exactos ni calibración del radio de rodadura. El pequeño descentrado de envolventes/teselación se declara. Perfil en contracts/geometry.json y constantes en contracts/afr2_geometry.h. Conserva80mm y usa170mm para Lx+Ly. CPR, signos, captura Hall, pila y aceptación física siguen siendo requisitos independientes; no se afirma precisión de odometría.

EEPROMv8 normaliza estos campos en RAM y no permite que los editores retirados ni registros históricos los cambien. El perfil Web predeterminado comparte geometry.json; documentos de secuencias guardados conservan su perfil histórico y mantienen su comprobación de compatibilidad con el controlador.
