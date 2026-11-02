# Registro de decisiones extraídas

02/11/2026 · `draft` · 0.1.0. Fuente y alcance en [sources.md](sources.md). Todos los patrones de implementación están No verificados en ejecución en esta entrega. La elección de Infraestructura como referencia está Confirmada por el pedido; eso no aprueba automáticamente cada detalle como regla global.

## Reglas y su justificación

| ID | Fecha | Categoría | Evidencia | Aprobación | Regla / razón | Fuentes | Alcance y consecuencias |
| --- | --- | --- | --- | --- | --- | --- | --- |
| INF-VIS-001 | 2026-11-02 | Esencia/composición | Inferida | Pendiente | Relato general → exploración → datos → glosario; combina expresividad técnica y lectura progresiva. | S01/S02 | Solo arquetipo explicativo; no impone esta secuencia a todas las pantallas. |
| INF-VIS-002 | 2026-11-02 | Color por zona | Observada | Pendiente | pc/info, esp/warning, link/action, client/success; herencia de tinta/fondo/borde y acento fijo de banner. | S01:20–26; S02:15–18; S12 | Repite ubicación a lo largo del viaje; no equivale a semántica de alarma/éxito físico. |
| INF-VIS-003 | 2026-11-02 | Banner/material | Observada | Pendiente | Base oscura fija, grilla 32 px, halos, glow independiente, h1 degradado, chips translúcidos y takeaway. | S01:149–193; S02:21–84; S12:471–493 | Fuente de la portada; no autoriza colorear superficies de trabajo del mismo modo. |
| INF-VIS-004 | 2026-11-02 | Tipografía | Observada | Pendiente | Familias Segoe de sistema, display para titulares/cifras, mono para contador/cables; escala y overrides locales explícitos. | S02; S12:118–120,384–405 | Preservar diferencias reales entre body token e herencia de párrafos. |
| INF-VIS-005 | 2026-11-02 | Espacio/forma | Observada | Pendiente | Stacks amplios, padding fluido, tarjetas sólidas, radios 24/18/14 px, bordes y separadores por contexto. | S02; S14:177 | Sin unificar arbitrariamente todas las medidas a un solo padding/radio. |
| INF-VIS-006 | 2026-11-02 | Selector | Observada | Pendiente | Ranura con copia activa recortada; clip-path 250 ms in-out, teclado/resize inmediatos, icono/texto/fondo coordinados. | S05/S06/S12 | Aquí choice/md/fill never; tablist y disabled no tienen muestra directa de uso. |
| INF-VIS-007 | 2026-11-02 | Estaciones/progreso | Observada | Pendiente | Anteriores/check, actual/halo, futuras/neutro; frente con gradiente fijo y scaleX de índice 520 ms. | S01:200–230; S02:90–112 | Progreso de selección, no ejecución ni visitas persistidas. |
| INF-VIS-008 | 2026-11-02 | Mapa/acción | Observada | Pendiente | Carriles por equipo y nodos-button que abren una etapa con scroll al ancla; hover zonal y foco visible. | S01:73–82,127–132,162–190; S02:45–66 | Sin estado de selección persistente en nodos ni cuarta lane link. |
| INF-VIS-009 | 2026-11-02 | Movimiento | Observada | Pendiente | Loops de cable/paquete/pulso/glow; entradas de sección una vez; tabla de tiempos explícita y reduced motion parcial. | S02/S05/S06/S13 | Describir restricciones actuales; no prometer estático total ni rendimiento validado. |
| INF-VIS-010 | 2026-11-02 | Detalle | Observada | Pendiente | Ficha remonta por key, main/side, Entra/Sale, hechos con counters y navegación limitada. | S01:211–230; S02:115–142 | No hay animación exit, morph de altura ni línea visible de source. |
| INF-VIS-011 | 2026-11-02 | Botones/enlaces | Observada | Pendiente | Button sm con escala active y spinner; colores de variante inline; enlaces conservan navegación y flecha discreta. | S07/S13; S02:10–12 | No inferir hover/active cromático por mera existencia de tokens. |
| INF-VIS-012 | 2026-11-02 | Datos/gráficos | Observada | Pendiente | Barras de GLB, compresión raw/gzip, cifras tabulares, estadísticas y tabla con procedencia. | S01:233–314; S02:148–220 | Fills mínimos son excepciones; no son medidas de memoria de ejecución. |
| INF-VIS-013 | 2026-11-02 | Estado/procedencia | Observada | Pendiente | Reporte local/ESP distinguido; storage siempre visible; anillo conic estático, progress accesible y ausencia explícita. | S01/S04/S21 | Datos no disponibles no se convierten en ocupación cero. |
| INF-VIS-014 | 2026-11-02 | Límites/gobernanza | Propuesta | Pendiente | Mantener un dossier separado; no elevar particularidades ni componentes ausentes a reglas aprobadas. | Pedido actual + límites S01/S02/S08/S21 | Cualquier corrección o adopción futura es un trabajo separado. |

