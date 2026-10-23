# Telemetría y estado de sensores

ID: `web:module/src/contexts/IrEmittersContext.tsx` · Capa: **servicio** · Fotografía: 2026-10-20.

Documentación de fuente; sin tests, compilación, navegador ni validación física.

## Responsabilidad y alcance

Administra suscripciones y presenta orientación/IR con procedencia y habilitación de emisores.

Estado remoto y fanout de muestras; la adquisición y sincronización física pertenecen a F4.

<a id="modularity"></a>
## Modularidad observada

Separación de suscripción/estado y escena; depende de mensajes JSON del bridge.

<a id="state"></a>
## Handle, estado y atributos externos

Hooks/contextos retienen última muestra y estado de petición; la fecha de recepción no reemplaza acquired_ms/seq del productor.

<a id="lifecycle"></a>
## Ciclo de vida y contexto

- Suscribir al activar consumidores y retirar/limpiar al desmontar.
- Aplicar estado de emisores desde GET/evento confirmado y no desde la intención del click.

<a id="interactions"></a>
## Interacciones

- StreamBroker ESP combina consumidores y F4 emite por endpoint.
- El visor OLED necesita una adaptación expresa para usar estas muestras; no las recibe por identidad de pantalla.

<a id="limitations"></a>
## Precauciones y límites

- Un stream conectado no acredita que la pantalla física use exactamente esa muestra.
- Señalar simulación y ausencia/vencimiento de dato.

## Hallazgos conectados


## Archivos y evidencia

| Archivo | Análisis | Hash SHA-256 |
| --- | --- | --- |
| `src/components/IrEmittersControls.tsx` | typescript-ast-static | `c3020c351d16217f541bd67786cfb097e95de9848693e7d06bfb45d70b3471b8` |
| `src/components/RealtimeEulerPanel.tsx` | typescript-ast-static | `8283dc54adc185d7e87cbe3fe9eaaec31bf153d924e57e36f52adca8731d34ef` |
| `src/components/SimulatedBadge.tsx` | typescript-ast-static | `c47d7a38665e522d838f1a88d367b7d7b903c3dd84efe0cf9ef2b8141ef7756b` |
| `src/contexts/IrEmittersContext.tsx` | typescript-ast-static | `ad310ef2a30661a2d4470bafe21fde073d63896204443874686f5c616c2c834b` |
| `src/hooks/useSensorSubscription.ts` | typescript-ast-static | `dbb52fa77371e6330e6d5685b71b78597ac72d415570d502f653ca7dd99ef041` |
| `src/pages/EstadoSection.tsx` | typescript-ast-static | `3e08619ffb3ad6a9f4725f154411e1221d763a01780d23097491e449cdddc8db` |

## Tipos, puertos, props y callbacks

En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.

### `Euler`

`web:src/components/RealtimeEulerPanel.tsx:5`

| Atributo / firma | Propósito |
| --- | --- |
| `yaw: number` | Dato de estado/configuración `yaw`; declaración estructural, interpretación específica pendiente de ampliar. |
| `pitch: number` | Dato de estado/configuración `pitch`; declaración estructural, interpretación específica pendiente de ampliar. |
| `roll: number` | Dato de estado/configuración `roll`; declaración estructural, interpretación específica pendiente de ampliar. |

### `Series`

`web:src/components/RealtimeEulerPanel.tsx:6`

Tipo: `Record<keyof Euler, number[]>`.

### `RealtimeEulerPanelProps`

`web:src/components/RealtimeEulerPanel.tsx:8`

| Atributo / firma | Propósito |
| --- | --- |
| `eulerDeg: Euler` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `connected: boolean` | Dato de estado/configuración `connected`; declaración estructural, interpretación específica pendiente de ampliar. |
| `simulated?: boolean` | Dato de estado/configuración `simulated`; declaración estructural, interpretación específica pendiente de ampliar. |
| `sampling: boolean` | Dato de estado/configuración `sampling`; declaración estructural, interpretación específica pendiente de ampliar. |
| `sampleVersion: number` | Dato de estado/configuración `sampleVersion`; declaración estructural, interpretación específica pendiente de ampliar. |
| `maxPoints?: number` | Dato de estado/configuración `maxPoints`; declaración estructural, interpretación específica pendiente de ampliar. |
| `sampleMs?: number` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `heightPx?: number` | Dato de estado/configuración `heightPx`; declaración estructural, interpretación específica pendiente de ampliar. |
| `sensorIntervalTime: number` | Dato de estado/configuración `sensorIntervalTime`; declaración estructural, interpretación específica pendiente de ampliar. |

### `IrEmittersState`

`web:src/contexts/IrEmittersContext.tsx:6`

