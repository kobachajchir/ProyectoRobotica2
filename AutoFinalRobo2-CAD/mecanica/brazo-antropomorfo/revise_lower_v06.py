# Fusion v06. Unificar las contraplacas inferiores; conservar montura baja.
exec(compile(open(r'D:\ProyectoRobotica2\AutoFinalRobo2-CAD\mecanica\brazo-antropomorfo\mount_low_v05.py',encoding='utf-8').read(),'mount_low_v05.py','exec'))
V06='AFR2_Brazo_PVC32_AGV_v06';REPORT=os.path.join(OUT_DIR,'revision_v06')

def stage_v06_lower():
    g=comp('00_Base_control')
    for o in list(g.occurrences):
        if o.component.name in ['V03_Contraplaca_aluminio_3mm','V04_Contrabarra_izquierda_aluminio_3mm']:o.deleteMe()
    c=sub(g,'V06_Contraplaca_inferior_unica_PETG')
    box(c,'Zona_central_derecha','-18 mm','-31 mm','-agv_upper_t-agv_back_t','94 mm','63 mm','agv_back_t')
    box(c,'Zona_izquierda_y_puente','agv_E_x-6 mm','agv_E_y-6 mm','-agv_upper_t-agv_back_t','-16 mm-agv_E_x+6 mm','12 mm','agv_back_t')
    unite(c,'Contraplaca_inferior_una_impresion')
    for label,x,y in ALL_HOLES:ring(c,'Paso_'+label,'Z','-agv_upper_t-agv_back_t-1 mm','agv_'+label+'_x','agv_'+label+'_y','3.4 mm',None,'agv_back_t+2 mm',cut=True)
    ring(c,'Paso_central_M10','Z','-agv_upper_t-agv_back_t-1 mm','agv_center_x','agv_center_y','10.5 mm',None,'agv_back_t+2 mm',cut=True)
    unite(c,'Contraplaca_terminada_una_pieza')
    c.attributes.add('AFR2','material_propuesto','PETG rojo, impresion FDM. Espesor conservado 3 mm; arandelas y tuercas metalicas. Resistencia y precarga pendientes.')
    for b in c.bRepBodies:b.name='Contraplaca inferior unica: central, derecha e izquierda'
    D.activateRootComponent()

def stage_v06_save():
    app=adsk.core.Application.get();doc=app.activeDocument
    if doc.isModified:doc.save('v06: contraplaca inferior unica PETG; ver evidencia geometrica y limites en revision_v06.')
    D.exportManager.execute(D.exportManager.createFusionArchiveExportOptions(os.path.join(OUT_DIR,V06+'.f3d')))
    import csv
    with open(os.path.join(OUT_DIR,'parametros_prototipo_v06.csv'),'w',encoding='utf-8-sig',newline='') as f:
        w=csv.writer(f);w.writerow(['parametro','expresion','unidad','procedencia'])
        for p in D.userParameters:w.writerow([p.name,p.expression,p.unit,p.comment])
    data={'document':doc.name,'file_id':doc.dataFile.id,'parameters':D.userParameters.count,'components':D.allComponents.count,'root_groups':R.occurrences.count,'timeline_items':D.timeline.count,'joints':R.joints.count,'motion_links':R.motionLinks.count,'mount_floor_mm':val('agv_mount_z')*10,'mount_top_mm':val('agv_mount_z+agv_mount_t')*10,'J1_axis_y_mm':val('v05_axis_y')*10,'J1_interface_z_mm':val('base_top')*10,'shoulder_z_mm':val('shoulder_z')*10,'general_lift_mm':val('agv_pan_lift')*10,'scope':'Revision geometrica CAD; no ensayo de estabilidad, resistencia ni carga.'}
    json.dump(data,open(os.path.join(REPORT,'modelo_v06.json'),'w',encoding='utf-8'),ensure_ascii=False,indent=2)

def run(context):
    if adsk.core.Application.get().activeDocument.name!=V06:raise RuntimeError('Activar v06')
    setup();phase=globals()['PHASE'];os.makedirs(REPORT,exist_ok=True)
    def log(state):
        with open(os.path.join(REPORT,'authoring_journal.jsonl'),'a',encoding='utf-8') as f:f.write(json.dumps({'phase':phase,'state':state})+'\n')
    log('started');globals()['stage_'+phase]();adsk.doEvents();adsk.core.Application.get().activeViewport.refresh();log('applied')
