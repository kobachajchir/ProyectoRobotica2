# Componentes comunes y estados visuales

ID: `web:module/src/components/SegmentedControl.tsx` · Capa: **integración** · Fotografía: 2026-10-20.

Documentación de fuente; sin tests, compilación, navegador ni validación física.

## Responsabilidad y alcance

Ofrece controles y presentación compartidos para secciones del cliente.

Props, selección, disclosure/modal y estados visuales; no decide autoridad del dispositivo.

<a id="modularity"></a>
## Modularidad observada

React con estilos compartidos; reutilización UI distinta de portabilidad de librería firmware.

<a id="state"></a>
## Handle, estado y atributos externos

Valor controlado y callbacks de props o estado local; definir dueño de selección para evitar desincronización.

<a id="lifecycle"></a>
## Ciclo de vida y contexto

- Montar con props válidas, emitir intención y reflejar valor confirmado/controlado.
- Liberar foco/listeners y restaurar scroll/contexto cuando corresponda.

<a id="interactions"></a>
## Interacciones

- Docs y paneles usan controles comunes; el selector de destino debe respetar el mismo contrato de valor/callback.

<a id="limitations"></a>
## Precauciones y límites

- Estado visual seleccionado no acredita éxito de una solicitud remota.

## Hallazgos conectados


## Archivos y evidencia

| Archivo | Análisis | Hash SHA-256 |
| --- | --- | --- |
| `src/components/AppLoadingScreen.tsx` | typescript-ast-static | `d2847cd7625f43a8fd4f3fc46ccfe53d4bc8f995ee30229aeb818a5f953a204a` |
| `src/components/ConditionStatus.css` | support-inventory | `af1f336b9587a0f6454737ff15d2bc6658d54a28795a4da4ae5be9d7c4fd88dd` |
| `src/components/ConditionStatus.tsx` | typescript-ast-static | `c157a321af8d2add1c00926d7f4117e259b3671ad51d9f192ffe5b3ea39e3273` |
| `src/components/ConnectingScreen.tsx` | typescript-ast-static | `13a7c76fdf894ef7118b71e74493dec5b5b3bc6e698f1e7658f1260e7b3de41d` |
| `src/components/SectionStatusStrip.tsx` | typescript-ast-static | `bc7117c4790849bae705b3943f9f535bd85298e1fb214ed6c607403048643e3d` |
| `src/components/SegmentedControl.css` | support-inventory | `91b04d78c7a574d3f3fc245ccb062b4315cdcf97933b06e7a96018f29c79693d` |
| `src/components/SegmentedControl.tsx` | typescript-ast-static | `89c7c19373f9dfaef7baec2a1cbf9bf615fc1d43f8eba19427efd6c2ef5556b7` |
| `src/components/SensorDisclosure.tsx` | typescript-ast-static | `a4691082e5512a12f0da186bb1e9b9c583e42ccc63ffe50180130bca19447d85` |
| `src/components/TimelineRow.tsx` | typescript-ast-static | `ec6f999b5b383a15bd4222a970d39052d2629007fc0e3c1854cdbfea96934116` |
| `src/components/ToggleButton.tsx` | typescript-ast-static | `c830ca8cb82dbcffa4cea4afb77a038de18608f2deccf1ce8b7964ac5fb7be7f` |
| `src/components/modal.tsx` | typescript-ast-static | `49811c5701c7cf1247b7b3ff3c46ae439fc84cc3da2051505804a52bd0fe9a9a` |

## Tipos, puertos, props y callbacks

En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.

### `SectionStatusTone`

`web:src/components/SectionStatusStrip.tsx:1`

Tipo: `"ok" \| "error" \| "info" \| "warn" \| "muted"`.

### `SectionStatusItem`

`web:src/components/SectionStatusStrip.tsx:3`

