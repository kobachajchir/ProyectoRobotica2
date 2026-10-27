"""Fusion: revision v07 GX3270BLS, fases explicitas sobre copia de estudio v06.

Solo modelado y extraccion para calculo; no barridos ni deteccion de interferencias.
Los valores inferidos de interfaz siguen identificados, no cotas de fabricacion.
"""
exec(compile(open(r'D:\ProyectoRobotica2\AutoFinalRobo2-CAD\mecanica\brazo-antropomorfo\revise_lower_v06.py',encoding='utf-8').read(),'revise_lower_v06.py','exec'))
V07='AFR2_Brazo_PVC32_AGV_v07_GX3270BLS'
REPORT=os.path.join(OUT_DIR,'revision_v07')

PARAMS_V07=[
 ('v07_motor_l','40.5 mm','mm','PLANO Yipin GX3270BLS, galeria Starware 2982-8705.'),
 ('v07_motor_w','21 mm','mm','PLANO Yipin GX3270BLS.'),
 ('v07_case_h','41.7 mm','mm','PLANO: cuerpo principal, no altura con resaltes/eje.'),
 ('v07_boss_h','3.45 mm','mm','DERIVADO: 45.15-41.70; reserva conservadora frente a foto de altura contradictoria.'),
 ('v07_spline_h','5 mm','mm','PLANO: longitud del estriado.'),
 ('v07_spline_d','5.94 mm','mm','PLANO: Z25; dientes no modelados como perfil de fabricacion.'),
 ('v07_ear_span','55.1 mm','mm','PLANO: ancho longitudinal con pestanas.'),
 ('v07_mount_pitch','48 mm','mm','PLANO: centros longitudinales de fijacion.'),
 ('v07_mount_cross','10 mm','mm','PLANO: centros transversales de fijacion.'),
 ('v07_mount_d','4.5 mm','mm','PLANO: pasos nominales del servo, cuatro.'),
 ('v07_ear_t','2.5 mm','mm','PLANO: espesor pestana.'),
 ('v07_ear_back','30.65 mm','mm','PLANO: cara posterior de pestana desde fondo de carcasa.'),
 ('v07_axis_offset','10.5 mm','mm','INFERIDO: eje a extremo corto del cuerpo; no acotado en plano. Ajustar a unidad comprada.'),
 ('v07_spline_tip_y','42 mm','mm','DISENO: mantener horn a Y40..42 y eje articular existente.'),
 ('v07_spline_base_y','v07_spline_tip_y+v07_spline_h','mm','DERIVADO.'),
 ('v07_case_front_y','v07_spline_base_y+v07_boss_h','mm','DERIVADO.'),
 ('v07_case_back_y','v07_case_front_y+v07_case_h','mm','DERIVADO: reserva total 50.15 mm incluido eje segun lectura conservadora del plano.'),
 ('v07_tab_rear_y','v07_case_back_y-v07_ear_back','mm','DERIVADO del plano; confirmar variante antes de fabricar.'),
 ('v07_clear','0.6 mm','mm','PROPUESTO: holgura de montaje.'),
 ('v07_mount_wall','6 mm','mm','PROPUESTO: apoyo de pestanas.'),
 ('v07_rib_t','4 mm','mm','PROPUESTO: nervios locales; sin FEA.'),
 ('v07_socket_extra','10 mm','mm','PROPUESTO: pasar solape tubular de 17 a 27 mm.'),
 ('v07_J1_outer','50 mm','mm','PROPUESTO: collar de apoyo, sin elevar base ni cambiar centros de rodamientos.'),
 ('v07_motor_mass','90 g','g','OFERTA Starware 2982, masa sin accesorios; pendiente de pesaje.'),
 ('v07_control_V','7.4','', 'PROPUESTO: alimentacion regulada del nuevo servo, no conectar 2S sin regular.'),
 ('v07_brake_time','0.1 s','s','HIPOTESIS de calculo, no tiempo de parada acreditado.'),
]

