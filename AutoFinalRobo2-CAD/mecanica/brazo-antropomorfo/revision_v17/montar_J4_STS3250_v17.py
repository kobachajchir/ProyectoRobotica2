"""Montaje J4 conservando los engranajes24/24 y apoyos de la arquitecturaV7.

Fases explicitas. Caso y horn son instancias de referencias comerciales;
carcasa impresa integrada en A12. No implica resistencia FDM aprobada.
"""

def act17_sts_J4_parametros():
    for n,e,u in [('v17_j4_centres','36 mm','mm'),('v17_j4_axis_y','0 mm','mm'),
                 ('v17_j4_axis_x','v17_j4_centres','mm'),
                 ('v17_j4_phase','7.5 deg','deg'),
                 ('v17_j4_bottom','link2-32.4 mm','mm'),('v17_j4_floor','v17_j4_bottom-2 mm','mm'),
                 ('v17_j4_top','link2+2.6 mm','mm')]:param(n,e,u)
    jo=comp('03_Antebrazo_PVC').jointOrigins.itemByName('V02_J4_MOTOR')
    if not D.modifyParameters([jo.offsetX,jo.offsetY,jo.offsetZ],[V('v17_j4_axis_x'),V('v17_j4_axis_y'),V('link2+6 mm')]):raise RuntimeError('No se pudo ubicar ejeJ4')
    R.joints.itemByName('T_J4_Motor').angle.expression='v17_j4_phase'


def act17_sts_J4_motor():
    act17_remove('03_Antebrazo_PVC',['V02_M04_MG90S_J4','V03_Tornillo_rosca_plastico_J4'])
    sts17_shared('V17_M03_STS3250_remoto','03_Antebrazo_PVC','A12_Encastre_distal_antebrazo_PETG',target_x='v17_j4_axis_x',target_y='v17_j4_axis_y',target_z='link2',tag='J4',source_axis='Y',twist_degrees=270)


def act17_sts_J4_housing():
    phase17_modo_trabajo()
    c=act17_get('A12_Encastre_distal_antebrazo_PETG')
    box(c,'V17_Retirar_cuna_micro_J4','19.6 mm','-30 mm','link2-40 mm','75 mm','65 mm','50 mm',cut=True)
    box(c,'V17_Piso_cuna_J4','v17_j4_axis_x-20.46 mm','-20.4 mm','v17_j4_floor','40.92 mm','65.8 mm','2 mm')
    box(c,'V17_Caja_cuna_J4','v17_j4_axis_x-15.36 mm','-13.11 mm','v17_j4_bottom','30.72 mm','51.22 mm','35 mm')
    for y in ['-8 mm','5 mm']:
      box(c,'V17_Puente_copa_J4','12 mm',y,'link2-13 mm','v17_j4_axis_x-12.66 mm-12 mm','3 mm','3 mm')
    for x in ['v17_j4_axis_x-20.46 mm','v17_j4_axis_x+12.66 mm']:
      for y in ['-20.4 mm','37.6 mm']:
        box(c,'V17_Pilar_J4',x,y,'v17_j4_floor','7.8 mm','7.8 mm','37 mm')
    join_all(c,'V17_A12_y_cuna_J4_integrales')
    box(c,'V17_Cavidad_STS_J4','v17_j4_axis_x-12.66 mm','-10.41 mm','v17_j4_bottom','25.32 mm','45.82 mm','35.1 mm',cut=True)
    box(c,'V17_Paso_TTL_fondo_J4','v17_j4_axis_x-10.5 mm','4 mm','v17_j4_floor-0.1 mm','21 mm','27 mm','2.2 mm',cut=True)
    ring(c,'V17_Paso_pivote_fondo_J4','Z','v17_j4_floor-0.1 mm','v17_j4_axis_x','v17_j4_axis_y','6.5 mm',None,'2.2 mm',cut=True)
    c.attributes.add('AFR2','material_v17','PLA PrintALot')
    c.attributes.add('AFR2','montaje_J4','STS3250 con eje originalX36,Y0 y cuerpo largo orientado+Y. Cuna integrada, cuerpo27.6..62.6 nominal para link2=60. Cup/608/eje y24/24 originales conservados. VentanaTTL inferida del plano. Perfil FDM y resistencia pendientes.')
    act17_one(c)


def act17_sts_J4_tapa():
    g=comp('03_Antebrazo_PVC');c=piece17(g,'V17_Tapa_STS3250_J4')
    box(c,'Tapa','v17_j4_axis_x-20.46 mm','-20.4 mm','v17_j4_top','40.92 mm','65.8 mm','2.5 mm')
    ring(c,'Despeje_tapa608_fija','Z','v17_j4_top-0.1 mm','0 mm','0 mm','39 mm',None,'2.7 mm',cut=True)
    ring(c,'Paso_horn_y_separador','Z','v17_j4_top-0.1 mm','v17_j4_axis_x','v17_j4_axis_y','20.5 mm',None,'2.7 mm',cut=True)
    act17_one(c)


def j4_taladros(c,name,z,points,diameter,depth):
    # Croquis separados evitan inferencias entre centros; todos coplanares
    # y reunidos en una sola extrusion.
    plane=datum(c,'Z',z,'Plano_'+name);sketches=[]
    profiles=adsk.core.ObjectCollection.create()
    for i,(x,y) in enumerate(points):
      s=c.sketches.add(plane);s.name='Croquis_'+name+'_'+str(i)
      s.isComputeDeferred=True;circle(s,x,y,diameter);s.isComputeDeferred=False
      profiles.add(s.profiles.item(0));sketches.append(s)
    f=extrude(c,sketches[0],depth,OP.CutFeatureOperation,name,profiles)
    for s in sketches:s.isVisible=False
    progress('operacion: '+name)
    return f


