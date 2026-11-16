# Informe de Robótica 2 - AutoFinalRobo2

Informe autocontenido de presentación y desarrollo del proyecto. Contiene siete capítulos, anexo de símbolos y parámetros y bibliografía. Las ecuaciones, fracciones, matrices y referencias cruzadas se componen directamente con LaTeX.

## Editar y generar el PDF

Abrir `AutoFinalRobo2.tex` como archivo principal. Los capítulos están separados en los archivos `01_proyecto.tex` a `08_anexos.tex`; `estilo.sty` contiene el diseño y `figuras/` las imágenes necesarias. Los datos de la portada se editan en el archivo principal.

En Overleaf, subir todo el contenido de este paquete y seleccionar **XeLaTeX** como compilador. También se puede compilar localmente con una distribución TeX que incluya los paquetes utilizados:

```sh
xelatex AutoFinalRobo2.tex
xelatex AutoFinalRobo2.tex
```

La segunda pasada actualiza el índice, las citas y los números de referencias. Como alternativa, Tectonic resuelve las pasadas automáticamente:

```sh
tectonic AutoFinalRobo2.tex
```

No se necesitan fuentes comerciales ni archivos externos al paquete. Se utiliza Latin Modern.

## Contenido

1. Presentación, alcance, requisitos y arquitectura.
2. Electrónica, sensores, alimentación y comunicaciones.
3. Cinemática Mecanum, odometría y control.
4. Manipulador: transformaciones, DH, cinemática directa e inversa, Jacobianos y pinza.
5. Manipulación móvil, trayectorias, dinámica, resistencia y estabilidad.
6. Resultados disponibles y análisis del desempeño.
7. Integración, evaluación y conclusiones.

El documento diferencia resultados medidos, cálculos sujetos a hipótesis y condiciones de integración pendientes. Los modelos no sustituyen la caracterización física.

Autores del proyecto indicados en portada: Koba Chajchir y Santiago Baccon. Edición documental: 12 de noviembre de 2026.
