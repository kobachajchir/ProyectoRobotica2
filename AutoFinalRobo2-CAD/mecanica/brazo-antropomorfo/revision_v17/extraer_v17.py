"""Extraccion nativa v17 para masas, inercias y archivos de autoría. Sin barridos."""
import adsk.core,adsk.fusion,os,json
from pathlib import Path


def materiales_vigentes(d,folder):
    phases=[]
    for line in (Path(folder)/'progreso_J3.jsonl').read_text(encoding='utf-8').splitlines():
      message=json.loads(line).get('message','')
      if not message.startswith('aplicada '):continue
      phase=message.removeprefix('aplicada ')
      if phase not in ['save','modo_trabajo','restaurar_vista','ensambles_limites'] and d.attributes.itemByName('AFR2','v17_phase_'+phase):phases.append(phase)
    return bool(phases and phases[-1].startswith('act_sts_materiales'))

def run(_context: str):
    app=adsk.core.Application.get();d=adsk.fusion.Design.cast(app.activeProduct);r=d.rootComponent
    if app.activeDocument.name!='AFR2_Brazo_v17_Base_V7_J3_Remoto':raise RuntimeError('Activar v17')
    folder=r'D:\ProyectoRobotica2\AutoFinalRobo2-CAD\mecanica\brazo-antropomorfo\revision_v17'
    os.makedirs(folder,exist_ok=True);parts=[]
    for o in r.allOccurrences:
        root=o.fullPathName.split('+')[0].split(':')[0]
        if root in ['80_Auto_referencia','81_Pan_HCSR04','82_Envolvente_pan_REFERENCIA']:continue
        for b in o.component.bRepBodies:
            if not b.isSolid or b.volume<1e-9:continue
            pp=b.getPhysicalProperties(adsk.fusion.CalculationAccuracy.HighCalculationAccuracy)
            ok,xx,yy,zz,xy,yz,xz=pp.getXYZMomentsOfInertia()
            okp,i1,i2,i3=pp.getPrincipalMomentsOfInertia()
            oka,ax,ay,az=pp.getPrincipalAxes()
            # Principal inertia and directions avoid ambiguity of products'
            # sign convention and translation from the component origin.
            cg=list(pp.centerOfMass.asArray());mass=pp.mass
            axes=[list(a.asArray()) for a in [ax,ay,az]];iv=[i1,i2,i3]
            central=[[sum(iv[k]*axes[k][i]*axes[k][j] for k in range(3)) for j in range(3)] for i in range(3)]
            parts.append({'root':root,'path':o.fullPathName,'component':o.component.name,'body':b.name,
                'volume_cm3':pp.volume,'lumps':b.lumps.count,
                'catalog_mass_g_component':o.component.attributes.itemByName('AFR2','catalog_mass_g').value if o.component.attributes.itemByName('AFR2','catalog_mass_g') else None,
                'material_v17':o.component.attributes.itemByName('AFR2','material_v17').value if o.component.attributes.itemByName('AFR2','material_v17') else None,'centroid_component_mm':[10*v for v in cg],
                'appearance':b.appearance.name if b.appearance else '',
                'material_assigned':b.material.name if b.material else '',
                'bbox_component_mm':[10*v for v in list(b.boundingBox.minPoint.asArray())+list(b.boundingBox.maxPoint.asArray())],
                'component_to_root_cm':list(o.transform2.asArray()),'native_mass_kg':mass,
                'native_centroidal_tensor_kg_cm2':central,'moments_valid':ok and okp and oka,
                'native_principal_kg_cm2':iv,'native_principal_axes':axes,
                'native_world_moments_kg_cm2':[xx,yy,zz,xy,yz,xz]})
    data={'document':app.activeDocument.name,'id':app.activeDocument.dataFile.id,
          'integrated_mini_mounts':all(d.attributes.itemByName('AFR2','v17_phase_act_mini_'+which+'_'+phase) for which in ['J6','J7'] for phase in ['horn','fasteners']),
          'integrated_J2_mount':all(d.attributes.itemByName('AFR2','v17_phase_act_sts_J2_'+phase) for phase in ['soporte','engranajes','motor','cuna','rotor','anclajes','retencion']),
          'integrated_J5_mount':all(d.attributes.itemByName('AFR2','v17_phase_act_sts_J5_'+phase) for phase in ['parametros','motor','housing','tapa','pasantes','fijaciones','horn','tornillos_horn','montaje_axial']),
          'integrated_J4_mount':all(d.attributes.itemByName('AFR2','v17_phase_act_sts_J4_'+phase) for phase in ['parametros','motor','housing','tapa','pasantes','fijaciones','horn','union']),
          'integrated_J1_mount':all(d.attributes.itemByName('AFR2','v17_phase_act_sts_J1_'+phase) for phase in ['parametros','motor','housing','cartucho','horn','adapter','union']),
          'physical_materials_synced':materiales_vigentes(d,folder),
          'scope':'Geometria y tensor uniforme de los cuerpos. Densidades y masas comerciales sustituidas externamente. Sin ensayos.',
          'parameters':{p.name:{'expression':p.expression,'value_internal':p.value,'unit':p.unit} for p in d.userParameters},
          'parts':parts,'joint_positions_rad':{j.name:j.jointMotion.rotationValue for j in r.joints if hasattr(j.jointMotion,'rotationValue')},
          'root_transforms_cm':{o.component.name:list(o.transform2.asArray()) for o in r.occurrences},
          'joint_limits':{j.name:{'min_rad':j.jointMotion.rotationLimits.minimumValue,
                                 'max_rad':j.jointMotion.rotationLimits.maximumValue,
                                 'min_enabled':j.jointMotion.rotationLimits.isMinimumValueEnabled,
                                 'max_enabled':j.jointMotion.rotationLimits.isMaximumValueEnabled}
                          for j in r.joints if j.name.startswith(('J1_','J2_','J3_','J4_','J5_','J6_','T_J3_'))}}
    with open(os.path.join(folder,'geometria_v17.json'),'w',encoding='utf-8') as f:json.dump(data,f,ensure_ascii=False,indent=2)
    print(json.dumps({'document':app.activeDocument.name,'bodies_exported':len(parts),'output':'revision_v17/geometria_v17.json'}))