Todas las filas registran la fecha, evidencia, aprobación, razón y alcance. Las alternativas siguientes distinguen lo observado de posibles decisiones posteriores; no se han implementado.

## Alternativas consideradas documentalmente

| Tema | Alternativa | Por qué no sustituye la extracción |
| --- | --- | --- |
| Selector | Pill vacía trasladada bajo una sola fila | Cambiaría el mecanismo observado de recorte sincronizado. |
| Progreso | Revelar con clip/width en vez de escalar el gradiente | Cambiaría cómo se distribuye el color durante el crecimiento. |
| Hero | Render raster/imagen de fondo | Esta muestra usa CSS/HTML/SVG; añadir un asset sería una propuesta. |
| Movimiento reducido | Eliminar todas las transformaciones | Puede ser mejora futura; la fuente actual solo cubre parte de ellas. |
| Iconos | Ampliar UiIconSize o usar solo 16/20/24 | Son dos correcciones posibles, ninguna autorizada por esta extracción. |
| Generalización | Reescribir el manual general con esta muestra | Excede la autoridad elegida y podría ocultar reglas de otras superficies. |

## Diferencias, contradicciones y deuda

| ID | Fuentes / condición | Impacto | Resolución documental actual | Estado |
| --- | --- | --- | --- | --- |
| INF-D01 | S08 declara 16/20/24; S01 pide además 14/18/22/28 | Contrato de tamaños no coincide con uso. | Registrar valores usados y tipo declarado por separado. | Abierta; sin TypeScript ejecutado. |
| INF-D02 | S02:63 vs S02:65 | Hover puede ganar al lift active en mouse. | Describir cascada, no asegurar hundimiento uniforme. | Abierta. |
| INF-D03 | S02:103 vs S02:112 | Check done puede recibir la misma tinta que su fondo al hover. | Excepción, no regla deseable de contraste. | Abierta. |
| INF-D04 | Comentario S02:5 / intención S22 vs reglas S02:284–290 | «Todo el movimiento» no describe barras, hover y delays restantes. | Cobertura de reduced motion marcada Parcial. | Abierta. |
| INF-D05 | askBoard en S01 vs catch interno de S21 | Error local puede no mostrarse ante fallo de comando. | Diferenciar rama declarada y alcanzabilidad no acreditada. | Abierta. |
| INF-D06 | S22 describe fuente visible y suma de auto+brazo; S01/S23 ya cambiaron | Historial no coincide con muestra actual. | Prima DOM actual: sin source visible y suma auto+brazo+cubo. | Resuelta para extracción. |
| INF-D07 | Manual global/index, tokens JSON y cabecera tokens.css no presentan versión uniforme | Una etiqueta de versión aislada puede inducir error. | Conservar 0.1.0 propio del dossier, fuentes por archivo. | Fuera del alcance resolver. |
| INF-D08 | Reglas `[data-zone=…]` de S02 sin raíz local | Posible alcance global al importar CSS. | No convertir selectores globales en API recomendada sin futura decisión. | Abierta. |

## Decisiones pendientes

La aprobación de este dossier como especificación reutilizable y cualquier ampliación a formularios, overlays u otras páginas permanecen Pendientes. No se requiere una decisión para terminar la extracción. No se propone una migración automática ni una corrección encubierta.

## Historial

| Versión | Fecha | Decisiones | Superficie | Aprobó |
| --- | --- | --- | --- | --- |
| 0.1.0 | 2026-11-02 | INF-VIS-001–014 | Extracción estática de Infraestructura; documentos, sin implementación | Pendiente |
