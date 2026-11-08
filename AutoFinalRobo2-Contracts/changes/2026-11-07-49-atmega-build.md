# AFR2-20261107-49 — Compilación del firmware ATmega con los cambios del 07/11

07/11/2026. Pedido del usuario: compilar el firmware del ATmega para revisar errores y memoria.

Estado real: atmega = compilado, sin cargar; esp/web = sin cambios en esta entrega. No se abrieron puertos COM ni se flasheó. Sin pruebas físicas.

## Cómo se compiló

En esta PC no hay `make` en el PATH. Se replicó el target `firmware` del `Makefile` con avr-gcc 7.3.0 (`Arduino15/packages/arduino/tools/avr-gcc/7.3.0-atmel3.6.1-arduino7`): cada fuente de `SOURCES` más `src/main.c` se compiló a `.o` con los mismos `CPPFLAGS`/`CFLAGS` (sin `-fstack-usage` en `afr2_watchdog.c`), se enlazó con `-Wl,--gc-sections,-Map,build/afr2.map -lm` y se generó el HEX con `avr-objcopy -O ihex -R .eeprom`.

## Resultado

- Sin errores. Un aviso preexistente de `-Wmisleading-indentation` en `src/services/afr2_product.c:204` (función `odometry`): es solo de estilo, porque los `if` encadenados en una línea hacen lo que se espera.
- Flash: 132 054 B (50,4 %).
- SRAM estática: 6 329 B (77,3 %), con 1 863 B libres para la pila. AFR2-20261107-40 registraba 6 315 B.
- Marcos de pila más grandes: `main` 305 B, `invoke` 158 B, `Afr2Ui_Render` 153 B, `Afr2Link_Push` 141 B. Camino profundo estimado de unos 900 B, sin medición en ejecución.
- `build/afr2.hex`: 371 457 B, SHA-256 `167cc56454263f54d84f068b2dc09b0ef1eef3c92dc0d94549578b65eeaea346`.

Corrección (AFR2-20261107-51): el archivo actual, cargado y verificado, tiene SHA-256 `16826727c1109e86119f98a5bf6fc1b7d3896328703fe6e0ef8d134d7b7ca0a4`.

## Cambios incluidos (fuente del working tree)

AFR2-20261107-41 (signo lateral), -43 (IR digitales y sentido del seguimiento), -46 (refresco OLED liviano y repaso a 250 ms), -47 (pantalla inicializada al final y configuración de confirmación) y -48 (clic sobre notificaciones de red). Con esta compilación, el estado atmega de esas fichas pasa a «compilado, sin cargar».

## Pendiente

Cargar en el Mega con el Loader y probar en el auto: arranques en frío y en caliente de la OLED, notificaciones con la Web abierta, izquierda/derecha y seguimiento de línea.
