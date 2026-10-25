# AFR2-20261025-12 — PCB OLED/encoder y soporte

Pedido del usuario: conectar KiCad/KiPilot con los modelos de Fusion, corregir el esquema, rutear una placa simple faz de hasta 20 × 35 mm y preparar soporte con M3 y torretas de latón de 15 mm en una ranura de esquina del auto.

## Contrato eléctrico

- J1 hembra 1x05, paso 2,54 mm: 1 CLK/D2, 2 DT/D3, 3 SW/D42, 4 SDA/D20, 5 SCL/D21. I2C se cablea a las cabeceras SDA/SCL del shield.
- J2 alimentación: 1 +3V3, 2 GND. **Paso 2,00 mm confirmado por el usuario**, reemplaza la propuesta anterior de 2,54 mm. El alojamiento JST concreto no está identificado por número de parte.
- OLED 0,96 pulgadas, 128×64, socket hembra 1x04 de 2,54 mm. De frente con pines arriba: GND, VCC, SCL, SDA, confirmado por el usuario. En esta disposición el módulo gira 180° para quedar fuera del cobre; la orientación de lectura del firmware debe decidirse después.
- Encoder sin breakout: comunes del giro y pulsador a GND, A→CLK, B→DT, SW→D42, coherente con los pull-ups y SW activo bajo de las fuentes actuales. No lleva alimentación directa; +3V3 alimenta el OLED y los capacitores.
- Se unificó +3V3 y se sustituyeron los capacitores de 4,7 µF sobre contactos del esquema aportado por desacoplo entre alimentación y GND: C1 100 nF/16 V, C2 4,7 µF/10 V. Los valores físicos y la tensión nominal deben corresponder al montaje adquirido.
- No se agregó adaptación de nivel ni pull-ups I2C nuevos. La compatibilidad entre OLED a 3,3 V, pull-ups del módulo/shield y Mega a 5 V queda pendiente de identificar esos componentes; este CAD no acredita esa compatibilidad.

## Entrega CAD

Fuentes en [proyecto KiCad](../../AutoFinalRobo2-CAD/kicad/oled-encoder/README.md); informe de montaje en [CAD](../../AutoFinalRobo2-CAD/docs/PANEL_UI_20261025.md). Placa 20×35×1,5 mm, ruteo B.Cu, GND por plano, dos agujeros NPTH Ø3,2 y zonas sin cobre R3 alrededor de cabezas Ø5. El esquema anterior de `Placa Kicad/` se conserva.

Modelos STEP exportados de los diseños del usuario en Fusion, asociados por rutas de proyecto. El JST original tenía 2,9 mm entre terminales: se conserva y se aporta una variante de representación con terminales separados 2 mm. Esto no sustituye el plano de un conector comercial específico. La huella del encoder usa terminales/pestañas del CAD aportado; sus cotas reales no están completamente definidas en la imagen.

Soporte y bisel desmontable construidos en el AutoFinalRobo2 abierto, con bastidor de fondo abierto, paredes de 3 mm, torretas M3×15, cabezas Ø5, fijaciones M3 al chasis y dos tornillos superiores del OLED. La ranura existente del CAD mide aproximadamente 2,78 mm de ancho; no se modifica el chasis. Su paso real para M3 o mecanizado a Ø3,2 queda pendiente.

## Estados reales

| Módulo | Estado de esta entrega |
|---|---|
| Atmega | Sin cambio de firmware, sin compilar ni flashear |
| ESP | Sin cambio |
| Web | Sin cambio |
| CAD | Fuentes KiCad, ruteo y modelos/soporte de referencia creados |
| Montaje | Sin prueba física, ERC/DRC ni análisis de interferencias |

No se publican Gerbers como una revisión aprobada para fabricar. Una exportación STEP/STL/F3D es una salida del diseño y no una validación del montaje.
