# Revision CAD solicitada: bajo montaje, pan, ejes, pinza y engranajes.
exec(compile(open(r'D:\ProyectoRobotica2\AutoFinalRobo2-CAD\mecanica\brazo-antropomorfo\check_motion_v03.py',encoding='utf-8').read(),'check_motion_v03.py','exec'))
_run_arm_v03=run
exec(compile(open(r'D:\ProyectoRobotica2\AutoFinalRobo2-CAD\mecanica\brazo-antropomorfo\check_low_v05.py',encoding='utf-8').read(),'check_low_v05.py','exec'))
HERE=HERE.replace('revision_v05','revision_v06')
_run_low_v05=run

def gear_specification():
    d=adsk.fusion.Design.cast(adsk.core.Application.get().activeProduct);r=d.rootComponent
    p=lambda n:d.userParameters.itemByName(n).value
    links=[]
    for ml in r.motionLinks:
        links.append({'name':ml.name,'reversed':ml.isReversed,'valueOne':ml.valueOne.expression,'valueTwo':ml.valueTwo.expression})
    data={'shoulder':{'module_mm':p('gear_module')*10,'pinion_teeth':p('pinion_teeth'),'wheel_teeth':p('wheel_teeth'),'axis_distance_mm':p('gear_centers')*10,'expected_distance_mm':p('gear_module')*(p('pinion_teeth')+p('wheel_teeth'))*5,'ratio':p('gear_ratio')},'J4':{'module_mm':1.5,'teeth':[24,24],'axis_distance_mm':36,'expected_distance_mm':1.5*(24+24)/2,'ratio':1},'gripper':{'module_mm':p('grip_module')*10,'pinion_teeth':p('grip_teeth'),'pitch_radius_mm':p('grip_r')*10,'rack_travel_each_mm':math.pi*p('grip_r')*10,'opening_min_mm':p('grip_min')*10,'opening_at_109_8_deg_mm':(p('grip_min')+2*p('grip_r')*math.radians(109.8))*10,'opening_at_133_7_deg_mm':(p('grip_min')+2*p('grip_r')*math.radians(133.7))*10},'links':links,'scope':'Relaciones geometricas nominales; interferencias de dientes se comprueban por muestreo. No calcula resistencia, desgaste, backlash real ni torque.'}
    json.dump(data,open(os.path.join(HERE,'transmisiones.json'),'w',encoding='utf-8'),ensure_ascii=False,indent=2)

def lower_audit():
    d=adsk.fusion.Design.cast(adsk.core.Application.get().activeProduct);r=d.rootComponent
    c=next(c for c in d.allComponents if c.name=='V06_Contraplaca_inferior_unica_PETG');bb=list(c.bRepBodies)
    data={'component':c.name,'bodies':len(bb),'lumps':[b.lumps.count for b in bb],'volume_mm3':sum(b.volume for b in bb)*1000,'bbox_mm':[10*v for v in list(bb[0].boundingBox.minPoint.asArray())+list(bb[0].boundingBox.maxPoint.asArray())],'mounting_holes':7,'thread_model':'pasos lisos M3/M10; tuercas y arandelas separadas','material':'PETG propuesto, espesor 3 mm; resistencia sin ensayo'}
    json.dump(data,open(os.path.join(HERE,'contraplaca_unica.json'),'w',encoding='utf-8'),ensure_ascii=False,indent=2)

def run(context):
    mode=globals().get('MODE','init')
    if mode in ['grip','wrist','axis']:_run_arm_v03(context);return
    if mode=='gears':gear_specification();return
    if mode=='lower':lower_audit();return
    if mode=='report':write_report('informe_movimiento_v06');return
    _run_low_v05(context)
