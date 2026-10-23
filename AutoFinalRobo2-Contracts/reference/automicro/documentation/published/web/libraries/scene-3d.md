# Escena 3D y carga de modelos

ID: `web:module/src/components/MpuIrScene3D.tsx` · Capa: **integración** · Fotografía: 2026-10-20.

Documentación de fuente; sin tests, compilación, navegador ni validación física.

## Responsabilidad y alcance

Carga el modelo del robot y aplica orientación/sensores a la escena WebGL.

Modelo, materiales, referencias y fallback de carga; no representa por sí mismo la pose física exacta sin datos válidos.

<a id="modularity"></a>
## Modularidad observada

Adaptación React/Three.js con datos de telemetría separados.

<a id="state"></a>
## Handle, estado y atributos externos

Recursos GPU/modelo, refs y estado de carga tienen lifetime de la escena; liberar recursos/listeners al reemplazar.

<a id="lifecycle"></a>
## Ciclo de vida y contexto

- Cargar GLB y decodificador configurado, montar escena y aplicar datos.
- Mostrar error/fallback cuando el asset no está disponible y limpiar al desmontar.

<a id="interactions"></a>
## Interacciones

- ESP sirve asset y bundle; sensores/cámara usan sistemas de ejes que deben documentarse juntos.

<a id="limitations"></a>
## Precauciones y límites

- DRACO wasm no es un port wasm de la biblioteca OLED.
- Carga/render WebGL exitoso no valida telemetría física.

## Hallazgos conectados


## Archivos y evidencia

| Archivo | Análisis | Hash SHA-256 |
| --- | --- | --- |
| `src/components/ModelLoadBoundary.tsx` | typescript-ast-static | `41ef61acf8ae7e59af812d422801fe82f972f7826d66cf75d16ae7ac645278ec` |
| `src/components/ModelLoadingScreen.tsx` | typescript-ast-static | `908a9ebb21533dbadf056a7a6758b59813ac61baef650ada65175edd8373eb4f` |
| `src/components/MpuIrScene3D.tsx` | typescript-ast-static | `19ace22b558c1e492bb75e8c9e862e2b8cc943942772e726ce449d39f900e586` |
| `src/components/ThreeModelViewer.tsx` | typescript-ast-static | `6bb5436fd5e2a1164ac495c7d2ea85dd0b166ab297a6a2fb541e51597583f881` |
| `src/components/WebGLCanvas.tsx` | typescript-ast-static | `6c0b9dc36457141353f78b817f19e543d1f2b6e6e82a7e3314410ce59bb30af4` |
| `src/hooks/useModelLoadingState.ts` | typescript-ast-static | `fbdb9a194f07e04ce37aff7ee4eda3b3803e96b87fab9e9e660877858c122db4` |
| `src/utils/dracoLoader.ts` | typescript-ast-static | `0e9606fe9606507a0ea291c898f1639ed6f696e2acb18df9b2d17561ace42310` |
| `src/utils/localModel.ts` | typescript-ast-static | `c6f5da578d5e232cc0d52a8d5e2111466fabdaa8286279d0c413337ff36c1581` |

## Tipos, puertos, props y callbacks

En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.

### `Props`

`web:src/components/ModelLoadBoundary.tsx:4`

| Atributo / firma | Propósito |
| --- | --- |
| `modelUrl: string` | Dato de estado/configuración `modelUrl`; declaración estructural, interpretación específica pendiente de ampliar. |
| `children: ReactNode` | Dato de estado/configuración `children`; declaración estructural, interpretación específica pendiente de ampliar. |

### `State`

`web:src/components/ModelLoadBoundary.tsx:9`

| Atributo / firma | Propósito |
| --- | --- |
| `modelUrl: string` | Dato de estado/configuración `modelUrl`; declaración estructural, interpretación específica pendiente de ampliar. |
| `failed: boolean` | Dato de estado/configuración `failed`; declaración estructural, interpretación específica pendiente de ampliar. |

### `ModelLoadBoundary`

`web:src/components/ModelLoadBoundary.tsx:15`

