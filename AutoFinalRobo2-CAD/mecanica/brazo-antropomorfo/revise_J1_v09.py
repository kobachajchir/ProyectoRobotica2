"""Iteracion J1 v09. Fases sobre copia nativa; cotas inferidas no liberadas."""
exec(compile(open(r'D:\ProyectoRobotica2\AutoFinalRobo2-CAD\mecanica\brazo-antropomorfo\revise_housing_v08.py',encoding='utf-8').read(),'revise_housing_v08.py','exec'))
V09='AFR2_Brazo_PVC32_AGV_v09_J1_GX3270BLS'
REPORT=os.path.join(OUT_DIR,'revision_v09')

ALIASES={'V03_R01_608_J1_inferior':'V09_R01_6000_J1_inferior','V03_R01_608_J1_superior':'V09_R01_6000_J1_superior','V03_Horn_movil_V03_M01_MG946_Base':'V09_Horn25T_J1'}
def gc(cn):
    return next(c for c in D.allComponents if c.name in [cn,ALIASES.get(cn,cn)])
def stage_labels():
    for legacy,current in ALIASES.items():
        c=gc(legacy);c.attributes.add('AFR2','legacy_component_id',legacy);c.name=current

def stage_parameters():
    for n,e in [('v09_face','55 mm'),('v09_bearing_d','26 mm'),('v09_bearing_w','8 mm'),('v09_lower','72.5 mm'),('v09_upper','85.5 mm'),('v09_shaft','10 mm'),('v09_flange','60 mm'),('v09_pcd','50 mm'),('v09_flange_t','3 mm'),('v09_cap_z','89.5 mm')]:
        param(n,e,comment='DISENO v09: geometria propuesta; material y proceso pendientes.')
    # Existing joint origins and platform height remain unchanged.

def cone_C(c,name,cut):
    sections=[]
    for i,z,diam in [(0,'10.14 mm','3 mm'),(1,'12 mm','6.72 mm')]:
        sk=c.sketches.add(datum(c,'Z',z,'Plano_'+name+str(i)));circle(sk,'agv_C_x','agv_C_y',diam);sections.append(sk)
    if D.activeComponent.name!=c.name:next(o for o in R.allOccurrences if o.component.name==c.name).activate()
    ip=c.features.loftFeatures.createInput(OP.CutFeatureOperation if cut else OP.NewBodyFeatureOperation)
    for sk in sections:ip.loftSections.add(sk.profiles.item(0))
    if cut:ip.participantBodies=list(c.bRepBodies)
    f=c.features.loftFeatures.add(ip);f.name=name
    for sk in sections:sk.isVisible=False
    if not cut:
        for b in f.bodies:b.appearance=AP['steel']

def cone_at(c,name,z0,z1,x,y,cut):
    sections=[]
    for i,z,diam in [(0,z0,'3 mm'),(1,z1,'6.72 mm')]:
        sk=c.sketches.add(datum(c,'Z',z,'Plano_'+name+str(i)));circle(sk,x,y,diam);sections.append(sk)
    if D.activeComponent.name!=c.name:next(o for o in R.allOccurrences if o.component.name==c.name).activate()
    ip=c.features.loftFeatures.createInput(OP.CutFeatureOperation if cut else OP.NewBodyFeatureOperation)
    for sk in sections:ip.loftSections.add(sk.profiles.item(0))
    if cut:ip.participantBodies=list(c.bRepBodies)
    f=c.features.loftFeatures.add(ip);f.name=name
    for sk in sections:sk.isVisible=False
    if not cut:
        for b in f.bodies:b.appearance=AP['steel']

def stage_motor():
    c=gc('V03_M01_MG946_Base');discard_geometry(c)
    # Output +Z; the longitudinal axis offset remains inferred from v07.
    box(c,'V09_Cuerpo_GX','-v07_axis_offset','v05_axis_y-v07_motor_w/2','v09_face-v07_case_h','v07_motor_l','v07_motor_w','v07_case_h','servo')
    ring(c,'V09_Resalte','Z','v09_face','0 mm','v05_axis_y','20 mm',None,'v07_boss_h','servo')
    ring(c,'V09_25T_envolvente','Z','v09_face+v07_boss_h','0 mm','v05_axis_y','v07_spline_d',None,'v07_spline_h','steel')
    box(c,'V09_Pestanas','v07_motor_l/2-v07_axis_offset-v07_ear_span/2','v05_axis_y-v07_motor_w/2','v09_face-v07_case_h+v07_ear_back','v07_ear_span','v07_motor_w','v07_ear_t','servo')
    join_all(c,'V09_Motor_comercial_REF')
    for sx in [-1,1]:
        for sy in [-1,1]:
            x=f'v07_motor_l/2-v07_axis_offset+({sx})*v07_mount_pitch/2';y=f'v05_axis_y+({sy})*v07_mount_cross/2'
            ring(c,'V09_Paso_motor'+str(sx)+str(sy),'Z','v09_face-v07_case_h+v07_ear_back-0.1 mm',x,y,'v07_mount_d',None,'v07_ear_t+0.2 mm',cut=True)
    ring(c,'V09_Rosca_REF','Z','v09_face+v07_boss_h+v07_spline_h-4 mm','0 mm','v05_axis_y','3.2 mm',None,'4.1 mm',cut=True)
    c.name='V09_M01_GX3270BLS_Base'
    g=comp('00_Base_control')
    for o in list(g.occurrences):
        n=o.component.name
        if any(t in n for t in ['Fijacion_servo_J1','Inserto_M3_servo_J1','Tornillo_tapa_J1','Inserto_M3_J1']):o.deleteMe()

