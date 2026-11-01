"""AdaptacionJ5 al STS3250. Solo se aplica mediante fases explicitas.

Conserva eje/brida, dos625 y limites de la muneca. No es aprobacion FDM.
"""


def j5_cones(c,name,y0,y1,x,z,d0,d1,cut=False):
    before=sum(b.volume for b in c.bRepBodies)
    sections=[]
    for y,d in [(y0,d0),(y1,d1)]:
      sk=c.sketches.add(datum(c,'Y',y,'Plano_'+name))
      circle(sk,x,'-('+z+')',d);sections.append(sk)
    next(o for o in R.allOccurrences if o.component==c).activate()
    ip=c.features.loftFeatures.createInput(OP.CutFeatureOperation if cut else OP.NewBodyFeatureOperation)
    for sk in sections:ip.loftSections.add(sk.profiles.item(0))
    if cut:ip.participantBodies=list(c.bRepBodies)
    f=c.features.loftFeatures.add(ip);f.name=name
    if f.healthState!=adsk.fusion.FeatureHealthStates.HealthyFeatureHealthState:
      raise RuntimeError('Cono J5 no calculado: '+f.errorOrWarningMessage)
    if cut and before-sum(b.volume for b in c.bRepBodies)<1e-7:
      raise RuntimeError('Cono J5 no retiro material '+name)
    for sk in sections:sk.isVisible=False
    if not cut:
      for b in f.bodies:b.appearance=AP['steel']

def act17_sts_J5_parametros():
    for name,expression in [
      ('v17_j5_output_y','36.3 mm'),
      ('v17_j5_bottom','wrist45-v17_sts_w/2'),
      ('v17_j5_floor','v17_j5_bottom-2 mm'),
      ('v17_j5_top','wrist45+v17_sts_w/2'),
    ]:param(name,expression,'mm','J5: caja/horn comerciales; eje y brida originales conservados.')


def act17_sts_J5_motor():
    act17_remove('04_Giro_muneca',['V02_M05_MG946_J5','V03_Inserto_fijacion_J5','V03_Tornillo_fijacion_J5'])
    sts17_shared('V17_M03_STS3250_remoto','04_Giro_muneca','V02_A15_Horquilla_J5_PETG',target_y='v17_j5_output_y',target_z='wrist45',tag='J5')


def act17_sts_J5_housing():
    c=act17_get('V02_A15_Horquilla_J5_PETG')
    # Retirar solo la cuna anterior, detras de la tapa625 positiva. La
    # ruedaJ4, rotor, brazos y asientos de ambos625 permanecen intactos.
    box(c,'V17_Retirar_cuna_J5_anterior','-50 mm','29.5 mm','wrist45-18 mm','110 mm','65 mm','40 mm',cut=True)
    box(c,'V17_Cuna_J5_exterior','-13.11 mm','30 mm','v17_j5_floor','51.22 mm','42.1 mm','v17_sts_w+2 mm')
    box(c,'V17_Puente_J5_integral','-10 mm','22 mm','v17_j5_floor-5.64 mm','20 mm','12 mm','8 mm')
    for x in ['-20.4 mm','37.6 mm']:
      for y in ['30.6 mm','64.6 mm']:
        box(c,'V17_Pilar_J5',x,y,'v17_j5_floor','7.8 mm','7.8 mm','v17_sts_w+2 mm')
    join_all(c,'V17_Horquilla_y_cuna_J5_una_pieza')
    box(c,'V17_Cavidad_caja_J5','-10.41 mm','33.4 mm','v17_j5_bottom','45.82 mm','35.6 mm','v17_sts_w+0.1 mm',cut=True)
    ring(c,'V17_Paso_brida_y_horn_J5','Y','29.9 mm','0 mm','wrist45','22.6 mm',None,'3.6 mm',cut=True)
    ring(c,'V17_Paso_pivote_trasero_J5','Y','68.9 mm','0 mm','wrist45','6.5 mm',None,'3.3 mm',cut=True)
    box(c,'V17_Acceso_conectores_TTL_J5','4 mm','68.9 mm','wrist45-10.5 mm','27 mm','3.3 mm','21 mm',cut=True)
    c.attributes.add('AFR2','material_v17','PLA PrintALot')
    c.attributes.add('AFR2','montaje_J5','Cuna y puente integrados a horquilla. Plano bridaY32 conservado; cuerpo33.7..68.7, tapa superior, apoyos625 originales. HolgurasXY0.3; apoyoZ nominal sin flotacion. FDM/calibracion/resistencia pendientes.')
    act17_one(c)


