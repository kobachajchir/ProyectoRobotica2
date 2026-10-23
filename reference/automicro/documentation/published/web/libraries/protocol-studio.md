# Interfaz Protocol Studio

ID: `web:module/src/features/protocolStudio/useProtocolStudioState.ts` · Capa: **integración** · Fotografía: 2026-10-20.

Documentación de fuente; sin tests, compilación, navegador ni validación física.

## Responsabilidad y alcance

Presenta catálogo, formularios y traducción/visualización de frames del protocolo.

Edición de request y diagnóstico visual, sin afirmar que todos los comandos están habilitados.

<a id="modularity"></a>
## Modularidad observada

React sobre codecs/catalogo, separado del backend de firmware.

<a id="state"></a>
## Handle, estado y atributos externos

Hook conserva comando, campos, bytes y salida; componentes reciben props derivadas.

<a id="lifecycle"></a>
## Ciclo de vida y contexto

- Actualizar selección/inputs y mostrar serialización/interpretación.
- No enviar una mutación por el solo hecho de cambiar campos de preview.

<a id="interactions"></a>
## Interacciones

- Codecs producen datos; conexión/sesión controla envío cuando la herramienta lo permite.

<a id="limitations"></a>
## Precauciones y límites

- La salida local no es respuesta de un dispositivo hasta recibirla por transporte.

## Hallazgos conectados


## Archivos y evidencia

| Archivo | Análisis | Hash SHA-256 |
| --- | --- | --- |
| `src/features/protocolStudio/components/BuilderColumn.tsx` | typescript-ast-static | `7826c2f36997c9e1bebe1d362357a2c1cd30011cb19407ec9588952625f55827` |
| `src/features/protocolStudio/components/FrameVisualizationShowcase.tsx` | typescript-ast-static | `d4f78c73a1ae4c01bb48334c1d386d140bfc1961ebb2efa6c93f4a60ce90e504` |
| `src/features/protocolStudio/components/OutputsColumn.tsx` | typescript-ast-static | `82428a26bc18011bdcdb23359625712d81aaef629ab58ad8b62b11cbcdd7b0dc` |
| `src/features/protocolStudio/components/ProtocolHelpModal.tsx` | typescript-ast-static | `b9431e8d09834cad952459b793f12ae8dd0d81b85e2579a0c0bf81ac8aec4f80` |
| `src/features/protocolStudio/components/ProtocolHero.tsx` | typescript-ast-static | `d41f849dbf26e274edc3381d265359595d5a80a8a80699059addb010f1efa9e5` |
| `src/features/protocolStudio/components/TranslatorColumn.tsx` | typescript-ast-static | `3c428324177274849b719f0e7da0d3196d276ea964d6e4d784372ce1089b8c7f` |
| `src/features/protocolStudio/ui.tsx` | typescript-ast-static | `ff8d70a3125693e3a56aebb931e089b76f191e23a8bd3bc22d91da47af2d7d83` |
| `src/features/protocolStudio/useProtocolStudioState.ts` | typescript-ast-static | `1ba944ef5d6a44c9112d45c2f9db993666348751d0b06e8031531cf3cfaca708` |
| `src/features/studio.css` | support-inventory | `d649c5e9294c0383a2fde7c04ab78561e49c870a075dc36f9a70111e6e2d9924` |
| `src/pages/ProtocolSection.tsx` | typescript-ast-static | `9a84347604c7d9bfd7696cecfbfdd76692cb51763ded7ed2965c1bea670869ba` |

## Tipos, puertos, props y callbacks

En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.

### `BuilderColumnProps`

`web:src/features/protocolStudio/components/BuilderColumn.tsx:12`

