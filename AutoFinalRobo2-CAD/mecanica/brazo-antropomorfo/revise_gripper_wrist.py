# Revision geometrica autorizada: pinza y rangos, 26/10/2026.
# Ejecutar en Fusion, sobre el prototipo v01; fases explicitas, una sola vez.
exec(compile(open(r'D:\ProyectoRobotica2\AutoFinalRobo2-CAD\mecanica\brazo-antropomorfo\build_prototype.py',encoding='utf-8').read(),'build_prototype.py','exec'))

REV_NAME='AFR2_Brazo_PVC32_Prototipo_v02'

def param(n,e,u='mm',comment='PROPUESTO v02: geometria y movimiento.'):
    p=D.userParameters.itemByName(n)
    if p: p.expression=e
    else: D.userParameters.add(n,V(e),u,comment)

def drop_child(group,names):
    for o in list(comp(group).occurrences):
        if o.component.name in names: o.deleteMe()

def cage_mount(parent,name,prefix,face,z,x='0 mm'):
    c=cage_y(parent,name,prefix,face,z,x)
    L=prefix+'_l'; W=prefix+'_w'
    sx='micro_shaft_x' if prefix=='micro' else 'std_shaft_x'
    ear='4 mm' if prefix=='micro' else 'std_ear'
    for side in [-1,1]:
        px=f'({x})+{L}/2-{sx}+({side})*({L}/2+({ear})/2)'
        box(c,'Salida_pestana_'+str(side),f'({px})-6 mm',f'({face})+std_ear_z-0.3 mm',f'({z})-{W}/2-1 mm','12 mm','3.1 mm',f'{W}+2 mm',cut=True)
        box(c,'Apoyo_pestana_'+str(side),f'({px})-std_ear/2',f'({face})+std_ear_z+2.5 mm',f'({z})-{W}/2','std_ear','5 mm',W)
        ring(c,'Paso_M3_'+str(side),'Y',f'({face})+std_ear_z+2 mm',px,z,'3.4 mm',None,'6 mm',cut=True)
    return c

def stem_servo(parent,name,face):
    c=servo(parent,name,'micro','Z',face)
    h=sub(parent,name+'_Cuna_PETG')
    # Bottom cradle and longitudinal walls, clear of the servo body and tabs.
    box(h,'Piso','-micro_shaft_x-3 mm','-10 mm',f'({face})-micro_h-2 mm','micro_l+6 mm','20 mm','2 mm')
    for sign in [-1,1]:
        yy='micro_w/2+servo_clear' if sign>0 else '-micro_w/2-servo_clear-2.5 mm'
        box(h,'Pared_'+str(sign),'-micro_shaft_x-2.5 mm',yy,f'({face})-micro_h','micro_l+5 mm','2.5 mm','micro_h-11 mm')
    # Tabs rest on standoffs; their screw axes use the same editable reference as servo().
    for sign in [-1,1]:
        px=f'micro_l/2-micro_shaft_x+({sign})*(micro_l/2+2 mm)'
        box(h,'Torre_'+str(sign),f'({px})-2 mm','-micro_w/2',f'({face})-micro_h','4 mm','micro_w','micro_h-std_ear_z')
        ring(h,'Taladro_'+str(sign),'Z',f'({face})-micro_h-1 mm',px,'0 mm','2.5 mm',None,'micro_h',cut=True)
    return c,h

def polygon(c,s,pts,module='1 mm',cx='0 mm',cy='0 mm'):
    # Every vertex has driving coordinate dimensions; module and centre remain editable.
    s.isComputeDeferred=True
    try:
        coords=[(f'({cx})+({module})*({x:.12f})',f'({cy})+({module})*({y:.12f})') for x,y in pts]
        lines=[]
        for i,(ex,ey) in enumerate(coords):
            nx,ny=coords[(i+1)%len(coords)]
            lines.append(s.sketchCurves.sketchLines.addByTwoPoints(P(val(ex),val(ey),0),P(val(nx),val(ny),0)))
        # Remove inferred H/V/parallel constraints before adding driving coordinates.
        for con in reversed(list(s.geometricConstraints)):
            if con.isDeletable:con.deleteMe()
        for i,l in enumerate(lines):
            nxt=lines[(i+1)%len(lines)].startSketchPoint
            if l.endSketchPoint.entityToken!=nxt.entityToken:s.geometricConstraints.addCoincident(l.endSketchPoint,nxt)
        s.isComputeDeferred=False
        for i,(l,(ex,ey)) in enumerate(zip(lines,coords)):
            for expr,ori in [(ex,ORI.HorizontalDimensionOrientation),(ey,ORI.VerticalDimensionOrientation)]:
                try:
                    dd=s.sketchDimensions.addDistanceDimension(s.originPoint,l.startSketchPoint,ori,P(0.5,0.5,0),True)
                    dd.parameter.expression=expr if val(expr)>0 else '-('+expr+')'
                except RuntimeError as err:
                    if 'OVER_CONSTRAINT' not in str(err):raise
                    dd=s.sketchDimensions.addDistanceDimension(s.originPoint,l.startSketchPoint,ori,P(0.5,0.5,0),False)
                    if abs(dd.parameter.value-abs(val(expr)))>1e-6:
                        raise RuntimeError('Vertice inferido incompatible: '+str(i)+' '+expr+' actual '+str(dd.parameter.value))
    finally:s.isComputeDeferred=False

def involute_gear(parent,name,n,module,bore,z='0 mm',x='0 mm',axis='Z',phase='0 deg',thickness='4 mm'):
    c=sub(parent,name)
    root=n/2-1.25; rb=n/2*math.cos(math.radians(20)); ra=n/2+1
    half=math.pi/(2*n)-0.1/(2*(n/2))
    invp=math.tan(math.radians(20))-math.radians(20)
    flank=[]
    for k in range(9):
        rr=rb+(ra-rb)*k/8
        al=math.acos(rb/rr)
        th=half+invp-(math.tan(al)-al)
        flank.append((rr*math.cos(th),rr*math.sin(th)))
    th0=half+invp
    pts=[((root-0.2)*math.cos(th0),(root-0.2)*math.sin(th0))]+flank
    pts += [(a,-b) for a,b in reversed(flank)]
    pts += [((root-0.2)*math.cos(th0),-(root-0.2)*math.sin(th0))]
    f=ring(c,'Raiz_engranaje',axis,z,x,'0 mm',f'({module})*({2*root})',bore,thickness)
    # Native Y-plane has sketch Y=-world Z. Tooth profile is symmetric.
    s=c.sketches.add(datum(c,axis,z,'Plano_evolvente_20grados'))
    s.name='Evolvente_modular_20grados_con_holgura'
    polygon(c,s,pts,module,x,'0 mm')
    t=extrude(c,s,thickness,OP.JoinFeatureOperation,'Diente_evolvente')
    es=adsk.core.ObjectCollection.create();es.add(t)
    face=next(fa for fa in f.bodies.item(0).faces if adsk.core.Cylinder.cast(fa.geometry))
    ip=c.constructionAxes.createInput();ip.setByCircularFace(face)
    ax=c.constructionAxes.add(ip);ax.isLightBulbOn=False
    ip=c.features.circularPatternFeatures.createInput(es,ax);ip.quantity=V(str(n));ip.totalAngle=V('360 deg')
    c.features.circularPatternFeatures.add(ip).name='Patron_dientes_evolventes'
    finish(c,'red')
    return c

