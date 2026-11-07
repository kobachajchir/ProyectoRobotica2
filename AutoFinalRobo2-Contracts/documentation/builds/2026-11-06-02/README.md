# AFR2-20261106-02 — Regeneración y comprobaciones

Autorización expresa: «Regenerar y hacer comprobaciones». Continúa la entrega de fuente AFR2-20261106-01. Sin COM, cargas, movimientos ni pruebas físicas.

| Comprobación | Resultado |
| --- | --- |
| Generador OLED, guardia activa sin copy-only | PASS: 70 nodos, 26 iconos, 950 bytes de fuente |
| TypeScript y npm run build:esp | PASS: Vite, paquete y copia local a ESP/data/dist |
| Make AVR -B firmware communication-firmware library | PASS: producto, comunicación y biblioteca; sin advertencias finales |
| PlatformIO run -e esp12s_candidate | PASS: firmware, sin advertencias finales de fuente |
| Loader Compilar.ps1 con OutputDirectory aislado | PASS: no abrió ni reemplazó el ejecutable instalado |
| Comparador C/TS | PASS: 1680/1680 fotogramas de 1024 bytes iguales |
| Integridad Web | PASS: 17 activos; gzip, SHA-256 y tamaños; manifiesto 5953/12288 bytes |
| LittleFS candidato | PASS: imagen creada y listada, 3121152 bytes |
| git diff --check | PASS; solo avisos LF/CRLF |
| Presupuesto SRAM/pila AVR | **FAIL: detener despliegue** |

## Bloqueo AVR

avr-size del ELF final: text=121936, data=1650, bss=5977, noinit=9. Flash **123586 bytes**. SRAM estática **7636/8192**, quedan **556 bytes antes de pila**.

Los .su del mismo build y las llamadas del ELF identifican main (305) → Afr2Ui_Render (155, dynamic,bounded) → Afr2Qr_Render (37) → finish (380): estimación conjunta **877 bytes**, superior al margen, sin interrupciones ni llamadas de dibujo. No es medición física de pila máxima. Este presupuesto no se puede aprobar: reducir SRAM/pila y repetir antes de cargar. El build de comunicación no sustituye al producto.

ESP: RAM estática **50676/81920**, flash **452575/1044464**. Heap y presión WS en ejecución no medidos. El perfil candidato esp12s_candidate con eagle.flash.4m3m.ld no identifica flash física ni modo SPI.

Web: **2487493 bytes gzip**, estimación LittleFS **2629632/2867200**. Se comprobó buildfs; la imagen definitiva se regeneró con mklittlefs -c staging-solo-dist -p 256 -b 8192 -s 3121152. Contiene solo /dist. La imagen provisional de buildfs incluía la configuración privada local de data; fue reemplazada. No cargar una imagen completa sin preservar configuración del dispositivo.

## Alcance y procedencia

[Comparador reproducible](../../../tools/verification/oled-parity/README.md): 70 nodos reales × 3 selecciones/progresos × 8 snapshots. Incluye páginas de 16 redes con candado, buscando/vacío/fallo, Hall pendiente, IR digital y QR AP/Web ficticios. Solo render completo en host; no acredita todos los parciales, notificaciones, navegación, ABI AVR o temporización física.

Se corrigieron cuatro advertencias de estilo en QR, dashboard y ordenación Wi-Fi, sin cambiar algoritmos o coordenadas. Hashes renovados después de revisar equivalencia C/TS; comparación repetida PASS. El generado anterior quedó respaldado en .tmp/oled-review-20261106/firmwareLibrary-before-56a6f7a7ff9c4c51b26878153c52d6dd.ts.

Pendientes: resolver presupuesto AVR, medir heap, HTTP AP/STA, primera conexión y reconexión WS/STOP bajo presión, EEPROM, navegación, sensores y táctil en el auto. [Aceptación pendiente](../../acceptance/2026-11-06-01.md). [Métricas y hashes](metrics.json). Logs vecinos; BIN/HEX/EXE e imagen quedan locales, fuera de Git.

## Captura aislada

Las salidas compartidas ESP/data/dist y .pio cambiaron después de la primera medición. Se repitió el build ESP/Web y se capturaron inmediatamente el BIN y paquete; LittleFS se creó desde esa captura. Los hashes de metrics.json apuntan a `.tmp/verification-20261106/artifacts`, no a salidas compartidas susceptibles de sobrescritura. No se mezclan hashes anteriores con los nuevos bytes. Fuente y coordenadas no cambiaron durante esa repetición.
