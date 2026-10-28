"""Cribado a30grados/s sin relajar velocidad: STS3250 y STS3215.

Solo cuerpos rigidos externos y presupuestos. No aprueba inercias internas,
freno, reducciones inexistentes ni PLA FDM. No usa ni modifica Fusion.
"""
from pathlib import Path
import json,sys,math,itertools,csv
H=Path(__file__).resolve().parent;V11=H.parent/'busqueda_actuadores_v11';PRE=H.parent/'preseleccion_actuadores_v10'
cfg=json.loads((PRE/'candidatos.json').read_text(encoding='utf-8'))
for key,rated,stall,speed in [('STS3250',16,50,450),('STS3215',10,30,270)]:
    cfg['motors'][key]=dict(name=key+'12V',mass_g=80,box_xyz_mm=[45.22,35,24.72],offset_from_left_mm=12,
        travel_deg=5040,rated_kgfcm=rated,stall_kgfcm=stall,no_load_deg_s=speed,
        mass_note='Presupuesto80g; ficha3250:74.5+-1g,3215:55+-1g. No peso de paquete como dato neto.')
(H/'candidatos.json').write_text(json.dumps(cfg,ensure_ascii=False,indent=2),encoding='utf-8')
src=(PRE/'comparar.py').read_text(encoding='utf-8')
src=src.replace("['J4','J7']","['J4','J6','J7']")
exec(compile(src[:src.index("case('v09_actual')")],str(H/'preparar_v10.py'),'exec'))
motor_ids['J1']='V09_M01_GX3270BLS_Base';motor_ids['J6']='V02_M06_MG90S_J6'
remove=[aggregate('retirar:'+j,[b for b in parts if b['component']==motor_ids[j]]) for j in ['J1','J6']]
chosenmotors={j:synthetic(j,'STS3250' if j in ['J2','J3'] else 'STS3215') for j in ['J2','J3','J4','J5','J6']}
j1=features[remove[0]]
chosenmotors={'J1':feature('motor_STS3250_J1',j1['root'],.080,j1['cg'],np.eye(3)*.04522**2/6),**chosenmotors}
grips=[synthetic('J7','MG92B'),synthetic('J7','STS3215')]
reducers=[]
for k,(j,idx) in enumerate(chosenmotors.items()):
    f=features[idx];child=['01_Giro_base','02_Brazo_PVC','03_Antebrazo_PVC','04_Giro_muneca','05_Inclinacion_muneca','06_Herramienta'][k]
    a=feature('reserva_reductor_fijo:'+j,f['root'],1,f['cg'],np.eye(3)*.05**2/2)
    b=feature('reserva_reductor_salida:'+j,child,1,np.array([0,0,.020]),np.eye(3)*.05**2/2)
    reducers.append((a,b))
power=feature('potencia_fija','00_Base_control',.200,np.array([0,0,.020]),np.eye(3)*.06**2/6)
budget=np.array([.125,.175,.175,.050,.075,.050])
profiles=[];ws=[]
for n,eta,gi in itertools.product([9,10,11],[.70,.85],range(2)):
    w=np.zeros(len(features))
    for k in struct+list(chosenmotors.values())+[grips[gi],power]:w[k]=features[k]['mass_kg']
    for k in remove:w[k]=-features[k]['mass_kg']
    for (a,b),mass in zip(reducers,budget):w[a]=mass/2;w[b]=mass/2
    profiles.append(dict(name=f'n{n}_eta{eta}_grip{gi}',ratio=[n,n,n,4,6,4],eta=eta,gripper='MG92B' if gi==0 else 'STS3215',reducer_budget_g=(budget*1000).tolist()))
    ws.append(w)
