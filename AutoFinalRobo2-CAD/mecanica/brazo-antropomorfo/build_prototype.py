# AutoFinalRobo2 - prototipo antropomorfo PVC, 26/10/2026.
# Ejecutar dentro de Autodesk Fusion mediante su MCP. No ejecuta ensayos ni validadores.
import adsk.core, adsk.fusion, math, json, os, traceback

ROOT_DIR = r'D:\ProyectoRobotica2\AutoFinalRobo2-CAD'
OUT_DIR = os.path.join(ROOT_DIR, 'mecanica', 'brazo-antropomorfo')
MODEL_NAME = 'AFR2_Brazo_PVC32_Prototipo_v01'
P = adsk.core.Point3D.create
V = adsk.core.ValueInput.createByString
OP = adsk.fusion.FeatureOperations
ORI = adsk.fusion.DimensionOrientations
R = None
D = None
AP = {}
LOG = []

PARAMETERS = [
('pvc_od','32 mm','mm','USUARIO: diametro exterior de la foto.'),
('pvc_id','28.3 mm','mm','USUARIO: diametro interior de la foto.'),
('pvc_wall','(pvc_od-pvc_id)/2','mm','DERIVADO: 1.85 mm. C=3.2 de la foto es incompatible con A/B; no usado.'),
('link1','130 mm','mm','PROPUESTO: distancia entre ejes hombro-codo.'),
('link2','120 mm','mm','PROPUESTO: distancia codo-J4.'),
('shoulder_z','120 mm','mm','PROPUESTO: altura del hombro respecto de la mesa.'),
('base_w','160 mm','mm','PROPUESTO: base cuadrada.'),
('base_h','55 mm','mm','PROPUESTO: caja de electronica.'),
('base_wall','4 mm','mm','PROPUESTO: paredes de caja.'),
('base_floor','4 mm','mm','PROPUESTO: piso de caja.'),
('lid_t','3 mm','mm','PROPUESTO: tapa desmontable.'),
('base_top','base_h+lid_t','mm','DERIVADO: interfaz giratoria J1.'),
('tower_h','shoulder_z-base_top','mm','DERIVADO: centro del hombro en conjunto J1.'),
('socket_clear','0.35 mm','mm','PROPUESTO: holgura diametral inicial PVC, sin calibrar impresion.'),
('socket_wall','3 mm','mm','PROPUESTO: pared radial de encastre.'),
('socket_od','pvc_od+socket_clear+2*socket_wall','mm','DERIVADO.'),
('socket_start','20 mm','mm','PROPUESTO: inicio del manguito respecto del eje.'),
('socket_length','22 mm','mm','PROPUESTO: largo axial del manguito.'),
('tube_inset','25 mm','mm','PROPUESTO: extremo de corte respecto del eje.'),
('tube1_cut','link1-2*tube_inset','mm','DERIVADO: largo de corte PVC 1.'),
('tube2_cut','link2-2*tube_inset','mm','DERIVADO: largo de corte PVC 2.'),
('socket_overlap','socket_start+socket_length-tube_inset','mm','DERIVADO: solape PVC-manguito.'),
('fit_clear','0.25 mm','mm','PROPUESTO: holgura diametral alojamientos rodamientos.'),
('servo_clear','0.6 mm','mm','PROPUESTO: holgura de caja servo.'),
('housing_wall','3 mm','mm','PROPUESTO: pared housing.'),
('fork_y','28 mm','mm','PROPUESTO: centros de los apoyos de hombro y codo.'),
('fork_t','9 mm','mm','PROPUESTO: espesor orejas grandes.'),
('b608_id','8 mm','mm','CATALOGO SKF 608-2Z: diametro interior.'),
('b608_od','22 mm','mm','CATALOGO SKF 608-2Z: diametro exterior.'),
('b608_w','7 mm','mm','CATALOGO SKF 608-2Z: ancho.'),
('b625_id','5 mm','mm','CATALOGO SKF 625-2Z: diametro interior.'),
('b625_od','16 mm','mm','CATALOGO SKF 625-2Z: diametro exterior.'),
('b625_w','5 mm','mm','CATALOGO SKF 625-2Z: ancho.'),
('gx_l','40 mm','mm','CATALOGO familia GX3240MG; variante V1 por confirmar.'),
('gx_w','20 mm','mm','CATALOGO familia GX3240MG.'),
('gx_h','40.5 mm','mm','CATALOGO familia GX3240MG; envolvente.'),
('ds_l','40 mm','mm','DATASHEET DS3218.'),
('ds_w','20 mm','mm','DATASHEET DS3218.'),
('ds_h','40.5 mm','mm','DATASHEET DS3218, envolvente.'),
('mg_l','40.7 mm','mm','FABRICANTE MG946R.'),
('mg_w','19.7 mm','mm','FABRICANTE MG946R.'),
('mg_h','42.9 mm','mm','FABRICANTE MG946R, envolvente.'),
('micro_l','22.8 mm','mm','FABRICANTE MG90S.'),
('micro_w','12.2 mm','mm','FABRICANTE MG90S.'),
('micro_h','28.5 mm','mm','FABRICANTE MG90S, envolvente.'),
('std_shaft_x','10 mm','mm','REFERENCIA de dibujo servo estandar; confirmar variante antes de fabricar.'),
('micro_shaft_x','5.5 mm','mm','INFERIDO: posicion de salida MG90S, editable.'),
('std_ear','7 mm','mm','REFERENCIA: pestaña de servo estandar, editable.'),
('std_ear_z','8 mm','mm','INFERIDO: plano de pestañas bajo cara superior.'),
('std_hole_pitch_x','49 mm','mm','REFERENCIA: patron longitudinal estandar; confirmar cada variante.'),
('std_hole_pitch_y','10 mm','mm','REFERENCIA: patron transversal estandar; confirmar cada variante.'),
('std_mount_d','4.2 mm','mm','PROPUESTO: paso M4.'),
('std_horn_d','22 mm','mm','REFERENCIA: plato/horn simplificado.'),
('std_spline_d','6 mm','mm','REFERENCIA: envolvente de spline; no genera estrias comerciales.'),
('gear_module','1.5 mm','mm','PROPUESTO: engranajes rectos de referencia.'),
('pinion_teeth','16','','PROPUESTO: dientes pinon.'),
('wheel_teeth','32','','PROPUESTO: dientes rueda.'),
('gear_ratio','wheel_teeth/pinion_teeth','','DERIVADO: reduccion externa del hombro.'),
('gear_centers','gear_module*(pinion_teeth+wheel_teeth)/2','mm','DERIVADO: distancia entre ejes de engranajes.'),
('gear_t','5 mm','mm','PROPUESTO: ancho dentado.'),
('wrist45','25 mm','mm','PROPUESTO: J4 a J5.'),
('wrist56','25 mm','mm','PROPUESTO: J5 a J6.'),
('tcp_offset','25 mm','mm','PROPUESTO: J6 a punto de agarre.'),
('tool_reach','wrist45+wrist56+tcp_offset','mm','DERIVADO: J4 a TCP = 75 mm.'),
('grip_open','60 mm','mm','PROPUESTO: apertura de muestra para bloque de 50 mm.'),
('grip_max','65 mm','mm','PROPUESTO: apertura maxima geometrica.'),
('finger_t','6 mm','mm','PROPUESTO: espesor mandibula.'),
('finger_depth','16 mm','mm','PROPUESTO: ancho de mandibula.'),
('finger_length','40 mm','mm','PROPUESTO: largo de mandibula.'),
('pad_t','2 mm','mm','PROPUESTO: recubrimiento elastico.'),
('palm_w','88 mm','mm','PROPUESTO: ancho de palma.'),
('palm_d','36 mm','mm','PROPUESTO: fondo palma.'),
('palm_t','10 mm','mm','PROPUESTO: espesor palma.'),
('part_size','50 mm','mm','USUARIO: bloque 50x50x50 mm.'),
('part_mass','100 g','g','USUARIO: carga nominal; no carga admisible validada.'),
('nano_l','45 mm','mm','ARDUINO Nano A000005; variante clon por confirmar.'),
('nano_w','18 mm','mm','ARDUINO Nano A000005.'),
('pca_l','70 mm','mm','OFERTA LOCAL PCA9685 Sistemsoft; variante de modulo.'),
('pca_w','25 mm','mm','OFERTA LOCAL PCA9685 Sistemsoft.'),
('buck_l','65 mm','mm','OFERTA LOCAL XL4016, variante simplificada.'),
('buck_w','48 mm','mm','OFERTA LOCAL XL4016, variante simplificada.'),
('pcb_t','1.6 mm','mm','REFERENCIA: PCB comercial.'),
('q1','20 deg','deg','POSE DE PRESENTACION: giro base.'),
('q2','50 deg','deg','POSE DE PRESENTACION: hombro desde vertical.'),
('q3','45 deg','deg','POSE DE PRESENTACION: codo.'),
('q4','0 deg','deg','POSE DE PRESENTACION: giro antebrazo.'),
('q5','80 deg','deg','POSE DE PRESENTACION: inclinacion muñeca.'),
('q6','15 deg','deg','POSE DE PRESENTACION: giro herramienta.'),
]

