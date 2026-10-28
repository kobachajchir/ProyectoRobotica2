"""Cribado previo al CAD: masas, tensores, pares, cortantes y flexion.

Reutiliza el inventario v09; ningun acceso a Fusion ni cambio de geometria.
La dinamica es lineal en masa y tensor: se evalua una base de cuerpos rigidos
y cada combinacion de motores se obtiene por suma ponderada de sus cargas.
"""
from pathlib import Path
import os,json,math,itertools,importlib.util,hashlib,csv
os.environ['OPENBLAS_NUM_THREADS']='1'
import numpy as np
H=Path(__file__).resolve().parent;V=H.parent/'revision_v09'
EXPORT=H.parents[2]/'exports'/'analisis-actuadores-v10';EXPORT.mkdir(parents=True,exist_ok=True)
spec=importlib.util.spec_from_file_location('dyn09',V/'calcular_v09.py')
d=importlib.util.module_from_spec(spec);spec.loader.exec_module(d)
cfg=json.loads((H/'candidatos.json').read_text(encoding='utf-8'))
parts,ff=d.load();data=np.load(V/'estados_J1_v09.npz')
sel=np.flatnonzero(data['yaw_end_deg']==0)
q=data['q_deg'][sel,:6];times=data['time_s'][sel]
labels=data['case_labels'];case_index=data['case_index'][sel]
names=[str(labels[i]) for i in case_index]
motor_ids={'J2':'V02_M02_GX3240_270','J3':'V07_M03_GX3270BLS_Codo','J4':'V02_M04_MG90S_J4','J5':'V02_M05_MG946_J5','J7':'V02_M07_MG90S_Pinza'}
features=[]

def feature(tag,root,mass,cg,tensor,kind='main'):
    features.append(dict(tag=tag,root=root,mass_kg=mass,cg=np.array(cg),I=np.array(tensor),kind=kind))
    return len(features)-1

def aggregate(tag,bb,kind='main'):
    m=sum(b['mass_g'] for b in bb)/1000
    cg=sum((b['mass_g']/1000*b['_local_g'] for b in bb),np.zeros(3))/m
    I=np.zeros((3,3))
    for b in bb:
        r=b['_local_g']-cg
        I+=b['_I_group']+b['mass_g']/1000*((r@r)*np.eye(3)-np.outer(r,r))
    return feature(tag,bb[0]['root'],m,cg,I/m,kind)

struct=[]
for root in dict.fromkeys(b['root'] for b in parts):
    bb=[b for b in parts if b['root']==root and b['component'] not in motor_ids.values()]
    if bb:struct.append(aggregate('estructura:'+root,bb))
for root,tag in [('02_Brazo_PVC','PVC1'),('03_Antebrazo_PVC','PVC2')]:
    # First free section at25mm, where the PVC starts. Components are assigned
    # by serial attachment and local centroid, as in the previous cut model.
    bb=[b for b in parts if b['root']==root and b['component'] not in motor_ids.values() and b['_local_g'][2]>=.025-1e-9]
    aggregate('estructura:'+tag,bb,kind=tag)
old={j:aggregate('motor_actual:'+j,[b for b in parts if b['component']==cn]) for j,cn in motor_ids.items()}
new={}
raw=d.st.RAW

def synthetic(j,motor,shift=(0,0,0)):
    key=(j,motor,tuple(shift))
    if key in new:return new[key]
    cn=motor_ids[j];bb=[b for b in raw['parts'] if b['component']==cn]
    # Body front plane is kept, not asserted to match an unverified horn.
    body=next(b for b in bb if 'spline' not in b['body'].lower())
    bound=body['bbox_component_mm'];z=(bound[2]+bound[5])/2
    s=cfg['motors'][motor];L,D,W=s['box_xyz_mm']
    if j in ['J4','J7']:
        spline=next(b for b in bb if 'spline' in b['body'].lower())['centroid_component_mm']
        cg=np.array([spline[0]+L/2-s['offset_from_left_mm'],spline[1],bound[5]-D/2])+shift
    else:cg=np.array([L/2-s['offset_from_left_mm'],bound[1]+D/2,z])+shift
    roots={k:np.array(v).reshape(4,4) for k,v in raw['root_transforms_cm'].items()}
    T=np.linalg.inv(roots[body['root']])@np.array(body['component_to_root_cm']).reshape(4,4)
    T[:3,3]*=.01;cg=(T@np.r_[cg*.001,1])[:3]
    xyz=np.array([L,W,D] if j in ['J4','J7'] else [L,D,W])*.001
    I=np.diag([xyz[1]**2+xyz[2]**2,xyz[0]**2+xyz[2]**2,xyz[0]**2+xyz[1]**2])/12
    I=T[:3,:3]@I@T[:3,:3].T
    idx=feature('motor_nuevo:'+str(key),body['root'],s['mass_g']/1000,cg,I)
    new[key]=idx;return idx

