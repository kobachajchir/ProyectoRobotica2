# AFR2-20261027-23 — Loader0.6.1: Paquete AT solo en familia AT

27/10/2026. El usuario observa Paquete AT en modo Web propio. El botón se
agregaba incondicionalmente a la fila común de opciones ESP en la entrega22.

Corrección en fuente: se conserva una referencia al botón y empieza oculto.
`SyncEspOptions` aplica `Visible` según EspFirmwareMode=at. El mismo refresco
se usa al iniciar, cambiar de familia, seleccionar/importar perfil y aplicar
perfil por MCP. En Web no ocupa espacio ni participa en la navegación por Tab.
La acción de abrir paquete también comprueba que la familia sea AT y que no
haya trabajo activo o pendiente. No cambia el mapa de carga ni las reglas de
firmware/filesystem.

Autoría de Koba Chajchir conservada. [Adaptaciones por archivo](../provenance/loader-at-visibility-20261027.json).

| Módulo | Estado de esta ficha |
| --- | --- |
| Loader | Fuente nominal0.6.1 y manual; sin compilar ni ejecutar esta revisión. |
| Contracts | Corrección y procedencia registradas. |
| atmega | Sin cambios de fuente, compilación ni carga por esta ficha. |
| esp | Sin cambios de firmware/INI, compilación ni carga por esta ficha. |
| web | Sin cambios de fuente, build ni promoción por esta ficha. |
| CAD | Sin cambios. |

Se efectuaron lecturas y edición de fuentes/documentación. No se ejecutaron
tests, compilaciones, validadores, GUI/MCP, apertura COM, flasheos ni pruebas
físicas. Interacción visual pendiente de comprobación ejecutada con pedido
expreso; editar fuente no actualiza el proceso ya abierto.
