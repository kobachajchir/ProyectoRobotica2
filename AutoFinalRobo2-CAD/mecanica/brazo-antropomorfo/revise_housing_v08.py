"""Housing GX3270BLS y correcciones mecanicas. Ejecutar por fases en Fusion.

Conserva v07. Referencias de montaje editables, no planos liberados.
"""
exec(compile(open(r'D:\ProyectoRobotica2\AutoFinalRobo2-CAD\mecanica\brazo-antropomorfo\revise_actuator_v07.py',encoding='utf-8').read(),'revise_actuator_v07.py','exec'))
V08='AFR2_Brazo_PVC32_AGV_v08_Housing'
REPORT=os.path.join(OUT_DIR,'revision_v08')

def stage_v08_parameters():
    for n,e,co in [
      ('v08_housing_wall','3 mm','PROPUESTO: paredes de carcasa abierta, rigidez no validada.'),
      ('v08_rear_clear','0.8 mm','PROPUESTO: holgura posterior del servo.'),
      ('v08_cable_window','16 mm','PROPUESTO: ventana lateral; cable no acotado por fabricante.'),
      ('v08_edge_radius','0.8 mm','PROPUESTO: redondeo de aristas externas accesibles.'),
      ('v08_horn_clear','3.2 mm','REFERENCIA: envolvente libre de tornillo M3 central, rosca real sin identificar.'),
    ]:param(n,e,'mm',co)

def stage_v08_proximal():
    c=gc('A05_Encastre_proximal_brazo_PETG')
    z='socket_start+socket_length/2';x='pvc_od/2+6 mm'
    # New ribs must preserve screw shank and washer/nut/tool access.
    ring(c,'V08_Acceso_cabeza_abrazadera','Y','-13 mm',x,z,'8 mm',None,'6.5 mm',cut=True)
    ring(c,'V08_Acceso_tuerca_abrazadera','Y','6.5 mm',x,z,'8 mm',None,'7 mm',cut=True)

def stage_v08_cleanup():
    # The two tool-access bores already clear the complete added rib thickness;
    # the original central clamp bore remains. Remove a redundant failed recut.
    c=gc('A05_Encastre_proximal_brazo_PETG')
    f=c.features.extrudeFeatures.itemByName('V08_Paso_perno_tras_nervios')
    if f:f.deleteMe()

def stage_v08_base():
    c=gc('V05_Montura_baja_PETG')
    # Keep continuous original columns at x=-25..-19; trim only new outboard ribs.
    box(c,'V08_Reserva_XL4016_1','-35 mm','-43 mm','25 mm','10.2 mm','50 mm','28 mm',cut=True)

def stage_v08_retention():
    c=gc('A06_Encastre_distal_brazo_PETG')
    # Matching retainer is 38 OD at Y32.5..36.5; do not carve the cap itself.
    ring(c,'V08_Reserva_tapa608','Y','fork_y+fork_t/2','0 mm','link1',
         '38 mm+v07_clear',None,'4.6 mm',cut=True)
    # Restore insert seats to the retained fasteners' positions, not +/-0.5 mm.
    for sy in [-1,1]:
        yy=f'({sy})*fork_y';a=f'({yy})-1 mm' if sy>0 else f'({yy})-3 mm'
        for sx in [-1,1]:
            ring(c,'V08_Asiento_inserto608_'+str(sy)+str(sx),'Y',a,f'({sx})*14 mm','link1','4.5 mm',None,'4 mm',cut=True)
    # The tray crossbeam overlapped the bottom 0.5 mm of each motor tab.
    box(c,'V08_Paso_pestanas_motor','-v07_axis_offset-9 mm','v07_tab_rear_y-v07_ear_t-v07_clear',
        'link1-v07_motor_w/2-v07_clear','v07_ear_span+2 mm','v07_ear_t+v07_clear','v07_motor_w+2*v07_clear',cut=True)
    m=gc('V07_M03_GX3270BLS_Codo')
    # Thread is represented by its free nominal envelope; not a tapped profile.
    f=m.features.extrudeFeatures.itemByName('V07_Rosca_central_REF')
    f.extentOne.distance.expression='4 mm'
    sk=m.sketches.itemByName('Croquis_V07_Rosca_central_REF')
    for dd in sk.sketchDimensions:
        if isinstance(dd,adsk.fusion.SketchDiameterDimension):dd.parameter.expression='v08_horn_clear'
    m.attributes.add('AFR2','tornillo_horn','M3 representado con juego 3.2x4; rosca real y largo util a confirmar. No dato del fabricante.')

