# AFR2-20261117-04 — Modo de robot en menú y rápidas OLED

Fecha: 17/11/2026. Autoría: Koba Chajchir / AFR2.
Versión: **1.34.0 (subversión)**.

## Pedido y comportamiento

El usuario confirmó que el alcance es OLED y precisó que la tarjeta Modo y el menú deben sincronizarse: **Reposo, Manual, Seguidor de linea, Ir**. No se agrega una segunda tarjeta Modo.

- «Modos y misiones» / «Modos/misiones» se renombran a «Modo de robot» en Estado y sus encabezados. La tarjeta rápida y su selector usan el mismo nombre.
- Los nodos existentes `AFR2_N_MODES` (`0x03000A`) y `AFR2_N_QUICK_MODES` (`0x03003D`) usan una única tabla AVR `robotModes[]`, de cinco filas contando Volver. Copia explícita en `firmwareMenuItem` Web.
- Reposo y Manual invocan `AFR2_A_QUICK_MODE` con 0 y 1, con los permisos, respuesta y aviso existentes. Seguidor de linea abre `0x03000B`; Ir abre GOTO relativo `0x03000D`. El usuario configura y solicita Iniciar desde sus controles existentes; navegar no inicia movimiento.
- Giro y selección mantienen ventana de tres filas. Volver y largo al soltar usan el contexto anterior: el selector conserva retorno a Estado o rápidas, y las pantallas de línea/GOTO regresan al selector desde el que se abrieron. Las guardas Sí/No se conservan.
- El carrusel mantiene ocho tarjetas: Modo de robot, Motores, Ultrasonido, Radar, Servo, Línea (lectura IR), Rastro y Ajustes.

## Archivos y procedencia

- ATmega: `src/ui/afr2_menu_catalog.c` (tabla común, nombres y recuentos) y `src/ui/afr2_screens.c` (título de tarjeta). El buffer local del título pasa de 12 a 18 bytes (+6 bytes locales); la tabla de títulos permanece en PROGMEM. Se añade una tabla de cinco opciones en flash, sin framebuffer ni memoria dinámica nuevos. Presupuesto compilado/pila sin medir.
- Web: `src/features/oled/firmwareRenderer.ts` y `reformCatalogue.ts`; se conservan fuente, iconos, coordenadas, clipping, scroll y gestos comunes. `firmwareRenderer.sources.json` registra las huellas normalizadas de los dos orígenes AVR modificados después del port en fuente.
- Contracts: `documentation/screens.json`, flujo de navegación, overlay de conexiones, ficha, CHANGELOG y versionado. Los IDs existentes se conservan; no hay CMD ni payload UART nuevos.

## Estado real

ATmega y Web modificados solo en fuente. ESP solo versión compartida. Loader y CAD sin cambios por esta ficha. Recursos OLED generados pendientes del próximo inicio/build que ejecute el usuario; no se editan manualmente sus bytes.

No se ejecutaron generadores, tests, compilaciones, validadores, navegador, puertos ni flasheos. Sin paridad visual o física acreditada para esta revisión. No se hizo commit ni publicación.
