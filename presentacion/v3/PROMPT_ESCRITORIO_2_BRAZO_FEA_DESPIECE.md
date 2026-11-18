# Prompt 2 para la PC de escritorio · brazo sobre el auto, despiece, FEA y capturas de KiCad

> Pegar en Claude Code abierto en `D:/ProyectoRobotica2` de la PC de escritorio. Es **adicional** al primer prompt
> (`PROMPT_ESCRITORIO_FUSION_BLENDER.md`): mismas reglas, misma carpeta `presentacion/v3/assets_claro/`, mismo estilo
> de render (Cycles claro, fondo transparente, sombra de contacto, recorte al contenido) y mismo `MANIFEST.json`.
> Si el primer prompt todavía no se ejecutó, hacer los dos en la misma sesión y un solo commit.

---

Trabajás en `D:/ProyectoRobotica2`. `git pull` en `main`, leer `AGENTS.md`, `AutoFinalRobo2-CAD/AGENTS.md`,
`presentacion/v3/PROMPT_ESCRITORIO_FUSION_BLENDER.md` (reglas y estilo) y
`AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/revision_v17/README.md`.

El generador `presentacion/v3/tools/generar_presentacion_v4.js` ya espera los archivos de abajo con estos nombres exactos
y los coloca solo. No tocar el generador ni los diagramas.

## Reglas extra

- El usuario pidió **expresamente** el ensayo por elementos finitos de la sección 4. Es la única simulación autorizada.
  No ejecutar otros análisis, tests ni compilaciones.
- No modificar ni guardar los maestros del auto ni del brazo. Para poner el brazo sobre el auto, trabajar en un
  **documento nuevo** de Fusion (o una escena nueva de Blender) que referencie/importe ambos, guardado aparte, y anotarlo.
- El GLB de la Web no tiene el brazo: no agregarlo al GLB ni a `AutoFinalRobo2-Web`.
- Para cada render que lleve flechas encima, generar además un **JSON de puntos** con las coordenadas en píxeles
  (origen arriba a la izquierda, en la imagen **ya recortada**) de los puntos pedidos. En Blender:
  `bpy_extras.object_utils.world_to_camera_view` sobre el centro (o el punto indicado) del objeto, convertido a píxeles
  y corregido por el recorte. Formato: `{"w": <px>, "h": <px>, "points": {"<clave>": [x, y], ...}}`.

## 1 · Brazo sobre el auto (filmina «Centro de masa, inercia y vuelco»)

Modelo del brazo seleccionado: `AFR2_Brazo_v17_Base_V7_J3_Remoto.f3d`, montado sobre el auto como en
`presentacion/fotos_existentes/FOTO-01_robot_completo_CAD_v17.png`, con el brazo **estirado hacia adelante y
horizontal** (pose del análisis `analisis_cargas_200g`), con una caja de 50 mm marcada como carga de 200 g en la pinza.

| Archivo | Vista | Puntos del JSON (mismo nombre `.json`) |
| --- | --- | --- |
| `renders/F18_auto_brazo_lateral_v001.png` | lateral ortográfica (el brazo apunta a la derecha de la imagen), auto completo + brazo, escala para que entre todo con margen, transparente | `tcp` (centro de la carga), `torre_top` (eje J2 sobre la torre), `cg` (centro de masa del conjunto calculado con las propiedades físicas de Fusion; si no se puede, centroide ponderado de los cuerpos con masa del análisis y anotarlo), `borde` (contacto de la rueda delantera con el piso) |
| `renders/F18_auto_brazo_frontal_v001.png` | frontal ortográfica, brazo girado 90° hacia el costado derecho, transparente | `tcp`, `torre_top`, `cg`, `borde` (contacto de las ruedas del lado hacia donde apunta el brazo) |

Anotar en el manifest la masa total y la posición del CG que se usaron, y de dónde salen.

## 2 · Brazo lateral limpio con puntos (filmina «Elementos de máquinas en el brazo»)

`renders/F_brazo_lateral_v001.png` + `renders/F_brazo_lateral_v001.json`: el brazo solo (sin auto), vista lateral 3/4
parecida a `FOTO-07_brazo_v17_CAD.png`, transparente, 2400 px de ancho. Puntos:

- `torre`: centro de la cruz (arriostramiento en X) de la torre J1–J2
- `j2`: engranaje de 72 dientes del hombro (montaje STS3250 + engranajes 18/72, eje 5 con dos 625)
- `j3`: polea de 50 del codo (correa HP3M)
- `j1`: cartucho del rodamiento 6000 en la montura baja
- `pinza`: piñón de la pinza
- `servo`: un servo STS3250 visible

Si alguna pieza no es visible desde ese ángulo, elegir otro ángulo que muestre las seis y anotarlo.

