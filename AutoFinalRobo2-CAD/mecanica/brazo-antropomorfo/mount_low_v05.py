# Fusion v05: montura baja, eje J1 retrasado y recorte del pan.
# Fases explicitas, una sola vez sobre la copia v05; v04 se conserva.
exec(compile(open(r'D:\ProyectoRobotica2\AutoFinalRobo2-CAD\mecanica\brazo-antropomorfo\mount_pan_v04.py',encoding='utf-8').read(),'mount_pan_v04.py','exec'))
V05='AFR2_Brazo_PVC32_AGV_v05';REPORT=os.path.join(OUT_DIR,'revision_v05')
ALL_HOLES=HOLES+LEFT

def extrude(c,s,depth,op,name,profile=None):
    # Keep the edited component active across successive operations. Scope cuts
    # explicitly; this avoids repeatedly activating the full detailed assembly.
    if D.activeComponent.name!=c.name:
        next(o for o in R.allOccurrences if o.component.name==c.name).activate()
    if profile is None:profile=s.profiles.item(0)
    ip=c.features.extrudeFeatures.createInput(profile,op);ip.setDistanceExtent(False,V(str(depth)))
    if op==OP.CutFeatureOperation:ip.participantBodies=[b for b in c.bRepBodies if b.isSolid and b.volume>1e-10]
    f=c.features.extrudeFeatures.add(ip);f.name=name
    if any(b.parentComponent.name!=c.name for b in f.bodies):raise RuntimeError('Operacion fuera de su componente: '+name)
    s.isVisible=False
    progress('operacion: '+name)
    return f

def shifted(c,x='0 mm',y='0 mm',z='0 mm'):
    f=c.features.moveFeatures.itemByName('V04_Alza_parametrica_para_pan_HCSR04')
    if not f:raise RuntimeError('Falta traslacion de '+c.name)
    dd=adsk.fusion.MoveFeatureTranslateXYZDefinition.cast(f.definition)
    dd.xDistance.expression=x;dd.yDistance.expression=y;dd.zDistance.expression=z

def boxes(c,name,z,height,rectangles):
    s=c.sketches.add(datum(c,'Z',z,'Plano_'+name));s.name='Croquis_'+name;s.isComputeDeferred=True
    loops=[]
    for x,y,w,depth in rectangles:
        lines=s.sketchCurves.sketchLines.addTwoPointRectangle(P(val(x),val(y),0),P(val(f'({x})+({w})'),val(f'({y})+({depth})'),0))
        loops.append((list(lines),x,y,w,depth))
    # Fusion infers alignment between repeated rectangles. Remove those
    # cross-profile constraints before giving each rectangle driving dimensions.
    for co in reversed(list(s.geometricConstraints)):
        if co.isDeletable:co.deleteMe()
    for lines,x,y,w,depth in loops:
        for i,line in enumerate(lines):
            nxt=lines[(i+1)%4].startSketchPoint
            if line.endSketchPoint.entityToken!=nxt.entityToken:s.geometricConstraints.addCoincident(line.endSketchPoint,nxt)
        for i in [0,2]:s.geometricConstraints.addHorizontal(lines[i])
        for i in [1,3]:s.geometricConstraints.addVertical(lines[i])
        locate(s,lines[0].startSketchPoint,x,y)
        dd=s.sketchDimensions.addDistanceDimension(lines[0].startSketchPoint,lines[0].endSketchPoint,ORI.HorizontalDimensionOrientation,P(0,-.5,0));dd.parameter.expression=w
        dd=s.sketchDimensions.addDistanceDimension(lines[1].startSketchPoint,lines[1].endSketchPoint,ORI.VerticalDimensionOrientation,P(.5,0,0));dd.parameter.expression=depth
    s.isComputeDeferred=False
    profiles=adsk.core.ObjectCollection.create()
    for pr in s.profiles:profiles.add(pr)
    f=extrude(c,s,height,OP.NewBodyFeatureOperation,name,profiles)
    for b in f.bodies:b.name=name;b.appearance=AP['red']
    return f

