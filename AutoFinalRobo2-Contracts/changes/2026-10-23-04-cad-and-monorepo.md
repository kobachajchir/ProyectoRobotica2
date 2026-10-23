# AFR2-20261023-04 - CAD/KiCad y Git global

Fecha: 2026-10-23. Pedido del usuario: crear el espacio interno para el diseño KiCad/3D, pieza nueva y modelo completo del auto; unificar Git y excluir dependencias/archivos recuperables.

Antes: cuatro Git independientes en Atmega, Contracts, ESP y Web, sin remotos y sin cambios pendientes; CADS contenía nueve insumos STEP/FreeCAD sin registro de origen. Después: un único Git en D:/ProyectoRobotica2, rama codex/monorepo-cad, y cinco módulos incluyendo AutoFinalRobo2-CAD. Los historiales anteriores se conservan como padres del commit de integración, con etiquetas y resguardo de los cuatro .git originales. Sin remotos/push.

CAD contiene kicad/oled-encoder, bibliotecas locales, mecanica/pieza-nueva, mecanica/auto-completo, mecanica/componentes, reference, docs y lugar para exports ignorados. Los nueve insumos se trasladan sin cambios de contenido desde CADS a CAD/reference/supplied; el inventario registra rutas, tamaño y SHA-256. Autoría/licencia/origen pendientes, sin suponer ajuste al robot ni equivalencia de motores/ruedas. No se crea geometría, esquemático ni ruteo nuevo.

La política global ignora dependencias instaladas, herramientas descargadas, cachés, builds, exportaciones y backups automáticos. Conserva manifests/lockfiles, fuentes editables, bibliotecas propias y evidencia. Insumos sin recuperación documentada siguen versionados. Documentacion/ y referencias históricas se preservan.

Archivos afectados: README/AGENTS/INFORME_MAESTRO/.gitignore/workspace raíz; README/AGENTS de los módulos existentes; en Contracts, README, INFORME_ADAPTACION, maestro, DECISIONES, ROADMAP, PROMPT_CONTINUIDAD, ESTRUCTURA_Y_GIT, PCB_OLED_ENCODER, CHANGELOG e índices/procedencia; documentación nueva de CAD y puntero CADS/README. Cada traslado de CAD figura en reference/insumos-locales.json. No se adaptan fuentes de firmware/Web ni se reescriben manifests históricos.

| Área | Estado real |
| --- | --- |
| atmega | Fuente OLED/UI parcial; backend/control pendientes; código sin cambio |
| esp | Esqueleto, bridge/configuración física pendientes; código sin cambio |
| web | Esqueleto, port funcional pendiente; código sin cambio |
| cad | Estructura e insumos organizados; diseños nuevos pendientes de planos |
| shared | Organización/documentación actualizadas; wire/capabilities sin cambio |

Sin tests, compilaciones, validadores, ERC/DRC, flasheos ni pruebas físicas. Hooks deshabilitados en las operaciones Git. No se afirma compatibilidad, fabricación viable ni dimensiones verificadas.
