# Traspaso · Presentación v4 (tema claro): assets de Fusion 360 y Blender

Estado al 18/11/2026 ~12:20, PC de escritorio (Fusion 360 + Blender 4.5). Repo `D:/ProyectoRobotica2`, rama `main`,
último commit `815d335` (producto 1.39.8). **Nada de esta tarea está commiteado todavía**: está sin trackear
`presentacion/v3/assets_claro/` y `presentacion/v3/tools/render_assets_claro.py`.

## Pedido original (resumen)

Producir en `presentacion/v3/assets_claro/{renders,planos,capturas,logos}/` los recursos que necesitan Fusion o Blender
para rehacer la presentación con tema claro (glassmorphism). En otra PC se rehacen maquetación, diagramas y texto: aquí
no se toca `generar_presentacion_v3.js` ni `diagramas_v3.py`. Reglas: no guardar maestros (`AutoFinalRobo2-Final-20261025.blend`,
documento Fusion `AutoFinalRobo2`, `AutoFinalRobo2-Web/public/models/AutoFinalRobo2.glb`), no compilar/flashear/tests,
no subir versión, nada generado por IA, nombres `F<nn>_<desc>_v001.png`, todo registrado en `MANIFEST.json`
(pendientes con `"estado": "pendiente"` y motivo). Cierre: **un único commit en main solo con
`presentacion/v3/assets_claro/**` y `presentacion/v3/tools/render_assets_claro.py`**, mensaje
`Presentación v4: assets de Fusion y Blender en tema claro`, `git push`, y responder con lista de archivos y tamaños,
contenido de `peso_modelo.json`, pendientes y procedencia por componente.

**Cambios pedidos por el usuario a mitad de la tarea:** los primeros renders estaban sobreexpuestos → se bajó la
exposición; y **todos los PNG deben quedar sin fondo** (transparentes, recortados). Por eso se quitó la sombra de
contacto (shadow catcher, solo con `AFR2_SHADOW=1`) y las escenas que iban sobre `#F4F6FB` también son transparentes.

## Hecho

| Qué | Dónde | Notas |
| --- | --- | --- |
| Script de renders | [tools/render_assets_claro.py](../tools/render_assets_claro.py) | Cycles 128 muestras, denoise, view transform **Standard** (AgX lavaba colores), exposición −0,4, luces key/fill/rim con energía ∝ distancia², todo transparente y recortado (alfa > 0,22, margen 4 %). Escribe `renders/manifest_renders.json`. Componentes de placas (Mega, WiFi, chip, ESP-12) salen de la **colección detallada** del .blend (`FINAL_RAW_*`); el resto del GLB. Fix clave: `view_layer.update()` tras abrir el .blend (en `-b` la matrix_world quedaba en identidad). |
| Renders (en curso) | [renders/](renders/) | 29 trabajos en total; **16 listos** al traspaso (F01, F_cierre, todos F04 de componentes, todos F05). Faltan 13: F04_auto_iso_superior, F07_placas_iso, F07_placas_iso_separadas, F07_placa_superior_planta, F07_placa_inferior_planta, F06_rueda_iso, F06_rueda_lateral, F08_explosionado, F09_panel_ui_montaje, F_lateral_ortho, F_planta_ortho, F_us_tof, F_linea_baja. El proceso `blender.exe -b` sigue corriendo; log en el scratchpad de la sesión anterior (`…/scratchpad/claro_full.log`, puede borrarse). |
| Plano de placas | [planos/F07_plano_placas_v001.pdf](planos/F07_plano_placas_v001.pdf), [PNG hoja 3](planos/F07_plano_placas_v001.png), [PNG hoja 4](planos/F07_plano_placa_inferior_v001.png), [recorte planta](planos/F07_plano_placa_superior_planta_v001.png) | Fusion: documento aparte **AFR2_Placas_Chasis_Plano** (copia de los cuerpos CapaSuperior/CapaInferior) y **AFR2_Placas_Chasis_Plano Drawing** (DrawingManager automático, ISO, mm, A3, 4 hojas, rótulo Koba Chajchir). Solo cotas envolventes: sup. 199,73 × 154,64; inf. 199,92 × 154,98; espesor 3,0 mm sin cota. **Lista de piezas dice «Steel» = default de Fusion, no documentado.** Cota 339,44 de hoja 2 = dos placas lado a lado, no es de pieza. Sin escala en rótulo. PNG a 300 ppp con PyMuPDF. |
| Captura ensamble Fusion | [capturas/F08_fusion_ensamble_v001.png](capturas/F08_fusion_ensamble_v001.png) | 1920×1080, navegador desplegado + timeline, tema oscuro. |
| Capturas Blender | [importado](capturas/F09_blender_importado_v001.png), [ligero](capturas/F09_blender_ligero_v001.png), [decimate](capturas/F09_blender_decimate_v001.png) | Instancia propia de Blender ([scripts/blender_capturas.py](scripts/blender_capturas.py)), no la sesión del usuario. Stats 490.526 vs 105.620 triángulos. El Decimate (SensorLinea 0,12) se **reconstruyó en memoria** con el ratio del script; el .blend no lo guarda. |
| Peso del modelo | [peso_modelo.json](peso_modelo.json) | Fusion→Blender 15.258.365 B (exports/2026-10-25-final/parts, 24 archivos) → GLB final 871.756 B: **−94,29 %**; GLB detallado sin Draco 17.571.416 B → −95,04 %; triángulos 490.524 → 105.618: **−78,47 %**. Draco nivel 10, pos 11, normal 8. |
| Generador de manifest | [scripts/armar_manifest.py](scripts/armar_manifest.py) | Une `renders/manifest_renders.json` + entradas manuales (planos, capturas, logos, pendientes) + procedencia por componente → `MANIFEST.json`. **Todavía no se ejecutó.** |

