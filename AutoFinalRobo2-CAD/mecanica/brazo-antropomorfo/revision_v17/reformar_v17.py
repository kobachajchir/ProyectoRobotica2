"""Reforma conservando la arquitectura V7, componentes y articulaciones.

Biblioteca de fases cargada por modelar_transmision_J3.py, dentro de Fusion.
No ejecutar por existir el archivo: cada fase requiere aplicacion explicita.
"""
import re

def cut_x17(c,name,start,y,z,diameter,length):
    # Orientacion nativaYZ comprobada en esta instalacion: U=-Z,V=Y,
    # normal+X. No suponer U=Y/V=Z por el nombre del plano.
    ip=c.constructionPlanes.createInput();ip.setByOffset(c.yZConstructionPlane,V(start))
    plane=c.constructionPlanes.add(ip);plane.name='Plano_'+name;plane.isLightBulbOn=False
    sk=c.sketches.add(plane);sk.name='Croquis_'+name;sk.isComputeDeferred=True
    curve=circle(sk,'-('+z+')',y,diameter);sk.isComputeDeferred=False
    center=sk.sketchToModelSpace(curve.centerSketchPoint.geometry)
    if max(abs(center.x-val(start)),abs(center.y-val(y)),abs(center.z-val(z)))>1e-6:
      raise RuntimeError('El taladroX no quedo en las coordenadas solicitadas')
    return extrude(c,sk,length,OP.CutFeatureOperation,name)

def light17_parametros():
    for n,e,u,comment in [
      ('v17_pvc1_od','25 mm','mm','Tigre AR CL10 codigo10162432, catalogo2025 p16.'),
      ('v17_pvc1_wall','1.2 mm','mm','Tigre AR CL10; espesor nominal, no medicion.'),
      ('v17_pvc1_id','v17_pvc1_od-2*v17_pvc1_wall','mm','Interior derivado22.6mm.'),
      ('v17_pvc2_od','20 mm','mm','Tigre AR CL10 codigo10162408, catalogo2025 p16.'),
      ('v17_pvc2_wall','1 mm','mm','Tigre AR CL10; espesor nominal, no medicion.'),
      ('v17_pvc2_id','v17_pvc2_od-2*v17_pvc2_wall','mm','Interior derivado18mm.'),
      ('v17_socket1_od','v17_pvc1_od+socket_clear+2*socket_wall','mm','Conservar pared de encastre y holgura editables.'),
      ('v17_socket2_od','v17_pvc2_od+socket_clear+2*socket_wall','mm','Encastre independiente del primer eslabon.'),
      ('v17_neck1','22 mm','mm','Cuello proximal propuesto, menor masa; requiere comprobacion de esfuerzos.'),
      ('v17_neck2','20 mm','mm','Cuello del antebrazo propuesto.'),
      ('v17_core_d','26 mm','mm','Nucleo articular propuesto; conservar eje y apoyos existentes.'),
    ]:param(n,e,u,comment)

def light17_map_pvc(root,prefix):
    g=comp(root);components={o.component.entityToken:o.component for o in g.allOccurrences}
    replacements=dict(pvc_od=prefix+'_od',pvc_id=prefix+'_id',pvc_wall=prefix+'_wall',socket_od='v17_socket1_od' if prefix.endswith('1') else 'v17_socket2_od')
    for c in components.values():
      for p in c.modelParameters:
        expression=p.expression;new=expression
        for old,target in replacements.items():new=re.sub(r'\b'+old+r'\b',target,new)
        if new!=expression:
          p.expression=new;progress('diametro: '+c.name+'/'+p.name)
    return components.values()

def light17_brazo_pvc():
    light17_map_pvc('02_Brazo_PVC','v17_pvc1')
    gc('L01_PVC32_Corte_1').attributes.add('AFR2','material_v17','PVC Tigre CL10')
    gc('L01_PVC32_Corte_1').attributes.add('AFR2','producto','Tigre10162432,25x1.2mm; ID22.6 derivado. Corte gobernado por el ensamble.')

