"""Calculo de ingenieria v08. Reutiliza ecuaciones de v06 sin ejecutar sus comprobaciones.

No es FEA ni certificado de frenada. Densidades y masas comerciales explicitas.
"""
from pathlib import Path
import importlib.util,json,csv,math
import numpy as np

HERE=Path(__file__).resolve().parent
spec=importlib.util.spec_from_file_location('statics_v06',HERE.parent/'analisis_cargas_200g'/'calcular_estatica.py')
st=importlib.util.module_from_spec(spec);spec.loader.exec_module(st)

def load():
    st.RAW=json.loads((HERE/'geometria_v08.json').read_text(encoding='utf-8'))
    st.P={k:v['value_internal'] for k,v in st.RAW['parameters'].items()}
    st.MOTOR_MASS.pop('V02_M03_DS3218_Codo',None)
    st.MOTOR_MASS['V07_M03_GX3270BLS_Codo']=90
    parts,ff=st.physical_parts([90,90,0,0,0,0,109.8],1.,200,0,False)
    for b in parts:
        if b['component']=='V03_Horn_movil_V02_M03_DS3218_Codo':
            b['mass_g']=b['volume_cm3']*2.70
            b['category']='Horn aluminio';b['basis']='Horn25T metalico, geometria adaptador de referencia, rho aluminio2.70'
        if b['component']=='V07_M03_GX3270BLS_Codo':
            b['basis']='Starware SKU2982: masa anunciada90g sin accesorios; distribucion uniforme de envolvente'
    # Component orientation relative to its root group, in SI units.
    roots={k:np.array(v).reshape(4,4) for k,v in st.RAW['root_transforms_cm'].items()}
    for b in parts:
        rel=np.linalg.inv(roots[b['root']])@np.array(b['component_to_root_cm']).reshape(4,4)
        rel[:3,3]*=.01
        local=np.array(b['centroid_component_mm'])*.001
        b['_local_g']=(rel@np.r_[local,1])[:3]
        native_tensor=np.array(b['native_centroidal_tensor_kg_cm2'])*1e-4
        scaled=native_tensor*((b['mass_g']/1000)/b['native_mass_kg'])
        b['_I_group']=rel[:3,:3]@scaled@rel[:3,:3].T
    return parts,ff

def root_frames(q):
    qq=list(np.degrees(q))+[109.8]
    ff=st.fk(qq)
    for x in ff.values():x[:3,3]*=.001
    return ff

def state_at(parts,q,dq,ddq):
    # Time differentiation of kinematics around one declared state.
    h=1e-4
    f0=root_frames(q)
    fp=root_frames(q+dq*h+.5*ddq*h*h)
    fm=root_frames(q-dq*h+.5*ddq*h*h)
    group_data={}
    for g,A in f0.items():
        Ap,Am=fp[g],fm[g];R=A[:3,:3]
        Rd=(Ap[:3,:3]-Am[:3,:3])/(2*h)
        Rdd=(Ap[:3,:3]-2*R+Am[:3,:3])/(h*h)
        W=Rd@R.T;Al=Rdd@R.T+Rd@Rd.T
        w=np.array([W[2,1]-W[1,2],W[0,2]-W[2,0],W[1,0]-W[0,1]])*.5
        alpha=np.array([Al[2,1]-Al[1,2],Al[0,2]-Al[2,0],Al[1,0]-Al[0,1]])*.5
        group_data[g]=(A,Ap,Am,R,w,alpha)
    values=[]
    for b in parts:
        A,Ap,Am,R,w,alpha=group_data[b['root']]
        p=np.r_[b['_local_g'],1]
        x=(A@p)[:3];acc=((Ap@p)[:3]-2*x+(Am@p)[:3])/(h*h)
        F=(b['mass_g']/1000)*(acc-np.array([0,0,-st.G]))
        I=R@b['_I_group']@R.T
        values.append((b['root'],x,F,I@alpha+np.cross(w,I@w)))
    # Joint locations/axes are derived directly from the same frame chain.
    defs=[
        ('J1','01_Giro_base',np.array([0.,0.,1.]),st.ROOTS[1:]),
        ('J2','02_Brazo_PVC',f0['01_Giro_base'][:3,1],['02_Brazo_PVC','03_Antebrazo_PVC','04_Giro_muneca','05_Inclinacion_muneca','06_Herramienta','07_Mandibula_izquierda','08_Mandibula_derecha','10_Pinion_pinza','11_Pinion_J4','90_Pieza_REFERENCIA_50mm_100g']),
        ('J3','03_Antebrazo_PVC',f0['02_Brazo_PVC'][:3,1],['03_Antebrazo_PVC','04_Giro_muneca','05_Inclinacion_muneca','06_Herramienta','07_Mandibula_izquierda','08_Mandibula_derecha','10_Pinion_pinza','11_Pinion_J4','90_Pieza_REFERENCIA_50mm_100g']),
    ]
    out=[]
    for name,root,axis,supported in defs:
        point=f0[root][:3,3]
        F=sum((v[2] for v in values if v[0] in supported),np.zeros(3))
        M=sum((np.cross(v[1]-point,v[2])+v[3] for v in values if v[0] in supported),np.zeros(3))
        tau=float(M@axis);bend=float(np.linalg.norm(M-tau*axis))
        ratio=2 if name=='J2' else 1;eta=.85 if name=='J2' else 1
        localF=f0[root][:3,:3].T@F;localM=f0[root][:3,:3].T@M
        out.append({'joint':name,'point_m':point.tolist(),'force_N':F.tolist(),'moment_Nm':M.tolist(),
                    'section_local_z_longitudinal':{'N_N':float(localF[2]),'Vx_N':float(localF[0]),
                     'Vy_N':float(localF[1]),'T_Nm':float(localM[2]),'Mx_Nm':float(localM[0]),'My_Nm':float(localM[1])},
                    'axis_torque_Nm':tau,'bending_Nm':bend,'servo_torque_kgfcm':abs(tau)/(ratio*eta*.0980665)})
    return out

