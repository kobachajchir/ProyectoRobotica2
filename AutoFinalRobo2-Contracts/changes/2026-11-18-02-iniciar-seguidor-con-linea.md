# AFR2-20261118-02 - Iniciar seguidor solo con línea a la vista

Autoría: Koba Chajchir / AFR2.
Versión: 1.39.7 (subversión mínima).

Estados:
- atmega = fuente editada, sin compilar ni probar;
- esp = solo versión;
- web = fuente editada, sin compilar ni probar.

## Cambio

- Web LineFollowerPanel.tsx: canPressStart = canStartLine && línea vista; «Iniciar movimiento» deshabilitado con aviso «Sin línea a la vista». No se restringe el resto del modo ni Ajuste y calibración.
- ATmega fr2_app.c: vailable() y eason() de AFR2_A_LINE_START/LINE_TOGGLE (al iniciar) exigen line_bits != 0; detener sigue permitido siempre.
- Se mantiene el frenado inmediato de AFR2-20261118-01.

## Pendiente

- Compilar y probar en el auto.
