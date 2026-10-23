# Preferencias NVS y caché

ID: `esp:module/src/nvs_store.h` · Capa: **servicio** · Fotografía: 2026-10-20.

Documentación de fuente; sin tests, compilación, navegador ni validación física.

## Responsabilidad y alcance

Carga/persiste AppConfig con caché, migración y recuperación de respaldo.

Transacción lógica de configuración; filesystem y codec se prestan por NvsStoreOps.

<a id="modularity"></a>
## Modularidad observada

Núcleo de store usa operaciones; nvs_utils es adaptación histórica. AppConfig sigue siendo esquema del producto.

<a id="state"></a>
## Handle, estado y atributos externos

NvsStoreHandle posee ops copiado, cached, valid y mounted. GetCached devuelve vista const de su almacenamiento, no una copia independiente.

<a id="lifecycle"></a>
## Ciclo de vida y contexto

- Init conecta backend; InitAndLoad monta y elige archivo/defaults/migración.
- Save solicita escritura mediante el backend y actualiza caché según resultado.
- RestoreBackup recupera configuración guardada si puede hacerlo.

<a id="interactions"></a>
## Interacciones

- Wi-Fi consume configuración; bridge distingue cambios persistentes y de sesión.

<a id="limitations"></a>
## Precauciones y límites

- No exponer contraseñas o PIN de cached como documentación/JSON de diagnóstico.
- Un callback de escritura sigue pudiendo bloquear el loop.

## Hallazgos conectados


## Archivos y evidencia

| Archivo | Análisis | Hash SHA-256 |
| --- | --- | --- |
| `src/nvs_store.cpp` | c-cpp-structural | `37ba7cacf1da449065de7564541b196caa68b773c95a2c6e41a5fcc100079795` |
| `src/nvs_store.h` | c-cpp-structural | `ed012af9355be2baa0de4eb60213850fbb979a52e77bb4195a6e378b06820a20` |
| `src/nvs_utils.cpp` | c-cpp-structural | `d06b92f6e37ff13c72a60642fd4a362755eac3c74c9d0aae7e48640b57681b70` |
| `src/nvs_utils.h` | c-cpp-structural | `1de3237c121616bcbf5342ed7878bc066cc66349e4d41712523e482d7db44fc6` |

## Tipos, puertos, props y callbacks

En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.

### `NvsStoreOps`

`esp:src/nvs_store.h:5`

| Atributo / firma | Propósito |
| --- | --- |
| `void *context` | Dato de estado/configuración `context`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool (*mount)(void *)` | Inicializa/monta backend antes de leer/escribir preferencias; informar fallo al store. |
| `bool (*exists)(void *, const char *)` | Consulta existencia de la ruta lógica pedida por el store. |
| `bool (*remove)(void *, const char *)` | Elimina archivo lógico del backend durante el protocolo de persistencia. |
| `bool (*rename)(void *, const char *, const char *)` | Renombra archivo temporal/final según transacción de guardado; comprobar el resultado. |
| `bool (*load)(void *, const char *, AppConfig &, bool *requires_migration)` | Carga bytes de una ruta en los buffers/argumentos indicados por la firma, sin inventar un struct portable. |
| `bool (*write)(void *, const char *, const AppConfig &)` | Escribe datos serializados del store a la ruta del backend; conservar longitud y resultado. |

### `NvsStoreHandle`

`esp:src/nvs_store.h:14`

| Atributo / firma | Propósito |
| --- | --- |
| `NvsStoreOps ops` | Contrato de operaciones externo; conservar la tabla y su contexto durante el uso. |
| `AppConfig cached{ estructura anidada }` | Dato de estado/configuración `cached`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool valid` | Dato de estado/configuración `valid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool mounted` | Dato de estado/configuración `mounted`; declaración estructural, interpretación específica pendiente de ampliar. |


