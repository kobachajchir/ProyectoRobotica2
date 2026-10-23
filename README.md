# AutoFinalRobo2

Un único repositorio Git local en D:/ProyectoRobotica2, organizado en cinco módulos. [Informe maestro](INFORME_MAESTRO.md) · [Central documental](AutoFinalRobo2-Contracts/README.md) · [Git, exclusiones e historial](AutoFinalRobo2-Contracts/docs/ESTRUCTURA_Y_GIT.md).

| Módulo | Contenido |
| --- | --- |
| [Atmega](AutoFinalRobo2-Atmega/README.md) | C/avr-libc para ATmega2560; fuente parcial |
| [ESP](AutoFinalRobo2-ESP/README.md) | Arduino ESP8266/LittleFS; esqueleto |
| [Web](AutoFinalRobo2-Web/README.md) | React/Vite; esqueleto |
| [Contracts](AutoFinalRobo2-Contracts/README.md) | Contratos, hardware, evidencia y cambios AFR2 |
| [CAD](AutoFinalRobo2-CAD/README.md) | KiCad, pieza nueva y modelo completo del auto; estructura e insumos |

El trabajo de diseño eléctrico y mecánico se concentra en AutoFinalRobo2-CAD. Los insumos que estaban en CADS se conservan allí con inventario; Documentacion/ conserva los documentos originales. No iniciar un Git dentro de cada módulo.

El .gitignore global excluye dependencias instaladas, cachés, compilados y exportaciones regenerables. Se conservan fuentes editables, manifests, lockfiles, bibliotecas propias e insumos únicos. Sin remotos ni push; los cuatro historiales anteriores están conservados en el Git global.

Leer AGENTS.md antes de trabajar. No ejecutar tests, compilaciones, validadores, ERC/DRC, flasheos ni pruebas físicas sin pedido expreso. Crear la estructura CAD no significa que el diseño esté adaptado: cotas, módulos, huellas y planos siguen pendientes.

Registro: [AFR2-20261023-04](AutoFinalRobo2-Contracts/changes/2026-10-23-04-cad-and-monorepo.md).
