# AutoFinalRobo2-web

Leer [informe de adaptación](INFORME_ADAPTACION.md) y [maestro](../INFORME_MAESTRO.md).

reference/web conserva src/public y manifests históricos inertes *.upstream.txt. src/ activo es una pantalla de estado React; no conecta al robot ni simula sensores. Reutilizar componentes gradualmente tras cerrar contrato. package.json propio no tiene postbuild entre repos. tools/package-web.mjs prepara gzip local (no ejecutado); promover assets a ESP en una etapa explícita. Falta instalación/lock/build y adaptación funcional.

[Contrato compartido](../AutoFinalRobo2-Contracts/README.md). No se han ejecutado compilaciones, tests, validaciones ni flasheos.

Organización AFR2-20261023-04: este módulo pertenece al único Git de D:/ProyectoRobotica2. [CAD/KiCad, pieza nueva y auto completo](../AutoFinalRobo2-CAD/README.md) · [Git e historial](../AutoFinalRobo2-Contracts/docs/ESTRUCTURA_Y_GIT.md). No se modificó código de este módulo al reorganizar el proyecto.