reserves={j:feature('reserva_montaje:'+j,features[old[j]]['root'],.015,features[old[j]]['cg'],np.eye(3)*(0.020**2/6)) for j in old}
cable=feature('reserva_cable','06_Herramienta',.005,np.array([0,0,.030]),np.eye(3)*1e-6)
heavy=feature('sensibilidad_J3_200g',features[old['J3']]['root'],.200,features[old['J3']]['cg'],np.diag([.060**2+.030**2,.060**2+.030**2,.030**2+.030**2])/12)
cases=[]
default_caps=np.array([63,40,63,1.8,13,1.8],float)
default_speeds=np.array([600,np.nan,600,600,352.94117647,600])

def case(name,swaps=None,hv=False,mounts=False,shift=(0,0,0),note=''):
    swaps=swaps or {};motors=dict(old)
    caps=default_caps.copy();speeds=default_speeds.copy()
    if hv:caps[[0,2]]=70;speeds[[0,2]]=666.666666667
    for j,m in swaps.items():
        motors[j]=synthetic(j,m,shift if j=='J5' else (0,0,0))
        v='6.0' if m in ['DS9401','MG92B'] else ('6.8' if m=='DS3225' else ('8.4' if hv and m in ['GX70','DS8930'] else '7.4'))
        if j!='J7':
            caps[int(j[1])-1]=cfg['motors'][m]['torque_kgfcm'][v]
            speeds[int(j[1])-1]=cfg['motors'][m]['no_load_deg_s'][v]
    chosen=struct+[i for i,f in enumerate(features) if f['kind']!='main']+list(motors.values())
    if mounts:chosen+=[reserves[j] for j in reserves if j not in ['J4','J7'] or j in swaps]+[cable]
    cases.append(dict(name=name,chosen=chosen,swaps=swaps,hv=hv,mounts=mounts,shift=list(shift),caps=caps,speeds=speeds,note=note))

case('v09_actual')
case('solo_J5_GX55',{'J5':'GX55'})
case('GX70_J2_GX55_J5_7V4',{'J2':'GX70','J5':'GX55'})
case('GX70_J2_GX55_J5_8V4',{'J2':'GX70','J5':'GX55'},hv=True)
case('GX70_J2_DS8930_J5_7V4',{'J2':'GX70','J5':'DS8930'})
case('GX70_J2_DS8930_J5_8V4',{'J2':'GX70','J5':'DS8930'},hv=True)
case('GX70_J2_DS3225_J5_8V4',{'J2':'GX70','J5':'DS3225'},hv=True,note='Importacion, no stock nacional confirmado')
case('GX70_DS8930_montajes_8V4',{'J2':'GX70','J5':'DS8930'},hv=True,mounts=True)
case('GX70_DS3225_montajes_8V4',{'J2':'GX70','J5':'DS3225'},hv=True,mounts=True)
case('GX70_GX55_montajes_7V4',{'J2':'GX70','J5':'GX55'},mounts=True)
case('ligero4_7V4',{'J2':'GX70','J4':'DS9401','J5':'DS8930'})
case('ligero4_8V4',{'J2':'GX70','J4':'DS9401','J5':'DS8930'},hv=True)
case('ligero4_montajes_7V4',{'J2':'GX70','J4':'DS9401','J5':'DS8930'},mounts=True)
case('ligero4_montajes_8V4',{'J2':'GX70','J4':'DS9401','J5':'DS8930'},hv=True,mounts=True)
case('ligero5_montajes_7V4',{'J2':'GX70','J4':'DS9401','J5':'DS8930','J7':'MG92B'},mounts=True)
case('ligero5_montajes_8V4',{'J2':'GX70','J4':'DS9401','J5':'DS8930','J7':'MG92B'},hv=True,mounts=True)
for s in itertools.product([-5,5],repeat=3):
    case('DS8930_CG_'+str(s),{'J2':'GX70','J5':'DS8930'},hv=True,mounts=True,shift=s,note='Sensibilidad de CG nuevo J5; esquina de cubo+-5mm, no garantia de extremo continuo')
