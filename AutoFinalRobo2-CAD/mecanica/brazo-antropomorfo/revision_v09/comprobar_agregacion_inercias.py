"""Comprobación numérica solicitada del modelo de cargas, sin ejecutar Fusion."""
import json
import numpy as np
import calcular_v09 as c

parts,_=c.load()
rows=[]
for q,dq,ddq in [([0,90,0,0,0,0],[0]*6,[0]*6),
                 ([40,89.6,-.4,.4,-.4,.4],[15,-15,15,-15,15,-15],[-600,600,-600,600,-600,600])]:
    args=[np.radians(x) for x in [q,dq,ddq]]
    bodies=c.body_state_at(parts,*args)
    groups=c.state_at(parts,*args)
    rows.append(dict(q_deg=q,max_force_difference_N=max(np.linalg.norm(np.array(a['force_N'])-b['force_N']) for a,b in zip(bodies,groups)),
                     max_moment_difference_Nm=max(np.linalg.norm(np.array(a['moment_Nm'])-b['moment_Nm']) for a,b in zip(bodies,groups))))
out=dict(scope='Equivalencia numérica de masa/CG/tensor agregado por conjunto rígido frente a suma por cuerpo, con derivadas finitas de igual paso. No ensayo físico.',rows=rows)
(c.HERE/'equivalencia_inercias.json').write_text(json.dumps(out,ensure_ascii=False,indent=2),encoding='utf-8')
print(json.dumps(out,ensure_ascii=False))