| Atributo / firma | Propósito |
| --- | --- |
| `objects: boolean` | Dato de estado/configuración `objects`; declaración estructural, interpretación específica pendiente de ampliar. |
| `line: boolean` | Dato de estado/configuración `line`; declaración estructural, interpretación específica pendiente de ampliar. |
| `lights: boolean` | Dato de estado/configuración `lights`; declaración estructural, interpretación específica pendiente de ampliar. |

### `ContextValue`

`web:src/contexts/IrEmittersContext.tsx:7`

| Atributo / firma | Propósito |
| --- | --- |
| `state: IrEmittersState \| null` | Dato de estado/configuración `state`; declaración estructural, interpretación específica pendiente de ampliar. |
| `available: boolean` | Dato de estado/configuración `available`; declaración estructural, interpretación específica pendiente de ampliar. |
| `pending: boolean` | Dato de estado/configuración `pending`; declaración estructural, interpretación específica pendiente de ampliar. |
| `error: string \| null` | Dato de estado/configuración `error`; declaración estructural, interpretación específica pendiente de ampliar. |
| `refresh: () => Promise<void>` | Operación refresh de Telemetría y estado de sensores; semántica fina pendiente de ampliación. |
| `setEnabled: (key: keyof IrEmittersState, enabled: boolean) => Promise<void>` | Operación set Enabled de Telemetría y estado de sensores; semántica fina pendiente de ampliación. |

### `SensorName`

`web:src/hooks/useSensorSubscription.ts:6`

Tipo: `"mpu" \| "ir"`.

### `SubscriptionState`

`web:src/hooks/useSensorSubscription.ts:7`

Tipo: `"idle" \| "subscribing" \| "active" \| "error"`.

### `Consumer`

`web:src/hooks/useSensorSubscription.ts:9`

| Atributo / firma | Propósito |
| --- | --- |
| `periodMs: number` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |

### `SensorRegistry`

`web:src/hooks/useSensorSubscription.ts:10`

| Atributo / firma | Propósito |
| --- | --- |
| `consumers: Map<symbol, Consumer>` | Dato de estado/configuración `consumers`; declaración estructural, interpretación específica pendiente de ampliar. |
| `appliedPeriodMs: number \| null \| undefined` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `operation: Promise<unknown> \| null` | Dato de estado/configuración `operation`; declaración estructural, interpretación específica pendiente de ampliar. |

### `RequestCommand`

`web:src/hooks/useSensorSubscription.ts:19`

Tipo: `<T>( command: string, args?: Record<string, unknown>, options?: { timeoutMs?: number }, ) => Promise<T>`.

### `StreamStatus`

`web:src/pages/EstadoSection.tsx:68`

| Atributo / firma | Propósito |
| --- | --- |
| `active: boolean` | Dato de estado/configuración `active`; declaración estructural, interpretación específica pendiente de ampliar. |
| `pending: boolean` | Dato de estado/configuración `pending`; declaración estructural, interpretación específica pendiente de ampliar. |
| `packetsReceived: number` | Dato de estado/configuración `packetsReceived`; declaración estructural, interpretación específica pendiente de ampliar. |
| `lastSeq: number \| null` | Dato de estado/configuración `lastSeq`; declaración estructural, interpretación específica pendiente de ampliar. |
| `lastStatus: number \| null` | Dato de estado/configuración `lastStatus`; declaración estructural, interpretación específica pendiente de ampliar. |
| `lastFlags: number \| null` | Dato de estado/configuración `lastFlags`; declaración estructural, interpretación específica pendiente de ampliar. |
| `ackCode: number \| null` | Dato de estado/configuración `ackCode`; declaración estructural, interpretación específica pendiente de ampliar. |
| `ackActive: boolean \| null` | Dato de estado/configuración `ackActive`; declaración estructural, interpretación específica pendiente de ampliar. |
| `ackPeriodMs: number \| null` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `statusText: string` | Dato de estado/configuración `statusText`; declaración estructural, interpretación específica pendiente de ampliar. |
| `lastFrameHex: string` | Dato de estado/configuración `lastFrameHex`; declaración estructural, interpretación específica pendiente de ampliar. |
| `updatedAt: number \| null` | Dato de estado/configuración `updatedAt`; declaración estructural, interpretación específica pendiente de ampliar. |

### `SensorIconName`

`web:src/pages/EstadoSection.tsx:1643`

Tipo: `\| "cube" \| "target" \| "frame" \| "focus" \| "snow" \| "grid" \| "orbit" \| "origin" \| "trash" \| "signal" \| "link"`.


