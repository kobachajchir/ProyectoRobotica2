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
| Script de renders | [tools/render_assets_claro.py](../tools/render_assets_claro.py) | Cycles 128 muestras, denoise, view transform **Standard** (AgX lavaba colores), luces key/fill/rim en base de cámara (`right`, `upv`, `d`) con energía ∝ distancia², exposición −0,55 en `v002`, todo transparente y recortado (alfa > 0,22, margen 4 %). Escribe `renders/manifest_renders.json`. Componentes de placas (Mega, WiFi, chip, ESP-12) salen de la **colección detallada** del .blend (`FINAL_RAW_*`); el resto del GLB. |
| Renders | [renders/](renders/) | 29 renders `v001` completos + **14 renders `v002`** corregidos tras auditoría sobre damero (sin pisar `v001`): `F04_bateria_superior`, `F04_driver_superior`, `F04_sensor_linea_inferior`, `F04_oled_encoder_superior`, `F04_motor_gm25_iso`, `F05_stack_mega_driver_wifi`, `F05_mega2560_iso`, `F05_mega2560_chip_detalle`, `F05_esp12_detalle`, `F06_rueda_iso`, `F06_rueda_lateral`, `F09_panel_ui_montaje`, `F_us_tof`, `F_linea_baja`. |
| Plano de placas | [planos/F07_plano_placas_v001.pdf](planos/F07_plano_placas_v001.pdf), [PNG hoja 3](planos/F07_plano_placas_v001.png), [PNG hoja 4](planos/F07_plano_placa_inferior_v001.png), [recorte planta](planos/F07_plano_placa_superior_planta_v001.png) | Fusion: documento aparte **AFR2_Placas_Chasis_Plano** (copia de los cuerpos CapaSuperior/CapaInferior) y **AFR2_Placas_Chasis_Plano Drawing** (DrawingManager automático, ISO, mm, A3, 4 hojas, rótulo Koba Chajchir). Solo cotas envolventes: sup. 199,73 × 154,64; inf. 199,92 × 154,98; espesor 3,0 mm sin cota. **Lista de piezas dice «Steel» = default de Fusion, no documentado.** Cota 339,44 de hoja 2 = dos placas lado a lado, no es de pieza. Sin escala en rótulo. PNG a 300 ppp con PyMuPDF. |
| Captura ensamble Fusion | [capturas/F08_fusion_ensamble_v001.png](capturas/F08_fusion_ensamble_v001.png) | 1920×1080, navegador desplegado + timeline, tema oscuro. |
| Capturas Blender | [importado](capturas/F09_blender_importado_v001.png), [ligero](capturas/F09_blender_ligero_v001.png), [decimate](capturas/F09_blender_decimate_v001.png) | Instancia propia de Blender ([scripts/blender_capturas.py](scripts/blender_capturas.py)), no la sesión del usuario. Stats 490.526 vs 105.620 triángulos. El Decimate (SensorLinea 0,12) se **reconstruyó en memoria** con el ratio del script; el .blend no lo guarda. |
| Logos | [logos/](logos/) | Descargados con permiso del usuario sin modificar: `blender.png` (`blender_logo_socket.png`, 62.780 B), `blender_logo.svg` (8.341 B), `fusion360.png` (12.888 B), `Fusion360_Logo.svg` (6.813 B), `Autodesk_Fusion_Logo.svg` (8.644 B). |
| Peso del modelo | [peso_modelo.json](peso_modelo.json) | Fusion→Blender 15.258.365 B (exports/2026-10-25-final/parts, 24 archivos) → GLB final 871.756 B: **−94,29 %**; GLB detallado sin Draco 17.571.416 B → −95,04 %; triángulos 490.524 → 105.618: **−78,47 %**. Draco nivel 10, pos 11, normal 8. |
| Manifest | [MANIFEST.json](MANIFEST.json) | Generado por [scripts/armar_manifest.py](scripts/armar_manifest.py) (59 entradas: 56 hechas, 3 pendientes documentados). |

## Datos verificados que corrigen el pedido

- GLB Web actual = **880.232 B** (rev. AFR2-20261031-06, 28 objetos, 105.616 tris); el 871.756 B es el GLB del 25/10.
- `.gz` en el ESP = **737.394 B** (no 730.799).
- Ensamble Fusion: `Rueda80_Referencia` 80 mm vs Mecanum real `Rueda80_ManoA` **109,3 mm** (explica la discrepancia con `AFR2_FIXED_WHEEL_MM 80` del firmware).
- Ensamble Fusion `AutoFinalRobo2` v5: **0 joints** (posiciones por transformaciones).
- GrabCAD: **sin evidencia** para ningún componente montado (ver `AutoFinalRobo2-CAD/docs/COMPONENTES_3D.csv`, `REVISION_CANDIDATOS_3D.md`).

## Pendientes registrados en `MANIFEST.json`

1. `capturas/F08_fusion_joints_v001.png`: el ensamble `AutoFinalRobo2` v5 tiene 0 joints y 0 as-built joints (`rootComponent.allJoints` vacío).
2. `capturas/F08_fusion_exportar_v001.png`: abrir el diálogo de exportación requiere controlar la interfaz de Fusion (acceso denegado); la exportación real se hizo por script (`20 .usdz + 2 .obj/.mtl`).
3. `capturas/F08_grabcad_modelos_v001.png`: opcional y sin evidencia de GrabCAD para ningún componente del ensamble.
