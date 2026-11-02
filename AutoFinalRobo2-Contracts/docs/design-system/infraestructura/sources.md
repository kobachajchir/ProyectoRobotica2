# Fuentes y alcance

## Contexto

| Campo | Valor |
| --- | --- |
| Fecha local | 02/11/2026, America/Buenos_Aires |
| Proyecto | D:/ProyectoRobotica2 |
| Fuente canónica elegida | Parte de Infraestructura de la Web |
| Ruta resuelta | `HashRouter` → `/infraestructura` → `InfrastructurePage` con carga diferida |
| Revisión base leída | `3c30707dcd56122bd03c8cce8de87d7d43d25b7b` |
| Estado de trabajo relevante al iniciar | `InfrastructurePage.tsx`, `infrastructureFlow.ts` e `infrastructure.css` ya modificados localmente; también había cambios ajenos de CAD/documentación. |
| Naturaleza de la captura | Lectura estática de archivos; sin render, capturas ni mediciones de estilos calculados |
| Modo | DESTILAR |
| Objetivo escrito | Este subdirectorio documental y ficha AFR2-20261102-11 en Contracts |
| Autoridad | Descriptiva y limitada a Infraestructura; no sustituye el manual general |

La revisión Git identifica la base, no un snapshot íntegro de los archivos locales. Los enlaces de línea son localizadores de la lectura y pueden desplazarse con futuras ediciones. No se han calculado hashes ni ejecutado scripts para fijar o validar la muestra.

## Manifiesto

Todas las rutas de la tabla son relativas a `D:/ProyectoRobotica2/`. `direct` define la muestra; `dependency` explica componentes usados por ella; `supporting` aporta contexto global sin convertir otras páginas en canónicas.

| ID | Archivo | Clase | Incorporado por / aporte |
| --- | --- | --- | --- |
| S01 | AutoFinalRobo2-Web/src/features/infrastructure/InfrastructurePage.tsx | direct | DOM completo, estado local, navegación, magnitudes y estados de datos. |
| S02 | AutoFinalRobo2-Web/src/styles/infrastructure.css | direct | Todos los selectores propios de la sección, media queries y keyframes. |
| S03 | AutoFinalRobo2-Web/src/features/infrastructure/infrastructureFlow.ts | dependency | S01: títulos, iconos, ocho pasos por recorrido y asignación de zona por `place`. |
| S04 | AutoFinalRobo2-Web/src/features/infrastructure/useInfrastructurePackage.ts | dependency | S01: carga inicial, actualización, error, origen y timeout que alteran la presentación. |
| S05 | AutoFinalRobo2-Web/src/components/ui/SegmentedControl.tsx | dependency | S01: copia activa, medición, recorte, activación y teclado. |
| S06 | AutoFinalRobo2-Web/src/styles/segmented.css | dependency | S05: ranura, pill, sombras, estados y responsive. |
| S07 | AutoFinalRobo2-Web/src/components/ui/Button.tsx | dependency | S01: variantes, dimensiones, disabled y spinner. |
| S08 | AutoFinalRobo2-Web/src/components/ui/UiIcon.tsx | dependency | S01/S05: SVG, trazo, tamaño, accesibilidad. |
| S09 | AutoFinalRobo2-Web/src/components/ui/icons/registry.ts | dependency | S08: procedencia Lucide/propia y correspondencia de símbolos. |
| S10 | AutoFinalRobo2-Web/src/components/three/modelAssets.ts | dependency | S01: etiquetas de modelos y rutas, sin recorrer escenas 3D. |
| S11 | AutoFinalRobo2-Web/src/components/three/modelAssetSizes.json | dependency | S01: tamaños declarados de auto, brazo y cubo. No se midieron GLB. |
| S12 | AutoFinalRobo2-Web/src/styles/tokens.css | supporting | S02/S06/S07: claro/oscuro, banner, tipo, radios, tiempos y material del selector. |
| S13 | AutoFinalRobo2-Web/src/styles/index.css | supporting | main.tsx: imports, reset, root, foco, pulsación, animación de ruta y prioridad de teclado. |
| S14 | AutoFinalRobo2-Web/src/styles/workspace.css | supporting | S13: contenedor principal, page-stack, kicker, help y shell responsive. |
| S15 | AutoFinalRobo2-Web/src/styles/headers.css | supporting | S13: contexto de cabeceras compartidas; las cabeceras propias de Infraestructura no usan PageHeader. |
| S16 | AutoFinalRobo2-Web/src/app/providers/ThemeProvider.tsx | supporting | App: clase `.dark`, preferencia system y `data-input` teclado/puntero. |
| S17 | AutoFinalRobo2-Web/src/main.tsx | supporting | Entrada, import de index.css y React.StrictMode. |
| S18 | AutoFinalRobo2-Web/src/app/App.tsx | supporting | Registro de ruta, HashRouter, ThemeProvider y shell. |
| S19 | AutoFinalRobo2-Web/src/app/navigation.ts | supporting | Destino Infraestructura en grupo Explorar, icono board. |
| S20 | AutoFinalRobo2-Web/src/components/navigation/RouteNav.tsx | supporting | Solo shell, elección inicial de sidebar y main que contienen la muestra. |
| S21 | AutoFinalRobo2-Web/src/features/connection/ConnectionContext.tsx | dependency parcial | S01: contrato visual de systemInfo, timestamp, estado ready y refreshSystemInfo; leídos sus puntos relevantes, sin recorrer el protocolo. |
| S22 | AutoFinalRobo2-Contracts/changes/2026-11-02-07-infrastructure-visual-redesign.md | supporting histórica | Intención y evidencia anterior declarada; las fuentes actuales prevalecen. |
| S23 | AutoFinalRobo2-Contracts/changes/2026-11-02-10-infrastructure-cube-board-fs-blender.md | supporting histórica | Cambios posteriores de contenido, cubo y consulta de placa; no sustituye verificación actual. |