| Atributo / firma | Propósito |
| --- | --- |
| `source: string` | Dato de estado/configuración `source`; declaración estructural, interpretación específica pendiente de ampliar. |
| `destination: string` | Dato de estado/configuración `destination`; declaración estructural, interpretación específica pendiente de ampliar. |
| `commandKey: string` | Dato de estado/configuración `commandKey`; declaración estructural, interpretación específica pendiente de ampliar. |
| `currentCommand?: CommandDefinition` | Dato de estado/configuración `currentCommand`; declaración estructural, interpretación específica pendiente de ampliar. |
| `fieldValues: Record<string, string>` | Dato de estado/configuración `fieldValues`; declaración estructural, interpretación específica pendiente de ampliar. |
| `nonHexFields: CommandDefinition["fields"]` | Dato de estado/configuración `nonHexFields`; declaración estructural, interpretación específica pendiente de ampliar. |
| `hexFields: CommandDefinition["fields"]` | Dato de estado/configuración `hexFields`; declaración estructural, interpretación específica pendiente de ampliar. |
| `manualPayload: string` | Dato de estado/configuración `manualPayload`; declaración estructural, interpretación específica pendiente de ampliar. |
| `onSourceChange: (value: string) => void` | Operación on Source Change de Interfaz Protocol Studio; semántica fina pendiente de ampliación. |
| `onDestinationChange: (value: string) => void` | Operación on Destination Change de Interfaz Protocol Studio; semántica fina pendiente de ampliación. |
| `onCommandChange: (value: string) => void` | Operación on Command Change de Interfaz Protocol Studio; semántica fina pendiente de ampliación. |
| `onFieldValueChange: (id: string, value: string) => void` | Operación on Field Value Change de Interfaz Protocol Studio; semántica fina pendiente de ampliación. |
| `onManualPayloadChange: (value: string) => void` | Operación on Manual Payload Change de Interfaz Protocol Studio; semántica fina pendiente de ampliación. |

### `FrameVisualizationShowcaseProps`

`web:src/features/protocolStudio/components/FrameVisualizationShowcase.tsx:6`

| Atributo / firma | Propósito |
| --- | --- |
| `builderData: BuilderData \| null` | Dato de estado/configuración `builderData`; declaración estructural, interpretación específica pendiente de ampliar. |
| `builderError: string` | Dato de estado/configuración `builderError`; declaración estructural, interpretación específica pendiente de ampliar. |
| `commandKey: string` | Dato de estado/configuración `commandKey`; declaración estructural, interpretación específica pendiente de ampliar. |
| `currentCommand?: CommandDefinition` | Dato de estado/configuración `currentCommand`; declaración estructural, interpretación específica pendiente de ampliar. |

### `OutputsColumnProps`

`web:src/features/protocolStudio/components/OutputsColumn.tsx:6`

| Atributo / firma | Propósito |
| --- | --- |
| `builderData: BuilderData \| null` | Dato de estado/configuración `builderData`; declaración estructural, interpretación específica pendiente de ampliar. |
| `builderError: string` | Dato de estado/configuración `builderError`; declaración estructural, interpretación específica pendiente de ampliar. |
| `commandKey: string` | Dato de estado/configuración `commandKey`; declaración estructural, interpretación específica pendiente de ampliar. |
| `currentCommand?: CommandDefinition` | Dato de estado/configuración `currentCommand`; declaración estructural, interpretación específica pendiente de ampliar. |
| `outputs: { plain: string; realterm: string; c: string; py: string; }` | Dato de estado/configuración `outputs`; declaración estructural, interpretación específica pendiente de ampliar. |
| `flashMessage: string` | Dato de estado/configuración `flashMessage`; declaración estructural, interpretación específica pendiente de ampliar. |
| `onCopyOutput: (format: "plain" \| "realterm" \| "c" \| "py") => void` | Operación on Copy Output de Interfaz Protocol Studio; semántica fina pendiente de ampliación. |
| `onValidateInTranslator: () => void` | Operación on Validate In Translator de Interfaz Protocol Studio; semántica fina pendiente de ampliación. |

### `ProtocolHelpModalProps`

`web:src/features/protocolStudio/components/ProtocolHelpModal.tsx:3`

| Atributo / firma | Propósito |
| --- | --- |
| `isOpen: boolean` | Dato de estado/configuración `isOpen`; declaración estructural, interpretación específica pendiente de ampliar. |
| `onClose: () => void` | Operación on Close de Interfaz Protocol Studio; semántica fina pendiente de ampliación. |

### `ProtocolHeroProps`

`web:src/features/protocolStudio/components/ProtocolHero.tsx:1`

