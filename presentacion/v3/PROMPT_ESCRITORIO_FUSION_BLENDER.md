# Prompt para la PC de escritorio · assets de Fusion 360 y Blender (presentación v4, tema claro)

> Copiar todo lo que sigue y pegarlo en Claude Code abierto en `D:/ProyectoRobotica2` de la PC de escritorio
> (la que tiene Fusion 360 y Blender 4.5 con los MCP/add-ins del proyecto).

---

Trabajás en el repo `D:/ProyectoRobotica2` (AutoFinalRobo2). Antes de empezar: `git pull` en `main` y leer `AGENTS.md`,
`AutoFinalRobo2-CAD/README.md`, `AutoFinalRobo2-CAD/AGENTS.md` y `presentacion/v3/README.md`.

Estamos rehaciendo la presentación (`presentacion/v3/tools/generar_presentacion_v3.js`) con **tema claro**, estilo
glassmorphism/Apple, con tarjetas translúcidas teñidas de los colores de cada rama. En otra computadora se rehacen la
maquetación, los diagramas y el texto. **Acá solo se producen los recursos que requieren Fusion 360 o Blender.**
No se toca el generador `.js`, `diagramas_v3.py` ni ningún archivo fuera de lo que se indica abajo.

## Reglas

- **No modificar ni guardar** los maestros: `AutoFinalRobo2-Final-20261025.f3d/.blend`, el documento AutoFinalRobo2 de
  Fusion, ni `AutoFinalRobo2-Web/public/models/AutoFinalRobo2.glb`. Para renders, partir de Blender vacío e importar el GLB
  (como hace `presentacion/v3/tools/render_assets.py`) o abrir el `.blend` final **sin guardarlo**. Para planos en Fusion,
  crear un **Drawing nuevo** derivado del diseño, guardado como documento aparte.
- No compilar, flashear, ni correr tests. No subir la versión de producto (esto es documentación).
- Nada de imágenes generadas por IA: todo sale de la geometría real (render, captura o Drawing).
- Todo va en `presentacion/v3/assets_claro/` con estas subcarpetas: `renders/`, `planos/`, `capturas/`, `logos/`.
- Nombres: `F<nn>_<descripcion>_v001.png` (minúsculas y guiones bajos). Si se rehace, `v002`, sin pisar.
- Registrar cada asset en `presentacion/v3/assets_claro/MANIFEST.json`: archivo, origen (GLB/f3d y objeto), cámara
  (tipo, lente, posición, dirección), resolución, fondo y nota. Si algo no se puede hacer, dejarlo en el manifest con
  `"estado": "pendiente"` y el motivo. No inventar datos.
- Al terminar: un único commit en `main` solo con `presentacion/v3/assets_claro/**` y el script nuevo
  `presentacion/v3/tools/render_assets_claro.py`, mensaje
  `Presentación v4: assets de Fusion y Blender en tema claro`, y `git push`. Avisar la lista final de archivos con su tamaño.

## Estilo de render (igual para todos)

- Blender 4.5, Cycles, 128 muestras, denoise, `view_transform = 'AgX'` (look `Medium High Contrast`) o `Standard` si
  AgX lava los colores del CAD; usar el mismo en todos.
- **Fondo transparente** (`film_transparent = True`) en todos los recortes de componentes, del auto y de las placas.
  Recortar el PNG al contenido con un margen de 4 % (no dejar lienzos vacíos enormes).
- Iluminación de estudio clara y suave: key area grande arriba-adelante, fill frío suave, rim por detrás; sombras
  blandas. Agregar un **shadow catcher** (plano con `is_shadow_catcher = True`) debajo de las piezas para que quede una
  sombra de contacto suave sobre el fondo transparente.
- Escenas con fondo (si hiciera falta alguna): mundo color `#F4F6FB`, nunca el azul oscuro `#0B1220` de la v3.
- Resolución: componentes 2400×2400 antes de recortar; vistas del auto 3840×2160.
- Escribir todo en un script reproducible `presentacion/v3/tools/render_assets_claro.py`
  (`blender -b --factory-startup --python render_assets_claro.py -- <salida> [ids...]`), basado en `render_assets.py`.
  Los nombres de objeto del GLB ya conocidos: `AFR2_Mega2560_CAD`, `AFR2_OSOYOOMotorDriver_ModelY_V2`,
  `AFR2_OSOYOOWifiShield`, `AFR2_GM25_370:1..4`, `AFR2_HC_SR04`, `AFR2_Soporte_HCSR04`, `AFR2_SG90`,
  `AFR2_SensorLinea_5IR`, `AFR2_Rueda80_ManoA/B`, `AFR2_Celda_18650` (×2), `AFR2_Portabaterias…`, `AFR2_CapaSuperior`,
  `AFR2_CapaInferior`, panel `AFR2_AFR2_UI_B2_Fondo_OLED15_Auto5`. El frente del auto está en **+Y**. Verificar los nombres
  con `presentacion/v3/tools/inspect_glb.py` antes de renderizar.

