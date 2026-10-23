# Selección de modo de arranque ESP

ID: `esp:module/src/boot_mode.h` · Capa: **integración** · Fotografía: 2026-10-20.

Documentación de fuente; sin tests, compilación, navegador ni validación física.

## Responsabilidad y alcance

Interpreta y conserva modo solicitado para el próximo arranque ESP.

AP-only frente a modo normal AP/STA y su relación con configuración persistida.

<a id="modularity"></a>
## Modularidad observada

Política de aplicación ESP, separada del bucle del servicio Wi-Fi.

<a id="state"></a>
## Handle, estado y atributos externos

Selección de modo y AppConfig; no usar el mismo dato como disparador de reinicio en cada boot.

<a id="lifecycle"></a>
## Ciclo de vida y contexto

- GetRequested se consulta después de cargar NVS y antes de iniciar Wi-Fi.
- Un comando explícito cambia selección/reinicio; el arranque consume la decisión sin bucle de reescritura.

<a id="interactions"></a>
## Interacciones

- UNER/JSON solicita reboot; main decide WifiRunMode a partir del modo elegido.

<a id="limitations"></a>
## Precauciones y límites

- Modo deseado/persistido y modo de radio activo son observaciones diferentes.

## Hallazgos conectados


## Archivos y evidencia

| Archivo | Análisis | Hash SHA-256 |
| --- | --- | --- |
| `src/boot_mode.cpp` | c-cpp-structural | `9118457648f1d9d1007a071769bdb1667a9cca6f4210856c8a9cf1b69acfe77f` |
| `src/boot_mode.h` | c-cpp-structural | `1ec624c16bdc8602ded71565665fce3182ed90271f311b8476f25e6f10b93186` |

## Tipos, puertos, props y callbacks

En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.


## Funciones públicas / componentes / hooks

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-bootmode_applyrequest"></a>`bool BootMode_ApplyRequest(const AppConfig *current, uint8_t mode, AppConfig *next)` | Adapta la llamada a `BootMode_IsValid` con el contexto de esta integración. | `esp:src/boot_mode.cpp:15`; structural |
| <a id="fn-bootmode_getrequested"></a>`uint8_t BootMode_GetRequested(const AppConfig *config)` | Devuelve el campo `config->nextBootMode` del estado actual de la instancia. | `esp:src/boot_mode.cpp:8`; structural |
| <a id="fn-bootmode_isvalid"></a>`bool BootMode_IsValid(uint8_t mode)` | Indica si se cumple la condición Valid dentro de Selección de modo de arranque ESP. | `esp:src/boot_mode.cpp:3`; inferred-name |

## Funciones y callbacks internos

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
