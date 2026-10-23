# Cobertura y límites de la documentación

Fotografía del 20-10-2026, archivos de trabajo con hash. Estos números describen extracción documental; no acreditan corrección, ejecución ni interoperabilidad.

| Raíz | Fichas de unidad | Archivos en inventario propio | Con ficha asignada |
| --- | --- | --- | --- |
| f4 | 39 | 168 | 166 |
| esp | 28 | 93 | 93 |
| web | 34 | 202 | 202 |

## Profundidad editorial de las funciones

Las fichas conservan firmas y una línea por función localizada. Las etiquetas describen de dónde viene esa explicación; completar la extracción no equivale a leer semánticamente cada closure.

| Raíz / origen de descripción | Entradas |
| --- | --- |
| esp:editorial | 48 |
| esp:inferred-name | 227 |
| esp:structural | 583 |
| f4:editorial | 502 |
| f4:inferred-name | 563 |
| f4:structural | 926 |
| web:editorial | 45 |
| web:inferred-name | 309 |
| web:structural | 979 |

`editorial`: explicación escrita de API; `structural`: descripción a partir de delegación/cuerpo; `inferred-name`: semántica pendiente de profundizar, no confirmada. C/C++ puede omitir sintaxis compleja, macros o sobrecargas; AST TS conserva retornos sin anotación como inferidos, sin typechecker.

F4, ESP y Web tienen fichas de alcance, handles/estado, modularidad, tipos, callbacks, funciones e interacciones. Los fuentes de soporte se conservan en source-map. Qt es consumidor del contrato, con manual interno equivalente pendiente.

El inventario general anterior de la skill y su caché de relaciones son una fotografía separada; el snapshot publicado actual se obtiene de estas fichas, incluyendo fragmentos .inc y estilos agrupados. No presentar tokens no resueltos como fallos de software.

Se incorporaron 147 fichas de identidad de pantalla con familias/limitaciones y referencias candidatas; no se resolvió exhaustivamente reachability/estado por cada código ni se ejecutaron comparaciones visuales. Ver [sistema de pantallas](shared/SCREEN_SYSTEM.md).

No se ejecutaron tests, compilaciones, validadores, navegador, despliegues ni pruebas físicas. Las correcciones de código y el port de la documentación al producto siguen pendientes.
