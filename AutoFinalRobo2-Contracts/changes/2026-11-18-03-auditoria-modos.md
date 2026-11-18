# AFR2-20261118-03 - Auditoría de modos reales

Autoría: Koba Chajchir / AFR2.
Versión: 1.39.8 (subversión mínima).

Estados:
- atmega = fuente editada, sin compilar ni probar;
- esp = solo versión;
- web = fuente editada, sin compilar ni probar.

## Hallazgos corregidos

- Web ControlPage.tsx: obotMode !== 'manual' bloqueaba el pad mientras el auto reportaba manual-pwm; ahora inManual = manual o manual-pwm.
- Web SequenceEditorPanel.tsx: inManual incluye sequence, para no ofrecer «Cambiar a Manual» durante una secuencia.
- ATmega Afr2Product_SetMode: Reposo devuelve OK con fallo activo.

## Pendiente

- Compilar, correr sync-oled-library.mjs y probar en el auto.