def light17_antebrazo_pvc():
    light17_map_pvc('03_Antebrazo_PVC','v17_pvc2')
    # Mantener el espesor radial de la copaJ4 al reducir su encastre.
    # Los travesanos entran4mm mas para continuar unidos a la copa menor.
    c=gc('V02_A14_Soporte_J4_PETG')
    for name in ['Croquis_Asiento','Croquis_Copa']:
      sketch=c.sketches.itemByName(name)
      if not sketch:raise RuntimeError('Falta croquis de copaJ4: '+name)
      for dim in sketch.sketchDimensions:
        if abs(dim.parameter.value-val('46 mm'))<1e-7:dim.parameter.expression='34 mm'
    for sign in [-1,1]:
      sketch=c.sketches.itemByName('Croquis_Travesano_'+str(sign))
      if not sketch:raise RuntimeError('Falta travesanoJ4')
      for dim in sketch.sketchDimensions:
        if abs(dim.parameter.value-val('16 mm'))<1e-7:dim.parameter.expression='12 mm'
        elif abs(dim.parameter.value-val('40 mm'))<1e-7:dim.parameter.expression='44 mm'
    gc('L02_PVC32_Corte_2').attributes.add('AFR2','material_v17','PVC Tigre CL10')
    gc('L02_PVC32_Corte_2').attributes.add('AFR2','producto','Tigre10162408,20x1mm; ID18 derivado. Corte gobernado por el ensamble.')

def light17_core(component,old_d,neck):
    c=gc(component)
    s=c.sketches.itemByName('Croquis_Nucleo')
    if not s:raise RuntimeError('No se encontro el croquis nativo del nucleo: '+component)
    found=False
    for dim in s.sketchDimensions:
      if abs(dim.parameter.value-val(str(old_d)+' mm'))<1e-7:
        dim.parameter.expression='v17_core_d';found=True
    if not found:raise RuntimeError('No se encontro el diametro esperado del nucleo: '+component)
    s=c.sketches.itemByName('Croquis_Cuello')
    if not s:raise RuntimeError('Falta cuello del nucleo')
    for dim in s.sketchDimensions:
      if abs(dim.parameter.value-val('13 mm'))<1e-7:dim.parameter.expression=neck+'/2'
      elif abs(dim.parameter.value-val('26 mm'))<1e-7:dim.parameter.expression=neck

def light17_nucleo_hombro():light17_core('A07_Nucleo_hombro_PETG',42,'v17_neck1')
def light17_nucleo_codo():light17_core('A13_Nucleo_codo_PETG',40,'v17_neck2')

def light17_nervios_proximales():
    # Los nervios originales arrancaban a X16: con PVC25 quedaban fuera
    # del manguito y el Combine devolvia varios solidos desconectados.
    # Llevar su raiz dentro del cuello nuevo; conservar cuatro nervios.
    c=gc('A05_Encastre_proximal_brazo_PETG')
    count=0
    for sketch in c.sketches:
      if not sketch.name.startswith('Croquis_V07_Nervio_proximal_'):continue
      for dim in sketch.sketchDimensions:
        if not dim.isDriving:continue
        p=dim.parameter
        if abs(p.value-val('16 mm'))<1e-7:
          p.expression=re.sub(r'16(?:\.0+)?\s*mm','(v17_neck1/2-1 mm)',p.expression);count+=1
        elif abs(p.value-val('28 mm'))<1e-7:
          p.expression=re.sub(r'28(?:\.0+)?\s*mm','(v17_neck1/2+9 mm)',p.expression);count+=1
    if count!=8:raise RuntimeError('Se esperaban ocho cotas conductoras de nervios; encontradas '+str(count))
    progress('nervios proximales adaptados al cuello22 y PVC25')

def light17_despeje_reconectado():
    c=gc('A05_Encastre_proximal_brazo_PETG')
    f=c.features.extrudeFeatures.itemByName('V17_Despeje_pared_PVC25')
    if not f:raise RuntimeError('No se encontro el despeje a reparar')
    # La anterior seleccion de cuerpos pertenecia al Combine fallido.
    # Recrear solamente este corte sobre el solido continuo recuperado.
    if c.bRepBodies.count!=1 or c.bRepBodies.item(0).lumps.count!=1:
      raise RuntimeError('El encastre debe ser un unico solido antes del despeje')
    f.deleteMe()
    phase17_paso_tubo()

