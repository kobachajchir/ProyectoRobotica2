# Layout y navegación de secciones

ID: `web:module/src/components/RootLayout.tsx` · Capa: **integración** · Fotografía: 2026-10-20.

Documentación de fuente; sin tests, compilación, navegador ni validación física.

## Responsabilidad y alcance

Presenta estructura y navegación común de las secciones del cliente.

Rutas, identidad visual y contenido general; no define contratos UNER.

<a id="modularity"></a>
## Modularidad observada

UI React/router; usa componentes compartidos y estado externo mediante hooks.

<a id="state"></a>
## Handle, estado y atributos externos

Estado visual local, navegación y props; el dispositivo confirmado pertenece a contextos de dominio.

<a id="lifecycle"></a>
## Ciclo de vida y contexto

- La ruta activa monta contenido dentro del layout.
- Efectos de bienvenida/navegación no deben recrear la sesión de dispositivo por accidente.

<a id="interactions"></a>
## Interacciones

- Las páginas enlazan Docs, control, estado, Wi-Fi y estudios.

<a id="limitations"></a>
## Precauciones y límites

- Ruta visible y dispositivo conectado/autorizado son condiciones diferentes.

## Hallazgos conectados


## Archivos y evidencia

| Archivo | Análisis | Hash SHA-256 |
| --- | --- | --- |
| `src/components/AboutProjectPanel.tsx` | typescript-ast-static | `88b1224d7683594637afadd38bc5365e474855027924708ebc94a5846c134c34` |
| `src/components/AppFooter.tsx` | typescript-ast-static | `8281ce3148cbb93426b34b10ee0e6ed6616fbc220590b6b5573c68249a9598ad` |
| `src/components/BrandMark.tsx` | typescript-ast-static | `d1978ec5278a8b075c6942e7e9d515e8bc67690225572d2b7814387feb273687` |
| `src/components/NavigateVT.tsx` | typescript-ast-static | `31a023c7b68c4c517673fc2d43349ee37cd0145e91c8955d075499ed40e44ae2` |
| `src/components/PageHeader.tsx` | typescript-ast-static | `ff22478a797278aee516d3c99f6c50cbc4c5e2ec8377dcbc88825b5967144da2` |
| `src/components/RedirectRoot.tsx` | typescript-ast-static | `14fee8127cd9c9c831a967c7ec647d25cb01625ed55175266ea190c3e808c3f9` |
| `src/components/RootLayout.tsx` | typescript-ast-static | `5ea671e5ea083e3d9c717c7a41c7ebef910085ffb211a57e2060e4efba735e98` |
| `src/components/StartupWelcome.css` | support-inventory | `6ab0dcb4f1ab0380ede53231dbc50c1450500113c3c760c069be25a8b8881f61` |
| `src/components/StartupWelcome.tsx` | typescript-ast-static | `ae78c3f9841e0534ac17642657635e2e4829493b6eafa2e50a28ca5a434ade87` |
| `src/pages/Home.tsx` | typescript-ast-static | `ff4620eca52e844974245db9d01af088ce49f8f3a873a38074e7c6cbfe6dd5bb` |
| `src/pages/NotFound.css` | support-inventory | `247e4b584f5d5588163bc04540a3ccb0ef2b6f626e0ebab38fef5723e7d524f6` |
| `src/pages/NotFound.tsx` | typescript-ast-static | `b9b5f61feb3406bca9986f8ca9b4df209df43014939152ea93e058ec4baf8dcd` |

## Tipos, puertos, props y callbacks

En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.

### `FirmwareInfo`

`web:src/components/AboutProjectPanel.tsx:17`

| Atributo / firma | Propósito |
| --- | --- |
| `name: string` | Dato de estado/configuración `name`; declaración estructural, interpretación específica pendiente de ampliar. |
| `version: string` | Dato de estado/configuración `version`; declaración estructural, interpretación específica pendiente de ampliar. |
| `target: string` | Dato de estado/configuración `target`; declaración estructural, interpretación específica pendiente de ampliar. |

### `BuildInfo`

`web:src/components/AboutProjectPanel.tsx:23`

| Atributo / firma | Propósito |
| --- | --- |
| `date: string` | Dato de estado/configuración `date`; declaración estructural, interpretación específica pendiente de ampliar. |
| `time: string` | Dato de estado/configuración `time`; declaración estructural, interpretación específica pendiente de ampliar. |
| `profile: string` | Dato de estado/configuración `profile`; declaración estructural, interpretación específica pendiente de ampliar. |
| `compiler: string` | Dato de estado/configuración `compiler`; declaración estructural, interpretación específica pendiente de ampliar. |
| `arduinoCore?: string` | Dato de estado/configuración `arduinoCore`; declaración estructural, interpretación específica pendiente de ampliar. |
| `sdk?: string` | Dato de estado/configuración `sdk`; declaración estructural, interpretación específica pendiente de ampliar. |