## 3 · Despieces (todas sin fondo, recortadas, 1600 px de lado mayor)

Auto (`renders/F19_*`), cada pieza sola en isométrica 3/4 desde el mismo lado:
`F19_placa_superior_v001.png`, `F19_placa_inferior_v001.png`, `F19_motor_soporte_v001.png` (GM25-370 + soporte),
`F19_casquillo_v001.png` (casquillo + prisionero de rueda), `F19_separadores_v001.png` (separadores y fijaciones de placas).
El resto del despiece del auto reutiliza los renders del primer prompt (ruedas, Mega, driver, shield, HC-SR04, SG90,
sensor de línea, panel OLED, baterías).

Brazo (`renders/F20_*`), por conjunto, misma orientación entre sí:
`F20_base_j1_v001.png` (montura y giro J1), `F20_torre_v001.png` (torre J1–J2), `F20_hombro_j2_v001.png`
(hombro con engranajes), `F20_eslabon1_v001.png`, `F20_codo_j3_v001.png` (codo con correa y poleas),
`F20_eslabon2_v001.png`, `F20_muneca_v001.png` (J4–J5), `F20_pinza_v001.png`. Usar los conjuntos de
`revision_v17/laminas_20261109` como guía de agrupación.

## 4 · Ensayo por elementos finitos de la torre J1–J2 (autorizado por el usuario)

Pieza: la torre vertical que une J1 con J2, la que tiene la cruz en el medio, del modelo v17.

1. En Fusion, **Simulation → Static Stress**, en un documento o estudio nuevo; no alterar la geometría del maestro.
2. Material: el del CAD si está asignado (probablemente PETG impreso). Si Fusion no tiene PETG, crear un material con
   las propiedades documentadas en `analisis_cargas_200g/01_materiales_y_masas.md` / `petg_por_pieza.csv` y anotarlas.
   Si no hay propiedades documentadas, preguntar al usuario antes de inventarlas.
3. Restricciones: fija en la interfaz con la montura J1 (agujeros/cara de apoyo inferior).
4. Cargas: las reacciones de la torre de `analisis_cargas_200g/reacciones_para_FEA.csv` para el caso CAD macizo de 200 g
   (fuerza vertical, momento flector del brazo horizontal y, si figura, torsor) aplicadas en el eje/alojamiento de J2.
   Si el CSV no tiene la torre, usar el peso soportado por J2 (`resultados_estatica.json`, caso `CAD_macizo_200g`) y el
   momento en J2 (2,911 N·m), y anotar cómo se aplicó.
5. Malla por defecto con refinamiento local en los nudos de la cruz y en los agujeros. Resolver localmente.
6. Exportar, con **cargas y restricciones visibles** y leyenda de colores:
   - `renders/F_fea_torre_von_mises_v001.png` (tensión de von Mises, vista 3/4 que muestre la cruz, 2400 px)
   - `renders/F_fea_torre_desplazamiento_v001.png`
   - `renders/F_fea_torre_seguridad_v001.png` (factor de seguridad)
   - `renders/F_fea_torre_v001.json`: `{"material": "...", "cargas": "texto corto", "von_mises_max_MPa": n,
     "desplazamiento_max_mm": n, "factor_seguridad_min": n, "malla": "elementos/nodos", "nota": "..."}`
7. Dejar escrito en el manifest: «Resultado ilustrativo: material isotrópico macizo; no acredita una pieza impresa con
   relleno parcial ni aprueba fabricación».

## 5 · Capturas de KiCad (filmina «Diseño propio: panel OLED + encoder»)

La PC de esta presentación tiene KiCad 9 y no abre la placa: hacerlo con la versión de KiCad del proyecto.

- `capturas/F12_kicad_esquematico_v001.png`: ventana del editor de esquemáticos con
  `AutoFinalRobo2-CAD/kicad/oled-encoder/PlacaOLED-Encoder-AutoRobo2.kicad_sch` completo, tema claro, 1920×1080 o más.
- `capturas/F12_kicad_pcb_editor_v001.png`: editor de PCB con la placa (B.Cu visible, contorno, serigrafía), zoom a la
  placa, tema claro.
- Si existe también el STEP en Fusion, `renders/F09_panel_ui_montaje_v001.png` del primer prompt sirve para la tercera
  imagen; si no se hizo, hacerlo ahora.

## Entrega

Commit único en `main` con `presentacion/v3/assets_claro/**`, el script de render y los documentos nuevos de Fusion si se
guardaron dentro del repo, mensaje `Presentación v4: brazo sobre el auto, despieces, FEA de la torre y capturas KiCad`,
y `git push`. Responder con la lista de archivos, los JSON de puntos, el JSON del FEA y lo que quedó pendiente.
