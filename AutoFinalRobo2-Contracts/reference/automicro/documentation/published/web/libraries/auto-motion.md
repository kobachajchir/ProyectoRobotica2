# GOTO y calibración de motores Web

ID: `web:module/src/features/autoMotion/useAutoMotion.ts` · Capa: **integración** · Fotografía: 2026-10-20.

Documentación de fuente; sin tests, compilación, navegador ni validación física.

## Responsabilidad y alcance

Edita objetivos y perfiles y presenta la evolución de misión/calibración AutoMotion.

Requests stm.goto.* y stm.motorCalibration.*, codec de respuestas y requisitos accionables.

<a id="modularity"></a>
## Modularidad observada

Codec de datos separado de UI; el algoritmo físico y perfil persistido son de F4.

<a id="state"></a>
## Handle, estado y atributos externos

Hooks conservan estado remoto y pending; paneles conservan x/y y borradores. No equiparar un destino editado con misión aceptada.

<a id="lifecycle"></a>
## Ciclo de vida y contexto

- Consultar estado/perfil, editar y enviar argumentos específicos por operación.
- Interpretar status de solicitud y reason/state/result_valid de la operación por separado.

<a id="interactions"></a>
## Interacciones

- ESP traduce xCm/yCm a dos float32; calibración usa bloques de 13/88/186 bytes.
- Cambios del formato deben actualizar codec, bridge y serializador F4.

<a id="limitations"></a>
## Precauciones y límites

- La posición mostrada es estimada; no es odometría medida.
- El validador compartido ESP admite formas incorrectas por comando; ver AMDOC-ESP-001.

## Hallazgos conectados


## Archivos y evidencia

| Archivo | Análisis | Hash SHA-256 |
| --- | --- | --- |
| `src/features/autoMotion/ControlNavigation.tsx` | typescript-ast-static | `213e67eabe94be54eba211923abc2114379613294a6aef0385beea27e16deb56` |
| `src/features/autoMotion/ControlToolbar.tsx` | typescript-ast-static | `64e7392f8e61a84c41360d2f2ae58fee9d7f94688793744a3b1c29f5c7440181` |
| `src/features/autoMotion/GotoPanel.tsx` | typescript-ast-static | `c73c4d684736c88c0423ef815341bb056c6de5e4e75c76b201f21b471dd2b184` |
| `src/features/autoMotion/MotionReadiness.tsx` | typescript-ast-static | `e4a874d07f861ad95d0dbb39941aa5ce40152c266cb106e55dbfa4dea467b2f3` |
| `src/features/autoMotion/MotorCalibration.tsx` | typescript-ast-static | `5df4ddbca5943f317c12fc12bb6f87b57cfb573e1df90eb19e4bac4f5e13b90f` |
| `src/features/autoMotion/MotorDialog.tsx` | typescript-ast-static | `2b9ab403a52999c176cf8ea97926deff9b2343309e036d2844a439b90e959bd5` |
| `src/features/autoMotion/autoMotion.css` | support-inventory | `8239cc0ebdfd255fd0f48a338569db97819356193319786997f03ed145309b45` |
| `src/features/autoMotion/motorCodec.ts` | typescript-ast-static | `e5fd46722e8599394345a2c5b99b1df147628f45e1ca0d7eaf252d7fadb5b9a5` |
| `src/features/autoMotion/motorDialog.css` | support-inventory | `38ce3e508d53eb14c486d088ae137f58041f5bcf71c10624f6424d554eaba9a3` |
| `src/features/autoMotion/useAutoMotion.ts` | typescript-ast-static | `ba80570a8f685025b3b9bd50bc9573de1449d1d52eb20fdb5a260bf14ec9f023` |
| `src/pages/MotorCalibrationSection.tsx` | typescript-ast-static | `34189bbe634433e1ad7b05e0250bd281dc96e0f4b62a0b697998b1a53a234d26` |

