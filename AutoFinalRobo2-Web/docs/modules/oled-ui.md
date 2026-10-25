# OLED vinculada al ATmega — AFR2-20261024-09

Actualización14: Pantallas y navegación organiza59identidades como árbol,
incluyendo avisos de retorno al contexto. Seleccionar abre la biblioteca local,
con encoder/historial/firstVisible y scroll del texto seleccionado. Vistas con
telemetría/credenciales no inventan datos ni ejecutan acciones. Cerrar catálogo
desmonta su reloj; el modal global pausa entrada/reloj local.

Controles laterales reutilizables adaptados de AutoMicro: filas1..3 para resaltar,
flechas para hasta3giros de desplazamiento. SW corto confirma, largo principal,
Volver restaura contexto. En remoto cada giro recibe captura/revisión nueva;
sesión/pantalla/selección inesperada o blur/hidden aborta el lote. Polling se
reanuda al terminar. Sin count dinámico reportado se conserva encoder básico.
Sin cambio de wire, firmware, algoritmo de dibujo ni ScreenCodes. Fuente compilada;
interoperabilidad/visual/placas pendientes. [Ficha14](../../../AutoFinalRobo2-Contracts/changes/2026-10-24-14-developer-oled-tree.md).

`src/features/oled/screenCatalog.ts` consume las59 identidades de Contracts
(48nodos/11avisos), con códigos normalizados. No mantiene otra lista de4pantallas.

`firmwareRenderer.ts` es la copia de layouts AVR actuales: menús, vistas,
edición/teclado, avisos genéricos/red, secuencias, iconos y scroll. Primitivas
en `oledRasterizer.ts`; glifos en `font7x10.ts`. `tools/sync-oled-library.mjs`
extrae del C activo las tablas,950bytes de fuente,14XBM y recursos de secuencia;
se ejecuta en inicio/dev/build. No tiene dependencias nuevas. Los hashes revisados
en `firmwareRenderer.sources.json` detienen un build si cambia un layout no portado.

La biblioteca recibe el mismo snapshot preparado/estado/reloj/buffer previo que
el renderer C. Las cadenas representan bytes de texto del AVR (ASCII para glifos);
no equivalen a texto Unicode arbitrario. La vista local expone solo diseños de
menús estáticos, sin comandos ni datos de sensores inventados. No es una captura.

`LiveOledViewer` y el modal comparten el consumidor de framebuffer1024bytes por
EX01v5. Se elimina la reconstrucción antigua desde seis campos incompletos; una
captura ausente se informa como tal. Los controles esperan respuesta del ATmega;
giro selecciona, corto confirma y largo abre principal, atrás solo por Volver.
La captura con clave AP/teclado visible permanece protegida.

Fuente/recursos enlazados y compilación documentados en
[ficha09](../../../AutoFinalRobo2-Contracts/changes/2026-10-24-09-linked-oled-library.md).
Pendientes: comparación diferencial de frames y pruebas visuales/físicas. No se
ejecutan tests/validadores/flasheos por actualizar la skill ni este módulo.
