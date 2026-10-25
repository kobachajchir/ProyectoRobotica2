# Informe de transferencia: glassmorphism, temas e interacción

Fecha: 23 de octubre de 2026. Versión: 0.1.0. Estado: `draft` para la aplicación de destino.

Documento preparado mediante lectura estática de ST2026-Radal. Es un insumo autocontenido para crear el manual de otra aplicación con `visual-identity-system`, la skill disponible para identidad y consistencia visual. No reemplaza el manual aprobado de ST2026.

**Dirección confirmada por el usuario:** trasladar el lenguaje de glassmorphism, animaciones y temas claro/oscuro; usar vidrio también en los paneles de la nueva aplicación; excluir los estilos planos y todos los colores de marca. Se conservan los colores que cumplen una función de tema, lectura o estado.

No se ejecutaron tests, builds, escáneres, mediciones de contraste, renderizados ni validaciones en navegador. Las descripciones de comportamiento corresponden al código leído. Los valores nuevos son propuestas para el destino.

## 1. Cómo interpretar el informe

Se utilizan cuatro etiquetas de evidencia:

| Etiqueta | Significado en este documento |
| --- | --- |
| **Confirmada** | Instrucción expresa del usuario para la transferencia. |
| **Observada** | Declaración o comportamiento presente en los archivos inspeccionados. No implica comprobación en ejecución. |
| **Inferida** | Conclusión obtenida al relacionar las piezas del sistema. |
| **Propuesta** | Regla o valor nuevo para completar una identidad coherente en otra app. |

La aprobación del manual de origen no aprueba automáticamente el manual de destino. La cobertura en código y la aprobación visual se registran por separado. Todo el comportamiento descrito aquí permanece **No verificado en ejecución en esta tarea**.

**Prioridad para resolver discrepancias:** instrucciones actuales del usuario → reglas confirmadas de transferencia → valores observados de vidrio/tema/movimiento → propuestas explícitas. La separación original entre superficies editoriales de vidrio y superficies operativas opacas queda fuera de la transferencia: el usuario eligió glassmorphism como material principal del destino.

## 2. La identidad material que se quiere conservar

El aspecto característico es un **vidrio esmerilado o acrílico**, con tinte de tema, desenfoque del contenido de fondo, borde fino, radios amplios y sombra difusa. El contenido se mantiene nítido sobre ese material. La sensación de profundidad surge de superponer capas y revelar parte del entorno.

La fórmula tiene seis partes:

1. **Entorno:** fondo con variación suave de luminosidad, un degradado o una imagen elegida por la nueva aplicación.
2. **Tinte:** blanco translúcido en claro; azul grisáceo oscuro translúcido en oscuro.
3. **Desenfoque de fondo:** `backdrop-filter: blur(...)`, que actúa sobre lo que queda detrás del panel.
4. **Contorno:** borde de 1 px y, en las recetas más elaboradas, un reflejo interior superior.
5. **Profundidad:** sombra exterior con más extensión que dureza.
6. **Contenido:** texto, iconos y controles con su opacidad normal; estados y foco perceptibles.

**Regla de implementación propuesta:** la transparencia pertenece al fondo o a una capa material independiente. No aplicar `opacity: .88` al contenedor completo para imitar el vidrio: eso también atenúa texto, iconos y controles. `opacity` del conjunto sí se usa transitoriamente para entrar o salir.

El `blur` del fondo y el `filter: blur(...)` del contenido son recursos distintos. Para los paneles se conserva el primero. No se desenfocan formularios o textos para decorar su estado estable.

### 2.1. Variaciones de material observadas

| Familia observada | Construcción | Fuente |
| --- | --- | --- |
| Tarjeta compartida `Card` | `surface-glass`, borde semántico, radio de tarjeta, `backdrop-blur-xl`, sombra de tarjeta. | S03 |
| Panel principal de bienvenida pública | `surface-glass`, blur de 20 px, composición amplia con padding fluido. | S07 |
| Controles flotantes expandibles | `surface-glass`, borde fuerte, blur de 16 px, forma pill o circular. | S10 |
| Contenedor público del usuario | Fondo construido con 10% de `currentColor`, borde con 28%, blur de 14 px. Hover/foco: 17% de fondo y 42% de borde. | S01 |
| Buscador público | Vidrio en degradado de tres opacidades, blur de 24 px, saturación de 150%, reflejo interior. | S09 |
| Diálogo de checkout | `surface-glass`, blur de 28 px y saturación de 155%, radio de panel y sombra amplia. | S01 |
| Toasts web | Fondos translúcidos al 80%, `backdrop-blur-xl`, radio de 16 px y variaciones semánticas de estado. | S13 |
| Overlay local de trabajo | Tinte glass, blur localizado y spinner; mantiene visible el contexto afectado. | S14, S15 |

En Tailwind estándar, `backdrop-blur-md` representa 12 px y `backdrop-blur-xl` 24 px; la configuración inspeccionada no redefine esa escala. Para trasladar el diseño conviene escribir esos valores en tokens, sin depender de nombres de utilidades de otra versión.

**Matiz importante:** `.brand-panel` aporta fondo, borde, radio y sombra, pero no añade blur por sí misma. El desenfoque depende de la receta que la acompaña. El nombre de una clase no basta para deducir el material final.

### 2.2. Jerarquía de vidrio propuesta para la nueva app

Todo panel normal del destino utiliza vidrio. La densidad cambia según su función:

| Material propuesto | Uso | Opacidad claro / oscuro | Blur | Saturación |
| --- | --- | --- | --- | --- |
| `glass-chrome` | Header, barra flotante, controles de navegación. | 72% / 76% | 16 px | 140% |
| `glass-panel` | Cards, secciones, formularios y panel principal. | 88% / 90% | 20 px | 145% |
| `glass-dense` | Tablas, menús largos, formularios extensos y lectura prolongada. | 94% / 96% | 20 px | 140% |
| `glass-field` | Campo interior, búsqueda y selector cerrado. | 56% / 62% | 12 px | 120% |
| `glass-popover` | Dropdown, notificaciones y menú contextual. | 94% / 96% | 20 px | 140% |
| `glass-dialog` | Modal principal y superficies temporales amplias. | 88% / 90% | 24 px | 150% |

Los RGB base son blanco para claro y `20, 35, 59` para oscuro. Las opacidades 88% y 90% provienen de `surface-glass`; el resto de la escala y su asignación a todos los paneles son **Propuesta**. `glass-dense` conserva transparencia y blur; da mayor estabilidad de lectura dentro del mismo lenguaje.

Evitar capas de vidrio repetidas sin necesidad. Una tabla dentro de un panel puede compartir el material de su contenedor y separar filas con bordes; no necesita un blur independiente por celda. Esa regla preserva la continuidad del vidrio y limita el trabajo de composición.

## 3. Colores: qué se traslada y qué se elimina

### 3.1. Paleta base de temas observada

Estos valores cumplen funciones de tema en `src/shared/styles.css`, independientemente de los aliases comerciales. Los azules grisáceos oscuros forman parte del tema oscuro que se quiere conservar.

| Rol | Claro | Oscuro | Uso en la transferencia |
| --- | --- | --- | --- |
| Fondo de página | `#f6f8fc` | `#0b1525` | Base bajo los materiales y degradados. |
| Referencia de superficie elevada | `#ffffff` | `#14233b` | RGB de referencia para crear vidrio; no se exporta como receta de panel plano. |
| Referencia de superficie sutil | `#eef3f8` | `#1c2f4f` | Separación tonal, campos y estados interiores. |
| Texto principal | `#132033` | `#ecf2fb` | Títulos, datos y contenido importante. |
| Texto secundario | `#4f6178` | `#b6c4d9` | Ayuda, metadatos y descripciones. |
| Texto sobre contexto oscuro | `#ffffff` | `#ffffff` | Cuando la composición concreta lo requiere. |
| Vidrio base | `rgba(255,255,255,.88)` | `rgba(20,35,59,.90)` | Material principal heredado. |
| Scrim de modal | `rgba(3,12,27,.68)` | `rgba(0,5,14,.78)` | Separación y atenuación del fondo. |
| Borde oscuro sutil | — | `rgba(255,255,255,.10)` | Contorno en oscuro. |
| Borde oscuro fuerte | — | `rgba(255,255,255,.20)` | Separación reforzada en oscuro. |

**Observada:** en origen, parte de los bordes, sombras, acciones y foco del tema claro depende de primitivas de marca. Esas dependencias se sustituyen en el destino; no se copian como si fueran colores neutrales.

### 3.2. Neutralización propuesta de acciones, foco y profundidad

