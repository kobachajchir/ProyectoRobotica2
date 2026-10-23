# Herramientas de calibración IR y línea

ID: `web:module/src/features/calibration/CalibrationWorkspace.tsx` · Capa: **integración** · Fotografía: 2026-10-20.

Documentación de fuente; sin tests, compilación, navegador ni validación física.

## Responsabilidad y alcance

Organiza materiales, curvas y herramientas de calibración/inspección del cliente.

Edición/análisis local y presentación; no sustituye automáticamente perfiles capturados/persistidos en F4.

<a id="modularity"></a>
## Modularidad observada

UI y cálculo específicos de sensores del proyecto, separados del productor físico.

<a id="state"></a>
## Handle, estado y atributos externos

Borradores, curvas y almacenamiento local se diferencian de flags calibrados del dispositivo.

<a id="lifecycle"></a>
## Ciclo de vida y contexto

- Seleccionar muestra/material/modelo y actualizar visualización.
- Persistir localmente o enviar por contrato explícito según la herramienta.

<a id="interactions"></a>
## Interacciones

- Geometría IR y datos de streaming alimentan herramientas; LineDeviceCalibration controla otra sesión en firmware.

<a id="limitations"></a>
## Precauciones y límites

- No mezclar mm/cm, ADC raw, darkness y valores normalizados de presentación.
- Una curva local guardada no acredita calibración aplicada al robot.

## Hallazgos conectados


## Archivos y evidencia

| Archivo | Análisis | Hash SHA-256 |
| --- | --- | --- |
| `src/features/calibration/CalibrationWorkspace.tsx` | typescript-ast-static | `869d73be3b6c92c4f0a6c46712bfeb319598cf2f08d5fe6b17794e66696068a8` |
| `src/features/calibration/LineWorkbench.tsx` | typescript-ast-static | `828cb18fa2e02649b8bcd7ba4fc5d449719cec596e9b059a945d3a124d90b1ad` |
| `src/features/calibration/MaterialPicker.tsx` | typescript-ast-static | `d20748da81405dcebadb0bccc6b545d0e239a715d6b7826cbb93394154846833` |
| `src/features/calibration/calibration.css` | support-inventory | `2ee7e2ca011992f6287923e2dff54101e34c1d4f1b7cbe169983f5072f5c897e` |
| `src/features/ir/IrCalibrationWorkbench.tsx` | typescript-ast-static | `0f61c78720a1974db42f3b6add983a381b8353adf9986f53bba1a99094c5ca1b` |
| `src/features/ir/irCalibration.ts` | typescript-ast-static | `5248d050bded257bec2a82abeda1d89f24c2e6b2469309d68fbef846ee362baf` |
| `src/features/ir/irCalibrationStorage.ts` | typescript-ast-static | `62d46424601feec4d47a8a196bf2ffb35aef4a38a8c373c3874718a1c9e5dfa2` |

## Tipos, puertos, props y callbacks

En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.

### `LineRecord`

`web:src/features/calibration/LineWorkbench.tsx:7`

| Atributo / firma | Propósito |
| --- | --- |
| `id: string` | Dato de estado/configuración `id`; declaración estructural, interpretación específica pendiente de ampliar. |
| `name: string` | Dato de estado/configuración `name`; declaración estructural, interpretación específica pendiente de ampliar. |
| `material: string` | Dato de estado/configuración `material`; declaración estructural, interpretación específica pendiente de ampliar. |
| `createdAt: string` | Dato de estado/configuración `createdAt`; declaración estructural, interpretación específica pendiente de ampliar. |
| `count: number` | Dato de estado/configuración `count`; declaración estructural, interpretación específica pendiente de ampliar. |
| `raw: number[]` | Dato de estado/configuración `raw`; declaración estructural, interpretación específica pendiente de ampliar. |
| `normalized: number[]` | Dato de estado/configuración `normalized`; declaración estructural, interpretación específica pendiente de ampliar. |

### `IrCalibrationWorkbenchProps`

`web:src/features/ir/IrCalibrationWorkbench.tsx:24`