def discard_geometry(c):
    for b in list(c.bRepBodies):
        c.features.removeFeatures.add(b)

def triangle_y(c,name,y,pts,depth,key='red'):
    """Triangulo en XZ y extrusion +Y, con vertices por expresiones."""
    if D.activeComponent.name!=c.name:
        next(o for o in R.allOccurrences if o.component.name==c.name).activate()
    s=c.sketches.add(datum(c,'Y',y,'Plano_'+name));s.name='Croquis_'+name
    s.isComputeDeferred=True
    lines=[]
    for i,(x,z) in enumerate(pts):
        x2,z2=pts[(i+1)%len(pts)]
        lines.append(s.sketchCurves.sketchLines.addByTwoPoints(P(val(x),-val(z),0),P(val(x2),-val(z2),0)))
    for co in reversed(list(s.geometricConstraints)):
        if co.isDeletable:co.deleteMe()
    for i,line in enumerate(lines):
        nxt=lines[(i+1)%len(lines)].startSketchPoint
        if line.endSketchPoint.entityToken!=nxt.entityToken:
            s.geometricConstraints.addCoincident(line.endSketchPoint,nxt)
    s.isComputeDeferred=False
    for i,line in enumerate(lines):
        for expr,ori in [(pts[i][0],ORI.HorizontalDimensionOrientation),('-('+pts[i][1]+')',ORI.VerticalDimensionOrientation)]:
            try:
                dd=s.sketchDimensions.addDistanceDimension(s.originPoint,line.startSketchPoint,ori,P(0.5,0.5,0),True)
                dd.parameter.expression=expr if val(expr)>0 else '-('+expr+')'
            except RuntimeError as err:
                if 'OVER_CONSTRAINT' not in str(err):raise
                # Shared vertices can already be located by the inferred H/V
                # edge. A reference dimension records the dependent coordinate.
                s.sketchDimensions.addDistanceDimension(s.originPoint,line.startSketchPoint,ori,P(0.5,0.5,0),False)
    f=extrude(c,s,depth,OP.NewBodyFeatureOperation,name)
    for b in f.bodies:b.appearance=AP[key]
    return f

def stage_v07_parameters():
    for n,e,u,co in PARAMS_V07:param(n,e,u,co)
    param('part_mass','200 g','g','USUARIO: requisito de calculo, no carga fisica acreditada.')
    D.attributes.add('AFR2','revision_v07','GX3270BLS J3; refuerzos propuestos. Sin barridos ni FEA.')

def motor_geometry(c,z):
    lo='-v07_axis_offset';mid='v07_motor_l/2-v07_axis_offset'
    box(c,'V07_Carcasa_principal',lo,'v07_case_front_y',f'({z})-v07_motor_w/2',
        'v07_motor_l','v07_case_h','v07_motor_w','servo')
    ring(c,'V07_Resalte_salida_REF','Y','v07_spline_base_y','0 mm',z,
         '20 mm',None,'v07_boss_h','servo')
    ring(c,'V07_Estriado25T_envolvente','Y','v07_spline_tip_y','0 mm',z,
         'v07_spline_d',None,'v07_spline_h','steel')
    box(c,'V07_Pestanas',f'({mid})-v07_ear_span/2','v07_tab_rear_y-v07_ear_t',
        f'({z})-v07_motor_w/2','v07_ear_span','v07_ear_t','v07_motor_w','servo')
    join_all(c,'V07_Servo_envolvente_unica')
    for sx in [-1,1]:
        x=f'({mid})+({sx})*v07_mount_pitch/2'
        for sz in [-1,1]:
            zz=f'({z})+({sz})*v07_mount_cross/2'
            ring(c,'V07_Paso_pestana_'+str(sx)+str(sz),'Y','v07_tab_rear_y-v07_ear_t-0.1 mm',
                 x,zz,'v07_mount_d',None,'v07_ear_t+0.2 mm',cut=True)
    ring(c,'V07_Rosca_central_REF','Y','v07_spline_tip_y','0 mm',z,'2.5 mm',None,'3 mm',cut=True)
    c.attributes.add('AFR2','modelo_comercial','Yipin GX3270BLS 270gr Starware SKU2982; Z25, offset inferido; carcasa de referencia, no internos.')

