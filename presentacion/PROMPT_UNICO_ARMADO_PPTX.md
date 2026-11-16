# Prompt único de armado del PowerPoint (con espacios para insertar las imágenes a mano)

**Uso:** adjuntar a la IA **solo estos 4 archivos de texto** (carpeta `presentacion/ADJUNTOS/`; no hacen falta imágenes: las insertás vos después) y pegar el bloque de abajo.
Cada adjunto reúne varios documentos originales separados por marcas `===== INICIO DEL ARCHIVO: nombre =====`.

1. `ADJUNTO_1_README_y_filminas_01-04.md`
2. `ADJUNTO_2_filminas_05-08.md`
3. `ADJUNTO_3_filminas_09-13.md`
4. `ADJUNTO_4_catalogos_y_anexos.md`

**Resultado:** un `.pptx` de 13 filminas con todo el texto, tablas, fórmulas y notas, y **44 recuadros vacíos rotulados** con posición y
tamaño exactos en cm, donde pegás a mano los diagramas (`D-xx_*.png`, de `presentacion/diagramas/png/`) y las fotos (`FOTO-NN_*`, de
`presentacion/fotos_existentes/` o las que tomes). Cada recuadro dice qué archivo va ahí.

````text
ROL
Sos un diseñador técnico de presentaciones. Armá un archivo .pptx de 13 filminas para exponer el proyecto AutoFinalRobo2 (robot móvil
Mecanum con brazo de 6 ejes), bien técnico, bien detallado y bien ordenado. Todo el contenido está en los adjuntos de texto (.md); los
adjuntos están sueltos, sin carpetas, y se identifican SOLO por su nombre. NO tenés imágenes: no las busques, no las generes y no uses
imágenes de stock. En cada lugar de imagen dejá un RECUADRO VACÍO (placeholder) con el tamaño y la posición que se indican abajo,
para que yo inserte la imagen a mano.

ADJUNTOS (verificá que estén los 4 y avisame si falta alguno antes de empezar)
ADJUNTO_1_README_y_filminas_01-04.md, ADJUNTO_2_filminas_05-08.md, ADJUNTO_3_filminas_09-13.md, ADJUNTO_4_catalogos_y_anexos.md.
Cada uno contiene varios documentos, cada uno entre las marcas "===== INICIO DEL ARCHIVO: <nombre> =====" y "===== FIN DEL ARCHIVO: <nombre> =====".
Los nombres que uso abajo (README.md, Filmina_NN_*.md, FOTOS.md, DIAGRAMAS.md, ANEXO_*.md) son esos documentos internos.
Algunas fichas mencionan rutas con carpetas (diagramas/..., fotos_existentes/...): ignoralas, no existen para vos.

LEER PRIMERO (en este orden)
1. README.md, secciones 3 y 4 (reglas de diseño, paleta, tipografía, criterio de evidencia). Ignorá la sección 2.
2. Las 13 fichas Filmina_NN_*.md: mensaje clave, layout, contenido por zona, fórmulas, guion del orador, fuentes y advertencias.
3. FOTOS.md y DIAGRAMAS.md (descripción de cada imagen que va en un recuadro).
4. ANEXO_Formulario.md y ANEXO_Pendientes_y_alertas.md.

FORMATO GLOBAL
- Lienzo 16:9 de 33,867 × 19,05 cm (13,333 × 7,5 pulgadas). Fondo blanco.
- Título de cada filmina: x=1,27 cm y=0,76 cm w=31,3 cm h=2,0 cm; 28 pt negrita, color #1F2430, alineado a la izquierda, sin punto final, mismo lugar en todas.
- Pie de cada filmina: x=1,27 cm y=18,05 cm w=20 cm h=0,64 cm; 9 pt gris #5A6275: "Filmina N · AutoFinalRobo2 · producto 1.27.0".
- Paleta: acento carmesí #B81D33 · texto #1F2430 · fondo #FFFFFF · panel #F3F5F9 · bordes #D5D9E2 · gris #5A6275 · hardware #6A4BB5 sobre #EFE9FF ·
  software #2D6CB5 sobre #E4F0FF · fallo #B81D33 sobre #FDE7EA · correcto #2A8A52 sobre #E3F5EA.