def val(expr, units='mm'):
    return D.unitsManager.evaluateExpression(str(expr), units)

def mm(expr):
    return val(expr)/10.0 if False else val(expr)

def point(x=0,y=0,z=0):
    return P(val(x),val(y),val(z))

def sub(parent,name):
    o=parent.occurrences.addNewComponent(adsk.core.Matrix3D.create())
    o.component.name=name
    o.isGroundToParent=True
    return o.component

def main(name):
    o=R.occurrences.addNewComponent(adsk.core.Matrix3D.create())
    o.component.name=name
    return o

def comp(name):
    for o in R.occurrences:
        if o.component.name==name:
            return o.component
    raise RuntimeError('No existe componente '+name)

def occ(name):
    for o in R.occurrences:
        if o.component.name==name:
            return o
    raise RuntimeError(name)

def appearance(name,template,color=None):
    a=D.appearances.itemByName(name)
    if a: return a
    lib=adsk.core.Application.get().materialLibraries.itemByName('Fusion Appearance Library')
    a=D.appearances.addByCopy(lib.appearances.itemByName(template),name)
    if color:
        for prop in a.appearanceProperties:
            if prop.objectType=='adsk::core::ColorProperty':
                prop.value=adsk.core.Color.create(*color)
                break
    return a

def finish(c,key):
    for b in c.bRepBodies: b.appearance=AP[key]

def datum(c,axis,offset,name):
    pl={'Z':c.xYConstructionPlane,'Y':c.xZConstructionPlane}[axis]
    e=str(offset)
    ip=c.constructionPlanes.createInput()
    ip.setByOffset(pl,V(e))
    p=c.constructionPlanes.add(ip)
    p.name=name
    p.isLightBulbOn=False
    return p

def locate(s,p,x,y):
    # Driving coordinate dimensions, with zero-coordinate constraints.
    for ex,orientation,zero in [(str(x),ORI.HorizontalDimensionOrientation,'x'),(str(y),ORI.VerticalDimensionOrientation,'y')]:
        a=val(ex)
        if abs(a)<1e-8:
            if zero=='x': s.geometricConstraints.addVerticalPoints(s.originPoint,p)
            else: s.geometricConstraints.addHorizontalPoints(s.originPoint,p)
        else:
            dim=s.sketchDimensions.addDistanceDimension(s.originPoint,p,orientation,P(a+0.3,a+0.3,0))
            dim.parameter.expression=ex if a>0 else '-('+ex+')'

def rect(s,x,y,w,h):
    lines=s.sketchCurves.sketchLines.addTwoPointRectangle(P(val(x),val(y),0),P(val('('+str(x)+')+('+str(w)+')'),val('('+str(y)+')+('+str(h)+')'),0))
    locate(s,lines.item(0).startSketchPoint,x,y)
    d=s.sketchDimensions.addDistanceDimension(lines.item(0).startSketchPoint,lines.item(0).endSketchPoint,ORI.HorizontalDimensionOrientation,P(0,-0.5,0))
    d.parameter.expression=str(w)
    d=s.sketchDimensions.addDistanceDimension(lines.item(1).startSketchPoint,lines.item(1).endSketchPoint,ORI.VerticalDimensionOrientation,P(0.5,0,0))
    d.parameter.expression=str(h)
    return lines

def circle(s,u,v,d,center_ref=None):
    ci=s.sketchCurves.sketchCircles.addByCenterRadius(P(val(u),val(v),0),val(d)/2)
    if center_ref is None:
        if not ci.centerSketchPoint.isFullyConstrained:
            locate(s,ci.centerSketchPoint,u,v)
    elif not ci.centerSketchPoint.isFullyConstrained:
        s.geometricConstraints.addCoincident(ci.centerSketchPoint,center_ref)
    dim=s.sketchDimensions.addDiameterDimension(ci,P(val(u)+0.5,val(v)+0.5,0))
    dim.parameter.expression=str(d)
    return ci

def extrude(c,s,depth,op,name,profile=None):
    if profile is None: profile=s.profiles.item(0)
    activated=False
    if op==OP.CutFeatureOperation:
        target=next((o for o in R.allOccurrences if o.component.name==c.name),None)
        if target and hasattr(target,'activate'):
            target.activate();activated=True
    ip=c.features.extrudeFeatures.createInput(profile,op)
    # Legacy API remains supported and accepts signed, expression-based distances.
    ip.setDistanceExtent(False,V(str(depth)))
    if op==OP.CutFeatureOperation:
        ip.participantBodies=[b for b in ip.participantBodies if b.parentComponent.name==c.name]
    f=c.features.extrudeFeatures.add(ip)
    f.name=name
    if op==OP.CutFeatureOperation and (any(b.parentComponent.name!=c.name for b in f.bodies) or any(g.parentComponent.name!=c.name for g in f.linkedFeatures)):
        # Fusion can expand a cut's participants when a nested component is inactive.
        # Apply the body scope to the actual feature at its timeline position too.
        f.timelineObject.rollTo(True)
        own=[b for b in f.participantBodies if b.parentComponent.name==c.name]
        if not own:raise RuntimeError('Corte sin cuerpo propio: '+c.name+'/'+name)
        f.participantBodies=own
        f.timelineObject.rollTo(False)
    if activated:D.activateRootComponent()
    s.isVisible=False
    return f

def box(c,name,x,y,z,w,d,h,key='red',cut=False):
    s=c.sketches.add(datum(c,'Z',z,'Plano_'+name))
    s.name='Croquis_'+name
    s.isComputeDeferred=True
    rect(s,x,y,w,d)
    s.isComputeDeferred=False
    f=extrude(c,s,h,OP.CutFeatureOperation if cut else OP.NewBodyFeatureOperation,name)
    if not cut:
        for b in f.bodies: b.name=name; b.appearance=AP[key]
    return f

def ring(c,name,axis,a,u,v,od,id,length,key='red',cut=False):
    s=c.sketches.add(datum(c,axis,a,'Plano_'+name))
    s.name='Croquis_'+name
    s.isComputeDeferred=True
    sv=str(v) if axis=='Z' else '-('+str(v)+')'
    outer=circle(s,u,sv,od)
    if id and val(id)>0: circle(s,u,sv,id,outer.centerSketchPoint)
    s.isComputeDeferred=False
    # Nested circles: select annulus by loop count rather than transient profile index.
    profile=None
    if id and val(id)>0:
        for pp in s.profiles:
            if pp.profileLoops.count==2: profile=pp; break
    else: profile=s.profiles.item(0)
    depth=str(length)
    f=extrude(c,s,depth,OP.CutFeatureOperation if cut else OP.NewBodyFeatureOperation,name,profile)
    if not cut:
        for b in f.bodies: b.name=name; b.appearance=AP[key]
    return f

def join_all(c,name):
    valid=[b for b in c.bRepBodies if b.isSolid and b.volume>1e-9]
    if len(valid)<2: return
    bodies=adsk.core.ObjectCollection.create()
    for b in valid[1:]: bodies.add(b)
    ip=c.features.combineFeatures.createInput(valid[0],bodies)
    ip.operation=OP.JoinFeatureOperation
    f=c.features.combineFeatures.add(ip); f.name=name

def bearing(parent,name,axis,a,u,v,kind='608'):
    c=sub(parent,name)
    p='b'+kind
    ring(c,'Aro_exterior',axis,a,u,v,p+'_od',p+'_od-3 mm',p+'_w','steel')
    ring(c,'Aro_interior',axis,a,u,v,p+'_id+3 mm',p+'_id',p+'_w','steel')
    ring(c,'Blindaje',axis,'('+a+')+0.2 mm',u,v,p+'_od-3 mm',p+'_id+3 mm',p+'_w-0.4 mm','darksteel')
    return c

