# Cámara y controles de orientación

ID: `web:module/src/components/CameraRig.tsx` · Capa: **integración** · Fotografía: 2026-10-20.

Documentación de fuente; sin tests, compilación, navegador ni validación física.

## Responsabilidad y alcance

Define vistas y manipulación de cámara de la escena del robot.

Presets, interacción y coordenadas visuales; cambiar cámara no modifica pose del equipo.

<a id="modularity"></a>
## Modularidad observada

UI/Three.js independiente de control físico.

<a id="state"></a>
## Handle, estado y atributos externos

Refs/estado local de cámara y controles; distinguir orientación de sensor de orientación de observador.

<a id="lifecycle"></a>
## Ciclo de vida y contexto

- Inicializar vista, aplicar preset o gesto y liberar listeners/controles al salir.

<a id="interactions"></a>
## Interacciones

- Escena IR y modelo usan transformaciones comunes para cámara, haces y marcadores.

<a id="limitations"></a>
## Precauciones y límites

- No comparar yaw visual con yaw físico sin aplicar la misma convención de ejes/referencia.

## Hallazgos conectados


## Archivos y evidencia

| Archivo | Análisis | Hash SHA-256 |
| --- | --- | --- |
| `src/components/CameraPresets.tsx` | typescript-ast-static | `1add9ea9d668deec57102960f3ad3a0714bbcb636dfc8459a5304db9b7d87f47` |
| `src/components/CameraPresetsPanel.tsx` | typescript-ast-static | `b031e649a8488e436aee07bb7c727128db3fa20a27cecf25b03a9304659f6cb6` |
| `src/components/CameraRig.tsx` | typescript-ast-static | `28c7fc2fc3a25728e19898733f6cad953392c846aa87b103f4f061e54da45e9d` |
| `src/components/OrientationControls.tsx` | typescript-ast-static | `ae9488a1a209ad06ddeffea47c157921b99690d3e747fc91092c64694bdacebf` |
| `src/components/SensorCameraViewIcon.tsx` | typescript-ast-static | `54107a5d7aed48cd734764e81aa4b2e4dddc64840c8021eaea6a9cd3275bc150` |

## Tipos, puertos, props y callbacks

En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.

### `PresetKey`

`web:src/components/CameraPresets.tsx:5`

Tipo: `\| "front" \| "back" \| "left" \| "right" \| "top" \| "bottom" \| "isoNE" \| "isoNW"`.

### `CameraPresetsProps`

`web:src/components/CameraPresets.tsx:15`

| Atributo / firma | Propósito |
| --- | --- |
| `target?: THREE.Vector3 \| [number, number, number]` | Dato de estado/configuración `target`; declaración estructural, interpretación específica pendiente de ampliar. |
| `durationMs?: number` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `className?: string` | Dato de estado/configuración `className`; declaración estructural, interpretación específica pendiente de ampliar. |

### `PresetKey`

`web:src/components/CameraRig.tsx:5`

Tipo: `\| "front" \| "back" \| "left" \| "right" \| "top" \| "bottom" \| "isoNE" \| "isoNW"`.

### `CameraRigHandle`

`web:src/components/CameraRig.tsx:15`

| Atributo / firma | Propósito |
| --- | --- |
| `goTo: (preset: PresetKey) => void` | Operación go To de Cámara y controles de orientación; semántica fina pendiente de ampliación. |

### `CameraRigProps`

`web:src/components/CameraRig.tsx:19`

| Atributo / firma | Propósito |
| --- | --- |
| `target?: [number, number, number] \| THREE.Vector3` | Dato de estado/configuración `target`; declaración estructural, interpretación específica pendiente de ampliar. |
| `durationMs?: number` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |

### `OrientationControlsProps`

`web:src/components/OrientationControls.tsx:3`

| Atributo / firma | Propósito |
| --- | --- |
| `eulerDeg: { yaw: number; pitch: number; roll: number }` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `isEmu: boolean` | Dato de estado/configuración `isEmu`; declaración estructural, interpretación específica pendiente de ampliar. |
| `onChange: (euler: { yaw: number; pitch: number; roll: number }) => void` | Operación on Change de Cámara y controles de orientación; semántica fina pendiente de ampliación. |
| `autoAxes: Record<"yaw" \| "pitch" \| "roll", boolean>` | Dato de estado/configuración `autoAxes`; declaración estructural, interpretación específica pendiente de ampliar. |
| `onAutoAxisChange: (axis: "yaw" \| "pitch" \| "roll", enabled: boolean) => void` | Operación on Auto Axis Change de Cámara y controles de orientación; semántica fina pendiente de ampliación. |

### `Vector`

`web:src/components/SensorCameraViewIcon.tsx:1`

Tipo: `readonly [number, number, number]`.

### `Face`

`web:src/components/SensorCameraViewIcon.tsx:2`

