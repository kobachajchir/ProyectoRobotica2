"""Modelo reproducible de masas y cargas de la v06. No es FEA ni ensayo fisico."""
from pathlib import Path
import csv,json,math
from collections import defaultdict
import numpy as np

HERE=Path(__file__).resolve().parent
RAW=json.loads((HERE/'geometria_v06.json').read_text(encoding='utf-8'))
BEARINGS={x['bearing'] for x in json.loads((HERE.parent/'revision_v06'/'continuidad_y_rodamientos.json').read_text(encoding='utf-8'))['bearings']}
G=9.80665
P={k:v['value_internal'] for k,v in RAW['parameters'].items()}
ROOTS=['00_Base_control','01_Giro_base','02_Brazo_PVC','03_Antebrazo_PVC','04_Giro_muneca','05_Inclinacion_muneca','06_Herramienta','07_Mandibula_izquierda','08_Mandibula_derecha','09_Pinion_hombro','10_Pinion_pinza','11_Pinion_J4','90_Pieza_REFERENCIA_50mm_100g']
MOTOR_MASS={'V03_M01_MG946_Base':55,'V02_M02_GX3240_270':100,'V02_M03_DS3218_Codo':60,'V02_M04_MG90S_J4':13.4,'V02_M05_MG946_J5':55,'V02_M06_MG90S_J6':13.4,'V02_M07_MG90S_Pinza':13.4}
ELECTRONIC_MASS={'V03_XL4016_vertical_1':50,'V03_XL4016_vertical_2':50,'V03_XL4016_vertical_3':50,'V03_Nano_vertical':7,'V03_PCA9685_vertical':12,'V03_Bornera_y_alimentacion_REF':8}
NAMES={'00_Base_control':'Montura/control fijo','01_Giro_base':'Torre y giro J1','02_Brazo_PVC':'Eslabon 1 + motor del codo','03_Antebrazo_PVC':'Eslabon 2 + motor J4','04_Giro_muneca':'Giro J4 + horquilla/motor J5','05_Inclinacion_muneca':'Nucleo J5 + motor J6','06_Herramienta':'Palma + servo pinza','07_Mandibula_izquierda':'Mordaza izquierda','08_Mandibula_derecha':'Mordaza derecha','09_Pinion_hombro':'Pinon y horn del hombro','10_Pinion_pinza':'Pinon y horn de pinza','11_Pinion_J4':'Pinon y horn de J4','90_Pieza_REFERENCIA_50mm_100g':'Carga util 200 g'}

def tr(x=0,y=0,z=0):
    a=np.eye(4);a[:3,3]=[x,y,z];return a
def rot(ax,deg):
    a=np.eye(4);c=math.cos(math.radians(deg));s=math.sin(math.radians(deg))
    if ax=='Y':a[:3,:3]=[[c,0,s],[0,1,0],[-s,0,c]]
    else:a[:3,:3]=[[c,-s,0],[s,c,0],[0,0,1]]
    return a
def fk(q):
    q1,q2,q3,q4,q5,q6,g=q
    # All geometry and matrices below use millimetres, matching exported centroids.
    p={k:v*10 for k,v in P.items()};a=tr(y=p['v05_axis_y'],z=p['base_top'])@rot('Z',q1)
    ff={'00_Base_control':np.eye(4),'01_Giro_base':a}
    a=a@tr(z=p['tower_h'])@rot('Y',q2);ff['02_Brazo_PVC']=a
    a=a@tr(z=p['link1'])@rot('Y',q3);ff['03_Antebrazo_PVC']=a
    a=a@tr(z=p['link2'])@rot('Z',q4);ff['04_Giro_muneca']=a
    a=a@tr(z=p['wrist45'])@rot('Y',q5);ff['05_Inclinacion_muneca']=a
    a=a@tr(z=p['wrist56'])@rot('Z',q6);ff['06_Herramienta']=a
    gap=p['grip_min']+2*p['grip_r']*math.radians(g)
    ff['07_Mandibula_izquierda']=a@tr(x=-gap/2-p['finger_t']/2-p['pad_t'])
    ff['08_Mandibula_derecha']=a@tr(x=gap/2+p['finger_t']/2+p['pad_t'])
    ff['10_Pinion_pinza']=a@tr(z=p['grip_deck']+8)@rot('Z',g)
    ff['90_Pieza_REFERENCIA_50mm_100g']=a@tr(z=p['grip_deck']+52)
    ff['11_Pinion_J4']=ff['03_Antebrazo_PVC']@tr(36,0,p['link2']+6)@rot('Z',7.5-q4)
    ff['09_Pinion_hombro']=ff['01_Giro_base']@tr(z=p['tower_h']-p['gear_centers'])@rot('Y',math.degrees(P['shoulder_phase'])-2*q2)
    return ff