| Token propuesto | Claro | Oscuro | Motivo |
| --- | --- | --- | --- |
| `border-subtle` | `rgba(19,32,51,.14)` | `rgba(255,255,255,.10)` | Conserva proporciones; elimina la dependencia de marca en claro. |
| `border-strong` | `rgba(19,32,51,.28)` | `rgba(255,255,255,.20)` | Mayor definición del vidrio. |
| `action-primary` | `#132033` | `#ecf2fb` | Acción neutral basada en tinta del tema. |
| `action-primary-hover` | `#26364b` | `#dce4ee` | Diferencia tonal sin introducir un acento comercial. |
| `action-on-primary` | `#ffffff` | `#0b1525` | Tinta sobre la acción neutral. |
| `focus-ring` | `#132033` | `#ecf2fb` | Foco neutral; se combina con una segunda línea de separación. |
| `selection-background` | `rgba(19,32,51,.08)` | `rgba(236,242,251,.10)` | Selección de filas y opciones. |
| `glass-highlight` | `rgba(255,255,255,.70)` | `rgba(255,255,255,.08)` | Reflejo superior contenido. |

Estos valores se entregan como **Propuesta**, sin afirmar que su contraste haya sido medido sobre fondos reales. El foco sobre imágenes puede necesitar un anillo compuesto claro/oscuro. No reducir el foco a un cambio de sombra apenas perceptible.

### 3.3. Colores de estado

Se pueden mantener colores semánticos de éxito, advertencia y error: señalan una condición del sistema. No identifican a una marca.

| Estado | Tinta clara / fondo suave claro | Tinta oscura / fondo suave oscuro | Evidencia |
| --- | --- | --- | --- |
| Éxito | `#0f8c53` / `#e9f8f0` | `#4fd18f` / `#123c2b` | Observada en tokens de tema. |
| Advertencia | `#9a640e` / `#fff7df` | `#ffce71` / `#493715` | Observada en tokens de tema. |
| Error | `#c43c3c` / `#fff0f0` | `#ff8a8a` / `#4a2027` | Observada en tokens de tema. |
| Información | Tinta y fondo suave del tema neutral. | Tinta y fondo suave del tema neutral. | Propuesta para evitar aliases de información que coinciden con el color de una marca. |

Para badges o alertas glass, mezclar el tono suave con transparencia sobre el material base; mantener texto, icono y mensaje explícitos. Una tinta de estado no debe aplicarse a todo el fondo de pantalla.

Los pares de estado son referencias de tema, no combinaciones universalmente aprobadas. En particular, no copiar automáticamente texto blanco sobre cualquier color de error en ambos temas. El manual de destino debe resolver cada par de tinta/fondo.

### 3.4. Exclusiones explícitas

No exportar `--brand-*`, `--public-accent`, `--public-ink`, `--public-soft`, `--public-background`, fotografías comerciales, logos, marcas de agua, barras cromáticas de marca ni fondos de redes sociales. Tampoco copiar el foco original si depende de un acento comercial.

La barra decorativa superior de 5 px que existe en paneles de origen es una señal de marca y no forma parte obligatoria del destino. **Propuesta:** omitirla y conservar la jerarquía con título, espaciado y borde; si se necesita un separador, utilizar el tema neutral.

## 4. Temas claro y oscuro

### 4.1. Apariencia

En **claro**, el vidrio es blanco lechoso, con sombras ligeras, borde de tinta neutral de baja opacidad y texto oscuro. El fondo puede tener una variación suave de luminosidad que se percibe a través de las zonas más transparentes.

En **oscuro**, el vidrio adquiere el RGB de la superficie oscura. El texto se aclara, el borde pasa a blanco translúcido y las sombras se oscurecen. El cambio de tema transforma toda la relación de capas; los radios, composición, tamaños y tiempos se conservan.

**Propuesta para el destino:** tema y material son ejes separados. `light/dark` cambia tinta y RGB; `chrome/panel/dense/field/popover/dialog` cambia opacidad, blur y profundidad. Un componente no debería contener dos árboles visuales distintos para resolver el tema.

### 4.2. Selección y persistencia observadas

- `ThemeProvider` admite `light`, `dark` y `system`.
- Guarda la preferencia en `localStorage` con una clave propia del producto de origen.
- Resuelve `system` con `matchMedia('(prefers-color-scheme: dark)')`.
- Alterna la clase `dark` en `document.documentElement`.
- CSS define `color-scheme: light` en raíz y `color-scheme: dark` en `.dark`.
- `PublicThemeToggle` muestra el icono y la etiqueta de la acción siguiente: pasar a claro o pasar a oscuro.

**Límite observado:** el efecto del provider depende de la preferencia guardada; en el fragmento leído no se suscribe a cambios del sistema. El toggle público sí escucha esa media query para su representación. El destino debe centralizar la suscripción y la resolución del tema en el provider (**Propuesta**).

Usar una clave de persistencia propia de la nueva app. El informe no prescribe una transición global de colores al cambiar de tema: no se encontró un contrato único de ese tipo. **Propuesta:** aplicar el tema de inmediato y reservar el movimiento para los componentes que ya lo justifican.

### 4.3. Fondo sin marca

**Propuesta:** base del tema más uno o dos degradados radiales de baja intensidad construidos desde sus propias tintas. Su función es hacer perceptible el vidrio y organizar la profundidad. Evitar halos de colores comerciales o movimiento continuo del fondo.

El fondo de página puede ser sólido bajo esos degradados; la decisión confirmada de usar vidrio se aplica a paneles, controles y capas de la interfaz. Una base de composición no es una variante plana de panel.

## 5. Geometría, sombras, tipo y espaciado

### 5.1. Radios observados

| Token | Valor | Asignación propuesta |
| --- | --- | --- |
| `radius-control` | `0.875rem` | Campos, botones y opciones interiores. |
| `radius-card` | `1.5rem` | Cards, secciones y grupos de contenido. |
| `radius-panel` | `2rem` | Modales y paneles principales. |
| `radius-pill` | `999px` | Barras compactas, chips, controles circulares y navegación flotante. |

A 16 px por rem, los tres primeros valores representan 14, 24 y 32 px. La forma crece con la escala del contenedor. Hover, foco, activo y abierto conservan radio, padding y tamaño; los estados se expresan con tinta, borde, reflejo y pequeñas transformaciones. Las expansiones del buscador o un acordeón son transiciones funcionales específicas, no cambios de geometría por hover.

### 5.2. Profundidad propuesta, con sombras neutrales

Se conservan las dimensiones y opacidades de las familias de sombra y se neutraliza su tinte:

| Nivel | Claro | Oscuro |
| --- | --- | --- |
| Control | `0 4px 14px rgba(0,0,0,.08)` | `0 4px 16px rgba(0,0,0,.24)` |
| Tarjeta | `0 14px 36px rgba(0,0,0,.08)` | `0 18px 44px rgba(0,0,0,.28)` |
| Panel | `0 30px 80px rgba(0,0,0,.20)` | `0 34px 90px rgba(0,0,0,.42)` |

El reflejo interior `inset 0 1px 0 var(--glass-highlight)` puede acompañar la sombra exterior. En oscuro debe mantenerse discreto para que no parezca una línea blanca gruesa. `shadow-popover` aparece consumido en CSS, pero no se encontró su definición en las fuentes `src` inspeccionadas; la propuesta de destino lo define expresamente como alias de la sombra de panel.

### 5.3. Tipografía

La UI operativa observada usa `"Segoe UI Variable", "Segoe UI", Tahoma, sans-serif`, cuerpo con interlineado 1.5, controles de 14 px y títulos de modal de 20 px, peso 700 y tracking `-.02em`. Hay otras pilas editoriales en origen; no son necesarias para trasladar el vidrio.

**Propuesta:** adoptar una sola pila de interfaz en el destino, o su equivalente multiplataforma. Conservar jerarquía y ritmo aunque cambie la fuente disponible. Títulos 20–24 px, cuerpo 14–16 px, ayuda 12–14 px; usar números tabulares donde haya importes o datos alineados. No importar logotipos tipográficos ni tratamientos de titulares comerciales.

### 5.4. Espaciado y dimensiones

- **Observada:** modal base con padding de 24 px; header con separación inferior de 24 px; footer con gap de 12 px.
- **Observada:** inputs/selects con altura mínima de 44 px y controles de búsqueda de 44 × 44 px. El botón base admite mínimo de 40 px.
- **Observada:** paneles amplios con padding fluido mediante `clamp(...)`, y componentes compactos con utilidades Tailwind.
- **Propuesta:** escala del destino de 4, 8, 12, 16, 24, 32 y 48 px; padding de panel de `clamp(16px, 3vw, 32px)`; área táctil preferida de 44 px.

La escala propuesta organiza lo observado; no se presenta como una escala global ya existente en el código.

