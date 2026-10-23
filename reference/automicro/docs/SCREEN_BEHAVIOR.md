# Comportamiento de pantallas y teclado

## Calibración de motores — 0x040104

AMI-20261007-01 agrega MOTOR_CALIBRATION dentro de Motores, con icono Tool. Encoder rota para ajustar intensidad/rampa; encoder corto cambia el campo; USER corto inicia, cancela o desarma y guarda un ensayo completado; encoder largo cancela y vuelve a Motores. USER largo conserva la prioridad global del router. Renderer F4: OledUtils_RenderMotorCalibration; métricas de tiempo y PWM por estado 0x27. Web y Qt identifican el nuevo código; los reportes existentes no exponen todos sus valores, por lo que no se afirma reconstrucción idéntica del OLED. Fuente implementada, sin compilación ni validación física.

## Orientacion base del encoder fisico

AMI-20261019-01 (F4 c544378): el muestreo del encoder invierte el delta fisico despues de corregir el wrap y antes de aplicar la preferencia Invertir giro. Con la misma preferencia guardada, el giro que antes avanzaba/aumentaba ahora retrocede/disminuye, y viceversa, en menus, ajustes y OSK. No se modifica la preferencia ni su valor predeterminado.

Los eventos logicos, handlers y transiciones conservan su significado. Los comandos remotos 0x57/0x58 se despachan directamente al router y conservan su sentido; la tabla OSK describe esos eventos logicos. No cambian payloads ni reportes: el estado OSK sigue sin publicarse por 0x52/0x95/0x96. Implementado solo en fuente; pendiente compilar y cargar F4, sin pruebas ni validaciones.

## Prioridad de entradas

F4 enruta primero permisos; encoder puede cerrar una notificación; luego aplica bloqueo; USER largo es global y vuelve al dashboard antes del eventManager de la pantalla; finalmente procesa el handler activo. No describir un botón solo por lo que dice el handler OSK: debe pasar por esas prioridades. El cliente adjunta screenCode actual; si quedó viejo recibe status2 y debe resincronizar.

## Teclado OSK

Evidencia: Core/Src/osk.c, osk_integration.c y ui_event_router.c. La integración instala renderFn/eventManager temporalmente y guarda contexto anterior. Alpha usa código efectivo 0x090101, numérico 0x090102, aunque el invocador haya declarado 0x02020E/F/10 o 0x030601/602. Documentar código declarado y observado por separado.

| Entrada/estado | Transición y efecto |
|---|---|
| Giro CCW (0x57) | OSK_OnRight: avanza una tecla. Última columna pasa a primera columna de fila siguiente; última tecla vuelve a primera. |
| Giro CW (0x58) | OSK_OnLeft: retrocede una tecla. Primera columna pasa a última columna de fila anterior; primera tecla vuelve a última. |
| Encoder corto | Activa tecla actual; no mueve cursor automáticamente por un ACK. |
| Encoder largo | OSK no lo procesa en su switch; router puede declararlo consumido sin cambio y responder status0. |
| Shift | Alterna mayúsculas/minúsculas, conserva fila/columna y aplica clamp según layout. |
| Symbols/ABC | Cambia layout y restablece cursor a fila0,columna0. |
| Backspace | Borra último carácter; actualiza indicador de decimal en numérico. |
| Espacio | Añade un espacio respetando capacidad. |
| Signo numérico | Cambia el indicador de signo según configuración de entrada. |
| Decimal numérico | Respeta permiso y decimal único; no es movimiento de fila. |
| OK | Valida longitud/dígitos/rango según invocador. Inválido conserva OSK activo con validationError; válido restaura contexto y después llama callback. |
| USER corto | OnBack cancela y restaura contexto. |
| USER largo | Interceptado globalmente por router: dashboard. No prometer que equivale a cancelar desde OSK. |

El recorrido es secuencial por filas con wrap; los comentarios que lo llaman serpentina no describen inversión de filas implementada. No invertir filas en Web. Hay contradicciones de constantes/counts del layout Symbols frente a arrays: queda revisión del productor pendiente, fuera de A–F; no arreglar por intuición.

Los reportes 0x52/0x95/0x96 no publican buffer, fila/columna, Shift, Symbols, validación ni límites del invocador. Web identifica teclado y conserva acciones remotas existentes, pero no dibuja un cursor inferido ni texto local como si fuese del equipo. Para reproducir teclado completo se necesita una futura ficha de snapshot, autorizada e implementada en productor y puente.

## Menús, variantes y cobertura

Selección 0-based; click usa posición visible 1-based. Ventana de tres elementos se deriva de selectedIndex y lista conocida, no se confunde posición global y visible. Test Teclado: ASCII, Numérico, Volver. Sistema: Reiniciar a BootV2, Reiniciar a BootST, Volver; ambas confirmaciones comparten 0x050501 y el reporte no distingue destino ni opción seleccionada: identidad-only. Estado IR: Objetos, Línea, Luces, Volver; etiquetas ON/OFF únicamente con estado confirmado GET/evento IR. Sin selectedIndex/itemCount o estado IR completo queda identidad-only.

Favoritos dependen de lista dinámica no reportada. Ambiente IR, bootloader y teclados no tienen snapshot completo por estos reportes. Las notificaciones Objetos/Línea/Luces ON/OFF comparten sus códigos; el último GET IR no prueba qué mensaje transitorio se estaba mostrando. No usarlo para elegir esa variante. No crear confirmaciones locales para accionar reinicios.

Los 17 códigos del informe están incluidos en Web. La presencia de constantes no prueba fidelidad visual. `remediatedScreens.ts` diferencia reported-menu e identity-only. Los renders de códigos previamente implementados conservan su recorrido. Informe histórico 139/122 permanece como auditoría; no se anuncia nueva paridad validada.

## Mantenimiento

La skill central extrae evidencia y callGraph acotado de revisiones explícitas. Los overlays manuales de este documento resuelven semántica del teclado/router; la extracción sola deja manual-review. Nuevos screen codes, handlers, payloads, variantes o gestos compartidos requieren ficha AMI y actualización de docs/catálogo. Las pruebas/CI del informe están diferidas, no completadas.
