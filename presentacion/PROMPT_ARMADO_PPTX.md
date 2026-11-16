# Prompt completo para armar el PowerPoint de AutoFinalRobo2

Copiar el bloque de abajo en el asistente (Claude con la skill `pptx`, o cualquier generador de presentaciones con acceso a la carpeta).
Rutas relativas a `D:/ProyectoRobotica2/`. Antes de pedir el armado, mirar el layout visual en
[INFORME_DIAGRAMACION.html](INFORME_DIAGRAMACION.html) (abrir en el navegador).

````text
ROL
Sos un diseñador técnico de presentaciones. Armá un archivo .pptx de 13 filminas para exponer el proyecto AutoFinalRobo2
(robot móvil Mecanum con brazo de 6 ejes). Todo el contenido está documentado en D:/ProyectoRobotica2/presentacion/.
No inventes datos: cada número sale de las fichas y sus fuentes. No edites nada fuera de presentacion/. No ejecutes
compilaciones, tests ni flasheos. Guardá el resultado en presentacion/AutoFinalRobo2_Presentacion.pptx.

LEER PRIMERO (en este orden)
1. presentacion/README.md                  reglas de diseño, paleta, tipografía, criterio de evidencia
2. presentacion/INFORME_DIAGRAMACION.html  wireframe de las 13 filminas con zonas A, B, C… y rangos de grilla
3. presentacion/Filmina_01_Introduccion.md … Filmina_13_Agregado_Encoder_y_OLED.md   una ficha por filmina
4. presentacion/FOTOS.md                   catálogo FOTO-01…FOTO-69, estados ✔ existe / ✱ por tomar / ⌗ captura / ◐ rehacer
5. presentacion/DIAGRAMAS.md               D-01…D-11 y en qué zona va cada uno
6. presentacion/ANEXO_Formulario.md        fórmulas para Insertar → Ecuación
7. presentacion/ANEXO_Pendientes_y_alertas.md   qué NO afirmar

FORMATO GLOBAL
- 16:9 (33,867 × 19,05 cm), márgenes 1,2 cm. Grilla 12 columnas × 6 filas; fila 1 = título. Respetar los rangos de cada zona
  (ej. "A · c1–c4 · f2–f6").
- Paleta: acento #B81D33, texto #1F2430, fondo #FFFFFF, panel #F3F5F9, hardware #6A4BB5/#EFE9FF, software #2D6CB5/#E4F0FF,
  fallo #B81D33/#FDE7EA, correcto #2A8A52/#E3F5EA.
- Tipografía Segoe UI: título 30 pt negrita; subtítulo de zona 16 pt negrita carmesí; texto 14 pt (nunca menos de 12);
  fórmulas Cambria Math 16 pt como ECUACIONES EDITABLES (no imágenes); pie 10 pt con "Filmina N · producto 1.27.0".
- Máximo 3 zonas de contenido por filmina (hasta 4 si hay fórmulas). Alineación estricta a la grilla; sin texto sobre fotos.
- Cada foto lleva una etiqueta carmesí pequeña "FOTO-NN" (esquina inferior izquierda) y un pie de una línea. Capturas con borde gris fino.
- Guion del orador de cada ficha → notas de la filmina. Fuentes y advertencias de cada ficha → también en las notas.

