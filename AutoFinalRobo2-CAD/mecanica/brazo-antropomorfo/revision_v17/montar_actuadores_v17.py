"""Montajes de la seleccion comercial v17. Fases nativas explicitas.

Conserva la pinza V7: solo adapta carcasa, horn y fijaciones del actuador.
Envolventes comerciales editables, nunca perfiles de fabricacion del servo.
"""

def act17_get(name):
    candidates=[o.component for o in R.allOccurrences if o.component.name==name]
    if not candidates:raise RuntimeError('Componente activo no encontrado: '+name)
    return candidates[0]

def act17_remove(root,prefixes):
    g=comp(root)
    for o in list(g.occurrences):
      if o.component.name.startswith(tuple(prefixes)):
        old=o.component.name
        g.features.removeFeatures.add(o)
        progress('act retirado '+old)

def act17_one(c):
    bs=[b for b in c.bRepBodies if b.isSolid and b.volume>1e-9]
    if len(bs)!=1 or bs[0].lumps.count!=1:
      raise RuntimeError('La pieza debe ser un unico solido conectado: '+c.name)

def act17_holes(c,name,z,depth,points,diam):
    # Croquis independientes: Fusion inferia relaciones entre cuatro centros
    # y rechazaba sus cotas incluso tras eliminar las restricciones visibles.
    for k,(x,y) in enumerate(points):
      ring(c,name+'_'+str(k),'Z',z,x,y,diam,None,depth,cut=True)


def act17_cones(c,name,z0,z1,points,d0,d1,cut=False):
    # Un par de secciones por cabeza; lofts independientes, unidos al vastago.
    for k,(x,y) in enumerate(points):
      sections=[]
      for z,d in [(z0,d0),(z1,d1)]:
        s=c.sketches.add(datum(c,'Z',z,'Plano_'+name+str(k)))
        circle(s,x,y,d);sections.append(s)
      next(o for o in R.allOccurrences if o.component==c).activate()
      ip=c.features.loftFeatures.createInput(OP.CutFeatureOperation if cut else OP.NewBodyFeatureOperation)
      for s in sections:ip.loftSections.add(s.profiles.item(0))
      if cut:ip.participantBodies=list(c.bRepBodies)
      f=c.features.loftFeatures.add(ip);f.name=name+str(k)
      for s in sections:s.isVisible=False
      if not cut:
        for b in f.bodies:b.appearance=AP['steel']

def act17_mini_config(which):
    if which=='J6':
      return dict(root='05_Inclinacion_muneca',motor='V02_M06_MG90S_J6',holder='V02_A18_Nucleo_J5_y_columna_J6',face='wrist56-22.9 mm',driven='06_Herramienta',front='-16.5 mm',coupler='V02_E05_Acople_J6')
    if which=='J7':
      return dict(root='06_Herramienta',motor='V02_M07_MG90S_Pinza',holder='V02_A20_Bastidor_pinza',face='grip_deck+1.6 mm',driven='10_Pinion_pinza',front='0 mm',coupler='V02_Pinon24_Pinza')
    raise ValueError(which)

def act17_mini_params():
    for n,e,u,co in [
      ('v17_mini_l','23 mm','mm','FEETECH STS3032 A0 plano: cuerpo23x12x27.5.'),
      ('v17_mini_w','12 mm','mm','FEETECH STS3032 A0.'),
      ('v17_mini_h','27.5 mm','mm','Cuerpo; altura total hasta spline31.9.'),
      ('v17_mini_axis','5.71 mm','mm','Plano A0, extremo corto del cuerpo.'),
      ('v17_mini_boss_d','8 mm','mm','INFERIDO, diametro de resalte no acotado. Mantener holgura de tapa9mm.'),
      ('v17_mini_clear','0.3 mm','mm','Propuesta de holgura por cara, calibrar PLA.'),
    ]:param(n,e,u,co)