### `EspRuntimeInfo`

`web:src/components/AboutProjectPanel.tsx:32`

| Atributo / firma | Propósito |
| --- | --- |
| `freeHeap: string` | Dato de estado/configuración `freeHeap`; declaración estructural, interpretación específica pendiente de ampliar. |
| `maxFreeBlock: string` | Dato de estado/configuración `maxFreeBlock`; declaración estructural, interpretación específica pendiente de ampliar. |
| `heapFragmentation: string` | Dato de estado/configuración `heapFragmentation`; declaración estructural, interpretación específica pendiente de ampliar. |

### `EspDeviceInfo`

`web:src/components/AboutProjectPanel.tsx:38`

| Atributo / firma | Propósito |
| --- | --- |
| `firmware: FirmwareInfo` | Dato de estado/configuración `firmware`; declaración estructural, interpretación específica pendiente de ampliar. |
| `build: BuildInfo` | Dato de estado/configuración `build`; declaración estructural, interpretación específica pendiente de ampliar. |
| `runtime: EspRuntimeInfo` | Dato de estado/configuración `runtime`; declaración estructural, interpretación específica pendiente de ampliar. |

### `LoadState`

`web:src/components/AboutProjectPanel.tsx:44`

Tipo: `"idle" \| "loading" \| "success" \| "partial" \| "error"`.

### `NavigateVTProps`

`web:src/components/NavigateVT.tsx:6`

| Atributo / firma | Propósito |
| --- | --- |
| `to: To` | Dato de estado/configuración `to`; declaración estructural, interpretación específica pendiente de ampliar. |
| `replace?: boolean` | Dato de estado/configuración `replace`; declaración estructural, interpretación específica pendiente de ampliar. |
| `state?: any` | Dato de estado/configuración `state`; declaración estructural, interpretación específica pendiente de ampliar. |

### `PageHeaderProps`

`web:src/components/PageHeader.tsx:8`

| Atributo / firma | Propósito |
| --- | --- |
| `setOpenSettingsModal?: (open: boolean) => void` | Operación set Open Settings Modal de Layout y navegación de secciones; semántica fina pendiente de ampliación. |
| `setOpenInfoModal?: (open: boolean) => void` | Operación set Open Info Modal de Layout y navegación de secciones; semántica fina pendiente de ampliación. |
| `showSettings?: boolean` | Dato de estado/configuración `showSettings`; declaración estructural, interpretación específica pendiente de ampliar. |
| `showInfo?: boolean` | Dato de estado/configuración `showInfo`; declaración estructural, interpretación específica pendiente de ampliar. |
| `showLogout?: boolean` | Dato de estado/configuración `showLogout`; declaración estructural, interpretación específica pendiente de ampliar. |
| `showHome?: boolean` | Dato de estado/configuración `showHome`; declaración estructural, interpretación específica pendiente de ampliar. |
| `className?: string` | Dato de estado/configuración `className`; declaración estructural, interpretación específica pendiente de ampliar. |
| `titleOverride?: string` | Dato de estado/configuración `titleOverride`; declaración estructural, interpretación específica pendiente de ampliar. |
| `statusSlot?: ReactNode` | Dato de estado/configuración `statusSlot`; declaración estructural, interpretación específica pendiente de ampliar. |

### `RequestStatus`

`web:src/pages/Home.tsx:32`

| Atributo / firma | Propósito |
| --- | --- |
| `tone: "idle" \| "loading" \| "success" \| "error"` | Dato de estado/configuración `tone`; declaración estructural, interpretación específica pendiente de ampliar. |
| `message: string` | Dato de estado/configuración `message`; declaración estructural, interpretación específica pendiente de ampliar. |

### `PinStep`

`web:src/pages/Home.tsx:37`

Tipo: `"validate" \| "change"`.

### `SettingsTab`

`web:src/pages/Home.tsx:38`

Tipo: `"settings" \| "about"`.

### `QtUsbDiagnostics`

`web:src/pages/Home.tsx:40`

| Atributo / firma | Propósito |
| --- | --- |
| `connectedSince: number \| null` | Dato de estado/configuración `connectedSince`; declaración estructural, interpretación específica pendiente de ampliar. |
| `connectedSinceEstimated: boolean` | Dato de estado/configuración `connectedSinceEstimated`; declaración estructural, interpretación específica pendiente de ampliar. |
| `alivePeriodMs: number \| null` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `aliveCount: number \| null` | Dato de estado/configuración `aliveCount`; declaración estructural, interpretación específica pendiente de ampliar. |
| `rxOverflowCount: number \| null` | Dato de estado/configuración `rxOverflowCount`; declaración estructural, interpretación específica pendiente de ampliar. |
| `txQueueFullCount: number \| null` | Dato de estado/configuración `txQueueFullCount`; declaración estructural, interpretación específica pendiente de ampliar. |
| `queuedTxCount: number \| null` | Dato de estado/configuración `queuedTxCount`; declaración estructural, interpretación específica pendiente de ampliar. |
| `txDropCount: number \| null` | Dato de estado/configuración `txDropCount`; declaración estructural, interpretación específica pendiente de ampliar. |
| `updatedAt: number \| null` | Dato de estado/configuración `updatedAt`; declaración estructural, interpretación específica pendiente de ampliar. |

