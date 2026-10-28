"""Optimo continuo en J2 para un presupuesto FIJO de masas/inercias."""
from pathlib import Path
import json
import numpy as np
H=Path(__file__).resolve().parent
z=np.load(H.parents[2]/'exports'/'analisis-actuadores-v12'/'cargas.npz')
cfg=json.loads((H/'resultados.json').read_text(encoding='utf-8'))
signed=z['motor_torque_signed_kgfcm'];v=np.abs(z['dq_rad_s'][:,1])*180/np.pi
n=np.linspace(1,13.49,1250);rows=[]
for eta in [.70,.85]:
    for grip in ['MG92B','STS3215']:
        i=next(i for i,r in enumerate(cfg['rows']) if r['ratio'][1]==9 and r['eta']==eta and r['gripper']==grip)
        load=np.abs(signed[:,i,1])*9*eta
        cap=eta*n[:,None]*np.minimum(16,45*np.maximum(0,1-v[None,:]*n[:,None]/405))
        margins=np.min(cap/(load[None,:]+1e-12),axis=1);best=int(np.argmax(margins))
        rows.append(dict(eta=eta,gripper=grip,optimal_ratio=float(n[best]),best_minimum_margin=float(margins[best]),
            max_joint_kgfcm=float(load.max()),passes=bool(margins[best]>=2)))
q=json.loads((H/'QDD_resultados.json').read_text(encoding='utf-8'))
r=next(r for r in q['rows'] if r['name']=='RS01_plate5_eta0.7_I2')
budget=[]
for j in [3,4,5]:
    headroom=r['capacity_Nm'][j]/2-r['demand_motor_Nm'][j]
    budget.append(dict(joint='J'+str(j+1),scalar_axial_output_inertia_budget_kgm2=max(0,headroom/(r['ratio'][j]*np.radians(900))),
        scope='Presupuesto escalar conservador abs(tau_externa)+J*maxabs(alpha_motor)<=cap/2. Sin acoplamiento con padre ni energia/freno. No propiedad medida del STS.'))
out=dict(scope='Optimo dentro de modelo lineal de sensibilidad de curvaSTS3250,12V,velocidad minima405deg/s,par inferior45kgfcm y nominal16. Masas/CG/tensores fijos, no optimizacion global de arquitectura.',
    rows=rows,unknown_STS_inertia_budget=budget)
(H/'optimo_y_presupuestos.json').write_text(json.dumps(out,ensure_ascii=False,indent=2),encoding='utf-8')
print(json.dumps(out,indent=2))