def screw_z(parent,name,x,y,z,d='3 mm',length='12 mm'):
    c=sub(parent,name)
    ring(c,'Vastago_liso_REFERENCIA','Z',z,x,y,d,None,length,'steel')
    ring(c,'Cabeza','Z','('+z+')+('+length+')',x,y,'('+d+')*1.8',None,'2 mm','darksteel')
    return c

def shaft_y(parent,name,z,length='76 mm',diam='b608_id'):
    c=sub(parent,name)
    ring(c,'Eje','Y','-('+length+')/2','0',z,diam,None,length,'steel')
    return c

def servo(parent,name,prefix,axis,face='0 mm',x='0 mm',z='0 mm'):
    c=sub(parent,name)
    L=prefix+'_l'; W=prefix+'_w'; H=prefix+'_h'
    sx='micro_shaft_x' if prefix=='micro' else 'std_shaft_x'
    ear='4 mm' if prefix=='micro' else 'std_ear'
    if axis=='Z':
        box(c,'Cuerpo',f'({x})-{sx}',f'-{W}/2',f'({face})-{H}',L,W,H,'servo')
        box(c,'Banda_carcasa',f'({x})-{sx}',f'-{W}/2-0.2 mm',f'({face})-{H}+6 mm',L,f'{W}+0.4 mm','4 mm','darksteel')
        box(c,'Pestanas',f'({x})-{sx}-({ear})',f'-{W}/2',f'({face})-std_ear_z',f'{L}+2*({ear})',W,'2.5 mm','servo')
        ring(c,'Salida_spline_REFERENCIA','Z',face,x,'0','4.8 mm' if prefix=='micro' else 'std_spline_d',None,'4 mm','brass')
        ring(c,'Horn_REFERENCIA','Z',f'({face})+4 mm',x,'0','16 mm' if prefix=='micro' else 'std_horn_d','2 mm','2 mm','white')
        for sign in [-1,1]:
            px=f'({x})+{L}/2-{sx}+({sign})*({L}/2+({ear})/2)'
            ring(c,'Paso_fijacion_'+str(sign),'Z',f'({face})-std_ear_z-1 mm',px,'0','2.4 mm' if prefix=='micro' else 'std_mount_d',None,'5 mm',cut=True)
    else:
        # Shaft points inward (-Y), case is outboard of the fork.
        box(c,'Cuerpo',f'({x})-{sx}',face,f'({z})-{W}/2',L,H,W,'servo')
        box(c,'Banda_carcasa',f'({x})-{sx}',f'({face})+{H}-10 mm',f'({z})-{W}/2-0.2 mm',L,'4 mm',f'{W}+0.4 mm','darksteel')
        box(c,'Pestanas',f'({x})-{sx}-({ear})',f'({face})+std_ear_z',f'({z})-{W}/2',f'{L}+2*({ear})','2.5 mm',W,'servo')
        ring(c,'Salida_spline_REFERENCIA','Y',f'({face})-4 mm',x,z,'4.8 mm' if prefix=='micro' else 'std_spline_d',None,'4 mm','brass')
        ring(c,'Horn_REFERENCIA','Y',f'({face})-6 mm',x,z,'16 mm' if prefix=='micro' else 'std_horn_d','2 mm','2 mm','white')
        for sign in [-1,1]:
            px=f'({x})+{L}/2-{sx}+({sign})*({L}/2+({ear})/2)'
            ring(c,'Paso_fijacion_'+str(sign),'Y',f'({face})+std_ear_z-1 mm',px,z,'2.4 mm' if prefix=='micro' else 'std_mount_d',None,'5 mm',cut=True)
    return c

def cage_y(parent,name,prefix,face,z,x='0 mm'):
    c=sub(parent,name)
    L=prefix+'_l'; W=prefix+'_w'; H=prefix+'_h'
    sx='micro_shaft_x' if prefix=='micro' else 'std_shaft_x'
    box(c,'Housing',f'({x})-{sx}-servo_clear-housing_wall',f'({face})-housing_wall',f'({z})-{W}/2-servo_clear-housing_wall',f'{L}+2*servo_clear+2*housing_wall',f'{H}+2*housing_wall',f'{W}+2*servo_clear+2*housing_wall')
    box(c,'Cavidad_servo',f'({x})-{sx}-servo_clear',f'({face})-0.1 mm',f'({z})-{W}/2-servo_clear',f'{L}+2*servo_clear',f'{H}+housing_wall+1 mm',f'{W}+2*servo_clear',cut=True)
    ring(c,'Paso_salida','Y',f'({face})-housing_wall-1 mm',x,z,'std_horn_d+2 mm',None,'housing_wall+2 mm',cut=True)
    return c

def origin(c,name,x='0 mm',y='0 mm',z='0 mm',axis='Z'):
    # Origin geometry refers to the component's true origin; expressions drive offsets.
    s=c.sketches.add(c.xYConstructionPlane); s.name='Referencia_'+name
    g=adsk.fusion.JointGeometry.createByPoint(s.originPoint)
    ip=c.jointOrigins.createInput(g)
    ip.xAxisEntity=c.xConstructionAxis
    ip.zAxisEntity=c.zConstructionAxis if axis=='Z' else c.yConstructionAxis
    # For Y-normal joint origin its local Y is -component Z.
    ip.offsetX=V(x)
    ip.offsetY=V(y if axis=='Z' else '-('+z+')')
    ip.offsetZ=V(z if axis=='Z' else y)
    jo=c.jointOrigins.add(ip); jo.name=name
    s.isVisible=False; jo.isLightBulbOn=False
    return jo

def setup():
    global D,R,AP
    app=adsk.core.Application.get()
    D=adsk.fusion.Design.cast(app.activeProduct)
    R=D.rootComponent
    AP={
      'red':appearance('AFR2_PETG_Rojo','Plastic - Matte (Red)',(198,24,31,255)),
      'white':appearance('AFR2_PVC_Blanco','Plastic - Matte (White)',(235,235,226,255)),
      'servo':appearance('AFR2_Servo_Negro','Plastic - Matte (Black)',(30,32,38,255)),
      'steel':appearance('AFR2_Acero','Steel - Satin'),
      'darksteel':appearance('AFR2_Acero_Oscuro','Paint - Enamel Glossy (Dark Grey)'),
      'brass':appearance('AFR2_Laton','Brass - Matte'),
      'pcb':appearance('AFR2_PCB','Plastic - Matte (Green)',(18,85,62,255)),
      'blue':appearance('AFR2_Modulo_Azul','Plastic - Matte (Blue)',(22,74,155,255)),
      'rubber':appearance('AFR2_Goma','Plastic - Matte (Black)',(45,45,45,255)),
      'part':appearance('AFR2_Pieza_100g','Plastic - Matte (Yellow)',(235,181,42,255)),
    }

def stage_init():
    global D,R
    app=adsk.core.Application.get()
    if D.rootComponent.occurrences.count or D.rootComponent.bRepBodies.count:
        doc=app.documents.add(adsk.core.DocumentTypes.FusionDesignDocumentType)
        D=adsk.fusion.Design.cast(app.activeProduct); R=D.rootComponent
    D.designIntent=adsk.fusion.DesignIntentTypes.HybridDesignIntentType
    D.designType=adsk.fusion.DesignTypes.ParametricDesignType
    D.fusionUnitsManager.distanceDisplayUnits=adsk.fusion.DistanceUnits.MillimeterDistanceUnits
    app.activeDocument.name=MODEL_NAME
    for n,e,u,comment in PARAMETERS:
        if not D.userParameters.itemByName(n): D.userParameters.add(n,V(e),u,comment)
    R.attributes.add('AFR2','scope','Prototipo conceptual PVC. No calculado/verificado/fabricado. Fotografia: OD32 ID28.3; pared derivada1.85.')
    for n in ['00_Base_control','01_Giro_base','02_Brazo_PVC','03_Antebrazo_PVC','04_Giro_muneca','05_Inclinacion_muneca','06_Herramienta','07_Mandibula_izquierda','08_Mandibula_derecha','09_Pinion_hombro','90_Pieza_REFERENCIA_50mm_100g']:
        main(n)
    occ('00_Base_control').isGrounded=True

