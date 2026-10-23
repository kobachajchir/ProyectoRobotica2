# Renderers Web de dashboard y modos

ID: `web:module/src/screens/dashboardScreens.ts` · Capa: **núcleo** · Fotografía: 2026-10-20.

Documentación de fuente; sin tests, compilación, navegador ni validación física.

## Responsabilidad y alcance

Construye pantallas de arranque, dashboard, selector de modo y test.

Modelos visuales y geometría adaptada del dominio F4; no adquiere conectividad directamente.

<a id="modularity"></a>
## Modularidad observada

Builder de comandos puro, con convenciones de fuente/montaje del proyecto.

<a id="state"></a>
## Handle, estado y atributos externos

Argumentos prestados de backend, conexión, SSID/IP y modo; texto y scroll necesitan valores/tiempo/ref exactos para coincidir con el equipo.

<a id="lifecycle"></a>
## Ciclo de vida y contexto

- Recibir modelo confirmado y construir comandos.
- No convertir ausencia de dato en un estado físico afirmado.

<a id="interactions"></a>
## Interacciones

- Extensión GET_CURRENT_SCREEN transporta datos dashboard; ScreenContext normaliza y registry adapta.
- F4 reserva carriles de scroll y banda de indicadores; replicar requiere misma ventana y fase temporal.

<a id="limitations"></a>
## Precauciones y límites

- Los lectores de modo del visor conservan casos incompletos para GOTO.
- Igual etiqueta no prueba igual fase de desplazamiento del SSID.

## Hallazgos conectados


## Archivos y evidencia

| Archivo | Análisis | Hash SHA-256 |
| --- | --- | --- |
| `src/screens/dashboardScreens.ts` | typescript-ast-static | `5a0247aea336c412265265e4a39da818c54a837e11f8134827afae31e1c6ed51` |

## Tipos, puertos, props y callbacks

En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.

### `DashboardConnectionState`

`web:src/screens/dashboardScreens.ts:19`

Tipo: `"none" \| "sta" \| "ap"`.

### `CarModeLabel`

`web:src/screens/dashboardScreens.ts:20`

Tipo: `"IDLE" \| "FOLLOW" \| "TEST" \| "GOTO"`.

### `DashboardScreenArgs`

`web:src/screens/dashboardScreens.ts:22`

| Atributo / firma | Propósito |
| --- | --- |
| `espPresent?: boolean` | Dato de estado/configuración `espPresent`; declaración estructural, interpretación específica pendiente de ampliar. |
| `connection: DashboardConnectionState` | Dato de estado/configuración `connection`; declaración estructural, interpretación específica pendiente de ampliar. |
| `ssid?: string` | Dato de estado/configuración `ssid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `ipAddress?: string` | Dato de estado/configuración `ipAddress`; declaración estructural, interpretación específica pendiente de ampliar. |
| `backend?: "WEB" \| "AT" \| "No ESP"` | Dato de estado/configuración `backend`; declaración estructural, interpretación específica pendiente de ampliar. |
| `usbActive?: boolean` | Dato de estado/configuración `usbActive`; declaración estructural, interpretación específica pendiente de ampliar. |
| `rfActive?: boolean` | Dato de estado/configuración `rfActive`; declaración estructural, interpretación específica pendiente de ampliar. |
| `carMode: CarModeLabel` | Dato de estado/configuración `carMode`; declaración estructural, interpretación específica pendiente de ampliar. |

### `ModeChangeScreenArgs`

`web:src/screens/dashboardScreens.ts:33`

| Atributo / firma | Propósito |
| --- | --- |
| `mode: "IDLE" \| "FOLLOW" \| "TEST" \| "GOTO"` | Dato de estado/configuración `mode`; declaración estructural, interpretación específica pendiente de ampliar. |


## Funciones públicas / componentes / hooks

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-buildscreen010001startupcommands"></a>`buildScreen010001StartupCommands(): OledCommand[]` | Coordina `clear`, `drawXbmMsb`, `setWhite` para Renderers Web de dashboard y modos. | `web:src/screens/dashboardScreens.ts:39`; structural |
| <a id="fn-buildscreen010101dashboardcommands"></a>`buildScreen010101DashboardCommands(args: DashboardScreenArgs): OledCommand[]` | Coordina `clear`, `commands.push`, `drawXbm` para Renderers Web de dashboard y modos. | `web:src/screens/dashboardScreens.ts:49`; structural |
| <a id="fn-buildscreen010102modechangecommands"></a>`buildScreen010102ModeChangeCommands(args: ModeChangeScreenArgs): OledCommand[]` | Coordina `buildScreen010102ModeOnlyPatchCommands`, `clear`, `drawXbm` para Renderers Web de dashboard y modos. | `web:src/screens/dashboardScreens.ts:98`; structural |
| <a id="fn-buildscreen010102modeonlypatchcommands"></a>`buildScreen010102ModeOnlyPatchCommands(args: ModeChangeScreenArgs): OledCommand[]` | Coordina `Math.floor`, `baselineY`, `clearBox` para Renderers Web de dashboard y modos. | `web:src/screens/dashboardScreens.ts:110`; structural |
| <a id="fn-buildscreen070101testscreencommands"></a>`buildScreen070101TestScreenCommands(): OledCommand[]` | Adapta la llamada a `clear` con el contexto de esta integración. | `web:src/screens/dashboardScreens.ts:122`; structural |

## Funciones y callbacks internos

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
