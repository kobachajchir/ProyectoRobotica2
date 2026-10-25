"""Construye el soporte editable y las fijaciones de referencia en AutoFinalRobo2.

Parámetros en ui-panel-parameters.json; milímetros convertidos a cm de Fusion.
No ejecuta validadores, simulaciones ni análisis de interferencias.
"""
import adsk.core, adsk.fusion, json, math, os

ROOT='D:/ProyectoRobotica2/AutoFinalRobo2-CAD'
def run(_context: str):
    cfg=json.load(open(ROOT+'/mecanica/pieza-nueva/ui-panel-parameters.json',encoding='utf-8'))
    app=adsk.core.Application.get()
    doc=next(doc for doc in app.documents if doc.name=='AutoFinalRobo2')
    doc.activate()
    d=adsk.fusion.Design.cast(app.activeProduct);root=d.rootComponent
    if any(o.component.name==cfg['assembly_name'] for o in root.occurrences):
        raise RuntimeError('El conjunto ya existe; editarlo antes de regenerar, no duplicar.')
    temp=adsk.fusion.TemporaryBRepManager.get()
    def point(x,y,z):return adsk.core.Point3D.create(x/10,y/10,z/10)
    def box(x1,y1,z1,x2,y2,z2):
        ob=adsk.core.OrientedBoundingBox3D.create(point((x1+x2)/2,(y1+y2)/2,(z1+z2)/2),adsk.core.Vector3D.create(1,0,0),adsk.core.Vector3D.create(0,1,0),(x2-x1)/10,(y2-y1)/10,(z2-z1)/10)
        return temp.createBox(ob)
    def cyl(x,y,z1,z2,r):return temp.createCylinderOrCone(point(x,y,z1),r/10,point(x,y,z2),r/10)
    def boolean(a,b,op):
        if not temp.booleanOperation(a,b,op):raise RuntimeError('Operacion de construccion no realizada')
        return a
    def cut(a,b):return boolean(a,b,adsk.fusion.BooleanTypes.DifferenceBooleanType)
    def join(a,b):return boolean(a,b,adsk.fusion.BooleanTypes.UnionBooleanType)
    def component(parent,name):
        o=parent.occurrences.addNewComponent(adsk.core.Matrix3D.create());o.component.name=name
        o.component.attributes.add('AFR2','change_id',cfg['change_id'])
        return o.component
    def material(name,rgb):
        a=d.appearances.itemByName(name)
        if not a:
            source=next(a for a in d.appearances if a.appearanceProperties.itemById('opaque_albedo'))
            a=d.appearances.addByCopy(source,name)
            adsk.core.ColorProperty.cast(a.appearanceProperties.itemById('opaque_albedo')).value=adsk.core.Color.create(*rgb,255)
        return a
    black=material('AFR2_UI_PLA_oscuro',(42,45,48))
    brass=material('AFR2_UI_Laton',(180,145,60))
    steel=material('AFR2_UI_Acero',(150,154,160))
    parent_occ=root.occurrences.addNewComponent(adsk.core.Matrix3D.create())
    assembly=parent_occ.component;assembly.name=cfg['assembly_name']
    assembly.description='Panel OLED/encoder; PCB 20x35, torretas M3x15, soporte de pared 3mm; referencia sin validaciones.'
    assembly.attributes.add('AFR2','parameters',json.dumps(cfg))
    def persist(c,name,body,appearance):
        base=c.features.baseFeatures.add();base.name=name;base.startEdit()
        result=c.bRepBodies.add(body,base);base.finishEdit();result.name=name
        if appearance:result.appearance=appearance
        return result
    # Bastidor inferior abierto, con travesano para las torretas.
    support=box(-3.4,-38.4,0,23.4,3.4,3)
    cut(support,box(0,-28.5,-1,20,.4,4))
    display_floor=box(-7.8,-7.5,0,27.8,27.5,3)
    cut(display_floor,box(-4.8,-4.5,-1,24.8,24.5,4))
    join(support,display_floor)
    # Tornillos al chasis sobre la ranura: dos centros separados 8,1 mm.
    a,b=cfg['auto_mounts_mm'];cx=(a[0]+b[0])/2;cy=(a[1]+b[1])/2
    v=adsk.core.Vector3D.create((b[0]-a[0]),(b[1]-a[1]),0);length=v.length;v.normalize()
    normal=adsk.core.Vector3D.create(-v.y,v.x,0)
    rib=temp.createBox(adsk.core.OrientedBoundingBox3D.create(point(cx,cy,1.5),v,normal,(length+1)/10,.6,.3))
    join(support,rib)
    for x,y in cfg['auto_mounts_mm']:join(support,cyl(x,y,0,3,4.6))
    # Paredes PCB. La parte superior rodea el borde, con juego lateral 0,4mm.
    for shape in [box(-3.4,-38.4,3,-.4,3.4,21.5),box(20.4,-38.4,3,23.4,3.4,21.5),box(-3.4,-38.4,3,23.4,-35.4,21.5)]:join(support,shape)
    # Laterales y extremo del OLED; el frente queda abierto para socket/cables.
    for shape in [box(-7.8,-7.5,3,-4.8,27.5,30.4),box(24.8,-7.5,3,27.8,27.5,30.4),box(-7.8,24.5,3,27.8,27.5,30.4)]:join(support,shape)
    # Dos apoyos superiores OLED: los otros agujeros quedan libres, sin invadir PCB.
    for x,y in cfg['oled_used_mounts_mm']:join(support,cyl(x,y,3,cfg['oled_back_z_mm'],6.5))
    for x,y in cfg['auto_mounts_mm']+cfg['pcb_mounts_mm']+cfg['oled_used_mounts_mm']:
        cut(support,cyl(x,y,-1,35,1.6))
    basecomp=component(assembly,'Soporte_UI_Base_Pared3mm')
    basebody=persist(basecomp,'Bastidor_abierto_pared3',support,black)
    basecomp.attributes.add('AFR2','post_diameter','13mm')
    # Bisel desmontable: se imprime plano y permite insertar el OLED desde arriba.
    bezel=box(-7.8,-7.5,30.4,27.8,27.5,33.4)
    cut(bezel,box(-3.5,.19,29.4,23.5,20.01,34.4))
    for x,y in cfg['oled_used_mounts_mm']:
        join(bezel,cyl(x,y,28.55,30.4,3.2))
        cut(bezel,cyl(x,y,27,35,1.6))
    lidcomp=component(assembly,'Bisel_OLED_Desmontable_3mm')
    lidbody=persist(lidcomp,'Bisel_superior',bezel,black)
    # Bisel de 0,6mm en los cantos rectos superiores de la tapa.
    edges=adsk.core.ObjectCollection.create()
    for e in lidbody.edges:
        if e.geometry.objectType==adsk.core.Line3D.classType() and e.startVertex and e.endVertex:
            p=e.startVertex.geometry;q=e.endVertex.geometry
            outer=any(abs(p.x-v)<1e-5 and abs(q.x-v)<1e-5 for v in [-.78,2.78]) or any(abs(p.y-v)<1e-5 and abs(q.y-v)<1e-5 for v in [-.75,2.75])
            if outer and abs(p.z-3.34)<1e-5 and abs(q.z-3.34)<1e-5:edges.add(e)
    ci=lidcomp.features.chamferFeatures.createInput(edges,True)
    ci.setToEqualDistance(adsk.core.ValueInput.createByString('0.6 mm'))
    lidcomp.features.chamferFeatures.add(ci).name='Bisel_06mm_45grados'
    # Torretas hexagonales de referencia M3, altura 15mm.
    def hex_component(name,x,y,z,af,height,appearance,hole=1.5):
        c=component(assembly,name)
        plane=c.constructionPlanes.createInput();plane.setByOffset(c.xYConstructionPlane,adsk.core.ValueInput.createByReal(z/10))
        sk=c.sketches.add(c.constructionPlanes.add(plane));sk.name='Hexagono_AF_'+str(af)
        radius=af/math.sqrt(3)
        pts=[point(x+radius*math.cos(i*math.pi/3),y+radius*math.sin(i*math.pi/3),0) for i in range(6)]
        for i in range(6):sk.sketchCurves.sketchLines.addByTwoPoints(pts[i],pts[(i+1)%6])
        ext=c.features.extrudeFeatures.addSimple(sk.profiles.item(0),adsk.core.ValueInput.createByString(str(height)+' mm'),adsk.fusion.FeatureOperations.NewBodyFeatureOperation)
        body=ext.bodies.item(0);body.appearance=appearance
        hole_sk=c.sketches.add(c.constructionPlanes.item(c.constructionPlanes.count-1))
        hole_sk.sketchCurves.sketchCircles.addByCenterRadius(point(x,y,0),hole/10)
        c.features.extrudeFeatures.addSimple(hole_sk.profiles.item(0),adsk.core.ValueInput.createByString(str(height)+' mm'),adsk.fusion.FeatureOperations.CutFeatureOperation)
        sk.isVisible=False;hole_sk.isVisible=False
        return c
    for i,(x,y) in enumerate(cfg['pcb_mounts_mm'],1):
        hex_component('Torreta_Laton_M3x15_'+str(i),x,y,3,5.5,15,brass)
        screwcomp=component(assembly,'Tornillos_PCB_M3x6_'+str(i))
        persist(screwcomp,'Superior_Cabeza5',join(cyl(x,y,13.5,19.5,1.5),cyl(x,y,19.5,21.5,2.5)),steel)
        persist(screwcomp,'Inferior',join(cyl(x,y,0,6,1.5),cyl(x,y,-2,0,2.5)),steel)
    for i,(x,y) in enumerate(cfg['auto_mounts_mm'],1):
        sc=component(assembly,'Tornillo_Chasis_M3x10_'+str(i))
        persist(sc,'M3x10',join(cyl(x,y,-7,3,1.5),cyl(x,y,3,5,2.5)),steel)
        hex_component('Tuerca_Chasis_M3_'+str(i),x,y,-5.4,5.5,2.4,steel)
    for i,(x,y) in enumerate(cfg['oled_used_mounts_mm'],1):
        sc=component(assembly,'Tornillo_Bisel_OLED_M3x12_'+str(i))
        persist(sc,'M3x12_Cabeza5',join(cyl(x,y,21.4,33.4,1.5),cyl(x,y,33.4,35.4,2.5)),steel)
        hex_component('Tuerca_OLED_M3_'+str(i),x,y,24.65,5.5,2.4,steel)
    m=adsk.core.Matrix3D.create();x,y,z=cfg['origin_in_auto_mm']
    m.setWithArray([1,0,0,x/10,0,0,1,y/10,0,-1,0,z/10,0,0,0,1]);parent_occ.transform2=m
    if d.snapshots.hasPendingSnapshot:d.snapshots.add()
    assembly.attributes.add('AFR2','state','Diseno; sin analisis de interferencias ni prueba fisica; ranura M3 pendiente')
    export=ROOT+'/exports/oled-encoder';os.makedirs(export,exist_ok=True)
    for c,name in [(basecomp,'Soporte_UI_Base'),(lidcomp,'Bisel_OLED')]:
        d.exportManager.execute(d.exportManager.createSTEPExportOptions(export+'/'+name+'.step',c))
        mesh=d.exportManager.createSTLExportOptions(c,export+'/'+name+'.stl');mesh.meshRefinement=adsk.fusion.MeshRefinementSettings.MeshRefinementMedium;d.exportManager.execute(mesh)
    native=ROOT+'/mecanica/pieza-nueva/Soporte_UI_A.f3d'
    print('NATIVE',d.exportManager.execute(d.exportManager.createFusionArchiveExportOptions(native,assembly)))
    print('CREATED',assembly.name,'base and bezel','M3 standoffs 15mm','origin',cfg['origin_in_auto_mm'])
