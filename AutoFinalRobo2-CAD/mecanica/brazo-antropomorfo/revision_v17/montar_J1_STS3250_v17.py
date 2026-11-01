"""J1 bajo sobre AGV: STS3250, cartucho6000 y dobleD originales.

No elevar conjunto. Horn comercial adaptado y secuencia de montaje
explicita; cotas del estriado son referencia, no plano para fabricarlo.
"""

def act17_sts_J1_parametros():
    for n,e in [('v17_j1_face','59.15 mm'),('v17_j1_bottom','v17_j1_face-32.4 mm'),
                ('v17_j1_floor','v17_j1_bottom-2 mm'),('v17_j1_top','v17_j1_face+2.6 mm'),
                ('v17_j1_horn_front','v17_j1_face+4.3 mm-base_top')]:param(n,e,'mm')


def act17_sts_J1_motor():
    act17_remove('00_Base_control',['V09_M01_GX3270BLS_Base','V09_Fijacion_motor_','V09_Arandela_motor_','V09_Inserto_motor_'])
    sts17_shared('V17_M03_STS3250_remoto','00_Base_control','V05_Montura_baja_PETG',target_x='0 mm',target_y='v05_axis_y',target_z='v17_j1_face',tag='J1',source_axis='Y')


def act17_sts_J1_housing():
    c=act17_get('V05_Montura_baja_PETG')
    box(c,'V17_Cuna_J1_exterior','-13.11 mm','v05_axis_y-14.66 mm','v17_j1_floor','51.22 mm','29.32 mm','60 mm-v17_j1_floor')
    join_all(c,'V17_Cuna_J1_integrada_en_montura')
    box(c,'V17_Cavidad_STS3250_J1','-10.41 mm','v05_axis_y-12.66 mm','v17_j1_bottom','45.82 mm','25.32 mm','35.1 mm',cut=True)
    box(c,'V17_Acceso_TTL_bajo_J1','4 mm','v05_axis_y-10.5 mm','11.9 mm','27 mm','21 mm','v17_j1_bottom-11.8 mm',cut=True)
    ring(c,'V17_Pivote_trasero_J1','Z','v17_j1_floor-0.1 mm','0 mm','v05_axis_y','6.5 mm',None,'2.2 mm',cut=True)
    c.attributes.add('AFR2','material_v17','PLA PrintALot')
    c.attributes.add('AFR2','montaje_J1','STS3250 retenido por forma entre piso26.75 y cartucho6000 aliviado a61.75. Montura bilateral, eje y altura96 conservados. Perfil y tolerancias FDM por definir; verificar giroHCSR despues.')
    act17_one(c)


def act17_sts_J1_cartucho():
    c=act17_get('V09_Cartucho6000_desmontable_PETG')
    box(c,'V17_Reserva_tapa_frontal_STS_J1','-10.41 mm','v05_axis_y-12.66 mm','59.9 mm','45.82 mm','25.32 mm','v17_j1_top-59.9 mm',cut=True)
    act17_one(c)


