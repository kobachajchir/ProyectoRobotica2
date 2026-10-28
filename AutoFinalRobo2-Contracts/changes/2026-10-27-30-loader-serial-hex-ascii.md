# AFR2-20261027-30 — Loader0.7.4: Monitor serie ASCII / HEX con espacios

27/10/2026. El usuario pidió cambiar el Monitor serie entre HEX separado por
espacios y ASCII. Adaptación propia asistida por Codex; autoría de Koba
Chajchir y cambios anteriores conservados.

## Comportamiento

Selector **Vista** disponible con captura abierta o cerrada: ASCII (inicial),
HEX con espacios y Texto decodificado para conservar UTF-8/ASCII/Latin-1.
ASCII y HEX usan los bytes originales, no una recodificación del texto.
HEX muestra dos dígitos mayúsculos por byte, un espacio entre bytes y16bytes
por fila, manteniendo la columna entre fragmentos de recepción. Los banners
de inicio de captura siguen siendo texto informativo separado de los bytes.

La captura entrega fragmentos con bytes y texto decodificado. Conserva bytes
incluso si un fragmento UTF-8 todavía no produjo caracteres. El worker sigue
siendo dueño exclusivo del COM y de los archivos; Vista no cambia el puerto,
baudios, DTR/RTS ni decodificador de la captura.

La UI retiene fragmentos hasta65536unidades (bytes + caracteres decodificados)
para reconvertir la ventana al cambiar Vista. La recepción normal sigue
agregando texto por lotes, sin reconstruir la consola en cada tick.
No se reconstruye historial anterior a esa ventana. Limpiar vista descarta
también los fragmentos retenidos y la cola visual ya pendiente para que no
reaparezcan al cambiar formato. Se conserva Seguir texto y la pausa de vista.
Copiar texto copia el formato visible.

`.rx.bin` mantiene bytes originales; `.rx.txt`, el journal MCP y el parser de
arranque mantienen el texto de la codificación elegida. Cambiar Vista no
cambia el ID ni los cursores de captura. Se agrega `skippedDisplayBytes` y
se conserva `skippedDisplayCharacters`; el aviso de atraso visual usa bytes.
La cola está acotada a262144unidades de bytes + caracteres decodificados.

| Módulo | Estado de esta ficha |
| --- | --- |
| Loader | Fuente nominal0.7.4 y documentación; lectura de fuente, sin compilar ni ejecutar. |
| atmega / esp / web | Sin cambios, compilación ni carga por esta ficha. |
| CAD | Sin cambios. |
| Contracts | Comportamiento, límites y procedencia registrados. |

No se ejecutaron tests, compilaciones, validadores, GUI/MCP, COM, flasheos
ni pruebas físicas en esta revisión. El ejemplo de representación es
explicativo, no evidencia de una recepción ejecutada.

## Adaptaciones por archivo

| Archivo | Adaptación |
| --- | --- |
| `AutoFinalRobo2-Loader/src/SerialCapture.cs` | Fragmentos visuales con bytes originales/texto, cola acotada y contadores de omisión por bytes/caracteres; registro y parser separados de Vista. |
| `AutoFinalRobo2-Loader/src/MainForm.Serial.cs` | Selector en vivo, formato ASCII/HEX/texto, historial de reconversión acotado y limpieza consistente. |
| `AutoFinalRobo2-Loader/src/JobState.cs` | Versión nominal0.7.4. |
| `AutoFinalRobo2-Loader/README.md` | Formatos, cambio en vivo y límites de historial. |
| `AutoFinalRobo2-Loader/MCP.md` | Vista independiente de journal/captura y contador adicional. |
| `AutoFinalRobo2-Loader/MANUAL.html` | Manual0.7.4, selector y alcance de limpiar/copiar. |
| `AutoFinalRobo2-Contracts/README.md` | Enlace y estado de la entrega. |
| `AutoFinalRobo2-Contracts/INFORME_ADAPTACION.md` | Estado sin ejecución. |
| `AutoFinalRobo2-Contracts/CHANGELOG.md` | Entrada incremental. |
| `AutoFinalRobo2-Contracts/changes/2026-10-27-30-loader-serial-hex-ascii.md` | Pedido, implementación, límites y procedencia por archivo. |