def physical_parts(q,printed_fraction=1,payload_g=200,extra_cable_g=0,tcp_payload=False):
    ff=fk(q);out=[];vols=defaultdict(float)
    for b in RAW['parts']:vols[b['component']]+=b['volume_cm3']
    roots_native={k:np.array(v).reshape(4,4) for k,v in RAW['root_transforms_cm'].items()}
    # Children of all arm groups are grounded at identity in this design. Derive
    # their transform explicitly rather than silently relying on that convention.
    for b in RAW['parts']:
        root=b['root'];cn=b['component'];v=b['volume_cm3'];ap=b['appearance']
        if root not in ff:continue
        rel=np.linalg.inv(roots_native[root])@np.array(b['component_to_root_cm']).reshape(4,4);rel[:3,3]*=10
        pt=(ff[root]@rel@np.r_[b['centroid_component_mm'],1])[:3]
        if cn in MOTOR_MASS:
            mass=MOTOR_MASS[cn]*v/vols[cn];cat='Servo';basis='masa de ficha; GX=100g supuesto conservador pendiente de pesar'
        elif cn in ELECTRONIC_MASS:
            mass=ELECTRONIC_MASS[cn]*v/vols[cn];cat='Electronica fija estimada';basis='supuesto de masa de modulo, centro de envolvente CAD'
        elif cn in BEARINGS:
            mass=(12.5 if '608' in cn else 5)*v/vols[cn];cat='Rodamiento';basis='masa SKF completa; no densidad de aros simplificados'
        elif root.startswith('90_'):
            mass=payload_g;cat='Carga util';basis='requisito de carga, independiente del material CAD'
            if tcp_payload:pt=(ff['06_Herramienta']@np.array([0,0,P['tcp_offset']*10,1]))[:3]
        elif ap=='AFR2_PETG_Rojo':
            mass=1.27*v*printed_fraction;cat='PETG impreso';basis='rho=1.27g/cm3; fraccion efectiva '+str(printed_fraction)
        elif cn.startswith('L0') and 'PVC' in cn:
            mass=1.45*v;cat='PVC';basis='rho=1.45g/cm3 (referencia PVC-U 1.42..1.48)'
        elif cn.startswith('V03_Horn_'):
            mass=1.14*v;cat='Horn polimero';basis='nylon supuesto 1.14g/cm3, independiente de apariencia blanca'
        elif ap in ['AFR2_Acero','AFR2_Acero_Oscuro']:
            mass=7.85*v;cat='Acero';basis='rho=7.85g/cm3, geometria simplificada maciza sin helices'
        elif ap=='AFR2_Laton':mass=8.5*v;cat='Laton';basis='rho=8.5g/cm3 supuesto'
        elif ap=='AFR2_Goma':mass=1.1*v;cat='Almohadilla';basis='rho=1.1g/cm3 supuesto'
        else:raise ValueError('Material no asignado '+cn+' / '+ap)
        out.append({**b,'mass_g':mass,'category':cat,'basis':basis,'com_mm':pt.tolist()})
    if extra_cable_g:
        pt=(ff['06_Herramienta']@np.array([0,0,P['tcp_offset']*10,1]))[:3]
        out.append({'root':'06_Herramienta','component':'Reserva_cables_distales','body':'masa puntual','mass_g':extra_cable_g,'category':'Reserva no CAD','basis':'supuesto conservador de cables en TCP','volume_cm3':0,'com_mm':pt.tolist()})
    return out,ff

