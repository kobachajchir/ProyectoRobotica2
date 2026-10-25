# Decisiones visuales — 23/10/2026

Todas afectan el objetivo Web. Alternativa de partida: lenguaje instrumental cian de AutoMicro con paneles más sólidos; la especificación adopta el informe de transferencia para material/temas/movimiento y el activo propio `D:/ProyectoRobotica2/assets/Logo.png` para la identidad carmesí/rubí sobre vidrio neutro. Cerrar el plan visual documental habilita su implementación cuando se solicite, sin confirmar experiencia en ejecución.

| ID | Categoría y regla | Evidencia | Aprobación | Fuente/razón | Consecuencia |
|---|---|---|---|---|---|
| VIS-001 | Estado y operación primero | Inferida | Aprobada | Manual AutoMicro, adaptado a robot actual | Orden compartido por páginas |
| VIS-002 | Vidrio en paneles y densidad por tarea | Confirmada en dirección; Propuesta en escala | Aprobada | Informe §§2, 6, 15 y pedido humano | Evitar blur anidado; fallback sólido accesible |
| VIS-003 | Base de temas neutral en claro/oscuro, sin marcas de terceros | Confirmada en exclusión; Observada en fondos/textos | Aprobada; acento precisado por VIS-013 | Informe §§3, 4, 15 y pedido humano | Sustituye cian heredado; base neutra para el vidrio |
| VIS-004 | Radio 14/24/32, ritmo 4–48, texto 16 | Observada en radios/controles; Propuesta en escala | Aprobada | Informe §5, Mobile Native | Tokens comunes; lectura y tacto |
| VIS-005 | Movimiento causado, curvas del informe y teclado inmediato | Observada en tokens/matriz; Propuesta en política única | Aprobada | Informe §§6, 9, 10, 12 y Apple/Emil | Teclado inmediato, reduced motion de 120ms sin conflicto |
| VIS-006 | Sin auth/Studios, SW único | Confirmada | Aprobada | Pedidos humanos en este chat | Precede reglas genéricas de gestos |
| VIS-007 | Sonner con Toaster único y resultado real | Confirmada para librería; Propuesta para receta | Aprobada | Pedido humano, Informe §8.3 y ask-sonner | Sin spam, sin secretos, feedback local conservado |
| VIS-008 | Móvil, teclado y estados extremos | Propuesta | Aprobada | Informe §10, Mobile Native y manual AutoMicro | Verificación física pendiente |

## Validacion documental 0.1.1

Fecha 23/10/2026. Alcance: manual/tokens, no Web. Evidencia: cálculo matemático de los pares listados en `validation.json`, sin render. Historial `0.1.0` en `AFR2-20261023-06`; revisión `0.1.1` en `AFR2-20261023-07`.

| ID | Regla y alternativa | Evidencia | Aprobación | Razón y consecuencia |
|---|---|---|---|---|
| VIS-009 | Éxito claro `#087a46` y `borderStrong` `.55`/`.50`; alternativa `#0f8c53` (4,18:1) y borde `.28`/`.20` descartados para texto/controles | Propuesta | Aprobada | Superar 4,5:1 en texto y 3:1 en bordes de control sobre las superficies declaradas |
| VIS-010 | Sonner custom con cierre explícito (`toast.dismiss(id)`) y posición superior (`top-right`/`top-center`) | Propuesta | Aprobada | Evitar que headless pierda cierre y que el toast inferior derecho de origen tape `STOP` |
| VIS-011 | Semántica de links/tabs/menu y teclado separada de OLED | Propuesta | Aprobada | Eliminar ambigüedad Enter/Space y navegación; sin cambiar wire |

## Cotejo íntegro del informe, identidad carmesí/rubí y logo propio (`0.1.2` → `0.1.3`)

Fecha 23/10/2026. Con `D:/ProyectoRobotica2/informe-portabilidad-glassmorphism.md` completo (815 líneas, §§1–18) y `D:/ProyectoRobotica2/assets/Logo.png` aportado por el usuario, se cierra el plan visual en versión `0.1.3` (`AFR2-20261023-08`).