def act17_sts_J5_tapa():
    g=comp('04_Giro_muneca');c=piece17(g,'V17_Tapa_STS3250_J5')
    box(c,'Tapa','-20.4 mm','30.6 mm','v17_j5_top','65.8 mm','41.8 mm','2.5 mm')
    act17_one(c)


def act17_sts_J5_pasantes():
    holder=act17_get('V02_A15_Horquilla_J5_PETG');cap=act17_get('V17_Tapa_STS3250_J5')
    for x in ['-16.5 mm','41.5 mm']:
      for y in ['34.5 mm','68.5 mm']:
        ring(holder,'V17_Paso_M3_J5','Z','v17_j5_floor-0.1 mm',x,y,'3.4 mm',None,'v17_sts_w+2.2 mm',cut=True)
        ring(cap,'Paso_M3','Z','v17_j5_top-0.1 mm',x,y,'3.4 mm',None,'2.7 mm',cut=True)
    act17_one(holder);act17_one(cap)


def act17_sts_J5_fijaciones():
    g=comp('04_Giro_muneca')
    for i,(x,y) in enumerate([(x,y) for x in ['-16.5 mm','41.5 mm'] for y in ['34.5 mm','68.5 mm']]):
      c=piece17(g,'V17_M3x35_tapa_J5_'+str(i),'Acero')
      ring(c,'Vastago','Z','v17_j5_top+2.5 mm-35 mm',x,y,'3 mm',None,'35 mm','steel')
      ring(c,'Cabeza','Z','v17_j5_top+2.5 mm',x,y,'5.5 mm',None,'3 mm','steel');join_all(c,'M3x35_REF')
      c=piece17(g,'V17_Arandela_M3_J5_'+str(i),'Acero')
      ring(c,'DIN125_REF','Z','v17_j5_floor-0.8 mm',x,y,'7 mm','3.2 mm','0.8 mm','steel')
      c=piece17(g,'V17_Tuerca_M3_J5_'+str(i),'Acero')
      ring(c,'DIN934_REF','Z','v17_j5_floor-3.2 mm',x,y,'6.4 mm','3 mm','2.4 mm','steel')


def act17_sts_J5_horn():
    act17_remove('05_Inclinacion_muneca',['V03_Horn_movil_V02_M05','V03_Tornillo_union_V02_M05'])
    sts17_shared('V17_Horn_STS3250_J3','05_Inclinacion_muneca','V02_E04_Eje_y_brida_J5',True,target_y='v17_j5_output_y',target_z='0 mm',tag='J5')
    shaft=act17_get('V02_E04_Eje_y_brida_J5')
    # Relleno CAD al final del historial: define una brida NUEVA mecanizada,
    # no recomienda reparar fisicamente la pieza anterior con soldadura.
    for x in ['-7.5 mm','7.5 mm']:
      ring(shaft,'V17_Cerrar_patron_antiguo_J5','Y','30 mm',x,'0 mm','2.7 mm',None,'2 mm','steel')
    join_all(shaft,'V17_Brida_J5_para_horn_nuevo')
    points=[('7 mm','0 mm'),('-7 mm','0 mm'),('0 mm','7 mm'),('0 mm','-7 mm')]
    for x,z in points:
      ring(shaft,'V17_Paso_M3_PCD14_J5','Y','29.4 mm',x,z,'3.4 mm',None,'2.7 mm',cut=True)
      ring(shaft,'V17_Banda_avellanado_M3_J5','Y','29.4 mm',x,z,'6.2 mm',None,'0.3 mm',cut=True)
      j5_cones(shaft,'V17_Cono90_M3_J5','29.7 mm','31.1 mm',x,z,'6.2 mm','3.4 mm',True)
    act17_one(shaft)
    shaft.attributes.add('AFR2','fabricacion_v17','Brida22x2.5 conservada,4pasantesM3PCD14 y avellanados6.2x90grados. CabezasM3x6ISO10642recesadas0.1 desdeY29.6, gap0.6 hasta tapa625que terminaY29. Pieza nueva mecanizada, no soldar rellenos. Taladros viejos eliminados en CADfinal.')