weights=np.array(ws)
exec(compile(src[src.index('roots=[f'):src.index('traj=[]')],str(H/'evaluar_v10.py'),'exec'))
source=np.load(H.parents[2]/'exports'/'analisis-actuadores-v11'/'cargas_comparadas.npz')
qc=source['q_deg'];tc=source['time_s'];ic=source['case_index'];labels=source['source_trajectory_labels']
ratio=np.array([p['ratio'] for p in profiles]);etas=np.array([p['eta'] for p in profiles])
nominal=np.array([16,16,16,10,10,10]);stall=np.array([50,50,50,30,30,30])*.9
speed_min=np.array([450,450,450,270,270,270])*.9
peak=np.zeros((len(profiles),6));minratio=np.full_like(peak,np.inf);minspeed=np.full_like(peak,np.inf)
maxF=peak.copy();maxM=peak.copy();witness={};rr=[];locs=[];qout=[];tout=[];caseout=[];lout=[];accs=[];dqout=[];signed=[]
for cid in np.unique(ic):
    label=str(labels[cid])
    if not label.endswith('_50ms'):continue
    ids=np.flatnonzero(ic==cid);end=np.radians(qc[ids[np.argmax(tc[ids])]])
    diff=qc[ids[np.argmax(tc[ids])]]-qc[ids[np.argmin(tc[ids])]];sign=np.sign(np.where(np.abs(diff)<1e-8,0,diff))
    speed=np.radians(sign)*30;co=len(lout);lout.append(label)
    for t in np.linspace(0,.05,21):
        s=t/.05;q=end-speed*.05/2+speed*.05*(s-s**3+.5*s**4)
        dq=speed*(1-3*s*s+2*s**3);ddq=speed*(-6*s+6*s*s)/.05
        wr,tau,loc,fr=evaluate(q,dq,ddq)
        motor=tau[:6].T/.0980665/(ratio*etas[:,None]);demand=np.abs(motor)
        atspeed=stall[None,:]*np.maximum(0,1-np.abs(np.degrees(dq))[None,:]*ratio/speed_min[None,:])
        cap=np.minimum(nominal[None,:],atspeed)
        r=np.divide(cap,demand,out=np.full_like(cap,np.inf),where=demand>1e-10)
        peak=np.maximum(peak,demand)
        minratio=np.minimum(minratio,np.divide(nominal,demand,out=np.full_like(demand,np.inf),where=demand>1e-10))
        minspeed=np.minimum(minspeed,r)
        maxF=np.maximum(maxF,np.linalg.norm(wr[:6,:,0,:],axis=-1).T)
        maxM=np.maximum(maxM,np.linalg.norm(wr[:6,:,1,:],axis=-1).T)
        rr.append(wr);locs.append(loc);qout.append(np.degrees(q));tout.append(t);caseout.append(co);accs.append(ddq);dqout.append(dq);signed.append(motor)
    if len(lout)%60==0:print('Trayectorias',len(lout),flush=True)
frames=d.root_frames(np.radians([90,90,0,0,0,0]))
worldcg=np.array([frames[f['root']][:3,:3]@f['cg']+frames[f['root']][:3,3] for f in features])
rows=[]
for i,p in enumerate(profiles):
    mass=weights[i].sum()
    rows.append(dict(**p,mass_g=float(mass*1000),cg_mm=(weights[i]@worldcg/mass*1000).tolist(),
        peak_motor_kgfcm=peak[i].tolist(),nominal_ratio=minratio[i].tolist(),linear_speed_ratio=minspeed[i].tolist(),
        nominal_torque_kgfcm=nominal.tolist(),speed_no_load_min_deg_s=speed_min.tolist(),
        max_joint_force_N=maxF[i].tolist(),max_joint_moment_Nm=maxM[i].tolist(),
        pass_nominal=bool(np.all(minratio[i]>=2)),pass_linear_sensitivity=bool(np.all(minspeed[i]>=2))))
out=dict(scope='30grados/s por eje,200g,50ms; familia finalesv11; sin inercia interna medida, sin freno ni reductor certificado.',
    states=len(tout),trajectories=len(lout),rows=rows)
(H/'resultados.json').write_text(json.dumps(out,ensure_ascii=False,indent=2),encoding='utf-8')
E=H.parents[2]/'exports'/'analisis-actuadores-v12';E.mkdir(exist_ok=True)
np.savez_compressed(E/'cargas.npz',F_M_world=rr,F_M_local=locs,q_deg=qout,time_s=tout,case_index=caseout,source_trajectory_labels=lout,
    motor_torque_signed_kgfcm=signed,ddq_rad_s2=accs,dq_rad_s=dqout,case_names=np.array([p['name'] for p in profiles]))
for p in rows:print(p['name'],round(p['mass_g']),round(min(p['nominal_ratio']),3),round(min(p['linear_speed_ratio']),3),flush=True)
