# AFR2-20261030-17 — Botón de pulsación larga con L integrada

Origen: bitmap de 26 bytes proporcionado explícitamente por el usuario en el chat, nombre original image_Ok_btn_pressed_bits. Se conserva el nombre semántico Icon_Encoder_Long_bits y la identidad AFR2_ICON_ENCODER_LONG=17. XBM13x13, dos bytes por fila, bit menos significativo primero. PROGMEM de avr-libc reemplaza U8X8_PROGMEM; no se incorpora Arduino/U8g2.

Reemplaza el antiguo recurso13x16 y su subrayado; reduce el bitmap de32 a26 bytes. Afr2Ui_GestureHint ahora dibuja el botón nativo13x13 con L central integrada en x,y-3; conserva etiqueta en x+16,y. Elimina el botón reducido7x7 y la L tipográfica externa. Corto conserva su bitmap y posición. Navegación, largo al soltar y tiempos conservados. Sin nuevos ScreenCodes, wire ni SRAM de bitmap; flash final no medida.

Contraparte Web: encoderLongIcon.ts contiene los mismos26bytes. firmwareHints.ts usa ese recurso y las mismas coordenadas; firmwareRenderer.ts también lo aplica al icono17 para evitar usar el recurso generado antiguo. Catálogo generado pendiente del siguiente build autorizado; hashes revisados tras portar comportamiento, sin ejecutar generador.

ATmega/Web: adaptados en fuente. ESP/Loader: sin cambios; paquete Web del ESP sin actualizar. Sin compilación, tests, validadores, render de prueba ni carga. Autoría del bitmap atribuida al usuario, autores anteriores preservados. Procedencia por archivo en ../provenance/encoder-long-icon-20261030-17.json.
