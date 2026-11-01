"""J3 remoto, sobre copia independiente de la arquitectura V7/v09.

Fases explicitas. Conserva las articulaciones y pinza existentes.
Las envolventes de productos comerciales no son planos de fabricacion.
"""
from pathlib import Path
BASE17=Path(globals().get('AFR2_REPO_ROOT',r'D:\ProyectoRobotica2'))/'AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo'
exec(compile((BASE17/'revise_J1_v09.py').read_text(encoding='utf-8'),'revise_J1_v09.py','exec'))
HERE17=BASE17/'revision_v17'
NAME17='AFR2_Brazo_v17_Base_V7_J3_Remoto'

def progress(message):
    import datetime
    with open(HERE17/'progreso_J3.jsonl','a',encoding='utf-8') as f:
        f.write(json.dumps(dict(message=message,utc=datetime.datetime.now(datetime.timezone.utc).isoformat()))+'\n')


def phase17_modo_trabajo():
    viewport=adsk.core.Application.get().activeViewport
    if not D.attributes.itemByName('AFR2','visual_style_before_work'):
      D.attributes.add('AFR2','visual_style_before_work',str(int(viewport.visualStyle)))
    viewport.visualStyle=adsk.core.VisualStyles.ShadedVisualStyle
    existing=D.attributes.itemByName('AFR2','reference_visibility_before_work')
    if existing:state=json.loads(existing.value)
    else:
      state={o.component.name:o.isLightBulbOn for o in R.occurrences if o.component.name.startswith(('80_','81_','82_'))}
      D.attributes.add('AFR2','reference_visibility_before_work',json.dumps(state))
    for o in R.occurrences:
      if o.component.name in state:o.isLightBulbOn=False
    print('Referencias ocultas temporalmente para reducir coste grafico; geometria y anclajes conservados.')


def phase17_restaurar_vista():
    attr=D.attributes.itemByName('AFR2','reference_visibility_before_work')
    if not attr:raise RuntimeError('No hay estado anterior de visibilidad')
    state=json.loads(attr.value)
    for o in R.occurrences:
      if o.component.name in state:o.isLightBulbOn=state[o.component.name]
    visual=D.attributes.itemByName('AFR2','visual_style_before_work')
    if visual:adsk.core.Application.get().activeViewport.visualStyle=int(visual.value)
    print('Visibilidad de referencias restaurada.')

def piece17(parent,name,material='PLA PrintALot',mass=None):
    c=sub(parent,name);c.attributes.add('AFR2','material_v17',material)
    if mass is not None:c.attributes.add('AFR2','catalog_mass_g',str(mass))
    return c

def block17(parent,name,x,y,z,w,d,h,material='Al6061'):
    c=piece17(parent,name,material);box(c,name,x,y,z,w,d,h,'red' if material.startswith('PLA') else 'steel');return c

def slot17(c,name,x,low,high,y,depth,width='4.5 mm'):
    """Tres cortes parametricos producen una oblonga exacta.

    Evita restricciones inferidas entre centros de arcos repetidos por Fusion.
    Los circulos de los extremos y la franja central forman una unica ranura.
    """
    for tag,z in [('inferior',low),('superior',high)]:
        ring(c,name+'_'+tag,'Y',y,x,z,width,None,depth,cut=True)
    f=box(c,name+'_recto','('+x+')-('+width+')/2',y,low,width,depth,'('+high+')-('+low+')',cut=True)
    c.attributes.add('AFR2','ranuras','Oblongas paralelas al primer eslabon; no elipses. Aflojar todas las fijaciones del carro antes de moverlo.')
    return f

def bearing17(g,name,y,z):
    c=piece17(g,name,'Rodamiento SKF625-2Z',5)
    ring(c,'625_2Z_5x16x5','Y',y,'0 mm',z,'16 mm','5 mm','5 mm','steel')
    c.attributes.add('AFR2','catalog','SKF625-2Z:5x16x5; C0=380N, C=1100N, masa5g. No mezclar capacidades EZO/SKF. Da_max13.6, da7.4..8.3mm.')
    return c

def locate17(s,point,x,y):
    # Fusion puede inferir alineacion o coincidencia entre centros de arcos.
    # No agregar dos veces la misma restriccion; se conserva la dimension
    # conductora del otro centro cuando ambos ya estan relacionados.
    for expression,direction in [(x,ORI.HorizontalDimensionOrientation),(y,ORI.VerticalDimensionOrientation)]:
      try:
        if abs(val(expression))<1e-9:
          if direction==ORI.HorizontalDimensionOrientation:s.geometricConstraints.addVerticalPoints(s.originPoint,point)
          else:s.geometricConstraints.addHorizontalPoints(s.originPoint,point)
        else:
          dim=s.sketchDimensions.addDistanceDimension(s.originPoint,point,direction,P(.7,.7,0))
          dim.parameter.expression=expression if val(expression)>0 else '-('+expression+')'
      except RuntimeError as error:
        if 'OVER_CONSTRAINT' not in str(error):raise

def pulley17(g,name,teeth,y,z,bore,hub=True):
    c=piece17(g,name,'Al6061')
    dp=f'{teeth*3/math.pi:.12f} mm'
    ring(c,'Disco_macizo_conservador','Y',y,'0 mm',z,dp,bore,'9 mm','steel')
    fd=f'{math.ceil(teeth*3/math.pi+2):.0f} mm'
    for yy in [f'({y})-0.8 mm',f'({y})+9 mm']:
        ring(c,'Pestana','Y',yy,'0 mm',z,fd,bore,'0.8 mm','steel')
    if hub:ring(c,'Cubo','Y',f'({y})+9.8 mm','0 mm',z,'16 mm' if teeth>25 else '10 mm',bore,'6 mm','steel')
    join_all(c,'Polea_aluminio_'+str(teeth)+'T_REF')
    c.attributes.add('AFR2','comercial',f'HTD3M {teeth}T, cara9 para correa6. Disco a diametro primitivo: sobreestima masa, sin dentado comercial. No fabricar desde esta envolvente.')
    return c

