# Protocol Studio Web — AFR2-20261028-09

Actualización vigente **AFR2-20261028-10**: formularios por comando, estructura
visual del paquete, salidas simultáneas y selección de bloques, cotejados en
fuente con AutoMicro. [Matriz de continuidad](../docs/PROTOCOL_STUDIO_AUTOMICRO.md).
Fuente sin tests, compilación, validadores ni navegador; paquete ESP anterior.

28/10/2026. Herramienta local en React/Vite, accesible desde Desarrollador en
`#/desarrollador/protocol-studio`. Inspirada en Constructor/Traductor de AutoMicro,
con codec y catálogo propios de ROBO2/perfil 2 y EX01v8. No modifica el wire,
`afr2-ws-1`, los endpoints de firmware ni la navegación OLED.

## Entrada, traducción y copia

- HEX con espacios, saltos, pares contiguos o prefijos `0xHH`; arrays C y
  `bytes([...])` de Python, con asignación opcional como `frame = bytes([...])`.
  En arrays, cada entero sin `0x` se lee en decimal y cada `0xHH` en hexadecimal,
  incluso cuando están mezclados. También se elige Decimal o Texto
  UTF-8 explícitamente. Un nombre del catálogo o CMD de un byte consulta su
  definición sin presentarlo como una trama.
- Las líneas completas `--- ... ---` del monitor son metadatos. Se conservan
  aparte, no se extraen sus horas, COM o baudios como bytes. Cada cabecera
  establece un límite de captura: el scanner no completa una trama con bytes
  del siguiente bloque. Las líneas de HEX contiguas sí se unen.
- El parser rechaza tokens desconocidos, bytes fuera de rango, número impar de
  dígitos HEX, entrada mayor de262144caracteres o de65536bytes. No extrae
  coincidencias HEX de palabras arbitrarias ni enmascara números mayores de255.
  Omite líneas vacías, cercas de código y comillas delimitadoras; admite un
  bloque completo entre comillas. No admite columnas de offsets/ASCII de un
  hexdump ni etiquetas de log arbitrarias como si fueran bytes.
- Salidas copiables: HEX (16bytes/fila), RealTerm `0xHH`, array C, Python bytes y
  decimal; texto escapado, UTF-8, ASCII legible y Latin-1. HEX conserva los bytes
  originales. Texto escapado identifica controles/binarios como `\r`, `\n`,
  `\t`, `\xHH`; los saltos visuales tras `\n` solo facilitan la lectura.
  UTF-8 advierte cuando reemplaza secuencias inválidas; ASCII usa `·` para bytes
  no imprimibles. Latin-1 mapea U+00A0..U+00FF directamente y escapa controles.
- Copiar confirma éxito después de resolver Clipboard API. Si no está disponible
  (por ejemplo, en un origen HTTP local), selecciona el texto del bloque y
  explica cómo copiarlo manualmente. No persiste capturas ni las envía a un
  servicio. Los borradores sobreviven al cambio de pestaña dentro del estudio;
  salir de la ruta o recargar los descarta.

La captura humana que motivó el cambio comienza con texto de arranque ESP:
`ets Jan  8 2013,rst cause:2, boot mode:(3,6)`, seguido de más texto y binarios.
El estudio conserva estos bytes y detecta literalmente `rst cause` / `boot mode`;
no deduce integridad del firmware, comunicación ROBO2, flash, SPI ni cableado a
partir del texto o de los datos binarios posteriores. El ejemplo incorporado
es solo un fragmento textual identificado como ejemplo, sin telemetría simulada.

## Scanner y alcance de interpretación

Autoridad de framing: [afr2_wire.h](afr2_wire.h) y [perfil2](UART_PROFILE_2.md).
Cabecera5bytes `52 4F 42 4F 32`; N:u8<=64; tokens `3A A5`; versión01;
rutas31 ESP→Mega /13 Mega→ESP; CMD:u8; payloadN; XOR de todos los bytes previos.
Total12+N, máximo76bytes. Se distingue framing correcto, candidato inválido,
captura incompleta y bloques fuera de protocolo. Un payload completo se consume
según N, sin ejecutar cabeceras interiores. Tokens/versión incorrectos permiten
buscar la siguiente cabecera. Los offsets son posiciones en los bytes extraídos,
no en el texto pegado. El mapa se pagina en24bloques; el stream continuo conserva
todos los bytes. Seleccionar un bloque muestra su estructura, campos, cuerpo,
comprobaciones disponibles y formatos de copia. Una única trama se selecciona
inicialmente; los bytes crudos se inspeccionan al elegir su bloque.