| Atributo / firma | Propósito |
| --- | --- |
| `normal: Vector` | Dato de estado/configuración `normal`; declaración estructural, interpretación específica pendiente de ampliar. |
| `corners: readonly Vector[]` | Dato de estado/configuración `corners`; declaración estructural, interpretación específica pendiente de ampliar. |
| `opacity: number` | Dato de estado/configuración `opacity`; declaración estructural, interpretación específica pendiente de ampliar. |


## Funciones públicas / componentes / hooks

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-camerapresets"></a>`CameraPresets({  target = [0, 0, 0],  durationMs = 450,  className = "", }: CameraPresetsProps): inferred-by-TypeScript` | Coordina `[ "rounded-2xl bg-[var(--ui-panel-hover)] dark:bg-[var(--ui-panel)]", "ring-1 ring-[var(--ui-ring)] shadow-sm backdrop-blur p-4", "transition-shadow hover:shadow-md", className, ].join`, `useCallback`, `useFrame` para Cámara y controles de orientación. | `web:src/components/CameraPresets.tsx:24`; structural |
| <a id="fn-camerapresetspanel"></a>`CameraPresetsPanel({   onPick,   className = "", }: { onPick: (k: PresetKey) => void; className?: string; }): inferred-by-TypeScript` | Operación Camera Presets Panel de Cámara y controles de orientación; semántica fina pendiente de ampliación. | `web:src/components/CameraPresetsPanel.tsx:3`; inferred-name |
| <a id="fn-orientationcontrols"></a>`OrientationControls({  eulerDeg,  isEmu,  onChange,  autoAxes,  onAutoAxisChange, }: OrientationControlsProps): inferred-by-TypeScript` | Coordina `React.useId`, `[ { label: "Guiñada / Yaw (Y)", key: "yaw", min: -180, max: 180, desc: "Rotación horizontal", }, { label: "Cabeceo / Pitch (X)", key: "pitch", min: -90, max: 90, desc: "Inclinación vertical", }, { label: "Alabeo / Roll (Z)", key: "roll", min: -180, max: 180, desc: "Rotación sobre eje", }, ].map` para Cámara y controles de orientación. | `web:src/components/OrientationControls.tsx:11`; structural |
| <a id="fn-sensorcameraviewicon"></a>`SensorCameraViewIcon({ position, up = [0, 1, 0] }: { position: Vector; up?: Vector; }): inferred-by-TypeScript` | Coordina `FACES.filter`, `FACES.filter((face) => dot(face.normal, direction) > .001).map`, `cross` para Cámara y controles de orientación. | `web:src/components/SensorCameraViewIcon.tsx:15`; structural |
| <a id="fn-cross"></a>`cross(a: Vector, b: Vector): Vector` | Operación cross de Cámara y controles de orientación; semántica fina pendiente de ampliación. | `web:src/components/SensorCameraViewIcon.tsx:42`; inferred-name |
| <a id="fn-dirfor"></a>`dirFor(k: PresetKey): THREE.Vector3` | Operación dir For de Cámara y controles de orientación; semántica fina pendiente de ampliación. | `web:src/components/CameraRig.tsx:48`; inferred-name |
| <a id="fn-dot"></a>`dot(a: Vector, b: Vector): inferred-by-TypeScript` | Operación dot de Cámara y controles de orientación; semántica fina pendiente de ampliación. | `web:src/components/SensorCameraViewIcon.tsx:41`; inferred-name |
| <a id="fn-goto"></a>`goTo(preset: PresetKey): inferred-by-TypeScript` | Coordina `Math.max`, `camera.position.clone`, `camera.position.clone().sub` para Cámara y controles de orientación. | `web:src/components/CameraRig.tsx:75`; structural |
| <a id="fn-normalize"></a>`normalize(vector: Vector): Vector` | Adapta la llamada a `Math.hypot` con el contexto de esta integración. | `web:src/components/SensorCameraViewIcon.tsx:45`; structural |
| <a id="fn-starttweento"></a>`startTweenTo(to: THREE.Vector3): inferred-by-TypeScript` | Coordina `fromPos.current.copy`, `performance.now`, `toPos.current.copy` para Cámara y controles de orientación. | `web:src/components/CameraRig.tsx:41`; structural |