def stage_v08_housing():
    c=gc('A06_Encastre_distal_brazo_PETG')
    # Correct the initial slot's right end without repeating the retention phase.
    for dd in c.sketches.itemByName('Croquis_V08_Paso_pestanas_motor').sketchDimensions:
        if dd.parameter.expression in ['v07_ear_span + 1 mm','v07_ear_span+1 mm']:
            dd.parameter.expression='v07_ear_span+2 mm'
    left='-v07_axis_offset-v07_clear-v08_housing_wall'
    right='v07_motor_l-v07_axis_offset+v07_clear'
    rear='v07_case_back_y+v08_rear_clear'
    bottom='link1-17 mm'
    # Open-front, open-top protective cradle: servo removes towards -Y after horn.
    box(c,'V08_Extension_bandeja',left,'v07_tab_rear_y',bottom,
        'v07_motor_l+2*v07_clear+2*v08_housing_wall',
        '('+rear+')+v08_housing_wall-v07_tab_rear_y','v07_rib_t')
    for side,x in [(-1,left),(1,right)]:
        box(c,'V08_Pared_lateral_'+str(side),x,'v07_tab_rear_y',bottom,
            'v08_housing_wall','('+rear+')+v08_housing_wall-v07_tab_rear_y','v07_motor_w+8 mm')
    box(c,'V08_Pared_posterior',left,rear,bottom,
        'v07_motor_l+2*v07_clear+2*v08_housing_wall','v08_housing_wall','v07_motor_w+8 mm')
    join_all(c,'V08_Housing_integrado_una_pieza')
    # Cable reserve on the end opposite the output axis; proposed, no cable body.
    box(c,'V08_Ventana_cable',right+'-0.2 mm',
        'v07_case_back_y-v08_cable_window+1 mm','link1-6 mm',
        'v08_housing_wall+0.4 mm','v08_cable_window','12 mm',cut=True)
    # Ventilation openings keep 5 mm continuous upper/lower rails.
    box(c,'V08_Ventana_lateral',left+'-0.2 mm','v07_tab_rear_y+9 mm','link1-8 mm',
        'v08_housing_wall+0.4 mm','17 mm','14 mm',cut=True)
    box(c,'V08_Ventana_posterior','-v07_axis_offset+7 mm',rear+'-0.2 mm','link1-8 mm',
        'v07_motor_l-14 mm','v08_housing_wall+0.4 mm','14 mm',cut=True)
    for b in c.bRepBodies:b.name='V08 manguito27 horquilla608 y housing GX3270BLS ventilado - una impresion'
    c.attributes.add('AFR2','montaje_v08','4M3 en pestanas, paredes3mm abiertas, ventanas de cable/ventilacion, tapa608 accesible. Sin ensayo de resistencia.')

def stage_v08_rounds():
    c=gc('A06_Encastre_distal_brazo_PETG');next(o for o in R.allOccurrences if o.component.name==c.name).activate()
    # Round long vertical edges of the external rear face and its window.
    b=c.bRepBodies.item(0);es=adsk.core.ObjectCollection.create()
    rear=val('v07_case_back_y+v08_rear_clear+v08_housing_wall')
    for e in b.edges:
        a=e.startVertex.geometry if e.startVertex else None;z=e.endVertex.geometry if e.endVertex else None
        if not a or not z:continue
        if abs(a.y-rear)<1e-6 and abs(z.y-rear)<1e-6 and abs(a.x-z.x)<1e-6 and abs(a.z-z.z)>1:
            es.add(e)
    if es.count<2:raise RuntimeError('No se encontraron aristas traseras esperadas')
    ip=c.features.filletFeatures.createInput();ip.edgeSetInputs.addConstantRadiusEdgeSet(es,V('v08_edge_radius'),False)
    f=c.features.filletFeatures.add(ip);f.name='V08_Redondeo_exterior_housing'
    D.activateRootComponent()
    print(json.dumps({'rounded_edges':es.count}))

def stage_v08_final_holes():
    c=gc('A06_Encastre_distal_brazo_PETG')
    # Separate parametric sketches avoid inferred cross-hole alignment constraints.
    for sx in [-1,1]:
        for sz in [-1,1]:
            xx=f'v07_motor_l/2-v07_axis_offset+({sx})*v07_mount_pitch/2'
            zz=f'link1+({sz})*v07_mount_cross/2';tag=str(sx)+str(sz)
            ring(c,'V08_Paso_M3_'+tag,'Y','v07_tab_rear_y-0.1 mm',xx,zz,'3.4 mm',None,'8.4 mm',cut=True)
            ring(c,'V08_Asiento_inserto_'+tag,'Y','v07_tab_rear_y+3 mm',xx,zz,'4.5 mm',None,'4 mm',cut=True)
    D.activateRootComponent()