def aggregate(parts,key):
    groups=defaultdict(list)
    for p in parts:groups[p[key]].append(p)
    result=[]
    for k,pp in groups.items():
        m=sum(p['mass_g'] for p in pp);com=sum((np.array(p['com_mm'])*p['mass_g'] for p in pp),np.zeros(3))/m if m else np.zeros(3)
        result.append({'name':k,'label':NAMES.get(k,k),'mass_g':m,'volume_cm3':sum(p['volume_cm3'] for p in pp),'com_mm':com.tolist()})
    return result

def wrench(parts,point):
    F=np.array([0.,0.,-sum(p['mass_g'] for p in parts)*G/1000]);M=sum((np.cross((np.array(p['com_mm'])-point)/1000,np.array([0,0,-p['mass_g']*G/1000])) for p in parts),np.zeros(3))
    return F,M

def joints(parts,ff):
    desc={
        'J1':(['01_Giro_base','02_Brazo_PVC','03_Antebrazo_PVC','04_Giro_muneca','05_Inclinacion_muneca','06_Herramienta','07_Mandibula_izquierda','08_Mandibula_derecha','09_Pinion_hombro','10_Pinion_pinza','11_Pinion_J4','90_Pieza_REFERENCIA_50mm_100g'],ff['01_Giro_base'][:3,3],np.array([0,0,1.])),
        'J2':(['02_Brazo_PVC','03_Antebrazo_PVC','04_Giro_muneca','05_Inclinacion_muneca','06_Herramienta','07_Mandibula_izquierda','08_Mandibula_derecha','10_Pinion_pinza','11_Pinion_J4','90_Pieza_REFERENCIA_50mm_100g'],ff['02_Brazo_PVC'][:3,3],ff['01_Giro_base'][:3,1]),
        'J3':(['03_Antebrazo_PVC','04_Giro_muneca','05_Inclinacion_muneca','06_Herramienta','07_Mandibula_izquierda','08_Mandibula_derecha','10_Pinion_pinza','11_Pinion_J4','90_Pieza_REFERENCIA_50mm_100g'],ff['03_Antebrazo_PVC'][:3,3],ff['02_Brazo_PVC'][:3,1]),
        'J4':(['04_Giro_muneca','05_Inclinacion_muneca','06_Herramienta','07_Mandibula_izquierda','08_Mandibula_derecha','10_Pinion_pinza','90_Pieza_REFERENCIA_50mm_100g'],ff['04_Giro_muneca'][:3,3],ff['03_Antebrazo_PVC'][:3,2]),
        'J5':(['05_Inclinacion_muneca','06_Herramienta','07_Mandibula_izquierda','08_Mandibula_derecha','10_Pinion_pinza','90_Pieza_REFERENCIA_50mm_100g'],ff['05_Inclinacion_muneca'][:3,3],ff['04_Giro_muneca'][:3,1]),
        'J6':(['06_Herramienta','07_Mandibula_izquierda','08_Mandibula_derecha','10_Pinion_pinza','90_Pieza_REFERENCIA_50mm_100g'],ff['06_Herramienta'][:3,3],ff['05_Inclinacion_muneca'][:3,2])}
    result=[]
    for j,(roots,pt,axis) in desc.items():
        pp=[p for p in parts if p['root'] in roots];F,M=wrench(pp,pt);torque=float(M@axis);bend=float(np.linalg.norm(M-torque*axis))
        payload=[p for p in pp if p['category']=='Carga util'];Fp,Mp=wrench(payload,pt)
        mass=sum(p['mass_g'] for p in pp);com=sum((np.array(p['com_mm'])*p['mass_g'] for p in pp),np.zeros(3))/mass
        ratio=2 if j=='J2' else 1;eta=.85 if j=='J2' else (.9 if j=='J4' else 1.)
        result.append({'joint':j,'point_mm':pt.tolist(),'axis_global':axis.tolist(),'supported_roots':roots,'supported_mass_g':mass,'downstream_com_mm':com.tolist(),'gravity_force_N':F.tolist(),'gravity_moment_Nm':M.tolist(),'reaction_force_N':(-F).tolist(),'reaction_moment_Nm':(-M).tolist(),'axis_torque_Nm':torque,'non_axis_bending_Nm':bend,'payload_axis_torque_Nm':float(Mp@axis),'self_weight_axis_torque_Nm':float((M-Mp)@axis),'servo_static_Nm_eta':abs(torque)/(ratio*eta),'servo_static_kgfcm_eta':abs(torque)/(ratio*eta)/.0980665,'preliminary_2x_servo_kgfcm':2*abs(torque)/(ratio*eta)/.0980665})
    return result

