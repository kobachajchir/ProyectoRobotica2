# AFR2-20261103-26 — Ayuda reescrita, código resaltado y cuarto de pinout

03/11/2026. Pedido humano:
- redactar mejor y con más explicación toda la Ayuda;
- resaltar en celeste archivos, rutas y comandos;
- tarjeta de cada módulo en 3/4 de contenido y 1/4 de pinout;
- separar los datos mezclados en un solo párrafo, con la misma metodología en todas las secciones.

Continúa AFR2-20261103-25.

## Cambios en Web/src
- `features/docs/helpContent.ts`: textos reescritos con una idea por viñeta. Lo que va entre comillas invertidas (`main.c`, `/dist`, `index.html`, pines, comandos, mensajes) se resalta. Datos que iban juntos (toolchain y bibliotecas del ESP, mapas de pines) pasan a viñetas separadas o a la tabla. Nuevo campo `pinout` por módulo; se quitan los campos `source`, que ya no se mostraban.
- `features/docs/HelpRich.tsx` (nuevo): convierte las comillas invertidas en `<code>` resaltado.
- `features/docs/DocsPage.tsx`: tarjeta con columna principal y panel de pinout; viñetas y tablas pasan por `HelpRich`.
- `styles/visual-language.css`: el enlace de retorno de las cabeceras (por ejemplo «Desarrollador» en Protocol Studio) toma la forma del botón vecino: borde, radio de 14 px, relleno 8×16 px, peso 600 y hover de acción.
- `styles/workspace.css`: tarjeta 3/4 + 1/4 por container query (apilada bajo 760 px), tokens `--help-code-*` claro/oscuro y estilo del panel.

## Panel del cuarto derecho
| Sección | Contenido |
| --- | --- |
| ATmega | Pinout: motores, Hall, sensores, panel y enlace con el ESP |
| ESP | Pinout: UART0, cableado al Mega, KEY/RST y alimentación |
| Web | Puntos de acceso: rutas, API, WebSocket y plazos del cliente |
| OLED | Pinout: OLED, encoder y conectores J1/J2 |

Los pines salen de `Contracts/hardware/PINOUT.md` y `WIFI_SHIELD.md`; son los del documento, no una comprobación del montaje físico.

## Estado por repo
- **atmega / esp / loader:** sin cambios.
- **web:** fuente modificada. Verificado en el navegador local: ATmega a 1440 px en oscuro y claro, ESP a 390 px sin desplazamiento horizontal. No verificado: compilación TypeScript ni las pestañas Web y OLED en celular.
- Sin commit.