def stage_base():
    g=comp('00_Base_control')
    c=sub(g,'B01_Piso_base_PETG')
    box(c,'Piso','-base_w/2','-base_w/2','0 mm','base_w','base_w','base_floor')
    for sx in [-1,1]:
        for sy in [-1,1]:
            ring(c,f'Anclaje_{sx}_{sy}','Z','-1 mm',f'({sx})*(base_w/2-12 mm)',f'({sy})*(base_w/2-12 mm)','5.5 mm',None,'base_floor+2 mm',cut=True)
    c=sub(g,'B02_Paredes_caja_PETG')
    box(c,'Trasera','-base_w/2','base_w/2-base_wall','base_floor','base_w','base_wall','base_h-base_floor')
    box(c,'Lateral_izq','-base_w/2','-base_w/2','base_floor','base_wall','base_w','base_h-base_floor')
    box(c,'Lateral_der','base_w/2-base_wall','-base_w/2','base_floor','base_wall','base_w','base_h-base_floor')
    c=sub(g,'B03_Frente_desmontable_PETG')
    box(c,'Frente','-base_w/2+base_wall','-base_w/2','base_floor','base_w-2*base_wall','base_wall','base_h-base_floor')
    box(c,'Ventana_control','-56 mm','-base_w/2-1 mm','14 mm','112 mm','base_wall+2 mm','25 mm',cut=True)
    for sx in [-1,1]:
        for z in ['10 mm','base_h-8 mm']:
            ring(c,f'Tornillo_frente_{sx}_{z}','Y','-base_w/2-1 mm',f'({sx})*(base_w/2-10 mm)',z,'3.4 mm',None,'base_wall+2 mm',cut=True)
    servo(g,'M01_MG946R_Giro_base','mg','Z','base_top-6 mm')
    c=sub(g,'B04_Cuna_servo_base_PETG')
    box(c,'Cuna','-std_shaft_x-4 mm','-mg_w/2-4 mm','base_floor','mg_l+8 mm','mg_w+8 mm','base_top-6 mm-mg_h-base_floor+4 mm')
    box(c,'Paso_cuerpo','-std_shaft_x-servo_clear','-mg_w/2-servo_clear','base_floor+1 mm','mg_l+2*servo_clear','mg_w+2*servo_clear','base_h',cut=True)
    origin(g,'J1_BASE','0 mm','0 mm','base_top','Z')

def stage_lid():
    g=comp('00_Base_control')
    c=sub(g,'B05_Tapa_base_desmontable_PETG')
    box(c,'Tapa','-base_w/2','-base_w/2','base_h','base_w','base_w','lid_t')
    ring(c,'Paso_eje_J1','Z','base_h-1 mm','0 mm','0 mm','28 mm',None,'lid_t+2 mm',cut=True)
    for x in ['-60 mm','-45 mm','45 mm','60 mm']:
        box(c,'Ventilacion_'+x,x,'-20 mm','base_h-1 mm','4 mm','40 mm','lid_t+2 mm',cut=True)
    for sx in [-1,1]:
        for sy in [-1,1]:
            ring(c,f'Taladro_tapa_{sx}_{sy}','Z','base_h-1 mm',f'({sx})*(base_w/2-8 mm)',f'({sy})*(base_w/2-8 mm)','3.4 mm',None,'lid_t+2 mm',cut=True)
    bearing(g,'R01_608ZZ_Apoyo_base','Z','base_h-b608_w','0 mm','0 mm')
    c=sub(g,'B06_Alojamiento_608_base_PETG')
    ring(c,'Casquillo','Z','base_h-b608_w-2 mm','0 mm','0 mm','32 mm','b608_id+1 mm','b608_w+2 mm')
    ring(c,'Asiento_608','Z','base_h-b608_w','0 mm','0 mm','b608_od+fit_clear',None,'b608_w+1 mm',cut=True)

def pcb(parent,name,x,y,l,w,z='9 mm',key='pcb'):
    c=sub(parent,name)
    box(c,'PCB',f'({x})-({l})/2',f'({y})-({w})/2',z,l,w,'pcb_t',key)
    return c

def stage_electronics():
    g=comp('00_Base_control')
    c=pcb(g,'E01_Arduino_Nano_A000005_REF','38 mm','59 mm','nano_l','nano_w')
    box(c,'USB_MINIB_REF','38 mm-nano_l/2-1 mm','55 mm','9 mm+pcb_t','8 mm','8 mm','4 mm','steel')
    box(c,'ATmega328_REF','33 mm','54 mm','9 mm+pcb_t','10 mm','10 mm','2 mm','servo')
    for sy in [-1,1]:
        box(c,'Header_'+str(sy),'38 mm-nano_l/2+3 mm',f'59 mm+({sy})*7.62 mm-1.27 mm','9 mm+pcb_t','38.1 mm','2.54 mm','3 mm','servo')
    c=pcb(g,'E02_PCA9685_Modulo70x25_REF','36 mm','28 mm','pca_l','pca_w')
    box(c,'Headers_16_canales_REF','2 mm','19 mm','9 mm+pcb_t','64 mm','7.62 mm','6 mm','servo')
    box(c,'PCA9685_REF','26 mm','29 mm','9 mm+pcb_t','10 mm','7 mm','2 mm','servo')
    box(c,'Bornera_Vservo_REF','2 mm','31 mm','9 mm+pcb_t','10 mm','8 mm','8 mm','blue')
    for idx,x,y in [(1,'-43 mm','-37 mm'),(2,'30 mm','-37 mm'),(3,'-43 mm','33 mm')]:
        c=pcb(g,f'E0{idx+2}_XL4016_REF',x,y,'buck_l','buck_w',key='blue')
        box(c,'Disipador_REF',f'({x})-24 mm',f'({y})-15 mm','9 mm+pcb_t','17 mm','30 mm','18 mm','steel')
        ring(c,'Inductor_REF','Z','9 mm+pcb_t',f'({x})+12 mm',y,'17 mm','8 mm','10 mm','brass')
        box(c,'Bornera_REF',f'({x})-30 mm',f'({y})-22 mm','9 mm+pcb_t','10 mm','8 mm','8 mm','blue')
    c=sub(g,'B07_Bandeja_control_PETG')
    box(c,'Bandeja','-base_w/2+6 mm','-base_w/2+6 mm','base_floor','base_w-12 mm','base_w-12 mm','2 mm')
    box(c,'Ventana_servo_central','-15 mm','-14 mm','base_floor-1 mm','50 mm','28 mm','5 mm',cut=True)
    for x,y in [('-72 mm','-65 mm'),('65 mm','-65 mm'),('-72 mm','65 mm'),('65 mm','65 mm')]:
        box(c,'Separador_'+x+y,x,y,'base_floor+2 mm','6 mm','6 mm','3 mm')
    c=sub(g,'E06_Entrada_12V_REF')
    ring(c,'Conector','Y','base_w/2-3 mm','60 mm','25 mm','12 mm','6 mm','8 mm','darksteel')
    c=sub(g,'E07_Interruptor_REF')
    box(c,'Cuerpo','-67 mm','base_w/2-5 mm','18 mm','14 mm','10 mm','20 mm','servo')


def stage_tower():
    g=comp('01_Giro_base')
    c=sub(g,'A01_Plataforma_giratoria_PETG')
    ring(c,'Plataforma','Z','0 mm','0 mm','0 mm','100 mm','8 mm','8 mm')
    c=sub(g,'A02_Torre_hombro_PETG')
    box(c,'Puente','-25 mm','-fork_y-fork_t/2','8 mm','50 mm','2*fork_y+fork_t','10 mm')
    for sign in [-1,1]:
        y=f'({sign})*fork_y-fork_t/2'
        box(c,'Oreja_'+str(sign),'-23 mm',y,'12 mm','46 mm','fork_t','tower_h+16 mm')
        ring(c,'Asiento_'+str(sign),'Y',f'({sign})*fork_y-fork_t/2-1 mm','0 mm','tower_h','b608_od+fit_clear',None,'fork_t+2 mm',cut=True)
    box(c,'Alivio_central','-24 mm','-fork_y-fork_t/2-1 mm','18 mm','48 mm','2*fork_y+fork_t+2 mm','tower_h-42 mm',cut=True)
    for sign in [-1,1]:
        bearing(g,'R02_608ZZ_Hombro_'+str(sign),'Y',f'({sign})*fork_y-b608_w/2','0 mm','tower_h')
    origin(g,'J1_GIRO')
    origin(g,'J2_HOMBRO','0 mm','0 mm','tower_h','Y')
    origin(g,'J2_PINION','0 mm','0 mm','tower_h-gear_centers','Y')

