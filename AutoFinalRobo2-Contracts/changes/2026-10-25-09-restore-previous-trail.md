# AFR2-20261025-09 — Restaurar07 y suavizar solo color

25/10/2026. El usuario pide volver a la versión anterior de los chevrones,
conservando únicamente el color más suave. Se restaura diseño07, anterior a08:
geometría360×240, tamaño, posición, máscara/degradé globales y animación.
El único ajuste visual sobre07 es opacity:0.58 en el SVG completo.

Inspección de hashes: ManualDriveOverlay coincide exactamente con el hash
previo a08; CSS coincide al descontar esa propiedad y comentario de registro.
ManualDirectionTrail de08 se retira tras comprobar que seguía idéntico a
la fuente propia entregada08, con copia local en .tmp/revert09. No se usa Git
reset/checkout ni se descartan cambios ajenos. Cámara52%07 conservada.

Entrada local04, foco/captura03, callbacks físicos, modelo y unidades intactos.
Firmware0.9.0/EX01v7/build01,30comandos/59ScreenCodes y gates0 sin cambios.
Web compilada/promovida09, ESP solo activos. Sin nueva dependencia, CAD,
navegador/teléfono, tests/validadores independientes, flasheo o pruebas físicas.
VIS-032 registra la preferencia humana; aspecto no revisado en navegador.
[Contrato](../contracts/WEB_TOUCH_SIMULATION.md),
[build09](../documentation/builds/2026-10-25-09/README.md),
[procedencia](../provenance/restore-previous-trail-20261025.json).