def act17_mini_motor(which):
    p=act17_mini_config(which);f=p['face'];b='('+f+')-v17_mini_h'
    c=act17_get(p['motor']);discard_geometry(c)
    box(c,'STS3032_Cuerpo','-v17_mini_axis','-v17_mini_w/2',b,'v17_mini_l','v17_mini_w','v17_mini_h','servo')
    box(c,'STS3032_Orejas_REF','-10.21 mm','-6 mm','('+b+')+20.5 mm','32 mm','12 mm','1.5 mm','servo')
    ring(c,'STS3032_Resalte_REF','Z',f,'0 mm','0 mm','v17_mini_boss_d',None,'1 mm','servo')
    ring(c,'STS3032_Spline25T_REF','Z','('+f+')+1 mm','0 mm','0 mm','4.95 mm',None,'3.4 mm','brass')
    join_all(c,'STS3032_Envolvente_comercial')
    ring(c,'STS3032_Rosca_M2_3_REF','Z','('+f+')-0.6 mm','0 mm','0 mm','2.3 mm',None,'5.1 mm',cut=True)
    c.attributes.add('AFR2','material_v17','Servo FEETECH STS3032 A0')
    c.attributes.add('AFR2','catalog_mass_g','21.6')
    c.attributes.add('AFR2','datasheet','STS3032 A0,23x12x27.5; masa20.6+1g; nominal1.5kgfcm a6V. Spline25T4.95 y tornilloM2.3. Orejas sin patron transversal confirmado: no fijacion estructural por esos agujeros.')
    c.attributes.add('AFR2','actuador_v17','STS3032 A0,6V,TTL; reemplaza MG90S. Nombre de componente historico conservado por trazabilidad.')
    act17_one(c)
    act17_remove(p['root'],['V03_Tornillo_rosca_plastico_'+which])

def act17_mini_holder(which):
    p=act17_mini_config(which);c=act17_get(p['holder']);f=p['face'];b='('+f+')-v17_mini_h';base='('+b+')-2.6 mm'
    if which=='J7':
      box(c,'V17_Reserva_tapa_STS3032','-13.6 mm','-10.85 mm','('+f+')-0.35 mm','36.7 mm','21.7 mm','5.15 mm',cut=True)
    # Retirar antiguo asiento del cuerpo y la cuna de orejas. Se mantienen
    # largueros, nucleo y guias. El cuerpo se retiene por forma y tapa.
    box(c,'V17_Reserva_cuerpo_STS3032','-6.01 mm','-6.3 mm',b,'23.6 mm','12.6 mm','27.6 mm',cut=True)
    box(c,'V17_Apoyo_STS3032','-13.25 mm','-10.5 mm',base,'36 mm','21 mm','2.6 mm')
    boxes(c,'V17_Cuatro_pilares',base,'v17_mini_h+2.6 mm',[(x,y,'4.5 mm','4 mm') for x in ['-13.25 mm','18.25 mm'] for y in ['-10.3 mm','6.3 mm']])
    if which=='J7':
      for yy in ['-10.5 mm','6.5 mm']:
        box(c,'V17_Nervio_apoyo_'+yy,'-13.25 mm',yy,'4.5 mm','36 mm','4 mm','('+base+')-4.5 mm')
    join_all(c,'V17_Holder_STS3032_integrado')
    # Pilares con borde interiorY6.3: las orejasY6 ya tienen0.3mm libres.
    points=[(x,y) for x in ['-11 mm','20.5 mm'] for y in ['-8.3 mm','8.3 mm']]
    # Perforacion en fase separada: un rechazo no revierte todo el soporte.
    c.attributes.add('AFR2','material_v17','PLA PrintALot')
    c.attributes.add('AFR2','montaje_v17','STS3032 retenido por forma, tapa y4M2pasantes. Cuerpo y orejas con holgura0.3. Guias de pinzaV7 conservadas. Calibrar piloto/perfil PLA antes de fabricar.')
    act17_one(c)

def act17_mini_cap(which):
    p=act17_mini_config(which);g=comp(p['root']);f=p['face']
    c=piece17(g,'V17_Tapa_STS3032_'+which)
    box(c,'Tapa','-13.25 mm','-10.5 mm',f,'36 mm','21 mm','3 mm')
    ring(c,'Paso_resalte','Z','('+f+')-0.1 mm','0 mm','0 mm','9 mm',None,'3.2 mm',cut=True)
    points=[(x,y) for x in ['-11 mm','20.5 mm'] for y in ['-8.3 mm','8.3 mm']]
    act17_holes(c,'Cuatro_pasantes_M2',f,'3.1 mm',points,'2.3 mm')
    c.attributes.add('AFR2','montaje','Tapa3mm, cuatroM2x35 cabeza cilindrica4x2. CentrosX-11/20.5,Y+-8.3mm frente a hornR9; no avellanados innecesarios.')
    act17_one(c)