### `HomeTone`

`web:src/pages/Home.tsx:1263`

Tipo: `"ok" \| "error" \| "info" \| "muted"`.

### `HomeIconName`

`web:src/pages/Home.tsx:1264`

Tipo: `\| "oled" \| "pulse" \| "clock" \| "eye" \| "eyeOff" \| "cube" \| "signal" \| "chip" \| "gamepad" \| "track" \| "wifi"`.


## Funciones públicas / componentes / hooks

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-aboutchevron"></a>`AboutChevron({ open }: { open: boolean }): inferred-by-TypeScript` | Operación About Chevron de Layout y navegación de secciones; semántica fina pendiente de ampliación. | `web:src/components/AboutProjectPanel.tsx:403`; inferred-name |
| <a id="fn-aboutdatacard"></a>`AboutDataCard({ title, eyebrow, children }: { title: string; eyebrow: string; children: React.ReactNode }): inferred-by-TypeScript` | Operación About Data Card de Layout y navegación de secciones; semántica fina pendiente de ampliación. | `web:src/components/AboutProjectPanel.tsx:352`; inferred-name |
| <a id="fn-aboutdatarow"></a>`AboutDataRow({ label, value }: { label: string; value: string }): inferred-by-TypeScript` | Operación About Data Row de Layout y navegación de secciones; semántica fina pendiente de ampliación. | `web:src/components/AboutProjectPanel.tsx:366`; inferred-name |
| <a id="fn-aboutdatasection"></a>`AboutDataSection({ title }: { title: string }): inferred-by-TypeScript` | Operación About Data Section de Layout y navegación de secciones; semántica fina pendiente de ampliación. | `web:src/components/AboutProjectPanel.tsx:362`; inferred-name |
| <a id="fn-aboutphotoplaceholder"></a>`AboutPhotoPlaceholder({ title }: { title: string }): inferred-by-TypeScript` | Operación About Photo Placeholder de Layout y navegación de secciones; semántica fina pendiente de ampliación. | `web:src/components/AboutProjectPanel.tsx:375`; inferred-name |
| <a id="fn-aboutprojectpanel"></a>`AboutProjectPanel(): inferred-by-TypeScript` | Coordina `REPOSITORIES.map`, `formatBuildDate`, `formatBytes` para Layout y navegación de secciones. | `web:src/components/AboutProjectPanel.tsx:72`; structural |
| <a id="fn-appfooter"></a>`AppFooter(): inferred-by-TypeScript` | Operación App Footer de Layout y navegación de secciones; semántica fina pendiente de ampliación. | `web:src/components/AppFooter.tsx:2`; inferred-name |
| <a id="fn-brandmark"></a>`BrandMark({ className = "" }: { className?: string }): inferred-by-TypeScript` | Adapta la llamada a ``home-brand-glyph app-brand-mark ${className}`.trim` con el contexto de esta integración. | `web:src/components/BrandMark.tsx:3`; structural |
| <a id="fn-chevrondownicon"></a>`ChevronDownIcon({ open = false }: { open?: boolean }): inferred-by-TypeScript` | Operación Chevron Down Icon de Layout y navegación de secciones; semántica fina pendiente de ampliación. | `web:src/pages/Home.tsx:1513`; inferred-name |
| <a id="fn-chevronrighticon"></a>`ChevronRightIcon(): inferred-by-TypeScript` | Operación Chevron Right Icon de Layout y navegación de secciones; semántica fina pendiente de ampliación. | `web:src/pages/Home.tsx:1505`; inferred-name |
| <a id="fn-devmodetoggle"></a>`DevModeToggle(): inferred-by-TypeScript` | Adapta la llamada a `useUser` con el contexto de esta integración. | `web:src/pages/Home.tsx:58`; structural |
| <a id="fn-home"></a>`Home(): inferred-by-TypeScript` | Coordina `/^\d{4}$/.test`, `Math.max`, `formatCarMode` para Layout y navegación de secciones. | `web:src/pages/Home.tsx:76`; structural |
| <a id="fn-homedropdownbutton"></a>`HomeDropdownButton({  label,  open,  ariaLabel,  disabled = false,  onClick, }: { label: string; open: boolean; ariaLabel: string; disabled?: boolean; onClick: () => void; }): inferred-by-TypeScript` | Operación Home Dropdown Button de Layout y navegación de secciones; semántica fina pendiente de ampliación. | `web:src/pages/Home.tsx:1303`; inferred-name |
| <a id="fn-homeicon"></a>`HomeIcon({ name }: { name: HomeIconName }): inferred-by-TypeScript` | Operación Home Icon de Layout y navegación de secciones; semántica fina pendiente de ampliación. | `web:src/pages/Home.tsx:1525`; inferred-name |
| <a id="fn-homesystemstatusstrip"></a>`HomeSystemStatusStrip({  connected,  connectionLabel,  connectionTone,  systemStateLabel,  networkLabel,  qtUsbConnected,  qtUsbStateKnown,  qtUsbPortForwardingLabel,  qtUsbPortForwardingTone,  onOpenQtDiagnostics, }: { connected: boolean; connectionLabel: string; connectionTone: HomeTone; systemStateLabel: string; networkLabel: string; qtUsbConnected: boolean; qtUsbStateKnown: boolean; qtUsbPortForwardingLabel: string; qtUsbPortForwardingTone: HomeTone; onOpenQtDiagnostics: () => void; }): inferred-by-TypeScript` | Operación Home System Status Strip de Layout y navegación de secciones; semántica fina pendiente de ampliación. | `web:src/pages/Home.tsx:1384`; inferred-name |
| <a id="fn-navigatevt"></a>`NavigateVT({ to, replace = false, state }: NavigateVTProps): inferred-by-TypeScript` | Coordina `useEffect`, `useNavigate` para Layout y navegación de secciones. | `web:src/components/NavigateVT.tsx:12`; structural |
| <a id="fn-navigationmodulecard"></a>`NavigationModuleCard({  title,  backgroundImage,  eyebrow,  description,  status,  statusTone,  metaLeft,  metaRight,  icon,  primary = false,  disabled = false,  onClick, }: { title: string; backgroundImage: string; eyebrow?: string; description: string; status: string; statusTone: HomeTone; metaLeft: string; metaRight: string; icon: HomeIconName; primary?: boolean; disabled?: boolean; onClick: () => void; }): inferred-by-TypeScript` | Coordina `metaRight.toLowerCase`, `metaRight.toLowerCase().includes` para Layout y navegación de secciones. | `web:src/pages/Home.tsx:1431`; structural |
| <a id="fn-notfound"></a>`NotFound(): inferred-by-TypeScript` | Operación Not Found de Layout y navegación de secciones; semántica fina pendiente de ampliación. | `web:src/pages/NotFound.tsx:5`; inferred-name |
| <a id="fn-pageheader"></a>`PageHeader({  setOpenSettingsModal,  setOpenInfoModal,  showSettings = true,  showInfo = true,  showLogout = true,  showHome = true,  className = "app-page-header",  titleOverride = "",  statusSlot, }: PageHeaderProps): inferred-by-TypeScript` | Coordina `getPageTitle`, `location.pathname.includes`, `location.pathname.startsWith` para Layout y navegación de secciones. | `web:src/components/PageHeader.tsx:33`; structural |
| <a id="fn-photoicon"></a>`PhotoIcon(): inferred-by-TypeScript` | Operación Photo Icon de Layout y navegación de secciones; semántica fina pendiente de ampliación. | `web:src/components/AboutProjectPanel.tsx:411`; inferred-name |
| <a id="fn-pindigitfield"></a>`PinDigitField({  label,  value,  onChange,  autoFocus = false, }: { label: string; value: string; onChange: (value: string) => void; autoFocus?: boolean; }): inferred-by-TypeScript` | Coordina `Array.from`, `Array.from({ length: 4 }).map`, `useEffect` para Layout y navegación de secciones. | `web:src/pages/Home.tsx:1636`; structural |
| <a id="fn-qrplaceholdericon"></a>`QrPlaceholderIcon(): inferred-by-TypeScript` | Operación Qr Placeholder Icon de Layout y navegación de secciones; semántica fina pendiente de ampliación. | `web:src/components/AboutProjectPanel.tsx:421`; inferred-name |
| <a id="fn-qtdiagnosticrow"></a>`QtDiagnosticRow({ label, value }: { label: string; value: string }): inferred-by-TypeScript` | Operación Qt Diagnostic Row de Layout y navegación de secciones; semántica fina pendiente de ampliación. | `web:src/pages/Home.tsx:1375`; inferred-name |
| <a id="fn-redirectroot"></a>`RedirectRoot(): inferred-by-TypeScript` | Adapta la llamada a `useUser` con el contexto de esta integración. | `web:src/components/RedirectRoot.tsx:5`; structural |
| <a id="fn-repositoryarrow"></a>`RepositoryArrow(): inferred-by-TypeScript` | Operación Repository Arrow de Layout y navegación de secciones; semántica fina pendiente de ampliación. | `web:src/components/AboutProjectPanel.tsx:429`; inferred-name |
| <a id="fn-repositoryqr"></a>`RepositoryQr({ title }: { title: string }): inferred-by-TypeScript` | Operación Repository Qr de Layout y navegación de secciones; semántica fina pendiente de ampliación. | `web:src/components/AboutProjectPanel.tsx:385`; inferred-name |
| <a id="fn-repositoryqrplaceholder"></a>`RepositoryQrPlaceholder({ title }: { title: string }): inferred-by-TypeScript` | Operación Repository Qr Placeholder de Layout y navegación de secciones; semántica fina pendiente de ampliación. | `web:src/components/AboutProjectPanel.tsx:394`; inferred-name |
| <a id="fn-rootlayout"></a>`RootLayout(): inferred-by-TypeScript` | Coordina `useLocation`, `useOutlet`, `useReducedMotion` para Layout y navegación de secciones. | `web:src/components/RootLayout.tsx:6`; structural |
| <a id="fn-startupwelcome"></a>`StartupWelcome({ children }: PropsWithChildren): inferred-by-TypeScript` | Inicia up Welcome dentro de Layout y navegación de secciones. | `web:src/components/StartupWelcome.tsx:7`; inferred-name |
| <a id="fn-statusnote"></a>`StatusNote({ status }: { status: RequestStatus }): inferred-by-TypeScript` | Operación Status Note de Layout y navegación de secciones; semántica fina pendiente de ampliación. | `web:src/pages/Home.tsx:1619`; inferred-name |
| <a id="fn-systemmetricrow"></a>`SystemMetricRow({  label,  value,  tone = "muted",  onClick,  ariaLabel, }: { label: string; value: string; tone?: HomeTone; onClick?: () => void; ariaLabel?: string; }): inferred-by-TypeScript` | Operación System Metric Row de Layout y navegación de secciones; semántica fina pendiente de ampliación. | `web:src/pages/Home.tsx:1331`; inferred-name |
| <a id="fn-assertokstatus"></a>`assertOkStatus(data: Record<string, unknown>, label: string): inferred-by-TypeScript` | Operación assert Ok Status de Layout y navegación de secciones; semántica fina pendiente de ampliación. | `web:src/components/AboutProjectPanel.tsx:509`; inferred-name |
| <a id="fn-buildqrpath"></a>`buildQrPath(): string` | Coordina `Math.floor`, `commands.join`, `commands.push` para Layout y navegación de secciones. | `web:src/components/AboutProjectPanel.tsx:525`; structural |
| <a id="fn-createemptyqtusbdiagnostics"></a>`createEmptyQtUsbDiagnostics(): QtUsbDiagnostics` | Operación create Empty Qt Usb Diagnostics de Layout y navegación de secciones; semántica fina pendiente de ampliación. | `web:src/pages/Home.tsx:1225`; inferred-name |
| <a id="fn-estimateqtconnectionstart"></a>`estimateQtConnectionStart(now: number, alivePeriodMs: number \| null, aliveCount: number \| null): inferred-by-TypeScript` | Adapta la llamada a `Math.max` con el contexto de esta integración. | `web:src/pages/Home.tsx:1245`; structural |
| <a id="fn-formatbuilddate"></a>`formatBuildDate(build: BuildInfo \| null \| undefined): string` | Adapta la llamada a ``${build.date} ${build.time}`.trim` con el contexto de esta integración. | `web:src/components/AboutProjectPanel.tsx:495`; structural |
| <a id="fn-formatbytes"></a>`formatBytes(value: unknown): string` | Coordina `Math.max`, `Math.max(0, Math.round(number)).toLocaleString`, `Math.round` para Layout y navegación de secciones. | `web:src/components/AboutProjectPanel.tsx:499`; structural |
| <a id="fn-formatcarmode"></a>`formatCarMode(mode: string): string` | Operación format Car Mode de Layout y navegación de secciones; semántica fina pendiente de ampliación. | `web:src/pages/Home.tsx:1259`; inferred-name |
| <a id="fn-formatdiagnosticcounter"></a>`formatDiagnosticCounter(value: number \| null): inferred-by-TypeScript` | Adapta la llamada a `String` con el contexto de esta integración. | `web:src/pages/Home.tsx:1255`; structural |
| <a id="fn-formatheartbeattime"></a>`formatHeartbeatTime(timestamp: number \| null): inferred-by-TypeScript` | Adapta la llamada a `new Intl.DateTimeFormat("es-AR", { hour: "2-digit", minute: "2-digit", second: "2-digit", }).format` con el contexto de esta integración. | `web:src/pages/Home.tsx:1202`; structural |
| <a id="fn-formathex32"></a>`formatHex32(value: number): string` | Coordina `(value >>> 0).toString`, `(value >>> 0).toString(16).toUpperCase`, `(value >>> 0).toString(16).toUpperCase().padStart` para Layout y navegación de secciones. | `web:src/components/AboutProjectPanel.tsx:491`; structural |
| <a id="fn-formathexbyte"></a>`formatHexByte(value: number): string` | Coordina `(value & 0xff).toString`, `(value & 0xff).toString(16).toUpperCase`, `(value & 0xff).toString(16).toUpperCase().padStart` para Layout y navegación de secciones. | `web:src/components/AboutProjectPanel.tsx:487`; structural |
| <a id="fn-formatlocaldatetime"></a>`formatLocalDateTime(timestamp: number): inferred-by-TypeScript` | Adapta la llamada a `new Intl.DateTimeFormat("es-AR", { day: "2-digit", month: "2-digit", year: "numeric", hour: "2-digit", minute: "2-digit", second: "2-digit", }).format` con el contexto de esta integración. | `web:src/pages/Home.tsx:1214`; structural |
| <a id="fn-formatpercent"></a>`formatPercent(value: unknown): string` | Coordina `Math.max`, `Math.round`, `Number` para Layout y navegación de secciones. | `web:src/components/AboutProjectPanel.tsx:504`; structural |
| <a id="fn-formatvalidationflags"></a>`formatValidationFlags(flags: number): string` | Adapta la llamada a `formatHexByte` con el contexto de esta integración. | `web:src/components/AboutProjectPanel.tsx:482`; structural |
| <a id="fn-normalizewebaliveperiodms"></a>`normalizeWebAlivePeriodMs(value: number): number` | Coordina `Math.max`, `Math.min`, `Math.round` para Layout y navegación de secciones. | `web:src/pages/Home.tsx:1277`; structural |
| <a id="fn-readbuildinfo"></a>`readBuildInfo(data: Record<string, unknown>): BuildInfo` | Coordina `assertOkStatus`, `readText` para Layout y navegación de secciones. | `web:src/components/AboutProjectPanel.tsx:446`; structural |
| <a id="fn-readespdeviceinfo"></a>`readEspDeviceInfo(data: Record<string, unknown>): EspDeviceInfo` | Coordina `formatBytes`, `formatPercent`, `readRecord` para Layout y navegación de secciones. | `web:src/components/AboutProjectPanel.tsx:456`; structural |
| <a id="fn-readfirmwareinfo"></a>`readFirmwareInfo(data: Record<string, unknown>): FirmwareInfo` | Coordina `assertOkStatus`, `readText` para Layout y navegación de secciones. | `web:src/components/AboutProjectPanel.tsx:437`; structural |
| <a id="fn-readnonnegativeinteger"></a>`readNonNegativeInteger(value: unknown): number \| null` | Adapta la llamada a `Number.isSafeInteger` con el contexto de esta integración. | `web:src/pages/Home.tsx:1239`; structural |
| <a id="fn-readportforwardingenabled"></a>`readPortForwardingEnabled(data: Record<string, unknown>): boolean \| null` | Operación read Port Forwarding Enabled de Layout y navegación de secciones; semántica fina pendiente de ampliación. | `web:src/pages/Home.tsx:1295`; inferred-name |
| <a id="fn-readrecord"></a>`readRecord(value: unknown): Record<string, unknown>` | Adapta la llamada a `Array.isArray` con el contexto de esta integración. | `web:src/components/AboutProjectPanel.tsx:515`; structural |
| <a id="fn-readtext"></a>`readText(value: unknown): string` | Adapta la llamada a `value.trim` con el contexto de esta integración. | `web:src/components/AboutProjectPanel.tsx:521`; structural |
| <a id="fn-readwebaliveperiodms"></a>`readWebAlivePeriodMs(data: Record<string, unknown>): number \| null` | Coordina `Number.isFinite`, `normalizeWebAlivePeriodMs` para Layout y navegación de secciones. | `web:src/pages/Home.tsx:1285`; structural |