def stage_rev_prepare():
    app=adsk.core.Application.get()
    app.activeDocument.saveAs(REV_NAME,app.data.activeProject.rootFolder,'Revision pinza y movilidad solicitada.','')
    for ml in list(R.motionLinks): ml.deleteMe()
    for j in list(R.joints):
        if j.name.startswith(('J4_','J5_','J6_','G1_','G2_','REF_')): j.deleteMe()
    for o in list(R.occurrences):
        if o.component.name in ['04_Giro_muneca','05_Inclinacion_muneca','06_Herramienta','07_Mandibula_izquierda','08_Mandibula_derecha']:
            o.deleteMe()
    drop_child('03_Antebrazo_PVC',['M04_MG90S_Giro_muneca','A14_Alojamiento_giro_muneca_PETG','R04_608ZZ_Giro_muneca','T03_J4_Pinon_motor_REF'])
    for n in ['04_Giro_muneca','05_Inclinacion_muneca','06_Herramienta','07_Mandibula_izquierda','08_Mandibula_derecha','10_Pinion_pinza','11_Pinion_J4']:
        main(n)
    for n,e,u in [
      ('wrist45','55 mm','mm'),('wrist56','65 mm','mm'),('tcp_offset','78.5 mm','mm'),
      ('palm_w','118 mm','mm'),('palm_d','60 mm','mm'),('palm_t','4 mm','mm'),
      ('grip_module','1 mm','mm'),('grip_teeth','24',''),('grip_r','grip_module*grip_teeth/2','mm'),
      ('grip_min','4 mm','mm'),('grip_max','grip_min+2*grip_r*3.141592653589793','mm'),
      ('grip_deck','36 mm','mm'),('grip_angle','(50 mm-grip_min)/(2*grip_r)*180 deg/3.141592653589793','deg'),
      ('grip_open','grip_min+2*grip_r*grip_angle/(1 rad)','mm'),
      ('q4','0 deg','deg'),('q5','65 deg','deg'),('q6','0 deg','deg')]:param(n,e,u)
    R.attributes.add('AFR2','revision','v02: corregir pinza, comprobar movilidad CAD; no ensayo fisico.')

def stage_rev_roll_fixed():
    g=comp('03_Antebrazo_PVC')
    c=sub(g,'V02_A14_Soporte_J4_PETG')
    ring(c,'Asiento','Z','link2-9 mm','0 mm','0 mm','46 mm','b608_id+0.5 mm','9 mm')
    ring(c,'Rebaje_608','Z','link2-8 mm','0 mm','0 mm','b608_od+fit_clear',None,'7 mm',cut=True)
    # Socket/cup starts after the PVC cut and has no rotating material inside the bore.
    ring(c,'Copa','Z','link2-24 mm','0 mm','0 mm','46 mm','socket_od+0.3 mm','15 mm')
    # Fixed gear cover rear plate. Open front clearance at z=link2+7.
    box(c,'Puente_servo','28 mm','-10 mm','link2-34.5 mm','30 mm','20 mm','2 mm')
    servo(g,'V02_M04_MG90S_J4','micro','Z','link2-4 mm','36 mm')
    for sign in [-1,1]:
        yy='micro_w/2+servo_clear' if sign>0 else '-micro_w/2-servo_clear-2.5 mm'
        box(c,'Carril_'+str(sign),'36 mm-micro_shaft_x-2.5 mm',yy,'link2-32.5 mm','micro_l+5 mm','2.5 mm','24.5 mm')
    bearing(g,'V02_R04_608_J4','Z','link2-8 mm','0 mm','0 mm')
    # Two narrow lateral struts connect the bearing boss and the servo cage behind the gears.
    for sign in [-1,1]:
        box(c,'Travesano_'+str(sign),'16 mm',f'({sign})*8 mm-1.5 mm','link2-11 mm','40 mm','3 mm','3 mm')
    origin(g,'V02_J4_MOTOR','36 mm','0 mm','link2')

def stage_rev_roll_rotor():
    g=comp('04_Giro_muneca')
    c=sub(g,'V02_A15_Horquilla_J5_PETG')
    ring(c,'Rotor','Z','9 mm','0 mm','0 mm','38 mm','b608_id','6 mm')
    box(c,'Puente','-18 mm','-25 mm','14 mm','36 mm','50 mm','7 mm')
    for sign in [-1,1]:
        y=f'({sign})*21 mm-4 mm'
        box(c,'Brazo_'+str(sign),'-16 mm',y,'20 mm','32 mm','8 mm','wrist45-20 mm')
        ring(c,'Oreja_'+str(sign),'Y',y,'0 mm','wrist45','32 mm','b625_od+fit_clear','8 mm')
    join_all(c,'Horquilla_sin_material_en_barrido_del_nucleo')
    for sign in [-1,1]:bearing(g,'V02_R05_625_'+str(sign),'Y',f'({sign})*21 mm-b625_w/2','0 mm','wrist45','625')
    servo(g,'V02_M05_MG946_J5','mg','Y','38 mm','0 mm','wrist45')
    cage_mount(g,'V02_A16_Housing_MG946_J5','mg','38 mm','wrist45')
    c=sub(g,'V02_A17_Apoyo_motor_J5')
    for sign in [-1,1]:
        box(c,'Ménsula_'+str(sign),f'({sign})*18 mm-3 mm','23 mm','wrist45-18 mm','6 mm','18 mm','6 mm')
    c=sub(g,'V02_E03_Eje_J4')
    ring(c,'Eje','Z','-8 mm','0 mm','0 mm','8 mm',None,'23 mm','steel')
    origin(g,'J4_ROTOR');origin(g,'J5_INCLINACION','0 mm','0 mm','wrist45','Y')

