# AFR2-20261110-07 — Carpeta común de revisión del informe, planos y pinout

10/11/2026. Pedido: reunir los materiales en una misma carpeta dentro de Revision para revisarlos e ir creando nuevas versiones hasta el informe final.

## Entrega

[Revision/R01_20261110](../../Revision/R01_20261110/README.md), con:

- `01_Informe/`: borrador D1 PDF/DOCX/Markdown, capítulos, figuras, tres partes y contenido JSON.
- `02_Planos/`: libro nominal de 224 hojas A3, seis tomos, índices, BOM, interfaces, metadatos y fuente editable F3D. Fichas y láminas iniciales del 02/11 como antecedentes.
- `03_Pinout/`: HTML actualizado del 07/11 como documento principal, PDF del 06/11 y mapa anterior como antecedentes, textos y fuentes de datos.
- Índice común, observaciones, historial y `ORIGENES.json` con la procedencia de los 66 archivos copiados.

`Revision/README.md` y `REVISIONES.csv` identifican R01 como entrega activa en revisión. Las próximas entregas se guardarán como R02/R03 con fecha, conservando las anteriores; todavía no se crea una nueva revisión ni se aprueba el informe final. La numeración del paquete no modifica la versión D1 del documento ni la versión del software.

## Alcance y estado

Se copiaron los archivos, sin mover o sobrescribir originales y sin cambiar el contenido técnico. No se regeneraron PDF, secciones del informe ni planos. Se distingue explícitamente el pinout HTML del 07/11 del PDF anterior del 06/11. Se preservan Documentacion/, CAD y el informe maestro.

Los scripts originales de regeneración permanecen en sus módulos y sus rutas constan en el manifiesto; el paquete contiene los materiales de revisión y fuentes editables. El script de organización `Revision/preparar_R01_20261110.ps1` se ejecutó para crear las copias y evita sobrescribir una R01 existente.

Estado: Contracts/Revision=documentación organizada; CAD=solo copias de entrega, sin cambio del modelo; atmega/esp/web/Loader=sin cambios. Sin tests, compilaciones, validaciones, renders, puertos, flasheos, commit ni publicación. No corresponde subir la versión de producto por ser documentación.
