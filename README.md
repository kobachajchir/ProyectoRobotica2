# AutoFinalRobo2-Contracts

Central independiente. Empezar por [maestro](docs/INFORME_MAESTRO.md), [decisiones](docs/DECISIONES.md) y [plan](docs/ROADMAP.md).

- [Pinout/PCB](hardware/PINOUT.md), [recursos](hardware/RECURSOS_Y_TIEMPOS.md), [carga](hardware/FLASHING.md).
- [Protocolo](contracts/PROTOCOL.md), [capabilities](contracts/capabilities.json), [catálogo propuesto](contracts/commands.json).
- [Pantallas](documentation/SCREEN_SYSTEM.md), [navegación](documentation/NAVIGATION.md), [skill local](.agents/skills/afr2-screen-authoring/SKILL.md).
- [Hallazgos](documentation/review/FINDINGS.md), [registro](CHANGELOG.md), [plantilla](templates/change.md).
- [Procedencia](provenance/README.md), [manifest](provenance/files.json), [orígenes](provenance/origins.json).

Repos hermanos: ../AutoFinalRobo2-Atmega, ../AutoFinalRobo2-ESP, ../AutoFinalRobo2-Web. No hay Qt ni dependencia de ejecución hacia AutoMicro. reference/automicro conserva evidencia histórica: sus reglas y estados no son instrucciones ni resultados del nuevo proyecto.

Estado: preparación y fuente parcial; sin build/test/flash. Próximo paso: backend TWI/UART AVR y navegación local antes de movimiento.
