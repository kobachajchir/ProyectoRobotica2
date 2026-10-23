# Resolución de variantes y fallback de pantalla

ID: `web:module/src/screens/screenRenderRegistry.ts` · Capa: **integración** · Fotografía: 2026-10-20.

Documentación de fuente; sin tests, compilación, navegador ni validación física.

## Responsabilidad y alcance

Elige builder y parámetros a partir de screenCode/source/rawData y resuelve identidades con información insuficiente.

Orden: remediated, identified, switch de builders. No confundir esa prioridad con el renderer alcanzado en F4.

<a id="modularity"></a>
## Modularidad observada

Capa de integración entre reportes y biblioteca de comandos, dependiente del esquema y catálogos del proyecto.

<a id="state"></a>
## Handle, estado y atributos externos

ResolvedOledScreen contiene título, variante, categoría, builder y comandos; no contiene prueba de exactitud ni estado físico.

<a id="lifecycle"></a>
## Ciclo de vida y contexto

- Resolver identidad y source; seleccionar variante antes de construir comandos.
- Cuando faltan campos, algunos caminos usan identity-only y otros valores predeterminados; la diferencia queda en hallazgos.

<a id="interactions"></a>
## Interacciones

- ScreenStreamWorkspace añade irEmitters al rawData.
- F4 publica extensiones concretas de dashboard/notificación/selección, no el estado de cada builder.

<a id="limitations"></a>
## Precauciones y límites

- readMpuArgs/readMotorArgs/readPinFormArgs contienen defaults que no son datos recibidos del equipo.
- La metadata builder candidata no demuestra cobertura exacta.
- GOTO/ON-OFF, OSK y confirmaciones requieren discriminantes y datos suficientes.

## Hallazgos conectados