| Atributo / firma | Propósito |
| --- | --- |
| `profile: IrCalibrationProfile` | Dato de estado/configuración `profile`; declaración estructural, interpretación específica pendiente de ampliar. |
| `snapshot: IrSnapshot` | Dato de estado/configuración `snapshot`; declaración estructural, interpretación específica pendiente de ampliar. |
| `captureAvailable: boolean` | Dato de estado/configuración `captureAvailable`; declaración estructural, interpretación específica pendiente de ampliar. |
| `onProfileChange: (profile: IrCalibrationProfile) => void` | Operación on Profile Change de Herramientas de calibración IR y línea; semántica fina pendiente de ampliación. |
| `onBusyChange?: (busy: boolean) => void` | Operación on Busy Change de Herramientas de calibración IR y línea; semántica fina pendiente de ampliación. |

### `CaptureState`

`web:src/features/ir/IrCalibrationWorkbench.tsx:32`

| Atributo / firma | Propósito |
| --- | --- |
| `kind: "baseline" \| "point"` | Dato de estado/configuración `kind`; declaración estructural, interpretación específica pendiente de ampliar. |
| `distanceCm: number \| null` | Dato de estado/configuración `distanceCm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `sensorKey: IrObjectSensorKey \| null` | Dato de estado/configuración `sensorKey`; declaración estructural, interpretación específica pendiente de ampliar. |
| `settleRemaining: number` | Dato de estado/configuración `settleRemaining`; declaración estructural, interpretación específica pendiente de ampliar. |
| `samples: IrCalibrationSample[]` | Dato de estado/configuración `samples`; declaración estructural, interpretación específica pendiente de ampliar. |

### `IrCalibrationPolarity`

`web:src/features/ir/irCalibration.ts:29`

Tipo: `"higher-is-closer" \| "lower-is-closer"`.

### `IrCalibrationSample`

`web:src/features/ir/irCalibration.ts:31`

Tipo: `Record<IrObjectSensorKey, number>`.

### `IrCalibrationStats`

`web:src/features/ir/irCalibration.ts:33`

| Atributo / firma | Propósito |
| --- | --- |
| `median: number` | Dato de estado/configuración `median`; declaración estructural, interpretación específica pendiente de ampliar. |
| `mean: number` | Dato de estado/configuración `mean`; declaración estructural, interpretación específica pendiente de ampliar. |
| `min: number` | Dato de estado/configuración `min`; declaración estructural, interpretación específica pendiente de ampliar. |
| `max: number` | Dato de estado/configuración `max`; declaración estructural, interpretación específica pendiente de ampliar. |
| `p10: number` | Dato de estado/configuración `p10`; declaración estructural, interpretación específica pendiente de ampliar. |
| `p90: number` | Dato de estado/configuración `p90`; declaración estructural, interpretación específica pendiente de ampliar. |
| `stdDev: number` | Dato de estado/configuración `stdDev`; declaración estructural, interpretación específica pendiente de ampliar. |
| `count: number` | Dato de estado/configuración `count`; declaración estructural, interpretación específica pendiente de ampliar. |

### `IrCalibrationSensorStats`

`web:src/features/ir/irCalibration.ts:44`

Tipo: `Record< IrObjectSensorKey, IrCalibrationStats >`.

### `IrCalibrationPointSensorStats`

`web:src/features/ir/irCalibration.ts:49`

Tipo: `Partial< Record<IrObjectSensorKey, IrCalibrationStats> >`.

### `IrCalibrationPoint`

`web:src/features/ir/irCalibration.ts:53`

| Atributo / firma | Propósito |
| --- | --- |
| `distanceCm: number` | Dato de estado/configuración `distanceCm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `sensors: IrCalibrationPointSensorStats` | Dato de estado/configuración `sensors`; declaración estructural, interpretación específica pendiente de ampliar. |

### `IrCalibrationProfile`

`web:src/features/ir/irCalibration.ts:58`