def stage_shoulder_motor():
    g=comp('01_Giro_base')
    servo(g,'M02_GX3240_270_REF','gx','Y','38 mm','0 mm','tower_h-gear_centers')
    cage_y(g,'A03_Housing_GX3240_PETG','gx','38 mm','tower_h-gear_centers')
    c=sub(g,'A04_Puente_housing_hombro_PETG')
    box(c,'Puente_housing','-15 mm','fork_y','tower_h-gear_centers-14 mm','30 mm','13 mm','8 mm')
    for x in ['-10 mm','10 mm']:
        ring(c,'Paso_M3_'+x,'Z','tower_h-gear_centers-15 mm',x,'34 mm','3.4 mm',None,'10 mm',cut=True)

def gear(c,name,teeth,bore,axis='Y'):
    # Simplified straight-sided teeth for prototype visualization.
    ring(c,name+'_Nucleo',axis,'32 mm' if axis=='Y' else '0 mm','0 mm','0 mm',f'gear_module*({teeth}-2.5)',bore,'gear_t','red')
    s=c.sketches.add(datum(c,axis,'32 mm' if axis=='Y' else '0 mm','Plano_diente'))
    s.name='Croquis_diente_REFERENCIA_NO_EVOLVENTE'
    rect(s,f'gear_module*({teeth}-2.5)/2-0.5 mm','-gear_module*0.7',f'gear_module*2.25+0.5 mm','gear_module*1.4')
    f=extrude(c,s,'gear_t',OP.JoinFeatureOperation,'Diente_maestro')
    entities=adsk.core.ObjectCollection.create(); entities.add(f)
    ip=c.features.circularPatternFeatures.createInput(entities,c.yConstructionAxis if axis=='Y' else c.zConstructionAxis)
    ip.quantity=V(teeth); ip.totalAngle=V('360 deg'); ip.isSymmetric=False
    c.features.circularPatternFeatures.add(ip).name='Patron_dientes_'+teeth

def stage_drive():
    g=comp('09_Pinion_hombro')
    c=sub(g,'T01_Pinion_16T_REFERENCIA')
    gear(c,'Pinon','pinion_teeth','std_spline_d')
    origin(g,'EJE_PINION','0 mm','0 mm','0 mm','Y')
    g=comp('02_Brazo_PVC')
    c=sub(g,'T02_Rueda_32T_REFERENCIA')
    gear(c,'Rueda','wheel_teeth','b608_id')
    shaft_y(g,'E01_Eje_hombro_8mm','0 mm')

def socket(parent,name,start):
    c=sub(parent,name)
    ring(c,'Manguito','Z',start,'0 mm','0 mm','socket_od','pvc_od+socket_clear','socket_length')
    for sign in [-1,1]:
        box(c,'Oreja_abrazadera_'+str(sign),'pvc_od/2+1 mm',f'({sign})*4 mm-2.5 mm',f'({start})+socket_length/2-5 mm','10 mm','5 mm','10 mm')
    join_all(c,'Union_manguito_orejas')
    box(c,'Ranura_apriete','pvc_od/2-1 mm','-0.75 mm',f'({start})-1 mm','13 mm','1.5 mm','socket_length+2 mm',cut=True)
    ring(c,'Paso_perno_apriete_M3','Y','-8 mm','pvc_od/2+6 mm',f'({start})+socket_length/2','3.4 mm',None,'16 mm',cut=True)
    bolt=sub(parent,name+'_Perno_M3_REF')
    ring(bolt,'Vastago','Y','-8 mm','pvc_od/2+6 mm',f'({start})+socket_length/2','3 mm',None,'16 mm','steel')
    ring(bolt,'Cabeza','Y','-10 mm','pvc_od/2+6 mm',f'({start})+socket_length/2','5.5 mm',None,'2 mm','darksteel')
    return c

def stage_link1():
    g=comp('02_Brazo_PVC')
    c=sub(g,'L01_PVC32_Corte_1')
    ring(c,'Tubo_PVC','Z','tube_inset','0 mm','0 mm','pvc_od','pvc_id','tube1_cut','white')
    socket(g,'A05_Encastre_proximal_brazo_PETG','socket_start')
    socket(g,'A06_Encastre_distal_brazo_PETG','link1-socket_start-socket_length')
    c=sub(g,'A07_Nucleo_hombro_PETG')
    ring(c,'Nucleo','Y','-20 mm','0 mm','0 mm','42 mm','b608_id','40 mm')
    box(c,'Cuello','-13 mm','-13 mm','8 mm','26 mm','26 mm','20 mm')
    join_all(c,'Nucleo_con_cuello')
    origin(g,'J2_BRAZO','0 mm','0 mm','0 mm','Y')
    origin(g,'J3_CODO','0 mm','0 mm','link1','Y')

def stage_elbow():
    g=comp('02_Brazo_PVC')
    c=sub(g,'A08_Horquilla_codo_PETG')
    box(c,'Puente','-18 mm','-fork_y-fork_t/2','link1-40 mm','36 mm','2*fork_y+fork_t','10 mm')
    ring(c,'Paso_PVC','Z','link1-41 mm','0 mm','0 mm','pvc_od+socket_clear',None,'12 mm',cut=True)
    for sign in [-1,1]:
        y=f'({sign})*fork_y-fork_t/2'
        box(c,'Brazo_horquilla_'+str(sign),'-18 mm',y,'link1-35 mm','36 mm','fork_t','35 mm')
        ring(c,'Oreja_'+str(sign),'Y',y,'0 mm','link1','42 mm','b608_od+fit_clear','fork_t')
        ring(c,'Asiento_completo_'+str(sign),'Y',f'({y})-1 mm','0 mm','link1','b608_od+fit_clear',None,'fork_t+2 mm',cut=True)
    join_all(c,'Horquilla_unificada')
    for sign in [-1,1]:
        bearing(g,'R03_608ZZ_Codo_'+str(sign),'Y',f'({sign})*fork_y-b608_w/2','0 mm','link1')
    servo(g,'M03_DS3218_Codo','ds','Y','38 mm','0 mm','link1')
    cage_y(g,'A09_Housing_DS3218_PETG','ds','38 mm','link1')
    c=sub(g,'A10_Puente_housing_codo')
    box(c,'Puente','-14 mm','fork_y','link1-14 mm','28 mm','13 mm','8 mm')

def stage_link2():
    g=comp('03_Antebrazo_PVC')
    c=sub(g,'L02_PVC32_Corte_2')
    ring(c,'Tubo_PVC','Z','tube_inset','0 mm','0 mm','pvc_od','pvc_id','tube2_cut','white')
    socket(g,'A11_Encastre_proximal_antebrazo_PETG','socket_start')
    socket(g,'A12_Encastre_distal_antebrazo_PETG','link2-socket_start-socket_length')
    c=sub(g,'A13_Nucleo_codo_PETG')
    ring(c,'Nucleo','Y','-20 mm','0 mm','0 mm','40 mm','b608_id','40 mm')
    box(c,'Cuello','-13 mm','-13 mm','8 mm','26 mm','26 mm','20 mm')
    join_all(c,'Nucleo_con_cuello')
    shaft_y(g,'E02_Eje_codo_8mm','0 mm')
    origin(g,'J3_ANTEBRAZO','0 mm','0 mm','0 mm','Y')
    origin(g,'J4_MUNECA','0 mm','0 mm','link2','Z')