def stage_structure():
    c=gc('V05_Montura_baja_PETG')
    box(c,'V09_Retirar_cartucho_postes','-30 mm','-40 mm','12 mm','73 mm','52 mm','83 mm',cut=True)
    # Continuous side walls, four mounting banks and an open front/rear cradle.
    for name,x,w in [('izq','-22 mm','10.9 mm'),('der','30.6 mm','10 mm')]:
        box(c,'V09_Pared_'+name,x,'v05_axis_y-14.5 mm','12 mm',w,'29 mm','48 mm')
    box(c,'V09_Puente_superior','-22 mm','v05_axis_y-14.5 mm','60 mm','62.6 mm','29 mm','5.5 mm')
    ring(c,'V09_Paso_horn','Z','59.9 mm','0 mm','v05_axis_y','24 mm',None,'5.1 mm',cut=True)

def stage_structure1():
    c=gc('V05_Montura_baja_PETG')
    ring(c,'V09_Puente_circular','Z','65 mm','0 mm','v05_axis_y','40 mm','24 mm','3 mm')
    ring(c,'V09_Cartucho6000','Z','68 mm','0 mm','v05_axis_y','40 mm','14 mm','21.5 mm')
    for n,z in [('inferior','v09_lower-v09_bearing_w/2'),('superior','v09_upper-v09_bearing_w/2')]:
        ring(c,'V09_Asiento_'+n,'Z',z,'0 mm','v05_axis_y','v09_bearing_d+fit_clear',None,'v09_bearing_w+0.05 mm',cut=True)
    box(c,'V09_Reserva_pestanas_cabezas','-18.8 mm','v05_axis_y-11 mm','43.95 mm','56.6 mm','22 mm','16.05 mm',cut=True)

def stage_structure2():
    c=gc('V05_Montura_baja_PETG')
    # Four transverse mounting screws have real insert bores.
    for sx in [-1,1]:
        for sy in [-1,1]:
            x=f'v07_motor_l/2-v07_axis_offset+({sx})*v07_mount_pitch/2';y=f'v05_axis_y+({sy})*v07_mount_cross/2';tag=str(sx)+str(sy)
            ring(c,'V09_Paso_fijacion'+tag,'Z','33 mm',x,y,'3.4 mm',None,'11 mm',cut=True)
            ring(c,'V09_Inserto_motor'+tag,'Z','37.95 mm',x,y,'4.5 mm',None,'6 mm',cut=True)
    for sy in [-1,1]:
        y=f'v05_axis_y+({sy})*16 mm'
        ring(c,'V09_Paso_tapa'+str(sy),'Z','80 mm','0 mm',y,'3.4 mm',None,'10 mm',cut=True)
        ring(c,'V09_Inserto_tapa'+str(sy),'Z','83.5 mm','0 mm',y,'4.5 mm',None,'6 mm',cut=True)

def stage_structure3():
    c=gc('V05_Montura_baja_PETG')
    join_all(c,'V09_Montura_housing_continuo')
    # C head flush, dimensional ISO10642 envelope, mating countersink.
    cone_C(c,'V09_Avellanado_C',True)
    cc=gc('V05_Anclaje_M3x25_C');discard_geometry(cc)
    ring(cc,'V09_Vastago_C','Z','-13 mm','agv_C_x','agv_C_y','3 mm',None,'23.14 mm','steel')
    cone_C(cc,'V09_Cabeza_C',False)
    join_all(cc,'V09_M3_avellanado_REF')

