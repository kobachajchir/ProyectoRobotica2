"""Revision CAD solicitada: interferencias exactas y posiciones muestreadas.

Ejecutar en Fusion, nunca como test de software. Conserva y restaura poses.
No elimina contactos por heuristicas: registra los resultados sin clasificarlos.
"""
import adsk.core,adsk.fusion,json,math
from pathlib import Path
H=Path(globals().get('AFR2_REPO_ROOT',r'D:\ProyectoRobotica2'))/'AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/revision_v17'
JOINTS=['J1_Giro_base','J2_Hombro','J3_Codo','J4_Giro_muneca','J5_Inclinacion_muneca','J6_Giro_herramienta']

def label17(entity,lookup=None,interference=None):
    body=adsk.fusion.BRepBody.cast(entity)
    if body:
      context=body.assemblyContext
      if context:return context.fullPathName+'/'+body.name
      key=(body.parentComponent.name,body.name)
      candidates=(lookup or {}).get(key,[])
      if len(candidates)==1:return candidates[0][0]
      if candidates and interference:
        bb=interference.boundingBox
        p=[(a+b)/2 for a,b in zip(bb.minPoint.asArray(),bb.maxPoint.asArray())]
        matching=[name for name,lo,hi in candidates if all(a-1e-5<=v<=b+1e-5 for a,v,b in zip(lo,p,hi))]
        if len(matching)==1:return matching[0]
        return 'AMBIGUO:'+str(matching or [n for n,_,_ in candidates])
      return body.parentComponent.name+'/'+body.name
    occurrence=adsk.fusion.Occurrence.cast(entity)
    return occurrence.fullPathName if occurrence else entity.objectType

