# Decisiones visuales — 23/10/2026

## VIS-045 — Escena y controles de los simuladores (31/10/2026)

Confirmada: pedido humano de aplicar Emil, Apple Design y Visual Identity a
Auto/Brazo/Conjunto, fondo celeste y suelo crema, montaje conforme al CAD y eje
del servo corregido. Supporting Observada: identidad0.1.9 autorizada, Button,
TaskSections, Disclosure, CollapsiblePanel y controles compartidos. Los objetivos
no se elevan a fuente canónica. No hay nuevas referencias externas.

Propuesta implementada: encabezado contextual, escena amplia, cámara agrupada,
lecturas debajo y controles por tarea; matemática opcional y puntos/motores
avanzados inicialmente plegados. Sustituye la apertura inicial de VIS-043,
conserva el análisis a la derecha cuando se solicita. Fondo/suelo/retícula usan
tokens locales para ambos temas; los valores exactos son Propuesta, no cambian
la paleta de marca. Botones de44px, ayudas14px, foco vuelve al botón Matemática
al cerrar el panel y la parada también cubre movimiento articular. No se agrega
movimiento decorativo ni dependencias; primitivas conservan reduced motion.

Se actualiza la montura v17 con el marco CAD/GLB y se separa la salida del SG90
en Blender conservando materiales. Fuente implementada y activo público
actualizado; aspecto, animación y montaje en navegador **No verificados**.
Sin tests/build/validadores/flasheo por instrucción humana. Los paquetes
dist/artifacts/ESP conservan la revisión05. [Cobertura](coverage-20261031-06.json).

## VIS-044 — Enseñar y recuperar destinos del brazo (27/10/2026)

Confirmada: plan humano de puntos individuales persistentes, misma biblioteca en
Brazo y Conjunto, y conservacion de posicion/orientacion al cambiar dimensiones.
Pedido explicito de Visual Consistency y Apple Design. Observada: identidad0.1.9
autorizada y primitivas de tarjeta, campos, Button y Chevron compartidas.
Propuesta aplicada: tarjeta plegable inferior, captura/lista/ficha, acciones
proximas al destino, marcadores seleccionables, postura transparente y recorrido
continuo por segmentos. Cancelar permanece junto al visor si se pliega la tarjeta.
Texto y leyenda distinguen destino, modelo, calculo y bloqueo. Sin rebotes ni
inercia en las articulaciones; feedback inmediato con primitivas existentes y
teclado/reduced motion heredados. Sin nueva dependencia o blur anidado.
[Cobertura](coverage-20261027-11.json). Fuente implementada, aspecto/interaccion
No verificados; sin tests/build/validadores/navegador por esta entrega.

## VIS-043 — Ayuda contextual y matematica protagonista (27/10/2026)

Confirmada: pedido humano de explicar cada parametro a la derecha, destacar
la matriz y usar el mismo formulario matematico en las tres simulaciones.
Reutilizar el pad simulado en Control y alinear los giros con las columnas
extremas. Observada: identidad0.1.9, superficies/tokens propios, tarjeta05,
pad simulado y numeros compactos. Propuesta aplicada: ficha con significado,
rango, interpretacion y dependencias; columna matematica mayor, corchetes
visibles, bloques de rotacion/posicion y lecturas trigonometricas centradas.
SimulationMatrix/HomogeneousMatrix, MecanumDrivePad, HoldActionButton y
RangeNumberField estandarizan presentacion; cada adaptador conserva su dominio.
No se animan las cifras ni se agregan materiales, activos o dependencias.
[Cobertura](coverage-20261027-06.json). Direccion Confirmada, composicion
Propuesta y aspecto/interaccion No verificados. Sin tests/build/navegador.

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



## VIS-035 — Sensores, radar y red (25/10/2026)

Confirmada por pedido humano: usar borrador0.1.9, disposición GOTO con mediciones/ángulo al costado, tres tareas separadas, modal propio de Red y nombre Configuración. Radar semicircular Three.js con escala física y PWM aplicado; Modelo3D se conserva estático. IR original/digital/ADC no se etiqueta como distancia.

Propuesta: anillos4, escalas0.5/1/2/4m, iconos propios coherentes y umbrales RSSI -55/-67/-75dBm. Amarillo AP y verde STA consumen warning/success existentes con texto/icono, no solo color. Origen de la sesión por IP del socket, sin atribuir RSSI STA al cliente AP. Superficies de trabajo sólidas, chrome/modal neutro; acento carmesí/rubí propio.