def light17_replace_print(name,root='02_Brazo_PVC'):
    g=comp(root)
    old=next(o for o in g.occurrences if o.component.name==name)
    old.component.name=name+'__HISTORICO_reemplazado'
    g.features.removeFeatures.add(old).name='Retirar_version_anterior_'+name
    c=piece17(g,name,'PLA PrintALot')
    c.attributes.add('AFR2','revision','v17: pieza simplificada para PVC menor y ejes70/60. Ejes y pinza V7 conservados; resistencia pendiente.')
    return c

def light17_nombres_encastres():
    # Fusion conserva componentes del historial aunque su ocurrencia haya
    # sido retirada. Reservar el nombre canonico al componente activo.
    g=comp('02_Brazo_PVC')
    for name in ['A05_Encastre_proximal_brazo_PETG','A06_Encastre_distal_brazo_PETG']:
      candidates=[o for o in g.occurrences if o.component.name==name or o.component.name.startswith(name+' (')]
      fresh=[o for o in candidates if o.component.attributes.itemByName('AFR2','revision') and o.component.attributes.itemByName('AFR2','revision').value.startswith('v17: pieza simplificada')]
      if len(fresh)!=1:raise RuntimeError('Encastre nuevo ambiguo '+name)
      active=fresh[0].component
      for old in candidates:
        if old.component==active:continue
        old.component.name=name+'__HISTORICO_reemplazado'
        # Delete puede reconstruir una ocurrencia con referencias del
        # historial. Remove explicito al final evita cuerpos duplicados.
        g.features.removeFeatures.add(old).name='Retirar_encastre_historico_'+name
      active.name=name

def light17_encastre_hombro_compacto():
    c=light17_replace_print('A05_Encastre_proximal_brazo_PETG')
    ring(c,'Nucleo_hombro','Y','-20 mm','0 mm','0 mm','v17_core_d','8.15 mm','40 mm')
    box(c,'Cuello_integral','-v17_neck1/2','-v17_neck1/2','8 mm','v17_neck1','v17_neck1','12 mm')
    ring(c,'Manguito_PVC25','Z','socket_start','0 mm','0 mm','v17_socket1_od','v17_pvc1_od+socket_clear','socket_length+v07_socket_extra')
    for sign in [-1,1]:
      box(c,'Oreja_abrazadera_'+str(sign),'v17_pvc1_od/2+1 mm',f'({sign})*4 mm-2.5 mm','socket_start+socket_length/2-5 mm','10 mm','5 mm','10 mm')
    box(c,'Travesano_tensor_integral','-34 mm','-23 mm','j3_fixed_z-8 mm','68 mm','10 mm','16 mm')
    for xx in [-29,29]:
      box(c,'Anclaje_tensor_'+str(xx),f'{xx-5} mm','-33 mm+j3_transverse_shift','j3_fixed_z-8 mm','10 mm','20 mm-j3_transverse_shift','16 mm')
    join_all(c,'Una_impresion_hombro_y_tensor')
    if c.bRepBodies.count!=1 or c.bRepBodies.item(0).lumps.count!=1:raise RuntimeError('Encastre proximal desconectado')
    ring(c,'Paso_anular_tubo','Z','tube_inset','0 mm','0 mm','v17_pvc1_od+socket_clear','v17_pvc1_id-0.2 mm','socket_start+socket_length+v07_socket_extra-tube_inset+0.1 mm',cut=True)
    box(c,'Ranura_abrazadera','v17_pvc1_od/2-1 mm','-0.75 mm','socket_start-1 mm','13 mm','1.5 mm','socket_length+2 mm',cut=True)
    ring(c,'Paso_M3_abrazadera','Y','-8 mm','v17_pvc1_od/2+6 mm','socket_start+socket_length/2','3.4 mm',None,'16 mm',cut=True)
    for xx in [-29,29]:
      ring(c,'Paso_M4_tensor_'+str(xx),'Y','-33.1 mm+j3_transverse_shift',f'{xx} mm','j3_fixed_z','4.5 mm',None,'20.2 mm-j3_transverse_shift',cut=True)
    if c.bRepBodies.count!=1 or c.bRepBodies.item(0).lumps.count!=1:raise RuntimeError('Los cortes separaron el encastre proximal')

