# PCB OLED/encoder

Revisión final manual guardada en KiCad 10.99 mediante KiPilot MCP: PCB de 20×35×2 mm, componentes TH, pistas B.Cu y plano GND. Abrir `PlacaOLED-Encoder-AutoRobo2.kicad_pro`. R2/R3 de 10 kΩ a +3V3 para CLK/DT; JST power de 2 mm; encoder directo y OLED en socket de cuatro pines. [Entrega final, modelos y límites](../../docs/MODELO_FINAL_WEB_20261025.md). El informe A se conserva como historia.

Fuente de requisitos: [PCB OLED/encoder](../../../AutoFinalRobo2-Contracts/hardware/PCB_OLED_ENCODER.md). Usar bibliotecas propias de ../libraries y rutas relativas cuando se definan. Preservar fuentes; colocar Gerber, taladros, renders y otras salidas regenerables en ../../exports, fuera de Git.

Bibliotecas de proyecto definidas en `fp-lib-table` y `sym-lib-table`. Los modelos usan `${KIPRJMOD}/../libraries/3d/fusion-ui`; capacitores, biblioteca KiCad10. Los scripts en `scripts/` registran la construcción inicial, con protección para no sobrescribir una PCB ya poblada/ruteada. No reejecutar órdenes de ruteo sobre pistas existentes: dibujan nuevas pistas por MCP.

Exportar STEP con `kicad-cli pcb export step --user-origin 100x100mm --include-tracks --include-pads --include-zones --subst-models --output <exports/oled-encoder/PCB_UI_Final_20261025.step> <PCB>`. Se conserva la pose ajustada por el usuario en Fusion. No regenerar desde los parámetros iniciales: el archivo nativo contiene los ajustes finales. Esta exportación no ejecuta ERC/DRC.