## Datos verificados que corrigen el pedido

- GLB Web actual = **880.232 B** (rev. AFR2-20261031-06, 28 objetos, 105.616 tris); el 871.756 B es el GLB del 25/10.
- `.gz` en el ESP = **737.394 B** (no 730.799).
- Ensamble Fusion: `Rueda80_Referencia` 80 mm vs Mecanum real `Rueda80_ManoA` **109,3 mm** (explica la discrepancia con `AFR2_FIXED_WHEEL_MM 80` del firmware).
- Ensamble Fusion `AutoFinalRobo2` v5: **0 joints** (posiciones por transformaciones).
- GrabCAD: **sin evidencia** para ningún componente montado (ver `AutoFinalRobo2-CAD/docs/COMPONENTES_3D.csv`, `REVISION_CANDIDATOS_3D.md`).

## Pendiente

1. Esperar que termine el lote (o relanzar solo lo faltante):
   `AFR2_SAMPLES=128 "C:/Program Files/Blender Foundation/Blender 4.5/blender.exe" -b --factory-startup --python presentacion/v3/tools/render_assets_claro.py -- "D:/ProyectoRobotica2/presentacion/v3/assets_claro/renders" <ids…>`
   (si se relanza con ids, `manifest_renders.json` se fusiona, no se pisa).
2. Revisar visualmente los 29 PNG sobre damero (transparencia, exposición, encuadre). Puntos a mirar: F04_oled_encoder_superior (OLED mira a −Y), F05_esp12_detalle (k=0,75), F_linea_baja (tramo de pista 600 mm), F09_panel_ui_montaje.
3. Logos: solo hay `.ico` ≤ 256 px (Fusion: `%LOCALAPPDATA%/Autodesk/webdeploy/production/6a0c9611…/Fusion360.ico`). **Pedir permiso al usuario** para descargar los oficiales (Blender: blender.org/about/logo; Fusion: kit de prensa Autodesk), diciendo URL y tamaño; usarlos sin modificar.
4. Quedan pendientes (ya redactados en `armar_manifest.py`): F08_fusion_joints (no hay joints), F08_fusion_exportar (requiere controlar la interfaz; acceso denegado; formato real 20 .usdz + 2 .obj/.mtl por script), F08_grabcad_modelos (sin evidencia), tema claro de Fusion.
5. `python -I presentacion/v3/assets_claro/scripts/armar_manifest.py` → `MANIFEST.json`.
6. Commit único y push (solo `presentacion/v3/assets_claro/**` + `presentacion/v3/tools/render_assets_claro.py`). Si `renders/manifest_renders.json` queda, se incluye (está dentro de assets_claro).
7. Responder al usuario: lista de archivos con tamaño, contenido de `peso_modelo.json`, pendientes y procedencia.

## Restricciones activas

- El usuario **denegó el control de la interfaz** de Fusion y Blender (computer-use): no hacer clics; solo API de Fusion (MCP), scripts de Blender en instancia propia y capturas pasivas.
- La sesión de Blender del usuario tiene abierto otro proyecto (`Aldo_Maestro_v003.blend`): no tocarla.
- Fusion tiene abiertos: AutoFinalRobo2 (sin cambios), AFR2_Placas_Chasis_Plano, el Drawing y un Untitled previo.
