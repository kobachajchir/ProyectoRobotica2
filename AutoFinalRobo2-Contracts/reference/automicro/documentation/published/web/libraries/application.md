# Arranque, proveedores y rutas Web

ID: `web:module/src/main.tsx` · Capa: **composición** · Fotografía: 2026-10-20.

Documentación de fuente; sin tests, compilación, navegador ni validación física.

## Responsabilidad y alcance

Monta React, proveedores y rutas que organizan el cliente del robot.

Composición de UI/estado; no ejecuta adquisición F4 ni sirve HTTP desde el navegador.

<a id="modularity"></a>
## Modularidad observada

Composición dependiente de React y router; las librerías de codecs/dibujo pueden ser puras fuera de esa composición.

<a id="state"></a>
## Handle, estado y atributos externos

Providers establecen ámbitos de socket, usuario, modo, sensores y pantalla; montar un provider dos veces crea estados distintos.

<a id="lifecycle"></a>
## Ciclo de vida y contexto

- main monta aplicación; App crea árbol de proveedores/rutas.
- Effects deben liberar listeners, timers y recursos al desmontar o cambiar dependencias.

<a id="interactions"></a>
## Interacciones

- WebSocketContext conecta servicios remotos y las rutas consumen sus contextos.
- Bundle generado se copia/serve desde ESP en otra etapa.

<a id="limitations"></a>
## Precauciones y límites

- Cambiar fuente no actualiza el bundle que ya sirve la placa.
- No se abrió navegador ni se ejecutó build en esta entrega.

## Hallazgos conectados


## Archivos y evidencia

| Archivo | Análisis | Hash SHA-256 |
| --- | --- | --- |
| `src/App.css` | support-inventory | `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855` |
| `src/App.tsx` | typescript-ast-static | `d7970b9dde5c51a919d47f82b59c78291a899472ee4f518314de83b00877e556` |
| `src/index.css` | support-inventory | `65fcc24133b0ece47adf986a049f88fcc89f6d9fd2a9abce454c833b92fc76d7` |
| `src/main.tsx` | typescript-ast-static | `b5fbfaa3ae523d1086f49f7cb3634ed797617669f6e07bde10b962b55043dd72` |
| `src/vite-env.d.ts` | typescript-ast-static | `001ae61186ec1298fb71b62db9fa0567d8ae4100bd6e6f0ebb446b1535726d4c` |

## Tipos, puertos, props y callbacks

En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.

### `ImportMetaEnv`

`web:src/vite-env.d.ts:3`

| Atributo / firma | Propósito |
| --- | --- |
| `VITE_REMOTE_AUTH_BRIDGE_MODE?: "uner" \| "json"` | Dato de estado/configuración `VITE_REMOTE_AUTH_BRIDGE_MODE`; declaración estructural, interpretación específica pendiente de ampliar. |
| `VITE_STM_REMOTE_COMMAND_MODE?: "legacy-hex" \| "json"` | Dato de estado/configuración `VITE_STM_REMOTE_COMMAND_MODE`; declaración estructural, interpretación específica pendiente de ampliar. |

### `ImportMeta`

`web:src/vite-env.d.ts:8`

| Atributo / firma | Propósito |
| --- | --- |
| `env: ImportMetaEnv` | Dato de estado/configuración `env`; declaración estructural, interpretación específica pendiente de ampliar. |


## Funciones públicas / componentes / hooks

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-app"></a>`App(): inferred-by-TypeScript` | Compone proveedores, navegación y rutas de la aplicación. | `web:src/App.tsx:35`; editorial |
| <a id="fn-routeloader"></a>`routeLoader(label: string, children: ReactNode): inferred-by-TypeScript` | Operación route Loader de Arranque, proveedores y rutas Web; semántica fina pendiente de ampliación. | `web:src/App.tsx:31`; inferred-name |

## Funciones y callbacks internos

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |

## Estado y efectos React encontrados

| Hook | Función/closure | Evidencia |
| --- | --- | --- |
| `useEffect` | `App` | `web:src/App.tsx:36` |
| `useMemo` | `App` | `web:src/App.tsx:42` |
