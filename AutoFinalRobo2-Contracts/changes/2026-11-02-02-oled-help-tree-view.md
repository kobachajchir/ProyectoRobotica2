# AFR2-20261102-02 — Simulador UI dentro de Ayuda → OLED, árbol como vista alternativa

02/11/2026. Pedido humano (con capturas de Ayuda → OLED):
- Sacar el Simulador UI de Desarrollador, porque Ayuda → OLED ya lo cubre.
- Dejar el árbol conceptual (AFR2-20261102-01) como vista alternativa de la lista.
- Primero la interfaz (pantalla) y a la derecha la lista; arriba de la lista,
  «Ver árbol de pantallas», que lleva al árbol.
- Corregir las referencias visuales de los gestos: icono arriba y texto abajo,
  una al lado de la otra.

## Cambios

- **Desarrollador:** se quitó la tarjeta Simulador UI. `/desarrollador/ui-simulator`
  redirige a `/ayuda?seccion=oled&vista=arbol`. Se borraron `UiSimulatorPage.tsx`,
  `UiSimulator.tsx` y `styles/ui-simulator.css` (sin otros usos). Se conservan
  `uiSimulation.ts` y `uiSimulationFlow.ts`, que usa el árbol para el grafo y
  las vistas previas.
- **Ayuda → OLED, lista:** `ScreenCatalogBrowser` muestra primero la vista previa
  (biblioteca del ATmega con botones de fila y encoder) y a la derecha la lista
  de pantallas. Arriba de la lista: contador y «Ver árbol de pantallas». Por
  debajo de 1200 px se apilan, con la pantalla primero. Acepta `?pantalla=` para
  abrir una pantalla puntual.
- **Ayuda → OLED, árbol:** `?vista=arbol` muestra `ScreenTreeMap` con «Ver lista».
  En la ficha, «Ver en la lista» abre esa pantalla en la vista previa.
- **Leyenda de gestos** (`FirmwareLibraryPreview`): de una línea con iconos
  intercalados a una fila de tres elementos, cada uno con el glifo del encoder
  arriba (escala 2) y el texto abajo: Confirmar, Volver al soltar, Desplazar
  selección. El texto general queda en su propio párrafo. Los glifos son los
  bitmaps OLED existentes; no cambian píxeles ni navegación del firmware.

## Adaptación por archivo

Web/src: `app/App.tsx`, `app/navigation.ts`, `features/developer/DeveloperPage.tsx`,
`features/developer/ScreenTreeMap.tsx` (`openLabel`), `features/docs/DocsPage.tsx`,
`features/oled/ScreenCatalogBrowser.tsx`, `features/oled/FirmwareLibraryPreview.tsx`,
`styles/oled-catalogue.css` y `styles/index.css`. Borrados: `UiSimulator.tsx`,
`UiSimulatorPage.tsx` y `styles/ui-simulator.css`. Contracts: CHANGELOG.md y
README.md; Web/README.md y `docs/ui-navigation-20261101.json`.

## Estado por repo

- **atmega / esp:** sin cambios.
- **web:** fuente modificada. **Verificado en el navegador local** (sin robot):
  - a 1400 px, la vista previa queda a la izquierda (x 140–897) y la lista a la
    derecha (x 921–1353);
  - en la leyenda, el icono queda arriba del texto en los tres gestos;
  - «Ver árbol de pantallas» → árbol, y «Ver en la lista» → Wi-Fi 0x03001f
    seleccionada;
  - la redirección del Simulador UI funciona;
  - a 360 px no hay desplazamiento horizontal en Ayuda → OLED (lista y árbol)
    ni en Desarrollador.

  No verificado: compilación TypeScript, `build:esp` ni hardware.
- Sin commit local ni remoto.
