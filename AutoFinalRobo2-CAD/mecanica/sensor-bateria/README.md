# Sensor de batería — conjunto ligero para la presentación

AFR2-20261109-10. Escena y export derivados por pedido del usuario para «Nuestros agregados» en el recorrido de armado. No sustituye el esquema eléctrico ni acredita montaje físico.

- **Editable:** `AFR2_Sensor_Bateria_Cableado_20261109_v2.blend`, también abierto en Blender.
- **Web:** `AutoFinalRobo2-Web/public/models/AFR2_Sensor_Bateria_Cableado.glb` desde la raíz del proyecto: **11.692 bytes**, **7.969 bytes gzip**, **1.400 triángulos**.
- **Procedencia y parámetros:** `entrega_agregados.json`.
- **Exporta únicamente:** R1 10k, R4 10k, C3 (104/100nF), J3 A5 y sus cables. Una malla, compresión Draco. No incluye OLED, encoder, soporte, voltímetro, portapilas ni celdas: sus mallas existentes se reutilizan en el navegador.

## Disposición final solicitada

Composición compacta y alineada verticalmente: voltímetro arriba, portapilas al centro y divisor debajo. El rojo sale recto hacia la izquierda y el negro hacia la derecha; suben por carriles laterales paralelos, con derivaciones del mismo color hacia ambos lados del voltímetro. Conexiones cortas en polilíneas, sin los bucles exteriores largos de los borradores iniciales. Los modelos reutilizados se reposicionan solo para la presentación del navegador.

El nodo entre R1/R4 va al pin A5. C3 está entre ese nodo y GND, en paralelo con R4. Relación nominal del divisor de resistencias iguales: `V_A5 = V_BATT / 2`; los textos usan 8V como referencia de100% de batería y4V en A5, sin confundirlo con el100% de la escala ADC. El modelo radial de capacitor solicitado es una representación visual del C3 indicado, no evidencia de la pieza real montada.

## Fuentes

Los tres STEP aportados por el usuario se resolvieron desde `C:/Program Files/KiCad/10.99/share/kicad/3dmodels/`, sustituyendo `${KICAD10_3DMODEL_DIR}`:

- `Connector_PinHeader_1.00mm.3dshapes/PinHeader_1x01_P1.00mm_Horizontal.step`.
- `Resistor_THT.3dshapes/R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal.step`, reutilizado para R1/R4.
- `Capacitor_THT.3dshapes/CP_Radial_Tantal_D4.5mm_P2.50mm.step`.

Los hashes exactos y rutas resueltas están en el JSON de procedencia. Modelos de biblioteca KiCad elegidos por el usuario; autoría/licencia individual no confirmadas en esta entrega. Las fuentes instaladas no se modifican. El esquema del usuario en `kicad/oled-encoder/sensorbateria.kicad_sch` se conserva sin cambios por esta tarea.

## Producción

1. `convertir_step.py`, con el Python de FreeCAD, convierte las superficies STEP a mallas en milímetros. La captura intermedia queda en `AutoFinalRobo2-CAD/exports/20261109-sensor-bateria/step_meshes.json`, excluida de Git.
2. `armar_agregados_blender.py` prepara geometría y referencias en una escena nueva; usa una copia del GLB del auto sin sobrescribirlo. No sustituye las fuentes CAD.
3. `exportar_ligero_blender.py` produce la distribución compacta final con cuatro componentes, cableado de sección reducida y export Draco. Guarda el `.blend`, el GLB Web y sus metadatos. Las mallas se simplifican en Blender, no se reemplazan por componentes inventados.

La versión de Blender utilizada es4.5.1 LTS. La exportación, teselación y simplificación son producción autorizada del asset. No se hicieron renders, ERC/DRC, comprobaciones eléctricas/mecánicas, pruebas, compilaciones o flasheos. El archivo nuevo está integrado en la lista de recursos del empaquetado Web/ESP; no se generó ni instaló una nueva imagen LittleFS.
