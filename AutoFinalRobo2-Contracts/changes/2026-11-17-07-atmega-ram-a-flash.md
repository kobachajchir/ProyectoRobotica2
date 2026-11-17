# AFR2-20261117-07 - ATmega: textos y tablas constantes de RAM a flash

Autoría: Koba Chajchir / AFR2.
Versión: 1.35.2 (subversión mínima).

Estados:
- atmega = compilado sin cargar;
- esp = solo versión;
- web = solo versión.

Sin cambio de contrato ni de comportamiento.

## Pedido

Liberar RAM del ATmega2560 moviendo a flash lo que no necesita estar en RAM. El usuario eligió «solo los seguros», es decir, los que no cambian funcionamiento. Quedan fuera:
- achicar el log USB;
- compartir la lista Wi-Fi;
- quitar la copia confirmada del OLED.

También pidió saber cómo detectar si el ATmega se queda sin memoria.

## Cambios (ATmega)

En AVR, todo literal o tabla `const` sin `PROGMEM` se copia de flash a RAM al arrancar (`.data`).

- **Firmas de 4 bytes:** `memcpy_P` / `memcmp_P` con `PSTR`.
  - `BT01` en `main.c` y `afr2_link.c`;
  - `AFR2` en `afr2_link.c` y `afr2_preferences.c`;
  - `EX01` en `afr2_app_wire.c`.
- **Textos con formato:** los argumentos de texto de `LINE`, `snprintf_P` y `AFR2_DBG` pasan de `%s` con literal en RAM a `%S` con `PSTR`, que avr-libc lee desde flash.
  - `afr2_app.c`: Salidas, Conectado/Desconectado, modo e IP Wi-Fi, seguridad, versiones ESP/Web.
  - `afr2_screens.c`: CONFIRM del log, Hall `*`/`OK`, título «Wi-Fi …», «Redes …+».
  - `afr2_sensor_screens.c`: ` >esc` del radar.
- **Copias a etiquetas:** `strcpy_P` / `strncpy_P`.
  - «Sin red», «Red oculta» y «Enviando…» en `afr2_app.c`;
  - «Traer credenciales ESP» en `afr2_menu_catalog.c`.
- **Comparaciones con «Volver»:** `strncmp_P` en `afr2_screens.c` (×2) y `afr2_icons.c`.
- **Indicadores de fila del menú** (`>`, espacio, `!`): `Afr2Oled_TextFlash`.
- **Dashboard:** «IP: --» se copia a un búfer local de 8 B con `strcpy_P`.
- `afr2_network_screen.c`: nuevo `labeled_flash`. «Sin clave» y «Wi-Fi» se dibujan desde flash.
- `afr2_qr_screen.c`: nuevo `append_P` para los prefijos fijos del QR («WIFI:T:WPA;S:», «WIFI:T:nopass;S:», «;P:», «;;», «http://», «/»); `strcmp_P` para «0.0.0.0».
- `third_party/qrcodegen.c` (Project Nayuki, MIT): adaptación local. Las tablas `table[4]` y `temp[3]` ×4 (int) pasan a `PROGMEM` y se leen con `pgm_read_word`. El algoritmo no cambia.
- `Makefile`: `-fno-tree-switch-conversion`, para que gcc no genere tablas `CSWTCH` en RAM para algunos `switch`; quedan como saltos en flash.

## Resultado (compilado el 17/11/2026, a pedido del usuario)

| | Antes (1.35.1) | Después (1.35.2) |
|---|---|---|
| `.data` | 354 B | 10 B |
| `.bss` | 6 848 B | 6 848 B |
| RAM estática | 7 202 B | **6 858 B** (−344 B) |
| Libre para pila (de 8 192) | ≈990 B | **≈1 334 B** |
| Flash (`.text` + `.data`) | 169 062 B | 169 776 B (+714 B, de 256 KB) |

- `make -B firmware communication-firmware library`: exit 0.
- Una única advertencia previa ajena: `afr2_product.c:262`.
- Queda en RAM un «Volver» (7 B) y tres contadores de 1 B.
- No hay ensayo físico: los textos de las pantallas Wi-Fi, QR, Hall y radar deberían verse igual, pero no se comprobaron en el OLED.

## Cómo detectar falta de memoria

- **En compilación:** `avr-size -A build/afr2.elf` da `.data + .bss` (RAM estática). Lo que sobra hasta 8 192 B es lo único disponible para la pila. Con menos de unos 600 B conviene revisar.
- **En marcha:** con `AFR2_DEBUG_LOG=1` (por defecto), el log USB (D1, 115200) escribe cada 5 s `STAT … stack>=NNNB`.
  - Es la pila que nunca se tocó desde el arranque: al iniciar se pinta la RAM libre con 0xC5 y se cuentan los bytes intactos.
  - Si baja de unos 150 B, el margen es crítico.
- **Síntomas de pila agotada:**
  - reinicios sin motivo aparente (causa en el log de arranque, incluida `stack-smash`);
  - variables que cambian solas, como el menú, valores de calibración o el estado Wi-Fi;
  - basura en el OLED;
  - el watchdog que reinicia el ATmega.

## Pendiente

- Cargar el ATmega (`build/afr2.hex`) y revisar en el OLED las pantallas tocadas: Estado/Salidas, Wi-Fi (modo, IP, seguridad, redes), versiones, QR, Hall, radar y avisos de red.
- ESP y Web no se recompilaron: siguen en 1.35.1. Si se cargan juntos con el ATmega, sus versiones difieren hasta la próxima compilación.
