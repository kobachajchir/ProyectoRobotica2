# Primer borrador del informe · D1

09/11/2026. Informe separado del maestro y de `Documentacion/`.

- [Informe PDF, 32 páginas](AFR2_Informe_Borrador_D1.pdf).
- [Informe editable DOCX](AFR2_Informe_Borrador_D1.docx).
- [Índice y capítulos individuales](INDICE.md).
- [Texto completo Markdown](AFR2_Informe_Borrador_D1.md).
- [Libro de planos complementario](../../../AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/revision_v17/laminas_20261109/README.md).

También se entrega en tres PDF para consulta por partes:

1. [Proyecto y mecánica](partes/AFR2_Informe_D1_01_Proyecto_y_mecanica.pdf).
2. [Modelos matemáticos](partes/AFR2_Informe_D1_02_Modelos_matematicos.pdf).
3. [Sistema, resultados y fuentes](partes/AFR2_Informe_D1_03_Sistema_resultados_y_fuentes.pdf).

Incluye problema y objetivos, arquitectura de seis módulos, dimensiones v17, estado de planos, marcos y transformaciones homogéneas, DH equivalente, cinemática directa e inversa, Jacobiano, singularidades, incertidumbre, transmisiones y pinza, matrices Mecanum con signos del contrato, encoders y odometría, control y trayectorias, manipulación móvil, estática/dinámica, resultados históricos de estabilidad, electrónica, sensores, comunicaciones, mejoras y pendientes. Contiene 23 capítulos numerados, dos apéndices y portada.

Las dos figuras son renders de geometría CAD ya existente. Las fórmulas y matrices del DOCX son elementos OMML editables; las tablas y el texto también son editables. `contenido_informe.json` y `generar_informe.py` son fuentes; ReportLab, python-docx y Arial producen PDF/DOCX, y `dividir_informe.py` usa pypdfium2 para separar las partes. `indice_paginas.json` conserva la ubicación de capítulos del PDF.

Los resultados del análisis del 01/11 se identifican como históricos. La derivación matemática no constituye una nueva comprobación de cinemática o estabilidad. Los comentarios antiguos que mencionan 130/120 mm no reemplazan al descriptor v17. El informe explicita las tareas no implementadas y los datos físicos que faltan. Los datos académicos de carátula permanecen por completar.

No se ejecutaron tests, compilaciones, validaciones, ensayos físicos ni flasheos. No se modificó código de producto ni su versión. Las envolventes y sus prompts siguen pendientes del aviso de diseño terminado solicitado por el usuario.
