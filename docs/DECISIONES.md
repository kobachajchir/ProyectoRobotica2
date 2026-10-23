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