| ID | Regla y alternativa | Evidencia | Aprobación | Razón y consecuencia |
|---|---|---|---|---|
| VIS-012 | Reconciliación íntegra con §§6–15 del informe: curvas (`cubic-bezier(.23,1,.32,1)`, `(.2,.75,.25,1)`, `(.77,0,.175,1)`, `(.32,.72,0,1)`), tiempos (`150/200/300ms`, modal `250ms`, drawer `240ms`, segmentado `250ms`, acordeón `200ms`, tooltip `150/280ms`, spinner `800ms`), fade `120ms` en reduced motion, capas `z-index` `0..110`, `surfaceSubtle`, 3 alcances de carga y exclusión de barra de 5px y entradas editoriales largas | Observada / Propuesta | Aprobada | Fidelidad completa al informe de transferencia sin heredar el reset `.01ms !important` contradictorio ni demorar controles del robot |
| VIS-013 | Identidad gráfica propia desde `D:/ProyectoRobotica2/assets/Logo.png` y acento carmesí/rubí sobre vidrio neutro: `action` `#b81d33` (hover `#961426`, `onAction` `#ffffff`, `focus` `#b81d33`, `selection` `rgba(184,29,51,.10)`) en claro; `action` `#ff6b7d` (hover `#ff8594`, `onAction` `#0b1525`, `focus` `#ff6b7d`, `selection` `rgba(255,107,125,.14)`) en oscuro. Diferenciación de `STOP`/`error` con rojo bermellón (`#c93b1b` claro / `#ff8a65` oscuro), contenedor `surfaceSolid`, borde e icono explícito. Insignia `BrandMark` (`36×36 px` header / `64×64 px` Inicio) con `Logo.png` sobre medallón `#000000` y fallback monograma `AFR2` | Confirmada en dirección y logo; Propuesta en valores exactos | Aprobada | Incorpora el logo Mecanum rojo y dota de identidad propia a acciones/foco/marca sin teñir el vidrio neutro ni confundirse con `STOP` |

El plan visual queda **cerrado y autorizado como especificación `0.1.3`**. El estado global del artefacto `tokens.json` / `index.md` permanece `draft` hasta que la interfaz sea implementada y verificada visualmente en ejecución (`verification: "No verificada"`).

## Reorganizacion por tareas 0.1.4

VIS-014 — 23/10/2026. Fuente: pedido humano explícito de reformar toda la interfaz Web con Apple Design, menos densidad y secciones como AutoMicro. Dirección Confirmada y autorizada para implementación; valores concretos Propuesta, aprobación estética final Pendiente, verificación en ejecución No verificada.

Regla: navegación lateral en escritorio y horizontal en móvil; portada de seis destinos; una tarea por panel, detalles cerrados al inicio y separación de 32px. Control separa Movimiento/Prueba/GOTO/Calibración; Wi-Fi Estado/Buscar/Configurar; Sensores Entorno/Ruedas. Conexión/apariencia se revelan a pedido. Documentación abre solo un tema; OLED mantiene visor/SW como área principal y catálogo/datos/árbol en detalles. 3D prioriza escena y pliega procedencia. Borradores viven en la página, fuera del panel desmontado.

Material: conservar vidrio/temas/acento/logo, suprimir halos de fondo y sombras de panel/dense. Tipografía de títulos usa familia de cuerpo y peso 600. Valores de workspace en tokens.json y CSS compartido. Alternativa descartada: mostrar formularios/pad/GOTO y diagnóstico juntos en grids densos. Consecuencia: más navegación explícita, menor contenido simultáneo. No se agregan comandos ni autenticación.

Seguridad de navegación: cambiar tarea detiene hold manual antes de cambiar panel; salir de Control elimina su intervalo y solicita STOP si había hold. STOP permanece en la cabecera sticky en todo viewport. Toasts se desplazan bajo ella (112px escritorio/96px móvil) para no cubrir su acción. Estas son adaptaciones de la UI, no pruebas físicas de parada.

## VIS-015 — Sidebar y modales de ST, 0.1.5