## 1 · Renders de componentes aislados (filmina de arquitectura y de microcontroladores)

Cada uno solo, fondo transparente, sombra de contacto, recortado:

| Archivo | Objeto(s) | Vista |
| --- | --- | --- |
| `renders/F04_bateria_superior_v001.png` | 2 × Celda 18650 + portabaterías | **desde arriba** (planta, leve perspectiva 85 mm) |
| `renders/F04_oled_encoder_superior_v001.png` | panel UI: OLED 0,96" + encoder + PCB + soporte | **desde arriba**, OLED de frente a cámara |
| `renders/F04_sensor_linea_inferior_v001.png` | SensorLinea_5IR | **desde abajo**, que se vean los 5 emisores/receptores |
| `renders/F04_hcsr04_frente_v001.png` | HC_SR04 (+ su soporte, sin servo) | **de frente**, los dos transductores mirando a cámara |
| `renders/F04_driver_superior_v001.png` | OSOYOOMotorDriver_ModelY_V2 | **desde arriba** |
| `renders/F04_motor_gm25_iso_v001.png` | un GM25-370 con encoder Hall | isométrica 3/4 |
| `renders/F04_servo_sg90_iso_v001.png` | SG90 | isométrica 3/4 |
| `renders/F05_mega2560_iso_v001.png` | Mega2560_CAD solo | isométrica 3/4, chip ATmega bien visible |
| `renders/F05_mega2560_superior_v001.png` | Mega2560_CAD solo | **desde arriba** (planta ortográfica) |
| `renders/F05_mega2560_chip_detalle_v001.png` | Mega2560_CAD | primer plano del chip ATmega2560 (lente 135 mm) |
| `renders/F05_wifi_shield_iso_v001.png` | OSOYOOWifiShield solo | isométrica 3/4, módulo ESP-12 visible |
| `renders/F05_wifi_shield_superior_v001.png` | OSOYOOWifiShield solo | **desde arriba** |
| `renders/F05_esp12_detalle_v001.png` | OSOYOOWifiShield | primer plano del módulo ESP |
| `renders/F05_stack_mega_driver_wifi_v001.png` | Mega + Driver + WiFi apilados | isométrica 3/4 |

Si la malla simplificada del GLB se ve pobre en algún primer plano (chips, serigrafía), usar la colección detallada
del `.blend` final (abrir sin guardar) y anotarlo en el manifest.

## 2 · El auto y las placas sin fondo

| Archivo | Contenido |
| --- | --- |
| `renders/F01_auto_hero_v001.png` | auto completo 3/4 (dirección ≈ (1, 1,3, 0,62)), transparente, sombra de contacto, para la portada |
| `renders/F_cierre_auto_hero_v001.png` | auto completo 3/4 desde el lado opuesto, transparente |
| `renders/F04_auto_iso_superior_v001.png` | auto completo 3/4 superior, transparente (arquitectura) |
| `renders/F07_placas_iso_v001.png` | **solo** `CapaSuperior` y `CapaInferior`, todo lo demás oculto, isométrica, en su posición de montaje, transparente |
| `renders/F07_placas_iso_separadas_v001.png` | las dos placas separadas en Z (≈ 60 mm) para que se vean las dos caras, isométrica, transparente; anotar que la separación es solo visual |
| `renders/F07_placa_superior_planta_v001.png` | `CapaSuperior` sola, planta ortográfica, transparente |
| `renders/F07_placa_inferior_planta_v001.png` | `CapaInferior` sola, planta ortográfica, transparente |
| `renders/F06_rueda_iso_v001.png` y `renders/F06_rueda_lateral_v001.png` | rehacer las dos vistas de rueda Mecanum de la v3 con la iluminación clara |
| `renders/F08_explosionado_v001.png` | rehacer el explosionado vertical de la v3 (mismos desplazamientos de `render_assets.py`), transparente |
| `renders/F09_panel_ui_montaje_v001.png` | rehacer el primer plano del panel OLED/encoder montado, fondo `#F4F6FB` |
| `renders/F_lateral_ortho_v001.png` y `renders/F_planta_ortho_v001.png` | lateral y planta ortográficas del auto completo, transparente |
| `renders/F_us_tof_v001.png` | rehacer la lateral con el obstáculo de escena (obstáculo gris claro, piso `#E9EDF4`) |
| `renders/F_linea_baja_v001.png` | rehacer la vista baja sobre pista: piso claro `#E9EDF4` con banda negra de 19 mm |

## 3 · Plano de las placas desde arriba (Fusion 360)

Para la filmina «De un perfil 2D a placas tridimensionales» se muestran el plano y el render lado a lado.

1. Buscar primero si ya hay un Drawing de las placas (`AutoFinalRobo2-CAD/**`, láminas
   `revision_v17/laminas_20261102`). Si no hay de las placas del chasis, crear un **Drawing nuevo** en Fusion a partir de
   `CapaSuperior` y otro de `CapaInferior` (o una hoja con las dos), seguir la skill
   `AutoFinalRobo2-Contracts/.agents/skills/fusion-technical-sheets/SKILL.md`: A3 horizontal, mm, fondo blanco, planta
   (vista superior) grande con cotas envolventes, posición de agujeros y ranuras principales desde un datum, vista de
   perfil con el espesor, rótulo con Koba Chajchir como responsable de documentación, escala exacta.
