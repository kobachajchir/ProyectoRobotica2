# Estado de entrega local

Fecha: 2026-10-23. Entrega inicial: cuatro repos independientes en rama codex/bootstrap. Esta organización fue sustituida por el Git global de AFR2-20261023-04, descrito al final. Sin remotos ni push. Los repositorios AutoMicro se consultaron sin modificación; sus cambios previos están en origins.json.

Se ejecutaron únicamente lecturas, extracción/render de PDFs, copias, autoría, inventario documental y operaciones Git locales. No se ejecutaron compilaciones, tests, validadores de código/skill, CI, flasheos ni pruebas físicas. Los hooks se deshabilitaron para los commits de esta entrega.

Atmega: base OLED/UI parcial y referencia; backend TWI/UART/control pendientes. ESP: esqueleto y referencia, bridge pendiente. Web: esqueleto y referencia, control pendiente. Contracts: documentación/skill/contratos candidatos; ninguna compatibilidad end-to-end declarada.

| Repo | Commit base local | Remoto |
| --- | --- | --- |
| atmega | 72238e3f6ef87730049febd8d750dd9957f118f4 | no configurado |
| esp | a6d10ce0cd1f6bbd51ee2439b27e1d06b2393ad9 | no configurado |
| web | f649f072045eb683a54f679619ddd08a70c6613c | no configurado |
| contracts | Este documento forma parte del commit inicial; consultar git log local | no configurado |

Se crearon 96 informes de carpeta. provenance/files.json conserva el detalle de 1113 archivos de referencia copiados; derived.json identifica la primera base C nueva. El registro de la entrega no certifica que todas las dependencias de los originales sean portables.

## Ampliación documental AFR2-20261023-02

Cabeceras SDA/SCL expuestas, dos conectores paso2,54mm, VCC/GND comunes, montaje con dos tornillos/tuercas y árbol de menús. Solo Contracts y maestro raíz actualizados; código de los tres consumidores intacto. CAD diferido. Commit local adicional sin remoto/push; sin tests/build/validadores.

## Ampliación documental AFR2-20261023-03

Evidencia OSOYOO UART V1.3, rutas UART/niveles/arranque, distinción UNO-Mega, flash nominal y prompt completo. Tres imágenes y ficha/Fritzing preservados en hardware/wifi-shield, con manifest incremental. Solo Contracts y maestro raíz actualizados; atmega/esp/web sin cambios. No tests/build/validadores/flash/CAD. Commit local, sin remoto ni push.

## Organización AFR2-20261023-04

Git único en D:/ProyectoRobotica2, rama codex/monorepo-cad. Los cuatro historiales anteriores se conservan como padres del commit de integración y etiquetas archive/pre-monorepo/{atmega,contracts,esp,web}; los .git originales quedan en .git/legacy-repositories, fuera del contenido versionado. [Detalle y recuperación](../docs/ESTRUCTURA_Y_GIT.md).

AutoFinalRobo2-CAD incorpora documentación y estructura para KiCad, pieza nueva y auto completo. Nueve archivos CAD existentes trasladados desde CADS sin adaptación, con inventario de procedencia en CAD. Fuentes de firmware/Web y reference/ histórico intactos; Documentacion/ original intacta. Se excluyen dependencias y productos regenerables mediante .gitignore global. No se ejecutaron tests, compilaciones, validadores, ERC/DRC, flasheos ni pruebas físicas; no se diseñaron piezas nuevas.