def stage_v05_prepare():
    # Remove the obsolete printed structure, retaining independent commercial
    # components, mounts on both sides, all arm joints and the moving pan.
    g=comp('00_Base_control')
    for o in list(g.occurrences):
        if o.component.name in ['V03_Plataforma_sandwich_superior','V04_Anclaje_M10x80']:o.deleteMe()
    for o in R.occurrences:o.isLightBulbOn=False
    occ('00_Base_control').isLightBulbOn=True
    for n,e in [('agv_mount_z','6 mm'),('agv_pan_lift','0 mm'),('v05_axis_y','-14 mm'),('v05_motor_bottom','agv_mount_z+agv_mount_t+9.1 mm'),('v05_motor_face','v05_motor_bottom+mg_h'),('v05_rack_y','-18 mm'),('electronics_z','26 mm'),('v05_rack_rail_z','22 mm'),('v05_logic_z','15 mm')]:param(n,e,comment='PROPUESTO v05: montura baja y reserva del pan.')
    param('base_h','v05_motor_face+32 mm-lid_t',comment='DERIVADO v05: interfaz J1 baja, sin alza general.')
    param('shoulder_z','base_top+113 mm',comment='DERIVADO v05: conserva geometria local del hombro.')
    # The same 608 supports, cap, screws and motor are repositioned together.
    fixed=['V03_M01_MG946_Base','V03_Inserto_M3_servo_J1_-1','V03_Fijacion_servo_J1_-1','V03_Inserto_M3_servo_J1_1','V03_Fijacion_servo_J1_1','V03_R01_608_J1_inferior','V03_R01_608_J1_superior','V03_Tapa_rodamientos_J1','V03_Tornillo_tapa_J1_-1','V03_Inserto_M3_J1_-1','V03_Tornillo_tapa_J1_1','V03_Inserto_M3_J1_1']
    for name in fixed:
        shifted(gc(name),y='v05_axis_y',z='v05_motor_face-80 mm');progress('reposicionado: '+name)
    for i in range(1,4):shifted(gc('V03_XL4016_vertical_'+str(i)),y='v05_rack_y')
    shifted(gc('V03_PCA9685_vertical'),z='v05_logic_z-29 mm')
    shifted(gc('V03_Nano_vertical'),z='v05_logic_z-31 mm')
    shifted(gc('V03_Bornera_y_alimentacion_REF'),x='49 mm',y='-1 mm',z='agv_mount_z+agv_mount_t-28 mm')
    comp('00_Base_control').jointOrigins.itemByName('J1_BASE').offsetY.expression='v05_axis_y'
    if D.snapshots.hasPendingSnapshot:D.snapshots.add()
    for label,x,y in ALL_HOLES:
        c=gc('V04_Anclaje_M3x70_'+label)
        c.constructionPlanes.itemByName('Plano_Tornillo_vastago').definition.offset.expression='agv_mount_z_final+agv_mount_t-25 mm'
        c.features.extrudeFeatures.itemByName('Tornillo_vastago').extentOne.distance.expression='25 mm'
        c.name='V05_Anclaje_M3x25_'+label;progress('M3x25: '+label)
    sub(g,'V05_Montura_baja_PETG')

def conical(c,name,z0,d0,z1,d1,cut=False,key='steel'):
    sections=[]
    for suffix,z,diam in [('inferior',z0,d0),('superior',z1,d1)]:
        s=c.sketches.add(datum(c,'Z',z,'Plano_'+name+'_'+suffix));s.name='Croquis_'+name+'_'+suffix
        circle(s,'agv_center_x','agv_center_y',diam);sections.append(s)
    if cut:
        target=next(o for o in R.allOccurrences if o.component.name==c.name);target.activate()
    ip=c.features.loftFeatures.createInput(OP.CutFeatureOperation if cut else OP.NewBodyFeatureOperation)
    for s in sections:ip.loftSections.add(s.profiles.item(0))
    if cut:ip.participantBodies=[b for b in c.bRepBodies if b.isSolid]
    f=c.features.loftFeatures.add(ip);f.name=name
    if cut:
        if any(b.parentComponent.name!=c.name for b in f.bodies):raise RuntimeError('Loft fuera de su componente: '+name)
        D.activateRootComponent()
    for s in sections:s.isVisible=False
    if not cut:
        for b in f.bodies:b.appearance=AP[key]
    return f