2. Si solo hay cotas envolventes confiables, rotular «preliminar, acotado funcional pendiente». No asignar material ni
   tolerancias que no estén documentados.
3. Exportar: `planos/F07_plano_placas_v001.pdf` y `planos/F07_plano_placas_v001.png` (300 ppp, fondo blanco),
   y además `planos/F07_plano_placa_superior_planta_v001.png` recortado solo a la vista en planta (para ponerlo al
   lado del render). Guardar el documento Drawing aparte (no dentro del maestro) y anotar su nombre en el manifest.

## 4 · Capturas de Fusion 360 (filmina de ensamble)

Con el documento AutoFinalRobo2 abierto, **sin guardar cambios**:

- `capturas/F08_fusion_ensamble_v001.png`: ventana completa de Fusion con el auto armado, el navegador de componentes
  desplegado (que se vean Mega, shields, motores, ruedas, placas) y el timeline abajo. Tema claro de Fusion si está
  disponible. 1920×1080 o más.
- `capturas/F08_fusion_joints_v001.png`: primer plano de un joint/restricción de ensamble (por ejemplo una rueda con su
  casquillo y motor) con el diálogo o la lista de joints visible.
- `capturas/F08_fusion_exportar_v001.png`: el diálogo de exportación usado para pasar a Blender (formato real: los
  parts de `exports/2026-10-24/parts` son **OBJ para las placas y USDZ para el resto**; capturar lo que de verdad se usa
  y anotarlo; no rotular «OBJ» si fue USDZ).
- `capturas/F08_grabcad_modelos_v001.png`: opcional, si los modelos de catálogo vinieron de GrabCAD, una grilla de los
  componentes de biblioteca. Cruzar con `AutoFinalRobo2-CAD/reference/inventario-CADS-2026-10-23.csv` y
  `reference/supplied/` y escribir en el manifest, **por componente**, de dónde salió (GrabCAD con URL/autor si figura,
  fabricante, propio, o «sin procedencia registrada»). No afirmar GrabCAD sin evidencia.

## 5 · Capturas de Blender y peso antes/después (filmina de Blender)

Abrir `AutoFinalRobo2-Final-20261025.blend` **sin guardar**:

- `capturas/F09_blender_importado_v001.png`: Blender con la colección **detallada** visible (la que vino de Fusion),
  outliner desplegado y estadísticas de escena activadas (Overlays → Statistics) para que se lean objetos/triángulos.
- `capturas/F09_blender_ligero_v001.png`: lo mismo con la colección **Web** (la simplificada) visible.
- `capturas/F09_blender_decimate_v001.png`: primer plano del modificador Decimate aplicado a una malla pesada (sensor de
  línea, Mega o WiFi shield) con su ratio visible.

Medir y anotar en `assets_claro/peso_modelo.json` (sin tocar los archivos medidos):

- **Antes:** suma en bytes de lo exportado de Fusion a Blender (`AutoFinalRobo2-CAD/exports/2026-10-24/parts/*`, si la
  exportación del 25/10 ya no está, usar esa y aclararlo); triángulos y materiales de la colección detallada.
  Además exportar la colección detallada a un **GLB sin Draco en una carpeta temporal fuera del repo** y anotar su
  peso, para comparar formato contra formato.
- **Después:** `AutoFinalRobo2-Web/public/models/AutoFinalRobo2.glb` = 871.756 bytes, 105.618 triángulos únicos,
  106 materiales, Draco nivel 10 (posiciones 11 bits, normales 8); y su `.gz` en el ESP = 730.799 bytes. Verificar con
  `AutoFinalRobo2-CAD/docs/modelo-final-20261025.json`.
- Calcular el porcentaje de reducción en bytes y en triángulos (antes→después) y dejarlo escrito con la fórmula.

## 6 · Logos de los programas

- `logos/fusion360.png` y `logos/blender.png`, PNG transparente de al menos 512 px. Primero buscarlos en las instalaciones
  locales (íconos de alta resolución de Fusion en `%LOCALAPPDATA%/Autodesk/webdeploy/...` y de Blender en
  `C:/Program Files/Blender Foundation/Blender 4.5/`); si solo hay `.ico` chicos o no hay, **pedirle al usuario permiso**
  para descargar el logo oficial (Blender: blender.org/about/logo; Fusion: kit de prensa de Autodesk), diciendo URL y
  tamaño. Usarlos sin modificar ni recolorear. Anotar la fuente en el manifest.

## Entrega

Commit y push como se indicó arriba. Responder con: lista de archivos y tamaños, el contenido de `peso_modelo.json`,
qué quedó pendiente y por qué, y la procedencia por componente si se pudo establecer.
