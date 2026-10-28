"""Propagar las cargas comparadas a las secciones J1 actuales, sin nuevo CAD.

Se conservan las hipotesis v09 de barras, apoyos y placa rigida. La tension
normal usa la caja envolvente de cada contorno: cota superior nominal de
la distribucion lineal, no FEA ni cota de concentraciones/contactos reales.
"""
from pathlib import Path
import os,json,math,importlib.util
os.environ['OPENBLAS_NUM_THREADS']='1'
import numpy as np
H=Path(__file__).resolve().parent;V=H.parent/'revision_v09'
sp=importlib.util.spec_from_file_location('j1_old',H.parent/'analisis_J1_uniones/calcular_J1.py')
old=importlib.util.module_from_spec(sp);sp.loader.exec_module(old)
data=np.load(H.parents[2]/'exports'/'analisis-actuadores-v10'/'cargas_comparadas.npz');results=json.loads((H/'resultados.json').read_text(encoding='utf-8'))
raw=json.loads((V/'secciones_CAD_v09.json').read_text(encoding='utf-8'))
v09=json.loads((V/'resistencia_v09.json').read_text(encoding='utf-8'))
static=json.loads((V/'estatica_v09.json').read_text(encoding='utf-8'))
p={k:v['value_internal']*10 for k,v in raw['parameters'].items()}
shaft=[old.section(z) for z in raw['sections'] if z['component']=='V03_E00_Husillo_base_completo']
housing=[old.section(z,[-24.3,-35,43.3,7]) for z in raw['sections'] if z['component']=='V09_Cartucho6000_desmontable_PETG']
for sec in shaft+housing:
    xy=sec['_points']-[sec['cx_mm'],sec['cy_mm']]
    sec['_rx'],sec['_ry']=np.abs(xy).max(axis=0)

def normal(sec,F,M):
    inv=np.linalg.inv([[sec['Iy_mm4'],sec['Ixy_mm4']],[sec['Ixy_mm4'],sec['Ix_mm4']]])
    co=np.c_[-M[:,1],M[:,0]]@inv.T
    return np.abs(F[:,2])/sec['A_mm2']+np.abs(co[:,0])*sec['_rx']+np.abs(co[:,1])*sec['_ry']

E=1500.;G=E/2.76;beams=[];top=60.;base=12.1
for name,roi in [('izquierda',[-24.21,-28.51,-11.09,.51]),('derecha',[30.59,-28.51,43.21,.51])]:
    ss=[old.section(z,roi) for z in raw['sections'] if z['component']=='V05_Montura_baja_PETG' and 12<z['z_mm']<60]
    for sec in ss:
        xy=sec['_points']-[sec['cx_mm'],sec['cy_mm']];sec['_rx'],sec['_ry']=np.abs(xy).max(axis=0)
    tip=ss[-1];zz=np.array([x['z_mm'] for x in ss]);C=np.zeros((6,6))
    for z in np.linspace(base,top,161)[:-1]+(top-base)/320:
        get=lambda k:float(np.interp(z,zz,[x[k] for x in ss]))
        A,Ix,Iy,Ixy=[get(k) for k in ['A_mm2','Ix_mm4','Iy_mm4','Ixy_mm4']]
        L=top-z;dx=tip['cx_mm']-get('cx_mm');dy=tip['cy_mm']-get('cy_mm');dz=(top-base)/160
        B=np.array([[-L,0,dx,0,-1,0],[0,-L,dy,1,0,0]])
        C+=B.T@np.linalg.inv([[Iy,Ixy],[Ixy,Ix]])@B*dz/E;C[2,2]+=dz/(E*A)
        v=np.array([-dy,dx,0,0,0,1.]);C+=np.outer(v,v)*dz/(G*.5*(Ix+Iy))
    r=np.array([tip['cx_mm'],tip['cy_mm']-p['v05_axis_y'],0.])
    B=np.block([[np.eye(3),-old.skew(r)],[np.zeros((3,3)),np.eye(3)]])
    beams.append(dict(name=name,ss=ss,tip=tip,B=B,K=np.linalg.inv(C)))
K=sum((b['B'].T@b['K']@b['B'] for b in beams),np.zeros((6,6)))
deadF=np.array(v09['fixed_J1_dead_load_frame']['force_N']);deadM=np.array(v09['fixed_J1_dead_load_frame']['moment_at_cap_Nmm'])
fixed=next(x for x in static['groups'] if x['name']=='00_Base_control')
fixedF=np.array([0,0,-fixed['mass_g']*9.80665/1000]);fixedM=np.cross(fixed['com_mm'],fixedF)

def anchor_map(xy,w):
    return np.array([old.anchor_loads(row[:3],row[3:],xy,w)[0] for row in np.eye(6)]).T

anchor_xy=np.array([[p['agv_'+n+'_x'],p['agv_'+n+'_y']] for n in 'ABCDEF']+[[p['agv_center_x'],p['agv_center_y']]])
maps=[anchor_map(anchor_xy,np.ones(7)),anchor_map(anchor_xy,np.array([5.03/19]*6+[58/14]))]
cart_map=anchor_map(np.array([[-21.5,-23],[-21.5,-5],[40.5,-23],[40.5,-5]]),np.ones(4))
ang=np.radians([45,135,225,315]);flange_map=anchor_map(np.c_[25*np.cos(ang),25*np.sin(ang)],np.ones(4))
ii=data['expanded_to_source'];yaw=np.radians(data['expanded_yaw_end_deg']);q1=np.radians(data['expanded_q_deg'][:,0])
def rotate(v,a):
    cc,ss=np.cos(a),np.sin(a)
    return np.c_[cc*v[:,0]-ss*v[:,1],ss*v[:,0]+cc*v[:,1],v[:,2]]