Movimiento: presión inmediata, modal común con foco/escape/retorno; interpolación críticamente amortiguada desde ángulo visual/velocidad actual. En reduced motion se actualiza el dato sin transición; nunca se congela el ángulo real ni se integran muestras visuales. Geometrías del grid memoizadas y recursos dispuestos al desmontar. Las nuevas lecturas no producen toasts por muestra.

Cinco reglas protectoras: (1) fuente autoritativa y edades propias, (2) misma composición GOTO/primitivas, (3) paleta y semántica existentes, (4) navegación OLED y copia Web vinculadas, (5) motion accesible sin datos fabricados. Sin validación visual, browser ni ensayos; cobertura de fuente no acredita contraste, paridad o montaje.

Revisión de barrido independiente: cada inicio cambia scanRevision para descartar mapas anteriores aunque no se haya observado la parada. La vista fija puede superponer la última muestra válida con ángulo/PWM conocido sobre el histórico; los puntos fuera de escala se cuentan. Ningún punto se genera desde la interpolación visual.

## VIS-036 — Puntos y fichas del modelo 3D (25/10/2026)

Dirección Confirmada por pedido humano: puntos fijados a piezas, ficha explicativa
local que conserva su posición al girar el auto y acceso a la sección relacionada.
Continúa aplicación del borrador0.1.9 autorizado, sin nueva fuente de marca.
Propuesta: numeración01–11, selectores44px separados con líneas, ficha superior
derecha/escritorio e inferior/móvil, desplegable alternativo, X/Escape y cierre
exterior que distingue clic de arrastre. Referencias de geometría Inferidas;
ESP y OLED/encoder requieren coordenadas locales del exportado conjunto actual.

Usa popover neutro compartido y tokens de acento, superficie, radio, tipografía
y sombra, con iconos propios. Seguimiento geométrico inmediato sin interpolar
selectores; solo feedback corto en botones, sin transición en reduced motion.
La ficha no es modal: permite seguir orbitando y hacer zoom. Scroll propio,
foco en cierre y retorno al selector/desplegable. La navegación abre tareas y
rueda elegida, sin disparar acciones físicas. OLED conserva SW único y Volver.

Cinco reglas protectoras: material/token común; una ficha por vez; puntos unidos
al CAD y panel independiente; teclado/tacto/movimiento reducido; explicaciones
con datos del proyecto y acciones físicas solo en sus controles. Composición
Propuesta e interacción/visual No verificadas; sin navegador, tests, build ni
validadores. Cobertura: coverage-20261025-15.json. Fuente nueva separada del
build14 anterior. Revisión visual de referencias, temas y gestos queda pendiente.


## VIS-037 — Radar volumétrico y controles vinculados (25/10/2026)

Confirmada: pedido humano de un radar tecnológico/neón con líneas 3D, rango
doble0–180°, barra y campo numérico para velocidad/estabilización/intervalo,
confirmación Enter/blur y límites coherentes. Configuracion sin acento por pedido
expreso. Se conserva el manual0.1.9 autorizado y la excepción Modelo3D.

Observada: disposición mapa/lecturas laterales de Control/GOTO, primitivas de
modal/Disclosure, paleta neutra con acento carmesí/rubí y límites del contrato8.
Propuesta: proyección oblicua, plataforma con espesor, anillos tubulares/halo
moderado, contorno elevado y hitos 3D. La altura del hito no representa altura
medida. No se agrega otra marca cromática ni un barrido ficticio.

Manipulación1:1, sin easing del rango/sector. Microfeedback de borde/foco140ms;
aguja conserva interpolación amortiguada del dato aplicado y reduced motion.
Canvas bajo demanda; geometrías/texturas liberadas. La vista previa se distingue
de la configuración aceptada, bloqueando iniciar hasta aplicar los cambios.
No toasts por muestra ni nuevos comandos/pantallas OLED.

Cinco reglas protectoras: escala y procedencia real, composición GOTO, tokens
existentes, estado de aplicación explícito y movimiento accesible funcional.
No verificada: presentación en temas/tamaños, táctil/teclado/lectores, rendimiento
WebGL y contraste final; sin ejecución de herramientas de comprobación.


## VIS-042 — Ancho util y tarjetas integradas (27/10/2026)

