# Contenido Docs y selector de destino

ID: `web:module/src/features/docs/docsContent.ts` · Capa: **integración** · Fotografía: 2026-10-20.

Documentación de fuente; sin tests, compilación, navegador ni validación física.

## Responsabilidad y alcance

Organiza temas y variantes documentales por destino y presenta navegación de Docs.

Modelo actual F4/Web y selector; la fuente autocontenida AMDOC deberá incorporar ESP con cobertura real.

<a id="modularity"></a>
## Modularidad observada

Consumidor React de contenido; la metodología/IDs y hallazgos viven en Contracts y no deben duplicarse a mano sin procedencia.

<a id="state"></a>
## Handle, estado y atributos externos

Destino/tema seleccionados, query/ruta y contenido son estado de navegación; no son configuración del robot.

<a id="lifecycle"></a>
## Ciclo de vida y contexto

- Resolver destino/tema desde navegación y construir variante.
- Para añadir ESP adaptar unión, contenido y selector coherentemente, conservando URL/volver.

<a id="interactions"></a>
## Interacciones

- Los JSON publicados por módulo permiten integrar contenido y enlaces de hallazgo sin releer firmware.
- Selector de tres opciones/píldora móvil y divisor se registran para porteo posterior.

<a id="limitations"></a>
## Precauciones y límites

- La generación documental de esta entrega no modifica React/CSS de Docs.
- No marcar como documentado un destino solo por mostrar su botón.

## Hallazgos conectados

