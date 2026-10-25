# Web flotante — AFR2-20261023-14

Objetivo: navegación visible, una tarea principal por sección y menos controles simultáneos. Pedido humano actual autoriza adaptar el manual 0.1.4 y los componentes concretos de ST2026-Radal a esta dirección, sin esperar una nueva aprobación del borrador.

## Fuentes y alcance

- Fuente directa: `D:/ST2026-Radal`, rama `codex/wa-pilot-finish`, revisión `335d3edb2f5f39457ae82c82ded6f0403bbbec65`.
- Componentes inspeccionados: `src/presentation/components/layout/app-shell.tsx`, `styles/app-shell-sidebar.css`, `components/catalog/public-catalog-search.tsx`, `styles/public-catalog-search.css` y `styles/radal-floating-controls.css` (rutas relativas a `src/presentation/`). Copias históricas sin modificación en Web/reference/st2026; no participan del build.
- Referencias visuales aportadas: `1000917115.jpg` (superficies claras, profundidad y acento) y `1000917116.png` (iconos frente a nombres en navegación). Son evidencia visual, no instrucciones ejecutables ni requisitos de login.
- Skills aplicadas: apple-design, emil-design-eng, visual-identity-system, animate y afr2-screen-authoring. Modal nativo como el buscador solicitado; no se incorpora otra librería para sustituirlo. Se conserva Sonner único para notificaciones; lectura periódica y errores del visor son inline.
- ST y AutoMicro son fuentes de lectura. No se modifican ni se ejecutan sus scripts. No se importa autenticación, comercio, Firebase ni permisos al árbol activo.

## Componentes adaptados

| Before | After | Why |
| --- | --- | --- |
| Barra fija sin iconos ni colapso | AppShell de ST adaptado: iconos, nombres, selección y preferencia de colapso | Orientación estable y espacio disponible |
| Navegación móvil horizontal | Rail lateral siempre visible; expansión sobre el contenido en móvil | Cumple la persistencia solicitada |
| Conexión desplegada dentro de la página | Ajustes en FloatingModal compartido | Conserva la tarea y simplifica cabecera |
| OLED local con estado de ejemplo y navegación optimista | Visor remoto compartido, vacío cuando falta productor | No atribuye una simulación al robot |
| Tarjetas sin representación de sección | Texto a izquierda, ilustración simbólica a derecha y acento propio | Jerarquía y reconocimiento sin ruido |
| Datos de modo ausentes | Modo recibido de robot.state, con vencimiento | No confunde una selección local con modo activo |

`RouteNav.tsx` adapta la composición y persistencia de AppShell. Se conservan los conceptos de `SIDEBAR_STORAGE_KEY`, `isSidebarCollapsed`, `sidebarCollapsed`, NavLink activo y nombres accesibles. Diferencias intencionales: rail móvil persistente, sin permisos comerciales y geometría de colapso inmediata. ST anima width/padding a 240ms; aquí la reorganización no anima layout. El modal sí conserva el material y tiempos del buscador.

`FloatingModal.tsx` adapta el estado closed/opening/open/closing, requestAnimationFrame, salida diferida, close nativo, retorno de foco, bloqueo de scroll y material expandible del buscador. La geometría se generaliza de una barra horizontal a un panel central mediante translate X/Y + clip-path. Apertura 240ms, salida 200ms, backdrop 180ms, contenido 100ms con 80ms de demora, curva existente ease-in-out de ST. Escape y teclado son inmediatos; reduced motion usa fade de 120ms, sin desplazamiento ni clip. La salida puede revertirse sobre la superficie. El cuerpo queda inert durante la salida; STOP y cierre permanecen accesibles en la cabecera sticky.

La profundidad se reserva a sidebar, ilustraciones, tarjetas accionables y modales. Las superficies de trabajo son sólidas; no se apilan blurs por control. Claro/oscuro, reducción de transparencia y contraste usan los tokens del producto. Las ilustraciones SVG son símbolos de sección, no representación mecánica del chasis. Modelo GLB/Draco y logo se conservan.

## Estado y operación

La cabecera muestra sección, modo recibido y conexión Web–ESP. El estado ESP–ATmega y el responsable del control siguen separados en Ajustes. No se usa lease ni botón seleccionado para deducir modo.

El botón superior de la barra abre OLED en un modal desde cualquier ruta. Cerrar restaura el foco y conserva la página actual. La ruta `/oled` usa el mismo visor y pausa su instancia cuando hay un modal, evitando dos consultas OLED activas. La navegación normal conserva las secciones por tarea del cambio 10. Abrir un modal cancela un hold de Control y solicita STOP; cualquier STOP cancela también el emisor periódico local. El resultado físico sigue dependiendo del callback AVR.

## Verificación y pendientes

Lectura de código y de las dos imágenes, revisión de fuentes preparadas y registro de hashes. No se ejecutaron tests, compilaciones, validadores, servidor, renderizado en navegador ni ensayos físicos. El build/paquete ESP anterior no representa estos cambios.

Pendiente con autorización de verificación: observar escritorio/móvil y ambos temas, foco/Tab/Escape, abrir/cerrar/revertir, scroll de modal con STOP, orientación y texto ampliado. Comprobar reconexión, rechazo de capacidad, snapshots inválidos/caducados y exclusión corto/largo cuando exista un productor o banco de pruebas autorizado. Ajuste estético final pendiente; no afirmar paridad OLED física por lectura de fuente.
