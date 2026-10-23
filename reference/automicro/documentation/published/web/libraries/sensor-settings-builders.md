# Renderers Web de sensores, ajustes y PIN

ID: `web:module/src/screens/sensorScreens.ts` · Capa: **núcleo** · Fotografía: 2026-10-20.

Documentación de fuente; sin tests, compilación, navegador ni validación física.

## Responsabilidad y alcance

Construye vistas OLED de sensores/motores, ajustes, canvas y autorización.

Geometría y comandos; la disponibilidad remota de cada argumento se documenta por familia de pantalla.

<a id="modularity"></a>
## Modularidad observada

Builders puros con tipos/recursos compartidos; no controlan actuadores.

<a id="state"></a>
## Handle, estado y atributos externos

Args de sensores, porcentajes, dígitos e índice son entradas locales al builder. Su existencia en una interfaz no significa que el ESP las publique.

<a id="lifecycle"></a>
## Ciclo de vida y contexto

- Adaptar unidades y variantes antes de invocar.
- No alimentar con valores de ejemplo una vista etiquetada como pantalla real.

<a id="interactions"></a>
## Interacciones

- MPU JSON publica accel/gyro por ejes en g/dps, mientras algunos builders esperan escalares de otra representación.
- PIN y OSK físicos necesitan estado/discriminante adicional o deben permanecer identity-only.

<a id="limitations"></a>
## Precauciones y límites

- Faltan modelos remotos completos para varias vistas/borradores.
- PinScreenModal local y PermissionPinFormArgs del preview son estados distintos.

## Hallazgos conectados