def act17_mini_fasteners(which):
    p=act17_mini_config(which);g=comp(p['root']);f=p['face'];top='('+f+')+3 mm';bottom='('+f+')-v17_mini_h-2.6 mm'
    for i,(x,y) in enumerate([(x,y) for x in ['-11 mm','20.5 mm'] for y in ['-8.3 mm','8.3 mm']]):
      c=piece17(g,'V17_M2x35_cilindrico_'+which+'_'+str(i),'Acero')
      ring(c,'Vastago_M2_REF','Z','('+top+')-35 mm',x,y,'2 mm',None,'35 mm','steel')
      ring(c,'Cabeza_M2_REF','Z',top,x,y,'4 mm',None,'2 mm','steel')
      join_all(c,'M2x35_REF');act17_one(c)
      c.attributes.add('AFR2','fijacion','M2x35 cilindrico; longitud bajo cabeza. Envolvente4x2, confirmar DIN/ISO comprado.')
      n=piece17(g,'V17_Tuerca_M2_'+which+'_'+str(i),'Acero')
      ring(n,'Tuerca_envolvente_REF','Z','('+bottom+')-1.2 mm',x,y,'4.6 mm','2 mm','1.2 mm','steel')
      n.attributes.add('AFR2','comercial','Tuerca baja DIN439 M2,1.2mm. Envolvente circular conservadora; verificar tolerancias del lote.')

def act17_mini_horn(which):
    p=act17_mini_config(which);g=comp(p['driven']);front=p['front']
    remove=['V03_Horn_movil_'+p['motor'],'V03_Tornillo_original_horn_'+p['motor']]
    if which=='J6':remove+=['V03_Tornillo_union_'+p['motor']]
    else:remove+=['V03_M2x8_union_horn_','V03_Tuerca_M2_horn_']
    act17_remove(p['driven'],remove)
    h=piece17(g,'V17_Horn_STS3032_'+which,'Al6061')
    ring(h,'Plato18_PCD12_REF','Z','('+front+')-2 mm','0 mm','0 mm','18 mm','2.3 mm','2 mm','steel')
    ring(h,'Cubo25T_REF','Z','('+front+')-4.5 mm','0 mm','0 mm','7 mm','4.95 mm','2.5 mm','steel')
    join_all(h,'Horn_comercial_REF')
    ring(h,'Caja_tornillo_central','Z','('+front+')-1.2 mm','0 mm','0 mm','4.4 mm',None,'1.3 mm',cut=True)
    hole=ring(h,'Rosca_M2_PCD12_REF','Z','('+front+')-2.1 mm','6 mm','0 mm','2 mm',None,'2.2 mm',cut=True)
    entities=adsk.core.ObjectCollection.create();entities.add(hole)
    ip=h.features.circularPatternFeatures.createInput(entities,h.zConstructionAxis)
    ip.quantity=V('8');ip.totalAngle=V('360 deg')
    h.features.circularPatternFeatures.add(ip).name='Ocho_roscas_PCD12_REF'
    h.attributes.add('AFR2','datasheet','STS3032 A0 p7:horn18mm,8M2PCD12,disco2,cubo2.5,25T4.95,M2.3central. Envolvente roscas/dentado, no fabricacion.')
    h.attributes.add('AFR2','material_inferido','Aluminio supuesto para masa, aleacion y masa real del accesorio pendientes.')
    c=piece17(g,'V17_Tornillo_original_M2_3_'+which,'Acero')
    ring(c,'Vastago_M2_3_REF','Z','('+front+')-6.2 mm','0 mm','0 mm','2.3 mm',None,'5 mm','steel')
    ring(c,'Cabeza_REF','Z','('+front+')-1.2 mm','0 mm','0 mm','4.2 mm',None,'1.8 mm','steel');join_all(c,'Tornillo_horn_REF')
    c.attributes.add('AFR2','fijacion','Tornillo original M2.3; cabeza4.2x1.8 inferida. Confirmar accesorio de la unidad adquirida.')
    # Acople/pinon ya adaptado en fase separada, sin tocar historial viejo.
    steel=act17_get(p['coupler'])
    # PCD12 reemplaza PCD11 solo en el acople, nunca cambia dientes/racks.
    if which=='J6':
      act17_holes(h,'Despeje_dos_horn_para_M2','('+front+')-2.1 mm','2.2 mm',[('-6 mm','0 mm'),('6 mm','0 mm')],'2.3 mm')
      for s in [-1,1]:
        c=piece17(g,'V17_Tornillo_M2x5_union_horn_'+which+str(s),'Acero');x=f'{s*6} mm'
        ring(c,'Vastago','Z','('+front+')-2 mm',x,'0 mm','2 mm',None,'5 mm','steel')
        ring(c,'Cabeza','Z','('+front+')-4 mm',x,'0 mm','3.8 mm',None,'2 mm','steel');join_all(c,'M2_REF')
    else:
      for s in [-1,1]:
        c=piece17(g,'V17_Tornillo_M2x6_union_horn_'+which+str(s),'Acero');x=f'{s*6} mm'
        ring(c,'Vastago','Z','-2 mm',x,'0 mm','2 mm',None,'6 mm','steel')
        ring(c,'Cabeza','Z','4 mm',x,'0 mm','3.8 mm',None,'2 mm','steel');join_all(c,'M2_REF')
    act17_one(h)

