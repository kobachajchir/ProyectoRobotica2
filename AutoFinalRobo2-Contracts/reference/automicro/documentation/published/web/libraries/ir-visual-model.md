# Geometría y modelo visual de sensores IR

ID: `web:module/src/features/ir/irSensorFrame.ts` · Capa: **núcleo** · Fotografía: 2026-10-20.

Documentación de fuente; sin tests, compilación, navegador ni validación física.

## Responsabilidad y alcance

Relaciona posiciones/ejes IR y magnitudes de sensor con haces/marcadores/lecturas visuales.

Modelo geométrico y representación de detección, separado del umbral/calibración efectiva F4.

<a id="modularity"></a>
## Modularidad observada

Cálculos separables de componentes de escena, con convenciones del montaje del proyecto.

<a id="state"></a>
## Handle, estado y atributos externos

Configuración visual local, geometría y arrays de muestras; los valores simulados conservan procedencia.

<a id="lifecycle"></a>
## Ciclo de vida y contexto

- Transformar referencia del sensor a robot/escena y aplicar muestra válida.
- Conservar magnitud/unidad antes de decidir marcador o alcance visual.

<a id="interactions"></a>
## Interacciones

- Telemetría ADC y calibración determinan interpretación; los efectos visuales no cambian emisores sin request explícito.

<a id="limitations"></a>
## Precauciones y límites

- Un haz dibujado no mide distancia ni certifica modelo óptico.
- No confundir luces blancas y grupos emisores IR.

## Hallazgos conectados


## Archivos y evidencia

| Archivo | Análisis | Hash SHA-256 |
| --- | --- | --- |
| `src/components/IrObjectDetectionMarkers.tsx` | typescript-ast-static | `30d23c2b3de719e94ce6749ef3a770385a13fdcb42724251f04479f22005b16a` |
| `src/components/SensorBeamControls.tsx` | typescript-ast-static | `a76aa5f75e4046c27899a1d875a5e43834797da279fda8dc36821b893190ecf2` |
| `src/components/StopDiscontinuityGlyph.tsx` | typescript-ast-static | `101c6ed650ac3af56fbef9f42670f68a9b6ba8cdf91032accd51755c8bb32cd9` |
| `src/components/UnderbodySensorBeams.tsx` | typescript-ast-static | `6fe5b0d70f75aaafa455ce8649143a3cd1785c28e445c38a3347c6557126bf5e` |
| `src/features/ir/beamVisualSettings.ts` | typescript-ast-static | `67dabdb64004a1df71b457e73fe30e4c61271aa461ffe12099831fb61a3e64d9` |
| `src/features/ir/irEmitterGeometry.ts` | typescript-ast-static | `192a5237bd90009883fc6db9e91828b4cdcc248d5ed374263aad927c3efe3f4b` |
| `src/features/ir/irObjectModel.ts` | typescript-ast-static | `588412b462e95a3306be01e1355111a5ed51d9ccc53cd97d30d738d036923553` |
| `src/features/ir/irSensorFrame.ts` | typescript-ast-static | `8899eb0b7f8b4850d41c64a3f51c4ba15d9354745bcee7add57f5c0e76e9070a` |
| `src/features/ir/sensorCameraViews.ts` | typescript-ast-static | `df8d3ebdc95607c97d04152b076d9afc06dd1bb59d418fdd1512fa85a72c7834` |

## Tipos, puertos, props y callbacks

En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.

### `StopDiscontinuityGlyphProps`

`web:src/components/StopDiscontinuityGlyph.tsx:1`

| Atributo / firma | Propósito |
| --- | --- |
| `className?: string` | Dato de estado/configuración `className`; declaración estructural, interpretación específica pendiente de ampliar. |

### `BeamGroup`

`web:src/features/ir/beamVisualSettings.ts:1`

Tipo: `"objects" \| "line"`.

### `BeamOpacities`

`web:src/features/ir/beamVisualSettings.ts:2`

Tipo: `Record<BeamGroup, number>`.

### `BeamVisibility`

`web:src/features/ir/beamVisualSettings.ts:3`

Tipo: `Record<BeamGroup, boolean>`.

### `IrObjectSensorKey`

