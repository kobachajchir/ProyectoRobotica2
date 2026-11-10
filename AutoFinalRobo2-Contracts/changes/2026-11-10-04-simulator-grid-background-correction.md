# AFR2-20261110-04 — Cuadrícula solo en los simuladores

Versión: **1.15.0**, conservada como corrección del trabajo aún sin cargar (regla 3).

Aclaración del usuario: el pedido de fondo basado en la cuadrícula final de Armado correspondía a los entornos 3D del simulador, no a la navegación de páginas. Se retira de html/body la cuadrícula de AFR2-20261110-02 y se eliminan sus tokens canvas-grid, restaurando canvas-gradient como antes. No se revierte el botón de audio ni la misión de recolección.

`SimulationViewport.tsx`, compartido por auto/brazo/conjunto y las escenas de control, pasa a canvas transparente sobre los mismos gradientes de fondo de Armado. Piso tenue de opacidad 0,055 y retícula 3D de 50 mm, líneas de opacidad 0,24 y ejes destacados, como la escena final de `AssemblyCollectionEnvironment.tsx`. Se conservan tamaño/centro del suelo por consumidor, iluminación, cámara, interacción y estados. Etiqueta de escala actualizada a 0,05 m. GridHelper propio con liberación de geometría y materiales al desmontar.

Tokens sim-scene ajustados al fondo y tonos de la referencia; `simulation-viewport.css` limita el nuevo material a los viewports. La cuadrícula pertenece al plano 3D y sigue la perspectiva de cámara; la interfaz general recupera su fondo anterior. No agrega modelos, imágenes, dependencias ni animaciones.

Estado: Web=fuente; ATmega/ESP/CAD/Loader=sin modificaciones adicionales. Sin tests, compilaciones, validaciones, navegador ni flasheo. Resultado visual sin comprobar en ejecución.
