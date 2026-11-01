# AFR2-20261028-16 — Loader 0.9.5: flujo por placa y envío serie

Autoría: Koba Chajchir, fuentes propias AFR2; adaptación mediante Codex.
Pedido humano: reorganización integral de interfaz/funcionamiento, ampliada con
envío serie inspirado en las funciones indicadas de RealTerm y configuración por
placa. Versión solicitada explícitamente: **0.9.5**.

## Operación y flasher

- Destino visible ATmega / ESP, puerto junto a COM, actualización separada y
  detección vinculada al destino. Se conserva el índice interno de operaciones
  para los trabajos/MCP existentes; no se cambia su esquema por la presentación.
- ATmega muestra firmware completo o solo comunicación. Esta interpretación de
  las «dos versiones» usa los dos HEX propios existentes; no crea otro firmware.
  La selección de comunicación solo afecta a Flash en la GUI, no a MCP/perfiles.
- ESP muestra variante Web propio / Paquete AT y sección firmware / Web / ambos.
  La sección decide qué imagen se escribe. No cambia silenciosamente el linker
  del firmware al elegir firmware solo. El filesystem se configura por layouts
  existentes; ya no hay una casilla redundante. Un AT monolítico no inventa un FS.
- ESP/ATmega ocultan los controles de la otra placa. Flasher ATmega queda al lado
  de Operación, con instalación abajo a la derecha, backup/detección separados,
  nota de baudios y detalles del servicio. Detección lleva primero a Operación.
- Se conservan las etapas y feedback0.9.3. Barra de actividad en el estado global;
  copiar/limpiar a la izquierda y compilar/flashear con flecha de 32px y tamaño
  compartido. Preferencias y manual dejan de ser pestañas/acciones redundantes.

## Configuración, preferencias y ayudas

- Navegación General / ATmega / ESP con subsecciones de herramientas, conexión,
  firmware, paquete AT, Web/filesystem y flash. Se conserva búsqueda combinada.
- Ayuda por campo y enlaces a la sección correspondiente del manual, abierto
  en ventana propia. Guardar/Guardar como comparten botón dividido a la derecha.
- Opciones → Preferencias abre Backups ATmega, Asistentes o Registros. Modos de
  revisión: casillas requeridas, marcadas por defecto o sin casillas. La última
  opción conserva confirmación de continuar y las comprobaciones técnicas.
- Asistentes con casillas verticales, Seleccionar todos / aceptar todo y ayuda.
  La referencia visible al botón pasa a Q por corrección del usuario. No cambia
  la lógica GPIO/ROM. Se retiran de los asistentes los avisos repetidos solicitados
  sobre 3,3V/5V; esto no añade una afirmación de tolerancia eléctrica al producto.
- COM: comprobación puntual con handle Windows exclusivo antes de una operación
  serie, después de cerrar el monitor. No envía bytes ni configura DTR/RTS.
  Se puede repetir desde el asistente. No reserva el puerto hasta esptool ni
  acredita cableado; el controlador del dispositivo puede reaccionar al abrir.
- Registros: ver carpeta o limpiar con cantidad/tamaño y confirmación explícita.
  No borra perfiles ni backups; bloqueado con jobs/monitor activos, sin recursión.

## Monitor, conversor, Eventos y MCP

- Monitor separa puerto/acciones, baudios/codificación y selector de vista encima
  del cuadro a la derecha. Tarjeta de estado con icono centrado y título mayor.
  Herramientas de vista agrupadas; el conversor aparece al seleccionar texto
  en vista ASCII/decodificada, copia la selección y la coloca como ASCII literal
  en el conversor sin convertir automáticamente. No promete recuperar bytes
  originales a partir de una selección textual decodificada.
- Enviar: editor libre, ASCII literal estricto, HEX HH/0xHH o números decimales
  0..255. CR y LF opcionales, en ese orden, incluidos en cada repetición. Permite
  terminadores solos. Límite 64KiB por envío, 1000 repeticiones, pausa0..60000ms.
  No importa la codificación RX para el modo ASCII literal ni interpreta escapes.
- TX pasa por una cola del mismo worker dueño del COM. No hay segundo puerto.
  Detener, cierre, cambio de baud o transferencia a flasher cancelan pendientes.
  Escribe bloques cortos con comprobación de cancelación entre bloques y timeout
  de2s por Write; no reintenta ni retira bytes ya aceptados por el puerto.
- RX conserva .rx.bin/.rx.txt. TX registra solicitud en .tx.txt y bloques
  aceptados por Write en .tx.bin, con estado final o cancelación. Una excepción
  de Write puede haber transmitido parte del bloque incierto, anotado en registro.
  «Transmitido» significa aceptado por el driver; no respuesta/ACK del dispositivo.
  No se añade una operación de envío a MCP ni envío al abrir/reconectar.
- Eventos tiene acción Ver detalles y amarillo para el detalle abierto, distinto
  de la selección normal. Copia con el toast común. Conversor mantiene su esencia,
  con controles por sección, vista arriba derecha, notas y Convertir abajo derecha.
- MCP: comando colapsado inicialmente, conector y conexión comprobada separados,
  prueba y explicación contiguas, solicitudes en tabla, rechazar izquierda y
  continuar derecha, opciones de aceptación debajo. No cambia aprobación/deduplicación.
- Márgenes base8/16px, iconos GDI propios y colores semánticos: acción verde,
  detener rojo, información/actualizar azul y navegación/tareas comunes blancas.
  Se aplican también a los formularios propios. No se añade una biblioteca.

## Estado y evidencia

Implementado en fuentes; revisión manual mediante lectura/diff. **Sin tests,
validadores, compilación, ejecución GUI/MCP, COM ni flasheos por el agente.**
La interfaz/DPI, disponibilidad de COM y transmisión requieren comprobación
autorizada posterior. No se afirma que el diseño se haya visto renderizado.
Se conservan conversor0.9.2, feedback0.9.3 y BIN AT reconocido0.9.4.
Atmega/ESP/Web/CAD sin cambios de firmware/producto por esta ficha.

Procedencia: `../provenance/loader-095-interfaz-envio-serie-20261028.json`.
