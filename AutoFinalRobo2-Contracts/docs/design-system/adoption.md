# Adopción

## Aplicación visual global — AFR2-20261102-15

El usuario autoriza aplicar manual 0.1.11 draft a toda la interfaz, solo visual. Fuente Web implementada desde PageHeader/Button/Field/AppShell y visual-language.css, con tokens --visual-* compartidos. Cabeceras, tarjetas, paneles específicos, formularios, navegación y overlays reciben el lenguaje de Infraestructura; flujos y lógica conservados. La composición extendida es Propuesta y la ejecución No verificada. Sin tests, compilación, validadores, navegador ni promoción ESP. Detalle en decisions.md VIS-062 y cobertura15.

## Situación vigente 0.1.11 — ampliación incorporada

02/11/2026 · VIS-057–061 · AFR2-20261102-14. El manual general integra el atlas completo, catálogo de componentes, paletas, valores, movimiento y estados procedentes de Infraestructura. La documentación ya no requiere tratar el dossier como guía separada: queda como evidencia de origen.

**Conservado:** logo vigente, carmesí/rubí y emergencia bermellón, superficies de trabajo, chrome/overlays, campos, primitivas, Sonner, escenarios, navegación, accesibilidad y gestos OLED. **Añadido:** portadas por capas, jerarquía narrativa, zonas cromáticas, flujo visual, estaciones, detalle, magnitudes y esquemas documentales. Sin reforma de código ni cambios en contratos físicos.

| Uso posterior | Acción recomendada | Límite |
| --- | --- | --- |
| Consultar identidad | Empezar por vigencia 0.1.11 en manual.md y catálogo integrado. | Las entradas antiguas son historia con precedencia explícita. |
| Elegir una composición | Distinguir operación inmediata y exploración narrativa. | No usar loops/entradas largas para operación o STOP. |
| Usar selector/botones | Reutilizar SegmentedControl y Button existentes. | No duplicar implementaciones por copiar la portada. |
| Consumir valores | Leer namespaces infrastructure junto a tokens generales. | No son imports nuevos de la Web ni un reemplazo global de CSS. |
| Extender a formularios/overlays | Conservar contratos actuales y especificar solo variantes necesarias. | Ausencia en la muestra no elimina el componente general. |
| Resolver particularidades | Separar tamaños de iconos, hover/active, reduced motion y fallos absorbidos. | Documentadas como deuda; sin correcciones de código en esta entrega. |

La integración documental está autorizada y completada. Aprobar estéticamente una aplicación futura o escoger qué pantallas reformar son decisiones distintas, sin bloquear este entregable. No se añade una obligación de pruebas/validaciones por defecto; solo se realizarán ante pedido expreso.

Las secciones siguientes conservan el historial de adopción, incluido el cierre inicial 0.1.3; sus frases «interfaz aún sin aplicar» o «esqueleto JS» describen aquel momento y no el estado vigente. Las evidencias históricas no cubren 0.1.11.

## Historial de adopción conservado

Estado: plan visual cerrado documentalmente en versión `0.1.3` (`status: draft` hasta verificación en ejecución); interfaz aún sin aplicar. Web actual conserva esqueleto JS. Sin `npm install`, servidor, build o tests en esta entrega.

1. Manual `0.1.3` validado documentalmente, cotejado al 100% con `D:/ProyectoRobotica2/informe-portabilidad-glassmorphism.md` (§§1–18) e integrado con el activo oficial `D:/ProyectoRobotica2/assets/Logo.png` y la paleta de acento carmesí/rubí (`VIS-001`..`VIS-013` aprobadas como línea base).
2. Consolidar tokens y primitivas compartidas (`ThemeProvider` `light/dark/system` con clave `afr2-theme`, `BrandMark` con `Logo.png` y fallback `AFR2`, `GlassSurface` con 6 densidades, escala `z-index` `0..110` y servicio de notificaciones `notify` sobre Sonner). Mantener un único `Toaster` en raíz (`top-right`/`top-center`).
3. Migrar el proyecto a TypeScript, instalar dependencias propias, incorporar `Logo.png` en los activos de `AutoFinalRobo2-Web` y portar inicio/layout y conexión como piloto. El plan aprobado autoriza `npm install` y Vite `start` al iniciar implementación; no autoriza por sí mismo tests/builds/validadores.
4. Portar páginas y modelo anterior, sin API histórica activa, sin barra de marca de 5 px ni simulación de sensores.
5. Continuar cliente, visor OLED y empaquetado/copias del plan Web. Registrar contratos propuestos y productores pendientes.
6. Cuando se solicite, verificar ambos temas y móvil/escritorio, teclado (`0ms`), diferenciación entre acciones carmesí/rubí y `STOP` bermellón, vacío/error/carga por alcance, `prefers-reduced-motion` (`120ms` fade), `prefers-reduced-transparency`/`prefers-contrast: more` (`surfaceSolid`), contraste real y gestos. Teléfono/placa requieren ensayo expresamente pedido; no afirmar acabado físico por inspección.