| Atributo / firma | Propósito |
| --- | --- |
| `label: string` | Dato de estado/configuración `label`; declaración estructural, interpretación específica pendiente de ampliar. |
| `value: string` | Dato de estado/configuración `value`; declaración estructural, interpretación específica pendiente de ampliar. |
| `detail?: string` | Dato de estado/configuración `detail`; declaración estructural, interpretación específica pendiente de ampliar. |
| `tone?: SectionStatusTone` | Dato de estado/configuración `tone`; declaración estructural, interpretación específica pendiente de ampliar. |
| `title?: string` | Dato de estado/configuración `title`; declaración estructural, interpretación específica pendiente de ampliar. |

### `SectionStatusStripProps`

`web:src/components/SectionStatusStrip.tsx:11`

| Atributo / firma | Propósito |
| --- | --- |
| `ariaLabel: string` | Dato de estado/configuración `ariaLabel`; declaración estructural, interpretación específica pendiente de ampliar. |
| `items: SectionStatusItem[]` | Dato de estado/configuración `items`; declaración estructural, interpretación específica pendiente de ampliar. |
| `className?: string` | Dato de estado/configuración `className`; declaración estructural, interpretación específica pendiente de ampliar. |

### `SegmentOption`

`web:src/components/SegmentedControl.tsx:3`

| Atributo / firma | Propósito |
| --- | --- |
| `value: string` | Dato de estado/configuración `value`; declaración estructural, interpretación específica pendiente de ampliar. |
| `label: string` | Dato de estado/configuración `label`; declaración estructural, interpretación específica pendiente de ampliar. |
| `disabled?: boolean` | Dato de estado/configuración `disabled`; declaración estructural, interpretación específica pendiente de ampliar. |

### `MinimalBlockProps`

`web:src/components/TimelineRow.tsx:25`

| Atributo / firma | Propósito |
| --- | --- |
| `fromPct?: number` | Dato de estado/configuración `fromPct`; declaración estructural, interpretación específica pendiente de ampliar. |
| `toPct?: number` | Dato de estado/configuración `toPct`; declaración estructural, interpretación específica pendiente de ampliar. |
| `speed?: number` | Dato de estado/configuración `speed`; declaración estructural, interpretación específica pendiente de ampliar. |

### `TimelineRowProps`

`web:src/components/TimelineRow.tsx:31`

| Atributo / firma | Propósito |
| --- | --- |
| `title: string` | Dato de estado/configuración `title`; declaración estructural, interpretación específica pendiente de ampliar. |
| `track: TrackKey` | Dato de estado/configuración `track`; declaración estructural, interpretación específica pendiente de ampliar. |
| `blocks: Block[]` | Dato de estado/configuración `blocks`; declaración estructural, interpretación específica pendiente de ampliar. |
| `totalMs: number` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `kindColor: Record<BlockKind, string>` | Dato de estado/configuración `kindColor`; declaración estructural, interpretación específica pendiente de ampliar. |
| `activeIndex: number` | Dato de estado/configuración `activeIndex`; declaración estructural, interpretación específica pendiente de ampliar. |
| `activeProgress: number` | Dato de estado/configuración `activeProgress`; declaración estructural, interpretación específica pendiente de ampliar. |
| `selectedId: string \| null` | Dato de estado/configuración `selectedId`; declaración estructural, interpretación específica pendiente de ampliar. |
| `onSelect: (id: string \| null) => void` | Operación on Select de Componentes comunes y estados visuales; semántica fina pendiente de ampliación. |
| `highlightIds?: string[]` | Dato de estado/configuración `highlightIds`; declaración estructural, interpretación específica pendiente de ampliar. |
| `blockProps?: Record<string, MinimalBlockProps>` | Contrato de operaciones externo; conservar la tabla y su contexto durante el uso. |
| `onReorder: (newOrder: Block[]) => void` | Operación on Reorder de Componentes comunes y estados visuales; semántica fina pendiente de ampliación. |
| `dndDisabled?: boolean` | Dato de estado/configuración `dndDisabled`; declaración estructural, interpretación específica pendiente de ampliar. |
| `onUpdateLabel?: (blockId: string, newLabel: string) => void` | Operación on Update Label de Componentes comunes y estados visuales; semántica fina pendiente de ampliación. |

