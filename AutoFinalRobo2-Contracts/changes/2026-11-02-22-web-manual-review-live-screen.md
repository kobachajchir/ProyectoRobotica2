# AFR2-20261102-22 — Revisión del manual en toda la Web y pantalla en vivo

02/11/2026. Pedido humano:
- revisar toda la interfaz con el manual de identidad visual vigente (0.1.11 draft), recorriendo todas las rutas del router;
- en la pantalla en vivo, agregar un selector entre actualización periódica y actualización solo cuando cambia;
- no esconder el lienzo mientras se adquiere una captura, porque la interfaz salta;
- acortar el texto bajo el pulsador del encoder y acompañarlo con los íconos de pulsación corta y larga.

Pedido posterior en la misma entrega:
- controles del encoder al lado de la pantalla en PC y debajo en celular;
- arriba, siempre visibles, la pill «Última entrada» y el botón «Actualizar pantalla» en el borde superior derecho;
- hover del botón con fondo y texto en buen contraste.

Continúa AFR2-20261102-15 (capa visual común) y AFR2-20261102-10 (portada clara/oscura de Infraestructura).

## Rutas revisadas

Todas las rutas de `app/App.tsx`:
- Inicio, Conducir, Programas, Puesta a punto y Sensores;
- Modelos y su inspección (`auto`, `brazo`), y Simulador (`brazo`, `conjunta`);
- Infraestructura, Conexión, Ayuda (Web y OLED), Desarrollador y Protocol Studio;
- el modal global Pantalla en vivo.

Las rutas de compatibilidad (`/control`, `/simulaciones`, `/brazo`, `/simulacion`, `/modelo-3d`, `/wifi`, `/oled`, `/documentacion` y `/desarrollador/ui-simulator`) redirigen a las anteriores.

## Cambios de identidad

- **Título de Inicio invisible:** el degradado del `h1` usaba `--banner-zone-*`, retirados en AFR2-20261102-10, y quedaba como texto transparente. Se reponen como tokens fijos del banner, con los valores de la columna «Banner fijo» de VIS-058 (`#7dd3fc`, `#fcd34d`, `#ff8a9a`, `#86efac`).
- **Cabeceras de página (PageHeader):**
  - Pasan de la base oscura fija a la familia `--infra-hero-*`, la misma de la portada de Infraestructura. En oscuro conservan el aspecto anterior; en claro usan papel frío con tinta oscura.
  - El ícono toma el color de zona del área: Sensores y Conexión en celeste, Modelos y Simulación en verde, Puesta a punto en ámbar, y el resto en rubí.
  - En el selector de la cabecera, la pista se tiñe levemente para que la opción elegida se distinga en claro.
- **Carga de rutas diferidas:** los avisos «Cargando…» pasan a una tarjeta de estado con spinner, que se detiene con movimiento reducido.

## Pantalla en vivo (`LiveOledViewer`)

- **Selector Periódica / Al cambiar,** que se recuerda en `localStorage` (`afr2-oled-refresh-mode`):
  - **Periódica:** pide `robot.screen.get` cada 1,2 s, como hasta ahora.
  - **Al cambiar:** pide la captura solo cuando `robot.screen.status`, que la Web ya lee una vez por segundo, informa otra pantalla, revisión, selección o ventana. Cada estado sin cambios confirma la captura dibujada, así que no caduca ni bloquea el encoder. Un cambio a mitad de una consulta provoca otra consulta. Los valores que cambian dentro de una misma pantalla (por ejemplo, el dashboard) no se refrescan en este modo.
- **El lienzo no desaparece:**
  - Al cambiar de pantalla, al consultar o al perder el enlace queda la última captura. Solo se olvida al cerrar el visor.
  - La caducidad se marca con contorno y atenuación, sin cambiar el tamaño.
  - Sin ninguna captura se muestra el mismo bisel 128×64 vacío, con «Sin captura» o «Consultando…».
- **Disposición:**
  - Barra superior fija con la pill «Última entrada» a la izquierda y «Actualizar pantalla» a la derecha.
  - El hover del botón pasa a fondo acción con texto `--text-on-action`: blanco sobre `#b81d33` en claro y `#190408` sobre `#ff6b7d` en oscuro.
  - Debajo de la barra van los enlaces y el selector, y luego la pantalla y el encoder: lado a lado desde 840 px, con el modal ampliado a 1120 px (`size="lg"`), y apilados en celular.
  - En una columna angosta, los dos giros quedan en fila y el pulsador SW debajo, a todo el ancho.
- **Encoder:**
  - Bajo el pulsador SW, una leyenda de dos filas con los íconos del firmware: «Corto — Inicio: menú · resto: confirma» y «Largo — Inicio: rápidas · resto: vuelve».
  - Al pulsar se ilumina la fila que corresponde. Se quitó el párrafo dinámico que aparecía y desaparecía.
  - La regla de navegación no cambia.

## Adaptación por archivo

Web/src:
- `features/oled/LiveOledViewer.tsx`, `features/oled/EncoderControl.tsx` y `features/oled/OledCanvas128x64.tsx`, que suma las props opcionales `badge` y `title`;
- `components/navigation/RouteNav.tsx` (modal `lg`) y `app/App.tsx` (clase de los avisos de carga);
- `styles/tokens.css`, `styles/visual-language.css` y `styles/workspace.css`.

Contracts: CHANGELOG.md y README.md.

## Estado por repo

- **atmega / esp:** sin cambios. No hay comandos nuevos: se usan `robot.screen.get` y `robot.screen.status`.
- **web:** fuente modificada.
  - **Verificado en el navegador local, sin robot:**
    - todas las rutas listadas, en oscuro y claro a 1440 px;
    - Simulador y Pantalla en vivo a 390 px, sin desplazamiento horizontal;
    - en Pantalla en vivo: cambio y persistencia del selector, hover del botón (habilitado solo en el navegador para probarlo), bisel vacío y disposición lado a lado y apilada;
    - consola sin errores.
  - **No verificado:** el modo «Al cambiar» con capturas y estados reales, la ausencia de saltos con el robot, la compilación TypeScript, `build:esp` y el hardware.
- Sin commit.
