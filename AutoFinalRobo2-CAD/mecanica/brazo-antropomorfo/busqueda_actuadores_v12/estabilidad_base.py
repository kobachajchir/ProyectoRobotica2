"""Sensibilidad dinamica de apoyo: no confundirla con comprobacion del auto."""
from pathlib import Path
import json,math
import numpy as np
H=Path(__file__).resolve().parent;V=H.parent/'revision_v09'
z=np.load(H.parents[2]/'exports'/'analisis-actuadores-v11'/'cargas_comparadas.npz')
ii=z['expanded_to_source'];ci=z['case_index'];ll=z['source_trajectory_labels']
keep=np.array([str(ll[ci[i]]).endswith('_50ms') for i in ii]);ii=ii[keep]
yaw=np.radians(z['expanded_yaw_end_deg'][keep]);qq=z['expanded_q_deg'][keep]
raw=json.loads((V/'secciones_CAD_v09.json').read_text(encoding='utf-8'));p={k:v['value_internal']*10 for k,v in raw['parameters'].items()}
fixed=next(x for x in json.loads((V/'estatica_v09.json').read_text(encoding='utf-8'))['groups'] if x['name']=='00_Base_control')
fixedF=np.array([0,0,-fixed['mass_g']/1000*9.80665]);fixedM=np.cross(fixed['com_mm'],fixedF)
def rotate(v):
    c,s=np.cos(yaw),np.sin(yaw);return np.c_[c*v[:,0]-s*v[:,1],s*v[:,0]+c*v[:,1],v[:,2]]
allw=z['F_M_world'];rows=[]
for index,name,extra in [(0,'v09_actual',0),(7,'DS51150_reforzado',.200)]:
    wr=allw[ii,0,index];F=rotate(-wr[:,0])+fixedF+[0,0,-extra*9.80665]
    M=rotate(-wr[:,1])*1000+np.cross([0,p['v05_axis_y'],p['base_top']],rotate(-wr[:,0]))+fixedM
    results=[]
    for gz in [0,-40,-80]:
        gm=M+np.cross([0,0,-gz],F);N=-F[:,2]
        for bx,by in [(80,60),(100,80),(122.5,100)]:
            req=np.maximum.reduce([np.abs(gm[:,0])/by-N,np.abs(gm[:,1])/bx-N,-N,np.zeros_like(N)])/9.80665
            worst=int(np.argmax(req));results.append(dict(ground_z_mm=gz,halfspan_x_mm=bx,halfspan_y_mm=by,
                minimum_centered_chassis_mass_kg=float(req[worst]),worst_q_deg=qq[worst].tolist()))
    rows.append(dict(name=name,states=len(ii),results=results))
out=dict(scope='Apoyos RECTANGULARES HIPOTETICOS centrados en origen del brazo; auto quieto,CG agregadox=y=0. No se midieron contactos,masa ni CG del auto. Sin margen extra ni aceleracion del AGV.',
    method='sumar reacciones arm+base al plano de apoyo; |Mx|<=by*N y |My|<=bx*N; masa centrada adicional necesaria en cada instante.',rows=rows)
(H/'estabilidad_base_resultados.json').write_text(json.dumps(out,ensure_ascii=False,indent=2),encoding='utf-8')
for r in rows:print(r['name'],[(x['ground_z_mm'],x['halfspan_x_mm'],x['halfspan_y_mm'],round(x['minimum_centered_chassis_mass_kg'],2)) for x in r['results']],flush=True)
