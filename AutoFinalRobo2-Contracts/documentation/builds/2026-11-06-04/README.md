# AFR2-20261106-04 — RAM y QR

Implementación pedida: conservar ambos buffers OLED, aplicar optimizaciones RAM y después QR. Comprobaciones dentro de la autorización expresa anterior «Regenerar y hacer comprobaciones». Sin COM, carga, motores ni prueba física.

| Medida del producto AVR | Antes (02) | Después (04) |
| --- | ---: | ---: |
| .data | 1650 | 284 |
| .bss | 5977 | 5321 |
| .noinit | 9 | 9 |
| SRAM estática | 7636 | **5614** |
| Libre antes de pila, sobre 8192 | 556 | **2578** |
| Flash (.text + .data) | 123586 | **124240** |
| Stack frame QR finish, .su | 380 | **29** |
| Objeto OLED | 2140 | **2140** |

Se recuperan **2022 bytes estáticos**: 1366 de .data y 656 de registros/kinds de secuencias deshabilitadas. El buffer de 1024 y su sombra ACK de 1024 se conservan. Product pasa de 1721 a 1065 bytes. El QR recupera **351 bytes en su frame**, separado del ahorro estático; no se añade una matriz static.

## Implementación y comprobaciones

- Textos PROGMEM con APIs de notificación RAM/flash/mixta, motivo flash documentado, formatos snprintf_P y ventanas/scroll flash sin copia temporal. Se conservan texto, truncado, cola y tiempos.
- Solo unión de pasos y kinds de secuencias condicionados por AFR2_SEQUENCE_MEMORY_APPROVED. Estado/metadatos y validaciones de comandos conservados; macro predeterminada sigue 0. Perfil 1 compilado/enlazado en copia aislada: SRAM 6270, libres 1922 y flash128830. Es comprobación de compilación, no aprobación para ejecutarlo.
- QR usa préstamo síncrono de 353 bytes de acknowledged, invalida antes los chunks intersectados, conserva tx/command y borra scratch al terminar. I2C Manager llama callbacks exclusivamente desde main; el raster no cede el control. No se agrega framebuffer ni hardware.
- Make -B firmware/communication-firmware/library: PASS. Perfil habilitado aislado: PASS. Builds AVR sin advertencias. PROGMEM termina en dirección 25390, dentro de ventana near de 65536.
- Web build:esp, incluyendo generador con guardia y tsc/Vite/paquete/copia local a ESP/data/dist: PASS. 70 nodos, 26 iconos y 950 bytes de fuente. 17 activos, 2487493 bytes gzip; presupuesto conservador2629632/2867200. No se recrea imagen LittleFS ni se compila ESP en esta ficha; wire/firmware ESP no cambia.
- [Pruebas de host](../../../tools/verification/avr-ram/README.md): PASS. Préstamo QR durante comando/datos pendientes, ACK/error, copia in-flight intacta y resincronización exacta de panel simulado. Texto RAM/flash, scroll, avisos y límites de préstamo. GCC nativo -Wall avisa sobre formatos heredados; ver log y README, sin atribuirlos al ABI AVR ni declararlos resueltos.
- [Paridad C/Web](../../../tools/verification/oled-parity/README.md): **2104/2104** fotogramas iguales, incluyendo los 1680 anteriores, avisos y QR directo220 bytes/rechazos. Sin afirmar cobertura de todos los estados posibles o tiempos físicos.

## Alcance del presupuesto

La cadena seleccionada main305 + Render155 + QrRender37 + finish29 suma526, frente a877 antes. No es el pico máximo del programa: el codificador QR tiene otras subllamadas, hay otros caminos de protocolo/acciones y faltan interrupciones y medición física. Los 2578 bytes son espacio antes de pila, no memoria libre medida durante uso.

La insuficiencia estática demostrada en 02 deja de ocurrir para esa cadena. La aceptación del auto sigue pendiente: marca de agua de pila, latencia I2C, recuperación, HTTP/WS/táctil y sensores. Este build no prueba que lo instalado en las placas esté actualizado.

[Medidas y hashes](metrics.json). Capturas de artefactos en .tmp/ram-optimization-04/artifacts; sin BIN/HEX/EXE agregados a Git. ESP, Loader y CAD sin modificaciones de fuente por esta ficha. [Contrato de almacenamiento](../../../contracts/AVR_RAM_STORAGE_20261106.md).
