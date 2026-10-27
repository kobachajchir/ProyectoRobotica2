# Accionamiento nativo de limites/apertura y vistas armadas; sin despieces.
exec(compile(open(r'D:\ProyectoRobotica2\AutoFinalRobo2-CAD\mecanica\brazo-antropomorfo\native_review_v03.py',encoding='utf-8').read(),'native_review_v03.py','exec'))
OUT=OUT.replace('revision_v03','revision_v06')

def gallery_v06(app,d):
    d.activateRootComponent();app.userInterface.activeSelections.clear()
    app.activeViewport.visualStyle=adsk.core.VisualStyles.ShadedWithVisibleEdgesOnlyVisualStyle
    for c in d.allComponents:
        for prop in ['isSketchFolderLightBulbOn','isConstructionFolderLightBulbOn','isOriginFolderLightBulbOn','isJointsFolderLightBulbOn']:
            if hasattr(c,prop):setattr(c,prop,False)
    r=d.rootComponent
    for j in r.joints:j.isLightBulbOn=False
    roots={o.component.name:o for o in r.occurrences};states={n:o.isLightBulbOn for n,o in roots.items()}
    folder=os.path.join(ROOT,'renders','2026-10-27');os.makedirs(folder,exist_ok=True)
    def capture(name,eye,target,up=(0,0,1)):
        cam=app.activeViewport.camera;cam.eye=P(*eye);cam.target=P(*target);cam.upVector=adsk.core.Vector3D.create(*up);cam.isFitView=True;app.activeViewport.camera=cam
        app.activeViewport.refresh();adsk.doEvents();app.activeViewport.saveAsImageFile(os.path.join(folder,name+'.png'),1600,1200)
    pose(d,[20,45,45,0,90,0,109.8])
    pan=r.joints.itemByName('J8_Pan_HCSR04');pan.jointMotion.rotationValue=0
    if d.snapshots.hasPendingSnapshot:d.snapshots.add()
    capture('brazo_agv_v06',(65,80,55),(12,0,17))
    for n,o in roots.items():o.isLightBulbOn=n in ['00_Base_control','80_Auto_referencia','81_Pan_HCSR04']
    capture('montura_baja_v06',(-40,55,30),(-1,0,3))
    capture('contraplaca_inferior_v06',(-36,-42,-35),(-1,-1,-.4))
    panrows=[]
    for angle in [-90,0,90]:
        pan.jointMotion.rotationValue=math.radians(angle)
        if d.snapshots.hasPendingSnapshot:d.snapshots.add()
        panrows.append({'command_deg':angle,'CAD_deg':math.degrees(pan.jointMotion.rotationValue),'transform':list(roots['81_Pan_HCSR04'].transform2.asArray())})
    pan.jointMotion.rotationValue=0
    if d.snapshots.hasPendingSnapshot:d.snapshots.add()
    for n,o in roots.items():o.isLightBulbOn=states[n]
    roots['82_Envolvente_pan_REFERENCIA'].isLightBulbOn=False
    capture('brazo_agv_v06',(65,80,55),(12,0,17))
    json.dump(panrows,open(os.path.join(OUT,'extremos_pan_nativos.json'),'w',encoding='utf-8'),indent=2)

def run(context):
    app=adsk.core.Application.get();d=adsk.fusion.Design.cast(app.activeProduct)
    if app.activeDocument.name!='AFR2_Brazo_PVC32_AGV_v06':raise RuntimeError('Activar v06')
    os.makedirs(OUT,exist_ok=True)
    if globals().get('MODE','audit')=='gallery':gallery_v06(app,d)
    else:audit(d)
