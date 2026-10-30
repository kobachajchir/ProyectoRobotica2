"""Interfaces STS3250 corregidas con fichaA0 y STEP comercial2021 medido.

12.5mm es el desplazamiento eje-centro de caja; extremo corto10.11mm.
No se redistribuye el STEP: esta envolvente parametrica propia usa sus
cotas externas, sin reproducir engranajes internos del fabricante.
"""

def act17_sts_parametros():
    for n,e,u,comment in [
      ('v17_sts_l','45.22 mm','mm','FichaFEETECH y STEP: largo de caja.'),
      ('v17_sts_w','24.72 mm','mm','FichaFEETECH y STEP.'),
      ('v17_sts_offset','12.5 mm','mm','Distancia desde CENTRO de caja al eje, no desde extremo.'),
      ('v17_sts_short','v17_sts_l/2-v17_sts_offset','mm','Extremo corto10.11, derivado y comprobado en STEP.'),
      ('v17_sts_j3_dx','v17_sts_short-12.5 mm','mm','Correccion-2.39mm respecto de envolvente anterior.'),
      ('v17_sts_step','2.6 mm','mm','STEP: plano frontal envolvente7.5 menos salida4.9.'),
      ('v17_sts_step_d','22.4 mm','mm','STEP: cilindro de transicion frontalR11.2.'),
      ('v17_sts_seat_d','10.5 mm','mm','STEP: alojamiento frontalR5.25.'),
      ('v17_sts_seat_depth','0.5 mm','mm','STEP:4.4..4.9.'),
      ('v17_sts_output_y','-73.4 mm+j3_transverse_shift','mm','Plano base del estriado del motorJ3.'),
    ]:param(n,e,u,comment)

def act17_sts_motor_J3():
    act17_sts_parametros();c=act17_get('V17_M03_STS3250_remoto');discard_geometry(c)
    box(c,'V17_STS3250_caja_referencia','-v17_sts_offset-v17_sts_l/2','v17_sts_output_y-32.4 mm','j3_motor_z-v17_sts_w/2','v17_sts_l','35 mm','v17_sts_w','servo')
    ring(c,'V17_Escalon_frontal_STS','Y','v17_sts_output_y','0 mm','j3_motor_z','v17_sts_step_d',None,'v17_sts_step+0.1 mm',cut=True)
    ring(c,'V17_Alojamiento_frontal_STS','Y','v17_sts_output_y-v17_sts_seat_depth','0 mm','j3_motor_z','v17_sts_seat_d',None,'v17_sts_seat_depth+0.1 mm',cut=True)
    ring(c,'V17_Salida25T_REF','Y','v17_sts_output_y-v17_sts_seat_depth','0 mm','j3_motor_z','5.9 mm',None,'3.4 mm+v17_sts_seat_depth','brass')
    ring(c,'V17_Pivote_trasero_REF','Y','v17_sts_output_y-33.1 mm','0 mm','j3_motor_z','6 mm',None,'0.7 mm','servo')
    ring(c,'V17_Rosca_M3_servo_REF','Y','v17_sts_output_y-2.7 mm','0 mm','j3_motor_z','3.2 mm',None,'6.2 mm',cut=True)
    c.attributes.add('AFR2','catalog_mass_g','75.5')
    c.attributes.add('AFR2','material_v17','Servo FEETECH STS3250')
    c.attributes.add('AFR2','referencia_corregida','Caja45.22x24.72, eje12.5desdecentro; corto10.11/largo35.11. Envolvente35, paso frontal2.6,receso10.5x0.5, salida3.4 y pivote0.7. STEP comercial2021 comparado con ficha2024. No interiores exactos ni plano de fabricar servo.')

def sts17_shift_x(sk,expression):
    if sk.sketchCurves.sketchLines.count:p=sk.sketchCurves.sketchLines.item(0).startSketchPoint
    elif sk.sketchCurves.sketchCircles.count:p=sk.sketchCurves.sketchCircles.item(0).centerSketchPoint
    else:raise RuntimeError('No se reconoce perfil para desplazar: '+sk.name)
    value=p.geometry.x;dim=sk.sketchDimensions.item(0)
    if abs(dim.parameter.value-abs(value))>1e-6:raise RuntimeError('Primera cota no es posicionX: '+sk.name)
    previous=dim.parameter.expression
    dim.parameter.expression='('+previous+')'+('-' if value<0 else '+')+'('+expression+')'

