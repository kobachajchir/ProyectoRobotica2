# AFR2-20261027-14 - Loader como módulo propio en la raíz

27/10/2026. El usuario solicita que el cargador forme parte del proyecto en
una carpeta propia dentro de la raíz, al mismo nivel que los demás módulos.

## Cambio

Se mueve completa la carpeta `AutoFinalRobo2-Contracts/tools/AFR2Loader` a
[`AutoFinalRobo2-Loader`](../../AutoFinalRobo2-Loader/README.md). Incluye C#,
lanzadores, plantilla de perfil, README y manual. No se crea Git anidado.
El repositorio pasa a seis módulos: Atmega, ESP, Web, Contracts, CAD y Loader.

AGENTS.md raíz, README raíz, central documental, informe maestro, organización
Git y referencias del cargador se actualizan. Loader recibe un AGENTS.md propio.
Contracts conserva los registros AFR2; el código del cargador deja de pertenecer
a tools de Contracts. El workspace ya abre la raíz completa: no necesita una
segunda entrada ni cambio de ruta.

Iniciar.cmd/Iniciar.ps1 resuelven su carpeta por ubicación; Profile busca la raíz
en los ancestros. No fue necesario cambiar la lógica de detección por moverlo.
La carpeta LOCALAPPDATA con perfiles/ejecutable/registros se conserva.

## Estado

Solo traslado y documentación. Sin cambios funcionales, compilación, tests,
validadores, ejecución de interfaz, apertura de puertos ni flasheo. El cargador
mantiene el estado de fuente sin ejecutar/probar de13. Atmega/ESP/Web/CAD no se
modifican en esta entrega. No se crea commit ni se publica Git.
