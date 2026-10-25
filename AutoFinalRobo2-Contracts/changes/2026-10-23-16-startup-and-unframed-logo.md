# AFR2-20261023-16 — Inicio animado y logo sin contenedor

Pedido humano: agregar la animación inicial de ST Radal y dejar solo el logo en barra e Inicio. Dirección autorizada; se adapta la animación al logo propio de AutoFinalRobo2. Fuentes ST de lectura, sin cambios en AutoMicro ni firmware.

La indicación posterior del usuario precisa además los toast abajo a la derecha. Esta posición sustituye la configuración superior anterior en ambos tamaños de pantalla.

| Módulo | Alcance |
| --- | --- |
| atmega / esp | Sin cambios de firmware, API ni capacidades por esta ficha |
| web | StartupWelcome, logo transparente compartido y Sonner inferior derecho; fuente adaptada, sin build/render de esta revisión |
| shared | Manual0.1.6/VIS-016 y procedencia por archivo, sin validación de ejecución |

## Implementación

StartupWelcome adapta el componente y CSS de ST2026-Radal `335d3ed`: entrada300ms, pausa600ms, salida de logo200ms y velo300ms con desfase de media salida, total1300ms desde imagen lista. Se monta sobre el router una vez por carga de la Web. Usa logo propio, diálogo nativo, Escape/Omitir, STOP disponible, foco contenido/retorno, cierre ante error de imagen y fallback4s. Reduced motion: fade120 sin zoom ni pausa. No simula progreso de carga ni espera hardware.

BrandMark queda sin medallón, borde, fondo ni sombra añadida en barra e Inicio. El PNG original era opaco; `image_gen` integrado produjo `public/brand/Logo-transparent.png` (RGBA1254×1254, esquina alpha0). Es un derivado generativo: no se certifica identidad exacta de píxeles. Se conserva el original en `assets/Logo.png` y `Web/reference/brand/Logo-original.png`; se retira de public para evitar duplicarlo en LittleFS. Favicon comparte el nuevo activo. Referencias ST íntegros en Web/reference/st2026, fuera del código activo.

Sonner conserva un único Toaster, explícitamente bottom-right en escritorio y móvil. Offsets24px/16px con safe areas; margen móvil conserva el rail. Se elimina el cálculo de offset superior con ResizeObserver. Cierre y feedback de operaciones existentes se conservan; resultados críticos dentro de modales siguen inline.

## Procedencia y límites

[Manifest por archivo y prompt](../provenance/web-startup.json), [manual](../docs/design-system/index.md). Skills aplicadas: Apple Design, animate, Visual Identity System, ask-sonner e imagegen. Se preservaron fuentes/autores ST, imagen original y trabajo previo. No se instalaron dependencias ni se modificaron otros repositorios de referencia.

Lectura de fuentes y observación del logo/alpha. Sin compilaciones, tests, validadores, servidor, renderizado Web o ensayos físicos en esta ficha. Los builds anteriores no acreditan esta revisión; dist/paquete ESP no se actualizan. Pendiente revisión en navegador de la introducción, omisión/foco/imagen fallida, contornos del derivado y ubicación de notificaciones en ambos temas y tamaños.
