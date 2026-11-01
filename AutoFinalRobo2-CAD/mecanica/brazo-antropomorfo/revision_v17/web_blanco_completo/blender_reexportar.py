"""Reejecuta las fases conocidas en colecciones nuevas; conserva version anterior."""
from pathlib import Path
HERE=Path('D:/ProyectoRobotica2/AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/revision_v17/web_blanco_completo')
source=(HERE.parent/'web_blanco/blender_preparar.py').read_text(encoding='utf-8')
source=source.replace("revision_v17/web_blanco')","revision_v17/web_blanco_completo')")
source=source.replace('brazo-v17-blanco-20261031','brazo-v17-completo-20261031')
source=source.replace('AFR2_Brazo_v17_Piezas_Origen','AFR2_Brazo_v17_Piezas_Completas')
source=source.replace('AFR2_Brazo_v17_Web','AFR2_Brazo_v17_Web_Completo')
exec(compile(source,'blender_preparar_completo.py','exec'))
