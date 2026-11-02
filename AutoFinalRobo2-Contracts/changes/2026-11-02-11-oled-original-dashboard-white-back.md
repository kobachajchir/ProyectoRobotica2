# AFR2-20261102-11 — Dashboard original, OLED blanca y Volver en submenús

Pedido humano: recuperar el dashboard anterior, permitir cabecera con fondo,
representar la OLED blanca y mostrar flecha anterior grande + texto Volver en
todos los submenús. Conservar las indicaciones de las demás pantallas.

- AVR `src/ui/afr2_screens.c` y Web `features/oled/firmwareRenderer.ts` recuperan
  las funciones del dashboard de `ca9a2ae`: icono de red, SSID/IP con scroll
  independiente, batería/voltaje, modo centrado y accesos Menú/Rápidas.
- La cabecera original de 24 px se invierte: fondo blanco con texto/iconos
  negros. Los redibujos parciales invierten solo las dos bandas de texto,
  conservando los iconos y la batería. El resto sigue blanco sobre negro.
- Filas Volver de menús: recurso anterior `AFR2_ICON_BACK` a16px en x3, texto
  en x23, padding4px y centros verticales alineados. La selección invertida y
  el retorno contextual con guardas se conservan. Las vistas informativas,
  QR, sensores y demás páginas mantienen sus ayudas de gestos.
- `OledCanvas128x64.tsx` usa blanco/negro en todos los modos: simulación,
  biblioteca y captura. Grilla, borde y rótulo OLED pasan a tonos blancos.
- Identidades, flujo del encoder, QR y transporte sin cambios. Reutiliza
  recursos existentes; copia de biblioteca con `--copy-only`, sin checks.

Estado: fuentes ATmega/Web y metadatos vinculados; ESP sin cambio/promoción.
Sin tests, compilación, validadores, navegador, flasheo, commit ni push.
Esta ficha reemplaza la composición dashboard y la fila Volver de05; el resto
de OLED_RUNTIME_QR_20261102.md conserva su alcance.
