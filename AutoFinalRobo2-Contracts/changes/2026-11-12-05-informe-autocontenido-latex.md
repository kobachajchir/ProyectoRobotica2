# AFR2-20261112-05 - Informe autocontenido en LaTeX

12/11/2026. El usuario aportó `PDFRobotica2.pdf` y solicitó un informe completo, autocontenido, mejor separado y sin menciones a informes anteriores, con ecuaciones bien presentadas en LaTeX.

## Entrega documental

[Informe y fuentes](../docs/informe_robotica_latex/README.md). PDF de 38 páginas con siete capítulos, anexo y bibliografía; fuentes LaTeX editables con las figuras incluidas en ZIP. Portada con institución, materia, docentes e integrantes tomados del PDF adjunto. [Procedencia por archivo](../docs/informe_robotica_latex/PROCEDENCIA.md) conservada fuera de los entregables.

Se recompone la matemática con fracciones, radicales, matrices, ecuaciones numeradas y referencias cruzadas nativas. Se reorganizan presentación, electrónica, movilidad, manipulador, dinámica/estabilidad, resultados e integración. El informe no remite a las ediciones documentales previas. No se atribuye al ajuste de fuente 1.28.1 la evidencia física obtenida con 1.28.0, ni se presentan como ensayadas capacidades físicas del manipulador.

## Estado real por módulo

| Módulo | Estado por esta ficha |
| --- | --- |
| atmega | Sin cambios de código, compilación, carga ni pruebas. |
| esp | Sin cambios de código, compilación, carga ni pruebas. |
| web | Sin cambios de código, build ni pruebas. |
| CAD | Sin modelado, cambios ni nuevos análisis; figuras y cálculos existentes usados como evidencia. |
| Loader | Sin ejecución ni cambios; no se operan puertos ni equipos. |
| Contracts | Nueva documentación LaTeX y registro de procedencia. Sin cambios de contrato wire ni versión de producto. |

## Revisión editorial

Compilación del documento con Tectonic 0.17.0 portátil y renderizado del PDF. Examen visual de las 38 páginas, con correcciones de glifos, títulos, matrices, tablas y separación de contenido. Esta revisión documental no es una validación matemática por software ni una prueba del producto.

No se ejecutaron tests, compilaciones de firmware, validadores de proyecto, scripts heredados, flasheos ni ensayos físicos. La compilación realizada corresponde exclusivamente al informe LaTeX solicitado. Herramienta portátil y caché local excluidas de Git y del ZIP entregable.
