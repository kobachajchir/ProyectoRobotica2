---
type: ux-audit
date: 2026-11-06
product: AFR2 Loader (WinForms)
journey: Preparar, compilar y cargar firmware
platform: desktop-windows
screens: 0 capturas archivadas (ver "Limitaciones")
findings-critical: 0
findings-major: 1
findings-minor: 9
findings-cosmetic: 1
---

# Auditoría UX — AFR2 Loader: preparar y cargar

## Resumen ejecutivo

El Loader protege bien las cargas (preparación obligatoria, KEY/RST, backups), pero el usuario no ve qué le falta antes de cargar, la barra inferior no tenía estructura propia y la zona de Operación repartía alturas por porcentaje. Esta entrega corrige en fuente: requisito pendiente visible junto a «Flashear» (F-01), barra inferior en fila propia de dos niveles (F-09), encabezados con altura medida (F-10), etapas compactas arriba (F-11), roles de botón explícitos (F-04, parcial), ayuda única por campo (F-06), estados vacíos en Eventos (F-05) y colores de filas compatibles con el tema oscuro. Sigue pendiente verificarlo en ejecución: nada se compiló ni se abrió.

## Alcance y método

- **Objetivo evaluado:** dejar un destino ATmega/ESP preparado y cargarlo sin errores de cableado.
- **Usuarios:** principiantes y experimentados; escritorio Windows 11, WinForms.
- **Evidencia:** capturas de la conversación (no son archivos del repositorio) y lectura del código en `AutoFinalRobo2-Loader/src`. Las capturas del chat no son archivos, así que no hay «antes» originales ni anotaciones. Sí hay capturas «después» (`assets/despues-*.png`) tomadas del ejecutable compilado.
- **Marcos aplicados:** los 15 principios de frontend-design-audit y Nielsen, Shneiderman, Gerhardt-Powals, Bastien & Scapin, leyes de comportamiento, Gestalt, Norman, Tognazzini, Content y WCAG (subconjunto estático).
- **No evaluable aquí:** teclado, lector de pantalla, comportamiento durante una carga, DPI 125–200 %, alto contraste real.

## Hallazgos (F-01…F-11)

Severidad: 3 = fricción importante; 2 = recuperable; 1 = detalle. Estado: **Observado** (visto en captura/código) · **Implementado en fuente** · **Pendiente**.

| ID | Sev | Pantalla | Hallazgo | Reglas | Estado |
|---|--:|---|---|---|---|
| F-01 | 3 | Operación | «Flashear» deshabilitado con motivo solo en tooltip | Nielsen #1, #6, #10 · RECALL-TAX | Implementado: `nextStep` junto a las acciones, derivado de `UpdatePreparationControls` (`MainForm.Preparation.cs`) sin cambiar la lógica de habilitación |
| F-02 | 2 | Operación | Scroll interno y superficies vacías | Gestalt: proximidad · OVERLOAD | Implementado: Operación en tres grupos rotulados (1 Destino y conexión, 2 Preparación, 3 Ejecución y resultado) y filas auto-medidas |
| F-03 | 2 | Monitor | Resumen de transferencia y envío permanentes | G-P #8 · HIERARCHY-FLAT | Implementado: la tarjeta de transferencia se reduce a su título sin transferencia; repetir y pausa quedan en «Avanzado» y conservan valores |
| F-04 | 2 | Flasher | Restaurar backup y cargar flasher con igual énfasis | Norman: Mapping · CTA-AMBIGUITY | Implementado: `ButtonRole.Secondary` para «Flashear último backup», a la izquierda de «Cargar flasher» |
| F-05 | 2 | Eventos | Regiones vacías, «Copiar detalle» sin contexto | Nielsen #1, #6 · STATE-GAP | Implementado en Eventos e Historial (texto guía y copiar deshabilitado sin selección) |
| F-06 | 2 | Configuración | «Explicar» y «Ver explicación» duplicados | Hick · FORM-FRICTION | Implementado: «Examinar…» solo para rutas |
| F-07 | 2 | Operación/Flasher | Rutas e identificadores compiten con el estado | Content #1, #4 · JARGON-LEAK | Implementado: fechas legibles en Historial y Eventos (el detalle y el registro conservan el valor completo) |
| F-08 | 2 | Varias | Texto pequeño y denso en registros | Tog: Readability | Implementado: Segoe UI 10 pt y Consolas 10 pt en la consola; el resto de registros no se midió |
| F-09 | 2 | Barra inferior | Distribución desordenada y márgenes irregulares | Nielsen #1 · PATTERN-DRIFT | Implementado: fila propia, ancho completo, márgenes de 16 px, 2 niveles (estado+progreso / seguir registro+conexiones), texto que envuelve |
| F-10 | 2 | Operación | La consola tapa títulos | B&S: Legibility | Implementado en Operación: encabezado con fila auto-medida y margen de 8 px. Otras pestañas: no reproducido |
| F-11 | 2 | Etapas | Filas porcentuales estiradas | Gestalt: proximidad | Implementado: filas auto, separación de 8 px, sobrante abajo; el registro cede altura primero (`FitOperationRows`) |