case('sensibilidad_J3_200g',{'J2':'GX70','J5':'DS8930'},hv=True,note='Masa200g hipotetica en J3, mismo torque63/70; no es una oferta de motor')
cases[-1]['chosen'].remove(old['J3']);cases[-1]['chosen'].append(heavy)

# Features may have been added after earlier cases. The weights matrix is built
# only once every candidate and its uncertainty cases have been defined.
weights=np.zeros((len(cases),len(features)))
for i,c in enumerate(cases):
    for j in c['chosen']:weights[i,j]=features[j]['mass_kg']
roots=[f['root'] for f in features];cg=np.array([f['cg'] for f in features]);I=np.array([f['I'] for f in features])
J2roots=['02_Brazo_PVC','03_Antebrazo_PVC','04_Giro_muneca','05_Inclinacion_muneca','06_Herramienta','07_Mandibula_izquierda','08_Mandibula_derecha','10_Pinion_pinza','11_Pinion_J4','90_Pieza_REFERENCIA_50mm_100g']
J3roots=[x for x in J2roots if x!='02_Brazo_PVC']
J4roots=[x for x in J3roots if x not in ['03_Antebrazo_PVC','11_Pinion_J4']]
J5roots=[x for x in J4roots if x!='04_Giro_muneca']
J6roots=[x for x in J5roots if x!='05_Inclinacion_muneca']
routes=[('J1','01_Giro_base',d.st.ROOTS[1:],'global',2,0),('J2','02_Brazo_PVC',J2roots,'01_Giro_base',1,0),
 ('J3','03_Antebrazo_PVC',J3roots,'02_Brazo_PVC',1,0),('J4','04_Giro_muneca',J4roots,'03_Antebrazo_PVC',2,0),
 ('J5','05_Inclinacion_muneca',J5roots,'04_Giro_muneca',1,0),('J6','06_Herramienta',J6roots,'05_Inclinacion_muneca',2,0),
 ('PVC1','02_Brazo_PVC',J2roots,'02_Brazo_PVC',2,.025),('PVC2','03_Antebrazo_PVC',J3roots,'03_Antebrazo_PVC',2,.025)]
mask=[]
for name,root,supported,axisroot,axis,z in routes:
    mask.append([f['root'] in supported and (f['kind']=='main' if name.startswith('J') or f['root']!=root else (f['kind']==name or f['tag'].startswith('motor_') or f['tag'].startswith('reserva_') or f['tag'].startswith('sensibilidad_'))) for f in features])
route_weights=np.array(mask)[:,None,:]*weights[None,:,:]
root_indexes={root:np.array([i for i,f in enumerate(features) if f['root']==root]) for root in set(roots)}

def evaluate(q,dq,ddq):
    global payload_force_tool,payload_couple_tool
    h=1e-4;frames=d.root_frames(q)
    fp=d.root_frames(q+dq*h+.5*ddq*h*h);fm=d.root_frames(q-dq*h+.5*ddq*h*h)
    F=np.zeros((len(features),3));O=np.zeros_like(F)
    for root,ii in root_indexes.items():
        A=frames[root];Ap=fp[root];Am=fm[root];R=A[:3,:3]
        Rd=(Ap[:3,:3]-Am[:3,:3])/(2*h);Rdd=(Ap[:3,:3]-2*R+Am[:3,:3])/(h*h)
        W=Rd@R.T;Al=Rdd@R.T+Rd@Rd.T
        w=np.array([W[2,1]-W[1,2],W[0,2]-W[2,0],W[1,0]-W[0,1]])*.5
        alpha=np.array([Al[2,1]-Al[1,2],Al[0,2]-Al[2,0],Al[1,0]-Al[0,1]])*.5
        x=cg[ii]@R.T+A[:3,3]
        acc=(cg[ii]@(Ap[:3,:3]-2*R+Am[:3,:3]).T+Ap[:3,3]-2*A[:3,3]+Am[:3,3])/(h*h)
        F[ii]=acc+np.array([0,0,d.st.G])
        tensors=R@I[ii]@R.T
        O[ii]=np.cross(x,F[ii])+tensors@alpha+np.cross(w,tensors@w)
        if root=='90_Pieza_REFERENCIA_50mm_100g':
            bi=next(i for i in ii if features[i]['kind']=='main')
            payload_force_tool=(features[bi]['mass_kg']*F[bi])@frames['06_Herramienta'][:3,:3]
            payload_couple_tool=(features[bi]['mass_kg']*(O[bi]-np.cross(x[list(ii).index(bi)],F[bi])))@frames['06_Herramienta'][:3,:3]
    forces=route_weights@F;moments=route_weights@O
    localF=[];localM=[];tau=[]
    for j,(name,root,supported,axisroot,axis,z) in enumerate(routes):
        A=frames[root];point=A[:3,3]+z*A[:3,2]
        moments[j]-=np.cross(point,forces[j]);ax=np.eye(3)[:,axis] if axisroot=='global' else frames[axisroot][:3,axis]
        tau.append(moments[j]@ax);localF.append(forces[j]@A[:3,:3]);localM.append(moments[j]@A[:3,:3])
    return np.stack((forces,moments),axis=-2),np.array(tau),np.stack((localF,localM),axis=-2),frames

