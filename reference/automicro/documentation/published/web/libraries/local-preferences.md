# Preferencias locales, tema y utilidades

ID: `web:module/src/utils/theme.ts` · Capa: **servicio** · Fotografía: 2026-10-20.

Documentación de fuente; sin tests, compilación, navegador ni validación física.

## Responsabilidad y alcance

Gestiona preferencias y efectos locales del navegador, independientes de preferencias persistidas F4.

Tema, media query, sonido y bloqueo de scroll de modal.

<a id="modularity"></a>
## Modularidad observada

Utilidades de plataforma navegador; no afectan el protocolo salvo acciones expresas de consumidores.

<a id="state"></a>
## Handle, estado y atributos externos

Storage/DOM/listeners/timers pertenecen a la sesión/página del navegador.

<a id="lifecycle"></a>
## Ciclo de vida y contexto

- Leer preferencia, instalar listeners y retirar/restaurar estado previo al desmontar.

<a id="interactions"></a>
## Interacciones

- Los componentes comparten estas utilidades para mantener comportamiento consistente.

<a id="limitations"></a>
## Precauciones y límites

- Preferencia local no se sincroniza con el firmware por implicación.

## Hallazgos conectados


## Archivos y evidencia

| Archivo | Análisis | Hash SHA-256 |
| --- | --- | --- |
| `src/components/ThemeModeToggleCard.tsx` | typescript-ast-static | `f9d3f39c11d2a25f77688496655985f8731186b91c431025ed7f024f9f6c15e5` |
| `src/hooks/useMediaQuery.ts` | typescript-ast-static | `76d89b91b78056f758e4a8dfaa6a846fd050e34bb73787248cdb23498e618f55` |
| `src/hooks/useModalScrollLock.ts` | typescript-ast-static | `58a04666c4d35938b1c07015a1f3ef017fc22555206a9283e6cd350c6186666f` |
| `src/utils/theme.ts` | typescript-ast-static | `e1d9e9c524b2e9d21523203bab9fee0df7ed493bc13d05b5f9fa5a5d974116a1` |

## Tipos, puertos, props y callbacks

En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.

### `ThemeColors`

`web:src/utils/theme.ts:1`

| Atributo / firma | Propósito |
| --- | --- |
| `base: string` | Dato de estado/configuración `base`; declaración estructural, interpretación específica pendiente de ampliar. |
| `accent: string` | Dato de estado/configuración `accent`; declaración estructural, interpretación específica pendiente de ampliar. |

### `ThemeMode`

`web:src/utils/theme.ts:6`

Tipo: `"dark" \| "light"`.


## Funciones públicas / componentes / hooks

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-appearanceicon"></a>`AppearanceIcon(): inferred-by-TypeScript` | Operación Appearance Icon de Preferencias locales, tema y utilidades; semántica fina pendiente de ampliación. | `web:src/components/ThemeModeToggleCard.tsx:56`; inferred-name |
| <a id="fn-thememodetogglecard"></a>`ThemeModeToggleCard({   className = "", }: { className?: string; }): inferred-by-TypeScript` | Coordina ``settings-identity-card settings-appearance-card ${className}`.trim`, `useState` para Preferencias locales, tema y utilidades. | `web:src/components/ThemeModeToggleCard.tsx:11`; structural |
| <a id="fn-applythemecolors"></a>`applyThemeColors(mode: ThemeMode): inferred-by-TypeScript` | Coordina `getReadableInk`, `hexToRgbCss`, `root.style.setProperty` para Preferencias locales, tema y utilidades. | `web:src/utils/theme.ts:69`; structural |
| <a id="fn-applythememode"></a>`applyThemeMode(mode: ThemeMode): inferred-by-TypeScript` | Adapta la llamada a `applyThemeColors` con el contexto de esta integración. | `web:src/utils/theme.ts:62`; structural |
| <a id="fn-buildthemepayload"></a>`buildThemePayload(colors: ThemeColors): inferred-by-TypeScript` | Adapta la llamada a `hexToRgbTuple` con el contexto de esta integración. | `web:src/utils/theme.ts:96`; structural |
| <a id="fn-getreadableink"></a>`getReadableInk(hex: string): inferred-by-TypeScript` | Adapta la llamada a `hexToRgbTuple` con el contexto de esta integración. | `web:src/utils/theme.ts:102`; structural |
| <a id="fn-hextorgbcss"></a>`hexToRgbCss(hex: string, alpha: inferred-by-TypeScript): inferred-by-TypeScript` | Adapta la llamada a `hexToRgbTuple` con el contexto de esta integración. | `web:src/utils/theme.ts:40`; structural |
| <a id="fn-hextorgbtuple"></a>`hexToRgbTuple(hex: string): [number, number, number]` | Coordina `normalizeHexColor`, `parseInt`, `safe.slice` para Preferencias locales, tema y utilidades. | `web:src/utils/theme.ts:31`; structural |
| <a id="fn-ishexcolor"></a>`isHexColor(value: string): inferred-by-TypeScript` | Coordina `HEX_COLOR_RE.test`, `value.trim` para Preferencias locales, tema y utilidades. | `web:src/utils/theme.ts:27`; structural |
| <a id="fn-loadthememode"></a>`loadThemeMode(): ThemeMode` | Adapta la llamada a `localStorage.getItem` con el contexto de esta integración. | `web:src/utils/theme.ts:45`; structural |
| <a id="fn-normalizehexcolor"></a>`normalizeHexColor(value: string, fallback: inferred-by-TypeScript): inferred-by-TypeScript` | Coordina `HEX_COLOR_RE.test`, `trimmed.startsWith`, `value.trim` para Preferencias locales, tema y utilidades. | `web:src/utils/theme.ts:21`; structural |
| <a id="fn-savethememode"></a>`saveThemeMode(mode: ThemeMode): inferred-by-TypeScript` | Adapta la llamada a `localStorage.setItem` con el contexto de esta integración. | `web:src/utils/theme.ts:54`; structural |
| <a id="fn-usemediaquery"></a>`useMediaQuery(query: string): boolean` | Coordina `useEffect`, `useState` para Preferencias locales, tema y utilidades. | `web:src/hooks/useMediaQuery.ts:3`; structural |
| <a id="fn-usemodalscrolllock"></a>`useModalScrollLock(isOpen: boolean): inferred-by-TypeScript` | Adapta la llamada a `useEffect` con el contexto de esta integración. | `web:src/hooks/useModalScrollLock.ts:7`; structural |

## Funciones y callbacks internos

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-thememodetogglecard.handlethememodetoggle"></a>`ThemeModeToggleCard.handleThemeModeToggle(nextChecked: boolean): inferred-by-TypeScript` | Coordina `applyThemeMode`, `saveThemeMode`, `setThemeMode` para Preferencias locales, tema y utilidades. | `web:src/components/ThemeModeToggleCard.tsx:18`; structural |
| <a id="fn-usemediaquery.listener"></a>`useMediaQuery.listener(): inferred-by-TypeScript` | Adapta la llamada a `setMatches` con el contexto de esta integración. | `web:src/hooks/useMediaQuery.ts:16`; structural |

## Estado y efectos React encontrados

| Hook | Función/closure | Evidencia |
| --- | --- | --- |
| `useState` | `ThemeModeToggleCard` | `web:src/components/ThemeModeToggleCard.tsx:16` |
| `useState` | `useMediaQuery` | `web:src/hooks/useMediaQuery.ts:4` |
| `useEffect` | `useMediaQuery` | `web:src/hooks/useMediaQuery.ts:11` |
| `useEffect` | `useModalScrollLock` | `web:src/hooks/useModalScrollLock.ts:8` |
