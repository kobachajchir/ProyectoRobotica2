# Calibración y seguimiento de línea del dispositivo

ID: `web:module/src/features/lineFollow/useLineDevice.ts` · Capa: **integración** · Fotografía: 2026-10-20.

Documentación de fuente; sin tests, compilación, navegador ni validación física.

## Responsabilidad y alcance

Presenta sesión de calibración blanco/pista y habilita seguimiento con estado del dispositivo.

Operaciones/sessionId y estado de 168 bytes convertido por ESP; no recalcula captura DMA en Web.

<a id="modularity"></a>
## Modularidad observada

Integración de API y UI; separada de las herramientas de calibración local/simulada.

<a id="state"></a>
## Handle, estado y atributos externos

Hook mantiene estado/progreso/rechazo; el ID de sesión confirma/cancela la operación del equipo, no la identidad de un componente.

<a id="lifecycle"></a>
## Ciclo de vida y contexto

- Arrancar blanco/pista con sesión inicial; usar sessionId devuelto para confirmar/cancelar.
- Mostrar estabilidad/calidad/frescura/rechazo sin convertir guardas en éxito.

<a id="interactions"></a>
## Interacciones

- F4 posee perfiles y revisión blanco-pista; ESP decodifica y Web presenta.
- Códigos OLED de esta familia requieren render/campos además de controles de página Web.

<a id="limitations"></a>
## Precauciones y límites

- Botón de seguimiento y pantalla OLED de seguimiento no son el mismo consumidor.
- No asumir render exacto por tener una página de calibración operativa.

## Hallazgos conectados

