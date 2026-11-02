"""Estatica del conjunto actual y predimensionado de traccion. Sin modificar CAD."""
from pathlib import Path
import importlib.util, json, math, collections, csv
import numpy as np

H=Path(__file__).resolve().parent
sp=importlib.util.spec_from_file_location('cad17',H.parent/'calcular_cad_v17.py')
c=importlib.util.module_from_spec(sp);sp.loader.exec_module(c)
c.GEOMETRY_PATH17=H/'geometria_actual.json'
# El cambio de apariencia a blanco no debe cambiar la clasificacion fisica.
raw=json.loads(c.GEOMETRY_PATH17.read_text(encoding='utf-8'))
for b in raw['parts']:
    if b['appearance']=='AFR2_PLA_PrintALot_Blanco' and not b.get('material_v17'):
        b['material_v17']='PLA PrintALot'
classified=H/'geometria_clasificada.json'
classified.write_text(json.dumps(raw,indent=2),encoding='utf-8')
c.GEOMETRY_PATH17=classified
raw,parts,groups=c.load()
c.s.P['grip_angle']=(5-c.s.P['grip_min'])/(2*c.s.P['grip_r']) # cm: apertura50mm
floor=np.array([0.,-.032,-.07905134505])
half=np.array([.107,.062])
auto_mass=1.5
auto_cg=floor+np.array([0,0,.04]) # altura supuesta; sensibilidad separada

def pose(q, fraction=1., reserve=False):
    ff=c.frames(np.radians(q)); rows=[]
    for b in parts:
        mass=b['mass_g']/1000*(fraction if b['material']=='PLA PrintALot' else 1.)
        pos=(ff[b['root']]@np.r_[b['cg_local_m'],1])[:3]
        rows.append(dict(root=b['root'],component=b['component'],material=b['material'],mass_kg=mass,cg_m=pos))
    if reserve:
        rows.extend([dict(root='reserva_fija',mass_kg=.05,cg_m=np.array([0,-.014,.03])),dict(root='reserva_distal',mass_kg=.01,cg_m=(ff['90_Pieza_REFERENCIA_50mm_100g']@np.array([0,0,0,1]))[:3])])
    rows.append(dict(root='auto',mass_kg=auto_mass,cg_m=auto_cg))
    return rows,ff

def result(q, fraction=1., reserve=False):
    rows,ff=pose(q,fraction,reserve);m=sum(x['mass_kg'] for x in rows)
    cg=sum((x['mass_kg']*x['cg_m'] for x in rows),np.zeros(3))/m
    p=cg-floor;edge_m=half-np.abs(p[:2]);sf=[];mm=[]
    for axis in [0,1]:
        direction=1 if p[axis]>=0 else -1
        lever=[half[axis]-direction*(x['cg_m'][axis]-floor[axis]) for x in rows]
        stabil=sum(x['mass_kg']*c.s.G*max(l,0) for x,l in zip(rows,lever))
        overturn=sum(x['mass_kg']*c.s.G*max(-l,0) for x,l in zip(rows,lever))
        sf.append(stabil/overturn if overturn>1e-12 else None)
        mm.append(stabil-overturn)
    return dict(q_deg=q,mass_kg=m,cg_from_chassis_center_ground_mm=(p*1000).tolist(),edge_clearance_mm=(edge_m*1000).tolist(),net_restoring_moment_Nm=mm,sf_moments=sf,critical_acceleration_toward_edge_m_s2=(c.s.G*edge_m/p[2]).tolist(),pair_min_reaction_N=(m*c.s.G/2*(1-np.abs(p[:2])/half)).tolist())

cardinal=[result([yaw,90,0,0,0,0]) for yaw in [0,90,-90,180]]
# Se documenta180 como orientacion geometrica contrafactual si J1 no la permite.
limits=[]
for j in range(6):
    lim=next(v for k,v in raw['joint_limits'].items() if k.startswith('J'+str(j+1)+'_'))
    limits.append([math.degrees(lim['min_rad']),math.degrees(lim['max_rad'])])
