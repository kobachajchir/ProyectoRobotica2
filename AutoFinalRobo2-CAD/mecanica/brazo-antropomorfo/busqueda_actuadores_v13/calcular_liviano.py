"""Nuevo BOM aluminio/PLA/PVC, FK reducida y cargas30grados/s200g50ms.

No se reduce la densidad de los cuerpos viejos. Las primitivas nuevas son
prediseno, no masas de CAD fabricable ni validacion de perfiles impresos.
"""
from pathlib import Path
import json,math,copy
import numpy as np
H=Path(__file__).resolve().parent
bom=json.loads((H/'prediseno_componentes.json').read_text(encoding='utf-8'))
z=np.load(H.parents[2]/'exports'/'analisis-actuadores-v11'/'cargas_comparadas.npz')
qc,tc,ic,ll=z['q_deg'],z['time_s'],z['case_index'],z['source_trajectory_labels']
traj=[];labels=[]
for cid in np.unique(ic):
    label=str(ll[cid])
    if not label.endswith('_50ms'):continue
    ids=np.flatnonzero(ic==cid);end=np.radians(qc[ids[np.argmax(tc[ids])]])
    delta=qc[ids[np.argmax(tc[ids])]]-qc[ids[np.argmin(tc[ids])]]
    signs=np.sign(np.where(np.abs(delta)<1e-8,0,delta));speed=np.radians(signs)*30;co=len(labels);labels.append(label)
    for t in np.linspace(0,.05,21):
        s=t/.05;q=end-speed*.05/2+speed*.05*(s-s**3+.5*s**4)
        traj.append((q,speed*(1-3*s*s+2*s**3),speed*(-6*s+6*s*s)/.05,t,co))
def tr(y=0,z=0):
    T=np.eye(4);T[:3,3]=[0,y,z];return T
def rot(axis,a):
    T=np.eye(4);c,s=math.cos(a),math.sin(a)
    T[:3,:3]=[[c,-s,0],[s,c,0],[0,0,1]] if axis=='Z' else [[c,0,s],[0,1,0],[-s,0,c]]
    return T
def frames(q,p):
    a=tr(p.get('axis_y_mm',-14)*.001,p['base_top']*.001)@rot('Z',q[0]);f={'00':np.eye(4),'01':a}
    for j,root,step,axis in [(1,'02',p['tower_h'],'Y'),(2,'03',p['link1'],'Y'),(3,'04',p['link2'],'Z'),(4,'05',p['wrist45'],'Y'),(5,'06',p['wrist56'],'Z')]:
        a=a@tr(z=step*.001)@rot(axis,q[j]);f[root]=a
    f['90']=a@tr(z=p['payload_z']*.001);return f
def aggregate(pp):
    groups={}
    for root in ['00','01','02','03','04','05','06','90']:
        parts=[x for x in pp if x['root']==root];m=sum(x['mass_g'] for x in parts)/1000
        cg=sum((x['mass_g']/1000*np.array(x['cg_local_mm'])*.001 for x in parts),np.zeros(3))/m
        I=np.zeros((3,3))
        for x in parts:
            mass=x['mass_g']/1000;xyz=np.array(x['box_mm'])*.001;r=np.array(x['cg_local_mm'])*.001-cg
            I+=np.array(x['tensor_kg_m2']) if 'tensor_kg_m2' in x else mass*np.diag([xyz[1]**2+xyz[2]**2,xyz[0]**2+xyz[2]**2,xyz[0]**2+xyz[1]**2])/12
            I+=mass*((r@r)*np.eye(3)-np.outer(r,r))
        groups[root]=(m,cg,I)
    return groups
def bodies(q,dq,ddq,p,groups):
    h=1e-4;f=frames(q,p);fp=frames(q+dq*h+.5*ddq*h*h,p);fm=frames(q-dq*h+.5*ddq*h*h,p)
    out={}
    for root,(m,cg,I) in groups.items():
        A,Ap,Am=f[root],fp[root],fm[root];R=A[:3,:3];x=R@cg+A[:3,3]
        acc=((Ap[:3,:3]-2*R+Am[:3,:3])@cg+Ap[:3,3]-2*A[:3,3]+Am[:3,3])/(h*h)
        Rd=(Ap[:3,:3]-Am[:3,:3])/(2*h);Rdd=(Ap[:3,:3]-2*R+Am[:3,:3])/(h*h)
        W=Rd@R.T;Al=Rdd@R.T+Rd@Rd.T
        w=np.array([W[2,1]-W[1,2],W[0,2]-W[2,0],W[1,0]-W[0,1]])/2
        alpha=np.array([Al[2,1]-Al[1,2],Al[0,2]-Al[2,0],Al[1,0]-Al[0,1]])/2
        F=m*(acc+[0,0,9.80665]);Iw=R@I@R.T;M=np.cross(x,F)+Iw@alpha+np.cross(w,Iw@w)
        out[root]=(F,M)
    return f,out
