"""Filtro de arquitectura antes de modificar CAD. No acredita piezas finales."""
from pathlib import Path
import sys,json,math
import numpy as np
H=Path(__file__).resolve().parent
sys.path.insert(0,str(H.parent/'busqueda_actuadores_v14'))
from modelo_dinamico import aggregate,bodies,rot
from compacto import trajectories
parts=[]
def add(root,name,mass,cg,box,kind='Presupuesto'):
    parts.append(dict(root=root,name=name,mass_g=mass,cg_local_mm=cg,box_mm=box,material=kind))
add('00','STS3215 J1',56,[30,0,25],[45.23,36.5,24.73],'Servo')
add('00','Control+alimentacion',280,[-20,0,20],[80,80,30])
add('00','Montura+apoyosJ1+retencion',96,[0,0,14],[90,80,28])
add('01','SalidaJ1',12,[0,0,4],[48,48,8])
add('01','STS3250 J2',75.5,[-32,35,0],[45.22,36.5,24.72],'Servo')
add('01','TransmisionJ2 completa sin motor',110,[-12,0,0],[74,52,54])
add('02','Eslabon1 CAD',30.8586,[0,0,27.5],[28,32,83],'Al6061')
add('02','STS3250 J3 remoto junto al hombro',75.5,[0,-35,0],[45.22,36.5,24.72],'Servo')
add('02','TransmisionJ3 completa sin motor',105,[0,-20,27.5],[50,52,55])
add('03','Eslabon2 CAD',29.4546,[0,0,25],[28,32,78],'Al6061')
add('03','ST3009 J4',46,[0,0,36.5],[20,30,36.5],'Servo')
add('03','Apoyos+eje+carcasa J4',33,[0,0,50],[32,32,30])
add('04','STS3215 J5',56,[0,25,20],[45.23,36.5,24.73],'Servo')
add('04','Horquilla+eje+apoyos J5',32,[0,0,20],[44,40,40])
add('05','ST3009 J6',46,[0,0,10],[20,30,36.5],'Servo')
add('05','Eje+apoyos+housingJ6',33,[0,0,15],[32,32,30])
add('06','Pinza CAD completa incluyendo motor',126.2555,[27.8366,22.5086,6.9574],[104,88,90])
add('06','Interfaz pinza+resorte+cable',10,[0,0,5],[35,30,20])
add('01','Tornilleria+cables generales',20,[0,0,10],[40,40,40])
add('90','Carga',200,[0,0,0],[50,50,50],'Carga')
p=dict(base_top=35,tower_h=40,axis_y_mm=0,link1=55,link2=50,wrist45=30,wrist56=42,payload_z=45)
if globals().get('AJUSTAR'):AJUSTAR(parts,p)
groups=aggregate(parts);roots=['01','02','03','04','05','06'];loads=[];forces=[];moments=[];cop=[]
for q,dq,ddq in trajectories():
    f,bb=bodies(q,dq,ddq,p,groups);tt=[];ff=[];mm=[]
    for j,r in enumerate(roots):
        F=sum((bb[x][0] for x in roots[j:]+['90']),np.zeros(3))
        M=sum((bb[x][1] for x in roots[j:]+['90']),np.zeros(3))-np.cross(f[r][:3,3],F)
        parent='00' if j==0 else roots[j-1];axis=f[parent][:3,2 if j in [0,3,5] else 1]
        tt.append(abs(M@axis));ff.append(float(np.linalg.norm(F)));mm.append(float(np.linalg.norm(M)))
    loads.append(tt);forces.append(ff);moments.append(mm)
    fixed=groups['00'];Fg=np.array([0,0,fixed[0]*9.80665]);Mg=np.cross(fixed[1],Fg)
    FF=sum((bb[x][0] for x in roots+['90']),np.zeros(3))+Fg
    MM=sum((bb[x][1] for x in roots+['90']),np.zeros(3))+Mg
    # Rectangulo de apoyo aun hipotetico; yaw sobre centro de base, auto1.5kg.
    pivot=np.array([0,0,.035]);floor=np.array([0,0,-.08])
    for yaw in [-90,-45,0,45,90]:
        RR=rot('Z',math.radians(yaw))[:3,:3];F=RR@(FF-Fg)+Fg
        M=RR@(MM-Mg-np.cross(pivot,FF-Fg))+np.cross(pivot,F-Fg)+Mg
        M-=np.cross(floor,F);F+=np.array([0,0,1.5*9.80665]);cop.append([-M[1]/F[2]*1000,M[0]/F[2]*1000])
maxloads=np.max(loads,axis=0);ratios=globals().get('RELACIONES',[1,3,3,1,1,1]);nom=globals().get('PARES_NOM',[10,16,16,3.8,10,3.8])
axes=[]
for j in range(6):
    capacity=nom[j]*.0980665*ratios[j]*(1 if ratios[j]==1 else .8)
    axes.append(dict(axis='J'+str(j+1),motor=['ST3215','STS3250','STS3250','ST3009','ST3215','ST3009'][j],ratio=ratios[j],peak_Nm=float(maxloads[j]),capacity_nominal_Nm=capacity,margin=float(capacity/maxloads[j])))
static_frames,static_bodies=bodies(np.radians([0,90,0,0,0,0]),np.zeros(6),np.zeros(6),p,groups)
total_m=sum(x[0] for x in groups.values());total_cg=sum((m*(static_frames[r][:3,:3]@cg+static_frames[r][:3,3]) for r,(m,cg,I) in groups.items()),np.zeros(3))/total_m
out=dict(scope='Presupuesto previo, geometria e inercias internas por cerrar. Pinza aun sin transformar a su montaje definitivo.',mass_without_load_g=sum(x['mass_g'] for x in parts)-200,parameters=p,axes=axes,peak_force_N=np.max(forces,axis=0).tolist(),peak_moment_Nm=np.max(moments,axis=0).tolist(),support_halfspan_mm=np.max(abs(np.array(cop)),axis=0).tolist(),cg_extended_with_payload_mm=(total_cg*1000).tolist(),parts=parts)
(H/globals().get('ARCHIVO','seleccion_previa.json')).write_text(json.dumps(out,indent=2),encoding='utf-8')
if not globals().get('SILENCIOSO'):print(json.dumps({k:v for k,v in out.items() if k!='parts'},indent=2))