- [AMDOC-WEB-006: Los builders de sensores no reciben el mismo modelo que la telemetría actual](../../../review/FINDINGS.md#amdoc-web-006)

## Archivos y evidencia

| Archivo | Análisis | Hash SHA-256 |
| --- | --- | --- |
| `src/screens/displayScreens.ts` | typescript-ast-static | `62caa259f7ac16cfcb6ef7f7b1197fdc022910bbdcfca00ac1a94a5888dc6341` |
| `src/screens/motorScreens.ts` | typescript-ast-static | `247997324939d92cd28078eef7c73fd27704be439c98019af3a8d0d23abaef54` |
| `src/screens/sensorScreens.ts` | typescript-ast-static | `e4827ad88aab6da0fa2810463897525d8310551c2d823f57c544f83c41b804a5` |
| `src/screens/settingsScreens.ts` | typescript-ast-static | `008417181e8e915ccc1315caf9febfc4765d968136b5485429d5e7d607daf53b` |
| `src/screens/warningScreens.ts` | typescript-ast-static | `0080ffe5b026a582a185f737ef106bc2c968e11e230cfd10b65a3297ce56cf80` |

## Tipos, puertos, props y callbacks

En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.

### `MotorSelection`

`web:src/screens/motorScreens.ts:5`

Tipo: `"left" \| "right" \| "both" \| "none" \| "unknown"`.

### `MotorDirection`

`web:src/screens/motorScreens.ts:6`

Tipo: `"forward" \| "backward"`.

### `MotorTestArgs`

`web:src/screens/motorScreens.ts:8`

| Atributo / firma | Propósito |
| --- | --- |
| `selectedMotor: MotorSelection` | Dato de estado/configuración `selectedMotor`; declaración estructural, interpretación específica pendiente de ampliar. |
| `direction: MotorDirection` | Dato de estado/configuración `direction`; declaración estructural, interpretación específica pendiente de ampliar. |
| `leftSpeed: number` | Dato de estado/configuración `leftSpeed`; declaración estructural, interpretación específica pendiente de ampliar. |
| `rightSpeed: number` | Dato de estado/configuración `rightSpeed`; declaración estructural, interpretación específica pendiente de ampliar. |
| `movementEnabled: boolean` | Dato de estado/configuración `movementEnabled`; declaración estructural, interpretación específica pendiente de ampliar. |

### `IrValuesArgs`

`web:src/screens/sensorScreens.ts:20`

| Atributo / firma | Propósito |
| --- | --- |
| `irValues: number[]` | Dato de estado/configuración `irValues`; declaración estructural, interpretación específica pendiente de ampliar. |

### `MpuValuesArgs`

`web:src/screens/sensorScreens.ts:24`

| Atributo / firma | Propósito |
| --- | --- |
| `accelX: number` | Dato de estado/configuración `accelX`; declaración estructural, interpretación específica pendiente de ampliar. |
| `accelY: number` | Dato de estado/configuración `accelY`; declaración estructural, interpretación específica pendiente de ampliar. |
| `accelZ: number` | Dato de estado/configuración `accelZ`; declaración estructural, interpretación específica pendiente de ampliar. |
| `gyroX: number` | Dato de estado/configuración `gyroX`; declaración estructural, interpretación específica pendiente de ampliar. |
| `gyroY: number` | Dato de estado/configuración `gyroY`; declaración estructural, interpretación específica pendiente de ampliar. |
| `gyroZ: number` | Dato de estado/configuración `gyroZ`; declaración estructural, interpretación específica pendiente de ampliar. |

### `LegacyMpuTableArgs`

`web:src/screens/sensorScreens.ts:33`

| Atributo / firma | Propósito |
| --- | --- |
| `calibrated: boolean` | Dato de estado/configuración `calibrated`; declaración estructural, interpretación específica pendiente de ampliar. |
| `angleXMd: number` | Dato de estado/configuración `angleXMd`; declaración estructural, interpretación específica pendiente de ampliar. |
| `angleYMd: number` | Dato de estado/configuración `angleYMd`; declaración estructural, interpretación específica pendiente de ampliar. |
| `angleZMd: number` | Dato de estado/configuración `angleZMd`; declaración estructural, interpretación específica pendiente de ampliar. |

### `LockScreenState`

`web:src/screens/warningScreens.ts:15`

Tipo: `"locked" \| "pinIncorrect" \| "pinModified" \| "enterPin"`.

### `PermissionPinState`

`web:src/screens/warningScreens.ts:16`

Tipo: `"input" \| "waiting" \| "denied" \| "timeout" \| "blocked"`.

### `PermissionPinFormArgs`

`web:src/screens/warningScreens.ts:18`

| Atributo / firma | Propósito |
| --- | --- |
| `digits: number[]` | Dato de estado/configuración `digits`; declaración estructural, interpretación específica pendiente de ampliar. |
| `pinIndex?: number` | Dato de estado/configuración `pinIndex`; declaración estructural, interpretación específica pendiente de ampliar. |


## Funciones públicas / componentes / hooks

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-buildlockscreencommands"></a>`buildLockScreenCommands(args: { state: LockScreenState }): OledCommand[]` | Coordina `baselineY`, `clear`, `drawXbm` para Renderers Web de sensores, ajustes y PIN. | `web:src/screens/warningScreens.ts:65`; structural |
| <a id="fn-buildpermissionpinformcommands"></a>`buildPermissionPinFormCommands(args: PermissionPinFormArgs & { waiting?: boolean; confirmText: string }): OledCommand[]` | Coordina `baselineY`, `commands.push`, `drawFrame` para Renderers Web de sensores, ajustes y PIN. | `web:src/screens/warningScreens.ts:34`; structural |
| <a id="fn-buildscreen030201irvaluescommands"></a>`buildScreen030201IrValuesCommands(args: IrValuesArgs): OledCommand[]` | Dibuja barras IR según el array recibido; no decide su origen o frescura. | `web:src/screens/sensorScreens.ts:53`; editorial |
| <a id="fn-buildscreen030201legacyirgraphcommands"></a>`buildScreen030201LegacyIrGraphCommands(args: IrValuesArgs): OledCommand[]` | Coordina `clear`, `clearBox`, `commands.push` para Renderers Web de sensores, ajustes y PIN. | `web:src/screens/sensorScreens.ts:84`; structural |
| <a id="fn-buildscreen030301legacymputablecommands"></a>`buildScreen030301LegacyMpuTableCommands(args: LegacyMpuTableArgs): OledCommand[]` | Coordina `String`, `baselineY`, `clear` para Renderers Web de sensores, ajustes y PIN. | `web:src/screens/sensorScreens.ts:136`; structural |
| <a id="fn-buildscreen030301mpuvaluescommands"></a>`buildScreen030301MpuValuesCommands(args: MpuValuesArgs): OledCommand[]` | Coordina `String`, `baselineY`, `clear` para Renderers Web de sensores, ajustes y PIN. | `web:src/screens/sensorScreens.ts:110`; structural |
| <a id="fn-buildscreen030401radarcommands"></a>`buildScreen030401RadarCommands(): OledCommand[]` | Coordina `clear`, `drawXbm`, `setWhite` para Renderers Web de sensores, ajustes y PIN. | `web:src/screens/sensorScreens.ts:165`; structural |
| <a id="fn-buildscreen030401radarobjectspatchcommands"></a>`buildScreen030401RadarObjectsPatchCommands(): OledCommand[]` | Operación Screen030401 Radar Objects Patch Commands de Renderers Web de sensores, ajustes y PIN; semántica fina pendiente de ampliación. | `web:src/screens/sensorScreens.ts:175`; inferred-name |
| <a id="fn-buildscreen030500displaymenucommands"></a>`buildScreen030500DisplayMenuCommands(args: { selectedIndex?: number; firstVisibleIndex?: number }): OledCommand[]` | Adapta la llamada a `buildMenuScreenCommands` con el contexto de esta integración. | `web:src/screens/displayScreens.ts:19`; structural |
| <a id="fn-buildscreen030503oledcanvasreadycommands"></a>`buildScreen030503OledCanvasReadyCommands(): OledCommand[]` | Coordina `clear`, `textAt` para Renderers Web de sensores, ajustes y PIN. | `web:src/screens/displayScreens.ts:30`; structural |
| <a id="fn-buildscreen040101motortestcommands"></a>`buildScreen040101MotorTestCommands(args: MotorTestArgs): OledCommand[]` | Coordina `barWidthFromByte`, `baselineY`, `clear` para Renderers Web de sensores, ajustes y PIN. | `web:src/screens/motorScreens.ts:47`; structural |
| <a id="fn-buildscreen040101motortestpatchcommands"></a>`buildScreen040101MotorTestPatchCommands(args: MotorTestArgs): OledCommand[]` | Coordina `barWidthFromByte`, `baselineY`, `clearBox` para Renderers Web de sensores, ajustes y PIN. | `web:src/screens/motorScreens.ts:64`; structural |
| <a id="fn-buildscreen050201aboutprojectcommands"></a>`buildScreen050201AboutProjectCommands(): OledCommand[]` | Coordina `baselineY`, `clear`, `drawXbm` para Renderers Web de sensores, ajustes y PIN. | `web:src/screens/settingsScreens.ts:11`; structural |
| <a id="fn-buildscreen050202aboutrepocommands"></a>`buildScreen050202AboutRepoCommands(): OledCommand[]` | Coordina `baselineY`, `clear`, `drawXbm` para Renderers Web de sensores, ajustes y PIN. | `web:src/screens/settingsScreens.ts:27`; structural |
| <a id="fn-buildscreen050301warningtimecommands"></a>`buildScreen050301WarningTimeCommands(args: { seconds: number }): OledCommand[]` | Coordina `baselineY`, `clear`, `drawXbm` para Renderers Web de sensores, ajustes y PIN. | `web:src/screens/settingsScreens.ts:41`; structural |
| <a id="fn-buildscreen080101lockedcommands"></a>`buildScreen080101LockedCommands(): OledCommand[]` | Adapta la llamada a `buildLockScreenCommands` con el contexto de esta integración. | `web:src/screens/warningScreens.ts:75`; structural |
| <a id="fn-buildscreen080102pinincorrectlockcommands"></a>`buildScreen080102PinIncorrectLockCommands(): OledCommand[]` | Adapta la llamada a `buildLockScreenCommands` con el contexto de esta integración. | `web:src/screens/warningScreens.ts:79`; structural |
| <a id="fn-buildscreen080103pinmodifiedlockcommands"></a>`buildScreen080103PinModifiedLockCommands(): OledCommand[]` | Adapta la llamada a `buildLockScreenCommands` con el contexto de esta integración. | `web:src/screens/warningScreens.ts:83`; structural |
| <a id="fn-buildscreen080104lockenterpincommands"></a>`buildScreen080104LockEnterPinCommands(): OledCommand[]` | Adapta la llamada a `buildLockScreenCommands` con el contexto de esta integración. | `web:src/screens/warningScreens.ts:87`; structural |
| <a id="fn-buildscreen080104permissionpinentrycommands"></a>`buildScreen080104PermissionPinEntryCommands(args: PermissionPinFormArgs): OledCommand[]` | Dibuja formulario de PIN a partir de digits/pinIndex suministrados al builder. | `web:src/screens/warningScreens.ts:91`; editorial |
| <a id="fn-buildscreen080105permissionpinwaitingcommands"></a>`buildScreen080105PermissionPinWaitingCommands(args: PermissionPinFormArgs): OledCommand[]` | Coordina `buildPermissionPinFormCommands`, `clear`, `setWhite` para Renderers Web de sensores, ajustes y PIN. | `web:src/screens/warningScreens.ts:101`; structural |
| <a id="fn-buildscreen080106permissionpindeniedcommands"></a>`buildScreen080106PermissionPinDeniedCommands(args: { attemptsLeft: number }): OledCommand[]` | Coordina `clear`, `setWhite`, `textAt` para Renderers Web de sensores, ajustes y PIN. | `web:src/screens/warningScreens.ts:111`; structural |
| <a id="fn-buildscreen080107permissionpintimeoutcommands"></a>`buildScreen080107PermissionPinTimeoutCommands(): OledCommand[]` | Coordina `clear`, `setWhite`, `textAt` para Renderers Web de sensores, ajustes y PIN. | `web:src/screens/warningScreens.ts:123`; structural |
| <a id="fn-buildscreen080108permissionpinblockedcommands"></a>`buildScreen080108PermissionPinBlockedCommands(): OledCommand[]` | Coordina `clear`, `setWhite`, `textAt` para Renderers Web de sensores, ajustes y PIN. | `web:src/screens/warningScreens.ts:135`; structural |
| <a id="fn-buildscreen080109permissiondeniednotificationcommands"></a>`buildScreen080109PermissionDeniedNotificationCommands(progress?: NotificationProgressArgs): OledCommand[]` | Coordina `baselineY`, `clear`, `drawXbm` para Renderers Web de sensores, ajustes y PIN. | `web:src/screens/warningScreens.ts:147`; structural |
| <a id="fn-completemotorlabel"></a>`completeMotorLabel(selectedMotor: MotorSelection): string` | Operación complete Motor Label de Renderers Web de sensores, ajustes y PIN; semántica fina pendiente de ampliación. | `web:src/screens/motorScreens.ts:16`; inferred-name |
| <a id="fn-locktextforstate"></a>`lockTextForState(state: LockScreenState): { screenCode: ScreenCode; message: string; x: number }` | Operación lock Text For State de Renderers Web de sensores, ajustes y PIN; semántica fina pendiente de ampliación. | `web:src/screens/warningScreens.ts:23`; inferred-name |
| <a id="fn-normalizeanglefrommd"></a>`normalizeAngleFromMd(angleMd: number): number` | Adapta la llamada a `Math.trunc` con el contexto de esta integración. | `web:src/screens/sensorScreens.ts:44`; structural |
| <a id="fn-patchmotorlabel"></a>`patchMotorLabel(selectedMotor: MotorSelection): string` | Operación patch Motor Label de Renderers Web de sensores, ajustes y PIN; semántica fina pendiente de ampliación. | `web:src/screens/motorScreens.ts:23`; inferred-name |
| <a id="fn-pindigit"></a>`pinDigit(digits: number[], index: number): string` | Coordina `Math.max`, `Math.min`, `String` para Renderers Web de sensores, ajustes y PIN. | `web:src/screens/warningScreens.ts:30`; structural |
| <a id="fn-selectedspeedforcomplete"></a>`selectedSpeedForComplete(args: MotorTestArgs): number` | Adapta la llamada a `Math.floor` con el contexto de esta integración. | `web:src/screens/motorScreens.ts:29`; structural |
| <a id="fn-selectedspeedforpatch"></a>`selectedSpeedForPatch(args: MotorTestArgs): number` | Operación selected Speed For Patch de Renderers Web de sensores, ajustes y PIN; semántica fina pendiente de ampliación. | `web:src/screens/motorScreens.ts:35`; inferred-name |
| <a id="fn-sensorvalueat"></a>`sensorValueAt(values: number[], index: number): number` | Operación sensor Value At de Renderers Web de sensores, ajustes y PIN; semántica fina pendiente de ampliación. | `web:src/screens/sensorScreens.ts:40`; inferred-name |
| <a id="fn-speedtext"></a>`speedText(speed: number, movementEnabled: boolean): string` | Coordina `Math.floor`, `Math.max`, `Math.min` para Renderers Web de sensores, ajustes y PIN. | `web:src/screens/motorScreens.ts:39`; structural |

## Funciones y callbacks internos

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