def act17_sts_J5_tornillos_horn():
    phase17_modo_trabajo()
    g=comp('05_Inclinacion_muneca')
    for i,(x,z) in enumerate([('7 mm','0 mm'),('-7 mm','0 mm'),('0 mm','7 mm'),('0 mm','-7 mm')]):
      c=piece17(g,'V17_M3x6_ISO10642_horn_J5_'+str(i),'Acero')
      ring(c,'Vastago','Y','31.3 mm',x,z,'3 mm',None,'4.3 mm','steel')
      ring(c,'Banda_cabeza_REF','Y','29.6 mm',x,z,'6 mm',None,'0.2 mm','steel')
      j5_cones(c,'Cono90_cabeza_REF','29.8 mm','31.3 mm',x,z,'6 mm','3 mm')
      join_all(c,'M3x6_ISO10642_REF');act17_one(c)
      c.attributes.add('AFR2','fijacion','M3x6 avellanado, longitudTOTAL6incluye cabeza. EnvolventeD6x1.7, cono90+banda0.2. FrenteY29.6, asiento en brida, punta35.6 dentro del alivio de motor. Confirmar norma/lote y tolerancias antes de mecanizar.')
    # La retencion central anterior ya apoya enY32 y queda alojada en la
    # caja6mm de la brida. Se conserva; no agregar un segundo tornillo.


def act17_sts_J5_montaje_axial():
    h=act17_get('V02_A15_Horquilla_J5_PETG');cap=act17_get('V17_Tapa_STS3250_J5')
    # Abrir el extremo trasero permite introducir motor+horn+eje ya
    # acoplados desde+Y, con las tapas625 colocadas sobre la horquilla.
    box(h,'V17_Abertura_insercion_axial_J5','-10.41 mm','68.9 mm','v17_j5_bottom','45.82 mm','3.3 mm','v17_sts_w+0.1 mm',cut=True)
    # El tope pasa a ser parte de la tapa desmontable: mismo numero de
    # impresos, sin pared permanente bloqueando el montaje horizontal.
    box(cap,'V17_Tope_trasero_solidario_a_tapa','-10.11 mm','69 mm','v17_j5_bottom','45.22 mm','3 mm','v17_sts_w+0.1 mm')
    join_all(cap,'V17_Tapa_y_tope_una_pieza')
    box(cap,'V17_Ventana_TTL_tope','4 mm','68.9 mm','wrist45-10.5 mm','27 mm','3.3 mm','21 mm',cut=True)
    ring(cap,'V17_Paso_pivote_tope','Y','68.9 mm','0 mm','wrist45','6.5 mm',None,'3.3 mm',cut=True)
    h.attributes.add('AFR2','secuencia_montaje_J5','1Montar625/tapas en horquilla. 2Atornillar horn al motor conM3central, antes de acoplar eje. 3Atornillar4M3avellanados de eje a horn fuera del robot. 4Introducir conjunto axialmente desde+Y, eje pasa por22.6/tapas/625. 5Colocar retencion negativa del eje. 6Cerrar tapa+tope trasero con4M3x35. No intentar apretar cabezasM3 a traves de tapas625 cerradas.')
    cap.attributes.add('AFR2','montaje_J5','Tapa y tope posterior integrados. HolguraX0.3 en abertura de insercion, portTTL y pivote accesibles. CuatroM3x35 sujetan tapa; no pieza flotante ni pared que impida montar motor/eje.')
    act17_one(h);act17_one(cap)