`web:src/features/ir/irObjectModel.ts:17`

Tipo: `(typeof IR_OBJECT_SENSOR_KEYS)[number]`.

### `IrObjectPoint`

`web:src/features/ir/irObjectModel.ts:19`

| Atributo / firma | Propósito |
| --- | --- |
| `lateralCm: number` | Dato de estado/configuración `lateralCm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `forwardCm: number` | Dato de estado/configuración `forwardCm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `verticalCm?: number` | Dato de estado/configuración `verticalCm`; declaración estructural, interpretación específica pendiente de ampliar. |

### `IrObjectSensorGeometry`

`web:src/features/ir/irObjectModel.ts:25`

| Atributo / firma | Propósito |
| --- | --- |
| `key: IrObjectSensorKey` | Dato de estado/configuración `key`; declaración estructural, interpretación específica pendiente de ampliar. |
| `label: string` | Dato de estado/configuración `label`; declaración estructural, interpretación específica pendiente de ampliar. |
| `offsetCm: number` | Dato de estado/configuración `offsetCm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `forwardOffsetCm?: number` | Dato de estado/configuración `forwardOffsetCm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `verticalOffsetCm?: number` | Dato de estado/configuración `verticalOffsetCm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bearingDeg: number` | Ángulo en grados; respetar eje y referencia de esta librería. |

### `IrObjectCalibrationCurvePoint`

`web:src/features/ir/irObjectModel.ts:34`

| Atributo / firma | Propósito |
| --- | --- |
| `distanceCm: number` | Dato de estado/configuración `distanceCm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `adc: number` | Dato de estado/configuración `adc`; declaración estructural, interpretación específica pendiente de ampliar. |

### `IrObjectForwardCalibration`

`web:src/features/ir/irObjectModel.ts:39`

| Atributo / firma | Propósito |
| --- | --- |
| `polarity: "higher-is-closer" \| "lower-is-closer"` | Dato de estado/configuración `polarity`; declaración estructural, interpretación específica pendiente de ampliar. |
| `baselineAdc: Record<IrObjectSensorKey, number>` | Dato de estado/configuración `baselineAdc`; declaración estructural, interpretación específica pendiente de ampliar. |
| `curves: Record< IrObjectSensorKey, readonly IrObjectCalibrationCurvePoint[] >` | Dato de estado/configuración `curves`; declaración estructural, interpretación específica pendiente de ampliar. |

### `IrObjectForwardOptions`

`web:src/features/ir/irObjectModel.ts:48`

| Atributo / firma | Propósito |
| --- | --- |
| `reflectivity?: number` | Dato de estado/configuración `reflectivity`; declaración estructural, interpretación específica pendiente de ampliar. |
| `ambientAdc?: number` | Dato de estado/configuración `ambientAdc`; declaración estructural, interpretación específica pendiente de ampliar. |
| `gain?: number` | Dato de estado/configuración `gain`; declaración estructural, interpretación específica pendiente de ampliar. |
| `distanceExponent?: number` | Dato de estado/configuración `distanceExponent`; declaración estructural, interpretación específica pendiente de ampliar. |
| `angularExponent?: number` | Dato de estado/configuración `angularExponent`; declaración estructural, interpretación específica pendiente de ampliar. |
| `detectionThreshold?: number` | Dato de estado/configuración `detectionThreshold`; declaración estructural, interpretación específica pendiente de ampliar. |
| `calibration?: IrObjectForwardCalibration \| null` | Dato de estado/configuración `calibration`; declaración estructural, interpretación específica pendiente de ampliar. |

### `IrObjectForwardResult`

`web:src/features/ir/irObjectModel.ts:58`

| Atributo / firma | Propósito |
| --- | --- |
| `readings: Record<IrObjectSensorKey, number>` | Dato de estado/configuración `readings`; declaración estructural, interpretación específica pendiente de ampliar. |
| `strengths: Record<IrObjectSensorKey, number>` | Dato de estado/configuración `strengths`; declaración estructural, interpretación específica pendiente de ampliar. |
| `activeMask: number` | Dato de estado/configuración `activeMask`; declaración estructural, interpretación específica pendiente de ampliar. |
| `supportCount: number` | Dato de estado/configuración `supportCount`; declaración estructural, interpretación específica pendiente de ampliar. |

### `IrObjectPositionEstimate`

`web:src/features/ir/irObjectModel.ts:65`

| Atributo / firma | Propósito |
| --- | --- |
| `detected: boolean` | Dato de estado/configuración `detected`; declaración estructural, interpretación específica pendiente de ampliar. |
| `rangeCm: number` | Dato de estado/configuración `rangeCm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bearingDeg: number` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `confidence: number` | Dato de estado/configuración `confidence`; declaración estructural, interpretación específica pendiente de ampliar. |
| `ambiguous: boolean` | Dato de estado/configuración `ambiguous`; declaración estructural, interpretación específica pendiente de ampliar. |
| `supportCount: number` | Dato de estado/configuración `supportCount`; declaración estructural, interpretación específica pendiente de ampliar. |
| `activeMask: number` | Dato de estado/configuración `activeMask`; declaración estructural, interpretación específica pendiente de ampliar. |
| `activeSensors: IrObjectSensorKey[]` | Dato de estado/configuración `activeSensors`; declaración estructural, interpretación específica pendiente de ampliar. |
| `estimatedReflectivity: number` | Dato de estado/configuración `estimatedReflectivity`; declaración estructural, interpretación específica pendiente de ampliar. |
| `residual: number` | Dato de estado/configuración `residual`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uncertaintyCm: number` | Dato de estado/configuración `uncertaintyCm`; declaración estructural, interpretación específica pendiente de ampliar. |

