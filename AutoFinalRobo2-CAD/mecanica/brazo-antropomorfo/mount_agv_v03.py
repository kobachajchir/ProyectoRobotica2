# Modelado Fusion, revision 27/10/2026. Maestro v02 conservado.
exec(compile(open(r'D:\ProyectoRobotica2\AutoFinalRobo2-CAD\mecanica\brazo-antropomorfo\revise_gripper_wrist.py',encoding='utf-8').read(),'revise_gripper_wrist.py','exec'))
import builtins
V03='AFR2_Brazo_PVC32_AGV_v03'
REPORT=os.path.join(OUT_DIR,'revision_v03')
HOLES=[('A','31.051 mm','19.739 mm'),('B','67.997 mm','19.739 mm'),('C','31.051 mm','-21.036 mm'),('D','68.418 mm','-21.036 mm')]

def gc(cn):return next(c for c in D.allComponents if c.name==cn)
def unite(c,name='V03_Una_pieza_impresa'):
    join_all(c,name)
    bodies=[b for b in c.bRepBodies if b.isSolid and b.volume>1e-9]
    if len(bodies)!=1 or bodies[0].lumps.count!=1:raise RuntimeError('Solido discontinuo: '+c.name)

def screw(parent,name,axis,start,u,v,diam,length,head='5.5 mm',ht='3 mm'):
    c=sub(parent,name)
    ring(c,'Tornillo_vastago',axis,start,u,v,diam,None,length,'steel')
    ring(c,'Cabeza',axis,f'({start})+({length})',u,v,head,None,ht,'darksteel')
    join_all(c,'Tornillo_comercial_REF')
    return c

def stage_v03_parameters():
    param('base_h','109 mm');param('shoulder_z','225 mm')
    for n,e in [('agv_mount_z','22 mm'),('agv_mount_t','6 mm'),('agv_upper_t','3 mm'),('agv_back_t','3 mm'),('agv_center_x','0.333 mm'),('agv_center_y','-8.501 mm'),('electronics_z','agv_mount_z+agv_mount_t+3 mm'),('electronics_pitch','23 mm')]:param(n,e)
    for label,x,y in HOLES:param('agv_'+label+'_x',x);param('agv_'+label+'_y',y)
    os.makedirs(REPORT,exist_ok=True)

def stage_v03_guides():
    c=gc('V02_A20_Bastidor_pinza')
    for side in [-1,1]:
        y='17.5 mm' if side>0 else '-19.2 mm'
        for end,x in [('I','-palm_w/2'),('D','palm_w/2-5.5 mm')]:
            box(c,'Poste_labio_'+str(side)+end,x,y,'grip_deck+4 mm','5.5 mm','1.7 mm','11 mm')
    unite(c,'V03_Palma_guias_labios_continuos')
    # Join the J5 nucleus, servo cradle and J6 bearing carrier with real material.
    c=gc('V02_A18_Nucleo_J5_y_columna_J6')
    for side in [-1,1]:
        box(c,'Puente_vertical_'+str(side),'-6 mm','12.5 mm' if side>0 else '-15.5 mm','10 mm','12 mm','3 mm','11 mm')
    unite(c,'V03_Nucleo_y_portarodamiento_unidos')

def stage_v03_j4():
    # Restore the complete shaft first through scoped cuts; then key the output.
    gear=gc('V02_Rueda24_J4')
    gear.constructionPlanes.itemByName('Plano_Raiz_engranaje').definition.offset.expression='6 mm'
    gear.constructionPlanes.itemByName('Plano_evolvente_20grados').definition.offset.expression='6 mm'
    comp('03_Antebrazo_PVC').jointOrigins.itemByName('V02_J4_MOTOR').offsetZ.expression='link2+6 mm'
    import re
    c=gc('V02_M04_MG90S_J4')
    for p in c.modelParameters:
        e=p.expression;n=re.sub(r'link2\s*-\s*4\s*mm','link2',e)
        if n!=e:p.expression=n
    c=gc('V02_E03_Eje_J4')
    c.constructionPlanes.itemByName('Plano_Eje').definition.offset.expression='-17 mm'
    c.features.extrudeFeatures.itemByName('Eje').extentOne.distance.expression='34 mm'
    box(c,'Plano_D_del_eje','-5 mm','-5 mm','6 mm','10 mm','1.7 mm','12 mm',key='steel',cut=True)
    ring(c,'Rosca_axial_REF','Z','12 mm','0 mm','0 mm','3.3 mm',None,'7 mm',cut=True)
    # Complementary flat in the hole, shared by wheel and rotor.
    c=gc('V02_Rueda24_J4')
    box(c,'Chaveta_D','-3 mm','-4.5 mm','6 mm','6 mm','1.2 mm','9 mm')
    g=comp('04_Giro_muneca')
    c=sub(g,'V03_E03_Retencion_superior_J4')
    ring(c,'Casquillo','Z','15 mm','0 mm','0 mm','10.8 mm','8 mm','2 mm','steel')
    ring(c,'Arandela','Z','17 mm','0 mm','0 mm','14 mm','3.3 mm','2 mm','steel')
    screw(g,'V03_Tornillo_axial_J4','Z','12 mm','0 mm','0 mm','3 mm','7 mm')
    # Fixed support extension for a second 608, with a lower axial shoulder.
    c=gc('A12_Encastre_distal_antebrazo_PETG')
    ring(c,'Boss_608_inferior_J4','Z','link2-17 mm','0 mm','0 mm','46 mm','11.6 mm','8 mm')
    ring(c,'Asiento_segundo608','Z','link2-16 mm','0 mm','0 mm','b608_od+fit_clear',None,'7 mm',cut=True)
    for side in [-1,1]:
        px=f'36 mm+micro_l/2-micro_shaft_x+({side})*(micro_l/2+2 mm)'
        box(c,'Alza_oreja_motor_J4_'+str(side),f'({px})-2 mm','-8 mm' if side<0 else '-micro_w/2','link2-12 mm','4 mm','16 mm' if side<0 else 'micro_w','4 mm')
        ring(c,'Paso_alza_'+str(side),'Z','link2-13 mm',px,'0 mm','2.5 mm',None,'6 mm',cut=True)
    bearing(comp('03_Antebrazo_PVC'),'V03_R04_608_J4_inferior','Z','link2-16 mm','0 mm','0 mm')
    # Driver hub is bolted to its horn; no unsupported shaft stub.
    c=gc('V02_Pinon24_J4')
    ring(c,'Cubo_horn','Z','-4 mm','0 mm','0 mm','14 mm','4.8 mm','4 mm')

def stage_v03_j6():
    import re
    for cn in ['V02_M06_MG90S_J6','V02_M06_MG90S_J6_Cuna_PETG']:
        for p in gc(cn).modelParameters:
            e=p.expression;n=re.sub(r'wrist56\s*-\s*18\s*mm','wrist56-22.5 mm',e)
            if n!=e:p.expression=n
    c=gc('V02_A18_Nucleo_J5_y_columna_J6')
    c.features.extrudeFeatures.itemByName('Columna').extentOne.distance.expression='3.4 mm'
    ring(c,'Asiento_inferior_J6','Z','wrist56-14 mm','0 mm','0 mm','26 mm','8.6 mm','6 mm')
    ring(c,'Alojamiento625_adicional','Z','wrist56-13 mm','0 mm','0 mm','b625_od+fit_clear',None,'5 mm',cut=True)
    bearing(comp('05_Inclinacion_muneca'),'V03_R06_625_J6_inferior','Z','wrist56-13 mm','0 mm','0 mm','625')
    # Coupler is below both bearings; its neck touches only the inner race.
    c=gc('V02_E05_Acople_J6')
    c.constructionPlanes.itemByName('Plano_Brida').definition.offset.expression='-16.5 mm'
    c.features.extrudeFeatures.itemByName('Brida').extentOne.distance.expression='2 mm'
    c.constructionPlanes.itemByName('Plano_Eje_5').definition.offset.expression='-14.5 mm'
    c.features.extrudeFeatures.itemByName('Eje_5').extentOne.distance.expression='22.5 mm'
    ring(c,'Cuello_de_apoyo_interior','Z','-14.5 mm','0 mm','0 mm','7.8 mm','5 mm','1.5 mm','steel')
    # Keyed shaft and tool platform transmit torque, not just a coincident cylinder.
    box(c,'Plano_D_J6','-3 mm','-3 mm','0 mm','6 mm','1 mm','8 mm',cut=True)
    c=gc('V02_A20_Bastidor_pinza')
    box(c,'Llave_D_J6','-2 mm','-2.8 mm','0.5 mm','4 mm','0.8 mm','5 mm')

def stage_v03_shoulder_hubs():
    # Give the bearing caps room behind both shoulder gears and their motor horn.
    import re
    for cn in ['V02_Rueda40_Hombro','V02_Pinon20_Hombro']:
        c=gc(cn)
        for pl in ['Plano_Raiz_engranaje','Plano_evolvente_20grados']:
            c.constructionPlanes.itemByName(pl).definition.offset.expression='37 mm'
    for cn in ['V02_M02_GX3240_270','V02_A03_Housing_GX']:
        for p in gc(cn).modelParameters:
            e=p.expression;n=re.sub(r'47\s*mm','49 mm',e)
            if n!=e:p.expression=n
    c=gc('V02_A04_Apoyo_GX_fuera_del_pinon')
    for sk in c.sketches:
        for dd in sk.sketchDimensions:
            e=dd.parameter.expression
            if e=='11.5 mm':dd.parameter.expression='13.5 mm'
            elif e=='44 mm':dd.parameter.expression='46 mm'
    param('shaft_shoulder_l','96 mm')
    c=gc('V02_Rueda40_Hombro')
    ring(c,'Cubo_hasta_nucleo','Y','20 mm','0 mm','0 mm','16 mm','8 mm','17 mm')
    c=gc('V02_Pinon20_Hombro')
    ring(c,'Cubo_hasta_horn','Y','43 mm','0 mm','0 mm','14 mm','6 mm','2 mm')
    # The elbow flange is outside its new bearing cap.
    c=gc('V02_E02_Eje_y_brida_codo')
    c.constructionPlanes.itemByName('Plano_Brida').definition.offset.expression='37 mm'
    c.features.extrudeFeatures.itemByName('Brida').extentOne.distance.expression='3 mm'
    c=gc('V02_E04_Eje_y_brida_J5')
    c.constructionPlanes.itemByName('Plano_Brida_horn').definition.offset.expression='29.5 mm'
    c.features.extrudeFeatures.itemByName('Brida_horn').extentOne.distance.expression='2.5 mm'