def cuts(parts,ff):
    out=[]
    for name,root,zvals in [('PVC1','02_Brazo_PVC',[25,42,65,88,105]),('PVC2','03_Antebrazo_PVC',[25,42,60,78,95])]:
        a=ff[root];axis=a[:3,2];o=a[:3,3]
        for z in zvals:
            point=(a@np.array([0,0,z,1]))[:3];pp=[]
            for p in parts:
                if p['root'] in ['00_Base_control','01_Giro_base','09_Pinion_hombro']:continue
                s=(np.array(p['com_mm'])-o)@axis
                if p['category']=='PVC' and p['root']==root:
                    # A straight tube crosses the cut: integrate its remaining
                    # uniform length instead of dropping the entire tube at its CG.
                    L=P['tube1_cut' if name=='PVC1' else 'tube2_cut']*10;end=25+L
                    if z<end:
                        cp=dict(p);cp['mass_g']=p['mass_g']*(end-z)/L;cp['com_mm']=(o+axis*(z+end)/2).tolist();pp.append(cp)
                elif s>=z-1e-7:pp.append(p)
            # Other groups can project past the cut although attached upstream
            # (e.g. static base motor). Restrict by the serial downstream graph.
            allowed=['02_Brazo_PVC','03_Antebrazo_PVC','04_Giro_muneca','05_Inclinacion_muneca','06_Herramienta','07_Mandibula_izquierda','08_Mandibula_derecha','10_Pinion_pinza','11_Pinion_J4','90_Pieza_REFERENCIA_50mm_100g']
            if name=='PVC2':allowed.remove('02_Brazo_PVC')
            pp=[p for p in pp if p['root'] in allowed]
            F,M=wrench(pp,point);torque=float(M@axis);bend=float(np.linalg.norm(M-torque*axis))
            od=32.;idd=28.3;A=math.pi*(od**2-idd**2)/4;I=math.pi*(od**4-idd**4)/64;J=2*I;W=I/(od/2)
            out.append({'section':name,'z_from_proximal_joint_mm':z,'point_mm':point.tolist(),'force_N':F.tolist(),'moment_Nm':M.tolist(),'torsion_Nm':torque,'bending_Nm':bend,'area_mm2':A,'I_mm4':I,'J_mm4':J,'W_mm3':W,'nominal_bending_stress_MPa':bend*1000/W,'nominal_torsion_shear_MPa':abs(torque)*1000*(od/2)/J,'sigma_at_2x_load_MPa':2*bend*1000/W,'scope':'resultante de corte aproximada por centroides; tubo distribuido. No incluye contacto de manguitos ni concentracion de tensiones.'})
    return out

def run_case(f=1.,payload=200,extra=0,tcp=False,q4=0,q6=0):
    q=[90,90,0,q4,0,q6,109.8];pp,ff=physical_parts(q,f,payload,extra,tcp)
    return {'q_deg':q,'printed_effective_fraction':f,'payload_g':payload,'cable_reserve_g':extra,'payload_at_TCP':tcp,'groups':aggregate(pp,'root'),'categories':aggregate(pp,'category'),'joints':joints(pp,ff),'pipe_sections':cuts(pp,ff)},pp,ff

def write_csv(path,rows,fields):
    with path.open('w',encoding='utf-8-sig',newline='') as f:
        w=csv.DictWriter(f,fieldnames=fields,extrasaction='ignore');w.writeheader();w.writerows(rows)