def act17_sts_J1_horn():
    act17_remove('01_Giro_base',['V09_Horn25T_J1','V09_M3_horn_','V09_Tornillo25T_REF'])
    g=comp('01_Giro_base');c=piece17(g,'V17_Horn_STS3250_J1_adaptado','Al6061')
    f='v17_j1_face-base_top'
    ring(c,'Disco_comercial19_95','Z','('+f+')+1.8 mm','0 mm','0 mm','19.95 mm','3.2 mm','2.5 mm','steel')
    ring(c,'Cubo_comercial9','Z','('+f+')-0.2 mm','0 mm','0 mm','9 mm','5.9 mm','2 mm','steel')
    join_all(c,'Horn_comercial_con_adaptacion_propia')
    ring(c,'Spline25T_REF','Z','('+f+')-0.3 mm','0 mm','0 mm','5.9 mm',None,'2.8 mm',cut=True)
    ring(c,'Alivio_socket_REF','Z','('+f+')+2.5 mm','0 mm','0 mm','6.2 mm',None,'0.9 mm',cut=True)
    for x,y in [('7 mm','0 mm'),('-7 mm','0 mm'),('0 mm','7 mm'),('0 mm','-7 mm')]:
      ring(c,'PCD14','Z','('+f+')+1.7 mm',x,y,'3.4 mm' if y=='0 mm' else '3 mm',None,'2.7 mm',cut=True)
      if y=='0 mm':ring(c,'Rebaje_inferior_abierto_M3','Z','('+f+')-0.3 mm',x,y,'6 mm',None,'2.4 mm',cut=True)
    c.partNumber='AFR2-J1-HORN25T-2M3-PASANTES'
    c.description='HornFEETECH19.95/25T comercial:2M3 enX+-7 repasados3.4; cajeados6 abiertos por detras, alivian borde de cubo y profundizan0.3 en disco. Pared minima cubo1.05mm radial, resistencia del accesorio por verificar. Otros2M3 sin modificar. No fabricar estriado desde envolvente.'
    act17_one(c)


def act17_sts_J1_adapter():
    c=act17_get('V09_Adaptador_dobleD_acero')
    for x in ['-7.5 mm','7.5 mm']:
      ring(c,'V17_Cerrar_patron_horn_anterior','Z','63.45 mm-base_top',x,'0 mm','3.2 mm',None,'3.35 mm','steel')
    join_all(c,'V17_Adaptador_J1_mecanizado_nuevo')
    for x in ['-7 mm','7 mm']:
      ring(c,'V17_Piloto_rosca_M3_PCD14','Z','63.4 mm-base_top',x,'0 mm','2.5 mm',None,'3.5 mm',cut=True)
    shaft=act17_get('V03_E00_Husillo_base_completo')
    ring(shaft,'V17_Caja_cabeza_M3_STS_J1','Z','63.4 mm-base_top','0 mm','0 mm','6.2 mm',None,'3.2 mm',cut=True)
    c.attributes.add('AFR2','fabricacion_v17','Adaptador nuevo:2roscasM3PCD14, piloto2.5; patron viejo eliminado en pieza final. No soldar rellenos CAD. DobleD/eje10/race6000 originales.')
    act17_one(c);act17_one(shaft)


def act17_sts_J1_union():
    g=comp('01_Giro_base')
    for sign in [-1]:
      x=f'{sign*7} mm';c=piece17(g,'V17_M3x5_horn_J1_'+str(sign),'Acero')
      ring(c,'Cabeza_baja_REF','Z','v17_j1_horn_front-3.85 mm',x,'0 mm','5.7 mm',None,'1.65 mm','steel')
      ring(c,'Vastago','Z','v17_j1_horn_front-2.2 mm',x,'0 mm','3 mm',None,'5 mm','steel');join_all(c,'M3x5_ISO7380_REF')
      D.activateRootComponent();occ(g.name).activate()
      m=adsk.core.Matrix3D.create();m.translation=adsk.core.Vector3D.create(1.4,0,0)
      twin=g.occurrences.addExistingComponent(c,m);twin.isGroundToParent=True
    c=piece17(g,'V17_M3x6_central_J1','Acero')
    ring(c,'Vastago','Z','v17_j1_horn_front-6 mm','0 mm','0 mm','3 mm',None,'6 mm','steel')
    ring(c,'Cabeza','Z','v17_j1_horn_front','0 mm','0 mm','5.5 mm',None,'3 mm','steel');join_all(c,'M3x6_REF')
    c.attributes.add('AFR2','secuencia_montaje','1PrecolocarM3central en horn. 2Atornillar horn al adaptador fuera del motor con2M3x5 desde abajo. 3Montar en dobleD,eje/circlip/cartucho. 4Alinear estriado con motor e insertar. 5AjustarM3central con llave2.5 larga por taladro3.4 del eje, accesible desde arriba. Cabeza6.2en caja inferior y holgura0.45a cara motor. Confirmar secuencia fisica y tolerancias antes de fabricar.')
