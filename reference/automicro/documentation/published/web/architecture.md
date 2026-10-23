# Cliente Web — arquitectura, datos y réplica OLED

ID: `web:architecture`. Fotografía documental del checkout del 20-10-2026.
[Índice de 34 unidades](README.md). Sin tests, build, navegador, despliegue ni
comparación de píxeles. Los hashes representan archivos de trabajo.

## Responsabilidad y capas

La Web presenta/controla el equipo a través del ESP. El estado confirmado del
dispositivo llega por respuestas/eventos; una selección local, un formulario o
un slider constituyen intención hasta recibir el resultado que corresponda.

| Capa | Responsabilidad y fronteras |
| --- | --- |
| Arranque/rutas | `main`, App, RootLayout y navegación montan providers y páginas; acceso a una ruta no demuestra permiso F4. |
| Transporte | `wsApi`, `EspClient`, provider y hooks construyen sobres, administran socket/pending y entregan eventos. |
| Estado | Contextos de usuario, modo, pantalla, IR y Wi-Fi mantienen proyecciones del dispositivo y ciclo de suscripción. No son un handle de memoria del F4. |
| Dominio | AutoMotion, balance, línea y calibración traducen intenciones a operaciones y gestionan respuestas/progreso. |
| Visor OLED | ScreenContext → resolver → builders → OledCommandPreview. La implementación actual reconstruye comandos TS y SVG; la meta es copiar/adaptar toda la biblioteca OLED F4 y alimentar el mismo estado. |
| Editor OLED | Documento/assets/geometría → rasterizador → protocolo de transferencia. Produce una imagen para enviar al F4; es distinto del visor del estado local F4. |
| Studio y Docs | Herramientas de comandos y documentación consumen catálogos/contenido; mostrar un comando o constante no acredita handler ni compatibilidad. |
| Escena 3D | Modelo, cámara y geometría IR interpretan datos con convenciones visuales propias. Su WASM DRACO no es una implementación de OLED. |

<a id="handles"></a>
## Qué sustituye al handle en TypeScript

| Forma | Propiedad y exposición | Precaución |
| --- | --- | --- |
| Instancia `EspClient` | Map de pendientes, sender, listeners, temporizadores. Request devuelve Promise; eventos llegan por suscripción. | Un listener se debe retirar; una respuesta debe completar el requestId/command correcto. |
| Provider/contexto | Mantiene estado y exporta valores/callbacks a consumidores. | Remontaje/reconexión puede invalidar valores previos. No confundir dato de otro contexto con el snapshot del renderer. |
| Hook | Agrupa estado, refs y efectos de una operación/feature. | Closure puede capturar valores antiguos; declarar dependencia y limpiar timers/listeners al terminar. |
| Props | Un componente recibe valor y funciones como `onChange/onClose/onSelect`. | Callback emite intención al dueño; no confirma por sí solo operación física. |
| Objeto/documento | Modelo serializable del editor o payload local, con arrays/recursos. | Su forma TS no es un struct C ni un contrato binario por memoria. |
| Ref | Recurso mutable asociado al montaje, socket, escena o gesto. | No es estado reactivo ni evidencia de dispositivo conectado. |

Las fichas listan firmas, atributos, props, callbacks y hooks encontrados por AST.
Los retornos no anotados figuran como `inferred-by-TypeScript`: no se ejecutó
typechecker. Las descripciones `inferred-name` son pendientes editoriales
identificados; no deben interpretarse como comportamiento revisado exhaustivamente.

<a id="transport"></a>
## Promesas, eventos y autoridad

`EspClient.request` registra correlación y timeout y envía un sobre API 1. Una
respuesta válida resuelve la Promise; un error, desconexión o timeout la rechaza.
`requestAccepted` es evento de admisión en cola, y los eventos de telemetría se
entregan a suscriptores. El cliente no elige el nodo UNER de una sesión; ESP lo
asigna y mantiene generación. Ver [API/cliente](libraries/websocket-client.md)
y [contrato](../shared/wire-contracts.md).

