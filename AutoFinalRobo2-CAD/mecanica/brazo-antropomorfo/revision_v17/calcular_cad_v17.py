"""Masas CAD y dinamica rigida mediante trabajo virtual de todos los grupos.

Incluye polea remota y pinones auxiliares en sus respectivas relaciones.
No representa inercia interna del motor, elasticidad, holguras ni FEA/FDM.
Ejecutar expresamente despues de extraer una revision nativa identificada.
"""
from pathlib import Path
import importlib.util,json,math,itertools,hashlib
from collections import defaultdict
import numpy as np

H=Path(__file__).resolve().parent
spec=importlib.util.spec_from_file_location('estatica_legacy',H.parent/'analisis_cargas_200g/calcular_estatica.py')
s=importlib.util.module_from_spec(spec);spec.loader.exec_module(s)

def frames(q):
    f=s.fk(list(np.degrees(q))+[math.degrees(s.P['grip_angle'])])
    # El origen del grupo auxiliar esta en la polea motriz, solidario al
    # primer eslabon. Correa abierta: entrada gira2*q3 respecto al eslabon.
    f['12_Polea_entrada_J3']=f['02_Brazo_PVC']@s.tr(z=s.P['j3_motor_z']*10)@s.rot('Y',math.degrees(q[2])*s.P['j3_ratio'])
    f['09_Pinion_hombro']=f['01_Giro_base']@s.tr(z=(s.P['tower_h']-s.P['gear_centers'])*10)@s.rot('Y',math.degrees(s.P['shoulder_phase'])-s.P['gear_ratio']*math.degrees(q[1]))
    if 'v17_j4_axis_x' in s.P:
      f['11_Pinion_J4']=f['03_Antebrazo_PVC']@s.tr(s.P['v17_j4_axis_x']*10,s.P['v17_j4_axis_y']*10,s.P['link2']*10+6)@s.rot('Z',math.degrees(s.P['v17_j4_phase'])-math.degrees(q[3]))
    for a in f.values():a[:3,3]*=.001
    return f

def load():
    geometry_path=Path(globals().get('GEOMETRY_PATH17',H/'geometria_v17.json'))
    contents=geometry_path.read_bytes();raw=json.loads(contents.decode('utf-8'))
    raw['source_sha256']=hashlib.sha256(contents).hexdigest()
    s.P={k:v['value_internal'] for k,v in raw['parameters'].items()}
    volumes=defaultdict(float)
    # Una pieza comercial puede aparecer en mas de una articulacion.
    # La masa de catalogo corresponde a CADA ocurrencia, no a la suma de
    # todas las instancias que comparten una definicion de componente.
    for b in raw['parts']:volumes[b['path']]+=b['volume_cm3']
    roots={k:np.array(v).reshape(4,4) for k,v in raw['root_transforms_cm'].items()}
    motors=dict(s.MOTOR_MASS,**{'V09_M01_GX3270BLS_Base':90,'V07_M03_GX3270BLS_Codo':90})
    parts=[]
    for b in raw['parts']:
      cn=b['component'];v=b['volume_cm3'];mat=b.get('material_v17') or '';ap=b['appearance'];override=b.get('catalog_mass_g_component')
      if cn=='V17_Correa_OMEGA_HP_3M_6':
        mass=s.P['j3_belt_length']*10*.0144*v/volumes[b['path']];kind='Correa';basis='OMEGA3M6:0.0144g/mm; largo parametrico actual'
      elif override is not None:
        mass=float(override)*v/volumes[b['path']];kind=mat;basis='Masa comercial por ocurrencia, distribuida por volumen de envolvente'
      elif cn in motors:
        mass=motors[cn]*v/volumes[b['path']];kind='Servo';basis='Ficha o masa conservada del modelo anterior; no pesaje'
      elif cn in s.ELECTRONIC_MASS:
        mass=s.ELECTRONIC_MASS[cn]*v/volumes[b['path']];kind='Electronica';basis='Masa estimada del modulo'
      elif cn.startswith('V09_R01_6000_J1_'):
        mass=20*v/volumes[b['path']];kind='Rodamiento';basis='6000:20g referencia por ocurrencia'
      elif cn in s.BEARINGS:
        mass=(12.5 if '608' in cn else 5)*v/volumes[b['path']];kind='Rodamiento';basis='608:12.5g;625:5g por ocurrencia'
      elif b['root'].startswith('90_'):
        mass=200*v/volumes[b['path']];kind='Carga';basis='Carga maxima solicitada200g'
      elif mat=='Al6061' or cn=='V03_Horn_movil_V02_M03_DS3218_Codo':mass=2.7*v;kind='Aluminio';basis='Volumen CAD macizo*rho2.70g/cm3'
      elif mat=='Al2024':mass=2.78*v;kind='Aluminio2024';basis='Volumen CAD macizo*rho2.78g/cm3'
      elif mat.startswith('PLA') or ap in ['AFR2_PETG_Rojo','AFR2_PLA_PrintALot_Rojo']:mass=1.24*v;kind='PLA PrintALot';basis='Volumen CAD macizo*rho1.24; no equivale a masa laminada FDM'
      elif 'PVC' in cn and cn.startswith('L0'):mass=1.45*v;kind='PVC';basis='rho1.45g/cm3 supuesta'
      elif cn=='V09_Horn25T_J1' or mat in ['Acero','steel'] or ap in ['AFR2_Acero','AFR2_Acero_Oscuro']:mass=7.85*v;kind='Acero';basis='rho7.85g/cm3, roscas simplificadas'
      elif cn.startswith('V03_Horn_'):mass=1.14*v;kind='Horn polimero';basis='Nylon supuesto1.14g/cm3'
      elif ap=='AFR2_Laton':mass=8.5*v;kind='Laton';basis='rho8.5g/cm3'
      elif ap=='AFR2_Goma':mass=1.1*v;kind='Almohadilla';basis='rho1.1g/cm3 supuesta'
      else:raise ValueError('Asignar material/masa explicita: '+cn+' / '+mat+' / '+ap)
      rel=np.linalg.inv(roots[b['root']])@np.array(b['component_to_root_cm']).reshape(4,4);rel[:3,3]*=.01
      cg=(rel@np.r_[np.array(b['centroid_component_mm'])*.001,1])[:3]
      if b['native_mass_kg']<=0:raise ValueError('Masa nativa nula '+cn)
      tensor=np.array(b['native_centroidal_tensor_kg_cm2'])*1e-4*(mass/1000/b['native_mass_kg'])
      parts.append(dict(root=b['root'],component=cn,body=b['body'],mass_g=mass,volume_cm3=v,material=kind,basis=basis,cg_local_m=cg.tolist(),I_local_kg_m2=(rel[:3,:3]@tensor@rel[:3,:3].T).tolist()))
    return raw,parts,regroup(parts)