def sts17_move(c,name,x='0 mm',y='0 mm',z='0 mm'):
    next(o for o in R.allOccurrences if o.component==c).activate()
    bodies=adsk.core.ObjectCollection.create()
    for b in c.bRepBodies:bodies.add(b)
    ip=c.features.moveFeatures.createInput2(bodies)
    ip.defineAsTranslateXYZ(V(x),V(y),V(z),False)
    c.features.moveFeatures.add(ip).name=name

def act17_sts_housing_J3():
    h=act17_get('V17_Housing_STS3250_y_apoyo625');dx='v17_sts_j3_dx'
    for sk in h.sketches:
      if sk.name.startswith(tuple('Croquis_'+n for n in ['Piso_liner','Lateral','Tope','Pilar_tapa','Paso_pilar'])):
        sts17_shift_x(sk,dx)
        if sk.name.startswith(('Croquis_Piso_liner','Croquis_Lateral')):
          depth=sk.sketchDimensions.item(3)
          if abs(depth.parameter.value-val('41 mm'))>1e-6:raise RuntimeError('Profundidad inesperada en '+sk.name)
          depth.parameter.expression='41 mm+v17_sts_step'
        elif sk.name.startswith('Croquis_Tope'):
          y=sk.sketchCurves.sketchLines.item(0).startSketchPoint.geometry.y
          if abs(y-val('-72.8 mm'))<1e-6:
            dim=sk.sketchDimensions.item(1);dim.parameter.expression='('+dim.parameter.expression+')-v17_sts_step'
          elif abs(y-val('-111.4 mm'))<1e-6:
            sk.sketchDimensions.item(3).parameter.expression='2.4 mm+v17_sts_step'
          else:raise RuntimeError('Tope no reconocido')
    cap=act17_get('V17_Tapa_housing_STS3250')
    for sk in cap.sketches:
      sts17_shift_x(sk,dx)
      if sk.name=='Croquis_V17_Tapa_housing_STS3250':sk.sketchDimensions.item(3).parameter.expression='41 mm+v17_sts_step'
    carrier=act17_get('V17_Carro_motor_apoyo_Al')
    for sk in carrier.sketches:
      if sk.name.startswith(('Croquis_Ala_motor','Croquis_Paso_tapa_Al')):sts17_shift_x(sk,dx)
    # Estos cortes se hacen al final del historial, tras los apoyos y sus
    # puentes, para que no vuelvan a rellenar el espacio de giro del horn.
    ring(h,'V17_Liberar_pivote_trasero_STS','Y','-111.5 mm+j3_transverse_shift','0 mm','j3_motor_z','6.5 mm',None,'5.2 mm',cut=True)
    ring(h,'V17_Corredor_horn_y_brida','Y','-73 mm+j3_transverse_shift','0 mm','j3_motor_z','22 mm',None,'11.5 mm',cut=True)
    for o in list(comp('02_Brazo_PVC').occurrences):
      if o.component.name.startswith(('V17_M3_tapa_','V17_Tuerca_tapa_')):
        sts17_move(o.component,'V17_Eje_caja_desde_centro',x=dx)
    for c in [h,cap,carrier]:act17_one(c)
    h.attributes.add('AFR2','caja_corregida','Caja desplazadaX-2.39, frente+2.6 y tope trasero extendido. Corredor22 para horn/brida, sin desplazar625 ni eje ni ranuras del carro.')

