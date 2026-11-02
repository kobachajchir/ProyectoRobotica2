"""Figura estatica del informe; no representa huellas medidas de rodillos."""
from pathlib import Path
import json
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle

H=Path(__file__).resolve().parent
r=json.loads((H/'resultados.json').read_text(encoding='utf-8'))
fig,ax=plt.subplots(figsize=(8,6.5),layout='constrained')
ax.add_patch(Rectangle((-107,-62),214,124,facecolor='#edf3f7',edgecolor='#334155',lw=2,label='Apoyo nominal entre centros'))
ax.scatter([-107,107,-107,107],[-62,-62,62,62],s=90,c='#334155',marker='s',zorder=4)
ax.scatter([0],[0],c='#64748b',marker='+',s=100,label='Centro del auto (1,5 kg)')
for a,color,label in zip(r['cardinal_extended'][:3],['#2563eb','#dc2626','#0f766e'],['Brazo lateral','Brazo hacia adelante','Brazo hacia atras']):
    x,y,z=a['cg_from_chassis_center_ground_mm']
    ax.scatter([x],[y],s=90,c=color,zorder=5,label=label)
ax.annotate('CG frontal: 0,85 mm del borde',xy=(-2.63,61.15),xytext=(-90,92),arrowprops=dict(arrowstyle='->',color='#dc2626'),color='#b91c1c',fontsize=11)
ax.set(xlim=(-145,145),ylim=(-110,110),xlabel='X transversal [mm]',ylabel='Y longitudinal, + hacia el frente [mm]',title='Conjunto actual con 200 g: proyeccion del centro de masa')
ax.set_aspect('equal');ax.grid(alpha=.15);ax.legend(loc='lower center',fontsize=9)
fig.text(.5,.006,'Apoyo inferido del CAD. No es la huella fisica de los rodillos Mecanum.',ha='center',fontsize=9,color='#475569')
fig.savefig(H/'estabilidad_planta.png',dpi=180)
fig.savefig(H/'estabilidad_planta.svg')
