# Usuario, PIN y permisos remotos

ID: `web:module/src/contexts/UserContext.tsx` · Capa: **integración** · Fotografía: 2026-10-20.

Documentación de fuente; sin tests, compilación, navegador ni validación física.

## Responsabilidad y alcance

Mantiene sesión del navegador y flujos de PIN/autorización que habilitan acciones remotas.

Login, validar pantalla, cambio PIN y cierre; la comprobación final de acciones sigue en ESP/F4.

<a id="modularity"></a>
## Modularidad observada

React/servicios del producto; separar formulario local, sesión ESP y permiso local F4.

<a id="state"></a>
## Handle, estado y atributos externos

El provider conserva autoridad confirmada y estado de solicitud; formularios PIN guardan edición local que no es el cursor PIN de la OLED física.

<a id="lifecycle"></a>
## Ciclo de vida y contexto

- Enviar PIN al comando correspondiente y esperar final.
- Logout/desconexión/expiración actualiza estado y cancela acciones según el flujo.
- Unmount del modal no debe confundirse con revocación remota sin comando.

<a id="interactions"></a>
## Interacciones

- ScreenStreamWorkspace puede pedir validar el screenCode actual.
- ESP asigna sesión/nodo; F4 conserva autorización por origen y TTL.
- PIN JSON string de cuatro dígitos se codifica como cuatro valores numéricos 0..9 en wire; ver shared:message/auth-pin/v1 para scopes y respuestas.

<a id="limitations"></a>
## Precauciones y límites

- No publicar PIN/token ni almacenarlos en artefactos documentales.
- Poder abrir un formulario no demuestra que el comando quedó autorizado.

## Hallazgos conectados


## Archivos y evidencia

| Archivo | Análisis | Hash SHA-256 |
| --- | --- | --- |
| `src/api/UserRepository.tsx` | typescript-ast-static | `ec12dfe00633a34b426262ad2d5446a67641901b2339eadfef8510a50aa5dffc` |
| `src/components/PinScreen.tsx` | typescript-ast-static | `f717ab48362a7e016a7d9fcd3ed9bdc255aba8f6e614091f5d70de93df1faca3` |
| `src/components/PinScreenModal.tsx` | typescript-ast-static | `b2d8bc59ac3e41b68dfff20493f3f99eecc23990aab48327c2bc5ff7d7ca03ee` |
| `src/components/ProtectedRoute.tsx` | typescript-ast-static | `44e6f7e65e602272d3ccc3369e12c3123d21753df281b7e523b035088fe73e57` |
| `src/components/PublicRoute.tsx` | typescript-ast-static | `42c4d51756bd1c88ff6779d57f3db25ab8d9cb531b6950e72e9f780334791eca` |
| `src/contexts/UserContext.tsx` | typescript-ast-static | `16c1437b753012f9afdeb4d5dce63fd70328a2f7a02e35fe81cf1017d93fc75e` |
| `src/pages/Login.tsx` | typescript-ast-static | `b1b2838e84590c844a3c368ff7da0df9686709524e46b8ac937d814402437925` |
| `src/types/PinAuthTypes.ts` | typescript-ast-static | `ba3d5fe21dca38ca9d75fc696678739177496776f63784b8acbd798a7e5bf722` |
| `src/types/UserTypes.ts` | typescript-ast-static | `0f27305478f8f6d27694d5f799085c749774b14d06c8db1edb4863cc0c70cbfe` |

## Tipos, puertos, props y callbacks

En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.

### `User`

`web:src/api/UserRepository.tsx:3`

| Atributo / firma | Propósito |
| --- | --- |
| `id: string` | Dato de estado/configuración `id`; declaración estructural, interpretación específica pendiente de ampliar. |
| `name: string` | Dato de estado/configuración `name`; declaración estructural, interpretación específica pendiente de ampliar. |

### `LoginResponse`

`web:src/api/UserRepository.tsx:9`

| Atributo / firma | Propósito |
| --- | --- |
| `user: User` | Dato de estado/configuración `user`; declaración estructural, interpretación específica pendiente de ampliar. |
| `token: string` | Dato de estado/configuración `token`; declaración estructural, interpretación específica pendiente de ampliar. |

### `PinScreenStatus`

`web:src/components/PinScreen.tsx:7`

Tipo: `"idle" \| "loading" \| "error"`.

### `PinScreenProps`

`web:src/components/PinScreen.tsx:9`

