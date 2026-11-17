# AFR2-20261117-06 - Encoder con botones redondos bajo la pantalla, pills que se estiran y captura del Loader

Autoría: Koba Chajchir / AFR2.
Versión: 1.35.1 (subversión mínima).

Estados:
- atmega = solo versión, compilado;
- esp = solo versión, compilado con LittleFS;
- web = compilada y publicada en Hosting.

Sin CMD nuevo ni cambio de contrato. Sin cargar.

## Pedido

1. **Pantalla en vivo y bancos OLED:**
   - los botones del encoder quedan solo con icono, circulares, debajo de la pantalla;
   - giros juntos a la izquierda y SW en el extremo derecho, alineados con el principio y el fin de la pantalla;
   - en hover, fondo de acción y color de frente invertido, como los demás botones;
   - debajo, la lista de referencia con los iconos y qué hace cada control.
2. **Barra superior:**
   - las pills conservan el icono y sin hover ni modal abierto son redondas, sin texto, como la de red;
   - en hover se estiran con animación y muestran el texto, y al salir se achican;
   - en hover, fondo de acción y color de frente invertido.
3. **Loader:** la captura de pantalla no se mostraba en la Web publicada.

## Cambios (Web)

- `src/features/oled/EncoderControl.tsx`:
  - Fila de botones circulares (`afr2-encoder-round`) con los mismos gestos de antes:
    - giro al hacer clic;
    - SW corto o largo al soltar, con barra de progreso, captura de puntero y cancelación;
    - teclado ← → y Espacio.
  - Prop nueva `buttonsHost`: dibuja la fila mediante un portal en un nodo bajo la pantalla; sin ella va dentro de la tarjeta.
  - La tarjeta queda con el título, la leyenda `afr2-encoder-legend` y los avisos de estado:
    - la leyenda tiene giro izquierdo, giro derecho, corto y largo con su umbral, y las teclas cuando el diálogo captura el teclado;
    - la fila del control que se está usando se resalta.
- `src/features/oled/OledWorkbench.tsx`: columna de pantalla `afr2-oled-bench__screen-col` con el nodo `afr2-oled-bench__buttons` del mismo ancho que el lienzo. Vale para Pantalla en vivo, la biblioteca y la simulación de Ayuda → OLED.
- `src/styles/workspace.css`: estilos de la fila, los botones redondos y la leyenda.
  - Los botones miden 56 px y el SW 64 px.
  - Estados idle, hover, pressed, long y disabled; respeta movimiento reducido.
- `src/styles/topbar-pills.css` (nuevo, último en `index.css`), para Sync web, Modo, Red, Batería y Alertas:
  - redondas de 44 px solo con el icono;
  - con `:hover`, `:focus-visible` o `aria-expanded="true"` se estiran (transición de `max-width`, margen y opacidad del texto) y toman `--action` / `--text-on-action`;
  - el número de alertas queda como insignia sobre el icono y se oculta sin avisos.
- `NetworkConnection.tsx`, `BatteryStatus.tsx`, `CloudSyncController.tsx`, `RouteNav.tsx`: las pills de Red, Batería y Sync reciben `open` y exponen `aria-expanded`, así quedan estiradas mientras su modal está abierto. Modo y Alertas ya lo tenían.
- **Loader:** `loader-inicio-*.webp` nunca entraba en la compilación (`copyPublicDir: false`).
  - `vite.config.ts` incluye `brand/loader-inicio-1206.webp` solo en el perfil completo (Hosting).
  - `LoaderPage.tsx` usa esa única resolución y no dibuja la figura en el perfil `esp`.
  - El paquete ESP está a un bloque LittleFS del presupuesto (la imagen ocupa entre 5 y 10 bloques) y el Storage admite 24 activos. El perfil completo queda en 24.

## Estado de validación

- Revisado en el servidor de desarrollo de Vite, en Ayuda → OLED:
  - los botones circulares quedan bajo la pantalla, alineados con el marco;
  - el hover pone fondo de acción y cambia el color del icono;
  - la leyenda queda debajo.
- Barra superior: las pills son redondas, «Sin datos» se estira en hover y la insignia de alertas queda visible.
- A 375 px, sin desplazamiento horizontal ni errores en la consola.

### Compilación y publicación (17/11/2026, autorizadas por el usuario)

| Módulo | Resultado |
|---|---|
| ATmega | `make -B firmware communication-firmware library`: exit 0. Flash 169 062 B (.text 168 708 + .data 354); SRAM estática 7 202 B. Una advertencia previa ajena: `afr2_product.c:262` |
| Web ESP | `npm run build:esp` (con `tsc`): exit 0. 23 activos; estimación FS 2 859 008 de 2 867 200 B. Promovida a `ESP/data/dist` |
| ESP | `platformio run`: SUCCESS, RAM 58,6 %, flash 45,9 %. `-t buildfs`: SUCCESS |
| Web completa | `npm run build`: exit 0, 24 activos (con la captura del Loader) |
| Hosting | Publicado en https://autofinalrobo2.web.app |

En Hosting se comprobó que:
- la captura del Loader carga (1206 px de ancho);
- las cinco pills de la barra miden 44×44 px.

La primera publicación mostraba pills de 48×44 px. Se corrigió el relleno (11 px) y se volvió a compilar y publicar.

No verificado: carga en el auto, Pantalla en vivo con el auto y tema claro.

## Pendiente

Cargar ATmega, ESP y LittleFS desde el Loader (1.35.1).