### `IrObjectAxialDetection`

`web:src/features/ir/irObjectModel.ts:79`

| Atributo / firma | Propósito |
| --- | --- |
| `detected: boolean` | Dato de estado/configuración `detected`; declaración estructural, interpretación específica pendiente de ampliar. |
| `distanceCm: number \| null` | Dato de estado/configuración `distanceCm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `strength: number` | Dato de estado/configuración `strength`; declaración estructural, interpretación específica pendiente de ampliar. |
| `source: "model" \| "calibration" \| "ambiguous" \| "outside-calibration" \| "outside-range"` | Dato de estado/configuración `source`; declaración estructural, interpretación específica pendiente de ampliar. |

### `IrObjectAxialDetections`

`web:src/features/ir/irObjectModel.ts:86`

Tipo: `Record<IrObjectSensorKey, IrObjectAxialDetection>`.

### `SearchOptions`

`web:src/features/ir/irObjectModel.ts:416`

| Atributo / firma | Propósito |
| --- | --- |
| `minX: number` | Dato de estado/configuración `minX`; declaración estructural, interpretación específica pendiente de ampliar. |
| `maxX: number` | Dato de estado/configuración `maxX`; declaración estructural, interpretación específica pendiente de ampliar. |
| `minForward: number` | Dato de estado/configuración `minForward`; declaración estructural, interpretación específica pendiente de ampliar. |
| `maxForward: number` | Dato de estado/configuración `maxForward`; declaración estructural, interpretación específica pendiente de ampliar. |
| `step: number` | Dato de estado/configuración `step`; declaración estructural, interpretación específica pendiente de ampliar. |
| `observed: number[]` | Dato de estado/configuración `observed`; declaración estructural, interpretación específica pendiente de ampliar. |
| `ambient: number` | Dato de estado/configuración `ambient`; declaración estructural, interpretación específica pendiente de ampliar. |
| `rangeCm: number` | Dato de estado/configuración `rangeCm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `distanceExponent: number` | Dato de estado/configuración `distanceExponent`; declaración estructural, interpretación específica pendiente de ampliar. |
| `angularExponent: number` | Dato de estado/configuración `angularExponent`; declaración estructural, interpretación específica pendiente de ampliar. |
| `activeSensors: IrObjectSensorKey[]` | Dato de estado/configuración `activeSensors`; declaración estructural, interpretación específica pendiente de ampliar. |
| `detectionThresholdNormalized: number[]` | Dato de estado/configuración `detectionThresholdNormalized`; declaración estructural, interpretación específica pendiente de ampliar. |
| `calibration?: IrObjectForwardCalibration \| null` | Dato de estado/configuración `calibration`; declaración estructural, interpretación específica pendiente de ampliar. |