| Atributo / firma | Propósito |
| --- | --- |
| `schemaVersion: typeof IR_CALIBRATION_SCHEMA_VERSION` | Dato de estado/configuración `schemaVersion`; declaración estructural, interpretación específica pendiente de ampliar. |
| `name: string` | Dato de estado/configuración `name`; declaración estructural, interpretación específica pendiente de ampliar. |
| `createdAt: string` | Dato de estado/configuración `createdAt`; declaración estructural, interpretación específica pendiente de ampliar. |
| `source: "real-car" \| "imported"` | Dato de estado/configuración `source`; declaración estructural, interpretación específica pendiente de ampliar. |
| `polarity: IrCalibrationPolarity` | Dato de estado/configuración `polarity`; declaración estructural, interpretación específica pendiente de ampliar. |
| `material: string` | Dato de estado/configuración `material`; declaración estructural, interpretación específica pendiente de ampliar. |
| `notes: string` | Dato de estado/configuración `notes`; declaración estructural, interpretación específica pendiente de ampliar. |
| `samplesPerPoint: number` | Dato de estado/configuración `samplesPerPoint`; declaración estructural, interpretación específica pendiente de ampliar. |
| `settleSamples: number` | Dato de estado/configuración `settleSamples`; declaración estructural, interpretación específica pendiente de ampliar. |
| `baseline: IrCalibrationSensorStats \| null` | Dato de estado/configuración `baseline`; declaración estructural, interpretación específica pendiente de ampliar. |
| `points: IrCalibrationPoint[]` | Dato de estado/configuración `points`; declaración estructural, interpretación específica pendiente de ampliar. |

### `IrCalibrationParseResult`

`web:src/features/ir/irCalibration.ts:72`

| Atributo / firma | Propósito |
| --- | --- |
| `ok: boolean` | Dato de estado/configuración `ok`; declaración estructural, interpretación específica pendiente de ampliar. |
| `profile: IrCalibrationProfile \| null` | Dato de estado/configuración `profile`; declaración estructural, interpretación específica pendiente de ampliar. |
| `errors: string[]` | Dato de estado/configuración `errors`; declaración estructural, interpretación específica pendiente de ampliar. |

### `IrCalibrationCoverage`

`web:src/features/ir/irCalibration.ts:78`

| Atributo / firma | Propósito |
| --- | --- |
| `baselineReady: boolean` | Dato de estado/configuración `baselineReady`; declaración estructural, interpretación específica pendiente de ampliar. |
| `measurementCount: number` | Dato de estado/configuración `measurementCount`; declaración estructural, interpretación específica pendiente de ampliar. |
| `requiredMeasurementCount: number` | Dato de estado/configuración `requiredMeasurementCount`; declaración estructural, interpretación específica pendiente de ampliar. |
| `sensorPointCounts: Record<IrObjectSensorKey, number>` | Dato de estado/configuración `sensorPointCounts`; declaración estructural, interpretación específica pendiente de ampliar. |
| `readySensorCount: number` | Dato de estado/configuración `readySensorCount`; declaración estructural, interpretación específica pendiente de ampliar. |
| `usable: boolean` | Dato de estado/configuración `usable`; declaración estructural, interpretación específica pendiente de ampliar. |


