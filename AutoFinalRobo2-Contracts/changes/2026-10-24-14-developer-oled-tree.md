# AFR2-20261024-14 — Desarrollador y árbol OLED

Pedido humano: retirar hardware/reinicios de Desarrollador, flecha en Volver al
inicio y espaciado del perfil; catálogo59 como árbol, selección/scroll laterales
tomados de la Web AutoMicro/F4.

Web adapta la estructura de selectores de filas1..3 y flechas de scroll de
ScreenStreamWorkspace de AutoMicro, conservando autorías/procedencia. Se usan
tokens propios y el contrato AFR2: números resaltan filas, SW corto confirma,
largo800ms abre principal y regreso solo por Volver. Las flechas desplazan hasta
tres opciones con la selección del encoder; no son paginación independiente F4.

El árbol deriva los59 ScreenCodes/padres del catálogo compartido. Avisos forman
un grupo de retorno al contexto, sin inventarles un padre de navegación. Selección
abre biblioteca local vinculada, ramas con chevrón, selección accent y scroll
dentro del árbol. Wrapper local conserva selección/firstVisible/historial8 y
scroll de texto40ms/pausas800ms del renderer copiado. No invoca acciones ni
inventa telemetría/credenciales: vistas con datos necesitan captura conectada.

El visor conectado conserva framebuffer1024bytes y añade los mismos controles
laterales. Cada giro usa delta±1 y revisión de su captura; selección/desplazamiento
de hasta3pasos serializados con captura nueva entre cada paso. Ante error,
selección inesperada, cambio de pantalla/sesión o pérdida de foco/visibilidad,
el lote se interrumpe. Sin confirmar acciones automáticamente desde números.
Polling se restablece al terminar. Menús dinámicos sin cuenta reportada conservan
encoder básico, sin inventar botones/filas laterales. No nuevo wire ni permiso.

Desarrollador conserva perfil/contactos propios y AppShell; hardware/reinicios
quedan en Conexión y apariencia. Nombre con márgenes explícitos y separación
uniforme, enlace de inicio con icono anterior al texto.

Estado: Web adaptada; build/paquete en evidencia14. ATmega/ESP sin nueva fuente de
firmware, conservan0.8.0/EX01v6/build12 y gates0. Biblioteca de dibujo C/TypeScript,
glifos/XBM y59identidades sin cambios de algoritmo o ScreenCode. Sin CAD ni
mutaciones de D:/AutoProyecto. Comparación visual/diferencial y pruebas de placa
pendientes; no ejecutar tests/validadores/navegador/flash por esta adaptación.
[Evidencia14](../documentation/builds/2026-10-24-14/README.md) y
[procedencia](../provenance/developer-oled-tree-20261024.json).