def stage_bearings():
    param('v09_lower','72.5 mm')
    for name,z in [('inferior','v09_lower-v09_bearing_w/2'),('superior','v09_upper-v09_bearing_w/2')]:
        c=gc('V03_R01_608_J1_'+name);discard_geometry(c)
        ring(c,'Aro_exterior','Z',z,'0 mm','v05_axis_y','v09_bearing_d','22 mm','v09_bearing_w','darksteel')
        ring(c,'Aro_interior','Z',z,'0 mm','v05_axis_y','13.2 mm','v09_shaft','v09_bearing_w','steel')
        ring(c,'Blindaje','Z',f'({z})+0.2 mm','0 mm','v05_axis_y','22 mm','13.2 mm','v09_bearing_w-0.4 mm','darksteel')
        c.attributes.add('AFR2','serie_v09','6000-2Z 10x26x8. Aros internos simplificados; tolerancias a definir.')

def stage_cap():
    c=gc('V03_Tapa_rodamientos_J1');discard_geometry(c)
    ring(c,'V09_Tapa','Z','v09_cap_z','0 mm','v05_axis_y','40 mm','20.6 mm','2 mm')
    for sy in [-1,1]:
        y=f'v05_axis_y+({sy})*16 mm';tag=str(sy)
        ring(c,'V09_Paso_tapa'+tag,'Z','89.4 mm','0 mm',y,'3.4 mm',None,'2.2 mm',cut=True)
        g=comp('00_Base_control')
        bolt=sub(g,'V09_Perno_tapa_'+tag)
        ring(bolt,'Vastago','Z','81.5 mm','0 mm',y,'3 mm',None,'8.14 mm','steel')
        cone_at(bolt,'Cabeza_avellanada','89.64 mm','91.5 mm','0 mm',y,False)
        join_all(bolt,'V09_Perno_tapa_REF')
        cone_at(c,'Avellanado_tapa'+tag,'89.64 mm','91.5 mm','0 mm',y,True)
        ins=sub(g,'V09_Inserto_tapa_'+tag);ring(ins,'Inserto','Z','83.5 mm','0 mm',y,'4.5 mm','3 mm','6 mm','brass')

def stage_motor_fasteners():
    g=comp('00_Base_control')
    for sx in [-1,1]:
        for sy in [-1,1]:
            x=f'v07_motor_l/2-v07_axis_offset+({sx})*v07_mount_pitch/2';y=f'v05_axis_y+({sy})*v07_mount_cross/2';tag=str(sx)+str(sy)
            screw(g,'V09_Fijacion_motor_'+tag,'Z','34.95 mm',x,y,'3 mm','12 mm')
            washer=sub(g,'V09_Arandela_motor_'+tag);ring(washer,'Arandela','Z','46.45 mm',x,y,'7 mm','3.2 mm','0.5 mm','steel')
            ins=sub(g,'V09_Inserto_motor_'+tag);ring(ins,'Inserto','Z','37.95 mm',x,y,'4.5 mm','3 mm','6 mm','brass')

def stage_shaft():
    c=gc('V03_E00_Husillo_base_completo');discard_geometry(c)
    # Local moving coordinates; world Z = base_top + local Z.
    ring(c,'V09_Eje10','Z','63.45 mm-base_top','0 mm','0 mm','v09_shaft',None,'29.55 mm','steel')
    ring(c,'V09_Cuello20','Z','v09_cap_z-base_top','0 mm','0 mm','20 mm',None,'3.5 mm','steel')
    ring(c,'V09_Brida_integral','Z','93 mm-base_top','0 mm','0 mm','v09_flange',None,'v09_flange_t','steel')
    join_all(c,'V09_Eje_bridas_mecanizado_integral')
    ring(c,'V09_Acceso_tornillo_horn','Z','63.4 mm-base_top','0 mm','0 mm','3.4 mm',None,'32.7 mm',cut=True)
    g=comp('01_Giro_base')
    ring(c,'V09_Reserva_cabeza_central','Z','63.4 mm-base_top','0 mm','0 mm','5.8 mm',None,'1.6 mm',cut=True)
    for sign,y in [(1,'4 mm'),(-1,'-6 mm')]:
        box(c,'V09_Plano_dobleD_'+str(sign),'-6 mm',y,'63.4 mm-base_top','12 mm','2 mm','3.4 mm',cut=True)
    ring(c,'V09_Ranura_DIN471','Z','66.8 mm-base_top','0 mm','0 mm','10.1 mm','9.6 mm','1.1 mm',cut=True)

