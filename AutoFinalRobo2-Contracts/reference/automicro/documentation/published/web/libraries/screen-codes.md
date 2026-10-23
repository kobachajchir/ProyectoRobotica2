# Códigos e identidad de pantallas

ID: `web:module/src/screens/screenCodes.ts` · Capa: **contrato** · Fotografía: 2026-10-20.

Documentación de fuente; sin tests, compilación, navegador ni validación física.

## Responsabilidad y alcance

Declara IDs y clasificación de pantallas para resolver contenido y controles.

Constantes, descomposición menu/submenu/page y definiciones auxiliares; no prueba render ni alcance de una pantalla.

<a id="modularity"></a>
## Modularidad observada

Contrato de datos TypeScript equivalente a códigos numéricos F4, con clasificación propia del consumidor.

<a id="state"></a>
## Handle, estado y atributos externos

Constantes/tablas sin estado de dispositivo; pantalla activa llega por reporte con source.

<a id="lifecycle"></a>
## Ciclo de vida y contexto

- Conservar ID al renombrar título.
- Distinguir código declarado por invocador de código efectivo que la integración OSK publica.

<a id="interactions"></a>
## Interacciones

- ScreenTypes tiene otro catálogo de metadata; registry y remediatedScreens resuelven contenido por código.
- El manifiesto documental lista valor F4, valor Web, metadata y candidatos por separado.

<a id="limitations"></a>
## Precauciones y límites

- Constante existente no implica entrada en SCREEN_META_BY_CODE ni builder activo.
- El source distingue variantes que comparten ID.

## Hallazgos conectados

- [AMDOC-WEB-008: Dos identidades de acción F4 no están declaradas en screenCodes Web](../../../review/FINDINGS.md#amdoc-web-008)
- [AMDOC-WEB-009: El catálogo de línea no tiene bindings específicos de visor localizados](../../../review/FINDINGS.md#amdoc-web-009)

## Archivos y evidencia

| Archivo | Análisis | Hash SHA-256 |
| --- | --- | --- |
| `src/screens/index.ts` | typescript-ast-static | `cc9de4e4a50b280ce38155c46b926fc0d11580ce1fcac3b15e3bd50d656988f4` |
| `src/screens/screenCodes.ts` | typescript-ast-static | `453b7d945bd473ccdfd8bdebbea99bbaeacd0c50c057d8b3abad7c35194e8aac` |

## Tipos, puertos, props y callbacks

En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.

### `ScreenDefinition`

`web:src/screens/screenCodes.ts:165`

| Atributo / firma | Propósito |
| --- | --- |
| `code: ScreenCode` | Dato de estado/configuración `code`; declaración estructural, interpretación específica pendiente de ampliar. |
| `isValidationScreen?: boolean` | Dato de estado/configuración `isValidationScreen`; declaración estructural, interpretación específica pendiente de ampliar. |
| `pinDigitsCount?: number` | Dato de estado/configuración `pinDigitsCount`; declaración estructural, interpretación específica pendiente de ampliar. |


## Funciones públicas / componentes / hooks

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-getscreendefinition"></a>`getScreenDefinition(code: ScreenCode \| null \| undefined): ScreenDefinition \| null` | Consulta definición auxiliar del código. | `web:src/screens/screenCodes.ts:202`; editorial |
| <a id="fn-getvalidationpindigitscount"></a>`getValidationPinDigitsCount(code: ScreenCode \| null \| undefined): number` | Consulta cantidad de dígitos configurada para validación Web. | `web:src/screens/screenCodes.ts:233`; editorial |
| <a id="fn-ispermissionvalidationscreencode"></a>`isPermissionValidationScreenCode(code: ScreenCode \| null \| undefined, source?: number): boolean` | Clasifica una pantalla PIN teniendo en cuenta source. | `web:src/screens/screenCodes.ts:218`; editorial |
| <a id="fn-isvalidationscreencode"></a>`isValidationScreenCode(code: ScreenCode \| null \| undefined): boolean` | Adapta la llamada a `getScreenDefinition` con el contexto de esta integración. | `web:src/screens/screenCodes.ts:212`; structural |
| <a id="fn-makescreencode"></a>`makeScreenCode(menu: number, submenu: number, page: number): ScreenCode` | Compone identidad de tres bytes sin datos de selección. | `web:src/screens/screenCodes.ts:5`; editorial |
| <a id="fn-screenmenu"></a>`screenMenu(code: ScreenCode): number` | Operación screen Menu de Códigos e identidad de pantallas; semántica fina pendiente de ampliación. | `web:src/screens/screenCodes.ts:13`; inferred-name |
| <a id="fn-screenpage"></a>`screenPage(code: ScreenCode): number` | Operación screen Page de Códigos e identidad de pantallas; semántica fina pendiente de ampliación. | `web:src/screens/screenCodes.ts:21`; inferred-name |
| <a id="fn-screensubmenu"></a>`screenSubmenu(code: ScreenCode): number` | Operación screen Submenu de Códigos e identidad de pantallas; semántica fina pendiente de ampliación. | `web:src/screens/screenCodes.ts:17`; inferred-name |

## Funciones y callbacks internos

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
