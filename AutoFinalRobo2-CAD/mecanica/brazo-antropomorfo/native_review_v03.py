# Comprobaciones geometricas y vistas pedidas por el usuario en Fusion.
import adsk.core,adsk.fusion,math,json,os
ROOT=r'D:\ProyectoRobotica2\AutoFinalRobo2-CAD'
OUT=os.path.join(ROOT,'mecanica','brazo-antropomorfo','revision_v03')
P=adsk.core.Point3D.create
NAMES=['J1_Giro_base','J2_Hombro','J3_Codo','J4_Giro_muneca','J5_Inclinacion_muneca','J6_Giro_herramienta']

def pose(d,q):
    for nm,angle in zip(NAMES,q):d.rootComponent.joints.itemByName(nm).jointMotion.rotationValue=math.radians(angle)
    d.rootComponent.joints.itemByName('J7_Servo_pinza').jointMotion.rotationValue=math.radians(q[6]-90)
    if d.snapshots.hasPendingSnapshot:d.snapshots.add()

def audit(d):
    r=d.rootComponent;oo={o.component.name:o for o in r.occurrences};rows=[];j=r.joints.itemByName('J7_Servo_pinza')
    for target in [0,30,60,90,109.8,133.7,180,133.7,109.8]:
        j.jointMotion.rotationValue=math.radians(target-90)
        actual=90+math.degrees(j.jointMotion.rotationValue)
        tf=oo['06_Herramienta'].transform2;tf.invert();xs=[]
        for gn in ['07_Mandibula_izquierda','08_Mandibula_derecha']:
            pt=P(0,0,0);pt.transformBy(oo[gn].transform2);pt.transformBy(tf);xs.append(pt.x)
        gap=(xs[1]-xs[0]-d.userParameters.itemByName('finger_t').value-2*d.userParameters.itemByName('pad_t').value)*10
        rows.append({'servo_requested_deg':target,'servo_actual_CAD_deg':actual,'opening_measured_mm':gap})
    with open(os.path.join(OUT,'apertura_nativa_fusion.json'),'w',encoding='utf-8') as f:json.dump(rows,f,indent=2)
    axes=[]
    for i,(lo,hi) in enumerate([(-90,90),(-10,125),(-90,90),(-90,90),(-90,90),(-90,90)]):
        rest=[0,35,-15,0,0,0,109.8]
        if i==1:rest[2]=-90
        if i==2:rest[1]=0
        pose(d,rest)
        for angle in [lo,(lo+hi)/2,hi]:
            jj=r.joints.itemByName(NAMES[i]);jj.jointMotion.rotationValue=math.radians(angle)
            actual=math.degrees(jj.jointMotion.rotationValue)
            axes.append({'axis':i+1,'requested_deg':angle,'actual_CAD_deg':actual,'error_deg':actual-angle,'rest':rest})
    with open(os.path.join(OUT,'extremos_nativos_fusion.json'),'w',encoding='utf-8') as f:json.dump(axes,f,indent=2)
    pose(d,[20,45,45,0,90,0,109.8])
    print('APERTURA',json.dumps(rows),'EJES_ERROR_MAX',max(abs(x['error_deg']) for x in axes))

def gallery(app,d):
    if app.userInterface.activeCommand=='OrbitCommand':app.userInterface.terminateActiveCommand()
    d.activateRootComponent();app.userInterface.activeSelections.clear()
    app.activeViewport.visualStyle=adsk.core.VisualStyles.ShadedWithVisibleEdgesOnlyVisualStyle
    r=d.rootComponent;folder=os.path.join(ROOT,'renders','2026-10-27');os.makedirs(folder,exist_ok=True)
    for c in d.allComponents:
        for prop in ['isSketchFolderLightBulbOn','isConstructionFolderLightBulbOn','isOriginFolderLightBulbOn','isJointsFolderLightBulbOn']:
            if hasattr(c,prop):setattr(c,prop,False)
    for j in r.joints:
        if hasattr(j,'isLightBulbOn'):j.isLightBulbOn=False
    oo={o.component.name:o for o in r.occurrences};states={n:o.isLightBulbOn for n,o in oo.items()}
    def capture(name,eye,target,up=(0,0,1)):
        cam=app.activeViewport.camera;cam.eye=P(*eye);cam.target=P(*target);cam.upVector=adsk.core.Vector3D.create(*up);cam.isFitView=True;app.activeViewport.camera=cam
        app.activeViewport.refresh();adsk.doEvents();app.activeViewport.saveAsImageFile(os.path.join(folder,name+'.png'),1600,1200)
    pose(d,[20,45,45,0,90,0,109.8]);capture('brazo_agv_v03',(65,80,58),(12,0,18))
    for n,o in oo.items():o.isLightBulbOn=n in ['00_Base_control','80_Auto_referencia']
    capture('montura_electronica_v03',(-40,55,38),(0,0,4))
    for n,o in oo.items():o.isLightBulbOn=n in ['06_Herramienta','07_Mandibula_izquierda','08_Mandibula_derecha','10_Pinion_pinza','90_Pieza_REFERENCIA_50mm_100g']
    tf=oo['06_Herramienta'].transform2
    eye=P(16,-20,14);target=P(0,0,5.5);up=adsk.core.Vector3D.create(0,0,1)
    eye.transformBy(tf);target.transformBy(tf);up.transformBy(tf)
    for theta,nm in [(133.7,'pinza_v03_abierta_60mm'),(109.8,'pinza_v03_cerrada_50mm')]:
        r.joints.itemByName('J7_Servo_pinza').jointMotion.rotationValue=math.radians(theta-90)
        capture(nm,eye.asArray(),target.asArray(),up.asArray())
    for n,o in oo.items():o.isLightBulbOn=states[n]
    if globals().get('PICKUP',False):
        pose(d,[0,125,-12.5,0,67.5,0,109.8]);capture('alcance_suelo_agv_v03',(65,80,48),(10,0,10))
    pose(d,[20,45,45,0,90,0,109.8]);capture('brazo_agv_v03',(65,80,58),(12,0,18))
    print('VISTAS',folder)

def run(context):
    app=adsk.core.Application.get();d=adsk.fusion.Design.cast(app.activeProduct)
    if app.activeDocument.name!='AFR2_Brazo_PVC32_AGV_v03':raise RuntimeError('Activar maestro v03')
    os.makedirs(OUT,exist_ok=True)
    if globals().get('MODE','audit')=='gallery':gallery(app,d)
    else:audit(d)