def phase17_parametros():
    # Se instala primero sobre la copia larga; el aligerado requiere su propia
    # fase y una nueva extraccion de masas. No se sustituye CAD por presupuesto.
    for n,e,u,co in [
      ('j3_belt_pitch','3 mm','mm','Paso HTD3M.'),
      ('j3_belt_length','300 mm','mm','Longitud primitiva de referencia para eslabon130; cambiar a210 al integrar eslabon70.'),
      ('j3_small_teeth','20','','Polea motriz.'),('j3_large_teeth','40','','Polea conducida.'),
      ('j3_ratio','j3_large_teeth/j3_small_teeth','','Reduccion externa2:1.'),
      ('j3_rsmall',f'j3_belt_pitch*j3_small_teeth/{2*math.pi:.15f}','mm','Radio primitivo.'),
      ('j3_rlarge',f'j3_belt_pitch*j3_large_teeth/{2*math.pi:.15f}','mm','Radio primitivo.'),
      ('j3_length_A',f'j3_belt_length-{math.pi:.15f}*(j3_rsmall+j3_rlarge)','mm','Termino de formula aproximada de longitud abierta.'),
      ('j3_centre','(j3_length_A+sqrt(j3_length_A*j3_length_A-8*(j3_rlarge-j3_rsmall)*(j3_rlarge-j3_rsmall)))/4','mm','Centro nominal por formula de longitud; ajuste final por tensor, no correa estirada.'),
      ('j3_slide','0 mm','mm','Desplazamiento del carro: -2..+6mm. Positivo acerca poleas y afloja.'),
      ('j3_motor_z','link1-j3_centre+j3_slide','mm','Motor, eje, apoyo y polea se trasladan juntos.'),
      ('j3_fixed_z','link1-j3_centre','mm','Posicion nominal de ranuras, no se desplaza con carro.'),
      ('j3_pulley_y','-49 mm','mm','Plano exterior, alejado de horquillas.'),
      ('v17_min_margin','1.2','','USUARIO: minimo de preseleccion de actuadores; no aprobacion fisica.')]:
        param(n,e,u,co)
    D.attributes.add('AFR2','tensor_J3','Carro completo, cuatro M4 en ranuras oblongas; motor y apoyo coaxial se desplazan juntos. Codo fijo.')

def phase17_motor():
    g=comp('02_Brazo_PVC')
    c=gc('V07_M03_GX3270BLS_Codo');discard_geometry(c);c.name='V17_M03_STS3250_remoto'
    c.attributes.add('AFR2','material_v17','Servo');c.attributes.add('AFR2','catalog_mass_g','75.5')
    box(c,'Cuerpo_STS3250','-32.72 mm','-103.4 mm','j3_motor_z-12.36 mm','45.22 mm','35 mm','24.72 mm','servo')
    ring(c,'Spline25T_REF','Y','-68.4 mm','0 mm','j3_motor_z','5.9 mm',None,'3.4 mm','brass')
    c.attributes.add('AFR2','datasheet','FEETECH STS3250 A0 2024-01-16:45.22x24.72x35; eje12.5 desde extremo corto. Sin uso estructural de tornillos de cierre M2.')
    # Se retira el material del antiguo alojamiento fuera de la horquilla.
    c=gc('A06_Encastre_distal_brazo_PETG')
    box(c,'Retirar_cuna_motor_distal','-45 mm','fork_y+fork_t/2+0.1 mm','link1-52 mm','100 mm','90 mm','80 mm',cut=True)
    # Herrajes y horn de accionamiento directo quedan obsoletos.
    for root in ['02_Brazo_PVC','03_Antebrazo_PVC']:
      for o in list(comp(root).occurrences):
        if o.component.name.startswith(('V07_Inserto_M3_codo','V07_Tornillo_M3x10_codo','V07_Arandela_codo','V03_Horn_movil_V02_M03','V03_Tornillo_union_V02_M03','V03_Tornillo_original_horn_V02_M03')):o.deleteMe()

def phase17_placa():
    g=comp('02_Brazo_PVC')
    fixed=block17(g,'V17_Placa_fija_tensor_Al','-34 mm','-35.5 mm','j3_fixed_z-19 mm','68 mm','2.5 mm','46 mm')
    for xx in [-17,17]:
      for zz in [-10,10]:
        slot17(fixed,'Ranura_M4_'+str(xx)+'_'+str(zz),f'{xx} mm',f'j3_fixed_z+({zz})*1 mm-2 mm',f'j3_fixed_z+({zz})*1 mm+6 mm','-35.6 mm','2.7 mm')

def phase17_apoyos():
    # Dos 625 externos con polea entre apoyos. Solo el motor se retrasa5mm;
    # los valores de los componentes siguientes ya usan esta reserva.
    param('j3_pulley_y','-54 mm',comment='Polea entre dos apoyos625; deja libre la horquilla del hombro.')
    param('j3_servo_y_offset','-5 mm',comment='Traslado del servo para liberar el acople respecto del primer625 externo.')
    c=gc('V17_M03_STS3250_remoto')
    next(o for o in R.allOccurrences if o.component==c).activate()
    bodies=adsk.core.ObjectCollection.create()
    for b in c.bRepBodies:bodies.add(b)
    ip=c.features.moveFeatures.createInput2(bodies)
    ip.defineAsTranslateXYZ(V('0 mm'),V('j3_servo_y_offset'),V('0 mm'),False)
    c.features.moveFeatures.add(ip).name='V17_Reserva_segundo_apoyo_entrada'

