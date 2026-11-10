# AFR2-20261110-17 · Renders de los módulos en la Ayuda

Versión: 1.23.1 (subversión mínima). Web fuente; ATmega/ESP solo versión. Sin cambios de protocolo ni CMD.

## Pedido del usuario

Renders de Fusion de la ATmega2560, del ESP-12S en su shield y del panel OLED + encoder, y una imagen del Inicio de la Web, colocados en la tarjeta de cada módulo de Ayuda sin tapar texto, en formato liviano y revisados en varias resoluciones.

## Origen de las imágenes

- Documento Fusion `AutoFinalRobo2` (ensamble del auto usado por las láminas técnicas). Ocurrencias aisladas: `Mega2560_CAD:1`, `OSOYOOWifiShield:1`, `AFR2_UI_B2_Fondo_OLED15_Auto5:1`.
- Captura del visor de Fusion (sombreado, antialias, fondo transparente) con cámara en perspectiva; se ocultan el resto de las ocurrencias y los cuerpos de la raíz (placas del chasis) y luego se restaura la visibilidad. El documento no se guardó. Script: `AutoFinalRobo2-CAD/renders/ayuda-web/captura_fusion.py`.
- Inicio de la Web: captura del servidor de desarrollo a 1440×900, sin robot conectado (muestra «Sin conexión»).
- Recorte del margen transparente y conversión a WebP (calidad 80–82): `public/help/atmega2560.webp` 37 KB, `esp12s-shield.webp` 46 KB, `oled-encoder.webp` 42 KB, `web-inicio.webp` 36 KB.

## Implementación (Web)

- `features/docs/helpContent.ts`: campo opcional `visual` (`src`, `alt`, tamaño, `render`/`screen`) por módulo. La especificación «Versión» de Web toma `package.json` en lugar del fijo `0.4.0`.
- `features/docs/DocsPage.tsx`: `<figure>` dentro de la grilla del encabezado, en su propia celda; `loading="lazy"` y tamaño declarado para no mover el layout.
- `styles/workspace.css`: con la tarjeta ≥ 1200 px la imagen va en una tercera columna al costado del texto; por debajo queda entre la descripción y las especificaciones, centrada y con alto máximo. Los renders llevan sombra; la captura de Web, marco y leve perspectiva (sin perspectiva con movimiento reducido).
- `vite.config.ts`: las cuatro imágenes se suman a la lista explícita de assets del build (perfil ESP incluido, +161 KB sin comprimir).

## Estado

- Web: fuente. Revisado en el servidor de desarrollo a 1920, 1600, 1280, 900 y 375 px (sin scroll horizontal ni texto tapado). Sin tests, typecheck ni build.
- ATmega / ESP: solo versión compartida. Sin compilar ni cargar.
