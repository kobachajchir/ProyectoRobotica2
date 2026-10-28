"""Presupuesto de reductores antes de modelar. No es un reductor comercial.

STS12V y6 reducciones24:1; capacidades nominales frente a cargas dinamicas.
Masas/tensores de reductores son presupuestos de estudio, no CAD ni catalogo.
"""
from pathlib import Path
import sys,json,itertools,math
here=Path(__file__).resolve().parent
full_envelope='--envelope' in sys.argv
sys.argv=[str(here/'iterar.py'),'--prepare-only']
exec(compile((here/'iterar.py').read_text(encoding='utf-8'),str(here/'iterar.py'),'exec'),globals())
cfg['motors']['STS12']=dict(name='ST-3215-C047 12V',mass_g=56,box_xyz_mm=[45.2,35,24.7],offset_from_left_mm=12,
    travel_deg=5040,torque_kgfcm={'12':27},rated_kgfcm=10,no_load_deg_s={'12':270},evaluation_voltage='12')
motor_ids.update(J1='V09_M01_GX3270BLS_Base',J6='V02_M06_MG90S_J6')
# Find actual J6 name instead of trusting a naming convention.
motor_ids['J6']=next(b['component'] for b in parts if '_M06_' in b['component'])
remove=[aggregate('retirar:'+j,[b for b in parts if b['component']==motor_ids[j]]) for j in ['J1','J6']]
sts={j:synthetic(j,'STS12') for j in ['J2','J3','J4','J5','J6']}
j1=features[remove[0]]
sts={'J1':feature('motor_nuevo:STS_J1',j1['root'],.056,j1['cg'],np.eye(3)*.0452**2/6),**sts}
newgrip=synthetic('J7','MG92B')
reducers=[]
for k,j in enumerate(sts):
    parent=features[sts[j]]['root'];child=['01_Giro_base','02_Brazo_PVC','03_Antebrazo_PVC','04_Giro_muneca','05_Inclinacion_muneca','06_Herramienta'][k]
    # Mass split50/50 between housing-parent and output side. Isotropic
    # inertia budget R=50mm, not a certified internal/reflected inertia.
    a=feature('estudio_reductor_fijo:'+j,parent,1.,features[sts[j]]['cg'],np.eye(3)*.05**2/2)
    b=feature('estudio_reductor_salida:'+j,child,1.,np.array([0,0,.020]),np.eye(3)*.05**2/2)
    reducers.append((a,b))
power=feature('estudio_fuente','00_Base_control',.200,np.array([0,0,.020]),np.eye(3)*.06**2/6)
names=[];ww=[]
for mass,eta in itertools.product([.150,.300,.450],[.50,.65]):
    w=np.zeros(len(features));chosen=struct+[v for j,v in old.items() if j not in ['J2','J3','J4','J5','J7']]+list(sts.values())+[newgrip,power]
    for i in chosen:w[i]=features[i]['mass_kg']
    for i in remove:w[i]=-features[i]['mass_kg']
    for a,b in reducers:w[a]=mass/2;w[b]=mass/2
    names.append(dict(reducer_each_g=mass*1000,eta=eta));ww.append(w)
weights=np.array(ww)
roots=[f['root'] for f in features];cg=np.array([f['cg'] for f in features]);I=np.array([f['I'] for f in features])
mask=[]
for name,root,supported,axisroot,axis,z in routes:
    mask.append([f['root'] in supported and f['kind']=='main' for f in features])
route_weights=np.array(mask)[:,None,:]*weights[None,:,:]
root_indexes={root:np.array([i for i,f in enumerate(features) if f['root']==root]) for root in set(roots)}
ratio=24.;speedmax=270/ratio;tb=.05;peak=np.zeros((6,len(names)));Fpeak=peak.copy();Mpeak=peak.copy()
for signs in itertools.product([-1,1],repeat=6):
    speed=np.radians(signs)*speedmax;end=np.radians([0,90,0,0,0,0])
    for t in np.linspace(0,tb,21):
        s=t/tb;q=end-speed*tb/2+speed*tb*(s-s**3+.5*s**4)
        dq=speed*(1-3*s*s+2*s**3);ddq=speed*(-6*s+6*s*s)/tb
        wr,tau,loc,fr=evaluate(q,dq,ddq)
        peak=np.maximum(peak,np.abs(tau[:6])/.0980665)
        Fpeak=np.maximum(Fpeak,np.linalg.norm(wr[:6,:,0,:],axis=-1))
        Mpeak=np.maximum(Mpeak,np.linalg.norm(wr[:6,:,1,:],axis=-1))