worst=None;count=0
for q2 in sorted(set(list(np.arange(limits[1][0],limits[1][1]+.01,15))+[0.,90.,limits[1][1]])):
 for q3 in np.arange(max(-90,limits[2][0]),min(90,limits[2][1])+.01,15):
  for q5 in np.arange(max(-90,limits[4][0]),min(90,limits[4][1])+.01,30):
   # Rotar toda la distribucion movil sin repetir transformaciones de cada pieza.
   rows,ff=pose([0,float(q2),float(q3),0,float(q5),0]);pivot=ff['01_Giro_base'][:3,3]
   fixed=[x for x in rows if x['root'] in ['00_Base_control','auto']]
   moving=[x for x in rows if x['root'] not in ['00_Base_control','auto']]
   sm=sum(x['mass_kg'] for x in moving);fm=sum(x['mass_kg'] for x in fixed)
   mc=sum((x['mass_kg']*(x['cg_m']-pivot) for x in moving),np.zeros(3))
   fc=sum((x['mass_kg']*x['cg_m'] for x in fixed),np.zeros(3))
   for yaw in np.arange(max(-180,limits[0][0]),min(180,limits[0][1])+.01,5):
    angle=math.radians(yaw);co=math.cos(angle);si=math.sin(angle);R=np.array([[co,-si,0],[si,co,0],[0,0,1]])
    cg=(R@mc+sm*pivot+fc)/(sm+fm)-floor
    edge=half-np.abs(cg[:2]);clear=float(min(edge));count+=1
    if worst is None or clear<worst['clearance_m']:
        worst=dict(clearance_m=clear,q_deg=[float(yaw),float(q2),float(q3),0,float(q5),0])
general_worst=result(worst['q_deg'])
# Incluir explicitamente la horizontal y giros de muneca: la malla15grados
# puede omitir90 si arranca desde un limite distinto de un multiplo15.
extended=[]
for yaw in [-90,0,90]:
 for q4 in [-90,0,90]:
  for q6 in [-90,0,90]:
   extended.append(result([yaw,90,0,q4,0,q6]))
worst=min(extended+[general_worst],key=lambda x:min(x['edge_clearance_mm']))

q=np.radians([0,90,0,0,0,0]);static,sections=c.state(groups,q,np.zeros(6),np.zeros(6))
selection=json.loads((H.parent/'preseleccion_actuadores_integrados.json').read_text())
act=[]
for i,a in enumerate(selection['axes']):
    load=abs(float(static[i]));act.append(dict(joint=a['joint'],motor=a['motor'],joint_static_Nm=load,ratio=a['ratio'],efficiency=a['assumed_efficiency'],servo_required_Nm=load/(a['ratio']*a['assumed_efficiency']),nominal_capacity_joint_Nm=a['capacity_Nm'],static_margin=a['capacity_Nm']/load if load>1e-9 else None))

cats=collections.defaultdict(float);roots=collections.defaultdict(float)
for b in parts:cats[b['material']]+=b['mass_g'];roots[b['root']]+=b['mass_g']
m=cardinal[0]['mass_kg'];G=c.s.G;r=.04;torque=.2941995
drive=[]
for cr,a,slope in [(.03,.3,0),(.06,.3,0),(.10,.3,0),(.10,.5,0),(.06,.3,5),(.10,.3,10)]:
    ang=math.radians(slope);F=m*(a+G*math.sin(ang)+cr*G*math.cos(ang))
    drive.append(dict(rolling_coefficient_assumed=cr,acceleration_m_s2=a,slope_deg=slope,force_required_N=F,torque_per_motor_Nm=F*r/4,torque_per_motor_with_50pct_usable_traction_Nm=F*r/(4*.5),margin_50pct=torque/(F*r/(4*.5))))