## Tipos, puertos, props y callbacks

En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.

### `ControlView`

`web:src/features/autoMotion/ControlNavigation.tsx:3`

Tipo: `"free" \| "goto" \| "calibration"`.

### `Run`

`web:src/features/autoMotion/MotorCalibration.tsx:11`

| Atributo / firma | Propósito |
| --- | --- |
| `date: string` | Dato de estado/configuración `date`; declaración estructural, interpretación específica pendiente de ampliar. |
| `notes: string` | Dato de estado/configuración `notes`; declaración estructural, interpretación específica pendiente de ampliar. |
| `state: AutoState` | Dato de estado/configuración `state`; declaración estructural, interpretación específica pendiente de ampliar. |

### `MotorDraft`

`web:src/features/autoMotion/MotorCalibration.tsx:13`

| Atributo / firma | Propósito |
| --- | --- |
| `percent: number` | Dato de estado/configuración `percent`; declaración estructural, interpretación específica pendiente de ampliar. |
| `ramp: number` | Dato de estado/configuración `ramp`; declaración estructural, interpretación específica pendiente de ampliar. |
| `timeout: number` | Dato de estado/configuración `timeout`; declaración estructural, interpretación específica pendiente de ampliar. |
| `notes: string` | Dato de estado/configuración `notes`; declaración estructural, interpretación específica pendiente de ampliar. |
| `curve: boolean` | Dato de estado/configuración `curve`; declaración estructural, interpretación específica pendiente de ampliar. |
| `levels: string` | Dato de estado/configuración `levels`; declaración estructural, interpretación específica pendiente de ampliar. |
| `repeats: number` | Dato de estado/configuración `repeats`; declaración estructural, interpretación específica pendiente de ampliar. |
| `trial: number` | Dato de estado/configuración `trial`; declaración estructural, interpretación específica pendiente de ampliar. |

### `MotorDialogIcon`

`web:src/features/autoMotion/MotorDialog.tsx:5`

Tipo: `"mode" \| "balance" \| "mpu" \| "objects" \| "connection" \| "line" \| "security" \| "calibration" \| "info" \| "settings"`.

### `AutoState`

`web:src/features/autoMotion/useAutoMotion.ts:4`

