# AFR2-20261103-31 — Compilación de la Web, bundle y peso

03/11/2026. Pedido humano: commit y push, compilar, crear el bundle, medir cuánto pesa y refrescar los valores del peso de la Web en Infraestructura.

## Compilación (`npm run build:esp`, Web 0.4.0)
`tsc -b` no compilaba: era la primera compilación completa desde varias entregas. Correcciones mínimas:
- `components/ui/UiIcon.tsx`: `UiIconSize` pasa de `16 | 20 | 24` a incluir 14, 18, 22, 26, 28, 40 y 56, que ya usaban las portadas, mapas y estados vacíos de Infraestructura, Loader, Conexión y Ayuda. Los tamaños del sistema siguen siendo 16, 20 y 24.
- `features/oled/vendor/qrcodegen.ts`: los dos `namespace qrcodegen.QrCode` y `qrcodegen.QrSegment` pasan a `export namespace`, porque TypeScript exige que todas las declaraciones fusionadas del espacio de nombres se exporten (17 errores). Es el único cambio al código vendido; el resto queda igual.
- `features/collector/CollectorScene.tsx` y `features/control/ControlPage.tsx`: se quitan una importación y una variable sin uso.

Resultado: `tsc -b` sin errores y `vite build --mode esp` completo (`dist/` con 17 archivos más `.afr2-build.json`).

## Peso medido (gzip nivel 9, mismo método que `package-web.mjs`)
| Concepto | Valor |
| --- | --- |
| Archivos | 17 |
| Antes de gzip | 4.656.048 B (4,44 MiB) |
| Guardado .gz | 2.477.870 B (2,36 MiB), 53,2 % del original |
| Estimación LittleFS (bloques de 8 KiB + reserva de 64 KiB) | 2.560 KiB de 2.800 KiB de presupuesto (91,4 %) |
| Mayores | `index.html` 2,29 MiB → 627 KiB · `AutoFinalRobo2.glb` 860 KiB → 718 KiB · `AFR2_Brazo_v17_Blanco.glb` 763 KiB → 674 KiB · `Logo-robot-1024.webp` 152 KiB |
Referencia anterior en `ESP/data/dist/manifest.json` (Web 0.3.0, 27/10/2026): 11 archivos, 4.112.446 B → 2.259.868 B.

## Bloqueo: el paquete ESP no se genera
`tools/package-web.mjs` aborta con «Storage del ESP admite hasta 16 activos Web»: el firmware ESP tiene `WebAsset assets_[16]` (`storage.h`) y el manifiesto del contrato admite hasta 16 archivos. La Web ya tiene 17; el archivo 17.º aparece con las imágenes del banner de Inicio (`home-hero-1600.webp` y `home-hero-mobile-720.webp`, AFR2-20261102-01) y las de los modelos.
- No se generó `ESP/data/dist` ni su `manifest.json`; siguen los del 27/10/2026.
- Infraestructura lee el peso del manifiesto (`/api/web-manifest`, o el local preparado en desarrollo), no de números escritos en la página; por eso no hay valores que editar a mano. Con el paquete anterior seguirá mostrando 0.3.0 hasta regenerarlo. Los tamaños de modelos salen de `generated` por `catalog-model-assets.mjs` y ya estaban al día.
- Para destrabarlo hay que elegir: quitar o fusionar un archivo de la Web (por ejemplo, incrustar el favicon o retirar un tamaño de imagen) o ampliar la tabla del firmware ESP y su contrato. No se decidió aquí.

## Estado por repo
- **atmega / loader:** sin cambios.
- **esp:** sin cambios; el límite de 16 activos es del firmware.
- **web:** compila y empaqueta hasta `dist/`; el paquete LittleFS está pendiente del bloqueo anterior. No verificado: `build` completo (perfil `full`), carga a la placa ni comportamiento con robot.
