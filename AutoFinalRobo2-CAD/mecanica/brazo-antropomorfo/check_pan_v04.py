# Comprobacion CAD del montaje y del conjunto movil completo del ultrasonido.
exec(compile(open(r'D:\ProyectoRobotica2\AutoFinalRobo2-CAD\mecanica\brazo-antropomorfo\check_motion_v03.py',encoding='utf-8').read(),'check_motion_v03.py','exec'))
HERE=HERE.replace('revision_v03','revision_v04')
_fk_arm=fk
def fk(q,p):
    a=_fk_arm(q[:7],p);angle=q[7] if len(q)>7 else 0
    a['81_Pan_HCSR04']=mul(mul(trans(-.5150029369594456,4.299997232182345,0),rot('Z',angle)),trans(.5150029369594456,-4.299997232182345,0))
    return a

def native_pan():
    d=adsk.fusion.Design.cast(adsk.core.Application.get().activeProduct);r=d.rootComponent
    group={o.component.name:o.fullPathName.split('+')[0].split(':')[0] for o in r.allOccurrences}
    es=adsk.core.ObjectCollection.create()
    for o in r.occurrences:
        if not o.component.name.startswith('82_'):es.add(o)
    ip=d.createInterferenceInput(es);ip.areCoincidentFacesIncluded=False
    rr=d.analyzeInterference(ip);out=[]
    for i in range(rr.count):
        v=rr.item(i);a=v.entityOne;b=v.entityTwo;ga=group.get(a.parentComponent.name);gb=group.get(b.parentComponent.name)
        if ga==gb:continue
        vol=v.interferenceBody.volume*1000
        if vol>.001:out.append({'ga':ga,'gb':gb,'a':a.parentComponent.name+'/'+a.name,'b':b.parentComponent.name+'/'+b.name,'mm3':vol})
    json.dump(out,open(os.path.join(HERE,'pose_nativa_interferencias.json'),'w',encoding='utf-8'),ensure_ascii=False,indent=2)

def circle():
    s=builtins._afr2_check;tm=adsk.fusion.TemporaryBRepManager.get()
    d=adsk.fusion.Design.cast(adsk.core.Application.get().activeProduct)
    p=lambda n:d.userParameters.itemByName(n).value
    envelope=tm.createCylinderOrCone(adsk.core.Point3D.create(p('pan_axis_x'),p('pan_axis_y'),p('pan_bottom_z')),p('pan_radius_bound')+p('pan_clearance'),adsk.core.Point3D.create(p('pan_axis_x'),p('pan_axis_y'),p('pan_top_z')),p('pan_radius_bound')+p('pan_clearance'))
    findings=[]
    for b in s['groups']['00_Base_control']['parts']:
        if not overlap(box(envelope),b['bbox']):continue
        cp=tm.copy(envelope)
        if tm.booleanOperation(cp,b['body'],adsk.fusion.BooleanTypes.IntersectionBooleanType) and cp.volume*1000>.001:findings.append({'body':b['name'],'mm3':cp.volume*1000})
    bound=json.load(open(os.path.join(HERE,'pan_selection.json'),encoding='utf-8'))
    out={'type':'conservative_complete_360deg_cylinder','moving_bodies':len(bound),'bounding_radius_mm':max(x['radius_bound_mm'] for x in bound),'adopted_radius_mm':p('pan_radius_bound')*10,'radial_margin_mm':p('pan_clearance')*10,'envelope_z_mm':[p('pan_bottom_z')*10,p('pan_top_z')*10],'mount_bottom_mm':p('agv_mount_z_final')*10,'vertical_margin_mm':(p('agv_mount_z_final')-p('pan_top_z'))*10,'intersections_mount':findings}
    out['moving_bbox_z_mm']=[min(x['bbox_mm'][2] for x in bound),max(x['bbox_mm'][5] for x in bound)]
    out['contains_all_original_moving_bodies']=out['bounding_radius_mm']<=out['adopted_radius_mm']+1e-6 and out['moving_bbox_z_mm'][0]>=out['envelope_z_mm'][0]-1e-6 and out['moving_bbox_z_mm'][1]<=out['envelope_z_mm'][1]+1e-6
    json.dump(out,open(os.path.join(HERE,'envolvente_circular.json'),'w',encoding='utf-8'),ensure_ascii=False,indent=2)

def parity():
    d=adsk.fusion.Design.cast(adsk.core.Application.get().activeProduct);s=builtins._afr2_check
    names=['J1_Giro_base','J2_Hombro','J3_Codo','J4_Giro_muneca','J5_Inclinacion_muneca','J6_Giro_herramienta','J7_Servo_pinza','J8_Pan_HCSR04']
    q=[math.degrees(d.rootComponent.joints.itemByName(n).jointMotion.rotationValue) for n in names];q[6]+=90
    ff=fk(q,s['p']);rows=[]
    for o in d.rootComponent.occurrences:
        if o.component.name not in ff:continue
        rows.append({'component':o.component.name,'maximum_matrix_error':max(abs(x-y) for x,y in zip(o.transform2.asArray(),ff[o.component.name]))})
    json.dump({'q_deg':q,'matrix_differences':rows},open(os.path.join(HERE,'paridad_cinematica.json'),'w',encoding='utf-8'),indent=2)

def run(context):
    mode=globals().get('MODE','init')
    if mode=='init':initialize();return
    if mode=='circle':circle();return
    if mode=='parity':parity();return
    if mode=='mounts':internal_mounts();return
    if mode=='native':native_pan();return
    if mode=='pan':
        q=globals().get('REST',[20,45,45,0,90,0,109.8]);start=builtins._afr2_check['tested']
        for angle in range(-90,91):check(q+[angle],True,{'81_Pan_HCSR04'})
        builtins._afr2_check['scenarios'].append({'type':'pan_HCSR04_completo','limits_deg':[-90,90],'step_deg':1,'poses':builtins._afr2_check['tested']-start,'arm_rest':q})
        write_report('barrido_pan');return
    if mode=='pose':check(globals()['Q']);write_report('poses_completas');return
    if mode=='report':write_report('informe_movimiento_v04')