def stage_v05_floor():
    param('v05_rack_rail_z','agv_mount_z+agv_mount_t+10 mm',comment='DERIVADO v05: puente 10 mm sobre cara superior de montura.')
    param('electronics_z','v05_rack_rail_z+4 mm',comment='DERIVADO v05: placas XL sobre el puente.')
    param('v05_logic_z','agv_mount_z+agv_mount_t+3 mm',comment='DERIVADO v05: placas logicas 3 mm sobre montura.')
    c=gc('V05_Montura_baja_PETG')
    box(c,'Placa_baja','-96 mm','-31 mm','agv_mount_z','172 mm','63 mm','agv_mount_t')
    box(c,'Ventana_voltimetro','-68 mm','-9 mm','agv_mount_z-1 mm','37 mm','18 mm','agv_mount_t+2 mm',cut=True)
    for label,x,y in ALL_HOLES:ring(c,'Paso_M3_'+label,'Z','agv_mount_z-1 mm','agv_'+label+'_x','agv_'+label+'_y','3.4 mm',None,'agv_mount_t+2 mm',cut=True)
    ring(c,'Paso_M10','Z','agv_mount_z-1 mm','agv_center_x','agv_center_y','10.5 mm',None,'agv_mount_t+2 mm',cut=True)
    # DIN 7991 M10: nominal head D20/h5.5, 90 degree cone and 0.5 rim.
    conical(c,'Asiento_conico_M10','agv_mount_z+agv_mount_t-5.25 mm','10.5 mm','agv_mount_z+agv_mount_t-0.5 mm','20 mm',True)
    ring(c,'Rebaje_borde_M10','Z','agv_mount_z+agv_mount_t-0.5 mm','agv_center_x','agv_center_y','20.4 mm',None,'1.5 mm',cut=True)
    ring(c,'Recorte_circular_pan','Z','agv_mount_z-1 mm','pan_axis_x','pan_axis_y','2*(pan_radius_bound+pan_clearance)',None,'agv_mount_t+2 mm',cut=True)
    g=comp('00_Base_control');bolt=sub(g,'V05_M10x30_DIN7991_REF')
    ring(bolt,'Vastago','Z','agv_mount_z+agv_mount_t-30 mm','agv_center_x','agv_center_y','10 mm',None,'24.5 mm','steel')
    conical(bolt,'Cabeza_conica','agv_mount_z+agv_mount_t-5.5 mm','10 mm','agv_mount_z+agv_mount_t-0.5 mm','20 mm')
    ring(bolt,'Borde_cabeza','Z','agv_mount_z+agv_mount_t-0.5 mm','agv_center_x','agv_center_y','20 mm',None,'0.5 mm','steel')
    join_all(bolt,'M10x30_comercial_sin_rosca_modelada')