### `Size`

`web:src/components/ToggleButton.tsx:4`

Tipo: `"sm" \| "md" \| "lg"`.

### `ToggleButtonProps`

`web:src/components/ToggleButton.tsx:6`

| Atributo / firma | Propósito |
| --- | --- |
| `checked?: boolean` | Dato de estado/configuración `checked`; declaración estructural, interpretación específica pendiente de ampliar. |
| `defaultChecked?: boolean` | Dato de estado/configuración `defaultChecked`; declaración estructural, interpretación específica pendiente de ampliar. |
| `onChange?: (checked: boolean) => void` | Operación on Change de Componentes comunes y estados visuales; semántica fina pendiente de ampliación. |
| `onActivate?: () => void` | Operación on Activate de Componentes comunes y estados visuales; semántica fina pendiente de ampliación. |
| `onDeactivate?: () => void` | Operación on Deactivate de Componentes comunes y estados visuales; semántica fina pendiente de ampliación. |
| `disabled?: boolean` | Dato de estado/configuración `disabled`; declaración estructural, interpretación específica pendiente de ampliar. |
| `size?: Size` | Dato de estado/configuración `size`; declaración estructural, interpretación específica pendiente de ampliar. |
| `className?: string` | Dato de estado/configuración `className`; declaración estructural, interpretación específica pendiente de ampliar. |
| `ariaLabel?: string` | Dato de estado/configuración `ariaLabel`; declaración estructural, interpretación específica pendiente de ampliar. |
| `labels?: boolean` | Dato de estado/configuración `labels`; declaración estructural, interpretación específica pendiente de ampliar. |
| `labelOn?: string` | Dato de estado/configuración `labelOn`; declaración estructural, interpretación específica pendiente de ampliar. |
| `labelOff?: string` | Dato de estado/configuración `labelOff`; declaración estructural, interpretación específica pendiente de ampliar. |
| `labelClassName?: string` | Dato de estado/configuración `labelClassName`; declaración estructural, interpretación específica pendiente de ampliar. |
| `title?: string` | Dato de estado/configuración `title`; declaración estructural, interpretación específica pendiente de ampliar. |
| `thumbContent?: (checked: boolean) => React.ReactNode` | Operación thumb Content de Componentes comunes y estados visuales; semántica fina pendiente de ampliación. |

### `ModalProps`

`web:src/components/modal.tsx:7`

| Atributo / firma | Propósito |
| --- | --- |
| `isOpen: boolean` | Dato de estado/configuración `isOpen`; declaración estructural, interpretación específica pendiente de ampliar. |
| `onClose: () => void` | Operación on Close de Componentes comunes y estados visuales; semántica fina pendiente de ampliación. |
| `children: React.ReactNode` | Dato de estado/configuración `children`; declaración estructural, interpretación específica pendiente de ampliar. |
| `closeOnOverlayClick?: boolean` | Dato de estado/configuración `closeOnOverlayClick`; declaración estructural, interpretación específica pendiente de ampliar. |
| `keyboardNavigationEnabled?: boolean` | Dato de estado/configuración `keyboardNavigationEnabled`; declaración estructural, interpretación específica pendiente de ampliar. |
| `keyboardScopeRef?: React.RefObject<HTMLDivElement \| null>` | Dato de estado/configuración `keyboardScopeRef`; declaración estructural, interpretación específica pendiente de ampliar. |
| `containerClassnames?: string` | Dato de estado/configuración `containerClassnames`; declaración estructural, interpretación específica pendiente de ampliar. |
| `ariaLabelledBy?: string` | Dato de estado/configuración `ariaLabelledBy`; declaración estructural, interpretación específica pendiente de ampliar. |
| `ariaDescribedBy?: string` | Dato de estado/configuración `ariaDescribedBy`; declaración estructural, interpretación específica pendiente de ampliar. |
| `closeDisabled?: boolean` | Dato de estado/configuración `closeDisabled`; declaración estructural, interpretación específica pendiente de ampliar. |
| `motionPreset?: "default" \| "motor"` | Dato de estado/configuración `motionPreset`; declaración estructural, interpretación específica pendiente de ampliar. |


