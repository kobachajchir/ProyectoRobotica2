# AFR2-20261110-15 · F1: guarda de ruido en interrupciones y corte no enclavante

Versión: 1.22.1 (parche).

## Problema informado

Al mover el auto, sobre todo con el panel táctil de la Web (a veces con flechas), aparece "Fallo 1 · lazo de control demorado" y hay que limpiarlo. El usuario no ve las líneas `FAULT F1 …` / `FAULT stages …` en el log USB ni en la respuesta del ATmega. No se puede modificar el hardware.

## Diagnóstico (lectura de fuente, sin mediciones)

- **Por qué no aparecían los tiempos:** el `build/afr2.hex` (10/11 12:08) es anterior a las fuentes de 1.22.0 (`main.c` 12:24, `afr2_product.c` 12:30), que eran "solo fuente". Además, con TAP en modo distinto de 0 (Loader mirando tramas), `tap_allows()` descartaba toda línea que no fuera AVR/ESP/LINK/TAP, incluidas FAULT y LOOP slow.
- **Causa probable:** no se halló en el código del lazo principal ninguna espera bloqueante (el render táctil es de unos pocos ms; el log y el I2C no bloquean). El PWM de motores es fast PWM de 8 bits a unos 976 Hz. A 100 % (flechas) la salida queda fija y no conmuta; con potencias parciales (joystick táctil, diagonales, rampa de arranque) conmuta todo el tiempo. Hall (PCINT2) y encoder (INT4/INT5) usan solo pull-ups internas. Los flancos por acoplamiento son IRQ con prioridad sobre Timer0/UART/TWI y una ráfaga puede robar CPU al lazo más de 100 ms. Es una hipótesis; no está medida.

## Cambios ATmega

1. **Guarda de tormentas de IRQ** (`src/platform/afr2_board.c`, `include/afr2_board.h`): presupuesto por tick de 1 ms de 64 IRQ Hall y 8 IRQ encoder. Agotado, la fuente se enmascara hasta el siguiente tick de Timer0, que la rearma desde el nivel actual de los pines (sin flanco fantasma). La captura Hall servida desde la ISR del encoder respeta el enmascaramiento. Las tasas reales quedan muy por debajo (Hall ~15 flancos/ms con 4 ruedas; encoder a mano <1/ms). En ventanas de ruido se pierden cuentas Hall, que en ese caso son mayormente ruido. Contadores saturados: `Afr2Board_NoiseCounts()`.
2. **F1 no enclavante si es aislado** (`src/services/afr2_product.c`, `include/afr2_product.h`), elegido por el usuario: el umbral de 100 ms y el corte inmediato de salidas se mantienen siempre. Un retraso aislado no deja `fault=1`: el próximo comando aceptado arranca de nuevo con rampa desde cero. Tres cortes dentro de 2 s (`AFR2_LATE_LATCH_COUNT`, `AFR2_LATE_WINDOW_MS`) enclavan F1 como antes y requieren limpieza explícita. `fault_late_count`, `fault_control_gap_ms` y los tiempos por etapa se registran en cada corte. `ClearFault` reinicia la ráfaga.
3. **Log** (`src/services/afr2_debug.c`): `FAULT`, `LOOP slow` y la nueva línea `NOISE irq storm hall+N enc+N (total h/e)`, como mucho 2 por segundo, pasan también en los modos TAP 1–4. La línea F1 ahora indica `cut` o `latched` y `n=` dentro de la ventana.

Sin cambios de trama ni de CMD: 0x56 FAULT conserva su layout. `late_count` cuenta ahora todos los cortes, enclavados o no. Memoria nominal: +5 bytes en `Afr2Product`, +6 bytes de estado de la guarda y +8 bytes en el estado del log.

## Estado

atmega: fuente modificada. esp: solo versión. web: solo versión (`package.json`/`package-lock.json`). No se ejecutaron tests, compilaciones, validaciones ni flasheos. Hay que compilar y cargar el ATmega para que cualquier cambio (y los tiempos de 1.22.0) llegue al auto. No se acredita la causa física ni su corrección.