| Atributo / firma | Propósito |
| --- | --- |
| `state: State` | Dato de estado/configuración `state`; declaración estructural, interpretación específica pendiente de ampliar. |
| `getDerivedStateFromProps(props: Props, state: State): inferred-by-TypeScript` | Operación get Derived State From Props de Escena 3D y carga de modelos; semántica fina pendiente de ampliación. |
| `getDerivedStateFromError(): inferred-by-TypeScript` | Operación get Derived State From Error de Escena 3D y carga de modelos; semántica fina pendiente de ampliación. |
| `componentDidCatch(error: Error): inferred-by-TypeScript` | Operación component Did Catch de Escena 3D y carga de modelos; semántica fina pendiente de ampliación. |
| `render(): inferred-by-TypeScript` | Operación render de Escena 3D y carga de modelos; semántica fina pendiente de ampliación. |

### `EulerDeg`

`web:src/components/MpuIrScene3D.tsx:58`

Tipo: `IrSensorRigEulerDeg`.

### `MpuIrScene3DHandle`

`web:src/components/MpuIrScene3D.tsx:60`

| Atributo / firma | Propósito |
| --- | --- |
| `resetCamera: () => void` | Operación reset Camera de Escena 3D y carga de modelos; semántica fina pendiente de ampliación. |
| `setCameraPreset: (preset: SensorCameraPreset) => void` | Operación set Camera Preset de Escena 3D y carga de modelos; semántica fina pendiente de ampliación. |
| `clearIrTarget: () => void` | Operación clear Ir Target de Escena 3D y carga de modelos; semántica fina pendiente de ampliación. |

### `MpuIrScene3DProps`

`web:src/components/MpuIrScene3D.tsx:66`