profiles=[('centrado_L130_120',130,120,False,1),('centrado_L100_85',100,85,False,1),
    ('centrado_PWM_L130_120',130,120,False,1),('centrado_PWM_L100_85',100,85,False,1),('centrado_PWM_L100_85_reserva25',100,85,False,1.25),
    ('centrado_PWM_L90_75',90,75,False,1),('centrado_PWM_L90_75_reserva25',90,75,False,1.25)]
rows=[];raw=[]
for name,L1,L2,ds,factor in profiles:
    p=dict(bom['lengths_mm'],link1=L1,link2=L2);parts=copy.deepcopy(bom['parts']);centered=name.startswith('centrado')
    if centered:
        p.update(axis_y_mm=-32,base_top=60)
        for x in parts:
            if x['name']=='Motor J1 GX3270BLS':x.update(name='Motor J1 STS3215',mass_g=56,cg_local_mm=[-42,-42.5,21.5],box_mm=[24.7,45.2,35])
            if x['name']=='Carcasa base con ventanas':x.update(cg_local_mm=[-42,-42.5,21.5],box_mm=[28.7,49.2,37],mass_g=12.5)
            if x['name']=='Par608 base':x['cg_local_mm']=[0,-32,43]
            if x['name']=='Eje yaw acero8mm':x['cg_local_mm'][2]=-10
        parts.extend([
            dict(root='00',name='Portal y portarrodamientos Al',mass_g=31,cg_local_mm=[0,-22,32],box_mm=[45,45,45],material='Al6061'),
            dict(root='01',name='Rueda yaw56T y pinon14T Al',mass_g=15,cg_local_mm=[-6,0,-17.5],box_mm=[66,58,3],material='Al6061')])
    for x in parts:
        if 'PWM' in name:
            if x['name']=='Motor J1 STS3215':x.update(name='Motor J1 GX3270BLS desplazado',mass_g=90,cg_local_mm=[-44,-32,25],box_mm=[40.5,21,41.7])
            if x['name']=='Carcasa base con ventanas':x.update(cg_local_mm=[-44,-32,25],box_mm=[45,26,46],mass_g=16)
            if x['name']=='Rueda yaw56T y pinon14T Al':x.update(name='Dos ruedas44T yaw1a1 Al',mass_g=18,box_mm=[90,46,3])
            if x['name']=='Motor J4 MG90S':x.update(name='Motor J4 MG92B',mass_g=13.8,cg_local_mm=[37,0,102],box_mm=[22.8,12,31])
            if x['name']=='Housing J4 con ventanas':x.update(mass_g=6.8,cg_local_mm=[37,0,102],box_mm=[28,17,35])
            if x['name']=='Soporte y pinon J4 desplazado':x.update(mass_g=8,cg_local_mm=[30,0,117],box_mm=[60,42,8])
            if x['name']=='Rueda conducida J4 28T':x.update(name='Rueda J4 34T',mass_g=3,box_mm=[36,36,4])
            if x['name']=='Motor J5 MG946R':x.update(name='Motor J5 GX3270BLS',mass_g=90,box_mm=[40.5,47,21])
        if 'J5GX' in name and x['name']=='Motor J5 MG946R':
            x.update(name='Motor J5 GX3270BLS',mass_g=90,box_mm=[40,47,20])
        if x['material'] in ['PLA','Al6061','Acero','PVC']:
            x['mass_g']*=factor
            if 'tensor_kg_m2' in x:x['tensor_kg_m2']=(np.array(x['tensor_kg_m2'])*factor).tolist()
        if ds and x['name']=='Motor J3 GX3270BLS':x['mass_g']=60;x['name']='Motor J3 DS3225 posicional270'
        if x['root'] in ['02','03']:
            old=140 if x['root']=='02' else 120;new=L1 if x['root']=='02' else L2;zz=x['cg_local_mm'][2]
            x['cg_local_mm'][2]+=new-old if zz>old/2+1e-7 else ((new-old)/2 if abs(zz-old/2)<1e-7 else 0)
            if x['material']=='PVC':x['mass_g']*=(new-30)/(old-30);x['box_mm'][2]=new-30
    if 'PWM' in name:
        parts.extend([
            dict(root='01',name='Reductor hombro1.9a1 y guardas',mass_g=14*factor,cg_local_mm=[0,24,40],box_mm=[58,8,40],material='Al6061'),
            dict(root='00',name='Reserva cuatro pies plegables y anclajes',mass_g=250*factor,cg_local_mm=[0,-32,0],box_mm=[150,160,20],material='Varios')])
    groups=aggregate(parts);mass=sum(x['mass_g'] for x in parts)
    caps=np.array([10 if centered else 63,63,21 if ds else 63,1.8,63 if 'J5GX' in name else 13,1.8]);speeds=np.array([270 if centered else 600,600,400 if ds else 600,600,600 if 'J5GX' in name else 60/.17,600])
    ratios=np.array([4 if centered else 1,1,1,1,1,1]);etas=np.array([.85 if centered else 1,1,1,1,1,1])
    if 'PWM' in name:
        caps=np.array([55,55,55,3.5,55,1.8]);ratios=np.array([1,1.9,1.4,.85,1,1]);etas=np.array([.85,.85,.85,.85,1,1]);speeds=np.array([500,500,500,750,500,600])
    qs=[];fs=[];ms=[];signed=[];allF=[];allM=[];limits=[];payload_forces=[]
    supported=['01','02','03','04','05','06']
    for q,dq,ddq,t,case in traj:
        ff,bb=bodies(q,dq,ddq,p,groups);jf=[];jm=[];tt=[]
        for j,root in enumerate(supported):
            roots=supported[j:]+['90'];F=sum((bb[r][0] for r in roots),np.zeros(3));M=sum((bb[r][1] for r in roots),np.zeros(3))-np.cross(ff[root][:3,3],F)
            parent='00' if j==0 else supported[j-1];axis=ff[parent][:3,2 if j in [0,3,5] else 1]
            jf.append(F);jm.append(M);tt.append(M@axis/.0980665/(ratios[j]*etas[j]))
        signed.append(tt);fs.append(jf);ms.append(jm);qs.append(np.degrees(q))
        payload_forces.append(np.linalg.norm(bb['90'][0]))
        allF.append(sum((bb[r][0] for r in supported+['90']),np.zeros(3)))
        allM.append(sum((bb[r][1] for r in supported+['90']),np.zeros(3)))
        avail=caps*np.maximum(0,1-np.abs(np.degrees(dq))*ratios/speeds)
        if centered and 'PWM' not in name:avail[0]=min(10,30*max(0,1-abs(np.degrees(dq[0]))*4/270))
        limits.append(avail)
    signed=np.array(signed);fs=np.array(fs);ms=np.array(ms);qs=np.array(qs);limits=np.array(limits)
    demand=np.abs(signed);ratio=np.divide(limits,demand,out=np.full_like(demand,np.inf),where=demand>1e-9)
    staticframes=frames(np.radians([90,90,0,0,0,0]),p)
    staticcg=sum((x['mass_g']*(staticframes[x['root']][:3,:3]@np.array(x['cg_local_mm'])*.001+staticframes[x['root']][:3,3]) for x in parts),np.zeros(3))/mass*1000
    # Rotate moving-group wrenches around the yaw axis; fixed base and car
    # remain fixed. Contact rectangles are reference hypotheses, not hardware.
    allF=np.array(allF);allM=np.array(allM);point=np.array([0,p.get('axis_y_mm',-14)*.001,p['base_top']*.001]);Ffix,Mfix=bodies(np.zeros(6),np.zeros(6),np.zeros(6),p,groups)[1]['00']
    ground=[]
    for bx,by,cy,gz,label in [(107,62,-32,-80,'ejes_CAD_inferidos'),(122.5,100,-32,-80,'envolvente_manual_optimista'),(110,110,-32,-80,'pies_desplegados_propuestos_220x220'),(140,140,-32,-80,'pies_desplegados_propuestos_280x280'),(160,160,-32,-80,'pies_desplegados_propuestos_320x320')]:
        worst=0.;minN=1e9;reqcar=0.;mu=0.;mu_yaw=0.;wq=None
        for yaw in range(-90,91,10):
            valid=(qs[:,0]+yaw>=-90)&(qs[:,0]+yaw<=90);R=rot('Z',math.radians(yaw))[:3,:3]
            Fmov=allF[valid]@R.T;Mmov=(allM[valid]-np.cross(point,allF[valid]))@R.T+np.cross(point,Fmov)
            Freac=-Fmov-Ffix;Mreac=-Mmov-Mfix+np.cross([0,-cy*.001,-gz*.001],Freac)
            N=-Freac[:,2]+1.5*9.80665
            ratio_tip=np.maximum(np.abs(Mreac[:,0])/(by*.001*np.maximum(N,1e-9)),np.abs(Mreac[:,1])/(bx*.001*np.maximum(N,1e-9)))
            if ratio_tip.max()>worst:worst=float(ratio_tip.max());wq=qs[valid][int(np.argmax(ratio_tip))].copy();wq[0]+=yaw
            minN=min(minN,float(N.min()));mu=max(mu,float(np.max(np.linalg.norm(Freac[:,:2],axis=1)/np.maximum(N,1e-9))))
            mu_yaw=max(mu_yaw,float(np.max(np.abs(Mreac[:,2])/(math.hypot(bx,by)*.001*np.maximum(N,1e-9)))))
            need=np.maximum.reduce([np.abs(Mreac[:,0])/(by*.001)+Freac[:,2],np.abs(Mreac[:,1])/(bx*.001)+Freac[:,2],Freac[:,2],np.zeros(len(Freac))])/9.80665
            reqcar=max(reqcar,float(need.max()))
        ground.append(dict(hypothesis=label,halfspan_mm=[bx,by],center_y_mm=cy,ground_z_mm=gz,chassis_mass_assumed_kg=1.5,
            maximum_overturning_utilization=worst,minimum_normal_N=minN,minimum_centered_car_mass_kg=reqcar,friction_mu_necessary=mu,friction_mu_yaw_necessary=mu_yaw,worst_q_deg=wq.tolist()))
    row=dict(name=name,parameters_mm=p,total_with_payload_g=mass,without_payload_g=mass-200,cg_horizontal_mm=staticcg.tolist(),
        groups=[dict(root=k,mass_g=v[0]*1000,cg_local_mm=(v[1]*1000).tolist(),tensor_kg_m2=v[2].tolist()) for k,v in groups.items()],
        peak_payload_force_N=max(payload_forces),peak_joint_force_N=np.max(np.linalg.norm(fs,axis=2),axis=0).tolist(),peak_joint_moment_Nm=np.max(np.linalg.norm(ms,axis=2),axis=0).tolist(),
        peak_motor_kgfcm=demand.max(axis=0).tolist(),capacity_kgfcm=caps.tolist(),capacity_type=['nominal' if centered and 'PWM' not in name else 'bloqueo']+['bloqueo']*5,transmission_ratio=ratios.tolist(),transmission_eta=etas.tolist(),capacity_ratio=(caps/demand.max(axis=0)).tolist(),
        linear_speed_sensitivity_min_ratio=np.min(ratio,axis=0).tolist(),ground_support=ground)
    rows.append(row);raw.append(dict(F=fs,M=ms,torque=signed,q=qs,F_moving=allF,M_origin_moving=allM))
    if name=='centrado_PWM_L90_75':
        (H/'candidato_compacto_componentes.json').write_text(json.dumps(dict(parameters_mm=p,parts=parts),ensure_ascii=False,indent=2),encoding='utf-8')
    print(name,round(mass),'tau',[round(x,2) for x in row['peak_motor_kgfcm']],'support',[round(x['maximum_overturning_utilization'],2) for x in ground],flush=True)
out=dict(scope='Nuevo prediseno,30grados/s200g50ms,primitivas no CAD. Carro1.5kg aproximado por usuario,CG supuesto al centro de referencia. Sin inercias internas de servos ni curva de freno. Uniformidad de envolventes no acredita masas/CG reales.',
    trajectories=len(labels),states=len(traj),rows=rows)
(H/'resultados_liviano.json').write_text(json.dumps(out,ensure_ascii=False,indent=2),encoding='utf-8')
E=H.parents[2]/'exports'/'analisis-actuadores-v13';E.mkdir(exist_ok=True)
np.savez_compressed(E/'liviano.npz',case_names=[x[0] for x in profiles],F=[x['F'] for x in raw],M=[x['M'] for x in raw],
    torque=[x['torque'] for x in raw],q_deg=[x['q'] for x in raw],F_moving=[x['F_moving'] for x in raw],M_origin_moving=[x['M_origin_moving'] for x in raw],
    time_s=[x[3] for x in traj],case_index=[x[4] for x in traj],labels=labels)
