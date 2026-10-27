# Fusion: correccion pedida de anclaje izquierdo y barrido del ultrasonido.
# Ejecutar cada fase una sola vez sobre la copia v04. No altera el auto maestro.
exec(compile(open(r'D:\ProyectoRobotica2\AutoFinalRobo2-CAD\mecanica\brazo-antropomorfo\mount_agv_v03.py',encoding='utf-8').read(),'mount_agv_v03.py','exec'))
V04='AFR2_Brazo_PVC32_AGV_v04'
REPORT=os.path.join(OUT_DIR,'revision_v04')
LEFT=[('E','-68.298597443 mm','-12.157024165 mm'),('F','-31.142269400 mm','-12.199106379 mm')]

def progress(detail):
    with open(os.path.join(REPORT,'progress.jsonl'),'a',encoding='utf-8') as f:f.write(json.dumps({'detail':detail})+'\n')

def stage_v04_left():
    for label,x,y in LEFT:param('agv_'+label+'_x',x);param('agv_'+label+'_y',y)
    param('pan_axis_x','-5.150029370 mm');param('pan_axis_y','42.999972322 mm')
    param('pan_radius_bound','31.4 mm');param('pan_clearance','3 mm')
    param('pan_top_z','51 mm');param('pan_bottom_z','10 mm')
    param('agv_pan_lift','pan_top_z+pan_clearance-agv_mount_z')
    param('agv_mount_z_final','agv_mount_z+agv_pan_lift')
    g=comp('00_Base_control');frame=gc('V03_Plataforma_sandwich_superior')
    for label,x,y in LEFT:
        ring(frame,'Anclaje_izquierdo_'+label,'Z','agv_mount_z-1 mm','agv_'+label+'_x','agv_'+label+'_y','3.4 mm',None,'agv_mount_t+2 mm',cut=True)
        c=sub(g,'V04_Separador_M3_'+label)
        ring(c,'Separador','Z','0 mm','agv_'+label+'_x','agv_'+label+'_y','7 mm','3.4 mm','agv_mount_z_final','steel')
        screw(g,'V04_Anclaje_M3x70_'+label,'Z','agv_mount_z_final+agv_mount_t-70 mm','agv_'+label+'_x','agv_'+label+'_y','3 mm','70 mm')
        c=sub(g,'V04_Tuerca_arandela_M3_'+label)
        ring(c,'Arandela','Z','-7 mm','agv_'+label+'_x','agv_'+label+'_y','7 mm','3.4 mm','1 mm','steel')
        ring(c,'Tuerca_REF','Z','-9.4 mm','agv_'+label+'_x','agv_'+label+'_y','6.3 mm','3.4 mm','2.4 mm','steel')
    c=sub(g,'V04_Contrabarra_izquierda_aluminio_3mm')
    box(c,'Contrabarra','agv_E_x-6 mm','agv_E_y-6 mm','-agv_upper_t-agv_back_t','agv_F_x-agv_E_x+12 mm','12 mm','agv_back_t','steel')
    for label,x,y in LEFT:
        ring(c,'Paso_M3_'+label,'Z','-7 mm','agv_'+label+'_x','agv_'+label+'_y','3.4 mm',None,'5 mm',cut=True)

def lift_body_component(c):
    es=adsk.core.ObjectCollection.create()
    for b in c.bRepBodies:
        if b.isSolid and b.volume>1e-10:es.add(b)
    if not es.count:return
    ip=c.features.moveFeatures.createInput2(es)
    ip.defineAsTranslateXYZ(V('0 mm'),V('0 mm'),V('agv_pan_lift'),False)
    c.features.moveFeatures.add(ip).name='V04_Alza_parametrica_para_pan_HCSR04'