## 6. Catálogo de componentes para una app completamente glass

Las recetas de esta sección son **Propuesta**, salvo los comportamientos identificados como observados. El material glass en todos los paneles deriva de la instrucción **Confirmada** del usuario.

### 6.1. Paneles y tarjetas

Anatomía: capa de vidrio → cabecera con título y contexto → contenido agrupado → acciones → feedback local. Usar `glass-panel` y radio de tarjeta; paneles principales con radio de panel. Para datos densos, subir a `glass-dense` conservando blur y transparencia.

Las tarjetas estáticas no se elevan al pasar el puntero. En tarjetas accionables, puede existir un desplazamiento de hasta 2 px y un borde más definido, siempre con hover fino y foco equivalente. El hover no mueve tablas ni datos que el usuario está leyendo.

### 6.2. Campos y selectores

Anatomía: label persistente → campo `glass-field` → ayuda o error. El foco fortalece borde y anillo; no modifica padding. Error y deshabilitado mantienen el mismo tamaño.

El componente `Select` de origen es un `<select>` nativo. Estiliza el control cerrado; no implementa una lista desplegable personalizada ni una animación de apertura. El popup de opciones depende del navegador o del sistema operativo. **No atribuirle una animación glass propia.**

Si el destino requiere controlar material y animación de todas las opciones, usar una primitiva accesible de select/listbox de su stack y aplicarle `glass-popover`. Documentar selección, foco, búsqueda por escritura, flechas, Enter, Escape y opción deshabilitada. Esa lista personalizada es una extensión propuesta.

### 6.3. Dropdown y menú contextual

**Observado en el menú de perfil:** ancho de 256 px, posición bajo el trigger alineada a la derecha, separación vertical de 8 px, radio de 16 px, borde, sombra y grupos separados. El componente actual se monta condicionalmente y no contiene una animación propia de entrada/salida. Su fondo de origen es elevado opaco; solo se toma como evidencia de anatomía e interacción, excluida su receta de material.

**Receta glass de destino:** `glass-popover`, blur de 20 px, borde sutil y sombra de panel. El panel entra desde `opacity: 0; translateY(-4px) scale(.97)` hasta estado estable en 200 ms con `ease-out`. Sale por el mismo recorrido en 150 ms. El origen de transformación coincide con el punto de anclaje: superior derecho para un menú bajo un trigger a la derecha, y se actualiza si cambia la colocación.

**Excepción confirmada por las convenciones de movimiento consultadas:** apertura/cierre por teclado o uso de muy alta frecuencia, inmediatos. No introducir un scrim que bloquee toda la app para un menú contextual corriente. Su cierre debe contemplar selección, Escape y clic fuera; las opciones deben usar semántica coherente de menú o navegación, con administración de foco de la primitiva elegida.

### 6.4. Tooltip

El CSS de origen declara fade y pequeña escala en 150 ms y un delay de hover de 280 ms. Sin embargo, el `Tooltip` React actual monta el contenido en un portal, calcula posición dentro del viewport y fija `transform: none` mediante estilo inline. Por eso el recorrido del CSS no equivale a una animación garantizada del componente actual.

**Propuesta:** vidrio denso, contenido breve, radio de 12 px, opacidad y escala `.97 → 1` en 150 ms si existe transición montada; delay de 280 ms solo para hover. Foco por teclado muestra la ayuda de inmediato. Escape la cierra. Mantener separación de 10 px respecto al ancla y margen mínimo de viewport de 8 px, como la lógica observada. No alojar acciones esenciales dentro de un tooltip.

### 6.5. Modal glass

Anatomía: scrim independiente → panel glass centrado → título/descripción → cuerpo → acciones → cierre. El modal base observado monta en portal, bloquea scroll de body, establece foco inicial, cicla Tab/Shift+Tab, contempla Escape y devuelve el foco al cerrar. `busy` y `dismissible` gobiernan los cierres permitidos.

**Movimiento observado:** scrim `opacity 0 → 1` en 250 ms; panel `opacity 0 → 1` y `translateY(.75rem) scale(.96) → translateY(0) scale(1)` en 250 ms con `ease-enter`. `.75rem` son 12 px con raíz de 16 px. Al cerrar invierte el recorrido y conserva el montaje 250 ms antes de retirarlo.

**Material de destino propuesto:** `glass-dialog`; conservar el contrato de foco y ciclo de vida. El CSS `.ds-modal` de origen declara superficie elevada opaca; ese fondo no se transfiere. El checkout aporta la referencia observada de un diálogo que sí usa vidrio.

Altura máxima observada: `min(90svh, 900px)` para modal base. Usar scroll interior cuando sea necesario. El destino no debe retirar el panel del DOM antes de completar la salida elegida. Si se cambia la duración, el mecanismo de desmontaje debe acompañarla.

### 6.6. Drawer y navegación lateral

**Observada:** la barra lateral móvil entra con `translateX(-100%) → 0`, 240 ms, `ease-drawer`. El scrim aparece con fade de 240 ms, blur de 8 px y tinte tenue. El ancho móvil es `min(75vw, 360px)` y en tablet pasa a `min(50vw, 360px)`. La navegación queda por encima del scrim.

**Propuesta:** trasladar ese recorrido y dar material `glass-dense` a la barra lateral. Mantener foco contenido mientras actúa como diálogo móvil, impedir la interacción con el fondo y restaurar foco al cerrar. La versión de escritorio puede conservar el mismo vidrio sin scrim. El colapso de ancho de escritorio observado anima propiedades de layout; no se convierte en requisito de la identidad.

### 6.7. Menú responsive de acciones

**Observada en público:** por debajo de 900 px, panel bajo el header con `translateY(-12px) scale(.98)` y opacidad inicial 0, origen superior derecho. Entrada de 200 ms y salida de 160 ms con `ease-out`; `visibility` y `pointer-events` impiden interacción cuando está cerrado. Por teclado, el componente admite modo instantáneo. Con movimiento reducido, queda un fade previsto de 120 ms.

**Propuesta:** conservar interacción y recorrido usando `glass-popover` en lugar del fondo dependiente de marca. Respetar altura visible y permitir scroll interior; no cortar las opciones cuando aparece el teclado móvil.

### 6.8. Tabs y controles segmentados

**Observada:** el indicador del segmentado se desplaza mediante `transform` en 250 ms con `ease-standard`. Las opciones transicionan color en 160 ms y presión en 140 ms; activo con escala `.97`. El teclado utiliza flechas, Home/End, foco itinerante y desactiva transiciones mediante `data-input="keyboard"`.

**Propuesta:** pista `glass-field`; indicador neutral translúcido con borde; texto en tinta del tema. Una selección persistente no debe depender únicamente del color. Conservar el movimiento del indicador y la respuesta inmediata de teclado. No usar un spring con rebote para un selector de trabajo.

### 6.9. Acordeones, tablas y listas

**Observada:** `CollapsiblePanel` anima `grid-template-rows` entre `0fr` y `1fr`, opacidad y chevron de 180 grados, en 200 ms. Usa `aria-expanded` y `aria-controls`, y elimina la transición con movimiento reducido. Su superficie plana de origen queda excluida.

**Propuesta:** contenedor `glass-dense`, grupos y filas sin desenfoque independiente. La expansión de altura es una excepción funcional que debe documentarse: no pertenece al grupo de transformaciones baratas. El contenido cerrado no debe seguir siendo alcanzable por teclado; `aria-hidden` por sí solo no garantiza eso. Las filas estables no necesitan entradas escalonadas ni desplazamientos por hover.

### 6.10. Botones e iconos

**Observada:** el botón base usa 150 ms, escala de presión `.97`, transiciones explícitas de color/fondo/borde/sombra/transform/opacidad, radio de control y estados deshabilitados. La utilidad `ease-out` del botón no referencia expresamente la curva custom del sistema.

**Propuesta:** botón glass con borde y variación de luminosidad; acción principal neutral de mayor densidad dentro del mismo material. Presión `.97` en 150 ms con `var(--ease-out)`; quitar esa transformación por teclado y movimiento reducido. Los iconos mantienen tamaño y área táctil. No copiar el brillo o tinte de ninguna marca.

**Observada como microinteracción complementaria:** favoritos usa transición de opacidad del relleno de 140 ms; por teclado es inmediata. Es un buen ejemplo de feedback pequeño que no necesita mover el icono. El fade local de movimiento reducido declara 120 ms, sujeto a la cascada global descrita más adelante.

## 7. Buscador expandible: la receta más completa de vidrio e interacción

El buscador público es especialmente útil como referencia porque coordina trigger, material, contenido, overlay, estados y teclado.

### 7.1. Composición observada

