"""Preseleccion con todas las masas comerciales propuestas y reservas de montaje.

No calcula vuelco. Conserva pinzaV7 y todas las masas estructurales actuales,
sin atribuir ahorros al estudio de optimizacion que esta diferido.
"""
from pathlib import Path
import importlib.util,json,math
import numpy as np
H=Path(__file__).resolve().parent
sp=importlib.util.spec_from_file_location('c17',H/'calcular_cad_v17.py');c=importlib.util.module_from_spec(sp);sp.loader.exec_module(c)


def proposed_sts(raw,old_name,root,mass):
    # Reutilizar la envolvente comercial YA modelada, no otra caja generica.
    # Las masas comerciales siguen distribuidas uniformemente por volumen;
    # no se infieren engranajes/interiores reales del fabricante.
    source=[b for b in raw['parts'] if b['root']=='02_Brazo_PVC' and b['component']=='V17_M03_STS3250_remoto']
    if not source:raise ValueError('Falta referencia nativa STS3250 para preseleccionar')
    p=c.s.P;total=sum(b['volume_cm3'] for b in source)
    origin=np.array([0,p['v17_sts_output_y']*.01,p['j3_motor_z']*.01])
    if old_name=='V09_M01_GX3270BLS_Base':
      R=np.array([[-1,0,0],[0,0,1],[0,1,0]]);target=np.array([0,p['v05_axis_y']*.01,.05915])
    elif old_name=='V02_M04_MG90S_J4':
      R=np.array([[0,0,-1],[-1,0,0],[0,1,0]]);target=np.array([p['v17_j4_axis_x']*.01,p['v17_j4_axis_y']*.01,p['link2']*.01])
    elif old_name=='V02_M05_MG946_J5':
      R=np.diag([-1,-1,1]);target=np.array([0,.0363,p['wrist45']*.01])
    else:raise ValueError('Actuador propuesto sin interfaz definida '+old_name)
    result=[]
    for b in source:
      m=mass*b['volume_cm3']/total
      if b['native_mass_kg']<=0 or not b['moments_valid']:raise ValueError('Inercia nativa no disponible')
      I=np.array(b['native_centroidal_tensor_kg_cm2'])*1e-4*(m/1000/b['native_mass_kg'])
      cg=R@(np.array(b['centroid_component_mm'])*.001-origin)+target
      result.append(dict(root=root,component='PROPUESTO_'+old_name,body=b['body'],mass_g=m,volume_cm3=b['volume_cm3'],material='Servo propuesto',basis='Envolvente nativa STS3250 trasladada/rotada,75.5g; internos aproximados por volumen, no CAD implementado en este eje',cg_local_m=cg.tolist(),I_local_kg_m2=(R@I@R.T).tolist()))
    return result

