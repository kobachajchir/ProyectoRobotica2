"""Cierra la vista de la PCB con tapa superior desmontable y guarda la revisión."""
import adsk.core,adsk.fusion,json
ROOT='D:/ProyectoRobotica2/AutoFinalRobo2-CAD'
def run(_context: str):
    app=adsk.core.Application.get();doc=next(x for x in app.documents if x.name=='AutoFinalRobo2');doc.activate()
    d=adsk.fusion.Design.cast(app.activeProduct)
    assembly=next(o.component for o in d.rootComponent.occurrences if o.component.name=='AFR2_UI_A_OLED_Encoder')
    base=next(o.component for o in assembly.occurrences if o.component.name=='Soporte_UI_Base_Pared3mm')
    lid=next(o.component for o in assembly.occurrences if o.component.name=='Bisel_OLED_Desmontable_3mm')
    cfg=json.load(open(ROOT+'/mecanica/pieza-nueva/ui-panel-parameters.json',encoding='utf-8'))
    def sketch_at(c,z,name):
        p=c.constructionPlanes.createInput();p.setByOffset(c.xYConstructionPlane,adsk.core.ValueInput.createByString(str(z)+' mm'))
        sk=c.sketches.add(c.constructionPlanes.add(p));sk.name=name;return sk
    def rect(sk,x1,y1,x2,y2):
        sk.sketchCurves.sketchLines.addTwoPointRectangle(adsk.core.Point3D.create(x1/10,y1/10,0),adsk.core.Point3D.create(x2/10,y2/10,0))
    def extrude(c,sk,h,op,name,ring_only=False):
        coll=adsk.core.ObjectCollection.create()
        for p in sk.profiles:
            if not ring_only or p.profileLoops.count==2:coll.add(p)
        e=c.features.extrudeFeatures.addSimple(coll,adsk.core.ValueInput.createByString(str(h)+' mm'),op);e.name=name;sk.isVisible=False
    if not assembly.attributes.itemByName('AFR2','pcb_cover'):
        join=adsk.fusion.FeatureOperations.JoinFeatureOperation;cut=adsk.fusion.FeatureOperations.CutFeatureOperation
        sk=sketch_at(base,0,'Pies_postes_OLED')
        for x,y in cfg['oled_used_mounts_mm']:
            p=adsk.core.Point3D.create(x/10,y/10,0)
            sk.sketchCurves.sketchCircles.addByCenterRadius(p,.65);sk.sketchCurves.sketchCircles.addByCenterRadius(p,.16)
        extrude(base,sk,3,join,'Pies_de_postes_sin_voladizo',True)
        sk=sketch_at(base,21.5,'Extension_paredes_PCB')
        rect(sk,-3.4,-38.4,-.4,3.4);rect(sk,20.4,-38.4,23.4,3.4);rect(sk,-3.4,-38.4,23.4,-35.4)
        extrude(base,sk,5.9,join,'Paredes_hasta_tapa_PCB')
        sk=sketch_at(lid,27.4,'Cubierta_PCB_3mm')
        rect(sk,-3.4,-38.4,23.4,-5.5)
        extrude(lid,sk,3,join,'Tapa_PCB_unida_al_bisel')
        sk=sketch_at(lid,27.3,'Accesos_encoder_conectores')
        sk.sketchCurves.sketchCircles.addByCenterRadius(adsk.core.Point3D.create(1.1,-2.1,0),.44)
        rect(sk,-.5,-28.1,4.5,-14.1)
        # Alimentación y capacitor comparten una ventana para enchufar desde arriba.
        rect(sk,6.0,-13.7,17.0,-4.8)
        extrude(lid,sk,6.3,cut,'Acceso_eje_header_power')
        assembly.attributes.add('AFR2','pcb_cover','tapa27.4..30.4; eje8.8; J1 y power accesibles; PCB oculta')
    out=ROOT+'/exports/oled-encoder'
    for c,name in [(base,'Soporte_UI_Base'),(lid,'Bisel_OLED')]:
        d.exportManager.execute(d.exportManager.createSTEPExportOptions(out+'/'+name+'.step',c))
        stl=d.exportManager.createSTLExportOptions(c,out+'/'+name+'.stl');stl.meshRefinement=adsk.fusion.MeshRefinementSettings.MeshRefinementMedium;d.exportManager.execute(stl)
    d.exportManager.execute(d.exportManager.createSTEPExportOptions(out+'/Panel_UI_Completo_A.step',assembly))
    print('AUTO_NATIVE',d.exportManager.execute(d.exportManager.createFusionArchiveExportOptions(ROOT+'/mecanica/auto-completo/AutoFinalRobo2-UI-A.f3d')))
    sd=app.documents.add(adsk.core.DocumentTypes.FusionDesignDocumentType);design=adsk.fusion.Design.cast(app.activeProduct);temp=adsk.fusion.TemporaryBRepManager.get()
    for c in [base,lid]:
        cc=design.rootComponent.occurrences.addNewComponent(adsk.core.Matrix3D.create()).component;cc.name=c.name
        bf=cc.features.baseFeatures.add();bf.startEdit()
        for b in c.bRepBodies:cc.bRepBodies.add(temp.copy(b),bf)
        bf.finishEdit();cc.attributes.add('AFR2','parameters',json.dumps(cfg))
    print('SUPPORT_NATIVE',design.exportManager.execute(design.exportManager.createFusionArchiveExportOptions(ROOT+'/mecanica/pieza-nueva/Soporte_UI_A.f3d')))
    doc.activate();app.activeViewport.refresh()
    app.activeViewport.saveAsImageFile(out+'/Panel_UI_Fusion_A.png',1600,1200)
    print('CREATED PCB cover and printable post feet; no interference analysis')
