"""ANTECEDENTE: casos con pies descartados por decision humana.

Par NOMINAL en la estructura liviana, antes de nuevos housings.

Compara presupuestos de reductores y CG; no hace valido un reductor sin
dimensionar ni acredita inercia interna o disponibilidad de compra.
"""
from pathlib import Path
import json,math,copy
import numpy as np
from modelo_dinamico import frames,bodies,aggregate
H=Path(__file__).resolve().parent;V13=H.parent/'busqueda_actuadores_v13'
base=json.loads((V13/'candidato_compacto_componentes.json').read_text(encoding='utf-8'))
src=np.load(H.parents[2]/'exports'/'analisis-actuadores-v11'/'cargas_comparadas.npz')
q0,t0,cids,labels=src['q_deg'],src['time_s'],src['case_index'],src['source_trajectory_labels']
traj=[];names=[]
for cid in np.unique(cids):
    label=str(labels[cid])
    if not label.endswith('_50ms'):continue
    ii=np.flatnonzero(cids==cid);end=np.radians(q0[ii[np.argmax(t0[ii])]])
    delta=q0[ii[np.argmax(t0[ii])]]-q0[ii[np.argmin(t0[ii])]]
    velocity=np.radians(np.sign(np.where(np.abs(delta)<1e-8,0,delta))*30);k=len(names);names.append(label)
    for t in np.linspace(0,.05,21):
        s=t/.05
        traj.append((end-velocity*.025+velocity*.05*(s-s**3+.5*s**4),velocity*(1-3*s*s+2*s**3),velocity*(-6*s+6*s*s)/.05,t,k))

def components(reserve=False,cg_shift=False,j3small=False,j5ratio=3):
    parts=copy.deepcopy(base['parts'])
    # Todos los casos incluyen la masa de la alternativa de pies aun no
    # aceptada. El dato no se usa para afirmar que el usuario la eligio.
    for x in parts:
        n=x['name'];root=x['root']
        if x['material']=='Servo':
            x.pop('tensor_kg_m2',None)
            if root in ['00','04']:
                x.update(name='STS3215_C047_'+root,mass_g=56,box_mm=[24.73,36.5,45.23] if root=='04' else [24.73,45.23,36.5])
                x['cg_local_mm']=[-86.2,-44.5,54] if root=='00' else [0,51,22.5]
            elif root in ['01','02']:
                x.update(name=('STS3215_C047_' if j3small and root=='02' else 'STS3250_')+root,mass_g=56 if j3small and root=='02' else 75.5,box_mm=[24.72,36.5,45.22])
                x['cg_local_mm']=[-35,60,27.5] if root=='01' else [0,60,53.5]
            elif root in ['03','05']:
                x.update(name='STS3032_'+root,mass_g=21.6,box_mm=[32,12,27.5])
                x['cg_local_mm']=[31.5,0,59] if root=='03' else [7.5,0,49]
            else:
                # Los dos cuerpos de motor de la pinza nativa se reemplazan
                # mas abajo por un motor unico; no se duplica masa comercial.
                x['mass_g']=0
            if cg_shift and x['root']!='00':x['cg_local_mm'][2]+=10
        if n=='Carcasa base con ventanas':x.update(mass_g=16,cg_local_mm=[-86.2,-44.5,54],box_mm=[29,50,41])
        if n=='Housing hombro aligerado':x.update(mass_g=16,cg_local_mm=[-35,60,27.5],box_mm=[29,41,50])
        if n=='Housing codo con ventanas':x.update(mass_g=16,cg_local_mm=[0,60,53.5],box_mm=[29,41,50])
        if n=='Housing J5 ventanas':x.update(mass_g=15,cg_local_mm=[0,51,22.5],box_mm=[29,41,50])
        if n=='Housing J4 con ventanas':x.update(mass_g=7,cg_local_mm=[31.5,0,59],box_mm=[36,16,32])
        if n=='Housing J6 abierto':x.update(mass_g=7,cg_local_mm=[7.5,0,49],box_mm=[36,16,32])
        if x['material']=='Servo' and root=='04':x['cg_local_mm'][2]=37.5-5*j5ratio+(10 if cg_shift else 0)
        if n=='Housing J5 ventanas':x['cg_local_mm'][2]=37.5-5*j5ratio
        if n=='Eje yaw acero8mm':
            x['mass_g']*=55/40;x['box_mm'][2]=55;x['cg_local_mm'][2]=-17.5
        if n=='Hub integral brida yaw':
            x['mass_g']*=.5;x['box_mm'][2]=6;x['cg_local_mm'][2]=-2.5
        if n=='Plataforma PLA abierta':x['cg_local_mm'][2]=5
        if n=='Portal y portarrodamientos Al':x.update(mass_g=45,box_mm=[86,35,45],cg_local_mm=[0,-32,32])
        if x['material']=='Potencia':x.update(mass_g=130,cg_local_mm=[-38,-100,50],box_mm=[65,35,28])
        # Las masas completas nuevas de reductor reemplazan las antiguas,
        # no se suman encima como si se conservaran dos cajas de engranes.
        if n in ['Dos ruedas44T yaw1a1 Al','Reductor hombro1.9a1 y guardas','Reductor codo1.4a1 y fijaciones','Rueda J4 34T','Soporte y pinon J4 desplazado']:
            x['mass_g']=0
        if n=='Reserva cuatro pies plegables y anclajes':x['mass_g']=350
        if n.startswith('Contrabarra inferior'):x['mass_g']=0
    parts=[x for x in parts if x['mass_g']>0]
    def add(root,name,m,cg,box,mat='Al6061'):
        parts.append(dict(root=root,name=name,mass_g=m,cg_local_mm=cg,box_mm=box,material=mat))
    add('00','Contraplaca inferior unica150x60x2 ventana130x40',20.52,[0,-32,-4],[150,60,2])
    add('00','Metrica de traslado y soporte J1',20,[-70,-32,25],[50,40,4])
    add('01','Transmision yaw5a1',50,[-25,0,-33],[150,70,12])
    add('01','Reductor hombro8-9a1',90,[-15,34,44],[72,26,65])
    add('02','Reductor codo6a1',70,[10,34,80],[50,26,58])
    add('03','Accionamiento J4 3a1',17,[24,0,73],[36,25,32])
    add('04','Rueda salida J4 60T',5,[0,0,0],[38,38,5])
    add('04','Reductor J5 3-4a1',45,[0,28,45],[38,15,45])
    add('06','STS3032_pinza',21.6,[31.5,0,24],[32,12,27.5],'Servo')
    add('06','Reductor y soporte pinza3a1',18,[12,0,44],[50,35,15])
    # 12V regulados +6V para micros y transceptorTTL:150g presupuestados,
    # no quitar la bateria90g ya incluida ni declarar potencia cerrada.
    for x in parts:
        if x['material']=='Electronica':x.update(mass_g=150,name='Control y potencia TTL 12V6V propuestos',cg_local_mm=[50,-82,50],box_mm=[60,30,25])
        if reserve and x['material'] not in ['Servo','Carga','Electronica','Potencia','Cables']:
            x['mass_g']*=1.25
            if 'tensor_kg_m2' in x:x['tensor_kg_m2']=(np.array(x['tensor_kg_m2'])*1.25).tolist()
    return parts

