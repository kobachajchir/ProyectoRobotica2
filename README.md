# AutoFinalRobo2-web

Leer [informe de adaptación](INFORME_ADAPTACION.md) y [maestro](../INFORME_MAESTRO.md).

reference/web conserva src/public y manifests históricos inertes *.upstream.txt. src/ activo es una pantalla de estado React; no conecta al robot ni simula sensores. Reutilizar componentes gradualmente tras cerrar contrato. package.json propio no tiene postbuild entre repos. tools/package-web.mjs prepara gzip local (no ejecutado); promover assets a ESP en una etapa explícita. Falta instalación/lock/build y adaptación funcional.

[Contrato compartido](../AutoFinalRobo2-Contracts/README.md). No se han ejecutado compilaciones, tests, validaciones ni flasheos.
