# AFR2-20261117-11 - Modos reales (Reposo/Manual/Seguir línea/Ir), seguidor en filas y overlays con desenfoque

Autoría: Koba Chajchir / AFR2.
Versión: 1.38.0 (subversión).

Estados:
- atmega = fuente editada, sin compilar ni probar en hardware;
- esp = fuente editada, sin compilar ni probar;
- web = fuente editada, sin compilar ni probar.

## Pedido

1. «Seguidor de línea» y «Ajuste y calibración de línea» uno debajo del otro, con chevrones. «Iniciar movimiento» aparece con Seguir línea activo; al iniciar, Detener arriba y el botón de abajo pasa a Detener. Ancho de cinta en Ajuste y calibración. Pill «Auto detenido» centrada con overlay con fade. Overlays de modales con desenfoque. GOTO se muestra como «Ir».
2. Modos reales y persistentes: **Reposo** (ver sensores, sin movimiento), **Manual** (botones, panel táctil, secuencias), **Seguir línea** (Web y OLED) e **Ir** (antes GOTO). Se fijan desde el auto (OLED) y se reportan, o desde la Web y se aplican al auto.

## Contrato (UART)

`AFR2_EXT_MODE` (0x15): petición de 5 o 6 bytes `[reservado u32=0][modo u8][inicio u8 opcional]`, modos 0 Reposo, 1 Manual, 2 Seguir línea, 4 Ir. `inicio=1` solo es válido con modo 2 y arranca el seguimiento en el mismo mensaje. `robot.mode` acepta `idle|manual|line-follow|goto` y `start`. Sin cambio de EX01 (v13). `wire_catalog.json`, `commands.json` y Protocol Studio (catálogo, guiado, códec) actualizados.

## Cambios

- ATmega: `Afr2Product.selected_mode` persistente (sobrevive a STOP y fallos); `Afr2Product_SetMode`; conducción PWM/cerrado solo en Manual; `SEQ_START` solo en Manual; seguidor de línea requiere Seguir línea; rutas y GOTO requieren Ir (los giros en sitio x=y=0 se permiten en Manual); acciones rápidas de línea/Ir con aviso «Cambiar a…». OLED: menú «Modo de robot» con Reposo/Manual/Seguir linea/Ir; dashboard con nombres REPOSO/MANUAL/SEGUIR LINEA/IR y marquesina; textos GOTO→Ir.
- ESP: `broker_product.cpp` acepta `goto` y `start` en `robot.mode` y publica el estado al responder.
- Web: `robotModeSelect.ts` (selector real de modo), `useRobotMode.ts`, `RobotModePill`; `armedMode.ts` eliminado. Paneles Conducir, Secuencias, Pruebas mínimas, Ir y Seguidor muestran un banner con «Cambiar a «X»». Seguidor en dos tarjetas plegables, ancho de cinta compartido (`lineTape.tsx`), pill centrada con fade. `workspace.css`: `::backdrop` con desenfoque literal. GOTO→Ir en textos de usuario, simulación OLED y ayuda. Hashes de `firmwareRenderer.sources.json` vigentes.

## Pendiente

- Compilar ATmega/ESP/Web y probar en el auto (no se ejecutó nada).
- Ejecutar `tools/sync-oled-library.mjs` para regenerar `generated/firmwareLibrary.ts` (nombres de ítems/títulos nuevos).
- Probar en hardware: persistencia del modo tras STOP, cambio OLED↔Web, bloqueos fuera de modo y desenfoque del modal en el navegador.
- La versión 1.37.0 (AFR2-20261117-10) sigue sin confirmar; esta ficha sube sobre ella.
