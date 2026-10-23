# Balance PID, sesiones y análisis

ID: `web:module/src/features/balanceCalibration/useBalancePid.ts` · Capa: **integración** · Fotografía: 2026-10-20.

Documentación de fuente; sin tests, compilación, navegador ni validación física.

## Responsabilidad y alcance

Edita PID, conserva sesiones/registros y ejecuta análisis de calibración en el cliente.

Ganancias remotas, almacenamiento y worker de análisis; el lazo de control sigue en F4.

<a id="modularity"></a>
## Modularidad observada

Cálculo/worker separado de React y almacenamiento, con esquema propio de sesiones.

<a id="state"></a>
## Handle, estado y atributos externos

Sesiones, muestras y borradores tienen versión/lifetime de Web; respuesta de persistencia F4 confirma otra copia distinta.

<a id="lifecycle"></a>
## Ciclo de vida y contexto

- Cargar estado, capturar/seleccionar sesión y analizar en worker.
- Aplicar ganancias mediante contrato remoto y confirmar valores realmente guardados.

<a id="interactions"></a>
## Interacciones

- ESP decodifica PID y telemetría; almacenamiento local no modifica la preferencia del robot sin request.

<a id="limitations"></a>
## Precauciones y límites

- Resultado analítico no constituye prueba física de estabilidad.
- Distinguir datos simulados, guardados y medidos al presentar conclusiones.

## Hallazgos conectados


## Archivos y evidencia

| Archivo | Análisis | Hash SHA-256 |
| --- | --- | --- |
| `src/features/balanceCalibration/BalanceWorkbench.tsx` | typescript-ast-static | `ec87b478b6d5ecda6961daaf726713ed296108b7f841d0eea68d28e8865d4629` |
| `src/features/balanceCalibration/analysis.worker.ts` | typescript-ast-static | `ada69f9e8f07294c6f6c54f5eb93661002c67976d3d1c1be08563ee04b1abd02` |
| `src/features/balanceCalibration/optimizer.ts` | typescript-ast-static | `a5ab18488cdb123793f5b0996e737b0ffdb825b6351741b4d8c5de57b9a86d77` |
| `src/features/balanceCalibration/remote.ts` | typescript-ast-static | `6c5ff031fca2178ac4097decf29d87db2a28ef66a862c31871f26f3bcbf15202` |
| `src/features/balanceCalibration/session.ts` | typescript-ast-static | `a53c37308ff0eb770837bbd7a863d8c2cac3963c88f3e22aa5e61f1b05af6b19` |
| `src/features/balanceCalibration/storage.ts` | typescript-ast-static | `9d4fea10eec127993ccac0c54b86f54389fca6ba0391f770ec79a60804bd2562` |
| `src/features/balanceCalibration/useBalancePid.ts` | typescript-ast-static | `d4fe3b4a8cade306347ee428f8eb97805f8407e745e8c481e43ba422fafbd08c` |

## Tipos, puertos, props y callbacks

En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.

### `Analysis`

`web:src/features/balanceCalibration/optimizer.ts:3`

