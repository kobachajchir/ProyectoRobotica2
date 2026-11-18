# AFR2-20261118-01 - Seguidor de línea: sin línea se queda quieto

Autoría: Koba Chajchir / AFR2.
Versión: 1.39.6 (subversión mínima).

Estados:
- atmega = fuente editada, sin compilar ni probar en hardware;
- esp = solo versión;
- web = solo versión.

## Cambio

- fr2_product.c: en AFR2_LINE, con line_bits == 0 se llama a Afr2Product_Stop de inmediato (aviso 2); ya no se sigue girando 250 ms con el último error.
- Afr2Product_StartLine devuelve AFR2_UNAVAILABLE si ningún sensor ve la cinta, así que no arranca ni se reanuda sin verla. El modo Seguir línea se conserva (el auto queda en el modo, detenido).

## Pendiente

- Compilar y probar en el auto: frenado al salir de la cinta y rechazo de «Iniciar movimiento» sin línea.