- Trigger circular de 44 × 44 px.
- Diálogo nativo `<dialog>` abierto mediante `showModal()`, renderizado en portal.
- Barra a la misma altura y con la misma altura física del trigger original.
- Ancho máximo de 720 px, con márgenes de viewport; en móvil, `calc(100vw - 24px)`.
- Capa material independiente, con `transform-origin: left center`.
- Gradiente glass desde superficie elevada al 94%, pasando por 78%, hasta 86%; borde calculado con 12% de tinta.
- Blur de 24 px, saturación de 150% y reflejo interior superior.
- Resultados en otro panel glass bajo la barra, con radio de panel y scroll interior.
- Overlay del diálogo transparente, con blur de 24 px y saturación 1.2; no añade un tinte de marca.

### 7.2. Secuencia observada

| Fase | Movimiento/estado | Tiempo y curva |
| --- | --- | --- |
| Apertura con puntero | Material parte del rectángulo del trigger y se revela horizontalmente mediante `transform` y `clip-path`. | 240 ms, `ease-in-out`. |
| Aparición del input | Fade del contenido una vez iniciada la expansión. | 100 ms, delay de 80 ms, `ease-out`. |
| Aparición de resultados | Fade y habilitación de interacción. | 160 ms, delay de 60 ms, `ease-out`. |
| Overlay | Fade del desenfoque ya configurado. | 180 ms, `ease-out`. |
| Cierre con puntero | Contracción del material hacia el trigger. | 200 ms. |
| Desaparición de resultados | Fade y ocultamiento diferido. | 120 ms. |
| Apertura/cierre por teclado | Estado instantáneo; foco al input o al trigger. | Sin transición. |
| Movimiento reducido | Omite expansión; prevé fades breves del contenido. | 120 ms; sujeto al reset global. |

La máquina de estados distingue `closed`, `opening`, `open` y `closing`. Cancela frames y timers al cerrar. Un puntero sobre la barra durante el cierre puede recuperar el estado abierto. El input recibe foco y la selección de un resultado cierra el buscador antes de navegar.

El alto de resultados responde a `visualViewport`, de modo que la aparición del teclado móvil tiene una ruta específica de ajuste. Escape cierra inmediatamente. El cambio de ancho de ventana provoca cierre inmediato en el código leído.

### 7.3. Qué adaptar

**Confirmada:** conservar la versión glass para el destino. La variante plana de plataforma se excluye de la receta.

**Propuesta:** sustituir referencias de selección y trigger de marca por tokens neutrales; preservar los tiempos y el patrón de expansión. No copiar textos comerciales, campos del catálogo o nombres de marcas. El patrón sirve para buscar entidades propias de cualquier aplicación.

El diálogo modal pertenece a la capa superior nativa del navegador. Sus relaciones con portales, tooltips y toasts deben diseñarse de forma explícita; aumentar un `z-index` en el documento no es una solución universal para atravesar esa capa.

## 8. Overlays y feedback de carga

Un overlay es una capa que expresa una relación con el fondo. El destino debe distinguir estos casos aunque compartan material.

| Tipo | Referencia observada | Comportamiento que se transfiere |
| --- | --- | --- |
| Scrim modal | `surface-scrim`, fade de 250 ms. | Atenúa el entorno y concentra la atención sobre el diálogo glass. |
| Overlay de búsqueda | Fondo transparente, blur de 24 px, saturación 1.2. | Mantiene visible el contexto, difuminado, sin colorearlo con marca. |
| Scrim de navegación móvil | Tinte tenue, blur de 8 px, fade de 240 ms. | Separa el drawer y recibe cierre exterior cuando corresponde. |
| Carga de opciones de un formulario | Overlay absoluto sobre la región, `surface-glass`, blur de 12 px y spinner. | Bloquea esa región mientras llegan opciones; no cubre toda la página. |
| Trabajo de autenticación | Capa absoluta, radio heredado, blur de 18 px y saturación de 140%. | Comunica proceso con spinner, mensaje y detalle. |
| Inicio/cierre de aplicación | Acrílico a viewport completo y fade coordinado. | Uso infrecuente o vinculado a una tarea real. |

### 8.1. Carga local

**Observada:** el formulario de compatibilidad utiliza una región posicionada y un overlay `absolute inset-0` mientras `optionsLoading` es verdadero. Muestra un spinner y un texto accesible. `searching`, en cambio, gobierna el spinner del botón de búsqueda.

**Propuesta:** conservar esa distinción por alcance. Carga de dependencias de un formulario → overlay del formulario. Acción concreta → feedback del botón. Proceso que impide continuar en toda la app → overlay global justificado. Nunca simular avance o tiempo de espera para exhibir una animación.

El overlay hereda el radio del contenedor y recorta su material dentro de la misma forma. Los controles cubiertos se deshabilitan o vuelven inertes según el caso; una capa visual por sí sola no impide que el teclado alcance controles del fondo. Anunciar carga mediante `role="status"`, `aria-live` o `aria-busy` según la región.

### 8.2. Spinners

El spinner compartido utiliza un icono con `motion-safe:animate-spin`. Existen también spinners locales con períodos distintos: 850 ms en estilos públicos y 800 ms en actividad de inicio. No hay un único período universal impuesto por todos los componentes.

**Propuesta:** token de spinner de 800 ms lineal, solo durante trabajo real, con texto accesible; con movimiento reducido, indicador estático y mensaje. Su color será tinta del tema o estado funcional, sin acento de marca.

### 8.3. Toasts

Los toasts web usan Sonner, ubicación inferior derecha, variantes semánticas y fondos translúcidos al 80%. El código de `App.tsx` personaliza su aspecto, pero no define allí un contrato de duración/easing para la entrada y salida de Sonner. Este informe no atribuye tiempos de librería que no se inspeccionaron.

**Propuesta:** `glass-popover`, tinte semántico tenue, icono y texto; una única composición compartida para ambos temas. Conservar la gestión de apilamiento y cierre de la librería elegida. No envolver cada toast en otra animación que compita con su ciclo de vida. Mensajes críticos deben tener permanencia o una vía para recuperarlos.

## 9. Vocabulario de movimiento observado

### 9.1. Tokens comunes

| Token | Valor observado | Intención |
| --- | --- | --- |
| `motion-fast` | `150ms` | Feedback y microestados. |
| `motion-standard` | `200ms` | Cambios de estado de componente. |
| `motion-slow` | `300ms` | Transiciones infrecuentes con más presencia. |
| `ease-standard` | `cubic-bezier(.2,.75,.25,1)` | Desplazamiento o cambio de estado contenido. |
| `ease-enter` | `cubic-bezier(.23,1,.32,1)` | Entrada perceptiblemente rápida y asentamiento suave. |
| `ease-out` | `cubic-bezier(.23,1,.32,1)` | Mismo valor de la entrada; alias general de respuesta. |
| `ease-in-out` | `cubic-bezier(.77,0,.175,1)` | Transformación entre estados o posiciones. |
| `ease-drawer` | `cubic-bezier(.32,.72,0,1)` | Recorrido de panel lateral. |

La curva `ease-enter` avanza rápido al comienzo y desacelera hacia el final. Evitar describirla como una entrada que arranca lenta.

### 9.2. Matriz operativa para el manual de destino

| Elemento | Disparador y propósito | Propiedades | Entrada / salida | Origen y reducción |
| --- | --- | --- | --- | --- |
| Botón | Presión; feedback. | `transform`, color/borde. | 150 ms; escala `.97`. | Centro; sin escala por teclado/reduced motion. |
| Dropdown glass | Apertura ocasional; relación con trigger. | `opacity`, `transform`. | 200 / 150 ms, `ease-out`. | Ancla real; teclado instantáneo. Propuesta. |
| Tooltip | Ayuda de hover/foco. | `opacity`, escala leve. | 150 ms; delay hover 280 ms. | Ancla; foco inmediato. Propuesta a partir de CSS. |
| Modal glass | Apertura ocasional; concentración de tarea. | `opacity`, `translateY`, `scale`. | 250 / 250 ms, `ease-enter`. | Centro; sin desplazamiento/escala con reduced motion. |
| Drawer glass | Mostrar navegación; relación espacial. | `translateX`; opacidad del scrim. | 240 / 240 ms, `ease-drawer`. | Borde izquierdo; estado inmediato o fade con reduced motion. |
| Acciones móviles | Mostrar grupo contextual. | `opacity`, `translateY`, escala leve. | 200 / 160 ms, `ease-out`. | Superior derecho; fade de 120 ms propuesto para reducción. |
| Buscador | Expandir desde trigger. | `transform`, `clip-path`, opacidad por capas. | 240 / 200 ms; detalle en sección 7. | Altura preservada; teclado instantáneo. |
| Segmentado | Selección por puntero. | `transform` del indicador. | 250 ms, `ease-standard`. | Dentro de pista; teclado instantáneo. |
| Acordeón | Revelar contenido relacionado. | Filas de grilla, opacidad, rotación. | 200 ms. | Sin transición con reduced motion; excepción de layout. |
| Favorito | Confirmar estado. | Opacidad del relleno. | 140 ms, `ease-out`. | Sin movimiento; teclado instantáneo. |
| Spinner | Trabajo pendiente real. | Rotación. | 800 ms lineal repetido, propuesto. | Se detiene al concluir; estático en reducción. |