| Atributo / firma | Propósito |
| --- | --- |
| `activeSection: "builder" \| "translator"` | Dato de estado/configuración `activeSection`; declaración estructural, interpretación específica pendiente de ampliar. |
| `onSectionChange: (section: "builder" \| "translator") => void` | Operación on Section Change de Interfaz Protocol Studio; semántica fina pendiente de ampliación. |

### `StreamSegment`

`web:src/features/protocolStudio/components/TranslatorColumn.tsx:14`

| Atributo / firma | Propósito |
| --- | --- |
| `type: "gap" \| "invalid" \| "frame"` | Dato de estado/configuración `type`; declaración estructural, interpretación específica pendiente de ampliar. |
| `order: number` | Dato de estado/configuración `order`; declaración estructural, interpretación específica pendiente de ampliar. |
| `label: string` | Dato de estado/configuración `label`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bytes: number[]` | Dato de estado/configuración `bytes`; declaración estructural, interpretación específica pendiente de ampliar. |
| `tone: string` | Dato de estado/configuración `tone`; declaración estructural, interpretación específica pendiente de ampliar. |

### `TranslatorColumnProps`

`web:src/features/protocolStudio/components/TranslatorColumn.tsx:22`

| Atributo / firma | Propósito |
| --- | --- |
| `translatorInput: string` | Dato de estado/configuración `translatorInput`; declaración estructural, interpretación específica pendiente de ampliar. |
| `translation: TranslationResult` | Dato de estado/configuración `translation`; declaración estructural, interpretación específica pendiente de ampliar. |
| `scanResult: ScanBlockResult \| null` | Dato de estado/configuración `scanResult`; declaración estructural, interpretación específica pendiente de ampliar. |
| `translatorStatus: { tone: "ok" \| "warn" \| "bad"; label: string; summary: string; }` | Dato de estado/configuración `translatorStatus`; declaración estructural, interpretación específica pendiente de ampliar. |
| `scanResultsPlaceholder: string` | Dato de estado/configuración `scanResultsPlaceholder`; declaración estructural, interpretación específica pendiente de ampliar. |
| `streamPlaceholder: string` | Dato de estado/configuración `streamPlaceholder`; declaración estructural, interpretación específica pendiente de ampliar. |
| `streamSegments: StreamSegment[]` | Dato de estado/configuración `streamSegments`; declaración estructural, interpretación específica pendiente de ampliar. |
| `onTranslatorInputChange: (value: string) => void` | Operación on Translator Input Change de Interfaz Protocol Studio; semántica fina pendiente de ampliación. |
| `onTranslate: () => void` | Operación on Translate de Interfaz Protocol Studio; semántica fina pendiente de ampliación. |
| `onLoadBuilderToTranslator: () => void` | Operación on Load Builder To Translator de Interfaz Protocol Studio; semántica fina pendiente de ampliación. |
| `onResetTranslator: () => void` | Operación on Reset Translator de Interfaz Protocol Studio; semántica fina pendiente de ampliación. |

### `DetailSectionKey`

`web:src/features/protocolStudio/components/TranslatorColumn.tsx:40`

Tipo: `"analysis" \| "payloads" \| "validations" \| "references"`.

### `AnalysisDetailCardProps`

`web:src/features/protocolStudio/components/TranslatorColumn.tsx:453`

| Atributo / firma | Propósito |
| --- | --- |
| `analysis: TranslationResult` | Dato de estado/configuración `analysis`; declaración estructural, interpretación específica pendiente de ampliar. |
| `frame: number[]` | Dato de estado/configuración `frame`; declaración estructural, interpretación específica pendiente de ampliar. |
| `offset: number \| null` | Dato de estado/configuración `offset`; declaración estructural, interpretación específica pendiente de ampliar. |
| `openSections: Record<DetailSectionKey, boolean>` | Dato de estado/configuración `openSections`; declaración estructural, interpretación específica pendiente de ampliar. |
| `copiedKey: string` | Dato de estado/configuración `copiedKey`; declaración estructural, interpretación específica pendiente de ampliar. |
| `onToggleSection: (section: DetailSectionKey) => void` | Operación on Toggle Section de Interfaz Protocol Studio; semántica fina pendiente de ampliación. |
| `onClose: () => void` | Operación on Close de Interfaz Protocol Studio; semántica fina pendiente de ampliación. |
| `onCopy: (key: string, value: string) => void` | Operación on Copy de Interfaz Protocol Studio; semántica fina pendiente de ampliación. |

### `PanelProps`

`web:src/features/protocolStudio/ui.tsx:6`

| Atributo / firma | Propósito |
| --- | --- |
| `title: string` | Dato de estado/configuración `title`; declaración estructural, interpretación específica pendiente de ampliar. |
| `children: ReactNode` | Dato de estado/configuración `children`; declaración estructural, interpretación específica pendiente de ampliar. |
| `className?: string` | Dato de estado/configuración `className`; declaración estructural, interpretación específica pendiente de ampliar. |
| `headerRight?: ReactNode` | Dato de estado/configuración `headerRight`; declaración estructural, interpretación específica pendiente de ampliar. |

### `InfoCellProps`

`web:src/features/protocolStudio/ui.tsx:13`

| Atributo / firma | Propósito |
| --- | --- |
| `label: string` | Dato de estado/configuración `label`; declaración estructural, interpretación específica pendiente de ampliar. |
| `value: string` | Dato de estado/configuración `value`; declaración estructural, interpretación específica pendiente de ampliar. |
| `tone?: "cyan" \| "emerald" \| "violet" \| "rose"` | Dato de estado/configuración `tone`; declaración estructural, interpretación específica pendiente de ampliar. |
| `className?: string` | Dato de estado/configuración `className`; declaración estructural, interpretación específica pendiente de ampliar. |

### `DetailItemProps`

`web:src/features/protocolStudio/ui.tsx:20`

| Atributo / firma | Propósito |
| --- | --- |
| `label: string` | Dato de estado/configuración `label`; declaración estructural, interpretación específica pendiente de ampliar. |
| `value: string` | Dato de estado/configuración `value`; declaración estructural, interpretación específica pendiente de ampliar. |
| `className?: string` | Dato de estado/configuración `className`; declaración estructural, interpretación específica pendiente de ampliar. |

### `StatusPillProps`

`web:src/features/protocolStudio/ui.tsx:26`

| Atributo / firma | Propósito |
| --- | --- |
| `tone: "ok" \| "warn" \| "bad"` | Dato de estado/configuración `tone`; declaración estructural, interpretación específica pendiente de ampliar. |
| `children: ReactNode` | Dato de estado/configuración `children`; declaración estructural, interpretación específica pendiente de ampliar. |

### `StudioView`

`web:src/pages/ProtocolSection.tsx:14`

Tipo: `"protocol" \| "oled"`.


## Funciones públicas / componentes / hooks

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-analysisdetailcard"></a>`AnalysisDetailCard({  analysis,  frame,  offset,  openSections,  copiedKey,  onToggleSection,  onClose,  onCopy, }: AnalysisDetailCardProps): inferred-by-TypeScript` | Coordina `analysis.validations.map`, `analysis.validations.some`, `asciiPreview` para Interfaz Protocol Studio. | `web:src/features/protocolStudio/components/TranslatorColumn.tsx:464`; structural |
| <a id="fn-buildercolumn"></a>`BuilderColumn(props: BuilderColumnProps): inferred-by-TypeScript` | Construye er Column dentro de Interfaz Protocol Studio. | `web:src/features/protocolStudio/components/BuilderColumn.tsx:28`; inferred-name |
| <a id="fn-chevronicon"></a>`ChevronIcon({ isOpen }: { isOpen: boolean }): inferred-by-TypeScript` | Operación Chevron Icon de Interfaz Protocol Studio; semántica fina pendiente de ampliación. | `web:src/features/protocolStudio/components/TranslatorColumn.tsx:676`; inferred-name |
| <a id="fn-closeicon"></a>`CloseIcon(): inferred-by-TypeScript` | Operación Close Icon de Interfaz Protocol Studio; semántica fina pendiente de ampliación. | `web:src/features/protocolStudio/components/TranslatorColumn.tsx:703`; inferred-name |
| <a id="fn-collapsibledetailsection"></a>`CollapsibleDetailSection({  title,  isOpen,  onToggle,  children, }: { title: string; isOpen: boolean; onToggle: () => void; children: ReactNode; }): inferred-by-TypeScript` | Operación Collapsible Detail Section de Interfaz Protocol Studio; semántica fina pendiente de ampliación. | `web:src/features/protocolStudio/components/TranslatorColumn.tsx:602`; inferred-name |
| <a id="fn-coloredframe"></a>`ColoredFrame({ bytes }: { bytes: number[] }): inferred-by-TypeScript` | Adapta la llamada a `bytes.map` con el contexto de esta integración. | `web:src/features/protocolStudio/ui.tsx:71`; structural |
| <a id="fn-copyicon"></a>`CopyIcon(): inferred-by-TypeScript` | Operación Copy Icon de Interfaz Protocol Studio; semántica fina pendiente de ampliación. | `web:src/features/protocolStudio/components/TranslatorColumn.tsx:694`; inferred-name |
| <a id="fn-copypill"></a>`CopyPill({  copyKey,  value,  copiedKey,  onCopy,  label = "Copiar", }: { copyKey: string; value: string; copiedKey: string; onCopy: (key: string, value: string) => void; label?: string; }): inferred-by-TypeScript` | Operación Copy Pill de Interfaz Protocol Studio; semántica fina pendiente de ampliación. | `web:src/features/protocolStudio/components/TranslatorColumn.tsx:629`; inferred-name |
| <a id="fn-detailitem"></a>`DetailItem({ label, value, className = "" }: DetailItemProps): inferred-by-TypeScript` | Operación Detail Item de Interfaz Protocol Studio; semántica fina pendiente de ampliación. | `web:src/features/protocolStudio/ui.tsx:54`; inferred-name |
| <a id="fn-framevisualizationshowcase"></a>`FrameVisualizationShowcase({ builderData, builderError, commandKey, currentCommand }: FrameVisualizationShowcaseProps): inferred-by-TypeScript` | Coordina `String`, `String(payloadEnd - 1).padStart`, `String(payloadEnd).padStart` para Interfaz Protocol Studio. | `web:src/features/protocolStudio/components/FrameVisualizationShowcase.tsx:13`; structural |
| <a id="fn-infocell"></a>`InfoCell({ label, value, tone = "cyan", className = "" }: InfoCellProps): inferred-by-TypeScript` | Operación Info Cell de Interfaz Protocol Studio; semántica fina pendiente de ampliación. | `web:src/features/protocolStudio/ui.tsx:45`; inferred-name |
| <a id="fn-outputscolumn"></a>`OutputsColumn(props: OutputsColumnProps): inferred-by-TypeScript` | Adapta la llamada a `([ { key: "plain", label: "Hexadecimal" }, { key: "realterm", label: "RealTerm" }, { key: "c", label: "C / Arduino" }, { key: "py", label: "Python" }, ] as const).map` con el contexto de esta integración. | `web:src/features/protocolStudio/components/OutputsColumn.tsx:22`; structural |
| <a id="fn-panel"></a>`Panel({ title, children, className = "", headerRight }: PanelProps): inferred-by-TypeScript` | Operación Panel de Interfaz Protocol Studio; semántica fina pendiente de ampliación. | `web:src/features/protocolStudio/ui.tsx:31`; inferred-name |
| <a id="fn-payloadvalue"></a>`PayloadValue({  label,  value,  copyValue,  copyKey,  copiedKey,  onCopy, }: { label: string; value: string; copyValue: string; copyKey: string; copiedKey: string; onCopy: (key: string, value: string) => void; }): inferred-by-TypeScript` | Operación Payload Value de Interfaz Protocol Studio; semántica fina pendiente de ampliación. | `web:src/features/protocolStudio/components/TranslatorColumn.tsx:574`; inferred-name |
| <a id="fn-protocolhelpmodal"></a>`ProtocolHelpModal({ isOpen, onClose }: ProtocolHelpModalProps): inferred-by-TypeScript` | Operación Protocol Help Modal de Interfaz Protocol Studio; semántica fina pendiente de ampliación. | `web:src/features/protocolStudio/components/ProtocolHelpModal.tsx:8`; inferred-name |
| <a id="fn-protocolhero"></a>`ProtocolHero({ activeSection, onSectionChange }: ProtocolHeroProps): inferred-by-TypeScript` | Operación Protocol Hero de Interfaz Protocol Studio; semántica fina pendiente de ampliación. | `web:src/features/protocolStudio/components/ProtocolHero.tsx:5`; inferred-name |
| <a id="fn-protocolsection"></a>`ProtocolSection(): inferred-by-TypeScript` | Coordina `searchParams.get`, `segmentedButtonStyle`, `useProtocolStudioState` para Interfaz Protocol Studio. | `web:src/pages/ProtocolSection.tsx:19`; structural |
| <a id="fn-statuspill"></a>`StatusPill({ tone, children }: StatusPillProps): inferred-by-TypeScript` | Operación Status Pill de Interfaz Protocol Studio; semántica fina pendiente de ampliación. | `web:src/features/protocolStudio/ui.tsx:63`; inferred-name |
| <a id="fn-streamsegmentbody"></a>`StreamSegmentBody({ segment }: { segment: StreamSegment }): inferred-by-TypeScript` | Adapta la llamada a `segment.bytes.map` con el contexto de esta integración. | `web:src/features/protocolStudio/components/TranslatorColumn.tsx:435`; structural |
| <a id="fn-summarymetric"></a>`SummaryMetric({ label, value, tone }: { label: string; value: number; tone: "emerald" \| "amber" \| "rose" }): inferred-by-TypeScript` | Operación Summary Metric de Interfaz Protocol Studio; semántica fina pendiente de ampliación. | `web:src/features/protocolStudio/components/TranslatorColumn.tsx:657`; inferred-name |
| <a id="fn-translatorcolumn"></a>`TranslatorColumn(props: TranslatorColumnProps): inferred-by-TypeScript` | Coordina `scanResult.gaps.map`, `scanResult.invalidCandidates.map`, `scanResult.validFrames.map` para Interfaz Protocol Studio. | `web:src/features/protocolStudio/components/TranslatorColumn.tsx:49`; structural |
| <a id="fn-validationrow"></a>`ValidationRow({ item }: { item: ValidationItem }): inferred-by-TypeScript` | Operación Validation Row de Interfaz Protocol Studio; semántica fina pendiente de ampliación. | `web:src/features/protocolStudio/ui.tsx:85`; inferred-name |
| <a id="fn-getpayloadbytes"></a>`getPayloadBytes(frame: number[]): inferred-by-TypeScript` | Adapta la llamada a `frame.slice` con el contexto de esta integración. | `web:src/features/protocolStudio/components/TranslatorColumn.tsx:669`; structural |
| <a id="fn-istranslationidle"></a>`isTranslationIdle(translation: TranslationResult): inferred-by-TypeScript` | Operación is Translation Idle de Interfaz Protocol Studio; semántica fina pendiente de ampliación. | `web:src/features/protocolStudio/components/TranslatorColumn.tsx:431`; inferred-name |
| <a id="fn-segmentedbuttonstyle"></a>`segmentedButtonStyle(active: boolean): inferred-by-TypeScript` | Operación segmented Button Style de Interfaz Protocol Studio; semántica fina pendiente de ampliación. | `web:src/pages/ProtocolSection.tsx:207`; inferred-name |
| <a id="fn-useprotocolstudiostate"></a>`useProtocolStudioState(): inferred-by-TypeScript` | Coordina `createIdleTranslation`, `currentCommand?.fields.filter`, `useEffect` para Interfaz Protocol Studio. | `web:src/features/protocolStudio/useProtocolStudioState.ts:20`; structural |

## Funciones y callbacks internos

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-coloredframe.role"></a>`ColoredFrame.role(index: number): inferred-by-TypeScript` | Operación role de Interfaz Protocol Studio; semántica fina pendiente de ampliación. | `web:src/features/protocolStudio/ui.tsx:73`; inferred-name |
| <a id="fn-protocolsection.handlestudiochange"></a>`ProtocolSection.handleStudioChange(nextStudio: StudioView): inferred-by-TypeScript` | Coordina `nextSearchParams.delete`, `nextSearchParams.set`, `setSearchParams` para Interfaz Protocol Studio. | `web:src/pages/ProtocolSection.tsx:34`; structural |
| <a id="fn-protocolsection.handlevalidateintranslator"></a>`ProtocolSection.handleValidateInTranslator(): inferred-by-TypeScript` | Coordina `setActiveSection`, `studio.loadBuilderToTranslator` para Interfaz Protocol Studio. | `web:src/pages/ProtocolSection.tsx:29`; structural |
| <a id="fn-translatorcolumn.copytext"></a>`TranslatorColumn.copyText(key: string, value: string): inferred-by-TypeScript` | Coordina `document.body.appendChild`, `document.body.removeChild`, `document.createElement` para Interfaz Protocol Studio. | `web:src/features/protocolStudio/components/TranslatorColumn.tsx:134`; structural |
| <a id="fn-translatorcolumn.handleresettranslator"></a>`TranslatorColumn.handleResetTranslator(): inferred-by-TypeScript` | Coordina `onResetTranslator`, `setSelectedFrameOffset`, `setShowAnalysisCard` para Interfaz Protocol Studio. | `web:src/features/protocolStudio/components/TranslatorColumn.tsx:165`; structural |
| <a id="fn-translatorcolumn.selectframe"></a>`TranslatorColumn.selectFrame(frame: ValidFrameItem): inferred-by-TypeScript` | Coordina `setSelectedFrameOffset`, `setShowAnalysisCard` para Interfaz Protocol Studio. | `web:src/features/protocolStudio/components/TranslatorColumn.tsx:129`; structural |
| <a id="fn-translatorcolumn.toggledetailsection"></a>`TranslatorColumn.toggleDetailSection(section: DetailSectionKey): inferred-by-TypeScript` | Adapta la llamada a `setOpenSections` con el contexto de esta integración. | `web:src/features/protocolStudio/components/TranslatorColumn.tsx:122`; structural |
| <a id="fn-useprotocolstudiostate.copyoutput"></a>`useProtocolStudioState.copyOutput(format: keyof typeof outputs): inferred-by-TypeScript` | Coordina `document.body.appendChild`, `document.body.removeChild`, `document.createElement` para Interfaz Protocol Studio. | `web:src/features/protocolStudio/useProtocolStudioState.ts:178`; structural |
| <a id="fn-useprotocolstudiostate.handletranslate"></a>`useProtocolStudioState.handleTranslate(rawValue: inferred-by-TypeScript): inferred-by-TypeScript` | Coordina `analyzeCommandOnly`, `analyzeFrame`, `countHeaders` para Interfaz Protocol Studio. | `web:src/features/protocolStudio/useProtocolStudioState.ts:205`; structural |
| <a id="fn-useprotocolstudiostate.loadbuildertotranslator"></a>`useProtocolStudioState.loadBuilderToTranslator(): inferred-by-TypeScript` | Coordina `handleTranslate`, `setTranslatorInput` para Interfaz Protocol Studio. | `web:src/features/protocolStudio/useProtocolStudioState.ts:242`; structural |
| <a id="fn-useprotocolstudiostate.resettranslator"></a>`useProtocolStudioState.resetTranslator(): inferred-by-TypeScript` | Coordina `createIdleTranslation`, `setScanResult`, `setTranslation` para Interfaz Protocol Studio. | `web:src/features/protocolStudio/useProtocolStudioState.ts:198`; structural |
| <a id="fn-useprotocolstudiostate.setfieldvalue"></a>`useProtocolStudioState.setFieldValue(id: string, value: string): inferred-by-TypeScript` | Adapta la llamada a `setFieldValues` con el contexto de esta integración. | `web:src/features/protocolStudio/useProtocolStudioState.ts:164`; structural |
| <a id="fn-useprotocolstudiostate.showflash"></a>`useProtocolStudioState.showFlash(text: string): inferred-by-TypeScript` | Coordina `setFlashMessage`, `window.clearTimeout`, `window.setTimeout` para Interfaz Protocol Studio. | `web:src/features/protocolStudio/useProtocolStudioState.ts:168`; structural |

## Estado y efectos React encontrados

| Hook | Función/closure | Evidencia |
| --- | --- | --- |
| `useState` | `TranslatorColumn` | `web:src/features/protocolStudio/components/TranslatorColumn.tsx:64` |
| `useState` | `TranslatorColumn` | `web:src/features/protocolStudio/components/TranslatorColumn.tsx:65` |
| `useState` | `TranslatorColumn` | `web:src/features/protocolStudio/components/TranslatorColumn.tsx:66` |
| `useState` | `TranslatorColumn` | `web:src/features/protocolStudio/components/TranslatorColumn.tsx:67` |
| `useState` | `TranslatorColumn` | `web:src/features/protocolStudio/components/TranslatorColumn.tsx:68` |
| `useRef` | `TranslatorColumn` | `web:src/features/protocolStudio/components/TranslatorColumn.tsx:69` |
| `useMemo` | `TranslatorColumn` | `web:src/features/protocolStudio/components/TranslatorColumn.tsx:71` |
| `useEffect` | `TranslatorColumn` | `web:src/features/protocolStudio/components/TranslatorColumn.tsx:82` |
| `useEffect` | `TranslatorColumn` | `web:src/features/protocolStudio/components/TranslatorColumn.tsx:86` |
| `useEffect` | `TranslatorColumn` | `web:src/features/protocolStudio/components/TranslatorColumn.tsx:109` |
| `useEffect` | `TranslatorColumn` | `web:src/features/protocolStudio/components/TranslatorColumn.tsx:114` |
| `useState` | `useProtocolStudioState` | `web:src/features/protocolStudio/useProtocolStudioState.ts:21` |
| `useState` | `useProtocolStudioState` | `web:src/features/protocolStudio/useProtocolStudioState.ts:22` |
| `useState` | `useProtocolStudioState` | `web:src/features/protocolStudio/useProtocolStudioState.ts:23` |
| `useState` | `useProtocolStudioState` | `web:src/features/protocolStudio/useProtocolStudioState.ts:24` |
| `useState` | `useProtocolStudioState` | `web:src/features/protocolStudio/useProtocolStudioState.ts:25` |
| `useState` | `useProtocolStudioState` | `web:src/features/protocolStudio/useProtocolStudioState.ts:26` |
| `useState` | `useProtocolStudioState` | `web:src/features/protocolStudio/useProtocolStudioState.ts:27` |
| `useState` | `useProtocolStudioState` | `web:src/features/protocolStudio/useProtocolStudioState.ts:28` |
| `useState` | `useProtocolStudioState` | `web:src/features/protocolStudio/useProtocolStudioState.ts:29` |
| `useState` | `useProtocolStudioState` | `web:src/features/protocolStudio/useProtocolStudioState.ts:30` |
| `useState` | `useProtocolStudioState` | `web:src/features/protocolStudio/useProtocolStudioState.ts:31` |
| `useRef` | `useProtocolStudioState` | `web:src/features/protocolStudio/useProtocolStudioState.ts:32` |
| `useEffect` | `useProtocolStudioState` | `web:src/features/protocolStudio/useProtocolStudioState.ts:38` |
| `useEffect` | `useProtocolStudioState` | `web:src/features/protocolStudio/useProtocolStudioState.ts:43` |
| `useMemo` | `useProtocolStudioState` | `web:src/features/protocolStudio/useProtocolStudioState.ts:51` |
| `useMemo` | `useProtocolStudioState` | `web:src/features/protocolStudio/useProtocolStudioState.ts:65` |
| `useMemo` | `useProtocolStudioState` | `web:src/features/protocolStudio/useProtocolStudioState.ts:79` |
| `useMemo` | `useProtocolStudioState` | `web:src/features/protocolStudio/useProtocolStudioState.ts:128` |
| `useProtocolStudioState` | `ProtocolSection` | `web:src/pages/ProtocolSection.tsx:20` |
| `useSearchParams` | `ProtocolSection` | `web:src/pages/ProtocolSection.tsx:21` |
| `useState` | `ProtocolSection` | `web:src/pages/ProtocolSection.tsx:22` |
| `useState` | `ProtocolSection` | `web:src/pages/ProtocolSection.tsx:23` |
