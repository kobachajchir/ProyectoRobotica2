# Modelo final CAD, Web y ESP — AFR2-20261025-13

Se incorporó la PCB guardada por el usuario en KiCad al AutoFinalRobo2 abierto en Fusion. Se conservaron el soporte editado, sus materiales, los herrajes y la pose anterior. La PCB anterior queda oculta como historial. No se regeneró el soporte desde parámetros antiguos.

## Fuentes guardadas

- KiCad: [proyecto OLED/encoder](../kicad/oled-encoder/PlacaOLED-Encoder-AutoRobo2.kicad_pro), con esquema, PCB y bibliotecas locales. Componentes TH; R2/R3 de 10 kΩ desde CLK/DT a +3V3. Señales por hembra 1×5 de 2,54 mm; power JST de 2 mm. OLED GND/VCC/SCL/SDA de frente, en socket hembra 1×4.
- Fusion completo: [AutoFinalRobo2-Final-20261025.f3d](../mecanica/auto-completo/AutoFinalRobo2-Final-20261025.f3d). Documento AutoFinalRobo2 también guardado en Fusion. Revisiones anteriores ocultas dentro del maestro.
- Soporte editable: [Soporte_UI_Final_20261025.f3d](../mecanica/pieza-nueva/Soporte_UI_Final_20261025.f3d). Sólo fondo impreso, sin tapas ni postes impresos. Torretas comerciales de 15 mm para OLED y 5 mm entre soporte y auto. PCB directa al soporte con tornillos/tuercas inferiores; encoder directo. Mandan las ediciones manuales del F3D sobre los parámetros históricos B.
- Blender: [AutoFinalRobo2-Final-20261025.blend](../mecanica/auto-completo/AutoFinalRobo2-Final-20261025.blend). Escena `AFR2_Final_20261025`, colección detallada oculta y colección Web visible. Escena anterior conservada. Maestro guardado con compresión.

## Modelo distribuido

`AutoFinalRobo2-Web/public/models/AutoFinalRobo2.glb`: **871.756 bytes**, margen **28.244** respecto de 900.000. GLB 2 con Draco obligatorio; 27 nodos de malla, 21 mallas únicas y 106 materiales. Cuatro ruedas y cuatro motores conservan los prefijos usados por el rig Web. Sin suelo, cámaras ni luces de estudio.

SHA-256: `c1bb1d87b40a4224a56e8e6473692929b36b0497799dd0971d6561b4939a4475`.

Escala en metros. La reimportación Draco mide aproximadamente 256 × 232,96 × 130 mm; el suelo queda a 0,0108 mm de Z=0 por cuantización. Compresión Draco nivel 10, posiciones 11 bits, normales 8. La simplificación sólo afecta la copia visual.

`npm run build:esp` completado: siete activos promovidos a `AutoFinalRobo2-ESP/data/dist`. La ruta pública sigue siendo `/models/AutoFinalRobo2.glb`. En LittleFS se guarda como `models/AutoFinalRobo2.glb.gz`, **730.799 bytes**; descomprimido coincide con CAD, Web/public y Web/dist. Paquete completo: **1.505.042 bytes gzip**, estimación conservadora FS **1.597.440 bytes**. No se creó ni flasheó una imagen de filesystem.

La primera compilación detectó hashes sensibles a los finales de línea de Windows. El generador OLED ahora calcula sobre LF y se normalizaron las dos referencias restantes. Renderer y fuentes AVR sin cambios funcionales; los demás cambios de texto continúan haciendo fallar el control.

## Comprobaciones solicitadas

- Fusion: pose nueva igual a la anterior; cero penetraciones PCB–fondo. El motor también enumeró 166 solapes internos del STEP entre sus propios componentes/pads/cobre; se registran aparte y no acreditan fabricación.
- Render ligero revisado en Blender: soporte, OLED, encoder y materiales presentes.
- GLB final reimportado con Draco en Blender: 27 objetos, cuatro ruedas y un panel; escala y suelo conservados.
- SHA-256 iguales en las cuatro copias, compilación y promoción Web completadas. Recursos OLED generados idénticos al commit previo salvo los hashes. Sin tests, compilación de firmware, ERC/DRC, flasheo ni prueba física. Web compilada accesible en Chrome; el visor informó «WebGL no está disponible», por lo que el render Web queda sin comprobar en este entorno. La reimportación Draco y el render Blender sí se realizaron.

Evidencia: [estadísticas GLB](modelo-final-20261025.json), [copias/paquete](modelo-final-entrega-20261025.json), [reimportación Draco](modelo-final-comprobacion-20261025.json), [intersecciones CAD](revision-importacion-final-20261025.json).

## Regeneración

[Actualización PCB](../mecanica/auto-completo/fusion_actualizar_pcb_final.py): documenta esta importación; no repetir sobre una instancia ya actualizada. Exportación STEP KiCad con origen `100x100mm`, modelos, pads, pistas y zonas a `exports/oled-encoder/PCB_UI_Final_20261025.step`.

[Exportador Fusion](../mecanica/auto-completo/fusion_exportar_final.py), mediante MCP en AutoFinalRobo2: sin `BATCH` guarda maestros/soporte; con `BATCH=(0,10)` y luego `(10,21)` produce definiciones y manifest en `exports/2026-10-25-final/parts`. El panel se exporta desde el contexto raíz para conservar posiciones particulares de instancias. Para otra revisión usar una carpeta nueva.

[Blender](../mecanica/auto-completo/blender_final_20261025.py), mediante Agent MCP: importar script y ejecutar `import_final()`, `prepare_light()`, `fit_budget()`, `export_final()`. La importación exige que no exista la colección final. Desde el maestro terminado basta `export_final()` para regenerar el GLB. [Procedencia de modelos aportados](../kicad/libraries/3d/fusion-ui/models-source.json); no se atribuye una licencia nueva.

STEP/STL/USDZ, renders, dist y paquete LittleFS son regenerables y quedan excluidos de Git. Se versionan GLB de Web/public, maestros editables, bibliotecas propias y scripts.