| Atributo / firma | Propósito |
| --- | --- |
| `algorithm: "arx2-grid-v1"` | Dato de estado/configuración `algorithm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `sessionId: string` | Dato de estado/configuración `sessionId`; declaración estructural, interpretación específica pendiente de ampliar. |
| `status: "insufficient-data" \| "no-candidate" \| "proposal"` | Último resultado y guardas del módulo; los getters pueden devolver una vista de este almacenamiento. |
| `reasons: string[]` | Dato de estado/configuración `reasons`; declaración estructural, interpretación específica pendiente de ampliar. |
| `candidate: Gains \| null` | Dato de estado/configuración `candidate`; declaración estructural, interpretación específica pendiente de ampliar. |
| `baselineCost: number \| null` | Dato de estado/configuración `baselineCost`; declaración estructural, interpretación específica pendiente de ampliar. |
| `candidateCost: number \| null` | Dato de estado/configuración `candidateCost`; declaración estructural, interpretación específica pendiente de ampliar. |
| `fitErrorDeg: number \| null` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `sensitivity: { term: keyof Gains; minusCost: number \| null; plusCost: number \| null }[]` | Dato de estado/configuración `sensitivity`; declaración estructural, interpretación específica pendiente de ampliar. |

### `Gains`

`web:src/features/balanceCalibration/session.ts:1`

| Atributo / firma | Propósito |
| --- | --- |
| `kp: number` | Dato de estado/configuración `kp`; declaración estructural, interpretación específica pendiente de ampliar. |
| `ki: number` | Dato de estado/configuración `ki`; declaración estructural, interpretación específica pendiente de ampliar. |
| `kd: number` | Dato de estado/configuración `kd`; declaración estructural, interpretación específica pendiente de ampliar. |

### `BalanceSample`

`web:src/features/balanceCalibration/session.ts:2`

| Atributo / firma | Propósito |
| --- | --- |
| `timeMs: number` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `sequence: number` | Dato de estado/configuración `sequence`; declaración estructural, interpretación específica pendiente de ampliar. |
| `pitch: number` | Dato de estado/configuración `pitch`; declaración estructural, interpretación específica pendiente de ampliar. |
| `target: number` | Dato de estado/configuración `target`; declaración estructural, interpretación específica pendiente de ampliar. |
| `output: number` | Dato de estado/configuración `output`; declaración estructural, interpretación específica pendiente de ampliar. |
| `left: number` | Dato de estado/configuración `left`; declaración estructural, interpretación específica pendiente de ampliar. |
| `right: number` | Dato de estado/configuración `right`; declaración estructural, interpretación específica pendiente de ampliar. |
| `error: number` | Dato de estado/configuración `error`; declaración estructural, interpretación específica pendiente de ampliar. |
| `integral: number` | Dato de estado/configuración `integral`; declaración estructural, interpretación específica pendiente de ampliar. |
| `derivative: number` | Dato de estado/configuración `derivative`; declaración estructural, interpretación específica pendiente de ampliar. |
| `gains: Gains` | Dato de estado/configuración `gains`; declaración estructural, interpretación específica pendiente de ampliar. |
| `stopped: boolean` | Dato de estado/configuración `stopped`; declaración estructural, interpretación específica pendiente de ampliar. |

### `ControllerConfig`

`web:src/features/balanceCalibration/session.ts:7`

| Atributo / firma | Propósito |
| --- | --- |
| `derivativeTauS: number` | Dato de estado/configuración `derivativeTauS`; declaración estructural, interpretación específica pendiente de ampliar. |
| `integralMin: number` | Dato de estado/configuración `integralMin`; declaración estructural, interpretación específica pendiente de ampliar. |
| `integralMax: number` | Dato de estado/configuración `integralMax`; declaración estructural, interpretación específica pendiente de ampliar. |
| `outputLimit: number` | Dato de estado/configuración `outputLimit`; declaración estructural, interpretación específica pendiente de ampliar. |
| `antiWindupGain: number` | Dato de estado/configuración `antiWindupGain`; declaración estructural, interpretación específica pendiente de ampliar. |
| `inverted: boolean` | Dato de estado/configuración `inverted`; declaración estructural, interpretación específica pendiente de ampliar. |

### `BalanceSession`

`web:src/features/balanceCalibration/session.ts:11`

| Atributo / firma | Propósito |
| --- | --- |
| `schema: "automicro.balance-session"` | Dato de estado/configuración `schema`; declaración estructural, interpretación específica pendiente de ampliar. |
| `version: 1` | Dato de estado/configuración `version`; declaración estructural, interpretación específica pendiente de ampliar. |
| `id: string` | Dato de estado/configuración `id`; declaración estructural, interpretación específica pendiente de ampliar. |
| `createdAt: string` | Dato de estado/configuración `createdAt`; declaración estructural, interpretación específica pendiente de ampliar. |
| `name: string` | Dato de estado/configuración `name`; declaración estructural, interpretación específica pendiente de ampliar. |
| `notes: string` | Dato de estado/configuración `notes`; declaración estructural, interpretación específica pendiente de ampliar. |
| `firmware: string` | Dato de estado/configuración `firmware`; declaración estructural, interpretación específica pendiente de ampliar. |
| `timing: "web-arrival" \| "f4-control-cycle"` | Dato de estado/configuración `timing`; declaración estructural, interpretación específica pendiente de ampliar. |
| `controller: ControllerConfig \| null` | Dato de estado/configuración `controller`; declaración estructural, interpretación específica pendiente de ampliar. |
| `samples: BalanceSample[]` | Dato de estado/configuración `samples`; declaración estructural, interpretación específica pendiente de ampliar. |
| `initialPid?: Gains` | Dato de estado/configuración `initialPid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `processed?: { name: string; processedAt: string; algorithm: string; candidate: Gains; confirmedPid?: Gains; appliedAt?: string }` | Dato de estado/configuración `processed`; declaración estructural, interpretación específica pendiente de ampliar. |


