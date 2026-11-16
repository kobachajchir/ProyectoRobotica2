# Presentación AutoFinalRobo2 · carpeta de documentación

Esta carpeta es **la base documental para armar el PowerPoint** (no contiene el `.pptx`). Cada filmina tiene su archivo con:
layout con zonas, textos listos para pegar, fórmulas, diagramas, fotos numeradas y dónde van, guion del orador, fuentes y
advertencias de veracidad.

Redactada el 11/11/2026 a partir del repositorio (producto 1.27.0). Autor del proyecto: Koba Chajchir.
No se compiló, ejecutó ni flasheó nada; todo número sale de una fuente citada en cada filmina.

## 1. Orden de la presentación (13 filminas)

| N.º | Filmina | Archivo | Bloque |
| --- | --- | --- | --- |
| 1 | Introducción y arquitectura del sistema | [Filmina_01_Introduccion.md](Filmina_01_Introduccion.md) | Proyecto |
| 2 | Modelos 3D: auto base, brazo v17 y conjunto | [Filmina_02_Modelos.md](Filmina_02_Modelos.md) | Proyecto |
| 3 | Pinout y conexionado | [Filmina_03_Pinout_y_conexionado.md](Filmina_03_Pinout_y_conexionado.md) | Proyecto |
| 4 | Firmwares: ATmega, ESP8266 y Web · ciclo principal | [Filmina_04_Firmwares_y_ciclo.md](Filmina_04_Firmwares_y_ciclo.md) | Proyecto |
| 5 | Matemática del auto (Mecanum) y cálculos de odometría | [Filmina_05_Matematica_auto_y_odometria.md](Filmina_05_Matematica_auto_y_odometria.md) | Proyecto |
| 6 | Programa GOTO y funcionamiento de la odometría | [Filmina_06_GOTO_y_odometria.md](Filmina_06_GOTO_y_odometria.md) | Proyecto |
| 7 | Matemática del brazo y del conjunto | [Filmina_07_Matematica_brazo_y_conjunto.md](Filmina_07_Matematica_brazo_y_conjunto.md) | Proyecto |
| 8 | Esfuerzos, tensiones, vuelco y estabilidad | [Filmina_08_Esfuerzos_y_vuelco.md](Filmina_08_Esfuerzos_y_vuelco.md) | Proyecto |
| 9 | Simulador, misión y ciclo de recolección | [Filmina_09_Simulador_y_mision.md](Filmina_09_Simulador_y_mision.md) | Proyecto |
| 10 | Interfaz: pasos y pantallas (OLED + Web) | [Filmina_10_Interfaz.md](Filmina_10_Interfaz.md) | Proyecto |
| 11 | **Agregado:** AFR2 Loader | [Filmina_11_Agregado_Loader.md](Filmina_11_Agregado_Loader.md) | Nuestros agregados |
| 12 | **Agregado:** KEY y RST automáticos con NPN | [Filmina_12_Agregado_NPN_KEY_RST.md](Filmina_12_Agregado_NPN_KEY_RST.md) | Nuestros agregados |
| 13 | **Agregado:** encoder y OLED | [Filmina_13_Agregado_Encoder_y_OLED.md](Filmina_13_Agregado_Encoder_y_OLED.md) | Nuestros agregados |

Informe visual de diagramación (wireframes con imágenes): [INFORME_DIAGRAMACION.html](INFORME_DIAGRAMACION.html) (se regenera con
`generar_informe_layout.py`) · Prompt de armado del PowerPoint: [PROMPT_ARMADO_PPTX.md](PROMPT_ARMADO_PPTX.md).

Material común: [FOTOS.md](FOTOS.md) (catálogo numerado de fotos y capturas) · [DIAGRAMAS.md](DIAGRAMAS.md) (diagramas D-01…D-10) ·
[ANEXO_Formulario.md](ANEXO_Formulario.md) (todas las fórmulas ordenadas, con ejemplos numéricos) ·
[ANEXO_Pendientes_y_alertas.md](ANEXO_Pendientes_y_alertas.md) (qué NO afirmar y qué datos faltan).

## 2. Contenido de la carpeta