def act17_sts_horn_J3():
    act17_remove('12_Polea_entrada_J3',['V17_Acople25T_a_eje5','V17_Prisionero_acople5'])
    g=comp('12_Polea_entrada_J3');h=piece17(g,'V17_Horn_STS3250_J3','Al6061')
    ring(h,'Disco_horn19_95','Y','-71.6 mm+j3_transverse_shift','0 mm','0 mm','19.95 mm','3.2 mm','2.5 mm','steel')
    ring(h,'Cubo_horn9','Y','-73.6 mm+j3_transverse_shift','0 mm','0 mm','9 mm','5.9 mm','2 mm','steel')
    join_all(h,'Horn_STS3250_REF')
    ring(h,'Spline25T_REF','Y','-73.7 mm+j3_transverse_shift','0 mm','0 mm','5.9 mm',None,'2.8 mm',cut=True)
    ring(h,'Alivio_socket_REF','Y','-70.9 mm+j3_transverse_shift','0 mm','0 mm','6.2 mm',None,'0.9 mm',cut=True)
    for x,z in [('7 mm','0 mm'),('-7 mm','0 mm'),('0 mm','7 mm'),('0 mm','-7 mm')]:
      ring(h,'Rosca_M3_PCD14_REF','Y','-71.7 mm+j3_transverse_shift',x,z,'3 mm',None,'2.7 mm',cut=True)
    h.attributes.add('AFR2','catalogo','Horn motriz FEETECH25T5.9,OD19.95,4M3PCD14,espesor4.5/disco2.5. Socket con alivio, no fabricar desde envolvente cilindrica.')
    # El eje continuo recibe una brida mecanizable. El tornillo central del
    # horn se coloca ANTES de montar esta brida: no queda atrapado en acople.
    shaft=act17_get('V17_Eje_entrada_5mm')
    ring(shaft,'V17_Brida21_integrada','Y','-69.1 mm+j3_transverse_shift','0 mm','0 mm','21 mm',None,'4 mm','steel')
    join_all(shaft,'V17_Eje_y_brida_una_pieza')
    ring(shaft,'V17_Caja_cabeza_central','Y','-69.2 mm+j3_transverse_shift','0 mm','0 mm','6.2 mm',None,'3.3 mm',cut=True)
    points=[('7 mm','0 mm'),('-7 mm','0 mm'),('0 mm','7 mm'),('0 mm','-7 mm')]
    for x,z in points:
      ring(shaft,'V17_Paso_M3_brida','Y','-69.2 mm+j3_transverse_shift',x,z,'3.4 mm',None,'4.2 mm',cut=True)
      ring(shaft,'V17_Asiento_cabeza_M3','Y','-65.6 mm+j3_transverse_shift',x,z,'6 mm',None,'0.6 mm',cut=True)
    for i,(x,z) in enumerate(points):
      bolt=piece17(g,'V17_M3x6_horn_J3_'+str(i),'Acero')
      ring(bolt,'Vastago','Y','-71.6 mm+j3_transverse_shift',x,z,'3 mm',None,'6 mm','steel')
      ring(bolt,'Cabeza','Y','-65.6 mm+j3_transverse_shift',x,z,'5.5 mm',None,'3 mm','steel')
      join_all(bolt,'M3x6_REF')
    sts17_move(act17_get('V17_Tornillo_central_servo_J3'),'V17_Asiento_horn_estandar',y='0.1 mm')
    shaft.attributes.add('AFR2','material_v17','Acero')
    shaft.attributes.add('AFR2','fabricacion_v17','Eje5 y brida21x4 torneados en una pieza, aceroRp>=600MPa requerido. Horn comercial4M3, tornillosM3x6 con asiento0.5. Caja6.2 deja0.8mmdealma; requiere comprobacion estructural final. MontarM3central primero, despues brida.')
    act17_one(h);act17_one(shaft)


def act17_sts_J2_soporte():
    for n,e in [('gear_module','0.8 mm'),('wheel_teeth','72'),('pinion_teeth','18'),
                ('gear_centers','gear_module*(wheel_teeth+pinion_teeth)/2'),
                ('shoulder_phase','0 deg'),('v17_j2_input_z','tower_h-gear_centers')]:
        param(n,e,'deg' if n=='shoulder_phase' else '' if n.endswith('teeth') else 'mm')
    act17_remove('01_Giro_base',['V02_M02_GX3240_270','V03_Inserto_fijacion_J2','V03_Tornillo_fijacion_J2'])
    c=act17_get('A01_Plataforma_giratoria_PETG')
    box(c,'V17_Retirar_cuna_motor_anterior','-70 mm','32.5 mm','8 mm','140 mm','123 mm','tower_h+12 mm',cut=True)
    box(c,'V17_Travesano_J2_integrado','-34 mm','24.5 mm','v17_j2_input_z-17 mm','68 mm','8 mm','33.5 mm')
    join_all(c,'V17_Torre_y_travesano_una_pieza')
    for x in ['-17 mm','17 mm']:
      for z in ['v17_j2_input_z-8 mm','v17_j2_input_z+10 mm']:
        ring(c,'V17_Paso_M4_modulo_J2','Y','24.4 mm',x,z,'4.5 mm',None,'8.2 mm',cut=True)
    act17_one(c)
    c.attributes.add('AFR2','montaje_J2','Travesano integrado a horquilla, modulo STS3250 sobre4M4. No elevar hombro. SeparadoresAl7mm y reduccion18/72mod0.8.')