def caps_y(parent_name,printed_name,z,kind,center,half,outer):
    g=comp(parent_name);body=gc(printed_name);w='b'+kind+'_w';diam='b'+kind+'_od';shaft='b'+kind+'_id'
    inner_d='11.6 mm' if kind=='608' else '8.6 mm'
    cap_t='4 mm';pitch='14 mm' if kind=='608' else '11 mm'
    screw_d='3 mm';hole='3.4 mm';head='5.5 mm';headhole='5.8 mm';headt='3 mm'
    # These lips become part of the stationary fork; outer retainers remain removable.
    for sign in [-1,1]:
        ipos=f'({center})-({half})' if sign>0 else f'-({center})+({w})/2'
        lip_len=f'({half})-({w})/2'
        ring(body,'Hombro_interior_'+kind+str(sign),'Y',ipos,'0 mm',z,outer,inner_d,lip_len)
        c=sub(g,'V03_Tapa_'+kind+'_'+printed_name+'_'+str(sign))
        disk=f'({center})+({half})' if sign>0 else f'-({center})-({half})-({cap_t})'
        step=f'({center})+({w})/2' if sign>0 else f'-({center})-({half})'
        ring(c,'Placa_retencion','Y',disk,'0 mm',z,outer,inner_d,cap_t)
        ring(c,'Pestana_retencion','Y',step,'0 mm',z,diam,inner_d,lip_len)
        unite(c,'Tapa_escalonada')
        for sx in [-1,1]:
            x=f'({sx})*({pitch})'
            # Through screws, with heads recessed flush on the outer side.
            front=f'({disk})+({cap_t})' if sign>0 else disk
            start=f'({disk})-10 mm' if sign>0 else f'({disk})'
            ring(c,'Paso_tornillo_'+str(sx),'Y',f'({disk})-1 mm',x,z,hole,None,f'({cap_t})+2 mm',cut=True)
            recess=f'({front})-({headt})-0.2 mm' if sign>0 else f'({front})-0.1 mm'
            ring(c,'Rebaje_cabeza_'+str(sx),'Y',recess,x,z,headhole,None,f'({headt})+0.4 mm',cut=True)
            ring(body,'Taladro_retencion_'+str(sign)+str(sx),'Y',f'({disk})-10 mm',x,z,hole,None,f'({cap_t})+20 mm',cut=True)
            ins=f'({center})-1 mm' if sign>0 else f'-({center})-3 mm'
            ring(body,'Asiento_inserto_'+str(sign)+str(sx),'Y',ins,x,z,'4.5 mm',None,'4 mm',cut=True)
            ic=sub(g,'V03_Inserto_M3_'+printed_name+str(sign)+str(sx))
            ring(ic,'Inserto_M3_REF','Y',ins,x,z,'4.5 mm','3 mm','4 mm','brass')
            bolt=sub(g,'V03_Tornillo_tapa_'+printed_name+str(sign)+str(sx))
            if sign>0:
                h=f'({front})-({headt})-0.2 mm';st=f'({h})-8 mm'
                ring(bolt,'Vastago','Y',st,x,z,screw_d,None,'8 mm','steel');ring(bolt,'Cabeza','Y',h,x,z,head,None,headt,'darksteel')
            else:
                ring(bolt,'Vastago','Y',f'({front})+({headt})+0.3 mm',x,z,screw_d,None,'8 mm','steel');ring(bolt,'Cabeza','Y',f'({front})+0.3 mm',x,z,head,None,headt,'darksteel')
            join_all(bolt,'Tornillo_REF')

def stage_v03_caps_hombro():caps_y('01_Giro_base','A01_Plataforma_giratoria_PETG','tower_h','608','fork_y','fork_t/2','38 mm')
def stage_v03_caps_codo():caps_y('02_Brazo_PVC','A06_Encastre_distal_brazo_PETG','link1','608','fork_y','fork_t/2','38 mm')
def stage_v03_caps_j5():caps_y('04_Giro_muneca','V02_A15_Horquilla_J5_PETG','wrist45','625','21 mm','4 mm','32 mm')

def stage_v03_cap_rims():
    # Enlarge the already-authored cap rims so counterbores have material outside them.
    for c in D.allComponents:
        if c.name.startswith('V03_Tapa_608_') and 'Placa_retencion' in [f.name for f in c.features.extrudeFeatures]:
            for dd in c.sketches.itemByName('Croquis_Placa_retencion').sketchDimensions:
                if dd.parameter.expression=='34 mm':dd.parameter.expression='38 mm'
        if c.name.startswith('V03_Tapa_625_V02_A15'):
            for dd in c.sketches.itemByName('Croquis_Placa_retencion').sketchDimensions:
                if dd.parameter.expression=='28 mm':dd.parameter.expression='32 mm'

def stage_v03_caps_j4():
    g=comp('03_Antebrazo_PVC');fixed=gc('A12_Encastre_distal_antebrazo_PETG')
    c=sub(g,'V03_Tapa_608_J4')
    ring(c,'Placa','Z','link2','0 mm','0 mm','38 mm','11.6 mm','4 mm')
    ring(c,'Pestana','Z','link2-1 mm','0 mm','0 mm','22 mm','11.6 mm','1 mm');unite(c,'Tapa_J4')
    for side in [-1,1]:
        xx=f'({side})*15 mm'
        ring(c,'Paso_'+str(side),'Z','link2-1 mm',xx,'0 mm','3.4 mm',None,'6 mm',cut=True)
        ring(c,'Rebaje_'+str(side),'Z','link2+0.6 mm',xx,'0 mm','5.8 mm',None,'4 mm',cut=True)
        ring(fixed,'Paso_retencion_J4_'+str(side),'Z','link2-8 mm',xx,'0 mm','3.4 mm',None,'9 mm',cut=True)
        screw(g,'V03_Tornillo_tapa_J4_'+str(side),'Z','link2-7.4 mm',xx,'0 mm','3 mm','8 mm')
        ring(fixed,'Asiento_inserto_J4_'+str(side),'Z','link2-5 mm',xx,'0 mm','4.5 mm',None,'4 mm',cut=True)
        ic=sub(g,'V03_Inserto_M3_J4_'+str(side));ring(ic,'Inserto','Z','link2-5 mm',xx,'0 mm','4.5 mm','3 mm','4 mm','brass')
    c=gc('V02_A15_Horquilla_J5_PETG')
    ring(c,'Caja_tornillo_axial_J4','Z','15 mm','0 mm','0 mm','16 mm',None,'8 mm',cut=True)

def stage_v03_caps_j6():
    g=comp('05_Inclinacion_muneca');fixed=gc('V02_A18_Nucleo_J5_y_columna_J6')
    c=sub(g,'V03_Tapa_625_J6')
    ring(c,'Placa','Z','wrist56','0 mm','0 mm','28 mm','8.6 mm','3 mm')
    ring(c,'Pestana','Z','wrist56-2 mm','0 mm','0 mm','16 mm','8.6 mm','2 mm');unite(c,'Tapa_J6')
    for side in [-1,1]:
        xx=f'({side})*10 mm'
        ring(c,'Paso_'+str(side),'Z','wrist56-1 mm',xx,'0 mm','2.4 mm',None,'5 mm',cut=True)
        ring(c,'Rebaje_'+str(side),'Z','wrist56+0.6 mm',xx,'0 mm','4.2 mm',None,'3 mm',cut=True)
        ring(fixed,'Paso_retencion_J6_'+str(side),'Z','wrist56-8 mm',xx,'0 mm','2.4 mm',None,'9 mm',cut=True)
        screw(g,'V03_Tornillo_tapa_J6_'+str(side),'Z','wrist56-7.4 mm',xx,'0 mm','2 mm','8 mm','3.8 mm','2 mm')
        ring(fixed,'Asiento_inserto_J6_'+str(side),'Z','wrist56-4 mm',xx,'0 mm','3.5 mm',None,'3 mm',cut=True)
        ic=sub(g,'V03_Inserto_M2_J6_'+str(side));ring(ic,'Inserto_REF','Z','wrist56-4 mm',xx,'0 mm','3.5 mm','2 mm','3 mm','brass')
    c=gc('V02_A20_Bastidor_pinza')
    c.constructionPlanes.itemByName('Plano_Plataforma_J6').definition.offset.expression='4 mm'
    for side in [-1,1]:
        c.constructionPlanes.itemByName('Plano_Columna_'+str(side)).definition.offset.expression='9 mm'
        c.features.extrudeFeatures.itemByName('Columna_'+str(side)).extentOne.distance.expression='grip_deck-9 mm'
    c.features.extrudeFeatures.itemByName('Llave_D_J6').extentOne.distance.expression='8.5 mm'
    ring(c,'Paso_prisionero_J6','Y','-16 mm','0 mm','6 mm','2.5 mm',None,'14 mm',cut=True)
    steel=gc('V02_E05_Acople_J6')
    steel.features.extrudeFeatures.itemByName('Eje_5').extentOne.distance.expression='23.5 mm'
    steel.features.extrudeFeatures.itemByName('Plano_D_J6').extentOne.distance.expression='9 mm'
    c=sub(comp('06_Herramienta'),'V03_Prisionero_J6_M2_5x12_REF')
    ring(c,'Prisionero','Y','-14 mm','0 mm','6 mm','2.5 mm',None,'12 mm','steel')