- Tipografía Calibri (o Segoe UI si está disponible): título 28 pt negrita; subtítulo de zona 14 pt negrita carmesí; texto 11–13 pt
  (viñetas con interlineado compacto); tablas 10,5–11 pt; fórmulas en Cambria Math 12–14 pt. Nunca menos de 10,5 pt salvo el pie.
- Zonas de texto: panel #F3F5F9 sin borde, sangría interna de 0,3 cm, subtítulo carmesí arriba y las viñetas debajo, alineadas a la izquierda.
- Zonas de fórmulas: caja blanca con borde #D5D9E2 de 1 pt, subtítulo carmesí y las ecuaciones en Cambria Math; las líneas de nota (las que
  empiezan con "//" en las fichas) en Calibri gris, un punto menos. Si el formato permite ecuaciones editables, usalas; si no, texto.
- Tablas: encabezado en negrita color carmesí sin relleno, líneas finas #D5D9E2 entre filas, texto a la izquierda, filas de unos 0,85 cm.
- Sin franjas decorativas, sin líneas de acento bajo los títulos, sin emojis (salvo el ⭐ que las fichas usan como marca de "agregado propio").
- Texto que no entra: no lo achiques por debajo del mínimo; recortá palabras o agrandá la zona (sin invadir otra) y avisame.

PLACEHOLDERS DE IMAGEN (lo más importante)
Cada línea "[PLACEHOLDER DE IMAGEN]" de la lista de zonas es un rectángulo vacío que yo voy a rellenar a mano:
- Rectángulo con relleno #F3F5F9 y borde punteado carmesí #B81D33 de 1 pt, EXACTAMENTE en la posición y el tamaño indicados.
- Dentro, centrado, en tres líneas: (1) el código en negrita carmesí 13 pt (por ejemplo "FOTO-03" o "D-01"), (2) la descripción en gris 11 pt tomada de
  FOTOS.md o DIAGRAMAS.md, (3) "insertar: <nombre de archivo>" en cursiva gris 10 pt, con el nombre que indica la lista de zonas.
- Poné el texto alternativo del rectángulo con el nombre del archivo y asignale el nombre de objeto "IMG_<código>" para encontrarlo en el panel de selección.
- Debajo de cada placeholder grande (más de 5 cm de alto) reservá una línea de pie de 0,6 cm (10 pt gris) con "<código> · <pie de la imagen de la ficha>" dentro del
  alto indicado (el rectángulo ocupa el alto menos 0,6 cm).
- No uses emojis, íconos ni dibujos dentro del placeholder, y no dejes ninguna imagen real.
- Los recuadros no se deforman ni se superponen con nada; el espacio entre zonas es de 0,2 cm.

GRILLA (de dónde salen las coordenadas)
Contenido en una grilla de 12 columnas × filas f2…f7 (f1 = título). Ancho de columna 2,61 cm; alto de fila 2,92 cm; origen x=1,27 cm, y=3,18 cm;
separación entre zonas 0,2 cm. "c1–4" = columnas 1 a 4; "f2–4,4" = desde el inicio de la fila 2 hasta 2,4 filas después. Usá siempre los
x, y, w, h en cm de la lista; la grilla es solo para que entiendas el orden.

LISTA DE ZONAS POR FILMINA (posiciones en cm; [texto]/[tabla]/[fórmulas] se rellenan con el contenido de la ficha indicada;
[PLACEHOLDER DE IMAGEN] se deja vacío con el rótulo descrito arriba)