def stage_rev_tilt():
    g=comp('05_Inclinacion_muneca')
    c=sub(g,'V02_A18_Nucleo_J5_y_columna_J6')
    ring(c,'Nucleo','Y','-16 mm','0 mm','0 mm','26 mm','b625_id','32 mm')
    box(c,'Columna','-8 mm','-8 mm','10 mm','16 mm','16 mm','wrist56-58.5 mm')
    for sign in [-1,1]:
        yy='9.5 mm' if sign>0 else '-12.5 mm'
        box(c,'Larguero_'+str(sign),'-8 mm',yy,'wrist56-46.5 mm','16 mm','3 mm','38.5 mm')
    ring(c,'Porta_625','Z','wrist56-8 mm','0 mm','0 mm','26 mm','b625_id+0.5 mm','8 mm')
    ring(c,'Rebaje_625','Z','wrist56-7 mm','0 mm','0 mm','b625_od+fit_clear',None,'5 mm',cut=True)
    # The servo rests 18 mm behind the roll plane; front coupler and bearing are separate.
    stem_servo(g,'V02_M06_MG90S_J6','wrist56-18 mm')
    bearing(g,'V02_R06_625_J6','Z','wrist56-7 mm','0 mm','0 mm','625')
    c=sub(g,'V02_E04_Eje_y_brida_J5')
    ring(c,'Eje','Y','-28 mm','0 mm','0 mm','5 mm',None,'60 mm','steel')
    ring(c,'Brida_horn','Y','27 mm','0 mm','0 mm','22 mm','2.5 mm','5 mm','steel')
    origin(g,'J5_NUCLEO','0 mm','0 mm','0 mm','Y')
    origin(g,'J6_HERRAMIENTA','0 mm','0 mm','wrist56','Z')

def stage_rev_palm():
    g=comp('06_Herramienta')
    c=sub(g,'V02_E05_Acople_J6')
    ring(c,'Brida','Z','-12 mm','0 mm','0 mm','20 mm','2.5 mm','3 mm','steel')
    ring(c,'Eje_5','Z','-9 mm','0 mm','0 mm','5 mm',None,'17 mm','steel')
    c=sub(g,'V02_A20_Bastidor_pinza')
    ring(c,'Plataforma_J6','Z','0 mm','0 mm','0 mm','30 mm','5 mm','5 mm')
    for sign in [-1,1]:
        box(c,'Columna_'+str(sign),'-18 mm',f'({sign})*12 mm-2 mm','5 mm','36 mm','4 mm','grip_deck-5 mm')
    box(c,'Base_guias','-palm_w/2','-palm_d/2','grip_deck','palm_w','palm_d','4 mm')
    ring(c,'Paso_salida_servo','Z','grip_deck-1 mm','0 mm','0 mm','6 mm',None,'6 mm',cut=True)
    # Fixed C-guides, with a central top slot for each jaw neck.
    for sign in [-1,1]:
        for yy in (['16.5 mm','25.5 mm'] if sign>0 else ['-27 mm','-18 mm']):
            box(c,'Pared_guia_'+str(sign)+yy,'-palm_w/2',yy,'grip_deck+4 mm','palm_w','1.5 mm','11 mm')
        for yy in (['17.5 mm','24.3 mm'] if sign>0 else ['-26 mm','-19.2 mm']):
            box(c,'Labio_'+str(sign)+yy,'-palm_w/2',yy,'grip_deck+13 mm','palm_w','1.7 mm','2 mm')
    stem_servo(g,'V02_M07_MG90S_Pinza','grip_deck+2 mm')
    origin(g,'J6_PALMA')
    origin(g,'GUIA_IZQ','-grip_min/2-finger_t/2-pad_t','0 mm','0 mm')
    origin(g,'GUIA_DER','grip_min/2+finger_t/2+pad_t','0 mm','0 mm')
    origin(g,'TCP','0 mm','0 mm','tcp_offset')
    origin(g,'PIEZA_CENTRO','0 mm','0 mm','grip_deck+52 mm')
    origin(g,'J7_PINION','0 mm','0 mm','grip_deck+8 mm')

def stage_rev_jaws():
    for sign,nm in [(-1,'07_Mandibula_izquierda'),(1,'08_Mandibula_derecha')]:
        g=comp(nm); c=sub(g,'V02_Mandibula_'+str(sign))
        yy='18.4 mm' if sign<0 else '-25.1 mm'
        box(c,'Carro','-8 mm',yy,'grip_deck+4.6 mm','16 mm','6.7 mm','7.8 mm')
        ny='19.7 mm' if sign<0 else '-23.8 mm'
        box(c,'Cuello','-3 mm',ny,'grip_deck+12 mm','6 mm','4.1 mm','9 mm')
        by='-8 mm' if sign<0 else '-24 mm'
        box(c,'Brazo','-3 mm',by,'grip_deck+17 mm','6 mm','32 mm','4 mm')
        box(c,'Dedo','-finger_t/2','-finger_depth/2','grip_deck+21 mm','finger_t','finger_depth','finger_length')
        # Rack is in the opposite lane to the other jaw: no overlapping carriage blocks.
        x='-5 mm' if sign<0 else '-58 mm'; y='13.25 mm' if sign<0 else '-17.25 mm'
        box(c,'Barra_cremallera',x,y,'grip_deck+8 mm','63 mm','4 mm','4 mm')
        # Connector lives outside the gear root and bridges to the guide carriage.
        cy='17 mm' if sign<0 else '-19 mm'
        box(c,'Union_barra_carro','-5 mm',cy,'grip_deck+8 mm','10 mm','2.5 mm','4 mm')
        join_all(c,'Carro_dedo_barra')
        pad=sub(g,'V02_Almohadilla_'+str(sign))
        px='finger_t/2' if sign<0 else '-finger_t/2-pad_t'
        box(pad,'Goma',px,'-finger_depth/2','grip_deck+24 mm','pad_t','finger_depth','finger_length-6 mm','rubber')
        origin(g,'CARRO')

def stage_rev_racks():
    for sign,nm in [(-1,'07_Mandibula_izquierda'),(1,'08_Mandibula_derecha')]:
        g=comp(nm); c=next(o.component for o in g.occurrences if o.component.name=='V02_Mandibula_'+str(sign))
        s=c.sketches.add(datum(c,'Z','grip_deck+8 mm','Plano_dentado_cremallera'))
        center=(7+math.pi/2-4*math.pi) if sign<0 else (-7-math.pi/2-15*math.pi)
        y=12 if sign<0 else -12
        base=math.pi/2-0.1+2*1.25*math.tan(math.radians(20));tip=math.pi/2-0.1-2*math.tan(math.radians(20))
        if sign<0:pts=[(-base/2,1.45),(base/2,1.45),(tip/2,-1),(-tip/2,-1)]
        else:pts=[(-base/2,-1.45),(base/2,-1.45),(tip/2,1),(-tip/2,1)]
        polygon(c,s,pts,'grip_module',f'{center:.12f} mm',f'{y} mm')
        f=extrude(c,s,'4 mm',OP.JoinFeatureOperation,'Diente_20grados')
        es=adsk.core.ObjectCollection.create();es.add(f)
        ip=c.features.rectangularPatternFeatures.createInput(es,c.xConstructionAxis,V('20'),V('3.141592653589793*grip_module'),adsk.fusion.PatternDistanceType.SpacingPatternDistanceType)
        c.features.rectangularPatternFeatures.add(ip).name='Cremallera_paso_pi_m'
        finish(c,'red')

