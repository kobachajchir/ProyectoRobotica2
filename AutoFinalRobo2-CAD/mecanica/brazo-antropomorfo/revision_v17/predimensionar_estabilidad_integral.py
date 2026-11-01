"""Layouts antes de modelar. No optimizacion topologica ni FEA.

Conservar masa de estructura CAD y reservar montaje; sustituir actuadores
por la alternativa robusta3250J1/J2/J3/J4/J5 y3032J6/pinza. PinzaV7 intacta.
"""
from pathlib import Path
import runpy,copy,json
import numpy as np
H=Path(__file__).resolve().parent

def adjustment(shoulder=154,tail=None,wrist45=55):
 def apply(c,raw,parts,groups):
    c.s.P['v05_axis_y']=-3.2;c.s.P['tower_h']=(shoulder-c.s.P['base_top']*10)/10
    c.s.P['gear_ratio']=3.6;c.s.P['gear_centers']=3.68
    c.s.P['j3_ratio']=2.5
    old_w=c.s.P['wrist45']*10;c.s.P['wrist45']=wrist45/10
    out=copy.deepcopy(parts)
    # Mantener cuerpos estructurales macizos; no inventar ahorro de material.
    for b in out:
      if b['root']=='04_Giro_muneca':b['cg_local_m'][2]*=wrist45/old_w
    motor_defs=[('V09_M01_GX3270BLS_Base',75.5,None),('V02_M02_GX3240_270',75.5,[.01011,.0949,c.s.P['tower_h']*.01-.0368]),('V02_M04_MG90S_J4',75.5,[.04611,.004,c.s.P['link2']*.01-.0175]),('V02_M05_MG946_J5',75.5,[.01011,.0575,wrist45*.001]),('V02_M06_MG90S_J6',21.6,None),('V02_M07_MG90S_Pinza',21.6,None)]
    for name,newmass,cg in motor_defs:
      pp=[b for b in out if b['component']==name]
      if not pp:raise ValueError('No se encontro '+name)
      oldmass=sum(b['mass_g'] for b in pp)
      for b in pp:
        ratio=newmass/oldmass;b['mass_g']*=ratio;b['I_local_kg_m2']=(np.array(b['I_local_kg_m2'])*ratio).tolist()
        if cg is not None:b['cg_local_m']=list(cg)
    def reserve(root,name,m,cg):out.append(dict(root=root,component=name,body='Reserva no modelada',mass_g=m,volume_cm3=0,material='Presupuesto',basis='No pieza construida',cg_local_m=cg,I_local_kg_m2=np.zeros((3,3)).tolist()))
    reserve('03_Antebrazo_PVC','Reserva20g_soporte_J4',20,[.04611,.004,c.s.P['link2']*.01-.0175])
    reserve('04_Giro_muneca','Reserva20g_soporte_J5',20,[.01011,.0575,wrist45*.001])
    if tail is not None:
      previous=c.s.P['j3_motor_z']*.01;c.s.P['j3_motor_z']=tail/10
      for b in out:
        n=b['component']
        if b['root']=='02_Brazo_PVC' and n.startswith('V17_') and not any(k in n for k in ['anclaje_fijo','Placa_fija','Correa_']):b['cg_local_m'][2]+=tail*.001-previous
      reserve('02_Brazo_PVC','Reserva20g_extension_Al_J3',20,[0,-.044,tail*.0005])
    return out,c.regroup(out)
 return apply

if __name__=='__main__':
    results=[]
    for name,shoulder,tail,wrist in [('centrado_bajo',154,None,55),('centrado_bajo_cola100',154,-100,55),('centrado_bajo_cola100_muneca42',154,-100,42)]:
      filename='propuesta_'+name+'_vuelco.json'
      note=f'Propuesta {name}: ejeJ1 centradoY-32, hombro{shoulder}mm, motorJ3Z{tail}, munecaJ4J5{wrist}. Masas CAD actuales conservadas +actuadores/reservas declarados. No geometria construida ni despejes comprobados.'
      runpy.run_path(str(H/'calcular_vuelco_v17.py'),run_name='__main__',init_globals=dict(AJUSTAR_VUELCO17=adjustment(shoulder,tail,wrist),OUT_FILE17=filename,LAYOUT_NOTE17=note))
      d=json.loads((H/filename).read_text());results.append(dict(name=name,mass_g=d['mass_arm_without_payload_g'],worst=d['worst'],static_max=max(x['static_utilization'] for x in d['by_yaw'])))
    (H/'comparacion_layouts_estabilidad.json').write_text(json.dumps(results,indent=2),encoding='utf-8')