Escáner de deriva sin ejecutar. `scanPaths` vacío hasta que existan y se acuerden los archivos objetivo; no preautoriza todo el repositorio. Firmware, CAD y AutoMicro fuera de la adopción visual. Deuda posterior al cierre documental: revisión visual real en navegador, presupuesto LittleFS (incluyendo el peso de `Logo.png` optimizado si se sirve desde el ESP) y productores ESP/ATmega.

## Plan visual cerrado; ejecución retenida hasta orden de implementación

Antes de implementar: leer `index/manual/tokens/components/decisions` y `PLAN_WEB`; usar versión `0.1.3` uniforme. Primera entrega de código futura: TypeScript, `ThemeProvider`, `BrandMark`, primitivas compartidas y Sonner, Inicio/Conexión; después OLED/3D y páginas. No instalar ni iniciar nada como parte de este cierre documental.

Criterios de cierre documental cumplidos: JSON parseable, metadatos y decisiones `VIS-001`..`VIS-013` cerrados, versión `0.1.3` uniforme, referencias internas resueltas, cotejo íntegro con `D:/ProyectoRobotica2/informe-portabilidad-glassmorphism.md`, registro de `D:/ProyectoRobotica2/assets/Logo.png`, mínimos de contraste matemático para pares declarados y separación estricta entre cierre documental y verificación en ejecución (`No verificada`). Evidencia en [validation.json](validation.json) y [VALIDACION.md](VALIDACION.md).

## Entrega 0.1.5 — VIS-015

Adaptación en fuente del sidebar y material del buscador de ST2026-Radal 335d3ed, compartida por toda la Web. Home usa ilustraciones propias de sección; OLED/Ajustes usan FloatingModal. El emisor manual se cancela al abrir modal y ante STOP global. La ruta OLED ya no expone el antiguo snapshot simulado como estado remoto. Fuente de referencia íntegra en Web/reference/st2026, inactiva.

No se instalaron dependencias ni se ejecutaron builds, validadores, tests, servidor o renderizado. La comprobación documental histórica 0.1.3 no cubre esta entrega. Primero revisar en navegador bajo autorización; luego build/empaquetado/copia ESP si se solicita. Productor OLED y perfil semántico requieren cierre ESP/AVR; capacidades continúan pendientes.

## Entrega 0.1.6

StartupWelcome bajo ConnectionProvider y por encima de HashRouter; BrandMark/favicons usan el PNG transparente generado. Original archivado fuera de public. Sonner bottom-right sustituye offsets superiores. Sin dependencias, compilaciones, validadores, servidor ni pruebas. Pendiente observar introducción/omisión/errores, foco y notificaciones en claro/oscuro/móvil bajo autorización.

## Entrega0.1.7

Fuente preparada para intro más fiel, notificaciones sin truncado y mapa. Lectura de robot.state consolidada en ConnectionProvider para modo y mapa; perfil opcional afr2-pose-1 diseñado y consumidor escrito, productores pendientes. No se instalaron dependencias ni se modificaron recursos binarios/firmware. Sin builds, tests, validadores o renderizado. Próximo: revisión visual autorizada y acuerdo de pose/odometría ESP–ATmega antes de afirmar mapa físico en vivo.

AFR2-20261025-02/VIS-026: aplicar identidad existente al táctil local y ruedas de mapas; no nueva marca. Fuente compilada, revisión visual/gestos/ambos temas/teléfono pendiente. [Cobertura](coverage-20261025-02.json).