IMÁGENES Y DIAGRAMAS (usar estos archivos exactos)
Fotos existentes (presentacion/fotos_existentes/):
  FOTO-01_robot_completo_CAD_v17.png (fondo transparente: poner fondo #F3F5F9) · FOTO-03_auto_base_frente_blender.png ·
  FOTO-04_auto_base_atras_blender.png · FOTO-05_auto_base_lado_derecho_fusion.png · FOTO-06_auto_base_lado_izquierdo_fusion.png ·
  FOTO-07_brazo_v17_CAD.png · FOTO-10_rueda_mecanum.jpg · FOTO-11_dimensiones_chasis.png · FOTO-12_shield_osoyoo_pinout.jpg ·
  FOTO-13_shield_mapa_pines_wifi.png · FOTO-29_estabilidad_planta_CG.png · FOTO-30_cargas_y_momentos.png ·
  FOTO-32_propuesta_montaje_recolector.svg · FOTO-53_autito_silueta_OLED_128x64.png ·
  FOTO-54…58_loader_*.png (capturas v1.2.0: marcarlas como "versión 1.2.0") · FOTO-61_shield_vista_superior_KEY_RST.jpg · LOGO_proyecto.png
Diagramas (presentacion/diagramas/png/, o los .svg homónimos): D-01_arquitectura_general · D-02_conexionado ·
  D-03a_ciclo_principal_ATmega · D-03b_servicio_de_control · D-04_flujo_GOTO · D-05_ciclo_mision_recolectora ·
  D-06_circuito_KEY_RST_NPN · D-07_navegacion_OLED_encoder · D-08_cadena_cinematica · D-09_flujo_Loader ·
  D-10_pipeline_odometria · D-11_patrones_rueda_mecanum.
Escalar proporcionalmente al ancho de la zona; no deformar.
Fotos NO existentes (✱ por tomar, ⌗ por capturar): NO inventarlas ni usar stock. Insertar un marcador de posición gris
(#F3F5F9, borde punteado carmesí) del tamaño de la zona con el texto "FOTO-NN · descripción · por tomar" tomado de FOTOS.md,
para que yo lo reemplace. Listar al final qué marcadores quedaron.

MAPA DE FILMINAS (resumen; el detalle manda en cada ficha)
 1 Introducción y arquitectura      A FOTO-01 · B D-01 · C problema/objetivo · D seis módulos · E cifras
 2 Modelos 3D                       A FOTO-03(+05/06) · B FOTO-07+cotas · C FOTO-01+FOTO-08 · D datos clave
 3 Pinout y conexionado             A tablas de pinout (3 bloques) · B D-02 · C FOTO-12 · D FOTO-14/15 (marcador)
 4 Firmwares y ciclo                A ATmega · B ESP8266 · C Web · D D-03a (vertical) · E enlace compartido
 5 Matemática del auto/odometría    A cinemática Mecanum · B D-11 · C odometría · D FOTO-10 + medidos · E parámetros
 6 GOTO y odometría                 A D-10 · B D-04 · C ley de control · D FOTO-21/22 (marcador)
 7 Matemática brazo y conjunto      A D-08 (banda) · B directa · C inversa · D conjunto+Jacobiano · E FOTO-25/26 (marcador)
 8 Esfuerzos, tensiones, vuelco     A masas · B pares/márgenes · C FOTO-29 · D método · E tabla de vuelco
 9 Simulador, misión, ciclo         A definición · B FOTO-33…36 (marcadores) · C misión · D D-05 · E FOTO-37…39 (marcadores)
10 Interfaz: pasos y pantallas      A pasos 1→6 · B FOTO-48…52 (marcadores)+FOTO-53 · C D-07 · D FOTO-41…47 (marcadores) · E reglas de entrada
11 Agregado: AFR2 Loader            A qué resuelve · B FOTO-54/55 · C D-09 · D chips
12 Agregado: NPN KEY/RST            A D-06 · B FOTO-61 · C problema→solución · D FOTO-62…64 (marcadores)
13 Agregado: encoder y OLED         A FOTO-65/66 (marcadores) · B encoder · C OLED · D conectores · E FOTO-67/68 (marcadores)

REGLAS DE CONTENIDO (obligatorias)
- Mostrar solo lo que dice la ficha. Respetar los niveles de evidencia: dato nominal · CAD · código · medido. Marcar con un ícono
  discreto lo que NO está medido físicamente.
- Datos medidos propios (únicos): 370 cuentas/vuelta; avance 1000 mm = 1000 mm; lateral 1000 mm = 775 mm reales (factor 0,775).
- Filmina 5: usar L = 170 mm (no 220). Filmina 7: eslabones del descriptor v17 (70/60/55/65/78,5 mm), no 130/120.
- Filmina 8: mostrar el factor de vuelco frontal 1,023 frente al criterio 1,2 como mensaje central; aclarar "resultado histórico del 01/11/2026, sobre CAD".
- Filmina 9: aclarar "simulación local, no misión física verificada".
- Filmina 11: flasheo real sin comprobar; capturas de la versión 1.2.0. Filmina 12: circuito sin armar ni medir.
  Filmina 13: montaje a 5 V informado; el CAD conserva +3V3; sin ERC/DRC.
- Portada/filmina 1: dejar campos [institución · asignatura · docentes · integrantes] vacíos y resaltados para completar; autor: Koba Chajchir.
- Tiempos sugeridos (20 min): 1,5 · 1,5 · 1,5 · 2 · 2 · 1,5 · 2 · 2 · 1,5 · 1,5 · 1 · 1 · 1,5.

ENTREGA
1. presentacion/AutoFinalRobo2_Presentacion.pptx con las 13 filminas, notas del orador y marcadores de foto.
2. Un informe breve (en el chat) con: filminas armadas, marcadores pendientes (lista FOTO-NN), cualquier dato de las fichas que
   no pudiste ubicar y toda inconsistencia detectada.
3. Verificación visual: renderizá cada filmina a imagen y revisá que no haya texto cortado, solapamientos, fuentes < 12 pt ni
   elementos fuera de su zona; corregí y volvé a revisar.
````

## Variante corta (si el asistente no tiene acceso a la carpeta)

Pegar el contenido de cada `Filmina_NN_*.md` junto con este bloque: «Armá UNA filmina 16:9 con el layout de la ficha (zonas A–E y rangos de
grilla 12×6), textos tal cual, fórmulas como ecuaciones editables y marcadores de posición para las fotos con su número FOTO-NN; paleta
#B81D33/#1F2430/#F3F5F9, Segoe UI; guion en notas».