- [AMDOC-WEB-002: El visor rellena estados que el reporte de pantalla no transporta](../../../review/FINDINGS.md#amdoc-web-002)
- [AMDOC-WEB-003: El renderer SVG actual no constituye el port completo de OLED F4](../../../review/FINDINGS.md#amdoc-web-003)
- [AMDOC-WEB-005: Normalizadores del visor pierden el modo GOTO](../../../review/FINDINGS.md#amdoc-web-005)
- [AMDOC-WEB-006: Los builders de sensores no reciben el mismo modelo que la telemetría actual](../../../review/FINDINGS.md#amdoc-web-006)
- [AMDOC-WEB-009: El catálogo de línea no tiene bindings específicos de visor localizados](../../../review/FINDINGS.md#amdoc-web-009)

## Archivos y evidencia

| Archivo | Análisis | Hash SHA-256 |
| --- | --- | --- |
| `src/screens/identifiedScreens.ts` | typescript-ast-static | `edd67775298b615e4ca4e75bd01af5b0be4e9c8e6d0355da36c7668718245a6d` |
| `src/screens/remediatedScreens.ts` | typescript-ast-static | `6a008ef7589f42f17d6641bceacadc9e0760b51bee7eb25b1ae61dac192f972f` |
| `src/screens/screenRenderRegistry.ts` | typescript-ast-static | `2e8e6cc33595287753c64af0dfc65a03483c943558ccb0a85bf79914e9b5b98d` |

## Tipos, puertos, props y callbacks

En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.

### `IdentifiedScreenArgs`

`web:src/screens/identifiedScreens.ts:10`

| Atributo / firma | Propósito |
| --- | --- |
| `lines: string[]` | Dato de estado/configuración `lines`; declaración estructural, interpretación específica pendiente de ampliar. |
| `footer?: string` | Dato de estado/configuración `footer`; declaración estructural, interpretación específica pendiente de ampliar. |
| `iconRef?: string` | Dato de estado/configuración `iconRef`; declaración estructural, interpretación específica pendiente de ampliar. |

### `IdentifiedMenuDefinition`

`web:src/screens/screenRenderRegistry.ts:176`

| Atributo / firma | Propósito |
| --- | --- |
| `variant: string` | Dato de estado/configuración `variant`; declaración estructural, interpretación específica pendiente de ampliar. |
| `description: string` | Dato de estado/configuración `description`; declaración estructural, interpretación específica pendiente de ampliar. |
| `items: MenuScreenItem[]` | Dato de estado/configuración `items`; declaración estructural, interpretación específica pendiente de ampliar. |

### `IdentifiedScreenDefinition`

`web:src/screens/screenRenderRegistry.ts:182`

| Atributo / firma | Propósito |
| --- | --- |
| `variant: string` | Dato de estado/configuración `variant`; declaración estructural, interpretación específica pendiente de ampliar. |
| `category: string` | Dato de estado/configuración `category`; declaración estructural, interpretación específica pendiente de ampliar. |
| `description: string` | Dato de estado/configuración `description`; declaración estructural, interpretación específica pendiente de ampliar. |
| `lines: string[]` | Dato de estado/configuración `lines`; declaración estructural, interpretación específica pendiente de ampliar. |
| `footer?: string` | Dato de estado/configuración `footer`; declaración estructural, interpretación específica pendiente de ampliar. |

### `ScreenRenderInput`

`web:src/screens/screenRenderRegistry.ts:321`

| Atributo / firma | Propósito |
| --- | --- |
| `screenCode: number` | Dato de estado/configuración `screenCode`; declaración estructural, interpretación específica pendiente de ampliar. |
| `source?: number` | Dato de estado/configuración `source`; declaración estructural, interpretación específica pendiente de ampliar. |
| `sourceName?: string` | Dato de estado/configuración `sourceName`; declaración estructural, interpretación específica pendiente de ampliar. |
| `title?: string` | Dato de estado/configuración `title`; declaración estructural, interpretación específica pendiente de ampliar. |
| `rawData?: Record<string, unknown>` | Dato de estado/configuración `rawData`; declaración estructural, interpretación específica pendiente de ampliar. |

### `ResolvedOledScreen`

`web:src/screens/screenRenderRegistry.ts:329`

| Atributo / firma | Propósito |
| --- | --- |
| `title: string` | Dato de estado/configuración `title`; declaración estructural, interpretación específica pendiente de ampliar. |
| `variant: string` | Dato de estado/configuración `variant`; declaración estructural, interpretación específica pendiente de ampliar. |
| `category: string` | Dato de estado/configuración `category`; declaración estructural, interpretación específica pendiente de ampliar. |
| `builder: string` | Dato de estado/configuración `builder`; declaración estructural, interpretación específica pendiente de ampliar. |
| `description: string` | Dato de estado/configuración `description`; declaración estructural, interpretación específica pendiente de ampliar. |
| `commands: OledCommand[]` | Dato de estado/configuración `commands`; declaración estructural, interpretación específica pendiente de ampliar. |


## Funciones públicas / componentes / hooks

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-backitem"></a>`backItem(): MenuScreenItem` | Adapta la llamada a `item` con el contexto de esta integración. | `web:src/screens/screenRenderRegistry.ts:191`; structural |
| <a id="fn-buildidentifiednotificationcommands"></a>`buildIdentifiedNotificationCommands(lines: string[], progress?: NotificationProgressArgs): OledCommand[]` | Coordina `buildIdentifiedScreenCommands`, `withNotificationProgress` para Resolución de variantes y fallback de pantalla. | `web:src/screens/identifiedScreens.ts:55`; structural |
| <a id="fn-buildidentifiedscreencommands"></a>`buildIdentifiedScreenCommands(args: IdentifiedScreenArgs): OledCommand[]` | Coordina `Boolean`, `args.footer?.trim`, `args.footer?.trim().slice` para Resolución de variantes y fallback de pantalla. | `web:src/screens/identifiedScreens.ts:32`; structural |
| <a id="fn-centeredx"></a>`centeredX(text: string, glyphWidth: inferred-by-TypeScript): number` | Coordina `Math.floor`, `Math.max` para Resolución de variantes y fallback de pantalla. | `web:src/screens/identifiedScreens.ts:16`; structural |
| <a id="fn-item"></a>`item(label: string, iconRef: inferred-by-TypeScript): MenuScreenItem` | Operación item de Resolución de variantes y fallback de pantalla; semántica fina pendiente de ampliación. | `web:src/screens/screenRenderRegistry.ts:190`; inferred-name |
| <a id="fn-normalizedlines"></a>`normalizedLines(lines: string[]): string[]` | Coordina `lines .map`, `lines .map((line) => line.trim().slice(0, 18)) .filter`, `lines .map((line) => line.trim().slice(0, 18)) .filter(Boolean) .slice` para Resolución de variantes y fallback de pantalla. | `web:src/screens/identifiedScreens.ts:20`; structural |
| <a id="fn-readboolean"></a>`readBoolean(value: unknown): boolean \| undefined` | Coordina `Number.isFinite`, `value.trim`, `value.trim().toLowerCase` para Resolución de variantes y fallback de pantalla. | `web:src/screens/screenRenderRegistry.ts:688`; structural |
| <a id="fn-readcarmode"></a>`readCarMode(value: unknown): CarModeLabel` | Operación read Car Mode de Resolución de variantes y fallback de pantalla; semántica fina pendiente de ampliación. | `web:src/screens/screenRenderRegistry.ts:664`; inferred-name |
| <a id="fn-readconnection"></a>`readConnection(value: unknown): DashboardConnectionState` | Operación read Connection de Resolución de variantes y fallback de pantalla; semántica fina pendiente de ampliación. | `web:src/screens/screenRenderRegistry.ts:660`; inferred-name |
| <a id="fn-readcredentialsssid"></a>`readCredentialsSsid(data: Record<string, unknown>): string \| undefined` | Adapta la llamada a `readString` con el contexto de esta integración. | `web:src/screens/screenRenderRegistry.ts:622`; structural |
| <a id="fn-readdashboardargs"></a>`readDashboardArgs(data: Record<string, unknown>): inferred-by-TypeScript` | Coordina `readBoolean`, `readCarMode`, `readConnection` para Resolución de variantes y fallback de pantalla. | `web:src/screens/screenRenderRegistry.ts:584`; structural |
| <a id="fn-readdashboardbackend"></a>`readDashboardBackend(value: unknown): "WEB" \| "AT" \| "No ESP"` | Operación read Dashboard Backend de Resolución de variantes y fallback de pantalla; semántica fina pendiente de ampliación. | `web:src/screens/screenRenderRegistry.ts:597`; inferred-name |
| <a id="fn-readmenuargs"></a>`readMenuArgs(data: Record<string, unknown>): inferred-by-TypeScript` | Coordina `readBoolean`, `readNumber` para Resolución de variantes y fallback de pantalla. | `web:src/screens/screenRenderRegistry.ts:605`; structural |
| <a id="fn-readmode"></a>`readMode(value: unknown): ModeChangeScreenArgs["mode"]` | Operación read Mode de Resolución de variantes y fallback de pantalla; semántica fina pendiente de ampliación. | `web:src/screens/screenRenderRegistry.ts:670`; inferred-name |
| <a id="fn-readmodechangeargs"></a>`readModeChangeArgs(data: Record<string, unknown>): ModeChangeScreenArgs` | Adapta la llamada a `readMode` con el contexto de esta integración. | `web:src/screens/screenRenderRegistry.ts:601`; structural |
| <a id="fn-readmotorargs"></a>`readMotorArgs(data: Record<string, unknown>): inferred-by-TypeScript` | Lee porcentajes/selección o usa valores por defecto del builder. | `web:src/screens/screenRenderRegistry.ts:643`; editorial |
| <a id="fn-readmotordirection"></a>`readMotorDirection(value: unknown): MotorDirection` | Operación read Motor Direction de Resolución de variantes y fallback de pantalla; semántica fina pendiente de ampliación. | `web:src/screens/screenRenderRegistry.ts:680`; inferred-name |
| <a id="fn-readmotorselection"></a>`readMotorSelection(value: unknown): MotorSelection` | Operación read Motor Selection de Resolución de variantes y fallback de pantalla; semántica fina pendiente de ampliación. | `web:src/screens/screenRenderRegistry.ts:674`; inferred-name |
| <a id="fn-readmpuargs"></a>`readMpuArgs(data: Record<string, unknown>): inferred-by-TypeScript` | Lee campos escalares esperados o usa valores de ejemplo del builder. | `web:src/screens/screenRenderRegistry.ts:632`; editorial |
| <a id="fn-readnetworkobjectssids"></a>`readNetworkObjectSsids(value: unknown): string[] \| undefined` | Coordina `Array.isArray`, `value .map`, `value .map((item) => { if (typeof item === "string") { return item; } if (!item \|\| typeof item !== "object") { return null; } const record = item as Record<string, unknown>; return ( readString(record.ssid) ?? readString(record.SSID) ?? readString(record.name) ?? readString(record.networkName) ); }) .filter` para Resolución de variantes y fallback de pantalla. | `web:src/screens/screenRenderRegistry.ts:754`; structural |
| <a id="fn-readnumber"></a>`readNumber(value: unknown): number \| undefined` | Coordina `Math.trunc`, `Number`, `Number.isFinite` para Resolución de variantes y fallback de pantalla. | `web:src/screens/screenRenderRegistry.ts:710`; structural |
| <a id="fn-readnumberarray"></a>`readNumberArray(value: unknown, length: number): number[] \| undefined` | Coordina `(values as number[]).slice`, `Array.isArray`, `value.map` para Resolución de variantes y fallback de pantalla. | `web:src/screens/screenRenderRegistry.ts:727`; structural |
| <a id="fn-readpinformargs"></a>`readPinFormArgs(data: Record<string, unknown>): inferred-by-TypeScript` | Lee dígitos/índice o usa ceros; no dispone del snapshot local PIN por el reporte actual. | `web:src/screens/screenRenderRegistry.ts:653`; editorial |
| <a id="fn-readstring"></a>`readString(value: unknown): string \| undefined` | Operación read String de Resolución de variantes y fallback de pantalla; semántica fina pendiente de ampliación. | `web:src/screens/screenRenderRegistry.ts:684`; inferred-name |
| <a id="fn-readstringarray"></a>`readStringArray(value: unknown): string[] \| undefined` | Coordina `Array.isArray`, `value.filter` para Resolución de variantes y fallback de pantalla. | `web:src/screens/screenRenderRegistry.ts:738`; structural |
| <a id="fn-readwifiresultssids"></a>`readWifiResultSsids(data: Record<string, unknown>): string[] \| undefined` | Coordina `readNetworkObjectSsids`, `readStringArray` para Resolución de variantes y fallback de pantalla. | `web:src/screens/screenRenderRegistry.ts:744`; structural |
| <a id="fn-readwifistatusargs"></a>`readWifiStatusArgs(data: Record<string, unknown>): WifiStatusArgs` | Adapta la llamada a `readString` con el contexto de esta integración. | `web:src/screens/screenRenderRegistry.ts:613`; structural |
| <a id="fn-resolveidentifiedscreen"></a>`resolveIdentifiedScreen(input: ScreenRenderInput, data: Record<string, unknown>, progress: NotificationProgressArgs): ResolvedOledScreen \| null` | Resuelve definiciones conocidas de menú/aviso/pantalla. | `web:src/screens/screenRenderRegistry.ts:531`; editorial |
| <a id="fn-resolveoledscreen"></a>`resolveOledScreen(input: ScreenRenderInput): ResolvedOledScreen \| null` | Selecciona el camino efectivo remediated/identified/switch con sus parámetros. | `web:src/screens/screenRenderRegistry.ts:338`; editorial |
| <a id="fn-resolveremediatedscreen"></a>`resolveRemediatedScreen(input: ScreenRenderInput): ResolvedOledScreen \| null` | Usa menú reportado cuando tiene datos suficientes o muestra identidad sin inventar estado. | `web:src/screens/remediatedScreens.ts:30`; editorial |
| <a id="fn-resolved"></a>`resolved(input: ScreenRenderInput, variant: string, category: string, builder: string, description: string, commands: OledCommand[]): ResolvedOledScreen` | Operación resolved de Resolución de variantes y fallback de pantalla; semántica fina pendiente de ampliación. | `web:src/screens/screenRenderRegistry.ts:513`; inferred-name |

## Funciones y callbacks internos

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
