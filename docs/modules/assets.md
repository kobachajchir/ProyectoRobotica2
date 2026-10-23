# Recursos, estilos y empaquetado

[Maestro](../../../INFORME_MAESTRO.md) · [Informe del repositorio](../../INFORME_ADAPTACION.md).

## Utilidad

Se conservan recursos necesarios para leer/reutilizar la UI anterior; el GLB es el robot previo y no representa Mecanum.

## Adaptación

Crear identidad/modelo del nuevo robot; retirar 3D pesado si excede LittleFS/bundle. El package activo es React/Vite mínimo, sin postbuild hacia otro repo. El empaquetador produce artifacts/littlefs/dist dentro de Web, no publica ni flashea. Versiones iniciales tomadas del manifest origen; falta instalar/fijar lock propio y revisar requisitos Node. No se ejecutó build/npm.

## Evidencia y alcance

- [reference/web/public](../../reference/web/public)
- [reference/web/src/assets](../../reference/web/src/assets)
- [reference/web/package.json.upstream.txt](../../reference/web/package.json.upstream.txt)
- [tools/package-web.mjs](../../tools/package-web.mjs)

Hallazgos: AFRDOC-005. [Registro](../../../AutoFinalRobo2-Contracts/documentation/review/FINDINGS.md).

Estado: lectura/copia o fuente parcial indicada; sin compilación, tests ni prueba física. La API detallada de origen está en el corpus AMDOC importado; no equivale a API AVR adaptada.