def light17_horquilla_codo_compacta():
    c=light17_replace_print('A06_Encastre_distal_brazo_PETG')
    start='link1-socket_start-socket_length'
    ring(c,'Manguito_PVC25','Z',start,'0 mm','0 mm','v17_socket1_od','v17_pvc1_od+socket_clear','socket_length+v07_socket_extra')
    box(c,'Puente_integral','-16 mm','-fork_y-fork_t/2','link1-21 mm','32 mm','2*fork_y+fork_t','6 mm')
    for sy in [-1,1]:
      yy=f'({sy})*fork_y-fork_t/2'
      box(c,'Rama_'+str(sy),'-16 mm',yy,'link1-21 mm','32 mm','fork_t','21 mm')
      ring(c,'Oreja608_'+str(sy),'Y',yy,'0 mm','link1','38 mm','b608_od+fit_clear','fork_t')
      lip='fork_y-fork_t/2-2 mm' if sy>0 else '-fork_y+fork_t/2'
      ring(c,'Hombro_exterior608_'+str(sy),'Y',lip,'0 mm','link1','23.25 mm','19 mm','2 mm')
    for sy in [-1,1]:
      box(c,'Oreja_abrazadera_'+str(sy),'v17_pvc1_od/2+1 mm',f'({sy})*4 mm-2.5 mm',f'({start})+socket_length/2-5 mm','10 mm','5 mm','10 mm')
    join_all(c,'Una_impresion_manguito_y_horquilla')
    ring(c,'Paso_tubo_y_puente','Z',f'({start})-0.1 mm','0 mm','0 mm','v17_pvc1_od+socket_clear',None,'socket_length+v07_socket_extra+0.2 mm',cut=True)
    box(c,'Ranura_abrazadera','v17_pvc1_od/2-1 mm','-0.75 mm',f'({start})-1 mm','13 mm','1.5 mm','socket_length+2 mm',cut=True)
    ring(c,'Paso_M3_abrazadera','Y','-8 mm','v17_pvc1_od/2+6 mm',f'({start})+socket_length/2','3.4 mm',None,'16 mm',cut=True)
    for sy in [-1,1]:
      for sx in [-1,1]:
        yy=f'({sy})*fork_y-fork_t/2-0.1 mm'
        ring(c,'Paso_tapa_'+str(sy)+str(sx),'Y',yy,f'({sx})*14 mm','link1','3.4 mm',None,'fork_t+0.2 mm',cut=True)
        ins=f'({sy})*fork_y-1 mm' if sy>0 else f'({sy})*fork_y-3 mm'
        ring(c,'Asiento_inserto_'+str(sy)+str(sx),'Y',ins,f'({sx})*14 mm','link1','4.5 mm',None,'4 mm',cut=True)
    if c.bRepBodies.count!=1 or c.bRepBodies.item(0).lumps.count!=1:raise RuntimeError('Horquilla de codo desconectada')

def light17_llave_hombro():
    c=gc('A05_Encastre_proximal_brazo_PETG')
    # Recuperar la cara D del eje original: no transmitir el par mediante
    # un simple agujero circular con juego. Holgura plana propuesta0.1mm.
    box(c,'Llave_D_hombro','-3 mm','-19 mm','3.4 mm','6 mm','38 mm','1.2 mm')
    join_all(c,'Nucleo_y_llave_D_integrales')
    c.attributes.add('AFR2','interfaz_eje','Eje8 con planoZ3.3, alojamiento planoZ3.4; ajuste FDM por verificar en probeta. No union por rozamiento de cilindro liso.')
    gc('V17_Correa_OMEGA_HP_3M_6').attributes.add('AFR2','catalog_mass_g',str(val('j3_belt_length')*10*.0144))