| Atributo / firma | Propósito |
| --- | --- |
| `title?: string` | Dato de estado/configuración `title`; declaración estructural, interpretación específica pendiente de ampliar. |
| `subtitle?: string` | Dato de estado/configuración `subtitle`; declaración estructural, interpretación específica pendiente de ampliar. |
| `kicker?: string` | Dato de estado/configuración `kicker`; declaración estructural, interpretación específica pendiente de ampliar. |
| `submitLabel?: string` | Dato de estado/configuración `submitLabel`; declaración estructural, interpretación específica pendiente de ampliar. |
| `digitsCount?: number` | Dato de estado/configuración `digitsCount`; declaración estructural, interpretación específica pendiente de ampliar. |
| `canClose?: boolean` | Dato de estado/configuración `canClose`; declaración estructural, interpretación específica pendiente de ampliar. |
| `onClose?: () => void` | El diálogo invoca al cancelar/cerrar para que el dueño retire la vista; no revoca sesión F4 por sí solo. |
| `onSubmit: (pin: string) => Promise<PinSubmitResult> \| PinSubmitResult` | Entrega PIN local al dueño y espera PinSubmitResult síncrono/Promise; la UI refleja el resultado sin inventar concesión física. |
| `successAction?: () => void` | Continuación opcional tras éxito confirmado por el flujo de PIN; no se ejecuta por solo escribir cuatro dígitos. |
| `idleMessage?: string` | Dato de estado/configuración `idleMessage`; declaración estructural, interpretación específica pendiente de ampliar. |
| `errorMessage?: string` | Dato de estado/configuración `errorMessage`; declaración estructural, interpretación específica pendiente de ampliar. |
| `loadingMessage?: string` | Dato de estado/configuración `loadingMessage`; declaración estructural, interpretación específica pendiente de ampliar. |
| `compactInvalidPinError?: boolean` | Dato de estado/configuración `compactInvalidPinError`; declaración estructural, interpretación específica pendiente de ampliar. |

### `PinScreenModalProps`

`web:src/components/PinScreenModal.tsx:9`

| Atributo / firma | Propósito |
| --- | --- |
| `isOpen: boolean` | Dato de estado/configuración `isOpen`; declaración estructural, interpretación específica pendiente de ampliar. |
| `onClose: () => void` | El diálogo invoca al cancelar/cerrar para que el dueño retire la vista; no revoca sesión F4 por sí solo. |
| `title?: string` | Dato de estado/configuración `title`; declaración estructural, interpretación específica pendiente de ampliar. |
| `subtitle?: string` | Dato de estado/configuración `subtitle`; declaración estructural, interpretación específica pendiente de ampliar. |
| `kicker?: string` | Dato de estado/configuración `kicker`; declaración estructural, interpretación específica pendiente de ampliar. |
| `submitLabel?: string` | Dato de estado/configuración `submitLabel`; declaración estructural, interpretación específica pendiente de ampliar. |
| `digitsCount?: number` | Dato de estado/configuración `digitsCount`; declaración estructural, interpretación específica pendiente de ampliar. |
| `canClose?: boolean` | Dato de estado/configuración `canClose`; declaración estructural, interpretación específica pendiente de ampliar. |
| `onSubmit: (pin: string) => Promise<PinSubmitResult> \| PinSubmitResult` | Entrega PIN local al dueño y espera PinSubmitResult síncrono/Promise; la UI refleja el resultado sin inventar concesión física. |
| `successAction?: () => void` | Continuación opcional tras éxito confirmado por el flujo de PIN; no se ejecuta por solo escribir cuatro dígitos. |
| `idleMessage?: string` | Dato de estado/configuración `idleMessage`; declaración estructural, interpretación específica pendiente de ampliar. |
| `errorMessage?: string` | Dato de estado/configuración `errorMessage`; declaración estructural, interpretación específica pendiente de ampliar. |
| `loadingMessage?: string` | Dato de estado/configuración `loadingMessage`; declaración estructural, interpretación específica pendiente de ampliar. |

### `User`

`web:src/contexts/UserContext.tsx:8`

| Atributo / firma | Propósito |
| --- | --- |
| `id: string` | Dato de estado/configuración `id`; declaración estructural, interpretación específica pendiente de ampliar. |
| `name: string` | Dato de estado/configuración `name`; declaración estructural, interpretación específica pendiente de ampliar. |

### `StmAuthState`

`web:src/contexts/UserContext.tsx:13`