## Funciones públicas / componentes / hooks

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-balanceworkbench"></a>`BalanceWorkbench({ onBusyChange }: { onBusyChange: (busy: boolean) => void }): inferred-by-TypeScript` | Coordina `(["angle", "output", "terms"] as const).map`, `(["kp", "ki", "kd"] as const).map`, `analysis.baselineCost?.toFixed` para Balance PID, sesiones y análisis. | `web:src/features/balanceCalibration/BalanceWorkbench.tsx:13`; structural |
| <a id="fn-sessionchart"></a>`SessionChart({ session, mode }: { session: BalanceSession; mode: "angle" \| "output" \| "terms" }): inferred-by-TypeScript` | Coordina `(duration / 1000).toFixed`, `Math.max`, `all.filter` para Balance PID, sesiones y análisis. | `web:src/features/balanceCalibration/BalanceWorkbench.tsx:207`; structural |
| <a id="fn-analyzesession"></a>`analyzeSession(value: unknown): Analysis` | Coordina `(["kp", "ki", "kd"] as const).map`, `Array.from`, `Math.abs` para Balance PID, sesiones y análisis. | `web:src/features/balanceCalibration/optimizer.ts:25`; structural |
| <a id="fn-clamp"></a>`clamp(v: number, min: number, max: number): inferred-by-TypeScript` | Coordina `Math.max`, `Math.min` para Balance PID, sesiones y análisis. | `web:src/features/balanceCalibration/optimizer.ts:10`; structural |
| <a id="fn-database"></a>`database(): Promise<IDBDatabase>` | Operación database de Balance PID, sesiones y análisis; semántica fina pendiente de ampliación. | `web:src/features/balanceCalibration/storage.ts:3`; inferred-name |
| <a id="fn-deletesession"></a>`deleteSession(id: string): inferred-by-TypeScript` | Adapta la llamada a `operation` con el contexto de esta integración. | `web:src/features/balanceCalibration/storage.ts:28`; structural |
| <a id="fn-downloadsession"></a>`downloadSession(session: BalanceSession): inferred-by-TypeScript` | Coordina `JSON.stringify`, `URL.createObjectURL`, `a.click` para Balance PID, sesiones y análisis. | `web:src/features/balanceCalibration/session.ts:100`; structural |
| <a id="fn-finite"></a>`finite(value: unknown): number` | Adapta la llamada a `Number.isFinite` con el contexto de esta integración. | `web:src/features/balanceCalibration/session.ts:29`; structural |
| <a id="fn-flag"></a>`flag(value: unknown): boolean` | Operación flag de Balance PID, sesiones y análisis; semántica fina pendiente de ampliación. | `web:src/features/balanceCalibration/session.ts:33`; inferred-name |
| <a id="fn-gains"></a>`gains(value: unknown): Gains` | Coordina `finite`, `record` para Balance PID, sesiones y análisis. | `web:src/features/balanceCalibration/session.ts:41`; structural |
| <a id="fn-listsessions"></a>`listSessions(): Promise<BalanceSession[]>` | Coordina `operation`, `values.map`, `values.map(parseSession).sort` para Balance PID, sesiones y análisis. | `web:src/features/balanceCalibration/storage.ts:21`; structural |
| <a id="fn-newsession"></a>`newSession(name: string, notes: string): BalanceSession` | Coordina `crypto.randomUUID`, `new Date().toISOString` para Balance PID, sesiones y análisis. | `web:src/features/balanceCalibration/session.ts:20`; structural |
| <a id="fn-numberornull"></a>`numberOrNull(v: unknown): inferred-by-TypeScript` | Adapta la llamada a `Number.isFinite` con el contexto de esta integración. | `web:src/features/balanceCalibration/remote.ts:24`; structural |
| <a id="fn-operation"></a>`operation(mode: IDBTransactionMode, action: (store: IDBObjectStore) => IDBRequest<T>): Promise<T>` | Adapta la llamada a `database` con el contexto de esta integración. | `web:src/features/balanceCalibration/storage.ts:12`; structural |
| <a id="fn-parsepid"></a>`parsePid(value: unknown): Gains` | Coordina `String`, `values.every` para Balance PID, sesiones y análisis. | `web:src/features/balanceCalibration/useBalancePid.ts:5`; structural |
| <a id="fn-parsesession"></a>`parseSession(value: unknown): BalanceSession` | Coordina `Array.isArray`, `String`, `["web-arrival", "f4-control-cycle"].includes` para Balance PID, sesiones y análisis. | `web:src/features/balanceCalibration/session.ts:44`; structural |
| <a id="fn-record"></a>`record(value: unknown): Record<string, unknown>` | Adapta la llamada a `Array.isArray` con el contexto de esta integración. | `web:src/features/balanceCalibration/session.ts:25`; structural |
| <a id="fn-remoteanalysis"></a>`remoteAnalysis(session: BalanceSession, endpoint: string, token: string, signal: AbortSignal): Promise<Analysis>` | Coordina `Array.isArray`, `JSON.stringify`, `["127.0.0.1", "localhost"].includes` para Balance PID, sesiones y análisis. | `web:src/features/balanceCalibration/remote.ts:3`; structural |
| <a id="fn-savesession"></a>`saveSession(session: BalanceSession): inferred-by-TypeScript` | Adapta la llamada a `operation` con el contexto de esta integración. | `web:src/features/balanceCalibration/storage.ts:25`; structural |
| <a id="fn-simulate"></a>`simulate(session: BalanceSession, m: number[], dt: number, gains: Gains): number` | Coordina `Math.abs`, `Math.ceil`, `Math.max` para Balance PID, sesiones y análisis. | `web:src/features/balanceCalibration/optimizer.ts:86`; structural |
| <a id="fn-solve"></a>`solve(matrix: number[][], vector: number[]): number[] \| null` | Coordina `Math.abs`, `a.map`, `matrix.map` para Balance PID, sesiones y análisis. | `web:src/features/balanceCalibration/optimizer.ts:11`; structural |
| <a id="fn-summarize"></a>`summarize(session: BalanceSession): inferred-by-TypeScript` | Coordina `Math.sqrt`, `p.reduce`, `p.slice` para Balance PID, sesiones y análisis. | `web:src/features/balanceCalibration/session.ts:93`; structural |
| <a id="fn-telemetrysample"></a>`telemetrySample(value: unknown, timeMs: number, sequence: number): BalanceSample` | Coordina `finite`, `flag`, `gains` para Balance PID, sesiones y análisis. | `web:src/features/balanceCalibration/session.ts:84`; structural |
| <a id="fn-text"></a>`text(value: unknown): string` | Operación text de Balance PID, sesiones y análisis; semántica fina pendiente de ampliación. | `web:src/features/balanceCalibration/session.ts:37`; inferred-name |
| <a id="fn-usebalancepid"></a>`useBalancePid(): inferred-by-TypeScript` | Coordina `useCallback`, `useEffect`, `useState` para Balance PID, sesiones y análisis. | `web:src/features/balanceCalibration/useBalancePid.ts:14`; structural |