def light17_asientos_codo():
    c=gc('A06_Encastre_distal_brazo_PETG')
    c.features.extrudeFeatures.itemByName('Manguito_PVC25').extentOne.distance.expression='socket_length'
    # Los brazos rectangulares se agregan antes del asiento circular:
    # mecanizar el alojamiento completo despues de la union, no solo el aro.
    for sy in [-1,1]:
      ring(c,'Asiento608_completo_'+str(sy),'Y',f'({sy})*fork_y-fork_t/2-0.05 mm','0 mm','link1','b608_od+fit_clear',None,'fork_t+0.1 mm',cut=True)
    for tag,y in [('cabeza','-13 mm'),('tuerca','6.5 mm')]:
      ring(c,'Reserva_arandela_'+tag,'Y',y,'v17_pvc1_od/2+6 mm','link1-socket_start-socket_length/2','8 mm',None,'6.5 mm',cut=True)

def light17_paso_eje_hombro():
    c=gc('A05_Encastre_proximal_brazo_PETG')
    # Despejar tambien el travesano integrado, agregado despues del nucleo.
    ring(c,'Paso_eje_en_travesano','Y','-43 mm','0 mm','0 mm','8.15 mm',None,'64 mm',cut=True)
    box(c,'Llave_D_tras_paso','-3 mm','-19 mm','3.4 mm','6 mm','38 mm','1.2 mm')
    join_all(c,'Llave_D_continua_tras_despeje')

def light17_poleas24_54():
    # Con masa CAD actual,2:1 queda en1.159 frente al minimo1.2.
    #2.25:1 eleva capacidad a2.824Nm sin aumentar la masa del motor.
    #225HP3M figura en gama HP del fabricante;216 solo figura HTD basica.
    for n,e in [('j3_small_teeth','24'),('j3_large_teeth','54'),('j3_belt_length','225 mm')]:
      param(n,e,'' if n.endswith('teeth') else 'mm','v17: iteracion desde cargas CAD actuales; volver a calcular despues de modelar.')
    for old,teeth,param_name in [('V17_Polea_20T_J3',24,'j3_small_teeth'),('V17_Polea_40T_J3',54,'j3_large_teeth')]:
      c=gc(old)
      for sketch in c.sketches:
        if not sketch.name.startswith(('Croquis_Disco_macizo_conservador','Croquis_Pestana')):continue
        for dim in sketch.sketchDimensions:
          if not isinstance(dim,adsk.fusion.SketchDiameterDimension) or not dim.isDriving:continue
          if 'Disco_macizo' in sketch.name:
            # El otro diametro es el taladro5/8: modificar solo el exterior.
            if dim.parameter.value>val('15 mm'):dim.parameter.expression=f'{param_name}*j3_belt_pitch/{math.pi:.15f}'
          elif dim.parameter.value>val('15 mm'):
            dim.parameter.expression=f'{math.ceil(teeth*3/math.pi+2)} mm'
      c.name=f'V17_Polea_{teeth}T_J3'
      c.attributes.add('AFR2','comercial',f'Aluminio HTD3M{teeth}T, cara9/correa6; disco conservador, sin perfil fabricable. Compra y cubo exactos pendientes.')
    j=R.joints.itemByName('T_J3_Correa_2a1');lim=j.jointMotion.rotationLimits
    lim.minimumValue=-math.pi*2.25;lim.maximumValue=math.pi*2.25
    # Despeje del collar de hombro tambien en el carro movil, no solo placa.
    c=gc('V17_Carro_motor_apoyo_Al')
    ring(c,'Paso_collar_hombro','Y','-56 mm','0 mm','0 mm','18 mm',None,'13 mm',cut=True)
    gc('V17_Correa_OMEGA_HP_3M_6').attributes.add('AFR2','catalog_mass_g',str(val('j3_belt_length')*10*.0144))