def act17_sts_J2_engranajes():
    act17_remove('09_Pinion_hombro',['V02_Pinon20_Hombro','V03_Horn_movil_V02_M02','V03_Tornillo_union_V02_M02','V03_Tuerca_union_V02_M02','V03_Tornillo_original_horn_V02_M02','V03_Tornillo_M2_5_pinon_','V03_Tuerca_M2_5_pinon_'])
    act17_remove('02_Brazo_PVC',['V02_Rueda40_Hombro','V03_Separador_8mm_delantero','V03_Retencion_rueda_hombro','V03_Tornillo_axial_hombro'])
    for root,name,n,bore,hub in [('09_Pinion_hombro','V17_Pinon18_J2_Al2024',18,'5 mm','10 mm'),('02_Brazo_PVC','V17_Rueda72_J2_Al2024',72,'8 mm','12 mm')]:
      c=involute_gear(comp(root),name,n,'gear_module',bore,z='48 mm',axis='Y',thickness='5 mm')
      ring(c,'Cubo_comercial','Y','53 mm','0 mm','0 mm',hub,bore,'7 mm','steel')
      join_all(c,'Engranaje_y_cubo_Al2024');finish(c,'steel')
      c.attributes.add('AFR2','material_v17','Al2024')
      c.attributes.add('AFR2','comercial','SDP/SI S10T08M018A0505 o S10T08M072A0508 segun dientes. Envolvente propia, cara5,total12,20grados; confirmar chavetero/mecanizado y lote antes de fabricar.')
      if n==18:
        box(c,'Chavetero_2mm','-3.5 mm','47.9 mm','-1 mm','2 mm','12.2 mm','2 mm',cut=True)
        ring(c,'Piloto_prisionero_M2','Z','2.3 mm','0 mm','56.5 mm','1.6 mm',None,'3 mm',cut=True)
      else:box(c,'Chavetero_2mm','3 mm','47.9 mm','-1 mm','2 mm','12.2 mm','2 mm',cut=True)
      act17_one(c)
    shaft=act17_get('V17_Eje_entrada_5mm')
    box(shaft,'V17_Plano_prisionero_J2','-3 mm','-54.5 mm+j3_transverse_shift','2.3 mm','6 mm','4 mm','1 mm',cut=True)
    param('gear_ratio','wheel_teeth/pinion_teeth','')
    lim=R.joints.itemByName('T_Hombro_Pinion_2a1').jointMotion.rotationLimits
    if lim:
      lim.isMinimumValueEnabled=False;lim.isMaximumValueEnabled=False


def sts17_shared(source,root,anchor,rotor=False,target_y='77.4 mm',target_z=None,tag='J2',target_x='0 mm',source_axis='Z'):
    c=act17_get(source);g=comp(root)
    if target_z is None:target_z='0 mm' if rotor else 'v17_j2_input_z'
    source_name='V17_Interfaz_modulo_STS'+('_Y' if source_axis=='Y' else '')
    jo=c.jointOrigins.itemByName(source_name)
    if not jo:jo=origin(c,source_name,'0 mm','v17_sts_output_y','0 mm' if rotor else 'j3_motor_z',axis=source_axis)
    a=act17_get(anchor)
    target=a.jointOrigins.itemByName('V17_Interfaz_'+tag+'_STS')
    if not target:target=origin(a,'V17_Interfaz_'+tag+'_STS',target_x,target_y,target_z)
    D.activateRootComponent()
    new=g.occurrences.addExistingComponent(c,adsk.core.Matrix3D.create());new.isGroundToParent=False
    proxy=new.createForAssemblyContext(occ(root))
    anchor_native=next(o for o in g.occurrences if o.component==a)
    anchor_proxy=anchor_native.createForAssemblyContext(occ(root))
    ip=R.joints.createInput(jo.createForAssemblyContext(proxy),target.createForAssemblyContext(anchor_proxy))
    ip.angle=V('180 deg');ip.isFlipped=False;ip.setAsRigidJointMotion()
    j=R.joints.add(ip);j.name='V17_'+tag+'_instancia_'+source
    adsk.doEvents()
    # Comprobar transformacion RELATIVA al grupo, no a coordenadas globales.
    # Consultar el proxy completo. En esta instalacion transform2 sobre la
    # ocurrencia nativa compartida devolvio el contexto del primer montaje.
    # El proxy y la junta estaban correctos; no usar esa matriz ambigua.
    inverse=occ(root).transform2;inverse.invert()
    p=P(0,val('v17_sts_output_y'),0 if rotor else val('j3_motor_z'));p.transformBy(proxy.transform2);p.transformBy(inverse)
    expected=P(val(target_x),val(target_y),val(target_z))
    (HERE17/('diagnostico_instancia_'+tag+'.json')).write_text(json.dumps(dict(source=source,new_path=new.fullPathName,proxy_path=proxy.fullPathName,anchor_path=anchor_proxy.fullPathName,new_matrix=list(new.transform2.asArray()),proxy_matrix=list(proxy.transform2.asArray()),parent_matrix=list(occ(root).transform2.asArray()),anchor_matrix=list(anchor_native.transform2.asArray()),new_ground=new.isGroundToParent,joint_health=int(j.healthState),joint_message=j.errorOrWarningMessage,point=list(p.asArray()),expected=list(expected.asArray())),indent=2),encoding='utf-8')
    if p.distanceTo(expected)>1e-5:raise RuntimeError('Instancia '+tag+' fuera de interfaz: '+source+' '+str(p.asArray()))
    expected_axes=[[-1,0,0],[0,-1,0],[0,0,1]] if source_axis=='Z' else [[-1,0,0],[0,0,1],[0,1,0]]
    for vector,wanted in zip([adsk.core.Vector3D.create(1,0,0),adsk.core.Vector3D.create(0,1,0),adsk.core.Vector3D.create(0,0,1)],expected_axes):
      vector.transformBy(proxy.transform2);vector.transformBy(inverse)
      if max(abs(v-w) for v,w in zip(vector.asArray(),wanted))>1e-5:raise RuntimeError('Orientacion '+tag+' incorrecta: '+source)
    progress('instancia '+tag+' '+source)


