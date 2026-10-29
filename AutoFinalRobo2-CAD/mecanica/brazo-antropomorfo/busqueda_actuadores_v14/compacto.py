"""Presupuesto compacto nuevo. No representa piezas CAD ni resistencia aprobada.

Solo ruedas, sin contrapeso ni traslado de electronica. Las reservas se cuentan
antes de elegir actuadores. Inercia interna y capacidad de frenado pendientes.
"""
from pathlib import Path
import json, math
import numpy as np
from modelo_dinamico import aggregate, bodies, rot

H = Path(__file__).resolve().parent

def trajectories():
    # 64 combinaciones simultaneas y 12 individuales; tres giros de J4/J6.
    # Todas terminan extendidas. No es una envolvente de todo el workspace.
    import itertools
    signs = list(itertools.product([-1, 1], repeat=6))
    signs += [tuple(s if j == i else 0 for j in range(6)) for i in range(6) for s in [-1, 1]]
    for a, b in itertools.product([-90, 0, 90], repeat=2):
        end = np.radians([0, 90, 0, a, 0, b])
        for v in signs:
            v = np.radians(v)*30
            for t in np.linspace(0, .05, 21):
                s=t/.05
                yield end-v*.025+v*.05*(s-s**3+.5*s**4), v*(1-3*s*s+2*s**3), v*(-6*s+6*s*s)/.05

def budget(L1=80, L2=70, reserve=False):
    pp=[]
    def add(root, name, mass, cg, box, material):
        pp.append(dict(root=root,name=name,mass_g=mass,cg_local_mm=cg,box_mm=box,material=material))
    def al(root,name,volume,cg,box):add(root,name,volume*.0027,cg,box,'Al6061')
    def steel(root,name,d,l,cg,axis='Y'):
        box=[d,l,d] if axis=='Y' else [d,d,l]
        add(root,name,math.pi*d*d*l/4*.00785,cg,box,'Acero')
    def servo(root,name,m,cg,box):add(root,name,m,cg,box,'Servo')
    # Base fija: se conserva una reserva de alimentacion independiente, sin
    # contar como gratuita la bateria ni reutilizar potencia del auto.
    servo('00','STS3215 J1',56,[48,-32,27],[45.23,24.73,36.5])
    add('00','Control y potencia TTL presupuesto',150,[-35,-32,25],[60,35,30],'Electronica')
    add('00','Bateria brazo presupuesto',130,[10,-60,25],[65,35,28],'Potencia')
    al('00','Montura ranurada superior 2mm',90*75*2*.7,[0,-32,8],[90,75,2])
    al('00','Contraplaca inferior unica 2mm',90*35*2*.8,[0,-32,-5],[90,35,2])
    add('00','Cartucho J1 y fijacion servo PLA',18,[0,-32,25],[50,45,35],'PLA')
    add('00','Par608 J1',24,[0,-32,30],[22,22,22],'Rodamiento')
    steel('01','Eje J1 8x35',8,35,[0,0,0],'Z')
    al('01','Plataforma anular J1 2mm',math.pi*(28**2-8**2)*2,[0,0,3],[56,56,2])
    # Dos mejillas de aluminio de 2mm con borde plegado. Se cuenta el borde;
    # sus propiedades resistentes NO se sustituyen por las de una placa plana.
    for root,L in [('02',L1),('03',L2)]:
        for side in [-1,1]:
            area=2*math.pi*14**2+12*max(0,L-28)-math.pi*8**2/2
            al(root,'Mejilla 2mm y borde 8mm '+str(side),2*area+2*8*max(0,L-28),[0,side*15,L/2],[28,10,L+28])
        od,di=(25,22) if root=='02' else (20,17)
        length=L-40
        add(root,'PVC propuesto OD'+str(od),math.pi*(od*od-di*di)/4*length*.00145,[0,0,L/2],[od,od,length],'PVC')
        add(root,'Dos encastres PLA abiertos',12,[0,0,L/2],[30,30,L],'PLA')
    servo('01','STS3215 J2',56,[0,46,20],[24.73,36.5,45.23])
    servo('02','STS3215 J3',56,[0,46,L1-15],[24.73,36.5,45.23])
    for root,z in [('02',0),('03',0)]:
        steel(root,'Eje 8x45',8,45,[0,0,z])
        add(root,'Par608',24,[0,0,z],[22,45,22],'Rodamiento')
    add('01','Horquilla hombro y fijaciones PLA',16,[0,0,20],[40,45,40],'PLA')
    # Reservas de transmisiones incluyen ruedas, apoyos auxiliares y tornillos.
    # No se descuenta masa al variar la relacion durante la optimizacion.
    for root,z,m in [('00',40,22),('01',25,25),('02',L1-5,22),('03',L2,12),('04',30,16)]:
        add(root,'Transmision reserva completa',m,[0,25,z],[45,25,40],'Transmision')
    servo('03','STS3032 J4',21.6,[8,0,L2-14],[32,12,27.5])
    servo('04','STS3032 J5',21.6,[0,27,19],[32,12,27.5])
    servo('05','STS3032 J6',21.6,[8,0,0],[32,12,27.5])
    for root,z in [('04',6),('05',0),('06',0)]:
        add(root,'Par625',10,[0,0,z],[16,28,16],'Rodamiento')
        steel(root,'Eje o dos munones equivalentes 5x25',5,25,[0,0,z])
        al(root,'Horquilla plegada 1.5mm',((30*20)+2*(30*15))*1.5,[0,0,z+10],[32,32,30])
        add(root,'Cubiertas y encastres PLA',6,[0,0,z+8],[35,35,25],'PLA')
    # Pinza pivotante: rango/rectitud de dedos y ventaja mecanica por resolver.
    servo('06','STS3032 pinza',21.6,[25,0,10],[32,12,27.5])
    al('06','Dos dedos 2x10x45',2*2*10*45,[0,0,35],[65,10,45])
    al('06','Palma 1.5mm con ventanas',60*30*1.5*.65,[0,0,9],[60,30,1.5])
    add('06','Bisagras cable reenvio fijaciones',10,[0,0,18],[60,30,20],'Varios')
    add('06','Pads y topes PLA',8,[0,0,38],[65,15,30],'PLA')
    add('01','Cableado conectores y tornilleria restante',25,[0,0,20],[35,35,40],'Varios')
    add('03','Cableado y tornilleria distal restante',12,[0,0,L2/2],[20,20,L2],'Varios')
    add('90','Carga cubo 50mm',200,[0,0,0],[50,50,50],'Carga')
    if reserve:
        for x in pp:
            if x['material'] not in ['Servo','Electronica','Potencia','Carga']:x['mass_g']*=1.25
    return pp