def stage_v05_j1_frame():
    c=gc('V05_Montura_baja_PETG');cy='v05_axis_y';face='v05_motor_face';floor='agv_mount_z+agv_mount_t'
    # The Model Y board projects to Y=-30 mm at Z=6.5..16.5 mm.
    # A rear-left relief keeps 3 mm in front of it at the new low floor.
    box(c,'Retiro_frente_ModelY','-73 mm','-32 mm','agv_mount_z-1 mm','49.5 mm','5 mm','agv_mount_t+2 mm',cut=True)
    for sx in [-1,1]:
        for sy in [-1,1]:
            yy=f'{cy}-13 mm' if sx<0 and sy<0 else f'{cy}+({sy})*14 mm-3 mm'
            box(c,'Columna_J1_'+str(sx)+str(sy),f'({sx})*22 mm-3 mm',yy,floor,'6 mm','6 mm',f'{face}+2 mm-({floor})')
    box(c,'Puente_J1','-25 mm',f'{cy}-25 mm',f'{face}+2 mm','50 mm','50 mm','8 mm')
    ring(c,'Torre_J1','Z',f'{face}+8 mm','0 mm',cy,'40 mm','11.6 mm','18 mm')
    ring(c,'Paso_eje_puente','Z',f'{face}+1 mm','0 mm',cy,'11.6 mm',None,'10 mm',cut=True)
    ring(c,'Caja_brida_horn','Z',f'{face}+1 mm','0 mm',cy,'24 mm',None,'7.5 mm',cut=True)
    for dz in [10,18]:ring(c,'Asiento_608_'+str(dz),'Z',f'{face}+{dz} mm','0 mm',cy,'b608_od+fit_clear',None,'7 mm',cut=True)
    ring(c,'Asiento_pestana_tapa','Z',f'{face}+25 mm','0 mm',cy,'b608_od+fit_clear',None,'1.2 mm',cut=True)
    for side in [-1,1]:
        x=f'mg_l/2-std_shaft_x+({side})*(mg_l/2+std_ear/2)'
        box(c,'Poste_motor_'+str(side),f'({x})-std_ear/2',f'{cy}-mg_w/2',floor,'std_ear','mg_w',f'{face}-std_ear_z-({floor})')
        ring(c,'Paso_pestana_'+str(side),'Z',f'{face}-10 mm',x,cy,'4.2 mm',None,'7 mm',cut=True)
        ring(c,'Inserto_motor_'+str(side),'Z',f'{face}-14 mm',x,cy,'4.5 mm',None,'4 mm',cut=True)
        ring(c,'Piloto_tornillo_'+str(side),'Z',f'{face}-19 mm',x,cy,'3.3 mm',None,'5.1 mm',cut=True)
        ring(c,'Paso_tapa_'+str(side),'Z',f'{face}+18 mm',f'({side})*16 mm',cy,'3.4 mm',None,'9 mm',cut=True)
        ring(c,'Inserto_tapa_'+str(side),'Z',f'{face}+21 mm',f'({side})*16 mm',cy,'4.5 mm',None,'4 mm',cut=True)
    # The rearward right servo tab crosses anchor C in plan. Leave an axial
    # access channel for the M3 head/key; fit the servo after tightening C.
    ring(c,'Acceso_vertical_anclaje_C','Z',floor,'agv_C_x','agv_C_y','7 mm',None,f'{face}-std_ear_z-({floor})+1 mm',cut=True)
    unite(c,'V05_Placa_y_apoyo_J1_continuos')

def stage_v05_xl_rack():
    c=gc('V05_Montura_baja_PETG');floor='agv_mount_z+agv_mount_t'
    fronts=[];rails=[];rears=[];lips=[]
    for i in range(3):
        x=f'-25 mm-({i})*electronics_pitch'
        fronts.append((f'({x})-5 mm','10 mm','7 mm','3 mm'))
        rails.append((f'({x})-5 mm','v05_rack_y-28 mm','7 mm','13 mm-v05_rack_y+28 mm'))
        rears.append((f'({x})-5 mm','v05_rack_y-28 mm','7 mm','3.6 mm'))
        for sy in [-1,1]:
            yy='v05_rack_y-25 mm' if sy<0 else 'v05_rack_y+18 mm'
            dep='7 mm' if sy<0 else '13 mm-v05_rack_y-18 mm'
            lips.append((f'({x})-4 mm',yy,'6 mm',dep))
    boxes(c,'Postes_frontales_XL',floor,f'electronics_z+buck_l+2 mm-({floor})',fronts)
    boxes(c,'Puentes_sobre_Mega','v05_rack_rail_z','4 mm',rails)
    boxes(c,'Postes_posteriores_XL','v05_rack_rail_z','electronics_z+buck_l+2 mm-v05_rack_rail_z',rears)
    boxes(c,'Labios_inferiores_XL','electronics_z-3 mm','3 mm',lips)
    boxes(c,'Labios_superiores_XL','electronics_z+buck_l','2 mm',lips)
    box(c,'Travesano_frontal_XL','-94 mm','10 mm',floor,'71 mm','3 mm','5 mm')
    box(c,'Travesano_posterior_XL','-94 mm','v05_rack_y-28 mm','v05_rack_rail_z','71 mm','3.6 mm','4 mm')
    unite(c,'V05_Rack_continuo_con_puentes')

