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

El .gitignore global excluye dependencias instaladas, cachés, compilados y exportaciones regenerables. Se conservan fuentes editables, manifests, lockfiles, bibliotecas propias e insumos únicos. Origin privado: [kobachajchir/ProyectoRobotica2](https://github.com/kobachajchir/ProyectoRobotica2), rama remota main. Los cuatro historiales anteriores y sus etiquetas están conservados en el Git global.

Leer AGENTS.md antes de trabajar. No ejecutar tests, compilaciones, validadores, ERC/DRC, flasheos ni pruebas físicas sin pedido expreso. El plan 3D distingue el ensamble base con inferencias documentadas de la pieza nueva/PCB pendientes de planos; aún no hay un modelo adaptado.

Registro: [AFR2-20261023-04](AutoFinalRobo2-Contracts/changes/2026-10-23-04-cad-and-monorepo.md).

## Plan 3D y continuación

[Plan completo de Fusion](AutoFinalRobo2-CAD/docs/PLAN_MODELO_3D_FUSION.md): ensamble del auto base sin cables, fuentes CAD y pendientes de la pieza nueva. MCP de Fusion existente. OLED 0,96 pulgadas y encoder KY-040 sin breakout/perilla como referencias para la integración futura. Registro [AFR2-20261023-05](AutoFinalRobo2-Contracts/changes/2026-10-23-05-fusion-plan-and-origin.md).

Para bajar el proyecto en la otra PC, con acceso a tu cuenta de GitHub:

```powershell
git clone https://github.com/kobachajchir/ProyectoRobotica2.git
```

Abrir la carpeta clonada y empezar por AutoFinalRobo2-CAD/docs/PLAN_MODELO_3D_FUSION.md. El clone usa main; los resguardos locales dentro de .git no se transfieren, pero los commits históricos y sus etiquetas sí.