def phase17_carro():
    g=comp('02_Brazo_PVC')
    carrier=block17(g,'V17_Carro_motor_apoyo_Al','-24 mm','-38 mm','j3_motor_z-16 mm','48 mm','2.5 mm','34 mm')
    # Escuadra horizontal solidaria al carro; carcasa retenida por forma,
    # sin pretender roscar sobre tornillos de cierre del servo.
    box(carrier,'Ala_motor','-41 mm','-111.4 mm','j3_motor_z-16 mm','63 mm','75.9 mm','2.5 mm','steel')
    join_all(carrier,'Carro_Al_continuo')
    for xx in [-17,17]:
      for zz in [-10,10]:
        z=f'j3_motor_z+({zz})*1 mm'
        ring(carrier,'Paso_M4_'+str(xx)+'_'+str(zz),'Y','-38.1 mm',f'{xx} mm',z,'4.5 mm',None,'2.7 mm',cut=True)
        b=piece17(g,'V17_M4_carro_'+str(xx)+'_'+str(zz),'Acero')
        ring(b,'Vastago_M4','Y','-39 mm',f'{xx} mm',z,'4 mm',None,'12 mm','steel')
        ring(b,'Cabeza_M4_REF','Y','-42 mm',f'{xx} mm',z,'7 mm',None,'3 mm','steel');join_all(b,'Tornillo_M4_REF')
        for label,y in [('Exterior','-39 mm'),('Interior','-33 mm')]:
          w=piece17(g,'V17_Arandela_carro_'+str(xx)+'_'+str(zz)+'_'+label,'Acero')
          ring(w,'Arandela_M4','Y',y,f'{xx} mm',z,'10 mm','4.3 mm','1 mm','steel')
        n=piece17(g,'V17_Tuerca_carro_'+str(xx)+'_'+str(zz),'Acero')
        ring(n,'Tuerca_M4_REF','Y','-32 mm',f'{xx} mm',z,'7 mm','4 mm','3.2 mm','steel')

def phase17_housing():
    g=comp('02_Brazo_PVC');carrier=gc('V17_Carro_motor_apoyo_Al')
    # Housing rojo de una sola pieza; contacta sobre el ala de aluminio.
    h=piece17(g,'V17_Housing_STS3250_y_apoyo625','PLA PrintALot')
    box(h,'Piso_liner','-35.72 mm','-111.4 mm','j3_motor_z-13.5 mm','51.22 mm','41 mm','1.14 mm')
    for x in ['-35.72 mm','13.1 mm']:
      box(h,'Lateral',x,'-111.4 mm','j3_motor_z-13.5 mm','2.62 mm','41 mm','29 mm')
    for y in ['-111.4 mm','-72.8 mm']:
      box(h,'Tope', '-35.72 mm',y,'j3_motor_z-13.5 mm','51.44 mm','2.4 mm','26.5 mm')
    # Salida delantera despejada de spline y acople.
    ring(h,'Paso_salida','Y','-73 mm','0 mm','j3_motor_z','14 mm',None,'3.3 mm',cut=True)
    ring(h,'Pod625_trasero','Y','-61.1 mm','0 mm','j3_motor_z','23 mm','16.1 mm','5 mm')
    ring(h,'Hombro625_trasero','Y','-63.1 mm','0 mm','j3_motor_z','23 mm','13.5 mm','2 mm')
    for xx in ['-16 mm','10 mm']:
      box(h,'Boss_retencion625',xx,'-60 mm','j3_motor_z-3 mm','6 mm','3.9 mm','6 mm')
    ring(h,'Pod625_delantero','Y','-42 mm','0 mm','j3_motor_z','23 mm','16.1 mm','4 mm')
    ring(h,'Hombro625_delantero','Y','-44 mm','0 mm','j3_motor_z','23 mm','13.5 mm','2 mm')
    box(h,'Puente_pod_trasero','-14.5 mm','-71 mm','j3_motor_z-13.5 mm','29 mm','15.5 mm','4 mm')
    box(h,'Puente_pod_delantero','-14.5 mm','-44 mm','j3_motor_z-13.5 mm','29 mm','2 mm','4 mm')
    for xx in ['-14.5 mm','12 mm']:
      box(h,'Larguero_exterior_polea',xx,'-60.5 mm','j3_motor_z-13.5 mm','2.5 mm','18.5 mm','7 mm')
    join_all(h,'Housing_y_apoyo_integrados')
    # Tapa separada, apoyada sobre los laterales, con paso M3.
    for xx in ['-40.9 mm','13.1 mm']:
      for yy in ['-109 mm','-84 mm']:
        box(h,'Pilar_tapa',xx,yy,'j3_motor_z-13.5 mm','7.8 mm','8 mm','29 mm')
    join_all(h,'Housing_con_pilares_unidos')
    cap=block17(g,'V17_Tapa_housing_STS3250','-41 mm','-111.4 mm','j3_motor_z+15.5 mm','63 mm','41 mm','2 mm','PLA PrintALot')
    for yy in ['-103 mm','-83 mm']:
      box(cap,'Patin_superior','-30 mm',yy,'j3_motor_z+12.36 mm','38 mm','3 mm','3.14 mm')
    join_all(cap,'Tapa_y_patines')
    for xx in ['-37 mm','17 mm']:
      for yy in ['-105 mm','-80 mm']:
        ring(cap,'Paso_tapa','Z','j3_motor_z+15.4 mm',xx,yy,'3.4 mm',None,'2.2 mm',cut=True)
        ring(h,'Paso_pilar','Z','j3_motor_z-13.6 mm',xx,yy,'3.4 mm',None,'29.2 mm',cut=True)
        ring(carrier,'Paso_tapa_Al','Z','j3_motor_z-16.1 mm',xx,yy,'3.4 mm',None,'2.7 mm',cut=True)
    bearing17(g,'V17_625_entrada_J3_trasero','-61.1 mm','j3_motor_z')
    bearing17(g,'V17_625_entrada_J3_delantero','-42 mm','j3_motor_z')
    ring(carrier,'Paso_eje_con_hombro625','Y','-38.1 mm','0 mm','j3_motor_z','13.5 mm',None,'2.7 mm',cut=True)
    ring(carrier,'Asiento_final625','Y','-38.1 mm','0 mm','j3_motor_z','16.1 mm',None,'1.1 mm',cut=True)

