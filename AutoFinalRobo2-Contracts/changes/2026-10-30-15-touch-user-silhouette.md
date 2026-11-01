# AFR2-20261030-15 — Silueta aportada para control táctil

30/10/2026. El usuario aporta autito_silueta_original_128x64.png y pide usarla como bitmap. Se conserva el archivo original sin modificar en documentation/assets/autito_silueta_original_128x64.png; autoría original no establecida, procedencia: archivo del usuario en Downloads. No se atribuye su dibujo a Codex.

## Conversión y render

PNG128x64, píxeles claros con R>=128; límites inclusivos x31..96/y2..61 (66x60). Se retiran márgenes negros y muestrea al centro de cada píxel por vecino más próximo, sin suavizado, a24x22. Se conserva orientación de la imagen, interpretando arriba como frente visual por continuidad del control, no como verificación mecánica. Bitmap empaquetado por filas LSB, stride3,66bytes PROGMEM (antes48; incremento18bytes), más la tabla Q8 existente48bytes. No cambia framebuffer ni agrega memoria de imagen en RAM.

Los renderers C/TS comparten dimensiones y bits. Ventana de rotación36x36 centrada64/32 dentro de la banda64x40 existente. Diagonal nominal sqrt(24²+22²)≈32,56px; ventana ampliada respecto de32x32 anterior para alojarla. Mantiene sectores15grados, pie numérico y tres chevrones. Bucle1296píxeles frente a1024, sin medición de latencia/pila/flash finales. No cambia ScreenCode030035, navegación, protocolo ni giro físico. La Web3D conserva el modelo editable existente; este recurso es la representación OLED y su copia raster Web.

## Adaptaciones por archivo

- AutoFinalRobo2-Atmega/include/afr2_touch_bitmap.h: bytes de la silueta, dimensiones24x22 y stride3, procedencia y autorías anteriores conservadas.
- AutoFinalRobo2-Atmega/src/ui/afr2_screens.c: centro/índices según dimensiones y ventana36x36.
- AutoFinalRobo2-Web/src/features/oled/touchBitmap.ts: mismos bytes y dimensiones, tabla seno conservada.
- AutoFinalRobo2-Web/src/features/oled/firmwareRenderer.ts: port literal de dimensiones, stride y giro.
- AutoFinalRobo2-Web/tools/sync-oled-library.mjs: conserva dimensiones al regenerar recurso TS; no se ejecutó.
- AutoFinalRobo2-Web/src/features/oled/firmwareRenderer.sources.json: hashes C/recurso revisados después del port, guard conservado.
- AutoFinalRobo2-Web/public/icons/touch-car.png y touch-car.pbm: exportaciones binarias24x22 actualizadas desde los mismos bits.
- AutoFinalRobo2-Contracts/documentation/assets/autito_silueta_original_128x64.png: original aportado preservado.
- AutoFinalRobo2-Contracts/documentation/screens.json y contracts/TOUCH_OLED_19.md: variante/medidas actuales, identidad y transporte conservados.
- Esta ficha: pedido, procedencia por archivo, dimensiones y estados.

## Estado real

ATmega/Web solo fuente; ESP sin cambios. Conversión determinista del recurso y previsualización PNG a escala10x solicitadas por el usuario, sin captura física. Se inspeccionó la previsualización. Sin tests, compilación, validadores, generador OLED, navegador, COM, promoción ni flasheo. Giro/paridad/latencia y margen SRAM/pila sin comprobar por ejecución. Nueva silueta aún no cargada al auto.