def act17_mini_J6_motor():act17_mini_params();act17_mini_motor('J6')
def act17_mini_J7_motor():act17_mini_motor('J7')
def act17_mini_J6_holder():act17_mini_holder('J6')
def act17_mini_J7_holder():act17_mini_holder('J7')
def act17_mini_holder_holes(which):
    p=act17_mini_config(which);c=act17_get(p['holder']);base='('+p['face']+')-v17_mini_h-2.6 mm'
    act17_holes(c,'V17_Cuatro_pasantes_M2',base,'v17_mini_h+2.8 mm',[(x,y) for x in ['-11 mm','20.5 mm'] for y in ['-8.3 mm','8.3 mm']],'2.3 mm')
    act17_one(c)

def act17_mini_J6_holder_holes():act17_mini_holder_holes('J6')
def act17_mini_J7_holder_holes():act17_mini_holder_holes('J7')
def act17_mini_J6_cap():act17_mini_cap('J6')
def act17_mini_J7_cap():act17_mini_cap('J7')
def act17_mini_J6_fasteners():act17_mini_base_y_fijaciones();act17_mini_fasteners('J6')
def act17_mini_J7_fasteners():act17_mini_fasteners('J7')
def act17_mini_J6_horn():act17_mini_horn('J6')
def act17_mini_J7_horn():act17_mini_horn('J7')


def act17_mini_refuerzo_pilares():
    # Ampliar0.6mm la separacion transversal para conservar0.85mm de pared
    # junto al pasanteM2 y las orejas del servo; antes quedaban0.25mm.
    for c in [act17_get(n) for n in ['V02_A18_Nucleo_J5_y_columna_J6','V17_Tapa_STS3032_J6','V02_A20_Bastidor_pinza','V17_Tapa_STS3032_J7']]:
      for sk in c.sketches:
        if sk.name=='Croquis_V17_Cuatro_pilares':
          for dim in sk.sketchDimensions:
            e=dim.parameter.expression
            if '9.7 mm' in e:dim.parameter.expression=e.replace('9.7 mm','10.3 mm')
            elif '5.7 mm' in e:dim.parameter.expression=e.replace('5.7 mm','6.3 mm')
        elif 'pasantes_M2' in sk.name:
          for dim in sk.sketchDimensions:
            e=dim.parameter.expression
            if '7.7 mm' in e:dim.parameter.expression=e.replace('7.7 mm','8.3 mm')
      act17_one(c)
    progress('PilaresJ6/J7: holgura de orejas0.3mm, pared al pasante0.85mm.')


def act17_mini_coupler(which):
    p=act17_mini_config(which);c=act17_get(p['coupler']);front=p['front']
    # Reconstruccion de material del DISENO de la pieza nueva. No soldar o
    # rellenar fisicamente una pieza vieja: fabricar con solo el PCD12 final.
    # Evita suprimir cortes del historial antiguo que rompen referencias
    # cruzadas heredadas del ensamble original.
    for sign in [-1,1]:
      ring(c,'V17_Reponer_material_PCD11_'+str(sign),'Z',front,f'{5.5*sign} mm','0 mm','2 mm' if which=='J6' else '2.4 mm',None,'2 mm' if which=='J6' else '4 mm','steel' if which=='J6' else 'red')
    join_all(c,'V17_Pieza_nueva_sin_PCD11')
    if which=='J6':
      ring(c,'V17_Reserva_cabeza_M2_3','Z','('+front+')-0.1 mm','0 mm','0 mm','4.6 mm',None,'0.9 mm',cut=True)
      act17_holes(c,'V17_Roscas_union_PCD12','('+front+')-0.1 mm','3.2 mm',[('-6 mm','0 mm'),('6 mm','0 mm')],'2 mm')
    else:
      act17_holes(c,'V17_Pasantes_union_PCD12','-0.1 mm','4.2 mm',[('-6 mm','0 mm'),('6 mm','0 mm')],'2.4 mm')
    c.attributes.add('AFR2','fabricacion_v17','Pieza nueva con patronPCD12. Reconstruccion CAD del material en posicionesPCD11 anteriores, no reparacion fisica por soldadura/relleno.')
    act17_one(c)