def stage_v03_newbase():
    R.joints.itemByName('J1_Giro_base').deleteMe();occ('00_Base_control').deleteMe()
    main('00_Base_control');occ('00_Base_control').isGrounded=True;g=comp('00_Base_control')
    c=sub(g,'V03_Plataforma_sandwich_superior')
    box(c,'Plataforma','-96 mm','-31 mm','agv_mount_z','172 mm','63 mm','agv_mount_t')
    box(c,'Ventana_voltimetro_auto','-68 mm','-9 mm','agv_mount_z-1 mm','37 mm','18 mm','agv_mount_t+2 mm',cut=True)
    for label,x,y in HOLES:ring(c,'Agujero_existente_'+label,'Z','agv_mount_z-1 mm','agv_'+label+'_x','agv_'+label+'_y','3.4 mm',None,'agv_mount_t+2 mm',cut=True)
    ring(c,'Paso_M10','Z','agv_mount_z-1 mm','agv_center_x','agv_center_y','10.5 mm',None,'agv_mount_t+2 mm',cut=True)
    c=sub(g,'V03_Contraplaca_aluminio_3mm')
    box(c,'Contraplaca','-18 mm','-31 mm','-agv_upper_t-agv_back_t','94 mm','63 mm','agv_back_t','steel')
    for label,x,y in HOLES:ring(c,'Paso_M3_'+label,'Z','-7 mm','agv_'+label+'_x','agv_'+label+'_y','3.4 mm',None,'5 mm',cut=True)
    ring(c,'Paso_M10','Z','-7 mm','agv_center_x','agv_center_y','10.5 mm',None,'5 mm',cut=True)
    c=sub(g,'V03_Casquillo_central_compresion')
    ring(c,'Casquillo','Z','-agv_upper_t','agv_center_x','agv_center_y','24 mm','10.5 mm','agv_mount_z+agv_upper_t','steel')
    for label,x,y in HOLES:
        c=sub(g,'V03_Separador_M3_'+label);ring(c,'Separador','Z','0 mm','agv_'+label+'_x','agv_'+label+'_y','7 mm','3.4 mm','agv_mount_z','steel')
        screw(g,'V03_Anclaje_M3x40_'+label,'Z','agv_mount_z+agv_mount_t-40 mm','agv_'+label+'_x','agv_'+label+'_y','3 mm','40 mm')
        c=sub(g,'V03_Tuerca_arandela_M3_'+label)
        ring(c,'Arandela','Z','-7 mm','agv_'+label+'_x','agv_'+label+'_y','7 mm','3.4 mm','1 mm','steel')
        ring(c,'Tuerca_REF','Z','-9.4 mm','agv_'+label+'_x','agv_'+label+'_y','6.3 mm','3.4 mm','2.4 mm','steel')
    screw(g,'V03_Anclaje_M10x45','Z','agv_mount_z+agv_mount_t-45 mm','agv_center_x','agv_center_y','10 mm','45 mm','17 mm','7 mm')
    c=sub(g,'V03_Tuerca_arandela_M10')
    ring(c,'Arandela','Z','-7.5 mm','agv_center_x','agv_center_y','30 mm','10.5 mm','1.5 mm','steel')
    ring(c,'Tuerca_REF','Z','-15.5 mm','agv_center_x','agv_center_y','19 mm','10.5 mm','8 mm','steel')
    origin(g,'J1_BASE','0 mm','0 mm','base_top')

def stage_v03_j1_support():
    g=comp('00_Base_control');c=gc('V03_Plataforma_sandwich_superior')
    for sx in [-1,1]:
        for sy in [-1,1]:box(c,'Columna_J1_'+str(sx)+str(sy),f'({sx})*22 mm-3 mm',f'({sy})*14 mm-3 mm','agv_mount_z+agv_mount_t','6 mm','6 mm','82 mm-agv_mount_z-agv_mount_t')
    box(c,'Puente_caja_rodamientos','-25 mm','-25 mm','82 mm','50 mm','50 mm','8 mm')
    ring(c,'Torre_608','Z','88 mm','0 mm','0 mm','40 mm','11.6 mm','18 mm')
    ring(c,'Paso_puente_J1','Z','81 mm','0 mm','0 mm','11.6 mm',None,'10 mm',cut=True)
    ring(c,'Caja_brida_y_horn_J1','Z','81 mm','0 mm','0 mm','24 mm',None,'7.5 mm',cut=True)
    for z in ['90 mm','98 mm']:ring(c,'Asiento_J1_'+z,'Z',z,'0 mm','0 mm','b608_od+fit_clear',None,'7 mm',cut=True)
    servo(g,'V03_M01_MG946_Base','mg','Z','80 mm')
    for side in [-1,1]:
        x=f'mg_l/2-std_shaft_x+({side})*(mg_l/2+std_ear/2)'
        box(c,'Poste_pestana_J1_'+str(side),f'({x})-std_ear/2','-mg_w/2','agv_mount_z+agv_mount_t','std_ear','mg_w','72 mm-agv_mount_z-agv_mount_t')
        ring(c,'Paso_pestana_J1_'+str(side),'Z','70 mm',x,'0 mm','4.2 mm',None,'7 mm',cut=True)
        ring(c,'Alojamiento_inserto_servo_'+str(side),'Z','66 mm',x,'0 mm','4.5 mm',None,'4 mm',cut=True)
        ic=sub(g,'V03_Inserto_M3_servo_J1_'+str(side));ring(ic,'Inserto','Z','66 mm',x,'0 mm','4.5 mm','3 mm','4 mm','brass')
        screw(g,'V03_Fijacion_servo_J1_'+str(side),'Z','62 mm',x,'0 mm','3 mm','12.5 mm')
    bearing(g,'V03_R01_608_J1_inferior','Z','90 mm','0 mm','0 mm')
    bearing(g,'V03_R01_608_J1_superior','Z','98 mm','0 mm','0 mm')
    c=sub(g,'V03_Tapa_rodamientos_J1')
    ring(c,'Placa','Z','106 mm','0 mm','0 mm','40 mm','11.6 mm','4 mm')
    ring(c,'Pestana','Z','105 mm','0 mm','0 mm','22 mm','11.6 mm','1 mm');unite(c,'Tapa_J1')
    for side in [-1,1]:
        ring(c,'Paso_'+str(side),'Z','105 mm',f'({side})*16 mm','0 mm','3.4 mm',None,'6 mm',cut=True)
        ring(c,'Caja_cabeza_'+str(side),'Z','106.8 mm',f'({side})*16 mm','0 mm','5.8 mm',None,'4 mm',cut=True)
        ring(gc('V03_Plataforma_sandwich_superior'),'Paso_tapa_J1_'+str(side),'Z','98 mm',f'({side})*16 mm','0 mm','3.4 mm',None,'9 mm',cut=True)
        screw(g,'V03_Tornillo_tapa_J1_'+str(side),'Z','98.8 mm',f'({side})*16 mm','0 mm','3 mm','8 mm')
        ring(gc('V03_Plataforma_sandwich_superior'),'Asiento_inserto_J1_'+str(side),'Z','101 mm',f'({side})*16 mm','0 mm','4.5 mm',None,'4 mm',cut=True)
        ic=sub(g,'V03_Inserto_M3_J1_'+str(side));ring(ic,'Inserto','Z','101 mm',f'({side})*16 mm','0 mm','4.5 mm','3 mm','4 mm','brass')

def stage_v03_j1_output():
    g=comp('01_Giro_base');drop_child('01_Giro_base',['V02_E00_Acople_base_con_608'])
    c=sub(g,'V03_E00_Husillo_base_completo')
    ring(c,'Brida','Z','-26 mm','0 mm','0 mm','22 mm','2.5 mm','2 mm','steel')
    ring(c,'Eje','Z','-24 mm','0 mm','0 mm','8 mm',None,'32 mm','steel')
    for z,l in [('-24 mm','2 mm'),('-15 mm','1 mm'),('-7 mm','3 mm')]:ring(c,'Apoyo_race_'+z,'Z',z,'0 mm','0 mm','10.8 mm','8 mm',l,'steel')
    join_all(c,'Husillo_metalico_una_pieza')
    box(c,'Plano_D','-5 mm','-5 mm','-1 mm','10 mm','1.7 mm','10 mm',cut=True)
    ring(c,'Rosca_axial_REF','Z','3 mm','0 mm','0 mm','3.3 mm',None,'6 mm',cut=True)
    c=gc('A01_Plataforma_giratoria_PETG')
    ring(c,'Cubo_D','Z','-1 mm','0 mm','0 mm','18 mm','8 mm','9 mm')
    box(c,'Plano_complementario_D','-3 mm','-4.5 mm','-1 mm','6 mm','1.2 mm','9 mm')
    ring(c,'Caja_retencion_axial','Z','7.8 mm','0 mm','0 mm','16 mm',None,'6.5 mm',cut=True)
    c=sub(g,'V03_Retencion_axial_J1');ring(c,'Arandela','Z','8 mm','0 mm','0 mm','14 mm','3.3 mm','2 mm','steel')
    screw(g,'V03_Tornillo_retencion_J1','Z','3 mm','0 mm','0 mm','3 mm','7 mm')
    j=make_joint('J1_Giro_base','01_Giro_base','J1_GIRO','00_Base_control','J1_BASE',angle='0 deg',limits=(-90,90))
    j.jointMotion.rotationValue=math.radians(20)
    if D.snapshots.hasPendingSnapshot:D.snapshots.add()

