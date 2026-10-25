"""Añade alojamientos M3, importa el STEP de KiCad y guarda archivos locales."""
import adsk.core,adsk.fusion,json,math,os
ROOT='D:/ProyectoRobotica2/AutoFinalRobo2-CAD'
def run(_context: str):
    app=adsk.core.Application.get()
    doc=next(doc for doc in app.documents if doc.name=='AutoFinalRobo2');doc.activate()
    d=adsk.fusion.Design.cast(app.activeProduct)
    cfg=json.load(open(ROOT+'/mecanica/pieza-nueva/ui-panel-parameters.json',encoding='utf-8'))
    parent_occ=next(o for o in d.rootComponent.occurrences if o.component.name==cfg['assembly_name'])
    assembly=parent_occ.component
    base=next(o.component for o in assembly.occurrences if o.component.name=='Soporte_UI_Base_Pared3mm')
    if not base.attributes.itemByName('AFR2','post_diameter'):
        ci=base.constructionPlanes.createInput();ci.setByOffset(base.xYConstructionPlane,adsk.core.ValueInput.createByString('3 mm'))
        sk=base.sketches.add(base.constructionPlanes.add(ci));sk.name='Refuerzo_postes_OLED_D13'
        for x,y in cfg['oled_used_mounts_mm']:
            p=adsk.core.Point3D.create(x/10,y/10,0)
            sk.sketchCurves.sketchCircles.addByCenterRadius(p,.65)
            sk.sketchCurves.sketchCircles.addByCenterRadius(p,.48)
        rings=adsk.core.ObjectCollection.create()
        for p in sk.profiles:
            if p.profileLoops.count==2:rings.add(p)
        base.features.extrudeFeatures.addSimple(rings,adsk.core.ValueInput.createByString('24.05 mm'),adsk.fusion.FeatureOperations.JoinFeatureOperation)
        sk.isVisible=False;base.attributes.add('AFR2','post_diameter','13mm')
    if not base.attributes.itemByName('AFR2','nut_pockets'):
        ci=base.constructionPlanes.createInput();ci.setByOffset(base.xYConstructionPlane,adsk.core.ValueInput.createByString('24.65 mm'))
        sketch=base.sketches.add(base.constructionPlanes.add(ci));sketch.name='Alojamientos_tuerca_M3_AF58'
        for x,y in cfg['oled_used_mounts_mm']:
            r=5.8/math.sqrt(3)
            pts=[adsk.core.Point3D.create((x+r*math.cos(i*math.pi/3))/10,(y+r*math.sin(i*math.pi/3))/10,0) for i in range(6)]
            for i in range(6):sketch.sketchCurves.sketchLines.addByTwoPoints(pts[i],pts[(i+1)%6])
        # Cada hexágono contiene el agujero previo; elegir los contornos exteriores.
        profiles=adsk.core.ObjectCollection.create()
        for p in sketch.profiles:profiles.add(p)
        feat=base.features.extrudeFeatures.addSimple(profiles,adsk.core.ValueInput.createByString('2.4 mm'),adsk.fusion.FeatureOperations.CutFeatureOperation)
        feat.name='Tuercas_cautivas_OLED';sketch.isVisible=False
        base.attributes.add('AFR2','nut_pockets','AF5.8, profundidad2.4, entrada superior; colocar antes del OLED')
    # Este subensamble es una copia regenerable del STEP, no una pieza del auto original.
    for o in list(assembly.occurrences):
        if o.component.name.startswith('PCB_UI_KiCad'):o.deleteMe()
    if not any(o.component.name.startswith('PCB_UI_KiCad') for o in assembly.occurrences):
        opts=app.importManager.createSTEPImportOptions(ROOT+'/exports/oled-encoder/PCB_UI_A.step')
        imported=app.importManager.importToTarget(opts,assembly)
        if not imported:raise RuntimeError('No se pudo importar PCB_UI_A.step')
        occ=assembly.occurrences.item(assembly.occurrences.count-1);occ.component.name='PCB_UI_KiCad_A_20x35'
        occ.component.attributes.add('AFR2','source','kicad/oled-encoder/PlacaOLED-Encoder-AutoRobo2.kicad_pcb; STEP user-origin100x100mm; PCB topZ19.5')
    occ=next(o for o in assembly.occurrences if o.component.name.startswith('PCB_UI_KiCad'))
    if not occ.component.attributes.itemByName('AFR2','placed'):
        proxy=occ.createForAssemblyContext(parent_occ)
        m=parent_occ.transform2.copy();m.translation=adsk.core.Vector3D.create(4.8,4.55,8.8)
        proxy.transform2=m
        occ.component.attributes.add('AFR2','placed','PCB top at localZ19.5; proxy in auto root context')
    if d.snapshots.hasPendingSnapshot:d.snapshots.add()
    out=ROOT+'/exports/oled-encoder'
    d.exportManager.execute(d.exportManager.createSTEPExportOptions(out+'/Panel_UI_Completo_A.step',assembly))
    d.exportManager.execute(d.exportManager.createSTEPExportOptions(out+'/Soporte_UI_Base.step',base))
    mesh=d.exportManager.createSTLExportOptions(base,out+'/Soporte_UI_Base.stl');mesh.meshRefinement=adsk.fusion.MeshRefinementSettings.MeshRefinementMedium;d.exportManager.execute(mesh)
    path=ROOT+'/mecanica/auto-completo/AutoFinalRobo2-UI-A.f3d'
    success=d.exportManager.execute(d.exportManager.createFusionArchiveExportOptions(path))
    print('LOCAL_NATIVE',success,path)
    # Archivo independiente del soporte: Brep editable, parámetros y generador anexos.
    support_doc=app.documents.add(adsk.core.DocumentTypes.FusionDesignDocumentType)
    support_design=adsk.fusion.Design.cast(app.activeProduct)
    temp=adsk.fusion.TemporaryBRepManager.get()
    for source in [base,next(o.component for o in assembly.occurrences if o.component.name=='Bisel_OLED_Desmontable_3mm')]:
        target=support_design.rootComponent.occurrences.addNewComponent(adsk.core.Matrix3D.create()).component;target.name=source.name
        bf=target.features.baseFeatures.add();bf.startEdit()
        for b in source.bRepBodies:target.bRepBodies.add(temp.copy(b),bf)
        bf.finishEdit();target.attributes.add('AFR2','parameters',json.dumps(cfg))
    support_path=ROOT+'/mecanica/pieza-nueva/Soporte_UI_A.f3d'
    print('SUPPORT_NATIVE',support_design.exportManager.execute(support_design.exportManager.createFusionArchiveExportOptions(support_path)),support_path)
    doc.activate()
    print('ASSEMBLY',assembly.name,'in AutoFinalRobo2; cloud save not requested')
    cam=app.activeViewport.camera
    cam.eye=adsk.core.Point3D.create(15,15,21);cam.target=adsk.core.Point3D.create(6,5.2,9.7);cam.upVector=adsk.core.Vector3D.create(0,1,0);cam.isFitView=False;app.activeViewport.camera=cam
    app.activeViewport.refresh()
    app.activeViewport.saveAsImageFile(out+'/Panel_UI_Fusion_A.png',1600,1200)