def stage_rev_gear_pinza():
    g=comp('10_Pinion_pinza');involute_gear(g,'V02_Pinon24_Pinza',24,'grip_module','4.8 mm')
    origin(g,'EJE_PINION')

def stage_rev_gear_j4_driver():
    g=comp('11_Pinion_J4');involute_gear(g,'V02_Pinon24_J4',24,'1.5 mm','4.8 mm')
    origin(g,'EJE_PINION')

def stage_rev_gear_j4_driven():
    g=comp('04_Giro_muneca');involute_gear(g,'V02_Rueda24_J4',24,'1.5 mm','8 mm','2 mm')

def stage_rev_assemble():
    for o in R.occurrences:
        if o.component.name not in ['00_Base_control']:o.isGroundToParent=False
    j4=make_joint('J4_Giro_muneca','04_Giro_muneca','J4_ROTOR','03_Antebrazo_PVC','J4_MUNECA',angle='0 deg',limits=(-90,90))
    j5=make_joint('J5_Inclinacion_muneca','05_Inclinacion_muneca','J5_NUCLEO','04_Giro_muneca','J5_INCLINACION','Y','0 deg',limits=(-90,90))
    j6=make_joint('J6_Giro_herramienta','06_Herramienta','J6_PALMA','05_Inclinacion_muneca','J6_HERRAMIENTA',angle='0 deg',limits=(-90,90))
    j7=make_joint('J7_Servo_pinza','10_Pinion_pinza','EJE_PINION','06_Herramienta','J7_PINION',angle='0 deg',limits=(0,180))
    jd=make_joint('T_J4_Motor','11_Pinion_J4','EJE_PINION','03_Antebrazo_PVC','V02_J4_MOTOR',angle='0 deg')
    # Driver gear plane matches rotor gear plane at +2 mm from J4.
    jo=comp('03_Antebrazo_PVC').jointOrigins.itemByName('V02_J4_MOTOR')
    jo.offsetZ.expression='link2+2 mm'
    jl=make_joint('G1_Mordaza_izquierda','07_Mandibula_izquierda','CARRO','06_Herramienta','GUIA_IZQ',kind='slider')
    jr=make_joint('G2_Mordaza_derecha','08_Mandibula_derecha','CARRO','06_Herramienta','GUIA_DER',kind='slider')
    for j in [jl,jr]:
        lm=j.jointMotion.slideLimits;lm.isMinimumValueEnabled=True;lm.isMaximumValueEnabled=True
        lm.minimumValue=-val('grip_r')*math.pi if j==jl else 0
        lm.maximumValue=0 if j==jl else val('grip_r')*math.pi
    def link(name,a,b,v1,v2,rev):
        ip=R.motionLinks.createInput(a,b);ip.valueOne=V(v1);ip.valueTwo=V(v2);ip.isReversed=rev
        R.motionLinks.add(ip).name=name
    link('Servo_pinza_a_cremallera',j7,jl,'180 deg','grip_r*3.141592653589793',True)
    link('Mordazas_opuestas',jl,jr,'1 mm','1 mm',True)
    link('Transmision_J4_1a1',j4,jd,'1 deg','1 deg',True)
    # Half-tooth phase gives a tooth/gap engagement at the line between J4 axes.
    jd.angle.expression='7.5 deg'
    j5.jointMotion.rotationValue=math.radians(65)
    j7.jointMotion.rotationValue=val('grip_angle','rad')
    # Existing cube remains a reference; attach it to the new tool frame.
    make_joint('REF_Pieza_en_pinza','90_Pieza_REFERENCIA_50mm_100g','CENTRO_PIEZA','06_Herramienta','PIEZA_CENTRO',kind='rigid')
    stage_presentation()

def stage_rev_save():
    app=adsk.core.Application.get();doc=app.activeDocument
    if doc.isModified:doc.save('Revision mecanica de pinza y muñeca; ver informe de movimiento.')
    path=os.path.join(OUT_DIR,REV_NAME+'.f3d')
    D.exportManager.execute(D.exportManager.createFusionArchiveExportOptions(path))
    import csv
    with open(os.path.join(OUT_DIR,'parametros_prototipo_v02.csv'),'w',encoding='utf-8-sig',newline='') as s:
        w=csv.writer(s);w.writerow(['parametro','expresion','unidad','procedencia_y_estado'])
        for p in D.userParameters:w.writerow([p.name,p.expression,p.unit,p.comment])
    print('GUARDADO',path)

def stage_rev_proximal_prepare():
    import re
    param('base_h','69 mm');param('shoulder_z','185 mm')
    param('pinion_teeth','20','');param('wheel_teeth','40','');param('gear_t','6 mm')
    param('shoulder_phase','9 deg','deg')
    # Keep the base servo below its external bearing; model a real stepped coupling.
    for c in D.allComponents:
        if c.name in ['M01_MG946R_Giro_base','B04_Cuna_servo_base_PETG','B09_Postes_MG946_Base_PETG']:
            for p in list(c.modelParameters):
                ee=p.expression; nn=re.sub(r'base_top\s*-\s*6\s*mm','base_top-20 mm',ee)
                if nn!=ee:p.expression=nn
    param('shaft_shoulder_l','84 mm')
    for c in D.allComponents:
        if c.name=='E01_Eje_hombro_8mm':
            for pp in c.modelParameters:
                ex=pp.expression;nn=re.sub(r'76\s*mm','shaft_shoulder_l',ex)
                if ex!=nn:pp.expression=nn
        if c.name.startswith('Fijacion_MG946_Base_'):
            for pp in c.modelParameters:
                ex=pp.expression;nn=re.sub(r'base_h\s*-\s*20\s*mm','base_top-33.5 mm',ex)
                nn=re.sub(r'12\s*mm','8 mm',nn)
                if ex!=nn:pp.expression=nn
        if c.name=='B09_Postes_MG946_Base_PETG':
            for sk in c.sketches:
                if not sk.name.startswith('Croquis_Poste_'):continue
                for dd in sk.sketchDimensions:
                    ex=dd.parameter.expression
                    nn='std_ear' if ex=='8 mm' else re.sub(r'-\s*4\s*mm','-std_ear/2',ex)
                    if ex!=nn:dd.parameter.expression=nn
    for j in list(R.joints):
        if j.name=='T_Hombro_Pinion_2a1':j.deleteMe()
    drop_child('01_Giro_base',['A02_Torre_hombro_PETG','M02_GX3240_270_REF','A03_Housing_GX3240_PETG','A04_Puente_housing_hombro_PETG'])
    drop_child('09_Pinion_hombro',['T01_Pinion_16T_REFERENCIA'])
    drop_child('02_Brazo_PVC',['T02_Rueda_32T_REFERENCIA','M03_DS3218_Codo','A09_Housing_DS3218_PETG','A10_Puente_housing_codo'])
    drop_child('03_Antebrazo_PVC',['E02_Eje_codo_8mm'])
    # Retain existing joint origins and editable PVC geometry.
    jo=comp('01_Giro_base').jointOrigins.itemByName('J2_PINION')
    jo.offsetY.expression='-(tower_h-gear_centers)'