def stage_v07_motor():
    c=gc('V02_M03_DS3218_Codo')
    discard_geometry(c);c.name='V07_M03_GX3270BLS_Codo'
    motor_geometry(c,'link1')
    # Existing moving horn stays rigid with the distal elbow flange, not the case.
    h=gc('V03_Horn_movil_V02_M03_DS3218_Codo')
    h.attributes.add('AFR2','interfaz_pendiente','Horn metalico 25T; patron brida conservado como adaptador a ajustar al horn comprado.')
    for b in h.bRepBodies:b.appearance=AP['steel']

def stage_v07_horn():
    h=gc('V03_Horn_movil_V02_M03_DS3218_Codo')
    ring(h,'V07_Cubo25T_alargado','Y','44 mm','0 mm','0 mm','8 mm','v07_spline_d','2 mm','steel')
    join_all(h,'V07_Horn_metalico_con_engagement4mm')
    h.attributes.add('AFR2','engagement','Cubo25T42..46mm sobre estriado42..47mm. Envolventes nominales; no dientes de fabricacion.')

def stage_v07_elbow():
    c=gc('A06_Encastre_distal_brazo_PETG');discard_geometry(c)
    z='link1';start='link1-socket_start-socket_length-v07_socket_extra'
    ring(c,'V07_Manguito_solape27','Z',start,'0 mm','0 mm','socket_od','pvc_od+socket_clear',
         'link1-tube_inset+2 mm-('+start+')')
    ring(c,'V07_Tope_tubo','Z','link1-tube_inset','0 mm','0 mm','socket_od','pvc_id+socket_clear','2 mm')
    box(c,'V07_Puente_horquilla','-18 mm','-fork_y-fork_t/2','link1-35 mm','36 mm','2*fork_y+fork_t','8 mm')
    for sign in [-1,1]:
        yy=f'({sign})*fork_y-fork_t/2'
        box(c,'V07_Brazo_horquilla_'+str(sign),'-18 mm',yy,'link1-35 mm','36 mm','fork_t','35 mm')
        ring(c,'V07_Oreja608_'+str(sign),'Y',yy,'0 mm',z,'42 mm',None,'fork_t')
    # Integrated tray is below the servo and connects both screw banks to the fork.
    box(c,'V07_Bandeja_motor','-v07_axis_offset-10 mm','fork_y-1 mm','link1-17 mm',
        'v07_motor_l+20 mm','v07_tab_rear_y+8 mm-fork_y+1 mm','v07_rib_t')
    for sx in [-1,1]:
        xx='-v07_axis_offset-10 mm' if sx<0 else 'v07_motor_l-v07_axis_offset+v07_clear'
        ww='10 mm-v07_clear'
        box(c,'V07_Banco_pestana_'+str(sx),xx,'v07_tab_rear_y','link1-17 mm',
            ww,'v07_mount_wall+2 mm','34 mm')
        # Rib runs from under-tray to mounting bank, outside motor's side faces.
        ribx=f'({xx})+({ww})/2-2 mm'
        box(c,'V07_Larguero_banco_'+str(sx),ribx,'fork_y-1 mm','link1-17 mm',
            '4 mm','v07_tab_rear_y-fork_y+3 mm','7 mm')
    # Sleeve clamp uses the established pin axis and keeps access to its nut.
    for sy in [-1,1]:
        box(c,'V07_Oreja_abrazadera_'+str(sy),'pvc_od/2+1 mm',f'({sy})*4 mm-2.5 mm',
            'link1-socket_start-socket_length/2-5 mm','10 mm','5 mm','10 mm')
    join_all(c,'V07_Encastre_horquilla_cuna_una_pieza')
    ring(c,'V07_Paso_tubo_horquilla','Z',start+'-1 mm','0 mm','0 mm','pvc_od+socket_clear',None,
         'link1-tube_inset-('+start+')+1 mm',cut=True)
    box(c,'V07_Ranura_abrazadera','pvc_od/2-1 mm','-0.75 mm',start+'-1 mm','13 mm','1.5 mm',
        'link1-tube_inset+3 mm-('+start+')',cut=True)
    ring(c,'V07_Paso_perno','Y','-8 mm','pvc_od/2+6 mm','link1-socket_start-socket_length/2','3.4 mm',None,'16 mm',cut=True)
    for sy in [-1,1]:
        yy=f'({sy})*fork_y'
        # Through bearing seat, backed by an inner shoulder clear of the inner race.
        seat_start=f'({yy})-b608_w/2' if sy>0 else f'({yy})-fork_t/2-0.1 mm'
        ring(c,'V07_Asiento608_'+str(sy),'Y',seat_start,'0 mm',z,'b608_od+fit_clear',None,'b608_w+fork_t/2-b608_w/2+0.1 mm',cut=True)
        ring(c,'V07_Paso_eje_'+str(sy),'Y',f'({yy})-fork_t/2-0.1 mm','0 mm',z,'11.6 mm',None,'fork_t+0.2 mm',cut=True)
        for tx in [-1,1]:
            xx=f'({tx})*14 mm'
            ring(c,'V07_Retencion_tapa_'+str(sy)+str(tx),'Y',f'({yy})-fork_t/2-0.2 mm',xx,z,'3.4 mm',None,'fork_t+0.4 mm',cut=True)
            # Same insert positions as retained bearing caps and hardware.
            a=f'({yy})+fork_t/2-5 mm' if sy>0 else f'({yy})-fork_t/2+1 mm'
            ring(c,'V07_Inserto_tapa_'+str(sy)+str(tx),'Y',a,xx,z,'4.5 mm',None,'4 mm',cut=True)
    g=comp('02_Brazo_PVC')
    for o in list(g.occurrences):
        if o.component.name.startswith(('V03_Inserto_fijacion_J3','V03_Tornillo_fijacion_J3')):o.deleteMe()
    mid='v07_motor_l/2-v07_axis_offset'
    for sx in [-1,1]:
        xx=f'({mid})+({sx})*v07_mount_pitch/2'
        for sz in [-1,1]:
            zz=f'link1+({sz})*v07_mount_cross/2';tag=str(sx)+str(sz)
            ring(c,'V07_Taladro_M3_'+tag,'Y','v07_tab_rear_y-0.1 mm',xx,zz,'3.4 mm',None,'8.2 mm',cut=True)
            ring(c,'V07_Caja_inserto_M3_'+tag,'Y','v07_tab_rear_y+3 mm',xx,zz,'4.5 mm',None,'4 mm',cut=True)
            ic=sub(g,'V07_Inserto_M3_codo_'+tag)
            ring(ic,'Inserto_M3_REF','Y','v07_tab_rear_y+3 mm',xx,zz,'4.5 mm','3 mm','4 mm','brass')
            sc=sub(g,'V07_Tornillo_M3x10_codo_'+tag)
            ring(sc,'Vastago','Y','v07_tab_rear_y-v07_ear_t-0.8 mm',xx,zz,'3 mm',None,'10 mm','steel')
            ring(sc,'Cabeza','Y','v07_tab_rear_y-v07_ear_t-3.8 mm',xx,zz,'5.5 mm',None,'3 mm','steel')
            join_all(sc,'Tornillo_M3x10_REF')
            wc=sub(g,'V07_Arandela_codo_'+tag)
            ring(wc,'Arandela','Y','v07_tab_rear_y-v07_ear_t-0.8 mm',xx,zz,'7 mm','3.2 mm','0.8 mm','steel')
    for b in c.bRepBodies:b.name='V07 manguito 27mm, horquilla608 y cuna GX3270BLS - una impresion'
    c.attributes.add('AFR2','alcance','Redisenado para GX3270BLS; referencias de fijacion segun plano, offset de eje inferido. Sin FEA/barridos.')

