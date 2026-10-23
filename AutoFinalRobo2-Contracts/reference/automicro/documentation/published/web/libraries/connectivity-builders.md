# Renderers Web de conectividad y avisos

ID: `web:module/src/screens/connectivityScreens.ts` · Capa: **núcleo** · Fotografía: 2026-10-20.

Documentación de fuente; sin tests, compilación, navegador ni validación física.

## Responsabilidad y alcance

Construye estado Wi-Fi, búsqueda, resultados/credenciales y avisos de enlaces.

Primitivas de pantallas/avisos, SSID/IP/número de cliente y progreso; la red real pertenece al ESP/F4.

<a id="modularity"></a>
## Modularidad observada

Builders puros, con assets y convenciones OLED del proyecto.

<a id="state"></a>
## Handle, estado y atributos externos

Args de contenido y NotificationProgressArgs; el tiempo reportado no incluye necesariamente la fase exacta del refresco físico.

<a id="lifecycle"></a>
## Ciclo de vida y contexto

- Elegir variante por código/source y datos, después dibujar.
- Un mismo ID puede representar aviso o vista completa; usar discriminante confirmado.

<a id="interactions"></a>
## Interacciones

- reports.inc publica tiempos y clientNumber cuando existen en el payload.
- F4 diferencia Cliente Wi-Fi N y Web N en su cola.

<a id="limitations"></a>
## Precauciones y límites

- Defaults de SSID/IP/contador no son prueba del dato del equipo.
- No elegir variante transitoria ON/OFF a partir del último GET de estado si el evento no la especifica.

## Hallazgos conectados


## Archivos y evidencia

| Archivo | Análisis | Hash SHA-256 |
| --- | --- | --- |
| `src/screens/connectivityScreens.ts` | typescript-ast-static | `86124559644319c516ef90557f3a3d83b09e082010caac846c483c96de21e9e1` |
| `src/screens/notificationScreens.ts` | typescript-ast-static | `23137d455a656d70ca07dfe0dd18e3885bd185cb70f27894447a4917bdb1e738` |

## Tipos, puertos, props y callbacks

En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.

### `WifiStatusState`

`web:src/screens/connectivityScreens.ts:46`

Tipo: `"sta" \| "ap" \| "disconnected"`.

### `WifiStatusArgs`

`web:src/screens/connectivityScreens.ts:48`

| Atributo / firma | Propósito |
| --- | --- |
| `state: WifiStatusState` | Dato de estado/configuración `state`; declaración estructural, interpretación específica pendiente de ampliar. |
| `ssid?: string` | Dato de estado/configuración `ssid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `ipAddress?: string` | Dato de estado/configuración `ipAddress`; declaración estructural, interpretación específica pendiente de ampliar. |

### `WifiAtCredentialsArgs`

`web:src/screens/connectivityScreens.ts:207`

| Atributo / firma | Propósito |
| --- | --- |
| `ssid?: string` | Dato de estado/configuración `ssid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `passwordLength?: number` | Dato de estado/configuración `passwordLength`; declaración estructural, interpretación específica pendiente de ampliar. |
| `selectedCharacter?: string` | Dato de estado/configuración `selectedCharacter`; declaración estructural, interpretación específica pendiente de ampliar. |