def stage_v05_logic_frame():
    # At floor 6 mm, the turning platform is at Z=96 mm and clears the
    # 93 mm tops of the XL rack by 3 mm. The mounting plate stays at 6 mm.
    param('v05_motor_bottom','electronics_z+buck_l+5 mm-mg_h-32 mm',comment='DERIVADO v05: plato J1 a 3 mm sobre postes XL; cuerpo de motor segun mg_h.')
    c=gc('V05_Montura_baja_PETG');floor='agv_mount_z+agv_mount_t'
    bottoms=[]
    for name,x,L,W in [('PCA9685','45 mm','70 mm','25 mm'),('Nano','64 mm','45 mm','18 mm')]:
        posts=[];tops=[]
        for sy in [-1,1]:
            yy=f'({W})/2+0.4 mm' if sy>0 else f'-({W})/2-4 mm'
            posts.append((f'({x})-2 mm',yy,'6 mm','3.6 mm'))
            ly=f'({W})/2-4 mm' if sy>0 else f'-({W})/2-2 mm'
            # Extend into the posts beyond the board clearance: 1.6 mm overlap.
            bottoms.append((f'({x})-1 mm',ly,'4 mm','6 mm'))
            tops.append((f'({x})-1 mm',ly,'4 mm','6 mm'))
        boxes(c,'Postes_'+name,floor,f'v05_logic_z+({L})+2 mm-({floor})',posts)
        boxes(c,'Clips_'+name,f'v05_logic_z+({L})','1.5 mm',tops)
    boxes(c,'Asientos_placas_logicas','v05_logic_z-0.5 mm','0.5 mm',bottoms)
    for nm,x,y,w,h in [('Izquierda','37 mm','-31 mm','2 mm','13 mm'),('Derecha','59 mm','-31 mm','2 mm','13 mm'),('Fondo','37 mm','-19 mm','24 mm','2 mm')]:box(c,'Cajetin_bornera_'+nm,x,y,floor,w,h,'9 mm')
    for nm,x in [('I','39 mm'),('D','58 mm')]:box(c,'Retencion_bornera_'+nm,x,'-29 mm',f'{floor}+8 mm','1 mm','9 mm','1 mm')
    unite(c,'V05_Montura_baja_una_impresion')
    for b in c.bRepBodies:b.name='Montura baja, apoyo J1 y racks con puentes - una impresion'
    for o in R.occurrences:o.isLightBulbOn=o.component.name!='82_Envolvente_pan_REFERENCIA'
    D.activateRootComponent()

def stage_v05_access():
    c=gc('V05_Montura_baja_PETG')
    # The back face of XL1 is at X=-25. Keep 1 mm to the J1 columns/bridge;
    # bottom and top retaining lips remain the intended board contacts.
    box(c,'Holgura_respaldo_XL1','-25 mm','v05_rack_y-buck_w/2','electronics_z','1 mm','buck_w','buck_l',cut=True)
    for label in ['E','F']:
        ring(c,'Acceso_montaje_'+label,'Z','agv_mount_z+agv_mount_t','agv_'+label+'_x','agv_'+label+'_y','9 mm',None,'electronics_z+buck_l+3 mm-agv_mount_z-agv_mount_t',cut=True)
    unite(c,'V05_Montura_con_accesos_a_fijaciones')
    D.activateRootComponent()