def stage_roll_motor():
    g=comp('03_Antebrazo_PVC')
    # MG90S sits behind the roll bearing, clear of the pipe end.
    servo(g,'M04_MG90S_Giro_muneca','micro','Z','link2-5 mm','36 mm')
    c=sub(g,'A14_Alojamiento_giro_muneca_PETG')
    ring(c,'Copa','Z','link2-25 mm','0 mm','0 mm','46 mm','30 mm','25 mm')
    ring(c,'Tabique_rodamiento','Z','link2-b608_w-2 mm','0 mm','0 mm','46 mm','b608_id+1 mm','b608_w+2 mm')
    ring(c,'Asiento_608','Z','link2-b608_w','0 mm','0 mm','b608_od+fit_clear',None,'b608_w+1 mm',cut=True)
    # Two red rails constitute an open mounting cage for the microservo.
    for sign in [-1,1]:
        box(c,'Carril_micro_'+str(sign),'36 mm-micro_shaft_x-3 mm',f'({sign})*(micro_w/2+servo_clear)','link2-5 mm-micro_h','micro_l+6 mm','3 mm','micro_h')
    box(c,'Puente_housing','17 mm','-10 mm','link2-30 mm','25 mm','20 mm','5 mm')
    bearing(g,'R04_608ZZ_Giro_muneca','Z','link2-b608_w','0 mm','0 mm')

def stage_wrist_roll():
    g=comp('04_Giro_muneca')
    c=sub(g,'A15_Rotor_y_horquilla_muneca_PETG')
    ring(c,'Rotor','Z','6 mm','0 mm','0 mm','38 mm','b608_id','6 mm')
    box(c,'Puente','-18 mm','-25 mm','11 mm','36 mm','50 mm','7 mm')
    for sign in [-1,1]:
        y=f'({sign})*21 mm-4 mm'
        box(c,'Oreja_vertical_'+str(sign),'-16 mm',y,'9 mm','32 mm','8 mm','wrist45-9 mm')
        ring(c,'Oreja_redonda_'+str(sign),'Y',y,'0 mm','wrist45','32 mm','b625_od+fit_clear','8 mm')
        ring(c,'Asiento_'+str(sign),'Y',f'({y})-1 mm','0 mm','wrist45','b625_od+fit_clear',None,'10 mm',cut=True)
    join_all(c,'Horquilla_unificada')
    for sign in [-1,1]:
        bearing(g,'R05_625ZZ_Inclinacion_'+str(sign),'Y',f'({sign})*21 mm-b625_w/2','0 mm','wrist45','625')
    servo(g,'M05_MG946R_Inclinacion_muneca','mg','Y','30 mm','0 mm','wrist45')
    cage_y(g,'A16_Housing_MG946R_Muneca_PETG','mg','30 mm','wrist45')
    c=sub(g,'A17_Puente_housing_muneca')
    box(c,'Puente','-13 mm','21 mm','wrist45-14 mm','26 mm','12 mm','7 mm')
    c=sub(g,'E03_Eje_giro_muneca_8mm')
    ring(c,'Eje','Z','-12 mm','0 mm','0 mm','b608_id',None,'18 mm','steel')
    origin(g,'J4_ROTOR')
    origin(g,'J5_INCLINACION','0 mm','0 mm','wrist45','Y')

def stage_wrist_tilt():
    g=comp('05_Inclinacion_muneca')
    c=sub(g,'A18_Nucleo_muneca_PETG')
    ring(c,'Nucleo','Y','-16 mm','0 mm','0 mm','26 mm','b625_id','32 mm')
    box(c,'Cuello','-12 mm','-12 mm','4 mm','24 mm','24 mm','wrist56-4 mm')
    ring(c,'Corona_J6','Z','wrist56-8 mm','0 mm','0 mm','34 mm','b625_od+fit_clear','8 mm')
    join_all(c,'Nucleo_unificado')
    shaft_y(g,'E04_Eje_inclinacion_5mm','0 mm','56 mm','b625_id')
    bearing(g,'R06_625ZZ_Giro_herramienta','Z','wrist56-b625_w','0 mm','0 mm','625')
    # Microservo kept behind J6; simplified open housing at the rear of the tilted wrist.
    servo(g,'M06_MG90S_Giro_herramienta','micro','Z','wrist56-11 mm','30 mm')
    c=sub(g,'A19_Housing_MG90S_J6_PETG')
    for sign in [-1,1]:
        box(c,'Carril_'+str(sign),'30 mm-micro_shaft_x-3 mm',f'({sign})*(micro_w/2+servo_clear)','wrist56-11 mm-micro_h','micro_l+6 mm','3 mm','micro_h')
    box(c,'Puente_housing','10 mm','-10 mm','-9 mm','29 mm','20 mm','5 mm')
    origin(g,'J5_NUCLEO','0 mm','0 mm','0 mm','Y')
    origin(g,'J6_HERRAMIENTA','0 mm','0 mm','wrist56','Z')

def stage_gripper():
    g=comp('06_Herramienta')
    c=sub(g,'A20_Palma_pinza_PETG')
    box(c,'Palma','-palm_w/2','-palm_d/2','0 mm','palm_w','palm_d','palm_t')
    # Slots allow the two parallel carriages to run across X.
    for sign in [-1,1]:
        box(c,'Guia_'+str(sign),'-palm_w/2-1 mm',f'({sign})*10 mm-2 mm','3 mm','palm_w+2 mm','4 mm','5 mm',cut=True)
    ring(c,'Paso_central','Z','-1 mm','0 mm','0 mm','b625_id',None,'palm_t+2 mm',cut=True)
    ring(c,'Alojamiento_transmision_J6','Z','-1 mm','0 mm','0 mm','36 mm',None,'7 mm',cut=True)
    c=sub(g,'A21_Tapa_guias_pinza_PETG')
    for sign in [-1,1]:
        box(c,'Tapa_'+str(sign),'-palm_w/2',f'({sign})*14 mm-2 mm','palm_t','palm_w','4 mm','3 mm')
    # Offset actuator drives a transverse linkage. Linkage is conceptual and editable.
    servo(g,'M07_MG90S_Apertura_pinza','micro','Z','0 mm','-27 mm')
    c=sub(g,'A22_Housing_MG90S_Pinza_PETG')
    box(c,'Housing','-27 mm-micro_shaft_x-3 mm','-micro_w/2-3 mm','-micro_h','micro_l+6 mm','micro_w+6 mm','micro_h')
    box(c,'Cavidad','-27 mm-micro_shaft_x-servo_clear','-micro_w/2-servo_clear','-micro_h-1 mm','micro_l+2*servo_clear','micro_w+2*servo_clear','micro_h+2 mm',cut=True)
    c=sub(g,'E05_Eje_herramienta_5mm')
    ring(c,'Eje','Z','-11 mm','0 mm','0 mm','b625_id',None,'14 mm','steel')
    origin(g,'J6_PALMA')
    # Slider origins offset to the commanded opening; joints keep carriage attachments parametric.
    origin(g,'GUIA_IZQ','-grip_open/2-finger_t/2-pad_t','0 mm','0 mm')
    origin(g,'GUIA_DER','grip_open/2+finger_t/2+pad_t','0 mm','0 mm')
    origin(g,'TCP','0 mm','0 mm','tcp_offset')

def stage_fingers():
    for side,name in [(-1,'07_Mandibula_izquierda'),(1,'08_Mandibula_derecha')]:
        g=comp(name)
        c=sub(g,'A23_Mandibula_'+str(side)+'_PETG')
        box(c,'Dedo','-finger_t/2','-finger_depth/2','palm_t','finger_t','finger_depth','finger_length')
        box(c,'Carro','-8 mm','-palm_d/2+3 mm','3.2 mm','16 mm','palm_d-6 mm','palm_t-3.2 mm')
        for sy in [-1,1]:
            box(c,'Patin_'+str(sy),'-8 mm',f'({sy})*10 mm-1.5 mm','3.5 mm','16 mm','3 mm','3 mm')
        join_all(c,'Mandibula_con_carro')
        pad=sub(g,'A24_Almohadilla_'+str(side)+'_Goma_REF')
        box(pad,'Recubrimiento', 'finger_t/2' if side<0 else '-finger_t/2-pad_t','-finger_depth/2','palm_t+3 mm','pad_t','finger_depth','finger_length-6 mm','rubber')
        origin(g,'CARRO')
    g=comp('90_Pieza_REFERENCIA_50mm_100g')
    c=sub(g,'Pieza_50mm_REFERENCIA')
    box(c,'Bloque','-part_size/2','-part_size/2','-part_size/2','part_size','part_size','part_size','part')
    origin(g,'CENTRO_PIEZA')