## Funciones públicas / componentes / hooks

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-consolecard"></a>`ConsoleCard({   title,   status,   active = false,   layout = "default",   children, }: { title: string; status?: string; active?: boolean; layout?: "default" \| "toggle"; children: ReactNode; }): inferred-by-TypeScript` | Operación Console Card de Telemetría y estado de sensores; semántica fina pendiente de ampliación. | `web:src/pages/EstadoSection.tsx:1563`; inferred-name |
| <a id="fn-estadosection"></a>`EstadoSection(): inferred-by-TypeScript` | Coordina `Math.max`, `new URLSearchParams(window.location.search).get`, `periodToHz` para Telemetría y estado de sensores. | `web:src/pages/EstadoSection.tsx:115`; structural |
| <a id="fn-headerpill"></a>`HeaderPill({   label,   value,   tone, }: { label: string; value: string; tone: "ok" \| "error" \| "info" \| "muted"; }): inferred-by-TypeScript` | Operación Header Pill de Telemetría y estado de sensores; semántica fina pendiente de ampliación. | `web:src/pages/EstadoSection.tsx:931`; inferred-name |
| <a id="fn-iremitterscontrols"></a>`IrEmittersControls({ icon }: { icon?: ReactNode; }): inferred-by-TypeScript` | Coordina `useId`, `useIrEmitters` para Telemetría y estado de sensores. | `web:src/components/IrEmittersControls.tsx:6`; structural |
| <a id="fn-iremittersprovider"></a>`IrEmittersProvider({ children }: { children: ReactNode }): inferred-by-TypeScript` | Coordina `useCallback`, `useEffect`, `useRef` para Telemetría y estado de sensores. | `web:src/contexts/IrEmittersContext.tsx:14`; structural |
| <a id="fn-legend"></a>`Legend({ label, dot }: { label: string; dot: string }): inferred-by-TypeScript` | Operación Legend de Telemetría y estado de sensores; semántica fina pendiente de ampliación. | `web:src/components/RealtimeEulerPanel.tsx:116`; inferred-name |
| <a id="fn-mpusimulationworkbench"></a>`MpuSimulationWorkbench({   eulerDeg,   autoMotion,   autoAxes,   periodMs,   onEulerChange,   onAutoEulerChange,   onAutoAxisChange,   onPeriodChange, }: { eulerDeg: EulerDeg; autoMotion: boolean; autoAxes: Record<keyof EulerDeg, boolean>; periodMs: number; onEulerChange: (value: EulerDeg) => void; onAutoEulerChange: (value: EulerDeg) => void; onAutoAxisChange: (axis: keyof EulerDeg, enabled: boolean) => void; onPeriodChange: (value: number) => void; }): inferred-by-TypeScript` | Coordina `Object.values`, `Object.values(autoAxes).some` para Telemetría y estado de sensores. | `web:src/pages/EstadoSection.tsx:1514`; structural |
| <a id="fn-numbercontrol"></a>`NumberControl({   value,   min,   max,   suffix,   onChange, }: { value: number; min: number; max: number; suffix: string; onChange: (value: number) => void; }): inferred-by-TypeScript` | Operación Number Control de Telemetría y estado de sensores; semántica fina pendiente de ampliación. | `web:src/pages/EstadoSection.tsx:1615`; inferred-name |
| <a id="fn-panelblock"></a>`PanelBlock({   title,   icon,   children,   className = "", }: { title: string; icon: SensorIconName; children: ReactNode; className?: string; }): inferred-by-TypeScript` | Operación Panel Block de Telemetría y estado de sensores; semántica fina pendiente de ampliación. | `web:src/pages/EstadoSection.tsx:1411`; inferred-name |
| <a id="fn-realtimeeulerpanel"></a>`RealtimeEulerPanel({   eulerDeg, connected, simulated = false, sampling, sampleVersion,   maxPoints = 80, sampleMs = 250, heightPx = 130, sensorIntervalTime, }: RealtimeEulerPanelProps): inferred-by-TypeScript` | Coordina `Array.from`, `useEffect`, `useMemo` para Telemetría y estado de sensores. | `web:src/components/RealtimeEulerPanel.tsx:20`; structural |
| <a id="fn-row"></a>`Row({ label, value, hasSamples, colorDot }: { label: string; value: number; hasSamples: boolean; colorDot: string; }): inferred-by-TypeScript` | Coordina `Number.isFinite`, `value.toFixed` para Telemetría y estado de sensores. | `web:src/components/RealtimeEulerPanel.tsx:106`; structural |
| <a id="fn-sensorbottomconsole"></a>`SensorBottomConsole({   connected, mpuStatus, irStatus, mpuPeriodMs, irPeriodMs,   irEmu, mpuEmu, mpuRequested, irRequested,   onMpuPeriodChange, onIrPeriodChange, onMpuToggle, onIrToggle, onZeroMpu, }: { connected: boolean; mpuStatus: StreamStatus; irStatus: StreamStatus; mpuPeriodMs: number; irPeriodMs: number; irEmu: boolean; mpuEmu: boolean; mpuRequested: boolean; irRequested: boolean; onMpuPeriodChange: (value: number) => void; onIrPeriodChange: (value: number) => void; onMpuToggle: () => void; onIrToggle: () => void; onZeroMpu: () => void; }): inferred-by-TypeScript` | Coordina `periodToHz`, `useIrEmitters` para Telemetría y estado de sensores. | `web:src/pages/EstadoSection.tsx:1448`; structural |
| <a id="fn-sensorcameraviews"></a>`SensorCameraViews({ selected, onPreset, simulated, simulationDescription }: { selected: SensorCameraPreset \| null; onPreset: (preset: SensorCameraPreset) => void; simulated: boolean; simulationDescription: string; }): inferred-by-TypeScript` | Coordina `SENSOR_CAMERA_VIEW_GROUPS.find`, `isometricViews.map`, `renderButtons` para Telemetría y estado de sensores. | `web:src/pages/EstadoSection.tsx:1015`; structural |
| <a id="fn-sensorcanvastelemetry"></a>`SensorCanvasTelemetry({   irSnapshot,   objectDetections, }: { irSnapshot: IrSnapshot; objectDetections: IrObjectAxialDetections; }): inferred-by-TypeScript` | Operación Sensor Canvas Telemetry de Telemetría y estado de sensores; semántica fina pendiente de ampliación. | `web:src/pages/EstadoSection.tsx:1266`; inferred-name |
| <a id="fn-sensorconnectionbar"></a>`SensorConnectionBar({   connected,   mpuStatus,   irStatus,   renderLink, }: { connected: boolean; mpuStatus: StreamStatus; irStatus: StreamStatus; renderLink: boolean; }): inferred-by-TypeScript` | Adapta la llamada a `useIrEmitters` con el contexto de esta integración. | `web:src/pages/EstadoSection.tsx:948`; structural |
| <a id="fn-sensorheaderstatus"></a>`SensorHeaderStatus({   connected,   mpuHz,   irHz,   mpuPeriodMs,   irPeriodMs, }: { connected: boolean; mpuHz: number; irHz: number; mpuPeriodMs: number; irPeriodMs: number; }): inferred-by-TypeScript` | Coordina `getEspConnectionDetail`, `getEspConnectionLabel`, `useEspWifiStatus` para Telemetría y estado de sensores. | `web:src/pages/EstadoSection.tsx:897`; structural |
| <a id="fn-sensoricon"></a>`SensorIcon({ name }: { name: SensorIconName }): inferred-by-TypeScript` | Operación Sensor Icon de Telemetría y estado de sensores; semántica fina pendiente de ampliación. | `web:src/pages/EstadoSection.tsx:1656`; inferred-name |
| <a id="fn-sensorsensitivitycontrol"></a>`SensorSensitivityControl({   valueCm,   onChange,   onClear,   onOpenCalibration, }: { valueCm: number; onChange: (value: number) => void; onClear: () => void; onOpenCalibration: () => void; }): inferred-by-TypeScript` | Adapta la llamada a `formatDecimal` con el contexto de esta integración. | `web:src/pages/EstadoSection.tsx:1292`; structural |
| <a id="fn-sensorstatuspanel"></a>`SensorStatusPanel({   connected,   systemMode,   mpuEulerRaw,   renderEuler,   mpuMagValid,   mpuStatus,   irStatus,   irSnapshot,   irObjectEstimate,   lastMpuSampleDtUs, }: { connected: boolean; systemMode: string; mpuEulerRaw: EulerDeg; renderEuler: EulerDeg; mpuMagValid: boolean \| null; mpuStatus: StreamStatus; irStatus: StreamStatus; irSnapshot: IrSnapshot; irObjectEstimate: IrObjectPositionEstimate; lastMpuSampleDtUs: number \| null; }): inferred-by-TypeScript` | Coordina `Math.round`, `formatDecimal`, `formatHexByte` para Telemetría y estado de sensores. | `web:src/pages/EstadoSection.tsx:1155`; structural |
| <a id="fn-sensortoolbox"></a>`SensorToolbox({   showGrid,   showObjectAxes,   showLineAxes,   showOrigin,   renderLink,   onResetCamera,   onToggleGrid,   onToggleObjectAxes,   onToggleLineAxes,   onToggleOrigin,   onToggleRenderLink, }: { showGrid: boolean; showObjectAxes: boolean; showLineAxes: boolean; showOrigin: boolean; renderLink: boolean; onResetCamera: () => void; onToggleGrid: () => void; onToggleObjectAxes: () => void; onToggleLineAxes: () => void; onToggleOrigin: () => void; onToggleRenderLink: () => void; }): inferred-by-TypeScript` | Operación Sensor Toolbox de Telemetría y estado de sensores; semántica fina pendiente de ampliación. | `web:src/pages/EstadoSection.tsx:1054`; inferred-name |
| <a id="fn-simulatedbadge"></a>`SimulatedBadge({ description }: { description: string }): inferred-by-TypeScript` | Operación Simulated Badge de Telemetría y estado de sensores; semántica fina pendiente de ampliación. | `web:src/components/SimulatedBadge.tsx:1`; inferred-name |
| <a id="fn-statusrow"></a>`StatusRow({   label,   value,   tone = "muted", }: { label: string; value: string; tone?: "ok" \| "error" \| "info" \| "muted"; }): inferred-by-TypeScript` | Operación Status Row de Telemetría y estado de sensores; semántica fina pendiente de ampliación. | `web:src/pages/EstadoSection.tsx:1429`; inferred-name |
| <a id="fn-switchbutton"></a>`SwitchButton({   active,   disabled = false,   onClick,   ariaLabel, }: { active: boolean; disabled?: boolean; onClick: () => void; ariaLabel?: string; }): inferred-by-TypeScript` | Operación Switch Button de Telemetría y estado de sensores; semántica fina pendiente de ampliación. | `web:src/pages/EstadoSection.tsx:1587`; inferred-name |
| <a id="fn-toolbutton"></a>`ToolButton({   label,   icon,   active,   danger = false,   title,   onClick, }: { label: string; icon: SensorIconName; active?: boolean; danger?: boolean; title?: string; onClick: () => void; }): inferred-by-TypeScript` | Operación Tool Button de Telemetría y estado de sensores; semántica fina pendiente de ampliación. | `web:src/pages/EstadoSection.tsx:1106`; inferred-name |
| <a id="fn-verticalsensorbar"></a>`VerticalSensorBar({   label,   shortLabel,   value,   tone,   detection, }: { label: string; shortLabel: string; value: number; tone: "cyan" \| "rose"; detection?: IrObjectAxialDetection; }): inferred-by-TypeScript` | Coordina `Math.max`, `Math.min`, `Math.round` para Telemetría y estado de sensores. | `web:src/pages/EstadoSection.tsx:1376`; structural |
| <a id="fn-verticalsensorgroup"></a>`VerticalSensorGroup({   title,   tone,   channels,   values,   detections, }: { title: string; tone: "cyan" \| "rose"; channels: Array<{ key: IrSensorKey; label: string; shortLabel: string; }>; values: Record<IrSensorKey, number>; detections?: Partial<Record<IrSensorKey, IrObjectAxialDetection>>; }): inferred-by-TypeScript` | Adapta la llamada a `channels.map` con el contexto de esta integración. | `web:src/pages/EstadoSection.tsx:1337`; structural |
| <a id="fn-assertstreamcommandaccepted"></a>`assertStreamCommandAccepted(sensor: SensorName, desiredPeriodMs: number \| null, data: Record<string, unknown>): void` | Adapta la llamada a `sensor.toUpperCase` con el contexto de esta integración. | `web:src/hooks/useSensorSubscription.ts:112`; structural |
| <a id="fn-clamp01"></a>`clamp01(value: number): inferred-by-TypeScript` | Coordina `Math.max`, `Math.min`, `Number.isFinite` para Telemetría y estado de sensores. | `web:src/pages/EstadoSection.tsx:1900`; structural |
| <a id="fn-createemptysensorrecord"></a>`createEmptySensorRecord(): Record<IrSensorKey, number>` | Adapta la llamada a `IR_SENSOR_ORDER.reduce` con el contexto de esta integración. | `web:src/pages/EstadoSection.tsx:1800`; structural |
| <a id="fn-createemulatedirsnapshot"></a>`createEmulatedIrSnapshot(objectReadings: Readonly<Record<IrObjectSensorKey, number>>, periodMs: number, sampleSeq: number): IrSnapshot` | Coordina `IR_OBJECT_SENSOR_KEYS.forEach`, `Math.round`, `createEmptySensorRecord` para Telemetría y estado de sensores. | `web:src/pages/EstadoSection.tsx:1822`; structural |
| <a id="fn-createidleirsnapshot"></a>`createIdleIrSnapshot(periodMs: number): IrSnapshot` | Adapta la llamada a `createEmptySensorRecord` con el contexto de esta integración. | `web:src/pages/EstadoSection.tsx:1781`; structural |
| <a id="fn-createstreamstatus"></a>`createStreamStatus(statusText: string): StreamStatus` | Operación create Stream Status de Telemetría y estado de sensores; semántica fina pendiente de ampliación. | `web:src/pages/EstadoSection.tsx:1754`; inferred-name |
| <a id="fn-decode"></a>`decode(value: unknown): IrEmittersState` | Operación decode de Telemetría y estado de sensores; semántica fina pendiente de ampliación. | `web:src/contexts/IrEmittersContext.tsx:9`; inferred-name |
| <a id="fn-formatdecimal"></a>`formatDecimal(value: number, digits: number): inferred-by-TypeScript` | Coordina `Number.isFinite`, `value.toFixed` para Telemetría y estado de sensores. | `web:src/pages/EstadoSection.tsx:1884`; structural |
| <a id="fn-formathexbyte"></a>`formatHexByte(value: number): inferred-by-TypeScript` | Coordina `value.toString`, `value.toString(16).toUpperCase`, `value.toString(16).toUpperCase().padStart` para Telemetría y estado de sensores. | `web:src/pages/EstadoSection.tsx:1872`; structural |
| <a id="fn-formatmpulatency"></a>`formatMpuLatency(sampleDtUs: number \| null): inferred-by-TypeScript` | Adapta la llamada a `formatDecimal` con el contexto de esta integración. | `web:src/pages/EstadoSection.tsx:1892`; structural |
| <a id="fn-formatobjectmask"></a>`formatObjectMask(value: number): inferred-by-TypeScript` | Coordina `(value & 0x1f).toString`, `(value & 0x1f).toString(2).padStart` para Telemetría y estado de sensores. | `web:src/pages/EstadoSection.tsx:1876`; structural |
| <a id="fn-formatsigned"></a>`formatSigned(value: number): inferred-by-TypeScript` | Adapta la llamada a `formatDecimal` con el contexto de esta integración. | `web:src/pages/EstadoSection.tsx:1880`; structural |
| <a id="fn-formatstreamsubscriptionerror"></a>`formatStreamSubscriptionError(sensor: "MPU" \| "IR", error: Error): string` | Operación format Stream Subscription Error de Telemetría y estado de sensores; semántica fina pendiente de ampliación. | `web:src/pages/EstadoSection.tsx:1771`; inferred-name |
| <a id="fn-normalizeirstreamperiodms"></a>`normalizeIrStreamPeriodMs(value: number): inferred-by-TypeScript` | Adapta la llamada a `normalizeStreamPeriodMs` con el contexto de esta integración. | `web:src/pages/EstadoSection.tsx:1860`; structural |
| <a id="fn-normalizempuperiodms"></a>`normalizeMpuPeriodMs(value: number): inferred-by-TypeScript` | Adapta la llamada a `normalizeStreamPeriodMs` con el contexto de esta integración. | `web:src/pages/EstadoSection.tsx:1856`; structural |
| <a id="fn-periodtohz"></a>`periodToHz(periodMs: number): inferred-by-TypeScript` | Coordina `Math.round`, `Number.isFinite` para Telemetría y estado de sensores. | `web:src/pages/EstadoSection.tsx:1864`; structural |
| <a id="fn-pickobjectsensorreadings"></a>`pickObjectSensorReadings(values: Readonly<Record<IrSensorKey, number>>): Record<IrObjectSensorKey, number>` | Adapta la llamada a `IR_OBJECT_SENSOR_KEYS.reduce` con el contexto de esta integración. | `web:src/pages/EstadoSection.tsx:1810`; structural |
| <a id="fn-pushclamped"></a>`pushClamped(values: number[], value: number, max: number): inferred-by-TypeScript` | Coordina `Math.max`, `values.slice` para Telemetría y estado de sensores. | `web:src/components/RealtimeEulerPanel.tsx:122`; structural |
| <a id="fn-reconcile"></a>`reconcile(sensor: SensorName, request: RequestCommand): Promise<void>` | Coordina `Math.min`, `[...registry.consumers.values()].map`, `registry.consumers.values` para Telemetría y estado de sensores. | `web:src/hooks/useSensorSubscription.ts:86`; structural |
| <a id="fn-useiremitters"></a>`useIrEmitters(): inferred-by-TypeScript` | Adapta la llamada a `useContext` con el contexto de esta integración. | `web:src/contexts/IrEmittersContext.tsx:56`; structural |
| <a id="fn-usesensorsubscription"></a>`useSensorSubscription(sensor: SensorName, enabled: boolean, requestedPeriodMs: number): inferred-by-TypeScript` | Coordina `normalizeStreamPeriodMs`, `useEffect`, `useMemo` para Telemetría y estado de sensores. | `web:src/hooks/useSensorSubscription.ts:30`; structural |