def light17_correa_coaxial():
    c=gc('V17_Correa_OMEGA_HP_3M_6');discard_geometry(c)
    sk=c.sketches.add(datum(c,'Y','j3_pulley_y+1.5 mm+j3_transverse_shift','Plano_correa_coaxial'))
    sk.name='Correa_centros_vinculados';sk.isComputeDeferred=True
    lo=val('j3_motor_z');hi=val('link1');r=val('j3_rsmall');Rr=val('j3_rlarge');angle=math.asin((Rr-r)/(hi-lo))
    contours=[]
    for offset,expr in [(-.11,'-1.1 mm'),(.12,'+1.2 mm')]:
      rr=r+offset;RR=Rr+offset
      a=sk.sketchCurves.sketchArcs.addByCenterStartSweep(P(0,-lo,0),P(rr*math.cos(angle),-lo+rr*math.sin(angle),0),math.pi-2*angle)
      b=sk.sketchCurves.sketchArcs.addByCenterStartSweep(P(0,-hi,0),P(-RR*math.cos(angle),-hi+RR*math.sin(angle),0),math.pi+2*angle)
      l=sk.sketchCurves.sketchLines.addByTwoPoints(a.endSketchPoint,b.startSketchPoint)
      k=sk.sketchCurves.sketchLines.addByTwoPoints(b.endSketchPoint,a.startSketchPoint)
      contours.append((a,b,l,k,expr))
    # Eliminar inferencias automaticas antes de definir las relaciones.
    for co in reversed(list(sk.geometricConstraints)):
      if co.isDeletable:co.deleteMe()
    gcst=sk.geometricConstraints
    for a,b,l,k,expr in contours:
      for p,q in [(l.startSketchPoint,a.endSketchPoint),(l.endSketchPoint,b.startSketchPoint),(k.startSketchPoint,b.endSketchPoint),(k.endSketchPoint,a.startSketchPoint)]:
        if p.entityToken!=q.entityToken:gcst.addCoincident(p,q)
      for arc,rad in [(a,'j3_rsmall'),(b,'j3_rlarge')]:
        sk.sketchDimensions.addRadialDimension(arc,P(.7,-lo,0)).parameter.expression=rad+expr
      for line,arc in [(l,a),(l,b),(k,a),(k,b)]:gcst.addTangent(line,arc)
    a,b,*_=contours[0];aa,bb,*_=contours[1]
    for p,q in [(aa.centerSketchPoint,a.centerSketchPoint),(bb.centerSketchPoint,b.centerSketchPoint)]:
      if p.entityToken!=q.entityToken:gcst.addCoincident(p,q)
    for arc,z in [(a,'j3_motor_z'),(b,'link1')]:
      gcst.addVerticalPoints(sk.originPoint,arc.centerSketchPoint)
      sk.sketchDimensions.addDistanceDimension(sk.originPoint,arc.centerSketchPoint,ORI.VerticalDimensionOrientation,P(.7,-val(z),0)).parameter.expression=z
    sk.isComputeDeferred=False
    for arc,z in [(a,'j3_motor_z'),(aa,'j3_motor_z'),(b,'link1'),(bb,'link1')]:
      if abs(arc.centerSketchPoint.geometry.y+val(z))>1e-6:raise RuntimeError('Centro de correa no vinculado a '+z)
    profiles=[p for p in sk.profiles if p.profileLoops.count==2]
    if len(profiles)!=1:raise RuntimeError('Contorno de correa ambiguo')
    f=extrude(c,sk,'6 mm',OP.NewBodyFeatureOperation,'Correa_coaxial_sin_dientes_REF',profiles[0])
    for body in f.bodies:body.appearance=AP['rubber']

def light17_poleas20_50():
    # Reservar par para los actuadores de muneca que reemplazan al miniJ4
    # descartado por las cargas. Se conserva motorJ3 y el recorrido del codo.
    for n,e in [('j3_small_teeth','20'),('j3_large_teeth','50'),('j3_belt_length','210 mm')]:
      param(n,e,'' if n.endswith('teeth') else 'mm','v17:2.5:1, revisar cargas finales tras montar los actuadores restantes.')
    for old,teeth in [('V17_Polea_24T_J3',20),('V17_Polea_54T_J3',50)]:
      c=gc(old)
      for sk in c.sketches:
        if not sk.name.startswith('Croquis_Pestana'):continue
        for dim in sk.sketchDimensions:
          if isinstance(dim,adsk.fusion.SketchDiameterDimension) and dim.isDriving and dim.parameter.value>val('15 mm'):
            dim.parameter.expression=f'{math.ceil(teeth*3/math.pi+2)} mm'
      c.name=f'V17_Polea_{teeth}T_J3'
      c.attributes.add('AFR2','comercial',f'HTD3M aluminio{teeth}T, cara9 para correa6; dimensiones de prototipo y masa conservadora, perfil comercial pendiente.')
    lim=R.joints.itemByName('T_J3_Correa_2a1').jointMotion.rotationLimits
    lim.minimumValue=-math.pi*2.5;lim.maximumValue=math.pi*2.5
    gc('V17_Correa_OMEGA_HP_3M_6').attributes.add('AFR2','catalog_mass_g',str(val('j3_belt_length')*10*.0144))
    belt=gc('V17_Correa_OMEGA_HP_3M_6').bRepBodies.item(0)
    expected=val('j3_motor_z-j3_rsmall-1.2 mm')
    if abs(belt.boundingBox.minPoint.z-expected)>1e-5:raise RuntimeError('La correa no conserva su centro parametrico')