Las tramas normales muestran session:u32LE y transaction:u32LE no cero;
respuestas agregan result:u8 (0..4) y errores exigen N=9. Las longitudes conocidas
del cuerpo se comprueban por catálogo, incluidas BEGIN13/14 y APPEND28/30 de
EX01v6. Las variantes no se correlacionan con el modo de una carga previa.
El catálogo retiene HALL_CAL0x19 solo como retirado. BOOT0x52 se interpreta como
evento BT01 de14bytes sin prefijo de correlación y queda solo para lectura.
NETWORK_STATE53 / NETWORK_EVENT54 se identifican sin decodificación completa.

Se muestran campos de HELLO, EXT_HELLO, ALIVE, estado, sensores base, Hall,
EXT_STATE, ultrasonido, IR, servo, navegación, radar y BT01 cuando hay longitud
suficiente. Los cuerpos también están disponibles en HEX. La semántica es
**parcial**: no replica todas las validaciones de endpoint, no acredita
negociación EX01/capabilities, correspondencia entre petición/respuesta, orden de
transacciones, autenticidad, timeout800ms, gap50ms ni ejecución física. Framing
correcto nunca se anuncia como comando aceptado o acción completada.

## Constructor

Selecciona CMD por familias, ruta, sesión y transacción; agrega el resultado en
respuestas y calcula N/XOR. Conserva little-endian y máximo64
incluido el prefijo. Rechaza los errores de longitud/semántica que conoce el
codec. Los comandos cuyo cuerpo requiere interpretación manual lo indican.
No construye el evento BOOT ni el comando HALL_CAL retirado. Los valores
iniciales sesión1/transacción1 son ejemplos editables, no identidades del robot.

La pestaña inicial es Constructor, como AutoMicro. Las solicitudes cuentan con
formularios para los cuerpos conocidos: presencia, movimiento, rueda, modo,
GOTO, configuración, preferencias, línea, captura/acciones UI, consulta de
trabajo Wi-Fi, secuencias, sistema y sensores. Las consultas sin cuerpo se
preparan automáticamente. GOTO y secuencias exponen variantes con sus longitudes
actuales. Los campos reserved:u32 se anteponen en cero; las identidades HELLO
y EX01 y la confirmación de reinicio son constantes. Respuestas y cuerpos sin
formulario conservan HEX manual. Si se completa el campo manual en solicitudes,
reemplaza los parámetros; vaciarlo vuelve al formulario, conservando su borrador.

Los formularios verifican enteros, representación con signo, rangos y reglas
locales explícitas. No negocian capabilities ni sustituyen límites de velocidad,
geometría, secuencia previa, parada o condiciones del endpoint. Los valores
iniciales son ejemplos editables. Las tarjetas de estructura usan los offsets
ROBO2 reales y muestran sesión/transacción/resultado o la excepción BT01.
Constructor y análisis comparten la misma visualización. Salidas simultáneas:
HEX, RealTerm, C y Python; decimal sigue disponible de forma desplegable.

La acción «Abrir en el traductor» copia el resultado al mismo codec offline.
No hay envío, apertura COM, adquisición de control, nueva API, evento remoto,
ACK, reintento ni temporizador de transporte. La conexión general existente de
la aplicación conserva su comportamiento independiente del estudio.

## Estado y despliegue

| Módulo | Estado de esta entrega |
|---|---|
| web | Adaptado en fuente; sin compilación, tests, validadores ni navegador |
| atmega | Sin cambios; no se acredita nuevo hardware o firmware probado |
| esp | Firmware y `data/dist` sin cambios; nuevo paquete Web pendiente |

No se ejecutan scripts heredados de AutoMicro. Autorías y referencia por archivo
en [procedencia02](../provenance/web-protocol-studio-essence-20261028.json);
[procedencia01](../provenance/web-protocol-studio-20261028.json) conserva el estado
anterior de los archivos antes de la ampliación.
Compilación, empaquetado/promoción ESP, revisión de navegador y pruebas de
protocolo quedan pendientes de pedido expreso; no hay flasheo de esta entrega.
