# Simulación y datos de desarrollo

ID: `web:module/src/hooks/useMockFirmware.ts` · Capa: **soporte** · Fotografía: 2026-10-20.

Documentación de fuente; sin tests, compilación, navegador ni validación física.

## Responsabilidad y alcance

Genera o suministra datos de desarrollo y fixtures para inspección del cliente.

Escenarios sintéticos; no son muestras ni resultados del robot real.

<a id="modularity"></a>
## Modularidad observada

Soporte del cliente; debe conservar separación/procedencia frente a telemetría recibida.

<a id="state"></a>
## Handle, estado y atributos externos

Estado local de simulación/temporizadores y fixtures estáticos.

<a id="lifecycle"></a>
## Ciclo de vida y contexto

- Activar explícitamente simulación y limpiar timers al desactivar/desmontar.

<a id="interactions"></a>
## Interacciones

- Consumidores de sensores pueden usar mismos tipos; el tipo compartido no convierte una muestra sintética en física.

<a id="limitations"></a>
## Precauciones y límites

- Esta entrega inventaría fixtures sin ejecutar tests ni generar otros.

## Hallazgos conectados


## Archivos y evidencia

| Archivo | Análisis | Hash SHA-256 |
| --- | --- | --- |
| `src/components/MockEulerGenerator.tsx` | typescript-ast-static | `65b6c37228d0346795f81ee6070082890b9459bd0fd9a32da7c31347b1d6faba` |
| `src/components/MockInjectButton.tsx` | typescript-ast-static | `0860aea879d61548f9b6ecb8f496d9044a531a2d93bd46b3b589994e7bab6ef1` |
| `src/hooks/useMockFirmware.ts` | typescript-ast-static | `0889fcc3695ff019c747df9411a81a2aa631d9c111cde68aedc9b188d0fdc291` |
| `src/protocol/__fixtures__/f4-uner-golden-vectors.json` | support-inventory | `eb4e811a9dfd9fc0330cf7faffb45a6756fee3e434e2122065fee763da802000` |

## Tipos, puertos, props y callbacks

En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.

### `Euler`

`web:src/components/MockEulerGenerator.tsx:3`

| Atributo / firma | Propósito |
| --- | --- |
| `yaw: number` | Dato de estado/configuración `yaw`; declaración estructural, interpretación específica pendiente de ampliar. |
| `pitch: number` | Dato de estado/configuración `pitch`; declaración estructural, interpretación específica pendiente de ampliar. |
| `roll: number` | Dato de estado/configuración `roll`; declaración estructural, interpretación específica pendiente de ampliar. |

### `MockEulerGeneratorProps`

`web:src/components/MockEulerGenerator.tsx:5`

| Atributo / firma | Propósito |
| --- | --- |
| `active: boolean` | Dato de estado/configuración `active`; declaración estructural, interpretación específica pendiente de ampliar. |
| `ms: number` | Dato de estado/configuración `ms`; declaración estructural, interpretación específica pendiente de ampliar. |
| `onMsChange: (ms: number) => void` | Operación on Ms Change de Simulación y datos de desarrollo; semántica fina pendiente de ampliación. |
| `onUpdate: (e: Euler) => void` | Operación on Update de Simulación y datos de desarrollo; semántica fina pendiente de ampliación. |


## Funciones públicas / componentes / hooks

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-mockeulergenerator"></a>`MockEulerGenerator({   active,   ms,   onMsChange,   onUpdate, }: MockEulerGeneratorProps): inferred-by-TypeScript` | Coordina `clamp`, `performance.now`, `useEffect` para Simulación y datos de desarrollo. | `web:src/components/MockEulerGenerator.tsx:12`; structural |
| <a id="fn-mockinjectbutton"></a>`MockInjectButton(): inferred-by-TypeScript` | Adapta la llamada a `useWebSocket` con el contexto de esta integración. | `web:src/components/MockInjectButton.tsx:4`; structural |
| <a id="fn-clamp"></a>`clamp(v: number, a: number, b: number): inferred-by-TypeScript` | Coordina `Math.max`, `Math.min` para Simulación y datos de desarrollo. | `web:src/components/MockEulerGenerator.tsx:125`; structural |
| <a id="fn-usemockfirmware"></a>`useMockFirmware(mode: "AP" \| "STATION", delay: number): inferred-by-TypeScript` | Coordina `useEffect`, `useWebSocket` para Simulación y datos de desarrollo. | `web:src/hooks/useMockFirmware.ts:6`; structural |

## Funciones y callbacks internos

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-mockeulergenerator.tick"></a>`MockEulerGenerator.tick(): inferred-by-TypeScript` | Coordina `Math.sin`, `clamp`, `performance.now` para Simulación y datos de desarrollo. | `web:src/components/MockEulerGenerator.tsx:43`; structural |

## Estado y efectos React encontrados

| Hook | Función/closure | Evidencia |
| --- | --- | --- |
| `useRef` | `MockEulerGenerator` | `web:src/components/MockEulerGenerator.tsx:18` |
| `useRef` | `MockEulerGenerator` | `web:src/components/MockEulerGenerator.tsx:19` |
| `useRef` | `MockEulerGenerator` | `web:src/components/MockEulerGenerator.tsx:20` |
| `useEffect` | `MockEulerGenerator` | `web:src/components/MockEulerGenerator.tsx:22` |
| `useTransition` | `MockEulerGenerator` | `web:src/components/MockEulerGenerator.tsx:26` |
| `useEffect` | `MockEulerGenerator` | `web:src/components/MockEulerGenerator.tsx:28` |
| `useWebSocket` | `MockInjectButton` | `web:src/components/MockInjectButton.tsx:5` |
| `useWebSocket` | `useMockFirmware` | `web:src/hooks/useMockFirmware.ts:10` |
| `useEffect` | `useMockFirmware` | `web:src/hooks/useMockFirmware.ts:12` |
