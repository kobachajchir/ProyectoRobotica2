# Accionamiento nativo y capturas del resultado. Sin ensayo fisico.
exec(compile(open(r'D:\ProyectoRobotica2\AutoFinalRobo2-CAD\mecanica\brazo-antropomorfo\native_review_v03.py',encoding='utf-8').read(),'native_review_v03.py','exec'))
OUT=OUT.replace('revision_v03','revision_v05')

def run(context):
    app=adsk.core.Application.get();d=adsk.fusion.Design.cast(app.activeProduct);r=d.rootComponent
    if app.activeDocument.name!='AFR2_Brazo_PVC32_AGV_v05':raise RuntimeError('Activar v05')
    if app.userInterface.activeCommand=='OrbitCommand':app.userInterface.terminateActiveCommand()
    d.activateRootComponent();app.userInterface.activeSelections.clear()
    app.activeViewport.visualStyle=adsk.core.VisualStyles.ShadedWithVisibleEdgesOnlyVisualStyle
    for c in d.allComponents:
        for prop in ['isSketchFolderLightBulbOn','isConstructionFolderLightBulbOn','isOriginFolderLightBulbOn','isJointsFolderLightBulbOn']:
            if hasattr(c,prop):setattr(c,prop,False)
    for j in r.joints:j.isLightBulbOn=False
    roots={o.component.name:o for o in r.occurrences};states={n:o.isLightBulbOn for n,o in roots.items()}
    folder=os.path.join(ROOT,'renders','2026-10-27');os.makedirs(folder,exist_ok=True)
    def capture(name,eye,target):
        cam=app.activeViewport.camera;cam.eye=P(*eye);cam.target=P(*target);cam.upVector=adsk.core.Vector3D.create(0,0,1);cam.isFitView=True;app.activeViewport.camera=cam
        app.activeViewport.refresh();adsk.doEvents();app.activeViewport.saveAsImageFile(os.path.join(folder,name+'.png'),1600,1200)
    pose(d,[20,45,45,0,90,0,109.8]);pan=r.joints.itemByName('J8_Pan_HCSR04');rows=[]
    for n,o in roots.items():o.isLightBulbOn=n in ['00_Base_control','80_Auto_referencia','81_Pan_HCSR04']
    for angle in [-90,0,90]:
        pan.jointMotion.rotationValue=math.radians(angle)
        if d.snapshots.hasPendingSnapshot:d.snapshots.add()
        rows.append({'requested_deg':angle,'native_CAD_deg':math.degrees(pan.jointMotion.rotationValue),'transform':list(roots['81_Pan_HCSR04'].transform2.asArray())})
        capture('pan_hcsr04_v05_'+str(angle).replace('-','menos'),(-38,52,33),(-1,2,6))
    pan.jointMotion.rotationValue=0
    if d.snapshots.hasPendingSnapshot:d.snapshots.add()
    capture('anclajes_bilaterales_v05',(-42,-32,28),(-2,0,4))
    for n,o in roots.items():o.isLightBulbOn=states[n]
    roots['82_Envolvente_pan_REFERENCIA'].isLightBulbOn=False
    capture('brazo_agv_v05',(65,80,64),(12,0,20))
    with open(os.path.join(OUT,'extremos_pan_nativos.json'),'w',encoding='utf-8') as f:json.dump(rows,f,indent=2)