- [AMDOC-WEB-001: Docs requiere tercer destino ESP y selector visual estable](../../../review/FINDINGS.md#amdoc-web-001)

## Archivos y evidencia

| Archivo | Análisis | Hash SHA-256 |
| --- | --- | --- |
| `src/features/docs/DocsFirmwareSelector.tsx` | typescript-ast-static | `b36aee2ab79ef63032500a331b4053621a5856b8df55f54e4e3618d7c2ecc532` |
| `src/features/docs/docsCatalog.tsx` | typescript-ast-static | `ffb57ae59be57fd21751269f2ee4a7ef2ab262b447e69f193d3ed64410504118` |
| `src/features/docs/docsContent.ts` | typescript-ast-static | `ac9ec651f81d01e58996736fe6b3808f0be6823da03d5f41853138c550c1e1d2` |
| `src/pages/DocTopicSection.tsx` | typescript-ast-static | `40dbc22ae03f084b4f91c3ecb131a71d02ed89307355b565be6b406d55c8d140` |
| `src/pages/DocsSection.tsx` | typescript-ast-static | `6116c87045bfce8b460366ad3d57c7f3ae2c1e7b52a685f46abfdaecd5378267` |

## Tipos, puertos, props y callbacks

En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.

### `DocsFirmwareSelectorProps`

`web:src/features/docs/DocsFirmwareSelector.tsx:8`

| Atributo / firma | Propósito |
| --- | --- |
| `activeTarget: DocFirmwareTarget` | Dato de estado/configuración `activeTarget`; declaración estructural, interpretación específica pendiente de ampliar. |
| `onChange: (target: DocFirmwareTarget) => void` | Operación on Change de Contenido Docs y selector de destino; semántica fina pendiente de ampliación. |

### `DocFirmwareTarget`

`web:src/features/docs/docsContent.ts:1`

Tipo: `"web" \| "f4"`.

### `DocTopicSlug`

`web:src/features/docs/docsContent.ts:3`

Tipo: `\| "motors" \| "oled" \| "mpu" \| "ir" \| "menu-system" \| "render-3d" \| "pcb"`.

### `DocTopicSection`

`web:src/features/docs/docsContent.ts:12`

| Atributo / firma | Propósito |
| --- | --- |
| `title: string` | Dato de estado/configuración `title`; declaración estructural, interpretación específica pendiente de ampliar. |
| `body: string` | Dato de estado/configuración `body`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bullets: string[]` | Dato de estado/configuración `bullets`; declaración estructural, interpretación específica pendiente de ampliar. |

### `DocFlowStep`

`web:src/features/docs/docsContent.ts:18`

| Atributo / firma | Propósito |
| --- | --- |
| `layer: string` | Dato de estado/configuración `layer`; declaración estructural, interpretación específica pendiente de ampliar. |
| `title: string` | Dato de estado/configuración `title`; declaración estructural, interpretación específica pendiente de ampliar. |
| `detail: string` | Dato de estado/configuración `detail`; declaración estructural, interpretación específica pendiente de ampliar. |
| `reference: string` | Dato de estado/configuración `reference`; declaración estructural, interpretación específica pendiente de ampliar. |

### `DocMediaItem`

`web:src/features/docs/docsContent.ts:25`

| Atributo / firma | Propósito |
| --- | --- |
| `kind: "captura" \| "diagrama" \| "foto"` | Dato de estado/configuración `kind`; declaración estructural, interpretación específica pendiente de ampliar. |
| `title: string` | Dato de estado/configuración `title`; declaración estructural, interpretación específica pendiente de ampliar. |
| `caption: string` | Dato de estado/configuración `caption`; declaración estructural, interpretación específica pendiente de ampliar. |
| `status: "disponible" \| "planificada"` | Último resultado y guardas del módulo; los getters pueden devolver una vista de este almacenamiento. |
| `href?: string` | Dato de estado/configuración `href`; declaración estructural, interpretación específica pendiente de ampliar. |
| `imageSrc?: string` | Dato de estado/configuración `imageSrc`; declaración estructural, interpretación específica pendiente de ampliar. |

### `DocReference`

`web:src/features/docs/docsContent.ts:34`

| Atributo / firma | Propósito |
| --- | --- |
| `label: string` | Dato de estado/configuración `label`; declaración estructural, interpretación específica pendiente de ampliar. |
| `path: string` | Dato de estado/configuración `path`; declaración estructural, interpretación específica pendiente de ampliar. |
| `note: string` | Dato de estado/configuración `note`; declaración estructural, interpretación específica pendiente de ampliar. |

### `DocTopicVariant`

`web:src/features/docs/docsContent.ts:40`

| Atributo / firma | Propósito |
| --- | --- |
| `summary: string` | Dato de estado/configuración `summary`; declaración estructural, interpretación específica pendiente de ampliar. |
| `scope: string` | Dato de estado/configuración `scope`; declaración estructural, interpretación específica pendiente de ampliar. |
| `sections: DocTopicSection[]` | Dato de estado/configuración `sections`; declaración estructural, interpretación específica pendiente de ampliar. |
| `flow: DocFlowStep[]` | Dato de estado/configuración `flow`; declaración estructural, interpretación específica pendiente de ampliar. |
| `media: DocMediaItem[]` | Dato de estado/configuración `media`; declaración estructural, interpretación específica pendiente de ampliar. |
| `references: DocReference[]` | Dato de estado/configuración `references`; declaración estructural, interpretación específica pendiente de ampliar. |

### `DocTopic`

`web:src/features/docs/docsContent.ts:49`

| Atributo / firma | Propósito |
| --- | --- |
| `slug: DocTopicSlug` | Dato de estado/configuración `slug`; declaración estructural, interpretación específica pendiente de ampliar. |
| `title: string` | Dato de estado/configuración `title`; declaración estructural, interpretación específica pendiente de ampliar. |
| `kicker: string` | Dato de estado/configuración `kicker`; declaración estructural, interpretación específica pendiente de ampliar. |
| `variants: Record<DocFirmwareTarget, DocTopicVariant>` | Dato de estado/configuración `variants`; declaración estructural, interpretación específica pendiente de ampliar. |

### `DocFirmwareProfile`

`web:src/features/docs/docsContent.ts:56`

| Atributo / firma | Propósito |
| --- | --- |
| `id: DocFirmwareTarget` | Dato de estado/configuración `id`; declaración estructural, interpretación específica pendiente de ampliar. |
| `label: string` | Dato de estado/configuración `label`; declaración estructural, interpretación específica pendiente de ampliar. |
| `eyebrow: string` | Dato de estado/configuración `eyebrow`; declaración estructural, interpretación específica pendiente de ampliar. |
| `title: string` | Dato de estado/configuración `title`; declaración estructural, interpretación específica pendiente de ampliar. |
| `description: string` | Dato de estado/configuración `description`; declaración estructural, interpretación específica pendiente de ampliar. |
| `authority: string` | Dato de estado/configuración `authority`; declaración estructural, interpretación específica pendiente de ampliar. |
| `transport: string` | Dato de estado/configuración `transport`; declaración estructural, interpretación específica pendiente de ampliar. |
| `output: string` | Dato de estado/configuración `output`; declaración estructural, interpretación específica pendiente de ampliar. |


## Funciones públicas / componentes / hooks

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-doctopicsection"></a>`DocTopicSection(): inferred-by-TypeScript` | Coordina `DOC_TOPICS.filter`, `DOC_TOPICS.filter( (entry) => entry.slug !== topic.slug, ).slice`, `docsTargetSearch` para Contenido Docs y selector de destino. | `web:src/pages/DocTopicSection.tsx:16`; structural |
| <a id="fn-docsfirmwareselector"></a>`DocsFirmwareSelector({   activeTarget,   onChange, }: DocsFirmwareSelectorProps): inferred-by-TypeScript` | Adapta la llamada a `TARGETS.map` con el contexto de esta integración. | `web:src/features/docs/DocsFirmwareSelector.tsx:13`; structural |
| <a id="fn-docssection"></a>`DocsSection(): inferred-by-TypeScript` | Coordina `DOC_TOPICS.map`, `getDocFirmwareTarget`, `searchParams.get` para Contenido Docs y selector de destino. | `web:src/pages/DocsSection.tsx:13`; structural |
| <a id="fn-mediacard"></a>`MediaCard({  item,  onOpen, }: { item: DocMediaItem; onOpen: (href: string) => void; }): inferred-by-TypeScript` | Adapta la llamada a `mediaKindLabel` con el contexto de esta integración. | `web:src/pages/DocTopicSection.tsx:240`; structural |
| <a id="fn-profilefact"></a>`ProfileFact({ label, value }: { label: string; value: string }): inferred-by-TypeScript` | Operación Profile Fact de Contenido Docs y selector de destino; semántica fina pendiente de ampliación. | `web:src/pages/DocsSection.tsx:104`; inferred-name |
| <a id="fn-docstargetsearch"></a>`docsTargetSearch(target: DocFirmwareTarget): string` | Operación docs Target Search de Contenido Docs y selector de destino; semántica fina pendiente de ampliación. | `web:src/features/docs/docsContent.ts:570`; inferred-name |
| <a id="fn-docstopichref"></a>`docsTopicHref(slug: DocTopicSlug, target: DocFirmwareTarget): string` | Adapta la llamada a `docsTargetSearch` con el contexto de esta integración. | `web:src/features/docs/docsContent.ts:574`; structural |
| <a id="fn-f4visual"></a>`f4Visual(diagramTitle: string, photoTitle: string, evidenceTitle: string): DocMediaItem[]` | Operación f4 Visual de Contenido Docs y selector de destino; semántica fina pendiente de ampliación. | `web:src/features/docs/docsContent.ts:125`; inferred-name |
| <a id="fn-getdocfirmwaretarget"></a>`getDocFirmwareTarget(value: string \| null \| undefined): DocFirmwareTarget` | Operación get Doc Firmware Target de Contenido Docs y selector de destino; semántica fina pendiente de ampliación. | `web:src/features/docs/docsContent.ts:566`; inferred-name |
| <a id="fn-getdoctopic"></a>`getDocTopic(slug?: string): inferred-by-TypeScript` | Adapta la llamada a `DOC_TOPICS.find` con el contexto de esta integración. | `web:src/features/docs/docsContent.ts:562`; structural |
| <a id="fn-mediakindlabel"></a>`mediaKindLabel(kind: DocMediaItem["kind"]): inferred-by-TypeScript` | Operación media Kind Label de Contenido Docs y selector de destino; semántica fina pendiente de ampliación. | `web:src/pages/DocTopicSection.tsx:286`; inferred-name |
| <a id="fn-renderdoctopicicon"></a>`renderDocTopicIcon(slug: DocTopicSlug, className: inferred-by-TypeScript): ReactNode` | Operación render Doc Topic Icon de Contenido Docs y selector de destino; semántica fina pendiente de ampliación. | `web:src/features/docs/docsCatalog.tsx:14`; inferred-name |
| <a id="fn-segmentedbuttonstyle"></a>`segmentedButtonStyle(active: boolean): inferred-by-TypeScript` | Operación segmented Button Style de Contenido Docs y selector de destino; semántica fina pendiente de ampliación. | `web:src/features/docs/DocsFirmwareSelector.tsx:44`; inferred-name |
| <a id="fn-webvisual"></a>`webVisual(liveTitle: string, href: string, diagramTitle: string, photoTitle: string): DocMediaItem[]` | Operación web Visual de Contenido Docs y selector de destino; semántica fina pendiente de ampliación. | `web:src/features/docs/docsContent.ts:95`; inferred-name |

## Funciones y callbacks internos

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-doctopicsection.handletargetchange"></a>`DocTopicSection.handleTargetChange(nextTarget: DocFirmwareTarget): inferred-by-TypeScript` | Coordina `nextSearchParams.set`, `setSearchParams` para Contenido Docs y selector de destino. | `web:src/pages/DocTopicSection.tsx:33`; structural |
| <a id="fn-docssection.handletargetchange"></a>`DocsSection.handleTargetChange(nextTarget: DocFirmwareTarget): inferred-by-TypeScript` | Coordina `nextSearchParams.set`, `setSearchParams` para Contenido Docs y selector de destino. | `web:src/pages/DocsSection.tsx:19`; structural |

## Estado y efectos React encontrados

| Hook | Función/closure | Evidencia |
| --- | --- | --- |
| `useNavigate` | `DocTopicSection` | `web:src/pages/DocTopicSection.tsx:17` |
| `useParams` | `DocTopicSection` | `web:src/pages/DocTopicSection.tsx:18` |
| `useSearchParams` | `DocTopicSection` | `web:src/pages/DocTopicSection.tsx:19` |
| `useNavigate` | `DocsSection` | `web:src/pages/DocsSection.tsx:14` |
| `useSearchParams` | `DocsSection` | `web:src/pages/DocsSection.tsx:15` |