traj=[]
for name,t,angles in zip(names,times,q):
    if name=='estatica':dq=np.zeros(6);ddq=dq.copy();tb=0
    else:
        bits=name.split('_');tb=float(bits[-1][:-2])/1000
        speed=np.zeros(6)
        if bits[0]=='S6':speed=np.array([int(x) for x in bits[1:7]])*math.radians(30)
        else:speed[int(bits[0][1:])-1]=int(bits[1])*math.radians(30)
        s=t/tb;dq=speed*(1-3*s*s+2*s**3);ddq=speed*(-6*s+6*s*s)/tb
    traj.append((np.radians(angles),dq,ddq,tb))
original_count=len(traj)
all_labels=list(map(str,labels));extra_groups=[]
q=list(q);times=list(times);case_index=list(case_index)
# Preserve full extension while changing wrist/tool roll. This is necessary
# for gripping loads: the original roll can put gravity along the jaw normal.
for r4,r6 in itertools.product([-90,0,90],repeat=2):
    if r4==r6==0:continue
    for signs in itertools.product([-1,1],repeat=6):
        speed=np.radians(signs)*30
        for tb in [.2,.1,.05]:
            end=np.radians([0,90,0,r4,0,r6]);start=end-speed*tb/2
            if any(min(start[j],end[j])<lo-1e-8 or max(start[j],end[j])>hi+1e-8 for j,(lo,hi) in enumerate(np.radians([[-90,90],[-10,125],[-90,90],[-90,90],[-90,90],[-90,90]]))):continue
            name=f'roll4{r4:+d}_roll6{r6:+d}_'+ '_'.join(map(str,signs))+f'_{tb*1000:.0f}ms'
            idx=len(all_labels);all_labels.append(name);group=[]
            for t in np.linspace(0,tb,21):
                s=t/tb;angles=end-speed*tb/2+speed*tb*(s-s**3+.5*s**4)
                dq=speed*(1-3*s*s+2*s**3);ddq=speed*(-6*s+6*s*s)/tb
                group.append(len(traj));traj.append((angles,dq,ddq,tb));q.append(np.degrees(angles));times.append(float(t));names.append(name);case_index.append(idx)
            extra_groups.append(group)
q=np.array(q);times=np.array(times);case_index=np.array(case_index)
print('Calculo',len(traj),'estados fuente;',len(cases),'combinaciones de masa',flush=True)
wrenches=[];local=[];torques=[];payload_forces=[];payload_couples=[]
for i,(angles,dq,ddq,tb) in enumerate(traj):
    wr,tau,loc,_=evaluate(angles,dq,ddq);wrenches.append(wr);local.append(loc);torques.append(tau)
    payload_forces.append(payload_force_tool.copy());payload_couples.append(payload_couple_tool.copy())
    if (i+1)%1000==0:print('Fuentes',i+1,flush=True)
wrenches=np.array(wrenches);local=np.array(local);torques=np.array(torques)
denom=np.array([1,1.7,1,.9,1,1])*.0980665
demand=np.abs(torques[:,:6,:])/denom[None,:,None]

