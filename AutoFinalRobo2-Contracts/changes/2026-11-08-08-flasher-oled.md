# AFR2-20261108-08 - El flasher ATmega muestra en la OLED qué se carga en el ESP

Pedido del usuario: que el flasher use el I2C para dar feedback; en pantalla «AFR2 Loader firmware» con la versión debajo
y, al cargar el ESP, barra de progreso y qué se está subiendo. Solo fuente: sin compilar, flashear ni probar.

Versión de producto: 1.3.0 (subversión: función nueva compatible). Loader: 2.1.0 (su propia línea, subversión).

## Pantallas del flasher (OLED 0x3C por D20/D21)

Reposo, al encender el flasher (después del arranque de la OLED, 1,2 s):

- «AFR2» al doble de tamaño (como la pantalla de inicio del auto), «Loader firmware», «Version 1.3.0» y
  «USB-ESP 115200» (el baud fijo compilado). Si una cola se desborda: «Error: desborde».

Durante una sesión de esptool:

- Encabezado invertido con el título que manda el Loader («Detectar ESP», «Cargando ESP», «Paquete AT»); sin Loader
  nuevo, «ESP8266».
- Imagen: el nombre que manda el Loader para esa dirección («Firmware ESP», «Web LittleFS» o el nombre de la región AT);
  si no hay nombre, «Firmware 0x0» o «Datos 0x200000».
- Fase: Conectando…, Preparando… (stub), Borrando…, Escribiendo N %, Escrito 100%, Verificando…, Leyendo…,
  «Listo sin error» o «Error: ver Loader».
- Barra de progreso (bloques enviados / bloques del FLASH_BEGIN) y abajo «bloques - KB» o «Img 1/2 812 KB» cuando
  hay dos imágenes; «ESP reiniciado» después del RUN de KEY/RST automático.
- Sin tráfico durante 60 s después de terminar, vuelve a la pantalla de reposo.

«Listo sin error» solo dice que esptool terminó sin excepción; la verificación que vale sigue siendo la del Loader
(«Hash of data verified» por imagen). La barra cuenta bloques enviados por la PC, no escritos confirmados por el ESP.

## Cómo sabe el progreso

El flasher solo **observa** el flujo PC→ESP (SLIP de esptool) sin cambiar ningún byte: SYNC, MEM_BEGIN (stub),
FLASH_BEGIN/FLASH_DEFL_BEGIN (tamaño, bloques, offset), FLASH_DATA/FLASH_DEFL_DATA (número de bloque), SPI_FLASH_MD5,
ERASE y READ_FLASH. Guarda solo los primeros 24 bytes de cada trama.

## Cambios en el transporte

Antes el puente atendía las dos UART por sondeo. Dibujar y enviar la OLED tarda más que un byte a 115200 (87 µs), así
que ahora **las dos UART reciben y transmiten por interrupciones** (RX y UDRE); el dibujo, el I2C y las órdenes con
esperas (`_delay_ms` de KEY/RST) quedan en el bucle principal sin frenar los bytes. Se conservan: colas de 512, baud fijo,
desborde = las dos UART se detienen hasta reset (esptool falla, nunca se afirma éxito), KEY/RST en alta impedancia hasta
orden de la PC, timeout de KEY de ~600 s y la captura de arranque a 74880 (por sondeo, con las interrupciones de UART1
apagadas y los bytes de la PC retenidos hasta el vaciado posterior). La trama de control ahora se detecta en la
interrupción de RX0 y se ejecuta en el bucle principal.

## Protocolo de control (aditivo)

- Respuesta: flags bit4 = el flasher tiene OLED de feedback (además de bit3 captura de arranque).
- `0x30` LABEL: después de `~cmd`, longitud N (5..22), offset u32LE y texto N-4 (ASCII imprimible, hasta 18).
  Offset `0xFFFFFFFF` = título y nueva sesión; otro offset = nombre de la imagen escrita ahí (hasta 3).
- `0x31` RESULT_OK / `0x32` RESULT_FAIL: cómo terminó esptool.
- Ninguna toca KEY/RST ni el flujo del ESP; no vacían las colas.
- Un flasher anterior no anuncia bit4: el wrapper no le manda LABEL/RESULT (su payload llegaría al ESP).

## Loader (wrapper `tools/esp_bridge_esptool.py`)

- `--oled-title` y `--oled-image OFFSET=NOMBRE` (repetible). Con KEY/RST automáticos usa la consulta que ya hacía; en modo
  manual hace una consulta QUERY (no toca pines) solo si hay textos para mostrar.
- Al cerrar el puerto manda RESULT_OK si no hay una excepción en curso (o `SystemExit` 0) y RESULT_FAIL si la hay.
- `Operations.EspArgs` agrega los textos: Detectar ESP (flash_id), Cargando ESP con Firmware ESP / Web LittleFS por sus
  offsets del perfil, Paquete AT con el nombre de cada región.

## OLED y EEPROM

Misma pila del producto y del diagnóstico I2C: `afr2_i2c_manager.c`, `afr2_twi.c`, `afr2_board.c` (Timer0 1 ms),
`afr2_oled.c`, `font7x10.c`. Rotación y contraste se **leen** de la EEPROM del producto (registro válido más nuevo; si no
hay, 180° y 0xCF). El flasher no escribe la EEPROM.

## Archivos

- ATmega: `service/esp_uart_bridge.c` (reescrito), `Makefile` (`BRIDGE_SOURCES`), `service/README.md`.
- Loader: `tools/esp_bridge_esptool.py`, `src/Operations.cs`, `src/Operations.EspModes.cs`, `src/JobState.cs` (2.1.0),
  `README.md`.
- Contracts: `contracts/afr2_build_info.h` (1.3.0), `docs/VERSIONADO.md`, CHANGELOG. Web: `package.json` y
  `package-lock.json` (1.3.0, solo versión).

## Estado y límites

atmega(flasher)=fuente reescrita, sin compilar ni cargar; loader=fuente editada, sin compilar; esp=sin cambios; web=solo
versión. Sin medir flash/RAM (la OLED suma ~2,1 KB de RAM a las colas de 1 KB) ni la carga de interrupciones a 115200
con esptool real: es el primer punto a probar. Las pantallas del flasher no están en `screens.json` ni en la biblioteca
Web: son de una imagen de servicio aparte, no del producto, y no se informan por EX01. Hay que volver a cargar el
flasher en el ATmega (Loader > Cargar flasher) para verlas.