out=dict(document=raw['document'],source_geometry_sha256=__import__('hashlib').sha256((H/'geometria_actual.json').read_bytes()).hexdigest(),parts=len(parts),mass_categories_g=dict(cats),mass_roots_g=dict(roots),arm_without_payload_g=sum(b['mass_g'] for b in parts)-200,car_mass_kg=auto_mass,payload_kg=.2,contact_model=dict(half_width_mm=107,half_length_mm=62,center_y_mm=-32,floor_z_mm=floor[2]*1000,car_cg_height_assumed_mm=40),cardinal_extended=cardinal,extended_wrist_samples=extended,sampled_static_worst=worst,static_samples=count+len(extended),joint_limits_deg=limits,actuator_static=act,static_sections=sections,drive=drive,drive_available_ideal_N=4*torque/r,drive_available_50pct_N=4*torque/r*.5,sensitivity=[dict(fraction=f,reserve=reserve,result=result(worst['q_deg'],f,reserve)) for f,reserve in [(1,True),(.7,False),(.5,False)]])
out['stowed']=result([0,0,0,0,0,0])
# Solicitacion inercial del chasis con brazo inmovil relativo al auto.
base=cardinal[1];height=base['cg_from_chassis_center_ground_mm'][2]/1000
offset=base['cg_from_chassis_center_ground_mm'][1]/1000
out['chassis_acceleration']=[dict(acceleration_toward_front_edge_m_s2=a,zmp_y_mm=(offset+a*height/G)*1000,edge_margin_mm=(.062-offset-a*height/G)*1000,net_restoring_moment_Nm=m*(G*(.062-offset)-a*height)) for a in [.05,.1,.3,.5]]
rows,_=pose([90,90,0,0,0,0]);lever=[.062-(x['cg_m'][1]-floor[1]) for x in rows]
stabil=sum(x['mass_kg']*G*max(l,0) for x,l in zip(rows,lever));overturn=sum(x['mass_kg']*G*max(-l,0) for x,l in zip(rows,lever))
out['front_moments']=dict(stabilizing_Nm=stabil,overturning_Nm=overturn,car_mass_for_sf1_2_kg=1.5+(1.2*overturn-stabil)/(G*.062),car_mass_for_neutral_balance_kg=1.5+(overturn-stabil)/(G*.062))
out['front_cg_height_sensitivity']=[dict(car_cg_height_mm=h,assembly_cg_height_mm=height*1000+1.5/m*(h-40),critical_acceleration_m_s2=G*(.062-offset)/(height+1.5/m*(h-40)/1000)) for h in [30,40,60]]
# Casos dinamicos ilustrativos del brazo, NO envolvente ni frenado instantaneo.
brakes=[]
for tb in [.05,.3,.5]:
 for j,sign in [(1,1),(1,-1),(2,1),(2,-1)]:
  for u in [0,.25,.5,.75,1]:
   end=np.radians([90,90,0,0,0,0]);speed=np.zeros(6);speed[j]=math.radians(30)*sign
   qs=end-speed*tb/2+speed*tb*(u-u**3+.5*u**4);dq=speed*(1-3*u*u+2*u**3);ddq=speed*(-6*u+6*u*u)/tb
   F=np.array([0.,0.,auto_mass*G]);M=np.zeros(3)
   f=c.frames(qs);fp=c.frames(qs+dq*1e-4+.5*ddq*1e-8);fm=c.frames(qs-dq*1e-4+.5*ddq*1e-8)
   for g in groups:
    root=g['root'];A=f[root];R=A[:3,:3];p=np.r_[g['cg'],1];x=(A@p)[:3]
    acc=((fp[root]@p)[:3]-2*x+(fm[root]@p)[:3])/1e-8
    Rd=(fp[root][:3,:3]-fm[root][:3,:3])/.0002;Rdd=(fp[root][:3,:3]-2*R+fm[root][:3,:3])/1e-8
    w=c.axial(Rd@R.T);alpha=c.axial(Rdd@R.T+Rd@Rd.T);I=R@g['I']@R.T
    force=g['mass_g']/1000*(acc+np.array([0,0,G]));F+=force
    M+=np.cross(x-floor,force)+I@alpha+np.cross(w,I@w)
   cop=np.array([-M[1],M[0]])/F[2]
   brakes.append(dict(brake_s=tb,joint=j+1,initial_speed_deg_s=30*sign,u=u,zmp_mm=(cop*1000).tolist(),front_margin_mm=(.062-cop[1])*1000,vertical_force_N=float(F[2])))
out['arm_braking_examples_worst_by_time']=[min([b for b in brakes if b['brake_s']==tb],key=lambda x:x['front_margin_mm']) for tb in [.05,.3,.5]]
def encode(o):
    if isinstance(o,np.ndarray):return o.tolist()
    if isinstance(o,np.generic):return o.item()
    raise TypeError(type(o))
(H/'resultados.json').write_text(json.dumps(out,indent=2,default=encode),encoding='utf-8')
(H/'masas_actuales.json').write_text(json.dumps(parts,indent=2),encoding='utf-8')
print(json.dumps({k:out[k] for k in ['parts','mass_categories_g','arm_without_payload_g','cardinal_extended','sampled_static_worst','static_samples','actuator_static','drive']},default=encode))