## Funciones y callbacks internos

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-aboutprojectpanel.applyreport"></a>`AboutProjectPanel.applyReport({ data }: { data: unknown }): inferred-by-TypeScript` | Coordina `parseBootReportData`, `setBootReport` para Layout y navegación de secciones. | `web:src/components/AboutProjectPanel.tsx:178`; structural |
| <a id="fn-home.closeonescape"></a>`Home.closeOnEscape(event: KeyboardEvent): inferred-by-TypeScript` | Adapta la llamada a `setIsModeMenuOpen` con el contexto de esta integración. | `web:src/pages/Home.tsx:154`; structural |
| <a id="fn-home.closeonoutsidepointer"></a>`Home.closeOnOutsidePointer(event: PointerEvent): inferred-by-TypeScript` | Coordina `modeMenuRef.current?.contains`, `setIsModeMenuOpen` para Layout y navegación de secciones. | `web:src/pages/Home.tsx:149`; structural |
| <a id="fn-home.handlecarmodechange"></a>`Home.handleCarModeChange(nextMode: SelectableCarMode): inferred-by-TypeScript` | Coordina `formatCarMode`, `setCarMode`, `setIsModeMenuOpen` para Layout y navegación de secciones. | `web:src/pages/Home.tsx:491`; structural |
| <a id="fn-home.handleportforwardingchange"></a>`Home.handlePortForwardingChange(nextEnabled: boolean): inferred-by-TypeScript` | Coordina `refreshF4Preferences`, `request`, `setPortForwardingEnabled` para Layout y navegación de secciones. | `web:src/pages/Home.tsx:531`; structural |
| <a id="fn-home.handlesendheartbeat"></a>`Home.handleSendHeartbeat(): inferred-by-TypeScript` | Coordina `onHeartbeatReceived`, `refreshWifiStatus` para Layout y navegación de secciones. | `web:src/pages/Home.tsx:397`; structural |
| <a id="fn-home.openpineditor"></a>`Home.openPinEditor(): inferred-by-TypeScript` | Coordina `setConfirmPin`, `setCurrentPin`, `setNewPin` para Layout y navegación de secciones. | `web:src/pages/Home.tsx:441`; structural |
| <a id="fn-home.openqtdiagnostics"></a>`Home.openQtDiagnostics(): inferred-by-TypeScript` | Coordina `refreshQtDiagnostics`, `setOpenQtDiagnosticsModal` para Layout y navegación de secciones. | `web:src/pages/Home.tsx:526`; structural |
| <a id="fn-home.refreshqtdiagnostics"></a>`Home.refreshQtDiagnostics(): inferred-by-TypeScript` | Coordina `Promise.all`, `refreshF4Preferences`, `refreshQtControlSnapshot` para Layout y navegación de secciones. | `web:src/pages/Home.tsx:500`; structural |
| <a id="fn-home.requestpinchange"></a>`Home.requestPinChange(): inferred-by-TypeScript` | Coordina `/^\d{4}$/.test`, `changePin`, `setConfirmPin` para Layout y navegación de secciones. | `web:src/pages/Home.tsx:469`; structural |
| <a id="fn-home.requestpinvalidation"></a>`Home.requestPinValidation(): inferred-by-TypeScript` | Coordina `/^\d{4}$/.test`, `login`, `setPinStatus` para Layout y navegación de secciones. | `web:src/pages/Home.tsx:453`; structural |
| <a id="fn-pageheader.getpagetitle"></a>`PageHeader.getPageTitle(): inferred-by-TypeScript` | Adapta la llamada a `path.startsWith` con el contexto de esta integración. | `web:src/components/PageHeader.tsx:55`; structural |
| <a id="fn-pindigitfield.handlechange"></a>`PinDigitField.handleChange(nextValue: string): inferred-by-TypeScript` | Coordina `nextValue.replace`, `nextValue.replace(/\D/g, "").slice`, `onChange` para Layout y navegación de secciones. | `web:src/pages/Home.tsx:1655`; structural |