def phase17_anclajes():
    g=comp('02_Brazo_PVC');fixed=gc('V17_Placa_fija_tensor_Al')
    # Estos anclajes se integran con el encastre proximal, evitando soportes
    # flotantes. El diseño aligerado debe conservar la misma union.
    host=gc('A05_Encastre_proximal_brazo_PETG')
    box(host,'V17_Travesano_anclajes_tensor','-34 mm','-23 mm','j3_fixed_z-8 mm','68 mm','10 mm','16 mm')
    for xx in [-29,29]:
      box(host,'V17_Puente_tensor_'+str(xx),f'{xx-5} mm','-33 mm','j3_fixed_z-8 mm','10 mm','20 mm','16 mm')
    join_all(host,'Encastre_y_anclajes_tensor_unidos')
    for xx in [-29,29]:
      # Dos fijaciones fijas distintas de las cuatro ranuras de ajuste.
      z='j3_fixed_z'
      ring(fixed,'Fijacion_encastre_'+str(xx),'Y','-35.6 mm',f'{xx} mm',z,'4.5 mm',None,'2.7 mm',cut=True)
      ring(host,'Paso_anclaje_tensor_'+str(xx),'Y','-33.1 mm',f'{xx} mm',z,'4.5 mm',None,'20.2 mm',cut=True)
      bolt=piece17(g,'V17_M4_anclaje_fijo_'+str(xx),'Acero')
      ring(bolt,'Vastago','Y','-36.3 mm',f'{xx} mm',z,'4 mm',None,'30 mm','steel')
      ring(bolt,'Cabeza_REF','Y','-39.3 mm',f'{xx} mm',z,'7 mm',None,'3 mm','steel');join_all(bolt,'M4x30_REF')
      for label,yy in [('externa','-36.3 mm'),('interna','-13 mm')]:
        washer=piece17(g,'V17_Arandela_anclaje_fijo_'+str(xx)+'_'+label,'Acero')
        ring(washer,'Arandela_M4_DIN125_REF','Y',yy,f'{xx} mm',z,'9 mm','4.3 mm','0.8 mm','steel')
    gc('V17_Housing_STS3250_y_apoyo625').attributes.add('AFR2','movimiento','Carro, housing, motor y apoyo de entrada dependen de j3_slide. Todos los puntos de fijacion ajustables sobre placa fija son oblongos.')

def phase17_poleas():
    j3=R.joints.itemByName('J3_Codo');original_angle=j3.jointMotion.rotationValue
    j3.jointMotion.rotationValue=0
    g=comp('02_Brazo_PVC');rot=main('12_Polea_entrada_J3').component
    origin(g,'V17_EJE_ENTRADA_J3','0 mm','0 mm','j3_motor_z','Y')
    origin(rot,'EJE_ENTRADA','0 mm','0 mm','0 mm','Y')
    # El rotor se une al primer eslabon mediante su eje de entrada; sus
    # coordenadas locales tienen origen en la salida, por eso Z=0.
    pulley17(rot,'V17_Polea_20T_J3',20,'j3_pulley_y','0 mm','5 mm',hub=False)
    sh=piece17(rot,'V17_Eje_entrada_5mm','Acero')
    ring(sh,'Eje','Y','-65.9 mm','0 mm','0 mm','5 mm',None,'33.4 mm','steel')
    cp=piece17(rot,'V17_Acople25T_a_eje5','Al6061')
    ring(cp,'Cuerpo','Y','-73.4 mm','0 mm','0 mm','12 mm',None,'11.8 mm','steel')
    ring(cp,'Spline25T_ENVOLVENTE','Y','-73.5 mm','0 mm','0 mm','5.9 mm',None,'3.5 mm',cut=True)
    ring(cp,'Paso5','Y','-65.9 mm','0 mm','0 mm','5 mm',None,'4.4 mm',cut=True)
    ring(cp,'Paso_tornillo_servo_M3','Y','-73.5 mm','0 mm','0 mm','3.3 mm',None,'7.7 mm',cut=True)
    ring(cp,'Cajeado_cabeza_M3','Y','-69.2 mm','0 mm','0 mm','6 mm',None,'3.3 mm',cut=True)
    cp.attributes.add('AFR2','interfaz','Acople metalico25T5.9 especifico, no fabricar spline desde agujero cilindrico. Requiere tornillos radiales/prisioneros antes de liberacion.')
    out=comp('03_Antebrazo_PVC');pulley17(out,'V17_Polea_40T_J3',40,'j3_pulley_y','0 mm','8 mm')
    s=gc('V02_E02_Eje_y_brida_codo')
    ring(s,'V17_Extension_eje8_continua','Y','-60 mm','0 mm','0 mm','8 mm',None,'25 mm','steel');join_all(s,'Eje_codo_extendido')
    # Conexion cinematica abierta: poleas giran en el mismo sentido, 2:1.
    j=make_joint('T_J3_Correa_2a1','12_Polea_entrada_J3','EJE_ENTRADA','02_Brazo_PVC','V17_EJE_ENTRADA_J3','Y',limits=(-180,180))
    ml=R.motionLinks.createInput(R.joints.itemByName('J3_Codo'),j)
    ml.valueOne=V('1 deg');ml.valueTwo=V('j3_ratio*1 deg');ml.isReversed=False
    R.motionLinks.add(ml).name='V17_Transmision_J3_correa_2a1'
    j3.jointMotion.rotationValue=original_angle