def stage_adapter():
    g=comp('01_Giro_base');c=sub(g,'V09_Adaptador_dobleD_acero')
    ring(c,'V09_Disco_desmontable','Z','63.45 mm-base_top','0 mm','0 mm','22 mm','10 mm','3.35 mm','steel')
    for sign,y in [(1,'4 mm'),(-1,'-5 mm')]:
        box(c,'V09_Llave_metalica_'+str(sign),'-3 mm',y,'63.45 mm-base_top','6 mm','1 mm','3.35 mm','steel')
    join_all(c,'V09_Adaptador_dobleD')
    for sign in [-1,1]:ring(c,'V09_Rosca_M3_'+str(sign),'Z','63.4 mm-base_top',f'({sign})*7.5 mm','0 mm','3 mm',None,'3.5 mm',cut=True)
    c=sub(g,'V09_Circlip_DIN471_10_REF')
    ring(c,'V09_Envolvente_montada','Z','66.8 mm-base_top','0 mm','0 mm','17 mm','9.6 mm','1 mm','steel')
    box(c,'V09_Abertura_circlip','-1.5 mm','-10 mm','66.7 mm-base_top','3 mm','10 mm','1.2 mm',cut=True)
    c.attributes.add('AFR2','referencia','DIN47110x1; ranura9.6/ancho1.1 de Maedler. Anillo simplificado instalado, orejas/elasticidad no resueltas.')
    c=sub(g,'V09_Arandela_axial10_REF');ring(c,'V09_Arandela','Z','67.9 mm-base_top','0 mm','0 mm','13 mm','10 mm','0.6 mm','steel')

def stage_flange():
    c=gc('V03_E00_Husillo_base_completo');g=comp('01_Giro_base')
    for i in range(globals().get('FLANGE_START',0),globals().get('FLANGE_START',0)+2):
        ang=45+i*90;x=f'cos({ang} deg)*v09_pcd/2';y=f'sin({ang} deg)*v09_pcd/2'
        ring(c,'V09_Paso_brida'+str(i),'Z','92.9 mm-base_top',x,y,'4.4 mm',None,'3.2 mm',cut=True)
        bolt=sub(g,'V09_M4_brida_'+str(i))
        ring(bolt,'Tornillo_vastago','Z','88.8 mm-base_top',x,y,'4 mm',None,'16 mm','steel')
        ring(bolt,'Cabeza','Z','104.8 mm-base_top',x,y,'7 mm',None,'4 mm','steel')
        join_all(bolt,'V09_M4x16_REF')
        washer=sub(g,'V09_Arandela_brida_'+str(i));ring(washer,'Arandela','Z','104 mm-base_top',x,y,'9 mm','4.3 mm','0.8 mm','steel')
        # Head above top platform; nut below metal flange at Z89..92.
        nut=sub(g,'V09_Tuerca_brida_'+str(i));ring(nut,'Tuerca_REF','Z','89.8 mm-base_top',x,y,'8.1 mm','4 mm','3.2 mm','steel')

def stage_platform():
    g=comp('01_Giro_base')
    # Remove the old D boss and open the shaft centre; distribute through four M4.
    c=gc('A01_Plataforma_giratoria_PETG')
    ring(c,'V09_Eliminar_acople_D','Z','-1.1 mm','0 mm','0 mm','24 mm',None,'9.2 mm',cut=True)
    for i in range(4):
        x=f'cos({45+i*90} deg)*v09_pcd/2';y=f'sin({45+i*90} deg)*v09_pcd/2'
        ring(c,'V09_Paso_M4_plato'+str(i),'Z','-1.1 mm',x,y,'4.4 mm',None,'10.2 mm',cut=True)

def stage_plate_access():
    c=gc('A01_Plataforma_giratoria_PETG')
    for i in range(4):
        x=f'cos({45+i*90} deg)*v09_pcd/2';y=f'sin({45+i*90} deg)*v09_pcd/2'
        ring(c,'V09_Acceso_M4_'+str(i),'Z','8 mm',x,y,'9.4 mm',None,'10.1 mm',cut=True)

def stage_limiters():
    c=gc('A01_Plataforma_giratoria_PETG');g=comp('01_Giro_base')
    start=globals().get('LIMITER_START',0)
    for i in range(start,start+2):
        x=f'cos({45+i*90} deg)*v09_pcd/2';y=f'sin({45+i*90} deg)*v09_pcd/2'
        ring(c,'V09_Paso_casquillo_'+str(i),'Z','-0.1 mm',x,y,'6.1 mm',None,'8.2 mm',cut=True)
        part=sub(g,'V09_Casquillo_compresion_M4_'+str(i));ring(part,'Casquillo','Z','0 mm',x,y,'6 mm','4.4 mm','8 mm','steel')
        part.attributes.add('AFR2','fabricacion','Casquillo metalico mecanizado6/4.4x8mm; dimensiones propuestas, no catalogo de compra confirmado.')