## Funciones públicas / componentes / hooks

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-calibrationworkspace"></a>`CalibrationWorkspace(props: { profile: IrCalibrationProfile; snapshot: IrSnapshot; captureAvailable: boolean; onProfileChange: (profile: IrCalibrationProfile) => void; }): inferred-by-TypeScript` | Coordina `useCallback`, `useState`, `useWebSocket` para Herramientas de calibración IR y línea. | `web:src/features/calibration/CalibrationWorkspace.tsx:10`; structural |
| <a id="fn-ircalibrationworkbench"></a>`IrCalibrationWorkbench({   profile,   snapshot,   captureAvailable,   onProfileChange,   onBusyChange, }: IrCalibrationWorkbenchProps): inferred-by-TypeScript` | Coordina `(["prepare", "capture", "results"] as const).map`, `IR_CALIBRATION_DISTANCES_CM.map`, `IR_OBJECT_SENSOR_KEYS.map` para Herramientas de calibración IR y línea. | `web:src/features/ir/IrCalibrationWorkbench.tsx:48`; structural |
| <a id="fn-ircurve"></a>`IrCurve({ profile, sensorKey }: { profile: IrCalibrationProfile; sensorKey: IrObjectSensorKey }): inferred-by-TypeScript` | Coordina `Math.max`, `max.toFixed`, `points.map` para Herramientas de calibración IR y línea. | `web:src/features/ir/IrCalibrationWorkbench.tsx:465`; structural |
| <a id="fn-lineworkbench"></a>`LineWorkbench({ snapshot, captureAvailable, onBusyChange }: { snapshot: IrSnapshot; captureAvailable: boolean; onBusyChange: (busy: boolean) => void; }): inferred-by-TypeScript` | Coordina `history.map`, `labels.map`, `material.trim` para Herramientas de calibración IR y línea. | `web:src/features/calibration/LineWorkbench.tsx:19`; structural |
| <a id="fn-materialpicker"></a>`MaterialPicker({ value, onChange, disabled = false }: { value: string; onChange: (value: string) => void; disabled?: boolean; }): inferred-by-TypeScript` | Adapta la llamada a `(["Blanco", "Negro"] as const).map` con el contexto de esta integración. | `web:src/features/calibration/MaterialPicker.tsx:1`; structural |
| <a id="fn-buildirforwardcalibration"></a>`buildIrForwardCalibration(profile: IrCalibrationProfile): IrObjectForwardCalibration \| null` | Coordina `IR_OBJECT_SENSOR_KEYS.reduce`, `getIrCalibrationCoverage` para Herramientas de calibración IR y línea. | `web:src/features/ir/irCalibration.ts:296`; structural |
| <a id="fn-clamp"></a>`clamp(value: number, min: number, max: number): inferred-by-TypeScript` | Coordina `Math.max`, `Math.min` para Herramientas de calibración IR y línea. | `web:src/features/ir/irCalibration.ts:616`; structural |
| <a id="fn-clampadc"></a>`clampAdc(value: number): inferred-by-TypeScript` | Coordina `Math.round`, `Number`, `clamp` para Herramientas de calibración IR y línea. | `web:src/features/ir/irCalibration.ts:588`; structural |
| <a id="fn-createemptyircalibrationprofile"></a>`createEmptyIrCalibrationProfile(overrides: Partial< Pick< IrCalibrationProfile, "name" \| "polarity" \| "material" \| "notes" \| "source" > >): IrCalibrationProfile` | Adapta la llamada a `new Date().toISOString` con el contexto de esta integración. | `web:src/features/ir/irCalibration.ts:87`; structural |
| <a id="fn-createircalibrationpoint"></a>`createIrCalibrationPoint(distanceCm: number, samples: readonly IrCalibrationSample[]): IrCalibrationPoint` | Coordina `Number.isFinite`, `round`, `summarizeIrCalibrationSamples` para Herramientas de calibración IR y línea. | `web:src/features/ir/irCalibration.ts:128`; structural |
| <a id="fn-createircalibrationsensorpoint"></a>`createIrCalibrationSensorPoint(distanceCm: number, sensorKey: IrObjectSensorKey, samples: readonly IrCalibrationSample[]): IrCalibrationPoint` | Coordina `Number.isFinite`, `round`, `summarizeIrCalibrationSamples` para Herramientas de calibración IR y línea. | `web:src/features/ir/irCalibration.ts:142`; structural |
| <a id="fn-downloadtext"></a>`downloadText(fileName: string, content: string, mimeType: string): inferred-by-TypeScript` | Coordina `URL.createObjectURL`, `URL.revokeObjectURL`, `anchor.click` para Herramientas de calibración IR y línea. | `web:src/features/ir/IrCalibrationWorkbench.tsx:487`; structural |
| <a id="fn-exportircalibrationasc"></a>`exportIrCalibrationAsC(profile: IrCalibrationProfile, symbolPrefix: inferred-by-TypeScript): string` | Coordina `IR_OBJECT_SENSOR_KEYS.forEach`, `IR_OBJECT_SENSOR_KEYS.map`, `IR_OBJECT_SENSOR_KEYS.map( (key) => Math.round(calibration.baselineAdc[key]), ).join` para Herramientas de calibración IR y línea. | `web:src/features/ir/irCalibration.ts:364`; structural |
| <a id="fn-formatdistance"></a>`formatDistance(value: number): inferred-by-TypeScript` | Coordina `Number.isInteger`, `value.toFixed` para Herramientas de calibración IR y línea. | `web:src/features/ir/IrCalibrationWorkbench.tsx:506`; structural |
| <a id="fn-getircalibrationcoverage"></a>`getIrCalibrationCoverage(profile: IrCalibrationProfile): IrCalibrationCoverage` | Coordina `IR_OBJECT_SENSOR_KEYS.filter`, `IR_OBJECT_SENSOR_KEYS.reduce` para Herramientas de calibración IR y línea. | `web:src/features/ir/irCalibration.ts:332`; structural |
| <a id="fn-isrecord"></a>`isRecord(value: unknown): value is Record<string, unknown>` | Adapta la llamada a `Array.isArray` con el contexto de esta integración. | `web:src/features/ir/irCalibration.ts:580`; structural |
| <a id="fn-load"></a>`load(): LineRecord[]` | Coordina `Array.isArray`, `JSON.parse`, `localStorage.getItem` para Herramientas de calibración IR y línea. | `web:src/features/calibration/LineWorkbench.tsx:8`; structural |
| <a id="fn-loadstoredircalibrationprofile"></a>`loadStoredIrCalibrationProfile(): IrCalibrationProfile` | Coordina `createEmptyIrCalibrationProfile`, `parseIrCalibrationProfile`, `window.localStorage.getItem` para Herramientas de calibración IR y línea. | `web:src/features/ir/irCalibrationStorage.ts:6`; structural |
| <a id="fn-nonnegativeinteger"></a>`nonNegativeInteger(value: unknown, fallback: number): inferred-by-TypeScript` | Coordina `Math.max`, `Math.round`, `Number` para Herramientas de calibración IR y línea. | `web:src/features/ir/irCalibration.ts:599`; structural |
| <a id="fn-normalizecsymbol"></a>`normalizeCSymbol(value: string): inferred-by-TypeScript` | Coordina `/^[0-9]/.test`, `value.replace` para Herramientas de calibración IR y línea. | `web:src/features/ir/irCalibration.ts:611`; structural |
| <a id="fn-parseircalibrationprofile"></a>`parseIrCalibrationProfile(json: string): IrCalibrationParseResult` | Coordina `JSON.parse`, `validateIrCalibrationProfile` para Herramientas de calibración IR y línea. | `web:src/features/ir/irCalibration.ts:196`; structural |
| <a id="fn-percentile"></a>`percentile(sorted: readonly number[], percentileValue: number): inferred-by-TypeScript` | Coordina `Math.ceil`, `Math.floor`, `clamp` para Herramientas de calibración IR y línea. | `web:src/features/ir/irCalibration.ts:568`; structural |
| <a id="fn-pickobjectsample"></a>`pickObjectSample(snapshot: IrSnapshot): IrCalibrationSample` | Coordina `IR_OBJECT_SENSOR_KEYS.map`, `Object.fromEntries` para Herramientas de calibración IR y línea. | `web:src/features/ir/IrCalibrationWorkbench.tsx:481`; structural |
| <a id="fn-positiveinteger"></a>`positiveInteger(value: unknown, fallback: number): inferred-by-TypeScript` | Coordina `Math.max`, `Math.round`, `Number` para Herramientas de calibración IR y línea. | `web:src/features/ir/irCalibration.ts:592`; structural |
| <a id="fn-round"></a>`round(value: number, digits: number): inferred-by-TypeScript` | Adapta la llamada a `Math.round` con el contexto de esta integración. | `web:src/features/ir/irCalibration.ts:606`; structural |
| <a id="fn-safefilename"></a>`safeFileName(value: string): inferred-by-TypeScript` | Coordina `value .normalize`, `value .normalize("NFD") .replace`, `value .normalize("NFD") .replace(/[\u0300-\u036f]/g, "") .replace` para Herramientas de calibración IR y línea. | `web:src/features/ir/IrCalibrationWorkbench.tsx:496`; structural |
| <a id="fn-safetext"></a>`safeText(value: unknown, fallback: string): inferred-by-TypeScript` | Adapta la llamada a `value.slice` con el contexto de esta integración. | `web:src/features/ir/irCalibration.ts:584`; structural |
| <a id="fn-serializeircalibrationprofile"></a>`serializeIrCalibrationProfile(profile: IrCalibrationProfile): string` | Adapta la llamada a `JSON.stringify` con el contexto de esta integración. | `web:src/features/ir/irCalibration.ts:290`; structural |
| <a id="fn-setircalibrationbaseline"></a>`setIrCalibrationBaseline(profile: IrCalibrationProfile, samples: readonly IrCalibrationSample[]): IrCalibrationProfile` | Coordina `new Date().toISOString`, `summarizeIrCalibrationSamples` para Herramientas de calibración IR y línea. | `web:src/features/ir/irCalibration.ts:185`; structural |
| <a id="fn-summarizeircalibrationsamples"></a>`summarizeIrCalibrationSamples(samples: readonly IrCalibrationSample[]): IrCalibrationSensorStats` | Adapta la llamada a `IR_OBJECT_SENSOR_KEYS.reduce` con el contexto de esta integración. | `web:src/features/ir/irCalibration.ts:110`; structural |
| <a id="fn-summarizevalues"></a>`summarizeValues(values: readonly number[]): IrCalibrationStats` | Coordina `Math.sqrt`, `[...values].sort`, `percentile` para Herramientas de calibración IR y línea. | `web:src/features/ir/irCalibration.ts:549`; structural |
| <a id="fn-upsertircalibrationpoint"></a>`upsertIrCalibrationPoint(profile: IrCalibrationProfile, point: IrCalibrationPoint): IrCalibrationProfile` | Coordina `new Date().toISOString`, `profile.points .filter`, `profile.points .filter((candidate) => Math.abs(candidate.distanceCm - point.distanceCm) > 1e-6) .concat` para Herramientas de calibración IR y línea. | `web:src/features/ir/irCalibration.ts:159`; structural |
| <a id="fn-validateircalibrationprofile"></a>`validateIrCalibrationProfile(value: unknown): IrCalibrationParseResult` | Coordina `Array.isArray`, `errors.push`, `isRecord` para Herramientas de calibración IR y línea. | `web:src/features/ir/irCalibration.ts:212`; structural |
| <a id="fn-validatepoint"></a>`validatePoint(value: unknown, index: number, errors: string[]): IrCalibrationPoint \| null` | Coordina `Number`, `Number.isFinite`, `errors.push` para Herramientas de calibración IR y línea. | `web:src/features/ir/irCalibration.ts:433`; structural |
| <a id="fn-validatepointsensorstats"></a>`validatePointSensorStats(value: unknown, path: string, errors: string[]): IrCalibrationPointSensorStats \| null` | Coordina `IR_OBJECT_SENSOR_KEYS.forEach`, `Object.keys`, `errors.push` para Herramientas de calibración IR y línea. | `web:src/features/ir/irCalibration.ts:458`; structural |
| <a id="fn-validatesensorstats"></a>`validateSensorStats(value: unknown, path: string, errors: string[]): IrCalibrationSensorStats \| null` | Coordina `IR_OBJECT_SENSOR_KEYS.forEach`, `errors.push`, `isRecord` para Herramientas de calibración IR y línea. | `web:src/features/ir/irCalibration.ts:487`; structural |
| <a id="fn-validatestats"></a>`validateStats(value: unknown, path: string, errors: string[]): IrCalibrationStats \| null` | Coordina `Math.max`, `Number`, `clampAdc` para Herramientas de calibración IR y línea. | `web:src/features/ir/irCalibration.ts:509`; structural |

