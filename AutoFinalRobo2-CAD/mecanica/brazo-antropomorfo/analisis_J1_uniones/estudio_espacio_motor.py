"""Estudio temporal de bajar motor J1 conservando plataforma. No modifica CAD."""
import adsk.core,adsk.fusion,json,os
OUT=r'D:\ProyectoRobotica2\AutoFinalRobo2-CAD\mecanica\brazo-antropomorfo\analisis_J1_uniones'

def run(_context: str):
    app=adsk.core.Application.get();d=adsk.fusion.Design.cast(app.activeProduct);tm=adsk.fusion.TemporaryBRepManager.get()
    if app.activeDocument.name!='AFR2_Brazo_PVC32_AGV_v08_Housing':raise RuntimeError('Activar v08')
    occs=list(d.rootComponent.allOccurrences);motor=next(o for o in occs if o.component.name=='V03_M01_MG946_Base')
    def world(b,o):return tm.copy(b.createForAssemblyContext(o))
    fixed=[]
    for o in occs:
        n=o.component.name;root=o.fullPathName.split('+')[0].split(':')[0]
        if root in ['80_Auto_referencia','81_Pan_HCSR04','82_Envolvente_pan_REFERENCIA'] or n.startswith(('V03_XL4016_vertical_','V03_Nano_vertical','V03_PCA9685_vertical','V03_Bornera_',
          'V03_Casquillo_central_','V05_M10x30','V05_Anclaje_M3x25_','V03_Tuerca_arandela_')):
            for b in o.component.bRepBodies:
                if b.isSolid and b.volume>1e-10:fixed.append((n,world(b,o)))
    def overlap(a,b):
        aa=a.boundingBox;bb=b.boundingBox
        return all(min(aa.maxPoint.asArray()[i],bb.maxPoint.asArray()[i])-max(aa.minPoint.asArray()[i],bb.minPoint.asArray()[i])>1e-7 for i in range(3))
    rows=[]
    for dz in [0,-8,-16]:
        hits=[];m=adsk.core.Matrix3D.create();m.translation=adsk.core.Vector3D.create(0,0,dz/10)
        for b in motor.component.bRepBodies:
            if not b.isSolid:continue
            cp=world(b,motor);tm.transform(cp,m)
            for name,other in fixed:
                if not overlap(cp,other):continue
                check=tm.copy(cp)
                if tm.booleanOperation(check,other,adsk.fusion.BooleanTypes.IntersectionBooleanType) and check.volume*1000>.001:
                    hits.append({'motor_body':b.name,'fixed':name,'volume_mm3':check.volume*1000})
        rows.append({'motor_translation_z_mm':dz,'hits':hits,
          'nominal_case_bottom_mm':21.1+dz,'existing_mount_top_mm':12,
          'crosses_existing_mount_floor':21.1+dz<12})
    result={'scope':'Solo motor MG946 v08 desplazado contra auto/electronica/anclajes/pan. Excluye piezas J1 a redisenar. No modela nuevo servo ni movimiento futuro.',
      'fixed_bodies':len(fixed),'translation_only':True,'native_modified':app.activeDocument.isModified,'rows':rows}
    # Virtual GX3270BLS from its native elbow component, rotated to output +Z.
    # No component is created in the document. C flush is a new standard-head
    # envelope, not a trimmed commercial part or an edit of the actual chassis.
    gx=next(c for c in d.allComponents if c.name=='V07_M03_GX3270BLS_Codo')
    p=lambda n:d.userParameters.itemByName(n).value
    candidates=[]
    for face in [56,58]:
        m=adsk.core.Matrix3D.create()
        m.setWithArray([1,0,0,0,0,0,1,p('v05_axis_y')-p('link1'),0,-1,0,face/10+p('v07_case_front_y'),0,0,0,1])
        for flush in [False,True]:
            targets=[(n,b) for n,b in fixed if not (flush and n=='V05_Anclaje_M3x25_C')]
            if flush:
                xx=p('agv_C_x');yy=p('agv_C_y')
                # Böllhoff ISO10642 M3: head max Ø6.72, height1.86, 90deg.
                head=tm.createCylinderOrCone(adsk.core.Point3D.create(xx,yy,1.014),.15,
                  adsk.core.Point3D.create(xx,yy,1.2),.336)
                stem=tm.createCylinderOrCone(adsk.core.Point3D.create(xx,yy,-1.3),.15,
                  adsk.core.Point3D.create(xx,yy,1.014),.15)
                targets.extend([('C_ISO10642_cabeza_al_ras_REF',head),('C_ISO10642_vastago_REF',stem)])
            hits=[]
            for b in gx.bRepBodies:
                cp=tm.copy(b);tm.transform(cp,m)
                for name,other in targets:
                    if not overlap(cp,other):continue
                    test=tm.copy(cp)
                    if tm.booleanOperation(test,other,adsk.fusion.BooleanTypes.IntersectionBooleanType) and test.volume*1000>.001:
                        hits.append({'fixed':name,'volume_mm3':test.volume*1000})
            candidates.append({'servo':'GX3270BLS','body_front_z_mm':face,
              'case_bottom_z_mm':face-p('v07_case_h')*10,'C_head_flush':flush,'hits':hits})
    result['GX_candidates_no_CAD_changes']=candidates
    result['candidate_scope']='Solo envolvente contra piezas que se conservarian; nueva montura/coupling/eje/apoyos y barrido futuros no incluidos.'
    json.dump(result,open(os.path.join(OUT,'espacio_bajar_motor.json'),'w',encoding='utf-8'),ensure_ascii=False,indent=2)
    print(json.dumps(result))