def stage_v03_bucks():
    param('electronics_z','agv_mount_z+agv_mount_t+3 mm')
    g=comp('00_Base_control');frame=gc('V03_Plataforma_sandwich_superior')
    # Three boards vertical, with components facing left; no invisible attachment.
    for i in range(3):
        x=f'-25 mm-({i})*electronics_pitch'
        c=sub(g,'V03_XL4016_vertical_'+str(i+1))
        box(c,'PCB',f'({x})-pcb_t','-buck_w/2','electronics_z','pcb_t','buck_w','buck_l','blue')
        box(c,'Disipador',f'({x})-pcb_t-18 mm','-15 mm','electronics_z+7 mm','18 mm','30 mm','17 mm','steel')
        ring(c,'Inductor','Y','-9 mm',f'({x})-pcb_t-8 mm','electronics_z+43 mm','16 mm','7 mm','18 mm','brass')
        box(c,'Bornera',f'({x})-pcb_t-8 mm','-22 mm','electronics_z+3 mm','8 mm','8 mm','10 mm','blue')
        # Bottom and top edge clips, attached to two continuous side walls.
        for sy in [-1,1]:
            yy='24.4 mm' if sy>0 else '-28 mm'
            box(frame,'Montante_XL_'+str(i)+str(sy),f'({x})-5 mm',yy,'agv_mount_z+agv_mount_t','7 mm','3.6 mm','electronics_z+buck_l+2 mm-agv_mount_z-agv_mount_t')
            clipy='18 mm' if sy>0 else '-25 mm'
            box(frame,'Labio_inferior_XL_'+str(i)+str(sy),f'({x})-4 mm',clipy,'agv_mount_z+agv_mount_t','6 mm','7 mm','electronics_z-agv_mount_z-agv_mount_t')
            box(frame,'Labio_superior_XL_'+str(i)+str(sy),f'({x})-4 mm',clipy,'electronics_z+buck_l','6 mm','7 mm','2 mm')
    # Side beams join all cartridge posts back into the mounting plate.
    for sy in [-1,1]:box(frame,'Travesano_rack_'+str(sy),'-94 mm',f'({sy})*26.2 mm-1.8 mm','28 mm','76 mm','3.6 mm','5 mm')

def stage_v03_logic():
    g=comp('00_Base_control');frame=gc('V03_Plataforma_sandwich_superior')
    for name,x,L,W,z in [('PCA9685','45 mm','70 mm','25 mm','29 mm'),('Nano','64 mm','45 mm','18 mm','31 mm')]:
        c=sub(g,'V03_'+name+'_vertical')
        box(c,'PCB',x,f'-({W})/2',z,'pcb_t',W,L,'pcb')
        box(c,'Conectores',f'({x})+pcb_t',f'-({W})/2+2 mm',f'({z})+3 mm','6 mm',f'({W})-4 mm',f'({L})-6 mm','servo')
        if name=='Nano':box(c,'USB',f'({x})+pcb_t','-4 mm',z,'4 mm','8 mm','8 mm','steel')
        for sy in [-1,1]:
            yy=f'({sy})*(({W})/2+0.4 mm)' if sy>0 else f'-({W})/2-4 mm'
            box(frame,'Montante_'+name+str(sy),f'({x})-2 mm',yy,'agv_mount_z+agv_mount_t','6 mm','3.6 mm',f'({z})+({L})+2 mm-agv_mount_z-agv_mount_t')
            box(frame,'Asiento_'+name+str(sy),f'({x})-1 mm',f'({sy})*(({W})/2-2 mm)-2 mm',f'({z})-0.5 mm','4 mm','4 mm','0.5 mm')
            box(frame,'Retencion_'+name+str(sy),f'({x})-1 mm',f'({sy})*(({W})/2-2 mm)-2 mm',f'({z})+({L})','4 mm','4 mm','1.5 mm')
    # Power distribution reference; source remains external, no electrical rewiring.
    c=sub(g,'V03_Bornera_y_alimentacion_REF')
    box(c,'Bornera','49 mm','17 mm','28 mm','20 mm','10 mm','8 mm','blue')

def stage_v03_reference_car():
    o=R.occurrences.addNewComponent(adsk.core.Matrix3D.create());o.component.name='80_Auto_referencia';o.isGrounded=True
    g=o.component;palette={}
    for name in sorted(set(p['component'] for p in builtins._afr2_car_v03)):
        c=sub(g,'REF_AUTO_'+name);bf=c.features.baseFeatures.add();bf.startEdit();pairs=[]
        for item in builtins._afr2_car_v03:
            if item['component']!=name:continue
            body=c.bRepBodies.add(item['body'],bf);body.name=item['name'];pairs.append((body,item['appearance']))
        bf.finishEdit()
        for b,ap in pairs:
            if not ap:continue
            key=ap.name
            if key not in palette:
                try:palette[key]=D.appearances.addByCopy(ap,'AUTO_'+key)
                except:palette[key]=AP['darksteel']
            b.appearance=palette[key]
    g.attributes.add('AFR2','source','AutoFinalRobo2 activo 27/10/2026, solidos visibles; transformacion X,-Z-32,Y-26. Referencia, sin modificar maestro del auto.')

def stage_v03_reference_appearance():
    # BaseFeature result bodies must receive their appearance after finishEdit.
    rows=[]
    def bbox(b):return list(b.boundingBox.minPoint.asArray())+list(b.boundingBox.maxPoint.asArray())
    for name in sorted(set(p['component'] for p in builtins._afr2_car_v03)):
        c=gc('REF_AUTO_'+name);remaining=[(b,bbox(b)) for b in c.bRepBodies]
        for item in [p for p in builtins._afr2_car_v03 if p['component']==name]:
            sourcebox=bbox(item['body'])
            idx=next((i for i,(b,bb) in enumerate(remaining) if max(abs(x-y) for x,y in zip(sourcebox,bb))<1e-6),None)
            if idx is None:raise RuntimeError('Copia sin correspondencia geometrica: '+name+'/'+item['name'])
            b,bb=remaining.pop(idx);b.name=item['name'];ap=item['appearance']
            if ap:
                target=D.appearances.itemByName('AUTO_'+ap.name)
                if not target:target=D.appearances.addByCopy(ap,'AUTO_'+ap.name)
                b.appearance=target
            rows.append({'component':c.name,'body':b.name,'source_body':item['name'],'appearance':b.appearance.name if b.appearance else None,'bbox_error_cm':max(abs(x-y) for x,y in zip(sourcebox,bb))})
        if remaining:raise RuntimeError('Cuerpos adicionales en copia: '+c.name)
    with open(os.path.join(REPORT,'referencia_auto_apariencias.json'),'w',encoding='utf-8') as ff:json.dump(rows,ff,ensure_ascii=False,indent=2)

def stage_v03_mount_joint():
    base=occ('00_Base_control');base.isGrounded=False;base.isGroundToParent=False
    origin(base.component,'AGV_MONTURA')
    origin(comp('80_Auto_referencia'),'AGV_CHASIS_SUPERIOR')
    make_joint('J0_Montura_rigida_sobre_AGV','00_Base_control','AGV_MONTURA','80_Auto_referencia','AGV_CHASIS_SUPERIOR',kind='rigid')
    if D.snapshots.hasPendingSnapshot:D.snapshots.add()

def stage_v03_unify():
    for gn in globals().get('GROUPS',['00_Base_control','01_Giro_base','02_Brazo_PVC','03_Antebrazo_PVC','04_Giro_muneca','05_Inclinacion_muneca','06_Herramienta']):
        g=comp(gn);count=0
        # Merge only the primary printed load path and new bridges. Removable caps stay separate.
        while True:
            bodies=[b for o in g.occurrences if not o.component.name.startswith('V03_Tapa_') for b in o.component.bRepBodies if b.volume>1e-10 and b.appearance and b.appearance.name=='AFR2_PETG_Rojo']
            merged=False;tm=adsk.fusion.TemporaryBRepManager.get()
            for i,a in enumerate(bodies):
                for b in bodies[i+1:]:
                    cp=tm.copy(a)
                    try:ok=tm.booleanOperation(cp,b,adsk.fusion.BooleanTypes.UnionBooleanType)
                    except:continue
                    if not ok or cp.lumps.count!=1:continue
                    es=adsk.core.ObjectCollection.create();es.add(b)
                    ip=a.parentComponent.features.combineFeatures.createInput(a,es);ip.operation=OP.JoinFeatureOperation
                    a.parentComponent.features.combineFeatures.add(ip).name='V03_Continuidad_impreso'
                    merged=True;count+=1;break
                if merged:break
            if not merged:break
        print('UNIFICADO',gn,count)

