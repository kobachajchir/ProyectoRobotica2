> **Loader AFR2-20261027-16 / 0.2.0:** [MCP para conectar con Codex](AutoFinalRobo2-Loader/MCP.md), revisión de solicitudes en GUI, historial y refuerzos de interfaz. [Ficha](AutoFinalRobo2-Contracts/changes/2026-10-27-16-loader-mcp-ui.md). Fuente/documentación; sin compilar, ejecutar, registrar MCP, probar COM ni flashear en esta revisión.

> **Vigente AFR2-20261025-01:** [control directo7](AutoFinalRobo2-Contracts/contracts/DIRECT_CONTROL_7.md), sin permiso ni opciones de adquisición. Los tres módulos compilan con firmware0.9.0/Web0.2.0. Estado anterior inferior histórico.

> **Vigente AFR2-20261024-15:** [Matriz del maestro](AutoFinalRobo2-Contracts/docs/ALINEACION_MAESTRO.md):33comandos y59identidades OLED con fuentes complementarias. Calibración Web adquiere control al escribir; ESP/Web informan FS/paquete. Web/ESP compilados15; AVR sin nueva fuente, build12/EX01v6/firmware0.8.0/gates0. Sin brecha software definida identificada tras lectura; aceptación física, memoria dinámica, flash/SPI y planos CAD pendientes. Solo documentación en CAD, sin diseño nuevo. Estados inferiores históricos.

> **Fuente vigente AFR2-20261024-11:** encoder CLK/DT en D2/D3 por INT4/INT5, SW D42; cola y acciones en main. Hall PCINT2 A8–A15 con preferencia de captura por software, macro0. **Esta adaptación no está compilada ni probada**; build08 corresponde a la fuente anterior. ESP/Web conservan su estado10. [Ficha11](AutoFinalRobo2-Contracts/changes/2026-10-24-11-encoder-hall-interrupts.md). Los estados previos inferiores son históricos.

> Vigente **AFR2-20261024-04**: **modelo final incluido en ESP**, paquete **1.450.201 bytes gzip** (49,96% menos que los 2,90 MB iniciales). WebP, JS/CSS minificados y dependencias/componentes optimizados. Layout candidato: flash 4 MiB / LittleFS nominal 3 MiB, solicitado por el usuario; flash fisica sin identificar. Web full/esp y firmware ESP compilados; activos promovidos a data/dist. Sin navegador, imagen FS ni flasheo. Atmega y comandos conservan el estado de02. [Evidencia](AutoFinalRobo2-Contracts/changes/2026-10-24-04-final-model-littlefs.md). Los estados previos inferiores son historicos.

> Estado vigente **AFR2-20261024-02**, 24/10/2026: integración ROBO2/perfil2 + EX01v3 compilada en Atmega/ESP/Web; GOTO completo, configuración por catálogo compartido y jobs Wi-Fi OLED conectados. Paquete Web actualizado en ESP/data/dist. Sin pruebas de navegador/hardware ni flasheo; secuencias físicas mantienen bloqueo de memoria. Ver [matriz vigente](AutoFinalRobo2-Contracts/contracts/EX01_ALIGNMENT_3.md) y [evidencia](AutoFinalRobo2-Contracts/changes/2026-10-24-02-project-alignment.md). Los estados previos que siguen son históricos.

# AutoFinalRobo2

Un único repositorio Git local en D:/ProyectoRobotica2, organizado en seis módulos. [Informe maestro](INFORME_MAESTRO.md) · [Central documental](AutoFinalRobo2-Contracts/README.md) · [Git, exclusiones e historial](AutoFinalRobo2-Contracts/docs/ESTRUCTURA_Y_GIT.md).

| Módulo | Contenido |
| --- | --- |
| [Atmega](AutoFinalRobo2-Atmega/README.md) | C/avr-libc para ATmega2560; fuente parcial con endpoint/USART1 |
| [ESP](AutoFinalRobo2-ESP/README.md) | Arduino ESP8266/LittleFS; servidor y broker escritos, sin compilar |
| [Web](AutoFinalRobo2-Web/README.md) | React/Vite; paquete previo y cambios fuente pendientes de reconstruir |
| [Contracts](AutoFinalRobo2-Contracts/README.md) | Contratos, hardware, evidencia y cambios AFR2 |
| [CAD](AutoFinalRobo2-CAD/README.md) | KiCad, pieza nueva y modelo completo del auto; estructura e insumos |
| [Loader](AutoFinalRobo2-Loader/README.md) | Cargador Windows: compilación y flasheo guiados de ATmega/ESP/Web/LittleFS; fuente sin ejecutar ni probar |

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

## Firmware ESP y comunicación — AFR2-20261023-11

[Estado de implementación y pendientes](AutoFinalRobo2-Contracts/changes/2026-10-23-11-esp-server-and-mega-link.md): HTTP/WS/LittleFS/Wi-Fi ESP y endpoint USART1 AVR escritos; perfil binario AFR2 1 con sesión/transacción. Sin tests, compilación ni flasheo de esta entrega. Identificar flash/layout y completar servicios AVR; motores/lease/GOTO siguen pendientes. La Web fuente tiene ajustes de capacidades/STOP; paquete nuevo pendiente.


## AVR integrado y ROBO2 — AFR2-20261023-15

Atmega integra UART1/Timer0/TWI/I2C Manager/OLED/encoder en `src/main.c`, con
potencia inactiva y sensores desconocidos. Fuente sin compilar ni probar.
ROBO2/perfil 2 sustituye el framing UNER/perfil 1 en ambos extremos; mantiene
sesión/transacción y rutas por nibbles. Contrato vigente y evidencia en
AutoFinalRobo2-Contracts/changes/2026-10-23-15-robo2-atmega-ui.md. Para continuar
ESP: AutoFinalRobo2-ESP/docs/PROTOCOLO_ROBO2.md. Motores/lease/Hall/GOTO/UI remota
pendientes. No se ejecutaron builds, tests, validadores ni flasheos.