| Atributo / firma | Propósito |
| --- | --- |
| `authenticated: boolean` | Dato de estado/configuración `authenticated`; declaración estructural, interpretación específica pendiente de ampliar. |
| `authSource: "stm32" \| null` | Dato de estado/configuración `authSource`; declaración estructural, interpretación específica pendiente de ampliar. |
| `ttlMs: number` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `attemptsLeft: number \| null` | Dato de estado/configuración `attemptsLeft`; declaración estructural, interpretación específica pendiente de ampliar. |
| `blocked: boolean` | Dato de estado/configuración `blocked`; declaración estructural, interpretación específica pendiente de ampliar. |

### `UserContextType`

`web:src/contexts/UserContext.tsx:21`

| Atributo / firma | Propósito |
| --- | --- |
| `user: User \| null` | Dato de estado/configuración `user`; declaración estructural, interpretación específica pendiente de ampliar. |
| `loading: boolean` | Dato de estado/configuración `loading`; declaración estructural, interpretación específica pendiente de ampliar. |
| `remotePinAuthenticated: boolean` | Dato de estado/configuración `remotePinAuthenticated`; declaración estructural, interpretación específica pendiente de ampliar. |
| `localEspDebugReady: boolean` | Dato de estado/configuración `localEspDebugReady`; declaración estructural, interpretación específica pendiente de ampliar. |
| `stmAuth: StmAuthState` | Dato de estado/configuración `stmAuth`; declaración estructural, interpretación específica pendiente de ampliar. |
| `login: (pin: string) => Promise<PinAuthResult>` | Envía PIN de login local al contrato de autenticación y conserva resultado/TTL de la sesión correspondiente. |
| `validatePin: (pin: string, screenCode?: number) => Promise<PinAuthResult>` | Solicita validación del PIN, con screenCode cuando corresponde; scope pantalla no concede login general nuevo. |
| `changePin: (currentPin: string, newPin: string) => Promise<PinAuthResult>` | Solicita cambio de PIN actual/nuevo; después de resultado revisar revocación de sesiones y streams. |
| `logout: () => void` | Inicia cierre de sesión y limpieza local/remota según el provider; quitar el modal no equivale a logout. |
| `sessionNotice: string \| null` | Dato de estado/configuración `sessionNotice`; declaración estructural, interpretación específica pendiente de ampliar. |
| `clearSessionNotice: () => void` | Limpia aviso local de estado de sesión después de mostrarlo; no cambia autoridad física. |
| `setUser: (user: User \| null) => void` | Actualiza proyección local de usuario; no concede permiso F4 por sí solo. |
| `devMode: boolean` | Dato de estado/configuración `devMode`; declaración estructural, interpretación específica pendiente de ampliar. |
| `setDevMode: (value: boolean) => void` | Configura modo local de desarrollo; no sustituye concesión de PIN F4. |

### `AuthResponse`

`web:src/contexts/UserContext.tsx:38`

| Atributo / firma | Propósito |
| --- | --- |
| `granted?: boolean` | Dato de estado/configuración `granted`; declaración estructural, interpretación específica pendiente de ampliar. |
| `authenticated?: boolean` | Dato de estado/configuración `authenticated`; declaración estructural, interpretación específica pendiente de ampliar. |
| `authSource?: string` | Dato de estado/configuración `authSource`; declaración estructural, interpretación específica pendiente de ampliar. |
| `ttlMs?: number` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `ttlRemainingMs?: number` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `attemptsLeft?: number` | Dato de estado/configuración `attemptsLeft`; declaración estructural, interpretación específica pendiente de ampliar. |
| `blocked?: boolean` | Dato de estado/configuración `blocked`; declaración estructural, interpretación específica pendiente de ampliar. |

### `PinAuthFailureReason`

`web:src/types/PinAuthTypes.ts:1`

Tipo: `\| "invalid-pin" \| "timeout" \| "busy" \| "bad-request" \| "transport-error" \| "grant-rejected" \| "unknown"`.

### `PinAuthResult`

`web:src/types/PinAuthTypes.ts:10`