profiles=[('TTL_8_6_3_eta85',[5,8,6,3,3,1],.85,False,False),('TTL_8_6_3_eta70',[5,8,6,3,3,1],.70,False,False),
    ('TTL_9_6_4_eta80',[5,9,6,3,4,1],.80,False,False),('TTL_9_6_4_reserva25_CG10',[5,9,6,3,4,1],.80,True,True),
    ('TTL_1x3250_9_7_4',[5,9,7,3,4,1],.80,False,False),('TTL_1x3250_9_7_4_reserva',[5,9,7,3,4,1],.80,True,True)]
if __name__ == '__main__':
    rows=[]
    for name,rr,eff,reserve,cgshift in profiles:
        p=base['parameters_mm'];small='1x3250' in name;parts=components(reserve,cgshift,small,rr[4]);groups=aggregate(parts)
        ratios=np.array(rr);eta=np.array([eff,eff,eff,eff,eff,1]);nominal=np.array([10,16,16,1.5,10,1.5])
        # Limite nominal + recta conservadora de velocidad; NO convierte esa
        # recta de motorizacion en curva validada de frenado regenerativo.
        stall=np.array([27,45,45,4.5,27,4.5]);speed=np.array([243,405,405,600/.9,243,600/.9])
        if small:nominal[2]=10;stall[2]=27;speed[2]=243
        demand=[];available=[];forces=[];moments=[];q_all=[];payload=[];allF=[];allM=[]
        supported=['01','02','03','04','05','06']
        for q,dq,ddq,t,k in traj:
            ff,bb=bodies(q,dq,ddq,p,groups);tor=[];jf=[];jm=[]
            for j,root in enumerate(supported):
                desc=supported[j:]+['90'];F=sum((bb[x][0] for x in desc),np.zeros(3));M=sum((bb[x][1] for x in desc),np.zeros(3))-np.cross(ff[root][:3,3],F)
                parent='00' if j==0 else supported[j-1];axis=ff[parent][:3,2 if j in [0,3,5] else 1]
                tor.append(abs(M@axis)/(.0980665*ratios[j]*eta[j]));jf.append(np.linalg.norm(F));jm.append(np.linalg.norm(M))
            demand.append(tor);available.append(np.minimum(nominal,stall*np.maximum(0,1-np.abs(np.degrees(dq))*ratios/speed)))
            forces.append(jf);moments.append(jm);q_all.append(q);payload.append(np.linalg.norm(bb['90'][0]))
            allF.append(sum((bb[x][0] for x in supported+['90']),np.zeros(3)));allM.append(sum((bb[x][1] for x in supported+['90']),np.zeros(3)))
        demand=np.array(demand);available=np.array(available);margin=available/np.maximum(demand,1e-12)
        # Umbral NECESARIO solo para aceleracion propia del rotor equivalente
        # referido al spline del servo. Faltan reacciones a padres y giroscopia.
        acc=np.abs(np.array([x[2] for x in traj]))*ratios
        headroom=(available/2-demand)*.0980665
        inertia_threshold=np.min(np.divide(np.maximum(0,headroom),acc,out=np.full_like(acc,np.inf),where=acc>1e-9),axis=0)
        # Retencion estatica horizontal, tres orientaciones de roll de muñeca.
        static=[]
        for roll in [-90,0,90]:
            q=np.radians([0,90,0,roll,0,0]);ff,bb=bodies(q,np.zeros(6),np.zeros(6),p,groups);tor=[]
            for j,root in enumerate(supported):
                desc=supported[j:]+['90'];F=sum((bb[x][0] for x in desc),np.zeros(3));M=sum((bb[x][1] for x in desc),np.zeros(3))-np.cross(ff[root][:3,3],F)
                parent='00' if j==0 else supported[j-1];axis=ff[parent][:3,2 if j in [0,3,5] else 1]
                tor.append(abs(M@axis)/(.0980665*ratios[j]*eta[j]))
            static.append(tor)
        row=dict(name=name,states=len(traj),trajectories=len(names),mass_with_payload_g=sum(x['mass_g'] for x in parts),
            motor_total_g=sum(x['mass_g'] for x in parts if x['material']=='Servo'),ratio=rr,eta=eta.tolist(),rated_kgfcm=nominal.tolist(),
            motor_peak_demand_kgfcm=demand.max(axis=0).tolist(),minimum_margin=np.min(margin,axis=0).tolist(),
            holding_static_motor_kgfcm=np.max(static,axis=0).tolist(),peak_force_N=np.max(forces,axis=0).tolist(),peak_moment_Nm=np.max(moments,axis=0).tolist(),
            rotor_equivalent_inertia_necessary_max_kgm2=inertia_threshold.tolist(),
            pinza_normal_budget_rated_N=1.5*3*eff*.7*.0980665/.012,pinza_pad_mu_required_n2=2*max(payload)/(1.5*3*eff*.7*.0980665/.012),
            hardware_status='CG de cajas propuestos con offset12.5mm de planos, no medidos. Reductores completos presupuestados. Inercia interna no incluida. Pies350g propuestos, pendientes de eleccion humana y mecanica.')
        rows.append(row)
        (H/(name+'_componentes.json')).write_text(json.dumps(dict(parameters_mm=p,parts=parts),ensure_ascii=False,indent=2),encoding='utf-8')
        E=H.parents[2]/'exports'/'analisis-actuadores-v14';E.mkdir(exist_ok=True)
        np.savez_compressed(E/(name+'.npz'),q_deg=np.degrees(q_all),F_moving=allF,M_origin_moving=allM,time_s=[x[3] for x in traj],demand_kgfcm=demand,available_kgfcm=available)
        print(name,round(row['mass_with_payload_g']),'g; nmin',round(min(row['minimum_margin']),3),'motores',row['motor_total_g'],flush=True)
    (H/'nominal_resultados.json').write_text(json.dumps(dict(scope='Filtro nominal con estructura liviana y nuevas reservas. No aprueba oferta argentina, freno ni inercia interna. Casos no son diseños CAD.',rows=rows),ensure_ascii=False,indent=2),encoding='utf-8')
