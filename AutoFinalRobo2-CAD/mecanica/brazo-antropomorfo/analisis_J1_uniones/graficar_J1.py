"""Figuras del analisis de J1; no render sintetico de resistencia."""
from pathlib import Path
import json,csv,math
import numpy as np
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from matplotlib.patches import Circle,Arc

H=Path(__file__).resolve().parent
r=json.loads((H/'resultados_J1.json').read_text(encoding='utf-8'))
fig,ax=plt.subplots(1,3,figsize=(14,4.8))
s=r['bearing_spacing_sensitivity']
ax[0].plot([x['centres_mm'] for x in s],[x['couple_radial_N'] for x in s],'o-',color='#b91c1c')
ax[0].set_xticks([8,16,24,32]);ax[0].set_xlabel('Separación entre centros (mm)');ax[0].set_ylabel('Fuerza ideal del par por apoyo (N)')
ax[0].set_title('Mismo momento: 4,522 N·m')
ax[0].annotate('v08',xy=(8,s[0]['couple_radial_N']),xytext=(10,520))
ax[0].grid(alpha=.2)
rows=list(csv.DictReader((H/'anclajes.csv').open(encoding='utf-8-sig')))
labels=list('ABCDEF')+['M10'];xx=np.arange(7)
for delta,model,name,color in [(-.18,'rigidez_igual','Igual rigidez','#6b7280'),(.18,'rigidez_At_sobre_L','At/L','#2563eb')]:
 vals=[max(float(z['service_tension_N']) for z in rows if z['bolt']==n and z['model']==model) for n in labels]
 ax[1].bar(xx+delta,vals,.36,label=name,color=color)
ax[1].set_xticks(xx,labels);ax[1].set_ylabel('Máxima tracción incremental (N)');ax[1].set_title('Anclajes: dos repartos ideales');ax[1].legend();ax[1].grid(axis='y',alpha=.2)
a=math.sqrt(16-3.3**2);theta0=math.asin(-3.3/4)
t=np.linspace(theta0,math.pi-theta0,250)
x=4*np.cos(t);y=4*np.sin(t)
ax[2].plot(np.r_[x,x[0]],np.r_[y,y[0]],color='#374151',lw=2)
ax[2].plot([-a,0],[-3.3,-3.3],color='#dc2626',lw=4)
for z in np.linspace(-a,0,6)[1:-1]:ax[2].arrow(z,-4.6,0,1.1,width=.015,head_width=.16,head_length=.18,length_includes_head=True,color='#dc2626')
ax[2].axhline(0,color='#d1d5db',lw=.7);ax[2].axvline(0,color='#d1d5db',lw=.7)
ax[2].annotate('a = 2,261 mm',xy=(-a,-3.3),xytext=(-5.6,-5.1),fontsize=9)
ax[2].text(-5.6,4.6,'Ø8 mm · plano a 3,3 mm del centro',fontsize=9)
ax[2].text(-5.6,-6.6,'Media cara comprimida; h = 8,8 mm\nSin fricción acreditada: p media ≈71 MPa',fontsize=9)
ax[2].set_xlim(-6,6);ax[2].set_ylim(-7.7,5.5);ax[2].set_aspect('equal');ax[2].set_title('Modelo simplificado del acople D');ax[2].axis('off')
fig.suptitle('J1 v08: cálculo bajo hipótesis · 200 g · 30°/s · parada 50 ms',fontweight='bold')
fig.text(.5,.015,'No es FEA de sólidos ni ensayo. El contacto D y los repartos de anclajes requieren comprobación posterior.',ha='center',fontsize=9)
fig.tight_layout(rect=(0,.06,1,.93))
fig.savefig(H/'J1_resultados.png',dpi=160);fig.savefig(H/'J1_resultados.svg')
