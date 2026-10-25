# Insumos OLED/encoder de 25/10/2026

Adiciones posteriores: `encoder-pullups-usuario.png` define resistencias de 10 kΩ hacia +3V3 para CLK/DT y comunes a GND; `torretas-comerciales-usuario.png` muestra separadores comerciales. `usuario-antes-revision-B.kicad_*` conserva el estado anterior a TH/pull-ups. El usuario confirmó TH, 15 mm sólo para OLED, encoder directo, 5 mm entre soporte y auto y PCB directa al soporte con tornillos/tuercas. [Entrega final manual](../../../docs/MODELO_FINAL_WEB_20261025.md).

Copias de las tres imágenes adjuntas por el usuario: `oled-dimensiones.png`, `encoder-dimensiones.png` y `ranura-montaje-auto.png`. Se conservan las fuentes `.kicad_sch` y `.kicad_pro` del esquema original aportado; no son la revisión corregida.

Confirmaciones textuales del usuario: OLED de frente con pines arriba = GND/VCC/SCL/SDA; alimentación 3,3 V; JST de power con **2 mm entre pines**; OLED mediante socket header hembra de cuatro pines de 2,54 mm. El header de cinco pines es el conector de señales externo.

Las imágenes no identifican número de parte del JST, terminales completos del encoder ni pull-ups/controlador del OLED. Autoría/licencia de las imágenes y CAD de terceros no están identificadas; se usan como insumos suministrados en este proyecto, sin declarar redistribución autorizada públicamente.

Modelos originales en los diseños del usuario de Fusion: `OLED0.96CAD`, `KY040RotaryEncoderCAD`, `KY040RotaryEncoderKnobCAD`, `4-Pin Header - Female - 2.54mm`, `5-Pin Header - Female - 2.54mm`, `jst2pins`. Enlace de la entrega: [informe](../../../docs/PANEL_UI_20261025.md).