## Funciones y callbacks internos

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-balanceworkbench.analyze"></a>`BalanceWorkbench.analyze(): inferred-by-TypeScript` | Coordina `setAnalysis`, `setAnalyzing`, `task.postMessage` para Balance PID, sesiones y análisis. | `web:src/features/balanceCalibration/BalanceWorkbench.tsx:79`; structural |
| <a id="fn-balanceworkbench.importfile"></a>`BalanceWorkbench.importFile(file: File \| undefined): inferred-by-TypeScript` | Coordina `JSON.parse`, `file.text`, `parseSession` para Balance PID, sesiones y análisis. | `web:src/features/balanceCalibration/BalanceWorkbench.tsx:121`; structural |
| <a id="fn-balanceworkbench.next"></a>`BalanceWorkbench.next(): inferred-by-TypeScript` | Coordina `performance.now`, `persist`, `request` para Balance PID, sesiones y análisis. | `web:src/features/balanceCalibration/BalanceWorkbench.tsx:47`; structural |
| <a id="fn-balanceworkbench.persist"></a>`BalanceWorkbench.persist(value: BalanceSession): inferred-by-TypeScript` | Coordina `listSessions`, `saveSession`, `setFeedback` para Balance PID, sesiones y análisis. | `web:src/features/balanceCalibration/BalanceWorkbench.tsx:39`; structural |
| <a id="fn-balanceworkbench.sendanalysis"></a>`BalanceWorkbench.sendAnalysis(): inferred-by-TypeScript` | Coordina `clearTimeout`, `endpoint.trim`, `new Date().toISOString` para Balance PID, sesiones y análisis. | `web:src/features/balanceCalibration/BalanceWorkbench.tsx:91`; structural |
| <a id="fn-balanceworkbench.sendpid"></a>`BalanceWorkbench.sendPid(candidate: inferred-by-TypeScript): inferred-by-TypeScript` | Coordina `Number`, `Object.values`, `Object.values(draft).some` para Balance PID, sesiones y análisis. | `web:src/features/balanceCalibration/BalanceWorkbench.tsx:109`; structural |
| <a id="fn-balanceworkbench.start"></a>`BalanceWorkbench.start(): inferred-by-TypeScript` | Coordina `name.trim`, `newSession`, `performance.now` para Balance PID, sesiones y análisis. | `web:src/features/balanceCalibration/BalanceWorkbench.tsx:72`; structural |
| <a id="fn-analyzesession.cost"></a>`analyzeSession.cost(g: Gains): number` | Adapta la llamada a `simulate` con el contexto de esta integración. | `web:src/features/balanceCalibration/optimizer.ts:66`; structural |
| <a id="fn-usebalancepid.write"></a>`useBalancePid.write(gains: Gains): Promise<Gains>` | Coordina `(["kp", "ki", "kd"] as const).some`, `[gains.kp, gains.ki, gains.kd].every`, `parsePid` para Balance PID, sesiones y análisis. | `web:src/features/balanceCalibration/useBalancePid.ts:36`; structural |