## Funciones y callbacks internos

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-camerapresets.btn"></a>`CameraPresets.Btn(p: { onClick: () => void; label: string }): inferred-by-TypeScript` | Operación Btn de Cámara y controles de orientación; semántica fina pendiente de ampliación. | `web:src/components/CameraPresets.tsx:86`; inferred-name |
| <a id="fn-camerapresets.back"></a>`CameraPresets.back(): inferred-by-TypeScript` | Adapta la llamada a `go` con el contexto de esta integración. | `web:src/components/CameraPresets.tsx:76`; structural |
| <a id="fn-camerapresets.bottom"></a>`CameraPresets.bottom(): inferred-by-TypeScript` | Adapta la llamada a `go` con el contexto de esta integración. | `web:src/components/CameraPresets.tsx:80`; structural |
| <a id="fn-camerapresets.front"></a>`CameraPresets.front(): inferred-by-TypeScript` | Adapta la llamada a `go` con el contexto de esta integración. | `web:src/components/CameraPresets.tsx:75`; structural |
| <a id="fn-camerapresets.isone"></a>`CameraPresets.isoNE(): inferred-by-TypeScript` | Adapta la llamada a `go` con el contexto de esta integración. | `web:src/components/CameraPresets.tsx:82`; structural |
| <a id="fn-camerapresets.isonw"></a>`CameraPresets.isoNW(): inferred-by-TypeScript` | Adapta la llamada a `go` con el contexto de esta integración. | `web:src/components/CameraPresets.tsx:83`; structural |
| <a id="fn-camerapresets.left"></a>`CameraPresets.left(): inferred-by-TypeScript` | Adapta la llamada a `go` con el contexto de esta integración. | `web:src/components/CameraPresets.tsx:77`; structural |
| <a id="fn-camerapresets.right"></a>`CameraPresets.right(): inferred-by-TypeScript` | Adapta la llamada a `go` con el contexto de esta integración. | `web:src/components/CameraPresets.tsx:78`; structural |
| <a id="fn-camerapresets.top"></a>`CameraPresets.top(): inferred-by-TypeScript` | Adapta la llamada a `go` con el contexto de esta integración. | `web:src/components/CameraPresets.tsx:79`; structural |
| <a id="fn-camerapresetspanel.btn"></a>`CameraPresetsPanel.Btn({ label, k }: { label: string; k: PresetKey }): inferred-by-TypeScript` | Operación Btn de Cámara y controles de orientación; semántica fina pendiente de ampliación. | `web:src/components/CameraPresetsPanel.tsx:10`; inferred-name |
| <a id="fn-orientationcontrols.handlequickrotation"></a>`OrientationControls.handleQuickRotation(axis: "yaw" \| "pitch" \| "roll", degrees: number): inferred-by-TypeScript` | Coordina `Math.max`, `Math.min`, `onChange` para Cámara y controles de orientación. | `web:src/components/OrientationControls.tsx:29`; structural |
| <a id="fn-orientationcontrols.handlereset"></a>`OrientationControls.handleReset(): inferred-by-TypeScript` | Adapta la llamada a `onChange` con el contexto de esta integración. | `web:src/components/OrientationControls.tsx:25`; structural |
| <a id="fn-orientationcontrols.handlesliderchange"></a>`OrientationControls.handleSliderChange(axis: "yaw" \| "pitch" \| "roll"): inferred-by-TypeScript` | Operación handle Slider Change de Cámara y controles de orientación; semántica fina pendiente de ampliación. | `web:src/components/OrientationControls.tsx:20`; inferred-name |
| <a id="fn-sensorcameraviewicon.project"></a>`SensorCameraViewIcon.project(point: Vector): inferred-by-TypeScript` | Adapta la llamada a `dot` con el contexto de esta integración. | `web:src/components/SensorCameraViewIcon.tsx:22`; structural |
| <a id="fn-sensorcameraviewicon.projectpoints"></a>`SensorCameraViewIcon.projectPoints(points: readonly Vector[]): inferred-by-TypeScript` | Coordina `points.map`, `points.map(project).join` para Cámara y controles de orientación. | `web:src/components/SensorCameraViewIcon.tsx:23`; structural |

## Estado y efectos React encontrados

| Hook | Función/closure | Evidencia |
| --- | --- | --- |
| `useThree` | `CameraPresets` | `web:src/components/CameraPresets.tsx:29` |
| `useMemo` | `CameraPresets` | `web:src/components/CameraPresets.tsx:30` |
| `useRef` | `CameraPresets` | `web:src/components/CameraPresets.tsx:37` |
| `useRef` | `CameraPresets` | `web:src/components/CameraPresets.tsx:38` |
| `useRef` | `CameraPresets` | `web:src/components/CameraPresets.tsx:39` |
| `useRef` | `CameraPresets` | `web:src/components/CameraPresets.tsx:40` |
| `useCallback` | `CameraPresets` | `web:src/components/CameraPresets.tsx:42` |
| `useFrame` | `CameraPresets` | `web:src/components/CameraPresets.tsx:52` |
| `useCallback` | `CameraPresets` | `web:src/components/CameraPresets.tsx:64` |
| `useThree` | `` | `web:src/components/CameraRig.tsx:27` |
| `useMemo` | `` | `web:src/components/CameraRig.tsx:29` |
| `useRef` | `` | `web:src/components/CameraRig.tsx:36` |
| `useRef` | `` | `web:src/components/CameraRig.tsx:37` |
| `useRef` | `` | `web:src/components/CameraRig.tsx:38` |
| `useRef` | `` | `web:src/components/CameraRig.tsx:39` |
| `useImperativeHandle` | `` | `web:src/components/CameraRig.tsx:72` |
| `useFrame` | `` | `web:src/components/CameraRig.tsx:86` |
| `React.useId` | `OrientationControls` | `web:src/components/OrientationControls.tsx:18` |
