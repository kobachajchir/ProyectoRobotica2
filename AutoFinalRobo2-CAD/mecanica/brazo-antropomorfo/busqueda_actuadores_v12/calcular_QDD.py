"""Comparar par de sostenimiento y efecto de inercia conocida RS01.

Geometrias de motor/cajas/disipadores son presupuestos de preseleccion.
No aprueba freno electrico, rigidez ni choques; no modifica Fusion.
"""
from pathlib import Path
import json,itertools,math
here=Path(__file__).resolve().parent
prepare=(here/'calcular.py').read_text(encoding='utf-8')
exec(compile(prepare[:prepare.index('profiles=[]')],str(here/'preparar_QDD.py'),'exec'))
cfg['motors']['RS01']=dict(name='RS01 referencia20261009',mass_g=398,box_xyz_mm=[80,45,80],offset_from_left_mm=40,travel_deg=999999,
    note='Caja80x80x45 propuesta para calcular tensor; masa395+-3 de ficha. No plano de montaje.')
mot={j:synthetic(j,'RS01') for j in ['J2','J3']}
oldbase=features[remove[0]]
mot={'J1':feature('RS01_base','00_Base_control',.398,oldbase['cg'],np.eye(3)*.08**2/6),**mot,
    **{j:chosenmotors[j] for j in ['J4','J5','J6']}}
heats=[]
for j in ['J1','J2','J3']:
    f=features[mot[j]];pos=f['cg']+np.array([0,.040,0])
    # Each plate150x150xthickness, normal to localY. Thickness is not
    # specified by the manufacturer table;3/5mm are explicit sensitivities.
    heats.append(feature('disipador:'+j,f['root'],1,pos,np.diag([.15**2+.005**2,.15**2*2,.15**2+.005**2])/12))
basepower=feature('potencia36V_presupuesto','00_Base_control',.650,np.array([0,0,.020]),np.eye(3)*.10**2/6)
profiles=[];ws=[]
budget=np.array([.150,.250,.250,.030,.075,.030])
for thick,eta,ifactor in itertools.product([.003,.005],[.70,.85],[1,2]):
    w=np.zeros(len(features))
    for k in struct+list(mot.values())+[grips[0],basepower]:w[k]=features[k]['mass_kg']
    for k in remove:w[k]=-features[k]['mass_kg']
    for k in heats:w[k]=.15*.15*thick*2700
    for (a,b),mass in zip(reducers,budget):w[a]=mass/2;w[b]=mass/2
    profiles.append(dict(name=f'RS01_plate{thick*1000:g}_eta{eta}_I{ifactor}',ratio=[3,6,3,1,4,1],eta=eta,inertia_factor=ifactor,plate_each_g=.15*.15*thick*2700*1000));ws.append(w)