## Funciones públicas / componentes / hooks

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-apploadingscreen"></a>`AppLoadingScreen({   label,   children,   className = "",   showSpinner = true, }: { label: string; children?: ReactNode; className?: string; showSpinner?: boolean; }): inferred-by-TypeScript` | Operación App Loading Screen de Componentes comunes y estados visuales; semántica fina pendiente de ampliación. | `web:src/components/AppLoadingScreen.tsx:4`; inferred-name |
| <a id="fn-conditioncheckbox"></a>`ConditionCheckbox({ checked, confirmed = true }: { checked: boolean; confirmed?: boolean }): inferred-by-TypeScript` | Operación Condition Checkbox de Componentes comunes y estados visuales; semántica fina pendiente de ampliación. | `web:src/components/ConditionStatus.tsx:3`; inferred-name |
| <a id="fn-conditionstatus"></a>`ConditionStatus({ label, detail, checked, confirmed = true, onResolve, pending = false }: { label: string; detail: string; checked: boolean; confirmed?: boolean; onResolve?: () => void; pending?: boolean; }): inferred-by-TypeScript` | Adapta la llamada a `useId` con el contexto de esta integración. | `web:src/components/ConditionStatus.tsx:10`; structural |
| <a id="fn-connectingscreen"></a>`ConnectingScreen(): inferred-by-TypeScript` | Operación Connecting Screen de Componentes comunes y estados visuales; semántica fina pendiente de ampliación. | `web:src/components/ConnectingScreen.tsx:1`; inferred-name |
| <a id="fn-modal"></a>`Modal({   isOpen,   onClose,   children,   closeOnOverlayClick = true,   keyboardNavigationEnabled = true,   keyboardScopeRef,   containerClassnames = "",   ariaLabelledBy,   ariaDescribedBy,   closeDisabled = false,   motionPreset = "default" }: inferred-by-TypeScript): inferred-by-TypeScript` | Coordina `createPortal`, `useEffect`, `useLayoutEffect` para Componentes comunes y estados visuales. | `web:src/components/modal.tsx:23`; structural |
| <a id="fn-sectionstatusstrip"></a>`SectionStatusStrip({   ariaLabel,   items,   className = "", }: SectionStatusStripProps): inferred-by-TypeScript` | Coordina ``section-status-strip ${className}`.trim`, `items.map` para Componentes comunes y estados visuales. | `web:src/components/SectionStatusStrip.tsx:17`; structural |
| <a id="fn-segmentedcontrol"></a>`SegmentedControl({ label, value, options, onChange }: { label: string; value: string; options: readonly SegmentOption[]; onChange: (value: string) => void; }): inferred-by-TypeScript` | Coordina `options.map`, `useId`, `useRef` para Componentes comunes y estados visuales. | `web:src/components/SegmentedControl.tsx:4`; structural |
| <a id="fn-sensordisclosure"></a>`SensorDisclosure({ title, icon, titleAccessory, headerActions, detail, className = "", defaultExpanded = false, disabled = false, disabledDescription, children }: { title: string; icon?: ReactNode; titleAccessory?: ReactNode; headerActions?: ReactNode; detail?: string; className?: string; defaultExpanded?: boolean; disabled?: boolean; disabledDescription?: string; children: ReactNode; }): inferred-by-TypeScript` | Coordina `title.toLowerCase`, `useEffect`, `useId` para Componentes comunes y estados visuales. | `web:src/components/SensorDisclosure.tsx:3`; structural |
| <a id="fn-sortableblock"></a>`SortableBlock({  b,  wPct,  isActive,  isPast,  isSelected,  isHighlighted,  fill,  onSelect,  shapeStyles,  title,  disabled,  onUpdateLabel, }: { b: Block; wPct: string; isActive: boolean; isPast: boolean; isSelected: boolean; isHighlighted: boolean; fill: number; onSelect: (id: string \| null) => void; shapeStyles: React.CSSProperties; title: string; disabled: boolean; onUpdateLabel?: (newLabel: string) => void; }): inferred-by-TypeScript` | Coordina `CSS.Transform.toString`, `Math.round`, `clamp01` para Componentes comunes y estados visuales. | `web:src/components/TimelineRow.tsx:159`; structural |
| <a id="fn-timelinerow"></a>`TimelineRow({  title,  track,  blocks,  totalMs,  kindColor,  activeIndex,  activeProgress,  selectedId,  onSelect,  highlightIds = [],  blockProps = {},  onReorder,  dndDisabled = false,  onUpdateLabel, }: TimelineRowProps): inferred-by-TypeScript` | Coordina `(() => { const ab = blocks.find((x) => x.id === activeId); if (!ab) return null; return ( <div className="flex items-center justify-between gap-3 rounded-md border border-cyan-200/70 bg-[var(--ui-bg-0)]/95 px-3 py-2 text-xs text-[var(--ui-text)] shadow-[0_20px_50px_rgba(0,0,0,0.55),0_0_24px_rgba(34,211,238,0.28)]" style={{ width: overlaySize?.w ?? 140, minWidth: 92, height: Math.min(overlaySize?.h ?? 72, 86), }} > <span className="min-w-0"> <span className="block truncate font-black">{ab.label}</span> <span className="mt-1 block text-[10px] text-[var(--ui-muted)]"> {ab.durationMs} ms </span> </span> {ab.kind !== "stop" ? ( <span className={`shrink-0 rounded-full border px-1.5 py-0.5 text-[9px] font-black ${ ab.direction === 1 ? "border-rose-300/45 text-rose-200" : "border-emerald-300/45 text-emerald-200" }`} > {ab.direction === 1 ? "ATRÁS" : "ADELANTE"} </span> ) : null} </div> ); })`, `blocks.map`, `blocks.some` para Componentes comunes y estados visuales. | `web:src/components/TimelineRow.tsx:379`; structural |
| <a id="fn-togglebutton"></a>`ToggleButton({  checked,  defaultChecked = false,  onChange,  onActivate,  onDeactivate,  disabled = false,  size = "md",  className = "",  ariaLabel = "Alternar",  labels = false, // Si se quieren mostrar etiquetas de "On" y "Off"  labelOn = "On",  labelOff = "Off",  labelClassName = "",  title = "",  thumbContent, }: ToggleButtonProps): inferred-by-TypeScript` | Coordina `[ "app-toggle inline-flex items-center rounded-full p-0.5 relative", "ring-1 ring-[var(--ui-ring)]", cfg.track, disabled ? "opacity-50 cursor-not-allowed" : "cursor-pointer", // enfoque accesible en nuestra app "focus-visible:outline-none focus-visible:ring-2 focus-visible:ring-cyan-400/50 focus-visible:ring-offset-2 focus-visible:ring-offset-[var(--ui-bg-0)]", className, ].join`, `[ "app-toggle__thumb rounded-full shadow-md", cfg.thumb ].join`, `thumbContent` para Componentes comunes y estados visuales. | `web:src/components/ToggleButton.tsx:36`; structural |
| <a id="fn-trackbadge"></a>`TrackBadge({ track }: { track: TrackKey }): inferred-by-TypeScript` | Operación Track Badge de Componentes comunes y estados visuales; semántica fina pendiente de ampliación. | `web:src/components/TimelineRow.tsx:348`; inferred-name |
| <a id="fn-trackmetric"></a>`TrackMetric({  label,  value,  active = false, }: { label: string; value: string; active?: boolean; }): inferred-by-TypeScript` | Operación Track Metric de Componentes comunes y estados visuales; semántica fina pendiente de ampliación. | `web:src/components/TimelineRow.tsx:357`; inferred-name |
| <a id="fn-clamp01"></a>`clamp01(v: number): inferred-by-TypeScript` | Coordina `Math.max`, `Math.min` para Componentes comunes y estados visuales. | `web:src/components/TimelineRow.tsx:53`; structural |
| <a id="fn-clamppct"></a>`clampPct(v: number, min: inferred-by-TypeScript, max: inferred-by-TypeScript): inferred-by-TypeScript` | Coordina `Math.max`, `Math.min` para Componentes comunes y estados visuales. | `web:src/components/TimelineRow.tsx:54`; structural |
| <a id="fn-getblockstyles"></a>`getBlockStyles(block: Block, p?: MinimalBlockProps): React.CSSProperties` | Coordina `Math.max`, `Math.min`, `clamp01` para Componentes comunes y estados visuales. | `web:src/components/TimelineRow.tsx:83`; structural |
| <a id="fn-timelinetoneclass"></a>`timelineToneClass(kind: BlockKind, highlighted: boolean, active: boolean): inferred-by-TypeScript` | Operación timeline Tone Class de Componentes comunes y estados visuales; semántica fina pendiente de ampliación. | `web:src/components/TimelineRow.tsx:58`; inferred-name |

