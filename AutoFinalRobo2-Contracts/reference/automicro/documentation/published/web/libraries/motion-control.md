# Modo del auto y control de movimiento

ID: `web:module/src/contexts/CarModeContext.tsx` · Capa: **integración** · Fotografía: 2026-10-20.

Documentación de fuente; sin tests, compilación, navegador ni validación física.

## Responsabilidad y alcance

Presenta modo confirmado, armado/balance y consignas de movimiento/prueba.

IDLE/FOLLOW/TEST/GOTO y requests porcentuales con duración; no cierra el lazo de balance en navegador.

<a id="modularity"></a>
## Modularidad observada

Capa React con API JSON; control y guardas efectivas pertenecen a F4.

<a id="state"></a>
## Handle, estado y atributos externos

Contexto guarda rawMode y label; hooks guardan pending/error/status. Intención local debe distinguirse de modo/estado confirmado.

<a id="lifecycle"></a>
## Ciclo de vida y contexto

- Consultar estado/escuchar eventos; al cambiar modo esperar status y valor aplicado.
- Enviar consignas acotadas y cancelar/sincronizar al cambiar contexto.

<a id="interactions"></a>
## Interacciones

- F4 responde antes de eventos derivados de cambio de modo.
- ScreenContext mantiene otro normalizador de modo que debe aceptar los mismos cuatro valores.

<a id="limitations"></a>
## Precauciones y límites

- PWM/porcentaje no equivale a velocidad física.
- Conflictos con calibración/GOTO/seguidor se rechazan en F4, aunque un control Web esté habilitado.

## Hallazgos conectados