def phase17_correa():
    g=comp('02_Brazo_PVC')
    c=piece17(g,'V17_Correa_OMEGA_HP_3M_6','Correa comercial',val('j3_belt_length')*10*.0144)
    s=c.sketches.add(datum(c,'Y','j3_pulley_y+1.5 mm','Plano_correa'))
    low=val('j3_motor_z');high=val('link1');r=val('j3_rsmall');big=val('j3_rlarge')
    aa=math.asin((big-r)/(high-low))
    for offset,expr in [(-.11,'-1.1 mm'),(.12,'+1.2 mm')]:
      rr=r+offset;RR=big+offset
      a=s.sketchCurves.sketchArcs.addByCenterStartSweep(P(0,-low,0),P(rr*math.cos(aa),-low+rr*math.sin(aa),0),math.pi-2*aa)
      b=s.sketchCurves.sketchArcs.addByCenterStartSweep(P(0,-high,0),P(-RR*math.cos(aa),-high+RR*math.sin(aa),0),math.pi+2*aa)
      l=s.sketchCurves.sketchLines.addByTwoPoints(a.endSketchPoint,b.startSketchPoint)
      k=s.sketchCurves.sketchLines.addByTwoPoints(b.endSketchPoint,a.startSketchPoint)
      for arc,z,rad in [(a,'j3_motor_z','j3_rsmall'),(b,'link1','j3_rlarge')]:
        locate17(s,arc.centerSketchPoint,'0 mm','-('+z+')')
        s.sketchDimensions.addRadialDimension(arc,P(.7,-val(z),0)).parameter.expression=rad+expr
      for line,arc in [(l,a),(l,b),(k,a),(k,b)]:
        try:s.geometricConstraints.addTangent(line,arc)
        except RuntimeError as err:
          if 'OVER_CONSTRAINT' not in str(err):raise
    profiles=[pr for pr in s.profiles if pr.profileLoops.count==2]
    if len(profiles)!=1:raise RuntimeError('Perfil de correa ambiguo; no se genera un bloque macizo')
    f=extrude(c,s,'6 mm',OP.NewBodyFeatureOperation,'Correa_sin_dientes_REF',profiles[0])
    for b in f.bodies:b.appearance=AP['rubber']
    c.attributes.add('AFR2','alcance','OMEGA HP3M6; contorno liso de referencia. Contacto con poleas intencional. Tensado real no se deduce del cambio de longitud de esta envolvente CAD.')

def phase17_tensor():
    g=comp('02_Brazo_PVC');fixed=gc('V17_Placa_fija_tensor_Al')
    box(fixed,'Extension_superior_tensor','-7 mm','-35.5 mm','j3_fixed_z+25 mm','14 mm','2.5 mm','6 mm','steel')
    box(fixed,'Ala_tornillo_tensor','-7 mm','-45 mm','j3_fixed_z+28 mm','14 mm','12 mm','3 mm','steel')
    join_all(fixed,'Placa_y_escuadra_tensor_unidas')
    ring(fixed,'Paso_tensor_M3','Z','j3_fixed_z+27.9 mm','0 mm','-36.75 mm','3.4 mm',None,'3.2 mm',cut=True)
    nut=piece17(g,'V17_Tuerca_tensor_M3','Acero')
    ring(nut,'Tuerca_M3_REF','Z','j3_fixed_z+31 mm','0 mm','-36.75 mm','5.5 mm','3 mm','2.4 mm','steel')
    screw=piece17(g,'V17_Tornillo_ajuste_M3x20','Acero')
    ring(screw,'Vastago_M3','Z','j3_motor_z+18 mm','0 mm','-36.75 mm','3 mm',None,'20 mm','steel')
    ring(screw,'Cabeza_REF','Z','j3_motor_z+38 mm','0 mm','-36.75 mm','5.5 mm',None,'3 mm','steel');join_all(screw,'Tensor_M3x20_REF')
    fixed.attributes.add('AFR2','operacion_tensor','Aflojar cuatro M4, retroceder M3 para instalar, tensar con M3 empujando carro lejos del codo y reapretar M4. Recorrido de diseno -2..+6mm. No aflojar acople ni eje de codo: todo el apoyo de entrada acompaña el carro.')