def act17_sts_J2_motor():
    sts17_shared('V17_M03_STS3250_remoto','01_Giro_base','A01_Plataforma_giratoria_PETG')


def act17_sts_J2_cuna():
    for name in ['V17_Housing_STS3250_y_apoyo625','V17_Carro_motor_apoyo_Al','V17_Tapa_housing_STS3250','V17_625_entrada_J3_trasero','V17_625_entrada_J3_delantero','V17_Tapa625_entrada_trasera','V17_M2_tapa625_-14','V17_M2_tapa625_14']:
      sts17_shared(name,'01_Giro_base','A01_Plataforma_giratoria_PETG')
    for o in list(comp('02_Brazo_PVC').occurrences):
      if o.component.name.startswith(('V17_M3_tapa_','V17_Tuerca_tapa_')):
        sts17_shared(o.component.name,'01_Giro_base','A01_Plataforma_giratoria_PETG')


def act17_sts_J2_rotor():
    names=['V17_Eje_entrada_5mm','V17_Horn_STS3250_J3','V17_Tornillo_central_servo_J3','V17_Chaveta_entrada_2x2x9','V17_Collar_entrada5','V17_Prisionero_collar_entrada5']
    names += ['V17_M3x6_horn_J3_'+str(i) for i in range(4)]
    for name in names:sts17_shared(name,'09_Pinion_hombro','V17_Pinon18_J2_Al2024',True)


def sts17_pattern4(g,c,name):
    D.activateRootComponent();occ(g.name).activate()
    entities=adsk.core.ObjectCollection.create()
    entities.add(next(o for o in g.occurrences if o.component==c))
    ip=g.features.rectangularPatternFeatures.createInput(entities,g.xConstructionAxis,V('2'),V('34 mm'),adsk.fusion.PatternDistanceType.SpacingPatternDistanceType)
    ip.setDirectionTwo(g.zConstructionAxis,V('2'),V('18 mm'))
    f=g.features.rectangularPatternFeatures.add(ip);f.name=name
    return f


def act17_sts_J2_anclajes():
    g=comp('01_Giro_base');x='-17 mm';z='v17_j2_input_z-8 mm'
    configs=[('V17_Separador7_J2_Al','Al6061','32.5 mm','8 mm','4.5 mm','7 mm'),
             ('V17_Tuerca_M4_J2','Acero','42.8 mm','8.1 mm','4 mm','3.2 mm'),
             ('V17_Arandela_M4_J2_frente','Acero','42 mm','9 mm','4.3 mm','0.8 mm')]
    for name,mat,y,od,id_,h in configs:
      c=piece17(g,name,mat);ring(c,name,'Y',y,x,z,od,id_,h,'steel');sts17_pattern4(g,c,name+'_cuatro')
    # Tres arandelas DIN125 por tornillo evitan puntas en el engrane sin
    # pedir tornillos cortados a longitud especial.
    for i in range(3):
      c=piece17(g,'V17_Arandela_M4_J2_dorso_'+str(i),'Acero')
      ring(c,'DIN125_REF','Y',f'{22.1+.8*i:.1f} mm',x,z,'9 mm','4.3 mm','0.8 mm','steel');sts17_pattern4(g,c,'Cuatro_arandelas_dorso_'+str(i))
    c=piece17(g,'V17_M4x25_J2_ISO7380','Acero')
    ring(c,'Vastago','Y','22.1 mm',x,z,'4 mm',None,'25 mm','steel')
    ring(c,'Cabeza_baja_REF','Y','19.9 mm',x,z,'7.6 mm',None,'2.2 mm','steel')
    join_all(c,'M4x25_REF');sts17_pattern4(g,c,'Cuatro_tornillos_J2')


