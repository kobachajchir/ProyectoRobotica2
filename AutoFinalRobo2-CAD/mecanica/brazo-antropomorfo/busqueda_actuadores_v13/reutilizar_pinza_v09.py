"""Conserva masas, centroides y tensores de la pinza nativa, a apertura50mm.

No ejecuta Fusion ni modifica v09. El cambio MG90S->MG92B solo presupone masa;
su alojamiento, spline y holgura tienen que revisarse antes de fabricar.
"""
from pathlib import Path
import importlib.util,json
import numpy as np
H=Path(__file__).resolve().parent
spec=importlib.util.spec_from_file_location('cargas09',H.parent/'revision_v09'/'calcular_v09.py')
c=importlib.util.module_from_spec(spec);spec.loader.exec_module(c)
source,_=c.load();f=c.root_frames(np.zeros(6));ref=f['06_Herramienta'];parts=[]
for b in source:
    if b['root'][:2] not in ['06','07','08','10']:continue
    T=np.linalg.inv(ref)@f[b['root']];cg=(T@np.r_[b['_local_g'],1])[:3]*1000
    I=T[:3,:3]@b['_I_group']@T[:3,:3].T;m=b['mass_g'];name=b['component']+' / '+b['body']
    mat={'PLA PrintaLot impreso':'PLA','Servo':'Servo','Almohadilla':'Pads','Horn polimero':'Polimero','Horn aluminio':'Al6061','Laton':'Laton','Rodamiento':'Rodamiento'}.get(b['category'],'Acero')
    if b['component']=='V02_M07_MG90S_Pinza':I*=13.8/13.4;m*=13.8/13.4;name+=' RESERVA MG92B'
    parts.append(dict(root='06',name=name,mass_g=m,cg_local_mm=cg.tolist(),box_mm=[0,0,0],material=mat,
        tensor_kg_m2=I.tolist(),detail='Geometria/CG/tensor v09, pinza50mm. Volumen nativo; MG92B reemplazo de masa pendiente de montaje.'))
parts.append(dict(root='06',name='Reserva adaptacion MG92B',mass_g=2,cg_local_mm=[30,0,35],box_mm=[30,20,20],material='PLA'))
(H/'pinza_reutilizada.json').write_text(json.dumps(parts,ensure_ascii=False,indent=2),encoding='utf-8')
print('Pinza conservada + cambio motor:',sum(x['mass_g'] for x in parts),'g')