Confirmada: pedido humano de quitar margenes amplios, unificar simulaciones en
una tarjeta, adosar/plegar matematica y mover controles debajo del renderizador.
Numeros de slider compactos; botones de motor distinguibles; Control agrupado
en Movimiento y Pruebas/calibracion, manteniendo GOTO y funciones existentes.
Observada: primitivas y Chevron0.1.9, con transition transform180ms ease-out;
teclado/reduced motion inmediatos. Propuesta: relleno12…24 px, matriz320 px o
rail52 px, divisores internos y controles distribuidos en dos columnas.
Se retira el limite1120 px (contentMax=none) por direccion humana confirmada.
[Cobertura](coverage-20261027-05.json). Fuente sin tests/build/validaciones ni
observacion visual; aspecto y respuesta No verificados.

## VIS-041 — Simulaciones y Modelos3D (27/10/2026)

Confirmada: pedido humano de unificar tres simulaciones, mantener Control aparte,
modelos elegibles mediante pills, gripper independiente y controles mas ordenados.
Observada: TaskSections, campos de radar con titulo/numero superiores y barra
inferior, tokens y superficies0.1.9 autorizados. Propuesta: controles compactos
derechos, matriz persistente debajo, motores en pills con nombre central.
Articulaciones seleccionables y arrastrables con ambos botones del mouse.
El selector del visor conserva herramientas; brazo final/referencias y burbuja
definitiva esperan los insumos del usuario. [Cobertura](coverage-20261027-04.json).
Fuente implementada, aspecto/interaccion No verificados, sin tests/build/navegador.

## VIS-040 — Control intacto, brazo y conjunto separados (27/10/2026)

Confirmada: correccion humana que exige conservar Control/control tactil y
agregar solamente Brazo robotico y Simulacion conjunta. Controles a la derecha,
articulaciones seleccionables y gripper, posiciones de cubos y toma segun alcance.
Observada: tokens0.1.9 autorizados, Button, TaskSections y disposiciones laterales
existentes. Propuesta: escena izquierda, controles derechos con eje seleccionado,
selector de tareas del conjunto, proyeccion del espacio de toma y cubos verdes
alcanzables/ocre fuera de alcance; en movil se apilan los controles debajo.
Sustituye rutas y disposicion VIS-039. Aspecto No verificado, sin ejecucion de
tests/build/validadores/navegador. [Cobertura](coverage-20261027-02.json).

## VIS-039 — Robot recolector y panel del brazo (27/10/2026)

Confirmada: pedido humano de simulador3D, referencias reversibles, auto
seleccionable/movil, motores adelante/atras y panel del brazo separado del robot.
Observada: identidad0.1.9 previamente autorizada, tokens, Button/Disclosure,
SectionHeader, navegacion y escenas propias. Propuesta: escena ancha, vistas
Superior/Lateral/Perspectiva, datos inferiores y controles por panel, con estado
compartido en memoria. Rig del brazo y deposito nominales; no CAD final.
[Cobertura](coverage-20261027-01.json). Direccion Confirmada, composicion
Propuesta y aspecto No verificado. Sin tests/build/validaciones/navegador.

## VIS-038 — Composición del radar (26/10/2026)

Confirmada: el usuario rechaza el resultado visual anterior y pide rehacer el
radar con Emil y skills visuales, coherente con el producto. Se conserva la
identidad0.1.9 autorizada. No se usa el radar rechazado como referencia canónica.
Observada: tokens de Control/GOTO, Button/Disclosure y jerarquías del manual.
Propuesta: campo continuo con retícula que desvanece, anillos/sector rubí y
encuadre de límites proyectados; distancia dominante y detalles plegables,
parámetros debajo del mapa. Inicio1m y dos vistas; no se agranda el modelo.

El shader de suelo no usa tiempo. Marcas en una sola geometría, sin bloom ni
paquetes adicionales. Cámara/alcance y rangos responden inmediatamente;
servo/aguja conservan interpolación funcional y reduced motion. Sin animar
lecturas numéricas, sin ecos de muestra y sin atribuir al halo un haz calibrado.

Cinco reglas: (1) tokens propios Control/GOTO, (2) jerarquía dato/control/detalle,
(3) escala y procedencia reales, (4) controles accesibles y datos desconocidos
explícitos, (5) movimiento ligado al dato. VIS-037 se conserva para los controles,
pero se sustituye su plataforma/composición. Diseño Propuesto; temas/móvil,
contraste, teclado y rendimiento No verificados. Sin tests/build/validadores.