def stage_rev_tower():
    g=comp('01_Giro_base');c=sub(g,'V02_A02_Torre_con_continuidad')
    box(c,'Puente','-25 mm','-fork_y-fork_t/2','8 mm','50 mm','2*fork_y+fork_t','10 mm')
    for sign in [-1,1]:
        y=f'({sign})*fork_y-fork_t/2'
        box(c,'Oreja_continua_'+str(sign),'-23 mm',y,'8 mm','46 mm','fork_t','tower_h+12 mm')
        ring(c,'Asiento_608_'+str(sign),'Y',f'({y})-1 mm','0 mm','tower_h','b608_od+fit_clear',None,'fork_t+2 mm',cut=True)
    join_all(c,'Torre_continua')
    c=sub(g,'V02_E00_Acople_base_con_608')
    ring(c,'Brida_horn','Z','-14 mm','0 mm','0 mm','22 mm','2.5 mm','2 mm','steel')
    ring(c,'Cuello_8','Z','-12 mm','0 mm','0 mm','8 mm',None,'20 mm','steel')
    join_all(c,'Acople_escalonado')

def stage_rev_shoulder_motor():
    g=comp('01_Giro_base');zz='tower_h-gear_centers'
    servo(g,'V02_M02_GX3240_270','gx','Y','47 mm','0 mm',zz)
    cage_mount(g,'V02_A03_Housing_GX','gx','47 mm',zz)
    c=sub(g,'V02_A04_Apoyo_GX_fuera_del_pinon')
    for sign in [-1,1]:
        box(c,'Brazo_'+str(sign),f'({sign})*22 mm-3 mm','32.5 mm',f'({zz})-18 mm','6 mm','11.5 mm','4.4 mm')
    box(c,'Bandeja','-25 mm','44 mm',f'({zz})-18 mm','59 mm','14 mm','4.4 mm')
    join_all(c,'Apoyo_motor')

def stage_rev_elbow_motor():
    g=comp('02_Brazo_PVC')
    servo(g,'V02_M03_DS3218_Codo','ds','Y','46 mm','0 mm','link1')
    cage_mount(g,'V02_A09_Housing_DS3218','ds','46 mm','link1')
    c=sub(g,'V02_A10_Apoyo_DS3218')
    box(c,'Bandeja','-22 mm','32.5 mm','link1-18 mm','56 mm','57 mm','4.4 mm')
    g=comp('03_Antebrazo_PVC');c=sub(g,'V02_E02_Eje_y_brida_codo')
    ring(c,'Eje','Y','-38 mm','0 mm','0 mm','8 mm',None,'78 mm','steel')
    ring(c,'Brida','Y','34.5 mm','0 mm','0 mm','22 mm',None,'5.5 mm','steel')
    join_all(c,'Eje_con_brida')

def stage_rev_shoulder_driver():
    involute_gear(comp('09_Pinion_hombro'),'V02_Pinon20_Hombro',20,'gear_module','std_spline_d','35 mm',axis='Y',thickness='gear_t')

def stage_rev_shoulder_driven():
    involute_gear(comp('02_Brazo_PVC'),'V02_Rueda40_Hombro',40,'gear_module','b608_id','35 mm',axis='Y',thickness='gear_t')

def stage_rev_finish_mounts():
    # Correct geometry authored before these source refinements, without rebuilding the model.
    g=comp('03_Antebrazo_PVC');c=next(o.component for o in g.occurrences if o.component.name=='V02_A14_Soporte_J4_PETG')
    sk=c.sketches.itemByName('Croquis_Puente_servo')
    for dd in sk.sketchDimensions:
        if dd.parameter.expression=='18 mm':dd.parameter.expression='28 mm'
        elif dd.parameter.expression=='40 mm':dd.parameter.expression='30 mm'
    for sign in [-1,1]:
        px=f'36 mm+micro_l/2-micro_shaft_x+({sign})*(micro_l/2+2 mm)'
        zz='link2-25 mm' if sign<0 else 'link2-32.5 mm'
        dep='13 mm' if sign<0 else '20.5 mm'
        box(c,'Apoyo_tab_J4_'+str(sign),f'({px})-2 mm','-8 mm' if sign<0 else '-micro_w/2',zz,'4 mm','16 mm' if sign<0 else 'micro_w',dep)
        ring(c,'Taladro_tab_J4_'+str(sign),'Z',f'({zz})-1 mm',px,'0 mm','2.5 mm',None,f'({dep})+2 mm',cut=True)
    # The pitch motor shelf touches its housing instead of penetrating it.
    drop_child('04_Giro_muneca',['V02_A17_Apoyo_motor_J5'])
    c=sub(comp('04_Giro_muneca'),'V02_A17_Bandeja_motor_J5')
    box(c,'Bandeja','-16 mm','25 mm','wrist45-18 mm','51 mm','59 mm','18 mm-mg_w/2-servo_clear-housing_wall')
    import re
    cage=next(o.component for o in comp('04_Giro_muneca').occurrences if o.component.name=='V02_A16_Housing_MG946_J5')
    for sk in cage.sketches:
        if not sk.name.startswith('Croquis_Apoyo_pestana_'):continue
        for dd in sk.sketchDimensions:
            ex=dd.parameter.expression
            nn='std_ear' if ex=='8 mm' else re.sub(r'-\s*4\s*mm','-std_ear/2',ex)
            if ex!=nn:dd.parameter.expression=nn