def run(context):
    app=adsk.core.Application.get();d=adsk.fusion.Design.cast(app.activeProduct);r=d.rootComponent
    if app.activeDocument.name!='AFR2_Brazo_v17_Base_V7_J3_Remoto':raise RuntimeError('Activar v17')
    scope=globals().get('SCOPE17','J3');name=globals().get('AUDIT_NAME17','interferencias_actuales')
    joints=[r.joints.itemByName(n) for n in JOINTS]
    original=[j.jointMotion.rotationValue for j in joints]
    grip=r.joints.itemByName('J7_Servo_pinza');grip_original=grip.jointMotion.rotationValue if grip else None
    grip_requested=globals().get('GRIPPER_ABS17');grip_moved=False
    if grip_requested is not None and not grip:raise RuntimeError('No se encontro J7_Servo_pinza; inspeccionar nombre nativo')
    samples=globals().get('POSES17',[dict(name='actual',degrees=None)])
    moved=False
    result=dict(document=app.activeDocument.name,scope=scope,method='Fusion analyzeInterference, sin caras coincidentes. Resultados sin filtrar; requieren clasificacion mecanica.',samples=[],unconnected_lumps=[],restored=False,original_degrees=[math.degrees(v) for v in original])
    for o in r.allOccurrences:
      root=o.fullPathName.split('+')[0].split(':')[0]
      if root.startswith(('80_','81_','82_')):continue
      for b in o.component.bRepBodies:
        if b.isSolid and b.lumps.count>1:result['unconnected_lumps'].append(dict(path=o.fullPathName,body=b.name,lumps=b.lumps.count))
    try:
      if grip_requested is not None:
        absolute=d.userParameters.itemByName('grip_angle').value if grip_requested=='pieza50' else math.radians(float(grip_requested))
        grip.jointMotion.rotationValue=absolute-math.pi/2;grip_moved=True
        result['gripper_requested_absolute_deg']=math.degrees(absolute)
      for sample in samples:
        requested=sample.get('degrees')
        if requested is not None:
          moved=True
          if len(requested)!=6:raise RuntimeError('Una pose requiere seis angulos')
          for joint,degrees in zip(joints,requested):
            theta=math.radians(degrees);lim=joint.jointMotion.rotationLimits
            if lim.isMinimumValueEnabled and theta<lim.minimumValue-1e-8:raise RuntimeError('Pose fuera de limite minimo: '+joint.name)
            if lim.isMaximumValueEnabled and theta>lim.maximumValue+1e-8:raise RuntimeError('Pose fuera de limite maximo: '+joint.name)
            joint.jointMotion.rotationValue=theta
          adsk.doEvents()
        # Aplicar J7 DESPUES de los seis ejes y avanzar por pasos para no
        # saltar de rama del solver en los extremos del mecanismo enlazado.
        chosen=sample.get('gripper_absolute_deg',grip_requested)
        if chosen is not None:
          if not grip:raise RuntimeError('La muestra requiere J7')
          absolute=d.userParameters.itemByName('grip_angle').value if chosen=='pieza50' else math.radians(float(chosen))
          target=absolute-math.pi/2;limits=grip.jointMotion.rotationLimits
          if limits.isMinimumValueEnabled and target<limits.minimumValue-1e-8:raise RuntimeError('Pinza fuera de limite inferior')
          if limits.isMaximumValueEnabled and target>limits.maximumValue+1e-8:raise RuntimeError('Pinza fuera de limite superior')
          start=grip.jointMotion.rotationValue
          steps=max(1,math.ceil(abs(target-start)/math.radians(15)))
          for step in range(1,steps+1):
            grip.jointMotion.rotationValue=start+(target-start)*step/steps;grip_moved=True;adsk.doEvents()
          if abs(grip.jointMotion.rotationValue-target)>1e-5:raise RuntimeError('J7 no alcanzo el angulo solicitado; no considerar valida esta pose')
        entities=adsk.core.ObjectCollection.create();bodies=0;lookup={}
        for o in r.allOccurrences:
          root=o.fullPathName.split('+')[0].split(':')[0]
          if sample.get('exclude_payload') and root.startswith('90_'):continue
          if scope=='J3' and root.startswith(('80_','81_','82_')):continue
          if scope=='arm' and root.startswith(('80_','81_','82_')):continue
          for body in o.bRepBodies:
            if body.isSolid and body.volume>1e-9:
              entities.add(body);bodies+=1
              bb=body.boundingBox
              lookup.setdefault((body.parentComponent.name,body.name),[]).append((o.fullPathName+'/'+body.name,list(bb.minPoint.asArray()),list(bb.maxPoint.asArray())))
        ip=d.createInterferenceInput(entities);ip.areCoincidentFacesIncluded=False
        found=d.analyzeInterference(ip)
        if found is None:raise RuntimeError('Fusion no devolvio resultado de interferencias')
        intersections=[]
        for pair in found:
          solid=pair.interferenceBody
          intersections.append(dict(one=label17(pair.entityOne,lookup,solid),two=label17(pair.entityTwo,lookup,solid),volume_mm3=solid.volume*1000 if solid else None,
            bbox_mm=[10*v for v in list(solid.boundingBox.minPoint.asArray())+list(solid.boundingBox.maxPoint.asArray())] if solid else None))
        row=dict(name=sample['name'],requested_degrees=requested,actual_degrees=[math.degrees(j.jointMotion.rotationValue) for j in joints],gripper_absolute_deg=math.degrees(grip.jointMotion.rotationValue)+90 if grip else None,bodies=bodies,raw_interferences=intersections)
        if grip:
          inverse=next(o for o in r.occurrences if o.component.name=='06_Herramienta').transform2;inverse.invert()
          xs=[]
          for group in ['07_Mandibula_izquierda','08_Mandibula_derecha']:
            o=next(o for o in r.occurrences if o.component.name==group)
            pt=adsk.core.Point3D.create(0,0,0);pt.transformBy(o.transform2);pt.transformBy(inverse);xs.append(pt.x)
          row['jaw_gap_mm']=(xs[1]-xs[0]-d.userParameters.itemByName('finger_t').value-2*d.userParameters.itemByName('pad_t').value)*10
          row['expected_jaw_gap_mm']=(d.userParameters.itemByName('grip_min').value+2*d.userParameters.itemByName('grip_r').value*(grip.jointMotion.rotationValue+math.pi/2))*10
        result['samples'].append(row)
        (H/(name+'_partial.json')).write_text(json.dumps(result,indent=2),encoding='utf-8')
        if grip and abs(row['jaw_gap_mm']-row['expected_jaw_gap_mm'])>.05:
          raise RuntimeError('Separacion real de mordazas distinta del enlace cinematico: '+str(row['jaw_gap_mm']))
        print(json.dumps(dict(pose=sample['name'],interferences=len(intersections))))
    finally:
      if moved:
        for joint,theta in zip(joints,original):joint.jointMotion.rotationValue=theta
        adsk.doEvents()
      if grip_moved:
        grip.jointMotion.rotationValue=grip_original;adsk.doEvents()
      final=[j.jointMotion.rotationValue for j in joints]
      result['final_degrees']=[math.degrees(v) for v in final]
      result['restoration_required']=moved
      # Fusion puede devolver la misma orientacion con vueltas completas
      # diferentes. La comparacion conserva esa equivalencia y la registra.
      result['restore_error_degrees']=[math.degrees(math.remainder(a-b,2*math.pi)) for a,b in zip(final,original)]
      result['restored']=not moved or all(abs(math.remainder(a-b,2*math.pi))<1e-5 for a,b in zip(final,original))
      if grip_moved:
        result['gripper_restore_error_deg']=math.degrees(math.remainder(grip.jointMotion.rotationValue-grip_original,2*math.pi))
        result['restored']=result['restored'] and abs(result['gripper_restore_error_deg'])<math.degrees(1e-5)
      (H/(name+'.json')).write_text(json.dumps(result,indent=2),encoding='utf-8')
    if not result['restored']:raise RuntimeError('No se restauraron todas las poses')
    print(json.dumps(dict(file=name+'.json',samples=len(result['samples']),restored=result['restored'])))
