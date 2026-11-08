# AFR2-20261108-03 - Pantalla «Prueba en curso» de la odometría

Pedido del usuario: una pantalla que muestre la odometría actual y el objetivo mientras la prueba de distancia
(AFR2-20261108-02) se mueve y que, al llegar, salga y muestre una notificación. Solo fuente: sin compilar, cargar ni probar.

## Comportamiento

- Calibración > Odometría > **Iniciar** inicia el GOTO relativo y abre `AFR2_N_ODOM_RUN` (nodo 72, código 0x030049).
  Si el inicio es rechazado, se queda en la lista y muestra el motivo de siempre.
- La vista (refresco 100 ms) muestra tres líneas: `Act X.. Y..` (pose estimada en mm), `Obj X.. Y..` (objetivo absoluto de
  la misión) y `Falta N mm` (distancia en línea recta al objetivo). Es la odometría por ruedas (Hall), no una medición externa.
- Clic corto o largo al soltar **detienen** la prueba y vuelven a la lista (el Volver explícito usa el mismo camino).
- Al terminar, la app saca la vista sola (`Afr2Ui_Leave`, mismo retorno que Volver) y avisa:
  - llegó (`goto_state` 2): «Llegó al objetivo» con `X.. Y..` final, 4 s;
  - tiempo agotado (`goto_state` 4, 60 s): «Odometria / No llego (60 s)»;
  - cualquier otra causa (STOP, otro modo, comando de la Web): «Odometria / Interrumpido».
  El aviso genérico «Operación lista» de GOTO se descarta para no duplicar. Si hay un aviso en pantalla, la salida espera a que cierre.

## Archivos

ATmega: `afr2_menu_catalog.h/.c` (nodo 72), `afr2_screens.c/.h` (`Afr2Ui_Leave`, apertura al iniciar, parada al salir),
`afr2_app.c` (líneas de la vista, salida y aviso, refresco). Web: `firmwareRenderer.ts` (el nodo virtual del selector de
motor pasa de 72 a 73), `firmwareLibrary.ts` regenerado con `sync-oled-library.mjs --copy-only`, hashes de
`firmwareRenderer.sources.json` actualizados. Contracts: `screens.json` (0x030049), CHANGELOG.

## Estado y límites

atmega=fuente editada, sin compilar ni cargar; web=fuente editada, sin build ni navegador (la vista local no inventa
telemetría: sin captura del auto no muestra valores); esp=sin cambios. SRAM/flash sin medir. El simulador del desarrollador no modela esta pantalla.