def regroup(parts):
    groups=[]
    for root in dict.fromkeys(b['root'] for b in parts):
      pp=[b for b in parts if b['root']==root];mass=sum(b['mass_g'] for b in pp);cg=sum((b['mass_g']*np.array(b['cg_local_m']) for b in pp),np.zeros(3))/mass;I=np.zeros((3,3))
      for b in pp:
        x=np.array(b['cg_local_m'])-cg;I+=np.array(b['I_local_kg_m2'])+b['mass_g']/1000*(x@x*np.eye(3)-np.outer(x,x))
      groups.append(dict(root=root,mass_g=mass,cg=cg,I=I))
    return groups

def axial(W):return np.array([W[2,1]-W[1,2],W[0,2]-W[2,0],W[1,0]-W[0,1]])*.5

def state(groups,q,dq,ddq):
    h=1e-4;eps=1e-5;f=frames(q);fp=frames(q+dq*h+.5*ddq*h*h);fm=frames(q-dq*h+.5*ddq*h*h)
    partial=[]
    for j in range(6):
      e=np.zeros(6);e[j]=eps;partial.append((frames(q+e),frames(q-e)))
    tau=np.zeros(6);wrenches=[];payload_force=None
    for g in groups:
      root=g['root'];A=f[root];R=A[:3,:3];p=np.r_[g['cg'],1];x=(A@p)[:3]
      acc=((fp[root]@p)[:3]-2*x+(fm[root]@p)[:3])/(h*h)
      Rd=(fp[root][:3,:3]-fm[root][:3,:3])/(2*h);Rdd=(fp[root][:3,:3]-2*R+fm[root][:3,:3])/(h*h)
      w=axial(Rd@R.T);alpha=axial(Rdd@R.T+Rd@Rd.T);I=R@g['I']@R.T
      F=g['mass_g']/1000*(acc-np.array([0,0,-s.G]));M=I@alpha+np.cross(w,I@w)
      if root.startswith('90_'):payload_force=F
      for j,(pa,ma) in enumerate(partial):
        Jv=((pa[root]@p)[:3]-(ma[root]@p)[:3])/(2*eps);Jw=axial((pa[root][:3,:3]-ma[root][:3,:3])/(2*eps)@R.T)
        tau[j]+=Jv@F+Jw@M
      wrenches.append(dict(root=root,x=x,F=F,M=M))
    # Seccion estructural en J1 y J2: fuerza y momento resultantes,
    # incluyendo el conjunto remoto y sus reacciones sobre el primer eslabon.
    sections=[]
    for name,root,excluded in [('J1','01_Giro_base',{'00_Base_control'}),('J2','02_Brazo_PVC',{'00_Base_control','01_Giro_base','09_Pinion_hombro'})]:
      pt=f[root][:3,3];F=np.zeros(3);M=np.zeros(3)
      for row in wrenches:
        if row['root'] in excluded:continue
        F+=row['F'];M+=np.cross(row['x']-pt,row['F'])+row['M']
      localF=f[root][:3,:3].T@F;localM=f[root][:3,:3].T@M
      sections.append(dict(joint=name,force_N=F.tolist(),moment_Nm=M.tolist(),local_force_N=localF.tolist(),local_moment_Nm=localM.tolist()))
    if payload_force is not None:sections.append(dict(joint='payload',force_N=payload_force.tolist()))
    return tau,sections

