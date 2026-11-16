# AFR2-20261112-06 - Fabricación 3D y filminas en el informe

12/11/2026. Pedido directo: explicar fabricación del brazo por impresión 3D, con relleno 25 % en housings y zonas no cargadas, 80 % en torre J1--J2 y soportes sometidos a momentos y engranajes/transmisión impresos macizos. Agregar enlace a las filminas existentes en el repositorio.

## Archivos y adaptación

- `docs/informe_robotica_latex/01_proyecto.tex`: subsección de fabricación con tabla 25/80/100 %, recorrido de carga y relación entre fuerza, brazo perpendicular y momento. Se distinguen envolventes de soportes estructurales.
- `05_dinamica.tex`: se aclara que el porcentaje de infill no escala directamente la masa sólida; paredes y capas exteriores también aportan masa.
- `06_resultados.tex`: el presupuesto CAD y la estabilidad conservan sus hipótesis de densidad sólida; no se inventa un nuevo cálculo para los rellenos indicados.
- `08_anexos.tex`: enlace GitHub y enlace local al archivo existente `presentacion/AutoFinalRobo2_Presentacion.pptx`. URL remota derivada del origin/rama configurados, sin comprobación de publicación; copia local existente, filminas sin cambios.
- `README.md`, `PROCEDENCIA.md`, PDF y ZIP de fuentes: actualizados, conservando autores y figuras.

## Estado real

| Módulo | Estado por este cambio |
| --- | --- |
| atmega | Sin cambios ni compilación, carga o pruebas. |
| esp | Sin cambios ni compilación, carga o pruebas. |
| web | Sin cambios ni build o pruebas. |
| CAD | Sin cambios ni nuevos análisis, laminados o pesajes. |
| Loader | Sin cambios ni ejecución. |
| Contracts | Solo documentación y sus entregables; sin cambio de contrato wire o versión de producto. |

Informe LaTeX recompuesto y renderizado: 40 páginas. Revisión visual de portada/índice, nueva sección de fabricación, texto de masas/dinámica, resultados y anexo. Compilación exclusivamente documental, con paquetes locales ya disponibles. Sin tests, validadores de proyecto, compilaciones de firmware, flasheos ni pruebas físicas.
