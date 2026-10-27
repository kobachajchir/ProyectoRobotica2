"""Extraccion nativa v08 para masas, inercias y archivos de autoría. Sin barridos."""
import adsk.core,adsk.fusion,os,json

def run(_context: str):
    app=adsk.core.Application.get();d=adsk.fusion.Design.cast(app.activeProduct);r=d.rootComponent
    if app.activeDocument.name!='AFR2_Brazo_PVC32_AGV_v08_Housing':raise RuntimeError('Activar v08')
    folder=r'D:\ProyectoRobotica2\AutoFinalRobo2-CAD\mecanica\brazo-antropomorfo\revision_v08'
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
                'volume_cm3':pp.volume,'centroid_component_mm':[10*v for v in cg],
                'appearance':b.appearance.name if b.appearance else '',
                'material_assigned':b.material.name if b.material else '',
                'bbox_component_mm':[10*v for v in list(b.boundingBox.minPoint.asArray())+list(b.boundingBox.maxPoint.asArray())],
                'component_to_root_cm':list(o.transform2.asArray()),'native_mass_kg':mass,
                'native_centroidal_tensor_kg_cm2':central,'moments_valid':ok and okp and oka,
                'native_principal_kg_cm2':iv,'native_principal_axes':axes,
                'native_world_moments_kg_cm2':[xx,yy,zz,xy,yz,xz]})
    data={'document':app.activeDocument.name,'id':app.activeDocument.dataFile.id,
          'scope':'Geometria y tensor uniforme de los cuerpos. Densidades y masas comerciales sustituidas externamente. Sin ensayos.',
          'parameters':{p.name:{'expression':p.expression,'value_internal':p.value,'unit':p.unit} for p in d.userParameters},
          'parts':parts,'root_transforms_cm':{o.component.name:list(o.transform2.asArray()) for o in r.occurrences},
          'joint_limits':{j.name:{'min_rad':j.jointMotion.rotationLimits.minimumValue,
                                 'max_rad':j.jointMotion.rotationLimits.maximumValue,
                                 'min_enabled':j.jointMotion.rotationLimits.isMinimumValueEnabled,
                                 'max_enabled':j.jointMotion.rotationLimits.isMaximumValueEnabled}
                          for j in r.joints if j.name.startswith(('J1_','J2_','J3_'))}}
    with open(os.path.join(folder,'geometria_v08.json'),'w',encoding='utf-8') as f:json.dump(data,f,ensure_ascii=False,indent=2)
    print(json.dumps({'document':app.activeDocument.name,'bodies_exported':len(parts),'output':'revision_v08/geometria_v08.json'}))
