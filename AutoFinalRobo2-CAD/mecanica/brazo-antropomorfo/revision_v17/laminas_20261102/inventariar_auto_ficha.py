"""Inventario del modelo editable del auto usado para las fichas generales."""
import bpy,json
from pathlib import Path
H=Path(__file__).resolve().parent
carfile=H.parents[2]/'auto-completo/AutoFinalRobo2-Final-20261025.blend'
with bpy.data.libraries.load(str(carfile),link=False) as (src,dst):
 dst.collections=['AFR2_Final_Detallado_20261025']
out=[{'name':o.name,'type':o.type} for o in dst.collections[0].all_objects]
(H/'inventario_auto_ficha.json').write_text(json.dumps(out,indent=2),encoding='utf-8')
print(json.dumps(out),flush=True)