def calculate(L1,L2,reserve):
    pp=budget(L1,L2,reserve); groups=aggregate(pp)
    p=dict(base_top=45,tower_h=40,axis_y_mm=-32,link1=L1,link2=L2,wrist45=35,wrist56=30,payload_z=45)
    roots=['01','02','03','04','05','06'];loads=[];speeds=[];FM=[];MM=[];QQ=[];payload=[];force=[];moment=[];sections=[];acc=[]
    for q,dq,ddq in trajectories():
        f,bb=bodies(q,dq,ddq,p,groups);tt=[];ff=[];mm=[];ss=[]
        for j,r in enumerate(roots):
            desc=roots[j:]+['90'];F=sum((bb[x][0] for x in desc),np.zeros(3))
            M=sum((bb[x][1] for x in desc),np.zeros(3))-np.cross(f[r][:3,3],F)
            parent='00' if j==0 else roots[j-1];axis=f[parent][:3,2 if j in [0,3,5] else 1]
            tt.append(abs(M@axis)/.0980665);ff.append(np.linalg.norm(F));mm.append(np.linalg.norm(M))
            fl=f[r][:3,:3].T@F;ml=f[r][:3,:3].T@M
            ss.append([np.linalg.norm(fl[:2]),abs(fl[2]),np.linalg.norm(ml[:2]),abs(ml[2])])
        loads.append(tt);speeds.append(abs(np.degrees(dq)));QQ.append(np.degrees(q));force.append(ff);moment.append(mm)
        sections.append(ss);acc.append(abs(ddq))
        FM.append(sum((bb[x][0] for x in roots+['90']),np.zeros(3)))
        MM.append(sum((bb[x][1] for x in roots+['90']),np.zeros(3)))
        payload.append(np.linalg.norm(bb['90'][0]))
    loads=np.array(loads);speeds=np.array(speeds);QQ=np.array(QQ);FM=np.array(FM);MM=np.array(MM)
    nominal=[10,10,10,1.5,1.5,1.5];stall=[27,27,27,4.5,4.5,4.5];v0=[243,243,243,666.6667,666.6667,666.6667]
    axes=[]
    for j in range(6):
        choices=[]
        for n in np.arange(1,12.01,.25):
            eta=1 if n==1 else .8
            available=n*eta*np.minimum(nominal[j],stall[j]*np.maximum(0,1-speeds[:,j]*n/v0[j]))
            margin=float(np.min(available/np.maximum(loads[:,j],1e-12)))
            choices.append((margin,float(n)))
        # Preferir una transmision simple que supere 2.3 frente a maximizar el
        # par con relaciones innecesarias. No se reduce la reserva de masa.
        simple=[x for x in choices if x[1] in [1,1.5,2,2.5,3,4,5,6,8,10] and x[0]>=2.3]
        margin,n=min(simple,key=lambda x:x[1]) if simple else max(choices)
        axes.append(dict(axis='J'+str(j+1),proposed_ratio=n,minimum_external_torque_margin=margin,peak_output_kgfcm=float(loads[:,j].max()),selected_by='smallest simple ratio >=2.3, else best external margin'))
        eta=1 if n==1 else .8
        available=n*eta*np.minimum(nominal[j],stall[j]*np.maximum(0,1-speeds[:,j]*n/v0[j]))
        alpha=np.array(acc)[:,j]
        # Condicion necesaria: solo rotor equivalente sobre el eje del servo.
        # No incorpora sus reacciones sobre los ejes anteriores.
        limit=(available/2-loads[:,j])*.0980665/(n*n*eta)
        axes[-1]['internal_inertia_necessary_max_kgm2']=float(np.min(np.divide(limit,alpha,out=np.full_like(alpha,np.inf),where=alpha>1e-8)))
    fixed=groups['00'];Ffix=np.array([0,0,fixed[0]*9.80665]);Mfix=np.cross(fixed[1],Ffix)
    point=np.array([0,-.032,.045]);ground=np.array([0,-.032,-.08]);need=np.zeros(2)
    for yaw in range(-90,91,10):
        valid=(QQ[:,0]+yaw>=-90)&(QQ[:,0]+yaw<=90);R=rot('Z',math.radians(yaw))[:3,:3]
        Fa=FM[valid]@R.T;Ma=(MM[valid]-np.cross(point,FM[valid]))@R.T+np.cross(point,Fa)
        F=Fa+Ffix;M=Ma+Mfix-np.cross(ground,F);F=F+[0,0,1.5*9.80665]
        cop=np.c_[-M[:,1]/F[:,2],M[:,0]/F[:,2]]
        need=np.maximum(need,np.max(abs(cop),axis=0)*1000)
    row=dict(parameters_mm=p,reserve25=reserve,total_with_payload_g=sum(x['mass_g'] for x in pp),arm_without_payload_g=sum(x['mass_g'] for x in pp)-200,
             motor_mass_g=sum(x['mass_g'] for x in pp if x['material']=='Servo'),axes=axes,necessary_support_halfspan_mm=need.tolist(),
             tip_utilization_90_50_hypothesis=float(max(need/[90,50])),peak_force_N=np.max(force,axis=0).tolist(),peak_moment_Nm=np.max(moment,axis=0).tolist(),peak_payload_force_N=max(payload),
             section_peak_columns=['shear_N','axial_N','bending_Nm','torsion_Nm'],section_peaks_local=np.max(sections,axis=0).tolist(),
             parts=pp,groups=[dict(root=r,mass_g=m*1000,cg_local_mm=(cg*1000).tolist(),inertia_kgm2=I.tolist()) for r,(m,cg,I) in groups.items()])
    return row

if __name__=='__main__':
    rows=[]
    for L1,L2,reserve in [(80,70,False),(80,70,True),(65,60,True),(55,50,True)]:
        r=calculate(L1,L2,reserve);rows.append(r)
        print(L1,L2,reserve,round(r['arm_without_payload_g']),[round(a['minimum_external_torque_margin'],2) for a in r['axes']],r['necessary_support_halfspan_mm'],flush=True)
    (H/'compacto_resultados.json').write_text(json.dumps(dict(scope='Presupuesto nuevo; filtro externo de cuerpos rigidos, sin CAD, inercia interna, curva de frenado, resistencia ni apoyo medido acreditados.',rows=rows),ensure_ascii=False,indent=2),encoding='utf-8')
