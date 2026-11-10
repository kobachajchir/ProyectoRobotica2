# AFR2-20261110-16 · Envío del recorrido GOTO, giros de 90°, barra lateral táctil y pill de modo

Versión: 1.23.0 (subversión). Web fuente; ATmega/ESP solo versión. Sin cambios de protocolo ni CMD.

## Pedidos del usuario

1. GOTO: botón de envío en el panel lateral del simulador, no abajo; armar el recorrido y, si se cumplen las condiciones, enviarlo entero.
2. CW/CCW: flecha al costado que abre un panel flotante con giros de 90°, habilitados solo con odometría calibrada. También se abre con toque largo.
3. Control táctil: barra lateral vertical con iconos cuadrados redondeados. Información, Parámetros (velocidad de movimiento y de giro) y Giros abren paneles flotantes sobre fondo con blur. Hall es un interruptor que se resalta con el acento cuando está activo. Botones transparentes salvo los seleccionados, con animaciones. Aviso abajo a la derecha al activar o desactivar Hall.
4. Modales con overlay con blur.
5. Pill con el modo actual visible en toda la interfaz.
6. Armado: quitar "Etapas y opciones del recorrido" y toda la sección "Del diseño al movimiento"; los enlaces Explorar el conjunto / Ir a simulación pasan al lado de "Del diseño a la acción".

## Implementación (Web)

- `features/control/goto/useGotoRouteRunner.ts` (nuevo): convierte el plan del simulador en tramos rectos relativos.
  - Arcos y curvas se envían como su polilínea muestreada, un tramo cada ≥150 mm. Se descartan tramos de <20 mm y se parten los de más de 5000 mm, límite del firmware.
  - Ejecuta `robot.goto.start` (θ=0, rumbo fijo como el simulador) por tramo y consulta `robot.state` cada 250 ms hasta `gotoState=completed`.
  - Corta con fault, `cancelled`, `failed`, STOP o 65 s por tramo. "Cancelar envío" usa `robot.goto.cancel`.
  - El auto se detiene brevemente en cada vértice; no hay misión multipunto en el firmware.
- `GotoSimulatorPanel.tsx`: bloque "Auto" en el panel lateral (Enviar recorrido / Cancelar envío, tramos, velocidad, motivo si está bloqueado). `ControlPage.tsx` quita el formulario inferior de objetivo único y define `gotoBlockedReason`, compartido con los giros.
- `components/robot/TurnPresets.tsx` (nuevo): giros de 90° y 180° por lado (|θ| ≤ 3140 mrad del firmware, así que 270° no existe como giro único) y `FloatingSheet`.
  - El panel flotante va sobre un scrim con blur, en modo portal (anclado) o contenido dentro de un `<dialog>`.
- `MecanumDrivePad.tsx`: flecha junto a CW/CCW. El toque largo usa el evento `contextmenu` (clic derecho en escritorio): corta el giro mantenido y abre los giros, solo si están disponibles.
- `ManualDriveOverlay.tsx`: barra lateral en lugar de los paneles plegables. Teclado y gestos se suspenden con un panel abierto, y abrirlo cancela el gesto en curso. El aviso Hall es local: el toaster global queda bajo la top layer del modal.
- `FloatingModal.tsx` + `styles/workspace.css`: scrim real con blur dentro del dialog (`::backdrop` transparente). La pulsación en el scrim sigue cerrando.
- `features/connection/RobotModePill.tsx` (nuevo) + `useRobotModeState` en `useRobotMode.ts`: pill en la cabecera (también en celular), con tono por modo. Reemplaza el texto "Modo · …".
- `features/assembly/AssemblyPage.tsx`: quitadas las opciones/etapas y el cierre. Los enlaces aparecen junto al aviso final. Movimiento reducido sigue la preferencia del sistema.
- Estilos: `styles/touch-rail.css` (nuevo, importado en `index.css`) y `styles/assembly.css`.

## Revisión de hashes del renderer OLED

`npm run dev` se detenía en `tools/sync-oled-library.mjs` con `afr2_oled.c`, `afr2_screens.c` y `afr2_icons.c` distintos de `firmwareRenderer.sources.json`.

- `FillRect` (máscara por página) e `Icon_Draw` (paso por suma) dibujan los mismos píxeles que antes. El escalado de iconos y la rotación táctil ya estaban portados en `firmwareRenderer.ts`.
- Lo nuevo de `afr2_screens.c` es navegación (historial sin duplicados, padre fijo de Wi-Fi, teclado, salidas de avisos). Ya está en `uiSimulation.ts`, salvo el aviso al elegir modo Wi-Fi, que ahora coincide con el firmware ("Cambiando Wi-Fi · a STA, reinicia").
- Hashes actualizados. Con autorización del usuario se ejecutaron `predev` (catálogo y sincronización OLED, OK) y `tsc --noEmit` (OK).

## Corrección: joystick sobre el modelo y barra clickeable

- La capa táctil (`.afr2-manual-input`, z-index 5) cubría todo el escenario y, sin contexto de apilamiento propio, quedaba encima de la barra lateral (z-index 4), que no recibía clics. La barra pasa a z-index 12, los paneles flotantes a 20 y el aviso a 14.
- La capa táctil ocupa ahora solo un área centrada del tamaño del modelo (`manual-drive.css`). El arrastre empieza sobre el auto y puede seguir por el resto del escenario.
- El joystick se dibuja y se limita en coordenadas del escenario (`useManualDrivePointers.ts`). El cursor de arrastre aparece solo sobre el auto y los textos de ayuda se actualizaron.

## Ajustes visuales pedidos

- Barra lateral: columna fija con elementos centrados. Seleccionado = solo el cuadro del icono (acento con brillo) y el texto; sin fondo en todo el botón.
- Información, Parámetros y Giros se abren al lado de la barra, a la altura del botón pulsado, y no centrados. En celular siguen como hoja inferior.
- CW/CCW: flecha chica (14 px) dentro del borde derecho del botón. Abre un div flotante sin scrim (`variant="popover"` de `FloatingSheet`) que se cierra al tocar fuera o con Escape.

## Parámetros táctiles y modo operativo en la pill

- Parámetros del control táctil usa `RangeNumberField`, el mismo campo número + deslizador de Conducir (Potencia de traslación / Potencia de giro; en vista local, Velocidad simulada en mm/s).
- El selector "Modo operativo" (Reposo / Manual / Línea, `robot.mode`) sale de Conducir. La pill de modo de la cabecera es ahora un botón que abre un `FloatingModal` con ese selector, sus mismas condiciones (enlace listo, STOP confirmado, capacidades) y activación manual. La pill muestra solo el modo informado por el robot.

## Estado

web: fuente modificada. atmega/esp: solo versión. Typecheck y predev ejecutados (OK); sin tests, build, navegador ni cargas. Pendiente probar en el auto el encadenado GOTO y los giros de 90° con odometría calibrada.