Los tiempos particulares de 240 o 250 ms son recetas observadas, aunque no estén representados por un token global específico. **Propuesta:** crear aliases de componente que apunten a esos valores; no redondear todo a 300 ms ni multiplicar curvas locales.

### 9.3. Entradas editoriales: conservar como excepciones

En origen hay movimientos de mayor amplitud, útiles para comprender la personalidad pública, pero no como duración base de una app de trabajo:

- Panel de presentación: opacidad de 600 ms y `translateY(32px)` de 700 ms, activado por la sección visible con IntersectionObserver.
- Panel de detalle: fade y subida de 20 px durante 600 ms, con demora de 600 ms.
- Imagen de detalle: secuencia de 1 s desde `translateX(25vw) scale(1.6)`; en móvil cambia a escala `1.2 → 1`.
- Transición entre páginas públicas: desplazamiento horizontal de viewport en 500 ms con curva de drawer y fallback a navegación directa.
- Chevron de invitación a scroll: período de 1.8 s repetido.

**Propuesta:** conservarlos solo en onboarding o una sección explicativa infrecuente del destino, si aporta algo. No retrasar el acceso a datos o controles por reproducir una entrada editorial. La decisión de glassmorphism global no obliga a usar animaciones largas globalmente.

### 9.4. Bienvenida acrílica

**Observada:** overlay a viewport completo, blur de 24 px, saturación 1.2, degradados de halo/reflejo y un elemento central. Cuando el recurso central está listo: entrada de 300 ms desde escala `.97`, permanencia de 600 ms, salida del elemento en 200 ms; el fade del velo comienza a los 1000 ms y dura 300 ms. La secuencia acaba a los 1300 ms desde ese inicio, no necesariamente desde que se solicita la página.

Con movimiento reducido, los tokens locales prevén 150 ms de entrada, 200 ms de permanencia, 150 ms de salida y velo de 150 ms; se elimina la escala y se usan fades. Hay un timeout de liberación de 4 s para fallos de la secuencia web. La variante de actividad Desktop permanece vinculada a `active` hasta que concluye el trabajo real; no usa la permanencia decorativa como indicador de progreso.

**Propuesta para trasladar:** conservar material, coordinación y recuperación ante fallo; sustituir el recurso central por identidad propia del destino y eliminar tintes/halos de marca. No incorporar la bienvenida como efecto de apertura de cada panel.

### 9.5. Springs

El CSS compartido declara `spring-damping: 1.0`, `spring-response: .35s`, `spring-bounce: 0`, y una variante de momentum con damping `.8` y response `.4s`. Son tokens observados; su presencia no significa que los componentes inspeccionados estén ejecutando un motor de resortes.

**Propuesta:** usar springs solo para arrastre, swipe o movimientos realmente interrumpibles que lo requieran. Los valores de damping/response deben traducirse al API concreto del motor elegido; no equivalen automáticamente a `stiffness` o `damping` de otra librería. Paneles, dropdowns y modales pueden conservar sus transiciones CSS sin dependencia adicional.

## 10. Accesibilidad, interrupción y rendimiento como parte del estilo

### 10.1. Movimiento reducido: separar intención y cascada actual

**Observada:** existe un reset global de animación/transición a `.01ms !important`, una sola iteración y scroll inmediato. Hay además reglas locales con animaciones desactivadas o fades previstos de 120–160 ms. La bienvenida añade overrides explícitos para sus fades.

Por ello, una regla local de 120 ms sin la prioridad necesaria puede seguir siendo anulada por el reset global. Este informe distingue el valor declarado localmente del efecto final de la cascada, que no se comprobó en navegador.

**Propuesta para el manual de destino:** definir una política única: eliminar desplazamiento, zoom, bounce y scroll suave; mantener, cuando ayude, un fade de 120–150 ms. Teclado y acciones de altísima frecuencia siguen siendo inmediatos. Eliminar giros continuos de loaders. No mezclar un reset universal con excepciones locales inconsistentes.

### 10.2. Transparencia reducida y contraste

**Observada:** buscador y checkout tienen reglas para `prefers-reduced-transparency` y/o `prefers-contrast` que retiran blur y aumentan solidez/borde.

**Propuesta:** extender esa adaptación a todas las primitivas glass del destino. El estilo normal sigue siendo glass; la representación sólida se reserva para la preferencia accesible o ausencia de soporte de blur. Se trata de una adaptación técnica del mismo componente, no de una familia visual plana para la aplicación.

Como el contenido de fondo cambia, el contraste de una superficie translúcida depende de la composición final. En este trabajo no se efectuaron mediciones. El manual no debe etiquetar ningún par como contraste aprobado a partir de esta lectura estática.

### 10.3. Interacción

- Foco visible y nombre accesible en controles de solo icono.
- Estados deshabilitados funcionales, no solo menos opacos.
- Hover con movimiento dentro de `@media (hover: hover) and (pointer: fine)`.
- Contenido oculto fuera del recorrido de teclado; overlays bloqueantes con fondo inerte o comportamiento modal equivalente.
- Retorno de foco al disparador tras cerrar; no retrasar el foco hasta que acabe una entrada decorativa.
- Mensajes de estado legibles sin depender de animación o color.
- No bloquear teclado o lectura mientras se completa un stagger de elementos.

### 10.4. Propiedades y composición

**Propuesta derivada de las skills:** preferir `transform` y `opacity` para desplazamiento y aparición, y transiciones explícitas de color/borde para feedback. `clip-path` se reserva para el buscador u otra revelación justificada; la expansión de acordeón es una excepción de layout documentada.

El blur es material estable. Mostrar u ocultar la capa mediante opacidad; evitar animar su radio continuamente. No instalar una librería para un fade. Las transiciones CSS pueden retargetear desde el valor visible; un keyframe reiniciado puede saltar cuando la acción se dispara repetidamente.

La combinación de blur, área grande, capas superpuestas y contenido de fondo cambiante tiene costo. Este informe no afirma que una receta garantice aceleración o fluidez en todo dispositivo. Limitar superficie y cantidad de filtros, evitar `will-change` permanente en todos los paneles y conservar un fallback explícito.

## 11. Capas y posicionamiento

**Observada:** el sistema contiene valores locales de z-index: controles alrededor de 30–50, drawer móvil a 60, modal base a 80 y valores muy elevados para bienvenida/tooltips. Es una colección de decisiones de componentes, no una escala global completa.

**Propuesta para el destino:**

| Capa | z-index propuesto | Alcance |
| --- | --- | --- |
| Contenido | 0 | Paneles de página. |
| Navegación fija | 20 | Header y controles permanentes. |
| Popover contextual | 40 | Dropdown y notificaciones. |
| Scrim de drawer | 50 | Fondo de navegación móvil. |
| Drawer | 60 | Navegación temporal. |
| Scrim modal | 80 | Fondo del diálogo. |
| Modal | 90 | Contenido del diálogo. |
| Toast/tooltip contextual | 100 | Dentro del contexto de interacción que corresponda. |
| Actividad global | 110 | Proceso que realmente cubre la app. |

Esta escala rige dentro de contextos de apilamiento compatibles. `backdrop-filter`, transforms e `isolation` pueden introducir límites que deben considerarse. Dialog y popover nativos en top layer necesitan una política propia; no se resuelve copiando números enormes del origen. Los tooltips de un modal deben pertenecer a su contexto adecuado.

## 12. Base CSS propuesta, sin marca

La siguiente base es una **especificación de referencia**, no código incorporado a la interfaz. Usa valores observados de tema y una jerarquía glass propuesta. El radio y la sombra pueden especializarse por componente sin alterar la paleta.

