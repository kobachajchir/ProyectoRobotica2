# Procedencia y adaptaciones por archivo

Registro documental separado del informe entregable. Autoría del proyecto: Koba Chajchir y Santiago Baccon, según la portada del PDF aportado por el usuario; se conserva la autoría de los insumos existentes.

Base de contenido: `C:/Users/kobac/OneDrive/Escritorio/PDFRobotica2.pdf`, documentación y fuentes propias de AutoFinalRobo2. Se consultó la edición documental D2 del proyecto como insumo técnico. El PDF final y el paquete de fuentes se redactan como documento autocontenido y no remiten a ediciones anteriores.

| Archivo | Procedencia y adaptación |
| --- | --- |
| `AutoFinalRobo2.tex` | Nuevo archivo principal y portada con datos académicos del PDF del usuario; resumen e índice reorganizados. |
| `estilo.sty` | Diseño LaTeX original para esta entrega: A4, Latin Modern, jerarquía, colores, encabezados y notación matemática. |
| `01_proyecto.tex` | Síntesis y nueva organización de objetivos, requisitos, mecánica y arquitectura a partir del contenido aportado y la documentación del proyecto. |
| `02_electronica.tex` | Reorganización de electrónica y comunicaciones; relaciones físicas y modelo de motor expresados en LaTeX. Pinout y navegación conservan los contratos del proyecto. |
| `03_movilidad.tex` | Desarrollo de matrices Mecanum, encoders, odometría y control; diferencias entre evidencia 1.28.0 y fuente 1.28.1 preservadas. |
| `04_manipulador.tex` | Cadena geométrica, DH equivalente, cinemática, Jacobianos y pinza reescritos con notación consistente. El centro de muñeca usa C para diferenciarlo del mundo W. |
| `05_dinamica.tex` | Desarrollo integrado de manipulación móvil, planificación, masas, dinámica, resistencia, estabilidad e incertidumbre. No se inventan inercias ni ensayos. |
| `06_resultados.tex` | Tablas y análisis de evidencia de base del 12/11/2026 y de cálculos CAD existentes; hipótesis y limitaciones explícitas. |
| `07_cierre.tex` | Redacción autónoma de aportes, priorización cualitativa, secuencia de integración y conclusiones. |
| `08_anexos.tex` | Nomenclatura, parámetros y conversiones; bibliografía externa primaria de Modern Robotics y FEETECH. |
| `figuras/robot.png` | Copia sin edición de `docs/informe_20261109/figuras/robot_ensamblado.png`, render del CAD del proyecto. |
| `figuras/brazo.png` | Copia sin edición de `docs/informe_20261109/figuras/brazo_v17.png`, render del manipulador v17. |
| `figuras/estabilidad.png` | Copia sin edición del gráfico `AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/revision_v17/analisis_conjunto_20261101/estabilidad_planta.png`; no se ejecuta nuevamente el análisis. |
| `render_documento.py` | Script editorial nuevo para rasterizar el PDF mediante pypdfium2 y generar planchas de revisión con Pillow. |
| `preparacion.tex` | Archivo auxiliar local de preparación tipográfica; fuera del paquete entregable. |
| `LEEME.md` | Instrucciones nuevas de edición y compilación, incluidas en el ZIP. |
| `README.md`, `PROCEDENCIA.md`, `.gitignore` | Organización y registro de esta edición; fuera del ZIP de fuentes salvo las instrucciones LEEME. |
| `salida/*.pdf`, `salida/*.zip` | Exportación LaTeX y empaquetado de las fuentes; recursos regenerables. |

Compilación documental con Tectonic 0.17.0 portátil, sin instalación global. Binario descargado de la publicación oficial `https://github.com/tectonic-typesetting/tectonic/releases/tag/tectonic%400.17.0`. SHA-256 del ZIP descargado: `f61ce51f0b0ade1015b7de7ef368541c5424e9756ecbd0d7af97d6d48030845f`. Herramientas y caché quedan excluidas de Git y de la entrega. La última compilación utiliza solo paquetes ya almacenados.

## Actualización de fabricación y filminas

Instrucción directa del usuario: impresión 3D de piezas del brazo con 25 % de relleno para housings y secciones no cargadas, 80 % para secciones que soportan momentos (torre J1--J2 y soportes) y construcción maciza para engranajes y transmisión. Incorporada en `01_proyecto.tex`; `05_dinamica.tex` y `06_resultados.tex` explican su relación con masas e inercias sin recalcular resultados ni inventar pesajes.

`08_anexos.tex`: se incorpora el PowerPoint existente `D:/ProyectoRobotica2/presentacion/AutoFinalRobo2_Presentacion.pptx`. Su existencia local fue comprobada. Enlace GitHub construido a partir de `origin` y rama configurada `codex/monorepo-cad`; disponibilidad remota no comprobada en esta edición. Se incluye además enlace relativo a la copia local desde la carpeta del PDF. No se modificaron las filminas.

`README.md`, PDF y ZIP actualizados: 40 páginas. Registro: AFR2-20261112-06. Autores y figuras conservados.
