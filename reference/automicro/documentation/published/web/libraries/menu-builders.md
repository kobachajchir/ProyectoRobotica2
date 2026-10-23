# Constructores de menús OLED

ID: `web:module/src/screens/menuScreens.ts` · Capa: **núcleo** · Fotografía: 2026-10-20.

Documentación de fuente; sin tests, compilación, navegador ni validación física.

## Responsabilidad y alcance

Construye primitivas de listas de menú con iconos y selección.

Filtrado visible, tres filas, ventana y catálogos locales; no consulta el árbol dinámico F4 completo.

<a id="modularity"></a>
## Modularidad observada

Cálculo TypeScript sin hardware, dependiente de geometría y datos de menú del robot.

<a id="state"></a>
## Handle, estado y atributos externos

MenuScreenArgs presta items y selección; el resultado es un array de comandos. Las listas constantes pueden divergir del catálogo F4 por modo/perfil.

<a id="lifecycle"></a>
## Ciclo de vida y contexto

- Filtrar visibles, calcular ventana, dibujar fila/bitmap/texto/cursor.
- Conservar distinción índice global visible y posición de click 1-based.

<a id="interactions"></a>
## Interacciones

- Contexto deriva firstVisibleIndex; F4 conserva firstVisibleItem y menú puede tener dos filas de favoritos.
- Remote click ejecuta posición visible en F4, no el índice derivado por Web.

<a id="limitations"></a>
## Precauciones y límites

- Ventana de tres fija no sirve por sí sola para todos los menús.
- Catálogos locales requieren actualización junto con menu_definition y política de visibilidad.

## Hallazgos conectados

- [AMDOC-WEB-004: La ventana del menú reconstruida en Web puede ejecutar otra fila en F4](../../../review/FINDINGS.md#amdoc-web-004)

## Archivos y evidencia

| Archivo | Análisis | Hash SHA-256 |
| --- | --- | --- |
| `src/screens/menuScreens.ts` | typescript-ast-static | `afcc19124be9f7f700e717bb783320eab903dfd2c78d803be914b95440206da5` |

## Tipos, puertos, props y callbacks

En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.

### `MenuScreenArgs`

`web:src/screens/menuScreens.ts:24`

| Atributo / firma | Propósito |
| --- | --- |
| `screenCode: ScreenCode` | Dato de estado/configuración `screenCode`; declaración estructural, interpretación específica pendiente de ampliar. |
| `items: MenuScreenItem[]` | Dato de estado/configuración `items`; declaración estructural, interpretación específica pendiente de ampliar. |
| `selectedIndex?: number` | Dato de estado/configuración `selectedIndex`; declaración estructural, interpretación específica pendiente de ampliar. |
| `firstVisibleIndex?: number` | Dato de estado/configuración `firstVisibleIndex`; declaración estructural, interpretación específica pendiente de ampliar. |


## Funciones públicas / componentes / hooks

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-buildlegacyverticalmenucommands"></a>`buildLegacyVerticalMenuCommands(args: MenuScreenArgs): OledCommand[]` | Coordina `Math.floor`, `Math.max`, `Math.min` para Constructores de menús OLED. | `web:src/screens/menuScreens.ts:236`; structural |
| <a id="fn-buildmenuitemcommands"></a>`buildMenuItemCommands(item: MenuScreenItem, y: number, selected: boolean): OledCommand[]` | Construye marco, icono, etiqueta y cursor de una fila. | `web:src/screens/menuScreens.ts:51`; editorial |
| <a id="fn-buildmenuscreencommands"></a>`buildMenuScreenCommands(args: MenuScreenArgs): OledCommand[]` | Construye filas de una ventana de tres a partir de items y selección. | `web:src/screens/menuScreens.ts:72`; editorial |
| <a id="fn-buildscreen010201mainmenucommands"></a>`buildScreen010201MainMenuCommands(args: { selectedIndex?: number; firstVisibleIndex?: number; sensoresVisible?: boolean }): OledCommand[]` | Coordina `buildMenuScreenCommands`, `mainMenuItems.map` para Constructores de menús OLED. | `web:src/screens/menuScreens.ts:140`; structural |
| <a id="fn-buildscreen020101wifimenucommands"></a>`buildScreen020101WifiMenuCommands(args: { selectedIndex?: number; firstVisibleIndex?: number }): OledCommand[]` | Adapta la llamada a `buildMenuScreenCommands` con el contexto de esta integración. | `web:src/screens/menuScreens.ts:157`; structural |
| <a id="fn-buildscreen020203wifiresultsmenucommands"></a>`buildScreen020203WifiResultsMenuCommands(args: { networkSsids?: string[]; selectedIndex?: number; firstVisibleIndex?: number; }): OledCommand[]` | Coordina `(args.networkSsids?.length ? args.networkSsids : ["Sin redes"]).map`, `buildMenuScreenCommands` para Constructores de menús OLED. | `web:src/screens/menuScreens.ts:201`; structural |
| <a id="fn-buildscreen020203wifiresultssummarycommands"></a>`buildScreen020203WifiResultsSummaryCommands(): OledCommand[]` | Coordina `clear`, `drawXbm`, `textAt` para Constructores de menús OLED. | `web:src/screens/menuScreens.ts:226`; structural |
| <a id="fn-buildscreen020301espmenucommands"></a>`buildScreen020301EspMenuCommands(args: { selectedIndex?: number; firstVisibleIndex?: number }): OledCommand[]` | Adapta la llamada a `buildMenuScreenCommands` con el contexto de esta integración. | `web:src/screens/menuScreens.ts:168`; structural |
| <a id="fn-buildscreen030101sensorsmenucommands"></a>`buildScreen030101SensorsMenuCommands(args: { selectedIndex?: number; firstVisibleIndex?: number }): OledCommand[]` | Adapta la llamada a `buildMenuScreenCommands` con el contexto de esta integración. | `web:src/screens/menuScreens.ts:179`; structural |
| <a id="fn-buildscreen050101settingsmenucommands"></a>`buildScreen050101SettingsMenuCommands(args: { selectedIndex?: number; firstVisibleIndex?: number }): OledCommand[]` | Adapta la llamada a `buildMenuScreenCommands` con el contexto de esta integración. | `web:src/screens/menuScreens.ts:190`; structural |
| <a id="fn-menuiconsize"></a>`menuIconSize(iconRef?: string): { width: number; height: number }` | Operación menu Icon Size de Constructores de menús OLED; semántica fina pendiente de ampliación. | `web:src/screens/menuScreens.ts:45`; inferred-name |
| <a id="fn-visibleitems"></a>`visibleItems(items: MenuScreenItem[]): Array<MenuScreenItem & { sourceIndex: number }>` | Coordina `items .map`, `items .map((item, sourceIndex) => ({ ...item, sourceIndex })) .filter` para Constructores de menús OLED. | `web:src/screens/menuScreens.ts:39`; structural |

## Funciones y callbacks internos

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
