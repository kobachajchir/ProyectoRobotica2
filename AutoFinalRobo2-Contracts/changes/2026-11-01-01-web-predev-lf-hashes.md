# AFR2-20261101-01 — Hashes OLED normalizados para predev

El usuario informa que `npm run dev` aborta en `sync-oled-library.mjs:87`
por `atmega:src/ui/afr2_screens.c` tras integrar origin/main.

Cuatro entradas de `firmwareRenderer.sources.json` coinciden exactamente con
el SHA-256 de los bytes actuales CRLF de los archivos AVR. El generador
normaliza CRLF a LF antes de calcular SHA-256. La diferencia corresponde a
la representación del texto, sin un cambio adicional de comportamiento.

Se revisaron las contrapartes existentes: dashboard, selector de motor y hints
en `firmwareRenderer.ts`/`motorSelector.ts`/`firmwareHints.ts`, iconos en
`encoderLongIcon.ts`/`reformResources.ts` y catálogo en los resolvers Web.
Se corrigen exclusivamente las cuatro firmas a la representación LF que exige
el generador; las otras cinco firmas ya coinciden con esa representación.

| Fuente AVR | SHA-256 UTF-8 normalizado a LF |
| --- | --- |
| src/ui/afr2_screens.c | 12ea38d77c1c1bd595ab5b8a5e04a888e1d5e0ec834c6402f881d81323ec818c |
| src/ui/afr2_icons.c | cc35885e3b499770dc948ac06327d1ca48dbc08e2c24e46610791e8d6ca22c63 |
| include/afr2_screens.h | 70a52f553698fbaaa846e97a84fda0a6bda61568ea2356de8ce213d0a1766319 |
| src/ui/afr2_menu_catalog.c | d363aef9f1d5d8f646a476d46d1d885bf6f8f759b8504a06e64f9520dfb87683 |

No se cambia el generador, no se usa `--copy-only` ni se desactiva el guard.
Se conservan cambios locales previos y autorías. ScreenCodes, recursos,
navegación, protocolo y presupuesto de firmware sin cambios por esta ficha.

ATmega/ESP: sin modificaciones. Web: registro corregido en fuente.
El próximo `npm run dev` del usuario volverá a generar la biblioteca con
la protección activa. No se ejecutaron npm, generadores, Vite, tests,
compilaciones, validadores, navegador, promoción ni cargas.
El arranque de Vite y la paridad visual/física no se dan por comprobados.

Procedencia: [registro por archivo](../provenance/web-predev-lf-hashes-20261101-01.json).
