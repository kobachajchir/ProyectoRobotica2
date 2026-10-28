"""Secciones J1 v09 sometidas a la propuesta STS, sin modificarlas."""
from pathlib import Path
import json
H=Path(__file__).resolve().parent
src=(H.parent/'preseleccion_actuadores_v10'/'uniones.py').read_text(encoding='utf-8')
src=src.replace("'analisis-actuadores-v10'","'analisis-actuadores-v11-STS'")
src=src.replace("'resultados.json'","'STS_resultados.json'").replace("'uniones_resultados.json'","'STS_uniones_resultados.json'")
row=json.loads((H/'STS_resultados.json').read_text(encoding='utf-8'))['results'][0]
# Metadata correction only; the dynamics weights already include80g each.
row['fixed_base_extra_mass_g']=340.
row['note']='Fixed340g = reductorfijo150 + motor80-90 + fuente200; frame140g excluye fuente en piso. CG fuente x=y=0.'
(H/'STS_resultados.json').write_text(json.dumps(dict(results=[row]),ensure_ascii=False,indent=2),encoding='utf-8')
line="fixedF=np.array([0,0,-fixed['mass_g']*9.80665/1000]);fixedM=np.cross(fixed['com_mm'],fixedF)"
extra="""
proposal=json.loads((H/'STS_resultados.json').read_text(encoding='utf-8'))['results'][0]
cg_extra=np.array(proposal['fixed_extra_cg_world_mm'])
extra_frame=np.array([0,0,-.140*9.80665])
deadF+=extra_frame;deadM+=np.cross(cg_extra-[0,p['v05_axis_y'],60],extra_frame)
fixedF+=np.array([0,0,-.340*9.80665]);fixedM+=np.cross(cg_extra,extra_frame)
"""
assert src.count(line)==1
src=src.replace(line,line+extra)
exec(compile(src,str(H/'uniones_STS_heredadas.py'),'exec'))