def stage_v03_horns():
    # Put each horn on its rotating output, preserving the fixed servo case.
    configs=[('V03_M01_MG946_Base','01_Giro_base','Z','-28 mm','22 mm','6 mm','8 mm'),
      ('V02_M02_GX3240_270','09_Pinion_hombro','Y','43 mm','22 mm','6 mm','8 mm'),
      ('V02_M03_DS3218_Codo','03_Antebrazo_PVC','Y','40 mm','22 mm','6 mm','8 mm'),
      ('V02_M04_MG90S_J4','11_Pinion_J4','Z','-2 mm','16 mm','4.8 mm','7.2 mm'),
      ('V02_M05_MG946_J5','05_Inclinacion_muneca','Y','32 mm','22 mm','6 mm','8 mm'),
      ('V02_M06_MG90S_J6','06_Herramienta','Z','-18.5 mm','16 mm','4.8 mm','7.2 mm'),
      ('V02_M07_MG90S_Pinza','10_Pinion_pinza','Z','-2 mm','16 mm','4.8 mm','7.2 mm')]
    for motor,group,axis,start,od,idd,neck in configs:
        gc(motor).features.extrudeFeatures.itemByName('Horn_REFERENCIA').isSuppressed=True
        c=sub(comp(group),'V03_Horn_movil_'+motor)
        ring(c,'Plato_horn_REF',axis,start,'0 mm','0 mm',od,'2.4 mm' if 'MG90' in motor else '3.3 mm','2 mm','white')
        ns=f'({start})-2 mm' if axis=='Z' else f'({start})+2 mm'
        ring(c,'Cubo_estriado_REF',axis,ns,'0 mm','0 mm',neck,idd,'2 mm','white')
        join_all(c,'Horn_comercial_REF')
    for cn,feature in [('V02_Pinon24_J4','Cubo_horn'),('V02_Pinon20_Hombro','Cubo_hasta_horn')]:
        gc(cn).features.extrudeFeatures.itemByName(feature).isSuppressed=True

def stage_v03_small_drive_bolts():
    for group,gear,motor in [('11_Pinion_J4','V02_Pinon24_J4','V02_M04_MG90S_J4'),('10_Pinion_pinza','V02_Pinon24_Pinza','V02_M07_MG90S_Pinza')]:
        g=comp(group);plastic=gc(gear);horn=gc('V03_Horn_movil_'+motor)
        for sx in [-1,1]:
            x=f'({sx})*5.5 mm'
            ring(plastic,'Paso_horn_'+str(sx),'Z','-1 mm',x,'0 mm','2.4 mm',None,'6 mm',cut=True)
            ring(horn,'Paso_brazo_'+str(sx),'Z','-3 mm',x,'0 mm','2.4 mm',None,'4 mm',cut=True)
            screw(g,'V03_M2x8_union_horn_'+str(sx),'Z','-4 mm',x,'0 mm','2 mm','8 mm','3.8 mm','2 mm')
            c=sub(g,'V03_Tuerca_M2_horn_'+str(sx));ring(c,'Tuerca_REF','Z','-3.6 mm',x,'0 mm','4.6 mm','2 mm','1.6 mm','steel')

def stage_v03_flange_bolts():
    for group,steel,motor,axis,front,screwlen in [
      ('01_Giro_base','V03_E00_Husillo_base_completo','V03_M01_MG946_Base','Z','-28 mm','4 mm'),
      ('03_Antebrazo_PVC','V02_E02_Eje_y_brida_codo','V02_M03_DS3218_Codo','Y','42 mm','4 mm'),
      ('05_Inclinacion_muneca','V02_E04_Eje_y_brida_J5','V02_M05_MG946_J5','Y','34 mm','4 mm'),
      ('06_Herramienta','V02_E05_Acople_J6','V02_M06_MG90S_J6','Z','-18.5 mm','4 mm')]:
        g=comp(group);s=gc(steel);horn=gc('V03_Horn_movil_'+motor)
        micro=motor=='V02_M06_MG90S_J6';pitch='5.5 mm' if micro else '7.5 mm';dia='2 mm' if micro else '2.5 mm'
        for sx in [-1,1]:
            x=f'({sx})*({pitch})'
            start=f'({front})-({screwlen})' if axis=='Y' else front
            ring(s,'Rosca_horn_REF_'+str(sx),axis,start,x,'0 mm',dia,None,screwlen,cut=True)
            ring(horn,'Paso_union_'+str(sx),axis,f'({front})-3 mm' if axis=='Y' else f'({front})-1 mm',x,'0 mm','2.4 mm' if micro else '2.9 mm',None,'5 mm',cut=True)
            c=sub(g,'V03_Tornillo_union_'+motor+str(sx))
            ring(c,'Vastago',axis,start,x,'0 mm',dia,None,screwlen,'steel')
            h=front if axis=='Y' else f'({front})-2.5 mm'
            ring(c,'Cabeza',axis,h,x,'0 mm','4.5 mm' if not micro else '3.8 mm',None,'2.5 mm','steel');join_all(c,'Tornillo_REF')
    # Shoulder gear: recessed heads do not pass through the stationary bearing cap.
    g=comp('09_Pinion_hombro');gear=gc('V02_Pinon20_Hombro');horn=gc('V03_Horn_movil_V02_M02_GX3240_270')
    for sx in [-1,1]:
        x=f'({sx})*7.5 mm'
        ring(gear,'Paso_union_'+str(sx),'Y','36 mm',x,'0 mm','2.9 mm',None,'8 mm',cut=True)
        ring(gear,'Rebaje_tornillo_'+str(sx),'Y','36.9 mm',x,'0 mm','4.8 mm',None,'2.8 mm',cut=True)
        ring(horn,'Paso_horn_'+str(sx),'Y','42 mm',x,'0 mm','2.9 mm',None,'4 mm',cut=True)
        c=sub(g,'V03_Tornillo_M2_5_pinon_'+str(sx));ring(c,'Cabeza','Y','37.2 mm',x,'0 mm','4.5 mm',None,'2.5 mm','steel');ring(c,'Vastago','Y','39.7 mm',x,'0 mm','2.5 mm',None,'8 mm','steel');join_all(c,'Tornillo_REF')
        c=sub(g,'V03_Tuerca_M2_5_pinon_'+str(sx));ring(c,'Tuerca_REF','Y','45 mm',x,'0 mm','5.5 mm','2.5 mm','2 mm','steel')

def stage_v03_join_shafts():
    gc('V03_E00_Husillo_base_completo').features.extrudeFeatures.itemByName('Apoyo_race_-7 mm').extentOne.distance.expression='6 mm'
    c=gc('V02_E03_Eje_J4')
    ring(c,'Hombro_inferior_del_eje','Z','-18 mm','0 mm','0 mm','10.8 mm','8 mm','2 mm','steel')
    for cn in ['E01_Eje_hombro_8mm','V02_E02_Eje_y_brida_codo','V02_E03_Eje_J4','V02_E04_Eje_y_brida_J5','V02_E05_Acople_J6','V03_E00_Husillo_base_completo']:
        unite(gc(cn),'V03_Eje_continuo_una_pieza')

def stage_v03_keys_and_axial_stack():
    # The shoulder wheel must be separate from the nucleus across the bearing fork.
    f=gc('V02_Rueda40_Hombro').features.extrudeFeatures.itemByName('Cubo_hasta_nucleo')
    if f:f.isSuppressed=True
    for shaft,nucleus,rad,flat,width in [('E01_Eje_hombro_8mm','A05_Encastre_proximal_brazo_PETG','4 mm','3.3 mm','38 mm'),('V02_E02_Eje_y_brida_codo','A11_Encastre_proximal_antebrazo_PETG','4 mm','3.3 mm','38 mm'),('V02_E04_Eje_y_brida_J5','V02_A18_Nucleo_J5_y_columna_J6','2.5 mm','2 mm','30 mm')]:
        c=gc(shaft)
        box(c,'Cara_D_nucleo','-5 mm',f'-({width})/2',flat,'10 mm',width,'2 mm',cut=True)
        c=gc(nucleus)
        box(c,'Llave_D_nucleo','-2 mm' if rad=='2.5 mm' else '-3 mm',f'-({width})/2',flat,'4 mm' if rad=='2.5 mm' else '6 mm',width,'0.9 mm' if rad=='2.5 mm' else '1.2 mm')
    c=gc('E01_Eje_hombro_8mm');box(c,'Cara_D_rueda','-5 mm','37 mm','3.3 mm','10 mm','11 mm','2 mm',cut=True)
    ring(c,'Rosca_retencion_axial_REF','Y','43 mm','0 mm','0 mm','3 mm',None,'6 mm',cut=True)
    c=gc('V02_Rueda40_Hombro');box(c,'Llave_D_rueda','-3 mm','37 mm','3.3 mm','6 mm','6 mm','1.2 mm')
    c=gc('V02_E02_Eje_y_brida_codo')
    c.constructionPlanes.itemByName('Plano_Eje').definition.offset.expression='-46 mm'
    c.features.extrudeFeatures.itemByName('Eje').extentOne.distance.expression='86 mm'
    for group,shaft in [('02_Brazo_PVC','E01_Eje_hombro_8mm'),('03_Antebrazo_PVC','V02_E02_Eje_y_brida_codo')]:
        g=comp(group);s=gc(shaft)
        box(s,'Cara_plana_collar','-5 mm','-46 mm','3.3 mm','10 mm','10 mm','2 mm',cut=True)
        c=sub(g,'V03_Collar_8mm_retencion')
        ring(c,'Collar','Y','-45 mm','0 mm','0 mm','16 mm','8 mm','8 mm','steel')
        ring(c,'Rosca_prisionero_REF','Z','3.3 mm','0 mm','-41 mm','3 mm',None,'5.3 mm',cut=True)
        c=sub(g,'V03_Prisionero_M3x5_collar');ring(c,'Prisionero','Z','3.3 mm','0 mm','-41 mm','3 mm',None,'5 mm','steel')
        c=sub(g,'V03_Separador_8mm_trasero');ring(c,'Separador','Y','-37 mm','0 mm','0 mm','10.8 mm','8 mm','5.5 mm','steel')
        c=sub(g,'V03_Separador_8mm_delantero');ring(c,'Separador','Y','31.5 mm','0 mm','0 mm','10.8 mm','8 mm','5.5 mm','steel')
    g=comp('02_Brazo_PVC')
    c=sub(g,'V03_Retencion_rueda_hombro');ring(c,'Separador_frontal','Y','43 mm','0 mm','0 mm','10.8 mm','8 mm','5 mm','steel');ring(c,'Arandela','Y','48 mm','0 mm','0 mm','16 mm','3.3 mm','2 mm','steel')
    screw(g,'V03_Tornillo_axial_hombro','Y','44 mm','0 mm','0 mm','3 mm','6 mm')
    for group,name,axis,z,od,idd,length in [('04_Giro_muneca','V03_Separador_axial_J4','Z','-1 mm','10.8 mm','8 mm','7 mm'),('06_Herramienta','V03_Separador_medio_J6','Z','-8 mm','7.8 mm','5 mm','1 mm'),('06_Herramienta','V03_Separador_axial_J6','Z','-2 mm','7.8 mm','5 mm','6 mm'),('05_Inclinacion_muneca','V03_Separador_axial_J5','Y','23.5 mm','7.8 mm','5 mm','3.5 mm')]:
        c=sub(comp(group),name);ring(c,'Separador',axis,z,'0 mm','0 mm',od,idd,length,'steel')

