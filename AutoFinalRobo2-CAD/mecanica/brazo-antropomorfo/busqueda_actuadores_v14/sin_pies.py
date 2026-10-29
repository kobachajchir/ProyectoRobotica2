"""Nueva arquitectura SOLO ruedas, tras la respuesta humana.

Importacion admitida. Reserva de pies eliminada. Control y bateria cumplen
funcion de contrapeso en la base giratoria; no se agrega lastre.
Las piezas son presupuestos geometricos nuevos, no CAD comprobado.
"""
from pathlib import Path
import json,math,copy
import numpy as np
from calcular_nominal import components,traj,names
from modelo_dinamico import frames,bodies,aggregate,rot
H=Path(__file__).resolve().parent

def cheek_area(L,R=17.5,w=18):
    a=w/2;h=math.sqrt(R*R-a*a)
    overlap=w*h+R*R*math.pi/2-(h*a+R*R*math.asin(h/R))
    return 2*math.pi*R*R+w*L-2*overlap

def build(L1,L2,reserve=False,cgshift=False,counterweight=True):
    pp=components(False,False,True,4);parts=[]
    for x in pp:
        n=x['name'];root=x['root']
        if root=='06' or 'pies' in n.lower():continue
        if any(v in n for v in ['Puente proximal','Placa codo','Placa J4','Manguito PVC','Accionamiento J4','Rueda salida J4']):continue
        if root in ['02','03']:
            old,new=(90,L1) if root=='02' else (75,L2);zz=x['cg_local_mm'][2]
            x['cg_local_mm'][2]+=new-old if zz>old/2+1e-7 else ((new-old)/2 if abs(zz-old/2)<1e-7 else 0)
            if x['material']=='PVC':
                length=new-30 if root=='02' else new-46
                x['mass_g']*=length/(old-30);x['box_mm'][2]=length;x['cg_local_mm'][2]=(15+(new-15 if root=='02' else new-31))/2
        if n=='STS3250_01':x.update(name='STS3215_C047_01',mass_g=56)
        if n in ['STS3215_C047_01','Housing hombro aligerado']:x['cg_local_mm'][0]=-42
        if n=='STS3215_C047_00' or n=='Carcasa base con ventanas':x['cg_local_mm'][0]=86.2
        if n=='Metrica de traslado y soporte J1':x['cg_local_mm'][0]=70
        if n=='Transmision yaw5a1':x['cg_local_mm'][0]=25
        if n=='Montura superior simple':x['cg_local_mm'][1]=-32
        if n=='Eje hombro acero8mm':x['mass_g']*=60/46;x['box_mm'][1]=60
        if n=='Par608 hombro':x['box_mm'][1]=51
        if n=='Plataforma PLA abierta' and counterweight:
            # Union aproximada: placa110x75 con semianilloD62, pasoD40 y
            # dos ventanas30x20. Se sustituye el anilloPLA, no se duplican.
            area=110*75+.5*math.pi*31**2-math.pi*20**2-2*30*20
            x.update(name='Plataforma y brazo contrapeso Al integral',material='Al6061',mass_g=area*2*.0027,cg_local_mm=[-45,0,5.5],box_mm=[141,75,2])
        if n=='Brida yaw aluminio' and counterweight:x['mass_g']*=4/3;x['box_mm'][2]=4;x['cg_local_mm'][2]=2.5
        if n.startswith('Control y potencia TTL') and counterweight:x.update(root='01',cg_local_mm=[-70,20,-10],box_mm=[60,30,25])
        if x['material']=='Potencia' and counterweight:x.update(root='01',cg_local_mm=[-70,-20,-10.5],box_mm=[65,35,28])
        if n=='STS3032_03':x.update(name='STS3032_J4_directo',cg_local_mm=[7.5,0,L2-16],box_mm=[32,12,27.5])
        if n=='Housing J4 con ventanas':x.update(mass_g=8,cg_local_mm=[7.5,0,L2-16],box_mm=[36,16,32])
        if n=='Par626 J4':x.update(name='Par625 J4',mass_g=10,cg_local_mm=[0,0,L2+13.5],box_mm=[16,16,12])
        if n=='Eje J4 acero6':x.update(name='Eje J4 acero5',mass_g=math.pi*25/4*28*.00785,cg_local_mm=[0,0,17.5],box_mm=[5,5,28])
        if n=='Travesano horquilla J5':x['cg_local_mm'][2]=24.75
        if n=='STS3215_C047_04':x.update(name='STS3032_J5',mass_g=21.6,cg_local_mm=[-20,53.75,37.5],box_mm=[12,27.5,32])
        if n=='Housing J5 ventanas':x.update(mass_g=8,cg_local_mm=[-20,53.75,37.5],box_mm=[16,32,36])
        if root=='05':
            if n=='Eje J5 acero5':continue
            if n in ['STS3032_05','Housing J6 abierto']:x['cg_local_mm']=[7.5,0,5.25]
            if n=='Par625 J6':x['cg_local_mm'][2]=34.75
        parts.append(x)
    def add(root,name,m,cg,box,material='Al6061'):
        parts.append(dict(root=root,name=name,mass_g=m,cg_local_mm=cg,box_mm=box,material=material))
    for root,L,y,dist_hole in [('02',L1,15,20),('03',L2,13,6)]:
        area=cheek_area(L)-math.pi*(8**2+dist_hole**2)/4-4*math.pi*3.4**2/4
        for sign in [-1,1]:add(root,'Mejilla Al continua'+str(sign),area*3*.0027,[0,sign*y,L/2],[35,3,L+35])
        add(root,'Cubierta y separadores no estructurales',8,[0,0,L/2],[28,28,max(10,L-30)],'PLA')
    add('03','Cartucho y apoyo J4 en Al',20,[0,0,L2+10],[30,30,27])
    add('04','Acople y brida J4 Al',8,[0,0,10],[30,30,27])
    add('05','Trunnion J5 izquierdo acero5',math.pi*25/4*12*.00785,[0,-18,0],[5,12,5],'Acero')
    add('05','Trunnion J5 derecho acero5',math.pi*25/4*18*.00785,[0,21,0],[5,18,5],'Acero')
    if counterweight:add('01','Cunas colgantes control y bateria',8,[-70,0,-8],[70,75,30],'PLA')
    # Pinza metalica nueva: centro del cuboZ52; base y accionamiento estan
    # por detras del cubo (su cara proxima quedaZ27).
    add('06','Palma Al70x70x2 con ventanas',((70*70-2*20*12)*2)*.0027,[0,0,15],[70,70,2])
    add('06','Puente J6 con escotadura portaeje',10.5,[0,0,5.5],[45,70,2])
    add('06','Separadores y fijaciones palma',7,[0,0,10],[40,60,12])
    add('06','STS3032_pinza',21.6,[67.5,0,26.75],[32,12,27.5],'Servo')
    add('06','Dos engranes6a1 y fijaciones',18,[30,0,10],[85,64,5])
    add('06','Pinon20T modulo0.8',5,[25,0,21.5],[18,18,5])
    add('06','Dos cremalleras Al modulo0.8',2*70*6*5*.0027,[0,0,21.5],[70,32,5])
    add('06','Dos ejes guia3x80',2*math.pi*9/4*80*.00785,[0,0,20],[80,22,3],'Acero')
    add('06','Cuatro LM3UU presupuesto2.5g',10,[0,0,20],[70,30,10],'Rodamiento')
    add('06','Dos625 eje pinza',10,[25,0,9.25],[16,16,19],'Rodamiento')
    add('06','Eje pinza acero5x30',math.pi*25/4*30*.00785,[25,0,12],[5,5,30],'Acero')
    for sign in [-1,1]:add('06','Dedo Al'+str(sign),6,[sign*25,0,52],[3,12,55])
    add('06','Pads y muelles guiados de precarga',8,[0,0,45],[64,14,30],'Varios')
    add('06','GuardasPLA y soportes auxiliares',7,[25,0,15],[75,60,20],'PLA')
    for x in parts:
        if cgshift and x['root'] not in ['00','01','90']:x['cg_local_mm'][2]+=5
        if reserve and x['material'] not in ['Servo','Carga','Potencia','Electronica','Cables']:
            x['mass_g']*=1.25
            if 'tensor_kg_m2' in x:x['tensor_kg_m2']=(np.array(x['tensor_kg_m2'])*1.25).tolist()
    return parts

