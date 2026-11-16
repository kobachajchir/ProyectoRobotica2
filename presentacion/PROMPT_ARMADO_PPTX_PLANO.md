# Prompt de armado para una IA con archivos sueltos (sin carpetas)

Para una IA que solo recibe archivos adjuntos, sin jerarquía de carpetas ni acceso al disco. Todas las referencias son por **nombre de archivo**.
Los nombres ya son únicos, así que se pueden subir todos juntos sin renombrar.

## 1. Qué archivos pasarle

Si el chat limita la cantidad de adjuntos, subirlos por tandas **en este orden** (cada tanda con el mismo prompt, avisando «tanda 2 de 3»).

### Tanda 1 · documentos (18 archivos, todos `.md`) · obligatorios
Salen de la carpeta `presentacion/`.

| Archivo | Para qué |
| --- | --- |
| `README.md` | reglas de diseño, paleta, tipografía, criterio de evidencia |
| `Filmina_01_Introduccion.md` | ficha de la filmina 1 |
| `Filmina_02_Modelos.md` | ficha de la filmina 2 |
| `Filmina_03_Pinout_y_conexionado.md` | ficha de la filmina 3 |
| `Filmina_04_Firmwares_y_ciclo.md` | ficha de la filmina 4 |
| `Filmina_05_Matematica_auto_y_odometria.md` | ficha de la filmina 5 |
| `Filmina_06_GOTO_y_odometria.md` | ficha de la filmina 6 |
| `Filmina_07_Matematica_brazo_y_conjunto.md` | ficha de la filmina 7 |
| `Filmina_08_Esfuerzos_y_vuelco.md` | ficha de la filmina 8 |
| `Filmina_09_Simulador_y_mision.md` | ficha de la filmina 9 |
| `Filmina_10_Interfaz.md` | ficha de la filmina 10 |
| `Filmina_11_Agregado_Loader.md` | ficha de la filmina 11 |
| `Filmina_12_Agregado_NPN_KEY_RST.md` | ficha de la filmina 12 |
| `Filmina_13_Agregado_Encoder_y_OLED.md` | ficha de la filmina 13 |
| `FOTOS.md` | catálogo FOTO-01…FOTO-69 y estado de cada una |
| `DIAGRAMAS.md` | catálogo D-01…D-11 |
| `ANEXO_Formulario.md` | fórmulas y ejemplos numéricos |
| `ANEXO_Pendientes_y_alertas.md` | qué NO afirmar |

No hace falta `INFORME_DIAGRAMACION.html` (sus imágenes dependen de carpetas) ni los `.py`.

### Tanda 2 · diagramas (12 PNG, de `presentacion/diagramas/png/`) · obligatorios
`D-01_arquitectura_general.png` · `D-02_conexionado.png` · `D-03a_ciclo_principal_ATmega.png` · `D-03b_servicio_de_control.png` ·
`D-04_flujo_GOTO.png` · `D-05_ciclo_mision_recolectora.png` · `D-06_circuito_KEY_RST_NPN.png` · `D-07_navegacion_OLED_encoder.png` ·
`D-08_cadena_cinematica.png` · `D-09_flujo_Loader.png` · `D-10_pipeline_odometria.png` · `D-11_patrones_rueda_mecanum.png`

### Tanda 3 · fotos existentes (18 imágenes, de `presentacion/fotos_existentes/`)
`FOTO-01_robot_completo_CAD_v17.png` · `FOTO-03_auto_base_frente_blender.png` · `FOTO-04_auto_base_atras_blender.png` ·
`FOTO-05_auto_base_lado_derecho_fusion.png` · `FOTO-06_auto_base_lado_izquierdo_fusion.png` · `FOTO-07_brazo_v17_CAD.png` ·
`FOTO-11_dimensiones_chasis.png` · `FOTO-12_shield_osoyoo_pinout.jpg` · `FOTO-13_shield_mapa_pines_wifi.png` ·
`FOTO-29_estabilidad_planta_CG.png` · `FOTO-30_cargas_y_momentos.png` · `FOTO-53_autito_silueta_OLED_128x64.png` ·
`FOTO-54_loader_operacion.png` · `FOTO-55_loader_flasher.png` · `FOTO-56_loader_monitor_serie.png` · `FOTO-57_loader_configuracion.png` ·
`FOTO-58_loader_mcp.png` · `FOTO-61_shield_vista_superior_KEY_RST.jpg` · y `LOGO_proyecto.png` (opcional, portada).

**No subir** `FOTO-10_rueda_mecanum.jpg` (muestra un chasis, no la rueda) ni `FOTO-32_propuesta_montaje_recolector.svg` (SVG).

### Después, las fotos nuevas
Las fotos y capturas que todavía no existen (FOTO-02, 08, 09, 14…28, 31, 33…52, 59, 60, 62…69) **se suben cuando las tengas**, con el nombre
`FOTO-NN_descripcion.jpg`. La IA debe dejar marcadores para ellas (ver prompt).

## 2. El prompt

