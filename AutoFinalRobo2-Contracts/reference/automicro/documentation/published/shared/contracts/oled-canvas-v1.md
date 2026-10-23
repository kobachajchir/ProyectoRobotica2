# OLED Canvas — documento Web a framebuffer F4

ID: `shared:message/oled-canvas/v1`. Fotografía del 20-10-2026 por lectura de
fuente; sin transferencia ni comprobación de placa. Es upload Web → ESP → F4,
distinto del visor de la pantalla F4 → Web.

## Representación común

128×64 monocromo, 1024 bytes. Para (x,y), el byte es
`x + floor(y/8)*128` y el bit es `1 << (y%8)`. JSON format es
`ssd1306-page-lsb`; el wire F4 usa format:u8=0. No confundirlo con bitmap row-major
LSB/MSB ni con píxeles RGBA. `Uint8Array` es almacenamiento local; JSON transmite
base64 y C/C++ recibe bytes decodificados.

CRC32 usa polinomio reflejado 0xEDB88320, inicialización y XOR final 0xFFFFFFFF
sobre los 1024 bytes en ese orden. JSON canónico usa ocho dígitos hex mayúsculos
sin prefijo; wire u32 LE. El cliente normaliza algunos formatos de respuesta,
pero el parser ESP de request exige la forma de ocho mayúsculas.

## JSON Web → ESP

| Comando | Args | Respuesta de datos |
| --- | --- | --- |
| `oledCanvas.begin` | width:128, height:64, format, totalBytes:1024, crc32:string | transferId no cero u16, chunkBytes:512, nextOffset:0. |
| `oledCanvas.chunk` | transferId, offset y dataBase64 | Mismo transferId y nextOffset confirmado. |
| `oledCanvas.commit` | transferId | Final: state:`rendered`, bytes:1024, crc32 y screenCode:0x030503. |
| `oledCanvas.cancel` | transferId | state:`canceled` del transferId correspondiente. |

Begin reserva una única transferencia con dueño client/generation y sesión F4
vigente. ESP recibe los dos bloques Web de 512 bytes y ensambla antes de enviar
el documento al F4. El número de bloque/offset y los duplicados no autorizan
sobrescribir otra transferencia. El usuario puede perder conexión o cambiar
modo/pantalla durante la operación; el state machine debe terminar/cancelar en
la generación original.

Commit comprueba CRC del ensamblado y coordina BEGIN/CHUNK/COMMIT F4. No resolver
la Promise como `rendered` por aceptar los chunks locales. El cliente contrasta
transferId, estado, total, CRC y screenCode de la respuesta final.

## ESP → F4, comando 0x6D

Enteros multibyte LE; operación en primer byte.

| Operación | Longitud | Layout |
| --- | --- | --- |
| BEGIN=1 | 12 | 0 op; 1..2 transferId; 3 width; 4 height; 5 format; 6..7 totalBytes; 8..11 crc32. |
| CHUNK=2 | 6+n | 0 op; 1..2 transferId; 3..4 offset; 5 n; 6.. bytes. n máximo 112. |
| COMMIT=3 | 3 | 0 op; 1..2 transferId. |
| CANCEL=4 | 3 | 0 op; 1..2 transferId. |

Los bloques UART no tienen tamaño 512: ESP divide los 1024 bytes en nueve
fragmentos de 112 y uno final de 16. No trasladar el tamaño Web al wire UART.
F4 requiere modo TEST y pantalla de servicio 0x030503 para el flujo; la sesión
remota y los deadlines son condiciones independientes de tener el buffer listo.

Respuesta F4 por 0x6D, diez bytes: op:u8 en 0, status:u8 en 1, transferId:u16 en
2..3, nextOffset:u16 en 4..5, computedCrc32:u32 en 6..9. El matcher ESP incluye
operación, transferId y nextOffset esperado en respuestas OK para que una
respuesta CHUNK duplicada tardía no complete el fragmento siguiente.

| Status | Significado |
| --- | --- |
| 0 | OK |
| 1 / 2 | Fuera de TEST / pantalla incorrecta |
| 3 / 4 | Busy / metadata incorrecta |
| 5 / 6 | Fuera de orden / CRC no coincide |
| 7 / 8 / 9 | Timeout / error de display / cancelado por UI local |

F4 define timeout de ensamblado 3000 ms y de display 1000 ms; esos plazos no son
el timeout de upload ESP o Promise Web. Conservar estados de finalización y
errores de la capa que los produce.

## Evidencia y puntos de mantenimiento

- Web: `src/features/oledEditor/oledCanvasRasterizer.ts`, `oledCanvasProtocol.ts`,
  `useOledCanvasTransfer.ts`.
- ESP: `src/oled_canvas_transfer.h/.c`, `src/ws_bridge/oled_canvas.inc`.
- F4: `Core/Inc/oled_canvas.h`, `Core/Src/oled_canvas.c` y aplicación UNER.
- Fichas: [Web](../../web/libraries/oled-transfer.md),
  [ESP](../../esp/libraries/oled-canvas.md), [F4](../../f4/libraries/oled-canvas.md).

Al cambiar dimensiones, layout, CRC, chunks o respuesta final actualizar los tres
productores/consumidores y la ficha AMI. No usar upload exitoso como evidencia de
paridad del visor ni de disponibilidad de un snapshot de UI.