def stage_v03_servo_retention():
    # Original horn screws are installed before the flange couplers are bolted on.
    configs=[('V03_M01_MG946_Base','01_Giro_base','Z','80 mm','0 mm','0 mm','-26 mm',True),
      ('V02_M02_GX3240_270','09_Pinion_hombro','Y','45 mm','0 mm','tower_h-gear_centers','43 mm',True),
      ('V02_M03_DS3218_Codo','03_Antebrazo_PVC','Y','42 mm','0 mm','link1','40 mm',True),
      ('V02_M04_MG90S_J4','11_Pinion_J4','Z','link2','36 mm','0 mm','0 mm',False),
      ('V02_M05_MG946_J5','05_Inclinacion_muneca','Y','34 mm','0 mm','wrist45','32 mm',True),
      ('V02_M06_MG90S_J6','06_Herramienta','Z','wrist56-22.5 mm','0 mm','0 mm','-16.5 mm',False),
      ('V02_M07_MG90S_Pinza','10_Pinion_pinza','Z','grip_deck+2 mm','0 mm','0 mm','0 mm',False)]
    for motor,group,axis,at,u,v,front,std in configs:
        dia='3 mm' if std else '2 mm';head='5.5 mm' if std else '3.8 mm';h='3 mm' if std else '2 mm'
        ring(gc(motor),'Taladro_central_salida_REF',axis,at,u,v,dia,None,'4 mm',cut=True)
        c=sub(comp(group),'V03_Tornillo_original_horn_'+motor)
        start=front if axis=='Y' else f'({front})-6 mm'
        hs=f'({front})-({h})' if axis=='Y' else front
        ring(c,'Vastago',axis,start,'0 mm','0 mm',dia,None,'6 mm','steel');ring(c,'Cabeza',axis,hs,'0 mm','0 mm',head,None,h,'steel');join_all(c,'Tornillo_horn_REF')
    for cn,axis,a,L in [('V03_E00_Husillo_base_completo','Z','-26.2 mm','3.4 mm'),('V02_E02_Eje_y_brida_codo','Y','36.8 mm','3.4 mm'),('V02_E05_Acople_J6','Z','-16.7 mm','2.4 mm')]:
        ring(gc(cn),'Caja_cabeza_horn_original',axis,a,'0 mm','0 mm','6 mm' if cn!='V02_E05_Acople_J6' else '4.2 mm',None,L,cut=True)
    # J5 has a smaller shaft; a local collar keeps its blind screw pocket connected.
    c=gc('V02_E04_Eje_y_brida_J5');ring(c,'Collar_cabeza_horn','Y','27 mm','0 mm','0 mm','10 mm','5 mm','2.5 mm','steel')
    ring(c,'Caja_cabeza_horn_original','Y','28.8 mm','0 mm','0 mm','6 mm',None,'3.4 mm',cut=True)
    for c in D.allComponents:
        if c.name.startswith('V03_Tapa_625_V02_A15'):
            for sn in ['Croquis_Placa_retencion','Croquis_Pestana_retencion']:
                for dd in c.sketches.itemByName(sn).sketchDimensions:
                    if dd.parameter.expression=='8.6 mm':dd.parameter.expression='10.5 mm'

def stage_v03_reference_refinements():
    for cn in ['V03_M01_MG946_Base','V02_M02_GX3240_270','V02_M03_DS3218_Codo','V02_M04_MG90S_J4','V02_M05_MG946_J5','V02_M06_MG90S_J6','V02_M07_MG90S_Pinza']:
        c=gc(cn);case=[b for b in c.bRepBodies if b.volume>1e-9 and b.name.startswith(('Cuerpo','Banda_carcasa','Pestanas'))]
        if len(case)>1:
            es=adsk.core.ObjectCollection.create()
            for b in case[1:]:es.add(b)
            ip=c.features.combineFeatures.createInput(case[0],es);ip.operation=OP.JoinFeatureOperation
            c.features.combineFeatures.add(ip).name='V03_Carcasa_servo_unica_REF'
            case[0].name='Carcasa_servo_REF'
    for i in range(1,4):
        c=gc('V03_XL4016_vertical_'+str(i));sk=c.sketches.itemByName('Croquis_Disipador')
        for dd in sk.sketchDimensions:
            if dd.parameter.expression=='15 mm':dd.parameter.expression='13 mm'
            elif dd.parameter.expression=='30 mm':dd.parameter.expression='28 mm'
    c=gc('V03_Nano_vertical')
    c.constructionPlanes.itemByName('Plano_Conectores').definition.offset.expression='41 mm'
    c.features.extrudeFeatures.itemByName('Conectores').extentOne.distance.expression='32 mm'

def stage_v03_pvc_fasteners():
    # Complete the four split-clamp bolts: washers fill the head gap; add nuts.
    for group,cn,z,turned in [('02_Brazo_PVC','A05_Encastre_proximal_brazo_PETG','socket_start+socket_length/2',False),('02_Brazo_PVC','A06_Encastre_distal_brazo_PETG','link1-socket_start-socket_length/2',False),('03_Antebrazo_PVC','A11_Encastre_proximal_antebrazo_PETG','socket_start+socket_length/2',True),('03_Antebrazo_PVC','A12_Encastre_distal_antebrazo_PETG','link2-socket_start-socket_length/2',False)]:
        bolt=gc(cn+'_Perno_M3_REF')
        bolt.features.extrudeFeatures.itemByName('Vastago').extentOne.distance.expression='18 mm'
        c=sub(comp(group),'V03_Arandelas_y_tuerca_'+cn)
        for name,a,L,od in [('Arandela_cabeza','-8 mm','1.5 mm','7 mm'),('Arandela_tuerca','6.5 mm','1 mm','7 mm'),('Tuerca_M3_REF','7.5 mm','2.4 mm','6.4 mm')]:
            ring(c,name,'Y',a,'pvc_od/2+6 mm',z,od,'3.4 mm' if 'Arandela' in name else '3 mm',L,'steel')
        if turned:
            es=adsk.core.ObjectCollection.create()
            for b in c.bRepBodies:es.add(b)
            parent=occ(group).transform2;ax=adsk.core.Vector3D.create(0,0,1);ax.transformBy(parent);centre=P(0,0,0);centre.transformBy(parent)
            m=adsk.core.Matrix3D.create();m.setToRotation(math.pi/2,ax,centre)
            ip=c.features.moveFeatures.createInput2(es);ip.defineAsFreeMove(m);c.features.moveFeatures.add(ip).name='Orientacion_abrazadera_Y'

def stage_v03_logic_bridges():
    c=gc('V03_Plataforma_sandwich_superior')
    # Extend the edge clips across the 0.4-mm PCB-side clearance to their posts.
    for module in ['PCA9685','Nano']:
        for side in [-1,1]:
            for feature in ['Asiento_','Retencion_']:
                sk=c.sketches.itemByName('Croquis_'+feature+module+str(side))
                sk.sketchDimensions.item(sk.sketchDimensions.count-1).parameter.expression='4.8 mm'
                if side<0:
                    p=sk.sketchDimensions.item(1).parameter;p.expression='('+p.expression+')+0.8 mm'
    # Captive terminal-block pocket, joined to the plate with insertion from front.
    for name,x,y,z,w,dep,h in [('Pared_bornera_I','47 mm','16 mm','28 mm','2 mm','13 mm','9 mm'),('Pared_bornera_D','69 mm','16 mm','28 mm','2 mm','13 mm','9 mm'),('Fondo_bornera','47 mm','27 mm','28 mm','24 mm','2 mm','9 mm'),('Labio_bornera_I','48 mm','18 mm','36 mm','2 mm','9 mm','1 mm'),('Labio_bornera_D','68 mm','18 mm','36 mm','2 mm','9 mm','1 mm'),('Pestillo_bornera','58 mm','15 mm','28 mm','2 mm','2 mm','9 mm'),('Cabeza_pestillo_bornera','58 mm','16 mm','36 mm','2 mm','2 mm','1 mm')]:
        box(c,name,x,y,z,w,dep,h)

def stage_v03_pvc_stops():
    # End stops establish insertion depth independently of the clamp's radial clearance.
    for cn,z in [('A05_Encastre_proximal_brazo_PETG','tube_inset-2 mm'),('A06_Encastre_distal_brazo_PETG','link1-tube_inset'),('A11_Encastre_proximal_antebrazo_PETG','tube_inset-2 mm'),('A12_Encastre_distal_antebrazo_PETG','link2-tube_inset')]:
        c=gc(cn);ring(c,'Tope_axial_PVC','Z',z,'0 mm','0 mm','socket_od','pvc_id+socket_clear','2 mm');unite(c,'V03_Manguito_con_tope_integrado')

