"""AdaptacionJ5 al STS3250. Solo se aplica mediante fases explicitas.

Conserva eje/brida, dos625 y limites de la muneca. No es aprobacion FDM.
"""

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
    act17_one(shaft)
    shaft.attributes.add('AFR2','fabricacion_v17','Brida22x2.5 conservada,4pasantesM3PCD14, tornillos desdeY27.85. Pieza nueva mecanizada, sin perforaciones viejas; no soldar rellenos.')


def act17_sts_J5_tornillos_horn():
    g=comp('05_Inclinacion_muneca')
    for i,(x,z) in enumerate([('7 mm','0 mm'),('-7 mm','0 mm'),('0 mm','7 mm'),('0 mm','-7 mm')]):
      c=piece17(g,'V17_M3x5_ISO7380_horn_J5_'+str(i),'Acero')
      ring(c,'Cabeza_baja_REF','Y','27.85 mm',x,z,'5.7 mm',None,'1.65 mm','steel')
      ring(c,'Vastago','Y','29.5 mm',x,z,'3 mm',None,'5 mm','steel');join_all(c,'M3x5_ISO7380_REF')
    # La retencion central anterior ya apoya enY32 y queda alojada en la
    # caja6mm de la brida. Se conserva; no agregar un segundo tornillo.
