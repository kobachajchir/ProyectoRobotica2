# AFR2-20261028-23 — Loader 0.9.5: flujo, manual y Servidor MCP

Autoría: Koba Chajchir / AFR2; adaptación mediante Codex.
Pedido humano: correcciones de operación, compilación previa, detección,
FileSystem/capacidad, verificación única, aviso KEY, prompts MCP, monitor,
manual/cableado, accesos visibles, distribución y marca. Conserva versión0.9.5.
Las fichas18–22 existentes de otros trabajos se preservan; esta entrega usa23.

## Flujo implementado

- Instalar flasher y cualquier operación aceptada llevan a Operación. Una nueva
  operación marca Seguir registro, drena la salida y va al final. El usuario puede
  volver a desmarcar/pausar el seguimiento para leer.
- Instalar el flasher exige D18/D19 **desconectados del ESP**, D0/D1 aislados,
  RESET Mega libre y potencia de actuadores desconectada. Tras completar, el
  recordatorio indica apagar y reconectar D18→E_RX / E_TX→D19 para usar el puente.
  Esto distingue instalación del servicio de su uso posterior.
- Detectar ESP incluye «ESP Detectado», completado solo por respuesta ROM y
  capacidad confirmada, no por observar boot mode o confirmar cableado.
- Paquete AT oculta Sección del paquete; en GUI queda Firmware ESP. Los esquemas
  internos y las funciones existentes de paquete AT no cambian por el rótulo.
- Firmware propio ofrece Firmware ESP / FileSystem / Firmware ESP + FileSystem.
  Confirmar tamaño de flash abre un diálogo de1/2/4/8/16MiB antes de mostrar
  particiones. Es una declaración humana de esta sesión, no medición. La carga
  mantiene la consulta física y exige coincidencia antes de escribir.
- El código conserva el máximo real del linker. No se atribuye todo el espacio
  liberado por FileSystem al código: el core ESP8266 actual limita el programa
  principal a aproximadamente1MiB. El resumen distingue espacio reservado/no
  asignable a código. No se alteran linkers ni firmware para inventar esa capacidad.
- Flashear permanece deshabilitado hasta completar la compilación de su sección
  en la sesión actual. Firmware y FileSystem requieren preparaciones independientes;
  ambos requiere ambas. AT se prepara revisando sus BIN precompilados y su mapa.
  Recibos de sesión incluyen perfil, contenido de fuentes relevantes, linker/INI,
  datos de FileSystem e imágenes. Se comprueban metadatos para la presentación y
  hashes completos al cargar; una compilación fallida invalida su preparación.
  La regla se exige también en Operations para las solicitudes MCP. No bloquea
  restaurar backups ya contrastados ni instalar el servicio que se compila dentro
  de su operación. No se considera preparado un BIN externo por existir.
- Se retiran la consulta read_mac y el bucle de recuperación/verify_flash tras la
  escritura ESP. Se usa la comprobación de hash integrada de write_flash, en su
  misma sesión. El Loader exige salida0 y una línea «Hash of data verified» por
  cada imagen; si falta evidencia, falla sin marcar Verified. No lanza otra
  escritura ni verificación desde el Loader. Los reintentos internos propios de
  la versión de esptool no se presentan como controlados por la interfaz.
- Primer porcentaje real «Writing at…%» de escritura ESP genera un aviso no modal
  «Ya podés soltar KEY», una vez por operación. Connecting no lo dispara. No pausa
  el proceso, no manda reset y recuerda no pulsar RST ni cortar alimentación.
  En detección sin escritura se espera al fin de la consulta. Sin porcentaje
  reconocido no se adelanta el aviso; la finalización conserva su recordatorio.

## Interfaz, monitor y ayuda

- Servidor MCP reemplaza el título Codex/MCP. Mostrar prompt agregar MCP ofrece
  texto genérico con nombre afr2-loader, stdio, comando, argumentos y ruta local
  de Mcp.ps1 para cualquier cliente compatible. No se inventa URL HTTP.
- Ejecutar prueba abre un prompt para Codex con sessionId/testId y caducidad de
  cinco minutos, y Copiar prompt de prueba. Prepararlo no ejecuta la prueba ni
  acredita conexión. No abre COM ni activa autoaceptación.