## Funciones públicas / componentes / hooks

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-nvs_getcached"></a>`const AppConfig *NVS_GetCached()` | Devuelve vista prestada de la configuración cargada. | `esp:src/nvs_utils.cpp:14`; editorial |
| <a id="fn-nvs_initandload"></a>`bool NVS_InitAndLoad(AppConfig *out)` | Monta y carga configuración con recuperación/migración disponible. | `esp:src/nvs_utils.cpp:10`; editorial |
| <a id="fn-nvs_restorebackup"></a>`bool NVS_RestoreBackup(AppConfig *out)` | Coordina `NvsStore_RestoreBackup`, `store` para Preferencias NVS y caché. | `esp:src/nvs_utils.cpp:12`; structural |
| <a id="fn-nvs_save"></a>`bool NVS_Save(const AppConfig &config)` | Persiste configuración y mantiene caché según éxito. | `esp:src/nvs_utils.cpp:11`; editorial |
| <a id="fn-nvs_storageready"></a>`bool NVS_StorageReady()` | Coordina `NvsStore_StorageReady`, `store` para Preferencias NVS y caché. | `esp:src/nvs_utils.cpp:13`; structural |
| <a id="fn-nvsstore_getcached"></a>`const AppConfig *NvsStore_GetCached(const NvsStoreHandle &h)` | Devuelve vista prestada de la configuración cargada. | `esp:src/nvs_store.cpp:283`; editorial |
| <a id="fn-nvsstore_init"></a>`void NvsStore_Init(NvsStoreHandle &h, const NvsStoreOps &ops)` | Conecta operaciones del backend y reinicia estado del store. | `esp:src/nvs_store.cpp:4`; editorial |
| <a id="fn-nvsstore_initandload"></a>`bool NvsStore_InitAndLoad(NvsStoreHandle &h, AppConfig *out)` | Monta y carga configuración con recuperación/migración disponible. | `esp:src/nvs_store.cpp:185`; editorial |
| <a id="fn-nvsstore_restorebackup"></a>`bool NvsStore_RestoreBackup(NvsStoreHandle &h, AppConfig *out)` | Coordina `exists`, `load`, `mountFs` para Preferencias NVS y caché. | `esp:src/nvs_store.cpp:255`; structural |
| <a id="fn-nvsstore_save"></a>`bool NvsStore_Save(NvsStoreHandle &h, const AppConfig &cfg)` | Persiste configuración y mantiene caché según éxito. | `esp:src/nvs_store.cpp:134`; editorial |
| <a id="fn-nvsstore_storageready"></a>`bool NvsStore_StorageReady(const NvsStoreHandle &h)` | Devuelve el campo `h.mounted` del estado actual de la instancia. | `esp:src/nvs_store.cpp:281`; structural |

## Funciones y callbacks internos

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-configsequal"></a>`static bool configsEqual(const AppConfig &a, const AppConfig &b)` | Coordina `strcmp` para Preferencias NVS y caché. | `esp:src/nvs_store.cpp:83`; structural |
| <a id="fn-cpy"></a>`static void cpy(char *dst, const char *src, size_t n)` | Coordina `memcpy` para Preferencias NVS y caché. | `esp:src/nvs_store.cpp:16`; structural |
| <a id="fn-filldefaults"></a>`static void fillDefaults(AppConfig &cfg)` | Coordina `cpy`, `memset` para Preferencias NVS y caché. | `esp:src/nvs_store.cpp:65`; structural |
| <a id="fn-mountfs"></a>`static bool mountFs(NvsStoreHandle &h)` | Adapta la llamada a `mount` con el contexto de esta integración. | `esp:src/nvs_store.cpp:8`; structural |
| <a id="fn-normalizeconfig"></a>`static void normalizeConfig(const AppConfig &input, AppConfig &output)` | Coordina `cpy`, `memset` para Preferencias NVS y caché. | `esp:src/nvs_store.cpp:33`; structural |
| <a id="fn-store"></a>`static NvsStoreHandle &store()` | Coordina `NVS_LittleFSOps`, `NvsStore_Init` para Preferencias NVS y caché. | `esp:src/nvs_utils.cpp:5`; structural |
| <a id="fn-verifyconfigfile"></a>`static bool verifyConfigFile(NvsStoreHandle &h, const char *path, const AppConfig &expected)` | Coordina `configsEqual`, `load` para Preferencias NVS y caché. | `esp:src/nvs_store.cpp:100`; structural |
| <a id="fn-writesanitizedbackup"></a>`static bool writeSanitizedBackup(NvsStoreHandle &h, const AppConfig &cfg)` | Coordina `exists`, `remove`, `rename` para Preferencias NVS y caché. | `esp:src/nvs_store.cpp:108`; structural |
