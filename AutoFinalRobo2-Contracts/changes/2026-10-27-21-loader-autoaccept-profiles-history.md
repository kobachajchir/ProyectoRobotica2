# AFR2-20261027-21 — Loader 0.5: aceptación automática, perfiles y copia

27/10/2026. Pedido humano: casilla para autoaceptar solicitudes Codex/MCP,
selección múltiple Ctrl/Shift y copia del historial con toast, selector de
perfiles guardados, presentación de errores del monitor y quitar la selección
inicial de instrucciones en asistentes de cableado/reset/K.

## Cambios en fuente

- **Autoaceptar todas las solicitudes de Codex (esta sesión)** empieza
  desmarcada. Activarla acepta la pendiente y nuevas operaciones/cambios de
  perfil admitidos, sin revisión MCP adicional. Las solicitudes de cancelación
  de un trabajo activo se aceptan sin otro diálogo MCP. Se registra el modo
  manual/automático; desmarcar no cancela trabajos iniciados. Los asistentes
  físicos de cableado, K/RST y confirmación final de escritura siguen activos.
- MCP agrega `loader_status.autoAcceptRequests`, `autoAccepted` en reserva,
  estado `accepted` previo al inicio y `approvalMode` en detalle/snapshot/resumen.
  `executed:false` describe el instante de reserva, no el resultado posterior.
  `accepted` no acredita ejecución ni escritura: consultar el mismo jobId.
  Deduplicación, exclusión, límites y herramientas/argumentos públicos se
  conservan. No se agregan shell libre ni reintentos automáticos. Un diálogo
  de perfiles abierto bloquea propuestas para no cambiar datos durante su guardado.
- Historial con FullRowSelect/MultiSelect: Ctrl+clic agrega/quita filas;
  Shift+clic/teclado selecciona rangos en ambas direcciones. El refresco actualiza
  filas existentes y conserva selección, ancla y posición. Ctrl+C/Ctrl+Insert,
  botón y menú contextual copian solo filas elegidas, con encabezados, orden
  visible y TSV escapado; no incluyen filas intermedias sin seleccionar.
- **Copiado al portapapeles** aparece abajo tras copiar con éxito, también en
  botones de registro/monitor/errores/MCP/detalles. Toast dentro de la ventana
  de origen, sin tomar foco; un fallo de portapapeles muestra otro mensaje.
- Selector superior de los JSON en LOCALAPPDATA/AutoFinalRobo2Loader/profiles,
  con nombre/archivo. Guardar actualiza el activo; Guardar como crea otro local.
  Importar permite revisar y Guardar incorpora una copia; Exportar sigue portable.
  `active-profile.txt` recuerda el último elegido; se consultan cambios sin
  guardar antes de cambiar. Un perfil ilegible conserva el visible. Schema1
  y guardado .bak existentes se mantienen; no se guardan datos en Git.
- Monitor: tarjeta con título/origen y mensaje desplazable, icono24px alineado,
  fondos por estado y primera letra mayúscula solo en presentación. Eventos y
  errores conserva su estructura y texto original, al igual que los informes.
- Asistentes de cableado/K-RST ponen el foco en primera casilla o Continuar,
  con instrucciones sin selección inicial. Seleccionar/copiar deliberadamente
  sigue disponible. Se conserva el flujo de apertura/liberación del COM.

## Procedencia y estado real

Adaptación propia sobre fuentes de Koba Chajchir y trabajo local0.4.0 conservado.
[Por archivo](../provenance/loader-usability-20261027.json),
[README](../../AutoFinalRobo2-Loader/README.md),
[manual](../../AutoFinalRobo2-Loader/MANUAL.html#mcp),
[MCP](../../AutoFinalRobo2-Loader/MCP.md).

| Módulo | Estado de esta ficha |
| --- | --- |
| Loader | Fuente nominal0.5.0; sin compilar ni ejecutar esta revisión. |
| Contracts | Ficha, procedencia y enlaces actualizados. |
| atmega | Sin cambios de firmware, build ni carga por esta ficha. |
| esp | Sin cambios de firmware, build, consulta COM ni carga por esta ficha. |
| web | Sin cambios de fuente, build ni promoción por esta ficha. |
| CAD | Sin cambios. |

Se efectuaron lecturas y edición de fuentes/documentación. No se ejecutaron
tests, compilaciones, validadores, aplicación, llamadas al Loader MCP, apertura
COM, flasheos ni pruebas físicas durante esta revisión. Interfaz y flujo
automático quedan pendientes de comprobación ejecutada con pedido expreso.
El Loader ya abierto conserva su versión: editar fuentes no actualiza el
ejecutable en ejecución ni inicia solicitudes anteriores.