### `Candidate`

`web:src/features/ir/irObjectModel.ts:432`

| Atributo / firma | Propósito |
| --- | --- |
| `point: IrObjectPoint` | Dato de estado/configuración `point`; declaración estructural, interpretación específica pendiente de ampliar. |
| `score: number` | Dato de estado/configuración `score`; declaración estructural, interpretación específica pendiente de ampliar. |
| `reflectivity: number` | Dato de estado/configuración `reflectivity`; declaración estructural, interpretación específica pendiente de ampliar. |

### `SensorResponse`

`web:src/features/ir/irObjectModel.ts:532`

| Atributo / firma | Propósito |
| --- | --- |
| `inside: boolean` | Dato de estado/configuración `inside`; declaración estructural, interpretación específica pendiente de ampliar. |
| `distanceCm: number` | Dato de estado/configuración `distanceCm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `angularGain: number` | Dato de estado/configuración `angularGain`; declaración estructural, interpretación específica pendiente de ampliar. |
| `strength: number` | Dato de estado/configuración `strength`; declaración estructural, interpretación específica pendiente de ampliar. |

### `IrSensorRigEulerDeg`

`web:src/features/ir/irSensorFrame.ts:4`

| Atributo / firma | Propósito |
| --- | --- |
| `yaw: number` | Dato de estado/configuración `yaw`; declaración estructural, interpretación específica pendiente de ampliar. |
| `pitch: number` | Dato de estado/configuración `pitch`; declaración estructural, interpretación específica pendiente de ampliar. |
| `roll: number` | Dato de estado/configuración `roll`; declaración estructural, interpretación específica pendiente de ampliar. |

### `IrScenePoint`

`web:src/features/ir/irSensorFrame.ts:10`

Tipo: `[number, number, number]`.

### `IrSensorFrameProjection`

`web:src/features/ir/irSensorFrame.ts:12`

| Atributo / firma | Propósito |
| --- | --- |
| `pivotScene: IrScenePoint` | Dato de estado/configuración `pivotScene`; declaración estructural, interpretación específica pendiente de ampliar. |
| `sensorOriginScene: IrScenePoint` | Dato de estado/configuración `sensorOriginScene`; declaración estructural, interpretación específica pendiente de ampliar. |
| `sceneUnitsPerCm: number` | Dato de estado/configuración `sceneUnitsPerCm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `beamVisualScale: number` | Dato de estado/configuración `beamVisualScale`; declaración estructural, interpretación específica pendiente de ampliar. |

### `CameraView`

`web:src/features/ir/sensorCameraViews.ts:1`

| Atributo / firma | Propósito |
| --- | --- |
| `key: string` | Dato de estado/configuración `key`; declaración estructural, interpretación específica pendiente de ampliar. |
| `label: string` | Dato de estado/configuración `label`; declaración estructural, interpretación específica pendiente de ampliar. |
| `shortLabel: string` | Dato de estado/configuración `shortLabel`; declaración estructural, interpretación específica pendiente de ampliar. |
| `position: readonly [number, number, number]` | Dato de estado/configuración `position`; declaración estructural, interpretación específica pendiente de ampliar. |
| `up?: readonly [number, number, number]` | Dato de estado/configuración `up`; declaración estructural, interpretación específica pendiente de ampliar. |

### `SensorCameraPreset`

`web:src/features/ir/sensorCameraViews.ts:33`

Tipo: `(typeof SENSOR_CAMERA_VIEW_GROUPS)[number]["views"][number]["key"]`.