````text
ROL
Sos un diseñador técnico de presentaciones. Armá una presentación de 13 filminas sobre el proyecto AutoFinalRobo2 (robot móvil Mecanum
con brazo de 6 ejes). No tenés acceso a carpetas: todos los archivos te llegan adjuntos, sueltos, y se identifican SOLO por su nombre.
Donde las fichas mencionen rutas con carpetas (por ejemplo "diagramas/D-01_arquitectura_general.svg" o "fotos_existentes/FOTO-03_..."),
ignorá la ruta y buscá el adjunto cuyo nombre empiece igual (D-01…, FOTO-03…). Si existe el .png de un diagrama, usá el .png en lugar del .svg.
No inventes datos: cada número sale de las fichas. No uses imágenes de stock ni generes imágenes propias para reemplazar fotos.

ADJUNTOS QUE DEBERÍAS TENER (verificalo antes de empezar y listame los que falten)
1. Documentos: README.md · Filmina_01_Introduccion.md … Filmina_13_Agregado_Encoder_y_OLED.md (13 fichas) · FOTOS.md · DIAGRAMAS.md ·
   ANEXO_Formulario.md · ANEXO_Pendientes_y_alertas.md
2. Diagramas PNG: D-01_arquitectura_general · D-02_conexionado · D-03a_ciclo_principal_ATmega · D-03b_servicio_de_control · D-04_flujo_GOTO ·
   D-05_ciclo_mision_recolectora · D-06_circuito_KEY_RST_NPN · D-07_navegacion_OLED_encoder · D-08_cadena_cinematica · D-09_flujo_Loader ·
   D-10_pipeline_odometria · D-11_patrones_rueda_mecanum
3. Fotos: FOTO-01, 03, 04, 05, 06, 07, 11, 12, 13, 29, 30, 53, 54, 55, 56, 57, 58, 61 (y LOGO_proyecto.png)
Si llegaron por tandas, trabajá con lo que tengas y avisá qué filminas quedaron incompletas por falta de archivos.

LEER PRIMERO (en este orden)
1. README.md, secciones 3 y 4 (reglas de diseño, paleta, tipografía, criterio de evidencia). Ignorá la sección 2 (estructura de carpetas).
2. Las 13 fichas Filmina_NN_*.md. Cada una trae: mensaje clave, layout en grilla 12×6 con zonas A, B, C… y sus rangos (c = columnas, f = filas),
   contenido por zona, fórmulas, guion del orador, fuentes y advertencias.
3. FOTOS.md (qué es cada FOTO-NN y su estado: ✔ existe, ✱ por tomar, ⌗ por capturar, ◐ rehacer) y DIAGRAMAS.md.
4. ANEXO_Formulario.md y ANEXO_Pendientes_y_alertas.md.

FORMATO GLOBAL
- 16:9 (33,867 × 19,05 cm), márgenes 1,2 cm. Grilla 12 columnas × 6 filas; fila 1 = título. Respetá los rangos de cada zona (ej. "A · c1–c4 · f2–f6").
- Paleta: acento #B81D33, texto #1F2430, fondo #FFFFFF, panel #F3F5F9, hardware #6A4BB5/#EFE9FF, software #2D6CB5/#E4F0FF,
  fallo #B81D33/#FDE7EA, correcto #2A8A52/#E3F5EA.
- Tipografía sans-serif legible (Segoe UI o Calibri): título 28–30 pt negrita; subtítulo de zona 14–16 pt negrita carmesí; texto 12–14 pt
  (nunca menos de 11 pt en tablas); fórmulas en fuente matemática (Cambria Math); pie 9–10 pt con "Filmina N · producto 1.27.0".
- Fórmulas: si el formato lo permite, como ecuaciones editables; si no, como texto con la fuente matemática, sin convertirlas en imagen.
- Máximo 3 zonas de contenido por filmina (hasta 4 si hay fórmulas). Alineación estricta a la grilla; sin texto sobre fotos; nada de rayas decorativas bajo los títulos.
- Cada foto lleva una etiqueta pequeña "FOTO-NN" y un pie de una línea. Los diagramas no se deforman: escalar proporcionalmente (ajustar sin recortar).
- El guion del orador, las fuentes y las advertencias de cada ficha van en las notas de la filmina.

MAPA DE IMÁGENES (qué adjunto va en qué zona; el detalle manda en la ficha)
 1: A FOTO-01 · B D-01
 2: A FOTO-03 (+05/06 miniaturas) · B FOTO-07 · C FOTO-01
 3: B D-02 · C FOTO-12 (FOTO-13 opcional)
 4: D D-03a (D-03b como apoyo)
 5: B D-11 (FOTO-11 opcional en notas)
 6: A D-10 · B D-04
 7: A D-08
 8: C FOTO-29 (FOTO-30 opcional)
 9: D D-05
10: B FOTO-53 · C D-07
11: B FOTO-54 y FOTO-55 (FOTO-56/57/58 opcionales) · C D-09; rotular "capturas de la versión 1.2.0"
12: A D-06 · B FOTO-61
13: sin imágenes adjuntas (solo marcadores)
FOTO-10 y FOTO-32 NO se usan.