## Funciones y callbacks internos

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-estadosection.handlecamerapreset"></a>`EstadoSection.handleCameraPreset(preset: SensorCameraPreset): inferred-by-TypeScript` | Coordina `sceneRef.current?.setCameraPreset`, `setCameraPreset` para Telemetría y estado de sensores. | `web:src/pages/EstadoSection.tsx:534`; structural |
| <a id="fn-estadosection.handleclearir"></a>`EstadoSection.handleClearIr(): inferred-by-TypeScript` | Coordina `createIdleIrSnapshot`, `sceneRef.current?.clearIrTarget`, `setHasIrSceneSample` para Telemetría y estado de sensores. | `web:src/pages/EstadoSection.tsx:521`; structural |
| <a id="fn-estadosection.handlerenderlinktoggle"></a>`EstadoSection.handleRenderLinkToggle(): inferred-by-TypeScript` | Adapta la llamada a `setRenderLink` con el contexto de esta integración. | `web:src/pages/EstadoSection.tsx:452`; structural |
| <a id="fn-estadosection.handlesimulationtoggle"></a>`EstadoSection.handleSimulationToggle(): inferred-by-TypeScript` | Coordina `setIrSimulationEnabled`, `setMpuSimulationEnabled` para Telemetría y estado de sensores. | `web:src/pages/EstadoSection.tsx:510`; structural |
| <a id="fn-estadosection.handlezerompu"></a>`EstadoSection.handleZeroMpu(): inferred-by-TypeScript` | Coordina `setRenderEuler`, `setZeroOffset` para Telemetría y estado de sensores. | `web:src/pages/EstadoSection.tsx:516`; structural |
| <a id="fn-estadosection.setirsimulationenabled"></a>`EstadoSection.setIrSimulationEnabled(next: boolean): inferred-by-TypeScript` | Coordina `createIdleIrSnapshot`, `sceneRef.current?.clearIrTarget`, `setHasIrSceneSample` para Telemetría y estado de sensores. | `web:src/pages/EstadoSection.tsx:462`; structural |
| <a id="fn-estadosection.setmpusimulationenabled"></a>`EstadoSection.setMpuSimulationEnabled(next: boolean): inferred-by-TypeScript` | Coordina `setHasRealMpuSample`, `setMpuAutoMotion`, `setMpuEmu` para Telemetría y estado de sensores. | `web:src/pages/EstadoSection.tsx:491`; structural |
| <a id="fn-realtimeeulerpanel.capture"></a>`RealtimeEulerPanel.capture(): inferred-by-TypeScript` | Coordina `Object.values`, `Object.values(latest.eulerDeg).every`, `setSeries` para Telemetría y estado de sensores. | `web:src/components/RealtimeEulerPanel.tsx:40`; structural |
| <a id="fn-realtimeeulerpanel.topath"></a>`RealtimeEulerPanel.toPath(values: number[]): inferred-by-TypeScript` | Coordina `values.map`, `values.map((value, index) => `${(maxPoints - values.length + index) * stepX},${toY(value)}`).join` para Telemetría y estado de sensores. | `web:src/components/RealtimeEulerPanel.tsx:61`; structural |
| <a id="fn-realtimeeulerpanel.toy"></a>`RealtimeEulerPanel.toY(value: number): inferred-by-TypeScript` | Coordina `Math.max`, `Math.min` para Telemetría y estado de sensores. | `web:src/components/RealtimeEulerPanel.tsx:59`; structural |
| <a id="fn-sensorcameraviews.renderbuttons"></a>`SensorCameraViews.renderButtons(group: (typeof SENSOR_CAMERA_VIEW_GROUPS)[number]): inferred-by-TypeScript` | Adapta la llamada a `group.views.map` con el contexto de esta integración. | `web:src/pages/EstadoSection.tsx:1024`; structural |

