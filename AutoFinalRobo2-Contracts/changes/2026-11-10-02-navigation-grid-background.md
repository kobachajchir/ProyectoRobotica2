# AFR2-20261110-02 — Fondo de cuadrícula en la navegación

> **Sustituido por aclaración del usuario:** la referencia era el simulador 3D. La cuadrícula global se retiró y la interfaz recuperó su fondo anterior. Ver [AFR2-20261110-04](2026-11-10-04-simulator-grid-background-correction.md).

Versión: **1.14.1 (subversión mínima)**.

Confirmada: el usuario pide el fondo de todos los entornos de navegación basado en la cuadrícula final de Armado. Referencia Observada: `AssemblyCollectionEnvironment.tsx`, piso con líneas menores y ejes destacados. Alcance Inferido: canvas general de las rutas Web, detrás de sus contenidos.

Implementado en el fondo compartido html/body de `styles/index.css`, con tokens de `styles/tokens.css`. Adaptación Propuesta: retícula ortogonal estática de 48 px y líneas principales cada 240 px, equivalente visual para navegación 2D del piso 3D; las líneas toman text-muted con opacidades 12/20 %, por lo que se adaptan a los dos temas. Conserva canvas y superficies opacas de tarjetas, formularios, cabeceras, escenas y navegación. No se agregan archivos gráficos, modelos, JavaScript, animaciones ni eventos. Sin perspectiva que deforme la interfaz.

Estado: Web=fuente; ATmega/ESP=solo versión; CAD/Loader=sin cambios. Sin tests, compilaciones, validaciones, navegador ni flasheo. Cobertura de fuente: fondo general de todas las rutas; cobertura visual de móvil/escritorio y ambos temas No verificada. No se altera la cuadrícula de las escenas 3D.
