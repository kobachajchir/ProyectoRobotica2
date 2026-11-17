# AFR2-20261117-02 — Registro de revisión OLED para el inicio Web

Fecha: 17/11/2026. Origen: errores aportados por el usuario al ejecutar `npm run dev` y, posteriormente, `build:esp` desde el Loader con Web 1.33.0.

## Antes/después

`predev` se detenía con `AVR renderer changed: atmega:src/ui/afr2_screens.c`. El registro `Web/src/features/oled/firmwareRenderer.sources.json` conservaba las huellas anteriores a AFR2-20261116-01, aunque sus adaptaciones Web ya estaban en las fuentes.

Se actualizan únicamente las cinco huellas del registro después de leer los cambios AVR y sus contrapartes existentes:

| Origen ATmega | Contraparte Web revisada en fuente |
|---|---|
| `src/ui/afr2_screens.c` | `firmwareRenderer.ts`: selector `0x03005B`, textos/iconos/coordenadas e inversión de selección; formato decimal de compensación por motor. `developer/uiSimulation.ts`: navegación del selector y códigos de edición `0x030057..0x03005A`. |
| `src/ui/afr2_network_screen.c` | `firmwareRenderer.ts`: aviso `AFR2_NET_SYNC_FAIL_5` (`0x04000D`), icono de advertencia y líneas de texto. |
| `src/ui/afr2_menu_catalog.c` | `firmwareRenderer.ts`: cuatro ajustes `FL/FR/RL/RR`, acciones `121..124`, rango `500..1500`, paso `5` y retorno; nodo de sincronización registrado en el catálogo compartido. |
| `src/ui/afr2_dashboard.c` | `dashboardRenderer.ts`: icono `AFR2_ICON_LINK` en `(93,1)` de tamaño `12` y actualización de cabecera al cambiar `syncOnline`. |
| `include/afr2_screens.h` | `FirmwareSnapshot.syncOnline` en `firmwareRenderer.ts` y su consumo en `dashboardRenderer.ts`, correspondientes al campo AVR `Afr2UiSnapshot.sync_online`. |

La primera corrección omitió el header, cuyo cambio ya estaba en el commit `a01e29d` y no aparecía en el diff de trabajo. El registro del Loader aportado después mostró que `build:esp` se detenía en esa huella. La lectura del cambio del commit identifica únicamente la adición de `sync_online` al snapshot. Se incorpora su huella tras revisar la contraparte Web existente. Se calcularon las huellas normalizadas de las 17 entradas del registro para preparar la corrección completa; no se detectaron otras pendientes en esa lectura.

Las huellas SHA-256 se calculan sobre texto UTF-8 con CRLF normalizado a LF, igual que el generador. Se conserva la comprobación estricta de `sync-oled-library.mjs`; no se agrega `--copy-only` a los scripts ni se desactiva el bloqueo ante cambios futuros.

## Estado por módulo y versión

- Web: corregidos metadatos de revisión del generador; fuentes de dibujo y simulador existentes conservadas. El próximo inicio solicitado por el usuario regenerará los recursos mediante el `predev` habitual.
- ATmega/ESP: sin modificaciones por esta ficha. Sin cambios de CMD, transporte, ScreenCode ni recursos gráficos.
- Versión: se conserva **1.33.0**; esta ficha solo corrige el registro de revisión, sin cambios de código cargable.
- No se ejecutaron generadores, inicio Web, tests, compilaciones, validadores, navegador ni flasheos. Arranque y paridad visual/física sin comprobar.
- El registro aportado por el usuario informa compilación del firmware ESP completada y fallo posterior de preparación Web; no acredita una nueva imagen FileSystem completada ni una carga.
- Sin commit ni publicación por esta ficha; se preservan los cambios previos del árbol de trabajo.