def phase17_fijaciones():
    g=comp('02_Brazo_PVC');out=comp('03_Antebrazo_PVC')
    for o in list(out.occurrences):
      if o.component.name.startswith(('V03_Collar_8mm_retencion','V03_Prisionero_M3x5_collar','V03_Separador_8mm_trasero')):o.deleteMe()
    # Chaveta2x2: transmite el par entre eje del codo y polea, sin depender
    # de la friccion de un cilindro suelto. El hub queda unido al antebrazo.
    sh=gc('V02_E02_Eje_y_brida_codo');po=gc('V17_Polea_40T_J3')
    box(sh,'Chavetero_eje','3 mm','-54 mm','-1 mm','2 mm','15 mm','2 mm',cut=True)
    box(po,'Chavetero_polea','3 mm','-54.1 mm','-1 mm','2 mm','16 mm','2 mm',cut=True)
    key=block17(out,'V17_Chaveta_2x2x15','3 mm','-54 mm','-1 mm','2 mm','15 mm','2 mm','Acero')
    spacer=piece17(out,'V17_Separador_codo_trasero','Al6061')
    ring(spacer,'Separador','Y','-38.2 mm','0 mm','0 mm','11 mm','8 mm','6.7 mm','steel')
    collar=piece17(out,'V17_Collar_retencion_codo','Acero')
    ring(collar,'Collar','Y','-58.8 mm','0 mm','0 mm','12 mm','8 mm','4 mm','steel')
    ring(collar,'Rosca_prisionero','Z','3.5 mm','0 mm','-56.8 mm','2.5 mm',None,'3 mm',cut=True)
    screw=piece17(out,'V17_Prisionero_collar_codo','Acero')
    ring(screw,'Prisionero_M3_REF','Z','3.5 mm','0 mm','-56.8 mm','3 mm',None,'2.5 mm','steel')
    rotor=comp('12_Polea_entrada_J3');cp=gc('V17_Acople25T_a_eje5')
    ring(cp,'Rosca_prisionero5','Z','1.9 mm','0 mm','-64 mm','2.5 mm',None,'4.2 mm',cut=True)
    sr=piece17(rotor,'V17_Prisionero_acople5','Acero')
    ring(sr,'Prisionero_M3_REF','Z','2 mm','0 mm','-64 mm','3 mm',None,'4 mm','steel')
    pin=gc('V17_Polea_20T_J3');shaft_in=gc('V17_Eje_entrada_5mm')
    for c in [pin,shaft_in]:
      box(c,'Chavetero_entrada2','1.5 mm','-54 mm','-1 mm','2 mm','9 mm','2 mm',cut=True)
    block17(rotor,'V17_Chaveta_entrada_2x2x9','1.5 mm','-54 mm','-1 mm','2 mm','9 mm','2 mm','Acero')
    for label,y,w in [('trasero','-55.5 mm','0.7 mm'),('delantero','-44.2 mm','2.2 mm')]:
      spacer=piece17(rotor,'V17_Separador_entrada_'+label,'Al6061')
      ring(spacer,'Separador','Y',y,'0 mm','0 mm','8 mm','5 mm',w,'steel')
    collar_in=piece17(rotor,'V17_Collar_entrada5','Acero')
    ring(collar_in,'Collar','Y','-37 mm','0 mm','0 mm','10 mm','5 mm','4 mm','steel')
    ring(collar_in,'Rosca_M2','Z','2.3 mm','0 mm','-35 mm','1.6 mm',None,'3 mm',cut=True)
    sr=piece17(rotor,'V17_Prisionero_collar_entrada5','Acero')
    ring(sr,'M2_REF','Z','2.3 mm','0 mm','-35 mm','2 mm',None,'2.7 mm','steel')
    sr=piece17(rotor,'V17_Tornillo_central_servo_J3','Acero')
    ring(sr,'Vastago_M3','Y','-75.2 mm','0 mm','0 mm','3 mm',None,'6 mm','steel')
    ring(sr,'Cabeza_M3_REF','Y','-69.2 mm','0 mm','0 mm','5.5 mm',None,'3 mm','steel');join_all(sr,'Tornillo25T_REF')
    for xx in ['-37 mm','17 mm']:
      for yy in ['-105 mm','-80 mm']:
        sr=piece17(g,'V17_M3_tapa_'+xx+'_'+yy,'Acero')
        ring(sr,'Vastago','Z','j3_motor_z-22.5 mm',xx,yy,'3 mm',None,'40 mm','steel')
        ring(sr,'Cabeza_REF','Z','j3_motor_z+17.5 mm',xx,yy,'5.5 mm',None,'3 mm','steel');join_all(sr,'M3x40_REF')
        nut=piece17(g,'V17_Tuerca_tapa_'+xx+'_'+yy,'Acero')
        ring(nut,'Tuerca_M3_REF','Z','j3_motor_z-18.4 mm',xx,yy,'5.5 mm','3 mm','2.4 mm','steel')
    # Tuercas de los anclajes fijos: no forman parte de las ranuras.
    for xx in [-29,29]:
      nut=piece17(g,'V17_Tuerca_anclaje_fijo_'+str(xx),'Acero')
      ring(nut,'Tuerca_M4_REF','Y','-12.2 mm',f'{xx} mm','j3_fixed_z','7 mm','4 mm','3.2 mm','steel')
    ret=piece17(g,'V17_Tapa625_entrada_trasera','Al6061')
    ring(ret,'Retencion_aro_exterior','Y','-56.1 mm','0 mm','j3_motor_z','23 mm','13.5 mm','1 mm','steel')
    for xx in ['-16 mm','10 mm']:
      box(ret,'Oreja_retencion',xx,'-56.1 mm','j3_motor_z-3 mm','6 mm','1 mm','6 mm','steel')
    join_all(ret,'Tapa625_Al_unica')
    for xx in [-14,14]:
      ring(ret,'Paso_M2','Y','-56.2 mm',f'{xx} mm','j3_motor_z','2.4 mm',None,'1.2 mm',cut=True)
      ring(gc('V17_Housing_STS3250_y_apoyo625'),'Piloto_M2_retencion','Y','-60.1 mm',f'{xx} mm','j3_motor_z','1.6 mm',None,'4.1 mm',cut=True)
      sr=piece17(g,'V17_M2_tapa625_'+str(xx),'Acero')
      ring(sr,'Vastago_M2_REF','Y','-60.1 mm',f'{xx} mm','j3_motor_z','2 mm',None,'5 mm','steel')
      ring(sr,'Cabeza_M2_REF','Y','-55.1 mm',f'{xx} mm','j3_motor_z','3.8 mm',None,'1.6 mm','steel');join_all(sr,'M2x5_REF')