Fecha 23/10/2026. Dirección Confirmada y autorizada por el pedido humano actual; patrón de sidebar y buscador Observada en ST2026-Radal 335d3ed; dimensiones/ilustraciones Propuesta con detalle estético Pendiente. Verificación en ejecución No verificada. Objetivo exclusivo AutoFinalRobo2-Web; fuente ST de lectura.

Reglas: rail lateral fijo/flotante y persistente, iconos al colapsar/nombres al expandir, OLED arriba como modal global y selección de ruta. Superficies operativas sólidas, acento propio y profundidad discreta; glass reservado a chrome, overlays y flotantes. Tarjetas con texto a izquierda e ilustración simbólica derecha. FloatingModal común adapta PublicCatalogSearch: diálogo nativo, material translate/clip, 240/200ms easeInOut, backdrop neutro 24px/180ms, contenido 100ms/80ms; teclado inmediato y reduced fade 120ms. La curva y tiempos observados se conservan; origen X/Y y panel centrado son extensión Propuesta.

Sustituye VIS-014 solo en navegación móvil horizontal, ancho fijo 228px y sombras de tarjetas accionables; sustituye VIS-012 solo en receta modal 250ms/scale .96. La geometría de sidebar cambia inmediatamente (la fuente ST anima width/padding); se evita animar layout. Modales conservan contenido hasta acabar salida, foco nativo, Escape y retorno, fondo inert y cierre con puntero reversible. STOP dentro de cada modal y cabecera sticky; Sonner queda debajo de la cabecera medida con ResizeObserver. Polling OLED no genera toasts ni simula resultados.

Consecuencia: identidad propia, sin colores comerciales ST, login, permisos ni dependencias nuevas. Estado real/ausencia se conserva; ui.action no predice navegación. La implementación no valida funcionamiento de firmware, fluidez o contraste en pantalla. Fuentes y hashes en provenance/web-floating-ui.json.

## VIS-016 — Introducción ST, marca libre y toast inferiores, 0.1.6

Fecha 23/10/2026. Dirección Confirmada y autorizada por el usuario. Secuencia Observada en ST2026-Radal 335d3ed (`startup-welcome.tsx/css`, montaje `src/app/App.tsx`). Logo propio Confirmado; derivado/tamaños44/80px Propuesta, revisión estética Pendiente. Verificación Web No verificada.

Reglas: BrandMark compartido sin medallón, borde, fondo ni sombra añadida. El PNG original es RGB con fondo negro opaco, por lo que se genera un derivado RGBA sin fondo mediante image_gen. Se preserva el original íntegro en assets/Logo.png y Web/reference/brand/Logo-original.png; no se promete identidad exacta de píxeles de un derivado generativo. El favicon usa el mismo derivado; el PNG opaco deja public para evitar empaquetar dos versiones.

StartupWelcome se monta una vez por carga completa, fuera de rutas: entrada300ms scale(.97→1), pausa600ms, salida de logo200ms y velo300ms con desfase de media salida (total1300ms tras imagen lista). Propósito: reconocimiento de marca en entrada poco frecuente. CSS opacity/transform con easeEnter existente; sin dependencias nuevas. Escape/Omitir inmediatos, dialog nativo, foco contenido/retornado y STOP accesible. Error de imagen libera; fallback4s; reduced motion fade120 sin pausa/escala. No representa progreso ni espera hardware.

Sonner único queda explícitamente en bottom-right para escritorio y móvil. Offset24px en escritorio/16px móvil con safe areas; margen izquierdo móvil respeta el rail. Se elimina ResizeObserver y --afr2-toast-top, que solo servían a su posición anterior. STOP permanece arriba y los resultados críticos dentro de modales siguen inline. No se duplican Toaster ni animaciones externas.

Sustituye VIS-013 solo en forma/material de marca; VIS-012 solo en exclusión del inicio solicitado; VIS-010/014/015 solo en ubicación/offset de notificaciones. No cambia API, firmware ni tiempos de los modales operativos. Fuente adaptada sin compilar/renderizar/probar.

## VIS-017 — Velo fiel a Radal, toast legibles y mapa