def make_joint(name,child_name,child_origin,parent_name,parent_origin,axis='Z',angle='0 deg',kind='revolute',limits=None):
    co=occ(child_name); po=occ(parent_name)
    cj=co.component.jointOrigins.itemByName(child_origin).createForAssemblyContext(co)
    pj=po.component.jointOrigins.itemByName(parent_origin).createForAssemblyContext(po)
    ip=R.joints.createInput(cj,pj)
    ip.isFlipped=False; ip.angle=V(angle)
    if kind=='rigid': ip.setAsRigidJointMotion()
    elif kind=='slider': ip.setAsSliderJointMotion(adsk.fusion.JointDirections.XAxisJointDirection)
    else: ip.setAsRevoluteJointMotion(adsk.fusion.JointDirections.ZAxisJointDirection)
    j=R.joints.add(ip); j.name=name
    if limits and kind=='revolute':
        lm=j.jointMotion.rotationLimits
        lm.minimumValue=math.radians(limits[0]); lm.maximumValue=math.radians(limits[1])
        lm.isMinimumValueEnabled=True; lm.isMaximumValueEnabled=True
    return j

def stage_assembly():
    for o in R.occurrences:
        o.isGroundToParent=False
    occ('00_Base_control').isGrounded=True
    D.userParameters.itemByName('grip_open').expression='50 mm'
    joints=[]
    joints.append(make_joint('J1_Giro_base','01_Giro_base','J1_GIRO','00_Base_control','J1_BASE',angle='q1',limits=(-80,80)))
    joints.append(make_joint('J2_Hombro','02_Brazo_PVC','J2_BRAZO','01_Giro_base','J2_HOMBRO','Y','q2',limits=(-10,125)))
    joints.append(make_joint('J3_Codo','03_Antebrazo_PVC','J3_ANTEBRAZO','02_Brazo_PVC','J3_CODO','Y','q3',limits=(-90,90)))
    joints.append(make_joint('J4_Giro_muneca','04_Giro_muneca','J4_ROTOR','03_Antebrazo_PVC','J4_MUNECA',angle='q4',limits=(-80,80)))
    joints.append(make_joint('J5_Inclinacion_muneca','05_Inclinacion_muneca','J5_NUCLEO','04_Giro_muneca','J5_INCLINACION','Y','q5',limits=(-90,90)))
    joints.append(make_joint('J6_Giro_herramienta','06_Herramienta','J6_PALMA','05_Inclinacion_muneca','J6_HERRAMIENTA',angle='q6',limits=(-80,80)))
    jp=make_joint('T_Hombro_Pinion_2a1','09_Pinion_hombro','EJE_PINION','01_Giro_base','J2_PINION','Y','-q2*gear_ratio')
    jl=make_joint('G1_Mordaza_izquierda','07_Mandibula_izquierda','CARRO','06_Herramienta','GUIA_IZQ',kind='slider')
    jr=make_joint('G2_Mordaza_derecha','08_Mandibula_derecha','CARRO','06_Herramienta','GUIA_DER',kind='slider')
    # Origin of the grasp is lower than the centre of mass of the reference block.
    # This keeps the cube above the palm and gear cover while the jaws hold its lower portion.
    origin(comp('06_Herramienta'),'PIEZA_CENTRO','0 mm','0 mm','tcp_offset+17 mm')
    make_joint('REF_Pieza_en_pinza','90_Pieza_REFERENCIA_50mm_100g','CENTRO_PIEZA','06_Herramienta','PIEZA_CENTRO',kind='rigid')
    # Opposing jaw travel is linked mechanically in the CAD.
    mi=R.motionLinks.createInput(jl,jr)
    mi.valueOne=V('1 mm'); mi.valueTwo=V('1 mm'); mi.isReversed=True
    R.motionLinks.add(mi).name='Apertura_simetrica_mordazas'
    # Keep auxiliary drive neutral relative to its parametric angular alignment.
    jp.isLocked=True
    for c in D.allComponents:
        if hasattr(c,'isSketchFolderLightBulbOn'): c.isSketchFolderLightBulbOn=False
        if hasattr(c,'isConstructionFolderLightBulbOn'): c.isConstructionFolderLightBulbOn=False
    app=adsk.core.Application.get()
    cam=app.activeViewport.camera
    cam.viewOrientation=adsk.core.ViewOrientations.IsoTopRightViewOrientation
    cam.isFitView=True
    app.activeViewport.camera=cam

def small_gear(parent,name,x,z,teeth,module,bore):
    c=sub(parent,name)
    f=ring(c,'Nucleo','Z',z,x,'0 mm',f'({module})*(({teeth})-2.5)',bore,'4 mm','red')
    face=next(fc for fc in f.bodies.item(0).faces if adsk.core.Cylinder.cast(fc.geometry))
    ip=c.constructionAxes.createInput(); ip.setByCircularFace(face)
    ax=c.constructionAxes.add(ip); ax.isLightBulbOn=False
    s=c.sketches.add(datum(c,'Z',z,'Plano_diente'))
    rect(s,f'({x})+({module})*(({teeth})-2.5)/2-0.4 mm',f'-({module})*0.7',f'({module})*2.25+0.4 mm',f'({module})*1.4')
    t=extrude(c,s,'4 mm',OP.JoinFeatureOperation,'Diente_REFERENCIA')
    es=adsk.core.ObjectCollection.create(); es.add(t)
    ip=c.features.circularPatternFeatures.createInput(es,ax)
    ip.quantity=V(str(teeth)); ip.totalAngle=V('360 deg'); ip.isSymmetric=False
    c.features.circularPatternFeatures.add(ip).name='Dentado_recto_REFERENCIA'
    return c

def stage_wrist_transmissions():
    g=comp('03_Antebrazo_PVC')
    small_gear(g,'T03_J4_Pinon_motor_REF','36 mm','link2+1 mm','24','1.5 mm','4.8 mm')
    g=comp('04_Giro_muneca')
    small_gear(g,'T04_J4_Rueda_eje_REF','0 mm','1 mm','24','1.5 mm','b608_id')
    g=comp('05_Inclinacion_muneca')
    small_gear(g,'T05_J6_Pinon_motor_REF','30 mm','wrist56-5 mm','20','1.5 mm','4.8 mm')
    g=comp('06_Herramienta')
    c=small_gear(g,'T06_J6_Rueda_eje_REF','0 mm','-5 mm','20','1.5 mm','b625_od+fit_clear')
    ring(c,'Tapa_union_eje','Z','-1 mm','0 mm','0 mm','30 mm','b625_id','4 mm')

def stage_gripper_transmission():
    g=comp('06_Herramienta')
    small_gear(g,'T07_Pinza_Entrada_REF','-27 mm','10 mm','30','0.9 mm','4.8 mm')
    small_gear(g,'T08_Pinza_Intermedio_REF','0 mm','10 mm','30','0.9 mm','b625_id')
    small_gear(g,'T09_Pinza_Pinon_cremalleras_REF','0 mm','13 mm','12','1.25 mm','b625_id')
    # Bearing for the gripper drive is independent from J6.
    c=sub(g,'T10_Pinza_Casquillo_REF')
    ring(c,'Casquillo','Z','6 mm','0 mm','0 mm','10 mm','b625_id','5 mm','steel')
    # Longitudinal racks: simplified rectangular teeth, not production involutes.
    for sign,nm in [(-1,'07_Mandibula_izquierda'),(1,'08_Mandibula_derecha')]:
        p=comp(nm)
        c=sub(p,'T11_Cremallera_'+str(sign)+'_REF')
        x='0 mm' if sign<0 else '-45 mm'
        y='9.5 mm' if sign<0 else '-12.5 mm'
        box(c,'Cremallera',x,y,'13 mm','45 mm','3 mm','4 mm')
        sy='8 mm' if sign<0 else '-10.5 mm'
        s=c.sketches.add(datum(c,'Z','13 mm','Plano_diente'))
        rect(s,f'({x})+2 mm',sy,'1.8 mm','2.5 mm')
        tooth=extrude(c,s,'4 mm',OP.JoinFeatureOperation,'Diente_cremallera')
        es=adsk.core.ObjectCollection.create(); es.add(tooth)
        ip=c.features.rectangularPatternFeatures.createInput(es,c.xConstructionAxis,V('11'),V('3.141592653589793*1.25 mm'),adsk.fusion.PatternDistanceType.SpacingPatternDistanceType)
        c.features.rectangularPatternFeatures.add(ip).name='Dientes_cremallera_REF'