def stage_v04_lift():
    g=comp('00_Base_control');moved=[]
    # The complete upper structure moves rigidly; its native sketches stay editable.
    stationary=('V03_Contraplaca','V03_Casquillo','V03_Separador','V03_Anclaje','V03_Tuerca','V04_')
    for o in list(g.occurrences):
        if o.component.name.startswith(stationary):continue
        lift_body_component(o.component);moved.append(o.component.name)
        progress('alza: '+o.component.name)
    for label,x,y in HOLES:
        c=gc('V03_Separador_M3_'+label)
        c.features.extrudeFeatures.itemByName('Separador').extentOne.distance.expression='agv_mount_z_final'
        c=gc('V03_Anclaje_M3x40_'+label)
        c.constructionPlanes.itemByName('Plano_Tornillo_vastago').definition.offset.expression='agv_mount_z_final+agv_mount_t-70 mm'
        c.features.extrudeFeatures.itemByName('Tornillo_vastago').extentOne.distance.expression='70 mm'
        c.constructionPlanes.itemByName('Plano_Cabeza').definition.offset.expression='agv_mount_z_final+agv_mount_t'
        c.name='V04_Anclaje_M3x70_'+label
        progress('anclaje alargado: '+label)
    gc('V03_Casquillo_central_compresion').features.extrudeFeatures.itemByName('Casquillo').extentOne.distance.expression='agv_mount_z_final+agv_upper_t'
    c=gc('V03_Anclaje_M10x45')
    c.constructionPlanes.itemByName('Plano_Tornillo_vastago').definition.offset.expression='agv_mount_z_final+agv_mount_t-80 mm'
    c.features.extrudeFeatures.itemByName('Tornillo_vastago').extentOne.distance.expression='80 mm'
    c.constructionPlanes.itemByName('Plano_Cabeza').definition.offset.expression='agv_mount_z_final+agv_mount_t'
    c.name='V04_Anclaje_M10x80'
    # Keep the 113 mm J1 tower and every local arm interface unchanged.
    param('shoulder_z','base_top+113 mm');param('base_h','109 mm+agv_pan_lift')
    with open(os.path.join(REPORT,'alza.json'),'w',encoding='utf-8') as f:json.dump({'moved_components':moved,'lift_mm':val('agv_pan_lift')*10,'mount_bottom_mm':val('agv_mount_z_final')*10,'sensor_top_mm':val('pan_top_z')*10,'vertical_clearance_mm':val('pan_clearance')*10},f,indent=2)

def stage_v04_pan():
    param('pan_bottom_z','5 mm')
    rows=json.load(open(os.path.join(REPORT,'pan_selection.json'),encoding='utf-8'))
    keys={(x['component'],x['name']) for x in rows}
    selected=[]
    for o in R.allOccurrences:
        if not o.fullPathName.startswith('80_Auto_referencia'):continue
        for b in o.bRepBodies:
            if (o.component.name,b.name) in keys:selected.append((o,b))
    if len(selected)!=25:raise RuntimeError('Seleccion pan incorrecta: '+str(len(selected)))
    po=main('81_Pan_HCSR04');g=po.component;log=[]
    # Each moved reference body remains selectable. Mixed SG90 body 13 alone rotates.
    for old,b in selected:
        name=old.component.name+'__'+b.name;ap=b.appearance
        moved=b.moveToComponent(po)
        if not moved:raise RuntimeError('No se pudo trasladar '+name)
        moved.name=name
        if ap:moved.appearance=ap
        log.append(name)
        progress('pan: '+name)
    origin(g,'PAN_MOVIL','pan_axis_x','pan_axis_y','0 mm')
    origin(comp('80_Auto_referencia'),'PAN_FIJO','pan_axis_x','pan_axis_y','0 mm')
    j=make_joint('J8_Pan_HCSR04','81_Pan_HCSR04','PAN_MOVIL','80_Auto_referencia','PAN_FIJO',limits=(-90,90))
    j.jointMotion.rotationValue=0
    if D.snapshots.hasPendingSnapshot:D.snapshots.add()
    # Hidden conservative envelope, also useful for later edits.
    o=main('82_Envolvente_pan_REFERENCIA');o.isGrounded=True
    ring(o.component,'Envolvente_circular_con_holgura','Z','pan_bottom_z','pan_axis_x','pan_axis_y','2*(pan_radius_bound+pan_clearance)',None,'pan_top_z-pan_bottom_z')
    o.component.attributes.add('AFR2','scope','Referencia oculta: cilindro conservador 360 grados incluyendo 3 mm radiales. No es una pieza de fabricacion.')
    o.isLightBulbOn=False
    with open(os.path.join(REPORT,'conjunto_pan.json'),'w',encoding='utf-8') as f:json.dump({'joint':'J8_Pan_HCSR04','limits_deg':[-90,90],'moving_bodies':log,'reference_origin_mm':[val('pan_axis_x')*10,val('pan_axis_y')*10,0]},f,ensure_ascii=False,indent=2)