| Atributo / firma | Propósito |
| --- | --- |
| `ok: boolean` | Dato de estado/configuración `ok`; declaración estructural, interpretación específica pendiente de ampliar. |
| `reason?: PinAuthFailureReason` | Dato de estado/configuración `reason`; declaración estructural, interpretación específica pendiente de ampliar. |
| `message?: string` | Dato de estado/configuración `message`; declaración estructural, interpretación específica pendiente de ampliar. |
| `code?: number` | Dato de estado/configuración `code`; declaración estructural, interpretación específica pendiente de ampliar. |
| `attemptsLeft?: number \| null` | Dato de estado/configuración `attemptsLeft`; declaración estructural, interpretación específica pendiente de ampliar. |
| `blocked?: boolean` | Dato de estado/configuración `blocked`; declaración estructural, interpretación específica pendiente de ampliar. |
| `authSource?: "stm32" \| null` | Dato de estado/configuración `authSource`; declaración estructural, interpretación específica pendiente de ampliar. |
| `retryAfterMs?: number` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |

### `PinSubmitResult`

`web:src/types/PinAuthTypes.ts:21`

Tipo: `boolean \| PinAuthResult`.

### `User`

`web:src/types/UserTypes.ts:1`

| Atributo / firma | Propósito |
| --- | --- |
| `id: string` | Dato de estado/configuración `id`; declaración estructural, interpretación específica pendiente de ampliar. |
| `name: string` | Dato de estado/configuración `name`; declaración estructural, interpretación específica pendiente de ampliar. |

### `UserContextType`

`web:src/types/UserTypes.ts:7`

| Atributo / firma | Propósito |
| --- | --- |
| `user: User \| null` | Dato de estado/configuración `user`; declaración estructural, interpretación específica pendiente de ampliar. |
| `loading?: boolean` | Dato de estado/configuración `loading`; declaración estructural, interpretación específica pendiente de ampliar. |
| `remotePinAuthenticated?: boolean` | Dato de estado/configuración `remotePinAuthenticated`; declaración estructural, interpretación específica pendiente de ampliar. |
| `login: (pin: string) => Promise<boolean>` | Envía PIN de login local al contrato de autenticación y conserva resultado/TTL de la sesión correspondiente. |
| `validatePin?: (pin: string) => Promise<boolean>` | Solicita validación del PIN, con screenCode cuando corresponde; scope pantalla no concede login general nuevo. |
| `logout: () => void` | Inicia cierre de sesión y limpieza local/remota según el provider; quitar el modal no equivale a logout. |


