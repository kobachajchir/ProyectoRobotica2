"""Entrega visual del brazo abierto. No exporta auto, pan ni referencias.

Mallas por solido y manifiesto de instancias; coordenadas locales en metros.
Conserva pose manual abierta y materiales fisicos. Solo cambia apariencias.
"""
import adsk.core, adsk.fusion, json, math
from pathlib import Path

HERE=Path('D:/ProyectoRobotica2/AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/revision_v17/web_blanco')
OUT=Path('D:/ProyectoRobotica2/AutoFinalRobo2-CAD/exports/brazo-v17-blanco-20261031')
EXCLUDE=('80_','81_','82_','90_')
NAME='AFR2_Brazo_v17_Base_V7_J3_Remoto'

def allowed(o):return not o.fullPathName.split('+')[0].startswith(EXCLUDE)

def material(a):
    rgb=[180,180,180]
    for key in ['opaque_albedo','metal_f0','generic_diffuse','transparent_color']:
      p=a.appearanceProperties.itemById(key)
      if p and p.objectType=='adsk::core::ColorProperty':
        rgb=[p.value.red,p.value.green,p.value.blue];break
    metal=any(s in a.name.lower() for s in ['acero','steel','laton','brass','alumin','metal'])
    return dict(name=a.name,srgb=[v/255 for v in rgb],metallic=.75 if metal else 0,roughness=.35 if metal else .55)

def run(context):
    app=adsk.core.Application.get();d=adsk.fusion.Design.cast(app.activeProduct);r=d.rootComponent
    if app.activeDocument.name!=NAME:raise RuntimeError('No es el brazo v17 esperado; no modificar otro documento')
    OUT.mkdir(parents=True,exist_ok=True);parts=OUT/'parts';parts.mkdir(exist_ok=True)
    phase=globals().get('EXPORT_PHASE','inspect')
    if phase=='appearance':
      white=d.appearances.itemByName('AFR2_PLA_PrintALot_Blanco')
      if not white:
        white=d.appearances.addByCopy(d.appearances.itemByName('AFR2_PLA_PrintALot_Rojo'),'AFR2_PLA_PrintALot_Blanco')
        white.appearanceProperties.itemById('opaque_albedo').value=adsk.core.Color.create(242,242,238,255)
      done=set();changed=[]
      for o in r.allOccurrences:
        if not allowed(o) or o.component.entityToken in done:continue
        done.add(o.component.entityToken)
        for b in o.component.bRepBodies:
          if not b.isSolid:continue
          if b.appearance and b.appearance.name in ['AFR2_PLA_PrintALot_Rojo','AFR2_PETG_Rojo']:
            b.appearance=white;changed.append(o.component.name+'/'+b.name)
          for f in b.faces:
            if f.appearance and f.appearance.name in ['AFR2_PLA_PrintALot_Rojo','AFR2_PETG_Rojo']:f.appearance=white
      report=dict(document=NAME,bodies_changed=changed,excluded=list(EXCLUDE),physical_materials_changed=False)
      (HERE/'cambio_apariencias.json').write_text(json.dumps(report,indent=2),encoding='utf-8')
      print(json.dumps(dict(changed=len(changed),appearance=white.name)));return
    if phase=='archive':
      # Resguardo local completo, NO se publica ni importa en Blender/Web.
      target=Path('D:/ProyectoRobotica2/AutoFinalRobo2-CAD/.downloads/brazo-v17-documento-abierto-20261031.f3d')
      if not d.exportManager.execute(d.exportManager.createFusionArchiveExportOptions(str(target))):raise RuntimeError('Fallo F3D blanco')
      print(json.dumps(dict(archive=str(target))));return
    if phase!='export':raise RuntimeError('Indicar appearance/export/archive')
    rootrows={o.component.name:dict(matrix_cm=list(o.transform2.asArray()),visible=o.isVisible) for o in r.occurrences if allowed(o)}
    params={p.name:p.value for p in d.userParameters}
    report=dict(document=NAME,units='m',surface_tolerance_mm=.05,excluded_roots=[o.component.name for o in r.occurrences if not allowed(o)],roots=rootrows,parameters=params,
      joint_positions_rad={j.name:j.jointMotion.rotationValue for j in r.joints if hasattr(j.jointMotion,'rotationValue')},
      sliders_cm={j.name:j.jointMotion.slideValue for j in r.joints if hasattr(j.jointMotion,'slideValue')},definitions=[],instances=[],materials={})
    index={};total=0
    for o in r.allOccurrences:
      if not allowed(o) or not o.isVisible:continue
      for b in o.component.bRepBodies:
        if not b.isSolid or not b.isVisible:continue
        key=b.entityToken
        if key not in index:
          ident='part_%04d'%len(index);index[key]=ident
          faceapps={f.appearance.name for f in b.faces if f.appearance}
          segments=[(b,b.appearance)] if len(faceapps)<=1 else [(f,f.appearance or b.appearance) for f in b.faces]
          with (parts/(ident+'.obj')).open('w',encoding='utf-8') as fp:
            fp.write('mtllib materials.mtl\no '+ident+'\n');offset=0;count=0
            for geom,ap in segments:
              calc=geom.meshManager.createMeshCalculator();calc.surfaceTolerance=.005;calc.maxNormalDeviation=math.radians(12)
              mesh=calc.calculate()
              if not mesh:raise RuntimeError('No se pudo teselar '+b.name)
              matid='mat_'+str(len(report['materials']))
              match=next((k for k,v in report['materials'].items() if v['name']==ap.name),None)
              if match:matid=match
              else:report['materials'][matid]=material(ap)
              co=mesh.nodeCoordinatesAsDouble;no=mesh.normalVectorsAsDouble;ii=mesh.nodeIndices
              for k in range(0,len(co),3):fp.write('v %.8g %.8g %.8g\n'%tuple(v*.01 for v in co[k:k+3]))
              for k in range(0,len(no),3):fp.write('vn %.7g %.7g %.7g\n'%tuple(no[k:k+3]))
              fp.write('usemtl '+matid+'\ns 1\n')
              for k in range(0,len(ii),3):fp.write('f '+' '.join('%d//%d'%(i+offset+1,i+offset+1) for i in ii[k:k+3])+'\n')
              offset+=mesh.nodeCount;count+=mesh.triangleCount
          report['definitions'].append(dict(id=ident,file='parts/'+ident+'.obj',component=o.component.name,body=b.name,triangles=count));total+=count
        report['instances'].append(dict(id=index[key],path=o.fullPathName,root=o.fullPathName.split('+')[0].split(':')[0],component=o.component.name,body=b.name,matrix_cm=list(o.transform2.asArray())))
    with (parts/'materials.mtl').open('w',encoding='utf-8') as fp:
      for key,m in report['materials'].items():fp.write('newmtl '+key+'\nKd '+' '.join(str(v) for v in m['srgb'])+'\nPm '+str(m['metallic'])+'\nPr '+str(m['roughness'])+'\n\n')
    (OUT/'manifest.json').write_text(json.dumps(report,indent=2),encoding='utf-8')
    (HERE/'origen_exportacion.json').write_text(json.dumps(dict(document=NAME,folder=str(OUT),excluded=report['excluded_roots'],definitions=len(index),instances=len(report['instances']),triangles_unique=total,pose_rad=report['joint_positions_rad'],tolerance_mm=.05),indent=2),encoding='utf-8')
    print(json.dumps(dict(definitions=len(index),instances=len(report['instances']),triangles=total,folder=str(OUT))))