def stage_v07_proximal():
    # Extend the proximal guide sleeve, with no metal added to the complete tube.
    c=gc('A05_Encastre_proximal_brazo_PETG')
    ring(c,'V07_Extension_solape27','Z','socket_start+socket_length','0 mm','0 mm','socket_od','pvc_od+socket_clear','v07_socket_extra')
    join_all(c,'V07_Nucleo_manguito_proximal_continuo')
    # Four short integral ribs, leaving the clamping slit and its screw accessible.
    for sx in [-1,1]:
        for sy in [-1,1]:
            triangle_y(c,'V07_Nervio_proximal_'+str(sx)+str(sy),f'({sy})*9 mm-v07_rib_t/2',
                [(f'({sx})*16 mm','10 mm'),(f'({sx})*28 mm','10 mm'),
                 (f'({sx})*16 mm','socket_start+socket_length')],'v07_rib_t')
    join_all(c,'V07_Manguito_con_nervios')

def stage_v07_tower():
    c=gc('A01_Plataforma_giratoria_PETG')
    for sx in [-1,1]:
        for sy in [-1,1]:
            triangle_y(c,'V07_Nervio_torre_'+str(sx)+str(sy),
                f'({sy})*fork_y-v07_rib_t/2',
                [(f'({sx})*21 mm','8 mm'),(f'({sx})*33 mm','8 mm'),(f'({sx})*21 mm','tower_h-30 mm')],'v07_rib_t')
    join_all(c,'V07_Torre_con_nervios_locales')

