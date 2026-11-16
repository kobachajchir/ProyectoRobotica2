# Informe de presentación y desarrollo del proyecto D2

12/11/2026 · AFR2-20261112-02 · Autoría documental de base: Koba Chajchir.

- [PDF diagramado para lectura, 49 páginas](entrega/AutoFinalRobo2_Informe_Proyecto_D2.pdf).
- [Word editable](entrega/AutoFinalRobo2_Informe_Proyecto_D2.docx).
- [Contenido íntegro Markdown](AutoFinalRobo2_Informe_Proyecto_D2.md).
- [Contenido estructurado](contenido_informe.json).
- [Índice con páginas del PDF](indice_paginas.json).
- [Borrador D1 conservado](../informe_20261109/README.md).
- [Ficha del cambio](../../changes/2026-11-12-02-informe-proyecto-d2.md).

## Contenido

23 capítulos, dos apéndices, portada e índice. Incluye 80 ecuaciones numeradas, 21 matrices, 27 tablas de contenido y cinco figuras numeradas, además del render de portada y la tabla de datos académicos. Las matrices incluyen transformaciones homogéneas, DH, orientación ZYZ, Jacobianos del brazo y del sistema integrado, Mecanum, escala direccional, GOTO y estado de motor DC.

Se desarrollan problema, objetivos, metodología, requisitos, diseño mecánico, fabricación, cinemática directa e inversa, singularidades, incertidumbre, transmisiones, pinza, odometría, PID discreto efectivo de la fuente, trayectorias quínticas, dinámica, rigidez, vuelco, alimentación, sensores, comunicaciones, recursos, resultados, riesgos e hitos de cierre. La portada Word contiene controles de texto editables para institución, asignatura, docente, integrantes y comisión/ciclo lectivo, conforme a la respuesta del usuario.

## Actualización y alcance de evidencia

La base D1 del 09/11 se conserva intacta. D2 integra los factores direccionales y la fuente 1.28.1 / EEPROM v13, contrastados por lectura con `afr2_geometry.h`, `afr2_product.h` y `afr2_product.c`. Se distingue la ley PID Q8 implementada del antiwindup completo propuesto, y la odometría de punto medio ya presente en fuente de una mejora futura.

Los resultados de base del 12/11 provienen de la ficha AFR2-20261112-01: firmware 1.28.0, 100 mm/s, sin carga, cinco repeticiones por movimiento. No se les asigna alcance al brazo ni a 1.28.1. El signo ± se conserva sin inventar su definición estadística. La reducción de tolerancias y la migración de giro siguen siendo cambios de fuente pendientes de demostrar en esa ficha.

Los resultados de masas y estabilidad se atribuyen al análisis CAD del 01/11/2026. No se reutilizan las masas anteriores al cierre de montajes J1/J4/J5 como si fueran las actuales. El modelo matemático explica relaciones y ejemplos analíticos; no constituye una nueva validación numérica, identificación dinámica ni aceptación física. Las envolventes y sus prompts diferidos no se ejecutaron.

## Procedencia y adaptación por archivo

| Archivo o grupo | Origen | Adaptación y estado |
| --- | --- | --- |
| `preparar_contenido.py` | Contenido D1 de `../informe_20261109/contenido_informe.json`, autoría Koba Chajchir | Adaptación documental D2, ampliación matemática y actualización por lectura de fuentes. Script propio de esta entrega; no ejecuta generadores heredados. |
| `contenido_informe.json` | D1 y ampliaciones propias de D2 | Contenido consolidado, referencias, estados y datos académicos. Fuente de la composición. |
| `generar_documentos.py` | Composición documental nueva; patrones ReportLab/python-docx/OMML del generador D1 | Nueva diagramación, índice PDF enlazado, ecuaciones numeradas, matrices con corchetes, tres esquemas originales y portada con campos Word. Sin importar código de producto. |
| `AutoFinalRobo2_Informe_Proyecto_D2.md` | Contenido consolidado D2 | Texto íntegro para lectura y revisión posterior. |
| `indice_paginas.json` | Composición final PDF | Ubicación real de capítulos en la edición de 49 páginas. |
| `render_pdf.py` | Script propio D2, API de pypdfium2 y Pillow del runtime | Render de documentos para revisión visual; no prueba firmware, geometría ni matemática. |
| `figuras/robot_ensamblado.png`, `brazo_v17.png` | Renders existentes en la carpeta D1 | Copia sin edición; geometría CAD v17. |
| `figuras/estabilidad.png` | CAD `revision_v17/analisis_conjunto_20261101/estabilidad_planta.png` | Copia sin edición, identificada como análisis histórico. |
| `figuras/arquitectura.png`, `cadena.png`, `odometria.png` | Esquemas originales generados por `generar_documentos.py` | Diagramas conceptuales desde las relaciones documentadas; no son planos ni geometría nueva. |
| `entrega/*.pdf`, `entrega/*.docx` | Fuentes D2 anteriores | Artefactos regenerables de entrega. No sustituyen D1. |
| `.gitignore` | Reglas locales de esta revisión | Excluye renders, figuras regenerables, entregas binarias y cachés; conserva las fuentes. |

No se atribuyen licencias nuevas a insumos existentes ni se alteran sus autores. La bibliografía técnica y las rutas de las fuentes se conservan en el capítulo 23.

## Revisión documental realizada

El PDF final se renderizó con el Python del runtime Codex y pypdfium2; se examinaron visualmente sus 49 páginas. Se corrigieron símbolos faltantes, subíndices con tildes, corchetes de matrices, números de ecuación, anchos de columnas, enlaces bibliográficos y saltos de página. Las imágenes intermedias permanecen en `revision_visual/`, excluida de Git.

El Word conserva texto, tablas, ecuaciones y matrices OMML editables. Se intentó el render con el `render_docx.py` de la skill documents. No pudo convertirse por ausencia de `soffice.exe` en el runtime disponible; no se utilizó LibreOffice de escritorio ni se instalaron programas. **La paginación y disposición visual del DOCX no se comprobaron en Word. El PDF es la referencia visual revisada.**

No se ejecutaron tests, compilaciones, validadores de proyecto, scripts heredados, nuevos cálculos CAD, puertos, cargas ni pruebas físicas. ATmega, ESP, Web, CAD y Loader no se modifican por esta revisión; no cambia versión ni contrato wire.
