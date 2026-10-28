"""Cargas mixtas QDD aplicadas a las secciones J1 existentes v09."""
from pathlib import Path
import json,importlib.util
import numpy as np
H=Path(__file__).resolve().parent;V=H.parent/'revision_v09'
spec=importlib.util.spec_from_file_location('qdd_native',V/'calcular_v09.py');dyn=importlib.util.module_from_spec(spec);spec.loader.exec_module(dyn)
parts,_=dyn.load();motor=next(p for p in parts if p['component']=='V09_M01_GX3270BLS_Base')
fr=dyn.root_frames(np.radians([90,90,0,0,0,0]));T=fr['00_Base_control'];cg0=(T[:3,:3]@motor['_local_g']+T[:3,3])*1000
cgheat=cg0+T[:3,:3]@np.array([0,40,0])
E=H.parents[2]/'exports'/'analisis-actuadores-v12-QDD';a=np.load(E/'cargas.npz');q=a['q_deg']
expanded=[];yaw_end=[];eq=[]
for yaw in range(-90,91,10):
    for i,qq in enumerate(q):
        if not -90<=qq[0]+yaw<=90:continue
        expanded.append(i);yaw_end.append(yaw);v=qq.copy();v[0]+=yaw;eq.append(v)
np.savez_compressed(E/'cargas_comparadas.npz',F_M_world=a['F_M_world'],F_M_local=a['F_M_local'],
    case_names=a['case_names'],expanded_to_source=expanded,expanded_yaw_end_deg=yaw_end,expanded_q_deg=eq)
inp=json.loads((H/'QDD_resultados.json').read_text(encoding='utf-8'))
for row in inp['rows']:row['states']=len(q);row['expanded_J1_states']=len(eq)
(H/'QDD_secciones_entrada.json').write_text(json.dumps(dict(results=inp['rows']),ensure_ascii=False,indent=2),encoding='utf-8')
src=(H.parent/'preseleccion_actuadores_v10'/'uniones.py').read_text(encoding='utf-8')
src=src.replace("'analisis-actuadores-v10'","'analisis-actuadores-v12-QDD'")
src=src.replace("'resultados.json'","'QDD_secciones_entrada.json'").replace("'uniones_resultados.json'","'QDD_uniones_resultados.json'")
marker="rows=[]\nfor ci,name in enumerate(data['case_names']):"
addition="""
deadF0=deadF.copy();deadM0=deadM.copy();fixedF0=fixedF.copy();fixedM0=fixedM.copy()
base_cg=np.array(BASE_CG_LITERAL);heat_cg=np.array(HEAT_CG_LITERAL)
rows=[]
for ci,name in enumerate(data['case_names']):
    proposal=results['results'][ci]
    df=np.array([0,0,-(.398-.090+.075)*9.80665])
    dh=np.array([0,0,-proposal['plate_each_g']/1000*9.80665])
    deadF=deadF0+df+dh
    deadM=deadM0+np.cross(base_cg-[0,p['v05_axis_y'],60],df)+np.cross(heat_cg-[0,p['v05_axis_y'],60],dh)
    fixedF=fixedF0+df+dh+np.array([0,0,-.650*9.80665])
    fixedM=fixedM0+np.cross(base_cg,df)+np.cross(heat_cg,dh)
""".replace('BASE_CG_LITERAL',repr(cg0.tolist())).replace('HEAT_CG_LITERAL',repr(cgheat.tolist()))
assert src.count(marker)==1
src=src.replace(marker,addition)
marker="    row=dict(name=str(name),states=len(F),bearing_P0_N=float(P0.max()),"
addition="""
    stability=[]
    for ground_z in [0,-40,-80]:
        groundM=FM[:,3:]+np.cross([0,0,-ground_z],FM[:,:3])
        N=-FM[:,2]
        for bx,by in [(80,60),(100,80),(120,100)]:
            req=np.maximum.reduce([np.abs(groundM[:,0])/by-N,np.abs(groundM[:,1])/bx-N,-N,np.zeros_like(N)])/9.80665
            worst=int(np.argmax(req))
            stability.append(dict(ground_z_mm=ground_z,halfspan_x_mm=bx,halfspan_y_mm=by,
                minimum_chassis_mass_kg=float(req[worst]),worst_q_deg=data['expanded_q_deg'][worst].tolist(),
                friction_mu_needed_with_1kg_chassis=float(np.max(np.linalg.norm(FM[:,:2],axis=1)/np.maximum(N+9.80665,1e-6)))))
    row=dict(name=str(name),states=len(F),bearing_P0_N=float(P0.max()),
"""
assert src.count(marker)==1
src=src.replace(marker,addition)
src=src.replace('    rows.append(row);print(',"    row['hypothetical_dynamic_stability']=stability\n    rows.append(row);print(")
exec(compile(src,str(H/'secciones_QDD_heredadas.py'),'exec'))