fr=d.root_frames(np.radians([90,90,0,0,0,0]))
worldcg=np.array([fr[f['root']][:3,:3]@f['cg']+fr[f['root']][:3,3] for f in features])
rows=[]
for i,n in enumerate(names):
    m=weights[i].sum();tau=peak[:,i]/(ratio*n['eta'])
    rows.append(dict(**n,mass_total_g=float(m*1000),cg_mm=(weights[i]@worldcg/m*1000).tolist(),
        peak_joint_kgfcm=peak[:,i].tolist(),peak_motor_kgfcm=tau.tolist(),rated_torque_ratio=(10/tau).tolist(),
        stall_lower_bound_ratio=(27/tau).tolist(),max_force_N=Fpeak[:,i].tolist(),max_moment_Nm=Mpeak[:,i].tolist()))
budget=[.300,.300,.300,.075,.150,.075]
ratios=np.array([12,28,24,4,12,4],float); eta=.50
w=weights[0].copy()
for (a,b),m in zip(reducers,budget):w[a]=m/2;w[b]=m/2
weights=np.array([w]);route_weights=np.array(mask)[:,None,:]*weights[None,:,:]
pk=np.zeros(6);j1F=0.;j1M=0.
for signs in itertools.product([-1,1],repeat=6):
    speed=np.radians(signs)*297/ratios;end=np.radians([0,90,0,0,0,0])
    for t in np.linspace(0,tb,21):
        s=t/tb;q=end-speed*tb/2+speed*tb*(s-s**3+.5*s**4)
        wr,tau,loc,fr=evaluate(q,speed*(1-3*s*s+2*s**3),speed*(-6*s+6*s*s)/tb)
        pk=np.maximum(pk,np.abs(tau[:6,0])/.0980665)
        j1F=max(j1F,float(np.linalg.norm(wr[0,0,0])));j1M=max(j1M,float(np.linalg.norm(wr[0,0,1])))
tau=pk/(ratios*eta);m=w.sum()
optimized=dict(scope='Otro contra/ejemplo local1344 estados; velocidad+10%,eta0.50. Propuesta de presupuesto, no reductor diseñado.',
    ratios=ratios.tolist(),reducer_mass_budget_g=(np.array(budget)*1000).tolist(),eta=eta,
    initial_output_speed_deg_s=(297/ratios).tolist(),mass_total_g=float(m*1000),
    cg_mm=(w@worldcg/m*1000).tolist(),peak_joint_kgfcm=pk.tolist(),peak_motor_kgfcm=tau.tolist(),
    rated_torque_ratio=(10/tau).tolist(),max_J1_force_N=j1F,max_J1_moment_Nm=j1M,
    total_output_travel_deg=(5040/ratios).tolist(),source_motor_no_load_tolerance_percent=10)