FOTOS QUE NO TENGO (✱ por tomar, ⌗ por capturar)
Cualquier FOTO-NN que no esté entre los adjuntos: NO la inventes ni la sustituyas. Insertá un marcador de posición del tamaño de la zona
(fondo #F3F5F9, borde punteado carmesí) con el texto "FOTO-NN · descripción · por tomar", tomando la descripción de FOTOS.md.
Al final devolveme la lista completa de marcadores (FOTO-NN · filmina · zona) para poder reemplazarlos.

MAPA DE FILMINAS (resumen; el detalle manda en cada ficha)
 1 Introducción y arquitectura      A foto · B D-01 · C problema/objetivo · D seis módulos · E cifras
 2 Modelos 3D                       A auto · B brazo + cotas · C conjunto · D datos clave
 3 Pinout y conexionado             A tablas de pinout (3 bloques) · B D-02 · C FOTO-12 · D marcador FOTO-14/15
 4 Firmwares y ciclo                A ATmega · B ESP8266 · C Web · D D-03a · E enlace compartido
 5 Matemática del auto/odometría    A cinemática Mecanum · B D-11 · C odometría · D medidos · E parámetros
 6 GOTO y odometría                 A D-10 · B D-04 · C ley de control · D marcador FOTO-21/22
 7 Matemática brazo y conjunto      A D-08 · B directa · C inversa · D conjunto+Jacobiano · E marcador FOTO-25/26
 8 Esfuerzos, tensiones, vuelco     A masas · B pares/márgenes · C FOTO-29 · D método · E tabla de vuelco
 9 Simulador, misión, ciclo         A definición · B marcadores FOTO-33…36 · C misión · D D-05 · E marcadores FOTO-37…39
10 Interfaz: pasos y pantallas      A pasos 1→6 · B marcadores FOTO-48…52 + FOTO-53 · C D-07 · D marcadores FOTO-41…47 · E reglas de entrada
11 Agregado: AFR2 Loader            A qué resuelve · B FOTO-54/55 · C D-09 · D chips
12 Agregado: NPN KEY/RST            A D-06 · B FOTO-61 · C problema→solución · D marcadores FOTO-62…64
13 Agregado: encoder y OLED         A marcadores FOTO-65/66 · B encoder · C OLED · D conectores · E marcadores FOTO-67/68/69

REGLAS DE CONTENIDO (obligatorias)
- Mostrá solo lo que dice la ficha. Respetá los niveles de evidencia: dato nominal · CAD · código · medido. Marcá con un ícono discreto lo que NO
  está medido físicamente.
- Datos medidos propios (únicos): 370 cuentas/vuelta; avance 1000 mm = 1000 mm; lateral 1000 mm = 775 mm reales (factor 0,775).
- Filmina 5: usar L = 170 mm (no 220). Filmina 7: eslabones del descriptor v17 (70/60/55/65/78,5 mm), no 130/120.
- Filmina 8: factor de vuelco frontal 1,023 frente al criterio 1,2 como mensaje central; aclarar "resultado histórico del 01/11/2026, sobre CAD".
- Filmina 9: aclarar "simulación local, no misión física verificada". Filmina 11: flasheo real sin comprobar. Filmina 12: circuito sin armar ni medir.
  Filmina 13: montaje a 5 V informado; el CAD conserva +3V3; sin ERC/DRC.
- Portada/filmina 1: dejá resaltados los campos [institución · asignatura · docentes · integrantes] para completar; autor: Koba Chajchir.
- Tiempos sugeridos (20 min): 1,5 · 1,5 · 1,5 · 2 · 2 · 1,5 · 2 · 2 · 1,5 · 1,5 · 1 · 1 · 1,5.
- Si algo de las fichas se contradice con otra ficha, avisame en vez de elegir en silencio.

ENTREGA
1. El archivo .pptx con las 13 filminas, notas del orador y marcadores de foto. Si no podés generar archivos .pptx, entregame el contenido filmina
   por filmina en el orden y con las zonas de las fichas, listo para pegar, y decímelo al principio.
2. Un informe breve en el chat: filminas armadas, marcadores pendientes (lista FOTO-NN), adjuntos que faltaron, datos que no pudiste ubicar e
   inconsistencias detectadas.
3. Verificación visual: si podés renderizar las filminas a imagen, revisá que no haya texto cortado, solapamientos, fuentes demasiado chicas ni
   elementos fuera de su zona; corregí y volvé a revisar. Si no podés renderizar, decílo.
````

## 3. Notas para quien lo usa

- Las fichas todavía mencionan carpetas en algunos links; el prompt ya le dice a la IA que los ignore y busque por nombre.
- El PNG de cada diagrama pesa 0,1–0,3 MB; las fotos CAD pesan hasta 1,7 MB. Si el chat limita el tamaño total, subir primero las 13 fichas y los 12 diagramas.
- Los datos de portada y las fotos reales las agregás vos al final; el prompt deja marcadores para todo lo que falte.