## Funciones y callbacks internos

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-ircalibrationworkbench.handleimport"></a>`IrCalibrationWorkbench.handleImport(event: ChangeEvent<HTMLInputElement>): inferred-by-TypeScript` | Coordina `file.text`, `new Date().toISOString`, `onProfileChange` para Herramientas de calibración IR y línea. | `web:src/features/ir/IrCalibrationWorkbench.tsx:155`; structural |
| <a id="fn-ircalibrationworkbench.removepoint"></a>`IrCalibrationWorkbench.removePoint(distanceCm: number): inferred-by-TypeScript` | Coordina `profile.points.filter`, `updateProfile` para Herramientas de calibración IR y línea. | `web:src/features/ir/IrCalibrationWorkbench.tsx:179`; structural |
| <a id="fn-ircalibrationworkbench.resetprofile"></a>`IrCalibrationWorkbench.resetProfile(): inferred-by-TypeScript` | Coordina `createEmptyIrCalibrationProfile`, `onProfileChange`, `setCapture` para Herramientas de calibración IR y línea. | `web:src/features/ir/IrCalibrationWorkbench.tsx:187`; structural |
| <a id="fn-ircalibrationworkbench.startcapture"></a>`IrCalibrationWorkbench.startCapture(kind: CaptureState["kind"]): inferred-by-TypeScript` | Coordina `formatDistance`, `profile.material.trim`, `setCapture` para Herramientas de calibración IR y línea. | `web:src/features/ir/IrCalibrationWorkbench.tsx:127`; structural |
| <a id="fn-ircalibrationworkbench.updateprofile"></a>`IrCalibrationWorkbench.updateProfile(patch: Partial<IrCalibrationProfile>): inferred-by-TypeScript` | Coordina `new Date().toISOString`, `onProfileChange` para Herramientas de calibración IR y línea. | `web:src/features/ir/IrCalibrationWorkbench.tsx:151`; structural |
| <a id="fn-ircurve.x"></a>`IrCurve.x(distance: number): inferred-by-TypeScript` | Operación x de Herramientas de calibración IR y línea; semántica fina pendiente de ampliación. | `web:src/features/ir/IrCalibrationWorkbench.tsx:470`; inferred-name |
| <a id="fn-ircurve.y"></a>`IrCurve.y(adc: number): inferred-by-TypeScript` | Operación y de Herramientas de calibración IR y línea; semántica fina pendiente de ampliación. | `web:src/features/ir/IrCalibrationWorkbench.tsx:471`; inferred-name |
| <a id="fn-lineworkbench.exportrecords"></a>`LineWorkbench.exportRecords(): inferred-by-TypeScript` | Coordina `JSON.stringify`, `URL.createObjectURL`, `a.click` para Herramientas de calibración IR y línea. | `web:src/features/calibration/LineWorkbench.tsx:50`; structural |
| <a id="fn-lineworkbench.median"></a>`LineWorkbench.median(values: number[]): inferred-by-TypeScript` | Adapta la llamada a `[...values].sort` con el contexto de esta integración. | `web:src/features/calibration/LineWorkbench.tsx:39`; structural |

