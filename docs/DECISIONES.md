# Decisiones

| ID | Decisión | Estado |
| --- | --- | --- |
| ADR-001 | Cuatro repos independientes | Acordado |
| ADR-002 | AVR C, ESP Arduino/LittleFS, React/Vite; sin AT/DMA/MPU | Acordado |
| ADR-003 | I2C Manager, solo OLED, servicio cooperativo | Acordado; backend pendiente |
| ADR-004 | Encoder UI; corto confirma, largo abre menú, atrás por ítem | Acordado; no botón menú |
| ADR-005 | UI D40/D41/D42, OLED D20/D21 | Propuesto; comprobar PCB real |
| ADR-006 | GOTO relativo por cuatro encoders de rueda | Diseño; sin orientación absoluta |
| ADR-007 | UNER v2 candidato | IDs/payload/capabilities por cerrar |
| ADR-008 | UART 115200 punto de partida | Error de baud AVR y robustez pendientes |
| ADR-009 | Flash/partición ESP por identificar | No asumir tamaño por el nombre ESP12S |
| ADR-010 | Un dueño de mando con lease; espectadores separados | Propuesto; STOP prioritario |

No hay .ino completo del proveedor entre los ocho adjuntos. El PDF conceptual muestra fragmentos con dos IR D52/D53, distintos del módulo de cinco canales A4..A0. No reconstruir un sketch y llamarlo original. Las URLs del manual son referencias, no instrucciones para instalar software ni firmware AT.

## Ampliación 2026-10-23

- ADR-011: el usuario confirma SDA/SCL expuestos en el shield. Usar las cabeceras con esos rótulos; D20/D21 conservan equivalencia lógica.
- ADR-012: nueva PCB OLED/encoder con J1 1x5 de señales y J2 1x2 VCC/GND, paso 2,54 mm, Dupont. Alimentación y masa comunes; tensión por identificar.
- ADR-013: soporte 3D y cubierta fijan conjunto con dos tornillos/tuercas. Dimensiones y tornillería pendientes de planos del usuario; CAD diferido.
- ADR-014: árbol funcional en documentation/MENU_PLAN.md como propuesta para implementar; no asigna IDs wire ni amplía firmware en este cambio.