```css
:root {
  color-scheme: light;
  --surface-canvas: #f6f8fc;
  --text-primary: #132033;
  --text-secondary: #4f6178;
  --glass-rgb: 255, 255, 255;
  --glass-alpha-chrome: .72;
  --glass-alpha-panel: .88;
  --glass-alpha-dense: .94;
  --glass-alpha-field: .56;
  --glass-highlight: rgba(255, 255, 255, .70);
  --surface-scrim: rgba(3, 12, 27, .68);
  --border-subtle: rgba(19, 32, 51, .14);
  --border-strong: rgba(19, 32, 51, .28);
  --focus-ring: #132033;
  --focus-separator: #ffffff;
  --selection-background: rgba(19, 32, 51, .08);
  --shadow-control: 0 4px 14px rgba(0, 0, 0, .08);
  --shadow-card: 0 14px 36px rgba(0, 0, 0, .08);
  --shadow-panel: 0 30px 80px rgba(0, 0, 0, .20);
  --shadow-popover: var(--shadow-panel);
  --radius-control: .875rem;
  --radius-card: 1.5rem;
  --radius-panel: 2rem;
  --radius-pill: 999px;
  --motion-fast: 150ms;
  --motion-standard: 200ms;
  --motion-slow: 300ms;
  --ease-standard: cubic-bezier(.2, .75, .25, 1);
  --ease-out: cubic-bezier(.23, 1, .32, 1);
  --ease-enter: var(--ease-out);
  --ease-in-out: cubic-bezier(.77, 0, .175, 1);
  --ease-drawer: cubic-bezier(.32, .72, 0, 1);
}

.dark {
  color-scheme: dark;
  --surface-canvas: #0b1525;
  --text-primary: #ecf2fb;
  --text-secondary: #b6c4d9;
  --glass-rgb: 20, 35, 59;
  --glass-alpha-chrome: .76;
  --glass-alpha-panel: .90;
  --glass-alpha-dense: .96;
  --glass-alpha-field: .62;
  --glass-highlight: rgba(255, 255, 255, .08);
  --surface-scrim: rgba(0, 5, 14, .78);
  --border-subtle: rgba(255, 255, 255, .10);
  --border-strong: rgba(255, 255, 255, .20);
  --focus-ring: #ecf2fb;
  --focus-separator: #0b1525;
  --selection-background: rgba(236, 242, 251, .10);
  --shadow-control: 0 4px 16px rgba(0, 0, 0, .24);
  --shadow-card: 0 18px 44px rgba(0, 0, 0, .28);
  --shadow-panel: 0 34px 90px rgba(0, 0, 0, .42);
}

.glass {
  --glass-alpha: var(--glass-alpha-panel);
  --glass-blur: 20px;
  --glass-saturation: 145%;
  color: var(--text-primary);
  border: 1px solid var(--border-subtle);
  border-radius: var(--radius-card);
  background: rgba(var(--glass-rgb), var(--glass-alpha));
  -webkit-backdrop-filter: blur(var(--glass-blur)) saturate(var(--glass-saturation));
  backdrop-filter: blur(var(--glass-blur)) saturate(var(--glass-saturation));
  box-shadow: var(--shadow-card), inset 0 1px 0 var(--glass-highlight);
}
.glass--chrome {
  --glass-alpha: var(--glass-alpha-chrome);
  --glass-blur: 16px;
  --glass-saturation: 140%;
}
.glass--dense, .glass--popover {
  --glass-alpha: var(--glass-alpha-dense);
  --glass-saturation: 140%;
}
.glass--field {
  --glass-alpha: var(--glass-alpha-field);
  --glass-blur: 12px;
  --glass-saturation: 120%;
  border-radius: var(--radius-control);
  box-shadow: inset 0 1px 0 var(--glass-highlight);
}
.glass--dialog {
  --glass-blur: 24px;
  --glass-saturation: 150%;
  border-radius: var(--radius-panel);
  box-shadow: var(--shadow-panel), inset 0 1px 0 var(--glass-highlight);
}
.glass--popover {
  box-shadow: var(--shadow-popover), inset 0 1px 0 var(--glass-highlight);
}
.glass-control:focus-visible {
  outline: 3px solid var(--focus-ring);
  outline-offset: 3px;
  box-shadow: 0 0 0 6px var(--focus-separator);
}

/* Contrato de presencia: conservar el montaje durante la salida. */
.glass-popover {
  opacity: 0;
  visibility: hidden;
  pointer-events: none;
  transform: translateY(-4px) scale(.97);
  transform-origin: var(--popover-origin, top right);
  transition: opacity 150ms var(--ease-out), transform 150ms var(--ease-out),
    visibility 0s linear 150ms;
}
.glass-popover[data-open="true"] {
  opacity: 1;
  visibility: visible;
  pointer-events: auto;
  transform: translateY(0) scale(1);
  transition-duration: 200ms, 200ms, 0s;
  transition-delay: 0s;
}
.glass-popover[data-input="keyboard"] { transition: none; }

@media (prefers-reduced-motion: reduce) {
  .glass-popover {
    transform: none;
    transition: opacity 120ms var(--ease-out), visibility 0s linear 120ms;
  }
  .glass-popover[data-open="true"] {
    transition-duration: 120ms, 0s;
    transition-delay: 0s;
  }
  .glass-popover[data-input="keyboard"] { transition: none; }
}

/* Adaptaciones accesibles y fallback; no son variantes planas normales. */
@media (prefers-reduced-transparency: reduce), (prefers-contrast: more) {
  .glass {
    background: rgb(var(--glass-rgb));
    border-color: var(--text-primary);
    -webkit-backdrop-filter: none;
    backdrop-filter: none;
  }
}
@supports not ((backdrop-filter: blur(1px)) or (-webkit-backdrop-filter: blur(1px))) {
  .glass { background: rgb(var(--glass-rgb)); }
}
```

Para un menú se combinan las clases de material `.glass.glass--popover` con la clase de presencia `.glass-popover`. La primitiva accesible mantiene además el estado cerrado fuera de foco y conserva el montaje durante la salida. El CSS no reemplaza esa lógica.

Las acciones neutrales y los feedbacks de la sección 3 deben añadirse como tokens semánticos del manual completo. El ejemplo se limita a temas, material y presencia para no presentar una implementación parcial como una librería terminada.

## 13. Cómo usar las skills de identidad y animación

### 13.1. Identidad y consistencia: `visual-identity-system`

Es la skill disponible que corresponde a la intención de “visual consistency”. Trabaja con tres tareas principales: `DESTILAR`, `APLICAR` y `AUDITAR`. Para esta transferencia, el siguiente paso es **DESTILAR el manual de la nueva aplicación** usando este informe como fuente explícita.

Su contexto instalado incluye decisiones particulares de ST2026. Debe recibir la instrucción expresa de que el destino es otro producto, sin aliases ni reglas de marca de origen y con glassmorphism como material general. La regla administrativa plana de ST2026 no se hereda. Esto aplica la prioridad de las instrucciones del usuario sobre el contexto de la skill.

Al crear el manual, separar:

- **Fuentes canónicas:** este informe y las secciones glass de origen enumeradas, si sus archivos están disponibles.
- **Objetivo:** la nueva aplicación. Su UI actual no se vuelve automáticamente referencia canónica.
- **Requisitos confirmados:** vidrio general, ambos temas y exclusión de marcas.
- **Valores observados:** los identificados expresamente en este informe.
- **Extensiones propuestas:** jerarquía glass, paleta de acciones neutral, dropdown animado, campos glass y escala de capas.

La skill debe producir sus artefactos habituales: `index.md`, `sources.md`, `manual.md`, `tokens.json`, `components.md`, `decisions.md`, `adoption.md` y `audit.config.json`. Este informe es insumo para ellos; no afirma que ya estén creados para el destino.

El primer manual se mantiene `draft` hasta aprobación explícita. Las instrucciones actuales de glass y ausencia de marcas se registran como confirmadas aunque el resto del borrador siga pendiente.

### 13.2. Función de cada skill de movimiento

| Skill | Cuándo utilizarla en el destino | Relación con este informe |
| --- | --- | --- |
| `animate` | Construir una animación concreta. | Recibe componente, propósito, frecuencia, tokens, estados, tiempos y regla de reducción. Extiende los tokens del manual. |
| `review-animations` | Revisar un cambio concreto de movimiento. | Contrasta propiedades, tiempos, origen, teclado, interrupción, salida y accesibilidad con el contrato. No sustituye el diseño de identidad. |
| `improve-animations` | Solicitud de auditoría amplia o planes para mejorar movimiento existente. | Separa deuda de la identidad deseada; no ejecutarlo automáticamente para una transferencia documental. |
| `find-animation-opportunities` | Buscar lugares que sí merecen movimiento adicional. | Filtra por frecuencia y propósito; no añade animación a cada superficie por ser glass. |
| `apple-design` | Gestos, continuidad espacial, materiales translúcidos y profundidad. | Complementa decisiones de vidrio e interrupción; no impone una paleta de marca ni sustituye los tokens. |
| `emil-design-eng` | Afinar respuesta, microinteracciones y coherencia perceptual. | Refuerza restricción, curvas, frecuencia y feedback. |
| `prototype` | Solo si se pide explícitamente comparar alternativas mediante un selector visual. | Puede explorar grados de vidrio conservando la dirección aprobada. |