def stage_horn():
    g=comp('01_Giro_base')
    h=gc('V03_Horn_movil_V03_M01_MG946_Base');discard_geometry(h)
    ring(h,'V09_Horn25T','Z','61.45 mm-base_top','0 mm','0 mm','22 mm','v07_spline_d','2 mm','steel')
    ring(h,'V09_Cubo_horn_4mm','Z','59.45 mm-base_top','0 mm','0 mm','8 mm','v07_spline_d','2 mm','steel')
    join_all(h,'V09_Horn25T_metalico_con_cubo')
    for sx in [-1,1]:ring(h,'V09_Paso_horn'+str(sx),'Z','61.4 mm-base_top',f'({sx})*7.5 mm','0 mm','3.4 mm',None,'2.2 mm',cut=True)
    for n in ['V03_Tornillo_union_V03_M01_MG946_Base-1','V03_Tornillo_union_V03_M01_MG946_Base1','V03_Tornillo_original_horn_V03_M01_MG946_Base']:
        for o in list(g.occurrences):
            if o.component.name==n:o.deleteMe()
    for sx in [-1,1]:
        bolt=sub(g,'V09_M3_horn_'+str(sx))
        ring(bolt,'Vastago','Z','61.45 mm-base_top',f'({sx})*7.5 mm','0 mm','3 mm',None,'5 mm','steel')
        ring(bolt,'Cabeza','Z','59.8 mm-base_top',f'({sx})*7.5 mm','0 mm','5.7 mm',None,'1.65 mm','steel')
        join_all(bolt,'V09_Perno_horn_REF')
    screw(g,'V09_Tornillo25T_REF','Z','59.45 mm-base_top','0 mm','0 mm','3 mm','4 mm',ht='1.2 mm')

def stage_split_cart():
    c=gc('V05_Montura_baja_PETG')
    box(c,'V09_Separar_cartucho_superior','-30 mm','-40 mm','60 mm','80 mm','52 mm','36 mm',cut=True)
    box(c,'V09_Refuerzo_pared_izq','-24.2 mm','v05_axis_y-14.5 mm','12 mm','2.2 mm','29 mm','48 mm')
    box(c,'V09_Refuerzo_pared_der','40.6 mm','v05_axis_y-14.5 mm','12 mm','2.6 mm','29 mm','48 mm')
    join_all(c,'V09_Cuna_baja_con_refuerzos')
    c=sub(comp('00_Base_control'),'V09_Cartucho6000_desmontable_PETG')
    box(c,'V09_Puente_desmontable','-24.2 mm','v05_axis_y-14.5 mm','60 mm','67.4 mm','29 mm','5.5 mm')
    ring(c,'V09_Paso_horn','Z','59.9 mm','0 mm','v05_axis_y','24 mm',None,'5.2 mm',cut=True)
    ring(c,'V09_Puente_circular','Z','65 mm','0 mm','v05_axis_y','40 mm','24 mm','3 mm')
    ring(c,'V09_Cartucho6000','Z','68 mm','0 mm','v05_axis_y','40 mm','14 mm','21.5 mm')
    join_all(c,'V09_Cartucho_integrado')

def stage_cart_seats():
    c=gc('V09_Cartucho6000_desmontable_PETG')
    for n,z in [('inferior','v09_lower-v09_bearing_w/2'),('superior','v09_upper-v09_bearing_w/2')]:
        ring(c,'V09_Asiento_'+n,'Z',z,'0 mm','v05_axis_y','v09_bearing_d+fit_clear',None,'v09_bearing_w+0.05 mm',cut=True)
    for sy in [-1,1]:
        y=f'v05_axis_y+({sy})*16 mm'
        ring(c,'V09_Paso_tapa'+str(sy),'Z','80 mm','0 mm',y,'3.4 mm',None,'10 mm',cut=True)
        ring(c,'V09_Inserto_tapa'+str(sy),'Z','83.5 mm','0 mm',y,'4.5 mm',None,'6 mm',cut=True)

def stage_horn_clearance():
    # The first pass ended at Z65.1, leaving a 0.4mm membrane in the bridge.
    # Cut the complete bridge; the separate annular boss already has ID24.
    c=gc('V09_Cartucho6000_desmontable_PETG')
    ring(c,'V09_Paso_horn_pasante_corregido','Z','59.9 mm','0 mm','v05_axis_y','24 mm',None,'5.7 mm',cut=True)

