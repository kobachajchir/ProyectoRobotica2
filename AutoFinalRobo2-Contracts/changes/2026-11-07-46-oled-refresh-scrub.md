# AFR2-20261107-46 — OLED: refresco periódico liviano y menos tráfico I2C

07/11/2026. Pedido del usuario: el OLED a veces inicia y a veces no, se cuelga, refresca de más, entra en loop de reseteo, o el ATmega arranca y el OLED no. Se investigó y luego se pidió aplicar los ajustes de firmware.

Estado real: atmega = fuente editada, sin compilar ni cargar; esp/web = sin cambios. Sin pruebas físicas.

## Revisión (sin cambios de código)

- `afr2_twi.c`: sin esperas bloqueantes. STOP con plazo, recuperación de bus con 9 pulsos y un tope de 50 ms, y estado FAILED que se reintenta en la siguiente transferencia.
- `afr2_i2c_manager.c`: plazo de 40 ms por transferencia y aborto con recuperación.
- `main.c` y `afr2_watchdog.c`: watchdog de 500 ms. Ningún camino de OLED, TWI ni EEPROM bloquea (la EEPROM se escribe byte a byte sin esperar).
- La causa del reinicio (MCUSR) ya se informa en la pantalla de diagnóstico (`Reset:`) y en la Web (`resetCauseBits`): 1 = encendido, 2 = externo, 4 = brown-out, 8 = watchdog.

## Cambios en `AutoFinalRobo2-Atmega/src/display/afr2_oled.c`

1. El refresco periódico (cada 1 s) pasa de reenviar casi toda la inicialización (26 B, con oscilador 0xD5, precarga 0xD9 y VCOMH 0xDB) a una lista de 18 comandos: multiplexado, offset, línea de inicio, direccionamiento, orientación actual, pines COM, contraste actual, 0xA4, 0xA6, bomba de carga 0x8D 0x14 y 0xAF. Se mantiene lo que el ruido o una caída de alimentación del panel pueden borrar, y se evita el posible parpadeo periódico. El arranque y la recuperación siguen usando la secuencia completa.
2. El repaso de RAM (`AFR2_OLED_SCRUB_MS`) pasa de 40 a 250 ms. El panel completo se reescribe cada unos 16 s en lugar de cada 2,6 s. Antes el bus no descansaba nunca (unas 50 transferencias por segundo), lo que con pull-ups débiles y ruido multiplicaba los fallos y las reinicializaciones del panel.

Cuesta unos 19 B de flash para la tabla nueva y nada de RAM; no está medido.

## Causas eléctricas probables (sin medir)

1. Continuidad entre SDA/SCL de la cabecera del shield (formato UNO) y A4/A5. En el Mega, A4 es IR1 (salida push-pull) y A5 es el divisor de batería con 100 nF. Si están unidos, el I2C queda comprometido. Medir con el tester, todo sin alimentar.
2. Solo pull-ups internos en SDA/SCL. Se recomiendan 4,7 kΩ externos a 5 V si el módulo OLED no los trae.
3. Caídas de los 5 V compartidos (servo, Hall, IR, OLED): pueden provocar brown-out del Mega o dejar al OLED sosteniendo SDA en bajo, y eso solo se resuelve cortando la alimentación.

## Pendiente

- El build `build:esp` del Loader fallaba en `sync-oled-library.mjs` con «AVR renderer changed: atmega:src/display/afr2_oled.c». El manifiesto `AutoFinalRobo2-Web/src/features/oled/firmwareRenderer.sources.json` tenía los hashes de `afr2_oled.c` y `afr2_oled.h` de `origin/codex/monorepo-cad`, pero en la mezcla del 07/11 se conservó el `afr2_oled.c` de main (con `load_config`), y después se aplicó este cambio. Revisado: las diferencias con la versión registrada están solo en el envío (estados, refresco y repaso, más los campos `refresh_ms`, `scrub_ms` y `scrub` del encabezado). Las funciones de dibujo y el formato del buffer no cambian, así que `firmwareRenderer.ts` no necesita portar nada. Se actualizaron los dos hashes (SHA-256 del texto con LF). Los 17 hashes del manifiesto coinciden con las fuentes. Web: manifiesto editado, build sin volver a correr.
- Compilación, carga y verificación física.
