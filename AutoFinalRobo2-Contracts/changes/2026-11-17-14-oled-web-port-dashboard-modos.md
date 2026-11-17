# AFR2-20261117-14 - Copia Web del OLED: dashboard y selector de modo (desbloquea `npm run dev`)

Autoría: Koba Chajchir / AFR2.
Versión: 1.39.2 (subversión mínima).

Estados:
- web = en fuente;
- atmega = solo versión;
- esp = solo versión.

Sin cambio de contrato.

## Pedido

`npm run dev` se detenía con `AVR renderer changed: atmega:src/ui/afr2_screens.c. Port/review firmwareRenderer.ts before building.`

## Causa

AFR2-20261117-10 (tarjeta rápida Sincronización) y AFR2-20261117-11 (Modo de robot / Seguir línea) cambiaron cuatro archivos del ATmega que la Web registra por huella sin haber actualizado la revisión:
- `src/ui/afr2_screens.c`;
- `src/ui/afr2_menu_catalog.c`;
- `include/afr2_dashboard.h`;
- `src/ui/afr2_dashboard.c`.

La tarjeta «Sincroniz.» ya estaba portada en `firmwareRenderer.ts`. Faltaba portar lo de -11.

## Port (Web)

- `src/features/oled/dashboardRenderer.ts`:
  - Nombres de modo iguales al AVR: «SEGUIR LINEA» e «IR», en lugar de «LINEA» y «GOTO».
  - El nombre se desplaza en su columna con el mismo cálculo de `offset(text, elapsed, width)`, ancho `DASH_MODE_W - 2` = 50 px, y se dibuja con `textWindow(2, 52, 40, …)`.
  - Se redibuja cuando cambia el desplazamiento (`modeOffset`).
- `src/features/oled/firmwareRenderer.ts`: selector de modo (Estado y tarjeta Modo):
  - etiqueta «Seguir linea»;
  - Seguir línea e Ir llevan la acción 80 (`AFR2_A_QUICK_MODE`) con arg 2 y 4, como `afr2_menu_catalog.c`.
- `src/features/developer/uiSimulation.ts`: la acción 80 con destino abre el control (Seguir línea o Ir), como `afr2_screens.c`. Sin destino avisa «Modo seleccionado» con Reposo, Manual, Seguir línea o Ir.
- `src/features/oled/firmwareRenderer.sources.json`: huellas de los cuatro archivos, actualizadas después del port.
- `generated/firmwareLibrary.ts`: regenerado por `sync-oled-library.mjs` (81 nodos AVR, 26 iconos).

## Estado de validación

- `node tools/sync-oled-library.mjs`: exit 0.
- `npm run dev` arranca (puerto 5173).
- Ayuda → OLED dibuja el dashboard («MODO REPOSO») sin errores en consola.
- No comparado píxel a píxel con el AVR: no se corrió la prueba de paridad.
- Sin compilar.