profiles=[('L70_65_equilibrado',70,65,True,False),('L60_60_equilibrado',60,60,True,False),('L50_55_equilibrado',50,55,True,False),
    ('L60_60_sin_contrapeso_funcional',60,60,False,False),('L50_55_reserva25_CG5',50,55,True,True)]
rows=[];E=H.parents[2]/'exports'/'analisis-actuadores-v14';E.mkdir(exist_ok=True)
for name,L1,L2,balance,reserve in profiles:
    pp=build(L1,L2,reserve,reserve,balance);p=dict(base_top=60,tower_h=40,axis_y_mm=-32,link1=L1,link2=L2,wrist45=55,wrist56=40,payload_z=52)
    groups=aggregate(pp);supported=['01','02','03','04','05','06'];rr=np.array([5,7,4,1,8,1]);eta=np.array([.8,.8,.8,1,.8,1])
    nominal=np.array([10,10,10,1.5,1.5,1.5]);stall=np.array([27,27,27,4.5,4.5,4.5]);speed=np.array([243,243,243,666.6667,666.6667,666.6667])
    torques=[];limits=[];Fs=[];Ms=[];Fmov=[];Mmov=[];qs=[];payload=[]
    for q,dq,ddq,t,k in traj:
        f,bb=bodies(q,dq,ddq,p,groups);tt=[];jF=[];jM=[]
        for j,root in enumerate(supported):
            desc=supported[j:]+['90'];F=sum((bb[x][0] for x in desc),np.zeros(3));M=sum((bb[x][1] for x in desc),np.zeros(3))-np.cross(f[root][:3,3],F)
            parent='00' if j==0 else supported[j-1];axis=f[parent][:3,2 if j in [0,3,5] else 1]
            tt.append(abs(M@axis)/(.0980665*rr[j]*eta[j]));jF.append(np.linalg.norm(F));jM.append(np.linalg.norm(M))
        torques.append(tt);Fs.append(jF);Ms.append(jM);qs.append(np.degrees(q));payload.append(np.linalg.norm(bb['90'][0]))
        limits.append(np.minimum(nominal,stall*np.maximum(0,1-np.abs(np.degrees(dq))*rr/speed)))
        Fmov.append(sum((bb[x][0] for x in supported+['90']),np.zeros(3)));Mmov.append(sum((bb[x][1] for x in supported+['90']),np.zeros(3)))
    torques=np.array(torques);limits=np.array(limits);qs=np.array(qs);Fmov=np.array(Fmov);Mmov=np.array(Mmov)
    fixed=groups['00'];Ffix=np.array([0,0,fixed[0]*9.80665]);Mfix=np.cross(fixed[1],Ffix);point=np.array([0,-.032,.06]);ground=np.array([0,-.032,-.08]);supports=[]
    for bx,by in [(107,62),(90,50),(90,40)]:
        worst=0.;states=0;need_xy=np.zeros(2);mu_req=0.
        for yaw in range(-90,91,10):
            valid=(qs[:,0]+yaw>=-90)&(qs[:,0]+yaw<=90);R=rot('Z',math.radians(yaw))[:3,:3]
            Fa=Fmov[valid]@R.T;Ma=(Mmov[valid]-np.cross(point,Fmov[valid]))@R.T+np.cross(point,Fa)
            F=Fa+Ffix;M=Ma+Mfix-np.cross(ground,F);F=F+[0,0,1.5*9.80665]
            cop=np.c_[-M[:,1]/F[:,2],M[:,0]/F[:,2]];need_xy=np.maximum(need_xy,np.max(np.abs(cop),axis=0)*1000)
            worst=max(worst,float(np.max(np.abs(cop)/[bx*.001,by*.001])));states+=len(F)
            mu_req=max(mu_req,float(np.max(np.linalg.norm(F[:,:2],axis=1)/F[:,2])))
        supports.append(dict(hypothetical_halfspan_mm=[bx,by],maximum_tip_utilization=worst,necessary_halfspan_mm=need_xy.tolist(),states=states,isotropic_mu_necessary_only=mu_req))
    acc=np.abs(np.array([x[2] for x in traj]))*rr
    residual=(limits/2-torques)*.0980665
    inertia=np.min(np.divide(np.maximum(0,residual),acc,out=np.full_like(acc,np.inf),where=acc>1e-9),axis=0)
    row=dict(name=name,parameters_mm=p,total_with_payload_g=sum(x['mass_g'] for x in pp),motor_mass_g=sum(x['mass_g'] for x in pp if x['material']=='Servo'),
        ratios=rr.tolist(),rated_kgfcm=nominal.tolist(),minimum_torque_margin=np.min(limits/np.maximum(torques,1e-12),axis=0).tolist(),peak_motor_kgfcm=torques.max(axis=0).tolist(),
        peak_force_N=np.max(Fs,axis=0).tolist(),peak_moment_Nm=np.max(Ms,axis=0).tolist(),rotor_inertia_necessary_max_kgm2=inertia.tolist(),
        ground_support=supports,peak_payload_force_N=max(payload),pinza_nominal_normal_N=1.5*6*.8*.7*.0980665/.008,
        pinza_closure_margin_for_30N_preload=(1.5*6*.8*.0980665)/(30*.008/.7),pad_mu_required_for_30N_preload_factor2=2*max(payload)/30,
        scope='Solo ruedas. Poligonos son hipotesis, no contactos medidos. Masa/CG/geometria nuevos propuestos; cuerpos rigidos, sin inercias internas ni choques completos.')
    rows.append(row);(H/(name+'_componentes.json')).write_text(json.dumps(dict(parameters_mm=p,parts=pp),ensure_ascii=False,indent=2),encoding='utf-8')
    np.savez_compressed(E/(name+'.npz'),q_deg=qs,F_moving=Fmov,M_origin_moving=Mmov,torques=torques,limits=limits)
    print(name,round(row['total_with_payload_g']),'g nmin',round(min(row['minimum_torque_margin']),3),'tip',[round(s['maximum_tip_utilization'],3) for s in supports],flush=True)
(H/'sin_pies_resultados.json').write_text(json.dumps(dict(scope='Actuadores nominales e importacion admitida; solo ruedas, hipotesis de apoyo diferenciadas. Sin CAD ni aprobacion fisica.',rows=rows),ensure_ascii=False,indent=2),encoding='utf-8')