## Estado y efectos React encontrados

| Hook | Función/closure | Evidencia |
| --- | --- | --- |
| `useState` | `CalibrationWorkspace` | `web:src/features/calibration/CalibrationWorkspace.tsx:14` |
| `useState` | `CalibrationWorkspace` | `web:src/features/calibration/CalibrationWorkspace.tsx:15` |
| `useCallback` | `CalibrationWorkspace` | `web:src/features/calibration/CalibrationWorkspace.tsx:15` |
| `useWebSocket` | `CalibrationWorkspace` | `web:src/features/calibration/CalibrationWorkspace.tsx:16` |
| `useState` | `LineWorkbench` | `web:src/features/calibration/LineWorkbench.tsx:22` |
| `useState` | `LineWorkbench` | `web:src/features/calibration/LineWorkbench.tsx:23` |
| `useState` | `LineWorkbench` | `web:src/features/calibration/LineWorkbench.tsx:23` |
| `useState` | `LineWorkbench` | `web:src/features/calibration/LineWorkbench.tsx:24` |
| `useState` | `LineWorkbench` | `web:src/features/calibration/LineWorkbench.tsx:24` |
| `useState` | `LineWorkbench` | `web:src/features/calibration/LineWorkbench.tsx:25` |
| `useState` | `LineWorkbench` | `web:src/features/calibration/LineWorkbench.tsx:25` |
| `useRef` | `LineWorkbench` | `web:src/features/calibration/LineWorkbench.tsx:26` |
| `useRef` | `LineWorkbench` | `web:src/features/calibration/LineWorkbench.tsx:26` |
| `useRef` | `LineWorkbench` | `web:src/features/calibration/LineWorkbench.tsx:26` |
| `useEffect` | `LineWorkbench` | `web:src/features/calibration/LineWorkbench.tsx:27` |
| `useEffect` | `LineWorkbench` | `web:src/features/calibration/LineWorkbench.tsx:28` |
| `useState` | `IrCalibrationWorkbench` | `web:src/features/ir/IrCalibrationWorkbench.tsx:55` |
| `useState` | `IrCalibrationWorkbench` | `web:src/features/ir/IrCalibrationWorkbench.tsx:59` |
| `useState` | `IrCalibrationWorkbench` | `web:src/features/ir/IrCalibrationWorkbench.tsx:60` |
| `useState` | `IrCalibrationWorkbench` | `web:src/features/ir/IrCalibrationWorkbench.tsx:61` |
| `useState` | `IrCalibrationWorkbench` | `web:src/features/ir/IrCalibrationWorkbench.tsx:64` |
| `useRef` | `IrCalibrationWorkbench` | `web:src/features/ir/IrCalibrationWorkbench.tsx:65` |
| `useState` | `IrCalibrationWorkbench` | `web:src/features/ir/IrCalibrationWorkbench.tsx:66` |
| `useEffect` | `IrCalibrationWorkbench` | `web:src/features/ir/IrCalibrationWorkbench.tsx:67` |
| `useEffect` | `IrCalibrationWorkbench` | `web:src/features/ir/IrCalibrationWorkbench.tsx:68` |
| `useEffect` | `IrCalibrationWorkbench` | `web:src/features/ir/IrCalibrationWorkbench.tsx:72` |