if full_envelope:
    # Bound local supplier's80g package as80g net, without asserting it is net.
    for k in sts.values():w[k]=.080
    weights=np.array([w]);route_weights=np.array(mask)[:,None,:]*weights[None,:,:]
    source=np.load(H.parents[2]/'exports'/'analisis-actuadores-v11'/'cargas_comparadas.npz')
    qc=source['q_deg'];tc=source['time_s'];ic=source['case_index'];ll=source['source_trajectory_labels']
    records=[];wwr=[];lloc=[];qq=[];tt=[];ci_out=[];labels_out=[]
    for caseid in np.unique(ic):
        label=str(ll[caseid])
        if not label.endswith('_50ms'):continue
        ids=np.flatnonzero(ic==caseid);first=ids[np.argmin(tc[ids])];last=ids[np.argmax(tc[ids])]
        delta=qc[last]-qc[first];signs=np.sign(np.where(np.abs(delta)<1e-8,0,delta))
        speed=np.radians(signs)*297/ratios;end=np.radians(qc[last]);idx=len(labels_out)
        labels_out.append(label)
        for t in np.linspace(0,.05,21):
            s=t/.05;q=end-speed*.05/2+speed*.05*(s-s**3+.5*s**4)
            wr,tau,loc,fr=evaluate(q,speed*(1-3*s*s+2*s**3),speed*(-6*s+6*s*s)/.05)
            wwr.append(wr);lloc.append(loc);qq.append(np.degrees(q));tt.append(t);ci_out.append(idx)
            records.append(np.abs(tau[:6,0])/.0980665/(ratios*eta))
    wwr=np.array(wwr);lloc=np.array(lloc);qq=np.array(qq);tt=np.array(tt)
    pk=np.max(records,axis=0);m=w.sum()
    E=H.parents[2]/'exports'/'analisis-actuadores-v11-STS';E.mkdir(exist_ok=True)
    expanded=[];yaw_end=[];expanded_q=[]
    for yaw in range(-90,91,10):
        for i,q in enumerate(qq):
            if not -90<=q[0]+yaw<=90:continue
            expanded.append(i);yaw_end.append(yaw);qs=q.copy();qs[0]+=yaw;expanded_q.append(qs)
    np.savez_compressed(E/'cargas_comparadas.npz',case_names=np.array(['STS_presupuesto_80g']),
        F_M_world=wwr,F_M_local=lloc,expanded_to_source=expanded,expanded_yaw_end_deg=yaw_end,expanded_q_deg=expanded_q,
        q_deg=qq,time_s=tt,case_index=ci_out,source_trajectory_labels=labels_out,motor_torque_kgfcm=np.array(records))
    envelope=dict(scope='Misma familia de finales v10, solo frenadas50ms; seis maximos de catalogo+10%, masa80g por motor y presupuestos de reductor. No workspace continuo.',
        name='STS_presupuesto_80g',mass_g=float(m*1000),cg_mm=(w@worldcg/m*1000).tolist(),states=len(tt),expanded_J1_states=len(expanded),trajectories=len(labels_out),
        ratios=ratios.tolist(),reducer_mass_budget_g=(np.array(budget)*1000).tolist(),eta=eta,
        initial_output_speed_deg_s=(297/ratios).tolist(),peak_motor_kgfcm=pk.tolist(),rated_torque_ratio=(10/pk).tolist(),
        max_joint_force_N=np.max(np.linalg.norm(wwr[:,:6,0,0],axis=-1),axis=0).tolist(),
        max_joint_moment_Nm=np.max(np.linalg.norm(wwr[:,:6,0,1],axis=-1),axis=0).tolist(),
        fixed_base_extra_mass_g=340.,fixed_extra_cg_world_mm=(worldcg[sts['J1']]*1000).tolist(),
        frame_extra_mass_g=140.,note='Fixed340g = reductorfijo150 + motor80-90 + fuente200; frame140g excluye fuente en piso. CG fuente x=y=0.',
        static_tipping_sensitivity={},scenarios={})
    (H/'STS_resultados.json').write_text(json.dumps(dict(results=[envelope]),ensure_ascii=False,indent=2),encoding='utf-8')
    print('ENVOLVENTE80g',len(tt),[round(x,2) for x in envelope['rated_torque_ratio']],flush=True)
out=dict(scope='Estudio local horizontal,1344 estados por perfil; no barrido global, no reductor comercial seleccionado ni resistencia de dientes/contacto acreditada.',
    ratio=ratio,output_no_load_deg_s=speedmax,motor_no_load_deg_s=270,stop_ms=50,
    source='https://files.seeedstudio.com/products/Feetech/108090003_FEETECH_ST-3215-C047-Datasheet.pdf',
    travel_total_output_deg=5040/ratio,travel_J2_needed_deg=135,travel_other_needed_deg=180,
    assumptions=['Masa56g tope55+-1','Par nominal10kgfcm no equivale a curva de frenado','Sensibilidad par bloqueo27kgfcm: nominal30 menos10% ficha',
       'Masa de reductor incluye housing,gears,ejes,rodamientos; mitad padre/mitad salida',
       'No rotor/engranajes internos medidos; tensor isotropico presupuesto R50mm no demuestra cota superior real',
       'Velocidad270deg/s central; tolerancia ficha+-10% no barrida','Multivuelta pierde contador al apagar; requiere inicializacion de postura, sin prometer posicion absoluta persistente',
       'Reducciones y eficiencias propuestas; no producto comprado ni dibujado'],rows=rows,optimized_budget=optimized)
(H/'estudio_STS_resultados.json').write_text(json.dumps(out,ensure_ascii=False,indent=2),encoding='utf-8')
for r in rows:print(r['reducer_each_g'],r['eta'],round(r['mass_total_g']),[round(x,2) for x in r['rated_torque_ratio']],flush=True)
print('OPTIMIZADO_LOCAL56g',round(optimized['mass_total_g']),[round(x,2) for x in optimized['rated_torque_ratio']],flush=True)