def main():
    case,parts,ff=run_case();cases={'CAD_macizo_200g':case}
    for name,f,pay,extra,tcp in [('fraccion_efectiva_70pct',.7,200,0,False),('fraccion_efectiva_50pct',.5,200,0,False),('cero_carga',1.,0,0,False),('200g_en_TCP_nominal',1.,200,0,True),('200g_CAD_y_30g_cables',1.,200,30,False)]:cases[name]=run_case(f,pay,extra,tcp)[0]
    # Roll changes can change torsional moments even with every link collinear.
    roll=[]
    for q4 in range(-90,91,15):
        for q6 in range(-90,91,15):
            cc,_,_=run_case(q4=q4,q6=q6)
            roll.append({'q4':q4,'q6':q6,'joints':[{k:j[k] for k in ['joint','axis_torque_Nm','non_axis_bending_Nm']} for j in cc['joints']]})
    native=json.loads((HERE/'pose_horizontal_nativa.json').read_text(encoding='utf-8'))
    parity=[]
    for root,xf in native['root_transforms_cm'].items():
        if root not in ff:continue
        actual=np.array(xf).reshape(4,4);actual[:3,3]*=10
        parity.append({'root':root,'max_matrix_error_mm_or_dimensionless':float(np.max(np.abs(ff[root]-actual)))})
    if max(x['max_matrix_error_mm_or_dimensionless'] for x in parity)>1e-7:raise ValueError('Cinematica analitica distinta de copia nativa horizontal: '+str(parity))
    bearing_mass=sum(p['mass_g'] for p in parts if p['category']=='Rodamiento')
    if abs(bearing_mass-120)>1e-6:raise ValueError('Masa BOM de rodamientos duplicada/incompleta')
    if len({p['component'] for p in parts if p['category']=='PETG impreso'})!=25:raise ValueError('Inventario de piezas PETG diferente de revision06')
    result={'scope':'Estatica gravitatoria CAD v06, carga 200g. Densidades/fichas y masas supuestas explicitadas. Sin FEA ni validacion fisica.','native_pose_parity':parity,'g_m_s2':G,'cases':cases,'roll_sweep':roll,'materials':{'PETG_g_cm3':1.27,'PVC_g_cm3':1.45,'steel_g_cm3':7.85,'brass_g_cm3':8.5,'horn_nylon_g_cm3':1.14,'rubber_g_cm3':1.1},'motor_mass_g':MOTOR_MASS,'electronics_mass_assumptions_g':ELECTRONIC_MASS}
    (HERE/'resultados_estatica.json').write_text(json.dumps(result,ensure_ascii=False,indent=2),encoding='utf-8')
    rows=[]
    shoulder=ff['02_Brazo_PVC'][:3,3];long=ff['02_Brazo_PVC'][:3,2]
    for p in parts:
        pt=np.array(p['com_mm']);s=float((pt-shoulder)@long)
        rows.append({**p,'x_mm':pt[0],'y_mm':pt[1],'z_mm':pt[2],'s_desde_hombro_mm':s,'peso_N':p['mass_g']*G/1000,'momento_hombro_Nm':s*p['mass_g']*G/1e6})
    write_csv(HERE/'masas_por_cuerpo.csv',rows,['root','component','body','category','volume_cm3','mass_g','x_mm','y_mm','z_mm','s_desde_hombro_mm','peso_N','momento_hombro_Nm','basis'])
    printed=aggregate([p for p in parts if p['category']=='PETG impreso'],'component')
    write_csv(HERE/'petg_por_pieza.csv',printed,['name','volume_cm3','mass_g','com_mm'])
    print('GRUPOS',json.dumps(case['groups'],ensure_ascii=False))
    print('CATEGORIAS',json.dumps(case['categories'],ensure_ascii=False))
    print('JUNTAS',json.dumps([{k:j[k] for k in ['joint','supported_mass_g','axis_torque_Nm','non_axis_bending_Nm','servo_static_kgfcm_eta']} for j in case['joints']]))
    print('PVC',json.dumps([{k:s[k] for k in ['section','z_from_proximal_joint_mm','bending_Nm','nominal_bending_stress_MPa']} for s in case['pipe_sections']]))

if __name__=='__main__':main()
