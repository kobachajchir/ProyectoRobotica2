# AFR2-20261102-20 — Visor y ejecución en una sola tarjeta

02/11/2026. Corrección solicitada: el visor de secuencias y Simulación y ejecución no deben parecer dos tarjetas independientes de distinta altura.

El contenedor común recibe borde, radio, fondo, sombra y recorte únicos. El mapa interior pierde su borde/radio/sombra exterior; el transporte deja de usar afr2-glass-panel y pasa a superficie interna con separador. Grid sin gap y con estiramiento: visor a izquierda y controles a derecha desde 1200 px; por debajo se apilan en la misma tarjeta, con separador horizontal. El mapa y su viewport crecen para acompañar la altura del lateral, conservando una altura mínima de escena.

Archivos: Web/src/features/control/sequences/SequenceEditorPanel.tsx y Web/src/styles/visual-language.css. Solo presentación: controles, estados, permisos, simulación y ejecución sin cambios. atmega/esp sin cambios; web fuente modificada; Contracts documentación.

Sin tests, builds, validadores, navegador, render, promoción ni flasheo. Resultado en ejecución No verificado. Sin commit ni publicación solicitados.
