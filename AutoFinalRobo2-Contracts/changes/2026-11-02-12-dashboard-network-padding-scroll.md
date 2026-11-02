# AFR2-20261102-12 — Margen de cabecera y scroll SSID/IP

Pedido humano: dar al menos2px de margen superior al nombre de red del
dashboard y conservar scroll para los textos que no entran.

ATmega `afr2_screens.c` y Web `firmwareRenderer.ts`, pantalla010001:
SSID pasa de y0 a y2; IP de y12 a y14. Iconos y voltaje bajan2px y el fondo
blanco crece a26px, con divisor en y26. El modo queda en y32.

Se mantiene scroll automático independiente de SSID e IP en su ventana de70px:
solo cuando desbordan, pausa800ms en cada extremo y movimiento1px/40ms de ida
y vuelta. Las bandas de borrado/inversión parcial también pasan a y2/y14 para
conservar el margen y los iconos fijos mientras se desplaza el texto.

Fuentes AVR/Web y catálogo vinculados. Recursos existentes; copia de biblioteca
en modo `--copy-only`, sin checks. ESP/transporte/navegación sin cambios. Sin
tests, compilación, validadores, navegador, flasheo, commit ni push.
