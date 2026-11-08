# AFR2-20261108-02 - Pruebas/Calibración, radar, Sí/No, odometría, Hall 370 y versión 1.0.0

Pedido del usuario tras probar el firmware del 08/11. Solo fuente: sin compilar, cargar ni probar en el auto.

## Correcciones

- **Sí/No sin giro** (HC-SR04/radar con cambios pendientes y cualquier diálogo Sí/No): el nodo `AFR2_N_SAVE_CONFIRM`
  cuenta 1 fila en el catálogo y `render_screen` reajustaba `selection` a 0 en cada cuadro, por lo que el clic llegaba
  pero el giro se deshacía. `Afr2Ui_MenuNode` ahora devuelve 2 filas para ese nodo (y la Web igual).
- **Radar sin graficar en Pruebas**: con el servo apagado `latest.pulse_us` es 0 y el eco se descartaba. Ahora se dibuja el
  último eco válido de hasta 1 s, con su ángulo si el servo estaba activo o de frente (90) si no. En rastro, el barrido
  sigue acumulando `bins[]`. Portado a `firmwareSensorRenderer.ts`.
- **Ícono de pulsación larga**: se restauran byte a byte los datos con la L del XBM del usuario (AFR2-20261030-17);
  AFR2-20261107-52 los había reemplazado. C y `encoderLongIcon.ts`.
- **QR (conectar AP / abrir Web)**: no se encontró por lectura un camino que reabra la pantalla. Se reforzó la salida:
  clic o largo limpia avisos y edición, vuelve a `AFR2_N_WIFI` por `back()` y fuerza repintado; `Afr2Qr_Invalidate()`
  anula la caché de cuadro al entrar y al salir. **Sin reproducir ni probar en hardware**; si persiste, revisar el log
  USART0 (líneas de entrada del encoder) para ver si llega el largo.

## Cambios de menú

- Pruebas ya no tiene "Hall una vuelta" (es calibración). Queda: Motor individual, Prueba Hall, Línea cinco, HC-SR04,
  Prueba encoder, Prueba ESP, Servo, Volver (8 filas). Calibración > Hall conserva Estado / Calibrar motor / Volver;
  su retorno de reserva pasa a Calibración.
- Calibración > **Odometría** (antes "Ver odometría", una vista vacía sin pose) es el nodo `AFR2_N_ODOM_MENU` (71, código
  0x030048): Estado (vista 0x030010) / Dirección / Distancia cm / Iniciar|Detener / Volver.
  - Dirección 0..7 con los códigos de `AFR2_A_MOVE` (Adelante, Atrás, Izquierda, Derecha, 4 diagonales); el editor muestra
    el nombre. Distancia 5..100 cm en pasos de 5. El botón del editor dice "Guardar"; la fila "Iniciar" ejecuta.
  - Iniciar = GOTO relativo (X adelante, Y izquierda; diagonales 0,7071 por eje), con los permisos de GOTO: motores
    habilitados, Hall y geometría, pose válida, sin radar. Largo vuelve atrás como siempre; no se inicia desde la Web.
  - Acciones nuevas `AFR2_A_ODOM_DIR=99`, `AFR2_A_ODOM_CM=100`, `AFR2_A_ODOM_TOGGLE=101`. Valores iniciales: Adelante, 50 cm.

## Hall

- `AFR2_DEFAULT_CPR` = 370 pulsos por vuelta en las 4 ruedas: valor de fábrica y migración en RAM de registros EEPROM con
  rueda en 0 (sin autoguardar). Sigue editable en Calibración > Hall. `hall_cw_mask` no cambia: la convención CW por rueda
  sigue siendo de calibración.
- **Pendiente**: una medición Hall interrumpida (se guarda 0 mientras mide) se recupera como 370 al reiniciar.

## Versión 1.0.0

`afr2_build_info.h` (ATmega y ESP) pasa de 0.11.0 a 1.0.0 (`MAJOR 1`, `MINOR 0`, `PATCH 0`; cambia el byte de versión del
boot) y Web de 0.4.0 a 1.0.0 (`package.json`, `package-lock.json`). El Loader sigue en 2.0.0 (versión propia, mayor).
`EEPROM v10` y `EX01` no cambian.

## Web (mismos cambios)

`firmwareRenderer.ts`: Pruebas 8 filas, SAVE_CONFIRM 2 filas, Odometría (5 filas), editor con nombres de dirección y
"Guardar", retorno de Hall; `firmwareSensorRenderer.ts`: eco del radar; `encoderLongIcon.ts`; `reformCatalogue.ts`;
`firmwareLibrary.ts` regenerado con `sync-oled-library.mjs --copy-only` (sin validadores); hashes de
`firmwareRenderer.sources.json` actualizados después de portar el comportamiento. `screens.json`: 0x030048, Pruebas 8 filas,
Hall una vuelta hija de Calibración. El simulador del desarrollador no modela la acción 101.

## Estado

atmega=fuente editada, sin compilar ni cargar; esp=solo versión (sin compilar); web=fuente editada, sin build ni navegador.
Presupuesto de SRAM/flash sin medir (+2 bytes en `Afr2App`, tabla `odom[]` en flash).