def stage_cart_bolts():
    g=comp('00_Base_control');c=gc('V09_Cartucho6000_desmontable_PETG');base=gc('V05_Montura_baja_PETG')
    positions=[('-21.5 mm','v05_axis_y-9 mm'),('-21.5 mm','v05_axis_y+9 mm'),('40.5 mm','v05_axis_y-9 mm'),('40.5 mm','v05_axis_y+9 mm')]
    start=globals().get('BOLT_START',0)
    for i,(x,y) in enumerate(positions[start:start+1],start):
        ring(c,'V09_Paso_cartucho'+str(i),'Z','59.9 mm',x,y,'3.4 mm',None,'5.7 mm',cut=True)
        cone_at(c,'V09_Avellanado_cartucho'+str(i),'63.64 mm','65.5 mm',x,y,True)
        ring(base,'V09_Paso_tirante'+str(i),'Z','5.9 mm',x,y,'3.4 mm',None,'54.2 mm',cut=True)
        ring(base,'V09_Cajeado_tuerca'+str(i),'Z','5.9 mm',x,y,'7.2 mm',None,'3.2 mm',cut=True)
        bolt=sub(g,'V09_Tirante_M3x60_'+str(i));ring(bolt,'Vastago','Z','5.5 mm',x,y,'3 mm',None,'58.14 mm','steel')
        cone_at(bolt,'Cabeza_avellanada','63.64 mm','65.5 mm',x,y,False);join_all(bolt,'V09_M3x60_REF')
        nut=sub(g,'V09_Tuerca_tirante_'+str(i));ring(nut,'Tuerca_REF','Z','6.1 mm',x,y,'6.5 mm','3 mm','2.4 mm','steel')
        washer=sub(g,'V09_Arandela_tirante_'+str(i));ring(washer,'Arandela','Z','8.5 mm',x,y,'7 mm','3.2 mm','0.6 mm','steel')

def stage_shaft_fit():
    unused=D.userParameters.itemByName('v09_wall')
    if unused:unused.deleteMe()
    for cn in ['V03_E00_Husillo_base_completo','V09_Adaptador_dobleD_acero']:
        gc(cn).attributes.add('AFR2','material_v09','Propuesto SAE4140 bonificado con certificado Rp0.2>=600MPa. No material comprado ni ensayado.')
    c=gc('V03_E00_Husillo_base_completo')
    c.features.extrudeFeatures.itemByName('V09_Eje10').extentOne.distance.expression='29.55 mm'
    c.features.extrudeFeatures.itemByName('V09_Cuello20').extentOne.distance.expression='3.5 mm'
    c.constructionPlanes.itemByName('Plano_V09_Brida_integral').definition.offset.expression='93 mm-base_top'
    c.features.extrudeFeatures.itemByName('V09_Acceso_tornillo_horn').extentOne.distance.expression='32.7 mm'
    for i in range(4):c.constructionPlanes.itemByName('Plano_V09_Paso_brida'+str(i)).definition.offset.expression='92.9 mm-base_top'
    for o in list(comp('01_Giro_base').occurrences):
        if o.component.name in ['V03_Retencion_axial_J1','V03_Tornillo_retencion_J1']:o.deleteMe()
    clip=gc('V09_Circlip_DIN471_10_REF')
    for dd in clip.sketches.itemByName('Croquis_V09_Abertura_circlip').sketchDimensions:
        if abs(dd.parameter.value-.8)<1e-6:dd.parameter.expression='10 mm'

def stage_hardware_finish():
    start=globals().get('HARDWARE_START',0)
    for i in range(start,start+2):
        c=gc('V09_M4_brida_'+str(i))
        c.features.extrudeFeatures.itemByName('Tornillo_vastago').extentOne.distance.expression='16 mm'
        c.constructionPlanes.itemByName('Plano_Tornillo_vastago').definition.offset.expression='88.8 mm-base_top'
        c.features.extrudeFeatures.itemByName('Cabeza').extentOne.distance.expression='4 mm'
        c.constructionPlanes.itemByName('Plano_Cabeza').definition.offset.expression='104.8 mm-base_top'
        c=gc('V09_Tuerca_brida_'+str(i))
        c.constructionPlanes.itemByName('Plano_Tuerca_REF').definition.offset.expression='89.8 mm-base_top'
        c.features.extrudeFeatures.itemByName('Tuerca_REF').extentOne.distance.expression='3.2 mm'
        for dd in c.sketches.itemByName('Croquis_Tuerca_REF').sketchDimensions:
            if isinstance(dd,adsk.fusion.SketchDiameterDimension) and abs(dd.parameter.value-.7)<1e-6:dd.parameter.expression='8.1 mm'
        c=gc('V09_Arandela_brida_'+str(i))
        c.constructionPlanes.itemByName('Plano_Arandela').definition.offset.expression='104 mm-base_top'
        c.features.extrudeFeatures.itemByName('Arandela').extentOne.distance.expression='0.8 mm'
        for dd in c.sketches.itemByName('Croquis_Arandela').sketchDimensions:
            if isinstance(dd,adsk.fusion.SketchDiameterDimension) and abs(dd.parameter.value-.42)<1e-6:dd.parameter.expression='4.3 mm'


