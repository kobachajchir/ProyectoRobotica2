> **Vigente AFR2-20261025-02:** [táctil local y giro simulado](../../../AutoFinalRobo2-Contracts/contracts/WEB_TOUCH_SIMULATION.md). Sesión local sin comandos, vista superior/flechas compartidas y ruedas por cursor en GOTO/secuencias. No se usa pose medida. Web compilada; visual/móvil pendientes. Descripciones previas conservadas como historia.

# GOTO local — AFR2-20261024-12

En Control → GOTO, tocá un punto del suelo para agregarlo a la lista o cargá
pasos como Adelante2m, Izquierda1m. Simular recorrido ejecuta todo en orden.
También hay coordenadas por teclado, reordenar/quitar, pausa/continuación,
repetición, reinicio y vaciado. Mapa y comando actual comparten columna;
X/Y/rumbo/tiempo se apilan en la barra derecha junto al reproductor. En móvil
angosto, la barra pasa debajo. Recorrido cerrado muestra una fila completa y
medias vecinas sin scrollbar. Abierto ofrece lista completa y Seguir activo.
Recorrido local agrupa el formulario plegado por encima de reproducir; detalles
del simulador se pliegan al pie. Fuente compilada, composición visual pendiente.

- `goto/gotoSimulation.ts`: plan puro acotado y muestreo analítico; +X adelante,
  +Y izquierda, puntos absolutos, rumbo0. mm/ms internos y metros en formulario.
- `goto/GotoSimulatorPanel.tsx`: borradores en ControlPage, reloj local30Hz,
  gates de edición, vida del reproductor y actividad STOP compartida.
- `goto/GotoCommandList.tsx`: estado activo/completado/pendiente y edición de orden.
- `goto/useActiveCommandScroll.ts`: scroll FLIP del contenedor, transform WAAPI,
  220ms easeInOut; cancelación sin salto, reduced motion/teclado y toma manual.
- `ControlMap{,Scene}.tsx`/`TopMap.tsx`: suelo seleccionable, destinos/trazo,
  encuadre y modelo final a escala1unidad=1m, cuadrícula0,5m, selección0,1m.
- `styles/goto-simulator.css`: capas de acento con opacity140ms ease, tokens y
  primitivas de marca existentes, sin Motion/dependencias nuevas.

64comandos/10min, radio por eje±50m, pasos0,1..10m y velocidad0,05..0,5m/s.
Representa traslación ideal; no certifica trayectorias físicas ni esquiva objetos.
Editar reinicia el cursor. Los borradores de lista/velocidad sobreviven al cambio
de tarea dentro de Control, no la reproducción. Scroll manual libera Seguir activo.

La lista no emite comandos ni adquiere control. El formulario de GOTO físico sigue
separado y condicionado a firmware/odometría/calibración. No sustituye telemetría.
Fuente compilada/empaquetada; aspecto, gestos y animaciones no comprobados en
navegador por denegación previa. Sin tests, validadores independientes ni hardware.
[Contrato/estado](../../../AutoFinalRobo2-Contracts/contracts/WEB_CONTROL_MAP.md).