def phase17_corredor():
    """Correcciones dirigidas por interferencias_J3_inicial.json.

    El desplazamiento es lateral, sin elevar el montaje ni cambiar ejes.
    Toda la transmision se conserva coaxial mediante un parametro comun.
    """
    import re
    param('j3_transverse_shift','-9 mm',comment='Reserva axial para tapas de hombro y todas las tuercas del carro, tambien con eslabon70mm.')
    targets=[c for c in D.allComponents if c.name.startswith('V17_')]
    # Los dos apoyos siguen al mismo carro. Las arandelas/tuercas interiores
    # de anclaje permanente se conservan sobre la cara del encastre.
    for c in targets:
      if c.name.startswith('V17_Tuerca_anclaje_fijo_'):continue
      if c.name.startswith('V17_Arandela_anclaje_fijo_') and '_interna' in c.name:continue
      if not c.bRepBodies.count:continue
      next(o for o in R.allOccurrences if o.component==c).activate()
      entities=adsk.core.ObjectCollection.create()
      for body in c.bRepBodies:entities.add(body)
      ip=c.features.moveFeatures.createInput2(entities)
      ip.defineAsTranslateXYZ(V('0 mm'),V('j3_transverse_shift'),V('0 mm'),False)
      c.features.moveFeatures.add(ip).name='V17_Corredor_lateral_parametrico'
      progress('corredor: '+c.name)
    # Subir2mm la fila inferior para que las arandelas no atraviesen el ala.
    for c in targets:
      for p in c.modelParameters:
        if ('j3_motor_z' in p.expression or 'j3_fixed_z' in p.expression) and re.search(r'\(\s*-10\s*\)',p.expression):
          p.expression=re.sub(r'\(\s*-10\s*\)','(-8)',p.expression)
    # Despejar el tensor de la pared posterior de su escuadra.
    for c in [gc('V17_Tornillo_ajuste_M3x20'),gc('V17_Tuerca_tensor_M3'),gc('V17_Placa_fija_tensor_Al')]:
      for p in c.modelParameters:
        if '36.75' in p.expression:p.expression=p.expression.replace('36.75','37.75')
    # El puente no debe penetrar en la tapa desmontable del625.
    h=gc('V17_Housing_STS3250_y_apoyo625')
    sketch=h.sketches.itemByName('Croquis_Puente_pod_trasero')
    for dim in sketch.sketchDimensions:
      if dim.parameter.expression=='15.5 mm':dim.parameter.expression='14.9 mm'
    fixed=gc('V17_Placa_fija_tensor_Al')
    slot17(fixed,'Paso_oblongo_eje_collar','0 mm','j3_fixed_z-2 mm','j3_fixed_z+6 mm','-35.6 mm+j3_transverse_shift','2.7 mm','12.5 mm')
    host=gc('A05_Encastre_proximal_brazo_PETG')
    for xx in [-29,29]:
      box(host,'V17_Alargar_anclaje_corredor_'+str(xx),f'{xx-5} mm','-33 mm+j3_transverse_shift','j3_fixed_z-8 mm','10 mm','-j3_transverse_shift','16 mm')
    join_all(host,'V17_Anclajes_laterales_continuos')
    for xx in [-29,29]:
      ring(host,'V17_Paso_anclaje_alargado_'+str(xx),'Y','-33.1 mm+j3_transverse_shift',f'{xx} mm','j3_fixed_z','4.5 mm',None,'-j3_transverse_shift+0.2 mm',cut=True)
      bolt=gc('V17_M4_anclaje_fijo_'+str(xx))
      bolt.features.extrudeFeatures.itemByName('Vastago').extentOne.distance.expression='40 mm'
      bolt.attributes.add('AFR2','fijacion','M4x40; arandelas y tuerca sobre caras de contacto, no suspendidas.')
    # Prolongar el apoyo de la polea conducida sin desplazar el eje del codo.
    sh=gc('V02_E02_Eje_y_brida_codo')
    ring(sh,'V17_Extension_corredor_eje8','Y','-60 mm+j3_transverse_shift','0 mm','0 mm','8 mm',None,'-j3_transverse_shift+5 mm','steel')
    join_all(sh,'V17_Eje_codo_continuo_corredor')
    box(sh,'V17_Chavetero_corredor','3 mm','-54 mm+j3_transverse_shift','-1 mm','2 mm','15 mm','2 mm',cut=True)
    spacer=gc('V17_Separador_codo_trasero')
    spacer.features.extrudeFeatures.itemByName('Separador').extentOne.distance.expression='6.7 mm-j3_transverse_shift'
    # Envolvente de la rosca comercial M3 del servo; no perforacion propuesta.
    servo=gc('V17_M03_STS3250_remoto')
    ring(servo,'V17_Rosca_M3_servo_envolvente_REF','Y','-76.1 mm+j3_transverse_shift','0 mm','j3_motor_z','3.2 mm',None,'6.2 mm',cut=True)
    D.attributes.add('AFR2','correccion_corredor','Despeje lateral9mm, paso oblongo central, fila inferior+2mm, anclajes extendidos. Exige repetir revision CAD; no es declaracion de ausencia de choques.')

def phase17_paso_hombro():
    # El collar del eje del hombro pertenece al primer eslabon y debe
    # atravesar libremente la placa fija del tensor, tambien al acortarlo.
    c=gc('V17_Placa_fija_tensor_Al')
    ring(c,'V17_Paso_collar_hombro','Y','-35.6 mm+j3_transverse_shift','0 mm','0 mm','18 mm',None,'2.7 mm',cut=True)

def phase17_paso_tubo():
    # Retirar solo la pared de insercion, dejando el nucleo interior
    # conectado al cuello como apoyo contra el aplastamiento del tubo.
    c=gc('A05_Encastre_proximal_brazo_PETG')
    ring(c,'V17_Despeje_pared_PVC25','Z','tube_inset','0 mm','0 mm','v17_pvc1_od+socket_clear','v17_pvc1_id-0.2 mm','socket_start+socket_length+v07_socket_extra-tube_inset+0.1 mm',cut=True)

def phase17_limites_transmisiones():
    # Fusion normaliza los angulos auxiliares al asignar limites mayores
    # de una vuelta. El codo principal ya limita el movimiento a +-90.
    # Como en la entrada de J2, dejar el seguidor sin un segundo tope.
    j=R.joints.itemByName('T_J3_Correa_2a1')
    limits=j.jointMotion.rotationLimits
    limits.isMinimumValueEnabled=False;limits.isMaximumValueEnabled=False
    j.attributes.add('AFR2','limites_v17','Entrada auxiliar sin topes independientes. J3_Codo conserva +-90 grados; MotionLink aplica relacion2.5. Evita intervalo angular normalizado135..225 que recortaba el seguidor. No altera pose ni recorrido del codo.')


