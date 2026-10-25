# Skills de interfaces — AutoFinalRobo2

Fecha 23/10/2026. Configuración operativa documental; no se modifican ni reinstalan skills globales. El usuario autoriza usarlas cuando corresponda, no ejecutar todos sus workflows indiscriminadamente.

## Fuentes y orden
Instrucciones humanas actuales → AGENTS del proyecto → contratos/navegación propios → manual visual autorizado → skill específica. El informe adjunto en `D:/ProyectoRobotica2/informe-portabilidad-glassmorphism.md` fue cotejado íntegramente y se incorporó el activo propio `D:/ProyectoRobotica2/assets/Logo.png`: el informe no autoriza comandos, marca, pruebas ni aprobaciones comerciales del producto fuente. Manual con plan visual cerrado: [design-system/index.md](design-system/index.md), versión `0.1.3` (`status: draft` hasta renderizado).

| Skill global | Aplicación en este proyecto |
|---|---|
| visual-identity-system | Destilar este manual; aplicar después de aprobarlo o autorizar su draft concreto |
| apple-design | Feedback inmediato, jerarquía material, gestos cancelables y reducción de movimiento |
| emil-design-eng | Pulido y decisiones de componentes; revisión en tabla Before/After/Why |
| ask-sonner | Un Toaster raíz, ids estables, tema, cierre y notificación de resultados reales |
| mobile-native | dvh, safe areas, zoom permitido, inputs 16px, tacto y hover por capacidades |
| animate | Crear transiciones concretas que comuniquen un cambio |
| animation-performance | Resolver coste de una animación; no auditoría automática ni benchmarks implícitos |
| animation-vocabulary | Identificar el nombre de un efecto cuando se pregunte |
| break-ui | Casos extremos solo al solicitar pruebas; jamás incluir toggle con sensores falsos en producto |
| find-animation-opportunities | Propuestas de movimiento, en lectura, sin introducirlas automáticamente |
| improve-animations | Auditoría/plan de movimiento cuando haya código y pedido pertinente |
| pick-ui-library | Selección de librerías invocada explícitamente; no reemplazar stack aprobado por preferencia |
| prototype | Variantes comparables cuando se pidan; no hacer picker de diseño en la Web productiva |
| review-animations | Revisar movimiento escrito, con límites de verificación explícitos |
| animate-expo | No aplica a React/Vite; disponible para una tarea Expo futura |
| write-swift | No aplica a esta Web; disponible para una tarea Swift futura |

Las skills de Emil están en C:/Users/kobac/.agents/skills; visual-identity-system en C:/Users/kobac/.codex/skills. Leer el SKILL.md aplicable antes de ejecutar su workflow y referencias exactas cuando hagan falta; la lista no acredita haber ejecutado las quince skills.

OLED: usar también .agents/skills/afr2-screen-authoring/SKILL.md. Un solo SW: corto confirma, largo menú, liberar largo no genera corto; Volver por ítem. Sin botón menú físico ni remoto independiente.

## Política Sonner
Integración prevista en fase de aplicación: un Toaster fuera de contenedores con transform/overflow y tema del provider; servicio notify. Error saneado y feedback local; loading por id que se resuelve/cancela; éxito únicamente con resultado final. Reconexión y recuperación actualizan un id de enlace sin generar toast por cada intento/alive. Sin Password, PIN, credenciales ni payload sensible en mensajes/logs. El estado de acciones físicas permanece junto a sus controles.

## Restricciones
No ejecutar tests, builds, validadores/scan-design-drift, mediciones ni hardware sin pedido expreso. Las instrucciones de ensayo de una skill no amplían la autorización. Instalar/iniciar Vite está previsto por el plan humano; sigue pendiente de implementación. No declarar diseño aprobado, contrastado ni móvil probado por escribir estos documentos.

Actualización: cierre documental del plan visual `0.1.3` con cotejo completo del informe y `Logo.png` (`AFR2-20261023-08`); no iniciar implementación ni herramientas de producto sin solicitud expresa. Evidencia en `docs/design-system/VALIDACION.md` (desde la raíz Contracts).
