# AFR2-20261110-18 · Paquete ESP dentro del presupuesto: logo de bienvenida a 768 px

Versión: 1.23.2 (parche). Web fuente; ATmega/ESP solo versión.

## Origen

`build:esp` pasó, pero `tools/package-web.mjs` rechazó el paquete con «Paquete ESP demasiado grande». Medido sobre el `dist` de 1.23.1, la estimación de `web-budget.mjs` era de 2.891.776 bytes contra un presupuesto de 2.867.200: 24.576 bytes de más, tres bloques de 8 KiB. El excedente apareció con las imágenes de Ayuda de AFR2-20261110-17.

## Cambio

- `Logo-robot-1024.webp` (155.902 bytes gzip, 20 bloques) solo se usa en la bienvenida, que lo muestra a 32rem como máximo (512 px CSS).
- Se genera `public/brand/Logo-robot-768.webp` desde el maestro `reference/brand/Logo-neon-robot-source.png`, con los mismos parámetros que los demás derivados (calidad 88, alpha 100, method 6, exact): 97.424 bytes, 12 bloques. Sigue siendo nítido a 512 px CSS en pantallas 1,5×.
- `BrandMark.tsx` (`AFR2_BRAND_LOGO`), `StartupWelcome.tsx` (768×576), `vite.config.ts` (lista de activos) y `tools/optimize-images.py` usan la versión de 768. `reference/brand/README.md` lo documenta.
- `public/brand/Logo-robot-1024.webp` queda en el repositorio sin referencias; no entra en ningún bundle (`copyPublicDir: false`).

## Medición

La estimación recalculada sobre el `dist` actual, sustituyendo solo el logo, es de 2.826.240 bytes: queda un margen de 40.960 bytes (5 bloques). No se recompiló: el `index.html` de 1.23.2 puede variar unos pocos bytes. Falta confirmar con un `build:esp` nuevo.

## Estado

web: fuente modificada. atmega/esp: solo versión. Typecheck ejecutado (OK); sin build, empaquetado ni cargas.