`pdp-consistency` tiene alcance de páginas de detalle de producto y depende de un manual aprobado; no es la autoridad global para una app nueva. `pick-ui-library` se utiliza únicamente cuando se invoca expresamente; este informe no selecciona ni instala dependencias.

### 13.3. Contrato mínimo que debe recibir `animate`

Para cada animación, proporcionar:

1. Componente y transición de estado: cerrado → abierto, pendiente → resuelto, etc.
2. Propósito: feedback, continuidad espacial o indicación de estado.
3. Frecuencia estimada y comportamiento por teclado.
4. Propiedades exactas, estado inicial/final y origen de transformación.
5. Duración, curva y delay tomados del manual.
6. Condiciones de interrupción, reversión y desmontaje.
7. Variante de movimiento reducido y soporte táctil.
8. Material y tema, sin animar continuamente el blur.

Ejemplo de encargo: “Implementar apertura de un menú glass ocasional con puntero: opacidad y `translateY(-4px) scale(.97)` hacia estado estable, 200 ms `var(--ease-out)`, salida de 150 ms, origen según ancla, teclado inmediato y fade reducido de 120 ms. Usar los tokens existentes y mantener foco/cierre de la primitiva accesible”.

### 13.4. Resolver tensiones entre skills

La identidad define material, color, geometría y personalidad; `animate` define una implementación coherente con ese contrato; `review-animations` señala desviaciones del cambio. Una recomendación genérica no debe borrar un comportamiento deliberado sin documentar la decisión.

Las skills desaconsejan UI larga, rebotes gratuitos, `transition: all`, entradas desde escala cero, movimiento en acciones de teclado y animaciones de layout innecesarias. Las secuencias editoriales observadas de 500–1000 ms se documentan como excepciones infrecuentes. El acordeón conserva una excepción funcional de expansión. Los filtros de material de 24–28 px requieren una decisión de alcance, no una afirmación automática de buen rendimiento.

El usuario pidió no hacer tests ni validaciones salvo solicitud expresa. Esa restricción debe acompañar el trabajo del destino: no ejecutar escáneres, builds, tests, capturas ni validación visual como efecto automático de una skill. Registrar lo omitido y el estado de evidencia; no convertirlo en una declaración de verificación.

## 14. Diferencias del origen que no deben convertirse en reglas accidentales

Esta tabla organiza la transferencia documental; no es una validación de funcionamiento ni una solicitud de cambiar ST2026.

| Antes: evidencia de origen | Después: contrato de destino | Por qué |
| --- | --- | --- |
| Manual distingue paneles editoriales glass y operativos planos. | Todos los paneles normales son glass, con distinta densidad. | Instrucción expresa posterior del usuario. |
| `Card` ya incluye glass, aunque el manual restringe su uso. | Material explícito por primitiva y rol. | Evita heredar contradicciones entre regla y componente. |
| `.brand-banner__panel` fuerza superficie opaca y desactiva blur. | Excluir esa receta material; conservar solo su movimiento si se necesita una entrada editorial. | El nombre o la documentación histórica no prueban vidrio actual. |
| `.ds-modal` fija fondo elevado; checkout sí declara vidrio. | Modal glass derivado del checkout y del contrato de foco del modal base. | Combina las referencias pertinentes al pedido. |
| Menú de perfil sin entrada/salida propia; manual menciona una transición de perfil. | Identificar la animación de dropdown como propuesta. | Evita presentar un tiempo documentado como comportamiento actual. |
| Tooltip portal fija `transform: none`, aunque el CSS tiene otra receta. | Contrato único de presencia del tooltip. | Evita transferir una animación que puede no ejecutarse. |
| Algunos componentes usan `ease-out` de Tailwind o curvas locales. | Alias de componentes sobre las curvas del manual. | Mantiene una respuesta común. |
| Reset global `.01ms !important` más fades locales. | Una política de reducción consistente. | Distingue intención declarada y cascada efectiva. |
| Foco, bordes y ciertas sombras dependen de marca. | Tokens neutrales de foco, contorno y sombra. | Cumple la exclusión de colores comerciales. |
| `shadow-popover` consumido sin definición encontrada en `src`. | Alias explícito a una sombra definida. | El nuevo manual no depende de un token incompleto. |
| Enlaces expandibles animan `max-width` y márgenes; sidebar desktop anima ancho/padding. | Conservar su función solo si aporta; no elevar esa técnica a regla de movimiento. | Son animaciones de layout con costo distinto. |
| Hover y escalas no siempre tienen la misma política de teclado/touch. | Política central por método de entrada y movimiento reducido. | Coherencia de interacción entre componentes. |

**Decisión de transferencia:** conservar el lenguaje glass y sus recetas explícitas. Las contradicciones y los valores sin implementación demostrada quedan documentados; no se aprueba una copia literal de todo el CSS de origen.

## 15. Cobertura para que la skill cree el manual

| Área | Evidencia | Cobertura en este informe | Ejecución comprobada ahora | Aprobación para destino |
| --- | --- | --- | --- | --- |
| Glass como material general | Confirmada por usuario. | Cubierta. | No verificada. | Dirección aprobada. |
| Exclusión de marca y paneles planos normales | Confirmada por usuario. | Cubierta. | No aplica como comportamiento. | Dirección aprobada. |
| Temas claro/oscuro | Observada + neutralización propuesta. | Cubierta como especificación. | No verificada. | Valores nuevos pendientes. |
| Paneles, tarjetas y chrome | Observada + propuesta. | Cubierta. | No verificada. | Pendiente visual. |
| Campos glass y lista personalizada | Propuesta; select nativo observado. | Parcial: falta elegir primitiva del destino. | No verificada. | Pendiente. |
| Dropdown glass animado | Propuesta sobre anatomía observada. | Cubierta como receta. | No verificada. | Pendiente. |
| Modales, overlays y loaders | Observada + propuesta de material. | Cubierta. | No verificada. | Pendiente visual. |
| Buscador expandible | Observada + neutralización propuesta. | Cubierta. | No verificada. | Pendiente visual. |
| Drawer, segmentado y acordeón | Observada + material propuesto. | Cubierta. | No verificada. | Pendiente visual. |
| Toasts | Apariencia observada; movimiento delegado a librería. | Parcial: no se extrajeron tiempos internos. | No verificada. | Pendiente. |
| Springs y gestos complejos | Tokens observados; uso propuesto. | Parcial: sin receta de gesto específica. | No verificada. | Pendiente si se necesitan. |
| Contraste y rendimiento real | Reglas de intención. | No cubiertos mediante medición. | No verificados. | Sin aprobación técnica nueva. |

El vocabulario del `tokens.json` final debe conservar campos independientes. Ejemplo de un token derivado:

```json
{
  "value": "rgba(20,35,59,0.90)",
  "purpose": "Panel glass principal en tema oscuro",
  "confidence": "Observada",
  "approval": "Pendiente",
  "verification": "No verificada",
  "decisionId": "GLASS-002",
  "theme": "dark",
  "source": "informe-portabilidad-glassmorphism.md, seccion 3.1"
}
```

IDs propuestos para decisiones del nuevo manual:

| ID | Regla que debe registrar |
| --- | --- |
| `GLASS-001` | Material glass general; exclusión de variantes planas normales. Confirmada. |
| `GLASS-002` | RGB de tema y escala de densidad glass; distinguir observados/propuestos. |
| `GLASS-003` | Exclusión de aliases y colores de marca; acciones y foco neutrales. Confirmada en alcance, propuesta en valores. |
| `GLASS-004` | Geometría y sombras por nivel. |
| `GLASS-005` | Vocabulario de curvas/tiempos y excepciones editoriales. |
| `GLASS-006` | Teclado inmediato y reducción de movimiento coherente. |
| `GLASS-007` | Overlays por alcance, foco y presencia hasta completar salida. |
| `GLASS-008` | Adaptación por transparencia/contraste y fallback de blur. |
| `GLASS-009` | Capas, portales y top layer. |
| `GLASS-010` | Sin tests ni validaciones sin solicitud expresa; evidencia declarada con precisión. Confirmada. |

Estos IDs son una propuesta para un manual nuevo; no sustituyen ni reutilizan IDs del manual aprobado de origen.

## 16. Texto listo para entregar a la skill en otra aplicación

Copiar el siguiente encargo y adjuntar este informe completo:

