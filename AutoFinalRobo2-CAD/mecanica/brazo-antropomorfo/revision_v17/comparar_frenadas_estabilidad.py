"""Sensibilidad explicita del tiempo de parada; no relaja el requisito vigente.

Comparar propuestas antes de decidir/modelar. No FEA ni shape optimization.
"""
from pathlib import Path
import runpy,json
from predimensionar_estabilidad_integral import adjustment
H=Path(__file__).resolve().parent

if __name__=='__main__':
 snapshot=H.parents[2]/'.downloads/v17_geometry_stability_snapshot.json'
 snapshot.write_bytes((H/'geometria_v17.json').read_bytes())
 rows=[]
 for tail in [None,-100]:
  for tb in [.1,.2,.3,.5]:
   name=f'propuesta_42_cola{tail}_{int(tb*1000)}ms.json'
   runpy.run_path(str(H/'calcular_vuelco_v17.py'),run_name='__main__',init_globals=dict(GEOMETRY_PATH17=str(snapshot),AJUSTAR_VUELCO17=adjustment(154,tail,42),BRAKE_SECONDS17=tb,OUT_FILE17=name,LAYOUT_NOTE17=f'Comparacion NO implementada: centroJ1Y-32, hombro154, muneca42, cola{tail}, parada{tb}s. No aceptar un tiempo nuevo sin acuerdo del usuario.'))
   d=json.loads((H/name).read_text());rows.append(dict(tail_mm=tail,brake_seconds=tb,utilization=d['worst']['utilization'],tip_factor=1/d['worst']['utilization'],all_yaws_sampled_fs1_2=len(d['safe_yaws_sampled_fs1_2'])==37,geometry_sha256=d['geometry_sha256']))
 (H/'sensibilidad_frenadas_estabilidad.json').write_text(json.dumps(rows,indent=2),encoding='utf-8')
 print(json.dumps(rows))