## Estado y efectos React encontrados

| Hook | Función/closure | Evidencia |
| --- | --- | --- |
| `useReducedMotion` | `AboutProjectPanel` | `web:src/components/AboutProjectPanel.tsx:73` |
| `useWebSocket` | `AboutProjectPanel` | `web:src/components/AboutProjectPanel.tsx:74` |
| `useEspWifiStatus` | `AboutProjectPanel` | `web:src/components/AboutProjectPanel.tsx:75` |
| `useUser` | `AboutProjectPanel` | `web:src/components/AboutProjectPanel.tsx:76` |
| `useState` | `AboutProjectPanel` | `web:src/components/AboutProjectPanel.tsx:77` |
| `useState` | `AboutProjectPanel` | `web:src/components/AboutProjectPanel.tsx:78` |
| `useState` | `AboutProjectPanel` | `web:src/components/AboutProjectPanel.tsx:79` |
| `useState` | `AboutProjectPanel` | `web:src/components/AboutProjectPanel.tsx:82` |
| `useState` | `AboutProjectPanel` | `web:src/components/AboutProjectPanel.tsx:83` |
| `useState` | `AboutProjectPanel` | `web:src/components/AboutProjectPanel.tsx:84` |
| `useState` | `AboutProjectPanel` | `web:src/components/AboutProjectPanel.tsx:85` |
| `useCallback` | `AboutProjectPanel` | `web:src/components/AboutProjectPanel.tsx:87` |
| `useEffect` | `AboutProjectPanel` | `web:src/components/AboutProjectPanel.tsx:173` |
| `useEffect` | `AboutProjectPanel` | `web:src/components/AboutProjectPanel.tsx:177` |
| `useNavigate` | `NavigateVT` | `web:src/components/NavigateVT.tsx:13` |
| `useEffect` | `NavigateVT` | `web:src/components/NavigateVT.tsx:14` |
| `useLocation` | `PageHeader` | `web:src/components/PageHeader.tsx:44` |
| `useNavigate` | `PageHeader` | `web:src/components/PageHeader.tsx:45` |
| `useUser` | `PageHeader` | `web:src/components/PageHeader.tsx:46` |
| `useScreenStreamModal` | `PageHeader` | `web:src/components/PageHeader.tsx:47` |
| `useUser` | `RedirectRoot` | `web:src/components/RedirectRoot.tsx:6` |
| `useLocation` | `RootLayout` | `web:src/components/RootLayout.tsx:7` |
| `useOutlet` | `RootLayout` | `web:src/components/RootLayout.tsx:8` |
| `useReducedMotion` | `RootLayout` | `web:src/components/RootLayout.tsx:9` |
| `useState` | `StartupWelcome` | `web:src/components/StartupWelcome.tsx:8` |
| `useState` | `StartupWelcome` | `web:src/components/StartupWelcome.tsx:9` |
| `useRef` | `StartupWelcome` | `web:src/components/StartupWelcome.tsx:10` |
| `useRef` | `StartupWelcome` | `web:src/components/StartupWelcome.tsx:11` |
| `useLayoutEffect` | `StartupWelcome` | `web:src/components/StartupWelcome.tsx:13` |
| `useEffect` | `StartupWelcome` | `web:src/components/StartupWelcome.tsx:20` |
| `useEffect` | `StartupWelcome` | `web:src/components/StartupWelcome.tsx:26` |
| `useUser` | `DevModeToggle` | `web:src/pages/Home.tsx:59` |
| `useWebSocket` | `Home` | `web:src/pages/Home.tsx:88` |
| `useEspWifiStatus` | `Home` | `web:src/pages/Home.tsx:92` |
| `useNavigate` | `Home` | `web:src/pages/Home.tsx:94` |
| `useUser` | `Home` | `web:src/pages/Home.tsx:95` |
| `useScreenStreamModal` | `Home` | `web:src/pages/Home.tsx:99` |
| `useScreen` | `Home` | `web:src/pages/Home.tsx:100` |
| `useCarMode` | `Home` | `web:src/pages/Home.tsx:101` |
| `useState` | `Home` | `web:src/pages/Home.tsx:102` |
| `useState` | `Home` | `web:src/pages/Home.tsx:103` |
| `useState` | `Home` | `web:src/pages/Home.tsx:104` |
| `useState` | `Home` | `web:src/pages/Home.tsx:105` |
| `useState` | `Home` | `web:src/pages/Home.tsx:107` |
| `useState` | `Home` | `web:src/pages/Home.tsx:108` |
| `useState` | `Home` | `web:src/pages/Home.tsx:109` |
| `useState` | `Home` | `web:src/pages/Home.tsx:110` |
| `useState` | `Home` | `web:src/pages/Home.tsx:111` |
| `useState` | `Home` | `web:src/pages/Home.tsx:112` |
| `useState` | `Home` | `web:src/pages/Home.tsx:113` |
| `useState` | `Home` | `web:src/pages/Home.tsx:114` |
| `useState` | `Home` | `web:src/pages/Home.tsx:115` |
| `useState` | `Home` | `web:src/pages/Home.tsx:119` |
| `useState` | `Home` | `web:src/pages/Home.tsx:123` |
| `useState` | `Home` | `web:src/pages/Home.tsx:126` |
| `useState` | `Home` | `web:src/pages/Home.tsx:127` |
| `useState` | `Home` | `web:src/pages/Home.tsx:128` |
| `useState` | `Home` | `web:src/pages/Home.tsx:130` |
| `useState` | `Home` | `web:src/pages/Home.tsx:131` |
| `useState` | `Home` | `web:src/pages/Home.tsx:135` |
| `useState` | `Home` | `web:src/pages/Home.tsx:136` |
| `useRef` | `Home` | `web:src/pages/Home.tsx:141` |
| `useRef` | `Home` | `web:src/pages/Home.tsx:142` |
| `useRef` | `Home` | `web:src/pages/Home.tsx:143` |
| `useRef` | `Home` | `web:src/pages/Home.tsx:144` |
| `useEffect` | `Home` | `web:src/pages/Home.tsx:146` |
| `useCallback` | `Home` | `web:src/pages/Home.tsx:166` |
| `useCallback` | `Home` | `web:src/pages/Home.tsx:184` |
| `useCallback` | `Home` | `web:src/pages/Home.tsx:193` |
| `useEffect` | `Home` | `web:src/pages/Home.tsx:230` |
| `useEffect` | `Home` | `web:src/pages/Home.tsx:253` |
| `useEffect` | `Home` | `web:src/pages/Home.tsx:273` |
| `useCallback` | `Home` | `web:src/pages/Home.tsx:328` |
| `useEffect` | `Home` | `web:src/pages/Home.tsx:364` |
| `useEffect` | `Home` | `web:src/pages/Home.tsx:385` |
| `useRef` | `PinDigitField` | `web:src/pages/Home.tsx:1647` |
| `useEffect` | `PinDigitField` | `web:src/pages/Home.tsx:1649` |