def act17_sts_J2_retencion():
    g=comp('09_Pinion_hombro')
    for name,y,od,id_,length in [('V17_Separador_J2_entrada_frente','46 mm','8 mm','5.1 mm','2 mm'),('V17_Separador_J2_entrada_dorso','65.2 mm','8 mm','5.1 mm','3.9 mm')]:
      c=piece17(g,name,'Al6061');ring(c,name,'Y',y,'0 mm','0 mm',od,id_,length,'steel')
    c=piece17(g,'V17_Prisionero_M2_J2','Acero');ring(c,'M2_REF','Z','2.3 mm','0 mm','56.5 mm','2 mm',None,'3 mm','steel')
    g=comp('02_Brazo_PVC');shaft=act17_get('E01_Eje_hombro_8mm')
    ring(shaft,'V17_Cerrar_perforacion_vieja_eje','Y','43 mm','0 mm','0 mm','3 mm',None,'6 mm','steel')
    ring(shaft,'V17_Extender_eje8_rueda72','Y','47.5 mm','0 mm','0 mm','8 mm',None,'12.5 mm','steel')
    join_all(shaft,'V17_Eje_hombro_una_pieza_mecanizada')
    ring(shaft,'V17_Rosca_M3_extremo','Y','54.7 mm','0 mm','0 mm','3 mm',None,'5.4 mm',cut=True)
    box(shaft,'V17_Chavetero_eje_hombro','3 mm','47.9 mm','-1 mm','2 mm','12.2 mm','2 mm',cut=True)
    c=piece17(g,'V17_Chaveta_hombro_2x2x12','Acero');box(c,'Chaveta','3 mm','48 mm','-1 mm','2 mm','12 mm','2 mm','steel')
    c=piece17(g,'V17_Separador8_J2_salida','Al6061');ring(c,'Separador','Y','31.5 mm','0 mm','0 mm','10.8 mm','8.2 mm','16.5 mm','steel')
    c=piece17(g,'V17_Arandela_M3_J2_salida','Acero');ring(c,'DIN9021_REF','Y','60 mm','0 mm','0 mm','9 mm','3.2 mm','0.8 mm','steel')
    c=piece17(g,'V17_M3x6_J2_salida','Acero');ring(c,'Vastago','Y','54.8 mm','0 mm','0 mm','3 mm',None,'6 mm','steel');ring(c,'Cabeza','Y','60.8 mm','0 mm','0 mm','5.5 mm',None,'3 mm','steel');join_all(c,'M3x6_REF')
    shaft.attributes.add('AFR2','fabricacion_v17','Eje8 prolongado a60mm, chavetero2mm y roscaM3 nueva. Geometria final de una pieza mecanizada, no soldar/rellenar el eje viejo.')
    act17_one(shaft)
    # Limite mecanico del codo+-90; entrada2.5veces ese recorrido. El
    # seguidor auxiliar no debe conservar un tope de un servo anterior.
    limits=R.joints.itemByName('T_J3_Correa_2a1').jointMotion.rotationLimits
    span=math.pi/2*D.userParameters.itemByName('j3_ratio').value
    limits.minimumValue=-span;limits.maximumValue=span
    limits.isMinimumValueEnabled=True;limits.isMaximumValueEnabled=True