- [AMDOC-WEB-005: Normalizadores del visor pierden el modo GOTO](../../../review/FINDINGS.md#amdoc-web-005)

## Archivos y evidencia

| Archivo | Análisis | Hash SHA-256 |
| --- | --- | --- |
| `src/components/CarModeDropdown.tsx` | typescript-ast-static | `259c48084a432d0e8fb7b60743ab9179b0937362b205ff0ec840ec506499082b` |
| `src/components/CarModeSelectorModal.tsx` | typescript-ast-static | `16e2e1d60b278c59464a1c84ef6d843a762b5a61944c09366a3506613335a2a9` |
| `src/components/MotorBlock.tsx` | typescript-ast-static | `ce792225f459ee85a121401e75756fc2ff9c56ae8762c05f7e67c094cd2e075c` |
| `src/contexts/CarModeContext.tsx` | typescript-ast-static | `98c86dec2d30c2f5b76dc1da01ad777e952d2edf23ba3ad0a1a99dab98e1b275` |
| `src/features/motion/MotionControlActions.tsx` | typescript-ast-static | `633e578cf73e9f8672bb81451aac6aaac6c06d757f9d5f7b910329b6261cb029` |
| `src/features/motion/MotionControlIcon.tsx` | typescript-ast-static | `e7ea1b14218f2eb702d07eaed5d46a0512eac5de413a93445024ecb7b58f4419` |
| `src/features/motion/MotionControlPanel.tsx` | typescript-ast-static | `f2c4968e3c79713fbefa07a9b03b42e96dd4f8187140d23311e36e95b54a8ccc` |
| `src/features/motion/motion.css` | support-inventory | `2817ff3f0beea49709e76d5b9fb5e05043f02b69571208c3cda129008eba136a` |
| `src/features/motion/useMotionControl.ts` | typescript-ast-static | `df12ce99e650538e600ed457cbe24f127fdef51cd8a19535d8b3e66fdb2df2e4` |
| `src/pages/ControlSection.tsx` | typescript-ast-static | `98c5d412e4906b34680c328b00d834fc03e90f42f9417570e8c265823758ae6d` |
| `src/types/MotorTypes.ts` | typescript-ast-static | `c68bd947fbd9a24658697772f4e0cf0a728103bf4af9227f633b84acd14d6c66` |

## Tipos, puertos, props y callbacks

En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.

### `BlockKind`

`web:src/components/MotorBlock.tsx:3`

Tipo: `"ramp" \| "hold" \| "pivot" \| "arc" \| "stop"`.

### `Props`

`web:src/components/MotorBlock.tsx:5`

| Atributo / firma | Propósito |
| --- | --- |
| `kind: BlockKind` | Dato de estado/configuración `kind`; declaración estructural, interpretación específica pendiente de ampliar. |
| `selected?: boolean` | Dato de estado/configuración `selected`; declaración estructural, interpretación específica pendiente de ampliar. |
| `pairSelected?: boolean` | Dato de estado/configuración `pairSelected`; declaración estructural, interpretación específica pendiente de ampliar. |
| `disabled?: boolean` | Dato de estado/configuración `disabled`; declaración estructural, interpretación específica pendiente de ampliar. |
| `onClick?: () => void` | Operación on Click de Modo del auto y control de movimiento; semántica fina pendiente de ampliación. |

### `CarModeLabel`

`web:src/contexts/CarModeContext.tsx:17`

Tipo: `"IDLE" \| "FOLLOW" \| "TEST" \| "GOTO" \| "UNKNOWN"`.

### `SelectableCarMode`

`web:src/contexts/CarModeContext.tsx:18`

Tipo: `Exclude<CarModeLabel, "UNKNOWN">`.

### `CarModeStatus`

`web:src/contexts/CarModeContext.tsx:19`

Tipo: `"idle" \| "loading" \| "synced" \| "error"`.

### `CarModeContextValue`

`web:src/contexts/CarModeContext.tsx:29`

| Atributo / firma | Propósito |
| --- | --- |
| `mode: CarModeLabel` | Dato de estado/configuración `mode`; declaración estructural, interpretación específica pendiente de ampliar. |
| `rawMode: number \| null` | Dato de estado/configuración `rawMode`; declaración estructural, interpretación específica pendiente de ampliar. |
| `isTestMode: boolean` | Dato de estado/configuración `isTestMode`; declaración estructural, interpretación específica pendiente de ampliar. |
| `status: CarModeStatus` | Último resultado y guardas del módulo; los getters pueden devolver una vista de este almacenamiento. |
| `lastUpdatedAt: number \| null` | Dato de estado/configuración `lastUpdatedAt`; declaración estructural, interpretación específica pendiente de ampliar. |
| `requestCarMode: () => void` | Operación request Car Mode de Modo del auto y control de movimiento; semántica fina pendiente de ampliación. |
| `setCarMode: (mode: SelectableCarMode) => Promise<boolean>` | Operación set Car Mode de Modo del auto y control de movimiento; semántica fina pendiente de ampliación. |
| `applyCarModeValue: (value: number \| string, source?: string) => void` | Operación apply Car Mode Value de Modo del auto y control de movimiento; semántica fina pendiente de ampliación. |

### `CarModeProviderProps`

`web:src/contexts/CarModeContext.tsx:40`

| Atributo / firma | Propósito |
| --- | --- |
| `children: ReactNode` | Dato de estado/configuración `children`; declaración estructural, interpretación específica pendiente de ampliar. |

### `MotionIconName`

`web:src/features/motion/MotionControlIcon.tsx:1`

Tipo: `"balance" \| "stop" \| "refresh" \| "move" \| "test"`.

### `MotionState`

`web:src/features/motion/useMotionControl.ts:5`

| Atributo / firma | Propósito |
| --- | --- |
| `status: number` | Último resultado y guardas del módulo; los getters pueden devolver una vista de este almacenamiento. |
| `motionArmed: boolean` | Dato de estado/configuración `motionArmed`; declaración estructural, interpretación específica pendiente de ampliar. |
| `balanceActive: boolean` | Dato de estado/configuración `balanceActive`; declaración estructural, interpretación específica pendiente de ampliar. |
| `commandActive: boolean` | Dato de estado/configuración `commandActive`; declaración estructural, interpretación específica pendiente de ampliar. |
| `safetyStop: boolean` | Dato de estado/configuración `safetyStop`; declaración estructural, interpretación específica pendiente de ampliar. |
| `rejectReason: number` | Dato de estado/configuración `rejectReason`; declaración estructural, interpretación específica pendiente de ampliar. |
| `carMode: number` | Dato de estado/configuración `carMode`; declaración estructural, interpretación específica pendiente de ampliar. |
| `motorTestActive: boolean` | Dato de estado/configuración `motorTestActive`; declaración estructural, interpretación específica pendiente de ampliar. |

### `MotionController`

`web:src/features/motion/useMotionControl.ts:138`

Tipo: `ReturnType<typeof useMotionControl>`.

### `BlockProps`

`web:src/pages/ControlSection.tsx:110`

| Atributo / firma | Propósito |
| --- | --- |
| `durationMs: number` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `speed?: number` | Dato de estado/configuración `speed`; declaración estructural, interpretación específica pendiente de ampliar. |
| `fromPct?: number` | Dato de estado/configuración `fromPct`; declaración estructural, interpretación específica pendiente de ampliar. |
| `toPct?: number` | Dato de estado/configuración `toPct`; declaración estructural, interpretación específica pendiente de ampliar. |
| `arcSide?: 0 \| 1` | Dato de estado/configuración `arcSide`; declaración estructural, interpretación específica pendiente de ampliar. |
| `pivotBaseDir?: Dir` | Dato de estado/configuración `pivotBaseDir`; declaración estructural, interpretación específica pendiente de ampliar. |
| `pivotPairId?: string` | Dato de estado/configuración `pivotPairId`; declaración estructural, interpretación específica pendiente de ampliar. |

### `MinimalBlockProps`

`web:src/pages/ControlSection.tsx:120`

| Atributo / firma | Propósito |
| --- | --- |
| `fromPct?: number` | Dato de estado/configuración `fromPct`; declaración estructural, interpretación específica pendiente de ampliar. |
| `toPct?: number` | Dato de estado/configuración `toPct`; declaración estructural, interpretación específica pendiente de ampliar. |
| `speed?: number` | Dato de estado/configuración `speed`; declaración estructural, interpretación específica pendiente de ampliar. |

### `ValidationIssue`

`web:src/pages/ControlSection.tsx:122`

| Atributo / firma | Propósito |
| --- | --- |
| `track: TrackKey` | Dato de estado/configuración `track`; declaración estructural, interpretación específica pendiente de ampliar. |
| `blockId?: string` | Dato de estado/configuración `blockId`; declaración estructural, interpretación específica pendiente de ampliar. |
| `severity: "error" \| "warning"` | Dato de estado/configuración `severity`; declaración estructural, interpretación específica pendiente de ampliar. |
| `message: string` | Dato de estado/configuración `message`; declaración estructural, interpretación específica pendiente de ampliar. |

### `SimulationPose`

`web:src/pages/ControlSection.tsx:129`

| Atributo / firma | Propósito |
| --- | --- |
| `x: number` | Dato de estado/configuración `x`; declaración estructural, interpretación específica pendiente de ampliar. |
| `y: number` | Dato de estado/configuración `y`; declaración estructural, interpretación específica pendiente de ampliar. |
| `yawDeg: number` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `leftPct: number` | Dato de estado/configuración `leftPct`; declaración estructural, interpretación específica pendiente de ampliar. |
| `rightPct: number` | Dato de estado/configuración `rightPct`; declaración estructural, interpretación específica pendiente de ampliar. |
| `path: Array<{ x: number; y: number }>` | Dato de estado/configuración `path`; declaración estructural, interpretación específica pendiente de ampliar. |

### `Dir`

`web:src/types/MotorTypes.ts:1`

Tipo: `0 \| 1`.

### `MotorTarget`

`web:src/types/MotorTypes.ts:2`

Tipo: `"left" \| "right" \| "both"`.

### `BlockKind`

`web:src/types/MotorTypes.ts:3`

Tipo: `"ramp" \| "hold" \| "pivot" \| "arc" \| "stop"`.

### `TrackKey`

`web:src/types/MotorTypes.ts:4`

Tipo: `"left" \| "right" \| "dual"`.

### `Block`

`web:src/types/MotorTypes.ts:6`

| Atributo / firma | Propósito |
| --- | --- |
| `id: string` | Dato de estado/configuración `id`; declaración estructural, interpretación específica pendiente de ampliar. |
| `kind: BlockKind` | Dato de estado/configuración `kind`; declaración estructural, interpretación específica pendiente de ampliar. |
| `label: string` | Dato de estado/configuración `label`; declaración estructural, interpretación específica pendiente de ampliar. |
| `durationMs: number` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `direction: Dir` | Dato de estado/configuración `direction`; declaración estructural, interpretación específica pendiente de ampliar. |
| `speed?: number` | Dato de estado/configuración `speed`; declaración estructural, interpretación específica pendiente de ampliar. |


## Funciones públicas / componentes / hooks

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-blockminishape"></a>`BlockMiniShape({ kind }: { kind: BlockKind }): inferred-by-TypeScript` | Operación Block Mini Shape de Modo del auto y control de movimiento; semántica fina pendiente de ampliación. | `web:src/pages/ControlSection.tsx:2499`; inferred-name |
| <a id="fn-blockpalettepanel"></a>`BlockPalettePanel({  selection,  dualMode,  paletteKind,  pairSelected,  selectionLocked,  onAddBlock,  onSelectKind, }: { selection: { track: TrackKey; id: string } \| null; dualMode: boolean; paletteKind: BlockKind \| null; pairSelected: { track: TrackKey; id: string } \| null; selectionLocked: boolean; onAddBlock: () => void; onSelectKind: (kind: BlockKind) => void; }): inferred-by-TypeScript` | Coordina `(["ramp", "hold", "pivot", "arc", "stop"] as BlockKind[]).map`, `trackLabel` para Modo del auto y control de movimiento. | `web:src/pages/ControlSection.tsx:2388`; structural |
| <a id="fn-blockpropertiespanel"></a>`BlockPropertiesPanel({  selected,  selection,  durationMs,  setDurationMs,  speed,  setSpeed,  direction,  onDirectionChange,  fromPct,  setFromPct,  toPct,  setToPct,  arcSide,  setArcSide,  selectedHasValidationError,  selectedHasUnsavedChanges,  selectedIssues,  selectionLocked,  onSave,  onDuplicate,  onDelete, }: { selected: Block \| null; selection: { track: TrackKey; id: string } \| null; durationMs: number; setDurationMs: (value: number) => void; speed: number; setSpeed: (value: number) => void; direction: Dir; onDirectionChange: (value: Dir) => void; fromPct: number; setFromPct: (value: number) => void; toPct: number; setToPct: (value: number) => void; arcSide: 0 \| 1; setArcSide: (value: 0 \| 1) => void; selectedHasValidationError: boolean; selectedHasUnsavedChanges: boolean; selectedIssues: ValidationIssue[]; selectionLocked: boolean; onSave: () => void; onDuplicate: () => void; onDelete: () => void; }): inferred-by-TypeScript` | Coordina `String`, `kindBadgeClass`, `panelActionClass` para Modo del auto y control de movimiento. | `web:src/pages/ControlSection.tsx:2515`; structural |
| <a id="fn-carmodedropdown"></a>`CarModeDropdown({value,open,onOpenChange,onSelect,disabled=false,label="Cambiar",excludeCurrent=false}: { value:string;open:boolean;onOpenChange:(open:boolean)=>void;onSelect:(mode:SelectableCarMode)=>void; disabled?:boolean;label?:string;excludeCurrent?:boolean; }): inferred-by-TypeScript` | Coordina `(["IDLE","FOLLOW","TEST","GOTO"] as const).filter`, `(["IDLE","FOLLOW","TEST","GOTO"] as const).filter(mode=>!excludeCurrent\|\|mode!==value).map`, `useEffect` para Modo del auto y control de movimiento. | `web:src/components/CarModeDropdown.tsx:4`; structural |
| <a id="fn-carmodeprovider"></a>`CarModeProvider({ children }: CarModeProviderProps): inferred-by-TypeScript` | Coordina `useCallback`, `useEffect`, `useMemo` para Modo del auto y control de movimiento. | `web:src/contexts/CarModeContext.tsx:60`; structural |
| <a id="fn-carmodeselectormodal"></a>`CarModeSelectorModal({ open, recommended, onClose }: {open:boolean;recommended:SelectableCarMode;onClose:()=>void}): inferred-by-TypeScript` | Coordina `useCarMode`, `useEffect`, `useRef` para Modo del auto y control de movimiento. | `web:src/components/CarModeSelectorModal.tsx:11`; structural |
| <a id="fn-controlheaderstatus"></a>`ControlHeaderStatus({  connected,  carMode,  modeLabel,  safetyArmed,  ackLabel, }: { connected: boolean; carMode: string; modeLabel: string; safetyArmed: boolean; ackLabel: string; }): inferred-by-TypeScript` | Coordina `getEspConnectionDetail`, `getEspConnectionLabel`, `useEspWifiStatus` para Modo del auto y control de movimiento. | `web:src/pages/ControlSection.tsx:2272`; structural |
| <a id="fn-controlsection"></a>`ControlSection(): inferred-by-TypeScript` | Coordina `(["ramp", "hold", "pivot", "arc", "stop"] as BlockKind[]).map`, `Boolean`, `Math.max` para Modo del auto y control de movimiento. | `web:src/pages/ControlSection.tsx:149`; structural |
| <a id="fn-diagnosticrow"></a>`DiagnosticRow({  label,  value,  tone, }: { label: string; value: string; tone: "cyan" \| "emerald" \| "amber" \| "rose" \| "muted"; }): inferred-by-TypeScript` | Operación Diagnostic Row de Modo del auto y control de movimiento; semántica fina pendiente de ampliación. | `web:src/pages/ControlSection.tsx:3220`; inferred-name |
| <a id="fn-executionstatuspanel"></a>`ExecutionStatusPanel({  motionControl,  compact = false,  connected,  carMode,  carModeStatus,  safetyArmed,  physicalBlockedReason,  isSimulating,  lastLocalEvent,  validationRan,  validationErrorCount,  validationWarningCount, }: { motionControl: MotionController; compact?: boolean; connected: boolean; carMode: string; carModeStatus: string; safetyArmed: boolean; physicalBlockedReason: string; isSimulating: boolean; lastLocalEvent: string; validationRan: boolean; validationErrorCount: number; validationWarningCount: number; }): inferred-by-TypeScript` | Adapta la llamada a `sectionCardClass` con el contexto de esta integración. | `web:src/pages/ControlSection.tsx:3146`; structural |
| <a id="fn-metriccard"></a>`MetricCard({  label,  value,  tone = "cyan", }: { label: string; value: string; tone?: "cyan" \| "muted"; }): inferred-by-TypeScript` | Operación Metric Card de Modo del auto y control de movimiento; semántica fina pendiente de ampliación. | `web:src/pages/ControlSection.tsx:3125`; inferred-name |
| <a id="fn-motionbalancepanel"></a>`MotionBalancePanel({ control, showStatus = false, defaultExpanded = false, title = "Balance" }: { control: MotionController; showStatus?: boolean; defaultExpanded?: boolean; title?: string }): inferred-by-TypeScript` | Coordina `Math.abs`, `Number.isFinite`, `[["Inclinación", "pitchMeasuredDeg", "°"], ["Objetivo", "pitchSetpointDeg", "°"], ["Motor izquierdo", "leftOutputPercent", "%"], ["Motor derecho", "rightOutputPercent", "%"]].map` para Modo del auto y control de movimiento. | `web:src/features/motion/MotionControlPanel.tsx:38`; structural |
| <a id="fn-motionbalancestatus"></a>`MotionBalanceStatus({ control }: { control: MotionController }): inferred-by-TypeScript` | Operación Motion Balance Status de Modo del auto y control de movimiento; semántica fina pendiente de ampliación. | `web:src/features/motion/MotionControlPanel.tsx:8`; inferred-name |
| <a id="fn-motioncontrolactions"></a>`MotionControlActions({ control, isSimulating = false, onStopSimulation }: { control: MotionController; isSimulating?: boolean; onStopSimulation?: () => void; }): inferred-by-TypeScript` | Operación Motion Control Actions de Modo del auto y control de movimiento; semántica fina pendiente de ampliación. | `web:src/features/motion/MotionControlActions.tsx:6`; inferred-name |
| <a id="fn-motioncontrolfeedback"></a>`MotionControlFeedback({ control }: { control: MotionController }): inferred-by-TypeScript` | Operación Motion Control Feedback de Modo del auto y control de movimiento; semántica fina pendiente de ampliación. | `web:src/features/motion/MotionControlPanel.tsx:105`; inferred-name |
| <a id="fn-motioncontrolicon"></a>`MotionControlIcon({ name }: { name: MotionIconName }): inferred-by-TypeScript` | Operación Motion Control Icon de Modo del auto y control de movimiento; semántica fina pendiente de ampliación. | `web:src/features/motion/MotionControlIcon.tsx:3`; inferred-name |
| <a id="fn-motioncontrolpanel"></a>`MotionControlPanel(): inferred-by-TypeScript` | Adapta la llamada a `useMotionControl` con el contexto de esta integración. | `web:src/features/motion/MotionControlPanel.tsx:114`; structural |
| <a id="fn-motionsimulationpanel"></a>`MotionSimulationPanel({  pose,  isSimulating,  dualMode,  totalMs,  variant = "compact", }: { pose: SimulationPose; isSimulating: boolean; dualMode: boolean; totalMs: number; variant?: "compact" \| "large"; }): inferred-by-TypeScript` | Coordina `Math.round`, `formatMs`, `pose.path .map` para Modo del auto y control de movimiento. | `web:src/pages/ControlSection.tsx:2838`; structural |
| <a id="fn-motionvaluecontrol"></a>`MotionValueControl({ label, value, min, max, step = 1, unit, hint, disabled, onChange }: { label: string; value: number; min: number; max: number; step?: number; unit: string; hint: string; disabled: boolean; onChange: (value: number) => void; }): inferred-by-TypeScript` | Coordina `Number.isFinite`, `label.toLowerCase`, `useId` para Modo del auto y control de movimiento. | `web:src/features/motion/MotionControlPanel.tsx:17`; structural |
| <a id="fn-motorblock"></a>`MotorBlock({   kind,   selected = false,   pairSelected = false,   disabled = false,   onClick, }: Props): inferred-by-TypeScript` | Operación Motor Block de Modo del auto y control de movimiento; semántica fina pendiente de ampliación. | `web:src/components/MotorBlock.tsx:46`; inferred-name |
| <a id="fn-motortopview3d"></a>`MotorTopView3D({  modelUrl,  pose,  isSimulating, }: { modelUrl: string; pose: SimulationPose; isSimulating: boolean; }): inferred-by-TypeScript` | Operación Motor Top View3 D de Modo del auto y control de movimiento; semántica fina pendiente de ampliación. | `web:src/pages/ControlSection.tsx:2921`; inferred-name |
| <a id="fn-palettetoolbutton"></a>`PaletteToolButton({  kind,  selected,  pairSelected,  disabled,  onClick, }: { kind: BlockKind; selected: boolean; pairSelected: boolean; disabled: boolean; onClick: () => void; }): inferred-by-TypeScript` | Adapta la llamada a `paletteTone` con el contexto de esta integración. | `web:src/pages/ControlSection.tsx:2452`; structural |
| <a id="fn-panelactionicon"></a>`PanelActionIcon({ name }: { name: "save" \| "copy" \| "trash" }): inferred-by-TypeScript` | Operación Panel Action Icon de Modo del auto y control de movimiento; semántica fina pendiente de ampliación. | `web:src/pages/ControlSection.tsx:3395`; inferred-name |
| <a id="fn-propertynumber"></a>`PropertyNumber({  label,  value,  min,  suffix,  icon,  onChange, }: { label: string; value: number; min?: number; suffix: string; icon: string; onChange: (value: number) => void; }): inferred-by-TypeScript` | Operación Property Number de Modo del auto y control de movimiento; semántica fina pendiente de ampliación. | `web:src/pages/ControlSection.tsx:2735`; inferred-name |
| <a id="fn-propertyrange"></a>`PropertyRange({  label,  value,  icon,  onChange, }: { label: string; value: number; icon: string; onChange: (value: number) => void; }): inferred-by-TypeScript` | Coordina `Math.max`, `Math.min` para Modo del auto y control de movimiento. | `web:src/pages/ControlSection.tsx:2768`; structural |
| <a id="fn-propertyreadonly"></a>`PropertyReadOnly({  label,  value,  icon, }: { label: string; value: string; icon: string; }): inferred-by-TypeScript` | Operación Property Read Only de Modo del auto y control de movimiento; semántica fina pendiente de ampliación. | `web:src/pages/ControlSection.tsx:2715`; inferred-name |
| <a id="fn-propertyselect"></a>`PropertySelect({  label,  value,  options,  icon,  onChange, }: { label: string; value: string; options: Array<[string, string]>; icon: string; onChange: (value: string) => void; }): inferred-by-TypeScript` | Adapta la llamada a `options.map` con el contexto de esta integración. | `web:src/pages/ControlSection.tsx:2806`; structural |
| <a id="fn-simulationpath3d"></a>`SimulationPath3D({ path }: { path: SimulationPose["path"] }): inferred-by-TypeScript` | Adapta la llamada a `useMemo` con el contexto de esta integración. | `web:src/pages/ControlSection.tsx:3008`; structural |
| <a id="fn-simulationtransportcontrols"></a>`SimulationTransportControls({  motionControl,  dualMode,  isSimulating,  isPlayingLeft,  isPlayingRight,  isPlayingDual,  canRunLeft,  canRunRight,  canRunBoth,  physicalEnabled,  physicalBlockedReason,  onStop,  onRunLeft,  onRunRight,  onRunBoth,  onReset,  onExecutePhysical, }: { motionControl: MotionController; dualMode: boolean; isSimulating: boolean; isPlayingLeft: boolean; isPlayingRight: boolean; isPlayingDual: boolean; canRunLeft: boolean; canRunRight: boolean; canRunBoth: boolean; physicalEnabled: boolean; physicalBlockedReason: string; onStop: () => void; onRunLeft: () => void; onRunRight: () => void; onRunBoth: () => void; onReset: () => void; onExecutePhysical: () => void; }): inferred-by-TypeScript` | Adapta la llamada a `sectionCardClass` con el contexto de esta integración. | `web:src/pages/ControlSection.tsx:3248`; structural |
| <a id="fn-statuspill"></a>`StatusPill({  label,  value,  tone, }: { label: string; value: string; tone: "cyan" \| "emerald" \| "amber" \| "rose" \| "muted"; }): inferred-by-TypeScript` | Operación Status Pill de Modo del auto y control de movimiento; semántica fina pendiente de ampliación. | `web:src/pages/ControlSection.tsx:2318`; inferred-name |
| <a id="fn-technicaltopgrid"></a>`TechnicalTopGrid(): inferred-by-TypeScript` | Adapta la llamada a `useMemo` con el contexto de esta integración. | `web:src/pages/ControlSection.tsx:2988`; structural |
| <a id="fn-topcameracontroller"></a>`TopCameraController(): inferred-by-TypeScript` | Coordina `useEffect`, `useThree` para Modo del auto y control de movimiento. | `web:src/pages/ControlSection.tsx:2976`; structural |
| <a id="fn-topfallbackvehicle"></a>`TopFallbackVehicle({ pose }: { pose: SimulationPose }): inferred-by-TypeScript` | Coordina `THREE.MathUtils.degToRad`, `clamp` para Modo del auto y control de movimiento. | `web:src/pages/ControlSection.tsx:3106`; structural |
| <a id="fn-topvehiclemodel"></a>`TopVehicleModel({  modelUrl,  pose,  isSimulating, }: { modelUrl: string; pose: SimulationPose; isSimulating: boolean; }): inferred-by-TypeScript` | Coordina `THREE.MathUtils.degToRad`, `clamp`, `useLoader` para Modo del auto y control de movimiento. | `web:src/pages/ControlSection.tsx:3033`; structural |
| <a id="fn-tracksimbutton"></a>`TrackSimButton({  isPlaying,  disabled,  onClick, }: { isPlaying: boolean; disabled: boolean; onClick: () => void; }): inferred-by-TypeScript` | Operación Track Sim Button de Modo del auto y control de movimiento; semántica fina pendiente de ampliación. | `web:src/pages/ControlSection.tsx:2360`; inferred-name |
| <a id="fn-transportbutton"></a>`TransportButton({  label,  detail,  icon,  tone,  active = false,  disabled = false,  onClick, }: { label: string; detail: string; icon: string; tone: "cyan" \| "emerald" \| "rose" \| "muted"; active?: boolean; disabled?: boolean; onClick: () => void; }): inferred-by-TypeScript` | Operación Transport Button de Modo del auto y control de movimiento; semántica fina pendiente de ampliación. | `web:src/pages/ControlSection.tsx:3336`; inferred-name |
| <a id="fn-actionbuttonclass"></a>`actionButtonClass(tone: "emerald" \| "rose" \| "indigo", active: inferred-by-TypeScript): inferred-by-TypeScript` | Operación action Button Class de Modo del auto y control de movimiento; semántica fina pendiente de ampliación. | `web:src/pages/ControlSection.tsx:3660`; inferred-name |
| <a id="fn-blockruntimepct"></a>`blockRuntimePct(block: Block, props: MinimalBlockProps \| undefined, progress: number): inferred-by-TypeScript` | Adapta la llamada a `clamp` con el contexto de esta integración. | `web:src/pages/ControlSection.tsx:3522`; structural |
| <a id="fn-clamp"></a>`clamp(value: number, min: number, max: number): inferred-by-TypeScript` | Coordina `Math.max`, `Math.min` para Modo del auto y control de movimiento. | `web:src/pages/ControlSection.tsx:3580`; structural |
| <a id="fn-createinitialsimpose"></a>`createInitialSimPose(): SimulationPose` | Operación create Initial Sim Pose de Modo del auto y control de movimiento; semántica fina pendiente de ampliación. | `web:src/pages/ControlSection.tsx:3420`; inferred-name |
| <a id="fn-formatms"></a>`formatMs(value: number): inferred-by-TypeScript` | Coordina `(value / 1000).toFixed`, `Math.round` para Modo del auto y control de movimiento. | `web:src/pages/ControlSection.tsx:3584`; structural |
| <a id="fn-integratesimulationpose"></a>`integrateSimulationPose(pose: SimulationPose, leftPct: number, rightPct: number, dt: number): SimulationPose` | Coordina `Math.cos`, `Math.hypot`, `Math.sin` para Modo del auto y control de movimiento. | `web:src/pages/ControlSection.tsx:3541`; structural |
| <a id="fn-kindbadgeclass"></a>`kindBadgeClass(kind: BlockKind): inferred-by-TypeScript` | Operación kind Badge Class de Modo del auto y control de movimiento; semántica fina pendiente de ampliación. | `web:src/pages/ControlSection.tsx:3650`; inferred-name |
| <a id="fn-normalizecarmodevalue"></a>`normalizeCarModeValue(value: number \| string): number \| null` | Coordina `Math.max`, `Math.min`, `Math.trunc` para Modo del auto y control de movimiento. | `web:src/contexts/CarModeContext.tsx:266`; structural |
| <a id="fn-normalizedeg"></a>`normalizeDeg(value: number): inferred-by-TypeScript` | Operación normalize Deg de Modo del auto y control de movimiento; semántica fina pendiente de ampliación. | `web:src/pages/ControlSection.tsx:3573`; inferred-name |
| <a id="fn-palettetone"></a>`paletteTone(kind: BlockKind, selected: boolean): inferred-by-TypeScript` | Operación palette Tone de Modo del auto y control de movimiento; semántica fina pendiente de ampliación. | `web:src/pages/ControlSection.tsx:3366`; inferred-name |
| <a id="fn-panelactionclass"></a>`panelActionClass(tone: "cyan" \| "muted" \| "rose"): inferred-by-TypeScript` | Operación panel Action Class de Modo del auto y control de movimiento; semántica fina pendiente de ampliación. | `web:src/pages/ControlSection.tsx:3385`; inferred-name |
| <a id="fn-parsestate"></a>`parseState(value: unknown): MotionState` | Coordina `Number.isInteger`, `[state.motionArmed, state.balanceActive, state.commandActive, state.safetyStop, state.motorTestActive].every` para Modo del auto y control de movimiento. | `web:src/features/motion/useMotionControl.ts:22`; structural |
| <a id="fn-runtimemotorpct"></a>`runtimeMotorPct(blocks: Block[], props: Record<string, MinimalBlockProps>, activeIndex: number, activeProgress: number): inferred-by-TypeScript` | Adapta la llamada a `blockRuntimePct` con el contexto de esta integración. | `web:src/pages/ControlSection.tsx:3511`; structural |
| <a id="fn-sectioncardclass"></a>`sectionCardClass(tone: "cyan" \| "emerald" \| "indigo" \| "amber" \| "rose" \| "sky"): inferred-by-TypeScript` | Adapta la llamada a `sectionToneClass` con el contexto de esta integración. | `web:src/pages/ControlSection.tsx:3634`; structural |
| <a id="fn-sectionsegmentedbuttonstyle"></a>`sectionSegmentedButtonStyle(active: boolean, tone: "cyan" \| "emerald"): React.CSSProperties` | Operación section Segmented Button Style de Modo del auto y control de movimiento; semántica fina pendiente de ampliación. | `web:src/pages/ControlSection.tsx:3597`; inferred-name |
| <a id="fn-sectiontoneclass"></a>`sectionToneClass(tone: "cyan" \| "emerald" \| "indigo" \| "amber" \| "rose" \| "sky"): inferred-by-TypeScript` | Operación section Tone Class de Modo del auto y control de movimiento; semántica fina pendiente de ampliación. | `web:src/pages/ControlSection.tsx:3621`; inferred-name |
| <a id="fn-tonefromkind"></a>`toneFromKind(kind: BlockKind): inferred-by-TypeScript` | Operación tone From Kind de Modo del auto y control de movimiento; semántica fina pendiente de ampliación. | `web:src/pages/ControlSection.tsx:3640`; inferred-name |
| <a id="fn-tracklabel"></a>`trackLabel(track: TrackKey): inferred-by-TypeScript` | Operación track Label de Modo del auto y control de movimiento; semántica fina pendiente de ampliación. | `web:src/pages/ControlSection.tsx:3589`; inferred-name |
| <a id="fn-uid"></a>`uid(): inferred-by-TypeScript` | Coordina `Math.random`, `Math.random().toString`, `Math.random().toString(36).slice` para Modo del auto y control de movimiento. | `web:src/pages/ControlSection.tsx:39`; structural |
| <a id="fn-usecarmode"></a>`useCarMode(): CarModeContextValue` | Adapta la llamada a `useContext` con el contexto de esta integración. | `web:src/contexts/CarModeContext.tsx:256`; structural |
| <a id="fn-usemotioncontrol"></a>`useMotionControl(active: inferred-by-TypeScript): inferred-by-TypeScript` | Coordina `useCallback`, `useEffect`, `useRef` para Modo del auto y control de movimiento. | `web:src/features/motion/useMotionControl.ts:37`; structural |
| <a id="fn-validatetracksequence"></a>`validateTrackSequence(track: TrackKey, blocks: Block[], props: Record<string, MinimalBlockProps>, dualMode: boolean): ValidationIssue[]` | Coordina `blocks.forEach`, `issues.push`, `trackLabel` para Modo del auto y control de movimiento. | `web:src/pages/ControlSection.tsx:3427`; structural |

## Funciones y callbacks internos

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-carmodedropdown.keyboard"></a>`CarModeDropdown.keyboard(event: KeyboardEvent<HTMLDivElement>): inferred-by-TypeScript` | Coordina `Array.from`, `["ArrowDown","ArrowUp","Home","End"].includes`, `event.preventDefault` para Modo del auto y control de movimiento. | `web:src/components/CarModeDropdown.tsx:15`; structural |
| <a id="fn-carmodedropdown.outside"></a>`CarModeDropdown.outside(event: PointerEvent): inferred-by-TypeScript` | Coordina `onOpenChange`, `ref.current?.contains` para Modo del auto y control de movimiento. | `web:src/components/CarModeDropdown.tsx:11`; structural |
| <a id="fn-carmodeprovider.applyevent"></a>`CarModeProvider.applyEvent({ data }: { data: unknown }): inferred-by-TypeScript` | Adapta la llamada a `applyCarModeValue` con el contexto de esta integración. | `web:src/contexts/CarModeContext.tsx:184`; structural |
| <a id="fn-carmodeselectormodal.apply"></a>`CarModeSelectorModal.apply(): inferred-by-TypeScript` | Coordina `mode.setCarMode`, `onClose`, `setPending` para Modo del auto y control de movimiento. | `web:src/components/CarModeSelectorModal.tsx:16`; structural |
| <a id="fn-controlsection.attemptselect"></a>`ControlSection.attemptSelect(track: TrackKey, id: string \| null): inferred-by-TypeScript` | Coordina `getBlocks`, `list.find`, `otherList.find` para Modo del auto y control de movimiento. | `web:src/pages/ControlSection.tsx:424`; structural |
| <a id="fn-controlsection.blockstartref"></a>`ControlSection.blockStartRef(t: TrackKey): inferred-by-TypeScript` | Operación block Start Ref de Modo del auto y control de movimiento; semántica fina pendiente de ampliación. | `web:src/pages/ControlSection.tsx:320`; inferred-name |
| <a id="fn-controlsection.buildminimalprops"></a>`ControlSection.buildMinimalProps(list: Block[], trackKey: TrackKey): Record<string, MinimalBlockProps>` | Adapta la llamada a `list.forEach` con el contexto de esta integración. | `web:src/pages/ControlSection.tsx:628`; structural |
| <a id="fn-controlsection.changeselecteddirection"></a>`ControlSection.changeSelectedDirection(newDirection: Dir): inferred-by-TypeScript` | Coordina `setBlocks`, `setBlocks(otherTrack)`, `setDirection` para Modo del auto y control de movimiento. | `web:src/pages/ControlSection.tsx:844`; structural |
| <a id="fn-controlsection.cleartimer"></a>`ControlSection.clearTimer(t: TrackKey): inferred-by-TypeScript` | Coordina `timerRef`, `window.clearTimeout` para Modo del auto y control de movimiento. | `web:src/pages/ControlSection.tsx:339`; structural |
| <a id="fn-controlsection.createpairedpivot"></a>`ControlSection.createPairedPivot(): inferred-by-TypeScript` | Coordina `attemptSelect`, `setBlockProps`, `setBlocksLeft` para Modo del auto y control de movimiento. | `web:src/pages/ControlSection.tsx:503`; structural |
| <a id="fn-controlsection.duplicateselected"></a>`ControlSection.duplicateSelected(): inferred-by-TypeScript` | Coordina `getDefaultPropsFor`, `setBlockProps`, `setBlocks` para Modo del auto y control de movimiento. | `web:src/pages/ControlSection.tsx:919`; structural |
| <a id="fn-controlsection.fill"></a>`ControlSection.fill(list: Block[]): inferred-by-TypeScript` | Adapta la llamada a `list.forEach` con el contexto de esta integración. | `web:src/pages/ControlSection.tsx:267`; structural |
| <a id="fn-controlsection.getblocks"></a>`ControlSection.getBlocks(t: TrackKey): inferred-by-TypeScript` | Operación get Blocks de Modo del auto y control de movimiento; semántica fina pendiente de ampliación. | `web:src/pages/ControlSection.tsx:311`; inferred-name |
| <a id="fn-controlsection.getdefaultpropsfor"></a>`ControlSection.getDefaultPropsFor(b: Block): BlockProps` | Operación get Default Props For de Modo del auto y control de movimiento; semántica fina pendiente de ampliación. | `web:src/pages/ControlSection.tsx:237`; inferred-name |
| <a id="fn-controlsection.handlereorder"></a>`ControlSection.handleReorder(track: TrackKey): inferred-by-TypeScript` | Operación handle Reorder de Modo del auto y control de movimiento; semántica fina pendiente de ampliación. | `web:src/pages/ControlSection.tsx:461`; inferred-name |
| <a id="fn-controlsection.onaddblock"></a>`ControlSection.onAddBlock(): inferred-by-TypeScript` | Coordina `attemptSelect`, `createPairedPivot`, `k.charAt` para Modo del auto y control de movimiento. | `web:src/pages/ControlSection.tsx:474`; structural |
| <a id="fn-controlsection.onremoveselected"></a>`ControlSection.onRemoveSelected(): inferred-by-TypeScript` | Coordina `setBlockProps`, `setBlocks`, `setBlocks(otherTrack)` para Modo del auto y control de movimiento. | `web:src/pages/ControlSection.tsx:545`; structural |
| <a id="fn-controlsection.resetlocalsimulation"></a>`ControlSection.resetLocalSimulation(): inferred-by-TypeScript` | Coordina `resetSimulationPose`, `setLastLocalEvent`, `stopAllTracks` para Modo del auto y control de movimiento. | `web:src/pages/ControlSection.tsx:889`; structural |
| <a id="fn-controlsection.resetsimulationpose"></a>`ControlSection.resetSimulationPose(): inferred-by-TypeScript` | Coordina `createInitialSimPose`, `setSimPose` para Modo del auto y control de movimiento. | `web:src/pages/ControlSection.tsx:840`; structural |
| <a id="fn-controlsection.runblock"></a>`ControlSection.runBlock(t: TrackKey, i: number): inferred-by-TypeScript` | Coordina `Date.now`, `blockStartRef`, `getBlocks` para Modo del auto y control de movimiento. | `web:src/pages/ControlSection.tsx:381`; structural |
| <a id="fn-controlsection.runblock.tick"></a>`ControlSection.runBlock.tick(): inferred-by-TypeScript` | Coordina `Date.now`, `blockStartRef`, `setActiveProgress` para Modo del auto y control de movimiento. | `web:src/pages/ControlSection.tsx:404`; structural |
| <a id="fn-controlsection.runvalidation"></a>`ControlSection.runValidation(): inferred-by-TypeScript` | Coordina `setLastLocalEvent`, `setValidationRan` para Modo del auto y control de movimiento. | `web:src/pages/ControlSection.tsx:874`; structural |
| <a id="fn-controlsection.saveselectedblock"></a>`ControlSection.saveSelectedBlock(): inferred-by-TypeScript` | Coordina `setBlockProps`, `setBlocks`, `setBlocks(otherTrack)` para Modo del auto y control de movimiento. | `web:src/pages/ControlSection.tsx:942`; structural |
| <a id="fn-controlsection.setactiveindex"></a>`ControlSection.setActiveIndex(t: TrackKey, idx: number): inferred-by-TypeScript` | Coordina `setActiveIndexDual`, `setActiveIndexLeft`, `setActiveIndexRight` para Modo del auto y control de movimiento. | `web:src/pages/ControlSection.tsx:327`; structural |
| <a id="fn-controlsection.setactiveprogress"></a>`ControlSection.setActiveProgress(t: TrackKey, prog: number): inferred-by-TypeScript` | Coordina `Math.max`, `Math.min`, `setActiveProgressDual` para Modo del auto y control de movimiento. | `web:src/pages/ControlSection.tsx:332`; structural |
| <a id="fn-controlsection.setblocks"></a>`ControlSection.setBlocks(t: TrackKey): inferred-by-TypeScript` | Operación set Blocks de Modo del auto y control de movimiento; semántica fina pendiente de ampliación. | `web:src/pages/ControlSection.tsx:313`; inferred-name |
| <a id="fn-controlsection.setcontrolmode"></a>`ControlSection.setControlMode(mode: "free"\|"goto"): inferred-by-TypeScript` | Adapta la llamada a `setControlSearch` con el contexto de esta integración. | `web:src/pages/ControlSection.tsx:155`; structural |
| <a id="fn-controlsection.startsimulationrun"></a>`ControlSection.startSimulationRun(scope: "left" \| "right" \| "both"): inferred-by-TypeScript` | Coordina `resetSimulationPose`, `runValidation`, `startTrack` para Modo del auto y control de movimiento. | `web:src/pages/ControlSection.tsx:895`; structural |
| <a id="fn-controlsection.starttrack"></a>`ControlSection.startTrack(t: TrackKey): inferred-by-TypeScript` | Coordina `clearTimer`, `getBlocks`, `runBlock` para Modo del auto y control de movimiento. | `web:src/pages/ControlSection.tsx:367`; structural |
| <a id="fn-controlsection.stopalltracks"></a>`ControlSection.stopAllTracks(): inferred-by-TypeScript` | Adapta la llamada a `(["left", "right", "dual"] as TrackKey[]).forEach` con el contexto de esta integración. | `web:src/pages/ControlSection.tsx:356`; structural |
| <a id="fn-controlsection.stoplocalsimulation"></a>`ControlSection.stopLocalSimulation(): inferred-by-TypeScript` | Coordina `setLastLocalEvent`, `stopAllTracks` para Modo del auto y control de movimiento. | `web:src/pages/ControlSection.tsx:884`; structural |
| <a id="fn-controlsection.stoptrack"></a>`ControlSection.stopTrack(t: TrackKey): inferred-by-TypeScript` | Coordina `clearTimer`, `setActiveIndex`, `setActiveProgress` para Modo del auto y control de movimiento. | `web:src/pages/ControlSection.tsx:347`; structural |
| <a id="fn-controlsection.timerref"></a>`ControlSection.timerRef(t: TrackKey): inferred-by-TypeScript` | Operación timer Ref de Modo del auto y control de movimiento; semántica fina pendiente de ampliación. | `web:src/pages/ControlSection.tsx:318`; inferred-name |
| <a id="fn-controlsection.updateintrack"></a>`ControlSection.updateInTrack(setter: React.Dispatch<React.SetStateAction<Block[]>>, blockIndex: number): inferred-by-TypeScript` | Adapta la llamada a `setter` con el contexto de esta integración. | `web:src/pages/ControlSection.tsx:1029`; structural |
| <a id="fn-motionbalancepanel.display"></a>`MotionBalancePanel.display(key: string, suffix: string): inferred-by-TypeScript` | Coordina `Number.isFinite`, `value.toFixed` para Modo del auto y control de movimiento. | `web:src/features/motion/MotionControlPanel.tsx:48`; structural |
| <a id="fn-usemotioncontrol.poll"></a>`useMotionControl.poll(): inferred-by-TypeScript` | Coordina `read`, `setError`, `setPending` para Modo del auto y control de movimiento. | `web:src/features/motion/useMotionControl.ts:67`; structural |
| <a id="fn-usemotioncontrol.refresh"></a>`useMotionControl.refresh(): inferred-by-TypeScript` | Coordina `read`, `setError`, `setPending` para Modo del auto y control de movimiento. | `web:src/features/motion/useMotionControl.ts:108`; structural |
| <a id="fn-usemotioncontrol.run"></a>`useMotionControl.run(command: string, args: Record<string, unknown>): inferred-by-TypeScript` | Coordina `parseState`, `request`, `setError` para Modo del auto y control de movimiento. | `web:src/features/motion/useMotionControl.ts:92`; structural |

## Estado y efectos React encontrados

| Hook | Función/closure | Evidencia |
| --- | --- | --- |
| `useId` | `CarModeDropdown` | `web:src/components/CarModeDropdown.tsx:8` |
| `useRef` | `CarModeDropdown` | `web:src/components/CarModeDropdown.tsx:8` |
| `useRef` | `CarModeDropdown` | `web:src/components/CarModeDropdown.tsx:8` |
| `useEffect` | `CarModeDropdown` | `web:src/components/CarModeDropdown.tsx:9` |
| `useEffect` | `CarModeDropdown` | `web:src/components/CarModeDropdown.tsx:25` |
| `useCarMode` | `CarModeSelectorModal` | `web:src/components/CarModeSelectorModal.tsx:12` |
| `useState` | `CarModeSelectorModal` | `web:src/components/CarModeSelectorModal.tsx:12` |
| `useState` | `CarModeSelectorModal` | `web:src/components/CarModeSelectorModal.tsx:12` |
| `useState` | `CarModeSelectorModal` | `web:src/components/CarModeSelectorModal.tsx:13` |
| `useRef` | `CarModeSelectorModal` | `web:src/components/CarModeSelectorModal.tsx:14` |
| `useEffect` | `CarModeSelectorModal` | `web:src/components/CarModeSelectorModal.tsx:15` |
| `useWebSocket` | `CarModeProvider` | `web:src/contexts/CarModeContext.tsx:61` |
| `useUser` | `CarModeProvider` | `web:src/contexts/CarModeContext.tsx:62` |
| `useState` | `CarModeProvider` | `web:src/contexts/CarModeContext.tsx:63` |
| `useState` | `CarModeProvider` | `web:src/contexts/CarModeContext.tsx:64` |
| `useState` | `CarModeProvider` | `web:src/contexts/CarModeContext.tsx:65` |
| `useState` | `CarModeProvider` | `web:src/contexts/CarModeContext.tsx:66` |
| `useRef` | `CarModeProvider` | `web:src/contexts/CarModeContext.tsx:67` |
| `useRef` | `CarModeProvider` | `web:src/contexts/CarModeContext.tsx:68` |
| `useCallback` | `CarModeProvider` | `web:src/contexts/CarModeContext.tsx:70` |
| `useCallback` | `CarModeProvider` | `web:src/contexts/CarModeContext.tsx:90` |
| `useCallback` | `CarModeProvider` | `web:src/contexts/CarModeContext.tsx:107` |
| `useEffect` | `CarModeProvider` | `web:src/contexts/CarModeContext.tsx:183` |
| `useEffect` | `CarModeProvider` | `web:src/contexts/CarModeContext.tsx:196` |
| `useEffect` | `CarModeProvider` | `web:src/contexts/CarModeContext.tsx:216` |
| `useMemo` | `CarModeProvider` | `web:src/contexts/CarModeContext.tsx:235` |
| `useContext` | `useCarMode` | `web:src/contexts/CarModeContext.tsx:257` |
| `useId` | `MotionValueControl` | `web:src/features/motion/MotionControlPanel.tsx:21` |
| `useId` | `MotionValueControl` | `web:src/features/motion/MotionControlPanel.tsx:22` |
| `useState` | `MotionBalancePanel` | `web:src/features/motion/MotionControlPanel.tsx:39` |
| `useState` | `MotionBalancePanel` | `web:src/features/motion/MotionControlPanel.tsx:40` |
| `useState` | `MotionBalancePanel` | `web:src/features/motion/MotionControlPanel.tsx:41` |
| `useState` | `MotionBalancePanel` | `web:src/features/motion/MotionControlPanel.tsx:42` |
| `useState` | `MotionBalancePanel` | `web:src/features/motion/MotionControlPanel.tsx:43` |
| `useId` | `MotionBalancePanel` | `web:src/features/motion/MotionControlPanel.tsx:44` |
| `useMotionControl` | `MotionControlPanel` | `web:src/features/motion/MotionControlPanel.tsx:115` |
| `useWebSocket` | `useMotionControl` | `web:src/features/motion/useMotionControl.ts:38` |
| `useState` | `useMotionControl` | `web:src/features/motion/useMotionControl.ts:39` |
| `useState` | `useMotionControl` | `web:src/features/motion/useMotionControl.ts:40` |
| `useState` | `useMotionControl` | `web:src/features/motion/useMotionControl.ts:41` |
| `useState` | `useMotionControl` | `web:src/features/motion/useMotionControl.ts:42` |
| `useRef` | `useMotionControl` | `web:src/features/motion/useMotionControl.ts:43` |
| `useRef` | `useMotionControl` | `web:src/features/motion/useMotionControl.ts:44` |
| `useCallback` | `useMotionControl` | `web:src/features/motion/useMotionControl.ts:47` |
| `useEffect` | `useMotionControl` | `web:src/features/motion/useMotionControl.ts:59` |
| `useWebSocket` | `ControlSection` | `web:src/pages/ControlSection.tsx:150` |
| `useMotionControl` | `ControlSection` | `web:src/pages/ControlSection.tsx:151` |
| `useAutoMotion` | `ControlSection` | `web:src/pages/ControlSection.tsx:152` |
| `useSearchParams` | `ControlSection` | `web:src/pages/ControlSection.tsx:153` |
| `useCarMode` | `ControlSection` | `web:src/pages/ControlSection.tsx:156` |
| `useState` | `ControlSection` | `web:src/pages/ControlSection.tsx:159` |
| `useState` | `ControlSection` | `web:src/pages/ControlSection.tsx:162` |
| `useState` | `ControlSection` | `web:src/pages/ControlSection.tsx:165` |
| `useState` | `ControlSection` | `web:src/pages/ControlSection.tsx:172` |
| `useState` | `ControlSection` | `web:src/pages/ControlSection.tsx:181` |
| `useState` | `ControlSection` | `web:src/pages/ControlSection.tsx:187` |
| `useState` | `ControlSection` | `web:src/pages/ControlSection.tsx:190` |
| `useState` | `ControlSection` | `web:src/pages/ControlSection.tsx:191` |
| `useState` | `ControlSection` | `web:src/pages/ControlSection.tsx:192` |
| `useState` | `ControlSection` | `web:src/pages/ControlSection.tsx:194` |
| `useState` | `ControlSection` | `web:src/pages/ControlSection.tsx:195` |
| `useState` | `ControlSection` | `web:src/pages/ControlSection.tsx:196` |
| `useState` | `ControlSection` | `web:src/pages/ControlSection.tsx:198` |
| `useState` | `ControlSection` | `web:src/pages/ControlSection.tsx:199` |
| `useState` | `ControlSection` | `web:src/pages/ControlSection.tsx:200` |
| `useState` | `ControlSection` | `web:src/pages/ControlSection.tsx:202` |
| `useState` | `ControlSection` | `web:src/pages/ControlSection.tsx:203` |
| `useState` | `ControlSection` | `web:src/pages/ControlSection.tsx:204` |
| `useState` | `ControlSection` | `web:src/pages/ControlSection.tsx:205` |
| `useState` | `ControlSection` | `web:src/pages/ControlSection.tsx:206` |
| `useRef` | `ControlSection` | `web:src/pages/ControlSection.tsx:209` |
| `useRef` | `ControlSection` | `web:src/pages/ControlSection.tsx:210` |
| `useRef` | `ControlSection` | `web:src/pages/ControlSection.tsx:211` |
| `useRef` | `ControlSection` | `web:src/pages/ControlSection.tsx:214` |
| `useRef` | `ControlSection` | `web:src/pages/ControlSection.tsx:215` |
| `useRef` | `ControlSection` | `web:src/pages/ControlSection.tsx:216` |
| `useRef` | `ControlSection` | `web:src/pages/ControlSection.tsx:217` |
| `useState` | `ControlSection` | `web:src/pages/ControlSection.tsx:234` |
| `useState` | `ControlSection` | `web:src/pages/ControlSection.tsx:254` |
| `useState` | `ControlSection` | `web:src/pages/ControlSection.tsx:255` |
| `useState` | `ControlSection` | `web:src/pages/ControlSection.tsx:256` |
| `useState` | `ControlSection` | `web:src/pages/ControlSection.tsx:257` |
| `useState` | `ControlSection` | `web:src/pages/ControlSection.tsx:258` |
| `useState` | `ControlSection` | `web:src/pages/ControlSection.tsx:259` |
| `useState` | `ControlSection` | `web:src/pages/ControlSection.tsx:260` |
| `useEffect` | `ControlSection` | `web:src/pages/ControlSection.tsx:263` |
| `useEffect` | `ControlSection` | `web:src/pages/ControlSection.tsx:281` |
| `useEffect` | `ControlSection` | `web:src/pages/ControlSection.tsx:360` |
| `useState` | `ControlSection` | `web:src/pages/ControlSection.tsx:468` |
| `useMemo` | `ControlSection` | `web:src/pages/ControlSection.tsx:581` |
| `useMemo` | `ControlSection` | `web:src/pages/ControlSection.tsx:590` |
| `useMemo` | `ControlSection` | `web:src/pages/ControlSection.tsx:599` |
| `useMemo` | `ControlSection` | `web:src/pages/ControlSection.tsx:610` |
| `useMemo` | `ControlSection` | `web:src/pages/ControlSection.tsx:615` |
| `useMemo` | `ControlSection` | `web:src/pages/ControlSection.tsx:621` |
| `useMemo` | `ControlSection` | `web:src/pages/ControlSection.tsx:650` |
| `useMemo` | `ControlSection` | `web:src/pages/ControlSection.tsx:662` |
| `useMemo` | `ControlSection` | `web:src/pages/ControlSection.tsx:674` |
| `useMemo` | `ControlSection` | `web:src/pages/ControlSection.tsx:690` |
| `useMemo` | `ControlSection` | `web:src/pages/ControlSection.tsx:733` |
| `useEffect` | `ControlSection` | `web:src/pages/ControlSection.tsx:765` |
| `useEffect` | `ControlSection` | `web:src/pages/ControlSection.tsx:797` |
| `useCallback` | `ControlSection` | `web:src/pages/ControlSection.tsx:1016` |
| `useCallback` | `ControlSection` | `web:src/pages/ControlSection.tsx:1081` |
| `useEspWifiStatus` | `ControlHeaderStatus` | `web:src/pages/ControlSection.tsx:2285` |
| `useThree` | `TopCameraController` | `web:src/pages/ControlSection.tsx:2977` |
| `useEffect` | `TopCameraController` | `web:src/pages/ControlSection.tsx:2979` |
| `useMemo` | `TechnicalTopGrid` | `web:src/pages/ControlSection.tsx:2989` |
| `useMemo` | `SimulationPath3D` | `web:src/pages/ControlSection.tsx:3009` |
| `useLoader` | `TopVehicleModel` | `web:src/pages/ControlSection.tsx:3042` |
| `useMemo` | `TopVehicleModel` | `web:src/pages/ControlSection.tsx:3046` |
