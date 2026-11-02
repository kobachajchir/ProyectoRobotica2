> **AFR2-20261029-26:** fuentes de batería0x55/EX01v9 y navegación/pill/confirmaciones vinculadas según Contracts/contracts/BATTERY_MEASUREMENT_9.md. Screens0x030039/0x03003A y acción69. Revisión solo por lectura/edición, sin build/generador/pruebas/carga; adaptación por archivo en Contracts/provenance/battery-system-20261029-26.json.

# Carpeta src/ui

Estado vigente AFR2-20261023-15: backend/composición integrados en fuente; el texto anterior de migración se conserva debajo como antecedente. Sin compilar/probar.

[Maestro](../../../INFORME_MAESTRO.md) · [Informe del repositorio](../../INFORME_ADAPTACION.md).

Base nueva de trabajo; ver estado de implementación en el informe del repositorio.

## Utilidad y reformas específicas

### [Wrappers, menús y pantallas](../../docs/modules/screens.md)

Wrappers preparan datos/contexto; renderer dibuja; navegación decide pantalla/selección; acciones llaman servicios. Reutilizar conceptos y recursos, no System/globales completos. La referencia incluye menús/OSK/avisos y toda la biblioteca propia para port posterior.

Reformas: Nueva UI usa snapshot explícito sin consultas de hardware en renderer. Mantener identidad/rol/estado/acciones por separado. Largo SW abre menú; corto selecciona; atrás solo por ítem Volver. Retirar llamadas al botón menú y no reinterpretar largo como atrás. Main procesa acciones y respeta valid/unknown. Llevar menús/textos a flash; decidir contexto de edición/confirmación sin ejecutar motor al navegar. Skill en Contracts documenta cada pantalla; paridad Web pendiente.

## Archivos vecinos

- [afr2_screens.c](<afr2_screens.c>)

Estado: material preparado sin build, tests, validadores ni prueba física. Los hashes de fuentes copiadas están en [manifest](../../../AutoFinalRobo2-Contracts/provenance/files.json).


AFR2-20261023-15: TWI AVR por IRQ y composición OLED/encoder/USART1 implementados en fuente, sin DMA ni Arduino. Ver docs/COMPOSICION.md y Contracts/changes/2026-10-23-15-robo2-atmega-ui.md. Sin builds/tests/validadores/flasheos; hardware y recursos finales pendientes.


## AFR2-20261029-19 — OLED táctil y visor bajo demanda

Fuente adaptada con Web/ATmega/ESP: overlay scrim+blur en modales, Volver por ítem del menú, ScreenCode0x030035 con aviso/bitmap/chevrones y suscripción explícita a capturas/metadatos. Contrato en AutoFinalRobo2-Contracts/contracts/TOUCH_OLED_19.md; procedencia por archivo en provenance/touch-oled-20261029-19.json de Contracts. El rumbo es solicitado, no medido, y no agrega giro físico. Sin tests, compilación, validadores, navegador, ejecución del generador OLED, promoción ESP ni flasheo de esta entrega. Memoria/paridad/latencia físicas pendientes.

## AFR2-20261102-05 — Componentes OLED128-5 y QR en runtime

`afr2_ui_components.c` centraliza cabecera, panel, progreso124x8, numerales5x7 y números compactos. `afr2_qr_screen.c` genera en el propio Mega QR AP/Web y acepta contenido RAM elegido. Menú Wi-Fi incorpora030045/030046. Mantiene snapshot puro, encoder y guardas; AP privado, auto/radar detenidos. Contraparte TypeScript y simulador de Ayuda actualizados. Estado y límites en Contracts/contracts/OLED_RUNTIME_QR_20261102.md. Sin compilación, tests, validadores, navegador ni flasheo; memoria total/tiempos/escaneo pendientes.