```text
presentacion/
├─ README.md                      este índice y las reglas de diseño
├─ Filmina_01 … Filmina_13 .md    una ficha por filmina
├─ FOTOS.md                       catálogo FOTO-01…FOTO-69 (existentes ✔ / por tomar ✱ / capturas ⌗)
├─ DIAGRAMAS.md                   D-01…D-10, en qué filmina va cada uno
├─ ANEXO_Formulario.md            fórmulas + ejemplos numéricos verificados a mano
├─ ANEXO_Pendientes_y_alertas.md  límites de lo que se puede afirmar
├─ fotos_existentes/              imágenes que YA existen en el repo, copiadas con su número FOTO-NN
└─ diagramas/
   ├─ D-xx_*.svg                  diagramas vectoriales (insertar como imagen; editables)
   ├─ png/                        las mismas en PNG ×2 (por si el PowerPoint no abre SVG)
   └─ generar_diagramas.py        regenera los SVG (solo escribe en esa carpeta)
```

## 3. Reglas de diseño (para que las 13 filminas se vean como una sola)

**Formato.** 16:9 (33,867 × 19,05 cm). Márgenes 1,2 cm. Zona útil 31,5 × 16,6 cm. Título arriba a la izquierda;
pie con número de filmina y versión del producto (1.27.0).

**Grilla.** Cada filmina usa una grilla de 12 columnas × 6 filas (columna ≈ 2,6 cm, fila ≈ 2,8 cm). En cada ficha las zonas
están nombradas **A, B, C…** y llevan su rango de columnas/filas (`c1–c6 · f2–f5`). Respetar esas zonas es lo que mantiene el
orden. Sin más de 3 zonas de contenido por filmina, salvo las de fórmulas (hasta 4).

**Paleta** (la del sistema visual del proyecto):

| Uso | Color |
| --- | --- |
| Acento / títulos de zona | carmesí `#B81D33` |
| Texto | `#1F2430` |
| Fondo | `#FFFFFF` |
| Panel claro | `#F3F5F9` |
| Hardware / entradas | violeta `#6A4BB5` sobre `#EFE9FF` |
| Datos / software | azul `#2D6CB5` sobre `#E4F0FF` |
| Alerta / fallo | `#B81D33` sobre `#FDE7EA` |
| Correcto | verde `#2A8A52` sobre `#E3F5EA` |

**Tipografía.** Segoe UI (o Arial). Título 30 pt negrita · subtítulo de zona 16 pt negrita carmesí · texto 14 pt (mínimo 12 pt)
· fórmulas 16 pt en Cambria Math (Insertar → Ecuación; **no** pegar como imagen) · pie 10 pt.

**Fotos.** Cada foto lleva su rótulo `FOTO-NN` en una etiqueta carmesí pequeña (esquina inferior izquierda) y un pie de una línea.
Las capturas de pantalla se muestran con borde fino gris y sombra suave. Las que son **fotos reales por tomar** (✱) están
descritas con encuadre, fondo y qué debe verse en [FOTOS.md](FOTOS.md).

**Diagramas.** Son SVG con las mismas fuentes y colores; en PowerPoint: Insertar → Imágenes → Este dispositivo. Para editarlos:
clic derecho → *Convertir en forma*. Ancho recomendado: el de la zona asignada en cada ficha; no deformar.

**Fórmulas.** Todas están en [ANEXO_Formulario.md](ANEXO_Formulario.md) con símbolos y unidades. En las filminas se muestran
las esenciales; el resto va en las notas del orador.

## 4. Criterio de evidencia (decirlo en la filmina 1 y respetarlo en todas)

El informe del proyecto usa cuatro niveles; conviene mostrar un pequeño ícono por dato cuando haya duda:

1. **Dato nominal** (hoja de datos, contrato).
2. **Geometría CAD** (modelo Fusion v17).
3. **Implementado en código** (fuente leída; no prueba que funcione en la placa).
4. **Medido físicamente** (con instrumento y fecha).

Hoy hay pocas mediciones físicas (CPR 370, avance 1000 mm = 1000 mm, lateral 1000 mm → 775 mm). El resto de resultados son
CAD, estimaciones o código. Ver [ANEXO_Pendientes_y_alertas.md](ANEXO_Pendientes_y_alertas.md).

## 5. Cómo trabajar con esta carpeta

1. Armar la plantilla maestra del PowerPoint con las reglas de la sección 3 (patrón de diapositivas con título, pie y grilla).
2. Por cada filmina: abrir su ficha, crear las zonas, pegar los textos, insertar diagramas (`diagramas/`) y fotos
   (`fotos_existentes/` o las que se tomen con el número indicado en [FOTOS.md](FOTOS.md)).
3. Guardar las fotos nuevas con el nombre `FOTO-NN_descripcion.jpg` en una carpeta `fotos_nuevas/` (no existe aún) para que
   el número coincida con el catálogo.
4. Pasar el guion del orador a las notas de cada filmina.

Los datos de portada (institución, asignatura, docentes, integrantes) **no están en el repositorio**: completarlos.