def act17_mini_J6_coupler():act17_mini_coupler('J6')
def act17_mini_J7_coupler():act17_mini_coupler('J7')

def act17_mini_limpiar_orejas():
    for name in ['V02_A18_Nucleo_J5_y_columna_J6','V02_A20_Bastidor_pinza']:
      c=act17_get(name);f=c.features.extrudeFeatures.itemByName('V17_Reserva_orejas_STS3032')
      if f:f.deleteMe()
      act17_one(c)


def act17_mini_base_y_fijaciones():
    if D.attributes.itemByName('AFR2','mini_base_y_fijaciones'):return
    # Base2.6mm hasta debajo del piso antiguo. TuercasM2bajas1.2mm por
    # debajo de toda la pieza, nunca embebidas en el piso existente.
    # Tornillo izquierdoX-11 libera la tuerca del nucleo circularR13.
    for which in ['J6','J7']:
      p=act17_mini_config(which);c=act17_get(p['holder']);f=p['face'];base='('+f+')-v17_mini_h-2.6 mm'
      for feature in ['V17_Apoyo_STS3032','V17_Cuatro_pilares']:
        c.constructionPlanes.itemByName('Plano_'+feature).definition.offset.expression=base
      c.features.extrudeFeatures.itemByName('V17_Apoyo_STS3032').extentOne.distance.expression='2.6 mm'
      c.features.extrudeFeatures.itemByName('V17_Cuatro_pilares').extentOne.distance.expression='v17_mini_h+2.6 mm'
      if which=='J7':
        for feature in ['V17_Nervio_apoyo_-10.5 mm','V17_Nervio_apoyo_6.5 mm']:
          c.features.extrudeFeatures.itemByName(feature).extentOne.distance.expression='('+base+')-4.5 mm'
      for k in range(4):
        name='V17_Cuatro_pasantes_M2_'+str(k)
        c.constructionPlanes.itemByName('Plano_'+name).definition.offset.expression=base
        c.features.extrudeFeatures.itemByName(name).extentOne.distance.expression='v17_mini_h+2.8 mm'
      cap=act17_get('V17_Tapa_STS3032_'+which)
      cap.features.extrudeFeatures.itemByName('Tapa').extentOne.distance.expression='3 mm'
      cap.attributes.add('AFR2','montaje','Tapa3mm, M2x35 y tuerca bajaDIN439de1.2. CentrosX-11/20.5,Y+-8.3; tuerca bajo todo el piso y fuera del nucleo.')
      for target in [c,cap]:
        for sk in target.sketches:
          if not (sk.name.startswith('Croquis_V17_') or target==cap):continue
          for dim in sk.sketchDimensions:
            e=dim.parameter.expression
            if '11.05 mm' in e:dim.parameter.expression=e.replace('11.05 mm','13.25 mm')
            elif '33.8 mm' in e:dim.parameter.expression=e.replace('33.8 mm','36 mm')
            elif '8.8 mm' in e:dim.parameter.expression=e.replace('8.8 mm','11 mm')
            elif which=='J7' and sk.name=='Croquis_V17_Reserva_tapa_STS3032':
              if '11.4 mm' in e:dim.parameter.expression=e.replace('11.4 mm','13.6 mm')
              elif '34.5 mm' in e:dim.parameter.expression=e.replace('34.5 mm','36.7 mm')
        act17_one(target)
    c=act17_get('V02_A20_Bastidor_pinza');base='grip_deck+1.6 mm-v17_mini_h-2.6 mm'
    for sign in [-1,1]:
      ring(c,'V17_Reserva_tuerca_baja_columna_'+str(sign),'Z','('+base+')-1.4 mm','-11 mm',f'{8.3*sign} mm','4.9 mm',None,'1.4 mm',cut=True)
    act17_one(c)
    D.attributes.add('AFR2','mini_base_y_fijaciones','Base2.6,tapa3,M2x35,tuercaDIN439M2baja1.2. X-11 libera nucleo; dos reservas laterales en columna de palma. Tuercas fuera del piso, proyeccion nominal0.7mm.')
    progress('Mini: base inferior completa2.6,tapa3,tuercas fuera del plastico, tornillo izquierdoX-11.')