## Funciones y callbacks internos

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-modal.getfocusables"></a>`Modal.getFocusables(): inferred-by-TypeScript` | Coordina `Array.from`, `Array.from(dialog?.querySelectorAll<HTMLElement>( 'a[href], button:not([disabled]), input:not([disabled]), select:not([disabled]), textarea:not([disabled]), [tabindex]:not([tabindex="-1"])', ) ?? []).filter`, `dialog?.querySelectorAll` para Componentes comunes y estados visuales. | `web:src/components/modal.tsx:94`; structural |
| <a id="fn-modal.keepdialogfocus"></a>`Modal.keepDialogFocus(): inferred-by-TypeScript` | Adapta la llamada a `dialog.focus` con el contexto de esta integración. | `web:src/components/modal.tsx:68`; structural |
| <a id="fn-modal.onfocusin"></a>`Modal.onFocusIn(event: FocusEvent): inferred-by-TypeScript` | Adapta la llamada a `keepDialogFocus` con el contexto de esta integración. | `web:src/components/modal.tsx:69`; structural |
| <a id="fn-modal.onkey"></a>`Modal.onKey(e: KeyboardEvent): inferred-by-TypeScript` | Coordina `dialog.contains`, `dialog?.focus`, `document.querySelectorAll` para Componentes comunes y estados visuales. | `web:src/components/modal.tsx:98`; structural |
| <a id="fn-modal.onpointerdown"></a>`Modal.onPointerDown(event: PointerEvent): inferred-by-TypeScript` | Coordina `event.preventDefault`, `keepDialogFocus` para Componentes comunes y estados visuales. | `web:src/components/modal.tsx:72`; structural |
| <a id="fn-modal.removecontrolfocus"></a>`Modal.removeControlFocus(): inferred-by-TypeScript` | Coordina `dialog.querySelectorAll`, `dialog.querySelectorAll<HTMLElement>( 'a[href], button, input, select, textarea, [tabindex], [contenteditable], summary, audio[controls], video[controls]', ).forEach` para Componentes comunes y estados visuales. | `web:src/components/modal.tsx:58`; structural |
| <a id="fn-sortableblock.handlefinishedit"></a>`SortableBlock.handleFinishEdit(): inferred-by-TypeScript` | Coordina `onUpdateLabel`, `setIsEditingLabel` para Componentes comunes y estados visuales. | `web:src/components/TimelineRow.tsx:214`; structural |
| <a id="fn-sortableblock.handlekeydown"></a>`SortableBlock.handleKeyDown(e: React.KeyboardEvent): inferred-by-TypeScript` | Coordina `handleFinishEdit`, `setEditValue`, `setIsEditingLabel` para Componentes comunes y estados visuales. | `web:src/components/TimelineRow.tsx:221`; structural |
| <a id="fn-sortableblock.handlestartedit"></a>`SortableBlock.handleStartEdit(): inferred-by-TypeScript` | Coordina `setEditValue`, `setIsEditingLabel` para Componentes comunes y estados visuales. | `web:src/components/TimelineRow.tsx:209`; structural |
| <a id="fn-timelinerow.handledragend"></a>`TimelineRow.handleDragEnd(e: DragEndEvent): inferred-by-TypeScript` | Coordina `arrayMove`, `blocks.findIndex`, `onReorder` para Componentes comunes y estados visuales. | `web:src/components/TimelineRow.tsx:415`; structural |
| <a id="fn-timelinerow.handledragstart"></a>`TimelineRow.handleDragStart(e: DragStartEvent): inferred-by-TypeScript` | Coordina `String`, `setActiveId` para Componentes comunes y estados visuales. | `web:src/components/TimelineRow.tsx:411`; structural |

