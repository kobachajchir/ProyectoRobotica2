# Método de documentación AutoMicro — AMDOC 1

Esta carpeta es la fuente documental compartida. La Web será un consumidor del
contenido; su diseño o framework no determina cómo se identifican las librerías.
Los tres manuales F4, ESP y Web comparten estas reglas. Qt permanece como
consumidor identificado del contrato, con su manual interno pendiente.

## Identidad y nombres

| Entidad | Convención | Ejemplo |
| --- | --- | --- |
| Repositorio | `f4`, `esp`, `web`, `qt`, `shared` | `esp` |
| Módulo | `<raíz>:module/<archivo-ancla-relativo>` | `f4:module/Core/Inc/encoder.h` |
| Ficha legible | `published/<raíz>/libraries/<slug>.md` | `published/f4/libraries/encoder.md` |
| Modelo de ficha | mismo nombre, extensión `.json` | `encoder.json` |
| Función | ID módulo + nombre exacto; ancla `fn-<nombre-en-minúscula>` | `#fn-enc_init` |
| Hallazgo | `AMDOC-<RAÍZ>-NNN`, número persistente | `AMDOC-F4-001` |
| Pantalla | `shared:screen/<hex de 6 dígitos en minúscula>` | `shared:screen/030205` |
| Familia de pantalla | `shared:screen-family/<nombre>` | `shared:screen-family/osk` |
| Mensaje compartido | `shared:message/<nombre>/v<versión documental>` | `shared:message/auth-pin/v1` |
| Cambio compartido | ficha `AMI-YYYYMMDD-XX` según la skill central | Solo cuando cambia contrato |

El ID no usa una ruta absoluta ni una posición de array. El slug sirve a la
navegación; el símbolo real conserva mayúsculas, guiones bajos y lenguaje. Ante
renombre se conserva un alias del ID previo, en lugar de renumerar documentación
o hallazgos. `firmware web` se registra como `web` para distinguirlo de `esp`, que
es el firmware que sirve la aplicación y traduce sus mensajes.

## Capas documentales

| Carpeta | Dueño / propósito |
| --- | --- |
| `overlays/` | Explicación editorial, responsabilidades, precauciones y descripciones de API |
| `published/` | Markdown y JSON consumibles, generados desde evidencia y explicación |
| `review/` | Hallazgos de código y cambios documentales asociados |
| `scripts/` | Extracción/generación documental, sin ejecutar código analizado |
| `analysis.config.json` | Raíces e inclusión del inventario general de la skill |
| `../.cache/code-docs/` | Inventario mecánico local, índices y relaciones no resueltas; excluido de Git |

Los archivos de `published/{f4,esp,web}/libraries/` se regeneran: editar su overlay, no solo
su Markdown. Las páginas de arquitectura/contratos escritas manualmente se
mantienen como documentos editoriales; el registro de hallazgos indica sus anclas
exactas. No se debe destruir una corrección editorial al actualizar extracción.

## Orden obligatorio de cada ficha

1. Identidad, responsabilidad y alcance: qué hace y qué no está dentro de su frontera.
2. Modularidad observada: plataforma, dependencias de tipos, llamadas y estado compartido.
3. Handle: qué posee, qué presta y cómo expone datos hacia fuera.
4. Ciclo de vida: construcción, configuración, activación, trabajo, finalización/error.
5. Interacciones: iniciador, receptor, datos, sincronía, cola y orden de acciones.
6. Precauciones: unidades, rangos, contexto IRQ/tarea, buffers, caducidad y reentrada.
7. Hallazgos enlazados por ID.
8. Archivos/includes/hash y evidencia relativa al repositorio.
9. Tipos, campos y callbacks con firma y propósito.
10. Funciones: una línea por función con nombre/firma y descripción corta; separar API e internas.

Un handle público de C no es opaco por llamar `private` al estado ni por entregar
una vista `const`. Documentar si hay copia, vista prestada, tabla, contexto opaco,
buffer del llamador o global activo. Un puntero a función necesita describir
argumentos, retorno, quién lo registra, quién lo invoca y duración del contexto.
Si el contexto de ejecución no se resolvió, se declara pendiente.

## Evidencia y certeza

Cada archivo lleva SHA-256 de sus bytes; se usa el checkout de trabajo, sin
afirmar que equivale a un HEAD limpio. Las líneas facilitan lectura y pueden
desplazarse; el símbolo/ID es el punto estable. El inventario estructural no
preprocesa C, resuelve todas las macros ni garantiza todas las variantes.

`observed` es una declaración/implementación leída; `resolved` enlaza productor y
consumidor concretos; `inferred` es interpretación; `unknown` queda abierto;
`contradicted` conserva dos fuentes incompatibles. Las descripciones de función
marcan `editorial`, `structural` o `inferred-name`. Una etiqueta inferida no debe
ascender a comportamiento confirmado sin leer su implementación y llamadas.

`source-documented` significa documentación de fuente. Construido, probado,
verificado en navegador, desplegado y probado físicamente son estados distintos.
En esta entrega no se ejecutan tests, compilaciones, validadores ni comparaciones
automáticas de paridad, por instrucción del usuario. La generación no acredita
interoperabilidad.

## Contratos entre lenguajes y repositorios

Para cada mensaje registrar productor, consumidor, transporte, ID/nombre, versión,
sentido, correlación, longitud, offsets, tipo wire, endian, unidad/rango, campos
opcionales, permisos, respuesta final, eventos y timeout. Describir por separado
aceptación de cola, ACK y resultado efectivo. La fuente canónica de un dato es
quien lo produce; el consumidor debe mostrar cómo lo decodifica.