def stage_v05_save():
    app=adsk.core.Application.get();doc=app.activeDocument
    comments={
        'agv_mount_z':'PROPUESTO v05: montura baja a 6 mm, Mega de referencia desde 4.5 mm.',
        'agv_pan_lift':'LEGADO v04, valor 0 mm. El bastidor v05 no usa un alza general.',
        'agv_mount_z_final':'DERIVADO: altura de separadores y fijaciones; coincide con montura baja sin alza.',
        'base_h':'DERIVADO v05: altura de interfaz J1 segun motor y espacio sobre rack.',
        'shoulder_z':'DERIVADO v05: conserva torre local de 113 mm sobre interfaz J1.',
        'v05_axis_y':'PROPUESTO v05: retraso de eje J1, 14 mm respecto de v04.',
        'v05_motor_bottom':'DERIVADO v05: plato J1 3 mm sobre postes XL, compensando altura mg_h.',
        'v05_motor_face':'DERIVADO v05: fondo de motor mas altura nominal mg_h.',
        'v05_rack_y':'PROPUESTO v05: traslado longitudinal de los tres XL4016.',
        'v05_rack_rail_z':'DERIVADO v05: puente 10 mm sobre cara superior de montura, libre de Mega.',
        'electronics_z':'DERIVADO v05: PCB XL sobre puente de 4 mm.',
        'v05_logic_z':'DERIVADO v05: placas PCA y Nano 3 mm sobre montura.'}
    for n,comment in comments.items():D.userParameters.itemByName(n).comment=comment
    if doc.isModified:doc.save('v05: montura a 6 mm, J1 retrasado, recorte pan y racks sostenidos; reemplaza elevacion v04.')
    D.exportManager.execute(D.exportManager.createFusionArchiveExportOptions(os.path.join(OUT_DIR,V05+'.f3d')))
    import csv
    with open(os.path.join(OUT_DIR,'parametros_prototipo_v05.csv'),'w',encoding='utf-8-sig',newline='') as f:
        w=csv.writer(f);w.writerow(['parametro','expresion','unidad','procedencia'])
        for p in D.userParameters:w.writerow([p.name,p.expression,p.unit,p.comment])
    data={'document':doc.name,'file_id':doc.dataFile.id,'parameters':D.userParameters.count,'components':D.allComponents.count,'root_groups':R.occurrences.count,'timeline_items':D.timeline.count,'joints':R.joints.count,'motion_links':R.motionLinks.count,'mount_floor_mm':val('agv_mount_z')*10,'mount_top_mm':val('agv_mount_z+agv_mount_t')*10,'J1_axis_y_mm':val('v05_axis_y')*10,'J1_interface_z_mm':val('base_top')*10,'shoulder_z_mm':val('shoulder_z')*10,'general_lift_mm':val('agv_pan_lift')*10,'scope':'Prototipo CAD; no ensayo fisico ni estabilidad acreditada.'}
    json.dump(data,open(os.path.join(REPORT,'modelo_v05.json'),'w',encoding='utf-8'),ensure_ascii=False,indent=2)

def run(context):
    if adsk.core.Application.get().activeDocument.name!=V05:raise RuntimeError('Activar v05')
    setup();phase=globals()['PHASE'];os.makedirs(REPORT,exist_ok=True)
    def log(state):
        with open(os.path.join(REPORT,'authoring_journal.jsonl'),'a',encoding='utf-8') as f:f.write(json.dumps({'phase':phase,'state':state})+'\n')
    log('started');globals()['stage_'+phase]();adsk.doEvents();adsk.core.Application.get().activeViewport.refresh();log('applied')