## Estado y efectos React encontrados

| Hook | Función/closure | Evidencia |
| --- | --- | --- |
| `useId` | `ConditionStatus` | `web:src/components/ConditionStatus.tsx:13` |
| `useId` | `ConditionStatus` | `web:src/components/ConditionStatus.tsx:13` |
| `useId` | `SegmentedControl` | `web:src/components/SegmentedControl.tsx:7` |
| `useRef` | `SegmentedControl` | `web:src/components/SegmentedControl.tsx:7` |
| `useState` | `SensorDisclosure` | `web:src/components/SensorDisclosure.tsx:15` |
| `useId` | `SensorDisclosure` | `web:src/components/SensorDisclosure.tsx:16` |
| `useId` | `SensorDisclosure` | `web:src/components/SensorDisclosure.tsx:17` |
| `useId` | `SensorDisclosure` | `web:src/components/SensorDisclosure.tsx:18` |
| `useEffect` | `SensorDisclosure` | `web:src/components/SensorDisclosure.tsx:20` |
| `useSortable` | `SortableBlock` | `web:src/components/TimelineRow.tsx:187` |
| `useState` | `SortableBlock` | `web:src/components/TimelineRow.tsx:190` |
| `useState` | `SortableBlock` | `web:src/components/TimelineRow.tsx:191` |
| `useSensors` | `TimelineRow` | `web:src/components/TimelineRow.tsx:396` |
| `useSensor` | `TimelineRow` | `web:src/components/TimelineRow.tsx:397` |
| `useSensor` | `TimelineRow` | `web:src/components/TimelineRow.tsx:400` |
| `useState` | `TimelineRow` | `web:src/components/TimelineRow.tsx:405` |
| `useState` | `TimelineRow` | `web:src/components/TimelineRow.tsx:406` |
| `useState` | `ToggleButton` | `web:src/components/ToggleButton.tsx:54` |
| `useReducedMotion` | `ToggleButton` | `web:src/components/ToggleButton.tsx:56` |
| `useCallback` | `ToggleButton` | `web:src/components/ToggleButton.tsx:58` |
| `useCallback` | `ToggleButton` | `web:src/components/ToggleButton.tsx:67` |
| `useMemo` | `ToggleButton` | `web:src/components/ToggleButton.tsx:73` |
| `useMediaQuery` | `Modal` | `web:src/components/modal.tsx:36` |
| `useReducedMotion` | `Modal` | `web:src/components/modal.tsx:37` |
| `useRef` | `Modal` | `web:src/components/modal.tsx:38` |
| `useRef` | `Modal` | `web:src/components/modal.tsx:40` |
| `useModalScrollLock` | `Modal` | `web:src/components/modal.tsx:45` |
| `useEffect` | `Modal` | `web:src/components/modal.tsx:47` |
| `useLayoutEffect` | `Modal` | `web:src/components/modal.tsx:52` |