23/10/2026. Confirmada: pedido humano de quitar la sensación de card del inicio, arreglar recortes e incorporar mapa por vistas. Observada: Radal usa velo completo con halo/sheen y logo libre; Sonner2.0.8 limita el ancho móvil con un cálculo simétrico y el título anterior usaba truncate. Propuesta: composición y chasis esquemático del mapa; estética final Pendiente. Verificación de ejecución No verificada.

Reglas: restaurar mezcla28%/halo/sheen, logo min(76vw,32rem), superficie nativa sin borde/radio/sombra de panel. Logo decodificado antes de iniciar secuencia; Escape/Omitir y STOP como controles discretos sin tarjeta. Conservar300/600/200/300ms, reduced fade120 y fallback4s.

Toast: portal en document.body, contenedor con ancho explícito min(400px,viewport-márgenes), item width100% y offsets coherentes. Títulos/descripciones pueden envolver palabras largas; sin truncate. Una notificación visible por vez, expandida, cierre aparte del texto; contenido excepcionalmente largo con scroll propio. Esto sustituye la pila de3 y margen izquierdo móvil de rail que Sonner descontaba dos veces. Sigue bottom-right; sin animador externo.

Mapa: superficie sólida/grilla, botones de cámara y zoom accesibles. Superior SVG disponible sin WebGL; Perspectiva/Seguir auto Three.js lazy/frameloop demand con fallback. Sin animación de pose por decoración ni integración de consignas. Datos válidos recibidos únicamente, huecos de trazo segmentados, edad y unknown explícitos. Chasis de referencia no implica CAD validado. Fuente/valores en tokens y contrato de mapa; firmware sin cambios.


## VIS-018 — Avisos cerrables, controles OLED siempre visibles, giro gráfico y secuencias, 0.1.8

23/10/2026. Confirmada: pedido humano de implementar en Web avisos cerrables en cabecera y modal, controles OLED siempre visibles con estado largo retenido hasta soltar, controles gráficos de giro sobre el eje y editor/simulador de secuencias Mecanum con cliente físico. Observada: disposición e iconografía de `ScreenStreamWorkspace` y bloques/líneas de tiempo de `TimelineRow`/`MotorBlock` en la base de referencia `AutoMicro-Web`. Propuesta: adaptación a superficies sólidas `--surface-solid`, acento carmesí/rubí `--action` y cinemática Mecanum de cuatro ruedas sin dependencias de PIN, balance ni tracción diferencial. Verificación en ejecución No verificada.
## VIS-019 — Control coherente, Sistema y Desarrollador,0.1.9

24/10/2026. Confirmada: dirección del pedido humano y autorización de aplicar el
borrador propio. Observada: títulos transformados por selector CSS amplio,
geometría aproximada de cajas, propiedades laterales y datos de desarrollador
en ST Radal. Propuesta: ritmo24px/20px, detalles bajo la pista y nueva composición
de Sistema/Desarrollador; estética final Pendiente. Compilación Web completada;
verificación visual No verificada: el navegador denegó localhost, sin elusión.

Sustituye VIS-018 en propiedades lateral/modal y selector Simulación/Robot.
Sustituye persistencia incondicional de STOP de VIS-014/015 por disponibilidad
durante operación o parada incierta, manteniendo la misma acción accesible en
diálogo cuando el documento queda inert. Conserva paleta/tokens de identidad,
logo WebP, superficies sólidas y navegación OLED. Chevron SVG común gira180°;
texto estable,180ms easeOut, teclado y reduced motion sin transición.

No se incorpora Radal como autoridad de color/material, no se instala librería
nueva ni se modifica CAD. Fuentes/hashes en provenance/control-system-sequence-20261024.json.
Cobertura: Control/editor/disclosures/selects/settings/Inicio/Desarrollador/Docs.
El cálculo preciso del suelo conserva metros y el GLB. No certifica cotas físicas.


## VIS-020 — GOTO local y recorrido ordenado

24/10/2026, AFR2-20261024-10. Pedido humano y aclaración de disposición autorizan
mapa ancho completo y X/Y/rumbo/tiempo en fila, lista inferior y controles a la
derecha (móvil apila). Acento solo al comando actual, texto de estado adicional;
scroll acompaña automáticamente y se libera al tomarlo manualmente. Estado
local de simulación separado del envío físico y sin feedback inventado.

