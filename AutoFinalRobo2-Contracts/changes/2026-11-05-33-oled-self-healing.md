# AFR2-20261105-33 — El OLED se repara solo ante ruido I2C

05/11/2026. Pedido humano: después de un rato de uso con el auto, la pantalla titila, gira la orientación y se redibuja entera a distintas velocidades; luego se arregla sola y vuelve a fallar.

Continúa AFR2-20261029-14/16 (driver OLED cooperativo).

## Diagnóstico (inferido del código, no medido)
- El bus usa solo los pull-ups internos del AVR (20–50 kΩ). Con motores, el ruido puede alterar un byte que el panel igual reconoce (ACK).
- Un byte de comando alterado cambia el estado del SSD1306: `0xA0/0xA1`, `0xC0/0xC8` (orientación), `0x40–0x7F` (línea de inicio), contraste o multiplex.
- El driver enviaba solo diferencias y nunca reenviaba la configuración, así que el error quedaba hasta que tres fallas I2C seguidas forzaban una re-inicialización completa. Eso explica el «se arregla de la nada» y la pantalla entera redibujándose.
- La secuencia de inicio tenía la orientación fija en 0° (`0xA1, 0xC8`) y la configuración por defecto es `rotation=1`: cada recuperación mostraba la imagen invertida hasta que llegaba el comando de ajustes.
- No se encontró condición de carrera: la ISR TWI solo publica estado y los callbacks corren en `main`.

## Cambios
| Módulo | Archivo | Antes | Ahora |
| --- | --- | --- | --- |
| atmega | `src/display/afr2_oled.c` | inicio con `0xA1/0xC8/0xCF` fijos | inicio, recuperación y refresco con la orientación y el contraste actuales (`load_config`) |
| atmega | `src/display/afr2_oled.c` | configuración enviada solo al iniciar o recuperar | refresco cada `AFR2_OLED_REFRESH_MS` = 1000 ms, sin `0xAE` (no apaga el panel), solo entre escrituras de página |
| atmega | `src/display/afr2_oled.c` | RAM del panel nunca reescrita si no cambia | se reescribe un bloque de 16 columnas cada `AFR2_OLED_SCRUB_MS` = 40 ms: la RAM entera se repara cada ~2,6 s |
| atmega | `src/display/afr2_oled.c` | cambiar la rotación dejaba columnas viejas espejadas | cambiar la rotación invalida toda la RAM y la reenvía |
| atmega | `include/afr2_oled.h` | — | campos `refresh_ms`, `scrub_ms`, `scrub` |

Costo estimado en el bus: ~26 B/s de configuración y ~21 B cada 40 ms de RAM (~0,5 kB/s, 100 kHz). No medido.

## Hardware recomendado (causa de fondo, no verificado)
- Pull-ups externos de 4,7 kΩ en SDA/SCL.
- Cables cortos o trenzados con GND; 100 nF + 10 µF junto al OLED.
- Retorno de los motores separado de la tierra del I2C.

## Estado por repo
- **atmega:** fuente modificada. Sin compilar ni flashear; falta confirmar en la placa que el parpadeo desaparece o se corrige en pocos segundos.
- **esp / web / loader:** sin cambios.
