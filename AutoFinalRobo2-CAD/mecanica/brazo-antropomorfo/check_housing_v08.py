"""Revision geometrica v08 autorizada. No acredita capacidad fisica.

MODE: init, parity, mounts, native, axis, grip, wrist, pan, base_yaw,
circle, geometry, gears, lower, report, connections, contacts, native_limits.
Los modos de muestreo usan BRep temporales y no mueven el documento.
"""
exec(compile(open(r'D:\ProyectoRobotica2\AutoFinalRobo2-CAD\mecanica\brazo-antropomorfo\check_complete_v06.py',encoding='utf-8').read(),'check_complete_v06.py','exec'))
HERE=HERE.replace('revision_v06','revision_v08')
_run_v06=run

def run(_context: str):
    if adsk.core.Application.get().activeDocument.name!='AFR2_Brazo_PVC32_AGV_v08_Housing':
        raise RuntimeError('Esperado documento v08')
    mode=globals().get('MODE','init')
    if mode in ['connections','contacts']:
        file='audit_connections_v06.py' if mode=='connections' else 'contact_graph_v06.py'
        ns={'__name__':'afr2_v08_'+mode}
        exec(compile(open(os.path.join(os.path.dirname(HERE),file),encoding='utf-8').read(),file,'exec'),ns)
        ns['OUT']=HERE;ns['run'](_context);return
    if mode=='native_limits':
        ns={'__name__':'afr2_v08_native'}
        file=os.path.join(os.path.dirname(HERE),'native_review_v03.py')
        exec(compile(open(file,encoding='utf-8').read(),file,'exec'),ns)
        ns['OUT']=HERE
        d=adsk.fusion.Design.cast(adsk.core.Application.get().activeProduct)
        ns['audit'](d);ns['pose'](d,[90,90,0,0,0,0,109.8])
        return
    if mode=='native':native_pan();return
    if mode=='body_geometry':
        d=adsk.fusion.Design.cast(adsk.core.Application.get().activeProduct)
        warnings=[]
        for i in range(d.timeline.count):
            it=d.timeline.item(i)
            if it.healthState in [adsk.fusion.FeatureHealthStates.WarningFeatureHealthState,adsk.fusion.FeatureHealthStates.ErrorFeatureHealthState]:
                warnings.append({'index':i,'name':it.name,'message':it.errorOrWarningMessage})
        c=next(c for c in d.allComponents if c.name=='A06_Encastre_distal_brazo_PETG')
        rows=[{'body':b.name,'solid':b.isSolid,'lumps':b.lumps.count,'volume_mm3':b.volume*1000} for b in c.bRepBodies]
        data={'housing':rows,'timeline_warnings_errors':warnings,'note':'Historico suprimido/consumido no se contabiliza como advertencia.'}
        json.dump(data,open(os.path.join(HERE,'geometria_housing.json'),'w',encoding='utf-8'),ensure_ascii=False,indent=2)
        print(json.dumps(data));return
    if mode=='actuator_ranges':
        d=adsk.fusion.Design.cast(adsk.core.Application.get().activeProduct);rows=[]
        specs=[('J1_Giro_base','MG946R',180,1),('J2_Hombro','GX3240MGV1',270,2),
          ('J3_Codo','GX3270BLS',270,1),('J4_Giro_muneca','MG90S',180,1),
          ('J5_Inclinacion_muneca','MG946R',180,1),('J6_Giro_herramienta','MG90S',180,1),
          ('J7_Servo_pinza','MG90S',180,1),('J8_Pan_HCSR04','SG90',180,1)]
        for name,motor,travel,ratio in specs:
            lim=d.rootComponent.joints.itemByName(name).jointMotion.rotationLimits
            lo=math.degrees(lim.minimumValue);hi=math.degrees(lim.maximumValue)
            needed=(hi-lo)*ratio
            rows.append({'joint':name,'motor':motor,'joint_limits_deg':[lo,hi],
              'servo_span_required_deg':needed,'catalogue_variant_span_deg':travel,
              'nominal_remaining_span_deg':travel-needed,'within_nominal_span':needed<=travel+1e-8})
        data={'scope':'Recorridos nominales de variantes elegidas y limites CAD. Sin calibracion de pulsos ni posicion fisica. Margen cero exige revisar extremos reales.','rows':rows}
        json.dump(data,open(os.path.join(HERE,'recorridos_actuadores.json'),'w',encoding='utf-8'),ensure_ascii=False,indent=2)
        print(json.dumps(data));return
    if mode=='report':write_report('informe_movimiento_v08');return
    if mode=='circle':
        # Unchanged pan geometry; recover original body bounds as explicit reference.
        import shutil
        shutil.copyfile(os.path.join(os.path.dirname(HERE),'revision_v06','pan_selection.json'),os.path.join(HERE,'pan_selection.json'))
    _run_v06(_context)