| Atributo / firma | Propósito |
| --- | --- |
| `modelUrl: string` | Dato de estado/configuración `modelUrl`; declaración estructural, interpretación específica pendiente de ampliar. |
| `eulerDeg: EulerDeg` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `irNorm: Record<IrSensorKey, number>` | Dato de estado/configuración `irNorm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `objectDetections: IrObjectAxialDetections` | Dato de estado/configuración `objectDetections`; declaración estructural, interpretación específica pendiente de ampliar. |
| `showGrid: boolean` | Dato de estado/configuración `showGrid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `showObjectAxes: boolean` | Dato de estado/configuración `showObjectAxes`; declaración estructural, interpretación específica pendiente de ampliar. |
| `showLineAxes: boolean` | Dato de estado/configuración `showLineAxes`; declaración estructural, interpretación específica pendiente de ampliar. |
| `beamOpacities: BeamOpacities` | Dato de estado/configuración `beamOpacities`; declaración estructural, interpretación específica pendiente de ampliar. |
| `showOrigin: boolean` | Dato de estado/configuración `showOrigin`; declaración estructural, interpretación específica pendiente de ampliar. |
| `sensorSensitivityCm: number` | Dato de estado/configuración `sensorSensitivityCm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `irSamplePeriodMs: number` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `renderLink: boolean` | Dato de estado/configuración `renderLink`; declaración estructural, interpretación específica pendiente de ampliar. |
| `irEmu: boolean` | Dato de estado/configuración `irEmu`; declaración estructural, interpretación específica pendiente de ampliar. |
| `irAutoMotion?: boolean` | Dato de estado/configuración `irAutoMotion`; declaración estructural, interpretación específica pendiente de ampliar. |
| `estimatedTarget: IrObjectPositionEstimate \| null` | Dato de estado/configuración `estimatedTarget`; declaración estructural, interpretación específica pendiente de ampliar. |
| `onEmulatedTarget: (point: IrObjectPoint) => void` | Operación on Emulated Target de Escena 3D y carga de modelos; semántica fina pendiente de ampliación. |
| `onManualCameraChange: () => void` | Operación on Manual Camera Change de Escena 3D y carga de modelos; semántica fina pendiente de ampliación. |

### `SceneCameraControllerHandle`

`web:src/components/MpuIrScene3D.tsx:291`

| Atributo / firma | Propósito |
| --- | --- |
| `resetCamera: () => void` | Operación reset Camera de Escena 3D y carga de modelos; semántica fina pendiente de ampliación. |
| `setCameraPreset: (preset: SensorCameraPreset) => void` | Operación set Camera Preset de Escena 3D y carga de modelos; semántica fina pendiente de ampliación. |

### `ThreeElements`

`web:src/components/ThreeModelViewer.tsx:13`

| Atributo / firma | Propósito |
| --- | --- |
| `orbitControls: ThreeElements["primitive"] & { args: ConstructorParameters<typeof OrbitControls>; }` | Dato de estado/configuración `orbitControls`; declaración estructural, interpretación específica pendiente de ampliar. |

### `ThreeModelViewerProps`

`web:src/components/ThreeModelViewer.tsx:20`

| Atributo / firma | Propósito |
| --- | --- |
| `modelUrl: string` | Dato de estado/configuración `modelUrl`; declaración estructural, interpretación específica pendiente de ampliar. |
| `eulerDeg: { yaw: number; pitch: number; roll: number; }` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `allowControls?: boolean` | Dato de estado/configuración `allowControls`; declaración estructural, interpretación específica pendiente de ampliar. |
| `exposure?: number` | Dato de estado/configuración `exposure`; declaración estructural, interpretación específica pendiente de ampliar. |
| `background?: string` | Dato de estado/configuración `background`; declaración estructural, interpretación específica pendiente de ampliar. |
| `classNames?: string` | Dato de estado/configuración `classNames`; declaración estructural, interpretación específica pendiente de ampliar. |
| `childrenInsideCanvas?: React.ReactNode` | Dato de estado/configuración `childrenInsideCanvas`; declaración estructural, interpretación específica pendiente de ampliar. |

### `ModelProps`

`web:src/components/ThreeModelViewer.tsx:43`

| Atributo / firma | Propósito |
| --- | --- |
| `url: string` | Dato de estado/configuración `url`; declaración estructural, interpretación específica pendiente de ampliar. |
| `eulerDeg: { yaw: number; pitch: number; roll: number; }` | Ángulo en grados; respetar eje y referencia de esta librería. |

### `ThreeModelViewerProps`

`web:src/components/ThreeModelViewer.tsx:151`

| Atributo / firma | Propósito |
| --- | --- |
| `modelUrl: string` | Dato de estado/configuración `modelUrl`; declaración estructural, interpretación específica pendiente de ampliar. |
| `eulerDeg: { yaw: number; pitch: number; roll: number }` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `allowControls?: boolean` | Dato de estado/configuración `allowControls`; declaración estructural, interpretación específica pendiente de ampliar. |
| `exposure?: number` | Dato de estado/configuración `exposure`; declaración estructural, interpretación específica pendiente de ampliar. |
| `background?: string` | Dato de estado/configuración `background`; declaración estructural, interpretación específica pendiente de ampliar. |
| `className?: string` | Dato de estado/configuración `className`; declaración estructural, interpretación específica pendiente de ampliar. |
| `childrenInsideCanvas?: React.ReactNode` | Dato de estado/configuración `childrenInsideCanvas`; declaración estructural, interpretación específica pendiente de ampliar. |

### `RendererFactory`

`web:src/components/WebGLCanvas.tsx:6`

Tipo: `Extract<NonNullable<CanvasProps["gl"]>, (defaults: never) => unknown>`.

### `RendererDefaults`

`web:src/components/WebGLCanvas.tsx:7`

Tipo: `Parameters<RendererFactory>[0]`.

### `RendererBoundary`

`web:src/components/WebGLCanvas.tsx:8`

| Atributo / firma | Propósito |
| --- | --- |
| `state: inferred-by-TypeScript` | Dato de estado/configuración `state`; declaración estructural, interpretación específica pendiente de ampliar. |
| `getDerivedStateFromError(): inferred-by-TypeScript` | Operación get Derived State From Error de Escena 3D y carga de modelos; semántica fina pendiente de ampliación. |
| `componentDidCatch(error: Error): inferred-by-TypeScript` | Operación component Did Catch de Escena 3D y carga de modelos; semántica fina pendiente de ampliación. |
| `render(): inferred-by-TypeScript` | Operación render de Escena 3D y carga de modelos; semántica fina pendiente de ampliación. |

### `DecoderResponseType`

`web:src/utils/dracoLoader.ts:4`

Tipo: `"arraybuffer" \| "text"`.

### `DecoderAsset`

`web:src/utils/dracoLoader.ts:5`

Tipo: `ArrayBuffer \| string`.

### `DracoLoaderInternals`

`web:src/utils/dracoLoader.ts:7`

Tipo: `DRACOLoader & { _loadLibrary: ( fileName: string, responseType: DecoderResponseType, ) => Promise<DecoderAsset>; }`.

### `ResilientGLTFLoader`

`web:src/utils/dracoLoader.ts:114`

| Atributo / firma | Propósito |
| --- | --- |
| `load(url: string, onLoad: (data: GLTF) => void, onProgress?: (event: ProgressEvent) => void, onError?: (error: unknown) => void): void` | Operación load de Escena 3D y carga de modelos; semántica fina pendiente de ampliación. |


## Funciones públicas / componentes / hooks

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-axisarrow"></a>`AxisArrow({   color,   direction,   labelPosition, }: { color: string; direction: [number, number, number]; labelPosition: [number, number, number]; }): inferred-by-TypeScript` | Adapta la llamada a `useMemo` con el contexto de esta integración. | `web:src/components/MpuIrScene3D.tsx:638`; structural |
| <a id="fn-axisguides"></a>`AxisGuides(): inferred-by-TypeScript` | Operación Axis Guides de Escena 3D y carga de modelos; semántica fina pendiente de ampliación. | `web:src/components/MpuIrScene3D.tsx:628`; inferred-name |
| <a id="fn-controls"></a>`Controls(): inferred-by-TypeScript` | Coordina `useEffect`, `useRef`, `useThree` para Escena 3D y carga de modelos. | `web:src/components/ThreeModelViewer.tsx:115`; structural |
| <a id="fn-fallbackmodel"></a>`FallbackModel({ eulerDeg }: { eulerDeg: ModelProps["eulerDeg"] }): inferred-by-TypeScript` | Coordina `useEffect`, `useRef` para Escena 3D y carga de modelos. | `web:src/components/ThreeModelViewer.tsx:96`; structural |
| <a id="fn-fallbackvehicle"></a>`FallbackVehicle(): inferred-by-TypeScript` | Operación Fallback Vehicle de Escena 3D y carga de modelos; semántica fina pendiente de ampliación. | `web:src/components/MpuIrScene3D.tsx:419`; inferred-name |
| <a id="fn-iremulationsurface"></a>`IrEmulationSurface({   sensitivityCm,   onTargetChange, }: { sensitivityCm: number; onTargetChange: (point: IrScenePoint) => void; }): inferred-by-TypeScript` | Coordina `IR_OBJECT_SENSOR_LAYOUT.map`, `Math.max`, `sensorCmToSceneUnits` para Escena 3D y carga de modelos. | `web:src/components/MpuIrScene3D.tsx:510`; structural |
| <a id="fn-irestimatedtargetmarker"></a>`IrEstimatedTargetMarker({   estimate, }: { estimate: IrObjectPositionEstimate; }): inferred-by-TypeScript` | Coordina `Math.max`, `scenePositionForObjectPoint` para Escena 3D y carga de modelos. | `web:src/components/MpuIrScene3D.tsx:585`; structural |
| <a id="fn-irworldtargetmarker"></a>`IrWorldTargetMarker({   worldPosition, }: { worldPosition: IrScenePoint; }): inferred-by-TypeScript` | Operación Ir World Target Marker de Escena 3D y carga de modelos; semántica fina pendiente de ampliación. | `web:src/components/MpuIrScene3D.tsx:554`; inferred-name |
| <a id="fn-model"></a>`Model({ url, eulerDeg }: ModelProps): inferred-by-TypeScript` | Coordina `useEffect`, `useLoader`, `useRef` para Escena 3D y carga de modelos. | `web:src/components/ThreeModelViewer.tsx:52`; structural |
| <a id="fn-modelloadingscreen"></a>`ModelLoadingScreen({   visible,   customLabel,   failed = false, }: { visible: boolean; customLabel?: string; failed?: boolean; }): inferred-by-TypeScript` | Coordina `useMemo`, `useThree` para Escena 3D y carga de modelos. | `web:src/components/ModelLoadingScreen.tsx:6`; structural |
| <a id="fn-mpuirscene3d"></a>`MpuIrScene3D({       modelUrl,       eulerDeg,       irNorm,       objectDetections,       showGrid,       showObjectAxes,       showLineAxes,       beamOpacities,       showOrigin,       sensorSensitivityCm,       irSamplePeriodMs,       renderLink,       irEmu,       irAutoMotion = false,       estimatedTarget,       onEmulatedTarget,       onManualCameraChange,     }: inferred-by-TypeScript, ref: inferred-by-TypeScript): inferred-by-TypeScript` | Coordina `useEffect`, `useImperativeHandle`, `useIrEmitters` para Escena 3D y carga de modelos. | `web:src/components/MpuIrScene3D.tsx:112`; structural |
| <a id="fn-scenecameracontroller"></a>`SceneCameraController({ onManualCameraChange }: inferred-by-TypeScript, ref: inferred-by-TypeScript): inferred-by-TypeScript` | Coordina `useCallback`, `useEffect`, `useImperativeHandle` para Escena 3D y carga de modelos. | `web:src/components/MpuIrScene3D.tsx:297`; structural |
| <a id="fn-scenefloor"></a>`SceneFloor(): inferred-by-TypeScript` | Operación Scene Floor de Escena 3D y carga de modelos; semántica fina pendiente de ampliación. | `web:src/components/MpuIrScene3D.tsx:428`; inferred-name |
| <a id="fn-sensorcones"></a>`SensorCones({   irNorm,   renderLink,   sensitivityCm,   opacity, }: { irNorm: Record<IrSensorKey, number>; renderLink: boolean; sensitivityCm: number; opacity: number; }): inferred-by-TypeScript` | Coordina `IR_EMITTER_GEOMETRY.map`, `Math.tan`, `clamp01` para Escena 3D y carga de modelos. | `web:src/components/MpuIrScene3D.tsx:447`; structural |
| <a id="fn-technicalgrid"></a>`TechnicalGrid(): inferred-by-TypeScript` | Adapta la llamada a `useMemo` con el contexto de esta integración. | `web:src/components/MpuIrScene3D.tsx:435`; structural |
| <a id="fn-threemodelviewer"></a>`ThreeModelViewer({   modelUrl,   eulerDeg,   allowControls = true,   exposure = 1.0,   className = "",   childrenInsideCanvas = null, }: ThreeModelViewerProps): inferred-by-TypeScript` | Adapta la llamada a `[ "relative w-full h-full", "transition-[opacity,transform,box-shadow] duration-300", className, ].join` con el contexto de esta integración. | `web:src/components/ThreeModelViewer.tsx:161`; structural |
| <a id="fn-vehiclemodel"></a>`VehicleModel({   modelUrl,   children, }: { modelUrl: string; children: ReactNode; }): inferred-by-TypeScript` | Coordina `useLoader`, `useMemo` para Escena 3D y carga de modelos. | `web:src/components/MpuIrScene3D.tsx:380`; structural |
| <a id="fn-vehiclesensorrig"></a>`VehicleSensorRig({   eulerDeg,   children, }: { eulerDeg: EulerDeg; children: ReactNode; }): inferred-by-TypeScript` | Coordina `useEffect`, `useRef` para Escena 3D y carga de modelos. | `web:src/components/MpuIrScene3D.tsx:353`; structural |
| <a id="fn-webglcanvas"></a>`WebGLCanvas({   children,   dpr = [1, 1.5],   ...props }: Omit<CanvasProps, "gl">): inferred-by-TypeScript` | Coordina `/GL_(?:VENDOR\|RENDERER)\s*=\s*Disabled\|WebGL.*disabled\|disabled.*WebGL/i.test`, `useCallback`, `useState` para Escena 3D y carga de modelos. | `web:src/components/WebGLCanvas.tsx:15`; structural |
| <a id="fn-clamp01"></a>`clamp01(value: number): inferred-by-TypeScript` | Coordina `Math.max`, `Math.min`, `Number.isFinite` para Escena 3D y carga de modelos. | `web:src/components/MpuIrScene3D.tsx:670`; structural |
| <a id="fn-fetchassetwithretry"></a>`fetchAssetWithRetry(url: string, responseType: DecoderResponseType, init: RequestInit): Promise<DecoderAsset>` | Coordina `fetch`, `response.arrayBuffer`, `response.text` para Escena 3D y carga de modelos. | `web:src/utils/dracoLoader.ts:22`; structural |
| <a id="fn-fetchdecoderasset"></a>`fetchDecoderAsset(url: string, responseType: DecoderResponseType): inferred-by-TypeScript` | Coordina `decoderAssets.get`, `decoderAssets.set`, `fetchAssetWithRetry` para Escena 3D y carga de modelos. | `web:src/utils/dracoLoader.ts:54`; structural |
| <a id="fn-getshareddracoloader"></a>`getSharedDracoLoader(): inferred-by-TypeScript` | Coordina `base.endsWith`, `loader.setDecoderConfig`, `loader.setDecoderPath` para Escena 3D y carga de modelos. | `web:src/utils/dracoLoader.ts:74`; structural |
| <a id="fn-scenepositionforobjectpoint"></a>`scenePositionForObjectPoint(point: IrObjectPoint, yOffset: inferred-by-TypeScript): [number, number, number]` | Adapta la llamada a `irObjectPointToLocalScenePoint` con el contexto de esta integración. | `web:src/components/MpuIrScene3D.tsx:690`; structural |
| <a id="fn-sensorcmtomodelmeters"></a>`sensorCmToModelMeters(valueCm: number): inferred-by-TypeScript` | Coordina `Math.max`, `Math.min`, `Number.isFinite` para Escena 3D y carga de modelos. | `web:src/components/MpuIrScene3D.tsx:682`; structural |
| <a id="fn-sensorcmtosceneunits"></a>`sensorCmToSceneUnits(valueCm: number): inferred-by-TypeScript` | Adapta la llamada a `sensorCmToModelMeters` con el contexto de esta integración. | `web:src/components/MpuIrScene3D.tsx:678`; structural |
| <a id="fn-usemodelloadingstate"></a>`useModelLoadingState(modelUrl: string): inferred-by-TypeScript` | Coordina `loadedModelUrls.has`, `useCallback`, `useState` para Escena 3D y carga de modelos. | `web:src/hooks/useModelLoadingState.ts:5`; structural |
| <a id="fn-wait"></a>`wait(delayMs: number): inferred-by-TypeScript` | Operación wait de Escena 3D y carga de modelos; semántica fina pendiente de ampliación. | `web:src/utils/dracoLoader.ts:18`; inferred-name |

## Funciones y callbacks internos

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-iremulationsurface.updatetarget"></a>`IrEmulationSurface.updateTarget(event: ThreeEvent<PointerEvent>): inferred-by-TypeScript` | Coordina `event.stopPropagation`, `onTargetChange` para Escena 3D y carga de modelos. | `web:src/components/MpuIrScene3D.tsx:525`; structural |
| <a id="fn-modelloadboundary.componentdidcatch"></a>`ModelLoadBoundary.componentDidCatch(error: Error): inferred-by-TypeScript` | Coordina `console.error`, `error.message.replace`, `this.props.modelUrl.startsWith` para Escena 3D y carga de modelos. | `web:src/components/ModelLoadBoundary.tsx:28`; structural |
| <a id="fn-modelloadboundary.getderivedstatefromerror"></a>`ModelLoadBoundary.getDerivedStateFromError(): inferred-by-TypeScript` | Operación get Derived State From Error de Escena 3D y carga de modelos; semántica fina pendiente de ampliación. | `web:src/components/ModelLoadBoundary.tsx:24`; inferred-name |
| <a id="fn-modelloadboundary.getderivedstatefromprops"></a>`ModelLoadBoundary.getDerivedStateFromProps(props: Props, state: State): inferred-by-TypeScript` | Operación get Derived State From Props de Escena 3D y carga de modelos; semántica fina pendiente de ampliación. | `web:src/components/ModelLoadBoundary.tsx:18`; inferred-name |
| <a id="fn-modelloadboundary.render"></a>`ModelLoadBoundary.render(): inferred-by-TypeScript` | Operación render de Escena 3D y carga de modelos; semántica fina pendiente de ampliación. | `web:src/components/ModelLoadBoundary.tsx:37`; inferred-name |
| <a id="fn-mpuirscene3d.clearirtarget"></a>`MpuIrScene3D.clearIrTarget(): inferred-by-TypeScript` | Adapta la llamada a `setWorldTarget` con el contexto de esta integración. | `web:src/components/MpuIrScene3D.tsx:215`; structural |
| <a id="fn-mpuirscene3d.movetarget"></a>`MpuIrScene3D.moveTarget(): inferred-by-TypeScript` | Coordina `Math.cos`, `Math.max`, `Math.sin` para Escena 3D y carga de modelos. | `web:src/components/MpuIrScene3D.tsx:170`; structural |
| <a id="fn-mpuirscene3d.resetcamera"></a>`MpuIrScene3D.resetCamera(): inferred-by-TypeScript` | Adapta la llamada a `cameraControllerRef.current?.resetCamera` con el contexto de esta integración. | `web:src/components/MpuIrScene3D.tsx:212`; structural |
| <a id="fn-mpuirscene3d.sampletarget"></a>`MpuIrScene3D.sampleTarget(): inferred-by-TypeScript` | Adapta la llamada a `onEmulatedTarget` con el contexto de esta integración. | `web:src/components/MpuIrScene3D.tsx:190`; structural |
| <a id="fn-mpuirscene3d.setcamerapreset"></a>`MpuIrScene3D.setCameraPreset(preset: inferred-by-TypeScript): inferred-by-TypeScript` | Adapta la llamada a `cameraControllerRef.current?.setCameraPreset` con el contexto de esta integración. | `web:src/components/MpuIrScene3D.tsx:213`; structural |
| <a id="fn-rendererboundary.componentdidcatch"></a>`RendererBoundary.componentDidCatch(error: Error): inferred-by-TypeScript` | Adapta la llamada a `this.props.onError` con el contexto de esta integración. | `web:src/components/WebGLCanvas.tsx:11`; structural |
| <a id="fn-rendererboundary.getderivedstatefromerror"></a>`RendererBoundary.getDerivedStateFromError(): inferred-by-TypeScript` | Operación get Derived State From Error de Escena 3D y carga de modelos; semántica fina pendiente de ampliación. | `web:src/components/WebGLCanvas.tsx:10`; inferred-name |
| <a id="fn-rendererboundary.render"></a>`RendererBoundary.render(): inferred-by-TypeScript` | Operación render de Escena 3D y carga de modelos; semántica fina pendiente de ampliación. | `web:src/components/WebGLCanvas.tsx:12`; inferred-name |
| <a id="fn-resilientgltfloader.load"></a>`ResilientGLTFLoader.load(url: string, onLoad: (data: GLTF) => void, onProgress?: (event: ProgressEvent) => void, onError?: (error: unknown) => void): void` | Coordina `/^(data\|blob):/.test`, `fetchAssetWithRetry`, `fetchAssetWithRetry(absoluteUrl, "arraybuffer", { headers: this.requestHeader, credentials: this.withCredentials ? "include" : "same-origin", }) .then` para Escena 3D y carga de modelos. | `web:src/utils/dracoLoader.ts:115`; structural |
| <a id="fn-scenecameracontroller.resetcamera"></a>`SceneCameraController.resetCamera(): inferred-by-TypeScript` | Adapta la llamada a `moveTo` con el contexto de esta integración. | `web:src/components/MpuIrScene3D.tsx:323`; structural |
| <a id="fn-webglcanvas.oncreationerror"></a>`WebGLCanvas.onCreationError(event: Event): inferred-by-TypeScript` | Operación on Creation Error de Escena 3D y carga de modelos; semántica fina pendiente de ampliación. | `web:src/components/WebGLCanvas.tsx:27`; inferred-name |
| <a id="fn-getshareddracoloader.loadwasm"></a>`getSharedDracoLoader.loadWasm(): inferred-by-TypeScript` | Coordina `fetchDecoderAsset`, `new URL("draco_decoder.wasm", decoderPath).toString` para Escena 3D y carga de modelos. | `web:src/utils/dracoLoader.ts:86`; structural |

## Estado y efectos React encontrados

| Hook | Función/closure | Evidencia |
| --- | --- | --- |
| `useThree` | `ModelLoadingScreen` | `web:src/components/ModelLoadingScreen.tsx:15` |
| `useMemo` | `ModelLoadingScreen` | `web:src/components/ModelLoadingScreen.tsx:16` |
| `useRef` | `MpuIrScene3D` | `web:src/components/MpuIrScene3D.tsx:134` |
| `useIrEmitters` | `MpuIrScene3D` | `web:src/components/MpuIrScene3D.tsx:135` |
| `useState` | `MpuIrScene3D` | `web:src/components/MpuIrScene3D.tsx:136` |
| `useMemo` | `MpuIrScene3D` | `web:src/components/MpuIrScene3D.tsx:137` |
| `useRef` | `MpuIrScene3D` | `web:src/components/MpuIrScene3D.tsx:152` |
| `useEffect` | `MpuIrScene3D` | `web:src/components/MpuIrScene3D.tsx:157` |
| `useEffect` | `MpuIrScene3D` | `web:src/components/MpuIrScene3D.tsx:161` |
| `useEffect` | `MpuIrScene3D` | `web:src/components/MpuIrScene3D.tsx:167` |
| `useEffect` | `MpuIrScene3D` | `web:src/components/MpuIrScene3D.tsx:185` |
| `useImperativeHandle` | `MpuIrScene3D` | `web:src/components/MpuIrScene3D.tsx:209` |
| `useThree` | `SceneCameraController` | `web:src/components/MpuIrScene3D.tsx:298` |
| `useRef` | `SceneCameraController` | `web:src/components/MpuIrScene3D.tsx:299` |
| `useRef` | `SceneCameraController` | `web:src/components/MpuIrScene3D.tsx:300` |
| `useCallback` | `SceneCameraController` | `web:src/components/MpuIrScene3D.tsx:302` |
| `useImperativeHandle` | `SceneCameraController` | `web:src/components/MpuIrScene3D.tsx:320` |
| `useEffect` | `SceneCameraController` | `web:src/components/MpuIrScene3D.tsx:329` |
| `useRef` | `VehicleSensorRig` | `web:src/components/MpuIrScene3D.tsx:360` |
| `useEffect` | `VehicleSensorRig` | `web:src/components/MpuIrScene3D.tsx:362` |
| `useLoader` | `VehicleModel` | `web:src/components/MpuIrScene3D.tsx:387` |
| `useMemo` | `VehicleModel` | `web:src/components/MpuIrScene3D.tsx:391` |
| `useMemo` | `TechnicalGrid` | `web:src/components/MpuIrScene3D.tsx:436` |
| `useMemo` | `SensorCones` | `web:src/components/MpuIrScene3D.tsx:461` |
| `useMemo` | `AxisArrow` | `web:src/components/MpuIrScene3D.tsx:647` |
| `useLoader` | `Model` | `web:src/components/ThreeModelViewer.tsx:54` |
| `useRef` | `Model` | `web:src/components/ThreeModelViewer.tsx:57` |
| `useEffect` | `Model` | `web:src/components/ThreeModelViewer.tsx:59` |
| `useEffect` | `Model` | `web:src/components/ThreeModelViewer.tsx:81` |
| `useRef` | `FallbackModel` | `web:src/components/ThreeModelViewer.tsx:97` |
| `useEffect` | `FallbackModel` | `web:src/components/ThreeModelViewer.tsx:99` |
| `useThree` | `Controls` | `web:src/components/ThreeModelViewer.tsx:117` |
| `useRef` | `Controls` | `web:src/components/ThreeModelViewer.tsx:118` |
| `useEffect` | `Controls` | `web:src/components/ThreeModelViewer.tsx:120` |
| `useState` | `WebGLCanvas` | `web:src/components/WebGLCanvas.tsx:20` |
| `useState` | `WebGLCanvas` | `web:src/components/WebGLCanvas.tsx:21` |
| `useCallback` | `WebGLCanvas` | `web:src/components/WebGLCanvas.tsx:23` |
| `useState` | `useModelLoadingState` | `web:src/hooks/useModelLoadingState.ts:6` |
| `useCallback` | `useModelLoadingState` | `web:src/hooks/useModelLoadingState.ts:11` |