> Usá `visual-identity-system` en modo `DESTILAR` para crear el manual de identidad de esta aplicación a partir del informe adjunto “Transferencia: glassmorphism, temas e interacción”, versión 0.1.0 del 23/10/2026.
>
> Tomá el informe como fuente canónica de transferencia. El producto de destino es otro: no heredes contexto comercial, nombres, logos, fotografías, aliases ni colores de marca de ST2026. Conservá únicamente sus colores funcionales de tema claro/oscuro, material y estados, con las neutralizaciones propuestas explícitamente identificadas.
>
> La decisión confirmada es usar GLASSMORPHISM como lenguaje general de paneles, tarjetas, formularios, menús, navegación y diálogos. No traslades la regla de superficies operativas planas del proyecto de origen. Usá distintas densidades de vidrio para mantener lectura y jerarquía. Las representaciones sólidas quedan limitadas a adaptaciones accesibles o ausencia de soporte.
>
> Documentá blur, saturación, opacidad, borde, reflejo, sombra, radios, tipografía, espaciado, capas, responsive y todos los estados. Para movimiento, especificá propósito, frecuencia, propiedades, origen, estados inicial/final, duración, curva, delay, interrupción, salida, desmontaje, teclado y movimiento reducido. Usá las matrices y valores del informe; no inventes animaciones “existentes” para dropdowns, select nativo, tooltip portal o toasts.
>
> Separá requisitos confirmados, observaciones, inferencias y propuestas. Todo lo extraído de código mantiene verificación de ejecución “No verificada” en esta transferencia. No importes las etiquetas de verificación histórica del manual de origen como evidencia de esta tarea.
>
> Generá `index.md`, `sources.md`, `manual.md`, `tokens.json`, `components.md`, `decisions.md`, `adoption.md` y `audit.config.json` dentro de la documentación de esta aplicación. Incluí recetas para panel, formulario, tabla, dropdown, select, tooltip, modal, drawer, buscador, segmentado, loader, alerta y toast. Dejá el conjunto en `draft` hasta aprobación explícita. No modifiques todavía la UI.
>
> Hacé compatible el contrato con `animate`, `review-animations`, `apple-design` y `emil-design-eng`. `animate` implementará piezas concretas posteriormente; `review-animations` revisará esos cambios. No actives automáticamente auditorías amplias ni nuevas dependencias. No ejecutes tests, builds, escáneres ni validaciones visuales o funcionales salvo que yo lo pida expresamente. Registrá los límites sin detener la producción del borrador.

## 17. Fuentes y trazabilidad

Raíz de origen: `C:/Dev/ST2026-Radal`. Las ubicaciones son pistas de procedencia; la especificación transferible está contenida en este informe y puede usarse sin acceso al checkout. Referencias a líneas corresponden a la lectura del 23/10/2026 y pueden cambiar.

Las fuentes S01–S04, S07, S09–S12 y S14–S16 se tomaron como entradas directas de material/interacción. S05, S06, S08 y S13 aportan dependencias o contexto. El manual previo aporta contexto histórico y contradicciones; sus colores de marca quedan excluidos.

| ID | Archivo de origen | Qué respalda |
| --- | --- | --- |
| S01 | `src/shared/styles.css`: 5–125, 228–418, 498–520, 534–583, 1067–1100, 1396–1461, 1485–1509, 2177–2184, 2292–2319. | Tokens de tema/movimiento, modales, tooltip CSS, controles glass, reset de movimiento, segmentado, checkout y adaptaciones. |
| S02 | `tailwind.config.ts`. | Mapeo semántico, radios, sombras, pila tipográfica y activación de oscuro por clase. |
| S03 | `src/presentation/components/ui/card.tsx`. | Material glass compartido de tarjeta. |
| S04 | `src/presentation/components/ui/button.tsx`, `input.tsx`, `select.tsx`. | Estados y geometría de controles; select nativo. Las superficies planas se excluyen. |
| S05 | `src/app/providers.tsx`: 115–133; `src/presentation/components/ui/public-theme-toggle.tsx`. | Persistencia, resolución de tema, clase raíz e interacción del toggle. |
| S06 | `src/presentation/components/modals/base-modal.tsx`; `src/presentation/components/ui/tooltip.tsx`. | Ciclo de vida, foco, portal, cierre y posicionamiento. |
| S07 | `src/presentation/styles/brand-showcase.css`: 57–69, 506–572, 614–639, 1077–1100, 3192–3225, 3233–3236, 3604–3613. | Panel glass observado, controles, excepciones opacas, movimientos editoriales, overlay busy y cambio de formulario. |
| S08 | `src/presentation/pages/brand-showcase-page.tsx`: 78–109 y 329–349; `src/presentation/styles/product-detail.css`: 24–35, 149–158, 177–183, 386–440. | Disparadores de intersección, View Transition y movimientos de detalle. |
| S09 | `src/presentation/components/catalog/public-catalog-search.tsx`; `src/presentation/styles/public-catalog-search.css`. | Máquina de estados, expansión, vidrio en gradiente, overlay, tiempos, teclado y viewport móvil. |
| S10 | `src/presentation/styles/radal-floating-controls.css`: 82–126. | Variante de controles glass y reducción de movimiento. Se excluyen logos y aliases de marca. |
| S11 | `src/presentation/components/auth/user-dropdown-menu.tsx`; `public-user-button.tsx`; `src/presentation/components/layout/app-shell.tsx`: 419–481. | Menú de perfil, anatomía, montaje condicional y notificaciones. |
| S12 | `src/presentation/styles/app-shell-mobile.css`; `responsive-public-actions.css`; `src/presentation/components/ui/responsive-public-actions.tsx`. | Drawer, scrim y apertura responsive con modo instantáneo. |
| S13 | `src/app/App.tsx`: 16–32; `package.json`. | Toasts web y dependencias declaradas, entre ellas Sonner, cmdk y Tailwind. |
| S14 | `src/presentation/components/catalog/yuasa-compatibility-search.tsx`: 293–299; `compatibility-search-ui.tsx`; `src/presentation/components/ui/spinner.tsx`. | Overlay local frente a spinner de botón. Se excluye el color de marca del spinner local. |
| S15 | `src/presentation/components/ui/startup-welcome.tsx`; `src/presentation/styles/startup-welcome.css`. | Secuencia de bienvenida, actividad real, material acrílico, tiempos y recuperación. |
| S16 | `src/presentation/components/ui/segmented-toggle.tsx`; `collapsible-panel.tsx`; `src/presentation/components/catalog/favorite-button.tsx`; `src/presentation/styles/favorite-button.css`. | Selección, expansión, teclado y feedback de favorito. |
| S17 | `docs/design-system/manual.md`, secciones 0, 3, 6 y 8; `components.md`; `decisions.md`; `tokens.json`. | Contrato anterior y diferencias con el código; no aprueba la identidad del destino. |

Skills consultadas como metodología o referencia para el encargo posterior:

- `.agents/skills/visual-identity-system/SKILL.md` y referencias de contexto, alcance, destilación y contrato del manual.
- `.agents/skills/animate/SKILL.md`.
- `.agents/skills/review-animations/SKILL.md`.
- `.agents/skills/apple-design/SKILL.md`.
- `.agents/skills/emil-design-eng/SKILL.md`.
- `.agents/skills/improve-animations/SKILL.md` y `.agents/skills/find-animation-opportunities/SKILL.md`, para delimitar su función; no se ejecutó su workflow de auditoría.

## 18. Reglas que deben proteger el nuevo manual

1. **Vidrio como material general:** variar densidad, blur y jerarquía conservando el mismo lenguaje en todos los paneles normales.
2. **Tema sin marca:** RGB, tinta, foco, selección y estados se expresan con semánticos propios del producto nuevo; no heredar aliases comerciales.
3. **Movimiento con causa:** cada transición tiene propósito, disparador y tiempos explícitos; teclado y acciones muy frecuentes responden de inmediato.
4. **Material y contenido separados:** el blur afecta al fondo; la transparencia estable no reduce la opacidad del texto ni de los controles.
5. **Evidencia y aprobación separadas:** no presentar propuestas, valores históricos o lectura de código como experiencia visual comprobada.

Decisiones pendientes del destino: nombre y stack de la aplicación; primitiva accesible de menús/selectores; adopción de la paleta neutral propuesta; necesidad real de gestos o entradas editoriales. Ninguna de ellas impide crear el borrador documental con este informe.

Las diferencias de mayor impacto para una copia directa son los paneles planos que aún existen en origen, los tiempos documentados que no coinciden con ciertos componentes actuales y la combinación de reset global con fades locales. Quedan excluidas como reglas automáticas de transferencia.

Siguiente paso concreto: entregar este archivo completo a `visual-identity-system` en el proyecto nuevo y producir su manual `draft` con los ocho artefactos, manteniendo las restricciones de marca, material y no validación indicadas arriba.