## Estado y efectos React encontrados

| Hook | Función/closure | Evidencia |
| --- | --- | --- |
| `useWebSocket` | `BalanceWorkbench` | `web:src/features/balanceCalibration/BalanceWorkbench.tsx:14` |
| `useBalancePid` | `BalanceWorkbench` | `web:src/features/balanceCalibration/BalanceWorkbench.tsx:15` |
| `useState` | `BalanceWorkbench` | `web:src/features/balanceCalibration/BalanceWorkbench.tsx:16` |
| `useState` | `BalanceWorkbench` | `web:src/features/balanceCalibration/BalanceWorkbench.tsx:17` |
| `useState` | `BalanceWorkbench` | `web:src/features/balanceCalibration/BalanceWorkbench.tsx:18` |
| `useState` | `BalanceWorkbench` | `web:src/features/balanceCalibration/BalanceWorkbench.tsx:19` |
| `useState` | `BalanceWorkbench` | `web:src/features/balanceCalibration/BalanceWorkbench.tsx:20` |
| `useState` | `BalanceWorkbench` | `web:src/features/balanceCalibration/BalanceWorkbench.tsx:21` |
| `useState` | `BalanceWorkbench` | `web:src/features/balanceCalibration/BalanceWorkbench.tsx:21` |
| `useState` | `BalanceWorkbench` | `web:src/features/balanceCalibration/BalanceWorkbench.tsx:22` |
| `useState` | `BalanceWorkbench` | `web:src/features/balanceCalibration/BalanceWorkbench.tsx:22` |
| `useState` | `BalanceWorkbench` | `web:src/features/balanceCalibration/BalanceWorkbench.tsx:23` |
| `useState` | `BalanceWorkbench` | `web:src/features/balanceCalibration/BalanceWorkbench.tsx:24` |
| `useState` | `BalanceWorkbench` | `web:src/features/balanceCalibration/BalanceWorkbench.tsx:25` |
| `useState` | `BalanceWorkbench` | `web:src/features/balanceCalibration/BalanceWorkbench.tsx:26` |
| `useRef` | `BalanceWorkbench` | `web:src/features/balanceCalibration/BalanceWorkbench.tsx:27` |
| `useState` | `BalanceWorkbench` | `web:src/features/balanceCalibration/BalanceWorkbench.tsx:28` |
| `useState` | `BalanceWorkbench` | `web:src/features/balanceCalibration/BalanceWorkbench.tsx:29` |
| `useRef` | `BalanceWorkbench` | `web:src/features/balanceCalibration/BalanceWorkbench.tsx:30` |
| `useRef` | `BalanceWorkbench` | `web:src/features/balanceCalibration/BalanceWorkbench.tsx:30` |
| `useRef` | `BalanceWorkbench` | `web:src/features/balanceCalibration/BalanceWorkbench.tsx:31` |
| `useEffect` | `BalanceWorkbench` | `web:src/features/balanceCalibration/BalanceWorkbench.tsx:35` |
| `useEffect` | `BalanceWorkbench` | `web:src/features/balanceCalibration/BalanceWorkbench.tsx:36` |
| `useEffect` | `BalanceWorkbench` | `web:src/features/balanceCalibration/BalanceWorkbench.tsx:37` |
| `useEffect` | `BalanceWorkbench` | `web:src/features/balanceCalibration/BalanceWorkbench.tsx:38` |
| `useEffect` | `BalanceWorkbench` | `web:src/features/balanceCalibration/BalanceWorkbench.tsx:43` |
| `useWebSocket` | `useBalancePid` | `web:src/features/balanceCalibration/useBalancePid.ts:15` |
| `useState` | `useBalancePid` | `web:src/features/balanceCalibration/useBalancePid.ts:16` |
| `useState` | `useBalancePid` | `web:src/features/balanceCalibration/useBalancePid.ts:17` |
| `useState` | `useBalancePid` | `web:src/features/balanceCalibration/useBalancePid.ts:17` |
| `useCallback` | `useBalancePid` | `web:src/features/balanceCalibration/useBalancePid.ts:18` |
| `useEffect` | `useBalancePid` | `web:src/features/balanceCalibration/useBalancePid.ts:27` |