Skill animate: State indication/Spatial consistency durante avance automático,
fade con capas opacity140ms ease y FLIP transform220ms easeInOut, tokens vigentes.
Interrupción conserva posición; reduced motion/teclado sin transform y fade120ms
suave con reduced motion. Sin animar alto/margen/color ni añadir Motion. Reusa
paleta carmesí/rubí, superficies/primitivas,44px y ritmo24/20px. Layout/dirección
aprobados por pedido; ajuste visual/tacto No verificados por denegación previa
del navegador. No actualiza identidad de marca ni afirma validación estética.

## VIS-021 — Hardware agrupado y simulador compacto (12)

Dirección Confirmada por los pedidos humanos de esta tarea; composición/valores
Propuesta, basada en tokens/primitivas Observados del borrador0.1.9 ya autorizado.
Verificación visual No verificada. Alcance Web de Control/GOTO y ajustes.

Información de hardware contiene cuatro bloques verticales (conexión, ESP,
ATmega, arranques); fecha/hora y Actualizar en una fila interior. Reinicio es
el otro desplegable principal. Apariencia usa botones, sin selector de control.
En GOTO, mapa y fila actual comparten columna; X/Y/rumbo/tiempo se apilan en
barra derecha230px, junto al reproductor. En ancho<620px se apila esa barra.
Mapa220..300px según alto; lista cerrada160px, fila72px con medias vecinas,
sin scrollbar. Recorrido local y detalles secundarios se pliegan para reservar
espacio al mapa y al movimiento. No se afirma que todo quepa en toda pantalla.

Pista seleccionada usa acento/borde de tokens actuales; botones FL/FR/RL/RR
separados, papelera44px junto a Editable con margen8px por lado. Eliminación
usa FloatingModal existente, animación interrumpible/teclado/reduced motion.
Carrusel conserva FLIP220ms y fade140ms; abrir gira solo el chevron180ms,
sin transform con reduced motion. No dependencias, colores ni marca nuevos.
Compilación completada; revisión visual/interactiva sigue pendiente, sin
reintentar la superficie de navegador que previamente denegó localhost.

## VIS-022 — Control táctil inmersivo (13)

Dirección Confirmada por el pedido humano; composición Propuesta y tokens
Observados del borrador0.1.9 autorizado. Objetivos: Movimiento, nueva superficie
de gesto y variante fullscreen del dialog compartido. Auto final centrado grande,
con encuadre de cámara; sombra y elevación fija, sin flotación continua ni pose
física inventada. Flechas visibles solo durante input, acento al sector elegido.
X izquierda, título centrado, dirección/velocidad debajo. Botón junto al título
Conducción manual, alineado verticalmente, encima de parámetros/modo.

Entrada ocasional: Spatial consistency; revelado del material240ms/salida200ms
existentes. Opacity de flechas140ms easeOut. Gesto funcional frecuente: respuesta
inmediata, sin spring/bob/inercia que continúe una consigna al soltar. Teclado
inmediato/reduced motion sin desplazamiento. Safe areas/dvh y touch-action:none
solo en control; global zoom preservado. Rotar/resize cancela. STOP contextual
dentro del dialog nativo por accesibilidad del resto inert. No nueva paleta,
tipografía, dependencia o activo raster. Cobertura fuente, visual No verificada.

## VIS-023 — Perfil y biblioteca OLED en árbol (14)

Dirección Confirmada por pedido humano; composición Propuesta con tokens propios
Observados del borrador0.1.9 autorizado. Perfil: nombre/copy sin márgenes heredados,
ritmo12/24px, logo/copy centrados y enlaces44px; Volver al inicio con flecha antes.
Hardware/Reinicio solo en Ajustes. Árbol por padres, avisos en grupo contextual,
selección accent y contenido local separado del framebuffer conectado. Se reusa
la estructura1..3/flechas SVG de AutoMicro ScreenStreamWorkspace, adaptada a
giros AFR2 con captura, sin portar sus comandos ni controles USER/Menú/Atrás.
Chevrón de rama cambia orientación; reduced motion/teclado sin transición.
Layout apila árbol/biblioteca en ancho reducido y conserva botones44px laterales.
No nuevos colores/recursos/dependencias; visual/interoperabilidad No verificadas.