def stage_v04_cap():
    c=gc('V03_Plataforma_sandwich_superior')
    # A cross-component boolean used the final translated cap at an earlier
    # timeline position. Replace it with a cut located by the common parameters.
    old=c.features.combineFeatures.itemByName('V03_Alojamiento_tapa_desmontable')
    old.isSuppressed=True
    ring(c,'V04_Asiento_parametrico_pestana_J1','Z','base_top-7 mm','0 mm','0 mm','b608_od+fit_clear',None,'1.2 mm',cut=True)
    progress('asiento J1 independiente del cuerpo de la tapa')

def stage_v04_save():
    app=adsk.core.Application.get();doc=app.activeDocument
    comments={
        'agv_E_x':'CAD auto 27/10/2026: agujero izquierdo bajo voltimetro, coordenada X.',
        'agv_E_y':'CAD auto 27/10/2026: agujero izquierdo bajo voltimetro, coordenada Y.',
        'agv_F_x':'CAD auto 27/10/2026: segundo agujero izquierdo, coordenada X.',
        'agv_F_y':'CAD auto 27/10/2026: segundo agujero izquierdo, coordenada Y.',
        'pan_axis_x':'CAD SG90 del auto: centro del eje de salida, X.',
        'pan_axis_y':'CAD SG90 del auto: centro del eje de salida, Y.',
        'pan_radius_bound':'ENVOLVENTE conservadora v04: radio maximo de cajas 31.3198822 mm, redondeado hacia arriba.',
        'pan_clearance':'PROPUESTO v04: margen geometrico 3 mm; no tolerancia de fabricacion validada.',
        'pan_top_z':'CAD: cota superior de los 25 solidos moviles del pan.',
        'pan_bottom_z':'ENVOLVENTE v04: limite inferior conservador 5 mm, geometria desde 5.7999569 mm.',
        'agv_pan_lift':'DERIVADO v04: eleva montura y partes superiores, conserva distribucion y torre local.',
        'agv_mount_z_final':'DERIVADO v04: cara inferior real de montura, gobierna separadores y anclajes.',
        'base_h':'DERIVADO v04: interfaz J1 acompana alza para el sensor.',
        'shoulder_z':'DERIVADO v04: hombro conserva torre local de 113 mm sobre base_top.'}
    for n,cmt in comments.items():D.userParameters.itemByName(n).comment=cmt
    if doc.isModified:doc.save('v04: seis anclajes laterales mas central; alza para pan HC-SR04 +/-90 grados; ver informe CAD.')
    D.exportManager.execute(D.exportManager.createFusionArchiveExportOptions(os.path.join(OUT_DIR,V04+'.f3d')))
    import csv
    with open(os.path.join(OUT_DIR,'parametros_prototipo_v04.csv'),'w',encoding='utf-8-sig',newline='') as f:
        w=csv.writer(f);w.writerow(['parametro','expresion','unidad','procedencia'])
        for p in D.userParameters:w.writerow([p.name,p.expression,p.unit,p.comment])
    old=globals()['V03'];globals()['V03']=V04
    stage_v03_manifest();globals()['V03']=old
    p=os.path.join(REPORT,'modelo_v03.json');data=json.load(open(p,encoding='utf-8'))
    data.update({'revision':'AFR2-20261027-09','mounting_holes_mm':HOLES+LEFT,'native_file':V04+'.f3d','parameters_csv':'parametros_prototipo_v04.csv','pan_sweep_deg':[-90,90],'mount_bottom_mm':val('agv_mount_z_final')*10})
    data['car_reference_solids']=643
    with open(os.path.join(REPORT,'modelo_v04.json'),'w',encoding='utf-8') as f:json.dump(data,f,ensure_ascii=False,indent=2)
    os.remove(p)

def run(context):
    if adsk.core.Application.get().activeDocument.name!=V04:raise RuntimeError('Activar la copia v04')
    setup();phase=globals()['PHASE'];os.makedirs(REPORT,exist_ok=True)
    def log(state):
        with open(os.path.join(REPORT,'authoring_journal.jsonl'),'a',encoding='utf-8') as f:f.write(json.dumps({'phase':phase,'state':state})+'\n')
    log('started');globals()['stage_'+phase]();adsk.doEvents();adsk.core.Application.get().activeViewport.refresh();log('applied')