**Riesgos encontrados en código (sin reproducir):** fondo blanco al abrir detalles de eventos en tema oscuro (**corregido** de paso con colores de tema); renderers personalizados bajo alto contraste; tamaños mínimos de etapas con DPI alto; botones clasificados por palabras de su etiqueta (**reducido**: los botones tocados usan rol explícito, el resto sigue en modo `Auto`).

## Seguimiento del registro (apartado D)

Sin cambios de código: `ExecuteCore` (`MainForm.cs`) ya marca «Seguir registro», drena la cola y va al final al iniciar el comando, después de rechazar inicios inválidos (ocupado, solicitud pendiente, diálogos, monitor en transición). La GUI y MCP pasan por `Execute`, y es el único punto que crea `Runner`. Desmarcar durante el comando se respeta porque nada lo reactiva por línea, temporizador o etapa. Verificado por lectura; falta confirmarlo ejecutando.

## Qué funciona (conservar)

- P-01 Configuración filtrada por placa (Nielsen #6).
- P-02 Iconos con texto (G-P #9, WCAG 1.4.1).
- P-03 Preparación obligatoria antes de cargar y confirmación KEY/RST (Nielsen #5).
- P-04 Protección de cambios pendientes y temas persistentes (Nielsen #3).

## Antes / después (estructura)

| Elemento | Antes | Después |
|---|---|---|
| Barra inferior | `StatusStrip` con 4 ítems en una línea | Fila propia del layout raíz, 2 niveles, márgenes simétricos |
| Encabezado de Operación | Fila fija de 44 | Fila auto-medida + 8 px |
| Etapas | Filas al 100/N % | Filas auto, 8 px, sobrante debajo |
| Registro vs etapas | 55 % fijo | `max(55 %, altura de etapas)` acotado |
| Requisito de carga | Tooltip | Texto visible bajo las acciones |
| Ayuda de campos | «Explicar» + «Ver explicación» | Solo «Ver explicación» (+ «Examinar…» en rutas) |

## Matriz de cobertura

| Marco | Hallazgos |
|---|---|
| Nielsen | F-01, F-05, F-09 |
| Shneiderman | sin hallazgos nuevos |
| Gerhardt-Powals | F-02, F-03 |
| Bastien & Scapin | F-10, F-07, F-08 |
| Hick/Fitts/Miller | F-06 |
| Gestalt | F-02, F-11, F-09 |
| Norman | F-04 |
| Tognazzini | F-08 |
| WCAG 2.1 | sin medir; contraste y teclado pendientes de revisión |
| Content | F-07 |

## Implementado en la segunda tanda

Conversor con ejemplo y error junto a la entrada; MCP en tres bloques; guardado de Configuración con «Cambios sin guardar»/«Guardado a las HH:mm:ss»; roles de color centralizados en `LoaderAppearance`; botones y renderers con colores del sistema bajo alto contraste; KEY/RST ya cumplía (casilla vacía y enfocada, sin AcceptButton) y no se tocó.

## Verificación en ejecución (ventana 1220×880, tema oscuro del sistema, 100 % DPI)

Capturas del ejecutable compilado en `assets/despues-*.png` (las 8 pestañas). Al abrirlo aparecieron dos defectos de mi propia implementación, ya corregidos y recapturados:

1. El área de preparación quedaba recortada (solo se veía «Puerto/Destino») y bajo las acciones sobraban ~100 px: los `AutoSize` de TableLayout sobreestimaban la altura de las acciones. Ahora `FitOperationRows` mide el ajuste de líneas y reparte: preparación primero, resultado siempre visible, registro con el resto.
2. El estado vacío del Historial tapaba los encabezados de columna; ahora empieza debajo de ellos.

Resultado verificado en Operación (ATmega): encabezado, «1 · Destino y conexión», «3 · Ejecución y resultado», registro, panel de etapas, acciones, «Siguiente paso» y barra inferior visibles a la vez, sin scroll ni texto cortado. También verificados visualmente: Flasher (backup secundario a la izquierda), Monitor (tarjeta plegada y «Avanzado: repetir ▸»), Eventos y Historial vacíos con texto guía, Conversor con ejemplo, MCP en tres bloques y Configuración con «Sin cambios pendientes».

**No verificado:** destino ESP (etapas pobladas durante una operación), tema claro y alto contraste, otros tamaños y DPI, teclado y foco, contraste medido. Observados sin corregir: en MCP el bloque «2 ·» está sangrado 16 px respecto de «1 ·» y «3 ·»; el panel de envío del Monitor queda parcialmente bajo el borde y se alcanza con el scroll de la pestaña (ya era así).

## Pendientes

 Falta: compilar y abrir en claro, oscuro y alto contraste; ventanas 1000×650, 1206×873 y 1440×900; DPI 100–200 %; medir contraste 4,5:1 y 3:1; probar Tab/Espacio/Enter/Escape y foco; confirmar que ningún título queda tapado en otras pestañas; capturas originales y anotadas; cierre visible tras cada operación más allá de `lastResult`.