| Atributo / firma | Propósito |
| --- | --- |
| `status: number` | Último resultado y guardas del módulo; los getters pueden devolver una vista de este almacenamiento. |
| `version: number` | Dato de estado/configuración `version`; declaración estructural, interpretación específica pendiente de ampliar. |
| `kind: number` | Dato de estado/configuración `kind`; declaración estructural, interpretación específica pendiente de ampliar. |
| `state: number` | Dato de estado/configuración `state`; declaración estructural, interpretación específica pendiente de ampliar. |
| `reason: number` | Dato de estado/configuración `reason`; declaración estructural, interpretación específica pendiente de ampliar. |
| `carMode: number` | Dato de estado/configuración `carMode`; declaración estructural, interpretación específica pendiente de ampliar. |
| `missionId: number` | Dato de estado/configuración `missionId`; declaración estructural, interpretación específica pendiente de ampliar. |
| `elapsedMs: number` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `mpuReady: boolean` | Dato de estado/configuración `mpuReady`; declaración estructural, interpretación específica pendiente de ampliar. |
| `mpuCalibrated: boolean` | Dato de estado/configuración `mpuCalibrated`; declaración estructural, interpretación específica pendiente de ampliar. |
| `motorCalibrated: boolean` | Dato de estado/configuración `motorCalibrated`; declaración estructural, interpretación específica pendiente de ampliar. |
| `irCalibrated: boolean` | Dato de estado/configuración `irCalibrated`; declaración estructural, interpretación específica pendiente de ampliar. |
| `balanceActive: boolean` | Dato de estado/configuración `balanceActive`; declaración estructural, interpretación específica pendiente de ampliar. |
| `balanceSettled: boolean` | Dato de estado/configuración `balanceSettled`; declaración estructural, interpretación específica pendiente de ampliar. |
| `safetyStop: boolean` | Dato de estado/configuración `safetyStop`; declaración estructural, interpretación específica pendiente de ampliar. |
| `lineCalibrated: boolean` | Dato de estado/configuración `lineCalibrated`; declaración estructural, interpretación específica pendiente de ampliar. |
| `objectsEnabled: boolean` | Dato de estado/configuración `objectsEnabled`; declaración estructural, interpretación específica pendiente de ampliar. |
| `objectsFresh: boolean` | Dato de estado/configuración `objectsFresh`; declaración estructural, interpretación específica pendiente de ampliar. |
| `lineFresh: boolean` | Dato de estado/configuración `lineFresh`; declaración estructural, interpretación específica pendiente de ampliar. |
| `xCm: number` | Dato de estado/configuración `xCm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `yCm: number` | Dato de estado/configuración `yCm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `goalXCm: number` | Dato de estado/configuración `goalXCm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `goalYCm: number` | Dato de estado/configuración `goalYCm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `yawDeg: number` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `speedCmS: number` | Dato de estado/configuración `speedCmS`; declaración estructural, interpretación específica pendiente de ampliar. |
| `percent: number` | Dato de estado/configuración `percent`; declaración estructural, interpretación específica pendiente de ampliar. |
| `rampMs: number` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `measurementMs: number` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `pwmHz: number` | Dato de estado/configuración `pwmHz`; declaración estructural, interpretación específica pendiente de ampliar. |
| `leftDutyMean: number` | Dato de estado/configuración `leftDutyMean`; declaración estructural, interpretación específica pendiente de ampliar. |
| `rightDutyMean: number` | Dato de estado/configuración `rightDutyMean`; declaración estructural, interpretación específica pendiente de ampliar. |
| `leftDutyMax: number` | Dato de estado/configuración `leftDutyMax`; declaración estructural, interpretación específica pendiente de ampliar. |
| `rightDutyMax: number` | Dato de estado/configuración `rightDutyMax`; declaración estructural, interpretación específica pendiente de ampliar. |
| `leftDutySeconds: number` | Dato de estado/configuración `leftDutySeconds`; declaración estructural, interpretación específica pendiente de ampliar. |
| `rightDutySeconds: number` | Dato de estado/configuración `rightDutySeconds`; declaración estructural, interpretación específica pendiente de ampliar. |
| `leftPwmCycles: number` | Dato de estado/configuración `leftPwmCycles`; declaración estructural, interpretación específica pendiente de ampliar. |
| `rightPwmCycles: number` | Dato de estado/configuración `rightPwmCycles`; declaración estructural, interpretación específica pendiente de ampliar. |
| `leftForwardMs: number` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `leftReverseMs: number` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `rightForwardMs: number` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `rightReverseMs: number` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `pointCount: number` | Dato de estado/configuración `pointCount`; declaración estructural, interpretación específica pendiente de ampliar. |
| `phase: number` | Dato de estado/configuración `phase`; declaración estructural, interpretación específica pendiente de ampliar. |
| `resultValid: boolean` | Dato de estado/configuración `resultValid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `revision: number` | Dato de estado/configuración `revision`; declaración estructural, interpretación específica pendiente de ampliar. |
| `yawErrorMax: number` | Dato de estado/configuración `yawErrorMax`; declaración estructural, interpretación específica pendiente de ampliar. |

### `MotorPoint`

`web:src/features/autoMotion/useAutoMotion.ts:12`

| Atributo / firma | Propósito |
| --- | --- |
| `percent: number` | Dato de estado/configuración `percent`; declaración estructural, interpretación específica pendiente de ampliar. |
| `speedCmS: number` | Dato de estado/configuración `speedCmS`; declaración estructural, interpretación específica pendiente de ampliar. |
| `rampMs: number` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `runs: number` | Dato de estado/configuración `runs`; declaración estructural, interpretación específica pendiente de ampliar. |

### `MotorProfile`

`web:src/features/autoMotion/useAutoMotion.ts:13`

| Atributo / firma | Propósito |
| --- | --- |
| `status: number` | Último resultado y guardas del módulo; los getters pueden devolver una vista de este almacenamiento. |
| `version: number` | Dato de estado/configuración `version`; declaración estructural, interpretación específica pendiente de ampliar. |
| `pointCount: number` | Dato de estado/configuración `pointCount`; declaración estructural, interpretación específica pendiente de ampliar. |
| `revision: number` | Dato de estado/configuración `revision`; declaración estructural, interpretación específica pendiente de ampliar. |
| `irCalibrated: boolean` | Dato de estado/configuración `irCalibrated`; declaración estructural, interpretación específica pendiente de ampliar. |
| `points: MotorPoint[]` | Dato de estado/configuración `points`; declaración estructural, interpretación específica pendiente de ampliar. |

### `AutoDevice`

`web:src/features/autoMotion/useAutoMotion.ts:74`

Tipo: `ReturnType<typeof useAutoMotion>`.


## Funciones públicas / componentes / hooks

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-controlnavigation"></a>`ControlNavigation({ view, onModeChange }: { view: ControlView; onModeChange?: (view: "free" \| "goto") => void; }): inferred-by-TypeScript` | Adapta la llamada a `useNavigate` con el contexto de esta integración. | `web:src/features/autoMotion/ControlNavigation.tsx:4`; structural |
| <a id="fn-controltoolbar"></a>`ControlToolbar({control,recommended="IDLE",isSimulating=false,onStopSimulation}: { control:MotionController; recommended?:"IDLE"\|"GOTO"; isSimulating?:boolean; onStopSimulation?:()=>void; }): inferred-by-TypeScript` | Coordina `useCarMode`, `useState` para GOTO y calibración de motores Web. | `web:src/features/autoMotion/ControlToolbar.tsx:7`; structural |
| <a id="fn-gotopanel"></a>`GotoPanel({device,control}: {device:AutoDevice;control:MotionController}): inferred-by-TypeScript` | Edita objetivo relativo y solicita inicio/cancelación de recorrido. | `web:src/features/autoMotion/GotoPanel.tsx:10`; editorial |
| <a id="fn-motionreadiness"></a>`MotionReadiness({auto,mode,line,onMotors,onLine,control,calibration=false}: {auto:AutoDevice;mode:"GOTO"\|"FOLLOW"; line?:LineDeviceState\|null;onMotors?:()=>void;onLine?:()=>void;control?:MotionController;calibration?:boolean}): inferred-by-TypeScript` | Coordina `line.pattern.toString`, `line.pattern.toString(2).padStart`, `useIrEmitters` para GOTO y calibración de motores Web. | `web:src/features/autoMotion/MotionReadiness.tsx:14`; structural |
| <a id="fn-motorcalibration"></a>`MotorCalibration({device,motorControl}: {device?:AutoDevice;motorControl?:MotionController}): inferred-by-TypeScript` | Coordina `([["Tiempo del metro",`${s.measurementMs} ms`],["Velocidad media",`${s.speedCmS.toFixed(2)} cm/s`],["PWM medio izq. / der.",`${s.leftDutyMean.toFixed(1)} / ${s.rightDutyMean.toFixed(1)} %`],["Ciclos PWM izq. / der.",`${s.leftPwmCycles.toFixed(1)} / ${s.rightPwmCycles.toFixed(1)}`],["Frecuencia PWM",`${s.pwmHz.toFixed(2)} Hz`],["Desvío de rumbo",`${s.yawErrorMax.toFixed(1)}°`]] as const).map`, `(auto.profile?.points??[]).map`, `Math.floor` para GOTO y calibración de motores Web. | `web:src/features/autoMotion/MotorCalibration.tsx:15`; structural |
| <a id="fn-motorcalibrationsection"></a>`MotorCalibrationSection(): inferred-by-TypeScript` | Coordina `search.get`, `useAutoMotion`, `useMotionControl` para GOTO y calibración de motores Web. | `web:src/pages/MotorCalibrationSection.tsx:9`; structural |
| <a id="fn-motordialog"></a>`MotorDialog({isOpen,onClose,title,description,icon,kicker="Control · AutoMicro",badge,footer,children,size="compact",closeDisabled=false,closeOnOverlayClick=true}: { isOpen:boolean;onClose:()=>void;title:string;description:string;icon:MotorDialogIcon;kicker?:string; badge?:ReactNode;footer?:ReactNode;children:ReactNode;size?:"compact"\|"wide";closeDisabled?:boolean;closeOnOverlayClick?:boolean; }): inferred-by-TypeScript` | Adapta la llamada a `useId` con el contexto de esta integración. | `web:src/features/autoMotion/MotorDialog.tsx:18`; structural |
| <a id="fn-autoreason"></a>`autoReason(n: number): inferred-by-TypeScript` | Operación auto Reason de GOTO y calibración de motores Web; semántica fina pendiente de ampliación. | `web:src/features/autoMotion/useAutoMotion.ts:14`; inferred-name |
| <a id="fn-destinationdraft"></a>`destinationDraft(): {x:number;y:number}` | Coordina `JSON.parse`, `Number.isFinite`, `sessionStorage.getItem` para GOTO y calibración de motores Web. | `web:src/features/autoMotion/GotoPanel.tsx:9`; structural |
| <a id="fn-downloadmotorjson"></a>`downloadMotorJson(value: unknown): inferred-by-TypeScript` | Coordina `JSON.stringify`, `URL.createObjectURL`, `document.createElement` para GOTO y calibración de motores Web. | `web:src/features/autoMotion/motorCodec.ts:22`; structural |
| <a id="fn-history"></a>`history(): Run[]` | Coordina `Array.isArray`, `JSON.parse`, `localStorage.getItem` para GOTO y calibración de motores Web. | `web:src/features/autoMotion/MotorCalibration.tsx:12`; structural |
| <a id="fn-motordraft"></a>`motorDraft(): MotorDraft` | Coordina `JSON.parse`, `["percent","ramp","timeout","repeats","trial"].every`, `sessionStorage.getItem` para GOTO y calibración de motores Web. | `web:src/features/autoMotion/MotorCalibration.tsx:14`; structural |
| <a id="fn-packmotorprofile"></a>`packMotorProfile(p: MotorProfile): number[]` | Coordina `Array.from`, `Array.isArray`, `p.points.forEach` para GOTO y calibración de motores Web. | `web:src/features/autoMotion/motorCodec.ts:4`; structural |
| <a id="fn-packstoredir"></a>`packStoredIr(): number[]` | Coordina `Array.from`, `IR_OBJECT_SENSOR_KEYS.forEach`, `loadStoredIrCalibrationProfile` para GOTO y calibración de motores Web. | `web:src/features/autoMotion/motorCodec.ts:10`; structural |
| <a id="fn-parse"></a>`parse(value: unknown): AutoState` | Coordina `Number.isInteger`, `["mpuReady","mpuCalibrated","motorCalibrated","irCalibrated","balanceActive","balanceSettled","safetyStop"].every`, `[0,1].includes` para GOTO y calibración de motores Web. | `web:src/features/autoMotion/useAutoMotion.ts:19`; structural |
| <a id="fn-useautomotion"></a>`useAutoMotion(active: inferred-by-TypeScript): inferred-by-TypeScript` | Consulta/controla operación AutoMotion y conserva estado confirmado de requests. | `web:src/features/autoMotion/useAutoMotion.ts:26`; editorial |

## Funciones y callbacks internos

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-motionreadiness.showdialog"></a>`MotionReadiness.showDialog(next: NonNullable<typeof dialog>): inferred-by-TypeScript` | Coordina `setDetailDialog`, `setDialog` para GOTO y calibración de motores Web. | `web:src/features/autoMotion/MotionReadiness.tsx:18`; structural |
| <a id="fn-motorcalibration.importfile"></a>`MotorCalibration.importFile(file: File\|undefined): inferred-by-TypeScript` | Coordina `JSON.parse`, `auto.send`, `file.text` para GOTO y calibración de motores Web. | `web:src/features/autoMotion/MotorCalibration.tsx:47`; structural |
| <a id="fn-motorcalibration.save"></a>`MotorCalibration.save(): inferred-by-TypeScript` | Coordina `auto.send`, `control.run`, `setFeedback` para GOTO y calibración de motores Web. | `web:src/features/autoMotion/MotorCalibration.tsx:38`; structural |
| <a id="fn-motorcalibration.transferir"></a>`MotorCalibration.transferIr(): inferred-by-TypeScript` | Coordina `auto.send`, `packStoredIr`, `setFeedback` para GOTO y calibración de motores Web. | `web:src/features/autoMotion/MotorCalibration.tsx:44`; structural |
| <a id="fn-useautomotion.poll"></a>`useAutoMotion.poll(): inferred-by-TypeScript` | Coordina `refresh`, `refreshProfile`, `setError` para GOTO y calibración de motores Web. | `web:src/features/autoMotion/useAutoMotion.ts:47`; structural |
| <a id="fn-useautomotion.send"></a>`useAutoMotion.send(name: string, args: Record<string,unknown>): inferred-by-TypeScript` | Coordina `autoReason`, `name.endsWith`, `parse` para GOTO y calibración de motores Web. | `web:src/features/autoMotion/useAutoMotion.ts:55`; structural |

## Estado y efectos React encontrados

| Hook | Función/closure | Evidencia |
| --- | --- | --- |
| `useNavigate` | `ControlNavigation` | `web:src/features/autoMotion/ControlNavigation.tsx:7` |
| `useCarMode` | `ControlToolbar` | `web:src/features/autoMotion/ControlToolbar.tsx:10` |
| `useState` | `ControlToolbar` | `web:src/features/autoMotion/ControlToolbar.tsx:10` |
| `useState` | `GotoPanel` | `web:src/features/autoMotion/GotoPanel.tsx:11` |
| `useState` | `GotoPanel` | `web:src/features/autoMotion/GotoPanel.tsx:11` |
| `useState` | `GotoPanel` | `web:src/features/autoMotion/GotoPanel.tsx:11` |
| `useRef` | `GotoPanel` | `web:src/features/autoMotion/GotoPanel.tsx:11` |
| `useEffect` | `GotoPanel` | `web:src/features/autoMotion/GotoPanel.tsx:12` |
| `useNavigate` | `MotionReadiness` | `web:src/features/autoMotion/MotionReadiness.tsx:16` |
| `useState` | `MotionReadiness` | `web:src/features/autoMotion/MotionReadiness.tsx:16` |
| `useState` | `MotionReadiness` | `web:src/features/autoMotion/MotionReadiness.tsx:17` |
| `useIrEmitters` | `MotionReadiness` | `web:src/features/autoMotion/MotionReadiness.tsx:19` |
| `useState` | `MotionReadiness` | `web:src/features/autoMotion/MotionReadiness.tsx:19` |
| `useMotionControl` | `MotionReadiness` | `web:src/features/autoMotion/MotionReadiness.tsx:20` |
| `useAutoMotion` | `MotorCalibration` | `web:src/features/autoMotion/MotorCalibration.tsx:16` |
| `useMotionControl` | `MotorCalibration` | `web:src/features/autoMotion/MotorCalibration.tsx:16` |
| `useNavigate` | `MotorCalibration` | `web:src/features/autoMotion/MotorCalibration.tsx:16` |
| `useState` | `MotorCalibration` | `web:src/features/autoMotion/MotorCalibration.tsx:17` |
| `useState` | `MotorCalibration` | `web:src/features/autoMotion/MotorCalibration.tsx:18` |
| `useState` | `MotorCalibration` | `web:src/features/autoMotion/MotorCalibration.tsx:18` |
| `useState` | `MotorCalibration` | `web:src/features/autoMotion/MotorCalibration.tsx:18` |
| `useState` | `MotorCalibration` | `web:src/features/autoMotion/MotorCalibration.tsx:18` |
| `useState` | `MotorCalibration` | `web:src/features/autoMotion/MotorCalibration.tsx:19` |
| `useState` | `MotorCalibration` | `web:src/features/autoMotion/MotorCalibration.tsx:19` |
| `useState` | `MotorCalibration` | `web:src/features/autoMotion/MotorCalibration.tsx:19` |
| `useState` | `MotorCalibration` | `web:src/features/autoMotion/MotorCalibration.tsx:19` |
| `useState` | `MotorCalibration` | `web:src/features/autoMotion/MotorCalibration.tsx:19` |
| `useState` | `MotorCalibration` | `web:src/features/autoMotion/MotorCalibration.tsx:20` |
| `useState` | `MotorCalibration` | `web:src/features/autoMotion/MotorCalibration.tsx:20` |
| `useRef` | `MotorCalibration` | `web:src/features/autoMotion/MotorCalibration.tsx:20` |
| `useState` | `MotorCalibration` | `web:src/features/autoMotion/MotorCalibration.tsx:21` |
| `useEffect` | `MotorCalibration` | `web:src/features/autoMotion/MotorCalibration.tsx:22` |
| `useEffect` | `MotorCalibration` | `web:src/features/autoMotion/MotorCalibration.tsx:30` |
| `useId` | `MotorDialog` | `web:src/features/autoMotion/MotorDialog.tsx:22` |
| `useWebSocket` | `useAutoMotion` | `web:src/features/autoMotion/useAutoMotion.ts:27` |
| `useUser` | `useAutoMotion` | `web:src/features/autoMotion/useAutoMotion.ts:27` |
| `useState` | `useAutoMotion` | `web:src/features/autoMotion/useAutoMotion.ts:29` |
| `useState` | `useAutoMotion` | `web:src/features/autoMotion/useAutoMotion.ts:29` |
| `useState` | `useAutoMotion` | `web:src/features/autoMotion/useAutoMotion.ts:30` |
| `useState` | `useAutoMotion` | `web:src/features/autoMotion/useAutoMotion.ts:30` |
| `useRef` | `useAutoMotion` | `web:src/features/autoMotion/useAutoMotion.ts:31` |
| `useRef` | `useAutoMotion` | `web:src/features/autoMotion/useAutoMotion.ts:31` |
| `useRef` | `useAutoMotion` | `web:src/features/autoMotion/useAutoMotion.ts:31` |
| `useRef` | `useAutoMotion` | `web:src/features/autoMotion/useAutoMotion.ts:31` |
| `useCallback` | `useAutoMotion` | `web:src/features/autoMotion/useAutoMotion.ts:32` |
| `useCallback` | `useAutoMotion` | `web:src/features/autoMotion/useAutoMotion.ts:38` |
| `useEffect` | `useAutoMotion` | `web:src/features/autoMotion/useAutoMotion.ts:44` |
| `useAutoMotion` | `MotorCalibrationSection` | `web:src/pages/MotorCalibrationSection.tsx:10` |
| `useMotionControl` | `MotorCalibrationSection` | `web:src/pages/MotorCalibrationSection.tsx:10` |
| `useSearchParams` | `MotorCalibrationSection` | `web:src/pages/MotorCalibrationSection.tsx:10` |