weights=np.array(ws)
exec(compile(src[src.index('roots=[f'):src.index('traj=[]')],str(H/'evaluar_QDD_v10.py'),'exec'))
source=np.load(H.parents[2]/'exports'/'analisis-actuadores-v11'/'cargas_comparadas.npz')
qc=source['q_deg'];tc=source['time_s'];ic=source['case_index'];labels=source['source_trajectory_labels']
ratios=np.array([p['ratio'] for p in profiles]);etas=np.array([p['eta'] for p in profiles]);factors=np.array([p['inertia_factor'] for p in profiles])
nominal=np.array([6.,6.,6.,.980665,.980665,.980665])
peak=np.zeros((len(profiles),6));peak_rigid=peak.copy();minimum=np.full_like(peak,np.inf);force=peak.copy();moment=peak.copy()
rowsW=[];rowsL=[];times=[];angles=[];idsout=[];labelsout=[];rotorpeaks=np.zeros(3)
for cid in np.unique(ic):
    label=str(labels[cid])
    if not label.endswith('_50ms'):continue
    ii=np.flatnonzero(ic==cid);end=np.radians(qc[ii[np.argmax(tc[ii])]])
    dif=qc[ii[np.argmax(tc[ii])]]-qc[ii[np.argmin(tc[ii])]];signs=np.sign(np.where(np.abs(dif)<1e-8,0,dif));speed=np.radians(signs)*30
    caseid=len(labelsout);labelsout.append(label)
    for t in np.linspace(0,.05,21):
        s=t/.05;q=end-speed*.05/2+speed*.05*(s-s**3+.5*s**4)
        dq=speed*(1-3*s*s+2*s**3);ddq=speed*(-6*s+6*s*s)/.05
        wr,tau,loc,frames=evaluate(q,dq,ddq)
        rigid=tau[:6].T/(ratios*etas[:,None])
        h=1e-4;fp=d.root_frames(q+dq*h+.5*ddq*h*h);fm=d.root_frames(q-dq*h+.5*ddq*h*h)
        intrinsic=np.zeros_like(rigid)
        for j,parent in enumerate(['00_Base_control','01_Giro_base','02_Brazo_PVC']):
            R=frames[parent][:3,:3];Rd=(fp[parent][:3,:3]-fm[parent][:3,:3])/(2*h);Rdd=(fp[parent][:3,:3]-2*R+fm[parent][:3,:3])/(h*h)
            O=Rd@R.T;A=Rdd@R.T+Rd@Rd.T
            omega=np.array([O[2,1]-O[1,2],O[0,2]-O[2,0],O[1,0]-O[0,1]])/2
            alpha=np.array([A[2,1]-A[1,2],A[0,2]-A[2,0],A[1,0]-A[0,1]])/2
            # Treat all equivalent inertia as an axial rotor at the actuator
            # output. Do not infer an undocumented distribution through7.75:1.
            axis=R[:,2 if j==0 else 1];ratio=ratios[:,j];J=.0042*factors;gin=1.
            intrinsic[:,j]=J*(ratio*ddq[j]+alpha@axis/gin)
            # Angular momentum derivative of the equivalent axial rotor.
            hd=(J/gin**2)[:,None]*((alpha@axis+gin*ratio*ddq[j])[:,None]*axis+
                (omega@axis+gin*ratio*dq[j])[:,None]*np.cross(omega,axis))
            for k,(_,root,supported,axisroot,ax,z) in enumerate(routes):
                if parent not in supported:continue
                wr[k,:,1]+=hd
                if k<6:
                    jointaxis=np.eye(3)[:,ax] if axisroot=='global' else frames[axisroot][:3,ax]
                    tau[k]+=hd@jointaxis
                loc[k,:,1]+=hd@frames[root][:3,:3]
            rotorpeaks[j]=max(rotorpeaks[j],float(np.max(np.abs(intrinsic[:,j]))))
        motor=tau[:6].T/(ratios*etas[:,None])+intrinsic
        demand=np.abs(motor);peak=np.maximum(peak,demand);peak_rigid=np.maximum(peak_rigid,np.abs(rigid))
        minimum=np.minimum(minimum,np.divide(nominal,demand,out=np.full_like(demand,np.inf),where=demand>1e-10))
        force=np.maximum(force,np.linalg.norm(wr[:6,:,0,:],axis=-1).T);moment=np.maximum(moment,np.linalg.norm(wr[:6,:,1,:],axis=-1).T)
        rowsW.append(wr);rowsL.append(loc);times.append(t);angles.append(np.degrees(q));idsout.append(caseid)
    if len(labelsout)%80==0:print('QDD trayectorias',len(labelsout),flush=True)
frames=d.root_frames(np.radians([90,90,0,0,0,0]));worldcg=np.array([frames[f['root']][:3,:3]@f['cg']+frames[f['root']][:3,3] for f in features])
rows=[]
for i,p in enumerate(profiles):
    mass=weights[i].sum()
    rows.append(dict(**p,mass_g=float(mass*1000),cg_mm=(weights[i]@worldcg/mass*1000).tolist(),
        demand_motor_Nm=peak[i].tolist(),demand_rigid_only_Nm=peak_rigid[i].tolist(),capacity_Nm=nominal.tolist(),ratio_capacity=minimum[i].tolist(),
        max_joint_force_N=force[i].tolist(),max_joint_moment_Nm=moment[i].tolist(),passes_filter=bool(np.all(minimum[i]>=2))))
out=dict(scope='RS01J1/J2/J3; STS3215J4/J5/J6.30grados/s200g50ms. RS01 par reposo6Nm bajo condiciones termicas de ficha; inercia0.0042 y sensibilidadx2. STS sin inercia interna acreditada.',
    states=len(times),trajectories=len(labelsout),rotor_peak_additional_motor_Nm=rotorpeaks.tolist(),rows=rows,
    limitations=['No actuador completo seleccionado','3mm/5mm de disipador son hipotesis; fabricante solo identifica150x150mm en tabla',
    'Caja80x80x45mm no es plano de motor','Modelo axial equivalente de rotor; no inercia transversal interna medida',
    'Inercia equivalente axial situada en salida del actuador; no distribucion interna inferida por7.75:1',
    'Inercia STS pendiente; falta freno/corriente regenerada y alimentacion36V','Mass650g potencia es reserva sin bateria/controladores seleccionados',
    'No hay nueva geometria, housing, FE ni choque acreditado'])
(H/'QDD_resultados.json').write_text(json.dumps(out,ensure_ascii=False,indent=2),encoding='utf-8')
E=H.parents[2]/'exports'/'analisis-actuadores-v12-QDD';E.mkdir(exist_ok=True)
np.savez_compressed(E/'cargas.npz',F_M_world=rowsW,F_M_local=rowsL,time_s=times,q_deg=angles,case_index=idsout,source_trajectory_labels=labelsout,case_names=[p['name'] for p in profiles])
for r in rows:print(r['name'],round(r['mass_g']),round(min(r['ratio_capacity']),3),r['passes_filter'],flush=True)
