"""Figura explicativa del calculo, sin ejecutar CAD ni ensayos."""
from pathlib import Path
import json
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import numpy as np

p=Path(__file__).resolve().parent
a=json.loads((p/'estatica_v07.json').read_text(encoding='utf-8'))
b=json.loads((p.parent/'analisis_cargas_200g'/'resultados_estatica.json').read_text(encoding='utf-8'))['cases']['CAD_macizo_200g']
d=json.loads((p/'frenadas_hipotesis_v07.json').read_text(encoding='utf-8'))
fig,ax=plt.subplots(1,2,figsize=(12,5))
names=['J2','J3','J5'];x=np.arange(3)
old=[next(z for z in b['joints'] if z['joint']==j)['servo_static_kgfcm_eta'] for j in names]
new=[next(z for z in a['joints'] if z['joint']==j)['servo_static_kgfcm_eta'] for j in names]
ax[0].bar(x-.18,old,.36,label='v06',color='#9ca3af')
ax[0].bar(x+.18,new,.36,label='v07',color='#c6181f')
ax[0].set_xticks(x,['Hombro J2','Codo J3','Muñeca J5'])
ax[0].set_ylabel('Par de servo estático (kgf·cm)')
ax[0].set_title('El refuerzo y el motor cambian el peso propio')
ax[0].legend()
for j,c in [('J1','#374151'),('J2','#c6181f'),('J3','#2563eb')]:
 vals=[max(z['value'] for z in d['peaks'] if z['joint']==j and z['metric']=='servo_torque_kgfcm' and f'_{t}ms_30deg_s' in z['case']) for t in [50,100,200]]
 ax[1].plot([50,100,200],vals,'o-',label=j,color=c)
ax[1].set_title('Frenadas por eje: hipótesis de 30°/s')
ax[1].set_xlabel('Tiempo de parada (ms)')
ax[1].set_ylabel('Máximo par de servo calculado (kgf·cm)')
ax[1].set_xticks([50,100,200]);ax[1].legend()
for aa in ax:aa.grid(axis='y',alpha=.25);aa.set_axisbelow(True)
fig.suptitle('Brazo PVC32 · v07 GX3270BLS · carga de 200 g',fontweight='bold')
fig.text(.5,.015,'Modelo rígido y masas uniformes; solo ejes individuales. No acredita operación, impactos ni envolvente completa.',ha='center',fontsize=9)
fig.tight_layout(rect=(0,.06,1,.94))
fig.savefig(p/'comparacion_cargas_v07.png',dpi=160)
fig.savefig(p/'comparacion_cargas_v07.svg')