## Funciones públicas / componentes / hooks

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-detectionmarker"></a>`DetectionMarker({ config, detection, sensitivityCm, opacity }: { config: (typeof IR_EMITTER_GEOMETRY)[number]; detection: IrObjectAxialDetection; sensitivityCm: number; opacity: number; }): inferred-by-TypeScript` | Coordina `Math.max`, `Math.min`, `Math.tan` para Geometría y modelo visual de sensores IR. | `web:src/components/IrObjectDetectionMarkers.tsx:15`; structural |
| <a id="fn-irobjectdetectionmarkers"></a>`IrObjectDetectionMarkers({ detections, sensitivityCm, opacity }: { detections: IrObjectAxialDetections; sensitivityCm: number; opacity: number; }): inferred-by-TypeScript` | Adapta la llamada a `IR_EMITTER_GEOMETRY.map` con el contexto de esta integración. | `web:src/components/IrObjectDetectionMarkers.tsx:6`; structural |
| <a id="fn-opticalbeam"></a>`OpticalBeam({ positionM, direction, halfAngleDeg, color, opacity, illuminate = false, glowTexture }: { positionM: readonly [number, number, number]; direction: readonly [number, number, number]; halfAngleDeg: number; color: string; opacity: number; illuminate?: boolean; glowTexture: THREE.Texture; }): inferred-by-TypeScript` | Coordina `Math.tan`, `THREE.MathUtils.degToRad`, `createPortal` para Geometría y modelo visual de sensores IR. | `web:src/components/UnderbodySensorBeams.tsx:56`; structural |
| <a id="fn-sensorbeamcontrols"></a>`SensorBeamControls({ opacities, visibility, onOpacityChange, onVisibilityChange }: { opacities: BeamOpacities; visibility: BeamVisibility; onOpacityChange: (group: BeamGroup, opacity: number) => void; onVisibilityChange: (group: BeamGroup, visible: boolean) => void; }): inferred-by-TypeScript` | Coordina `GROUPS.map`, `useId` para Geometría y modelo visual de sensores IR. | `web:src/components/SensorBeamControls.tsx:10`; structural |
| <a id="fn-stopdiscontinuityglyph"></a>`StopDiscontinuityGlyph({   className = "", }: StopDiscontinuityGlyphProps): inferred-by-TypeScript` | Detiene Discontinuity Glyph dentro de Geometría y modelo visual de sensores IR. | `web:src/components/StopDiscontinuityGlyph.tsx:5`; inferred-name |
| <a id="fn-underbodysensorbeams"></a>`UnderbodySensorBeams({ lineEnabled, lightsEnabled, lineOpacity }: { lineEnabled: boolean \| null; lightsEnabled: boolean; lineOpacity: number; }): inferred-by-TypeScript` | Coordina `IR_LINE_EMITTER_GEOMETRY.map`, `WHITE_LED_GEOMETRY.map`, `useEffect` para Geometría y modelo visual de sensores IR. | `web:src/components/UnderbodySensorBeams.tsx:37`; structural |
| <a id="fn-calibratedadcforresponse"></a>`calibratedAdcForResponse(key: IrObjectSensorKey, response: SensorResponse, amplitude: number, calibration: IrObjectForwardCalibration): inferred-by-TypeScript` | Coordina `Math.max`, `Math.round`, `clamp` para Geometría y modelo visual de sensores IR. | `web:src/features/ir/irObjectModel.ts:598`; structural |
| <a id="fn-calibrateddistancestrength"></a>`calibratedDistanceStrength(key: IrObjectSensorKey, distanceCm: number, calibration: IrObjectForwardCalibration): inferred-by-TypeScript` | Coordina `calibrationMaximumSignal`, `clamp01`, `interpolateCalibrationCurve` para Geometría y modelo visual de sensores IR. | `web:src/features/ir/irObjectModel.ts:631`; structural |
| <a id="fn-calibrationmaximumsignal"></a>`calibrationMaximumSignal(key: IrObjectSensorKey, calibration: IrObjectForwardCalibration): inferred-by-TypeScript` | Adapta la llamada a `calibration.curves[key].reduce` con el contexto de esta integración. | `web:src/features/ir/irObjectModel.ts:701`; structural |
| <a id="fn-clamp"></a>`clamp(value: number, min: number, max: number): inferred-by-TypeScript` | Coordina `Math.max`, `Math.min` para Geometría y modelo visual de sensores IR. | `web:src/features/ir/irObjectModel.ts:781`; structural |
| <a id="fn-clamp01"></a>`clamp01(value: number): inferred-by-TypeScript` | Adapta la llamada a `clamp` con el contexto de esta integración. | `web:src/features/ir/irObjectModel.ts:785`; structural |
| <a id="fn-cleanangle"></a>`cleanAngle(value: number): inferred-by-TypeScript` | Adapta la llamada a `Math.abs` con el contexto de esta integración. | `web:src/features/ir/irSensorFrame.ts:114`; structural |
| <a id="fn-collectcandidates"></a>`collectCandidates(options: SearchOptions): Candidate[]` | Coordina `candidates.push`, `evaluateCandidate` para Geometría y modelo visual de sensores IR. | `web:src/features/ir/irObjectModel.ts:446`; structural |
| <a id="fn-createobjectsensorrecord"></a>`createObjectSensorRecord(value: inferred-by-TypeScript): Record<IrObjectSensorKey, number>` | Adapta la llamada a `IR_OBJECT_SENSOR_KEYS.reduce` con el contexto de esta integración. | `web:src/features/ir/irObjectModel.ts:340`; structural |
| <a id="fn-createrigquaternion"></a>`createRigQuaternion(eulerDeg: IrSensorRigEulerDeg): inferred-by-TypeScript` | Coordina `THREE.MathUtils.degToRad`, `new THREE.Quaternion().setFromEuler` para Geometría y modelo visual de sensores IR. | `web:src/features/ir/irSensorFrame.ts:103`; structural |
| <a id="fn-createwhiteglowtexture"></a>`createWhiteGlowTexture(): inferred-by-TypeScript` | Coordina `canvas.getContext`, `context.createRadialGradient`, `context.fillRect` para Geometría y modelo visual de sensores IR. | `web:src/components/UnderbodySensorBeams.tsx:17`; structural |
| <a id="fn-degtorad"></a>`degToRad(value: number): inferred-by-TypeScript` | Operación deg To Rad de Geometría y modelo visual de sensores IR; semántica fina pendiente de ampliación. | `web:src/features/ir/irObjectModel.ts:789`; inferred-name |
| <a id="fn-emptyestimate"></a>`emptyEstimate(activeMask: number, activeSensors: IrObjectSensorKey[]): IrObjectPositionEstimate` | Operación empty Estimate de Geometría y modelo visual de sensores IR; semántica fina pendiente de ampliación. | `web:src/features/ir/irObjectModel.ts:760`; inferred-name |
| <a id="fn-estimateirobjectaxialdetections"></a>`estimateIrObjectAxialDetections(readings: Readonly<Record<IrObjectSensorKey, number>>, sensitivityCm: number, calibration?: IrObjectForwardCalibration \| null, available: inferred-by-TypeScript): IrObjectAxialDetections` | Coordina `IR_OBJECT_SENSOR_KEYS.reduce`, `Number.isFinite`, `clamp` para Geometría y modelo visual de sensores IR. | `web:src/features/ir/irObjectModel.ts:362`; structural |
| <a id="fn-estimateirobjectposition"></a>`estimateIrObjectPosition(readings: Readonly<Record<IrObjectSensorKey, number>>, sensitivityCm: number, options: Pick< IrObjectForwardOptions, \| "ambientAdc" \| "distanceExponent" \| "angularExponent" \| "detectionThreshold" \| "calibration" >): IrObjectPositionEstimate` | Coordina `IR_OBJECT_SENSOR_KEYS.filter`, `IR_OBJECT_SENSOR_KEYS.map`, `IR_OBJECT_SENSOR_LAYOUT.map` para Geometría y modelo visual de sensores IR. | `web:src/features/ir/irObjectModel.ts:203`; structural |
| <a id="fn-evaluatecandidate"></a>`evaluateCandidate(point: IrObjectPoint, options: Pick< SearchOptions, \| "observed" \| "ambient" \| "rangeCm" \| "distanceExponent" \| "angularExponent" \| "activeSensors" \| "detectionThresholdNormalized" \| "calibration" >): Candidate \| null` | Coordina `IR_OBJECT_SENSOR_LAYOUT.map`, `centeredObserved.reduce`, `clamp` para Geometría y modelo visual de sensores IR. | `web:src/features/ir/irObjectModel.ts:472`; structural |
| <a id="fn-interpolatecalibrationcurve"></a>`interpolateCalibrationCurve(curve: readonly IrObjectCalibrationCurvePoint[], distanceCm: number): inferred-by-TypeScript` | Coordina `Math.max`, `[...curve].sort`, `linearCurveValue` para Geometría y modelo visual de sensores IR. | `web:src/features/ir/irObjectModel.ts:715`; structural |
| <a id="fn-inverserotatescenepointaroundpivot"></a>`inverseRotateScenePointAroundPivot(worldScenePoint: readonly [number, number, number], eulerDeg: IrSensorRigEulerDeg, pivotScene: readonly [number, number, number]): IrScenePoint` | Coordina `createRigQuaternion`, `createRigQuaternion(eulerDeg).invert`, `new THREE.Vector3(...worldScenePoint) .sub` para Geometría y modelo visual de sensores IR. | `web:src/features/ir/irSensorFrame.ts:32`; structural |
| <a id="fn-irobjectpointtolocalscenepoint"></a>`irObjectPointToLocalScenePoint(point: IrObjectPoint, projection: IrSensorFrameProjection): IrScenePoint` | Operación ir Object Point To Local Scene Point de Geometría y modelo visual de sensores IR; semántica fina pendiente de ampliación. | `web:src/features/ir/irSensorFrame.ts:69`; inferred-name |
| <a id="fn-ispointinsideircone"></a>`isPointInsideIrCone(point: IrObjectPoint, sensor: IrObjectSensorGeometry, sensitivityCm: number): boolean` | Adapta la llamada a `sensorResponseAtPoint` con el contexto de esta integración. | `web:src/features/ir/irObjectModel.ts:130`; structural |
| <a id="fn-linearcurvevalue"></a>`linearCurveValue(left: IrObjectCalibrationCurvePoint, right: IrObjectCalibrationCurvePoint, distanceCm: number): inferred-by-TypeScript` | Adapta la llamada a `Math.max` con el contexto de esta integración. | `web:src/features/ir/irObjectModel.ts:750`; structural |
| <a id="fn-normalizeirobjectreading"></a>`normalizeIrObjectReading(key: IrObjectSensorKey, adc: number, calibration?: IrObjectForwardCalibration \| null): inferred-by-TypeScript` | Adapta la llamada a `normalizeObservedReading` con el contexto de esta integración. | `web:src/features/ir/irObjectModel.ts:352`; structural |
| <a id="fn-normalizeobservedreading"></a>`normalizeObservedReading(key: IrObjectSensorKey, adc: number, calibration: IrObjectForwardCalibration \| null \| undefined, ambient: number): inferred-by-TypeScript` | Coordina `calibrationMaximumSignal`, `clamp`, `clamp01` para Geometría y modelo visual de sensores IR. | `web:src/features/ir/irObjectModel.ts:649`; structural |
| <a id="fn-normalizeddetectionthreshold"></a>`normalizedDetectionThreshold(key: IrObjectSensorKey, thresholdAdc: number, calibration: IrObjectForwardCalibration \| null \| undefined): inferred-by-TypeScript` | Adapta la llamada a `calibrationMaximumSignal` con el contexto de esta integración. | `web:src/features/ir/irObjectModel.ts:686`; structural |
| <a id="fn-radtodeg"></a>`radToDeg(value: number): inferred-by-TypeScript` | Operación rad To Deg de Geometría y modelo visual de sensores IR; semántica fina pendiente de ampliación. | `web:src/features/ir/irObjectModel.ts:793`; inferred-name |
| <a id="fn-relativerigeulerdeg"></a>`relativeRigEulerDeg(current: IrSensorRigEulerDeg, zero: IrSensorRigEulerDeg): IrSensorRigEulerDeg` | Coordina `THREE.MathUtils.radToDeg`, `cleanAngle`, `createRigQuaternion` para Geometría y modelo visual de sensores IR. | `web:src/features/ir/irSensorFrame.ts:84`; structural |
| <a id="fn-rotatescenepointaroundpivot"></a>`rotateScenePointAroundPivot(localScenePoint: readonly [number, number, number], eulerDeg: IrSensorRigEulerDeg, pivotScene: readonly [number, number, number]): IrScenePoint` | Coordina `createRigQuaternion`, `new THREE.Vector3(...localScenePoint) .sub`, `new THREE.Vector3(...localScenePoint) .sub(pivot) .applyQuaternion` para Geometría y modelo visual de sensores IR. | `web:src/features/ir/irSensorFrame.ts:19`; structural |
| <a id="fn-searchbestcandidate"></a>`searchBestCandidate(options: SearchOptions): Candidate \| null` | Coordina `candidates.reduce`, `collectCandidates` para Geometría y modelo visual de sensores IR. | `web:src/features/ir/irObjectModel.ts:438`; structural |
| <a id="fn-sensorresponseatpoint"></a>`sensorResponseAtPoint(point: IrObjectPoint, sensor: IrObjectSensorGeometry, sensitivityCm: number, distanceExponent: inferred-by-TypeScript, angularExponent: inferred-by-TypeScript, calibration?: IrObjectForwardCalibration \| null): SensorResponse` | Coordina `Math.cos`, `Math.hypot`, `Math.max` para Geometría y modelo visual de sensores IR. | `web:src/features/ir/irObjectModel.ts:539`; structural |
| <a id="fn-signaladcabovebaseline"></a>`signalAdcAboveBaseline(key: IrObjectSensorKey, adc: number, calibration: IrObjectForwardCalibration \| null \| undefined, ambientAdc: number): inferred-by-TypeScript` | Coordina `Math.max`, `clamp` para Geometría y modelo visual de sensores IR. | `web:src/features/ir/irObjectModel.ts:670`; structural |
| <a id="fn-simulateirobjectreadings"></a>`simulateIrObjectReadings(point: IrObjectPoint, sensitivityCm: number, options: IrObjectForwardOptions): IrObjectForwardResult` | Coordina `IR_OBJECT_SENSOR_LAYOUT.forEach`, `Math.max`, `clamp` para Geometría y modelo visual de sensores IR. | `web:src/features/ir/irObjectModel.ts:138`; structural |
| <a id="fn-worldscenepointtoirobjectpoint"></a>`worldScenePointToIrObjectPoint(worldScenePoint: readonly [number, number, number], eulerDeg: IrSensorRigEulerDeg, projection: IrSensorFrameProjection): IrObjectPoint` | Adapta la llamada a `inverseRotateScenePointAroundPivot` con el contexto de esta integración. | `web:src/features/ir/irSensorFrame.ts:45`; structural |