def act17_sts_identificacion():
    # Se conservan los nombres historicos que usan las fases; numero de
    # pieza y descripcion exponen el producto REAL en propiedades/BOM.
    entries=[
      ('V17_M03_STS3250_remoto','FEETECH STS3250','12V TTL,75.5g; envolvente propia contrastada con fichaA0/STEP2021, no interiores exactos.'),
      ('V02_M06_MG90S_J6','FEETECH STS3032','J6:6V TTL,21.6g,23x12x27.5. Reemplaza referencia historicaMG90S.'),
      ('V02_M07_MG90S_Pinza','FEETECH STS3032','PinzaV7:6V TTL,21.6g. Reemplaza referencia historicaMG90S.'),
      ('V17_Pinon18_J2_Al2024','AFR2-J2-P18-M08-B5','SDP/SI S10T08M018A0505, Al2024. Adaptacion propia con chavetero2mm; confirmar mecanizado.'),
      ('V17_Rueda72_J2_Al2024','AFR2-J2-R72-M08-B8','SDP/SI S10T08M072A0508, Al2024. Adaptacion propia con chavetero2mm; confirmar mecanizado.'),
      ('V17_625_entrada_J3_delantero','SKF 625-2Z','5x16x5mm,5g por ocurrencia; compartido en modulosJ2/J3.'),
      ('V17_625_entrada_J3_trasero','SKF 625-2Z','5x16x5mm,5g por ocurrencia; compartido en modulosJ2/J3.'),
    ]
    for name,number,description in entries:
      c=act17_get(name);c.partNumber=number;c.description=description
    for name,od,wall in [('L01_PVC32_Corte_1',25,1.2),('L02_PVC32_Corte_2',20,1)]:
      c=act17_get(name);c.partNumber=f'PVC-{od}x{wall}'
      c.description=f'TuboPVC OD{od}mm,pared{wall}mm; nombrePVC32 historico, ver parametros de corte actuales.'


def act17_sts_conectores():
    c=act17_get('V17_Housing_STS3250_y_apoyo625')
    # FichaA0p6: dos5264/2.54x3P en cara posterior, hacia lado largo.
    # Ventana de acceso conservadora, NO geometria exacta del conector.
    box(c,'V17_Acceso_TTL_cara_posterior','-31 mm','-111.5 mm+j3_transverse_shift','j3_motor_z-10.5 mm','27 mm','5.2 mm','21 mm',cut=True)
    c.attributes.add('AFR2','acceso_conectores','FichaFEETECHA0p6,2puertos5264/2.54x3P traseros. Ventana27x21mm en tope posterior, ubicacion amplia inferida desde dibujo. Confirmar plug y radio de cable antes de fabricar. Se comparte enJ2/J3.')
    act17_one(c)


def act17_sts_corregir_anclajes():
    g=comp('01_Giro_base')
    names=['V17_Separador7_J2_Al','V17_Tuerca_M4_J2','V17_Arandela_M4_J2_frente']+['V17_Arandela_M4_J2_dorso_'+str(i) for i in range(3)]+['V17_M4x25_J2_ISO7380']
    patterns=['V17_Separador7_J2_Al_cuatro','V17_Tuerca_M4_J2_cuatro','V17_Arandela_M4_J2_frente_cuatro']+['Cuatro_arandelas_dorso_'+str(i) for i in range(3)]+['Cuatro_tornillos_J2']
    for name in reversed(patterns):
      f=next(f for f in g.features if f.name==name)
      if not f.deleteMe():raise RuntimeError('No se pudo retirar patron fallido '+name)
    for name in names:
      c=act17_get(name)
      for x,z in [(34,0),(0,18),(34,18)]:
        D.activateRootComponent();occ(g.name).activate()
        m=adsk.core.Matrix3D.create();m.translation=adsk.core.Vector3D.create(x/10,0,z/10)
        new=g.occurrences.addExistingComponent(c,m);new.isGroundToParent=True
        proxy=new.createForAssemblyContext(occ(g.name))
        inverse=occ(g.name).transform2;inverse.invert()
        p=P(0,0,0);p.transformBy(proxy.transform2);p.transformBy(inverse)
        if p.distanceTo(P(x/10,0,z/10))>1e-5:raise RuntimeError('Fijacion fuera del soporte '+name+' '+str(p.asArray()))
      c.attributes.add('AFR2','instancias_J2','4ocurrencias fijadas a01_Giro_base, paso fijo34x18mm. Altura del cuerpo depende de v17_j2_input_z; al cambiar altura del hombro siguen todas. Paso de montaje comercial fijo, editable por posicion inicial de cada ocurrencia.')
      progress('cuatro fijaciones '+name)


