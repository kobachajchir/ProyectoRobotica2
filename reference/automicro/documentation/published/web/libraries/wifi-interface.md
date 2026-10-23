# Estado Wi-Fi, AP y credenciales Web

ID: `web:module/src/contexts/EspWifiStatusContext.tsx` · Capa: **integración** · Fotografía: 2026-10-20.

Documentación de fuente; sin tests, compilación, navegador ni validación física.

## Responsabilidad y alcance

Presenta y controla Wi-Fi/AP del ESP, flujo de credenciales y política fallback.

Estado radio, clientes y configuración deseada/aplicada; la autoridad F4 es otro plano.

<a id="modularity"></a>
## Modularidad observada

React y API de red del producto; no llama al SDK del ESP directamente.

<a id="state"></a>
## Handle, estado y atributos externos

Provider conserva reportes de ESP, modal conserva edición local y sessionId/estado del flujo según contrato.

<a id="lifecycle"></a>
## Ciclo de vida y contexto

- Consultar/suscribir estado, abrir flujo solicitado y confirmar/cancelar con su correlación.
- Al cambiar radio pueden existir cortes breves; conservar semántica de reanudación de sesión.

<a id="interactions"></a>
## Interacciones

- WifiService gobierna radio, NVS persiste y bridge publica estados.
- Docs de OLED conectividad necesita el mismo SSID/IP confirmado, no valores de ejemplo.

<a id="limitations"></a>
## Precauciones y límites

- Cambios de depuración pueden vivir solo en RAM.
- No copiar contraseñas reales a reportes/capturas/documentación.

## Hallazgos conectados


## Archivos y evidencia

| Archivo | Análisis | Hash SHA-256 |
| --- | --- | --- |
| `src/components/ApFallbackControls.tsx` | typescript-ast-static | `b5545fa4f6ad21ab292bd9a40a0ff9a0f5c1d89ba6295c00ba450abbc35f96ca` |
| `src/components/ApProvisioningGate.tsx` | typescript-ast-static | `84c5580cca99ea39717a7b7997f576b53b12fbb90804c3b01b1bbdde1f11bb49` |
| `src/components/WifiCredentialsModal.tsx` | typescript-ast-static | `16d923074848347858da78421ffbded52d5328a05b8ef2f9f2cf80344093727e` |
| `src/contexts/EspWifiStatusContext.tsx` | typescript-ast-static | `1d663764517e012717391d0a05d149ad468ac4c1469a41391b5769b65e553ae1` |
| `src/contexts/WifiCredentialsContext.tsx` | typescript-ast-static | `c32486779a87bf8cbe01da6d71520c37e89fc9ee3d3a274e71e8319493209994` |
| `src/hooks/useApFallback.ts` | typescript-ast-static | `9ab32f167cb40449246238e84d2860ccefac91234b2ef314e9a69b90e6946a7f` |
| `src/pages/WifiSection.tsx` | typescript-ast-static | `055cb98e443cd2e9c342010457bd9bd53f196f5b449ffc310f7f8e8f8d121208` |
| `src/types/WifiTypes.ts` | typescript-ast-static | `f9fa20bf52c72e3f6a6921befa13289b9d9ed533036e690fb67d24437834e0e4` |
| `src/utils/wifiModeSwitch.ts` | typescript-ast-static | `15dd891be9fab39151b90b1fdabfd53c1e9123f07d69335fab1583f6b15b6925` |

## Tipos, puertos, props y callbacks

En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.

### `WifiCredentialsModalProps`

`web:src/components/WifiCredentialsModal.tsx:7`

| Atributo / firma | Propósito |
| --- | --- |
| `isOpen: boolean` | Dato de estado/configuración `isOpen`; declaración estructural, interpretación específica pendiente de ampliar. |
| `status: WifiCredentialsStatus` | Último resultado y guardas del módulo; los getters pueden devolver una vista de este almacenamiento. |
| `ssid: string \| null` | Dato de estado/configuración `ssid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `error: string \| null` | Dato de estado/configuración `error`; declaración estructural, interpretación específica pendiente de ampliar. |
| `ip: string \| null` | Dato de estado/configuración `ip`; declaración estructural, interpretación específica pendiente de ampliar. |
| `timeoutMs: number \| null` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `onSubmit: (ssid: string, password: string) => boolean` | Operación on Submit de Estado Wi-Fi, AP y credenciales Web; semántica fina pendiente de ampliación. |
| `onCancel: () => boolean` | Informa cancelación del teclado al contexto del usuario. |
| `onDismiss: () => void` | Operación on Dismiss de Estado Wi-Fi, AP y credenciales Web; semántica fina pendiente de ampliación. |

### `EspWifiStatus`

`web:src/contexts/EspWifiStatusContext.tsx:15`

| Atributo / firma | Propósito |
| --- | --- |
| `mode: string` | Dato de estado/configuración `mode`; declaración estructural, interpretación específica pendiente de ampliar. |
| `apActive: boolean` | Dato de estado/configuración `apActive`; declaración estructural, interpretación específica pendiente de ampliar. |
| `staConnected: boolean` | Dato de estado/configuración `staConnected`; declaración estructural, interpretación específica pendiente de ampliar. |
| `apIp: string` | Dato de estado/configuración `apIp`; declaración estructural, interpretación específica pendiente de ampliar. |
| `staIp: string` | Dato de estado/configuración `staIp`; declaración estructural, interpretación específica pendiente de ampliar. |
| `networkSsid: string` | Dato de estado/configuración `networkSsid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `rssiDbm: number \| null` | Dato de estado/configuración `rssiDbm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `raw: Record<string, unknown>` | Dato de estado/configuración `raw`; declaración estructural, interpretación específica pendiente de ampliar. |

### `EspWifiStatusContextValue`

`web:src/contexts/EspWifiStatusContext.tsx:26`

| Atributo / firma | Propósito |
| --- | --- |
| `status: EspWifiStatus \| null` | Último resultado y guardas del módulo; los getters pueden devolver una vista de este almacenamiento. |
| `statusKnown: boolean` | Dato de estado/configuración `statusKnown`; declaración estructural, interpretación específica pendiente de ampliar. |
| `loading: boolean` | Dato de estado/configuración `loading`; declaración estructural, interpretación específica pendiente de ampliar. |
| `error: string \| null` | Dato de estado/configuración `error`; declaración estructural, interpretación específica pendiente de ampliar. |
| `lastCheckedAt: number \| null` | Dato de estado/configuración `lastCheckedAt`; declaración estructural, interpretación específica pendiente de ampliar. |
| `isApProvisioning: boolean` | Dato de estado/configuración `isApProvisioning`; declaración estructural, interpretación específica pendiente de ampliar. |
| `refresh: () => Promise<EspWifiStatus \| null>` | Operación refresh de Estado Wi-Fi, AP y credenciales Web; semántica fina pendiente de ampliación. |

### `WifiCredentialsState`

`web:src/contexts/WifiCredentialsContext.tsx:42`

| Atributo / firma | Propósito |
| --- | --- |
| `status: WifiCredentialsStatus` | Último resultado y guardas del módulo; los getters pueden devolver una vista de este almacenamiento. |
| `ssid: string \| null` | Dato de estado/configuración `ssid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `error: string \| null` | Dato de estado/configuración `error`; declaración estructural, interpretación específica pendiente de ampliar. |
| `reason: string \| null` | Dato de estado/configuración `reason`; declaración estructural, interpretación específica pendiente de ampliar. |
| `ip: string \| null` | Dato de estado/configuración `ip`; declaración estructural, interpretación específica pendiente de ampliar. |
| `timeoutMs: number \| null` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `requestId: string \| null` | Dato de estado/configuración `requestId`; declaración estructural, interpretación específica pendiente de ampliar. |