def stage_mounts():
    g=comp('00_Base_control')
    c=sub(g,'B08_Retencion_placas_PETG')
    for y in ['49 mm','67 mm']:
        box(c,'Cuna_Nano_'+y,'14 mm',y,'6 mm','48 mm','3 mm','3 mm')
    for x,y in [('4 mm','17 mm'),('64 mm','17 mm'),('4 mm','37 mm'),('64 mm','37 mm')]:
        box(c,'Apoyo_PCA_'+x+y,x,y,'6 mm','5 mm','5 mm','3 mm')
    for idx,x,y in [(1,'-43 mm','-37 mm'),(2,'30 mm','-37 mm'),(3,'-43 mm','33 mm')]:
        for sx,sy in [(-1,-1),(1,-1),(-1,1),(1,1)]:
            box(c,f'Apoyo_buck{idx}_{sx}_{sy}',f'({x})+({sx})*27 mm-2 mm',f'({y})+({sy})*18 mm-2 mm','6 mm','4 mm','4 mm','3 mm')
    c=sub(g,'B09_Postes_MG946_Base_PETG')
    for side in [-1,1]:
        x=f'mg_l/2-std_shaft_x+({side})*(mg_l/2+std_ear/2)'
        box(c,'Poste_'+str(side),f'({x})-4 mm','-mg_w/2','base_floor','8 mm','mg_w','base_top-6 mm-std_ear_z-base_floor')
        ring(c,'Taladro_'+str(side),'Z','base_floor-1 mm',x,'0 mm','3.4 mm',None,'base_h',cut=True)
        screw_z(g,'Fijacion_MG946_Base_'+str(side),x,'0 mm','base_h-20 mm','3 mm','12 mm')
    # Joint housing mounting tabs. Their final hole pattern remains a variant reference.
    for group,cage,prefix,face,z in [
       ('01_Giro_base','A03_Housing_GX3240_PETG','gx','38 mm','tower_h-gear_centers'),
       ('02_Brazo_PVC','A09_Housing_DS3218_PETG','ds','38 mm','link1'),
       ('04_Giro_muneca','A16_Housing_MG946R_Muneca_PETG','mg','30 mm','wrist45')]:
        pg=comp(group); cc=next(o.component for o in pg.occurrences if o.component.name==cage)
        for side in [-1,1]:
            x=f'{prefix}_l/2-std_shaft_x+({side})*({prefix}_l/2+std_ear/2)'
            box(cc,'Soporte_pestana_'+str(side),f'({x})-4 mm',f'({face})+std_ear_z+2.5 mm',f'({z})-{prefix}_w/2','8 mm','6 mm',prefix+'_w')
            ring(cc,'Taladro_pestana_'+str(side),'Y',f'({face})+std_ear_z+1 mm',x,z,'3.4 mm',None,'9 mm',cut=True)
    # Relieve the end plugs exactly where the PVC begins.
    for gn,cn in [('02_Brazo_PVC','A07_Nucleo_hombro_PETG'),('03_Antebrazo_PVC','A13_Nucleo_codo_PETG')]:
        pg=comp(gn); cc=next(o.component for o in pg.occurrences if o.component.name==cn)
        ring(cc,'Paso_extremo_PVC','Z','tube_inset','0 mm','0 mm','pvc_od+socket_clear',None,'10 mm',cut=True)

def stage_presentation():
    app=adsk.core.Application.get()
    for c in D.allComponents:
        for prop in ['isSketchFolderLightBulbOn','isConstructionFolderLightBulbOn','isOriginFolderLightBulbOn','isJointsFolderLightBulbOn']:
            if hasattr(c,prop): setattr(c,prop,False)
    for j in R.joints:
        if hasattr(j,'isLightBulbOn'): j.isLightBulbOn=False
    cam=app.activeViewport.camera
    cam.eye=P(45,55,42); cam.target=P(7,0,16); cam.upVector=adsk.core.Vector3D.create(0,0,1)
    cam.isFitView=True
    app.activeViewport.camera=cam
    folder=os.path.join(ROOT_DIR,'renders','2026-10-26')
    os.makedirs(folder,exist_ok=True)
    app.activeViewport.saveAsImageFile(os.path.join(folder,'brazo_pvc32_prototipo.png'),1600,1200)
    # Separate authoring illustration of the electronics layout; restore complete assembly.
    hidden=[]
    for o in R.occurrences:
        if o.component.name!='00_Base_control':
            hidden.append((o,o.isLightBulbOn)); o.isLightBulbOn=False
    for o in comp('00_Base_control').occurrences:
        if o.component.name in ['B02_Paredes_caja_PETG','B03_Frente_desmontable_PETG','B05_Tapa_base_desmontable_PETG']:
            hidden.append((o,o.isLightBulbOn)); o.isLightBulbOn=False
    cam=app.activeViewport.camera
    cam.eye=P(23,-29,29); cam.target=P(0,0,2.5); cam.upVector=adsk.core.Vector3D.create(0,0,1)
    cam.isFitView=True
    app.activeViewport.camera=cam
    adsk.doEvents()
    app.activeViewport.saveAsImageFile(os.path.join(folder,'brazo_base_electronica.png'),1600,1200)
    for o,state in hidden: o.isLightBulbOn=state
    cam=app.activeViewport.camera
    cam.eye=P(45,55,42); cam.target=P(7,0,16); cam.upVector=adsk.core.Vector3D.create(0,0,1)
    cam.isFitView=True
    app.activeViewport.camera=cam


def stage_save():
    app=adsk.core.Application.get()
    os.makedirs(OUT_DIR,exist_ok=True)
    doc=app.activeDocument
    if not doc.isSaved:
        doc.saveAs(MODEL_NAME,app.data.activeProject.rootFolder,'Prototipo parametrico PVC32. Para revision. No validado.','')
    elif doc.isModified: doc.save('Prototipo PVC32, componentes y articulaciones editables.')
    path=os.path.join(OUT_DIR,MODEL_NAME+'.f3d')
    D.exportManager.execute(D.exportManager.createFusionArchiveExportOptions(path))
    import csv
    with open(os.path.join(OUT_DIR,'parametros_prototipo.csv'),'w',encoding='utf-8-sig',newline='') as stream:
        w=csv.writer(stream); w.writerow(['parametro','expresion','unidad','procedencia_y_estado'])
        for p in D.userParameters: w.writerow([p.name,p.expression,p.unit,p.comment])
    render_dir=os.path.join(ROOT_DIR,'renders','2026-10-26')
    os.makedirs(render_dir,exist_ok=True)
    app.activeViewport.saveAsImageFile(os.path.join(render_dir,'brazo_pvc32_prototipo.png'),1600,1200)
    print('F3D',path)
    print('Cloud',doc.dataFile.id if doc.dataFile else 'guardado en curso')


def stage_scope_cuts():
    # Repair explicitly scoped cut participants in the prototype authored in this session.
    # This is a geometry edit, not an interference or resistance check.
    cuts=[]
    for c in D.allComponents:
        if globals().get('SCOPE_COMPONENTS') and c.name not in globals()['SCOPE_COMPONENTS']:continue
        for f in c.features.extrudeFeatures:
            try:
                if f.operation==OP.CutFeatureOperation:
                    cuts.append((f.timelineObject.index,c,f))
            except RuntimeError:
                # Linked features left by an earlier cross-component cut can be invalid.
                # The owning sketch feature is still processed at its timeline index.
                continue
    cuts.sort(key=lambda a:a[0])
    done=set()
    for idx,c,f in cuts:
        if idx in done: continue
        done.add(idx)
        f.timelineObject.rollTo(True)
        owner=f.profile.parentSketch.parentComponent
        old=f.participantBodies
        target=[b for b in old if b.parentComponent.name==owner.name]
        if not target:
            raise RuntimeError('Sin cuerpo destino propio: '+owner.name+' / '+f.name)
        f.participantBodies=target
        f.timelineObject.rollTo(False)
    D.timeline.moveToEnd()
    print('Cortes limitados a su componente:',len(cuts))

def run(context):
    try:
        setup()
        phase=globals().get('PHASE','init')
        fn=globals().get('stage_'+phase)
        if not fn: raise RuntimeError('Fase desconocida: '+phase)
        fn()
        adsk.doEvents()
        adsk.core.Application.get().activeViewport.refresh()
        print(json.dumps({'phase':phase,'document':adsk.core.Application.get().activeDocument.name,'result':'modelado aplicado'},ensure_ascii=False))
    except:
        print(traceback.format_exc())
        raise