def stage_v07_base():
    c=gc('V05_Montura_baja_PETG')
    ring(c,'V07_Collar_cartucho_J1','Z','v05_motor_face+10 mm','0 mm','v05_axis_y',
         'v07_J1_outer','40 mm','15 mm')
    for sx in [-1,1]:
        for sy in [-1,1]:
            triangle_y(c,'V07_Nervio_columna_J1_'+str(sx)+str(sy),
                f'v05_axis_y+({sy})*14 mm-2 mm',
                [(f'({sx})*22 mm','agv_mount_z+agv_mount_t'),
                 (f'({sx})*33 mm','agv_mount_z+agv_mount_t'),
                 (f'({sx})*22 mm','v05_motor_face+2 mm')],'v07_rib_t')
    join_all(c,'V07_Montura_baja_con_nervios')
    # One-piece lower plate kept. Thickening, metal substitution and bearing
    # separation require a contact/clearance study; not asserted here.
    D.activateRootComponent()

def stage_v07_reinforce():
    stage_v07_proximal();stage_v07_tower();stage_v07_base()

def stage_v07_axial_stop():
    # Repair initial authoring dimensions in-place, no duplicate geometry.
    c=gc('A06_Encastre_distal_brazo_PETG')
    start='link1-socket_start-socket_length-v07_socket_extra'
    c.features.extrudeFeatures.itemByName('V07_Manguito_solape27').extentOne.distance.expression='link1-tube_inset+2 mm-('+start+')'
    c.features.extrudeFeatures.itemByName('V07_Paso_tubo_horquilla').extentOne.distance.expression='link1-tube_inset-('+start+')+1 mm'
    c.features.extrudeFeatures.itemByName('V07_Ranura_abrazadera').extentOne.distance.expression='link1-tube_inset+3 mm-('+start+')'
    D.activateRootComponent()