def brake_scenarios(parts):
    q_end=np.radians([90,90,0,0,0,0])
    cases=[]
    # Operating speed is a design hypothesis, not the catalogue no-load speed.
    for tb in [.2,.1,.05]:
        for axis,deg_s in [(0,30),(1,30),(2,30),(2,600)]:
            for direction in [-1,1]:
                speed=np.zeros(6);speed[axis]=direction*math.radians(deg_s)
                start=q_end-speed*tb/2
                lim=next((v for k,v in st.RAW['joint_limits'].items() if k.startswith(f'J{axis+1}_')),None)
                if lim and ((lim['min_enabled'] and min(start[axis],q_end[axis])<lim['min_rad']-1e-8)
                            or (lim['max_enabled'] and max(start[axis],q_end[axis])>lim['max_rad']+1e-8)):
                    continue
                snapshots=[]
                for t in np.linspace(0,tb,21):
                    s=t/tb
                    # Integral of smoothstep deceleration speed, endpoint q_end.
                    q=q_end-speed*tb/2+speed*tb*(s-s**3+.5*s**4)
                    dq=speed*(1-3*s*s+2*s**3)
                    ddq=speed*(-6*s+6*s*s)/tb
                    snapshots.append({'time_s':float(t),'q_deg':np.degrees(q).tolist(),'dq_deg_s':np.degrees(dq).tolist(),
                        'ddq_deg_s2':np.degrees(ddq).tolist(),'joints':state_at(parts,q,dq,ddq)})
                cases.append({'name':f'J{axis+1}_{direction:+d}_{tb*1000:.0f}ms_{deg_s}deg_s',
                    'speed_deg_s':deg_s,
                    'assumption':('Un eje cada vez; 30gr/s propuestos, no nominales bajo carga.' if deg_s==30 else
                      'Sensibilidad a600gr/s, velocidad SIN CARGA anunciada del nuevo servo; no demuestra que pueda alcanzarla con este brazo.'),
                    'snapshots':snapshots})
    return cases

def main():
    parts,ff=load()
    joints=st.joints(parts,ff)
    groups=st.aggregate(parts,'root');cats=st.aggregate(parts,'category')
    mass=sum(b['mass_g'] for b in parts)
    cg=sum((np.array(b['com_mm'])*b['mass_g'] for b in parts),np.zeros(3))/mass
    static={'mass_total_g':mass,'cg_global_mm':cg.tolist(),'groups':groups,'categories':cats,'joints':joints}
    (HERE/'estatica_v08.json').write_text(json.dumps(static,ensure_ascii=False,indent=2),encoding='utf-8')
    rows=[{k:v for k,v in b.items() if not k.startswith('_')} for b in parts]
    st.write_csv(HERE/'masas_por_cuerpo_v08.csv',rows,['root','component','body','category','volume_cm3','mass_g','com_mm','basis'])
    red=st.aggregate([b for b in parts if b['category']=='PETG impreso'],'component')
    st.write_csv(HERE/'petg_por_pieza_v08.csv',red,['name','volume_cm3','mass_g','com_mm'])
    cases=brake_scenarios(parts)
    peaks=[]
    for c in cases:
        for j in ['J1','J2','J3']:
            data=[(s,next(x for x in s['joints'] if x['joint']==j)) for s in c['snapshots']]
            for metric in ['servo_torque_kgfcm','bending_Nm']:
                s,w=max(data,key=lambda z:z[1][metric])
                peaks.append({'case':c['name'],'joint':j,'metric':metric,'value':w[metric],'time_s':s['time_s'],
                              'q_deg':s['q_deg'],'force_N':w['force_N'],'moment_Nm':w['moment_Nm']})
    result={'scope':'Calculo rigido ilustrativo de frenada un eje a30gr/s y sensibilidad J3 a600gr/s SIN CARGA anunciado, 200/100/50ms; no envolvente completa ni capacidad fisica.',
        'tensor_basis':'Tensor uniforme CAD escalado a masa por cuerpo. Motores sin interiores; rotor/reflejo, flexibilidad, backlash y AGV omitidos.',
        'g':st.G,'peaks':peaks,'cases':cases}
    (HERE/'frenadas_hipotesis_v08.json').write_text(json.dumps(result,ensure_ascii=False,indent=2),encoding='utf-8')
    st.write_csv(HERE/'picos_frenadas_v08.csv',peaks,['case','joint','metric','value','time_s','q_deg','force_N','moment_Nm'])
    print(json.dumps({'mass_g':mass,'cg_mm':cg.tolist(),'joints':[{k:j[k] for k in ['joint','axis_torque_Nm','non_axis_bending_Nm','servo_static_kgfcm_eta']} for j in joints],
        'peak30_kgfcm_J1_J2_J3':{j:max(p['value'] for p in peaks if p['joint']==j and p['metric']=='servo_torque_kgfcm' and p['case'].endswith('_30deg_s')) for j in ['J1','J2','J3']},
        'peak600_kgfcm_J3':max(p['value'] for p in peaks if p['joint']=='J3' and p['metric']=='servo_torque_kgfcm' and p['case'].endswith('_600deg_s'))},ensure_ascii=False))

if __name__=='__main__':main()
