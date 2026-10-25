> AFR2-20261024-15: versión/fecha de empaquetado en manifiesto, identidad del índice y diagnóstico FS mostrados desde ESP; no hash recalculado en placa. Paquete ESP 1480773 bytes gzip, siete activos. [Contrato](../../../AutoFinalRobo2-Contracts/contracts/SYSTEM_INFO_FILESYSTEM_15.md). Cifras inferiores históricas.

> Vigente **AFR2-20261024-04**: **modelo final incluido en ESP**, paquete **1.450.201 bytes gzip** (49,96% menos que los 2,90 MB iniciales). WebP, JS/CSS minificados y dependencias/componentes optimizados. Layout candidato: flash 4 MiB / LittleFS nominal 3 MiB, solicitado por el usuario; flash fisica sin identificar. Web full/esp y firmware ESP compilados; activos promovidos a data/dist. Sin navegador, imagen FS ni flasheo. Atmega y comandos conservan el estado de02. [Evidencia](../../../AutoFinalRobo2-Contracts/changes/2026-10-24-04-final-model-littlefs.md). Los estados previos inferiores son historicos.

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
