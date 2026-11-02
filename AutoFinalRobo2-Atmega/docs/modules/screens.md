> **AFR2-20261024-08:** IP/AP y avisos con iconos F4, scroll seleccionado y reinicio local por Sistema → Reinicio, compilados. [EX01v5](../../../AutoFinalRobo2-Contracts/contracts/EX01_NETWORK_UI_5.md) · [Estado real](../../../AutoFinalRobo2-Contracts/docs/ESTADO_IMPLEMENTACION.md). APIs nuevas FillRect/TextWindow, iconos y snapshot de red separados del transporte/I2C. Sin prueba de placa ni flasheo; estados inferiores históricos.

# Wrappers, menús y pantallas

AFR2-20261023-19, fuente integrada sin compilar/probar: 44 nodos, historial8, editores/teclado y cola4, renderer puro; fidelidad F4 no acreditada.
[Producto vigente](../PRODUCTO_ATMEGA.md). El diseño histórico siguiente conserva su evidencia.


Estado vigente AFR2-20261023-15: backend/composición integrados en fuente; el texto anterior de migración se conserva debajo como antecedente. Sin compilar/probar.

[Maestro](../../../INFORME_MAESTRO.md) · [Informe del repositorio](../../INFORME_ADAPTACION.md).

## Utilidad

Wrappers preparan datos/contexto; renderer dibuja; navegación decide pantalla/selección; acciones llaman servicios. Reutilizar conceptos y recursos, no System/globales completos. La referencia incluye menús/OSK/avisos y toda la biblioteca propia para port posterior.

## Adaptación

Nueva UI usa snapshot explícito sin consultas de hardware en renderer. Mantener identidad/rol/estado/acciones por separado. Largo SW abre menú; corto selecciona; atrás solo por ítem Volver. Retirar llamadas al botón menú y no reinterpretar largo como atrás. Main procesa acciones y respeta valid/unknown. Llevar menús/textos a flash; decidir contexto de edición/confirmación sin ejecutar motor al navegar. Skill en Contracts documenta cada pantalla; paridad Web pendiente.

## Evidencia y alcance

- [reference/f4/Core/Src/screenWrappers.c](../../reference/f4/Core/Src/screenWrappers.c)
- [reference/f4/Core/Src/oled_utils.c](../../reference/f4/Core/Src/oled_utils.c)
- [reference/f4/Core/Src/menusystem.c](../../reference/f4/Core/Src/menusystem.c)
- [reference/f4/Core/Inc/screen_context.h](../../reference/f4/Core/Inc/screen_context.h)
- [src/ui/afr2_screens.c](../../src/ui/afr2_screens.c)

Hallazgos: AFRDOC-009, AFRDOC-010. [Registro](../../../AutoFinalRobo2-Contracts/documentation/review/FINDINGS.md).

Estado: lectura/copia o fuente parcial indicada; sin compilación, tests ni prueba física. La API detallada de origen está en el corpus AMDOC importado; no equivale a API AVR adaptada.


AFR2-20261023-15: TWI AVR por IRQ y composición OLED/encoder/USART1 implementados en fuente, sin DMA ni Arduino. Ver docs/COMPOSICION.md y Contracts/changes/2026-10-23-15-robo2-atmega-ui.md. Sin builds/tests/validadores/flasheos; hardware y recursos finales pendientes.

## AFR2-20261102-05 — Estado actual

Los componentes OLED128-5 se integran en AVR y Web. QR AP030045 y Web030046 usan datos preparados del ESP; API de contenido propio en `afr2_qr_screen.h`. Corto desde Inicio abre menú; largo al soltar abre rápidas y retorna por contexto en otras vistas, con Sí/No obligatorio para cambios. Ver Contracts/contracts/OLED_RUNTIME_QR_20261102.md. Esta entrega no compila, prueba, valida ni flashea.

## AFR2-20261102-14 — Composición y estado

Dashboard010001 usa `afr2_dashboard.c` y estado cooperativo3s/4s con pausa por encoder mantenido, fallo prioritario y actualización por regiones. `Afr2UiSnapshot` prepara campos reales; renderer sin hardware/delays. Se alinean cabeceras, selectores, datos y confirmaciones con Web. Navegación/privacidad QR sin cambios. Ver Contracts/contracts/OLED_DASHBOARD_STATUS_20261102.md. Fuente sin compilar/probar/flashear.