def stage_v03_seating_clearances():
    # Clear only stationary printed material; never cut shafts or bearing races.
    c=gc('A12_Encastre_distal_antebrazo_PETG')
    ring(c,'Paso_libre_separador_J4','Z','link2-20 mm','0 mm','0 mm','11.6 mm',None,'26 mm',cut=True)
    c=gc('V02_A18_Nucleo_J5_y_columna_J6')
    ring(c,'Paso_libre_separadores_J6','Z','wrist56-17 mm','0 mm','0 mm','8.6 mm',None,'21 mm',cut=True)
    c=gc('V02_A20_Bastidor_pinza')
    c.constructionPlanes.itemByName('Plano_Llave_D_J6').definition.offset.expression='4 mm'
    c.features.extrudeFeatures.itemByName('Llave_D_J6').extentOne.distance.expression='5 mm'
    for gn,cn,cap in [('00_Base_control','V03_Plataforma_sandwich_superior','V03_Tapa_rodamientos_J1'),('02_Brazo_PVC','A06_Encastre_distal_brazo_PETG','V03_Tapa_608_A06_Encastre_distal_brazo_PETG_1'),('03_Antebrazo_PVC','A12_Encastre_distal_antebrazo_PETG','V03_Tapa_608_J4'),('04_Giro_muneca','V02_A15_Horquilla_J5_PETG','V03_Tapa_625_V02_A15_Horquilla_J5_PETG_1'),('05_Inclinacion_muneca','V02_A18_Nucleo_J5_y_columna_J6','V03_Tapa_625_J6')]:
        target=next(b for b in gc(cn).bRepBodies if b.volume>1e-9)
        tool=next(b for b in gc(cap).bRepBodies if b.volume>1e-9)
        es=adsk.core.ObjectCollection.create();es.add(tool)
        ip=gc(cn).features.combineFeatures.createInput(target,es);ip.operation=OP.CutFeatureOperation;ip.isKeepToolBodies=True
        gc(cn).features.combineFeatures.add(ip).name='V03_Alojamiento_tapa_desmontable'
    c=gc('V03_Plataforma_sandwich_superior')
    for side in [-1,1]:
        x=f'mg_l/2-std_shaft_x+({side})*(mg_l/2+std_ear/2)'
        ring(c,'Piloto_punta_tornillo_servo_'+str(side),'Z','61 mm',x,'0 mm','3.3 mm',None,'5.1 mm',cut=True)

def stage_v03_move_terminal():
    c=gc('V03_Plataforma_sandwich_superior')
    box(c,'Retiro_cajetin_junto_anclaje_B','47 mm','15 mm','28 mm','25 mm','15 mm','10 mm',cut=True)
    for name,x,y,z,w,dep,h in [('Pared_bornera_I_nueva','-12 mm','-29 mm','28 mm','2 mm','13 mm','9 mm'),('Pared_bornera_D_nueva','10 mm','-29 mm','28 mm','2 mm','13 mm','9 mm'),('Fondo_bornera_nuevo','-12 mm','-18 mm','28 mm','24 mm','2 mm','9 mm'),('Labio_bornera_I_nuevo','-11 mm','-27 mm','36 mm','2 mm','9 mm','1 mm'),('Labio_bornera_D_nuevo','9 mm','-27 mm','36 mm','2 mm','9 mm','1 mm'),('Pestillo_bornera_nuevo','-1 mm','-30 mm','28 mm','2 mm','2 mm','9 mm'),('Cabeza_pestillo_bornera_nuevo','-1 mm','-29 mm','36 mm','2 mm','2 mm','1 mm')]:box(c,name,x,y,z,w,dep,h)
    unite(c,'V03_Montura_con_bornera_reubicada')
    c=gc('V03_Bornera_y_alimentacion_REF');es=adsk.core.ObjectCollection.create()
    for b in c.bRepBodies:es.add(b)
    ip=c.features.moveFeatures.createInput2(es);m=adsk.core.Matrix3D.create();m.translation=adsk.core.Vector3D.create(-5.9,-4.5,0);ip.defineAsFreeMove(m);c.features.moveFeatures.add(ip).name='V03_Bornera_fuera_de_anclaje_B'

def stage_v03_rotor_clearance():
    c=gc('V02_A15_Horquilla_J5_PETG')
    for dd in c.sketches.itemByName('Croquis_Rotor').sketchDimensions:
        if dd.parameter.expression=='38 mm':dd.parameter.expression='32 mm'

def stage_v03_mount_access():
    ring(gc('V03_Plataforma_sandwich_superior'),'Acceso_cabeza_M10','Z','28 mm','agv_center_x','agv_center_y','18 mm',None,'12 mm',cut=True)

def stage_v03_microbolt_clearance():
    import re
    for gn in ['10_Pinion_pinza','11_Pinion_J4']:
        for oc in comp(gn).occurrences:
            for p in oc.component.modelParameters:
                e=p.expression;n=re.sub(r'5.5\s*mm','6 mm',e)
                if e!=n:p.expression=n

def stage_v03_j5_rear_retention():
    # Independent rear axial retention; washer and spacer clear the fixed cap bore.
    g=comp('05_Inclinacion_muneca')
    ring(gc('V02_E04_Eje_y_brida_J5'),'Rosca_axial_trasera_REF','Y','-28 mm','0 mm','0 mm','3 mm',None,'6 mm',cut=True)
    c=sub(g,'V03_Separador_trasero_J5');ring(c,'Separador','Y','-29 mm','0 mm','0 mm','7.8 mm','5 mm','5.5 mm','steel')
    c=sub(g,'V03_Arandela_trasera_J5');ring(c,'Arandela_M3_REF','Y','-30 mm','0 mm','0 mm','7 mm','3.3 mm','1 mm','steel')
    c=sub(g,'V03_Tornillo_axial_trasero_J5');ring(c,'Vastago','Y','-30 mm','0 mm','0 mm','3 mm',None,'6 mm','steel');ring(c,'Cabeza','Y','-33 mm','0 mm','0 mm','5.5 mm',None,'3 mm','steel');join_all(c,'Tornillo_M3x6_REF')

def stage_v03_restore_stock():
    # Reconstruct complete stock after legacy foreign cuts, preserving their history.
    for cn,length in [('L01_PVC32_Corte_1','tube1_cut'),('L02_PVC32_Corte_2','tube2_cut')]:
        c=gc(cn);ring(c,'V03_PVC_entero_sin_cortes_ajenos','Z','tube_inset','0 mm','0 mm','pvc_od','pvc_id',length,'white');unite(c,'V03_Cano_completo')
    for group,cn,z,turned in [('02_Brazo_PVC','A05_Encastre_proximal_brazo_PETG','socket_start+socket_length/2',False),('02_Brazo_PVC','A06_Encastre_distal_brazo_PETG','link1-socket_start-socket_length/2',False),('03_Antebrazo_PVC','A11_Encastre_proximal_antebrazo_PETG','socket_start+socket_length/2',True),('03_Antebrazo_PVC','A12_Encastre_distal_antebrazo_PETG','link2-socket_start-socket_length/2',False)]:
        c=gc(cn+'_Perno_M3_REF');new=[]
        for name,a,diam,L in [('V03_Vastago_completo','-8 mm','3 mm','18 mm'),('V03_Cabeza_completa','-10 mm','5.5 mm','2 mm')]:
            f=ring(c,name,'Y',a,'pvc_od/2+6 mm',z,diam,None,L,'steel');new.extend(list(f.bodies))
        if turned:
            es=adsk.core.ObjectCollection.create()
            for b in new:es.add(b)
            parent=occ(group).transform2;ax=adsk.core.Vector3D.create(0,0,1);ax.transformBy(parent);centre=P(0,0,0);centre.transformBy(parent)
            m=adsk.core.Matrix3D.create();m.setToRotation(math.pi/2,ax,centre);ip=c.features.moveFeatures.createInput2(es);ip.defineAsFreeMove(m);c.features.moveFeatures.add(ip).name='V03_Orientacion_perno_completo'
        unite(c,'V03_Perno_completo_sin_muescas')

def stage_v03_standard_motor_mounts():
    for group,printed,prefix,face,z,label in [('01_Giro_base','A01_Plataforma_giratoria_PETG','gx','49 mm','tower_h-gear_centers','J2'),('02_Brazo_PVC','A06_Encastre_distal_brazo_PETG','ds','46 mm','link1','J3'),('04_Giro_muneca','V02_A15_Horquilla_J5_PETG','mg','38 mm','wrist45','J5')]:
        g=comp(group);c=gc(printed)
        for side in [-1,1]:
            x=f'{prefix}_l/2-std_shaft_x+({side})*({prefix}_l/2+std_ear/2)';front=f'({face})+std_ear_z'
            ring(c,'Caja_cabeza_fijacion_'+label+str(side),'Y',f'({front})-3.2 mm',x,z,'6 mm',None,'3.3 mm',cut=True)
            ring(c,'Inserto_fijacion_'+label+str(side),'Y',f'({front})+3 mm',x,z,'4.5 mm',None,'4 mm',cut=True)
            ic=sub(g,'V03_Inserto_fijacion_'+label+str(side));ring(ic,'Inserto_M3_REF','Y',f'({front})+3 mm',x,z,'4.5 mm','3 mm','4 mm','brass')
            sc=sub(g,'V03_Tornillo_fijacion_'+label+str(side));ring(sc,'Vastago_M3x8','Y',front,x,z,'3 mm',None,'8 mm','steel');ring(sc,'Cabeza','Y',f'({front})-3 mm',x,z,'5.5 mm',None,'3 mm','steel');join_all(sc,'Tornillo_M3x8_REF')