También se leyeron las instrucciones raíz/Web/Contracts, el informe maestro raíz y README de Contracts para resolver alcance y política de trabajo; no definen la estética observada.

## Localizadores principales

| Tema | Fuente / localizador al extraer |
| --- | --- |
| Portada, mapa y chips | S01:148–193; S02:20–84 |
| Recorrido, estaciones y ficha | S01:195–230; S02:86–142 |
| Peso, compresión, LittleFS y tabla | S01:233–314; S02:144–220 |
| Glosario, reveal y anchos | S01:53–70 y 316–322; S02:222–292 |
| Estado, cambio y scroll | S01:97–146 |
| Selector | S05 completo; S06:7–109 |
| Paleta y temas | S12:66–102, 230–263, 455–493 |
| Tipografía y espacios globales | S12:118–147, 384–435; S14:177 |
| Reducción de movimiento | S02:284–292; S12:301–318; S13 reglas globales |
| Error de consulta absorbido | S21:440–447 |

## Recursos y exclusiones

SVG locales del registro; sin búsqueda ni descarga de iconos. Familias de sistema con fallbacks; la declaración JetBrains Mono no acredita una fuente descargada. No hay fotografía, vídeo, render raster, logo específico ni escena WebGL dentro del banner de Infraestructura: la ilustración observada se compone de HTML, SVG y CSS. Los modelos aportan etiquetas y tamaños, no se cargan aquí para dibujarlos.

Excluidos: otras páginas como fuente estética, referencia AutoMicro/Radal, CAD, Loader, firmware, servidor ESP y librerías instaladas. Los títulos mencionan herramientas de infraestructura; esta extracción no verifica sus afirmaciones técnicas ni ejecuta esos procesos.

## Cobertura real

| Superficie / estado | Tema | Viewport | Evidencia | Ejecución |
| --- | --- | --- | --- | --- |
| Portada y contenido inicial | Claro/oscuro declarados | Reglas base y 600/840/1200 px | Observada en código | No verificada |
| Selección, hover, active, focus, teclado | Ambos declarados | Reglas de puntero y ancho | Observada en código | No verificada |
| Carga, error, datos locales y ESP | Ambos declarados | Condicionales React | Observada en código | No verificada |
| Ocupación LittleFS con dato | Ambos declarados | Rama numérica/anillo | Observada en código | No verificada |
| Movimiento reducido y contraste aumentado | Ambos declarados | Media queries leídas | Observada en código | No verificada |

No se ejecutaron el resolvedor ni el escáner de la skill: se siguieron imports y selectores manualmente. La prohibición expresa del usuario de tests/validaciones también excluye la línea de base automática y la revisión en navegador sugeridas por la skill. No hay resultados de contraste, rendimiento, compilación, hardware ni accesibilidad práctica.