## Funciones públicas / componentes / hooks

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-feedbackstep"></a>`FeedbackStep({  label,  value,  tone, }: { label: string; value: string; tone: "ok" \| "error"; }): inferred-by-TypeScript` | Operación Feedback Step de Usuario, PIN y permisos remotos; semántica fina pendiente de ampliación. | `web:src/components/PinScreen.tsx:387`; inferred-name |
| <a id="fn-login"></a>`Login(): inferred-by-TypeScript` | Adapta la llamada a `useUser` con el contexto de esta integración. | `web:src/pages/Login.tsx:5`; structural |
| <a id="fn-pinscreen"></a>`PinScreen({  title = "Ingresar PIN",  subtitle = "Usa el PIN de 4 digitos guardado en el ESP.",  kicker = "Acceso seguro",  submitLabel = "Entrar",  digitsCount = 4,  canClose = false,  onClose,  onSubmit,  successAction,  idleMessage = "Escribi los digitos con el teclado.",  errorMessage = "No se pudo validar el PIN.",  loadingMessage = "Validando...",  compactInvalidPinError = false, }: PinScreenProps): inferred-by-TypeScript` | Coordina `Array.from`, `Array.from({ length: 3 }).map`, `Array.from({ length: safeDigitsCount }).map` para Usuario, PIN y permisos remotos. | `web:src/components/PinScreen.tsx:25`; structural |
| <a id="fn-pinscreenmodal"></a>`PinScreenModal({   isOpen,   onClose,   title,   subtitle,   kicker,   submitLabel,   digitsCount = 4,   canClose = true,   onSubmit,   successAction,   idleMessage,   errorMessage,   loadingMessage, }: PinScreenModalProps): inferred-by-TypeScript` | Presenta edición local del PIN; no reconstruye el buffer de la pantalla F4. | `web:src/components/PinScreenModal.tsx:25`; editorial |
| <a id="fn-protectedroute"></a>`ProtectedRoute({   children,   loadingLabel = "Cargando dashboard", }: inferred-by-TypeScript): inferred-by-TypeScript` | Adapta la llamada a `useUser` con el contexto de esta integración. | `web:src/components/ProtectedRoute.tsx:5`; structural |
| <a id="fn-publicroute"></a>`PublicRoute({   children, }: inferred-by-TypeScript): inferred-by-TypeScript` | Adapta la llamada a `useUser` con el contexto de esta integración. | `web:src/components/PublicRoute.tsx:7`; structural |
| <a id="fn-userprovider"></a>`UserProvider({ children }: inferred-by-TypeScript): inferred-by-TypeScript` | Coordina `useCallback`, `useEffect`, `useRef` para Usuario, PIN y permisos remotos. | `web:src/contexts/UserContext.tsx:65`; structural |
| <a id="fn-fetchwithauth"></a>`fetchWithAuth(endpoint: string, options: RequestInit): Promise<T>` | Coordina `fetch`, `getAuthToken`, `res.json` para Usuario, PIN y permisos remotos. | `web:src/api/UserRepository.tsx:73`; structural |
| <a id="fn-finitenumber"></a>`finiteNumber(value: unknown): number \| null` | Adapta la llamada a `Number.isFinite` con el contexto de esta integración. | `web:src/contexts/UserContext.tsx:379`; structural |
| <a id="fn-getauthtoken"></a>`getAuthToken(): string \| null` | Adapta la llamada a `localStorage.getItem` con el contexto de esta integración. | `web:src/api/UserRepository.tsx:65`; structural |
| <a id="fn-getcurrentuser"></a>`getCurrentUser(): User \| null` | Coordina `JSON.parse`, `localStorage.getItem` para Usuario, PIN y permisos remotos. | `web:src/api/UserRepository.tsx:52`; structural |
| <a id="fn-getpinfailuretitle"></a>`getPinFailureTitle(failure: PinAuthResult): string` | Operación get Pin Failure Title de Usuario, PIN y permisos remotos; semántica fina pendiente de ampliación. | `web:src/components/PinScreen.tsx:366`; inferred-name |
| <a id="fn-getprofile"></a>`getProfile(): Promise<User>` | Adapta la llamada a `fetchWithAuth` con el contexto de esta integración. | `web:src/api/UserRepository.tsx:99`; structural |
| <a id="fn-login"></a>`login(username: string, password: string): Promise<User>` | Coordina `JSON.stringify`, `fetch`, `localStorage.setItem` para Usuario, PIN y permisos remotos. | `web:src/api/UserRepository.tsx:22`; structural |
| <a id="fn-logout"></a>`logout(): void` | Adapta la llamada a `localStorage.removeItem` con el contexto de esta integración. | `web:src/api/UserRepository.tsx:43`; structural |
| <a id="fn-normalizepinsubmitresult"></a>`normalizePinSubmitResult(result: PinSubmitResult, fallbackMessage: string): PinAuthResult` | Operación normalize Pin Submit Result de Usuario, PIN y permisos remotos; semántica fina pendiente de ampliación. | `web:src/components/PinScreen.tsx:340`; inferred-name |
| <a id="fn-pinfailure"></a>`pinFailure(cause: unknown): PinAuthResult` | Operación pin Failure de Usuario, PIN y permisos remotos; semántica fina pendiente de ampliación. | `web:src/contexts/UserContext.tsx:387`; inferred-name |
| <a id="fn-useuser"></a>`useUser(): inferred-by-TypeScript` | Adapta la llamada a `useContext` con el contexto de esta integración. | `web:src/contexts/UserContext.tsx:373`; structural |
| <a id="fn-wait"></a>`wait(delayMs: number): Promise<void>` | Operación wait de Usuario, PIN y permisos remotos; semántica fina pendiente de ampliación. | `web:src/contexts/UserContext.tsx:383`; inferred-name |

## Funciones y callbacks internos

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-pinscreen.handlepinchange"></a>`PinScreen.handlePinChange(value: string): inferred-by-TypeScript` | Coordina `setFailure`, `setPin`, `setStatus` para Usuario, PIN y permisos remotos. | `web:src/components/PinScreen.tsx:78`; structural |
| <a id="fn-pinscreen.resetattempt"></a>`PinScreen.resetAttempt(): inferred-by-TypeScript` | Coordina `setFailure`, `setPin`, `setStatus` para Usuario, PIN y permisos remotos. | `web:src/components/PinScreen.tsx:84`; structural |
| <a id="fn-pinscreen.sendattempt"></a>`PinScreen.sendAttempt(): inferred-by-TypeScript` | Coordina `console.error`, `normalizePinSubmitResult`, `onSubmit` para Usuario, PIN y permisos remotos. | `web:src/components/PinScreen.tsx:92`; structural |
| <a id="fn-pinscreen.updatecountdown"></a>`PinScreen.updateCountdown(): inferred-by-TypeScript` | Coordina `Date.now`, `Math.ceil`, `Math.max` para Usuario, PIN y permisos remotos. | `web:src/components/PinScreen.tsx:69`; structural |
| <a id="fn-pinscreenmodal.onkeydown"></a>`PinScreenModal.onKeyDown(event: KeyboardEvent): inferred-by-TypeScript` | Adapta la llamada a `onClose` con el contexto de esta integración. | `web:src/components/PinScreenModal.tsx:47`; structural |