PIN de login, autorización de una pantalla y estado de edición del PIN físico
son objetos diferentes. UserContext y los diálogos Web conservan entrada local;
ScreenContext interpreta reportes F4. No se debe inferir el cursor o los dígitos
del F4 a partir de un formulario local ni publicar secretos para completar un visor.

<a id="oled-target"></a>
## La copia exacta OLED es un contrato de código y de estado

La fuente canónica es el conjunto F4 de primitivas, fuentes, bitmaps, renderers,
wrappers y lógica UI. Copiar solo una fuente o reproducir un texto con SVG no
porta ese conjunto. El renderer Web actual no establece igualdad de píxeles,
y el estado disponible tampoco basta para todas las variantes.

La documentación separa dos pendientes: port del motor/recursos y disponibilidad
del modelo de pantalla. Con ambos resueltos se podrá comprobar fidelidad cuando
se autoricen comparaciones. El detalle de primitivas, ventana de menú, OSK, PIN,
acciones y datos faltantes está en [SCREEN_SYSTEM](../shared/SCREEN_SYSTEM.md).

Hallazgos principales: [motor SVG y port completo](../../review/FINDINGS.md#amdoc-web-003),
[estado inventado](../../review/FINDINGS.md#amdoc-web-002),
[ventana/click](../../review/FINDINGS.md#amdoc-web-004),
[modo GOTO](../../review/FINDINGS.md#amdoc-web-005),
[sensores/unidades](../../review/FINDINGS.md#amdoc-web-006) y
[vistas de línea](../../review/FINDINGS.md#amdoc-web-009).

<a id="docs-adapter"></a>
## Documentación autocontenida y porteo posterior

Contracts mantiene contenido Markdown/JSON, IDs y enlaces de hallazgos. La Web
deberá consumir una versión explícita de ese contenido en el bundle, con tres
destinos `f4`, `esp`, `web`. El modelo actual `DocFirmwareTarget`/variants solo
representa Web y F4. El tercer destino y el selector con píldora móvil/divisor
estable están pendientes en [AMDOC-WEB-001](../../review/FINDINGS.md#amdoc-web-001).

El adaptador deberá conservar ID de módulo, anclas de función y códigos de
pantalla al convertir contenido a rutas/componentes. Un enlace de hallazgo debe
resolver a la sección indicada aunque cambie la presentación. Los JSON permiten
generar navegación, búsqueda y referencias cruzadas sin volver a analizar el
firmware dentro del navegador.

La publicación propuesta es un snapshot del corpus preparado durante el build,
con revisión/fecha y manifest de archivos. No implementar lectura del filesystem
de Contracts desde el ESP o navegador: el producto final debe incluir sus propios
recursos. La documentación consumida debe corresponder a los firmware soportados;
la copia no puede actualizarse silenciosamente desde otro checkout.

El selector debe tener un único valor controlado entre tres opciones, conservar
fondo/divisor y centrar la píldora en el destino activo. Este documento registra
el requisito; no se modificó React/CSS ni se diagnosticó por runtime la causa del
defecto visual reportado.

## Alcance y mantenimiento incremental

Las 34 unidades agrupan módulos de dominio, contextos, componentes y soporte de
`src`. Los CSS se inventarían junto a sus consumidores; node_modules, dist,
artefactos generados y assets binarios no son funciones propias. No se equipara
un componente React con un núcleo agnóstico C: se documenta qué depende de React,
DOM, almacenamiento, Three.js o transporte.

`overlays/web-libraries.json` conserva explicación manual. El AST local se extrae
sin importar la aplicación ni llamar al compilador. Para una modificación local,
refrescar solo el archivo en caché y regenerar la ficha afectada según
[METHOD](../../METHOD.md); luego reconstruir [conexiones](../shared/CONNECTIONS.md).