## Estado y efectos React encontrados

| Hook | Función/closure | Evidencia |
| --- | --- | --- |
| `useIrEmitters` | `IrEmittersControls` | `web:src/components/IrEmittersControls.tsx:9` |
| `useId` | `IrEmittersControls` | `web:src/components/IrEmittersControls.tsx:10` |
| `useState` | `RealtimeEulerPanel` | `web:src/components/RealtimeEulerPanel.tsx:24` |
| `useRef` | `RealtimeEulerPanel` | `web:src/components/RealtimeEulerPanel.tsx:25` |
| `useRef` | `RealtimeEulerPanel` | `web:src/components/RealtimeEulerPanel.tsx:26` |
| `useEffect` | `RealtimeEulerPanel` | `web:src/components/RealtimeEulerPanel.tsx:29` |
| `useEffect` | `RealtimeEulerPanel` | `web:src/components/RealtimeEulerPanel.tsx:33` |
| `useEffect` | `RealtimeEulerPanel` | `web:src/components/RealtimeEulerPanel.tsx:38` |
| `useMemo` | `RealtimeEulerPanel` | `web:src/components/RealtimeEulerPanel.tsx:56` |
| `useWebSocket` | `IrEmittersProvider` | `web:src/contexts/IrEmittersContext.tsx:15` |
| `useUser` | `IrEmittersProvider` | `web:src/contexts/IrEmittersContext.tsx:16` |
| `useState` | `IrEmittersProvider` | `web:src/contexts/IrEmittersContext.tsx:18` |
| `useState` | `IrEmittersProvider` | `web:src/contexts/IrEmittersContext.tsx:19` |
| `useState` | `IrEmittersProvider` | `web:src/contexts/IrEmittersContext.tsx:20` |
| `useRef` | `IrEmittersProvider` | `web:src/contexts/IrEmittersContext.tsx:21` |
| `useRef` | `IrEmittersProvider` | `web:src/contexts/IrEmittersContext.tsx:22` |
| `useRef` | `IrEmittersProvider` | `web:src/contexts/IrEmittersContext.tsx:23` |
| `useCallback` | `IrEmittersProvider` | `web:src/contexts/IrEmittersContext.tsx:24` |
| `useCallback` | `IrEmittersProvider` | `web:src/contexts/IrEmittersContext.tsx:25` |
| `useCallback` | `IrEmittersProvider` | `web:src/contexts/IrEmittersContext.tsx:40` |
| `useCallback` | `IrEmittersProvider` | `web:src/contexts/IrEmittersContext.tsx:41` |
| `useEffect` | `IrEmittersProvider` | `web:src/contexts/IrEmittersContext.tsx:44` |
| `useEffect` | `IrEmittersProvider` | `web:src/contexts/IrEmittersContext.tsx:50` |
| `useContext` | `useIrEmitters` | `web:src/contexts/IrEmittersContext.tsx:57` |
| `useWebSocket` | `useSensorSubscription` | `web:src/hooks/useSensorSubscription.ts:31` |
| `useMemo` | `useSensorSubscription` | `web:src/hooks/useSensorSubscription.ts:32` |
| `useState` | `useSensorSubscription` | `web:src/hooks/useSensorSubscription.ts:33` |
| `useState` | `useSensorSubscription` | `web:src/hooks/useSensorSubscription.ts:34` |
| `useRef` | `useSensorSubscription` | `web:src/hooks/useSensorSubscription.ts:36` |
| `useEffect` | `useSensorSubscription` | `web:src/hooks/useSensorSubscription.ts:39` |
| `useEffect` | `useSensorSubscription` | `web:src/hooks/useSensorSubscription.ts:66` |
| `useWebSocket` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:121` |
| `useUser` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:122` |
| `useRef` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:126` |
| `useRef` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:127` |
| `useState` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:129` |
| `useState` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:130` |
| `useState` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:131` |
| `useState` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:133` |
| `useState` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:134` |
| `useState` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:135` |
| `useState` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:136` |
| `useState` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:141` |
| `useState` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:146` |
| `useState` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:151` |
| `useState` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:152` |
| `useState` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:153` |
| `useState` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:154` |
| `useState` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:155` |
| `useState` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:156` |
| `useState` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:157` |
| `useState` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:158` |
| `useState` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:161` |
| `useState` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:162` |
| `useState` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:163` |
| `useState` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:164` |
| `useState` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:165` |
| `useState` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:166` |
| `useRef` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:167` |
| `useState` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:168` |
| `useState` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:169` |
| `useState` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:174` |
| `useState` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:177` |
| `useState` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:180` |
| `useState` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:183` |
| `useState` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:186` |
| `useState` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:187` |
| `useState` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:188` |
| `useState` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:189` |
| `useRef` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:190` |
| `useRef` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:191` |
| `useRef` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:192` |
| `useRef` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:193` |
| `useSensorSubscription` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:194` |
| `useSensorSubscription` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:199` |
| `useCallback` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:210` |
| `useMemo` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:222` |
| `useMemo` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:235` |
| `useMemo` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:239` |
| `useMemo` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:248` |
| `useMemo` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:252` |
| `useEffect` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:283` |
| `useEffect` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:289` |
| `useEffect` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:293` |
| `useEffect` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:297` |
| `useEffect` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:301` |
| `useEffect` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:345` |
| `useEffect` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:363` |
| `useEffect` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:381` |
| `useCallback` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:398` |
| `useCallback` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:417` |
| `useCallback` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:539` |
| `useCallback` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:541` |
| `useCallback` | `EstadoSection` | `web:src/pages/EstadoSection.tsx:578` |
| `useEspWifiStatus` | `SensorHeaderStatus` | `web:src/pages/EstadoSection.tsx:910` |
| `useIrEmitters` | `SensorConnectionBar` | `web:src/pages/EstadoSection.tsx:959` |
| `useIrEmitters` | `SensorBottomConsole` | `web:src/pages/EstadoSection.tsx:1468` |
