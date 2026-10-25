# AutoFinalRobo2 — maestro de contratos

Vigente24/10/2026, AFR2-20261024-15. Continuar desde la
[matriz de alineación](ALINEACION_MAESTRO.md) y el [maestro de raíz](../../INFORME_MAESTRO.md).
Estos reemplazan como estado actual los pendientes históricos de esqueleto,
TWI, transporte y port de pantallas. Fuentes, compilación y aceptación física
se registran por separado; los originales se conservan en history/alignment15/.

- [Roadmap vigente](ROADMAP.md): cobertura y condiciones restantes.
- [ATmega M01..M19](PLAN_ATMEGA.md), [ESP](PLAN_ESP.md), [Web](PLAN_WEB.md).
- [Catálogo de comandos](../contracts/commands.json), [capabilities de diseño](../contracts/capabilities.json).
- [ROBO2/perfil2](../contracts/UART_PROFILE_2.md), [EX01v6](../contracts/EX01_SEQUENCE_DISTANCE_6.md).
- [Árbol completo de menús](../documentation/MENU_PLAN.md), [59pantallas](../documentation/screens.json).
- [Shield](../hardware/WIFI_SHIELD.md), [carga](../hardware/FLASHING.md), [IRQ/Hall](../hardware/INTERRUPCIONES_ENCODER_HALL.md).
- [Cambio15](../changes/2026-10-24-15-master-alignment.md), [compilaciones](../documentation/builds/2026-10-24-15/README.md).

Sin nuevo wire, ScreenCode, cambio de permisos AVR o CAD. Firmware nominal0.8.0;
Hall y memoria de secuencias mantienen macro0. El software del alcance definido
tiene fuente complementaria; no se certifica operación del robot sin verificar
hardware, calibración, memoria dinámica, protocolos y comportamiento visual.
PCB/pieza/soporte siguen diferidos a insumos del usuario. No inferir cierres
físicos desde documentos del proveedor ni ejecutar pruebas/flash sin pedido.