def stage_v03_micro_motor_mounts():
    for group,printed,face,cx,label in [('03_Antebrazo_PVC','A12_Encastre_distal_antebrazo_PETG','link2','36 mm','J4'),('05_Inclinacion_muneca','V02_A18_Nucleo_J5_y_columna_J6','wrist56-22.5 mm','0 mm','J6'),('06_Herramienta','V02_A20_Bastidor_pinza','grip_deck+2 mm','0 mm','J7')]:
        g=comp(group);c=gc(printed)
        for side in [-1,1]:
            x=f'({cx})+micro_l/2-micro_shaft_x+({side})*(micro_l/2+2 mm)'
            # Nominal thread envelope. Actual pilot for a screw into PETG must be calibrated.
            ring(c,'Rosca_nominal_plastico_'+label+str(side),'Z',f'({face})-14 mm',x,'0 mm','2.5 mm','2 mm','6 mm')
            sc=sub(g,'V03_Tornillo_rosca_plastico_'+label+str(side))
            ring(sc,'Vastago_REF','Z',f'({face})-13.5 mm',x,'0 mm','2 mm',None,'8 mm','steel');ring(sc,'Cabeza','Z',f'({face})-5.5 mm',x,'0 mm','3.8 mm',None,'2 mm','steel');join_all(sc,'Tornillo_2x8_REF')
        unite(c,'V03_Anclaje_microservo_con_rosca_nominal')

def stage_v03_j3_mount_location():
    import re
    c=gc('A06_Encastre_distal_brazo_PETG')
    for side in [-1,1]:
        for name in ['Caja_cabeza_fijacion_J3','Inserto_fijacion_J3']:
            p=c.constructionPlanes.itemByName('Plano_'+name+str(side)).definition.offset;p.expression=re.sub(r'42\s*mm','46 mm',p.expression)
        for cn in ['V03_Inserto_fijacion_J3','V03_Tornillo_fijacion_J3']:
            for p in gc(cn+str(side)).modelParameters:
                e=p.expression;n=re.sub(r'42\s*mm','46 mm',e)
                if e!=n:p.expression=n

def stage_v03_save():
    a=adsk.core.Application.get();doc=a.activeDocument
    if doc.isModified:doc.save('Montura sobre AGV, continuidad fisica, ejes y apoyos revisados. Ver informe v03.')
    D.exportManager.execute(D.exportManager.createFusionArchiveExportOptions(os.path.join(OUT_DIR,V03+'.f3d')))
    import csv
    with open(os.path.join(OUT_DIR,'parametros_prototipo_v03.csv'),'w',encoding='utf-8-sig',newline='') as f:
        w=csv.writer(f);w.writerow(['parametro','expresion','unidad','procedencia'])
        for p in D.userParameters:w.writerow([p.name,p.expression,p.unit,p.comment])
    print('GUARDADO',V03,doc.dataFile.id)

def stage_v03_manifest():
    app=adsk.core.Application.get();doc=app.activeDocument;components=[];warnings=[];inactive=0
    for c in D.allComponents:
        if c.name.startswith('REF_AUTO_'):continue
        bb=[{'name':b.name,'volume_mm3':round(b.volume*1000,3),'lumps':b.lumps.count} for b in c.bRepBodies if b.isSolid and b.volume>1e-9]
        if bb:components.append({'component':c.name,'bodies':bb})
    for i in range(D.timeline.count):
        it=D.timeline.item(i)
        if it.healthState in [adsk.fusion.FeatureHealthStates.WarningFeatureHealthState,adsk.fusion.FeatureHealthStates.ErrorFeatureHealthState]:
            warnings.append({'index':i,'name':it.name,'state':int(it.healthState),'message':it.errorOrWarningMessage})
        elif it.healthState!=adsk.fusion.FeatureHealthStates.HealthyFeatureHealthState:inactive+=1
    joints=[]
    for j in R.joints:
        row={'name':j.name,'type':str(j.jointMotion.jointType)}
        if j.jointMotion.jointType==adsk.fusion.JointTypes.RevoluteJointType:
            lm=j.jointMotion.rotationLimits;row.update({'CAD_deg':math.degrees(j.jointMotion.rotationValue),'minimum_deg':math.degrees(lm.minimumValue) if lm.isMinimumValueEnabled else None,'maximum_deg':math.degrees(lm.maximumValue) if lm.isMaximumValueEnabled else None})
        joints.append(row)
    result={'revision':'AFR2-20261027-03','document':doc.name,'file_id':doc.dataFile.id,'scope':'Prototipo CAD editable; no fabricacion ni ensayo de carga','user_parameters':D.userParameters.count,'components_including_root':D.allComponents.count,'root_groups':R.occurrences.count,'motion_links':R.motionLinks.count,'joints':joints,'arm_components_with_solid_geometry':components,'timeline_health_messages':warnings,'payload_mm':[50,50,50],'payload_requirement_g':100,'car_reference_solids':len(builtins._afr2_car_v03),'native_car_to_arm_mm':'(X, -Z-32, Y-26)','mounting_holes_mm':HOLES,'base_top_mm':val('base_top')*10,'shoulder_z_mm':val('shoulder_z')*10,'native_file':V03+'.f3d','parameters_csv':'parametros_prototipo_v03.csv'}
    result['inactive_historical_timeline_items']=inactive
    with open(os.path.join(REPORT,'modelo_v03.json'),'w',encoding='utf-8') as f:json.dump(result,f,ensure_ascii=False,indent=2)
    print('MANIFIESTO',D.userParameters.count,D.allComponents.count,'mensajes_historial',len(warnings),json.dumps(warnings[:10],ensure_ascii=False))

def stage_v03_scope_upper625():
    def fingerprint():
        out={}
        for c in D.allComponents:
            if c.name.startswith('REF_AUTO_'):continue
            for b in c.bRepBodies:
                if b.isSolid and b.volume>1e-9:out[c.name+'/'+b.name]=[b.volume,b.faces.count,b.edges.count]+list(b.boundingBox.minPoint.asArray())+list(b.boundingBox.maxPoint.asArray())
        return out
    before=fingerprint();f=D.timeline.item(421).entity
    if f.name!='Rebaje_625':raise RuntimeError('Indice de reparacion distinto del historial documentado')
    f.timelineObject.rollTo(True);owner=f.profile.parentSketch.parentComponent
    target=[b for b in f.participantBodies if b.parentComponent.name==owner.name]
    if not target:raise RuntimeError('No hay cuerpo de alojamiento propio')
    f.participantBodies=target;f.timelineObject.rollTo(False);D.timeline.moveToEnd()
    after=fingerprint()
    if before.keys()!=after.keys() or any(max(abs(x-y) for x,y in zip(before[k],after[k]))>1e-7 for k in before):raise RuntimeError('La retirada del corte obsoleto altero geometria; revisar antes de continuar')
    with open(os.path.join(REPORT,'reparacion_asiento625.json'),'w',encoding='utf-8') as ff:json.dump({'feature':'Rebaje_625','state':'participants limited to sketch component','arm_bodies_compared':len(before),'geometry_fingerprint_unchanged':True,'fingerprint':'component/body keys, volume, face/edge counts, native bounding boxes'},ff,indent=2)

def stage_v03_labels():
    labels={'V03_Plataforma_sandwich_superior':'Montura, rack y apoyo fijo J1 - una impresion','A01_Plataforma_giratoria_PETG':'Plataforma, torre y soporte de hombro - una impresion','A05_Encastre_proximal_brazo_PETG':'Nucleo de hombro y manguito proximal - una impresion','A06_Encastre_distal_brazo_PETG':'Manguito, horquilla de codo y housing - una impresion','A11_Encastre_proximal_antebrazo_PETG':'Nucleo de codo y manguito proximal - una impresion','A12_Encastre_distal_antebrazo_PETG':'Manguito, doble apoyo J4 y soporte de motor - una impresion','V02_A15_Horquilla_J5_PETG':'Rotor, rueda J4, horquilla J5 y housing - una impresion','V02_A18_Nucleo_J5_y_columna_J6':'Nucleo J5, cuna J6 y portarodamientos - una impresion','V02_A20_Bastidor_pinza':'Plataforma, palma, guias y labios unidos - una impresion'}
    hidden=0
    for o in R.allOccurrences:
        c=o.component
        if c.name=='80_Auto_referencia' or c.name.startswith('REF_AUTO_'):continue
        if c.name in labels:
            c.attributes.add('AFR2','pieza_impresa',labels[c.name])
            for b in c.bRepBodies:
                if b.isSolid and b.volume>1e-9:b.name=labels[c.name]
        if c.occurrences.count==0 and not any(b.isSolid and b.volume>1e-9 for b in c.bRepBodies):
            o.isLightBulbOn=False;hidden+=1
    print('ETIQUETAS',len(labels),'componentes historicos vacios ocultos',hidden)

def run(context):
    setup();phase=globals()['PHASE']
    if adsk.core.Application.get().activeDocument.name!=V03:raise RuntimeError('Activar v03 antes de modificar')
    os.makedirs(REPORT,exist_ok=True)
    with open(os.path.join(REPORT,'authoring_journal.jsonl'),'a',encoding='utf-8') as f:f.write(json.dumps({'phase':phase,'state':'started'})+'\n')
    globals()['stage_'+phase]();adsk.doEvents();adsk.core.Application.get().activeViewport.refresh()
    with open(os.path.join(REPORT,'authoring_journal.jsonl'),'a',encoding='utf-8') as f:f.write(json.dumps({'phase':phase,'state':'applied'})+'\n')
    print('FASE',phase,'APLICADA')