# Check the linear rigid-body summation against the already executed v09
# calculation, and also compare an independent direct sum at two states.
parity=[]
for i in [0,len(traj)//2]:
    a,b,c,t=traj[i];direct=d.body_state_at(parts,a,b,c)
    parity.append(dict(source_index=i,max_force_error_N=max(np.linalg.norm(wrenches[i,j,0,0]-r['force_N']) for j,r in enumerate(direct)),
                       max_moment_error_Nm=max(np.linalg.norm(wrenches[i,j,0,1]-r['moment_Nm']) for j,r in enumerate(direct))))
oldW=np.stack((data['F_N'][sel],data['M_Nmm'][sel]/1000),axis=1)
parity.append(dict(all_4411_v09_J1_force_error_N=float(np.max(np.abs(wrenches[:original_count,0,0,0]+oldW[:,0]))),
                   all_4411_v09_J1_moment_error_Nm=float(np.max(np.abs(wrenches[:original_count,0,0,1]+oldW[:,1])))))

# Mass and CG at the same horizontal posture as the existing inventory.
_,_,_,frames=evaluate(np.radians([90,90,0,0,0,0]),np.zeros(6),np.zeros(6))
worldcg=np.array([frames[f['root']][:3,:3]@f['cg']+frames[f['root']][:3,3] for f in features])
mass_weights=weights*np.array([f['kind']=='main' for f in features])
masses=mass_weights.sum(axis=1);coms=(mass_weights@worldcg)/masses[:,None]
results=[]
od=32.;idd=28.3;A=math.pi*(od**2-idd**2)/4;Iz=math.pi*(od**4-idd**4)/64;J=2*Iz
for ci,c in enumerate(cases):
    row={k:c[k] for k in ['name','swaps','hv','mounts','shift','note']}
    row.update(mass_g=float(masses[ci]*1000),delta_mass_g=float((masses[ci]-masses[0])*1000),cg_mm=(coms[ci]*1000).tolist(),scenarios={})
    for stop in [.2,.1,.05]:
        ids=np.array([i for i,v in enumerate(traj) if v[3]==stop])
        jj=[]
        for j in range(6):
            di=demand[ids,j,ci];idx=ids[int(np.argmax(di))];tau=float(demand[idx,j,ci]);cap=float(c['caps'][j])
            ds=np.array([abs(traj[k][1][j])*180/math.pi*(2 if j==1 else 1) for k in ids]);speed=c['speeds'][j]
            # Illustrative linear torque-speed sensitivity only. This is not
            # a measured regenerative/braking curve for these servos.
            lin=cap*np.maximum(0,1-ds/speed) if np.isfinite(speed) else np.full(len(ids),np.nan)
            valid=lin>1e-9
            quot=np.divide(lin,di,out=np.full_like(lin,np.inf),where=di>1e-10)
            linmargin=float(np.min(quot)) if np.all(valid) and np.isfinite(quot.min()) else None
            jj.append(dict(joint='J'+str(j+1),peak_servo_kgfcm=tau,advertised_peak_kgfcm=cap,stall_peak_ratio=cap/tau,
                passes_preliminary_2x=cap>=2*tau,linear_speed_margin_sensitivity=linmargin,
                case=names[idx],time_s=float(times[idx]),q_deg=q[idx].tolist(),
                max_force_N=float(np.max(np.linalg.norm(wrenches[ids,j,ci,0],axis=1))),
                max_moment_Nm=float(np.max(np.linalg.norm(wrenches[ids,j,ci,1],axis=1))),
                max_local_N_N=float(np.max(np.abs(local[ids,j,ci,0,2]))),
                max_local_V_N=float(np.max(np.linalg.norm(local[ids,j,ci,0,:2],axis=1))),
                max_local_T_Nm=float(np.max(np.abs(local[ids,j,ci,1,2]))),
                max_local_bending_Nm=float(np.max(np.linalg.norm(local[ids,j,ci,1,:2],axis=1)))))
        row['scenarios'][str(round(stop*1000))]=jj
    row['pipes']=[]
    for j in [6,7]:
        F=local[:,j,ci,0];M=local[:,j,ci,1]*1000
        sigma=np.abs(F[:,2])/A+np.linalg.norm(M[:,:2],axis=1)*16/Iz
        shear=np.abs(M[:,2])*16/J+2*np.linalg.norm(F[:,:2],axis=1)/A
        ii=int(np.argmax(sigma))
        row['pipes'].append(dict(section=routes[j][0],normal_nominal_MPa=float(sigma[ii]),shear_envelope_MPa=float(shear.max()),case=names[ii],time_s=float(times[ii]),note='Seccion libre25mm: corte por grafo/centroides; flexion+axial, torsion+cortante2V/A. Sin contacto o admisible PVC asignado.'))
    results.append(row)

lookup={(int(c),float(t)):i for i,(c,t) in enumerate(zip(case_index,times))}
all_to_source=np.array([lookup[(int(c),float(t))] for c,t in zip(data['case_index'],data['time_s'])])
expanded=list(all_to_source);expanded_yaw=list(data['yaw_end_deg']);expanded_q=list(data['q_deg'][:,:6])
for group in extra_groups:
    for yaw in range(-90,91,10):
        if min(q[j,0]+yaw for j in group)<-90-1e-8 or max(q[j,0]+yaw for j in group)>90+1e-8:continue
        for j in group:
            angles=q[j].copy();angles[0]+=yaw;expanded.append(j);expanded_yaw.append(yaw);expanded_q.append(angles)
all_to_source=np.array(expanded)
np.savez_compressed(EXPORT/'cargas_comparadas.npz',case_names=np.array([c['name'] for c in cases]),
    case_index=case_index,source_trajectory_labels=np.array(all_labels),time_s=times,q_deg=q,F_M_world=wrenches,F_M_local=local,
    motor_torque_kgfcm=demand,root_route_names=np.array([r[0] for r in routes]),
    payload_force_tool_N=np.array(payload_forces),payload_couple_tool_Nm=np.array(payload_couples),
    expanded_to_source=all_to_source,expanded_yaw_end_deg=np.array(expanded_yaw),expanded_q_deg=np.array(expanded_q))
baseline={'inventory':'../revision_v09/geometria_v09.json','sha256':hashlib.sha256((V/'geometria_v09.json').read_bytes()).hexdigest(),
          'source_states':len(traj),'trajectory_cases':len(all_labels),'original_v09_states':original_count,'expanded_J1_states':len(all_to_source),'cases':len(cases),'parity':parity}
(H/'resultados.json').write_text(json.dumps(dict(baseline=baseline,results=results),ensure_ascii=False,indent=2),encoding='utf-8')
(H/'base_influencias.json').write_text(json.dumps(dict(features=[{k:(v.tolist() if isinstance(v,np.ndarray) else v) for k,v in f.items()} for f in features],weights_kg=weights.tolist(),note='Duplicados kindPVC se usan solo para cortes y se excluyen de masa total. CG nuevos supuestos; no geometria de montaje.'),ensure_ascii=False,indent=2),encoding='utf-8')
grip=[]
for stop in [.2,.1,.05]:
    idx=np.array([i for i,v in enumerate(traj) if v[3]==stop]);F=np.array(payload_forces)[idx];M=np.array(payload_couples)[idx]
    tangential=np.linalg.norm(F[:,1:],axis=1);ft=float(tangential.max());fn=float(np.abs(F[:,0]).max())
    # Necessary translational-friction condition only; finite-pad wrench and
    # contact pressure are not solved by this test.
    grip.append(dict(stop_ms=round(stop*1000),max_tangential_force_N=ft,max_normal_force_N=fn,
       max_couple_Nm=np.max(np.abs(M),axis=0).tolist(),
       required_servo_kgfcm_by_mu={str(mu):ft*.012/(mu*.8*.0980665) for mu in [.2,.3,.5,.8]},
       minimum_mu_translation_only={str(t):ft*.012/(t*.0980665*.8) for t in [1.8,2.2,3.5]}))
(H/'pinza_carga.json').write_text(json.dumps(dict(scope='Carga200g: condicion necesaria de friccion tangencial 2muN>=Ft, pinion12mm, eficiencia0.8 supuesta. Sin factor de seguridad ni momentos/contacto de almohadillas resueltos.',rows=grip),ensure_ascii=False,indent=2),encoding='utf-8')
for r in results[:10]:print(r['name'],round(r['mass_g'],2),[round(x['peak_servo_kgfcm'],3) for x in r['scenarios']['50']],flush=True)