No copiar structs por `sizeof`, bitfields, punteros o enums de tamaño implícito
como si fueran contratos portables. C/C++ y TypeScript pueden compartir datos
sin compartir representación en memoria. Ejemplos sintéticos, sin secretos.
Una constante declarada o un catálogo no demuestra que exista handler activo.

## Corrección focalizada de un hallazgo

1. Localizar ID en `review/findings.json`; leer solo código y documentación indicados.
2. Resolver el comportamiento y registrar archivo/símbolo/commit; conservar historia del hallazgo.
3. Modificar el overlay de la librería y los puntos manuales enlazados. Si cambia wire,
   crear ficha AMI y adaptar productores/consumidores con estados reales.
4. Regenerar las fichas afectadas, por ejemplo:

   ```text
   python documentation/scripts/build_reference.py --module encoder
   python documentation/scripts/build_reference.py --module uner-core --module uner-router
   python documentation/scripts/build_multi_reference.py --root esp --module json-bridge
   node documentation/scripts/extract_web.cjs --file src/protocol/screenMenuState.ts
   python documentation/scripts/build_multi_reference.py --root web --module screen-state
   python documentation/scripts/build_screen_reference.py --screen 0x030205
   ```

5. Actualizar estado del hallazgo sin cambiar su ID; no editar a mano copias
   generadas ni relanzar todo el corpus para un cambio local.
6. Publicar `render_findings.py`, `build_connections.py` y `build_snapshot.py`:
   estos tres solo leen los registros/fichas publicados; no vuelven a analizar
   los firmware ni ejecutan tests. No reescribir un estado de validación como
   aprobado por haber regenerado documentación.

La entrada del hallazgo conserva `modules`, `code`, `documentation`, selector
estable y cierre. Las fichas generan el enlace inverso al mismo ID. `revisar`
significa incertidumbre pendiente; `abierto` tampoco equivale a fallo observado
en hardware. Una resolución conserva fecha, explicación y commit; no borra el
antecedente. Una hipótesis descartada se cierra con evidencia.

## Reproducción y límites de herramientas

El inventario general de `document-codebase` se obtiene con su comando `inventory`
y `analysis.config.json`. El script local genera fichas con el modelo `amdoc-1`;
es un adaptador editorial y no pretende ser el schema interno de esa skill.
JSON y Markdown de cada ficha proceden del mismo modelo local.

La extracción es estructural: ramas `#if` pueden aportar varias firmas del mismo
símbolo; las declaraciones/callbacks no prueban llamadas activas. Los `.inc` de
ESP requieren tratarse como partes de una unidad C++, no como librerías
independientes. Las relaciones no resueltas permanecen como límites explícitos,
sin convertir tokens de llamada en dependencias ejecutadas.

## Pantallas, callbacks y relaciones entre manuales

`overlays/screen-families.json` describe disponibilidad y comportamiento por
familia; `screen-overrides.json` precisa diferencias por código sin inventar
estado. `build_screen_reference.py` publica declaraciones/candidatos y fichas
por código. Una identidad puede ser centinela, acción de ítem, invocador o vista:
no exigir renderer dedicado a toda macro. Los bindings candidatos, metadata,
estado disponible, acciones y fidelidad son dimensiones separadas.

El modo `--screen` actualiza solo páginas pedidas y las referencias F4 conocidas
de su manifest previo. Si aparecen referencias nuevas o códigos nuevos, ejecutar
extracción general explícita; no usar una actualización focalizada para afirmar
que se descubrieron todos los callers. Las páginas no seleccionadas conservan
su evidencia previa. No ejecuta comparador de paridad.

En Web `extract_web.cjs --file` refresca solo archivos pedidos en la caché AST;
sin ese flag realiza el inventario inicial. Requiere TypeScript instalado en el
checkout Web, sin cargar/importar la aplicación. Una ficha que agrupa varios
archivos conserva el AST previo de los demás: refrescar todos los modificados.
CSS y recursos no TypeScript se inventarían con hash y dueño, sin inventar API.

`esp-callback-notes.json` añade propósito editorial de tipos callback y campos
de las tablas de puertos ESP; `web-callback-notes.json` precisa props/acciones
de los flujos principales Web. Los atributos específicos de módulo prevalecen.
Firmas, argumento de contexto y lifetime se documentan sin suponer que toda
declaración esté activa en cada build. TypeScript describe props/closures como
su mecanismo local; un callback de React no es un puntero de función wire.

`overlays/connections.json` conserva recorridos editoriales de productor/consumidor.
`published/shared/connections.json` añade ownership e imports/includes candidatos,
pantallas y enlaces inversos de hallazgos. `documentation_impact.py --file
web:src/protocol/screenMenuState.ts` o `--finding AMDOC-WEB-004` consulta ese índice
sin leer código. Es una ayuda de impacto, no una garantía de dependencias agotadas.

La réplica exacta copia/adapta motor, recursos y renderers de la revisión F4
acordada y recibe estado/variante equivalentes. OLED Canvas es un envío Web→F4,
no captura del framebuffer F4→Web. Un dato ausente permanece desconocido; los
defaults de ejemplo deben identificarse como simulación, nunca como observación.
Ver `published/shared/SCREEN_SYSTEM.md` para formatos y pendientes concretos.
