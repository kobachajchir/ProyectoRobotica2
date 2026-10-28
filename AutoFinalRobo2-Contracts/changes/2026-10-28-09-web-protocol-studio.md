# AFR2-20261028-09 — Protocol Studio desde Desarrollador

28/10/2026. Pedido humano: agregar a la Web un Protocol Studio similar al de
AutoMicro para copiar y traducir comandos/capturas HEX del monitor serie.

Se agregó una tarjeta de acceso en Desarrollador, una ruta lazy con retorno y
título propios y dos tareas: Traductor y Constructor ROBO2. Se reutilizan
SectionHeader, TaskSections, Field, Button, UiIcon y notify, identidad0.1.9 y
tokens de ambos temas. No se agregan dependencias ni recursos gráficos.

El Traductor omite las cabeceras del monitor como metadatos, conserva bytes y
separa sesiones de captura. Convierte HEX, RealTerm, C, Python, decimal, UTF-8,
ASCII, Latin-1 y escapes de binarios. Informa UTF-8 inválido y conserva el texto
de arranque ESP del ejemplo. Desglosa tramas/candidatos/incompletas/bloques crudos
con offsets y copia por bloque. Los errores de entrada no producen traducciones
parciales. Límites explícitos64KiB/262144caracteres y paginación24bloques.

El Constructor usa ROBO2/perfil2 y EX01v8, rutas31/13, sesión/transacción LE,
resultados0..4, longitudes conocidas y XOR. Adapta variantes de secuencias de
EX01v6, reconoce BT01 sin correlación en lectura y HALL_CAL retirado. Cuerpos no
decodificados por completo quedan explícitamente manuales. Copia con confirmación
real o selección para copia manual si Clipboard API no está disponible. Ninguna
acción del estudio transmite comandos ni abre puertos.

Contrato: [WEB_PROTOCOL_STUDIO](../contracts/WEB_PROTOCOL_STUDIO.md).
Procedencia/adaptación por archivo: [manifest](../provenance/web-protocol-studio-20261028.json).
Identidad aplicada: VIS-045, composición Propuesta, aspecto No verificado.

| Módulo | Estado real |
|---|---|
| web | Fuente implementada; sin tests, build, validadores ni navegador |
| atmega | Sin cambios de esta ficha |
| esp | Fuente y paquete LittleFS sin cambios de esta ficha |
| Loader / CAD | Sin intervención |

Lectura manual de fuentes/contratos y diff; sin ejecución del codec, capturas,
tests, compilaciones, validadores, servidor, hardware ni flasheos. El paquete
actual del ESP no incluye esta nueva fuente. No se afirma interoperabilidad,
memoria suficiente, aceptación del diseño ni resultado físico. Cambios locales
previos del usuario preservados; sin commit/push.