def stage_rev_normalize_axes():
    # Fixed zero alignment, actual joint displacement and limits are in the same frame.
    for name,q,lim in [('J1_Giro_base',20,(-90,90)),('J2_Hombro',35,(-10,125)),('J3_Codo',-15,(-90,90))]:
        j=R.joints.itemByName(name);j.angle.expression='0 deg'
        lm=j.jointMotion.rotationLimits;lm.minimumValue=math.radians(lim[0]);lm.maximumValue=math.radians(lim[1])
        lm.isMinimumValueEnabled=True;lm.isMaximumValueEnabled=True
        j.jointMotion.rotationValue=0
    jp=make_joint('T_Hombro_Pinion_2a1','09_Pinion_hombro','EJE_PINION','01_Giro_base','J2_PINION','Y','shoulder_phase')
    j2=R.joints.itemByName('J2_Hombro')
    ip=R.motionLinks.createInput(j2,jp);ip.valueOne=V('1 deg');ip.valueTwo=V('gear_ratio*1 deg');ip.isReversed=True
    R.motionLinks.add(ip).name='Reduccion_hombro_2a1'
    for name,theta,pn in [('J1_Giro_base',20,'q1'),('J2_Hombro',35,'q2'),('J3_Codo',-15,'q3')]:
        param(pn,str(theta)+' deg','deg')
        R.joints.itemByName(name).jointMotion.rotationValue=math.radians(theta)
    for j in R.joints:
        if hasattr(j,'isLightBulbOn'):j.isLightBulbOn=False

def stage_rev_clearances():
    g=comp('06_Herramienta');c=next(o.component for o in g.occurrences if o.component.name=='V02_A20_Bastidor_pinza')
    box(c,'Paso_carcasa_MG90S','-micro_shaft_x-servo_clear','-micro_w/2-servo_clear','grip_deck-1 mm','micro_l+2*servo_clear','micro_w+2*servo_clear','4 mm',cut=True)
    c.constructionPlanes.itemByName('Plano_Plataforma_J6').definition.offset.expression='0.5 mm'
    for sign in [-1,1]:
        c.constructionPlanes.itemByName('Plano_Columna_'+str(sign)).definition.offset.expression='5.5 mm'
        c.features.extrudeFeatures.itemByName('Columna_'+str(sign)).extentOne.distance.expression='grip_deck-5.5 mm'
    # Connect the forearm socket to its J4 housing as one printable fitting.
    c=next(o.component for o in comp('03_Antebrazo_PVC').occurrences if o.component.name=='V02_A14_Soporte_J4_PETG')
    ring(c,'Union_al_manguito','Z','link2-24 mm','0 mm','0 mm','46 mm','pvc_od+socket_clear','4 mm')

def stage_rev_union_printed():
    group=globals().get('GROUP','06_Herramienta');g=comp(group)
    tm=adsk.fusion.TemporaryBRepManager.get();count=0
    def bodies():
        out=[]
        for o in g.occurrences:
            if group=='00_Base_control' and o.component.name in ['B03_Frente_desmontable_PETG','B05_Tapa_base_desmontable_PETG','B06_Alojamiento_608_base_PETG']:continue
            for b in o.component.bRepBodies:
                if b.isSolid and b.volume>1e-10 and b.appearance and b.appearance.name=='AFR2_PETG_Rojo':out.append(b)
        return out
    # Co-rigid intersecting or face-connected PETG is one physical printed part.
    # Preserve each contributing sketch/feature as editable source history.
    while True:
        bb=bodies(); merged=False
        for i,a in enumerate(bb):
            for b in bb[i+1:]:
                aa=a.boundingBox;zz=b.boundingBox
                av=list(aa.minPoint.asArray())+list(aa.maxPoint.asArray());bv=list(zz.minPoint.asArray())+list(zz.maxPoint.asArray())
                if any(min(av[k+3],bv[k+3])<max(av[k],bv[k])-1e-7 for k in range(3)):continue
                cp=tm.copy(a)
                try:ok=tm.booleanOperation(cp,b,adsk.fusion.BooleanTypes.UnionBooleanType)
                except:continue
                if not ok or cp.lumps.count!=1:continue
                tools=adsk.core.ObjectCollection.create();tools.add(b)
                ip=a.parentComponent.features.combineFeatures.createInput(a,tools)
                ip.operation=OP.JoinFeatureOperation;ip.isKeepToolBodies=False
                a.parentComponent.features.combineFeatures.add(ip).name='V02_Unificar_impreso_continuo'
                merged=True;count+=1;break
            if merged:break
        if not merged:break
    # Re-open clamp slits where the newly integrated collar crosses them.
    if group in ['02_Brazo_PVC','03_Antebrazo_PVC']:
        for o in g.occurrences:
            c=o.component
            if not c.bRepBodies.count:continue
            # Only an integrated socket body occupies the annulus at this station.
            for start in (['socket_start','link1-socket_start-socket_length'] if group.startswith('02') else ['socket_start','link2-socket_start-socket_length']):
                # A fresh scoped cut has participants only if its slot intersects this body.
                zz=val(start)
                if any(b.boundingBox.minPoint.z<zz+val('socket_length') and b.boundingBox.maxPoint.z>zz for b in c.bRepBodies) and c.name.startswith(('A05','A06','A07','A08','A11','A12','A13','V02_A14')):
                    try:
                        if group=='03_Antebrazo_PVC' and start=='socket_start' and c.name.startswith('A11'):
                            box(c,'Ranura_integrada_'+start,'-0.75 mm','pvc_od/2-1 mm',f'({start})-1 mm','1.5 mm','13 mm','socket_length+2 mm',cut=True)
                        else:
                            box(c,'Ranura_integrada_'+start,'pvc_od/2-1 mm','-0.75 mm',f'({start})-1 mm','13 mm','1.5 mm','socket_length+2 mm',cut=True)
                    except RuntimeError as ex:
                        if 'No target' not in str(ex) and 'participant' not in str(ex).lower():raise
    print('Impresos_unificados',group,count)

def stage_rev_contact_fixes():
    c=next(o.component for o in comp('04_Giro_muneca').occurrences if o.component.name=='V02_A15_Horquilla_J5_PETG')
    for sign in [-1,1]:
        ring(c,'Paso_completo_625_'+str(sign),'Y',f'({sign})*21 mm-5 mm','0 mm','wrist45','b625_od+fit_clear',None,'10 mm',cut=True)
    c=next(o.component for o in comp('05_Inclinacion_muneca').occurrences if o.component.name=='V02_A18_Nucleo_J5_y_columna_J6')
    for sign in [-1,1]:
        sk=c.sketches.itemByName('Croquis_Larguero_'+str(sign))
        for dd in sk.sketchDimensions:
            if dd.parameter.expression==('9.5 mm' if sign>0 else '12.5 mm') or (sign<0 and '12.5 mm' in dd.parameter.expression):
                dd.parameter.expression='12.5 mm' if sign>0 else '15.5 mm'
    c=next(o.component for o in comp('06_Herramienta').occurrences if o.component.name=='V02_A20_Bastidor_pinza')
    for nm,new_y in [('Pared_guia_116.5 mm','17.5 mm'),('Pared_guia_-1-18 mm','-18.1 mm')]:
        c.features.extrudeFeatures.itemByName(nm).extentOne.distance.expression='3.5 mm'
        sk=c.sketches.itemByName('Croquis_'+nm)
        for dd in sk.sketchDimensions:
            ex=dd.parameter.expression
            if ex in ['16.5 mm','18 mm']:dd.parameter.expression=new_y if val(new_y)>0 else '-('+new_y+')'
            elif ex=='1.5 mm':dd.parameter.expression='0.6 mm'

