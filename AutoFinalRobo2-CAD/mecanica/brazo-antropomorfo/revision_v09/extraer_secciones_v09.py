"""Lectura CAD v09 para comprobacion de J1. Solo BRep temporales, sin modificar Fusion."""
import adsk.core,adsk.fusion,json,os
OUT=r'D:\ProyectoRobotica2\AutoFinalRobo2-CAD\mecanica\brazo-antropomorfo\revision_v09'

def run(_context: str):
    app=adsk.core.Application.get();d=adsk.fusion.Design.cast(app.activeProduct)
    if app.activeDocument.name!='AFR2_Brazo_PVC32_AGV_v09_J1_GX3270BLS':raise RuntimeError('Activar v09')
    tm=adsk.fusion.TemporaryBRepManager.get();rows=[];meta=[]
    specs=[('V03_E00_Husillo_base_completo',[-32.4,-31.5,-29,-28,-27,-24,-20,-16,-12,-9,-7,-5.5,-3.5,-2]),
      ('V05_Montura_baja_PETG',[12.1,13,20,30,35,38,40,43,44,46,49,52,56,59]),
      ('V09_Cartucho6000_desmontable_PETG',[60.2,63,65.2,67,68.2,70,73,77,80,83,86,88]),
      ('A01_Plataforma_giratoria_PETG',[1.5,4.5,7.5,12,35,65,95,110]),
      ('V06_Contraplaca_inferior_unica_PETG',[-4.5])]
    for cn,levels in specs:
        c=next(c for c in d.allComponents if c.name==cn);b=c.bRepBodies.item(0)
        for requested_z in levels:
            # Avoid coincident planar faces, which add tangent/internal wires.
            z=requested_z+.0137
            wire=tm.planeIntersection(b,adsk.core.Plane.create(adsk.core.Point3D.create(0,0,z/10),adsk.core.Vector3D.create(0,0,1)))
            edges=[]
            if wire:
                for e in wire.edges:
                    ok,a,bb=e.evaluator.getParameterExtents()
                    if not ok:raise RuntimeError('Curva sin parametros')
                    ok,pts=e.evaluator.getStrokes(a,bb,.00025)
                    if not ok:raise RuntimeError('Curva sin discretizacion')
                    edges.append([[p.x*10,p.y*10,p.z*10] for p in pts])
            rows.append({'component':cn,'z_mm':z,'requested_z_mm':requested_z,
              'chord_tolerance_mm':.0025,'edges':edges})
    for o in d.rootComponent.allOccurrences:
        cn=o.component.name
        if cn.startswith(('V09_','V03_R01_608_J1','V05_Anclaje','V03_Fijacion_servo_J1','V03_Inserto_M3_servo_J1','V03_Tornillo_tapa_J1','V03_Inserto_M3_J1')) or cn=='V03_M01_MG946_Base':
            meta.append({'component':cn,'root':o.fullPathName.split('+')[0].split(':')[0],
              'transform_cm':list(o.transform2.asArray()),'bodies':[{'name':b.name,'volume_mm3':b.volume*1000,
              'bbox_mm':[v*10 for v in list(b.boundingBox.minPoint.asArray())+list(b.boundingBox.maxPoint.asArray())]} for b in o.component.bRepBodies]})
    p={p.name:{'expression':p.expression,'value_internal':p.value} for p in d.userParameters}
    os.makedirs(OUT,exist_ok=True)
    json.dump({'document':app.activeDocument.name,'modified':app.activeDocument.isModified,
      'scope':'Secciones geometricas locales, no FEA. mm y tolerancia de cuerda explicita.',
      'sections':rows,'mounts':meta,'parameters':p},open(os.path.join(OUT,'secciones_CAD_v09.json'),'w',encoding='utf-8'),ensure_ascii=False,indent=2)
    print(json.dumps({'sections':len(rows),'mount_components':len(meta),'modified':app.activeDocument.isModified}))