### `WifiCredentialsProviderProps`

`web:src/contexts/WifiCredentialsContext.tsx:52`

| Atributo / firma | Propósito |
| --- | --- |
| `children: ReactNode` | Dato de estado/configuración `children`; declaración estructural, interpretación específica pendiente de ampliar. |

### `WifiCredentialsContextValue`

`web:src/contexts/WifiCredentialsContext.tsx:56`

| Atributo / firma | Propósito |
| --- | --- |
| `state: WifiCredentialsState` | Dato de estado/configuración `state`; declaración estructural, interpretación específica pendiente de ampliar. |
| `requestCurrentCredentials: () => string \| null` | Operación request Current Credentials de Estado Wi-Fi, AP y credenciales Web; semántica fina pendiente de ampliación. |

### `Policy`

`web:src/hooks/useApFallback.ts:6`

| Atributo / firma | Propósito |
| --- | --- |
| `status: number` | Último resultado y guardas del módulo; los getters pueden devolver una vista de este almacenamiento. |
| `enabled: boolean` | Dato de estado/configuración `enabled`; declaración estructural, interpretación específica pendiente de ampliar. |
| `syncState: number` | Dato de estado/configuración `syncState`; declaración estructural, interpretación específica pendiente de ampliar. |

### `ApFallbackController`

`web:src/hooks/useApFallback.ts:66`

Tipo: `ReturnType<typeof useApFallback>`.

### `FeedbackTone`

`web:src/pages/WifiSection.tsx:48`

Tipo: `"info" \| "success" \| "error"`.

### `InlineFeedback`

`web:src/pages/WifiSection.tsx:50`

| Atributo / firma | Propósito |
| --- | --- |
| `tone: FeedbackTone` | Dato de estado/configuración `tone`; declaración estructural, interpretación específica pendiente de ampliar. |
| `text: string` | Dato de estado/configuración `text`; declaración estructural, interpretación específica pendiente de ampliar. |

### `PersistedWifiConfig`

`web:src/pages/WifiSection.tsx:55`

| Atributo / firma | Propósito |
| --- | --- |
| `loaded: boolean` | Dato de estado/configuración `loaded`; declaración estructural, interpretación específica pendiente de ampliar. |
| `stationSsid: string` | Dato de estado/configuración `stationSsid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `stationIp: string` | Dato de estado/configuración `stationIp`; declaración estructural, interpretación específica pendiente de ampliar. |
| `stationGateway: string` | Dato de estado/configuración `stationGateway`; declaración estructural, interpretación específica pendiente de ampliar. |
| `stationSubnet: string` | Dato de estado/configuración `stationSubnet`; declaración estructural, interpretación específica pendiente de ampliar. |
| `apSsid: string` | Dato de estado/configuración `apSsid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `apIp: string` | Dato de estado/configuración `apIp`; declaración estructural, interpretación específica pendiente de ampliar. |

### `ModeSwitchPhase`

`web:src/pages/WifiSection.tsx:65`

Tipo: `"idle" \| "submitting" \| "scheduled"`.

### `WifiSectionProps`

`web:src/pages/WifiSection.tsx:84`

| Atributo / firma | Propósito |
| --- | --- |
| `provisioning?: boolean` | Dato de estado/configuración `provisioning`; declaración estructural, interpretación específica pendiente de ampliar. |

### `WifiScanResult`

`web:src/pages/WifiSection.tsx:2237`

| Atributo / firma | Propósito |
| --- | --- |
| `networks: WifiNetworkDetail[]` | Dato de estado/configuración `networks`; declaración estructural, interpretación específica pendiente de ampliar. |
| `reportedCount: number \| null` | Dato de estado/configuración `reportedCount`; declaración estructural, interpretación específica pendiente de ampliar. |
| `hasExplicitList: boolean` | Dato de estado/configuración `hasExplicitList`; declaración estructural, interpretación específica pendiente de ampliar. |

### `WifiMode`

`web:src/types/WifiTypes.ts:1`

Tipo: `"AP" \| "STATION"`.

### `WifiSectionView`

`web:src/types/WifiTypes.ts:2`

Tipo: `"WIFI" \| "AP"`.

### `WifiCredentialsResultStatus`

`web:src/types/WifiTypes.ts:4`

Tipo: `\| "success" \| "failed" \| "timeout" \| "cancelled"`.

### `WifiCredentialsStatus`

`web:src/types/WifiTypes.ts:10`

Tipo: `\| "idle" \| "requested" \| "submitting" \| "connecting" \| WifiCredentialsResultStatus \| "cancelling"`.

### `WifiCredentialsSubmitParams`

`web:src/types/WifiTypes.ts:32`

| Atributo / firma | Propósito |
| --- | --- |
| `ssid: string` | Dato de estado/configuración `ssid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `password: string` | Dato de estado/configuración `password`; declaración estructural, interpretación específica pendiente de ampliar. |
| `reusePassword?: boolean` | Dato de estado/configuración `reusePassword`; declaración estructural, interpretación específica pendiente de ampliar. |
| `ip?: string` | Dato de estado/configuración `ip`; declaración estructural, interpretación específica pendiente de ampliar. |
| `gateway?: string` | Dato de estado/configuración `gateway`; declaración estructural, interpretación específica pendiente de ampliar. |
| `subnet?: string` | Dato de estado/configuración `subnet`; declaración estructural, interpretación específica pendiente de ampliar. |

### `WifiCredentialsCancelParams`

`web:src/types/WifiTypes.ts:41`

| Atributo / firma | Propósito |
| --- | --- |
| `ssid: string` | Dato de estado/configuración `ssid`; declaración estructural, interpretación específica pendiente de ampliar. |

### `WifiScanNetwork`

`web:src/types/WifiTypes.ts:45`

| Atributo / firma | Propósito |
| --- | --- |
| `ssid: string` | Dato de estado/configuración `ssid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `ssidBytes?: number[]` | Dato de estado/configuración `ssidBytes`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bssid?: string` | Dato de estado/configuración `bssid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `signalStrength: number` | Dato de estado/configuración `signalStrength`; declaración estructural, interpretación específica pendiente de ampliar. |
| `rssi?: number` | Dato de estado/configuración `rssi`; declaración estructural, interpretación específica pendiente de ampliar. |
| `encryptionType: number` | Dato de estado/configuración `encryptionType`; declaración estructural, interpretación específica pendiente de ampliar. |
| `security?: number` | Dato de estado/configuración `security`; declaración estructural, interpretación específica pendiente de ampliar. |
| `channel: number` | Dato de estado/configuración `channel`; declaración estructural, interpretación específica pendiente de ampliar. |
| `index?: number` | Dato de estado/configuración `index`; declaración estructural, interpretación específica pendiente de ampliar. |

