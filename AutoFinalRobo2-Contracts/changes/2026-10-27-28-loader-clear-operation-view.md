# AFR2-20261027-28 — Loader0.7.2: limpiar la ventana de Operación

27/10/2026. Pedido del usuario: agregar un botón para limpiar la ventana de
Operación. Adaptación propia asistida por Codex; autoría de Koba Chajchir y
trabajo anterior conservados.

Se agrega **Limpiar ventana** junto a **Copiar registro**. Vacía la consola
y descarta el tramo que ya estaba en su cola de visualización al pulsarlo,
para que ese texto no reaparezca en el siguiente tick. Las líneas nuevas
continúan apareciendo. El botón permanece disponible durante una operación.

No modifica los archivos de log, historial MCP, estados, eventos, captura
serie ni la tarea en curso. Conserva la preferencia Seguir registro. No se
agrega una confirmación ni se escribe una línea nueva en la consola al limpiarla.

| Módulo | Estado de esta ficha |
| --- | --- |
| Loader | Fuente nominal0.7.2 y documentación; revisión de fuente, sin compilar ni ejecutar. |
| atmega / esp / web | Sin cambios, compilación ni carga por esta ficha. |
| CAD | Sin cambios. |
| Contracts | Registro y estados actualizados. |

No se ejecutaron tests, compilaciones, validadores, GUI/MCP, COM, flasheos
ni pruebas físicas en esta revisión.

## Adaptaciones por archivo

| Archivo | Adaptación |
| --- | --- |
| `AutoFinalRobo2-Loader/src/MainForm.cs` | Botón activo durante trabajos y limpieza de consola/cola de visualización. |
| `AutoFinalRobo2-Loader/src/JobState.cs` | Versión nominal0.7.2. |
| `AutoFinalRobo2-Loader/README.md` | Versión y comportamiento de la limpieza. |
| `AutoFinalRobo2-Loader/MCP.md` | Encabezado de versión, sin cambios de API. |
| `AutoFinalRobo2-Loader/MANUAL.html` | Manual0.7.2 y alcance de Limpiar ventana. |
| `AutoFinalRobo2-Contracts/README.md` | Enlace y estado de la entrega. |
| `AutoFinalRobo2-Contracts/INFORME_ADAPTACION.md` | Estado sin ejecución. |
| `AutoFinalRobo2-Contracts/CHANGELOG.md` | Entrada incremental. |
| `AutoFinalRobo2-Contracts/changes/2026-10-27-28-loader-clear-operation-view.md` | Pedido, alcance y procedencia por archivo. |