def act17_mini_fijacion_J6_avellanada():
    # Cabezas por debajo del horn chocaban con la tapa fija al girar J6.
    # Dos roscas libres del horn, aY+-6, permiten atornillar desde arriba
    # de la brida. Cabezas avellanadas en el acero quedan al ras.
    act17_remove('06_Herramienta',['V17_Tornillo_M2x5_union_horn_J6'])
    c=act17_get('V02_E05_Acople_J6');points=[('0 mm','-6 mm'),('0 mm','6 mm')]
    act17_holes(c,'V17_Paso_M2_avellanado_J6','-16.6 mm','2.2 mm',points,'2.3 mm')
    act17_cones(c,'V17_Asiento_avellanado_J6','-15.55 mm','-14.7 mm',points,'2.3 mm','4 mm',True)
    act17_holes(c,'V17_Receso_cilindrico_J6','-14.7 mm','0.3 mm',points,'4.1 mm')
    for index,(x,y) in enumerate(points):
      bolt=piece17(comp('06_Herramienta'),'V17_M2x4_avellanado_horn_J6_'+str(index),'Acero')
      ring(bolt,'Vastago_M2_REF','Z','-18.5 mm',x,y,'2 mm',None,'2.8 mm','steel')
      act17_cones(bolt,'Cabeza_conica_REF','-15.7 mm','-14.7 mm',[(x,y)],'2 mm','4 mm')
      ring(bolt,'Borde_cabeza_REF','Z','-14.7 mm',x,y,'4 mm',None,'0.2 mm','steel')
      join_all(bolt,'M2x4_avellanado_REF');act17_one(bolt)
      bolt.attributes.add('AFR2','fijacion','M2x4 avellanado,total4mm,incluye cabeza. Envolvente maxima4x1.2; confirmar tornillo comercial antes de mecanizar asiento. Rosca en horn, paso libre en brida.')
    c.attributes.add('AFR2','fijacion_v17','DosM2x4avellanados enY+-6, pasantes en acero y roscados en horn. Cabeza al ras bajo el625. AgujerosX+-6 sin usar; no fabricar rosca doble ni tornillos hacia la tapa fija.')
    act17_one(c)

def act17_mini_pasajes_palma():
    c=act17_get('V02_A20_Bastidor_pinza')
    # El piso original no terminaba donde el nuevo: abrir desde debajo de
    # TODA la palma. La cara de apoyo de la tuerca sigue aZ7.5 nominal.
    base='grip_deck+1.6 mm-v17_mini_h-2.6 mm'
    for x in ['-11 mm','20.5 mm']:
      for y in ['-8.3 mm','8.3 mm']:
        ring(c,'V17_Pocket_tuerca_accesible_'+x+y,'Z','3.9 mm',x,y,'4.9 mm',None,'('+base+')-3.9 mm',cut=True)
    ring(c,'V17_Paso_eje_J6_en_piso','Z','7.49 mm','0 mm','0 mm','5.2 mm',None,'2.71 mm',cut=True)
    box(c,'V17_Llave_D_superior_con_holgura','-2 mm','-2.8 mm','7.49 mm','4 mm','0.7 mm','1.61 mm')
    join_all(c,'V17_Palma_con_llave_D_continua')
    ring(c,'V17_Restituir_paso_prisionero','Y','-16 mm','0 mm','6 mm','2.5 mm',None,'14 mm',cut=True)
    c.attributes.add('AFR2','montaje_inferior_v17','Cuatro bolsillos de tuerca abiertos desdeZ3.9 hasta piso7.5; eje5 con paso5.2 y llaveD superiorY-2.1, holgura0.1. Paso prisioneroM2.5 restaurado en nervio nuevo. Perfil/piloto FDM pendientes.')
    act17_one(c)