FILMINA 1 · título exacto: "AutoFinalRobo2 · manipulador móvil con base Mecanum 4WD" (ficha: Filmina_01_Introduccion.md)
   [texto] Subtítulo de autoría · x=1,3 y=2,6 w=31,2 h=0,7 cm · 'Koba Chajchir · [institución · asignatura · docentes · integrantes: completar]' en carmesí negrita 12 pt, y=2,6 cm (resaltar los corchetes para completar)
   [PLACEHOLDER DE IMAGEN] FOTO-01 · robot completo CAD · c1–4 f2–7 · x=1.4 y=3.3 w=10.2 h=14.4 cm · insertar: FOTO-01_robot_completo_CAD_v17.png (fondo transparente: poner fondo #F3F5F9 detrás)
   [PLACEHOLDER DE IMAGEN] D-01 · arquitectura · c5–12 f2–4.4 · x=11.8 y=3.3 w=20.7 h=6.8 cm · insertar: D-01_arquitectura_general.png
   [texto] Problema y objetivo (viñetas) · c5–8 f4.4–7 · x=11.8 y=10.3 w=10.2 h=7.4 cm · Ficha 1, zona C + criterio de evidencia + cifras (4 ruedas + 6 ejes · 7 servos · 54 comandos · 101 pantallas · 1.27.0)
   [tabla] Seis módulos, un único Git · c9–12 f4.4–7 · x=22.3 y=10.3 w=10.2 h=7.4 cm · Ficha 1, zona D: 7 filas × 2 columnas (Módulo | Qué es)

FILMINA 2 · título exacto: "Modelos 3D: auto base, brazo v17 y conjunto" (ficha: Filmina_02_Modelos.md)
   [PLACEHOLDER DE IMAGEN] FOTO-03 · auto base · c1–4 f2–4 · x=1.4 y=3.3 w=10.2 h=5.6 cm · insertar: FOTO-03_auto_base_frente_blender.png (FOTO-05/06 opcionales)
   [texto] A · Auto base (viñetas) · c1–4 f4–7 · x=1.4 y=9.1 w=10.2 h=8.6 cm · Ficha 2, zona A
   [PLACEHOLDER DE IMAGEN] FOTO-07 · brazo v17 · c5–8 f2–4 · x=11.8 y=3.3 w=10.2 h=5.6 cm · insertar: FOTO-07_brazo_v17_CAD.png
   [tabla] B · Brazo v17: cotas · c5–8 f4–7 · x=11.8 y=9.1 w=10.2 h=8.6 cm · Ficha 2, zona B: tabla de cotas, transmisiones y actuadores
   [PLACEHOLDER DE IMAGEN] FOTO-01 · conjunto · c9–12 f2–4 · x=22.3 y=3.3 w=10.2 h=5.6 cm · insertar: FOTO-01_robot_completo_CAD_v17.png (fondo #F3F5F9)
   [texto] C · Conjunto (viñetas) · c9–12 f4–6 · x=22.3 y=9.1 w=10.2 h=5.6 cm · Ficha 2, zona C
   [PLACEHOLDER DE IMAGEN] FOTO-08 · captura Web #/modelos · c9–12 f6–7 · x=22.3 y=15.0 w=10.2 h=2.7 cm · captura por hacer: FOTO-08_captura_web_modelos.png

FILMINA 3 · título exacto: "Pinout y conexionado: Mega 2560 + shield OSOYOO" (ficha: Filmina_03_Pinout_y_conexionado.md)
   [tabla] Motores (IN1 IN2 PWM) · c1–3 f2–4 · x=1.4 y=3.3 w=7.6 h=5.6 cm · Ficha 3, bloque 1
   [tabla] Hall (A/B) · c1–3 f4–7 · x=1.4 y=9.1 w=7.6 h=8.6 cm · Ficha 3, bloque 2 (Hall)
   [tabla] Sensores, enlace y agregados ⭐ · c4–6 f2–7 · x=9.2 y=3.3 w=7.6 h=14.4 cm · Ficha 3, bloques 2 y 3: IR, HC-SR04, servo, batería A5, UART D18/D19, OLED, encoder, D32/D33, USB
   [PLACEHOLDER DE IMAGEN] D-02 · conexionado · c7–12 f2–4.4 · x=17.0 y=3.3 w=15.5 h=6.8 cm · insertar: D-02_conexionado.png
   [PLACEHOLDER DE IMAGEN] FOTO-12 · pinout shield · c7–9 f4.4–7 · x=17.0 y=10.3 w=7.6 h=7.4 cm · insertar: FOTO-12_shield_osoyoo_pinout.jpg
   [PLACEHOLDER DE IMAGEN] FOTO-14/15 · cableado real · c10–12 f4.4–7 · x=24.9 y=10.3 w=7.6 h=7.4 cm · foto por tomar: FOTO-14_mega_shield_cableado.jpg y FOTO-15_motores_hall.jpg

FILMINA 4 · título exacto: "Firmwares: qué son, qué hacen y ciclo principal" (ficha: Filmina_04_Firmwares_y_ciclo.md)
   [texto] ATmega2560 (viñetas) · c1–6 f2–3.7 · x=1.4 y=3.3 w=15.5 h=4.8 cm · Ficha 4, zona A
   [texto] ESP8266 (viñetas) · c1–6 f3.7–5.4 · x=1.4 y=8.2 w=15.5 h=4.8 cm · Ficha 4, zona B
   [texto] Web (viñetas) · c1–6 f5.4–7 · x=1.4 y=13.2 w=15.5 h=4.5 cm · Ficha 4, zona C + chip Contracts
   [PLACEHOLDER DE IMAGEN] D-03a · ciclo principal · c7–12 f2–7 · x=17.0 y=3.3 w=15.5 h=14.4 cm · insertar: D-03a_ciclo_principal_ATmega.png (D-03b como apoyo en notas)

FILMINA 5 · título exacto: "Matemática del auto Mecanum y cálculos de odometría" (ficha: Filmina_05_Matematica_auto_y_odometria.md)
   [fórmulas] Cinemática Mecanum · c1–6 f2–5.4 · x=1.4 y=3.3 w=15.5 h=9.7 cm · Ficha 5, zona A, 6 líneas, fuente 14 pt
   [tabla] Medido por nosotros · c1–6 f5.4–7 · x=1.4 y=13.2 w=15.5 h=4.5 cm · Ficha 5, zona D (3 mediciones)
   [PLACEHOLDER DE IMAGEN] D-11 · patrones de rueda · c7–12 f2–4.2 · x=17.0 y=3.3 w=15.5 h=6.2 cm · insertar: D-11_patrones_rueda_mecanum.png
   [fórmulas] Odometría (cada 10 ms) · c7–12 f4.2–7 · x=17.0 y=9.7 w=15.5 h=8.0 cm · Ficha 5, zona C, 5 líneas, fuente 13 pt

FILMINA 6 · título exacto: "Programa GOTO y funcionamiento de la odometría" (ficha: Filmina_06_GOTO_y_odometria.md)
   [PLACEHOLDER DE IMAGEN] D-10 · pipeline odometría · c1–4 f2–7 · x=1.4 y=3.3 w=10.2 h=14.4 cm · insertar: D-10_pipeline_odometria.png
   [PLACEHOLDER DE IMAGEN] D-04 · flujo GOTO · c5–8 f2–7 · x=11.8 y=3.3 w=10.2 h=14.4 cm · insertar: D-04_flujo_GOTO.png
   [fórmulas] Ley de control · c9–12 f2–5.4 · x=22.3 y=3.3 w=10.2 h=9.7 cm · Ficha 6, zona C
   [PLACEHOLDER DE IMAGEN] FOTO-21/22 · Web GOTO y odometría · c9–12 f5.4–7 · x=22.3 y=13.2 w=10.2 h=4.5 cm · capturas por hacer: FOTO-21_web_goto.png y FOTO-22_web_prueba_odometria.png

FILMINA 7 · título exacto: "Matemática del brazo y del conjunto" (ficha: Filmina_07_Matematica_brazo_y_conjunto.md)
   [PLACEHOLDER DE IMAGEN] D-08 · cadena cinemática · c2–11 f2–3.65 · x=4.0 y=3.3 w=25.9 h=4.6 cm · insertar: D-08_cadena_cinematica.png
   [fórmulas] Cinemática directa · c1–4 f3.7–7 · x=1.4 y=8.2 w=10.2 h=9.4 cm · Ficha 7, zona B
   [fórmulas] Cinemática inversa · c5–8 f3.7–7 · x=11.8 y=8.2 w=10.2 h=9.4 cm · Ficha 7, zona C
   [fórmulas] Conjunto y Jacobiano · c9–12 f3.7–6 · x=22.3 y=8.2 w=10.2 h=6.5 cm · Ficha 7, zona D
   [PLACEHOLDER DE IMAGEN] FOTO-25/26 · simulador brazo y conjunta · c9–12 f6–7 · x=22.3 y=15.0 w=10.2 h=2.7 cm · capturas por hacer: FOTO-25_web_sim_brazo.png y FOTO-26_web_sim_conjunta.png

FILMINA 8 · título exacto: "Esfuerzos, pares y vuelco: factor frontal 1,023 frente a 1,2 exigido" (ficha: Filmina_08_Esfuerzos_y_vuelco.md)
   [tabla] Masas (CAD, PLA macizo) · c1–4 f2–4.6 · x=1.4 y=3.3 w=10.2 h=7.4 cm · Ficha 8, zona A (8 filas)
   [tabla] Par: STS3250 16 kgf·cm = 1,569 N·m · c5–8 f2–4.6 · x=11.8 y=3.3 w=10.2 h=7.4 cm · Ficha 8, zona B (J2–J5)
   [PLACEHOLDER DE IMAGEN] FOTO-29 · centro de masa · c9–12 f2–4.6 · x=22.3 y=3.3 w=10.2 h=7.4 cm · insertar: FOTO-29_estabilidad_planta_CG.png (FOTO-30 opcional)
   [fórmulas] Método · c1–8 f4.6–7 · x=1.4 y=10.9 w=20.7 h=6.8 cm · Ficha 8, zona D
   [tabla] Vuelco, brazo horizontal con 200 g · c9–12 f4.6–7 · x=22.3 y=10.9 w=10.2 h=6.8 cm · Ficha 8, zona E (lateral 3,63 · atrás 1,86 · adelante 1,023)

FILMINA 9 · título exacto: "Simulador, misión y ciclo de recolección" (ficha: Filmina_09_Simulador_y_mision.md)
   [texto] Cómo está definido el simulador · c1–4 f2–4.5 · x=1.4 y=3.3 w=10.2 h=7.1 cm · Ficha 9, zona A
   [texto] Misión del recolector · c1–4 f4.5–7 · x=1.4 y=10.6 w=10.2 h=7.1 cm · Ficha 9, zonas C
   [PLACEHOLDER DE IMAGEN] FOTO-33 · perspectiva · c5–6 f2–3.8 · x=11.8 y=3.3 w=5.0 h=5.1 cm · captura por hacer: FOTO-33_sim_perspectiva.png
   [PLACEHOLDER DE IMAGEN] FOTO-34 · vista superior · c7–8 f2–3.8 · x=17.0 y=3.3 w=5.0 h=5.1 cm · captura por hacer: FOTO-34_sim_superior.png
   [PLACEHOLDER DE IMAGEN] FOTO-35 · lateral · c9–10 f2–3.8 · x=22.3 y=3.3 w=5.0 h=5.1 cm · captura por hacer: FOTO-35_sim_lateral.png
   [PLACEHOLDER DE IMAGEN] FOTO-36 · radar · c11–12 f2–3.8 · x=27.5 y=3.3 w=5.0 h=5.1 cm · captura por hacer: FOTO-36_sim_radar.png
   [PLACEHOLDER DE IMAGEN] D-05 · ciclo de misión · c5–9 f3.8–7 · x=11.8 y=8.5 w=12.8 h=9.1 cm · insertar: D-05_ciclo_mision_recolectora.png
   [PLACEHOLDER DE IMAGEN] FOTO-37 · preagarre · c10–12 f3.8–5 · x=24.9 y=8.5 w=7.6 h=3.3 cm · captura por hacer: FOTO-37_sim_preagarre.png
   [PLACEHOLDER DE IMAGEN] FOTO-38 · elevación y transporte · c10–12 f5–6 · x=24.9 y=12.0 w=7.6 h=2.7 cm · captura por hacer: FOTO-38_sim_transporte.png
   [PLACEHOLDER DE IMAGEN] FOTO-39 · suelta en celda · c10–12 f6–7 · x=24.9 y=15.0 w=7.6 h=2.7 cm · captura por hacer: FOTO-39_sim_suelta.png

FILMINA 10 · título exacto: "Interfaz: pasos de uso y pantallas (OLED + encoder y Web)" (ficha: Filmina_10_Interfaz.md)
   [texto] Pasos de uso 1→6 · c1–4 f2–7 · x=1.4 y=3.3 w=10.2 h=14.4 cm · Ficha 10, zona A, 12 pt
   [PLACEHOLDER DE IMAGEN] FOTO-48 · OLED Inicio · c5–6 f2–3.2 · x=11.8 y=3.3 w=5.0 h=3.3 cm · foto por tomar: FOTO-48_oled_inicio.jpg
   [PLACEHOLDER DE IMAGEN] FOTO-49/50 · OLED Menú y Estado · c7–8 f2–3.2 · x=17.0 y=3.3 w=5.0 h=3.3 cm · fotos por tomar: FOTO-49_oled_menu.jpg y FOTO-50_oled_estado.jpg
   [PLACEHOLDER DE IMAGEN] FOTO-51/52 · OLED Radar y Guardar · c9–10 f2–3.2 · x=22.3 y=3.3 w=5.0 h=3.3 cm · fotos por tomar: FOTO-51_oled_radar.jpg y FOTO-52_oled_guardar.jpg
   [PLACEHOLDER DE IMAGEN] FOTO-53 · silueta del autito · c11–12 f2–3.2 · x=27.5 y=3.3 w=5.0 h=3.3 cm · insertar: FOTO-53_autito_silueta_OLED_128x64.png (fondo negro)
   [PLACEHOLDER DE IMAGEN] D-07 · navegación del encoder · c5–9 f3.2–5.6 · x=11.8 y=6.8 w=12.8 h=6.8 cm · insertar: D-07_navegacion_OLED_encoder.png
   [tabla] Reglas de entrada · c10–12 f3.2–5.6 · x=24.9 y=6.8 w=7.6 h=6.8 cm · Ficha 10, zona E (5 filas)
   [PLACEHOLDER DE IMAGEN] FOTO-41…47 · Web (7 capturas) · c5–12 f5.6–7 · x=11.8 y=13.8 w=20.7 h=3.9 cm · capturas por hacer: FOTO-41_web_inicio … FOTO-47_web_ayuda_oled (.png)

FILMINA 11 · título exacto: "Agregado propio: AFR2 Loader, compilar, cargar y depurar" (ficha: Filmina_11_Agregado_Loader.md)
   [texto] Qué resuelve · c1–4 f2–7 · x=1.4 y=3.3 w=10.2 h=14.4 cm · Ficha 11, zona A (6 viñetas)
   [PLACEHOLDER DE IMAGEN] FOTO-54 · Loader Operación · c5–8 f2–4.2 · x=11.8 y=3.3 w=10.2 h=6.2 cm · insertar: FOTO-54_loader_operacion.png (rotular 'captura v1.2.0')
   [PLACEHOLDER DE IMAGEN] FOTO-55 · Loader Flasher · c9–12 f2–4.2 · x=22.3 y=3.3 w=10.2 h=6.2 cm · insertar: FOTO-55_loader_flasher.png (rotular 'captura v1.2.0')
   [PLACEHOLDER DE IMAGEN] D-09 · flujo del Loader · c5–12 f4.2–6.5 · x=11.8 y=9.7 w=20.7 h=6.5 cm · insertar: D-09_flujo_Loader.png
   [texto] Línea de datos · c5–12 f6.5–7 · x=11.8 y=16.4 w=20.7 h=1.3 cm · 'Fuente 2.4.0 · distribución 2.0.0 (ZIP + SHA-256) · C# / Windows · portátil · MANUAL.html · el flasheo con hardware aún no está comprobado' 11 pt gris

FILMINA 12 · título exacto: "Agregado propio: KEY y RST automáticos con transistores NPN" (ficha: Filmina_12_Agregado_NPN_KEY_RST.md)
   [PLACEHOLDER DE IMAGEN] D-06 · circuito NPN · c1–7 f2–6 · x=1.4 y=3.3 w=18.1 h=11.5 cm · insertar: D-06_circuito_KEY_RST_NPN.png
   [PLACEHOLDER DE IMAGEN] FOTO-61 · shield KEY/RST · c8–12 f2–4 · x=19.6 y=3.3 w=12.8 h=5.6 cm · insertar: FOTO-61_shield_vista_superior_KEY_RST.jpg
   [texto] Problema y solución · c8–12 f4–6 · x=19.6 y=9.1 w=12.8 h=5.6 cm · Ficha 12, zona C
   [PLACEHOLDER DE IMAGEN] FOTO-62 · circuito armado · c1–4 f6–7 · x=1.4 y=15.0 w=10.2 h=2.7 cm · foto por tomar: FOTO-62_circuito_armado.jpg
   [PLACEHOLDER DE IMAGEN] FOTO-63 · medición con tester · c5–8 f6–7 · x=11.8 y=15.0 w=10.2 h=2.7 cm · foto por tomar: FOTO-63_tester.jpg
   [PLACEHOLDER DE IMAGEN] FOTO-64 · cables de colores · c9–12 f6–7 · x=22.3 y=15.0 w=10.2 h=2.7 cm · foto por tomar: FOTO-64_cables_colores.jpg

FILMINA 13 · título exacto: "Agregado propio: panel con encoder y OLED" (ficha: Filmina_13_Agregado_Encoder_y_OLED.md)
   [PLACEHOLDER DE IMAGEN] FOTO-65 · panel montado · c1–2 f2–4 · x=1.4 y=3.3 w=5.0 h=5.6 cm · foto por tomar: FOTO-65_panel_montado.jpg
   [PLACEHOLDER DE IMAGEN] FOTO-66 · PCB en KiCad 3D · c3–4 f2–4 · x=6.6 y=3.3 w=5.0 h=5.6 cm · captura por hacer: FOTO-66_pcb_kicad.png
   [tabla] Conectores · c1–4 f4–7 · x=1.4 y=9.1 w=10.2 h=8.6 cm · Ficha 13, zona D (J1, J2, OLED, PCB)
   [texto] Encoder KY-040 por interrupción · c5–8 f2–6 · x=11.8 y=3.3 w=10.2 h=11.5 cm · Ficha 13, zona B
   [texto] OLED SSD1306 128×64 · c9–12 f2–6 · x=22.3 y=3.3 w=10.2 h=11.5 cm · Ficha 13, zona C
   [PLACEHOLDER DE IMAGEN] FOTO-67 · PCB + soporte en Fusion · c5–6 f6–7 · x=11.8 y=15.0 w=5.0 h=2.7 cm · captura por hacer: FOTO-67_pcb_soporte_fusion.png
   [PLACEHOLDER DE IMAGEN] FOTO-68 · esquema KiCad · c7–9 f6–7 · x=17.0 y=15.0 w=7.6 h=2.7 cm · captura por hacer: FOTO-68_esquema_kicad.png
   [PLACEHOLDER DE IMAGEN] FOTO-69 · OLED encendido · c10–12 f6–7 · x=24.9 y=15.0 w=7.6 h=2.7 cm · foto por tomar: FOTO-69_oled_encendido.jpg

CONTENIDO DE CADA ZONA NO IMAGEN
- Copiá los textos, las tablas y las fórmulas de la ficha de esa filmina, en la zona de igual nombre, con las cifras exactas. Condensá a viñetas cortas
  (máximo 2 líneas por viñeta) si el texto largo de la ficha no entra; las fichas tienen más detalle del que cabe: lo que sobra va a las notas.
- Notas del orador de cada filmina: "GUION" (sección "Guion del orador" de la ficha, sin las comillas «»), "FUENTES" y "ADVERTENCIAS" (secciones con esos nombres),
  más una línea "IMÁGENES A INSERTAR" con los códigos de los placeholders de esa filmina.
- Filmina 1: sin zona de cifras aparte (las cifras van en la zona de problema y objetivo). Filmina 4: el panel de Web incluye la línea de Contracts (54 comandos).
- Filmina 5: tres zonas de contenido más una de patrones (placeholder D-11); filmina 7: cuatro zonas de fórmulas/placeholder.

REGLAS DE CONTENIDO (obligatorias)
- Mostrá solo lo que dicen las fichas; no inventes datos ni cifras. Respetá los niveles de evidencia: dato nominal · CAD · código · medido.
- Datos medidos propios (únicos): 370 cuentas/vuelta; avance 1000 mm = 1000 mm; lateral 1000 mm = 775 mm reales (factor 0,775).
- Filmina 5: L = 170 mm (no 220). Filmina 7: eslabones del descriptor v17 (70/60/55/65/78,5 mm), no 130/120.
- Filmina 8: mensaje central = factor de vuelco frontal 1,023 frente al criterio 1,2; aclarar "resultado histórico del 01/11/2026, sobre CAD, sin FEA".
- Filmina 9: "simulación local, no misión física verificada". Filmina 11: capturas de la v1.2.0 y flasheo con hardware sin comprobar. Filmina 12: circuito
  sin armar ni medir. Filmina 13: montaje a 5 V informado; el CAD conserva +3V3; sin ERC/DRC.
- Filmina 1: resaltar en carmesí los campos a completar [institución · asignatura · docentes · integrantes]; autor Koba Chajchir.
- No afirmes nada que ANEXO_Pendientes_y_alertas.md prohíba afirmar. Si dos fichas se contradicen, avisame en lugar de elegir en silencio.
- Tiempos sugeridos para 20 min: 1,5 · 1,5 · 1,5 · 2 · 2 · 1,5 · 2 · 2 · 1,5 · 1,5 · 1 · 1 · 1,5 (ponelos en las notas).

ENTREGA
1. El archivo .pptx con las 13 filminas, notas y los 44 placeholders. Si no podés generar .pptx, decímelo al principio y entregá filmina por filmina
   el contenido de cada zona (con su x, y, w, h) y los placeholders descritos, listo para pegar.
2. Al final, en el chat, una tabla "IMÁGENES A INSERTAR" con una fila por placeholder: filmina · código · nombre de archivo · x, y, w, h en cm.
3. Un informe breve: adjuntos que faltaron, texto que tuviste que recortar, datos que no pudiste ubicar e inconsistencias detectadas.
4. Verificación visual si podés renderizar: sin texto cortado, sin zonas solapadas, sin fuentes menores a 10,5 pt, todos los placeholders en su posición.
   Si no podés renderizar, decilo.
````

## Dónde va cada imagen (resumen para insertarlas a mano)

- **Diagramas** (carpeta `presentacion/diagramas/png/`): D-01 filmina 1 · D-02 filmina 3 · D-03a filmina 4 · D-11 filmina 5 · D-10 y D-04 filmina 6 ·
  D-08 filmina 7 · D-05 filmina 9 · D-07 filmina 10 · D-09 filmina 11 · D-06 filmina 12. (D-03b queda como apoyo en las notas de la filmina 4.)
- **Fotos existentes** (carpeta `presentacion/fotos_existentes/`): FOTO-01 (filminas 1 y 2) · FOTO-03 y FOTO-07 (2) · FOTO-12 (3) · FOTO-29 (8) ·
  FOTO-53 (10) · FOTO-54 y FOTO-55 (11) · FOTO-61 (12).
- **Fotos y capturas por hacer:** las demás (FOTO-08, 14/15, 21/22, 25/26, 33–39, 41–52, 62–69); su descripción y cómo tomarlas está en `FOTOS.md`.
- Al insertar una imagen: clic derecho sobre el placeholder → copiar posición y tamaño, o simplemente insertar la imagen y ponerle los mismos x, y, w, h
  (Formato de imagen → Tamaño y posición, con "Bloquear relación de aspecto" y recortando si hace falta).
