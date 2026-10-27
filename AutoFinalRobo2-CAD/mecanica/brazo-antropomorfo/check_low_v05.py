# Comprobacion CAD autorizada de la montura baja y el pan completo.
exec(compile(open(r'D:\ProyectoRobotica2\AutoFinalRobo2-CAD\mecanica\brazo-antropomorfo\check_pan_v04.py',encoding='utf-8').read(),'check_pan_v04.py','exec'))
HERE=HERE.replace('revision_v04','revision_v05')
_fk_v04=fk;_initialize_v04=initialize;_circle_v04=circle

def initialize():
    _initialize_v04()
    d=adsk.fusion.Design.cast(adsk.core.Application.get().activeProduct)
    builtins._afr2_check['p']['v05_axis_y']=d.userParameters.itemByName('v05_axis_y').value

def fk(q,p):
    a=_fk_v04(q,p);shift=trans(y=p.get('v05_axis_y',-1.4))
    for name in list(a):
        if name not in ['00_Base_control','80_Auto_referencia','81_Pan_HCSR04']:a[name]=mul(shift,a[name])
    return a

def circle():
    _circle_v04()
    path=os.path.join(HERE,'envolvente_circular.json');data=json.load(open(path,encoding='utf-8'))
    data['strategy']='lateral circular cutout, rearward J1 and supported rack bridges; no general lift'
    data['mount_below_sensor_top_by_mm']=-data.pop('vertical_margin_mm')
    json.dump(data,open(path,'w',encoding='utf-8'),ensure_ascii=False,indent=2)

def mount_geometry():
    d=adsk.fusion.Design.cast(adsk.core.Application.get().activeProduct);r=d.rootComponent
    c=next(c for c in d.allComponents if c.name=='V05_Montura_baja_PETG')
    solids=[b for b in c.bRepBodies if b.isSolid and b.volume>1e-9]
    warnings=[]
    for i in range(d.timeline.count):
        it=d.timeline.item(i)
        if it.healthState in [adsk.fusion.FeatureHealthStates.WarningFeatureHealthState,adsk.fusion.FeatureHealthStates.ErrorFeatureHealthState]:warnings.append({'index':i,'name':it.name,'message':it.errorOrWarningMessage})
    out={'printed_mount_solids':len(solids),'printed_mount_lumps':[b.lumps.count for b in solids],'printed_mount_volume_mm3':sum(b.volume for b in solids)*1000,'mount_plate_bottom_mm':d.userParameters.itemByName('agv_mount_z').value*10,'historical_general_lift_mm':d.userParameters.itemByName('agv_pan_lift').value*10,'timeline_health_messages':warnings,'joints':r.joints.count,'motion_links':r.motionLinks.count}
    json.dump(out,open(os.path.join(HERE,'continuidad_montura.json'),'w',encoding='utf-8'),ensure_ascii=False,indent=2)

_run_v04=run
def run(context):
    if globals().get('MODE','init')=='geometry':mount_geometry();return
    if globals().get('MODE')=='base_yaw':
        for yaw in range(-90,91,10):
            for pan in [-90,0,90]:check([yaw,45,45,0,90,0,109.8,pan])
        builtins._afr2_check['scenarios'].append({'type':'J1_y_pan','J1_deg':[-90,90],'J1_step_deg':10,'pan_deg':[-90,0,90],'cases':57,'other_arm_joints':[45,45,0,90,0,109.8]})
        write_report('barrido_J1_pan');return
    _run_v04(context)