- Copiar texto del monitor, por botón, atajos o WM_COPY, ofrece Mostrar en
  conversor HEX. Conserva formato y copia para reutilizarlos sin selección activa.
  Se pega como HEX si la vista copiada era HEX; como ASCII literal si era texto.
  No convierte ni transmite automáticamente.
- Eventos: Ver detalles rellena el alto y ancho de su celda de acciones; no cambia
  a amarillo el botón ni su fondo al abrir el detalle. Se mantiene selección
  normal de la tabla. No se eliminan las demás columnas para extender el botón
  sobre ellas.
- Leer ATmega abre la sección exacta del manual, sin abrir COM. Los enlaces de
  ayuda de configuración tienen un ancla por campo y el visor desplaza al ancla
  al terminar de cargar. El manual sigue en ventana propia de la aplicación.
- Navegación del manual con bloques separados compatibles con el visor integrado;
  atajos y comandos en rojo sobre fondos diferenciados. Se retira el aviso de
  «fuente sin compilar/abierto» indicado por el usuario. La evidencia de esta
  revisión permanece en Contracts, sin atribuir validación física al manual.
- Historial indica Doble clic: detalles y atajos en cajas separadas. Guardar perfil
  conserva icono/texto con más padding. Información del flasher queda junto al
  título y Flashear último backup a la izquierda de Cargar flasher.
- Acerca de y manual explican el propósito académico del auto Mecanum e incluyen
  las imágenes UNER/FCAL aportadas. Se usa afr2-loader-logo-blue.png en la ventana
  Acerca de, el manual y el icono de la ventana en ejecución. No se reescribe la
  imagen fuente seleccionada en ficha22. El icono de archivo EXE de Explorer no
  se ha sustituido por un recurso win32icon embebido.

## Cableado explicado y límite técnico

El manual usa documentación local hardware/WIFI_SHIELD.md y hardware/FLASHING.md,
manual OSOYOO y fuente avr2_uart1.c. D18/TX1/PD3 y D19/RX1/PD2 son pines del Mega
y USART1 es su periférico de comunicación con ESP. USART0 corresponde a D0/RX0/PE0
y D1/TX0/PE1, ya conectados al conversor USB del Mega.

RESET Mega→GND detiene el ATmega y libera sus drivers; **no redirige internamente
USART1 ni conecta automáticamente ESP a USB**. Para leer ESP se documenta el cruce
externo E_TX→D1 y, si se requiere envío, D0→E_RX por la ruta identificada del shield,
con D18/D19 aislados. No se confunde selector con UART directo de ESP.

Para observar USART1 del ATmega por USB: D18→D1, y D0→D19 solo si se necesita
comunicación bidireccional; RESET del Mega libre y ESP aislado. Esto requiere que
USART0 no conduzca los nodos: no unir TX1/TX0 activos ni mantener estos puentes
durante carga/reset/bootloader. Alternativa de observación con receptor USB–TTL
compatible con niveles y masa común. El canal del producto es binario: vista HEX,
sin prometer texto espontáneo ni debug USART0 implementado. Otros trabajos de
diagnóstico pueden usar USART0: revisar el firmware efectivamente cargado.

## Evidencia y estado

Loader: compilación C# de interfaz y revisión visual autorizada con capturas de
Operación (Flashear bloqueado), Flasher reorganizado, ayuda en ancla exacta,
Servidor MCP, prompt genérico y ventana de prompt de prueba, y Acerca de con logo
AFR2/UNER. No se ejecutaron compilaciones de ATmega/ESP/Web, tests de firmware,
validadores, detecciones físicas, apertura COM, envíos TX ni flasheos por el agente.
El prompt de conexión se generó para observar la interfaz; no se invocó
loader_connection_test. No se acredita conexión por solo preparar el prompt.

No se ha comprobado en hardware la verificación integrada, la nueva liberación
KEY ni el cableado. La preparación por sección se revisó en fuente; no se compilaron
firmwares para habilitarla en la GUI. ATmega/ESP/Web/CAD sin cambios de producto
por esta ficha; se preservan modificaciones concurrentes ajenas.

Referencias técnicas consultadas:
- [Esptool: verificación integrada](https://docs.espressif.com/projects/esptool/en/latest/esp8266/esptool/advanced-commands.html).
- [ESP8266 Core: límite del programa](https://arduino-esp8266.readthedocs.io/_/downloads/en/3.0.2/pdf/).

Procedencia por archivo: ../provenance/loader-flujo-manual-mcp-20261028.json.