def stage_v08_save():
    import csv
    app=adsk.core.Application.get();doc=app.activeDocument;D.activateRootComponent()
    for c in D.allComponents:
        for prop in ['isSketchFolderLightBulbOn','isConstructionFolderLightBulbOn','isOriginFolderLightBulbOn','isJointsFolderLightBulbOn']:
            if hasattr(c,prop):setattr(c,prop,False)
    for j in R.joints:j.isLightBulbOn=False
    cam=app.activeViewport.camera
    cam.target=P(0,15,13);cam.eye=P(-55,75,55);cam.upVector=adsk.core.Vector3D.create(0,0,1)
    cam.isFitView=True;app.activeViewport.camera=cam
    app.activeViewport.saveAsImageFile(os.path.join(REPORT,'ensamble_v08.png'),1400,1000)
    cam=app.activeViewport.camera;cam.isFitView=False
    cam.target=P(-6.5,11.6,20.5);cam.eye=P(-30,30,36);cam.upVector=adsk.core.Vector3D.create(0,0,1);cam.viewExtents=16
    app.activeViewport.camera=cam
    app.activeViewport.saveAsImageFile(os.path.join(REPORT,'housing_con_motor.png'),1400,1000)
    # Isolated manufactured component, no exploded assembly or displaced hardware.
    states=[(o,o.isLightBulbOn) for o in R.occurrences]
    for o,_ in states:o.isLightBulbOn=o.component.name=='02_Brazo_PVC'
    group=comp('02_Brazo_PVC');substates=[(o,o.isLightBulbOn) for o in group.occurrences]
    for o,_ in substates:o.isLightBulbOn=o.component.name=='A06_Encastre_distal_brazo_PETG'
    app.activeViewport.saveAsImageFile(os.path.join(REPORT,'housing_sin_motor.png'),1400,1000)
    for o,state in substates:o.isLightBulbOn=state
    for o,state in states:o.isLightBulbOn=state
    cam.target=P(0,15,13);cam.eye=P(-55,75,55);cam.upVector=adsk.core.Vector3D.create(0,0,1)
    cam.isFitView=True;app.activeViewport.camera=cam
    doc.save('v08: housing ventilado, reservas de montaje y revision CAD autorizada.')
    D.exportManager.execute(D.exportManager.createFusionArchiveExportOptions(os.path.join(OUT_DIR,V08+'.f3d')))
    for cn,file in [('A06_Encastre_distal_brazo_PETG','Housing_GX3270BLS_Parametrico.f3d'),('V07_M03_GX3270BLS_Codo','GX3270BLS_Referencia_v08.f3d')]:
        D.exportManager.execute(D.exportManager.createFusionArchiveExportOptions(os.path.join(REPORT,file),gc(cn)))
    with open(os.path.join(OUT_DIR,'parametros_prototipo_v08.csv'),'w',encoding='utf-8-sig',newline='') as f:
        w=csv.writer(f);w.writerow(['parametro','expresion','unidad','procedencia'])
        for p in D.userParameters:w.writerow([p.name,p.expression,p.unit,p.comment])
    json.dump({'document':doc.name,'id':doc.dataFile.id,'parameters':D.userParameters.count,
      'components':D.allComponents.count,'joints':R.joints.count,'motion_links':R.motionLinks.count,
      'saved':not doc.isModified,'scope':'CAD editable y revision geometrica muestreada. No prueba fisica ni FEA.'},
      open(os.path.join(REPORT,'modelo_v08.json'),'w',encoding='utf-8'),ensure_ascii=False,indent=2)

def run(_context: str):
    app=adsk.core.Application.get()
    if app.activeDocument.name!=V08:raise RuntimeError('Activar copia '+V08)
    setup();os.makedirs(REPORT,exist_ok=True);phase=globals()['PHASE']
    def log(state):
        with open(os.path.join(REPORT,'authoring_journal.jsonl'),'a',encoding='utf-8') as f:f.write(json.dumps({'phase':phase,'state':state})+'\n')
    log('started');globals()['stage_'+phase]();adsk.doEvents();app.activeViewport.refresh();log('applied')
    print(json.dumps({'phase':phase,'applied':True}))