def light17_corredor_horquilla_hombro():
    c=gc('A01_Plataforma_giratoria_PETG')
    # Los anclajes del tensor barren el exterior de la oreja negativa.
    # Abrir el anillo20..50mm y recuperar el camino de carga por el sector
    # inferior izquierdo que queda fuera de ese barrido (sin elevar base).
    ring(c,'Corredor_anclajes_J3','Y','-33 mm','0 mm','tower_h','100 mm','40 mm','10 mm',cut=True)
    triangle_y(c,'Diagonal_negativa_continua','-fork_y-fork_t/2',
      [('-13 mm','tower_h-12 mm'),('-22 mm','tower_h-5 mm'),('-34 mm','7 mm'),('-24 mm','7 mm')],'fork_t')
    join_all(c,'Horquilla_y_diagonal_una_pieza')
    if c.bRepBodies.count!=1 or c.bRepBodies.item(0).lumps.count!=1:raise RuntimeError('La diagonal no recupero la continuidad del hombro')
    c.attributes.add('AFR2','revision_corredor','Oreja608 retenida; apoyo diagonal fuera del barrido de anclajesJ3. Recalcular tensiones y barrer limites; no aprobacion de resistencia.')

def light17_despejes_antebrazo():
    if not D.attributes.itemByName('AFR2','v17_phase_reforma_encastre_codo_compacto'):
      raise RuntimeError('Reconstruir A11 compacto antes de los despejes finales')
    param('tube2_cut','link2-tube_inset-17.5 mm',comment='Separar extremoPVC20 de608 inferior:1.5mm hasta asiento link2-16.')
    c=gc('A12_Encastre_distal_antebrazo_PETG')
    ring(c,'Asiento608_inferior_completo','Z','link2-16 mm','0 mm','0 mm','b608_od+fit_clear',None,'7 mm',cut=True)
    ring(c,'Paso_M3_abrazadera_completo','Y','-8 mm','v17_pvc2_od/2+6 mm','link2-socket_start-socket_length/2','3.4 mm',None,'16 mm',cut=True)
    for tag,y in [('cabeza','-13 mm'),('tuerca','6.5 mm')]:
      ring(c,'Reserva_arandela_'+tag,'Y',y,'v17_pvc2_od/2+6 mm','link2-socket_start-socket_length/2','8 mm',None,'6.5 mm',cut=True)

def light17_encastre_codo_compacto():
    c=light17_replace_print('A11_Encastre_proximal_antebrazo_PETG','03_Antebrazo_PVC')
    ring(c,'Nucleo_codo','Y','-20 mm','0 mm','0 mm','v17_core_d','8.15 mm','40 mm')
    box(c,'Cuello_integral','-9 mm','-9 mm','8 mm','18 mm','18 mm','10 mm')
    ring(c,'Manguito_PVC20','Z','socket_start','0 mm','0 mm','v17_socket2_od','v17_pvc2_od+socket_clear','socket_length+2 mm')
    for sign in [-1,1]:
      box(c,'Oreja_abrazadera_'+str(sign),f'({sign})*4 mm-2.5 mm','v17_pvc2_od/2+1 mm','socket_start+socket_length/2-5 mm','5 mm','10 mm','10 mm')
    join_all(c,'Nucleo_y_abrazadera_una_pieza')
    ring(c,'Paso_pared_PVC20','Z','tube_inset','0 mm','0 mm','v17_pvc2_od+socket_clear','v17_pvc2_id-0.2 mm','socket_start+socket_length+2 mm-tube_inset+0.1 mm',cut=True)
    box(c,'Ranura_abrazadera','-0.75 mm','v17_pvc2_od/2-1 mm','socket_start-1 mm','1.5 mm','13 mm','socket_length+2 mm',cut=True)
    cut_x17(c,'Paso_M3_abrazadera','-8 mm','v17_pvc2_od/2+6 mm','socket_start+socket_length/2','3.4 mm','16 mm')
    for tag,x in [('uno','-13 mm'),('otro','6.5 mm')]:
      cut_x17(c,'Reserva_arandela_'+tag,x,'v17_pvc2_od/2+6 mm','socket_start+socket_length/2','8 mm','6.5 mm')
    box(c,'Llave_D_codo','-3 mm','-19 mm','3.4 mm','6 mm','38 mm','1.2 mm')
    join_all(c,'Llave_D_y_nucleo_integrales')
    if c.bRepBodies.count!=1 or c.bRepBodies.item(0).lumps.count!=1:raise RuntimeError('A11 compacto desconectado')

