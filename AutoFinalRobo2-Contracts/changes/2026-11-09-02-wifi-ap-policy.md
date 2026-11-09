# AFR2-20261109-02 — AP manual y política de fallback

Versión: 1.9.0 (subversión: opción persistente y detención explícita del AP).

Pedido: preguntar por AP solo con la opción seleccionada; AP elegido a mano o guardado arranca sin buscar/reconectar STA. Buscar redes solo en STA/AP-STA. AP activo muestra Detener AP. Reutilizar los bitmaps de antena/AP y largo suministrados.

Implementación en fuente:
- `askFallback` en wifi.json: true pregunta al OLED cuando falla STA; false (valor inicial si falta la opción) inicia AP automáticamente tras el plazo STA. No cambia el modo guardado. Sin OLED capaz se conserva la recuperación automática. El timeout extra ESP de20s se retira para respetar interacción y cuenta del OLED.
- AP puro mantiene STA deshabilitada incluso al buscar (operación rechazada y fila oculta en OLED/Web). Un modo AP guardado sin credenciales AP válidas usa la identidad AP de provisión, sin pasar a STA. Un cambio manual de modo conserva el mecanismo existente de guardar y reiniciar ESP.
- AP / fallback pasa a Iniciar/Detener AP, Preguntar AP SI/NO/-- y Volver. El modo se elige en Cambiar modo. La política desconocida se muestra -- y no permite modificarla.
- Detener AP apaga solo esa interfaz; conserva STA si estaba activa. Bloquea la recreación automática del AP durante la sesión hasta reiniciar/aplicar modo o iniciarlo explícitamente. En AP puro apagado queda OFF; volver a Iniciar AP conserva AP puro.
- Web nativa: casilla persistente, botón Detener AP y búsqueda solo en STA/AP-STA; biblioteca OLED y simulador vinculados en la misma tarea.
- Los bytes entregados ya estaban exactamente en AVR y Web: AP 15x16/32bytes, largo13x13/26bytes. Se conserva esa identidad y se registra la nueva entrega del usuario. Dashboard usa antena en AP puro aun mientras se informa inicio/detención; STA conectado mantiene su icono Wi-Fi.
- Corregido el receptor `MegaLink`: op13/14 anteriores estaban fuera del máximo12; ahora admite hasta16. Contrato ampliado con op15/16 y byte opcional de política en detalles; ver contrato.

Pantallas compartidas: 010001, 03001f, 030024 y 03004b. Sin nuevos ScreenCodes. Corto/largo al soltar/Volver contextual y Sí/No obligatorio conservados.

Estado: atmega=fuente editada, sin compilar ni cargar; esp=fuente editada, sin compilar ni flashear; web=fuente editada, sin build ni promoción LittleFS; loader/CAD=sin cambios. No se ejecutaron tests, validadores, generadores OLED ni pruebas físicas. Inspección de fuente únicamente; sin paridad raster ni memoria medida. AVR agrega5bytes de estado (3 wifi+2 snapshot); ESP detalle+1byte y dos booleanos de estado. Presupuesto real de memoria/pila pendiente de compilación autorizada.

Autorías anteriores conservadas. Adaptaciones por archivo y hashes de recursos en provenance/wifi-ap-policy-20261109-02.json. Recursos generados pendientes de regeneración autorizada; la copia TS usa las filas/algoritmos escritos y los bytes existentes que coinciden con el pedido.
