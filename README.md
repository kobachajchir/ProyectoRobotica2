# AutoFinalRobo2-Contracts

Central independiente. Empezar por [maestro](docs/INFORME_MAESTRO.md), [decisiones](docs/DECISIONES.md) y [plan](docs/ROADMAP.md).

- [Pinout/PCB](hardware/PINOUT.md), [recursos](hardware/RECURSOS_Y_TIEMPOS.md), [carga](hardware/FLASHING.md).
- [Protocolo](contracts/PROTOCOL.md), [capabilities](contracts/capabilities.json), [catálogo propuesto](contracts/commands.json).
- [Pantallas](documentation/SCREEN_SYSTEM.md), [navegación](documentation/NAVIGATION.md), [skill local](.agents/skills/afr2-screen-authoring/SKILL.md).
- [Hallazgos](documentation/review/FINDINGS.md), [registro](CHANGELOG.md), [plantilla](templates/change.md).
- [Procedencia](provenance/README.md), [manifest](provenance/files.json), [orígenes](provenance/origins.json).

Repos hermanos: ../AutoFinalRobo2-Atmega, ../AutoFinalRobo2-ESP, ../AutoFinalRobo2-Web. No hay Qt ni dependencia de ejecución hacia AutoMicro. reference/automicro conserva evidencia histórica: sus reglas y estados no son instrucciones ni resultados del nuevo proyecto.

Estado: preparación y fuente parcial; sin build/test/flash. Próximo paso: backend TWI/UART AVR y navegación local antes de movimiento.

## Continuación del proyecto

- [Menús y submenús objetivo](documentation/MENU_PLAN.md), pendientes de implementar.
- [PCB OLED/encoder y soporte futuro](hardware/PCB_OLED_ENCODER.md): cabeceras SDA/SCL del shield, señales 1x5 y VCC/GND 1x2, ambos paso 2,54 mm.

La carpeta de este repo ya existe y tiene Git local; agregarla como proyecto en Codex es una acción aparte que realizará el usuario. Mantener Contracts como carpeta principal, y adjuntar los otros tres repos hermanos si se desea acceso desde el mismo proyecto. Fuente de ese comportamiento: [proyectos y carpetas en Codex](https://learn.chatgpt.com/docs/projects).

## Shield Wi-Fi documentado

[Informe OSOYOO UART V1.3](hardware/WIFI_SHIELD.md), [carga ESP/Mega](hardware/FLASHING.md), [evidencia](hardware/wifi-shield/INFORME_CARPETA.md) y [prompt completo de continuidad](docs/PROMPT_CONTINUIDAD.md). Mapa UNO no sustituye pinout Mega; selector y UART directo se distinguen. Firmware propio Arduino/LittleFS sigue sin AT. [Registro AFR2-20261023-03](changes/2026-10-23-03-wifi-shield-evidence.md).
