# AFR2-20261028-12 — Conversor universal HEX / ASCII en el Loader

28/10/2026. Pedido humano: conversión de HEX a ASCII sin asociar los bytes a
comandos; aclaración explícita de que corresponde al Loader, no a la Web.

Loader **0.9.2 en fuente** agrega la pestaña **Conversor HEX / ASCII**, accesible
también desde un botón del Monitor serie. Se usa sin abrir COM, incluso durante
un trabajo, porque no comparte el puerto, estados de operación ni buffers del
monitor. La entrada es texto pegado/escrito; no captura tráfico por sí misma.

- **HEX → ASCII** admite pares HH, tokens0xHH, pares continuos, separadores de
  espacios/saltos/coma/punto y coma y las cabeceras completas `--- ... ---` del
  monitor. Las cabeceras se omiten y cuentan como metadatos; no aportan bytes.
  Comillas exteriores y cercas hex/text de código se pueden omitir. No extrae
  coincidencias de palabras, columnas de offsets/ASCII ni etiquetas de logs.
- **Vista legible** conserva CR/LF/tab reales. Los demás controles (incluidos
  NUL y DEL) y bytes80..FF se muestran como `\xHH`, sin sustituciones por `?`.
  **Vista escapada** representa CR/LF/tab como `\r`, `\n`, `\t` y escapa la barra
  inversa. El cambio de vista usa los bytes retenidos, sin reinterpretar entrada.
- **ASCII literal → HEX** acepta exclusivamente caracteres0..127, con sus saltos
  reales. `\n` o `\x41` escritos se codifican como caracteres literales; no se
  ejecuta una gramática de escapes. Un carácter fuera de ASCII produce error con
  posición; no se convierte implícitamente a UTF-8 o Latin-1 ni se reemplaza.
- Copiar resultado y Copiar HEX normalizado conservan la política de copia y
  toast del Loader. El HEX normalizado usa16bytes por fila y siempre se obtiene
  de los bytes originales. Pegar, Limpiar y Ctrl+Enter completan el flujo.
- Hasta262144caracteres de entrada y65536bytes. Entrada inválida o excesiva no
  produce resultado parcial. Editar la entrada o cambiar dirección elimina el
  resultado anterior para no mostrarlo como traducción del texto nuevo.

El conversor no busca cabeceras ROBO2/UNER, CMD, resultados ni texto de arranque.
`41` representa el byte ASCII A; `02` es control y se muestra `\x02`, sin nombre
de comando. La lógica pura en `HexAsciiConverter.cs` depende solo de tipos y
utilidades estándar .NET; no utiliza BootObservation, SerialCapture, Operations
ni catálogos. La UI no agrega eventos de dispositivo, jobs MCP ni logs del
contenido pegado. No guarda la entrada/salida en perfiles ni disco.

La fuente Web transitoria iniciada por la interpretación anterior se retiró
antes de registrarse. Web conserva exactamente el commit de integración
`e13605b`; Protocol Studio previo se mantiene. No se cambia el monitor existente,
el transporte de flasheo, backups, preferencias o permisos MCP.

| Módulo | Estado real de esta ficha |
|---|---|
| Loader | Fuente0.9.2 implementada; sin compilar ni ejecutar |
| Web | Sin cambios por el conversor universal |
| atmega / esp | Sin cambios; no se abre COM ni se promueve data/dist |
| CAD | Sin intervención |

Solo lectura/revisión manual de fuente y diferencias Git. Sin tests,
compilaciones, validadores, GUI, puertos, hardware ni flasheos. No se afirma
interacción o presentación comprobada. No se actualiza un ejecutable ya abierto.
La integración remota/local anterior quedó en `e13605b`, con resguardo stash
conservado. [Procedencia por archivo](../provenance/loader-conversor-universal-20261028.json).