def phase17_ensambles_limites():
    # Entrega 3D: inventario de uniones y limites existentes, sin calculos
    # mecanicos ni barridos de interferencias. No modificar la pose.
    import math
    report=dict(document=adsk.core.Application.get().activeDocument.name,
                scope='Ensamble CAD y limites configurados; no prueba de resistencia ni de recorrido libre de colisiones.',
                joints=[],motion_links=[m.name for m in R.motionLinks])
    active={R.entityToken:R}
    for o in R.allOccurrences:active[o.component.entityToken]=o.component
    for c in active.values():
      for j in c.joints:
        row=dict(component=c.name,name=j.name,motion_type=int(j.jointMotion.jointType))
        motion=adsk.fusion.RevoluteJointMotion.cast(j.jointMotion)
        if motion:
          limits=motion.rotationLimits
          row['minimum_deg']=math.degrees(limits.minimumValue) if limits.isMinimumValueEnabled else None
          row['maximum_deg']=math.degrees(limits.maximumValue) if limits.isMaximumValueEnabled else None
        slider=adsk.fusion.SliderJointMotion.cast(j.jointMotion)
        if slider:
          limits=slider.slideLimits
          row['minimum_mm']=10*limits.minimumValue if limits.isMinimumValueEnabled else None
          row['maximum_mm']=10*limits.maximumValue if limits.isMaximumValueEnabled else None
        report['joints'].append(row)
    (HERE17/'ensambles_y_limites_3D.json').write_text(json.dumps(report,indent=2),encoding='utf-8')
    D.attributes.add('AFR2','alcance_entrega_3D','Solo geometria, ensambles y limites. Calculos, tensiones y aligerado diferidos por el usuario.')
    print(json.dumps(dict(joints=len(report['joints']),motion_links=len(report['motion_links']))))


def phase17_save():
    app=adsk.core.Application.get();doc=app.activeDocument
    closed=all(D.attributes.itemByName('AFR2','v17_phase_'+p) for p in ['act_sts_J1_union','act_sts_J4_union','act_sts_J5_montaje_axial','ensambles_limites'])
    scope=('Entrega 3D ensamblada con limites configurados. Calculos, resistencia, aligerado y revision global de interferencias diferidos.' if closed else 'Rediseño en curso. El archivo guardado no implica revision de interferencias ni masa final ni brazo terminado.')
    doc.save('V17 antropomorfo V7: J3 remoto, PVC25/20, interfaces STS y reduccion J2. '+scope)
    ok=D.exportManager.execute(D.exportManager.createFusionArchiveExportOptions(str(HERE17/(NAME17+'.f3d'))))
    if not ok:raise RuntimeError('Fallo exportacion F3D')
    stages=[]
    for line in (HERE17/'progreso_J3.jsonl').read_text(encoding='utf-8').splitlines():
      msg=json.loads(line).get('message','')
      if msg.startswith('aplicada '):stages.append(msg.removeprefix('aplicada '))
    state=dict(document=doc.name,saved=not doc.isModified,exported=ok,components=D.allComponents.count,joints=R.joints.count,motion_links=R.motionLinks.count,applied_stages=stages,
        model_3d_assembled=closed,scope=scope)
    state['joints_scope']='joints cuenta solo el componente raiz; ver joint_counts_by_component para montajes rigidos internos.'
    state['joint_counts_by_component']={c.name:c.joints.count for c in D.allComponents if c.joints.count}
    scripted=[phase for phase in stages if phase not in ['save','modo_trabajo','restaurar_vista','ensambles_limites'] and D.attributes.itemByName('AFR2','v17_phase_'+phase)]
    state['physical_materials_synced']=bool(scripted and scripted[-1].startswith('act_sts_materiales'))
    (HERE17/'estado_cad_v17.json').write_text(json.dumps(state,indent=2),encoding='utf-8')
    print(json.dumps(dict(document=doc.name,saved=not doc.isModified,exported=ok,components=D.allComponents.count,joints=R.joints.count)))

def run(context):
    app=adsk.core.Application.get()
    if app.activeDocument.name!=NAME17:raise RuntimeError('Activar v17; maestros anteriores no se editan')
    setup();phase=globals().get('FASE17')
    if not phase:raise RuntimeError('Indicar FASE17 explicita')
    history=HERE17/'progreso_J3.jsonl'
    if phase not in ['save','ensambles_limites'] and history.exists():
      for line in history.read_text(encoding='utf-8').splitlines():
        if json.loads(line).get('message')=='aplicada '+phase:
          raise RuntimeError('Fase ya aplicada; no se repite una mutacion: '+phase)
    progress('inicio '+phase)
    try:
      if phase.startswith('act_'):
        exec(compile((HERE17/'montar_actuadores_v17.py').read_text(encoding='utf-8'),'montar_actuadores_v17.py','exec'),globals())
        if phase.startswith('act_sts_'):
          exec(compile((HERE17/'montar_STS3250_v17.py').read_text(encoding='utf-8'),'montar_STS3250_v17.py','exec'),globals())
          if phase.startswith('act_sts_J5_'):
            exec(compile((HERE17/'montar_J5_STS3250_v17.py').read_text(encoding='utf-8'),'montar_J5_STS3250_v17.py','exec'),globals())
          if phase.startswith('act_sts_J4_'):
            exec(compile((HERE17/'montar_J4_STS3250_v17.py').read_text(encoding='utf-8'),'montar_J4_STS3250_v17.py','exec'),globals())
          if phase.startswith('act_sts_J1_'):
            exec(compile((HERE17/'montar_J1_STS3250_v17.py').read_text(encoding='utf-8'),'montar_J1_STS3250_v17.py','exec'),globals())
          if phase.startswith('act_sts_materiales'):
            exec(compile((HERE17/'materiales_fisicos_v17.py').read_text(encoding='utf-8'),'materiales_fisicos_v17.py','exec'),globals())
        globals()['act17_'+phase.removeprefix('act_')]()
      elif phase.startswith('reforma_'):
        exec(compile((HERE17/'reformar_v17.py').read_text(encoding='utf-8'),'reformar_v17.py','exec'),globals())
        globals()['light17_'+phase.removeprefix('reforma_')]()
      else:globals()['phase17_'+phase]()
      D.activateRootComponent();adsk.doEvents()
      if phase!='save':D.attributes.add('AFR2','v17_phase_'+phase,'applied')
      progress('aplicada '+phase)
    except Exception as error:
      progress('error '+phase+': '+str(error));raise
    print(json.dumps(dict(phase=phase,applied=True)))