def stage_rev_clamp_orientation():
    for cn in ['A11_Encastre_proximal_antebrazo_PETG','A11_Encastre_proximal_antebrazo_PETG_Perno_M3_REF']:
        c=next(c for c in D.allComponents if c.name==cn)
        es=adsk.core.ObjectCollection.create()
        for b in c.bRepBodies:es.add(b)
        ip=c.features.moveFeatures.createInput2(es)
        parent=occ('03_Antebrazo_PVC').transform2
        ax=adsk.core.Vector3D.create(0,0,1);ax.transformBy(parent)
        centre=P(0,0,0);centre.transformBy(parent)
        m=adsk.core.Matrix3D.create();m.setToRotation(math.pi/2,ax,centre)
        ip.defineAsFreeMove(m)
        c.features.moveFeatures.add(ip).name='V02_Abrazadera_fuera_del_plano_de_flexion'

def stage_rev_clamp_slot_fix():
    c=next(c for c in D.allComponents if c.name=='A11_Encastre_proximal_antebrazo_PETG')
    f=c.features.extrudeFeatures.itemByName('Ranura_integrada_socket_start')
    if f:f.isSuppressed=True
    box(c,'Ranura_integrada_orientada_Y','-0.75 mm','pvc_od/2-1 mm','socket_start-1 mm','1.5 mm','13 mm','socket_length+2 mm',cut=True)

def stage_rev_pose():
    vals=globals().get('POSE',[20,45,45,0,90,0,110])
    names=['J1_Giro_base','J2_Hombro','J3_Codo','J4_Giro_muneca','J5_Inclinacion_muneca','J6_Giro_herramienta','J7_Servo_pinza']
    for n,theta in zip(names,vals):R.joints.itemByName(n).jointMotion.rotationValue=math.radians(theta-90 if n=='J7_Servo_pinza' else theta)
    if D.snapshots.hasPendingSnapshot:D.snapshots.add()
    print('Pose_capturada',vals)

def stage_rev_contact_calibration():
    param('grip_min','50 mm-2*grip_r*(109.8 deg)/(1 rad)',comment='Referencia geometrica: agarre50 mm al angulo109.8 grados capturable por Fusion; no calibracion fisica.')
    param('grip_contact','109.8 deg','deg','REFERENCIA CAD: contacto50 mm; calibracion real de PWM pendiente.')

def stage_rev_internal_mount_fixes():
    c=next(c for c in D.allComponents if c.name=='V02_A15_Horquilla_J5_PETG')
    ring(c,'Paso_eje_J4_completo','Z','-1 mm','0 mm','0 mm','8 mm',None,'23 mm',cut=True)
    c=next(c for c in D.allComponents if c.name=='V02_A20_Bastidor_pinza')
    ring(c,'Paso_final_eje_J6','Z','5 mm','0 mm','0 mm','5.3 mm',None,'4.5 mm',cut=True)
    c=next(c for c in D.allComponents if c.name=='B01_Piso_base_PETG')
    ring(c,'Paso_conector_12V','Y','base_w/2-base_wall-1 mm','60 mm','25 mm','12.5 mm',None,'base_wall+2 mm',cut=True)
    box(c,'Paso_interruptor','-67.2 mm','base_w/2-base_wall-1 mm','17.8 mm','14.4 mm','base_wall+2 mm','20.4 mm',cut=True)
    for cn,delta in [('E03_XL4016_REF',-2),('E04_XL4016_REF',-2),('E05_XL4016_REF',6)]:
        cc=next(c for c in D.allComponents if c.name==cn)
        es=adsk.core.ObjectCollection.create()
        for b in cc.bRepBodies:es.add(b)
        ip=cc.features.moveFeatures.createInput2(es)
        m=adsk.core.Matrix3D.create();m.translation=adsk.core.Vector3D.create(0,delta/10,0)
        ip.defineAsFreeMove(m);cc.features.moveFeatures.add(ip).name='V02_Modulo_fuera_del_soporte_servo'
    import re
    cc=next(c for c in D.allComponents if c.name=='B08_Retencion_placas_PETG')
    for sk in cc.sketches:
        if not sk.name.startswith('Croquis_Apoyo_buck'):continue
        for dd in sk.sketchDimensions:
            e=dd.parameter.expression;n=re.sub(r'-\s*37\s*mm','-39 mm',e);n=re.sub(r'33\s*mm','39 mm',n)
            if n!=e:dd.parameter.expression=n

def stage_rev_rebuild_grip_joints():
    for ml in list(R.motionLinks):
        if ml.name in ['Servo_pinza_a_cremallera','Mordazas_opuestas']:ml.deleteMe()
    for n in ['J7_Servo_pinza','G1_Mordaza_izquierda','G2_Mordaza_derecha']:
        R.joints.itemByName(n).deleteMe()
    for gn in ['07_Mandibula_izquierda','08_Mandibula_derecha','10_Pinion_pinza']:
        o=occ(gn);o.isGroundToParent=False;o.isGrounded=False
    param('grip_mid','grip_min+grip_r*3.141592653589793')
    tool=comp('06_Herramienta')
    tool.jointOrigins.itemByName('GUIA_IZQ').offsetX.expression='-grip_mid/2-finger_t/2-pad_t'
    tool.jointOrigins.itemByName('GUIA_DER').offsetX.expression='grip_mid/2+finger_t/2+pad_t'
    j7=make_joint('J7_Servo_pinza','10_Pinion_pinza','EJE_PINION','06_Herramienta','J7_PINION',angle='90 deg',limits=(-90,90))
    jl=make_joint('G1_Mordaza_izquierda','07_Mandibula_izquierda','CARRO','06_Herramienta','GUIA_IZQ',kind='slider')
    jr=make_joint('G2_Mordaza_derecha','08_Mandibula_derecha','CARRO','06_Herramienta','GUIA_DER',kind='slider')
    half=val('grip_r')*math.pi/2
    for j,lo,hi in [(jl,-half,half),(jr,-half,half)]:
        lm=j.jointMotion.slideLimits;lm.minimumValue=lo;lm.maximumValue=hi
        lm.isMinimumValueEnabled=True;lm.isMaximumValueEnabled=True
    for n,a,b,v1,v2 in [('Servo_pinza_a_cremallera',j7,jl,'180 deg','grip_r*3.141592653589793'),('Mordazas_opuestas',jl,jr,'1 mm','1 mm')]:
        ip=R.motionLinks.createInput(a,b);ip.valueOne=V(v1);ip.valueTwo=V(v2);ip.isReversed=True
        R.motionLinks.add(ip).name=n
    j7.jointMotion.rotationValue=math.radians(109.8-90)
    print('Pinza_servo_grados',90+math.degrees(j7.jointMotion.rotationValue),jl.jointMotion.slideValue,jr.jointMotion.slideValue)
    if D.snapshots.hasPendingSnapshot:D.snapshots.add()