## Funciones y callbacks internos

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |

## Estado y efectos React encontrados

| Hook | Función/closure | Evidencia |
| --- | --- | --- |
| `useId` | `SensorBeamControls` | `web:src/components/SensorBeamControls.tsx:16` |
| `useMemo` | `UnderbodySensorBeams` | `web:src/components/UnderbodySensorBeams.tsx:42` |
| `useEffect` | `UnderbodySensorBeams` | `web:src/components/UnderbodySensorBeams.tsx:43` |
| `useRef` | `OpticalBeam` | `web:src/components/UnderbodySensorBeams.tsx:65` |
| `useRef` | `OpticalBeam` | `web:src/components/UnderbodySensorBeams.tsx:66` |
| `useRef` | `OpticalBeam` | `web:src/components/UnderbodySensorBeams.tsx:67` |
| `useRef` | `OpticalBeam` | `web:src/components/UnderbodySensorBeams.tsx:68` |
| `useRef` | `OpticalBeam` | `web:src/components/UnderbodySensorBeams.tsx:69` |
| `useThree` | `OpticalBeam` | `web:src/components/UnderbodySensorBeams.tsx:70` |
| `useMemo` | `OpticalBeam` | `web:src/components/UnderbodySensorBeams.tsx:73` |
| `useMemo` | `OpticalBeam` | `web:src/components/UnderbodySensorBeams.tsx:75` |
| `useMemo` | `OpticalBeam` | `web:src/components/UnderbodySensorBeams.tsx:78` |
| `useMemo` | `OpticalBeam` | `web:src/components/UnderbodySensorBeams.tsx:83` |
| `useEffect` | `OpticalBeam` | `web:src/components/UnderbodySensorBeams.tsx:96` |
| `useFrame` | `OpticalBeam` | `web:src/components/UnderbodySensorBeams.tsx:100` |