def stage_bornera():
    param('v09_bornera_x','-51 mm',comment='Reubicacion tras interferencia con cuna/tirante; envolvente estudiada contra todos los otros cuerpos.')
    param('v09_bornera_y','23 mm',comment='Mantiene altura12mm y evita placas verticales, auto y giro del HCSR.')
    c=gc('V05_Montura_baja_PETG')
    box(c,'V09_Retirar_cajetin_anterior','43.2 mm','-32 mm','12 mm','19 mm','16 mm','10 mm',cut=True)
    shifted(gc('V03_Bornera_y_alimentacion_REF'),x='v09_bornera_x',y='v09_bornera_y+23 mm',z='agv_mount_z+agv_mount_t-28 mm')
    for tag,x,y,w,h in [('Izq','v09_bornera_x-12 mm','v09_bornera_y-7 mm','2 mm','13 mm'),('Der','v09_bornera_x+10 mm','v09_bornera_y-7 mm','2 mm','13 mm'),('Fondo','v09_bornera_x-12 mm','v09_bornera_y+5 mm','24 mm','2 mm')]:
        box(c,'V09_Cajetin_bornera_'+tag,x,y,'12 mm',w,h,'9 mm')
    for tag,x in [('I','v09_bornera_x-10 mm'),('D','v09_bornera_x+9 mm')]:
        box(c,'V09_Clip_bornera_'+tag,x,'v09_bornera_y-5 mm','20 mm','1 mm','9 mm','1 mm')
    join_all(c,'V09_Cuna_y_bornera_continuas')

def stage_PLA():
    app=adsk.core.Application.get()
    name='AFR2 PLA PrintaLot rho1.24 E1500 HIPOTESIS sin resistencia FDM'
    mat=D.materials.itemByName(name)
    if not mat:
        template=next(lib.materials.itemByName('ABS Plastic') for lib in app.materialLibraries if lib.materials.itemByName('ABS Plastic'))
        mat=D.materials.addByCopy(template,name)
    edits={}
    for prop in mat.materialProperties:
        if prop.isReadOnly:continue
        if prop.id=='structural_Density':prop.value=1240.;edits[prop.id]=prop.value
        elif prop.id.startswith('structural_Young_modulus'):prop.value=1500000.;edits[prop.id]=prop.value
        elif prop.id.startswith('structural_Poisson_ratio'):prop.value=.38;edits[prop.id]=prop.value
        elif prop.id.startswith('structural_Shear_modulus'):prop.value=1500000./2.76;edits[prop.id]=prop.value
        elif 'strength' in prop.id.lower():prop.value=0.;edits[prop.id]=prop.value
    ap=D.appearances.itemByName('AFR2_PLA_PrintALot_Rojo')
    if not ap:ap=D.appearances.addByCopy(AP['red'],'AFR2_PLA_PrintALot_Rojo')
    assigned=[]
    allowed={o.component.name for o in R.allOccurrences if o.fullPathName.split('+')[0].split(':')[0] not in ['80_Auto_referencia','81_Pan_HCSR04','82_Envolvente_pan_REFERENCIA']}
    for c in D.allComponents:
        if c.name not in allowed:continue
        for b in c.bRepBodies:
            if b.appearance and b.appearance.name in ['AFR2_PETG_Rojo','AFR2_PLA_PrintALot_Rojo']:
                b.material=mat;b.appearance=ap;assigned.append(c.name)
                c.attributes.add('AFR2','filamento_v09','USUARIO PLA PrintaLot. Densidad1240kg/m3. E1500MPa hipotesis; resistencia FDM no asignada; perfil y orientacion pendientes.')
    data={'material':mat.name,'density_kg_m3':mat.materialProperties.itemById('structural_Density').value,'edited_properties':edits,'printed_bodies':len(assigned),'components':assigned,'note':'Plantilla ABS solo contenedor de propiedades. rho oficial, E y nu hipotesis. Resistencias editables puestas a0 para no heredar admisibles ABS. Propiedades restantes no calificadas para FEA/termica.'}
    json.dump(data,open(os.path.join(REPORT,'material_PLA_v09.json'),'w',encoding='utf-8'),ensure_ascii=False,indent=2)
    print(json.dumps(data))

def stage_XL_clearance():
    # Nuts under the moving flange swept through the nearest vertical PCB.
    # Move this module sideways only; keep the low base and the existing PCD.
    param('v09_XL1_dx','-6 mm',comment='Traslado lateral para liberar tuercas de brida durante J1; sin elevar brazo.')
    shifted(gc('V03_XL4016_vertical_1'),x='v09_XL1_dx',y='v05_rack_y')
    c=gc('V05_Montura_baja_PETG')
    box(c,'V09_Puente_XL1_reubicado','-30 mm+v09_XL1_dx','v05_rack_y-28 mm','v05_rack_rail_z','7 mm','13 mm-v05_rack_y+28 mm','4 mm')
    for tag,y,z,h in [('frente','10 mm','12 mm','electronics_z+buck_l+2 mm-12 mm'),('fondo','v05_rack_y-28 mm','v05_rack_rail_z','electronics_z+buck_l+2 mm-v05_rack_rail_z')]:
        box(c,'V09_Poste_XL1_'+tag,'-30 mm+v09_XL1_dx',y,z,'7 mm','3 mm',h)
    for tag,y,dep in [('I','v05_rack_y-25 mm','7 mm'),('D','v05_rack_y+18 mm','13 mm-v05_rack_y-18 mm')]:
        box(c,'V09_Labio_XL1_bajo_'+tag,'-29 mm+v09_XL1_dx',y,'electronics_z-3 mm','6 mm',dep,'3 mm')
        box(c,'V09_Labio_XL1_alto_'+tag,'-29 mm+v09_XL1_dx',y,'electronics_z+buck_l','6 mm',dep,'2 mm')
    join_all(c,'V09_Cuna_rack_XL1_reubicado')