def stage_v07_save():
    import csv
    app=adsk.core.Application.get();doc=app.activeDocument
    if doc.isModified:doc.save('v07 GX3270BLS codo, housing4M3, solape27 y nervios locales. Sin FEA ni barridos.')
    D.exportManager.execute(D.exportManager.createFusionArchiveExportOptions(os.path.join(OUT_DIR,V07+'.f3d')))
    with open(os.path.join(OUT_DIR,'parametros_prototipo_v07.csv'),'w',encoding='utf-8-sig',newline='') as f:
        w=csv.writer(f);w.writerow(['parametro','expresion','unidad','procedencia'])
        for p in D.userParameters:w.writerow([p.name,p.expression,p.unit,p.comment])
    json.dump({'document':doc.name,'id':doc.dataFile.id,'parameters':D.userParameters.count,
        'components':D.allComponents.count,'joints':R.joints.count,'motion_links':R.motionLinks.count,
        'scope':'Modelado parametricamente editable. Sin barridos, FEA ni capacidad fisica acreditada.',
        'completed_phases':[json.loads(line)['phase'] for line in open(os.path.join(REPORT,'authoring_journal.jsonl'),encoding='utf-8') if json.loads(line)['state']=='applied'],
        'J1_bearing_separation_mm':8,'J1_mount_floor_mm':val('agv_mount_z')*10,
        'shoulder_z_mm':val('shoulder_z')*10},
        open(os.path.join(REPORT,'modelo_v07.json'),'w',encoding='utf-8'),ensure_ascii=False,indent=2)
    # This is a presentation capture, not a movement/interference validation.
    D.activateRootComponent()
    cam=app.activeViewport.camera
    # Model world is Z-up; document's inherited standard iso can be Y-up.
    cam.target=P(0,15,13);cam.eye=P(-55,75,55);cam.upVector=adsk.core.Vector3D.create(0,0,1)
    cam.isFitView=True;app.activeViewport.camera=cam

    app.activeViewport.saveAsImageFile(os.path.join(REPORT,'ensamble_v07.png'),1400,950)
    cam=app.activeViewport.camera
    cam.isFitView=False
    cam.target=P(-5,11.6,20.9);cam.eye=P(-30,30,35);cam.upVector=adsk.core.Vector3D.create(0,0,1);cam.viewExtents=15
    app.activeViewport.camera=cam
    app.activeViewport.saveAsImageFile(os.path.join(REPORT,'codo_v07.png'),1200,900)
    cam.target=P(0,15,13);cam.eye=P(-55,75,55);cam.upVector=adsk.core.Vector3D.create(0,0,1)
    cam.isFitView=True;app.activeViewport.camera=cam
    doc.save('v07: vista de presentacion Z vertical; geometria y calculos sin cambios.')
    D.exportManager.execute(D.exportManager.createFusionArchiveExportOptions(os.path.join(OUT_DIR,V07+'.f3d')))

def stage_v07_motor_archive():
    opt=D.exportManager.createFusionArchiveExportOptions(
        os.path.join(REPORT,'GX3270BLS_Referencia_Parametrica.f3d'),gc('V07_M03_GX3270BLS_Codo'))
    ok=D.exportManager.execute(opt)
    print(json.dumps({'motor_archive_exported':ok}))

def run(_context: str):
    app=adsk.core.Application.get()
    if app.activeDocument.name!=V07:raise RuntimeError('Activar copia '+V07)
    setup();os.makedirs(REPORT,exist_ok=True);phase=globals()['PHASE']
    def log(state):
        with open(os.path.join(REPORT,'authoring_journal.jsonl'),'a',encoding='utf-8') as f:f.write(json.dumps({'phase':phase,'state':state})+'\n')
    log('started');globals()['stage_'+phase]();adsk.doEvents();app.activeViewport.refresh();log('applied')
    print(json.dumps({'phase':phase,'document':app.activeDocument.name,'applied':True}))