def light17_actualizar_fichas():
    gc('A11_Encastre_proximal_antebrazo_PETG').attributes.add('AFR2','revision','v17: pieza simplificada para PVC20x1 y nucleo26. Abrazadera orientada+Y, eje8 con caraD. Resistencia pendiente.')
    gc('L02_PVC32_Corte_2').attributes.add('AFR2','producto','Tigre10162408,20x1mm ID18 derivado; corte'+str(val('tube2_cut')*10)+'mm para dejar1.5mm hasta608 inferior.')

def light17_taladrosX_codo():
    c=gc('A11_Encastre_proximal_antebrazo_PETG')
    for name in ['Paso_M3_abrazadera','Reserva_arandela_uno','Reserva_arandela_otro']:
      f=c.features.extrudeFeatures.itemByName(name)
      if f:f.deleteMe()
    cut_x17(c,'V17_Paso_M3_X','-8 mm','v17_pvc2_od/2+6 mm','socket_start+socket_length/2','3.4 mm','16 mm')
    for tag,x in [('uno','-13 mm'),('otro','6.5 mm')]:
      cut_x17(c,'V17_Reserva_arandela_X_'+tag,x,'v17_pvc2_od/2+6 mm','socket_start+socket_length/2','8 mm','6.5 mm')

def light17_holgura_final_tensor():
    # Ganar0.5mm respecto a la tapa estacionaria sin alterar co-axialidad:
    # el parametro mueve juntos motor, carro, ejes y ambas poleas.
    param('j3_transverse_shift','-10 mm',comment='Reserva axial final: transmite mismo plano de correa; no eleva montura.')
    c=gc('V17_Placa_fija_tensor_Al');sk=c.sketches.itemByName('Croquis_Ala_tornillo_tensor')
    if not sk:raise RuntimeError('Falta croquis del ala del tensor')
    for dim in sk.sketchDimensions:
      if not dim.isDriving:continue
      p=dim.parameter
      if abs(p.value-val('45 mm'))<1e-7:p.expression=re.sub(r'45(?:\.0+)?\s*mm','43.5 mm',p.expression)
      elif abs(p.value-val('12 mm'))<1e-7:p.expression='10.5 mm'

def light17_longitudes():
    # Extra2 evita invalidar la extrusion historica de extension del encastre.
    # Los diametros y nucleos deben reformarse antes de acortar los eslabones.
    required=['reforma_brazo_pvc','reforma_antebrazo_pvc','reforma_nucleo_hombro','reforma_nucleo_codo']
    for phase in required:
      if not D.attributes.itemByName('AFR2','v17_phase_'+phase):raise RuntimeError('Aplicar antes '+phase)
    for n,e in [('socket_start','14 mm'),('socket_length','12 mm'),('tube_inset','14 mm'),('v07_socket_extra','2 mm'),('link1','70 mm'),('link2','60 mm'),('j3_belt_length','210 mm')]:
      param(n,e,comment='Reforma liviana v17; mantener ejes, recorridos y pinzaV7. Recalcular y revisar choques tras aplicar.')
    D.attributes.add('AFR2','reforma_pvc','Tigre25x1.2 y20x1, ejes70/60, cortes42/32. Mantener revision mecanica pendiente hasta nuevo calculo/CAD.')