### `WifiNetworkDetail`

`web:src/types/WifiTypes.ts:57`

| Atributo / firma | Propósito |
| --- | --- |
| `ssid: string` | Dato de estado/configuración `ssid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `ssidBytes?: number[]` | Dato de estado/configuración `ssidBytes`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bssid?: string` | Dato de estado/configuración `bssid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `signalStrength: number \| null` | Dato de estado/configuración `signalStrength`; declaración estructural, interpretación específica pendiente de ampliar. |
| `rssi?: number` | Dato de estado/configuración `rssi`; declaración estructural, interpretación específica pendiente de ampliar. |
| `encryptionType: string \| number \| null` | Dato de estado/configuración `encryptionType`; declaración estructural, interpretación específica pendiente de ampliar. |
| `security?: number` | Dato de estado/configuración `security`; declaración estructural, interpretación específica pendiente de ampliar. |
| `channel: number \| null` | Dato de estado/configuración `channel`; declaración estructural, interpretación específica pendiente de ampliar. |
| `index?: number` | Dato de estado/configuración `index`; declaración estructural, interpretación específica pendiente de ampliar. |

### `WifiApClient`

`web:src/types/WifiTypes.ts:69`

| Atributo / firma | Propósito |
| --- | --- |
| `name: string` | Dato de estado/configuración `name`; declaración estructural, interpretación específica pendiente de ampliar. |
| `nameSource: "generated-from-mac" \| string` | Dato de estado/configuración `nameSource`; declaración estructural, interpretación específica pendiente de ampliar. |
| `mac: string` | Dato de estado/configuración `mac`; declaración estructural, interpretación específica pendiente de ampliar. |
| `ip: string` | Dato de estado/configuración `ip`; declaración estructural, interpretación específica pendiente de ampliar. |

### `EspOperatingMode`

`web:src/utils/wifiModeSwitch.ts:1`

Tipo: `"AP" \| "RED"`.

### `EspBootMode`

`web:src/utils/wifiModeSwitch.ts:2`

Tipo: `"ap" \| "normal"`.

### `DestinationIpSource`

`web:src/utils/wifiModeSwitch.ts:3`

Tipo: `"fixed" \| "default" \| "dhcp"`.

### `WifiModeConfig`

`web:src/utils/wifiModeSwitch.ts:5`

| Atributo / firma | Propósito |
| --- | --- |
| `apSsid: string` | Dato de estado/configuración `apSsid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `apIp: string` | Dato de estado/configuración `apIp`; declaración estructural, interpretación específica pendiente de ampliar. |
| `stationSsid: string` | Dato de estado/configuración `stationSsid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `stationIp: string` | Dato de estado/configuración `stationIp`; declaración estructural, interpretación específica pendiente de ampliar. |

### `WifiModeDestination`

`web:src/utils/wifiModeSwitch.ts:12`

| Atributo / firma | Propósito |
| --- | --- |
| `mode: EspOperatingMode` | Dato de estado/configuración `mode`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bootMode: EspBootMode` | Dato de estado/configuración `bootMode`; declaración estructural, interpretación específica pendiente de ampliar. |
| `ssid: string` | Dato de estado/configuración `ssid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `ip: string \| null` | Dato de estado/configuración `ip`; declaración estructural, interpretación específica pendiente de ampliar. |
| `ipSource: DestinationIpSource` | Dato de estado/configuración `ipSource`; declaración estructural, interpretación específica pendiente de ampliar. |
| `url: string \| null` | Dato de estado/configuración `url`; declaración estructural, interpretación específica pendiente de ampliar. |

### `WifiStatusLike`

`web:src/utils/wifiModeSwitch.ts:21`

| Atributo / firma | Propósito |
| --- | --- |
| `apActive: boolean` | Dato de estado/configuración `apActive`; declaración estructural, interpretación específica pendiente de ampliar. |
| `staConnected: boolean` | Dato de estado/configuración `staConnected`; declaración estructural, interpretación específica pendiente de ampliar. |


## Funciones públicas / componentes / hooks

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-apfallbackcontrols"></a>`ApFallbackControls({ control, disabled = false }: { control: ApFallbackController; disabled?: boolean; }): inferred-by-TypeScript` | Operación Ap Fallback Controls de Estado Wi-Fi, AP y credenciales Web; semántica fina pendiente de ampliación. | `web:src/components/ApFallbackControls.tsx:3`; inferred-name |
| <a id="fn-apicon"></a>`ApIcon(): inferred-by-TypeScript` | Operación Ap Icon de Estado Wi-Fi, AP y credenciales Web; semántica fina pendiente de ampliación. | `web:src/pages/WifiSection.tsx:2015`; inferred-name |
| <a id="fn-approvisioninggate"></a>`ApProvisioningGate(): inferred-by-TypeScript` | Coordina `useEffect`, `useLocation`, `useNavigate` para Estado Wi-Fi, AP y credenciales Web. | `web:src/components/ApProvisioningGate.tsx:5`; structural |
| <a id="fn-espwifistatusprovider"></a>`EspWifiStatusProvider({ children }: { children: ReactNode }): inferred-by-TypeScript` | Coordina `useCallback`, `useEffect`, `useMemo` para Estado Wi-Fi, AP y credenciales Web. | `web:src/contexts/EspWifiStatusContext.tsx:48`; structural |
| <a id="fn-feedbackbanner"></a>`FeedbackBanner({ feedback }: { feedback: InlineFeedback }): inferred-by-TypeScript` | Operación Feedback Banner de Estado Wi-Fi, AP y credenciales Web; semántica fina pendiente de ampliación. | `web:src/pages/WifiSection.tsx:1872`; inferred-name |
| <a id="fn-metricchip"></a>`MetricChip({ label, value }: { label: string; value: string }): inferred-by-TypeScript` | Operación Metric Chip de Estado Wi-Fi, AP y credenciales Web; semántica fina pendiente de ampliación. | `web:src/pages/WifiSection.tsx:2035`; inferred-name |
| <a id="fn-wificredentialsmodal"></a>`WifiCredentialsModal({  isOpen,  status,  ssid,  error,  ip,  timeoutMs,  onSubmit,  onCancel,  onDismiss, }: WifiCredentialsModalProps): inferred-by-TypeScript` | Coordina `BUSY_STATUSES.has`, `Boolean`, `useEffect` para Estado Wi-Fi, AP y credenciales Web. | `web:src/components/WifiCredentialsModal.tsx:19`; structural |
| <a id="fn-wificredentialsprovider"></a>`WifiCredentialsProvider({   children, }: WifiCredentialsProviderProps): inferred-by-TypeScript` | Coordina `useCallback`, `useEffect`, `useEspWifiStatus` para Estado Wi-Fi, AP y credenciales Web. | `web:src/contexts/WifiCredentialsContext.tsx:75`; structural |
| <a id="fn-wifiheaderstatus"></a>`WifiHeaderStatus({  connected,  activeView,  scanLoading,  networksCount,  stationPhase,  apSaving,  provisioning,  mode,  apIp,  connectionLabel,  connectionDetail,  aliveAt, }: { connected: boolean; activeView: WifiSectionView; scanLoading: boolean; networksCount: number; stationPhase: "idle" \| "submitting" \| "connecting"; apSaving: boolean; provisioning: boolean; mode: string; apIp: string; connectionLabel: string; connectionDetail?: string; aliveAt: number \| null; }): inferred-by-TypeScript` | Coordina `String`, `formatAliveLabel` para Estado Wi-Fi, AP y credenciales Web. | `web:src/pages/WifiSection.tsx:1887`; structural |
| <a id="fn-wifiicon"></a>`WifiIcon(): inferred-by-TypeScript` | Operación Wifi Icon de Estado Wi-Fi, AP y credenciales Web; semántica fina pendiente de ampliación. | `web:src/pages/WifiSection.tsx:2004`; inferred-name |
| <a id="fn-wifimodeswitchcard"></a>`WifiModeSwitchCard({  sessionOnly = false,  apFallback,  queryBusy,  onConsult,  currentMode,  destination,  connected,  busy,  configurationLoaded,  hasUnsavedChanges,  onOpen, }: { sessionOnly?: boolean; apFallback: ApFallbackController; queryBusy: boolean; onConsult: () => void; currentMode: "AP" \| "RED"; destination: WifiModeDestination; connected: boolean; busy: boolean; configurationLoaded: boolean; hasUnsavedChanges: boolean; onOpen: () => void; }): inferred-by-TypeScript` | Adapta la llamada a `formatDestinationIp` con el contexto de esta integración. | `web:src/pages/WifiSection.tsx:1786`; structural |
| <a id="fn-wifisection"></a>`WifiSection({ provisioning = false }: WifiSectionProps): inferred-by-TypeScript` | Coordina `String`, `apClients.map`, `availableNetworks.map` para Estado Wi-Fi, AP y credenciales Web. | `web:src/pages/WifiSection.tsx:88`; structural |
| <a id="fn-wifisignalicon"></a>`WifiSignalIcon({ signal }: { signal: number \| null }): inferred-by-TypeScript` | Coordina `Math.max`, `formatSignalPercent` para Estado Wi-Fi, AP y credenciales Web. | `web:src/pages/WifiSection.tsx:2025`; structural |
| <a id="fn-wifistatuspill"></a>`WifiStatusPill({  label,  value,  detail,  tone, }: { label: string; value: string; detail?: string; tone: "ok" \| "error" \| "info" \| "muted"; }): inferred-by-TypeScript` | Operación Wifi Status Pill de Estado Wi-Fi, AP y credenciales Web; semántica fina pendiente de ampliación. | `web:src/pages/WifiSection.tsx:1981`; inferred-name |
| <a id="fn-createrequestid"></a>`createRequestId(prefix: string): inferred-by-TypeScript` | Coordina `Date.now`, `Math.random`, `Math.random().toString` para Estado Wi-Fi, AP y credenciales Web. | `web:src/pages/WifiSection.tsx:2595`; structural |
| <a id="fn-decodewifissid"></a>`decodeWifiSsid(bytes: number[] \| undefined): string \| undefined` | Coordina `Uint8Array.from`, `wifiSsidDecoder.decode`, `wifiSsidDecoder.decode(Uint8Array.from(bytes)).replace` para Estado Wi-Fi, AP y credenciales Web. | `web:src/pages/WifiSection.tsx:2531`; structural |
| <a id="fn-formatalivelabel"></a>`formatAliveLabel(timestamp: number \| null): inferred-by-TypeScript` | Coordina `Date.now`, `Math.max`, `Math.round` para Estado Wi-Fi, AP y credenciales Web. | `web:src/pages/WifiSection.tsx:1975`; structural |
| <a id="fn-formatavailablenetworkssummary"></a>`formatAvailableNetworksSummary(visibleCount: number, reportedCount: number \| null): inferred-by-TypeScript` | Operación format Available Networks Summary de Estado Wi-Fi, AP y credenciales Web; semántica fina pendiente de ampliación. | `web:src/pages/WifiSection.tsx:2333`; inferred-name |
| <a id="fn-formatdestinationip"></a>`formatDestinationIp(destination: WifiModeDestination): inferred-by-TypeScript` | Operación format Destination Ip de Estado Wi-Fi, AP y credenciales Web; semántica fina pendiente de ampliación. | `web:src/pages/WifiSection.tsx:1864`; inferred-name |
| <a id="fn-formatencryption"></a>`formatEncryption(value: WifiNetworkDetail["encryptionType"]): inferred-by-TypeScript` | Adapta la llamada a `String` con el contexto de esta integración. | `web:src/pages/WifiSection.tsx:2167`; structural |
| <a id="fn-formatscanresultsfeedback"></a>`formatScanResultsFeedback(scanResult: WifiScanResult): inferred-by-TypeScript` | Operación format Scan Results Feedback de Estado Wi-Fi, AP y credenciales Web; semántica fina pendiente de ampliación. | `web:src/pages/WifiSection.tsx:2318`; inferred-name |
| <a id="fn-formatsignalpercent"></a>`formatSignalPercent(signal: number \| null): inferred-by-TypeScript` | Coordina `Math.max`, `Math.min`, `Math.round` para Estado Wi-Fi, AP y credenciales Web. | `web:src/pages/WifiSection.tsx:2046`; structural |
| <a id="fn-getdeviceeventdata"></a>`getDeviceEventData(message: unknown): Record<string, unknown>` | Coordina `isRecord`, `toRecord` para Estado Wi-Fi, AP y credenciales Web. | `web:src/pages/WifiSection.tsx:2462`; structural |
| <a id="fn-getdeviceeventname"></a>`getDeviceEventName(message: unknown): inferred-by-TypeScript` | Coordina `isRecord`, `readString`, `toRecord` para Estado Wi-Fi, AP y credenciales Web. | `web:src/pages/WifiSection.tsx:2454`; structural |
| <a id="fn-getespconnectiondetail"></a>`getEspConnectionDetail(status: EspWifiStatus \| null): inferred-by-TypeScript` | Adapta la llamada a `isEspApOnly` con el contexto de esta integración. | `web:src/contexts/EspWifiStatusContext.tsx:162`; structural |
| <a id="fn-getespconnectionlabel"></a>`getEspConnectionLabel(status: EspWifiStatus \| null): inferred-by-TypeScript` | Adapta la llamada a `isEspApOnly` con el contexto de esta integración. | `web:src/contexts/EspWifiStatusContext.tsx:156`; structural |
| <a id="fn-getespnetworklabel"></a>`getEspNetworkLabel(status: EspWifiStatus \| null): inferred-by-TypeScript` | Adapta la llamada a `isEspApOnly` con el contexto de esta integración. | `web:src/contexts/EspWifiStatusContext.tsx:168`; structural |
| <a id="fn-getfeedback"></a>`getFeedback(status: string, error: string \| null, ip: string \| null, timeoutMs: number \| null): inferred-by-TypeScript` | Adapta la llamada a `Math.round` con el contexto de esta integración. | `web:src/components/WifiCredentialsModal.tsx:170`; structural |
| <a id="fn-getresponsecommand"></a>`getResponseCommand(message: unknown): inferred-by-TypeScript` | Coordina `isRecord`, `readString`, `toRecord` para Estado Wi-Fi, AP y credenciales Web. | `web:src/pages/WifiSection.tsx:2471`; structural |
| <a id="fn-getresponsedata"></a>`getResponseData(message: unknown): inferred-by-TypeScript` | Coordina `isRecord`, `toRecord` para Estado Wi-Fi, AP y credenciales Web. | `web:src/pages/WifiSection.tsx:2494`; structural |
| <a id="fn-getresponseerrorcode"></a>`getResponseErrorCode(message: unknown): inferred-by-TypeScript` | Coordina `getResponseData`, `isRecord`, `readString` para Estado Wi-Fi, AP y credenciales Web. | `web:src/pages/WifiSection.tsx:2575`; structural |
| <a id="fn-getresponsemessage"></a>`getResponseMessage(message: unknown): inferred-by-TypeScript` | Coordina `getResponseData`, `isRecord`, `readString` para Estado Wi-Fi, AP y credenciales Web. | `web:src/pages/WifiSection.tsx:2503`; structural |
| <a id="fn-getresponseok"></a>`getResponseOk(message: unknown): inferred-by-TypeScript` | Coordina `isRecord`, `toRecord` para Estado Wi-Fi, AP y credenciales Web. | `web:src/pages/WifiSection.tsx:2484`; structural |
| <a id="fn-getwifinetworkdetailtext"></a>`getWifiNetworkDetailText(network: WifiNetworkDetail): inferred-by-TypeScript` | Coordina `String`, `formatEncryption` para Estado Wi-Fi, AP y credenciales Web. | `web:src/pages/WifiSection.tsx:2361`; structural |
| <a id="fn-getwifinetworkkey"></a>`getWifiNetworkKey(network: WifiNetworkDetail): inferred-by-TypeScript` | Operación get Wifi Network Key de Estado Wi-Fi, AP y credenciales Web; semántica fina pendiente de ampliación. | `web:src/pages/WifiSection.tsx:2348`; inferred-name |
| <a id="fn-getwifinetworklabel"></a>`getWifiNetworkLabel(network: WifiNetworkDetail): inferred-by-TypeScript` | Operación get Wifi Network Label de Estado Wi-Fi, AP y credenciales Web; semántica fina pendiente de ampliación. | `web:src/pages/WifiSection.tsx:2357`; inferred-name |
| <a id="fn-isespaponly"></a>`isEspApOnly(status: EspWifiStatus \| null): inferred-by-TypeScript` | Operación is Esp Ap Only de Estado Wi-Fi, AP y credenciales Web; semántica fina pendiente de ampliación. | `web:src/contexts/EspWifiStatusContext.tsx:152`; inferred-name |
| <a id="fn-isrecord"></a>`isRecord(value: unknown): value is Record<string, unknown>` | Operación is Record de Estado Wi-Fi, AP y credenciales Web; semántica fina pendiente de ampliación. | `web:src/pages/WifiSection.tsx:2626`; inferred-name |
| <a id="fn-issameipv4"></a>`isSameIpv4(left: string \| undefined, right: string \| undefined): boolean` | Coordina `/^\d{1,3}(?:\.\d{1,3}){3}$/.test`, `normalize` para Estado Wi-Fi, AP y credenciales Web. | `web:src/pages/WifiSection.tsx:2228`; structural |
| <a id="fn-isvalidipv4"></a>`isValidIpv4(value: string): inferred-by-TypeScript` | Coordina `parts.every`, `value.split` para Estado Wi-Fi, AP y credenciales Web. | `web:src/pages/WifiSection.tsx:2131`; structural |
| <a id="fn-logwifiscanarrival"></a>`logWifiScanArrival(source: "device.event" \| "device.response", message: unknown, data: unknown, scanResult: WifiScanResult): inferred-by-TypeScript` | Adapta la llamada a `console.log` con el contexto de esta integración. | `web:src/pages/WifiSection.tsx:2281`; structural |
| <a id="fn-networkrowstyle"></a>`networkRowStyle(selected: boolean): inferred-by-TypeScript` | Operación network Row Style de Estado Wi-Fi, AP y credenciales Web; semántica fina pendiente de ampliación. | `web:src/pages/WifiSection.tsx:2067`; inferred-name |
| <a id="fn-normalizeip"></a>`normalizeIp(value: string): inferred-by-TypeScript` | Distingue dirección configurada de vacío o dirección no especificada antes de construir destino. | `web:src/utils/wifiModeSwitch.ts:63`; editorial |
| <a id="fn-normalizependingcredentials"></a>`normalizePendingCredentials(data: Record<string, unknown>, fallback?: unknown): inferred-by-TypeScript` | Coordina `Boolean`, `readBoolean`, `readNumber` para Estado Wi-Fi, AP y credenciales Web. | `web:src/contexts/WifiCredentialsContext.tsx:596`; structural |
| <a id="fn-normalizewifinetwork"></a>`normalizeWifiNetwork(value: unknown, fallbackSsid?: string \| null, fallbackBssid?: string \| null, fallbackIndex?: number): WifiNetworkDetail \| null` | Coordina `decodeWifiSsid`, `isRecord`, `readAnyString` para Estado Wi-Fi, AP y credenciales Web. | `web:src/pages/WifiSection.tsx:2374`; structural |
| <a id="fn-normalizewifistatus"></a>`normalizeWifiStatus(data: Record<string, unknown>): EspWifiStatus` | Coordina `Math.round`, `readNumber`, `readString` para Estado Wi-Fi, AP y credenciales Web. | `web:src/contexts/EspWifiStatusContext.tsx:177`; structural |
| <a id="fn-readanystring"></a>`readAnyString(value: unknown): string \| undefined` | Operación read Any String de Estado Wi-Fi, AP y credenciales Web; semántica fina pendiente de ampliación. | `web:src/pages/WifiSection.tsx:2605`; inferred-name |
| <a id="fn-readapclients"></a>`readApClients(value: unknown): WifiApClient[]` | Coordina `Array.isArray`, `clients.flatMap`, `toRecord` para Estado Wi-Fi, AP y credenciales Web. | `web:src/pages/WifiSection.tsx:2212`; structural |
| <a id="fn-readboolean"></a>`readBoolean(value: unknown): boolean \| undefined` | Coordina `Number.isFinite`, `value.trim`, `value.trim().toLowerCase` para Estado Wi-Fi, AP y credenciales Web. | `web:src/contexts/WifiCredentialsContext.tsx:715`; structural |
| <a id="fn-readbytearray"></a>`readByteArray(value: unknown): number[] \| undefined` | Coordina `Array.isArray`, `value.map`, `value.some` para Estado Wi-Fi, AP y credenciales Web. | `web:src/pages/WifiSection.tsx:2520`; structural |
| <a id="fn-readeventname"></a>`readEventName(message: unknown): inferred-by-TypeScript` | Adapta la llamada a `isRecord` con el contexto de esta integración. | `web:src/contexts/EspWifiStatusContext.tsx:203`; structural |
| <a id="fn-readnumber"></a>`readNumber(value: unknown): inferred-by-TypeScript` | Coordina `Math.trunc`, `Number`, `Number.isFinite` para Estado Wi-Fi, AP y credenciales Web. | `web:src/pages/WifiSection.tsx:2609`; structural |
| <a id="fn-readreportednetworkcount"></a>`readReportedNetworkCount(value: Record<string, unknown> \| undefined, visibleCount: number): inferred-by-TypeScript` | Coordina `Math.max`, `readNumber` para Estado Wi-Fi, AP y credenciales Web. | `web:src/pages/WifiSection.tsx:2300`; structural |
| <a id="fn-readresultstatus"></a>`readResultStatus(value: unknown): WifiCredentialsResultStatus \| null` | Operación read Result Status de Estado Wi-Fi, AP y credenciales Web; semántica fina pendiente de ampliación. | `web:src/contexts/WifiCredentialsContext.tsx:672`; inferred-name |
| <a id="fn-readstring"></a>`readString(value: unknown): string \| undefined` | Operación read String de Estado Wi-Fi, AP y credenciales Web; semántica fina pendiente de ampliación. | `web:src/pages/WifiSection.tsx:2601`; inferred-name |
| <a id="fn-readwificredentialsstatus"></a>`readWifiCredentialsStatus(value: unknown): WifiCredentialsResultStatus \| null` | Operación read Wifi Credentials Status de Estado Wi-Fi, AP y credenciales Web; semántica fina pendiente de ampliación. | `web:src/pages/WifiSection.tsx:2182`; inferred-name |
| <a id="fn-readwifinetworks"></a>`readWifiNetworks(value: unknown): WifiNetworkDetail[]` | Coordina `Array.isArray`, `readByteArray`, `readWifiNetworksFromBytes` para Estado Wi-Fi, AP y credenciales Web. | `web:src/pages/WifiSection.tsx:2197`; structural |
| <a id="fn-readwifinetworksfrombytes"></a>`readWifiNetworksFromBytes(bytes: number[]): WifiNetworkDetail[]` | Coordina `[ parse(1, countFirst), parse(2, statusThenCount), ].filter`, `candidates.sort`, `parse` para Estado Wi-Fi, AP y credenciales Web. | `web:src/pages/WifiSection.tsx:2539`; structural |
| <a id="fn-readwifiscanresult"></a>`readWifiScanResult(value: unknown): WifiScanResult` | Coordina `Array.isArray`, `readReportedNetworkCount`, `readWifiNetworks` para Estado Wi-Fi, AP y credenciales Web. | `web:src/pages/WifiSection.tsx:2243`; structural |
| <a id="fn-resolveespoperatingmode"></a>`resolveEspOperatingMode(status: WifiStatusLike \| null): EspOperatingMode` | Deriva AP o RED del estado conocido de STA/AP para preparar la transición local. | `web:src/utils/wifiModeSwitch.ts:29`; editorial |
| <a id="fn-resolveoppositemodedestination"></a>`resolveOppositeModeDestination(currentMode: EspOperatingMode, config: WifiModeConfig): WifiModeDestination` | Resuelve SSID, IP conocida o DHCP y URL del modo opuesto; no acredita que el dispositivo ya haya reiniciado. | `web:src/utils/wifiModeSwitch.ts:35`; editorial |
| <a id="fn-responseerrortomessage"></a>`responseErrorToMessage(code: inferred-by-TypeScript): inferred-by-TypeScript` | Operación response Error To Message de Estado Wi-Fi, AP y credenciales Web; semántica fina pendiente de ampliación. | `web:src/contexts/WifiCredentialsContext.tsx:637`; inferred-name |
| <a id="fn-resultstatustomessage"></a>`resultStatusToMessage(status: WifiCredentialsResultStatus, reason?: string, ip?: string): inferred-by-TypeScript` | Operación result Status To Message de Estado Wi-Fi, AP y credenciales Web; semántica fina pendiente de ampliación. | `web:src/contexts/WifiCredentialsContext.tsx:653`; inferred-name |
| <a id="fn-segmentedbuttonstyle"></a>`segmentedButtonStyle(active: boolean): inferred-by-TypeScript` | Operación segmented Button Style de Estado Wi-Fi, AP y credenciales Web; semántica fina pendiente de ampliación. | `web:src/pages/WifiSection.tsx:2054`; inferred-name |
| <a id="fn-stationresultmessage"></a>`stationResultMessage(status: WifiCredentialsResultStatus, ssid: string, ip?: string, reason?: string): inferred-by-TypeScript` | Operación station Result Message de Estado Wi-Fi, AP y credenciales Web; semántica fina pendiente de ampliación. | `web:src/pages/WifiSection.tsx:2142`; inferred-name |
| <a id="fn-torecord"></a>`toRecord(value: unknown): Record<string, unknown> \| undefined` | Adapta la llamada a `isRecord` con el contexto de esta integración. | `web:src/pages/WifiSection.tsx:2622`; structural |
| <a id="fn-useapfallback"></a>`useApFallback(): inferred-by-TypeScript` | Coordina `useCallback`, `useEffect`, `useRef` para Estado Wi-Fi, AP y credenciales Web. | `web:src/hooks/useApFallback.ts:9`; structural |
| <a id="fn-useespwifistatus"></a>`useEspWifiStatus(): inferred-by-TypeScript` | Adapta la llamada a `useContext` con el contexto de esta integración. | `web:src/contexts/EspWifiStatusContext.tsx:144`; structural |
| <a id="fn-usewificredentials"></a>`useWifiCredentials(): inferred-by-TypeScript` | Adapta la llamada a `useContext` con el contexto de esta integración. | `web:src/contexts/WifiCredentialsContext.tsx:475`; structural |
| <a id="fn-validateapcredentials"></a>`validateApCredentials(ssid: string, password: string, ip: string): inferred-by-TypeScript` | Coordina `isValidIpv4`, `ssid.trim` para Estado Wi-Fi, AP y credenciales Web. | `web:src/pages/WifiSection.tsx:2108`; structural |
| <a id="fn-validatestationcredentials"></a>`validateStationCredentials(ssid: string, password: string, reusePassword: boolean, ip: string, gateway: string, subnet: string): inferred-by-TypeScript` | Coordina `isValidIpv4`, `ssid.trim` para Estado Wi-Fi, AP y credenciales Web. | `web:src/pages/WifiSection.tsx:2079`; structural |
| <a id="fn-validatesubmit"></a>`validateSubmit(state: WifiCredentialsState, ssid: string, password: string): inferred-by-TypeScript` | Adapta la llamada a `ssid.trim` con el contexto de esta integración. | `web:src/contexts/WifiCredentialsContext.tsx:487`; structural |

## Funciones y callbacks internos

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-wificredentialsmodal.handlecancel"></a>`WifiCredentialsModal.handleCancel(): inferred-by-TypeScript` | Coordina `onCancel`, `onDismiss` para Estado Wi-Fi, AP y credenciales Web. | `web:src/components/WifiCredentialsModal.tsx:74`; structural |
| <a id="fn-wificredentialsmodal.handleclose"></a>`WifiCredentialsModal.handleClose(): inferred-by-TypeScript` | Coordina `onCancel`, `onDismiss` para Estado Wi-Fi, AP y credenciales Web. | `web:src/components/WifiCredentialsModal.tsx:53`; structural |
| <a id="fn-wificredentialsmodal.handlesubmit"></a>`WifiCredentialsModal.handleSubmit(event: FormEvent<HTMLFormElement>): inferred-by-TypeScript` | Coordina `event.preventDefault`, `onSubmit` para Estado Wi-Fi, AP y credenciales Web. | `web:src/components/WifiCredentialsModal.tsx:64`; structural |
| <a id="fn-wifisection.closemodeswitchconfirmation"></a>`WifiSection.closeModeSwitchConfirmation(): inferred-by-TypeScript` | Coordina `setModeSwitchDestination`, `setModeSwitchError`, `setModeSwitchPhase` para Estado Wi-Fi, AP y credenciales Web. | `web:src/pages/WifiSection.tsx:1036`; structural |
| <a id="fn-wifisection.confirmmodeswitch"></a>`WifiSection.confirmModeSwitch(): inferred-by-TypeScript` | Coordina `createRequestId`, `readAnyString`, `request` para Estado Wi-Fi, AP y credenciales Web. | `web:src/pages/WifiSection.tsx:1043`; structural |
| <a id="fn-wifisection.openmodeswitchconfirmation"></a>`WifiSection.openModeSwitchConfirmation(): inferred-by-TypeScript` | Coordina `setModeSwitchDestination`, `setModeSwitchError`, `setModeSwitchPhase` para Estado Wi-Fi, AP y credenciales Web. | `web:src/pages/WifiSection.tsx:1030`; structural |
| <a id="fn-wifisection.retrystation"></a>`WifiSection.retryStation(): inferred-by-TypeScript` | Coordina `refreshWifiStatus`, `request`, `setStationFeedback` para Estado Wi-Fi, AP y credenciales Web. | `web:src/pages/WifiSection.tsx:981`; structural |
| <a id="fn-issameipv4.normalize"></a>`isSameIpv4.normalize(value: string): inferred-by-TypeScript` | Coordina `value.trim`, `value.trim().replace` para Estado Wi-Fi, AP y credenciales Web. | `web:src/pages/WifiSection.tsx:2230`; structural |
| <a id="fn-readwifinetworksfrombytes.parse"></a>`readWifiNetworksFromBytes.parse(offset: number, count: number): inferred-by-TypeScript` | Coordina `bytes.slice`, `decodeWifiSsid`, `networks.push` para Estado Wi-Fi, AP y credenciales Web. | `web:src/pages/WifiSection.tsx:2540`; structural |

## Estado y efectos React encontrados

| Hook | Función/closure | Evidencia |
| --- | --- | --- |
| `useLocation` | `ApProvisioningGate` | `web:src/components/ApProvisioningGate.tsx:6` |
| `useNavigate` | `ApProvisioningGate` | `web:src/components/ApProvisioningGate.tsx:7` |
| `useUser` | `ApProvisioningGate` | `web:src/components/ApProvisioningGate.tsx:8` |
| `useEffect` | `ApProvisioningGate` | `web:src/components/ApProvisioningGate.tsx:10` |
| `useState` | `WifiCredentialsModal` | `web:src/components/WifiCredentialsModal.tsx:30` |
| `useEffect` | `WifiCredentialsModal` | `web:src/components/WifiCredentialsModal.tsx:32` |
| `useMemo` | `WifiCredentialsModal` | `web:src/components/WifiCredentialsModal.tsx:48` |
| `useWebSocket` | `EspWifiStatusProvider` | `web:src/contexts/EspWifiStatusContext.tsx:49` |
| `useState` | `EspWifiStatusProvider` | `web:src/contexts/EspWifiStatusContext.tsx:50` |
| `useState` | `EspWifiStatusProvider` | `web:src/contexts/EspWifiStatusContext.tsx:51` |
| `useState` | `EspWifiStatusProvider` | `web:src/contexts/EspWifiStatusContext.tsx:52` |
| `useState` | `EspWifiStatusProvider` | `web:src/contexts/EspWifiStatusContext.tsx:53` |
| `useState` | `EspWifiStatusProvider` | `web:src/contexts/EspWifiStatusContext.tsx:54` |
| `useRef` | `EspWifiStatusProvider` | `web:src/contexts/EspWifiStatusContext.tsx:55` |
| `useCallback` | `EspWifiStatusProvider` | `web:src/contexts/EspWifiStatusContext.tsx:57` |
| `useEffect` | `EspWifiStatusProvider` | `web:src/contexts/EspWifiStatusContext.tsx:93` |
| `useEffect` | `EspWifiStatusProvider` | `web:src/contexts/EspWifiStatusContext.tsx:106` |
| `useEffect` | `EspWifiStatusProvider` | `web:src/contexts/EspWifiStatusContext.tsx:121` |
| `useMemo` | `EspWifiStatusProvider` | `web:src/contexts/EspWifiStatusContext.tsx:127` |
| `useContext` | `useEspWifiStatus` | `web:src/contexts/EspWifiStatusContext.tsx:145` |
| `useWebSocket` | `WifiCredentialsProvider` | `web:src/contexts/WifiCredentialsContext.tsx:78` |
| `useUser` | `WifiCredentialsProvider` | `web:src/contexts/WifiCredentialsContext.tsx:79` |
| `useEspWifiStatus` | `WifiCredentialsProvider` | `web:src/contexts/WifiCredentialsContext.tsx:81` |
| `useState` | `WifiCredentialsProvider` | `web:src/contexts/WifiCredentialsContext.tsx:82` |
| `useRef` | `WifiCredentialsProvider` | `web:src/contexts/WifiCredentialsContext.tsx:83` |
| `useRef` | `WifiCredentialsProvider` | `web:src/contexts/WifiCredentialsContext.tsx:84` |
| `useCallback` | `WifiCredentialsProvider` | `web:src/contexts/WifiCredentialsContext.tsx:86` |
| `useCallback` | `WifiCredentialsProvider` | `web:src/contexts/WifiCredentialsContext.tsx:93` |
| `useCallback` | `WifiCredentialsProvider` | `web:src/contexts/WifiCredentialsContext.tsx:98` |
| `useCallback` | `WifiCredentialsProvider` | `web:src/contexts/WifiCredentialsContext.tsx:106` |
| `useCallback` | `WifiCredentialsProvider` | `web:src/contexts/WifiCredentialsContext.tsx:123` |
| `useCallback` | `WifiCredentialsProvider` | `web:src/contexts/WifiCredentialsContext.tsx:159` |
| `useCallback` | `WifiCredentialsProvider` | `web:src/contexts/WifiCredentialsContext.tsx:186` |
| `useEffect` | `WifiCredentialsProvider` | `web:src/contexts/WifiCredentialsContext.tsx:216` |
| `useEffect` | `WifiCredentialsProvider` | `web:src/contexts/WifiCredentialsContext.tsx:371` |
| `useEffect` | `WifiCredentialsProvider` | `web:src/contexts/WifiCredentialsContext.tsx:396` |
| `useEffect` | `WifiCredentialsProvider` | `web:src/contexts/WifiCredentialsContext.tsx:411` |
| `useEffect` | `WifiCredentialsProvider` | `web:src/contexts/WifiCredentialsContext.tsx:418` |
| `useEffect` | `WifiCredentialsProvider` | `web:src/contexts/WifiCredentialsContext.tsx:424` |
| `useRef` | `WifiCredentialsProvider` | `web:src/contexts/WifiCredentialsContext.tsx:430` |
| `useEffect` | `WifiCredentialsProvider` | `web:src/contexts/WifiCredentialsContext.tsx:431` |
| `useContext` | `useWifiCredentials` | `web:src/contexts/WifiCredentialsContext.tsx:476` |
| `useWebSocket` | `useApFallback` | `web:src/hooks/useApFallback.ts:10` |
| `useUser` | `useApFallback` | `web:src/hooks/useApFallback.ts:11` |
| `useState` | `useApFallback` | `web:src/hooks/useApFallback.ts:14` |
| `useState` | `useApFallback` | `web:src/hooks/useApFallback.ts:15` |
| `useState` | `useApFallback` | `web:src/hooks/useApFallback.ts:16` |
| `useRef` | `useApFallback` | `web:src/hooks/useApFallback.ts:17` |
| `useRef` | `useApFallback` | `web:src/hooks/useApFallback.ts:18` |
| `useCallback` | `useApFallback` | `web:src/hooks/useApFallback.ts:20` |
| `useEffect` | `useApFallback` | `web:src/hooks/useApFallback.ts:30` |
| `useCallback` | `useApFallback` | `web:src/hooks/useApFallback.ts:37` |
| `useCallback` | `useApFallback` | `web:src/hooks/useApFallback.ts:52` |
| `useCallback` | `useApFallback` | `web:src/hooks/useApFallback.ts:53` |
| `useWebSocket` | `WifiSection` | `web:src/pages/WifiSection.tsx:89` |
| `useUser` | `WifiSection` | `web:src/pages/WifiSection.tsx:90` |
| `useEspWifiStatus` | `WifiSection` | `web:src/pages/WifiSection.tsx:98` |
| `useApFallback` | `WifiSection` | `web:src/pages/WifiSection.tsx:99` |
| `useState` | `WifiSection` | `web:src/pages/WifiSection.tsx:100` |
| `useState` | `WifiSection` | `web:src/pages/WifiSection.tsx:103` |
| `useState` | `WifiSection` | `web:src/pages/WifiSection.tsx:104` |
| `useState` | `WifiSection` | `web:src/pages/WifiSection.tsx:105` |
| `useState` | `WifiSection` | `web:src/pages/WifiSection.tsx:106` |
| `useState` | `WifiSection` | `web:src/pages/WifiSection.tsx:107` |
| `useState` | `WifiSection` | `web:src/pages/WifiSection.tsx:108` |
| `useState` | `WifiSection` | `web:src/pages/WifiSection.tsx:109` |
| `useState` | `WifiSection` | `web:src/pages/WifiSection.tsx:110` |
| `useState` | `WifiSection` | `web:src/pages/WifiSection.tsx:111` |
| `useState` | `WifiSection` | `web:src/pages/WifiSection.tsx:114` |
| `useState` | `WifiSection` | `web:src/pages/WifiSection.tsx:117` |
| `useState` | `WifiSection` | `web:src/pages/WifiSection.tsx:120` |
| `useState` | `WifiSection` | `web:src/pages/WifiSection.tsx:121` |
| `useState` | `WifiSection` | `web:src/pages/WifiSection.tsx:122` |
| `useState` | `WifiSection` | `web:src/pages/WifiSection.tsx:125` |
| `useState` | `WifiSection` | `web:src/pages/WifiSection.tsx:126` |
| `useState` | `WifiSection` | `web:src/pages/WifiSection.tsx:127` |
| `useState` | `WifiSection` | `web:src/pages/WifiSection.tsx:128` |
| `useState` | `WifiSection` | `web:src/pages/WifiSection.tsx:129` |
| `useState` | `WifiSection` | `web:src/pages/WifiSection.tsx:130` |
| `useState` | `WifiSection` | `web:src/pages/WifiSection.tsx:133` |
| `useState` | `WifiSection` | `web:src/pages/WifiSection.tsx:136` |
| `useState` | `WifiSection` | `web:src/pages/WifiSection.tsx:137` |
| `useState` | `WifiSection` | `web:src/pages/WifiSection.tsx:139` |
| `useState` | `WifiSection` | `web:src/pages/WifiSection.tsx:149` |
| `useState` | `WifiSection` | `web:src/pages/WifiSection.tsx:151` |
| `useState` | `WifiSection` | `web:src/pages/WifiSection.tsx:152` |
| `useRef` | `WifiSection` | `web:src/pages/WifiSection.tsx:154` |
| `useRef` | `WifiSection` | `web:src/pages/WifiSection.tsx:155` |
| `useRef` | `WifiSection` | `web:src/pages/WifiSection.tsx:160` |
| `useRef` | `WifiSection` | `web:src/pages/WifiSection.tsx:161` |
| `useRef` | `WifiSection` | `web:src/pages/WifiSection.tsx:162` |
| `useRef` | `WifiSection` | `web:src/pages/WifiSection.tsx:163` |
| `useRef` | `WifiSection` | `web:src/pages/WifiSection.tsx:164` |
| `useRef` | `WifiSection` | `web:src/pages/WifiSection.tsx:165` |
| `useEffect` | `WifiSection` | `web:src/pages/WifiSection.tsx:167` |
| `useEffect` | `WifiSection` | `web:src/pages/WifiSection.tsx:176` |
| `useCallback` | `WifiSection` | `web:src/pages/WifiSection.tsx:182` |
| `useCallback` | `WifiSection` | `web:src/pages/WifiSection.tsx:287` |
| `useCallback` | `WifiSection` | `web:src/pages/WifiSection.tsx:312` |
| `useEffect` | `WifiSection` | `web:src/pages/WifiSection.tsx:338` |
| `useEffect` | `WifiSection` | `web:src/pages/WifiSection.tsx:347` |
| `useCallback` | `WifiSection` | `web:src/pages/WifiSection.tsx:355` |
| `useCallback` | `WifiSection` | `web:src/pages/WifiSection.tsx:400` |
| `useCallback` | `WifiSection` | `web:src/pages/WifiSection.tsx:426` |
| `useCallback` | `WifiSection` | `web:src/pages/WifiSection.tsx:446` |
| `useCallback` | `WifiSection` | `web:src/pages/WifiSection.tsx:514` |
| `useEffect` | `WifiSection` | `web:src/pages/WifiSection.tsx:592` |
| `useEffect` | `WifiSection` | `web:src/pages/WifiSection.tsx:720` |
| `useMemo` | `WifiSection` | `web:src/pages/WifiSection.tsx:998` |
| `useMemo` | `WifiSection` | `web:src/pages/WifiSection.tsx:1100` |