def act17_sts_corregir_conectores():
    c=act17_get('V17_Housing_STS3250_y_apoyo625')
    f=next(f for f in c.features if f.name=='V17_Acceso_TTL_cara_posterior')
    if not f.deleteMe():raise RuntimeError('No se pudo retirar corte sin objetivo')
    original=next(o for o in R.allOccurrences if o.component==c and o.fullPathName.startswith('02_Brazo_PVC:'))
    original.activate()
    target=c.bRepBodies.item(0)
    before=sum(b.volume for b in c.bRepBodies)
    tool=box(c,'V17_Herramienta_TTL_nativa','-31 mm','-111.5 mm+j3_transverse_shift','j3_motor_z-10.5 mm','27 mm','5.2 mm','21 mm').bodies.item(0)
    tools=adsk.core.ObjectCollection.create();tools.add(tool)
    ip=c.features.combineFeatures.createInput(target,tools);ip.operation=OP.CutFeatureOperation;ip.isKeepToolBodies=False
    f=c.features.combineFeatures.add(ip);f.name='V17_Acceso_TTL_nativo'
    after=sum(b.volume for b in c.bRepBodies)
    if f.healthState!=adsk.fusion.FeatureHealthStates.HealthyFeatureHealthState or before-after<1e-4:
      raise RuntimeError('La ventana TTL no retiro material del alojamiento nativo: '+f.errorOrWarningMessage)
    act17_one(c)


def act17_sts_despejes_J2_J3():
    c=act17_get('A01_Plataforma_giratoria_PETG')
    # La torre anterior ocupa parte del dorso del travesano nuevo. Abrir
    # alojamientos accesibles: no dejar arandelas incrustadas en plastico.
    for x in ['-17 mm','17 mm']:
      for z in ['v17_j2_input_z-8 mm','v17_j2_input_z+10 mm']:
        ring(c,'V17_Asiento_dorso_M4_J2','Y','19.7 mm',x,z,'9.4 mm',None,'4.8 mm',cut=True)
    # En125grados el extremo positivo del travesano invade el encastre
    # del codo. Recortar aX29, conservando7.5mm hasta borde de arandela.
    box(c,'V17_Despeje_codo_en_hombro125','29 mm','24.4 mm','v17_j2_input_z-17.1 mm','5.1 mm','8.2 mm','33.7 mm',cut=True)
    act17_one(c)
    # El puente trasero anterior entraba0.2mm en esquinas del frente del
    # servo. Acortarlo0.5mm, manteniendo fijo su extremo junto al625.
    housing=act17_get('V17_Housing_STS3250_y_apoyo625')
    sk=housing.sketches.itemByName('Croquis_Puente_pod_trasero')
    dy=sk.sketchDimensions.item(1).parameter;length=sk.sketchDimensions.item(3).parameter
    if abs(dy.value-val('71 mm'))>1e-6 or abs(length.value-val('14.9 mm'))>1e-6:
      raise RuntimeError('Puente625 distinto del croquis esperado: '+str((dy.value,length.value)))
    if not D.modifyParameters([dy,length],[V('70.5 mm'),V('14.4 mm')]):raise RuntimeError('No se pudo acortar puente625')
    act17_one(housing)


def sts17_box_cut_native(c,name,x,y,z,w,d,h,root):
    next(o for o in R.allOccurrences if o.component==c and o.fullPathName.startswith(root+':')).activate()
    target=c.bRepBodies.item(0);before=sum(b.volume for b in c.bRepBodies)
    tool=box(c,name+'_herramienta',x,y,z,w,d,h).bodies.item(0)
    tools=adsk.core.ObjectCollection.create();tools.add(tool)
    ip=c.features.combineFeatures.createInput(target,tools);ip.operation=OP.CutFeatureOperation;ip.isKeepToolBodies=False
    f=c.features.combineFeatures.add(ip);f.name=name
    if f.healthState!=adsk.fusion.FeatureHealthStates.HealthyFeatureHealthState or before-sum(b.volume for b in c.bRepBodies)<1e-7:
      raise RuntimeError('Corte nativo sin material retirado '+name+': '+f.errorOrWarningMessage)
    act17_one(c)


def act17_sts_planos_collares():
    # Sustituir penetracion nominal de las puntas por caras planas reales.
    # Los prisioneros apoyan; la transmision principal sigue por chavetas.
    c=act17_get('V17_Eje_entrada_5mm')
    sts17_box_cut_native(c,'V17_Plano_M2_collar5','-3 mm','-37 mm+j3_transverse_shift','2.3 mm','6 mm','4 mm','1 mm','12_Polea_entrada_J3')
    c=act17_get('V02_E02_Eje_y_brida_codo')
    sts17_box_cut_native(c,'V17_Plano_M3_collar8','-5 mm','-58.8 mm+j3_transverse_shift','3.5 mm','10 mm','4 mm','1.5 mm','03_Antebrazo_PVC')