## Funciones públicas / componentes / hooks

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-buildscreen010105remotemodechangednotificationcommands"></a>`buildScreen010105RemoteModeChangedNotificationCommands(args: { mode: string }, progress?: NotificationProgressArgs): OledCommand[]` | Coordina `["IDLE", "FOLLOW", "TEST"].includes`, `baselineY`, `center7x10` para Renderers Web de conectividad y avisos. | `web:src/screens/notificationScreens.ts:19`; structural |
| <a id="fn-buildscreen020201wifistatuscommands"></a>`buildScreen020201WifiStatusCommands(args: WifiStatusArgs): OledCommand[]` | Coordina `baselineY`, `clear`, `drawXbm` para Renderers Web de conectividad y avisos. | `web:src/screens/connectivityScreens.ts:60`; structural |
| <a id="fn-buildscreen020202wifisearchcommands"></a>`buildScreen020202WifiSearchCommands(args: { secondsRemaining: number }): OledCommand[]` | Coordina `String`, `baselineY`, `clear` para Renderers Web de conectividad y avisos. | `web:src/screens/connectivityScreens.ts:93`; structural |
| <a id="fn-buildscreen020204wifinotconnectednotificationcommands"></a>`buildScreen020204WifiNotConnectedNotificationCommands(progress?: NotificationProgressArgs): OledCommand[]` | Coordina `baselineY`, `clear`, `drawXbm` para Renderers Web de conectividad y avisos. | `web:src/screens/connectivityScreens.ts:109`; structural |
| <a id="fn-buildscreen020205wificonnectingcommands"></a>`buildScreen020205WifiConnectingCommands(args: { ssid: string }, progress?: NotificationProgressArgs): OledCommand[]` | Coordina `baselineY`, `clear`, `drawXbm` para Renderers Web de conectividad y avisos. | `web:src/screens/connectivityScreens.ts:121`; structural |
| <a id="fn-buildscreen020206wificonnectedcommands"></a>`buildScreen020206WifiConnectedCommands(args: { ssid: string; ipAddress: string }, progress?: NotificationProgressArgs): OledCommand[]` | Coordina `baselineY`, `clear`, `drawXbm` para Renderers Web de conectividad y avisos. | `web:src/screens/connectivityScreens.ts:133`; structural |
| <a id="fn-buildscreen020207wifisearchcompletenotificationcommands"></a>`buildScreen020207WifiSearchCompleteNotificationCommands(progress?: NotificationProgressArgs): OledCommand[]` | Coordina `baselineY`, `clear`, `drawXbm` para Renderers Web de conectividad y avisos. | `web:src/screens/connectivityScreens.ts:147`; structural |
| <a id="fn-buildscreen020208wifisearchcancelednotificationcommands"></a>`buildScreen020208WifiSearchCanceledNotificationCommands(progress?: NotificationProgressArgs): OledCommand[]` | Coordina `baselineY`, `clear`, `drawXbm` para Renderers Web de conectividad y avisos. | `web:src/screens/connectivityScreens.ts:159`; structural |
| <a id="fn-buildscreen02020awificredentialswebcommands"></a>`buildScreen02020aWifiCredentialsWebCommands(args: { ssid?: string }, progress?: NotificationProgressArgs): OledCommand[]` | Coordina `baselineY`, `clear`, `drawXbm` para Renderers Web de conectividad y avisos. | `web:src/screens/connectivityScreens.ts:171`; structural |
| <a id="fn-buildscreen02020bwificredentialssucceededcommands"></a>`buildScreen02020bWifiCredentialsSucceededCommands(args: { ssid?: string }, progress?: NotificationProgressArgs): OledCommand[]` | Coordina `baselineY`, `clear`, `drawXbm` para Renderers Web de conectividad y avisos. | `web:src/screens/connectivityScreens.ts:184`; structural |
| <a id="fn-buildscreen02020cwificredentialsfailedcommands"></a>`buildScreen02020cWifiCredentialsFailedCommands(args: { ssid?: string }, progress?: NotificationProgressArgs): OledCommand[]` | Coordina `baselineY`, `clear`, `drawXbm` para Renderers Web de conectividad y avisos. | `web:src/screens/connectivityScreens.ts:196`; structural |
| <a id="fn-buildscreen02020dwificredentialsatcommands"></a>`buildScreen02020dWifiCredentialsAtCommands(args: WifiAtCredentialsArgs): OledCommand[]` | Coordina `"*".repeat`, `Math.max`, `Math.min` para Renderers Web de conectividad y avisos. | `web:src/screens/connectivityScreens.ts:215`; structural |
| <a id="fn-buildscreen020302espcheckingconnectionnotificationcommands"></a>`buildScreen020302EspCheckingConnectionNotificationCommands(progress?: NotificationProgressArgs): OledCommand[]` | Coordina `baselineY`, `clear`, `setWhite` para Renderers Web de conectividad y avisos. | `web:src/screens/connectivityScreens.ts:235`; structural |
| <a id="fn-buildscreen020303espfirmwarerequestnotificationcommands"></a>`buildScreen020303EspFirmwareRequestNotificationCommands(progress?: NotificationProgressArgs): OledCommand[]` | Coordina `baselineY`, `clear`, `drawXbm` para Renderers Web de conectividad y avisos. | `web:src/screens/connectivityScreens.ts:246`; structural |
| <a id="fn-buildscreen020304espresetsentnotificationcommands"></a>`buildScreen020304EspResetSentNotificationCommands(progress?: NotificationProgressArgs): OledCommand[]` | Coordina `baselineY`, `clear`, `drawXbm` para Renderers Web de conectividad y avisos. | `web:src/screens/connectivityScreens.ts:260`; structural |
| <a id="fn-buildscreen020305espcheckconnectionrequirednotificationcommands"></a>`buildScreen020305EspCheckConnectionRequiredNotificationCommands(progress?: NotificationProgressArgs): OledCommand[]` | Coordina `baselineY`, `clear`, `setWhite` para Renderers Web de conectividad y avisos. | `web:src/screens/connectivityScreens.ts:274`; structural |
| <a id="fn-buildscreen020306espbootreceivednotificationcommands"></a>`buildScreen020306EspBootReceivedNotificationCommands(progress?: NotificationProgressArgs): OledCommand[]` | Coordina `baselineY`, `clear`, `drawXbmMsb` para Renderers Web de conectividad y avisos. | `web:src/screens/connectivityScreens.ts:285`; structural |
| <a id="fn-buildscreen020307espfirmwarereceivednotificationcommands"></a>`buildScreen020307EspFirmwareReceivedNotificationCommands(args: { firmwareVersion: string }, progress?: NotificationProgressArgs): OledCommand[]` | Coordina `baselineY`, `clear`, `drawXbm` para Renderers Web de conectividad y avisos. | `web:src/screens/connectivityScreens.ts:346`; structural |
| <a id="fn-buildscreen020307espfirmwarescreencommands"></a>`buildScreen020307EspFirmwareScreenCommands(args: { firmwareVersion?: string }): OledCommand[]` | Coordina `baselineY`, `clear`, `drawXbm` para Renderers Web de conectividad y avisos. | `web:src/screens/connectivityScreens.ts:333`; structural |
| <a id="fn-buildscreen020308espmodechangednotificationcommands"></a>`buildScreen020308EspModeChangedNotificationCommands(progress?: NotificationProgressArgs): OledCommand[]` | Coordina `baselineY`, `clear`, `drawXbm` para Renderers Web de conectividad y avisos. | `web:src/screens/connectivityScreens.ts:358`; structural |
| <a id="fn-buildscreen020309espapstartednotificationcommands"></a>`buildScreen020309EspApStartedNotificationCommands(args: { ipAddress: string }, progress?: NotificationProgressArgs): OledCommand[]` | Coordina `baselineY`, `clear`, `drawXbm` para Renderers Web de conectividad y avisos. | `web:src/screens/connectivityScreens.ts:370`; structural |
| <a id="fn-buildscreen02030bespunresponsivenotificationcommands"></a>`buildScreen02030bEspUnresponsiveNotificationCommands(progress?: NotificationProgressArgs): OledCommand[]` | Coordina `baselineY`, `clear`, `drawXbm` para Renderers Web de conectividad y avisos. | `web:src/screens/connectivityScreens.ts:297`; structural |
| <a id="fn-buildscreen02030cespwatchdogresetnotificationcommands"></a>`buildScreen02030cEspWatchdogResetNotificationCommands(progress?: NotificationProgressArgs): OledCommand[]` | Coordina `baselineY`, `clear`, `drawXbm` para Renderers Web de conectividad y avisos. | `web:src/screens/connectivityScreens.ts:309`; structural |
| <a id="fn-buildscreen02030despexceptionresetnotificationcommands"></a>`buildScreen02030dEspExceptionResetNotificationCommands(progress?: NotificationProgressArgs): OledCommand[]` | Coordina `baselineY`, `clear`, `drawXbm` para Renderers Web de conectividad y avisos. | `web:src/screens/connectivityScreens.ts:321`; structural |
| <a id="fn-buildscreen020401usbconnectednotificationcommands"></a>`buildScreen020401UsbConnectedNotificationCommands(progress?: NotificationProgressArgs): OledCommand[]` | Coordina `baselineY`, `clear`, `drawXbm` para Renderers Web de conectividad y avisos. | `web:src/screens/connectivityScreens.ts:382`; structural |
| <a id="fn-buildscreen020402usbdisconnectednotificationcommands"></a>`buildScreen020402UsbDisconnectedNotificationCommands(progress?: NotificationProgressArgs): OledCommand[]` | Coordina `baselineY`, `clear`, `drawXbm` para Renderers Web de conectividad y avisos. | `web:src/screens/connectivityScreens.ts:394`; structural |
| <a id="fn-buildscreen020501webserverupnotificationcommands"></a>`buildScreen020501WebServerUpNotificationCommands(progress?: NotificationProgressArgs): OledCommand[]` | Coordina `baselineY`, `clear`, `drawXbm` para Renderers Web de conectividad y avisos. | `web:src/screens/connectivityScreens.ts:406`; structural |
| <a id="fn-buildscreen020502webclientconnectednotificationcommands"></a>`buildScreen020502WebClientConnectedNotificationCommands(progress?: NotificationProgressArgs, clientNumber: inferred-by-TypeScript): OledCommand[]` | Adapta la llamada a `numberedClientNotification` con el contexto de esta integración. | `web:src/screens/connectivityScreens.ts:436`; structural |
| <a id="fn-buildscreen020503webclientdisconnectednotificationcommands"></a>`buildScreen020503WebClientDisconnectedNotificationCommands(progress?: NotificationProgressArgs, clientNumber: inferred-by-TypeScript): OledCommand[]` | Adapta la llamada a `numberedClientNotification` con el contexto de esta integración. | `web:src/screens/connectivityScreens.ts:441`; structural |
| <a id="fn-buildscreen020601apdeviceconnectednotificationcommands"></a>`buildScreen020601ApDeviceConnectedNotificationCommands(progress?: NotificationProgressArgs, clientNumber: inferred-by-TypeScript): OledCommand[]` | Adapta la llamada a `numberedClientNotification` con el contexto de esta integración. | `web:src/screens/connectivityScreens.ts:446`; structural |
| <a id="fn-buildscreen020602apdevicedisconnectednotificationcommands"></a>`buildScreen020602ApDeviceDisconnectedNotificationCommands(progress?: NotificationProgressArgs, clientNumber: inferred-by-TypeScript): OledCommand[]` | Adapta la llamada a `numberedClientNotification` con el contexto de esta integración. | `web:src/screens/connectivityScreens.ts:451`; structural |
| <a id="fn-buildscreen050417unerforwardingenablednotificationcommands"></a>`buildScreen050417UnerForwardingEnabledNotificationCommands(progress?: NotificationProgressArgs): OledCommand[]` | Adapta la llamada a `buildUnerForwardingNotificationCommands` con el contexto de esta integración. | `web:src/screens/notificationScreens.ts:63`; structural |
| <a id="fn-buildscreen050418unerforwardingdisablednotificationcommands"></a>`buildScreen050418UnerForwardingDisabledNotificationCommands(progress?: NotificationProgressArgs): OledCommand[]` | Adapta la llamada a `buildUnerForwardingNotificationCommands` con el contexto de esta integración. | `web:src/screens/notificationScreens.ts:69`; structural |
| <a id="fn-buildscreen060101controllerconnectednotificationcommands"></a>`buildScreen060101ControllerConnectedNotificationCommands(progress?: NotificationProgressArgs): OledCommand[]` | Coordina `baselineY`, `clear`, `drawXbm` para Renderers Web de conectividad y avisos. | `web:src/screens/notificationScreens.ts:77`; structural |
| <a id="fn-buildscreen060102controllerdisconnectednotificationcommands"></a>`buildScreen060102ControllerDisconnectedNotificationCommands(progress?: NotificationProgressArgs): OledCommand[]` | Coordina `baselineY`, `clear`, `drawXbm` para Renderers Web de conectividad y avisos. | `web:src/screens/notificationScreens.ts:89`; structural |
| <a id="fn-buildscreen060201commandreceivednotificationcommands"></a>`buildScreen060201CommandReceivedNotificationCommands(progress?: NotificationProgressArgs): OledCommand[]` | Coordina `baselineY`, `clear`, `drawXbm` para Renderers Web de conectividad y avisos. | `web:src/screens/notificationScreens.ts:106`; structural |
| <a id="fn-buildscreen060202pingreceivednotificationcommands"></a>`buildScreen060202PingReceivedNotificationCommands(progress?: NotificationProgressArgs): OledCommand[]` | Coordina `baselineY`, `clear`, `drawXbm` para Renderers Web de conectividad y avisos. | `web:src/screens/notificationScreens.ts:118`; structural |
| <a id="fn-buildscreen060301espconnectionsucceededcommands"></a>`buildScreen060301EspConnectionSucceededCommands(): OledCommand[]` | Coordina `baselineY`, `clear`, `drawXbm` para Renderers Web de conectividad y avisos. | `web:src/screens/notificationScreens.ts:130`; structural |
| <a id="fn-buildscreen060302espconnectionfailedcommands"></a>`buildScreen060302EspConnectionFailedCommands(): OledCommand[]` | Coordina `baselineY`, `clear`, `drawXbm` para Renderers Web de conectividad y avisos. | `web:src/screens/notificationScreens.ts:143`; structural |
| <a id="fn-buildunerforwardingnotificationcommands"></a>`buildUnerForwardingNotificationCommands(enabled: boolean, progress?: NotificationProgressArgs): OledCommand[]` | Coordina `baselineY`, `centerSmallNotificationTextX`, `clear` para Renderers Web de conectividad y avisos. | `web:src/screens/notificationScreens.ts:38`; structural |
| <a id="fn-centersmallnotificationtextx"></a>`centerSmallNotificationTextX(text: string): inferred-by-TypeScript` | Coordina `Math.floor`, `Math.max` para Renderers Web de conectividad y avisos. | `web:src/screens/notificationScreens.ts:35`; structural |
| <a id="fn-notification"></a>`notification(commands: OledCommand[], progress?: NotificationProgressArgs): OledCommand[]` | Adapta la llamada a `withNotificationProgress` con el contexto de esta integración. | `web:src/screens/notificationScreens.ts:13`; structural |
| <a id="fn-numberedclientnotification"></a>`numberedClientNotification(label: string, connected: boolean, web: boolean, progress?: NotificationProgressArgs, clientNumber: inferred-by-TypeScript): OledCommand[]` | Coordina `Number.isInteger`, `baselineY`, `center` para Renderers Web de conectividad y avisos. | `web:src/screens/connectivityScreens.ts:416`; structural |
| <a id="fn-simplenotification"></a>`simpleNotification(commands: OledCommand[], progress?: NotificationProgressArgs): OledCommand[]` | Adapta la llamada a `withNotificationProgress` con el contexto de esta integración. | `web:src/screens/connectivityScreens.ts:54`; structural |

## Funciones y callbacks internos

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-buildscreen010105remotemodechangednotificationcommands.center7x10"></a>`buildScreen010105RemoteModeChangedNotificationCommands.center7x10(text: string): inferred-by-TypeScript` | Coordina `Math.floor`, `Math.max` para Renderers Web de conectividad y avisos. | `web:src/screens/notificationScreens.ts:25`; structural |
| <a id="fn-numberedclientnotification.center"></a>`numberedClientNotification.center(text: string): inferred-by-TypeScript` | Coordina `Math.floor`, `Math.max` para Renderers Web de conectividad y avisos. | `web:src/screens/connectivityScreens.ts:420`; structural |
