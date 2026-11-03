# AFR2-20261103-30 — Banco OLED compartido y modal de Configuración

03/11/2026. Pedido humano:
- la pantalla y los controles de Ayuda → OLED deben ser los mismos que los de Pantalla en vivo, como un solo componente reutilizado;
- la lista de pantallas y «Ver árbol de pantallas» a la altura de la pantalla y los controles;
- quitar las referencias grandes de gestos;
- «Simular flujo» / «Biblioteca sin datos» a la derecha del título, contra el borde de la pantalla; la simulación local y los escenarios ordenados;
- corregir márgenes y bordes del modal «Configuración y apariencia».

El mensaje del pedido se cortó al final («Ahora está como desfasada la…»); no se adivinó el resto.

## Cambios en Web/src
- `features/oled/OledWorkbench.tsx` (nuevo): un solo banco con encabezado opcional, pill «Última entrada», acción a la derecha, barra de estado, avisos, pantalla con marco (caducada / en espera), selector lateral de filas, encoder y pie. Usa `EncoderControl` y `MenuSideControls` sin cambios. Con `lockFocus` (solo Pantalla en vivo) nada se selecciona ni recibe foco.
- `LiveOledViewer.tsx`: ahora solo aporta captura, estado y acciones al banco; mismo aspecto que antes.
- `UiSimulationPreview.tsx` y `FirmwareLibraryPreview.tsx`: usan el mismo banco. Se quitan la leyenda de gestos con íconos grandes, el párrafo introductorio y el selector de escenario nativo; ahora hay una pill «Simulación local · datos ficticios» / «Vista local del diseño» y el escenario es un selector segmentado («Con datos» / «Sin datos»). Cada uno registra su «Última entrada».
- `ScreenCatalogBrowser.tsx`: encabezado con el título a la izquierda y el selector de vista a la derecha; la lista llega a la misma altura que el banco y se desplaza por dentro.
- Estilos: las clases pasan de `afr2-live-oled*` a `afr2-oled-bench*`; el banco es un contenedor y pone pantalla y controles lado a lado desde 840 px de ancho propio, apilados por debajo (modal ancho: lado a lado; Ayuda: apilados). Se elimina `.afr2-gesture-legend`.
- Configuración y apariencia (`workspace.css`): una sola separación entre filas, cada fila con su línea superior y cierre inferior, y «Acerca de» con el enlace «Desarrollador» alineado al borde izquierdo (antes con sangría).

## Estado por repo
- **atmega / esp / loader:** sin cambios.
- **web:** fuente modificada. Verificado en el navegador local sin robot a 1440 px: Ayuda → OLED (lista y banco con la misma altura, 966 px), modal de Pantalla en vivo (dos columnas, bloqueo de foco activo) y modal de Configuración. No verificado: celular, la pestaña «Biblioteca sin datos», captura real con robot ni compilación TypeScript.
- Sin commit.