def stage_XL_spacing():
    param('v09_XL1_dx','-5.5 mm',comment='Libera tuercas J1; ajuste tras estudiar la separacion entre reguladores.')
    param('v09_XL2_dx','-3 mm',comment='Reparto de holgura lateral del rack; conserva apoyo en labios existentes.')
    shifted(gc('V03_XL4016_vertical_2'),x='v09_XL2_dx',y='v05_rack_y')

def stage_save():
    import csv
    app=adsk.core.Application.get();doc=app.activeDocument
    for c in D.allComponents:
        for prop in ['isSketchFolderLightBulbOn','isConstructionFolderLightBulbOn','isOriginFolderLightBulbOn','isJointsFolderLightBulbOn']:
            if hasattr(c,prop):setattr(c,prop,False)
    cam=app.activeViewport.camera;cam.target=P(0,10,13);cam.eye=P(-55,65,48);cam.upVector=adsk.core.Vector3D.create(0,0,1);cam.isFitView=True;app.activeViewport.camera=cam
    app.activeViewport.saveAsImageFile(os.path.join(REPORT,'ensamble_v09.png'),1400,1000)
    cam=app.activeViewport.camera;cam.isFitView=False;cam.target=P(0,-1.4,6);cam.eye=P(-18,-23,18);cam.viewExtents=14;app.activeViewport.camera=cam
    app.activeViewport.saveAsImageFile(os.path.join(REPORT,'detalle_J1_v09.png'),1400,1000)
    doc.save('v09 J1: GX3270BLS, eje/brida integral, 6000 y housing bajo. Revision geometrica y calculos bajo hipotesis.')
    D.exportManager.execute(D.exportManager.createFusionArchiveExportOptions(os.path.join(OUT_DIR,V09+'.f3d')))
    for cn,file in [('V05_Montura_baja_PETG','Housing_J1_GX3270BLS_v09.f3d'),('V09_M01_GX3270BLS_Base','GX3270BLS_Base_REF_v09.f3d'),('V03_E00_Husillo_base_completo','Eje_brida_J1_v09.f3d'),('V09_Cartucho6000_desmontable_PETG','Cartucho6000_J1_v09.f3d'),('V09_Adaptador_dobleD_acero','Adaptador_dobleD_J1_v09.f3d')]:
        D.exportManager.execute(D.exportManager.createFusionArchiveExportOptions(os.path.join(REPORT,file),gc(cn)))
    with open(os.path.join(OUT_DIR,'parametros_prototipo_v09.csv'),'w',encoding='utf-8-sig',newline='') as f:
        w=csv.writer(f);w.writerow(['parametro','expresion','unidad','procedencia'])
        for p in D.userParameters:w.writerow([p.name,p.expression,p.unit,p.comment])
    json.dump({'document':doc.name,'id':doc.dataFile.id,'saved':not doc.isModified,'parameters':D.userParameters.count,'components':D.allComponents.count,'joints':R.joints.count,'motion_links':R.motionLinks.count},open(os.path.join(REPORT,'modelo_v09.json'),'w',encoding='utf-8'),ensure_ascii=False,indent=2)

def run(_context: str):
    if adsk.core.Application.get().activeDocument.name!=V09:raise RuntimeError('Activar copia v09')
    setup();os.makedirs(REPORT,exist_ok=True)
    journal=os.path.join(REPORT,'authoring_journal.jsonl')
    with open(journal,'a',encoding='utf-8') as f:f.write(json.dumps({'phase':PHASE,'flange_start':globals().get('FLANGE_START'),'bolt_start':globals().get('BOLT_START'),'state':'started'})+'\n')
    globals()['stage_'+PHASE]();D.activateRootComponent();adsk.doEvents()
    with open(journal,'a',encoding='utf-8') as f:f.write(json.dumps({'phase':PHASE,'flange_start':globals().get('FLANGE_START'),'bolt_start':globals().get('BOLT_START'),'state':'applied'})+'\n')
    print(json.dumps({'phase':PHASE,'complete':True}))
