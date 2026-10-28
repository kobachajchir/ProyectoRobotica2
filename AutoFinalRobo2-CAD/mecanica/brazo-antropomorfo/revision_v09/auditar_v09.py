# Revision de continuidad y apoyos, autorizada por el usuario. Fusion API.
import adsk.core,adsk.fusion,json,os,math
OUT=r'D:\ProyectoRobotica2\AutoFinalRobo2-CAD\mecanica\brazo-antropomorfo\revision_v09'

def run(context):
    app=adsk.core.Application.get();d=adsk.fusion.Design.cast(app.activeProduct);r=d.rootComponent
    occurrences={o.component.name:o for o in r.allOccurrences}
    def physical(c):
        out=[]
        for b in c.bRepBodies:
            if b.isSolid and b.volume>1e-9:out.append(b)
        return out
    def proxy(b):return b.createForAssemblyContext(occurrences[b.parentComponent.name])
    def distance(a,b):return app.measureManager.measureMinimumDistance(proxy(a),proxy(b)).value*10
    printed=[];shafts=[];bearings=[];stock=[];issues=[]
    for cn,o in occurrences.items():
        if cn.startswith('REF_AUTO_') or cn in ['80_Auto_referencia','81_Pan_HCSR04','82_Envolvente_pan_REFERENCIA']:continue
        for b in physical(o.component):
            if b.appearance and b.appearance.name in ['AFR2_PETG_Rojo','AFR2_PLA_PrintALot_Rojo']:
                row={'component':cn,'body':b.name,'lumps':b.lumps.count,'volume_mm3':round(b.volume*1000,3)};printed.append(row)
                if b.lumps.count!=1:issues.append('Multiples islas impresas: '+cn)
        if cn in ['E01_Eje_hombro_8mm','V02_E02_Eje_y_brida_codo','V02_E03_Eje_J4','V02_E04_Eje_y_brida_J5','V02_E05_Acople_J6','V03_E00_Husillo_base_completo']:
            bb=physical(o.component);row={'component':cn,'bodies':len(bb),'lumps':[b.lumps.count for b in bb]};shafts.append(row)
            if len(bb)!=1 or bb[0].lumps.count!=1:issues.append('Eje discontinuo: '+cn)
    expected=[('V09_R01_6000_J1_inferior','V03_E00_Husillo_base_completo','00_Base_control'),('V09_R01_6000_J1_superior','V03_E00_Husillo_base_completo','00_Base_control'),
      ('R02_608ZZ_Hombro_-1','E01_Eje_hombro_8mm','01_Giro_base'),('R02_608ZZ_Hombro_1','E01_Eje_hombro_8mm','01_Giro_base'),
      ('R03_608ZZ_Codo_-1','V02_E02_Eje_y_brida_codo','02_Brazo_PVC'),('R03_608ZZ_Codo_1','V02_E02_Eje_y_brida_codo','02_Brazo_PVC'),
      ('V03_R04_608_J4_inferior','V02_E03_Eje_J4','03_Antebrazo_PVC'),('V02_R04_608_J4','V02_E03_Eje_J4','03_Antebrazo_PVC'),
      ('V02_R05_625_-1','V02_E04_Eje_y_brida_J5','04_Giro_muneca'),('V02_R05_625_1','V02_E04_Eje_y_brida_J5','04_Giro_muneca'),
      ('V03_R06_625_J6_inferior','V02_E05_Acople_J6','05_Inclinacion_muneca'),('V02_R06_625_J6','V02_E05_Acople_J6','05_Inclinacion_muneca')]
    for name,shaft,group in expected:
        if name not in occurrences:issues.append('Rodamiento ausente: '+name);continue
        bb=physical(occurrences[name].component)
        if len(bb)!=3:issues.append('Rodamiento incompleto: '+name);continue
        inner=next(b for b in bb if b.name=='Aro_interior');outer=next(b for b in bb if b.name=='Aro_exterior')
        idd,od,w=(10,26,8) if 'J1_' in name else ((5,16,5) if '625' in name else (8,22,7))
        inner_od,outer_id=(13.2,22) if 'J1_' in name else (idd+3,od-3)
        ev={'Aro_interior':math.pi*(inner_od**2-idd**2)*w/4,'Aro_exterior':math.pi*(od**2-outer_id**2)*w/4,'Blindaje':math.pi*(outer_id**2-inner_od**2)*(w-.4)/4}
        volume_errors={b.name:b.volume*1000-ev[b.name] for b in bb}
        if any(abs(v)>.005 for v in volume_errors.values()):issues.append('Rodamiento recortado: '+name)
        sb=physical(occurrences[shaft].component)
        ds=min(distance(inner,b) for b in sb)
        g=next(o.component for o in r.occurrences if o.component.name==group)
        supports=[b for o in g.occurrences for b in physical(o.component) if b.appearance and b.appearance.name in ['AFR2_PETG_Rojo','AFR2_PLA_PrintALot_Rojo']]
        do=min(distance(outer,b) for b in supports)
        row={'bearing':name,'bodies':len(bb),'inner_shaft_gap_mm':ds,'outer_support_gap_mm':do,'volume_error_vs_complete_rings_mm3':volume_errors};bearings.append(row)
        if ds>0.01 or do>0.01:issues.append('Apoyo sin contacto axial/radial: '+name)
    # Empty historical components are source history, not additional floating parts.
    primaries={}
    for o in r.occurrences:
        gn=o.component.name
        if gn=='80_Auto_referencia':continue
        parts=[]
        for oc in o.component.occurrences:
            if oc.component.name.startswith('V03_Tapa_'):continue
            for b in physical(oc.component):
                if b.appearance and b.appearance.name in ['AFR2_PETG_Rojo','AFR2_PLA_PrintALot_Rojo']:parts.append({'component':oc.component.name,'body':b.name})
        primaries[gn]=parts
    expected_counts={'00_Base_control':3,'01_Giro_base':1,'02_Brazo_PVC':3,'03_Antebrazo_PVC':2,'04_Giro_muneca':1,'05_Inclinacion_muneca':1,'06_Herramienta':1,'07_Mandibula_izquierda':1,'08_Mandibula_derecha':1}
    for gn,n in expected_counts.items():
        if len(primaries.get(gn,[]))!=n:issues.append('Revisar piezas impresas separadas '+gn+': '+str(len(primaries.get(gn,[]))))
    for cn,lp in [('L01_PVC32_Corte_1','tube1_cut'),('L02_PVC32_Corte_2','tube2_cut')]:
        bb=physical(occurrences[cn].component);p=d.userParameters;length=p.itemByName(lp).value*10;od=p.itemByName('pvc_od').value*10;idd=p.itemByName('pvc_id').value*10
        expected=math.pi*(od*od-idd*idd)*length/4;actual=sum(b.volume*1000 for b in bb)
        stock.append({'component':cn,'length_mm':length,'volume_mm3':actual,'expected_volume_mm3':expected})
        if len(bb)!=1 or abs(actual-expected)>.005:issues.append('Cano incompleto: '+cn)
    for cn,o in occurrences.items():
        if not cn.endswith('_Perno_M3_REF'):continue
        bb=physical(o.component);expected=math.pi*(3**2*18+5.5**2*2)/4;actual=sum(b.volume*1000 for b in bb)
        stock.append({'component':cn,'bodies':len(bb),'lumps':[b.lumps.count for b in bb],'volume_mm3':actual,'expected_volume_mm3':expected})
        if len(bb)!=1 or bb[0].lumps.count!=1 or abs(actual-expected)>.005:issues.append('Perno recortado: '+cn)
    result={'document':app.activeDocument.name,'scope':'continuidad CAD y contacto de aros con eje/apoyo; no ensayo de carga','printed':printed,'shafts':shafts,'bearings':bearings,'stock':stock,'primary_parts':primaries,'issues':issues}
    os.makedirs(OUT,exist_ok=True)
    with open(os.path.join(OUT,'continuidad_y_rodamientos.json'),'w',encoding='utf-8') as f:json.dump(result,f,ensure_ascii=False,indent=2)
    print('AUDITORIA',len(printed),'impresos',len(shafts),'ejes',len(bearings),'rodamientos','PENDIENTES',json.dumps(issues,ensure_ascii=False))