## Estado y efectos React encontrados

| Hook | Función/closure | Evidencia |
| --- | --- | --- |
| `useReducedMotion` | `PinScreen` | `web:src/components/PinScreen.tsx:40` |
| `useRef` | `PinScreen` | `web:src/components/PinScreen.tsx:41` |
| `useState` | `PinScreen` | `web:src/components/PinScreen.tsx:42` |
| `useState` | `PinScreen` | `web:src/components/PinScreen.tsx:43` |
| `useState` | `PinScreen` | `web:src/components/PinScreen.tsx:44` |
| `useState` | `PinScreen` | `web:src/components/PinScreen.tsx:45` |
| `useMemo` | `PinScreen` | `web:src/components/PinScreen.tsx:48` |
| `useEffect` | `PinScreen` | `web:src/components/PinScreen.tsx:53` |
| `useEffect` | `PinScreen` | `web:src/components/PinScreen.tsx:61` |
| `useReducedMotion` | `PinScreenModal` | `web:src/components/PinScreenModal.tsx:40` |
| `useModalScrollLock` | `PinScreenModal` | `web:src/components/PinScreenModal.tsx:41` |
| `useEffect` | `PinScreenModal` | `web:src/components/PinScreenModal.tsx:43` |
| `useUser` | `ProtectedRoute` | `web:src/components/ProtectedRoute.tsx:9` |
| `useUser` | `PublicRoute` | `web:src/components/PublicRoute.tsx:10` |
| `useWebSocket` | `UserProvider` | `web:src/contexts/UserContext.tsx:66` |
| `useState` | `UserProvider` | `web:src/contexts/UserContext.tsx:67` |
| `useState` | `UserProvider` | `web:src/contexts/UserContext.tsx:68` |
| `useState` | `UserProvider` | `web:src/contexts/UserContext.tsx:69` |
| `useState` | `UserProvider` | `web:src/contexts/UserContext.tsx:70` |
| `useState` | `UserProvider` | `web:src/contexts/UserContext.tsx:71` |
| `useState` | `UserProvider` | `web:src/contexts/UserContext.tsx:72` |
| `useRef` | `UserProvider` | `web:src/contexts/UserContext.tsx:73` |
| `useCallback` | `UserProvider` | `web:src/contexts/UserContext.tsx:75` |
| `useEffect` | `UserProvider` | `web:src/contexts/UserContext.tsx:77` |
| `useEffect` | `UserProvider` | `web:src/contexts/UserContext.tsx:83` |
| `useEffect` | `UserProvider` | `web:src/contexts/UserContext.tsx:103` |
| `useCallback` | `UserProvider` | `web:src/contexts/UserContext.tsx:115` |
| `useCallback` | `UserProvider` | `web:src/contexts/UserContext.tsx:150` |
| `useEffect` | `UserProvider` | `web:src/contexts/UserContext.tsx:170` |
| `useCallback` | `UserProvider` | `web:src/contexts/UserContext.tsx:227` |
| `useCallback` | `UserProvider` | `web:src/contexts/UserContext.tsx:254` |
| `useCallback` | `UserProvider` | `web:src/contexts/UserContext.tsx:303` |
| `useCallback` | `UserProvider` | `web:src/contexts/UserContext.tsx:310` |
| `useEffect` | `UserProvider` | `web:src/contexts/UserContext.tsx:319` |
| `useEffect` | `UserProvider` | `web:src/contexts/UserContext.tsx:339` |
| `useCallback` | `UserProvider` | `web:src/contexts/UserContext.tsx:351` |
| `useCallback` | `UserProvider` | `web:src/contexts/UserContext.tsx:360` |
| `useContext` | `useUser` | `web:src/contexts/UserContext.tsx:374` |
| `useUser` | `Login` | `web:src/pages/Login.tsx:6` |