def main():
    raw,parts,groups=load()
    if globals().get('AJUSTAR_CAD'):parts,groups=globals()['AJUSTAR_CAD'](raw,parts,groups)
    prefix=globals().get('RESULT_PREFIX17','')
    q=np.radians([0,90,0,0,0,0]);ff=frames(q)
    mass=sum(b['mass_g'] for b in parts);cg=sum((g['mass_g']*(ff[g['root']]@np.r_[g['cg'],1])[:3] for g in groups),np.zeros(3))/mass
    static,sections=state(groups,q,np.zeros(6),np.zeros(6));peaks=np.abs(static);peakstates=[dict(q_deg=np.degrees(q).tolist(),dq_deg_s=[0]*6,ddq_deg_s2=[0]*6) for _ in range(6)]
    sectionpeaks={j:dict(force_N=0,moment_Nm=0,local_components_abs=[0]*6) for j in ['J1','J2']};count=0;payload_peak=s.G*.2
    # Frenada cubica50ms a30deg/s: punto de mayor deceleracion900deg/s2.
    # 64 combinaciones simultaneas, mas seis ejes aislados,9 munecas.
    signs=list(itertools.product([-1,1],repeat=6))+[tuple(sign if k==axis else 0 for k in range(6)) for axis in range(6) for sign in [-1,1]]
    for q4,q5 in itertools.product([-90,0,90],repeat=2):
      end=np.radians([0,90,0,q4,q5,0])
      for sign,u in itertools.product(signs,[0,.25,.5,.75,1]):
        speed=np.radians(np.array(sign)*30);tb=.05
        qs=end-speed*tb/2+speed*tb*(u-u**3+.5*u**4);dq=speed*(1-3*u*u+2*u**3);ddq=speed*(-6*u+6*u*u)/tb
        # No usar estados fuera de los limites nativos habilitados.
        valid=True
        for i in range(6):
          lim=next(v for k,v in raw['joint_limits'].items() if k.startswith('J'+str(i+1)+'_'))
          start=end[i]-speed[i]*tb/2
          if (lim['min_enabled'] and min(start,end[i])<lim['min_rad']-1e-8) or (lim['max_enabled'] and max(start,end[i])>lim['max_rad']+1e-8):valid=False
        if not valid:continue
        tor,sec=state(groups,qs,dq,ddq);count+=1
        for i in range(6):
          if abs(tor[i])>peaks[i]:
            peaks[i]=abs(tor[i]);peakstates[i]=dict(q_deg=np.degrees(qs).tolist(),dq_deg_s=np.degrees(dq).tolist(),ddq_deg_s2=np.degrees(ddq).tolist())
        for row in sec:
          if row['joint']=='payload':payload_peak=max(payload_peak,float(np.linalg.norm(row['force_N'])));continue
          p=sectionpeaks[row['joint']];p['force_N']=max(p['force_N'],float(np.linalg.norm(row['force_N'])));p['moment_Nm']=max(p['moment_Nm'],float(np.linalg.norm(row['moment_Nm'])))
          p['local_components_abs']=np.maximum(p['local_components_abs'],np.abs(row['local_force_N']+row['local_moment_Nm'])).tolist()
    out=dict(scope='CAD v17 en curso: masas macizas y dinamica rigida,200g. No prueba de resistencia, FEA ni envolvente global.',document=raw['document'],parameters=raw['parameters'],mass_without_payload_g=mass-200,cg_with_payload_extended_mm=(cg*1000).tolist(),groups=[dict(root=g['root'],mass_g=g['mass_g'],cg_local_mm=(g['cg']*1000).tolist()) for g in groups],static_torque_Nm=static.tolist(),sampled_peak_Nm=peaks.tolist(),peak_states=peakstates,samples=count,section_peaks=sectionpeaks,static_sections=sections,limitations=['Inercia interna/curva par-velocidad y frenado del servo no disponibles','Masa PLA maciza; perfil FDM y resistencia intercapas sin definir','CG motor uniforme en envolvente; no interiores','No demuestra estabilidad sobre contactos de rueda reales','Muestreo parcial de poses/frenadas; no maximo global ni choque geometrico'])
    if prefix:out['scope']='CONTRAFACTUAL previo al modelado: '+globals().get('CANDIDATE_SCOPE17','Modificacion declarada de piezas CAD; no geometria construida')
    out['payload_peak_force_N']=payload_peak;out['geometry_sha256']=raw['source_sha256']
    (H/(prefix+'masas_cad_actuales.json')).write_text(json.dumps(parts,indent=2),encoding='utf-8')
    (H/(prefix+'cargas_cad_actuales.json')).write_text(json.dumps(out,indent=2),encoding='utf-8')
    print(json.dumps({k:out[k] for k in ['mass_without_payload_g','groups','static_torque_Nm','sampled_peak_Nm','samples','section_peaks']}))

if __name__=='__main__':main()
