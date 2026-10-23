# Codec y almacenamiento LittleFS

ID: `esp:module/src/nvs_littlefs.h` · Capa: **plataforma** · Fotografía: 2026-10-20.

Documentación de fuente; sin tests, compilación, navegador ni validación física.

## Responsabilidad y alcance

Implementa archivos y codificación de configuración sobre LittleFS del ESP.

Mount, load/write/rename/remove y traducción AppConfig; separado del bundle Web servido.

<a id="modularity"></a>
## Modularidad observada

Dependencia Arduino/FS/codec esperada; NvsStore recibe la tabla para no importar filesystem directamente.

<a id="state"></a>
## Handle, estado y atributos externos

Recursos de filesystem y buffers temporales pertenecen al adaptador; los strings de configuración se copian al modelo.

<a id="lifecycle"></a>
## Ciclo de vida y contexto

- Montar antes de lectura/escritura.
- Conservar semántica de archivo temporal/respaldo y resultado al sustituir backend.

<a id="interactions"></a>
## Interacciones

- NvsStore recibe operaciones; servidor también usa LittleFS pero con ciclo y recursos propios.

<a id="limitations"></a>
## Precauciones y límites

- Espacio total se comparte con assets Web; escribir configuración no debe suponer disponibilidad ilimitada.
- No se montó ni verificó filesystem físico en esta entrega.

## Hallazgos conectados


## Archivos y evidencia

| Archivo | Análisis | Hash SHA-256 |
| --- | --- | --- |
| `src/nvs_littlefs.cpp` | c-cpp-structural | `27675fea9eb2dc6f68be1d1b8bf09e525d898c7b9e1f40217b3bcf09af43e979` |
| `src/nvs_littlefs.h` | c-cpp-structural | `d4ed8400806dd869a5c9cbd4d542085259a3b7670e07e38debb5d3e55fc444e9` |

## Tipos, puertos, props y callbacks

En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.


## Funciones públicas / componentes / hooks

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-nvs_littlefsops"></a>`const NvsStoreOps &NVS_LittleFSOps()` | Operación NVS Little FSOps de Codec y almacenamiento LittleFS; semántica fina pendiente de ampliación. | `esp:src/nvs_littlefs.cpp:123`; inferred-name |
| <a id="fn-doc"></a>`DynamicJsonDocument doc(768)` | Operación doc de Codec y almacenamiento LittleFS; semántica fina pendiente de ampliación. | `esp:src/nvs_littlefs.cpp:37`; inferred-name |

## Funciones y callbacks internos

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-cpy"></a>`static void cpy(char *dst, const char *src, size_t n)` | Coordina `memcpy` para Codec y almacenamiento LittleFS. | `esp:src/nvs_littlefs.cpp:6`; structural |
| <a id="fn-exists"></a>`static bool exists(void *, const char *path)` | Coordina `exists` para Codec y almacenamiento LittleFS. | `esp:src/nvs_littlefs.cpp:120`; structural |
| <a id="fn-loadconfigfile"></a>`static bool loadConfigFile(void *, const char *path, AppConfig &out, bool *requires_migration)` | Coordina `close`, `cpy`, `deserializeJson` para Codec y almacenamiento LittleFS. | `esp:src/nvs_littlefs.cpp:23`; structural |
| <a id="fn-mount"></a>`static bool mount(void *)` | Adapta la llamada a `begin` con el contexto de esta integración. | `esp:src/nvs_littlefs.cpp:119`; structural |
| <a id="fn-removefile"></a>`static bool removeFile(void *, const char *path)` | Adapta la llamada a `remove` con el contexto de esta integración. | `esp:src/nvs_littlefs.cpp:121`; structural |
| <a id="fn-renamefile"></a>`static bool renameFile(void *, const char *from, const char *to)` | Adapta la llamada a `rename` con el contexto de esta integración. | `esp:src/nvs_littlefs.cpp:122`; structural |
| <a id="fn-writeconfigfile"></a>`static bool writeConfigFile(void *, const char *path, const AppConfig &cfg)` | Coordina `close`, `exists`, `flush` para Codec y almacenamiento LittleFS. | `esp:src/nvs_littlefs.cpp:86`; structural |