def stage_rev_native_grip_audit():
    rows=[];j=R.joints.itemByName('J7_Servo_pinza')
    for target in [0,30,60,90,109.8,133.7,180,133.7,109.8]:
        j.jointMotion.rotationValue=math.radians(target-90)
        actual=90+math.degrees(j.jointMotion.rotationValue)
        tf=occ('06_Herramienta').transform2;tf.invert()
        xs=[]
        for gn in ['07_Mandibula_izquierda','08_Mandibula_derecha']:
            pt=P(0,0,0);pt.transformBy(occ(gn).transform2);pt.transformBy(tf);xs.append(pt.x)
        gap=(xs[1]-xs[0]-val('finger_t')-2*val('pad_t'))*10
        expected=(val('grip_min')+2*val('grip_r')*math.radians(actual))*10
        rows.append({'servo_requested_deg':target,'servo_actual_CAD_deg':actual,'opening_measured_mm':gap,'opening_expected_mm':expected,'error_mm':gap-expected})
    if D.snapshots.hasPendingSnapshot:D.snapshots.add()
    dest=os.path.join(OUT_DIR,'revision_v02','apertura_nativa_fusion.json')
    with open(dest,'w',encoding='utf-8') as f:json.dump(rows,f,ensure_ascii=False,indent=2)
    print('Apertura_nativa',json.dumps(rows,ensure_ascii=False))

def stage_rev_gallery():
    app=adsk.core.Application.get();folder=os.path.join(ROOT_DIR,'renders','2026-10-26');os.makedirs(folder,exist_ok=True)
    for c in D.allComponents:
        for prop in ['isSketchFolderLightBulbOn','isConstructionFolderLightBulbOn','isOriginFolderLightBulbOn','isJointsFolderLightBulbOn']:
            if hasattr(c,prop):setattr(c,prop,False)
    for j in R.joints:
        if hasattr(j,'isLightBulbOn'):j.isLightBulbOn=False
    cam=app.activeViewport.camera
    cam.eye=P(58,65,48);cam.target=P(12,0,22);cam.upVector=adsk.core.Vector3D.create(0,0,1);cam.isFitView=True
    app.activeViewport.camera=cam
    app.activeViewport.saveAsImageFile(os.path.join(folder,'brazo_pvc32_v02.png'),1600,1200)
    full=app.activeViewport.camera
    hidden=[]
    for o in R.occurrences:
        if o.component.name not in ['06_Herramienta','07_Mandibula_izquierda','08_Mandibula_derecha','10_Pinion_pinza','90_Pieza_REFERENCIA_50mm_100g']:
            hidden.append((o,o.isLightBulbOn));o.isLightBulbOn=False
    tf=occ('06_Herramienta').transform2
    eye=P(16,-20,14);target=P(0,0,5.5);up=adsk.core.Vector3D.create(0,0,1)
    eye.transformBy(tf);target.transformBy(tf);up.transformBy(tf)
    cam=app.activeViewport.camera;cam.eye=eye;cam.target=target;cam.upVector=up;cam.isFitView=True;app.activeViewport.camera=cam
    for theta,nm in [(133.7,'pinza_v02_abierta_60mm.png'),(109.8,'pinza_v02_cerrada_50mm.png')]:
        R.joints.itemByName('J7_Servo_pinza').jointMotion.rotationValue=math.radians(theta-90)
        app.activeViewport.refresh();adsk.doEvents()
        app.activeViewport.saveAsImageFile(os.path.join(folder,nm),1600,1200)
    for o,state in hidden:o.isLightBulbOn=state
    if D.snapshots.hasPendingSnapshot:D.snapshots.add()
    app.activeViewport.camera=full

def stage_rev_native_axes_audit():
    names=['J1_Giro_base','J2_Hombro','J3_Codo','J4_Giro_muneca','J5_Inclinacion_muneca','J6_Giro_herramienta']
    rows=[]
    for i,(lo,hi) in enumerate([(-90,90),(-10,125),(-90,90),(-90,90),(-90,90),(-90,90)]):
        rest=[0,35,-15,0,0,0]
        if i==1:rest[2]=-90
        if i==2:rest[1]=0
        for nm,q in zip(names,rest):R.joints.itemByName(nm).jointMotion.rotationValue=math.radians(q)
        for q in [lo,(lo+hi)/2,hi]:
            j=R.joints.itemByName(names[i]);j.jointMotion.rotationValue=math.radians(q)
            actual=math.degrees(j.jointMotion.rotationValue)
            rows.append({'axis':i+1,'requested_deg':q,'actual_CAD_deg':actual,'error_deg':actual-q,'rest':rest})
    for nm,q in zip(names,[20,45,45,0,90,0]):R.joints.itemByName(nm).jointMotion.rotationValue=math.radians(q)
    R.joints.itemByName('J7_Servo_pinza').jointMotion.rotationValue=math.radians(19.8)
    if D.snapshots.hasPendingSnapshot:D.snapshots.add()
    with open(os.path.join(OUT_DIR,'revision_v02','extremos_nativos_fusion.json'),'w',encoding='utf-8') as f:json.dump(rows,f,ensure_ascii=False,indent=2)
    print('Ejes_nativos',len(rows),'error_max_grados',max(abs(r['error_deg']) for r in rows))
    print('Desvios',[r for r in rows if abs(r['error_deg'])>0.01])


def run(context):
    setup()
    phase=globals().get('PHASE')
    globals()['stage_'+phase]()
    adsk.doEvents();adsk.core.Application.get().activeViewport.refresh()
    print(json.dumps({'fase':phase,'documento':adsk.core.Application.get().activeDocument.name,'estado':'aplicado'},ensure_ascii=False))