rows=[]
for ci,name in enumerate(data['case_names']):
    wr=data['F_M_world'][ii,0,ci]
    F=rotate(-wr[:,0],yaw);M=rotate(-wr[:,1]*1000,yaw)
    za=p['v09_lower']-p['base_top'];zb=p['v09_upper']-p['base_top']
    rx0=(-M[:,1]+zb*F[:,0])/(za-zb);ry0=(M[:,0]+zb*F[:,1])/(za-zb)
    lo=np.c_[rx0,ry0,np.zeros(len(F))];up=np.c_[-F[:,0]-rx0,-F[:,1]-ry0,-F[:,2]]
    Fr=np.maximum(np.linalg.norm(lo[:,:2],axis=1),np.linalg.norm(up[:,:2],axis=1))
    P0=np.maximum(Fr,.6*Fr+.5*np.abs(F[:,2]))
    peak_vm=0.;peak_cart=0.
    for sec in shaft:
        z=sec['z_mm'];fc=F.copy();mc=M+np.cross([0,0,-z],F)
        for zz,reac in [(za,lo),(zb,up)]:
            if zz>z:fc+=reac;mc+=np.cross([0,0,zz-z],reac)
        sigma=normal(sec,rotate(fc,-q1),rotate(mc,-q1))
        od,idd=(8.,5.8) if z<66.8-p['base_top'] else (9.6,3.4)
        tau=np.abs(M[:,2])*16/(math.pi*od**3*(1-(idd/od)**4))
        peak_vm=max(peak_vm,float(np.sqrt(sigma**2+3*tau**2).max()))
    for sec in housing:
        z=sec['z_mm'];sf=np.zeros_like(F);mc=np.zeros_like(M)
        for zz,reac in [(p['v09_lower'],lo),(p['v09_upper'],up)]:
            if zz>z:
                sf-=reac;mc+=np.cross(np.array([0,p['v05_axis_y'],zz])-[sec['cx_mm'],sec['cy_mm'],z],-reac)
        mc[:,2]=0;peak_cart=max(peak_cart,float(normal(sec,sf,mc).max()))
    capM=M+np.cross([0,0,p['base_top']-60],F)+deadM
    u=np.c_[F+deadF,capM]@np.linalg.inv(K).T
    peak_frame=0.
    for b in beams:
        load=u@(b['K']@b['B']).T
        for sec in b['ss']:
            off=np.array([b['tip']['cx_mm']-sec['cx_mm'],b['tip']['cy_mm']-sec['cy_mm'],60-sec['z_mm']])
            moment=load[:,3:]+np.cross(off,load[:,:3])
            peak_frame=max(peak_frame,float(normal(sec,load[:,:3],moment).max()))
    FM=np.c_[F+fixedF,M+np.cross([0,p['v05_axis_y'],p['base_top']],F)+fixedM]
    anchor=max(max(0,float((FM@am.T).max())) for am in maps)
    cartM=M+np.cross([0,p['v05_axis_y'],p['base_top']-60],F)
    cartN=max(0,float((np.c_[F,cartM]@cart_map.T).max()))
    flangeN=max(0,float((np.c_[rotate(F,-q1),rotate(M,-q1)]@flange_map.T).max()))
    row=dict(name=str(name),states=len(F),bearing_P0_N=float(P0.max()),bearing_C0_over_P0=1960/float(P0.max()),
      shaft_vm_nominal_bbox_bound_MPa=peak_vm,shaft_yield_requirement_n2_Kt2_MPa=4*peak_vm,
      cart_normal_bbox_bound_MPa=peak_cart,housing_frame_normal_bbox_bound_MPa=peak_frame,
      FDM_normal_requirement_n2_Kt2_MPa=4*peak_frame,cap_translation_mm=float(np.linalg.norm(u[:,:3],axis=1).max()),
      cap_rotation_deg=float(np.linalg.norm(np.degrees(u[:,3:]),axis=1).max()),
      anchor_tension_increment_N=anchor,cart_M3_tension_increment_N=cartN,
      cart_washer_pressure_MPa=cartN/(math.pi*(7**2-3.2**2)/4),flange_M4_tension_increment_N=flangeN)
    rows.append(row);print(str(name),'FDM_req',round(4*peak_frame,3),'bearing',round(row['bearing_P0_N'],2),flush=True)

# Static tipping sensitivity, no claim about the actual wheel support polygon.
for r in results['results']:
    m=r['mass_g']/1000;y=r['cg_mm'][1]
    r['static_tipping_sensitivity']=dict(assumption='Linea de vuelco hipotetica y=b; CG del auto sin brazo y=0. Sin factor ni dinamica. b no medido.',
       chassis_mass_min_g_for_halfspan={str(b):max(0,m*(y-b)/b)*1000 for b in [60,80,100]})
(H/'resultados.json').write_text(json.dumps(results,ensure_ascii=False,indent=2),encoding='utf-8')
(H/'uniones_resultados.json').write_text(json.dumps(dict(scope='Geometria v09 inalterada; cargas declaradas por alternativa. Cotas superiores NOMINALES por caja de seccion. Sin FEA, contacto, concentraciones reales ni resistencia FDM asignada.',rows=rows),ensure_ascii=False,indent=2),encoding='utf-8')