## VIS-024 — Diagnóstico y configuración conectados (15)

Pedido humano de alineación del maestro. Dirección Confirmada; composición
Propuesta sobre borrador0.1.9 autorizado. Se reutiliza Hardware/Disclosure/dl,
sin nueva paleta ni controles de control. LittleFS/paquete muestra valores reales
o desconocidos; hash declarado identificado. Calibración conserva sus botones,
solicita lease al escribir e informa consulta necesaria. No aceptación visual,
interacción móvil, tests o datos simulados. Cobertura15 separa fuente de runtime.
## VIS-025 — Control directo sin autorización previa (25/01)

Dirección Confirmada por pedido humano explícito. VIS-024 no obliga a adquirir
control: esa conducta se retira en Web/ESP/AVR. Se eliminan opciones y estados
de acceso; acciones directas con supervisión automática. OLED/C/TS conserva
59identidades, Volver y recursos; control4/sistema7. Sin nueva paleta, layout
gráfico o dependencia. Manual0.1.9 conservado, composición Propuesta, visual
No verificada; compilación y aceptación se separan.

## VIS-026 — Táctil local, superior y ruedas (25/02)

Dirección Confirmada por pedido humano; composición Propuesta sobre manual0.1.9
autorizado. Aplicación en ManualDriveOverlay/Model, RobotModel y mapas locales.
Se reutilizan UiIcon arrow, superficies sólidas y tokens de acento/borde/texto;
se reserva una franja para modo y se conserva ritmo8/12/16/24 y controles44px.
Frente arriba, texto inferior inmediato y acento por dirección; aparición por
opacity sin animar layout. Ruedas son simulación funcional, sin inercia al soltar
ni pose física inventada. reduced motion elimina giro/fade secundario; teclado
conserva respuesta inmediata. Sin nueva marca/paleta/dependencia. Fuente compilada,
visual/teléfono No verificados; no sustituir esa aceptación por lectura o build.

## VIS-027 — Captura y feedback de conducción (25/03)

Pedido humano Confirmado; dirección sobre manual0.1.9 autorizado, composición
Propuesta y visual No verificada. Objetivo Overlay/Model/FloatingModal/CSS;
fuentes canónicas siguen UiIcon/tokens/manual actuales. Foco inicial optativo
y superficie táctil pasiva para Canvas resuelven conflictos de interacción
en fuente. Solo dirección activa56px, radio pill y acento existentes; opacity/
scale en140ms y oscilación±3px/800ms para indicar hold. Pequeña respuesta CSS
del modelo8px, sin trayectoria física. Teclado inmediato y reduced motion
sin oscilación/desplazamiento. Sin nuevos colores/activos/dependencias; sin
afirmar prueba visual o aceptación móvil. Mantener soltado/STOP inmediato.

## VIS-028 — Estela direccional de degradé (25/05)

Dirección Confirmada por el pedido humano. Manual0.1.9 autorizado; composición
Propuesta, visual No verificada. Fuentes: tokens de acento del tema y reglas
de movimiento; objetivo Overlay/CSS de conducción, sin cambiar marca ni datos.
Tres trazos sin superficie/borde, degradé transparente→acento, entrada/salida
opacity/scale140ms ease-out y ciclo800ms linear de±6px con fases cola→punta.
Pausar conserva frame al salir; teclado/reduced motion estáticos. El ciclo
indica hold continuo, no un éxito físico ni una medición. Paleta/dependencias
y captura intactas; CSS/SVG sin filtros y solo tres trazos activos. Revisión
visual en ambos temas/anchos pendiente, no cubierta por la compilación.

## VIS-029 — Chevrones bajo el chasis (25/06)