def adjust(raw,parts,groups):
    if not raw.get('integrated_J2_mount'):
      c.s.P['gear_ratio']=4;c.s.P['gear_centers']=3.6;c.s.P['shoulder_phase']=0
    if not raw.get('integrated_J4_mount'):
      c.s.P['v17_j4_axis_y']=0;c.s.P['v17_j4_axis_x']=3.6
      c.s.P['v17_j4_phase']=math.radians(7.5)
    definitions=[
      ('V09_M01_GX3270BLS_Base',75.5,[.0125,c.s.P['v05_axis_y']*.01,.04425],[.04522,.02472,.035]),
      ('V02_M02_GX3240_270',75.5,[.0125,.0923,c.s.P['tower_h']*.01-.036],[.04522,.035,.02472]),
      ('V02_M04_MG90S_J4',75.5,[c.s.P['v17_j4_axis_x']*.01,.0125,c.s.P['link2']*.01-.0149],[.02472,.04522,.035]),
      ('V02_M05_MG946_J5',75.5,[.0125,.0512,c.s.P['wrist45']*.01],[.04522,.035,.02472]),
      ('V02_M06_MG90S_J6',21.6,[.00579,0,c.s.P['wrist56']*.01-.03665],[.023,.012,.0275]),
      ('V02_M07_MG90S_Pinza',21.6,[.00579,0,c.s.P['grip_deck']*.01-.01215],[.023,.012,.0275]),
    ]
    out=list(parts)
    for name,mass,cg,box in definitions:
      if raw.get('integrated_J2_mount') and name=='V02_M02_GX3240_270':continue
      if raw.get('integrated_J5_mount') and name=='V02_M05_MG946_J5':continue
      if raw.get('integrated_J4_mount') and name=='V02_M04_MG90S_J4':continue
      if raw.get('integrated_J1_mount') and name=='V09_M01_GX3270BLS_Base':continue
      if raw.get('integrated_mini_mounts') and name in ['V02_M06_MG90S_J6','V02_M07_MG90S_Pinza']:
        # Ambos servos y sus montajes ya son geometria nativa: conservar
        # sus CG/inercias y no volver a sustituirlos por cajas uniformes.
        continue
      old=[b for b in out if b['component']==name]
      if not old:raise ValueError('No se encontro el motor a reemplazar '+name)
      if name in ['V09_M01_GX3270BLS_Base','V02_M04_MG90S_J4','V02_M05_MG946_J5']:
        out=[b for b in out if b['component']!=name]
        out.extend(proposed_sts(raw,name,old[0]['root'],mass));continue
      if cg is None:
        original=sum(b['mass_g'] for b in old)
        for b in old:
          b['mass_g']*=mass/original;b['I_local_kg_m2']=(np.array(b['I_local_kg_m2'])*mass/original).tolist()
        continue
      out=[b for b in out if b['component']!=name]
      I=mass/1000/12*np.diag([box[1]**2+box[2]**2,box[0]**2+box[2]**2,box[0]**2+box[1]**2])
      out.append(dict(root=old[0]['root'],component='PROPUESTO_'+name,body='Caja uniforme comercial',mass_g=mass,volume_cm3=0,material='Servo propuesto',basis='Masa comercial con tolerancia superior; no CAD implementado',cg_local_m=cg,I_local_kg_m2=I.tolist()))
    for root,name,mass,cg in [('01_Giro_base','Reserva_soporte_J2',40,[.01,.075,c.s.P['tower_h']*.01-.036]),('02_Brazo_PVC','Reserva_rueda_J2_Al',25,[0,.043,0]),('03_Antebrazo_PVC','Reserva_housing_J4',40,[.036,.0125,c.s.P['link2']*.01-.018]),('04_Giro_muneca','Reserva_housing_J5',25,[.01,.057,c.s.P['wrist45']*.01]),('05_Inclinacion_muneca','Reserva_clamp_J6',15,[.006,0,c.s.P['wrist56']*.01-.036]),('06_Herramienta','Reserva_clamp_motor_pinza',15,[.006,0,.025]),('06_Herramienta','Reserva_cableado_distal',10,[0,0,.088])]:
      if raw.get('integrated_J2_mount') and name in ['Reserva_soporte_J2','Reserva_rueda_J2_Al']:continue
      if raw.get('integrated_J5_mount') and name=='Reserva_housing_J5':continue
      if raw.get('integrated_J4_mount') and name=='Reserva_housing_J4':continue
      if raw.get('integrated_mini_mounts') and name in ['Reserva_clamp_J6','Reserva_clamp_motor_pinza']:continue
      out.append(dict(root=root,component=name,body='Reserva adicional sin descontar soporte viejo',mass_g=mass,volume_cm3=0,material='Reserva',basis='Presupuesto conservador adicional, no parte modelada',cg_local_m=cg,I_local_kg_m2=np.zeros((3,3)).tolist()))
    return out,c.regroup(out)

if __name__=='__main__':
    c.AJUSTAR_CAD=adjust;c.RESULT_PREFIX17='actuadores_integrados_'
    c.CANDIDATE_SCOPE17='NativosJ2/J3 yJ6/pinza cuando sus fases estan completas. PropuestasSTS3250J1/J4/J5 con reservas restantes; J2=4(18/72modulo0.8),J3=2.5,J4=1. Cableado10g en punta; estructuras CAD conservadas. Sin calculo de vuelco.'
    c.main();d=json.loads((H/'actuadores_integrados_cargas_cad_actuales.json').read_text());rows=[]
    for i,(motor,nom,ratio,eta) in enumerate(zip(['STS3250']*5+['STS3032'],[16]*5+[1.5],[1,4,2.5,1,1,1],[.95,.8,.8,.8,.95,.95])):
      peak=d['sampled_peak_Nm'][i];cap=nom*.0980665*ratio*eta
      rows.append(dict(joint='J'+str(i+1),motor=motor,rated_kgfcm=nom,ratio=ratio,assumed_efficiency=eta,external_peak_Nm=peak,capacity_Nm=cap,margin=cap/peak,passes_1_2=cap/peak>=1.2,required_servo_speed_deg_s=30*ratio))
    result=dict(scope=c.CANDIDATE_SCOPE17,mass_budget_without_payload_g=d['mass_without_payload_g'],axes=rows,payload_peak_force_N=d['payload_peak_force_N'],gripper=dict(motor='STS3032',rated_kgfcm=1.5,assumed_rack_efficiency=.85,pinion_radius_mm=12,minimum_pad_friction_for_margin1_2=d['payload_peak_force_N']*1.2*.012/(1.5*.0980665*.85),status='Requisito de friccion, no agarre fisico verificado. MecanismoV7 conservado.'),geometry_sha256=d['geometry_sha256'],limitations=d['limitations'])
    result['proposed_J4_layout']=dict(axis_x_mm=c.s.P['v17_j4_axis_x']*10,axis_y_mm=c.s.P['v17_j4_axis_y']*10,phase_deg=math.degrees(c.s.P['v17_j4_phase']),motor_long_direction='+Y',basis='Envolvente nativa del motor, no caja uniforme nueva')
    (H/'preseleccion_actuadores_integrados.json').write_text(json.dumps(result,indent=2),encoding='utf-8')
    print(json.dumps(result))