def act17_sts_J4_pasantes():
    h=act17_get('A12_Encastre_distal_antebrazo_PETG');cap=act17_get('V17_Tapa_STS3250_J4')
    points=[(x,y) for x in ['v17_j4_axis_x-16.56 mm','v17_j4_axis_x+16.56 mm'] for y in ['-16.5 mm','41.5 mm']]
    j4_taladros(h,'V17_Pasos_M3_J4','v17_j4_floor-0.1 mm',points,'3.4 mm','37.2 mm')
    j4_taladros(cap,'Pasos_M3','v17_j4_top-0.1 mm',points,'3.4 mm','2.7 mm')
    act17_one(h);act17_one(cap)


def j4_repetir_fijacion(g,c,offsets):
    # Instancias del mismo componente: no duplicar historiales identicos.
    # Pasos comerciales fijos; altura y eje siguen expresiones del cuerpo.
    for dx,dy in offsets:
      D.activateRootComponent();occ(g.name).activate()
      m=adsk.core.Matrix3D.create();m.translation=adsk.core.Vector3D.create(dx/10,dy/10,0)
      instance=g.occurrences.addExistingComponent(c,m);instance.isGroundToParent=True
    progress('instancias compartidas '+c.name)


def act17_sts_J4_fijaciones():
    g=comp('03_Antebrazo_PVC')
    for i,(x,y) in enumerate([('v17_j4_axis_x-16.56 mm','-16.5 mm')]):
      c=piece17(g,'V17_M3x45_tapa_J4_'+str(i),'Acero')
      ring(c,'Vastago','Z','v17_j4_top+2.5 mm-45 mm',x,y,'3 mm',None,'45 mm','steel')
      ring(c,'Cabeza','Z','v17_j4_top+2.5 mm',x,y,'5.5 mm',None,'3 mm','steel');join_all(c,'M3x45_REF')
      j4_repetir_fijacion(g,c,[(33.12,0),(0,58),(33.12,58)])
      c=piece17(g,'V17_Arandela_M3_J4_'+str(i),'Acero');ring(c,'DIN125_REF','Z','v17_j4_floor-0.8 mm',x,y,'7 mm','3.2 mm','0.8 mm','steel')
      j4_repetir_fijacion(g,c,[(33.12,0),(0,58),(33.12,58)])
      c=piece17(g,'V17_Tuerca_M3_J4_'+str(i),'Acero');ring(c,'DIN934_REF','Z','v17_j4_floor-3.2 mm',x,y,'6.4 mm','3 mm','2.4 mm','steel')
      j4_repetir_fijacion(g,c,[(33.12,0),(0,58),(33.12,58)])


def act17_sts_J4_horn():
    act17_remove('11_Pinion_J4',['V03_Horn_movil_V02_M04','V03_Tornillo_original_horn_V02_M04','V03_Tornillo_M2x8','V03_Tuerca_M2_horn_'])
    sts17_shared('V17_Horn_STS3250_J3','11_Pinion_J4','V02_Pinon24_J4',True,target_y='0 mm',target_z='-6 mm',tag='J4',source_axis='Y',twist_degrees=270)
    g=comp('11_Pinion_J4');c=piece17(g,'V17_Separador_horn_J4','Al6061')
    ring(c,'Separador','Z','-1.7 mm','0 mm','0 mm','19.95 mm','6.2 mm','1.7 mm','steel')
    gear=act17_get('V02_Pinon24_J4')
    for x in ['-5.5 mm','5.5 mm']:ring(gear,'V17_Cerrar_patron_M2_antiguo','Z','0 mm',x,'0 mm','5.2 mm',None,'4 mm')
    join_all(gear,'V17_Pinon_J4_nuevo_patron')
    ring(gear,'V17_Paso_cabeza_central','Z','-0.1 mm','0 mm','0 mm','6.2 mm',None,'4.2 mm',cut=True)
    points=[('7 mm','0 mm'),('-7 mm','0 mm'),('0 mm','7 mm'),('0 mm','-7 mm')]
    j4_taladros(gear,'V17_Pasos_M3_PCD14','-0.1 mm',points,'3.4 mm','4.2 mm')
    j4_taladros(c,'Pasos_M3_PCD14','-1.8 mm',points,'3.4 mm','1.9 mm')
    act17_one(gear);act17_one(c)


def act17_sts_J4_union():
    act17_remove('11_Pinion_J4',['V03_M2x8_union_horn_'])
    g=comp('11_Pinion_J4')
    for i,(x,y) in enumerate([('7 mm','0 mm')]):
      c=piece17(g,'V17_M3x8_horn_J4_'+str(i),'Acero')
      ring(c,'Vastago','Z','-4 mm',x,y,'3 mm',None,'8 mm','steel');ring(c,'Cabeza','Z','4 mm',x,y,'5.5 mm',None,'3 mm','steel');join_all(c,'M3x8_REF')
      j4_repetir_fijacion(g,c,[(-14,0),(-7,7),(-7,-7)])
    c=piece17(g,'V17_M3x6_central_J4','Acero')
    ring(c,'Vastago','Z','-7.7 mm','0 mm','0 mm','3 mm',None,'6 mm','steel');ring(c,'Cabeza','Z','-1.7 mm','0 mm','0 mm','5.5 mm',None,'3 mm','steel');join_all(c,'M3x6_REF')