Dirección Confirmada por pedido humano; bajo el auto se interpreta como capa
trasera al modelo y raíz oculta por el chasis, rotada hacia cada dirección.
Manual0.1.9 autorizado, composición Propuesta, visual No verificada. Mayor
cuerpo, origen central y dimensiones relativas al menor lado del viewport
local. Degradé longitudinal de acento y máscara de opacidad transversal;
sin colores visibles nuevos, filtros, raster ni dependencias. Fade220ms
ease-out/ciclo1200ms linear con pausa de frame, teclado y reduced motion
estáticos. Datos/captura/modelo y órdenes físicas conservados. Ver ambos
temas/anchos y oclusión del Canvas queda pendiente, no acreditado por build.

## VIS-030 — Cintas anchas y punta intensa (25/07)

Dirección Confirmada por pedido humano. Composición Propuesta sobre manual0.1.9
autorizado, visual No verificada. Fuentes: tokens de acento/movimiento y
profundidad actuales; objetivo SVG/CSS/cámara manual. Banda más gruesa y mayor
extensión, encuadre52% para dar aire. Punta de acento pleno, cola transparente
que muestra el fondo de cada tema. Máscara de extremos con cuerpo opaco;
sin nueva paleta/filtros/recursos. Fade220ms/flujo1200ms, teclado y reducción
de movimiento estáticos. Solo cámara local, sin alterar escala CAD o mapas.
Revisión visual de puntas/oclusiones/ambos temas pendiente; build no la acredita.

## VIS-031 — Difusión por brazo y punta aguda (25/08)

Dirección Confirmada por el nuevo pedido humano sustituye la punta de color
pleno07. Composición Propuesta sobre manual0.1.9 autorizado, visual No verificada.
Mismo acento con opacidad máxima0,58 y fondo real por transparencia. Cada brazo
usa gradiente normal simétrico y longitudinal alineado, reflejados en la unión;
se retira la difusión global. V más aguda/proporción conservada, sin estirar
el dibujo. Capa bajo chasis, cámara52% y límites de control intactos. SVG/CSS
propios sin filtros/dependencias ni colores nuevos. Fade220ms/flujo1200ms,
teclado y reduced motion estáticos. Contraste/oclusión/unión/ambos temas y
anchos siguen pendientes de revisión visual; compilación no los acredita.

## VIS-032 — Preferencia por diseño anterior (25/09)

El usuario revoca la composición08 y prefiere07 conservando solo color suave.
Dirección Confirmada, composición Propuesta sobre manual0.1.9 autorizado,
visual No verificada. Se restaura íntegro el gráfico/posición/animación07;
opacidadSVG0,58 como único ajuste visual. Se retira componente08 propio tras
comprobar que no tenía cambios posteriores. Hashes de fuente respaldan la
restauración, no una aceptación visual. Cámara/captura/modo local intactos.

## VIS-033 — Modelo manual mayor (25/10)

Pedido humano Confirmado. Solo encuadre52%→60% en ambos ejes (≈15,4% lineal),
centrado/vista superior iguales; chevrones09 intactos. Composición Propuesta
con manual0.1.9 autorizado, visual No verificada. Sin nueva paleta, recursos,
animación o controles. Malla/unidades y cámaras de otras vistas intactas.

## VIS-034 — Manipulación local con dos dedos (25/10/2026)

Pedido humano Confirmado para simular giro sobre su propio eje. Composición
Propuesta sobre manual0.1.9 autorizado, visual/gesto No verificados. Seguimiento
directo del ángulo de dedos, sin inercia; vista/cámara no orbitan. Q/E15° y0
alternativa. Modo local y rumbo en chapa existente, sentido en estado inferior;
número sin anuncios por frame. Chevrones09/cámara60%10 conservados. Un dedo
sigue pantalla después de girar, ruedas convertidas a marco del cuerpo.
Captura dual, cancelar y drenar al perder un contacto; no yaw físico. Máscara/
paleta/animación de chevrones sin cambio; teclado/reduced motion conservados.
Apple-design/mobile-native/animate aplicadas como manipulación funcional,
no inercia de liberación por ser control explícito. Teléfono/lectores/temas
y transiciones de contacto por comprobar, no acreditados por build.

