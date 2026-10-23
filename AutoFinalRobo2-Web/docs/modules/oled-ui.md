# Visor OLED, pantallas y acciones

[Maestro](../../../INFORME_MAESTRO.md) · [Informe del repositorio](../../INFORME_ADAPTACION.md).

## Utilidad

Identidades, builders, rasterizador/editor y controles permiten estudiar la biblioteca completa; SVG no es captura de pantalla física.

## Adaptación

Portar motor/recursos/estado o transportar framebuffer si se requiere fidelidad. Distinguir acción/invocador/vista. Mismo esquema de navegación que encoder: menú explícito, atrás por ítem; no simular botón físico inexistente. Implementar estados nuevos solo si Mega los reporta. No mantener sensores falsos para rellenar una pantalla.

## Evidencia y alcance

- [reference/web/src/screens](../../reference/web/src/screens)
- [reference/web/src/features/oledEditor](../../reference/web/src/features/oledEditor)
- [reference/web/src/components](../../reference/web/src/components)
- [../AutoFinalRobo2-Contracts/documentation/SCREEN_SYSTEM.md](../../../AutoFinalRobo2-Contracts/documentation/SCREEN_SYSTEM.md)

Hallazgos: AFRDOC-009, AFRDOC-010. [Registro](../../../AutoFinalRobo2-Contracts/documentation/review/FINDINGS.md).

Estado: lectura/copia o fuente parcial indicada; sin compilación, tests ni prueba física. La API detallada de origen está en el corpus AMDOC importado; no equivale a API AVR adaptada.