- [AMDOC-WEB-009: El catálogo de línea no tiene bindings específicos de visor localizados](../../../review/FINDINGS.md#amdoc-web-009)

## Archivos y evidencia

| Archivo | Análisis | Hash SHA-256 |
| --- | --- | --- |
| `src/features/lineFollow/LineDeviceCalibration.tsx` | typescript-ast-static | `892057e0736bf8a4823da53a9b557cc570c8fe704d219529ddba663902c66960` |
| `src/features/lineFollow/LineFollowPanel.tsx` | typescript-ast-static | `1051126febe29e46693f4f4949093bf41ff244f36267fe7713db4a4df3370d7d` |
| `src/features/lineFollow/lineFollow.css` | support-inventory | `3ac7b6f84ac33b883a416e6143c3055afa719c120dd5119a5411eb0f5ef89cf8` |
| `src/features/lineFollow/useLineDevice.ts` | typescript-ast-static | `679cb499155b751d9242e65d9af53b3fb4c264ec1c12af9c115b2addb2c5fbe5` |
| `src/pages/TrackFollowerSection.tsx` | typescript-ast-static | `ae100afc801f8fc22c75a643b8cefe4190b582fdcdbdc86a6f9d328c4d1415d8` |

## Tipos, puertos, props y callbacks

En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.

### `LineProfile`

`web:src/features/lineFollow/useLineDevice.ts:6`

| Atributo / firma | Propósito |
| --- | --- |
| `means: number[][]` | Dato de estado/configuración `means`; declaración estructural, interpretación específica pendiente de ampliar. |
| `noise: number[][]` | Dato de estado/configuración `noise`; declaración estructural, interpretación específica pendiente de ampliar. |
| `usefulScans: number` | Dato de estado/configuración `usefulScans`; declaración estructural, interpretación específica pendiente de ampliar. |
| `pitchDeg: number` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `rollDeg: number` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `yawDeg: number` | Ángulo en grados; respetar eje y referencia de esta librería. |

### `LineDeviceState`

`web:src/features/lineFollow/useLineDevice.ts:7`

| Atributo / firma | Propósito |
| --- | --- |
| `status: number` | Último resultado y guardas del módulo; los getters pueden devolver una vista de este almacenamiento. |
| `version: number` | Dato de estado/configuración `version`; declaración estructural, interpretación específica pendiente de ampliar. |
| `calibrationState: number` | Dato de estado/configuración `calibrationState`; declaración estructural, interpretación específica pendiente de ampliar. |
| `target: number` | Dato de estado/configuración `target`; declaración estructural, interpretación específica pendiente de ampliar. |
| `phase: number` | Dato de estado/configuración `phase`; declaración estructural, interpretación específica pendiente de ampliar. |
| `progress: number` | Dato de estado/configuración `progress`; declaración estructural, interpretación específica pendiente de ampliar. |
| `calibrationReason: number` | Dato de estado/configuración `calibrationReason`; declaración estructural, interpretación específica pendiente de ampliar. |
| `followReason: number` | Dato de estado/configuración `followReason`; declaración estructural, interpretación específica pendiente de ampliar. |
| `whiteValid: boolean` | Dato de estado/configuración `whiteValid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `trackValid: boolean` | Dato de estado/configuración `trackValid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `balanceSettled: boolean` | Dato de estado/configuración `balanceSettled`; declaración estructural, interpretación específica pendiente de ampliar. |
| `following: boolean` | Dato de estado/configuración `following`; declaración estructural, interpretación específica pendiente de ampliar. |
| `motionArmed: boolean` | Dato de estado/configuración `motionArmed`; declaración estructural, interpretación específica pendiente de ampliar. |
| `balanceActive: boolean` | Dato de estado/configuración `balanceActive`; declaración estructural, interpretación específica pendiente de ampliar. |
| `safetyStop: boolean` | Dato de estado/configuración `safetyStop`; declaración estructural, interpretación específica pendiente de ampliar. |
| `routeValid: boolean` | Dato de estado/configuración `routeValid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `carMode: number` | Dato de estado/configuración `carMode`; declaración estructural, interpretación específica pendiente de ampliar. |
| `pattern: number` | Dato de estado/configuración `pattern`; declaración estructural, interpretación específica pendiente de ampliar. |
| `confidence: number` | Dato de estado/configuración `confidence`; declaración estructural, interpretación específica pendiente de ampliar. |
| `sessionId: number` | Dato de estado/configuración `sessionId`; declaración estructural, interpretación específica pendiente de ampliar. |
| `whiteRevision: number` | Dato de estado/configuración `whiteRevision`; declaración estructural, interpretación específica pendiente de ampliar. |
| `trackWhiteRevision: number` | Dato de estado/configuración `trackWhiteRevision`; declaración estructural, interpretación específica pendiente de ampliar. |
| `usefulScans: number` | Dato de estado/configuración `usefulScans`; declaración estructural, interpretación específica pendiente de ampliar. |
| `neutralPitchDeg: number` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `referenceRollDeg: number` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `referenceYawDeg: number` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `white: LineProfile` | Dato de estado/configuración `white`; declaración estructural, interpretación específica pendiente de ampliar. |
| `track: LineProfile` | Dato de estado/configuración `track`; declaración estructural, interpretación específica pendiente de ampliar. |

### `LineDevice`

`web:src/features/lineFollow/useLineDevice.ts:128`

Tipo: `ReturnType<typeof useLineDevice>`.

### `CameraMode`

`web:src/pages/TrackFollowerSection.tsx:43`

Tipo: `"top" \| "iso" \| "chase" \| "orbit"`.

### `RenderMode`

`web:src/pages/TrackFollowerSection.tsx:44`

Tipo: `"points" \| "ribbon"`.

### `SampleSource`

`web:src/pages/TrackFollowerSection.tsx:45`

Tipo: `"real" \| "simulado"`.

### `TrackPose`

`web:src/pages/TrackFollowerSection.tsx:47`

| Atributo / firma | Propósito |
| --- | --- |
| `x: number` | Dato de estado/configuración `x`; declaración estructural, interpretación específica pendiente de ampliar. |
| `z: number` | Dato de estado/configuración `z`; declaración estructural, interpretación específica pendiente de ampliar. |
| `theta: number` | Dato de estado/configuración `theta`; declaración estructural, interpretación específica pendiente de ampliar. |
| `seq: number` | Dato de estado/configuración `seq`; declaración estructural, interpretación específica pendiente de ampliar. |

### `TrackPoint`

`web:src/pages/TrackFollowerSection.tsx:54`

| Atributo / firma | Propósito |
| --- | --- |
| `id: string` | Dato de estado/configuración `id`; declaración estructural, interpretación específica pendiente de ampliar. |
| `seq: number` | Dato de estado/configuración `seq`; declaración estructural, interpretación específica pendiente de ampliar. |
| `x: number` | Dato de estado/configuración `x`; declaración estructural, interpretación específica pendiente de ampliar. |
| `z: number` | Dato de estado/configuración `z`; declaración estructural, interpretación específica pendiente de ampliar. |
| `confidence: number` | Dato de estado/configuración `confidence`; declaración estructural, interpretación específica pendiente de ampliar. |
| `lateralErrorMm: number` | Dato de estado/configuración `lateralErrorMm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `source: SampleSource` | Dato de estado/configuración `source`; declaración estructural, interpretación específica pendiente de ampliar. |
| `segmentId: number` | Dato de estado/configuración `segmentId`; declaración estructural, interpretación específica pendiente de ampliar. |
| `tsHostMs: number` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |

### `ObstaclePoint`

`web:src/pages/TrackFollowerSection.tsx:66`

| Atributo / firma | Propósito |
| --- | --- |
| `id: string` | Dato de estado/configuración `id`; declaración estructural, interpretación específica pendiente de ampliar. |
| `seq: number` | Dato de estado/configuración `seq`; declaración estructural, interpretación específica pendiente de ampliar. |
| `x: number` | Dato de estado/configuración `x`; declaración estructural, interpretación específica pendiente de ampliar. |
| `z: number` | Dato de estado/configuración `z`; declaración estructural, interpretación específica pendiente de ampliar. |
| `confidence: number` | Dato de estado/configuración `confidence`; declaración estructural, interpretación específica pendiente de ampliar. |
| `label: string` | Dato de estado/configuración `label`; declaración estructural, interpretación específica pendiente de ampliar. |
| `source: SampleSource` | Dato de estado/configuración `source`; declaración estructural, interpretación específica pendiente de ampliar. |

### `StreamState`

`web:src/pages/TrackFollowerSection.tsx:76`

| Atributo / firma | Propósito |
| --- | --- |
| `active: boolean` | Dato de estado/configuración `active`; declaración estructural, interpretación específica pendiente de ampliar. |
| `pending: boolean` | Dato de estado/configuración `pending`; declaración estructural, interpretación específica pendiente de ampliar. |
| `packets: number` | Dato de estado/configuración `packets`; declaración estructural, interpretación específica pendiente de ampliar. |
| `lastSeq: number \| null` | Dato de estado/configuración `lastSeq`; declaración estructural, interpretación específica pendiente de ampliar. |
| `updatedAt: number \| null` | Dato de estado/configuración `updatedAt`; declaración estructural, interpretación específica pendiente de ampliar. |
| `statusText: string` | Dato de estado/configuración `statusText`; declaración estructural, interpretación específica pendiente de ampliar. |
| `lastFrameHex: string` | Dato de estado/configuración `lastFrameHex`; declaración estructural, interpretación específica pendiente de ampliar. |

### `LineObservation`

`web:src/pages/TrackFollowerSection.tsx:706`

| Atributo / firma | Propósito |
| --- | --- |
| `present: boolean` | Dato de estado/configuración `present`; declaración estructural, interpretación específica pendiente de ampliar. |
| `confidence: number` | Dato de estado/configuración `confidence`; declaración estructural, interpretación específica pendiente de ampliar. |
| `lateralM: number` | Dato de estado/configuración `lateralM`; declaración estructural, interpretación específica pendiente de ampliar. |


## Funciones públicas / componentes / hooks

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-consolegroup"></a>`ConsoleGroup({   title,   children, }: { title: string; children: ReactNode; }): inferred-by-TypeScript` | Operación Console Group de Calibración y seguimiento de línea del dispositivo; semántica fina pendiente de ampliación. | `web:src/pages/TrackFollowerSection.tsx:1217`; inferred-name |
| <a id="fn-fallbackrobotbody"></a>`FallbackRobotBody(): inferred-by-TypeScript` | Operación Fallback Robot Body de Calibración y seguimiento de línea del dispositivo; semántica fina pendiente de ampliación. | `web:src/pages/TrackFollowerSection.tsx:1077`; inferred-name |
| <a id="fn-fallbackrobotmarker"></a>`FallbackRobotMarker({   pose,   latestLine, }: { pose: TrackPose; latestLine: LineObservation; }): inferred-by-TypeScript` | Operación Fallback Robot Marker de Calibración y seguimiento de línea del dispositivo; semántica fina pendiente de ampliación. | `web:src/pages/TrackFollowerSection.tsx:1062`; inferred-name |
| <a id="fn-inlinetoggle"></a>`InlineToggle({   label,   active,   onClick, }: { label: string; active: boolean; onClick: () => void; }): inferred-by-TypeScript` | Operación Inline Toggle de Calibración y seguimiento de línea del dispositivo; semántica fina pendiente de ampliación. | `web:src/pages/TrackFollowerSection.tsx:1270`; inferred-name |
| <a id="fn-linedevicecalibration"></a>`LineDeviceCalibration({ device, embedded=false }: { device: LineDevice; embedded?:boolean }): inferred-by-TypeScript` | Coordina `Math.min`, `[s.white, s.track].map`, `lineReason` para Calibración y seguimiento de línea del dispositivo. | `web:src/features/lineFollow/LineDeviceCalibration.tsx:9`; structural |
| <a id="fn-linedevicecalibrationpanel"></a>`LineDeviceCalibrationPanel({ onBusyChange }: { onBusyChange?: (busy: boolean) => void }): inferred-by-TypeScript` | Coordina `useEffect`, `useLineDevice` para Calibración y seguimiento de línea del dispositivo. | `web:src/features/lineFollow/LineDeviceCalibration.tsx:53`; structural |
| <a id="fn-linefollowpanel"></a>`LineFollowPanel(): inferred-by-TypeScript` | Coordina `[!s.whiteValid ? "Calibrar blanco: los tres sensores sobre blanco" : null, !s.trackValid ? "Calibrar sobre pista: blanco, negro, blanco" : null].filter`, `["Iniciando", "Deteniendo"].includes`, `lineReason` para Calibración y seguimiento de línea del dispositivo. | `web:src/features/lineFollow/LineFollowPanel.tsx:12`; structural |
| <a id="fn-obstacleinstances"></a>`ObstacleInstances({ obstacles }: { obstacles: ObstaclePoint[] }): inferred-by-TypeScript` | Coordina `obstacles.slice`, `obstacles.slice(-MAX_OBSTACLES).map` para Calibración y seguimiento de línea del dispositivo. | `web:src/pages/TrackFollowerSection.tsx:989`; structural |
| <a id="fn-panelcard"></a>`PanelCard({   title,   kicker,   children, }: { title: string; kicker: string; children: ReactNode; }): inferred-by-TypeScript` | Operación Panel Card de Calibración y seguimiento de línea del dispositivo; semántica fina pendiente de ampliación. | `web:src/pages/TrackFollowerSection.tsx:1197`; inferred-name |
| <a id="fn-segmentbutton"></a>`SegmentButton({   active,   label,   onClick, }: { active: boolean; label: string; onClick: () => void; }): inferred-by-TypeScript` | Operación Segment Button de Calibración y seguimiento de línea del dispositivo; semántica fina pendiente de ampliación. | `web:src/pages/TrackFollowerSection.tsx:1249`; inferred-name |
| <a id="fn-sensorbars"></a>`SensorBars({ snapshot }: { snapshot: IrSnapshot }): inferred-by-TypeScript` | Adapta la llamada a `LINE_CHANNELS.map` con el contexto de esta integración. | `web:src/pages/TrackFollowerSection.tsx:1177`; structural |
| <a id="fn-statuspair"></a>`StatusPair({   label,   value,   tone = "muted", }: { label: string; value: string; tone?: "ok" \| "error" \| "info" \| "warn" \| "muted"; }): inferred-by-TypeScript` | Operación Status Pair de Calibración y seguimiento de línea del dispositivo; semántica fina pendiente de ampliación. | `web:src/pages/TrackFollowerSection.tsx:1232`; inferred-name |
| <a id="fn-trackactionbutton"></a>`TrackActionButton({   title,   subtitle,   tone,   disabled = false,   onClick, }: { title: string; subtitle: string; tone: "cyan" \| "emerald" \| "rose" \| "muted"; disabled?: boolean; onClick: () => void; }): inferred-by-TypeScript` | Adapta la llamada a `title.startsWith` con el contexto de esta integración. | `web:src/pages/TrackFollowerSection.tsx:1293`; structural |
| <a id="fn-trackcamerarig"></a>`TrackCameraRig({ pose, mode, onManualCameraChange }: { pose: TrackPose; mode: CameraMode \| null; onManualCameraChange: () => void; }): inferred-by-TypeScript` | Coordina `useEffect`, `useFrame`, `useRef` para Calibración y seguimiento de línea del dispositivo. | `web:src/pages/TrackFollowerSection.tsx:831`; structural |
| <a id="fn-trackfollowersection"></a>`TrackFollowerSection(): inferred-by-TypeScript` | Coordina `Math.round`, `String`, `formatDecimal` para Calibración y seguimiento de línea del dispositivo. | `web:src/pages/TrackFollowerSection.tsx:123`; structural |
| <a id="fn-trackground"></a>`TrackGround(): inferred-by-TypeScript` | Adapta la llamada a `useMemo` con el contexto de esta integración. | `web:src/pages/TrackFollowerSection.tsx:895`; structural |
| <a id="fn-trackmapscene"></a>`TrackMapScene({   modelUrl,   points,   obstacles,   pose,   cameraMode,   onManualCameraChange,   renderMode,   debugSensors,   objectDetections,   latestLine, }: { modelUrl: string; points: TrackPoint[]; obstacles: ObstaclePoint[]; pose: TrackPose; cameraMode: CameraMode \| null; onManualCameraChange: () => void; renderMode: RenderMode; debugSensors: boolean; objectDetections: IrObjectAxialDetections; latestLine: LineObservation; }): inferred-by-TypeScript` | Operación Track Map Scene de Calibración y seguimiento de línea del dispositivo; semántica fina pendiente de ampliación. | `web:src/pages/TrackFollowerSection.tsx:772`; inferred-name |
| <a id="fn-trackpointinstances"></a>`TrackPointInstances({ points }: { points: TrackPoint[] }): inferred-by-TypeScript` | Coordina `useEffect`, `useMemo`, `useRef` para Calibración y seguimiento de línea del dispositivo. | `web:src/pages/TrackFollowerSection.tsx:915`; structural |
| <a id="fn-trackribbon"></a>`TrackRibbon({ points }: { points: TrackPoint[] }): inferred-by-TypeScript` | Coordina `segments.map`, `useMemo` para Calibración y seguimiento de línea del dispositivo. | `web:src/pages/TrackFollowerSection.tsx:951`; structural |
| <a id="fn-tracksensoroverlay"></a>`TrackSensorOverlay({   latestLine, }: { latestLine: LineObservation; }): inferred-by-TypeScript` | Operación Track Sensor Overlay de Calibración y seguimiento de línea del dispositivo; semántica fina pendiente de ampliación. | `web:src/pages/TrackFollowerSection.tsx:1092`; inferred-name |
| <a id="fn-trackstatusstrip"></a>`TrackStatusStrip({   connected,   stream,   mappingMode,   mapQuality,   latestLine,   pointCount,   obstaclesCount,   seqGapCount, }: { connected: boolean; stream: StreamState; mappingMode: string; mapQuality: string; latestLine: LineObservation; pointCount: number; obstaclesCount: number; seqGapCount: number; }): inferred-by-TypeScript` | Coordina `Math.round`, `String`, `getEspConnectionDetail` para Calibración y seguimiento de línea del dispositivo. | `web:src/pages/TrackFollowerSection.tsx:1113`; structural |
| <a id="fn-trackvehiclemodel"></a>`TrackVehicleModel({   modelUrl,   pose,   latestLine,   debugSensors,   objectDetections, }: { modelUrl: string; pose: TrackPose; latestLine: LineObservation; debugSensors: boolean; objectDetections: IrObjectAxialDetections; }): inferred-by-TypeScript` | Coordina `useEffect`, `useLoader`, `useMemo` para Calibración y seguimiento de línea del dispositivo. | `web:src/pages/TrackFollowerSection.tsx:1002`; structural |
| <a id="fn-calibrationbusy"></a>`calibrationBusy(s: LineDeviceState \| null): inferred-by-TypeScript` | Operación calibration Busy de Calibración y seguimiento de línea del dispositivo; semántica fina pendiente de ampliación. | `web:src/features/lineFollow/useLineDevice.ts:29`; inferred-name |
| <a id="fn-clamp"></a>`clamp(value: number, min: number, max: number): inferred-by-TypeScript` | Coordina `Math.max`, `Math.min`, `Number.isFinite` para Calibración y seguimiento de línea del dispositivo. | `web:src/pages/TrackFollowerSection.tsx:1407`; structural |
| <a id="fn-clamp01"></a>`clamp01(value: number): inferred-by-TypeScript` | Adapta la llamada a `clamp` con el contexto de esta integración. | `web:src/pages/TrackFollowerSection.tsx:1403`; structural |
| <a id="fn-createidletrackirsnapshot"></a>`createIdleTrackIrSnapshot(periodMs: number): IrSnapshot` | Coordina `IR_SENSOR_ORDER.map`, `Object.fromEntries` para Calibración y seguimiento de línea del dispositivo. | `web:src/pages/TrackFollowerSection.tsx:1329`; structural |
| <a id="fn-createsimulatedtrackirsnapshot"></a>`createSimulatedTrackIrSnapshot(seq: number, periodMs: number, objectsEnabled: boolean): IrSnapshot` | Coordina `IR_SENSOR_ORDER.map`, `LINE_CHANNELS.filter`, `LINE_CHANNELS.forEach` para Calibración y seguimiento de línea del dispositivo. | `web:src/pages/TrackFollowerSection.tsx:1350`; structural |
| <a id="fn-derivelineobservation"></a>`deriveLineObservation(snapshot: IrSnapshot): LineObservation` | Coordina `Number.isFinite`, `clamp`, `clamp01` para Calibración y seguimiento de línea del dispositivo. | `web:src/pages/TrackFollowerSection.tsx:712`; structural |
| <a id="fn-formatdecimal"></a>`formatDecimal(value: number, digits: number): inferred-by-TypeScript` | Coordina `Number.isFinite`, `value.toFixed` para Calibración y seguimiento de línea del dispositivo. | `web:src/pages/TrackFollowerSection.tsx:1399`; structural |
| <a id="fn-formathexbyte"></a>`formatHexByte(value: number): inferred-by-TypeScript` | Coordina `value.toString`, `value.toString(16).toUpperCase`, `value.toString(16).toUpperCase().padStart` para Calibración y seguimiento de línea del dispositivo. | `web:src/pages/TrackFollowerSection.tsx:1395`; structural |
| <a id="fn-linereason"></a>`lineReason(code: number): inferred-by-TypeScript` | Operación line Reason de Calibración y seguimiento de línea del dispositivo; semántica fina pendiente de ampliación. | `web:src/features/lineFollow/useLineDevice.ts:28`; inferred-name |
| <a id="fn-parsestate"></a>`parseState(value: unknown): LineDeviceState` | Coordina `[0, 1].includes`, `[s.calibrationState, s.target, s.phase, s.progress, s.calibrationReason, s.followReason, s.carMode, s.pattern, s.sessionId].every`, `[s.whiteValid, s.trackValid, s.balanceSettled, s.following, s.motionArmed, s.balanceActive, s.safetyStop, s.routeValid].every` para Calibración y seguimiento de línea del dispositivo. | `web:src/features/lineFollow/useLineDevice.ts:31`; structural |
| <a id="fn-projectobstacle"></a>`projectObstacle(snapshot: IrSnapshot, source: SampleSource, pose: TrackPose): ObstaclePoint \| null` | Coordina `Math.cos`, `Math.sin`, `THREE.MathUtils.degToRad` para Calibración y seguimiento de línea del dispositivo. | `web:src/pages/TrackFollowerSection.tsx:736`; structural |
| <a id="fn-trackcameraview"></a>`trackCameraView(pose: TrackPose, mode: CameraMode): inferred-by-TypeScript` | Coordina `Math.cos`, `Math.sin`, `new THREE.Vector3(pose.x, 0.08, pose.z).addScaledVector` para Calibración y seguimiento de línea del dispositivo. | `web:src/pages/TrackFollowerSection.tsx:880`; structural |
| <a id="fn-uselinedevice"></a>`useLineDevice(): inferred-by-TypeScript` | Coordina `calibrationBusy`, `useCallback`, `useEffect` para Calibración y seguimiento de línea del dispositivo. | `web:src/features/lineFollow/useLineDevice.ts:41`; structural |

## Funciones y callbacks internos

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-linefollowpanel.togglefollowing"></a>`LineFollowPanel.toggleFollowing(): inferred-by-TypeScript` | Coordina `device.follow`, `device.refresh`, `lineReason` para Calibración y seguimiento de línea del dispositivo. | `web:src/features/lineFollow/LineFollowPanel.tsx:23`; structural |
| <a id="fn-trackfollowersection.exportndjson"></a>`TrackFollowerSection.exportNdjson(): inferred-by-TypeScript` | Coordina `JSON.stringify`, `URL.createObjectURL`, `URL.revokeObjectURL` para Calibración y seguimiento de línea del dispositivo. | `web:src/pages/TrackFollowerSection.tsx:432`; structural |
| <a id="fn-trackfollowersection.handlesimulationtoggle"></a>`TrackFollowerSection.handleSimulationToggle(): inferred-by-TypeScript` | Coordina `createIdleTrackIrSnapshot`, `setLastIr`, `setObstacles` para Calibración y seguimiento de línea del dispositivo. | `web:src/pages/TrackFollowerSection.tsx:315`; structural |
| <a id="fn-trackfollowersection.integratetrackpoint"></a>`TrackFollowerSection.integrateTrackPoint(snapshot: IrSnapshot, observation: LineObservation, source: SampleSource): TrackPoint \| null` | Coordina `Date.now`, `Math.cos`, `Math.hypot` para Calibración y seguimiento de línea del dispositivo. | `web:src/pages/TrackFollowerSection.tsx:336`; structural |
| <a id="fn-trackfollowersection.resetsession"></a>`TrackFollowerSection.resetSession(): inferred-by-TypeScript` | Coordina `setLineLostCount`, `setObstacles`, `setPose` para Calibración y seguimiento de línea del dispositivo. | `web:src/pages/TrackFollowerSection.tsx:419`; structural |
| <a id="fn-trackfollowersection.sendirstream"></a>`TrackFollowerSection.sendIrStream(enable: boolean): inferred-by-TypeScript` | Coordina `normalizeIrStreamPeriodMs`, `setIrPeriodMs`, `setSimulationEnabled` para Calibración y seguimiento de línea del dispositivo. | `web:src/pages/TrackFollowerSection.tsx:399`; structural |
| <a id="fn-uselinedevice.balance"></a>`useLineDevice.balance(on: boolean): inferred-by-TypeScript` | Adapta la llamada a `commandBalance` con el contexto de esta integración. | `web:src/features/lineFollow/useLineDevice.ts:111`; structural |
| <a id="fn-uselinedevice.calibrate"></a>`useLineDevice.calibrate(operation: 0 \| 1 \| 2 \| 3): inferred-by-TypeScript` | Coordina `command`, `setError` para Calibración y seguimiento de línea del dispositivo. | `web:src/features/lineFollow/useLineDevice.ts:104`; structural |
| <a id="fn-uselinedevice.command"></a>`useLineDevice.command(name: string, args: Record<string, unknown>, label: string): inferred-by-TypeScript` | Coordina `Number`, `lineReason`, `parseState` para Calibración y seguimiento de línea del dispositivo. | `web:src/features/lineFollow/useLineDevice.ts:83`; structural |
| <a id="fn-uselinedevice.commandbalance"></a>`useLineDevice.commandBalance(on: boolean): inferred-by-TypeScript` | Coordina `refresh`, `request`, `setAction` para Calibración y seguimiento de línea del dispositivo. | `web:src/features/lineFollow/useLineDevice.ts:112`; structural |
| <a id="fn-uselinedevice.follow"></a>`useLineDevice.follow(on: boolean): inferred-by-TypeScript` | Adapta la llamada a `command` con el contexto de esta integración. | `web:src/features/lineFollow/useLineDevice.ts:110`; structural |
| <a id="fn-uselinedevice.poll"></a>`useLineDevice.poll(): inferred-by-TypeScript` | Coordina `refresh`, `setError`, `setState` para Calibración y seguimiento de línea del dispositivo. | `web:src/features/lineFollow/useLineDevice.ts:67`; structural |

## Estado y efectos React encontrados

| Hook | Función/closure | Evidencia |
| --- | --- | --- |
| `useLineDevice` | `LineDeviceCalibrationPanel` | `web:src/features/lineFollow/LineDeviceCalibration.tsx:54` |
| `useEffect` | `LineDeviceCalibrationPanel` | `web:src/features/lineFollow/LineDeviceCalibration.tsx:55` |
| `useLineDevice` | `LineFollowPanel` | `web:src/features/lineFollow/LineFollowPanel.tsx:13` |
| `useAutoMotion` | `LineFollowPanel` | `web:src/features/lineFollow/LineFollowPanel.tsx:14` |
| `useState` | `LineFollowPanel` | `web:src/features/lineFollow/LineFollowPanel.tsx:14` |
| `useState` | `LineFollowPanel` | `web:src/features/lineFollow/LineFollowPanel.tsx:15` |
| `useState` | `LineFollowPanel` | `web:src/features/lineFollow/LineFollowPanel.tsx:16` |
| `useState` | `LineFollowPanel` | `web:src/features/lineFollow/LineFollowPanel.tsx:17` |
| `useWebSocket` | `useLineDevice` | `web:src/features/lineFollow/useLineDevice.ts:42` |
| `useUser` | `useLineDevice` | `web:src/features/lineFollow/useLineDevice.ts:43` |
| `useState` | `useLineDevice` | `web:src/features/lineFollow/useLineDevice.ts:45` |
| `useState` | `useLineDevice` | `web:src/features/lineFollow/useLineDevice.ts:46` |
| `useState` | `useLineDevice` | `web:src/features/lineFollow/useLineDevice.ts:47` |
| `useState` | `useLineDevice` | `web:src/features/lineFollow/useLineDevice.ts:48` |
| `useRef` | `useLineDevice` | `web:src/features/lineFollow/useLineDevice.ts:49` |
| `useRef` | `useLineDevice` | `web:src/features/lineFollow/useLineDevice.ts:50` |
| `useRef` | `useLineDevice` | `web:src/features/lineFollow/useLineDevice.ts:51` |
| `useRef` | `useLineDevice` | `web:src/features/lineFollow/useLineDevice.ts:52` |
| `useRef` | `useLineDevice` | `web:src/features/lineFollow/useLineDevice.ts:53` |
| `useCallback` | `useLineDevice` | `web:src/features/lineFollow/useLineDevice.ts:55` |
| `useEffect` | `useLineDevice` | `web:src/features/lineFollow/useLineDevice.ts:63` |
| `useWebSocket` | `TrackFollowerSection` | `web:src/pages/TrackFollowerSection.tsx:124` |
| `useUser` | `TrackFollowerSection` | `web:src/pages/TrackFollowerSection.tsx:125` |
| `useIrEmitters` | `TrackFollowerSection` | `web:src/pages/TrackFollowerSection.tsx:126` |
| `useRef` | `TrackFollowerSection` | `web:src/pages/TrackFollowerSection.tsx:128` |
| `useRef` | `TrackFollowerSection` | `web:src/pages/TrackFollowerSection.tsx:129` |
| `useRef` | `TrackFollowerSection` | `web:src/pages/TrackFollowerSection.tsx:130` |
| `useRef` | `TrackFollowerSection` | `web:src/pages/TrackFollowerSection.tsx:131` |
| `useState` | `TrackFollowerSection` | `web:src/pages/TrackFollowerSection.tsx:133` |
| `useState` | `TrackFollowerSection` | `web:src/pages/TrackFollowerSection.tsx:134` |
| `useState` | `TrackFollowerSection` | `web:src/pages/TrackFollowerSection.tsx:135` |
| `useState` | `TrackFollowerSection` | `web:src/pages/TrackFollowerSection.tsx:136` |
| `useCallback` | `TrackFollowerSection` | `web:src/pages/TrackFollowerSection.tsx:137` |
| `useState` | `TrackFollowerSection` | `web:src/pages/TrackFollowerSection.tsx:138` |
| `useState` | `TrackFollowerSection` | `web:src/pages/TrackFollowerSection.tsx:139` |
| `useMemo` | `TrackFollowerSection` | `web:src/pages/TrackFollowerSection.tsx:140` |
| `useState` | `TrackFollowerSection` | `web:src/pages/TrackFollowerSection.tsx:141` |
| `useState` | `TrackFollowerSection` | `web:src/pages/TrackFollowerSection.tsx:142` |
| `useState` | `TrackFollowerSection` | `web:src/pages/TrackFollowerSection.tsx:143` |
| `useState` | `TrackFollowerSection` | `web:src/pages/TrackFollowerSection.tsx:144` |
| `useState` | `TrackFollowerSection` | `web:src/pages/TrackFollowerSection.tsx:145` |
| `useState` | `TrackFollowerSection` | `web:src/pages/TrackFollowerSection.tsx:146` |
| `useState` | `TrackFollowerSection` | `web:src/pages/TrackFollowerSection.tsx:147` |
| `useState` | `TrackFollowerSection` | `web:src/pages/TrackFollowerSection.tsx:148` |
| `useState` | `TrackFollowerSection` | `web:src/pages/TrackFollowerSection.tsx:149` |
| `useState` | `TrackFollowerSection` | `web:src/pages/TrackFollowerSection.tsx:150` |
| `useState` | `TrackFollowerSection` | `web:src/pages/TrackFollowerSection.tsx:153` |
| `useState` | `TrackFollowerSection` | `web:src/pages/TrackFollowerSection.tsx:162` |
| `useRef` | `TrackFollowerSection` | `web:src/pages/TrackFollowerSection.tsx:163` |
| `useRef` | `TrackFollowerSection` | `web:src/pages/TrackFollowerSection.tsx:164` |
| `useSensorSubscription` | `TrackFollowerSection` | `web:src/pages/TrackFollowerSection.tsx:165` |
| `useState` | `TrackFollowerSection` | `web:src/pages/TrackFollowerSection.tsx:170` |
| `useState` | `TrackFollowerSection` | `web:src/pages/TrackFollowerSection.tsx:171` |
| `useMemo` | `TrackFollowerSection` | `web:src/pages/TrackFollowerSection.tsx:173` |
| `useMemo` | `TrackFollowerSection` | `web:src/pages/TrackFollowerSection.tsx:174` |
| `useCallback` | `TrackFollowerSection` | `web:src/pages/TrackFollowerSection.tsx:189` |
| `useEffect` | `TrackFollowerSection` | `web:src/pages/TrackFollowerSection.tsx:250` |
| `useEffect` | `TrackFollowerSection` | `web:src/pages/TrackFollowerSection.tsx:266` |
| `useEffect` | `TrackFollowerSection` | `web:src/pages/TrackFollowerSection.tsx:283` |
| `useEffect` | `TrackFollowerSection` | `web:src/pages/TrackFollowerSection.tsx:289` |
| `useEffect` | `TrackFollowerSection` | `web:src/pages/TrackFollowerSection.tsx:302` |
| `useThree` | `TrackCameraRig` | `web:src/pages/TrackFollowerSection.tsx:836` |
| `useRef` | `TrackCameraRig` | `web:src/pages/TrackFollowerSection.tsx:837` |
| `useRef` | `TrackCameraRig` | `web:src/pages/TrackFollowerSection.tsx:838` |
| `useRef` | `TrackCameraRig` | `web:src/pages/TrackFollowerSection.tsx:839` |
| `useEffect` | `TrackCameraRig` | `web:src/pages/TrackFollowerSection.tsx:842` |
| `useFrame` | `TrackCameraRig` | `web:src/pages/TrackFollowerSection.tsx:861` |
| `useMemo` | `TrackGround` | `web:src/pages/TrackFollowerSection.tsx:896` |
| `useRef` | `TrackPointInstances` | `web:src/pages/TrackFollowerSection.tsx:916` |
| `useMemo` | `TrackPointInstances` | `web:src/pages/TrackFollowerSection.tsx:917` |
| `useMemo` | `TrackPointInstances` | `web:src/pages/TrackFollowerSection.tsx:918` |
| `useEffect` | `TrackPointInstances` | `web:src/pages/TrackFollowerSection.tsx:920` |
| `useMemo` | `TrackRibbon` | `web:src/pages/TrackFollowerSection.tsx:952` |
| `useLoader` | `TrackVehicleModel` | `web:src/pages/TrackFollowerSection.tsx:1015` |
| `useRef` | `TrackVehicleModel` | `web:src/pages/TrackFollowerSection.tsx:1019` |
| `useRef` | `TrackVehicleModel` | `web:src/pages/TrackFollowerSection.tsx:1020` |
| `useMemo` | `TrackVehicleModel` | `web:src/pages/TrackFollowerSection.tsx:1022` |
| `useEffect` | `TrackVehicleModel` | `web:src/pages/TrackFollowerSection.tsx:1024` |
| `useEffect` | `TrackVehicleModel` | `web:src/pages/TrackFollowerSection.tsx:1042` |
| `useEspWifiStatus` | `TrackStatusStrip` | `web:src/pages/TrackFollowerSection.tsx:1132` |
